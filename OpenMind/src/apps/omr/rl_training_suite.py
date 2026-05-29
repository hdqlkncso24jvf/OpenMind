import random
import logging
import torch
import torch.nn as nn
from torch.distributions import Categorical
import numpy as np
from typing import List, Dict, Tuple, Any, Set, Union
from collections import defaultdict
from scipy.stats import ks_2samp
from tqdm import tqdm
from data_structure import Graph, DualStarPattern, RLState, AddEdgeAction, StopAction, PathInConstruction, PatternNode
from tie_matcher import compute_compact_match
from attribute_config import load_attribute_config
logger = logging.getLogger(__name__)

class GraphSchema:

    def __init__(self, graph: Graph):
        self.graph = graph
        self.valid_edge_schemas: Set[Tuple[str, str, str]] = set()
        if hasattr(graph, 'edge_sources') and hasattr(graph, 'edge_targets'):
            for edge_label in graph.edge_sources:
                src_labels = set()
                tgt_labels = set()
                for nid in list(graph.edge_sources[edge_label])[:200]:
                    node = graph.get_node(nid)
                    if node:
                        src_labels.add(node.label)
                for nid in list(graph.edge_targets.get(edge_label, set()))[:200]:
                    node = graph.get_node(nid)
                    if node:
                        tgt_labels.add(node.label)
                for sl in src_labels:
                    for tl in tgt_labels:
                        self.valid_edge_schemas.add((sl, edge_label, tl))
        else:
            for edge in graph.all_edges():
                source = graph.get_node(edge.source)
                target = graph.get_node(edge.target)
                if source and target:
                    self.valid_edge_schemas.add((source.label, edge.label, target.label))
        self._build_neighbor_index()

    def _build_neighbor_index(self):
        self.neighbors_out = defaultdict(set)
        self.neighbors_in = defaultdict(set)
        for src_label, edge_label, tgt_label in self.valid_edge_schemas:
            self.neighbors_out[src_label].add((edge_label, tgt_label))
            self.neighbors_in[tgt_label].add((edge_label, src_label))

    def is_valid_edge(self, source_label: str, edge_label: str, target_label: str) -> bool:
        return (source_label, edge_label, target_label) in self.valid_edge_schemas

    def get_valid_outgoing(self, node_label: str) -> List[Tuple[str, str]]:
        return list(self.neighbors_out.get(node_label, []))

    def get_valid_incoming(self, node_label: str) -> List[Tuple[str, str]]:
        return list(self.neighbors_in.get(node_label, []))

    def get_valid_bidirectional(self, node_label: str) -> List[Tuple[str, str]]:
        out = self.get_valid_outgoing(node_label)
        in_edges = self.get_valid_incoming(node_label)
        all_edges = set(out + in_edges)
        return list(all_edges)

class PatternSpaceEstimator:

    def __init__(self, schema: GraphSchema):
        self.schema = schema
        self.num_labels = len(set([src for src, _, _ in schema.valid_edge_schemas] + [tgt for _, _, tgt in schema.valid_edge_schemas]))
        self.num_edge_types = len(set([edge for _, edge, _ in schema.valid_edge_schemas]))

    def estimate_pattern_count(self, max_paths_per_star: int, max_path_length: int, complexity: str=None) -> int:
        L = self.num_labels
        E = self.num_edge_types
        if complexity == 'simple':
            paths_per_star = 1
            path_length = 1
        elif complexity == 'medium':
            paths_per_star = 2
            path_length = 2
        elif complexity == 'complex':
            paths_per_star = 3
            path_length = 3
        else:
            paths_per_star = max_paths_per_star
            path_length = max_path_length
        single_path_variants = E ** path_length * L ** max(0, path_length - 1)
        user_star_variants = single_path_variants ** paths_per_star
        item_star_variants = single_path_variants ** paths_per_star
        total_estimate = user_star_variants * item_star_variants
        sparsity_factor = 0.001 if L <= 5 else 0.01
        realistic_estimate = int(total_estimate * sparsity_factor)
        realistic_estimate = max(10, min(realistic_estimate, 1000000))
        logger.debug(f'Estimated {complexity} patterns: {realistic_estimate}')
        return realistic_estimate

    def recommend_targets(self, desired_total: int, max_paths_per_star: int, max_path_length: int) -> Dict[str, int]:
        simple_est = self.estimate_pattern_count(max_paths_per_star, max_path_length, 'simple')
        medium_est = self.estimate_pattern_count(max_paths_per_star, max_path_length, 'medium')
        complex_est = self.estimate_pattern_count(max_paths_per_star, max_path_length, 'complex')
        total_est = simple_est + medium_est + complex_est
        if total_est < desired_total:
            logger.warning(f'Desired {desired_total} patterns but only ~{total_est} exist!')
            logger.warning(f'Adjusting target to {total_est}')
            desired_total = total_est
        if total_est > 0:
            simple_ratio = simple_est / total_est
            medium_ratio = medium_est / total_est
            complex_ratio = complex_est / total_est
        else:
            simple_ratio = 0.3
            medium_ratio = 0.4
            complex_ratio = 0.3
        recommended = {'simple': min(int(desired_total * simple_ratio), simple_est), 'medium': min(int(desired_total * medium_ratio), medium_est), 'complex': min(int(desired_total * complex_ratio), complex_est)}
        recommended['total'] = sum((v for k, v in recommended.items() if k != 'total'))
        return recommended

class AttributeImportanceAnalyzer:

    def __init__(self, graph: Graph, training_data: Dict[Tuple[Any, Any], bool]):
        self.graph = graph
        self.training_data = training_data
        self.importance_scores = self._compute_importance()

    def _compute_importance(self) -> Dict[str, float]:
        all_attributes = set()
        for node in self.graph.all_nodes():
            attrs = node.attributes
            if isinstance(attrs, dict):
                all_attributes.update(attrs.keys())
            elif isinstance(attrs, (set, list)):
                all_attributes.update(attrs)
        all_attributes = sorted(all_attributes)
        positive_pairs = [(u, i) for (u, i), label in self.training_data.items() if label]
        negative_pairs = [(u, i) for (u, i), label in self.training_data.items() if not label]
        sample_size = min(5000, len(positive_pairs))
        pos_sample_idx = random.sample(range(len(positive_pairs)), sample_size)
        neg_sample_idx = random.sample(range(len(negative_pairs)), min(sample_size, len(negative_pairs)))
        importance_dict = {}
        for attr in all_attributes:
            pos_user_numeric = []
            pos_item_numeric = []
            neg_user_numeric = []
            neg_item_numeric = []
            user_cat_counts = defaultdict(lambda: [0, 0])
            item_cat_counts = defaultdict(lambda: [0, 0])
            for idx in pos_sample_idx:
                user_id, item_id = positive_pairs[idx]
                user = self.graph.get_node(user_id)
                item = self.graph.get_node(item_id)
                if user:
                    val = self._get_attribute_value(user, attr)
                    if val is not None:
                        if isinstance(val, (int, float)):
                            pos_user_numeric.append(float(val))
                        elif isinstance(val, str):
                            user_cat_counts[val][0] += 1
                if item:
                    val = self._get_attribute_value(item, attr)
                    if val is not None:
                        if isinstance(val, (int, float)):
                            pos_item_numeric.append(float(val))
                        elif isinstance(val, str):
                            item_cat_counts[val][0] += 1
            for idx in neg_sample_idx:
                user_id, item_id = negative_pairs[idx]
                user = self.graph.get_node(user_id)
                item = self.graph.get_node(item_id)
                if user:
                    val = self._get_attribute_value(user, attr)
                    if val is not None:
                        if isinstance(val, (int, float)):
                            neg_user_numeric.append(float(val))
                        elif isinstance(val, str):
                            user_cat_counts[val][1] += 1
                if item:
                    val = self._get_attribute_value(item, attr)
                    if val is not None:
                        if isinstance(val, (int, float)):
                            neg_item_numeric.append(float(val))
                        elif isinstance(val, str):
                            item_cat_counts[val][1] += 1
            user_score = 0.0
            if len(pos_user_numeric) > 10 and len(neg_user_numeric) > 10:
                ks_stat, ks_pval = ks_2samp(pos_user_numeric, neg_user_numeric)
                cohens_d = self._compute_cohens_d(pos_user_numeric, neg_user_numeric)
                user_score = ks_stat * (1 - ks_pval) * cohens_d if ks_pval < 0.05 else 0.0
            item_score = 0.0
            if len(pos_item_numeric) > 10 and len(neg_item_numeric) > 10:
                ks_stat, ks_pval = ks_2samp(pos_item_numeric, neg_item_numeric)
                cohens_d = self._compute_cohens_d(pos_item_numeric, neg_item_numeric)
                item_score = ks_stat * (1 - ks_pval) * cohens_d if ks_pval < 0.05 else 0.0
            numeric_score = max(user_score, item_score)
            cat_score = max(self._compute_cramers_v(user_cat_counts), self._compute_cramers_v(item_cat_counts))
            importance_dict[attr] = max(numeric_score, cat_score)
        max_importance = max(importance_dict.values()) if importance_dict else 1.0
        if max_importance == 0:
            logger.warning('  All attributes have 0 discriminative power')
            logger.warning('  Using frequency-based importance as fallback')
            importance_dict = self._compute_frequency_importance(all_attributes)
            max_importance = max(importance_dict.values()) if importance_dict else 1.0
            if max_importance == 0:
                logger.warning('  Using uniform distribution')
                for attr in importance_dict:
                    importance_dict[attr] = 1.0 / len(importance_dict)
        else:
            for attr in importance_dict:
                importance_dict[attr] /= max_importance
        for attr, score in sorted(importance_dict.items(), key=lambda x: x[1], reverse=True):
            pass
        return importance_dict

    def _get_attribute_value(self, node, attr):
        attrs = node.attributes
        if isinstance(attrs, dict):
            return attrs.get(attr)
        elif isinstance(attrs, (set, list)):
            return 1.0 if attr in attrs else 0.0
        return None

    def _compute_frequency_importance(self, attributes: List[str]) -> Dict[str, float]:
        positive_pairs = [(u, i) for (u, i), label in self.training_data.items() if label]
        sample_size = min(1000, len(positive_pairs))
        sample = random.sample(positive_pairs, sample_size)
        attr_counts = defaultdict(int)
        total_nodes = 0
        for user_id, item_id in sample:
            user = self.graph.get_node(user_id)
            item = self.graph.get_node(item_id)
            for node in [user, item]:
                if not node:
                    continue
                total_nodes += 1
                attrs = node.attributes
                if isinstance(attrs, dict):
                    for attr in attrs.keys():
                        if attr in attributes:
                            attr_counts[attr] += 1
                elif isinstance(attrs, (set, list)):
                    for attr in attrs:
                        if attr in attributes:
                            attr_counts[attr] += 1
        importance_dict = {}
        max_count = max(attr_counts.values()) if attr_counts else 1
        for attr in attributes:
            count = attr_counts.get(attr, 0)
            importance_dict[attr] = count / max_count if max_count > 0 else 0.0
        return importance_dict

    def _compute_cohens_d(self, values1: List[float], values2: List[float]) -> float:
        if len(values1) == 0 or len(values2) == 0:
            return 0.0
        mean1 = np.mean(values1)
        mean2 = np.mean(values2)
        std1 = np.std(values1, ddof=1) if len(values1) > 1 else 0
        std2 = np.std(values2, ddof=1) if len(values2) > 1 else 0
        n1 = len(values1)
        n2 = len(values2)
        pooled_std = np.sqrt(((n1 - 1) * std1 ** 2 + (n2 - 1) * std2 ** 2) / (n1 + n2 - 2))
        if pooled_std == 0:
            return 0.0
        return abs(mean1 - mean2) / pooled_std

    def _compute_cramers_v(self, cat_counts: Dict) -> float:
        if len(cat_counts) < 2:
            return 0.0
        table = [counts for counts in cat_counts.values()]
        total = sum((c[0] + c[1] for c in table))
        if total < 20:
            return 0.0
        try:
            from scipy.stats import chi2_contingency
            chi2, pval, _, _ = chi2_contingency(table)
            n = total
            k = min(len(table), 2)
            if k <= 1 or n == 0:
                return 0.0
            cramers_v = np.sqrt(chi2 / (n * (k - 1)))
            return float(cramers_v) if pval < 0.05 else 0.0
        except Exception:
            return 0.0

    def _extract_features(self, user: Any, item: Any, all_attributes: List[str]) -> List[float]:
        features = []
        for attr in all_attributes:
            user_val = user.attributes.get(attr, 0) if isinstance(user.attributes, dict) else 0
            if isinstance(user_val, (int, float)):
                user_val = min(float(user_val) / 10000.0, 1.0)
            elif isinstance(user_val, str):
                user_val = hash(user_val) % 100 / 100.0
            else:
                user_val = 0.0
            features.append(user_val)
            item_val = item.attributes.get(attr, 0) if isinstance(item.attributes, dict) else 0
            if isinstance(item_val, (int, float)):
                item_val = min(float(item_val) / 10000.0, 1.0)
            elif isinstance(item_val, str):
                item_val = hash(item_val) % 100 / 100.0
            else:
                item_val = 0.0
            features.append(item_val)
        return features

    def _compute_attribute_cooccurrence(self, attributes: List[str]) -> Dict[Tuple[str, str], float]:
        positive_pairs = [(u, i) for (u, i), label in self.training_data.items() if label]
        sample_size = min(2000, len(positive_pairs))
        sample = random.sample(positive_pairs, sample_size)
        cooccurrence_counts = defaultdict(int)
        single_counts = defaultdict(int)
        total_pairs = 0
        for user_id, item_id in sample:
            user = self.graph.get_node(user_id)
            item = self.graph.get_node(item_id)
            if not user or not item:
                continue
            present_attrs = set()
            for node in [user, item]:
                attrs = node.attributes
                if isinstance(attrs, dict):
                    for attr in attrs.keys():
                        if attr in attributes:
                            present_attrs.add(attr)
                elif isinstance(attrs, (set, list)):
                    for attr in attrs:
                        if attr in attributes:
                            present_attrs.add(attr)
            for attr in present_attrs:
                single_counts[attr] += 1
            for attr1 in present_attrs:
                for attr2 in present_attrs:
                    if attr1 < attr2:
                        cooccurrence_counts[attr1, attr2] += 1
            total_pairs += 1
        cooccurrence_scores = {}
        for (attr1, attr2), count in cooccurrence_counts.items():
            p_ab = count / total_pairs if total_pairs > 0 else 0
            p_a = single_counts[attr1] / total_pairs if total_pairs > 0 else 0
            p_b = single_counts[attr2] / total_pairs if total_pairs > 0 else 0
            if p_a > 0 and p_b > 0:
                pmi = np.log(max(p_ab, 1e-10) / max(p_a * p_b, 1e-10))
                cooccurrence_scores[attr1, attr2] = max(0, min(1, (pmi + 10) / 20))
        return cooccurrence_scores

    def get_complementary_attributes(self, selected_attrs: Set[str], available_attrs: List[str], top_k: int=5) -> List[str]:
        if not hasattr(self, '_cooccurrence'):
            all_attrs = sorted(set(available_attrs) | selected_attrs)
            self._cooccurrence = self._compute_attribute_cooccurrence(all_attrs)
        scores = {}
        for candidate in available_attrs:
            if candidate in selected_attrs:
                continue
            cooccur_scores = []
            for selected in selected_attrs:
                pair = tuple(sorted([candidate, selected]))
                if pair in self._cooccurrence:
                    cooccur_scores.append(self._cooccurrence[pair])
            if cooccur_scores:
                scores[candidate] = np.mean(cooccur_scores)
            else:
                scores[candidate] = 0.0
        ranked = sorted(scores.items(), key=lambda x: x[1], reverse=True)
        return [attr for attr, score in ranked[:top_k]]

    def get_importance(self, attribute: str) -> float:
        return self.importance_scores.get(attribute, 0.0)

    def get_ranked_attributes(self) -> List[Tuple[str, float]]:
        return sorted(self.importance_scores.items(), key=lambda x: x[1], reverse=True)

class BudgetAttributeAssigner:

    def __init__(self, graph: Graph, training_data: Dict, data_dir: str=None):
        self.graph = graph
        self.training_data = training_data
        self.importance_analyzer = AttributeImportanceAnalyzer(graph, training_data)
        self.ranked_attributes = self.importance_analyzer.get_ranked_attributes()
        self.attr_config = None
        if data_dir is not None:
            self.attr_config = load_attribute_config(data_dir, graph)

    def assign_attributes(self, state: RLState, strategy: str='important', ensure_leaf_coverage: bool=True) -> DualStarPattern:
        pattern = state.to_pattern()
        node_roles = self._classify_node_roles(pattern)
        node_type_attrs = self._get_attributes_by_node_type()
        attribute_assignments = {}
        for var, node in pattern.nodes.items():
            role = node_roles.get(var, 'intermediate')
            available_attrs = node_type_attrs.get(node.label, [])
            if not available_attrs:
                attribute_assignments[var] = set()
                continue
            ranked_attrs = self._rank_attributes_for_label(node.label, available_attrs, strategy)
            if role == 'pivot':
                num_attrs = random.randint(2, 3)
                selected = set(ranked_attrs[:min(num_attrs, len(ranked_attrs))])
            elif role == 'leaf':
                if ensure_leaf_coverage:
                    num_attrs = random.randint(2, 3)
                else:
                    num_attrs = random.randint(1, 3)
                selected = set(ranked_attrs[:min(num_attrs, len(ranked_attrs))])
                if strategy == 'diverse' and len(selected) < num_attrs and (len(ranked_attrs) > num_attrs):
                    complementary = self.importance_analyzer.get_complementary_attributes(selected, ranked_attrs, top_k=num_attrs - len(selected))
                    selected.update(complementary[:num_attrs - len(selected)])
            else:
                num_attrs = random.randint(1, 2)
                selected = set(ranked_attrs[:min(num_attrs, len(ranked_attrs))])
            if not selected and ranked_attrs:
                selected.add(ranked_attrs[0])
            attribute_assignments[var] = selected
        attribute_assignments = self._enforce_id_pairing(pattern, attribute_assignments)
        for var, attrs in attribute_assignments.items():
            pattern.nodes[var].attributes = attrs
        total_attrs = sum((len(attrs) for attrs in attribute_assignments.values()))
        leaf_count = sum((1 for role in node_roles.values() if role == 'leaf'))
        leaf_attrs = sum((len(attribute_assignments.get(var, set())) for var, role in node_roles.items() if role == 'leaf'))
        logger.debug(f'Pattern: {len(pattern.nodes)} nodes ({leaf_count} leaves with {leaf_attrs} attrs total), {total_attrs} attrs assigned')
        return pattern

    def _classify_node_roles(self, pattern: DualStarPattern) -> Dict[str, str]:
        roles = {}
        roles['x'] = 'pivot'
        roles['y'] = 'pivot'
        for path in pattern.user_paths + pattern.item_paths:
            path_length = len(path.path_sequence)
            for i, var in enumerate(path.path_sequence):
                if var in ['x', 'y']:
                    continue
                if i == path_length - 1:
                    roles[var] = 'leaf'
                else:
                    roles[var] = 'intermediate'
        return roles

    def _rank_attributes_for_label(self, node_label: str, available_attrs: List[str], strategy: str) -> List[str]:
        attr_scores = [(attr, self.importance_analyzer.get_importance(attr)) for attr in available_attrs]
        attr_scores.sort(key=lambda x: x[1], reverse=True)
        if strategy == 'important':
            return [attr for attr, score in attr_scores]
        elif strategy == 'balanced':
            top_third = max(1, len(attr_scores) // 3)
            top = [attr for attr, score in attr_scores[:top_third]]
            mid = [attr for attr, score in attr_scores[top_third:2 * top_third]]
            random.shuffle(mid)
            return top + mid[:len(top)]
        elif strategy == 'diverse':
            random.shuffle(attr_scores)
            return [attr for attr, score in attr_scores]
        else:
            return [attr for attr, score in attr_scores]

    def _get_attributes_by_node_type(self) -> Dict[str, List[str]]:
        if hasattr(self, '_node_type_attrs_cache'):
            return self._node_type_attrs_cache
        node_type_attrs = defaultdict(set)
        for node in self.graph.all_nodes():
            attrs = node.attributes
            if isinstance(attrs, dict):
                node_type_attrs[node.label].update(attrs.keys())
            elif isinstance(attrs, (set, list)):
                node_type_attrs[node.label].update(attrs)
        self._node_type_attrs_cache = {label: list(attrs) for label, attrs in node_type_attrs.items()}
        return self._node_type_attrs_cache

    def _enforce_id_pairing(self, pattern, attribute_assignments: Dict[str, set]) -> Dict[str, set]:
        if self.attr_config is None:
            return attribute_assignments
        id_groups = defaultdict(list)
        for var, attrs in attribute_assignments.items():
            if var not in pattern.nodes:
                continue
            node = pattern.nodes[var]
            for attr in list(attrs):
                if self.attr_config.is_id(attr, node.label):
                    id_groups[node.label, attr].append(var)
        if not id_groups:
            return attribute_assignments
        for (label, id_attr), vars_with_attr in id_groups.items():
            count = len(vars_with_attr)
            if count == 0:
                continue
            if count % 2 == 0:
                continue
            partner_found = False
            for var, node in pattern.nodes.items():
                if node.label != label:
                    continue
                if var in vars_with_attr:
                    continue
                attribute_assignments[var].add(id_attr)
                logger.debug(f'    [ID Pairing] Added {id_attr} to {var} (partner for {vars_with_attr})')
                partner_found = True
                break
            if not partner_found:
                remove_from = vars_with_attr[-1]
                attribute_assignments[remove_from].discard(id_attr)
                logger.debug(f'    [ID Pairing] Removed unpaired {id_attr} from {remove_from}')
        total_id_attrs = 0
        for var, attrs in attribute_assignments.items():
            if var not in pattern.nodes:
                continue
            node = pattern.nodes[var]
            for attr in attrs:
                if self.attr_config.is_id(attr, node.label):
                    total_id_attrs += 1
        if total_id_attrs % 2 != 0:
            logger.warning(f'    [ID Pairing] Total ID attrs = {total_id_attrs} (odd!) — removing one')
            for var, attrs in attribute_assignments.items():
                if var not in pattern.nodes:
                    continue
                node = pattern.nodes[var]
                for attr in list(attrs):
                    if self.attr_config.is_id(attr, node.label):
                        attrs.discard(attr)
                        logger.debug(f'    [ID Pairing] Emergency removal: {attr} from {var}')
                        total_id_attrs -= 1
                        break
                if total_id_attrs % 2 == 0:
                    break
        logger.debug(f'    [ID Pairing] Final ID attr count: {total_id_attrs} (paired: {total_id_attrs // 2})')
        return attribute_assignments

    def get_statistics(self) -> Dict:
        if not self.ranked_attributes:
            return {}
        importances = [score for _, score in self.ranked_attributes]
        return {'num_attributes': len(self.ranked_attributes), 'min_importance': min(importances) if importances else 0, 'max_importance': max(importances) if importances else 0, 'avg_importance': sum(importances) / len(importances) if importances else 0, 'top_5_most_important': self.ranked_attributes[:5], 'top_5_least_important': self.ranked_attributes[-5:]}

class PolicyNetwork(nn.Module):

    def __init__(self, state_dim: int=32, action_dim: int=16, hidden_dim: int=128):
        super().__init__()
        self.state_dim = state_dim
        self.action_dim = action_dim
        self.state_encoder = nn.Sequential(nn.Linear(state_dim, hidden_dim), nn.ReLU(), nn.Linear(hidden_dim, hidden_dim), nn.ReLU())
        self.action_scorer = nn.Sequential(nn.Linear(hidden_dim + action_dim, hidden_dim), nn.ReLU(), nn.Linear(hidden_dim, 1))

    def forward(self, state_features: torch.Tensor) -> torch.Tensor:
        return self.state_encoder(state_features)

    def encode_action(self, action: Union[AddEdgeAction, StopAction]) -> torch.Tensor:
        features = torch.zeros(self.action_dim)
        if isinstance(action, StopAction):
            features[0] = 1.0
            return features
        features[0] = 0.0
        features[1] = 1.0 if action.is_new_path() else 0.0
        features[2] = 1.0 if action.star_type == 'user' else 0.0
        edge_hash = hash(action.edge_label) % 1000
        features[3] = edge_hash / 1000.0
        features[4] = edge_hash % 100 / 100.0
        target_hash = hash(action.to_label) % 1000
        features[5] = target_hash / 1000.0
        features[6] = target_hash % 100 / 100.0
        if not action.is_new_path():
            from_hash = hash(action.from_var) % 100
            features[7] = from_hash / 100.0
        features[8] = 1.0 if action.is_forward else 0.0
        return features

    def encode_state(self, state: RLState) -> torch.Tensor:
        features = torch.zeros(self.state_dim)
        num_user = min(state.num_user_paths(), 5)
        num_item = min(state.num_item_paths(), 5)
        if num_user < 6:
            features[num_user] = 1.0
        if num_item < 6:
            features[6 + num_item] = 1.0
        if state.user_paths:
            avg_user_len = sum((p.length() for p in state.user_paths)) / len(state.user_paths)
            features[12] = avg_user_len / 5.0
        if state.item_paths:
            avg_item_len = sum((p.length() for p in state.item_paths)) / len(state.item_paths)
            features[13] = avg_item_len / 5.0
        features[14] = state.total_edges() / 20.0
        return features

    def select_action(self, state: RLState, valid_actions: List[Union[AddEdgeAction, StopAction]], deterministic: bool=False) -> Tuple[Union[AddEdgeAction, StopAction], torch.Tensor]:
        state_features = self.encode_state(state).unsqueeze(0)
        device = next(self.parameters()).device
        state_features = state_features.to(device)
        state_emb = self.state_encoder(state_features)
        action_logits = []
        for action in valid_actions:
            action_features = self.encode_action(action).unsqueeze(0).to(device)
            combined = torch.cat([state_emb, action_features], dim=1)
            logit = self.action_scorer(combined)
            action_logits.append(logit)
        logits = torch.cat(action_logits, dim=0).squeeze()
        if logits.dim() == 0:
            logits = logits.unsqueeze(0)
        dist = Categorical(logits=logits)
        if deterministic:
            action_idx_tensor = logits.argmax()
        else:
            action_idx_tensor = dist.sample()
        action_idx = action_idx_tensor.item()
        selected_action = valid_actions[action_idx]
        log_prob = dist.log_prob(action_idx_tensor)
        return (selected_action, log_prob)

class AdaptiveSampler:

    def __init__(self, start_size: int=50, end_size: int=500, warmup_episodes: int=500):
        self.start_size = start_size
        self.end_size = end_size
        self.warmup_episodes = warmup_episodes
        self.episode_count = 0

    def get_sample_size(self) -> int:
        if self.episode_count < self.warmup_episodes:
            progress = self.episode_count / self.warmup_episodes
            size = int(self.start_size + (self.end_size - self.start_size) * progress)
            return size
        else:
            return self.end_size

    def step(self):
        self.episode_count += 1

class RewardEstimator:

    def __init__(self, graph: Graph, training_data: Dict[Tuple[Any, Any], bool], sample_size: int=50, min_support: int=10, min_confidence: float=0.6, lambda_complexity: float=0.1, use_adaptive_sampling: bool=True):
        self.graph = graph
        self.training_data = training_data
        self.sample_size = sample_size
        self.min_support = min_support
        self.min_confidence = min_confidence
        self.lambda_complexity = lambda_complexity
        self.positive_pairs = [(u, i) for (u, i), label in training_data.items() if label]
        self.negative_pairs = [(u, i) for (u, i), label in training_data.items() if not label]
        self.support_cache: Dict[str, int] = {}
        self.confidence_cache: Dict[str, float] = {}
        self.seen_patterns: Set[str] = set()
        self.use_adaptive_sampling = use_adaptive_sampling
        if use_adaptive_sampling:
            self.adaptive_sampler = AdaptiveSampler(start_size=50, end_size=500, warmup_episodes=500)

    def estimate_support(self, pattern: DualStarPattern) -> int:
        sig = self._get_signature(pattern)
        if sig in self.support_cache:
            return self.support_cache[sig]
        if self.use_adaptive_sampling:
            base_sample_size = self.adaptive_sampler.get_sample_size()
        else:
            base_sample_size = self.sample_size
        num_paths = len(pattern.user_paths) + len(pattern.item_paths)
        num_edges = sum((len(p.path_sequence) - 1 for p in pattern.user_paths + pattern.item_paths))
        if num_paths <= 2 and num_edges <= 2:
            sample_size = base_sample_size
        elif num_paths <= 4 and num_edges <= 6:
            sample_size = int(base_sample_size * 1.5)
        else:
            sample_size = int(base_sample_size * 2.0)
        sample_size = min(sample_size, len(self.positive_pairs))
        sample_pairs = random.sample(self.positive_pairs, sample_size)
        match_count = 0
        for user_id, item_id in sample_pairs:
            try:
                match = compute_compact_match(self.graph, pattern, user_id, item_id, {})
                if match is not None:
                    match_count += 1
            except Exception:
                pass
        match_rate = match_count / len(sample_pairs) if sample_pairs else 0
        estimated_support = int(match_rate * len(self.positive_pairs))
        self.support_cache[sig] = estimated_support
        return estimated_support

    def estimate_confidence(self, pattern: DualStarPattern) -> float:
        sig = self._get_signature(pattern)
        if sig in self.confidence_cache:
            return self.confidence_cache[sig]
        support = self.estimate_support(pattern)
        pos_match_rate = support / len(self.positive_pairs) if self.positive_pairs else 0.0
        if self.use_adaptive_sampling:
            base_sample_size = self.adaptive_sampler.get_sample_size()
        else:
            base_sample_size = self.sample_size
        neg_sample_size = min(base_sample_size, len(self.negative_pairs))
        if neg_sample_size > 0:
            neg_sample = random.sample(self.negative_pairs, neg_sample_size)
            neg_match_count = 0
            for user_id, item_id in neg_sample:
                try:
                    match = compute_compact_match(self.graph, pattern, user_id, item_id, {})
                    if match is not None:
                        neg_match_count += 1
                except Exception:
                    pass
            neg_match_rate = neg_match_count / neg_sample_size
        else:
            neg_match_rate = 0.0
        est_tp = pos_match_rate * len(self.positive_pairs)
        est_fp = neg_match_rate * len(self.negative_pairs)
        if est_tp + est_fp > 0:
            confidence = est_tp / (est_tp + est_fp)
        else:
            confidence = 0.0
        self.confidence_cache[sig] = confidence
        return confidence

    def compute_reward(self, old_state: RLState, action: Union[AddEdgeAction, StopAction], new_state: RLState) -> float:
        if isinstance(action, StopAction):
            return self._compute_terminal_reward(new_state)
        else:
            return self._compute_intermediate_reward(old_state, new_state)

    def _compute_intermediate_reward(self, old_state: RLState, new_state: RLState) -> float:

        def _path_struct_sig(p) -> str:
            parts = []
            for i, el in enumerate(p.edges):
                sl = p.nodes[i].label
                tl = p.nodes[i + 1].label
                d = 'F' if i < len(p.edge_directions) and p.edge_directions[i] else 'R'
                parts.append(f'{sl}-{el}:{d}-{tl}')
            return '|'.join(parts) if parts else p.nodes[0].label if p.nodes else ''
        new_user_sigs = set((_path_struct_sig(p) for p in new_state.user_paths))
        new_item_sigs = set((_path_struct_sig(p) for p in new_state.item_paths))
        if len(new_user_sigs) < len(new_state.user_paths):
            return -0.5
        if len(new_item_sigs) < len(new_state.item_paths):
            return -0.5
        old_balance = abs(old_state.num_user_paths() - old_state.num_item_paths())
        new_balance = abs(new_state.num_user_paths() - new_state.num_item_paths())
        if new_balance < old_balance:
            return 0.3
        elif new_balance > old_balance + 1:
            return -0.3
        return 0.1

    def _compute_terminal_reward(self, state: RLState) -> float:
        pattern = state.to_pattern()
        sig = state.get_signature()
        num_user = state.num_user_paths()
        num_item = state.num_item_paths()
        if num_user < 1 or num_item < 1:
            return -5.0
        support = self.estimate_support(pattern)
        confidence = self.estimate_confidence(pattern)
        r_support = np.log10(max(support, 1)) * 1.0
        if confidence >= self.min_confidence:
            r_confidence = 3.0
        elif confidence >= self.min_confidence * 0.75:
            r_confidence = 1.0
        elif confidence >= self.min_confidence * 0.5:
            r_confidence = -0.5
        else:
            r_confidence = -2.0
        total_paths = num_user + num_item
        total_edges = sum((p.length() for p in state.user_paths + state.item_paths))
        complexity_score = total_paths * 3 + total_edges * 1
        r_complexity = -0.5 * complexity_score
        if sig in self.seen_patterns:
            r_diversity = -3.0
        else:
            r_diversity = 0.5
            self.seen_patterns.add(sig)
        path_diff = abs(num_user - num_item)
        if path_diff == 0:
            r_balance = 1.0
        elif path_diff == 1:
            r_balance = 0.3
        else:
            r_balance = -0.5 * path_diff
        if num_user > 0 and num_item > 0:
            avg_user_len = sum((p.length() for p in state.user_paths)) / num_user
            avg_item_len = sum((p.length() for p in state.item_paths)) / num_item
            avg_length = (avg_user_len + avg_item_len) / 2.0
            if avg_length >= 2.0:
                r_path_length = 0.5
            else:
                r_path_length = 0.0
        else:
            r_path_length = 0.0
        total_reward = r_support * 2.0 + r_confidence * 1.5 + r_complexity + r_diversity + r_balance + r_path_length
        total_reward = max(-10.0, min(10.0, total_reward))
        return total_reward

    def _get_signature(self, pattern: DualStarPattern) -> str:

        def _path_sig(path) -> str:
            parts = []
            seq = path.path_sequence
            for i in range(len(seq) - 1):
                src_label = pattern.nodes[seq[i]].label
                tgt_label = pattern.nodes[seq[i + 1]].label
                edge = path.edges[i]
                direction = 'F' if edge.is_forward else 'R'
                parts.append(f'{src_label}-{edge.label}:{direction}-{tgt_label}')
            return '|'.join(parts) if parts else pattern.nodes[seq[0]].label if seq else ''
        user_sigs = sorted((_path_sig(p) for p in pattern.user_paths))
        item_sigs = sorted((_path_sig(p) for p in pattern.item_paths))
        return f'U[{';'.join(user_sigs)}]|I[{';'.join(item_sigs)}]'

class RLActionSpace:

    def __init__(self, schema: GraphSchema, max_paths_per_star: int=3, max_path_length: int=3):
        self.schema = schema
        self.max_paths_per_star = max_paths_per_star
        self.max_path_length = max_path_length
        self._labels_with_attributes = self._compute_labels_with_attributes()

    def _compute_labels_with_attributes(self) -> Set[str]:
        label_attrs = defaultdict(set)
        graph = self.schema.graph
        label_sample_count = defaultdict(int)
        for node in graph.all_nodes():
            if label_sample_count[node.label] >= 500:
                continue
            label_sample_count[node.label] += 1
            attrs = node.attributes
            if isinstance(attrs, dict):
                label_attrs[node.label].update(attrs.keys())
            elif isinstance(attrs, (set, list)):
                label_attrs[node.label].update(attrs)
        return {label for label, attrs in label_attrs.items() if len(attrs) >= 2}

    def _precompute_edge_frequencies(self):
        out_counts = defaultdict(lambda: defaultdict(int))
        in_counts = defaultdict(lambda: defaultdict(int))
        graph = self.schema.graph
        if hasattr(graph, 'edge_sources') and hasattr(graph, 'edge_targets'):
            for edge_label in graph.edge_sources:
                src_sample = list(graph.edge_sources[edge_label])[:200]
                tgt_sample = list(graph.edge_targets.get(edge_label, set()))[:200]
                src_labels = defaultdict(int)
                tgt_labels = defaultdict(int)
                for nid in src_sample:
                    node = graph.get_node(nid)
                    if node:
                        src_labels[node.label] += 1
                for nid in tgt_sample:
                    node = graph.get_node(nid)
                    if node:
                        tgt_labels[node.label] += 1
                total_edges = len(graph.edge_sources[edge_label])
                for sl, sc in src_labels.items():
                    for tl, tc in tgt_labels.items():
                        if self.schema.is_valid_edge(sl, edge_label, tl):
                            est_count = total_edges * (sc / len(src_sample)) * (tc / len(tgt_sample))
                            out_counts[sl][edge_label, tl] += int(est_count)
                            in_counts[tl][edge_label, sl] += int(est_count)
        else:
            for edge in graph.all_edges():
                source = graph.get_node(edge.source)
                target = graph.get_node(edge.target)
                if source and target:
                    out_counts[source.label][edge.label, target.label] += 1
                    in_counts[target.label][edge.label, source.label] += 1
        self._frequent_edges_directed = {}
        all_labels = set(out_counts.keys()) | set(in_counts.keys())
        for label in all_labels:
            directed = []
            out_sorted = sorted(out_counts[label].items(), key=lambda x: x[1], reverse=True)
            for (el, tl), cnt in out_sorted[:5]:
                directed.append((el, tl, True))
            in_sorted = sorted(in_counts[label].items(), key=lambda x: x[1], reverse=True)
            for (el, sl), cnt in in_sorted[:5]:
                directed.append((el, sl, False))
            self._frequent_edges_directed[label] = directed

    def _get_frequent_edges_directed(self, node_label: str) -> List[Tuple[str, str, bool]]:
        if not hasattr(self, '_frequent_edges_directed'):
            self._precompute_edge_frequencies()
        frequent = self._frequent_edges_directed.get(node_label, [])
        if frequent:
            return [(el, tl, fwd) for el, tl, fwd in frequent if tl in self._labels_with_attributes]
        result = []
        for el, tl in self.schema.get_valid_outgoing(node_label):
            if tl in self._labels_with_attributes:
                result.append((el, tl, True))
        for el, sl in self.schema.get_valid_incoming(node_label):
            if sl in self._labels_with_attributes:
                result.append((el, sl, False))
        return result

    def get_valid_actions(self, state: RLState) -> List[Union[AddEdgeAction, StopAction]]:
        valid_actions = []
        has_user_paths = state.num_user_paths() >= 1
        has_item_paths = state.num_item_paths() >= 1
        is_dual_star = has_user_paths and has_item_paths
        if is_dual_star:
            valid_actions.append(StopAction())
        user_budget_full = state.num_user_paths() >= self.max_paths_per_star
        item_budget_full = state.num_item_paths() >= self.max_paths_per_star
        if user_budget_full and item_budget_full:
            if is_dual_star:
                return [StopAction()]
            else:
                logger.warning(f'Both budgets full but not dual-star: user={state.num_user_paths()}, item={state.num_item_paths()}')
                return [StopAction()]
        if not user_budget_full:
            for edge_label, target_label, is_fwd in self._get_frequent_edges_directed(state.user_center_label):
                valid_actions.append(AddEdgeAction(from_var='x', edge_label=edge_label, to_label=target_label, star_type='user', is_forward=is_fwd))
            for path_idx, path in enumerate(state.user_paths):
                if path.length() < self.max_path_length:
                    last_node = path.last_node()
                    for edge_label, target_label, is_fwd in self._get_frequent_edges_directed(last_node.label):
                        valid_actions.append(AddEdgeAction(from_var=last_node.variable, edge_label=edge_label, to_label=target_label, star_type='user', is_forward=is_fwd))
        if not item_budget_full:
            for edge_label, target_label, is_fwd in self._get_frequent_edges_directed(state.item_center_label):
                valid_actions.append(AddEdgeAction(from_var='y', edge_label=edge_label, to_label=target_label, star_type='item', is_forward=is_fwd))
            for path_idx, path in enumerate(state.item_paths):
                if path.length() < self.max_path_length:
                    last_node = path.last_node()
                    for edge_label, target_label, is_fwd in self._get_frequent_edges_directed(last_node.label):
                        valid_actions.append(AddEdgeAction(from_var=last_node.variable, edge_label=edge_label, to_label=target_label, star_type='item', is_forward=is_fwd))
        if not valid_actions:
            logger.error(f'No valid actions! State: user_paths={state.num_user_paths()}, item_paths={state.num_item_paths()}')
            if not has_user_paths and (not user_budget_full):
                for edge_label, target_label in self.schema.get_valid_outgoing(state.user_center_label):
                    valid_actions.append(AddEdgeAction(from_var='x', edge_label=edge_label, to_label=target_label, star_type='user', is_forward=True))
                    break
                if not valid_actions:
                    for edge_label, source_label in self.schema.get_valid_incoming(state.user_center_label):
                        valid_actions.append(AddEdgeAction(from_var='x', edge_label=edge_label, to_label=source_label, star_type='user', is_forward=False))
                        break
            if not has_item_paths and (not item_budget_full):
                for edge_label, target_label in self.schema.get_valid_outgoing(state.item_center_label):
                    valid_actions.append(AddEdgeAction(from_var='y', edge_label=edge_label, to_label=target_label, star_type='item', is_forward=True))
                    break
                if len(valid_actions) == 0 or (has_user_paths and (not has_item_paths)):
                    for edge_label, source_label in self.schema.get_valid_incoming(state.item_center_label):
                        valid_actions.append(AddEdgeAction(from_var='y', edge_label=edge_label, to_label=source_label, star_type='item', is_forward=False))
                        break
        return valid_actions

class DualStarRLEnvironment:

    def __init__(self, graph: Graph, training_data: Dict[Tuple[Any, Any], bool], max_paths_per_star: int=3, max_path_length: int=3, lambda_complexity: float=0.1, sample_size: int=50, min_support: int=10, min_confidence: float=0.6):
        self.graph = graph
        self.training_data = training_data
        self.schema = GraphSchema(graph)
        self.action_space = RLActionSpace(self.schema, max_paths_per_star, max_path_length)
        self.reward_estimator = RewardEstimator(graph, training_data, sample_size=sample_size, min_support=min_support, min_confidence=min_confidence, lambda_complexity=lambda_complexity)
        self.positive_pairs = [(u, i) for (u, i), label in training_data.items() if label]
        self.current_state: RLState = None

    def reset(self) -> RLState:
        if not hasattr(self, '_node_degree_stats'):
            self._compute_node_degree_stats()
        num_candidates = 10
        candidates = random.sample(self.positive_pairs, min(num_candidates, len(self.positive_pairs)))
        best_pair = None
        best_score = float('inf')
        for user_id, item_id in candidates:
            user = self.graph.get_node(user_id)
            item = self.graph.get_node(item_id)
            if not user or not item:
                continue
            user_degree = len(list(self.graph.get_out_edges(user_id)))
            item_degree = len(list(self.graph.get_in_edges(item_id)))
            user_dev = abs(user_degree - self._median_user_degree)
            item_dev = abs(item_degree - self._median_item_degree)
            total_dev = user_dev + item_dev
            if total_dev < best_score:
                best_score = total_dev
                best_pair = (user_id, item_id)
        if best_pair is None:
            best_pair = candidates[0]
        user_id, item_id = best_pair
        user = self.graph.get_node(user_id)
        item = self.graph.get_node(item_id)
        self.current_state = RLState(user_center_id=user_id, user_center_label=user.label, item_center_id=item_id, item_center_label=item.label, user_paths=[], item_paths=[])
        return self.current_state

    def _compute_node_degree_stats(self):
        user_degrees = []
        item_degrees = []
        sample_size = min(1000, len(self.positive_pairs))
        sample = random.sample(self.positive_pairs, sample_size)
        for user_id, item_id in sample:
            user = self.graph.get_node(user_id)
            item = self.graph.get_node(item_id)
            if user:
                user_degree = len(list(self.graph.get_out_edges(user_id)))
                user_degrees.append(user_degree)
            if item:
                item_degree = len(list(self.graph.get_in_edges(item_id)))
                item_degrees.append(item_degree)
        self._median_user_degree = int(np.median(user_degrees)) if user_degrees else 10
        self._median_item_degree = int(np.median(item_degrees)) if item_degrees else 10
        self._node_degree_stats = True

    def step(self, action: Union[AddEdgeAction, StopAction]) -> Tuple[RLState, float, bool, Dict]:
        old_state = self.current_state.copy()
        if isinstance(action, StopAction):
            reward = self.reward_estimator.compute_reward(old_state, action, self.current_state)
            return (self.current_state, reward, True, {'terminal': True})
        new_state = self._execute_add_edge(self.current_state, action)
        if new_state is None:
            logger.warning(f'Invalid action: {action}')
            return (self.current_state, -10.0, False, {'success': False})
        self.current_state = new_state
        reward = self.reward_estimator.compute_reward(old_state, action, new_state)
        valid_actions = self.action_space.get_valid_actions(new_state)
        done = all((isinstance(a, StopAction) for a in valid_actions))
        return (new_state, reward, done, {'success': True})

    def _execute_add_edge(self, state: RLState, action: AddEdgeAction) -> RLState:
        new_state = state.copy()
        if action.star_type == 'user':
            if action.is_new_path():
                new_var = f'x{new_state.next_user_var_idx}'
                new_state.next_user_var_idx += 1
                new_path = PathInConstruction(nodes=[PatternNode('x', state.user_center_label, set()), PatternNode(new_var, action.to_label, set())], edges=[action.edge_label], edge_directions=[action.is_forward])
                new_state.user_paths.append(new_path)
            else:
                extended = False
                for path in new_state.user_paths:
                    if path.last_node().variable == action.from_var:
                        new_var = f'x{new_state.next_user_var_idx}'
                        new_state.next_user_var_idx += 1
                        path.nodes.append(PatternNode(new_var, action.to_label, set()))
                        path.edges.append(action.edge_label)
                        path.edge_directions.append(action.is_forward)
                        extended = True
                        break
                if not extended:
                    return None
        elif action.star_type == 'item':
            if action.is_new_path():
                new_var = f'y{new_state.next_item_var_idx}'
                new_state.next_item_var_idx += 1
                new_path = PathInConstruction(nodes=[PatternNode('y', state.item_center_label, set()), PatternNode(new_var, action.to_label, set())], edges=[action.edge_label], edge_directions=[action.is_forward])
                new_state.item_paths.append(new_path)
            else:
                extended = False
                for path in new_state.item_paths:
                    if path.last_node().variable == action.from_var:
                        new_var = f'y{new_state.next_item_var_idx}'
                        new_state.next_item_var_idx += 1
                        path.nodes.append(PatternNode(new_var, action.to_label, set()))
                        path.edges.append(action.edge_label)
                        path.edge_directions.append(action.is_forward)
                        extended = True
                        break
                if not extended:
                    return None
        return new_state

    def get_valid_actions(self):
        return self.action_space.get_valid_actions(self.current_state)

class REINFORCETrainer:

    def __init__(self, env: DualStarRLEnvironment, policy_net: PolicyNetwork, lr: float=0.001, gamma: float=0.99, device: str='cuda'):
        self.env = env
        self.policy_net = policy_net.to(device)
        self.device = device
        self.gamma = gamma
        self.optimizer = torch.optim.Adam(policy_net.parameters(), lr=lr)

    def train(self, num_episodes: int=1000):
        episode_rewards = []
        with tqdm(total=num_episodes, desc='Training RL Policy', unit='episode') as pbar:
            for episode in range(num_episodes):
                trajectory = self._collect_trajectory()
                loss = self._update_policy(trajectory)
                episode_rewards.append(trajectory['total_reward'])
                if (episode + 1) % 10 == 0:
                    avg_reward = np.mean(episode_rewards[-10:])
                    pbar.set_postfix({'reward': f'{avg_reward:.2f}', 'loss': f'{loss:.4f}'})
                if hasattr(self.env.reward_estimator, 'adaptive_sampler'):
                    self.env.reward_estimator.adaptive_sampler.step()
                pbar.update(1)

    def _collect_trajectory(self) -> Dict:
        state = self.env.reset()
        states = []
        actions = []
        rewards = []
        log_probs = []
        done = False
        steps = 0
        max_steps = 50
        while not done and steps < max_steps:
            valid_actions = self.env.get_valid_actions()
            action, log_prob = self.policy_net.select_action(state, valid_actions, deterministic=False)
            next_state, reward, done, info = self.env.step(action)
            states.append(state)
            actions.append(action)
            rewards.append(reward)
            log_probs.append(log_prob)
            state = next_state
            steps += 1
        returns = self._compute_returns(rewards)
        return {'states': states, 'actions': actions, 'rewards': rewards, 'log_probs': torch.stack(log_probs), 'returns': returns, 'total_reward': sum(rewards)}

    def _compute_returns(self, rewards: List[float]) -> torch.Tensor:
        returns = []
        R = 0
        for r in reversed(rewards):
            R = r + self.gamma * R
            returns.insert(0, R)
        returns = torch.tensor(returns, dtype=torch.float32)
        if len(returns) > 1:
            returns = (returns - returns.mean()) / (returns.std() + 1e-08)
        return returns

    def _update_policy(self, trajectory: Dict) -> float:
        log_probs = trajectory['log_probs']
        returns = trajectory['returns'].to(self.device)
        loss = -(log_probs * returns).mean()
        self.optimizer.zero_grad()
        loss.backward()
        torch.nn.utils.clip_grad_norm_(self.policy_net.parameters(), 1.0)
        self.optimizer.step()
        return loss.item()

    def save_checkpoint(self, path: str):
        torch.save({'policy_state_dict': self.policy_net.state_dict(), 'optimizer_state_dict': self.optimizer.state_dict()}, path)

    def load_checkpoint(self, path: str):
        checkpoint = torch.load(path)
        self.policy_net.load_state_dict(checkpoint['policy_state_dict'])
        self.optimizer.load_state_dict(checkpoint['optimizer_state_dict'])

def validate_pattern_domain_constraints(pattern: DualStarPattern) -> Tuple[bool, str]:
    for path in pattern.user_paths + pattern.item_paths:
        if len(set(path.path_sequence)) < len(path.path_sequence):
            return (False, 'Pattern contains self-loop')
    user_sigs = ['|'.join(p.path_sequence) for p in pattern.user_paths]
    if len(set(user_sigs)) < len(user_sigs):
        return (False, 'Duplicate user paths')
    item_sigs = ['|'.join(p.path_sequence) for p in pattern.item_paths]
    if len(set(item_sigs)) < len(item_sigs):
        return (False, 'Duplicate item paths')
    for path in pattern.user_paths + pattern.item_paths:
        if len(path.edges) < 1:
            return (False, 'Path has no edges')
    for path in pattern.user_paths:
        if len(path.path_sequence) >= 2:
            last_var = path.path_sequence[-1]
            last_node = pattern.nodes.get(last_var)
            if last_node and last_node.label == 'user':
                last_edge = path.edges[-1]
                if last_edge.is_forward:
                    allowed_social_edges = {'friend', 'follows', 'recommends'}
                    if last_edge.label not in allowed_social_edges:
                        return (False, f'Invalid user->user edge: {last_edge.label}')
    return (True, '')

def validate_leaf_attribute_coverage(pattern: DualStarPattern, min_attrs: int=2) -> Tuple[bool, str]:
    leaf_nodes = set()
    for path in pattern.user_paths + pattern.item_paths:
        if len(path.path_sequence) > 1:
            last_var = path.path_sequence[-1]
            if last_var not in ['x', 'y']:
                leaf_nodes.add(last_var)
    for leaf_var in leaf_nodes:
        node = pattern.nodes.get(leaf_var)
        if not node:
            return (False, f'Leaf node {leaf_var} not found in pattern')
        num_attrs = len(node.attributes) if node.attributes else 0
        if num_attrs < min_attrs:
            return (False, f'Leaf node {leaf_var} has only {num_attrs} attributes (min: {min_attrs})')
    return (True, '')

def validate_pattern_quality(pattern: DualStarPattern) -> Tuple[bool, str]:
    valid, error = validate_pattern_domain_constraints(pattern)
    if not valid:
        return (False, error)
    valid, error = validate_leaf_attribute_coverage(pattern, min_attrs=2)
    if not valid:
        return (False, error)
    return (True, '')
__all__ = ['GraphSchema', 'AttributeImportanceAnalyzer', 'BudgetAttributeAssigner', 'PolicyNetwork', 'AdaptiveSampler', 'RewardEstimator', 'RLActionSpace', 'DualStarRLEnvironment', 'REINFORCETrainer', 'PatternSpaceEstimator', 'validate_pattern_domain_constraints', 'validate_leaf_attribute_coverage', 'validate_pattern_quality']
