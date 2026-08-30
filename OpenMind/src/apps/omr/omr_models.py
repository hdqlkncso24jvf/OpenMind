import logging
import re
import ast
import math
from typing import Any, Dict, Set, FrozenSet, Optional, Tuple
from collections import defaultdict
import numpy as np
from scipy import sparse
from data_structure import Graph
logger = logging.getLogger(__name__)

def _normalize_token(value: Any) -> str:
    return re.sub('\\s+', '_', str(value).strip().lower())

def _split_category_tokens(value: Any) -> Set[str]:
    if value is None:
        return set()
    if isinstance(value, (set, frozenset, list, tuple)):
        out: Set[str] = set()
        for item in value:
            out.update(_split_category_tokens(item))
        return out
    if isinstance(value, str):
        text = value.strip()
        if not text:
            return set()
        if text.startswith('[') and text.endswith(']'):
            try:
                parsed = ast.literal_eval(text)
            except Exception:
                parsed = None
            if parsed is not None:
                return _split_category_tokens(parsed)
        return {_normalize_token(part) for part in re.split('[|,/;]', text) if part.strip()}
    return {_normalize_token(value)}

def _passes_threshold(score: float, operator: str, threshold: float) -> bool:
    if operator == '>=':
        return score >= threshold
    if operator == '>':
        return score > threshold
    if operator == '<=':
        return score <= threshold
    if operator == '<':
        return score < threshold
    return True

class LegacyHeuristicDisModel:
    IS_LEGACY_FALLBACK = True
    MODEL_KIND = 'heuristic_mdis_fallback'

    def __init__(self, graph: Graph, edge_label: str='has_tag'):
        self.graph = graph
        self.edge_label = edge_label
        self.edge_labels = self._resolve_edge_labels(edge_label)
        self._neighbor_cache: Dict[Any, frozenset] = {}
        self._precompute()

    def _resolve_edge_labels(self, edge_label: Any) -> Tuple[str, ...]:
        if isinstance(edge_label, (list, tuple, set, frozenset)):
            return tuple((str(label) for label in edge_label))
        return (str(edge_label),)

    def _precompute(self):
        sources = set()
        for label in self.edge_labels:
            for edge in self.graph.get_edges_by_label(label):
                sources.add(edge.source)
        for src in sources:
            neighbors = set()
            for label in self.edge_labels:
                neighbors.update((n.node_id for n in self.graph.get_out_neighbor_nodes(src, label)))
            self._neighbor_cache[src] = frozenset(neighbors)

    def _get_neighbors(self, item_id: Any) -> frozenset:
        if item_id not in self._neighbor_cache:
            neighbors = set()
            for label in self.edge_labels:
                neighbors.update((n.node_id for n in self.graph.get_out_neighbor_nodes(item_id, label)))
            self._neighbor_cache[item_id] = frozenset(neighbors)
        return self._neighbor_cache[item_id]

    def compute(self, item1_id: Any, item2_id: Any) -> float:
        n1 = self._get_neighbors(item1_id)
        n2 = self._get_neighbors(item2_id)
        if not n1 and (not n2):
            return 0.0
        intersection = len(n1 & n2)
        union = len(n1 | n2)
        if union == 0:
            return 0.0
        jaccard = intersection / union
        return 1.0 - jaccard

    def compute_batch(self, item1_id: Any, item_ids: Set[Any]) -> Dict[Any, float]:
        n1 = self._get_neighbors(item1_id)
        results = {}
        for item2_id in item_ids:
            n2 = self._get_neighbors(item2_id)
            if not n1 and (not n2):
                results[item2_id] = 0.0
                continue
            union = len(n1 | n2)
            if union == 0:
                results[item2_id] = 0.0
            else:
                results[item2_id] = 1.0 - len(n1 & n2) / union
        return results

    def precompute_all_scores(self) -> None:
        import time
        _t0 = time.time()
        tag_to_items: Dict[Any, Set[Any]] = defaultdict(set)
        for item_id, tags in self._neighbor_cache.items():
            for tag in tags:
                tag_to_items[tag].add(item_id)
        self._cotag_scores: Dict[Any, Dict[Any, float]] = {}
        self._all_tagged_items: FrozenSet[Any] = frozenset(self._neighbor_cache.keys())
        self._threshold_fail_cache: Dict[Tuple[Any, str, float], FrozenSet[Any]] = {}
        _total_pairs = 0
        for v_id, v_tags in self._neighbor_cache.items():
            if not v_tags:
                continue
            cotag_items: Set[Any] = set()
            for tag in v_tags:
                cotag_items.update(tag_to_items[tag])
            cotag_items.discard(v_id)
            if not cotag_items:
                self._cotag_scores[v_id] = {}
                continue
            v_len = len(v_tags)
            scores: Dict[Any, float] = {}
            for w_id in cotag_items:
                w_tags = self._neighbor_cache.get(w_id)
                if not w_tags:
                    continue
                inter = len(v_tags & w_tags)
                union = v_len + len(w_tags) - inter
                if union > 0:
                    scores[w_id] = 1.0 - inter / union
                else:
                    scores[w_id] = 0.0
            self._cotag_scores[v_id] = scores
            _total_pairs += len(scores)
        _dt = time.time() - _t0

    def ensure_threshold_index(self) -> None:
        if hasattr(self, '_cotag_scores'):
            return
        self.precompute_all_scores()

    def get_items_above_threshold(self, v_id: Any, threshold: float, candidate_set: Set[Any], operator: str='>=') -> Set[Any]:
        if not hasattr(self, '_cotag_scores'):
            scores = self.compute_batch(v_id, candidate_set)
            if operator == '>=':
                return {w for w, s in scores.items() if s >= threshold}
            else:
                return {w for w, s in scores.items() if s > threshold}
        v_scores = self._cotag_scores.get(v_id)
        if v_scores is None:
            v_tags = self._neighbor_cache.get(v_id, frozenset())
            if not v_tags:
                if operator == '>=':
                    if threshold <= 0.0:
                        return set(candidate_set)
                    return candidate_set & self._all_tagged_items
                else:
                    if threshold < 0.0:
                        return set(candidate_set)
                    if threshold == 0.0:
                        return candidate_set & self._all_tagged_items
                    return candidate_set & self._all_tagged_items
            return set(candidate_set)
        if operator == '>=' and threshold <= 0.0:
            return set(candidate_set)
        if operator == '>' and threshold < 0.0:
            return set(candidate_set)
        cotag_in_candidates = candidate_set & v_scores.keys()
        if not cotag_in_candidates:
            return set(candidate_set)
        fail_key = (v_id, operator, float(threshold))
        fail_all = self._threshold_fail_cache.get(fail_key)
        if fail_all is None:
            if operator == '>=':
                fail_all = frozenset((w for w, score in v_scores.items() if score < threshold))
            else:
                fail_all = frozenset((w for w, score in v_scores.items() if score <= threshold))
            self._threshold_fail_cache[fail_key] = fail_all
        fail = cotag_in_candidates & fail_all
        if not fail:
            return set(candidate_set)
        return candidate_set - fail

    def get_score(self, v_id: Any, w_id: Any) -> Optional[float]:
        if not hasattr(self, '_cotag_scores'):
            return None
        v_scores = self._cotag_scores.get(v_id)
        if v_scores is None:
            return None
        if w_id in v_scores:
            return v_scores[w_id]
        return 1.0

class DisModel:
    IS_LEGACY_FALLBACK = False
    MODEL_KIND = 'cache_first_mdis'

    def __init__(self, graph: Graph, edge_label: str='has_tag', interaction_edge_label: Optional[str]=None, restart_probability: float=0.2, iterations: int=6, tag_max_df: int=256, row_cache_size: int=2048, ml_cache: Optional[Any]=None, fallback_mode: str='rwr'):
        if not 0.0 <= restart_probability <= 1.0:
            raise ValueError('restart_probability must be in [0, 1]')
        if iterations < 1:
            raise ValueError('iterations must be positive')
        fallback_aliases = {'rwr': 'rwr', 'jac': 'jac', 'jaccard': 'jac', 'tag_jac': 'jac', 'tag_jaccard': 'jac'}
        normalized_fallback = fallback_aliases.get(str(fallback_mode).strip().lower())
        if normalized_fallback is None:
            raise ValueError("fallback_mode must be 'rwr' or 'jac'")
        self.graph = graph
        self.edge_label = edge_label
        self.edge_labels = self._resolve_edge_labels(edge_label)
        self.interaction_edge_label = interaction_edge_label
        self.restart_probability = float(restart_probability)
        self.iterations = int(iterations)
        self.tag_max_df = max(int(tag_max_df), 1)
        self.row_cache_size = max(int(row_cache_size), 1)
        self.ml_cache = ml_cache
        self.fallback_mode = normalized_fallback
        self._category_cache: Dict[Any, FrozenSet[str]] = {}
        self._row_cache: Dict[Any, np.ndarray] = {}
        self.heuristic_fallback = LegacyHeuristicDisModel(graph, edge_label=edge_label) if self.fallback_mode == 'jac' else None
        if self.fallback_mode == 'rwr':
            self._build_transition()

    def _resolve_edge_labels(self, edge_label: Any) -> Tuple[str, ...]:
        if isinstance(edge_label, (list, tuple, set, frozenset)):
            return tuple((str(label) for label in edge_label))
        return (str(edge_label),)

    def _get_categories(self, item_id: Any) -> FrozenSet[str]:
        cached = self._category_cache.get(item_id)
        if cached is not None:
            return cached
        categories: Set[str] = set()
        for edge_label, prefix in (('has_genre', 'genre'), ('has_topic', 'topic')):
            for node in self.graph.get_out_neighbor_nodes(item_id, edge_label):
                categories.add(f'{prefix}:{_normalize_token(node.attributes.get("name", node.node_id))}')
        node = self.graph.get_node(item_id)
        if node is not None:
            for attr_name in ('genre', 'category', 'categories', 'topic'):
                for token in _split_category_tokens(node.attributes.get(attr_name)):
                    categories.add(f'{attr_name}:{token}')
        result = frozenset(categories)
        self._category_cache[item_id] = result
        return result

    def _build_transition(self) -> None:
        feature_to_items: Dict[Any, Set[Any]] = defaultdict(set)
        item_ids: Set[Any] = set()
        item_labels: Set[str] = set()
        for label in self.edge_labels:
            for edge in self.graph.get_edges_by_label(label):
                item_ids.add(edge.source)
                feature_to_items[label, edge.target].add(edge.source)
                node = self.graph.get_node(edge.source)
                if node is not None:
                    item_labels.add(node.label)
        if self.interaction_edge_label:
            for edge in self.graph.get_edges_by_label(self.interaction_edge_label):
                item_ids.add(edge.target)
                node = self.graph.get_node(edge.target)
                if node is not None:
                    item_labels.add(node.label)
        for label in item_labels:
            item_ids.update(self.graph._nodes_by_label.get(label, set()))
        self.item_ids = sorted(item_ids, key=repr)
        self.item_to_index = {item_id: index for index, item_id in enumerate(self.item_ids)}
        rows = []
        cols = []
        values = []
        for items in feature_to_items.values():
            ordered = sorted(items, key=repr)
            if len(ordered) <= 1 or len(ordered) > self.tag_max_df:
                continue
            weight = 1.0 / math.log2(len(ordered) + 1.0)
            for left_pos in range(len(ordered)):
                left = self.item_to_index[ordered[left_pos]]
                for right_pos in range(left_pos + 1, len(ordered)):
                    right = self.item_to_index[ordered[right_pos]]
                    rows.extend((left, right))
                    cols.extend((right, left))
                    values.extend((weight, weight))
        if self.interaction_edge_label:
            histories: Dict[Any, Set[Any]] = defaultdict(set)
            for edge in self.graph.get_edges_by_label(self.interaction_edge_label):
                if edge.target in self.item_to_index:
                    histories[edge.source].add(edge.target)
            for history in histories.values():
                ordered = sorted(history, key=repr)
                if len(ordered) <= 1:
                    continue
                weight = 1.0 / math.sqrt(len(ordered))
                for left_pos in range(len(ordered)):
                    left = self.item_to_index[ordered[left_pos]]
                    for right_pos in range(left_pos + 1, len(ordered)):
                        right = self.item_to_index[ordered[right_pos]]
                        rows.extend((left, right))
                        cols.extend((right, left))
                        values.extend((weight, weight))
        size = len(self.item_ids)
        if size == 0:
            self._transition_t = sparse.csr_matrix((0, 0), dtype=np.float32)
            return
        rows.extend(range(size))
        cols.extend(range(size))
        values.extend([1.0] * size)
        adjacency = sparse.coo_matrix((np.asarray(values, dtype=np.float32), (rows, cols)), shape=(size, size), dtype=np.float32).tocsr()
        degrees = np.asarray(adjacency.sum(axis=1)).reshape(-1)
        inv_degrees = np.divide(1.0, degrees, out=np.zeros_like(degrees), where=degrees > 0)
        transition = sparse.diags(inv_degrees).dot(adjacency).tocsr()
        self._transition_t = transition.transpose().tocsr()

    def _rwr_row(self, item_id: Any) -> Optional[np.ndarray]:
        cached = self._row_cache.get(item_id)
        if cached is not None:
            return cached
        index = self.item_to_index.get(item_id)
        if index is None:
            return None
        restart = np.zeros(len(self.item_ids), dtype=np.float32)
        restart[index] = 1.0
        probability = restart.copy()
        for _ in range(self.iterations):
            probability = self.restart_probability * restart + (1.0 - self.restart_probability) * self._transition_t.dot(probability)
        row_max = float(probability.max()) if probability.size else 0.0
        if row_max > 0.0:
            probability = probability / row_max
        if len(self._row_cache) >= self.row_cache_size:
            self._row_cache.pop(next(iter(self._row_cache)))
        self._row_cache[item_id] = probability
        return probability

    def _get_cached_score(self, item1_id: Any, item2_id: Any) -> Optional[float]:
        if self.ml_cache is None:
            return None
        getter = getattr(self.ml_cache, 'get_dis_score', None)
        if getter is None:
            return None
        score = getter(item1_id, item2_id)
        if score is None:
            return None
        return float(score)

    def _compute_rwr(self, item1_id: Any, item2_id: Any) -> float:
        if item1_id == item2_id:
            return 0.0
        categories1 = self._get_categories(item1_id)
        categories2 = self._get_categories(item2_id)
        if categories1 and categories2 and categories1.isdisjoint(categories2):
            return 1.0
        row1 = self._rwr_row(item1_id)
        row2 = self._rwr_row(item2_id)
        index1 = self.item_to_index.get(item1_id)
        index2 = self.item_to_index.get(item2_id)
        if row1 is None or row2 is None or index1 is None or index2 is None:
            return 1.0
        proximity = max(float(row1[index2]), float(row2[index1]))
        return float(np.clip(1.0 - proximity, 0.0, 1.0))

    def _compute_fallback(self, item1_id: Any, item2_id: Any) -> float:
        if self.fallback_mode == 'jac':
            return self.heuristic_fallback.compute(item1_id, item2_id)
        return self._compute_rwr(item1_id, item2_id)

    def compute(self, item1_id: Any, item2_id: Any) -> float:
        cached = self._get_cached_score(item1_id, item2_id)
        if cached is not None:
            return cached
        return self._compute_fallback(item1_id, item2_id)

    def compute_batch(self, item1_id: Any, item_ids: Set[Any]) -> Dict[Any, float]:
        results: Dict[Any, float] = {}
        missing: Set[Any] = set()
        for item2_id in item_ids:
            cached = self._get_cached_score(item1_id, item2_id)
            if cached is None:
                missing.add(item2_id)
            else:
                results[item2_id] = cached
        if self.fallback_mode == 'jac' and missing:
            results.update(self.heuristic_fallback.compute_batch(item1_id, missing))
        else:
            results.update({item2_id: self._compute_rwr(item1_id, item2_id) for item2_id in missing})
        return results

    def precompute_all_scores(self) -> None:
        if self.fallback_mode == 'jac':
            self.heuristic_fallback.precompute_all_scores()
        else:
            for item_id in self.item_ids:
                self._rwr_row(item_id)

    def ensure_threshold_index(self) -> None:
        self.precompute_all_scores()

    def get_items_above_threshold(self, v_id: Any, threshold: float, candidate_set: Set[Any], operator: str='>=') -> Set[Any]:
        return {item_id for item_id, score in self.compute_batch(v_id, candidate_set).items() if _passes_threshold(score, operator, threshold)}

    def get_score(self, v_id: Any, w_id: Any) -> Optional[float]:
        return self.compute(v_id, w_id)

class AggrModel:

    def __init__(self, graph: Graph, interaction_edge_label: str='plays'):
        self.graph = graph
        self.interaction_edge_label = interaction_edge_label
        self._scores: Dict[Any, float] = {}
        self._precompute()

    def _precompute(self):
        counts: Dict[Any, int] = defaultdict(int)
        for edge in self.graph.get_edges_by_label(self.interaction_edge_label):
            counts[edge.target] += 1
        if not counts:
            return
        max_count = max(counts.values())
        if max_count == 0:
            return
        for item_id, count in counts.items():
            self._scores[item_id] = count / max_count

    def compute(self, item_id: Any) -> float:
        return self._scores.get(item_id, 0.0)

    def compute_batch(self, item_ids: Set[Any]) -> Dict[Any, float]:
        return {item_id: self._scores.get(item_id, 0.0) for item_id in item_ids}

class LegacyHeuristicTopicProximityModel:
    IS_LEGACY_FALLBACK = True
    MODEL_KIND = 'heuristic_topic_fallback'

    def __init__(self, graph: Graph, category_attr: str='genre'):
        self.graph = graph
        self.category_attr = category_attr
        self._category_cache: Dict[Any, frozenset] = {}
        self._precompute()

    def _precompute(self):
        for node_id, node in self.graph._nodes.items():
            cats = set()
            cats.update((f'genre:{_normalize_token(genre_node.attributes.get('name', genre_node.node_id))}' for genre_node, _ in self.graph.get_out_neighbors(node_id, 'has_genre')))
            cats.update((f'topic:{_normalize_token(topic_node.attributes.get('name', topic_node.node_id))}' for topic_node, _ in self.graph.get_out_neighbors(node_id, 'has_topic')))
            if not cats:
                for token in _split_category_tokens(node.attributes.get(self.category_attr)):
                    cats.add(f'genre:{token}')
            if cats:
                self._category_cache[node_id] = frozenset(cats)

    def compute(self, item1_id: Any, item2_id: Any) -> float:
        cats1 = self._category_cache.get(item1_id)
        cats2 = self._category_cache.get(item2_id)
        if cats1 is None or cats2 is None:
            node1 = self.graph.get_node(item1_id)
            node2 = self.graph.get_node(item2_id)
            if node1 is not None and node2 is not None and (node1.label == node2.label):
                return 0.5
            return 0.0
        if not cats1 and (not cats2):
            return 0.0
        intersection = len(cats1 & cats2)
        union = len(cats1 | cats2)
        if union == 0:
            return 0.0
        return intersection / union

    def compute_batch(self, item1_id: Any, item_ids: Set[Any]) -> Dict[Any, float]:
        cats1 = self._category_cache.get(item1_id)
        node1 = None
        if cats1 is None:
            node1 = self.graph.get_node(item1_id)
        results: Dict[Any, float] = {}
        for item2_id in item_ids:
            cats2 = self._category_cache.get(item2_id)
            if cats1 is None or cats2 is None:
                if node1 is None:
                    node1 = self.graph.get_node(item1_id)
                node2 = self.graph.get_node(item2_id)
                if node1 is not None and node2 is not None and (node1.label == node2.label):
                    results[item2_id] = 0.5
                else:
                    results[item2_id] = 0.0
                continue
            if not cats1 and (not cats2):
                results[item2_id] = 0.0
                continue
            union = len(cats1 | cats2)
            if union == 0:
                results[item2_id] = 0.0
            else:
                results[item2_id] = len(cats1 & cats2) / union
        return results

class TopicProximityModel:
    IS_LEGACY_FALLBACK = False
    MODEL_KIND = 'runtime_topic_model'

    def __init__(self, graph: Graph, category_attr: str='genre', ml_cache: Optional[Any]=None):
        self.graph = graph
        self.category_attr = category_attr
        self.ml_cache = ml_cache
        self.heuristic_fallback = LegacyHeuristicTopicProximityModel(graph, category_attr=category_attr)

    def __getattr__(self, name: str) -> Any:
        return getattr(self.heuristic_fallback, name)

    def compute(self, item1_id: Any, item2_id: Any) -> float:
        return self.heuristic_fallback.compute(item1_id, item2_id)

    def compute_batch(self, item1_id: Any, item_ids: Set[Any]) -> Dict[Any, float]:
        return self.heuristic_fallback.compute_batch(item1_id, item_ids)
__all__ = ['DisModel', 'TopicProximityModel', 'AggrModel', 'LegacyHeuristicDisModel', 'LegacyHeuristicTopicProximityModel']
