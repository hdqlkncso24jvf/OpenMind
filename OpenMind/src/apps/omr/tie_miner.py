import os
from threading import Lock
from typing import List, Set, Dict, Tuple, Optional, Any
import random
import logging
import time
import math
from tqdm import tqdm
from datetime import datetime
from dataclasses import dataclass
from collections import defaultdict
import json
from data_structure import DualStarPattern, EdgePredicate, PointWisePredicate, PairWisePredicate, MLPredicate, ComparisonOp, TIERule, RuleMetrics
from tie_matcher import compute_rule_metrics
from models import MLPredicateCache
from attribute_config import load_attribute_config, AttributeConfig
from concurrent.futures import ThreadPoolExecutor, as_completed
import multiprocessing
logger = logging.getLogger(__name__)

def _get_parallel_context() -> multiprocessing.context.BaseContext:
    methods = set(multiprocessing.get_all_start_methods())
    if 'forkserver' in methods:
        return multiprocessing.get_context('forkserver')
    if 'spawn' in methods:
        return multiprocessing.get_context('spawn')
    return multiprocessing.get_context()

@dataclass
class ProfilingStats:
    predicate_building_time: float = 0.0
    gini_computation_time: float = 0.0
    rule_expansion_time: float = 0.0
    rule_metrics_time: float = 0.0
    rule_metrics_calls: int = 0
    cache_hits: int = 0
    cache_misses: int = 0
    patterns_with_support: int = 0
    patterns_without_support: int = 0
    total_rules_generated: int = 0
    gini_cache_hits: int = 0
    gini_cache_misses: int = 0

    def __post_init__(self):
        import threading
        self._lock = threading.Lock()

    def inc(self, field: str, delta=1):
        with self._lock:
            setattr(self, field, getattr(self, field) + delta)

class MetricsCache:

    def __init__(self, max_size: int=10000):
        from collections import OrderedDict
        self.cache = OrderedDict()
        self.max_size = max_size
        self.hits = 0
        self.misses = 0
        self.evictions = 0
        logger.debug(f'MetricsCache initialized (max_size={max_size})')

    def get_key(self, pattern: DualStarPattern, preconditions: List) -> str:
        import hashlib
        pattern_key = f'n{len(pattern.nodes)}_e{len(pattern.edges)}'
        node_labels = '_'.join(sorted([f'{var}:{node.label}' for var, node in pattern.nodes.items()]))
        edge_labels = '_'.join(sorted([f'{e.source_var}-{e.label}{('>' if e.is_forward else '<')}{e.target_var}' for e in pattern.edges]))
        pred_key = '_'.join(sorted([str(p) for p in preconditions]))
        full_key = f'{pattern_key}:{node_labels}:{edge_labels}:{pred_key}'
        if len(full_key) > 200:
            hash_digest = hashlib.md5(full_key.encode()).hexdigest()
            return f'{pattern_key}:{hash_digest}'
        return full_key

    def get(self, pattern: DualStarPattern, preconditions: List):
        key = self.get_key(pattern, preconditions)
        if key in self.cache:
            self.hits += 1
            self.cache.move_to_end(key)
            return self.cache[key]
        self.misses += 1
        return None

    def put(self, pattern: DualStarPattern, preconditions: List, metrics):
        key = self.get_key(pattern, preconditions)
        if key in self.cache:
            self.cache.move_to_end(key)
        elif len(self.cache) >= self.max_size:
            self.cache.popitem(last=False)
            self.evictions += 1
        self.cache[key] = metrics

    def clear(self):
        self.cache.clear()
        self.hits = 0
        self.misses = 0

    def get_stats(self) -> Dict[str, int]:
        total = self.hits + self.misses
        hit_rate = self.hits / total * 100 if total > 0 else 0
        return {'hits': self.hits, 'misses': self.misses, 'total': total, 'hit_rate': hit_rate, 'evictions': self.evictions, 'current_size': len(self.cache), 'max_size': self.max_size}

def _are_contradictory(pred_a, pred_b) -> bool:
    if not isinstance(pred_a, PairWisePredicate) or not isinstance(pred_b, PairWisePredicate):
        return False
    same_pair_forward = pred_a.var1 == pred_b.var1 and pred_a.var2 == pred_b.var2 and (pred_a.attr1 == pred_b.attr1) and (pred_a.attr2 == pred_b.attr2)
    same_pair_reverse = pred_a.var1 == pred_b.var2 and pred_a.var2 == pred_b.var1 and (pred_a.attr1 == pred_b.attr2) and (pred_a.attr2 == pred_b.attr1)
    if not same_pair_forward and (not same_pair_reverse):
        return False
    op_a = pred_a.operator
    if same_pair_reverse:
        op_b = _flip_operator(pred_b.operator)
    else:
        op_b = pred_b.operator
    return _ops_contradictory(op_a, op_b)

def _flip_operator(op) -> 'ComparisonOp':
    flips = {ComparisonOp.LT: ComparisonOp.GT, ComparisonOp.GT: ComparisonOp.LT, ComparisonOp.LE: ComparisonOp.GE, ComparisonOp.GE: ComparisonOp.LE, ComparisonOp.EQ: ComparisonOp.EQ, ComparisonOp.NE: ComparisonOp.NE}
    return flips.get(op, op)

def _ops_contradictory(op_a, op_b) -> bool:
    contradictions = {ComparisonOp.LT: {ComparisonOp.GT, ComparisonOp.GE, ComparisonOp.EQ}, ComparisonOp.GT: {ComparisonOp.LT, ComparisonOp.LE, ComparisonOp.EQ}, ComparisonOp.LE: {ComparisonOp.GT}, ComparisonOp.GE: {ComparisonOp.LT}, ComparisonOp.EQ: {ComparisonOp.NE, ComparisonOp.LT, ComparisonOp.GT}, ComparisonOp.NE: {ComparisonOp.EQ}}
    return op_b in contradictions.get(op_a, set())
_SHARED_GRAPH = None
_SHARED_TRAINING_DATA = None
_SHARED_TRAINING_INDEX = None
_SHARED_ML_CACHE = None
_SHARED_ML_PREDICATES = None
_SHARED_ATTR_CONFIG = None
_SHARED_PATTERNS = None
_SHARED_MINING_CONFIG = None
_WORKER_PRED_GEN = None

def _init_worker(graph, training_data, training_index, ml_cache, ml_predicates, attr_config, patterns, mining_config):
    global _SHARED_GRAPH, _SHARED_TRAINING_DATA, _SHARED_TRAINING_INDEX
    global _SHARED_ML_CACHE, _SHARED_ML_PREDICATES, _SHARED_ATTR_CONFIG
    global _SHARED_PATTERNS, _SHARED_MINING_CONFIG, _WORKER_PRED_GEN
    _SHARED_GRAPH = graph
    _SHARED_TRAINING_DATA = training_data
    _SHARED_TRAINING_INDEX = training_index
    _SHARED_ML_CACHE = ml_cache
    _SHARED_ML_PREDICATES = ml_predicates
    _SHARED_ATTR_CONFIG = attr_config
    _SHARED_PATTERNS = patterns
    _SHARED_MINING_CONFIG = mining_config
    cfg = mining_config
    _WORKER_PRED_GEN = PredicateGenerator(graph=_SHARED_GRAPH, ml_cache=_SHARED_ML_CACHE, gini_top_k=cfg['gini_top_k'], max_candidate_predicates=cfg['max_candidate_predicates'], max_attribute_samples=cfg['max_attribute_samples'], use_incremental_matching=cfg['use_incremental_matching'], enable_prefix_pruning=cfg['enable_prefix_pruning'], attr_config=_SHARED_ATTR_CONFIG)
    _WORKER_PRED_GEN.training_index = _SHARED_TRAINING_INDEX
    _WORKER_PRED_GEN.conclusion_edge_label = cfg.get('conclusion_edge_label')

def _worker_mine_pattern(pattern_idx: int):
    try:
        pattern = _SHARED_PATTERNS[pattern_idx]
        cfg = _SHARED_MINING_CONFIG
        rules = _WORKER_PRED_GEN.generate_predicates(pattern=pattern, training_data=_SHARED_TRAINING_DATA, ml_predicates=_SHARED_ML_PREDICATES, min_support=cfg['min_support'], min_confidence=cfg['min_confidence'], max_predicates=cfg['max_predicates'], use_parallel=False, n_workers=1, shared_executor=None)
        return rules
    except Exception as e:
        import traceback
        logger.error(f'Worker error on pattern {pattern_idx}: {e}')
        logger.debug(traceback.format_exc())
        return []

class _UnionFind:
    __slots__ = ('parent', 'rank')

    def __init__(self, elements):
        self.parent = {e: e for e in elements}
        self.rank = {e: 0 for e in elements}

    def find(self, x):
        while self.parent[x] != x:
            self.parent[x] = self.parent[self.parent[x]]
            x = self.parent[x]
        return x

    def union(self, a, b):
        ra, rb = (self.find(a), self.find(b))
        if ra == rb:
            return
        if self.rank[ra] < self.rank[rb]:
            ra, rb = (rb, ra)
        self.parent[rb] = ra
        if self.rank[ra] == self.rank[rb]:
            self.rank[ra] += 1

class PredicateGenerator:

    def __init__(self, graph, ml_cache: MLPredicateCache, gini_top_k: int=20, max_candidate_predicates: int=5, max_attribute_samples: int=3, use_gini: bool=True, use_incremental_matching: bool=True, enable_prefix_pruning: bool=True, attr_config: 'AttributeConfig'=None):
        self.graph = graph
        self.ml_cache = ml_cache
        self.attr_config = attr_config
        self.gini_top_k = gini_top_k
        self.max_candidate_predicates = max_candidate_predicates
        self.max_attribute_samples = max_attribute_samples
        self.use_gini = use_gini
        self.metrics_cache = MetricsCache()
        self.stats = ProfilingStats()
        from collections import OrderedDict
        self.verified_rules_cache: OrderedDict = OrderedDict()
        self.max_verified_cache_size = 5000
        self.verified_rules_lock = Lock()
        self.predicate_gini_cache: OrderedDict = OrderedDict()
        self.max_gini_cache_size = 10000
        self.gini_cache_lock = Lock()
        self.use_incremental_matching = use_incremental_matching
        self.enable_prefix_pruning = enable_prefix_pruning
        self.pruning_lock = Lock()
        self.pruning_stats = {'prefixes_pruned': 0, 'rules_skipped': 0}
        self.low_quality_patterns: Set[str] = set()

    @staticmethod
    def _min_positive_required(min_support: int, min_confidence: float) -> int:
        return int(math.ceil(min_support * min_confidence - 1e-12))

    def _is_confidence_branch_impossible(self, support: int, positive_count: int, min_support: int, min_confidence: float) -> bool:
        if support < min_support:
            return True
        return positive_count < self._min_positive_required(min_support, min_confidence)

    def generate_predicates(self, pattern: DualStarPattern, training_data: Dict[Tuple[Any, Any], bool], ml_predicates: List[MLPredicate], min_support: int, min_confidence: float, max_predicates: int=5, use_parallel: bool=True, n_workers: int=4, shared_executor: 'ThreadPoolExecutor'=None) -> List[TIERule]:
        logger.debug(f'  Generating predicates for pattern with {len(pattern.nodes)} nodes')
        if not hasattr(self, 'training_index') or self.training_index is None:
            from collections import defaultdict as _defaultdict
            self.training_index = _defaultdict(set)
            for u, v in training_data.keys():
                self.training_index[u].add(v)
        t0 = time.time()
        delta = self._build_predicate_set(pattern)
        self.stats.inc('predicate_building_time', time.time() - t0)
        if len(delta) == 0:
            logger.debug('    No candidate predicates, skipping')
            return []
        seed_groups = self._select_seed_groups(pattern, delta, max_groups=3)
        if not seed_groups:
            logger.debug('    无可用种子组，跳过此模式')
            return []
        if ml_predicates:
            filtered_training_data = {}
            for (u, v), label in training_data.items():
                if all((ml.evaluate(self.ml_cache.get_prediction(u, v)) for ml in ml_predicates)):
                    filtered_training_data[u, v] = label
            training_data = filtered_training_data
        valid_rules = []
        seen_rule_sigs = set()
        edge_label = getattr(self, 'conclusion_edge_label', None) or self._extract_edge_label_from_pattern(pattern)
        for group_idx, seed_predicates in enumerate(seed_groups):
            initial_preconditions = list(ml_predicates) + seed_predicates
            t0 = time.time()
            initial_matches = self._compute_all_compact_matches(pattern, initial_preconditions, training_data)
            initial_pw_preds = [p for p in initial_preconditions if isinstance(p, PairWisePredicate)]
            _initial_rule = TIERule(pattern=pattern, preconditions=initial_preconditions, conclusion=EdgePredicate(pattern.user_center, pattern.item_center, edge_label))
            initial_metrics = self._compute_metrics_from_compact_matches(initial_matches, training_data, variable_predicates=initial_pw_preds if initial_pw_preds else None, rule=_initial_rule)
            self.stats.inc('rule_metrics_time', time.time() - t0)
            self.stats.inc('rule_metrics_calls')
            if initial_metrics.support == 0:
                logger.warning('    ✗ 支持度为0')
                if group_idx == 0:
                    pattern_sig = self._get_pattern_signature(pattern)
                    self.low_quality_patterns.add(pattern_sig)
                    self.stats.inc('patterns_without_support')
                continue
            if initial_metrics.support < min_support * 0.1:
                logger.warning(f'    ✗ 支持度过低 ({initial_metrics.support} < {min_support * 0.1:.0f})')
                if group_idx == 0:
                    self.stats.inc('patterns_without_support')
                continue
            if initial_metrics.support < min_support:
                logger.warning(f'    ✗ 支持度不足 ({initial_metrics.support} < {min_support})')
                if group_idx == 0:
                    self.stats.inc('patterns_without_support')
                continue
            if group_idx == 0:
                self.stats.inc('patterns_with_support')
            skip_expansion = False
            if initial_metrics.support >= min_support and initial_metrics.confidence >= min_confidence:
                if self._is_trivial_rule(pattern, initial_preconditions):
                    logger.warning('    ⚠️ 初始规则为平凡规则，跳过此组')
                    continue
                rule_sig = '|'.join(sorted((str(p) for p in initial_preconditions)))
                if rule_sig in seen_rule_sigs:
                    logger.debug('    ⚠️ 规则已在其他种子组中发现，跳过')
                    continue
                seen_rule_sigs.add(rule_sig)
                rule = TIERule(pattern=pattern, preconditions=initial_preconditions, conclusion=EdgePredicate(pattern.user_center, pattern.item_center, edge_label))
                valid_rules.append((rule, initial_metrics))
                self.stats.inc('total_rules_generated')
                skip_expansion = True
            if skip_expansion:
                continue
            if initial_metrics.support >= min_support and initial_metrics.confidence < min_confidence:
                initial_positive = len(initial_metrics.positive_pivots)
                if self._is_confidence_branch_impossible(initial_metrics.support, initial_positive, min_support, min_confidence):
                    continue
            seed_strs = {str(p) for p in seed_predicates}
            remaining_delta = [p for p in delta if str(p) not in seed_strs]
            remaining_budget = max_predicates - len(seed_predicates)
            if remaining_budget <= 0:
                continue
            pruned_prefixes: Set[frozenset] = set()
            X_current = {tuple(sorted((str(p) for p in initial_preconditions))): (initial_preconditions, initial_matches)}
            for level in range(remaining_budget):
                if not X_current:
                    break
                if use_parallel and len(X_current) >= 2:
                    X_next = self._expand_level_parallel(pattern, X_current, remaining_delta, training_data, min_support, min_confidence, n_workers, level, pruned_prefixes=pruned_prefixes, executor=shared_executor)
                else:
                    X_next = self._expand_level_sequential(pattern, X_current, remaining_delta, training_data, min_support, min_confidence, level, pruned_prefixes=pruned_prefixes)
                for new_X_tuple, (new_X, child_matches, is_valid, exp_metrics) in X_next.items():
                    if is_valid:
                        rule_sig = '|'.join(sorted((str(p) for p in new_X)))
                        if rule_sig in seen_rule_sigs:
                            continue
                        new_pred_set = frozenset((str(p) for p in new_X))
                        is_subsumed = False
                        for existing_rule, _ in valid_rules:
                            existing_set = frozenset((str(p) for p in existing_rule.preconditions))
                            if existing_set.issubset(new_pred_set):
                                is_subsumed = True
                                break
                        if not is_subsumed:
                            seen_rule_sigs.add(rule_sig)
                            rule = TIERule(pattern=pattern, preconditions=new_X, conclusion=EdgePredicate(pattern.user_center, pattern.item_center, edge_label))
                            valid_rules.append((rule, exp_metrics))
                            self.stats.inc('total_rules_generated')
                X_current = {k: (v[0], v[1]) for k, v in X_next.items() if not v[2]}
                if valid_rules:
                    logger.debug(f'    Level {level + 1}: {len(valid_rules)} valid rules')
        logger.debug(f'    Generated {len(valid_rules)} valid rules (across {len(seed_groups)} seed groups)')
        return valid_rules

    def _expand_level_sequential(self, pattern: DualStarPattern, X_current: Dict, delta: List, training_data: Dict, min_support: int, min_confidence: float, level: int, pruned_prefixes: Optional[Set[frozenset]]=None) -> Dict:
        X_next = {}
        for X_tuple, (X_list, parent_matches) in tqdm(list(X_current.items()), desc=f'    Level {level + 1}', leave=False, disable=len(X_current) < 3):
            t0 = time.time()
            expanded = self._expand_precondition(pattern, X_list, delta, training_data, min_support, min_confidence, parent_matches=parent_matches, pruned_prefixes=pruned_prefixes)
            self.stats.inc('rule_expansion_time', time.time() - t0)
            for new_X, child_matches, is_valid, metrics in expanded:
                new_X_tuple = tuple(sorted([str(p) for p in new_X]))
                if new_X_tuple not in X_next:
                    X_next[new_X_tuple] = (new_X, child_matches, is_valid, metrics)
        return X_next

    def _expand_level_parallel(self, pattern: DualStarPattern, X_current: Dict, delta: List, training_data: Dict, min_support: int, min_confidence: float, n_workers: int, level: int, pruned_prefixes: Optional[Set[frozenset]]=None, executor: 'ThreadPoolExecutor'=None) -> Dict:
        X_next = {}

        def expand_single(item):
            X_tuple, (X_list, parent_matches) = item
            t0 = time.time()
            result = self._expand_precondition(pattern, X_list, delta, training_data, min_support, min_confidence, parent_matches=parent_matches, pruned_prefixes=pruned_prefixes)
            return (result, time.time() - t0)
        total_expansion_time = 0
        _own_executor = executor is None
        if _own_executor:
            executor = ThreadPoolExecutor(max_workers=n_workers)
        try:
            futures = {executor.submit(expand_single, item): item for item in X_current.items()}
            for future in tqdm(as_completed(futures), total=len(futures), desc=f'    Level {level + 1} (parallel)', leave=False):
                try:
                    expanded, elapsed = future.result(timeout=300)
                    total_expansion_time += elapsed
                    for new_X, child_matches, is_valid, metrics in expanded:
                        new_X_tuple = tuple(sorted([str(p) for p in new_X]))
                        if new_X_tuple not in X_next:
                            X_next[new_X_tuple] = (new_X, child_matches, is_valid, metrics)
                except TimeoutError:
                    logger.error('Task timed out after 300s - skipping')
                    future.cancel()
                except Exception as e:
                    logger.error(f'Error in parallel expansion: {e}')
                    import traceback
                    logger.error(traceback.format_exc())
        finally:
            if _own_executor:
                executor.shutdown(wait=True)
        self.stats.inc('rule_expansion_time', total_expansion_time)
        return X_next

    def _extract_edge_label_from_pattern(self, pattern: DualStarPattern) -> Optional[str]:
        user_center = pattern.user_center
        item_center = pattern.item_center
        for edge in pattern.edges:
            if edge.source_var == user_center and edge.target_var == item_center:
                return edge.label
            if edge.source_var == item_center and edge.target_var == user_center:
                return edge.label
        return None

    def _get_rule_signature(self, pattern: DualStarPattern, preconditions: List) -> str:
        import hashlib
        pattern_key = f'n{len(pattern.nodes)}_e{len(pattern.edges)}'
        node_labels = '_'.join(sorted([f'{var}:{node.label}' for var, node in pattern.nodes.items()]))
        edge_labels = '_'.join(sorted([f'{e.source_var}-{e.label}{('>' if e.is_forward else '<')}{e.target_var}' for e in pattern.edges]))
        pred_strs = sorted([str(p) for p in preconditions])
        pred_sig = '|'.join(pred_strs)
        full_key = f'{pattern_key}:{node_labels}:{edge_labels}::{pred_sig}'
        if len(full_key) > 200:
            hash_digest = hashlib.md5(full_key.encode()).hexdigest()
            return f'{pattern_key}:{hash_digest}'
        return full_key

    def _build_predicate_set(self, pattern: DualStarPattern) -> List:
        delta = []
        point_wise_count = 0
        pair_wise_count = 0
        pivot_vars = {pattern.user_center, pattern.item_center}
        leaf_vars = set()
        for path in pattern.user_paths + pattern.item_paths:
            if len(path.path_sequence) >= 2:
                last_var = path.path_sequence[-1]
                if last_var not in pivot_vars:
                    leaf_vars.add(last_var)

        def _get_attr_type(attr: str, node_label: str) -> str:
            if self.attr_config is not None:
                return self.attr_config.get_type(attr, node_label)
            a_lower = attr.lower()
            if a_lower == 'id' or a_lower.endswith('_id') or a_lower.startswith('id_'):
                return 'id'
            return 'category'

        def _get_cardinality(attr: str, node_label: str) -> int:
            _by_attr = getattr(self.graph, '_attr_to_values', None)
            if _by_attr is not None and attr in _by_attr:
                return len(_by_attr[attr])
            label_nodes = self.graph._nodes_by_label.get(node_label, set())
            if not label_nodes:
                return 999
            unique_values = set()
            for i, nid in enumerate(label_nodes):
                if i >= 500:
                    break
                node = self.graph._nodes.get(nid)
                if node and isinstance(node.attributes, dict) and (attr in node.attributes):
                    unique_values.add(node.attributes[attr])
            return len(unique_values)

        def _get_ops(attr: str, node_label: str) -> dict:
            if self.attr_config is not None:
                cardinality = _get_cardinality(attr, node_label)
                return self.attr_config.get_allowed_operations(attr, node_label, cardinality)
            attr_type = _get_attr_type(attr, node_label)
            if attr_type == 'id':
                return {'pointwise_eq': False, 'pointwise_lt_gt': False, 'pairwise_eq': True, 'pairwise_lt_gt': False}
            return {'pointwise_eq': True, 'pointwise_lt_gt': True, 'pairwise_eq': True, 'pairwise_lt_gt': True}

        def _node_attrs(node) -> set:
            if isinstance(node.attributes, dict):
                return set(node.attributes.keys())
            elif isinstance(node.attributes, (set, frozenset)):
                return set(node.attributes)
            return set()
        for var, node in pattern.nodes.items():
            for attr in _node_attrs(node):
                attr_type = _get_attr_type(attr, node.label)
                if attr_type == 'id':
                    continue
                ops = _get_ops(attr, node.label)
                if ops['pointwise_eq']:
                    sample_values = self._get_frequent_attribute_values(node.label, attr, max_samples=self.max_attribute_samples, min_frequency=0.01)
                    for value in sample_values:
                        delta.append(PointWisePredicate(var, attr, ComparisonOp.EQ, value))
                        point_wise_count += 1
                if ops['pointwise_lt_gt']:
                    sample_values = self._get_frequent_attribute_values(node.label, attr, max_samples=self.max_attribute_samples, min_frequency=0.01)
                    for value in sample_values:
                        if isinstance(value, (int, float)):
                            delta.append(PointWisePredicate(var, attr, ComparisonOp.LT, value))
                            delta.append(PointWisePredicate(var, attr, ComparisonOp.GT, value))
                            point_wise_count += 2
        var_star = {}
        var_star[pattern.user_center] = 'user'
        var_star[pattern.item_center] = 'item'
        for path in pattern.user_paths:
            for v in path.path_sequence:
                var_star.setdefault(v, 'user')
        for path in pattern.item_paths:
            for v in path.path_sequence:
                var_star.setdefault(v, 'item')
        pair_vars = sorted(pivot_vars | leaf_vars)
        for i, var_a in enumerate(pair_vars):
            if var_a not in pattern.nodes:
                continue
            node_a = pattern.nodes[var_a]
            attrs_a = _node_attrs(node_a)
            for var_b in pair_vars[i + 1:]:
                if var_b not in pattern.nodes:
                    continue
                if var_star.get(var_a) == var_star.get(var_b):
                    continue
                if var_a in pivot_vars and var_b in pivot_vars:
                    continue
                node_b = pattern.nodes[var_b]
                attrs_b = _node_attrs(node_b)
                shared_attrs = attrs_a & attrs_b
                for attr in sorted(shared_attrs):
                    attr_type = _get_attr_type(attr, node_a.label)
                    if attr_type == 'id':
                        continue
                    ops = _get_ops(attr, node_a.label)
                    if ops['pairwise_eq']:
                        delta.append(PairWisePredicate(var_a, attr, ComparisonOp.EQ, var_b, attr))
                        pair_wise_count += 1
                    if ops['pairwise_lt_gt']:
                        delta.append(PairWisePredicate(var_a, attr, ComparisonOp.LT, var_b, attr))
                        delta.append(PairWisePredicate(var_a, attr, ComparisonOp.GT, var_b, attr))
                        pair_wise_count += 2
        label_to_vars = defaultdict(list)
        for var, node in pattern.nodes.items():
            label_to_vars[node.label].append(var)
        for label, vars_in_label in label_to_vars.items():
            if len(vars_in_label) < 2:
                continue
            for i in range(len(vars_in_label)):
                for j in range(i + 1, len(vars_in_label)):
                    var_a = vars_in_label[i]
                    var_b = vars_in_label[j]
                    if var_star.get(var_a) == var_star.get(var_b):
                        continue
                    node_a = pattern.nodes[var_a]
                    node_b = pattern.nodes[var_b]
                    shared_id_attrs = _node_attrs(node_a) & _node_attrs(node_b)
                    for attr in sorted(shared_id_attrs):
                        attr_type = _get_attr_type(attr, label)
                        if attr_type != 'id':
                            continue
                        delta.append(PairWisePredicate(var_a, attr, ComparisonOp.EQ, var_b, attr))
                        pair_wise_count += 1
        covered_by_delta = set()
        for pred in delta:
            if isinstance(pred, PointWisePredicate):
                covered_by_delta.add(pred.variable)
            elif isinstance(pred, PairWisePredicate):
                covered_by_delta.add(pred.var1)
                covered_by_delta.add(pred.var2)
        uncovered_leaves = leaf_vars - covered_by_delta
        if uncovered_leaves:
            logger.warning(f'    ⚠️ {len(uncovered_leaves)} 叶子无谓词覆盖: {sorted(uncovered_leaves)}')
            for leaf_var in uncovered_leaves:
                if leaf_var not in pattern.nodes:
                    continue
                leaf_node = pattern.nodes[leaf_var]
                for attr in _node_attrs(leaf_node):
                    attr_type = _get_attr_type(attr, leaf_node.label)
                    if attr_type == 'id':
                        continue
                    sample_values = self._get_frequent_attribute_values(leaf_node.label, attr, max_samples=1, min_frequency=0.0)
                    if sample_values:
                        delta.append(PointWisePredicate(leaf_var, attr, ComparisonOp.EQ, sample_values[0]))
                        point_wise_count += 1
                        break
        return delta

    def _select_seed_groups(self, pattern: DualStarPattern, delta: List, max_groups: int=3) -> List[List]:
        pivot_vars = {pattern.user_center, pattern.item_center}
        leaf_vars = set()
        for path in pattern.user_paths + pattern.item_paths:
            if len(path.path_sequence) >= 2:
                last_var = path.path_sequence[-1]
                if last_var not in pivot_vars:
                    leaf_vars.add(last_var)
        if not leaf_vars:
            logger.debug('    [Seed] 无叶子节点，跳过 seed 分配')
            return []
        all_pairwise = []
        var_to_pointwise = defaultdict(list)
        for pred in delta:
            if isinstance(pred, PairWisePredicate):
                all_pairwise.append(pred)
            elif isinstance(pred, PointWisePredicate):
                var_to_pointwise[pred.variable].append(pred)
        importance = {}
        if hasattr(self, '_importance_cache'):
            importance = self._importance_cache
        else:
            attr_type = getattr(self.graph, 'attr_type', {})
            for attr, atype in attr_type.items():
                if atype == 'id':
                    importance[attr] = 0.0
                elif atype == 'categorical':
                    importance[attr] = 0.7
                elif atype == 'numeric_discrete':
                    importance[attr] = 0.5
                elif atype == 'numeric_continuous':
                    importance[attr] = 0.8
                else:
                    importance[attr] = 0.3
            self._importance_cache = importance

        def pairwise_score(pred):
            attr_imp = importance.get(pred.attr1, 0.5)
            return 3.0 + attr_imp

        def pointwise_score(pred):
            attr_imp = importance.get(pred.attribute, 0.5)
            if pred.operator == ComparisonOp.EQ:
                return 2.0 + attr_imp
            return 1.0 + attr_imp
        pw_sorted = sorted(all_pairwise, key=pairwise_score, reverse=True)
        pointwise_base = []
        pointwise_strs = set()
        covered_leaves = set()
        for leaf_var in sorted(leaf_vars):
            pw_candidates = var_to_pointwise.get(leaf_var, [])
            if not pw_candidates:
                logger.warning(f'    [Seed] ⚠️ 叶子 {leaf_var} 无 PointWise 谓词（路径将无法剪枝）')
                continue
            best = sorted(pw_candidates, key=pointwise_score, reverse=True)[0]
            pointwise_base.append(best)
            pointwise_strs.add(str(best))
            covered_leaves.add(leaf_var)
        uncovered = leaf_vars - covered_leaves
        if uncovered:
            logger.warning(f'    [Seed] 未覆盖叶子(无PointWise): {sorted(uncovered)}')
        user_star_vars = set()
        item_star_vars = set()
        for path in pattern.user_paths:
            user_star_vars.update(path.path_sequence)
        for path in pattern.item_paths:
            item_star_vars.update(path.path_sequence)
        shared_cross_star_vars = (user_star_vars & item_star_vars) - pivot_vars
        has_structural_cross_star_join = bool(shared_cross_star_vars)
        if not pw_sorted:
            if has_structural_cross_star_join:
                pass
            else:
                logger.warning('    [Seed] ⚠️ 无 PairWise 谓词可用，且不存在结构性跨星连接')
        groups = []
        seen_sigs = set()

        def _try_add_group(pw_combo):
            seeds = list(pw_combo) + pointwise_base
            sig = frozenset((str(s) for s in seeds))
            if sig in seen_sigs:
                return False
            if self._is_trivial_rule(pattern, seeds):
                pw_strs = ', '.join((str(p) for p in pw_combo))
                logger.debug(f'    [Seed] 跳过平凡组合: [{pw_strs}]')
                return False
            groups.append(seeds)
            seen_sigs.add(sig)
            return True
        top_k = min(len(pw_sorted), 6)
        for pw in pw_sorted[:top_k]:
            if len(groups) >= max_groups:
                break
            _try_add_group([pw])
        if len(groups) < max_groups:
            pair_k = min(len(pw_sorted), 5)
            for i in range(pair_k):
                if len(groups) >= max_groups:
                    break
                for j in range(i + 1, pair_k):
                    if len(groups) >= max_groups:
                        break
                    pw1, pw2 = (pw_sorted[i], pw_sorted[j])
                    if _are_contradictory(pw1, pw2):
                        continue
                    _try_add_group([pw1, pw2])
        if len(groups) < max_groups:
            triple_k = min(len(pw_sorted), 4)
            for i in range(triple_k):
                if len(groups) >= max_groups:
                    break
                for j in range(i + 1, triple_k):
                    if len(groups) >= max_groups:
                        break
                    for k in range(j + 1, triple_k):
                        if len(groups) >= max_groups:
                            break
                        pw1, pw2, pw3 = (pw_sorted[i], pw_sorted[j], pw_sorted[k])
                        if _are_contradictory(pw1, pw2) or _are_contradictory(pw1, pw3) or _are_contradictory(pw2, pw3):
                            continue
                        _try_add_group([pw1, pw2, pw3])
        if not groups:
            fallback_seed = list(pointwise_base)
            if not fallback_seed:
                all_pointwise = [p for p in delta if isinstance(p, PointWisePredicate)]
                if all_pointwise:
                    best_pw = sorted(all_pointwise, key=pointwise_score, reverse=True)[0]
                    fallback_seed = [best_pw]
            if fallback_seed and (has_structural_cross_star_join or not pw_sorted):
                sig = frozenset((str(s) for s in fallback_seed))
                if sig not in seen_sigs and (not self._is_trivial_rule(pattern, fallback_seed)):
                    groups.append(fallback_seed)
                    seen_sigs.add(sig)
        if not groups:
            logger.warning('    [Seed] ⚠️ 所有 PairWise 组合均为平凡或矛盾，无可用种子')
            return []
        for g_idx, g in enumerate(groups):
            pw_count = sum((1 for p in g if isinstance(p, PairWisePredicate)))
            ptw_count = len(g) - pw_count
            pw_descs = [str(p) for p in g if isinstance(p, PairWisePredicate)]
        return groups

    def _is_id_attribute(self, attr: str, node_label: str) -> bool:
        if self.attr_config is not None:
            return self.attr_config.get_type(attr, node_label) == 'id'
        a_lower = attr.lower()
        return a_lower == 'id' or a_lower.endswith('_id') or a_lower.startswith('id_')

    def _is_trivial_rule(self, pattern: DualStarPattern, preconditions: List) -> bool:
        user_center = pattern.user_center
        item_center = pattern.item_center
        id_eq_pairs = []
        for pred in preconditions:
            if not isinstance(pred, PairWisePredicate):
                continue
            if pred.operator != ComparisonOp.EQ:
                continue
            node1 = pattern.nodes.get(pred.var1)
            node2 = pattern.nodes.get(pred.var2)
            if node1 is None or node2 is None:
                continue
            if self._is_id_attribute(pred.attr1, node1.label) and self._is_id_attribute(pred.attr2, node2.label):
                id_eq_pairs.append((pred.var1, pred.var2))
        if not id_eq_pairs:
            return False
        all_vars = set(pattern.nodes.keys())
        uf = _UnionFind(all_vars)
        for v1, v2 in id_eq_pairs:
            uf.union(v1, v2)
        x_direct_neighbors = set()
        for path in pattern.user_paths:
            seq = path.path_sequence
            if len(seq) >= 2 and seq[0] == user_center:
                x_direct_neighbors.add(seq[1])
        y_direct_neighbors = set()
        for path in pattern.item_paths:
            seq = path.path_sequence
            if len(seq) >= 2 and seq[0] == item_center:
                y_direct_neighbors.add(seq[1])
        cond1 = any((uf.find(u) == uf.find(item_center) for u in x_direct_neighbors))
        if cond1:
            return True
        cond2 = any((uf.find(v) == uf.find(user_center) for v in y_direct_neighbors))
        return cond2

    def _get_frequent_attribute_values(self, label: str, attr: str, max_samples: int, min_frequency: float=0.0) -> List[Any]:
        label_nodes = self.graph._nodes_by_label.get(label, set())
        total_nodes = len(label_nodes)
        if total_nodes == 0:
            return []
        attr_type = getattr(self.graph, 'attr_type', {}).get(attr, 'categorical')
        if attr_type == 'numeric_continuous':
            all_values = []
            for nid in label_nodes:
                node = self.graph._nodes.get(nid)
                if node and attr in node.attributes:
                    all_values.append(node.attributes[attr])
            if all_values and self._is_continuous_numerical_attribute(all_values):
                discretized = self._discretize_continuous_attribute(all_values, num_levels=5)
                logger.debug(f'      {label}.{attr}: 连续数值属性，离散化为 {len(discretized)} 个level')
                return discretized
        attr_value_index = getattr(self.graph, 'attr_value_index', None)
        if attr_value_index is not None:
            _by_attr = getattr(self.graph, '_attr_to_values', None)
            if _by_attr is None:
                _by_attr = {}
                for (a, v), node_set in attr_value_index.items():
                    _by_attr.setdefault(a, {})[v] = node_set
                self.graph._attr_to_values = _by_attr
            value_counts = {}
            attr_entries = _by_attr.get(attr, {})
            for v, node_set in attr_entries.items():
                count = len(node_set & label_nodes)
                if count > 0:
                    value_counts[v] = count
        else:
            value_counts = {}
            for nid in label_nodes:
                node = self.graph._nodes.get(nid)
                if node and attr in node.attributes:
                    value = node.attributes[attr]
                    value_counts[value] = value_counts.get(value, 0) + 1
        frequent_values = [(value, count) for value, count in value_counts.items() if count / total_nodes >= min_frequency]
        frequent_values.sort(key=lambda x: x[1], reverse=True)
        return [value for value, _ in frequent_values[:max_samples]]

    def _is_continuous_numerical_attribute(self, values: List[Any]) -> bool:
        if not values:
            return False
        numeric_count = 0
        for val in values:
            if isinstance(val, (int, float)):
                numeric_count += 1
        if numeric_count < len(values) * 0.8:
            return False
        unique_values = set(values)
        if len(unique_values) <= 10:
            return False
        return True

    def _discretize_continuous_attribute(self, values: List[Any], num_levels: int=5) -> List[Any]:
        numeric_values = [v for v in values if isinstance(v, (int, float))]
        if not numeric_values:
            return []
        sorted_values = sorted(numeric_values)
        n = len(sorted_values)
        representative_values = []
        for i in range(num_levels):
            start_idx = int(i * n / num_levels)
            end_idx = int((i + 1) * n / num_levels)
            if start_idx < end_idx:
                mid_idx = (start_idx + end_idx) // 2
                representative_values.append(sorted_values[mid_idx])
        unique_representatives = []
        seen = set()
        for val in representative_values:
            if val not in seen:
                unique_representatives.append(val)
                seen.add(val)
        logger.debug(f'        离散化: {len(numeric_values)}个值 -> {len(unique_representatives)}个level: {unique_representatives}')
        return unique_representatives

    def _compute_gini_batch_optimized(self, pattern: DualStarPattern, current_preconditions: List, candidate_preds: List, training_data: Dict[Tuple[Any, Any], bool], parent_matches: Dict[Tuple[Any, Any], Dict[str, Set[Any]]]=None) -> List[Tuple[Any, float]]:
        if parent_matches is not None:
            compact_matches = parent_matches
        else:
            logger.warning('      [批量Gini] parent_matches is None — returning neutral Gini')
            return [(pred, 1.0) for pred in candidate_preds]
        if not compact_matches:
            return [(pred, 1.0) for pred in candidate_preds]
        gini_scores = []
        eval_start = time.time()
        for pred in candidate_preds:
            D1_labels = []
            D2_labels = []
            for (u, v), cm in compact_matches.items():
                label = training_data.get((u, v))
                if label is None:
                    continue
                satisfies = self._evaluate_predicate_on_compact_match(pred, cm, u, v)
                if satisfies:
                    D1_labels.append(label)
                else:
                    D2_labels.append(label)
            gini_value = self._compute_gini_from_splits(D1_labels, D2_labels)
            gini_scores.append((pred, gini_value))
        eval_time = time.time() - eval_start
        return gini_scores

    def _evaluate_predicate_on_compact_match(self, predicate, compact_match: Dict[str, Set[Any]], user_id: Any, item_id: Any) -> bool:
        from data_structure import ComparisonOp
        _nodes = self.graph._nodes
        if isinstance(predicate, PointWisePredicate):
            var = predicate.variable
            if var not in compact_match:
                return False
            attr = predicate.attribute
            op = predicate.operator
            const = predicate.constant
            for node_id in compact_match[var]:
                node = _nodes.get(node_id)
                if node is None:
                    continue
                val = node.attributes.get(attr)
                if val is None:
                    continue
                if op == ComparisonOp.EQ:
                    if val == const:
                        return True
                elif op == ComparisonOp.NE:
                    if val != const:
                        return True
                elif op == ComparisonOp.LT:
                    if val < const:
                        return True
                elif op == ComparisonOp.LE:
                    if val <= const:
                        return True
                elif op == ComparisonOp.GT:
                    if val > const:
                        return True
                elif op == ComparisonOp.GE:
                    if val >= const:
                        return True
            return False
        elif isinstance(predicate, PairWisePredicate):
            var1 = predicate.var1
            var2 = predicate.var2
            if var1 not in compact_match or var2 not in compact_match:
                return False
            attr1, attr2, op = (predicate.attr1, predicate.attr2, predicate.operator)
            vals1 = set()
            for nid in compact_match[var1]:
                node = _nodes.get(nid)
                if node is not None:
                    v = node.attributes.get(attr1)
                    if v is not None:
                        vals1.add(v)
            if not vals1:
                return False
            vals2 = set()
            for nid in compact_match[var2]:
                node = _nodes.get(nid)
                if node is not None:
                    v = node.attributes.get(attr2)
                    if v is not None:
                        vals2.add(v)
            if not vals2:
                return False
            if op == ComparisonOp.EQ:
                return bool(vals1 & vals2)
            elif op == ComparisonOp.NE:
                if len(vals1) > 1 or len(vals2) > 1:
                    return True
                return vals1 != vals2
            else:
                try:
                    min1, max1 = (min(vals1), max(vals1))
                    min2, max2 = (min(vals2), max(vals2))
                except TypeError:
                    return False
                if op == ComparisonOp.LT:
                    return min1 < max2
                elif op == ComparisonOp.LE:
                    return min1 <= max2
                elif op == ComparisonOp.GT:
                    return max1 > min2
                elif op == ComparisonOp.GE:
                    return max1 >= min2
                return False
        elif isinstance(predicate, MLPredicate):
            score = self.ml_cache.get_prediction(user_id, item_id)
            return predicate.evaluate(score)
        return True

    def _compute_gini_from_splits(self, D1_labels: List[bool], D2_labels: List[bool]) -> float:
        if not D1_labels and (not D2_labels):
            return 1.0
        total = len(D1_labels) + len(D2_labels)
        if D1_labels:
            pos_count_D1 = sum(D1_labels)
            neg_count_D1 = len(D1_labels) - pos_count_D1
            f1_D1 = pos_count_D1 / len(D1_labels)
            f2_D1 = neg_count_D1 / len(D1_labels)
            gini_D1 = 1 - f1_D1 ** 2 - f2_D1 ** 2
        else:
            gini_D1 = 0.0
        if D2_labels:
            pos_count_D2 = sum(D2_labels)
            neg_count_D2 = len(D2_labels) - pos_count_D2
            f1_D2 = pos_count_D2 / len(D2_labels)
            f2_D2 = neg_count_D2 / len(D2_labels)
            gini_D2 = 1 - f1_D2 ** 2 - f2_D2 ** 2
        else:
            gini_D2 = 0.0
        gini_index = (len(D1_labels) * gini_D1 + len(D2_labels) * gini_D2) / total
        return gini_index

    def _get_pattern_signature(self, pattern: DualStarPattern) -> str:
        node_labels = sorted([node.label for node in pattern.nodes.values()])
        edge_labels = sorted([edge.label for edge in pattern.edges])
        return f'n{len(pattern.nodes)}_e{len(pattern.edges)}_{'-'.join(node_labels)}_{'-'.join(edge_labels)}'

    def _compute_all_compact_matches(self, pattern: DualStarPattern, preconditions: List, training_data: Dict[Tuple[Any, Any], bool]) -> Dict[Tuple[Any, Any], Dict[str, Set[Any]]]:
        from tie_matcher import get_pivot_candidates, _prefilter_by_attribute_index, _PREFILTER_NA, _match_star_paths, build_star_match_context, apply_path_anchor_filters
        from _compact_graph import CompactGraph
        point_wise_preds: Dict[str, list] = {}
        for pred in preconditions:
            if isinstance(pred, PointWisePredicate):
                point_wise_preds.setdefault(pred.variable, []).append(pred)
        user_candidates, item_candidates = get_pivot_candidates(self.graph, pattern, point_wise_preds)
        training_index = getattr(self, 'training_index', None)
        user_candidates, item_candidates, _pw_handled, cross_eq_pairs = apply_path_anchor_filters(self.graph, pattern, preconditions, user_candidates, item_candidates, training_index=training_index, training_data=training_data)
        if cross_eq_pairs is not None:
            training_pairs = [p for p in cross_eq_pairs if p in training_data]
        elif training_index is not None:
            training_pairs = [(u, v) for u in user_candidates for v in training_index.get(u, set()) & item_candidates if (u, v) in training_data]
        else:
            training_pairs = [(u, v) for u, v in training_data if u in user_candidates and v in item_candidates]
        logger.debug(f'    [Incremental] Pre-filtered: {len(training_data)} → {len(training_pairs)} pairs')
        if not training_pairs:
            return {}
        pivot_vars = {pattern.user_center, pattern.item_center}
        prefilter_cache: Dict[str, Any] = {}
        for var, preds in point_wise_preds.items():
            if preds and var not in pivot_vars:
                var_label = pattern.nodes[var].label
                prefiltered = _prefilter_by_attribute_index(self.graph, var_label, preds)
                prefilter_cache[var] = prefiltered if prefiltered is not None else _PREFILTER_NA
        all_paths = pattern.user_paths + pattern.item_paths
        edge_lookups_all = []
        for path in all_paths:
            edge_lookups_all.append({(edge.source_var, edge.target_var): (edge.label, edge.is_forward) for edge in path.edges})
        user_edge_lookups = edge_lookups_all[:len(pattern.user_paths)]
        item_edge_lookups = edge_lookups_all[len(pattern.user_paths):]
        _compact: 'CompactGraph' = getattr(self.graph, '_compact', None)
        user_star_ctx = build_star_match_context(self.graph, pattern, pattern.user_paths, user_edge_lookups, point_wise_preds, prefilter_cache, _compact) if pattern.user_paths else {'compact_graph': _compact}
        item_star_ctx = build_star_match_context(self.graph, pattern, pattern.item_paths, item_edge_lookups, point_wise_preds, prefilter_cache, _compact) if pattern.item_paths else {'compact_graph': _compact}
        unique_users = {u for u, v in training_pairs}
        unique_items = {v for u, v in training_pairs}
        user_star_cache: Dict = {}
        for u in unique_users:
            if pattern.user_paths:
                user_star_cache[u] = _match_star_paths(self.graph, pattern, u, pattern.user_paths, point_wise_preds, user_edge_lookups, prefilter_cache, compact_graph=_compact, star_match_ctx=user_star_ctx)
            else:
                user_star_cache[u] = {pattern.user_center: {u}}
        item_star_cache: Dict = {}
        for v in unique_items:
            if pattern.item_paths:
                item_star_cache[v] = _match_star_paths(self.graph, pattern, v, pattern.item_paths, point_wise_preds, item_edge_lookups, prefilter_cache, compact_graph=_compact, star_match_ctx=item_star_ctx)
            else:
                item_star_cache[v] = {pattern.item_center: {v}}
        matches: Dict[Tuple, Dict[str, Set]] = {}
        for u, v in training_pairs:
            u_match = user_star_cache.get(u)
            v_match = item_star_cache.get(v)
            if u_match is None or v_match is None:
                continue
            merged = {}
            merged[pattern.user_center] = {u}
            merged[pattern.item_center] = {v}
            for var, cands in u_match.items():
                if var != pattern.user_center:
                    merged[var] = cands
            _merge_failed = False
            for var, cands in v_match.items():
                if var != pattern.item_center:
                    if var in merged:
                        merged[var] = merged[var] & cands
                        if not merged[var]:
                            _merge_failed = True
                            break
                    else:
                        merged[var] = cands
            if _merge_failed:
                continue
            matches[u, v] = merged
        logger.debug(f'    [Incremental] Star-cached: {len(unique_users)} users + {len(unique_items)} items → {len(matches)}/{len(training_pairs)} pairs matched')
        return matches

    def _filter_compact_match_by_predicate(self, compact_match: Dict[str, Set[Any]], predicate, user_id: Any, item_id: Any) -> Optional[Dict[str, Set[Any]]]:
        from tie_matcher import _safe_compare
        if isinstance(predicate, PointWisePredicate):
            var = predicate.variable
            if var not in compact_match:
                return None
            filtered = set()
            for node_id in compact_match[var]:
                node = self.graph.get_node(node_id)
                if node and predicate.evaluate(node):
                    filtered.add(node_id)
            if not filtered:
                return None
            result = compact_match.copy()
            result[var] = filtered
            return result
        elif isinstance(predicate, PairWisePredicate):
            from data_structure import ComparisonOp, _normalize_predicate_equality_value
            var1, var2 = (predicate.var1, predicate.var2)
            if var1 not in compact_match or var2 not in compact_match:
                return None
            attr1, attr2, op = (predicate.attr1, predicate.attr2, predicate.operator)
            _nodes = self.graph._nodes
            map1 = {}
            for nid in compact_match[var1]:
                node = _nodes.get(nid)
                if node is not None:
                    v = node.attributes.get(attr1)
                    if v is not None:
                        map1[nid] = v
            if not map1:
                return None
            map2 = {}
            for nid in compact_match[var2]:
                node = _nodes.get(nid)
                if node is not None:
                    v = node.attributes.get(attr2)
                    if v is not None:
                        map2[nid] = v
            if not map2:
                return None
            valid_var1 = set()
            valid_var2 = set()
            if op == ComparisonOp.EQ:
                val_to_nids2 = {}
                for nid, v in map2.items():
                    nv = _normalize_predicate_equality_value(v)
                    val_to_nids2.setdefault(nv, set()).add(nid)
                for nid1, v1 in map1.items():
                    matched = val_to_nids2.get(_normalize_predicate_equality_value(v1))
                    if matched:
                        valid_var1.add(nid1)
                        valid_var2.update(matched)
            elif op == ComparisonOp.NE:
                vals2_set = {_normalize_predicate_equality_value(v) for v in map2.values()}
                for nid1, v1 in map1.items():
                    nv1 = _normalize_predicate_equality_value(v1)
                    if len(vals2_set) > 1 or (len(vals2_set) == 1 and nv1 not in vals2_set):
                        valid_var1.add(nid1)
                vals1_set = {_normalize_predicate_equality_value(v) for v in map1.values()}
                for nid2, v2 in map2.items():
                    nv2 = _normalize_predicate_equality_value(v2)
                    if len(vals1_set) > 1 or (len(vals1_set) == 1 and nv2 not in vals1_set):
                        valid_var2.add(nid2)
            else:
                for nid1, v1 in map1.items():
                    for nid2, v2 in map2.items():
                        if _safe_compare(v1, v2, op) is True:
                            valid_var1.add(nid1)
                            valid_var2.add(nid2)
            if not valid_var1 or not valid_var2:
                return None
            result = compact_match.copy()
            result[var1] = compact_match[var1] & valid_var1
            result[var2] = compact_match[var2] & valid_var2
            if not result[var1] or not result[var2]:
                return None
            return result
        elif isinstance(predicate, MLPredicate):
            return compact_match
        return compact_match

    def _compute_metrics_from_compact_matches(self, compact_matches: Dict[Tuple[Any, Any], Dict[str, Set[Any]]], training_data: Dict[Tuple[Any, Any], bool], variable_predicates: List[PairWisePredicate]=None, rule: 'TIERule'=None) -> 'RuleMetrics':
        from data_structure import RuleMetrics
        from tie_matcher import verify_variable_predicates, _check_conclusion_edge
        matched_pivots = set()
        positive_pivots = set()
        valid_pairs = {(u, v): compact_match for (u, v), compact_match in compact_matches.items() if (u, v) in training_data}
        for (u, v), compact_match in valid_pairs.items():
            if variable_predicates:
                if not verify_variable_predicates(self.graph, compact_match, variable_predicates):
                    continue
            matched_pivots.add((u, v))
            if rule is not None:
                if _check_conclusion_edge(self.graph, u, v, rule, training_data=training_data):
                    positive_pivots.add((u, v))
            elif training_data[u, v]:
                positive_pivots.add((u, v))
        support = len(matched_pivots)
        matched_count = support
        confidence = len(positive_pivots) / support if support > 0 else 0.0
        return RuleMetrics(support=support, confidence=confidence, matched_count=matched_count, matched_pivots=matched_pivots, positive_pivots=positive_pivots)

    def _expand_precondition(self, pattern: DualStarPattern, current_preconditions: List, delta: List, training_data: Dict[Tuple[Any, Any], bool], min_support: int, min_confidence: float, parent_matches: Optional[Dict[Tuple[Any, Any], Dict[str, Set[Any]]]]=None, pruned_prefixes: Optional[Set[frozenset]]=None) -> List[Tuple[List, Dict, bool]]:
        from data_structure import RuleMetrics
        from tie_matcher import verify_variable_predicates
        current_pred_strs = {str(p) for p in current_preconditions}
        existing_pairwise = sum((1 for p in current_preconditions if isinstance(p, PairWisePredicate)))
        max_pairwise = 3
        current_pairwise_preds = [p for p in current_preconditions if isinstance(p, PairWisePredicate)]
        candidates = []
        for p in delta:
            if str(p) in current_pred_strs:
                continue
            if isinstance(p, PairWisePredicate) and existing_pairwise >= max_pairwise:
                continue
            if isinstance(p, PairWisePredicate):
                contradicts = False
                for existing in current_pairwise_preds:
                    if _are_contradictory(p, existing):
                        contradicts = True
                        break
                if contradicts:
                    continue
            candidates.append(p)
        if not candidates:
            return []
        if parent_matches is None:
            logger.warning('      [警告] parent_matches 未传入，回退到全量计算')
            parent_matches = self._compute_all_compact_matches(pattern, current_preconditions, training_data)
        if not parent_matches:
            return []
        if self.use_gini and candidates:
            t0 = time.perf_counter()
            gini_scores = self._compute_gini_batch_optimized(pattern, current_preconditions, candidates, training_data, parent_matches=parent_matches)
            elapsed = time.perf_counter() - t0
            self.stats.inc('gini_computation_time', elapsed)
            gini_scores.sort(key=lambda x: x[1])
            for rank, (pred, gini) in enumerate(gini_scores[:5], 1):
                pred_type = '变量' if isinstance(pred, PairWisePredicate) else '点'
            selected = [pred for pred, _ in gini_scores[:self.gini_top_k]]
        else:
            selected = candidates[:self.gini_top_k]
        results = []
        for pred in selected:
            extended = current_preconditions + [pred]
            if self._is_trivial_rule(pattern, extended):
                logger.debug(f'        [Trivial] 跳过平凡规则: {pred}')
                continue
            if self.enable_prefix_pruning and pruned_prefixes is not None:
                extended_frozen = frozenset((str(p) for p in extended))
                with self.pruning_lock:
                    is_pruned_superset = any((pruned_prefix.issubset(extended_frozen) for pruned_prefix in pruned_prefixes))
                    if is_pruned_superset:
                        self.pruning_stats['rules_skipped'] += 1
                if is_pruned_superset:
                    logger.debug(f'        [Pruning] Skipped {pred} (superset of pruned prefix)')
                    continue
            rule_sig = self._get_rule_signature(pattern, extended)
            already_verified = False
            with self.verified_rules_lock:
                if rule_sig in self.verified_rules_cache:
                    self.verified_rules_cache.move_to_end(rule_sig)
                    cached_entry = self.verified_rules_cache[rule_sig]
                    if len(cached_entry) == 3:
                        cached_support, cached_confidence, cached_positive_count = cached_entry
                    else:
                        cached_support, cached_confidence = cached_entry
                        cached_positive_count = int(round(cached_support * cached_confidence))
                    logger.debug(f'        ✓ 规则已验证（缓存）: support={cached_support}, conf={cached_confidence:.3f}')
                    cached_child = None
                    if parent_matches is not None and cached_support >= min_support:
                        cached_pw = [p for p in extended if isinstance(p, PairWisePredicate)]
                        cached_child = {}
                        for (u, v), cm in parent_matches.items():
                            filtered = self._filter_compact_match_by_predicate(cm, pred, u, v)
                            if filtered is None:
                                continue
                            if cached_pw and (not verify_variable_predicates(self.graph, filtered, cached_pw)):
                                continue
                            if filtered is not None:
                                cached_child[u, v] = filtered
                    if cached_support >= min_support and cached_confidence >= min_confidence:
                        cached_metrics = RuleMetrics(support=cached_support, confidence=cached_confidence, matched_count=0, matched_pivots=set(), positive_pivots=set())
                        results.append((extended, cached_child if cached_child is not None else {}, True, cached_metrics))
                    elif cached_support >= min_support:
                        if self._is_confidence_branch_impossible(cached_support, cached_positive_count, min_support, min_confidence):
                            if self.enable_prefix_pruning and pruned_prefixes is not None:
                                pruned_frozen = frozenset((str(p) for p in extended))
                                with self.pruning_lock:
                                    if len(pruned_prefixes) < 2000:
                                        pruned_prefixes.add(pruned_frozen)
                                        self.pruning_stats['prefixes_pruned'] += 1
                        else:
                            cached_metrics = RuleMetrics(support=cached_support, confidence=cached_confidence, matched_count=0, matched_pivots=set(), positive_pivots=set())
                            results.append((extended, cached_child if cached_child else None, False, cached_metrics))
                    elif self.enable_prefix_pruning and pruned_prefixes is not None:
                        pruned_frozen = frozenset((str(p) for p in extended))
                        with self.pruning_lock:
                            if len(pruned_prefixes) < 2000:
                                pruned_prefixes.add(pruned_frozen)
                                self.pruning_stats['prefixes_pruned'] += 1
                    already_verified = True
            if already_verified:
                continue
            t0 = time.perf_counter()
            child_matches = {}
            for (u, v), compact_match in parent_matches.items():
                filtered_match = self._filter_compact_match_by_predicate(compact_match, pred, u, v)
                if filtered_match is not None:
                    child_matches[u, v] = filtered_match
            filtering_time = time.perf_counter() - t0
            logger.debug(f'        [增量] {len(parent_matches)} → {len(child_matches)} pairs ({filtering_time:.3f}s)')
            all_pw = [p for p in extended if isinstance(p, PairWisePredicate)]
            _edge_label = getattr(self, 'conclusion_edge_label', None) or self._extract_edge_label_from_pattern(pattern)
            _expand_rule = TIERule(pattern=pattern, preconditions=extended, conclusion=EdgePredicate(pattern.user_center, pattern.item_center, _edge_label))
            t0 = time.perf_counter()
            metrics = self._compute_metrics_from_compact_matches(child_matches, training_data, variable_predicates=all_pw if all_pw else None, rule=_expand_rule)
            metrics_time = time.perf_counter() - t0
            self.stats.inc('rule_metrics_time', filtering_time + metrics_time)
            self.stats.inc('rule_metrics_calls')
            with self.verified_rules_lock:
                self.verified_rules_cache[rule_sig] = (metrics.support, metrics.confidence, len(metrics.positive_pivots))
                if len(self.verified_rules_cache) > self.max_verified_cache_size:
                    self.verified_rules_cache.popitem(last=False)
            if metrics.support >= min_support and metrics.confidence >= min_confidence:
                results.append((extended, child_matches, True, metrics))
            elif metrics.support >= min_support:
                positive_count = len(metrics.positive_pivots)
                if self._is_confidence_branch_impossible(metrics.support, positive_count, min_support, min_confidence):
                    if self.enable_prefix_pruning and pruned_prefixes is not None:
                        pruned_frozen = frozenset((str(p) for p in extended))
                        with self.pruning_lock:
                            if len(pruned_prefixes) < 2000:
                                pruned_prefixes.add(pruned_frozen)
                                self.pruning_stats['prefixes_pruned'] += 1
                else:
                    results.append((extended, child_matches, False, metrics))
            elif self.enable_prefix_pruning and pruned_prefixes is not None:
                pruned_frozen = frozenset((str(p) for p in extended))
                with self.pruning_lock:
                    if len(pruned_prefixes) < 2000:
                        pruned_prefixes.add(pruned_frozen)
                        self.pruning_stats['prefixes_pruned'] += 1
                logger.debug(f'          [Pruning] Recorded pruned prefix ({len(pruned_prefixes)} total)')
        return results

    def get_profiling_stats(self) -> Dict[str, Any]:
        cache_stats = self.metrics_cache.get_stats()
        gini_total = self.stats.gini_cache_hits + self.stats.gini_cache_misses
        gini_hit_rate = self.stats.gini_cache_hits / gini_total * 100 if gini_total > 0 else 0
        return {'predicate_building_time': self.stats.predicate_building_time, 'rule_expansion_time': self.stats.rule_expansion_time, 'rule_metrics_time': self.stats.rule_metrics_time, 'rule_metrics_calls': self.stats.rule_metrics_calls, 'avg_metrics_time': self.stats.rule_metrics_time / self.stats.rule_metrics_calls if self.stats.rule_metrics_calls > 0 else 0, 'cache_hits': cache_stats['hits'], 'cache_misses': cache_stats['misses'], 'cache_hit_rate': cache_stats['hit_rate'], 'gini_cache_hits': self.stats.gini_cache_hits, 'gini_cache_misses': self.stats.gini_cache_misses, 'gini_cache_hit_rate': gini_hit_rate, 'patterns_with_support': self.stats.patterns_with_support, 'patterns_without_support': self.stats.patterns_without_support, 'total_rules_generated': self.stats.total_rules_generated, 'pruned_prefixes': self.pruning_stats['prefixes_pruned'], 'rules_skipped_by_pruning': self.pruning_stats['rules_skipped'], 'prefixes_recorded': self.pruning_stats['prefixes_pruned']}

class TIELearner:

    def __init__(self, graph, training_data: Dict[Tuple[Any, Any], bool], ml_cache: MLPredicateCache, max_pattern_nodes: int=20, max_predicates: int=20, min_support: int=10, min_confidence: float=0.6, user_label: str='user', item_label: str='item', use_parallel: bool=True, n_workers: int=None, use_incremental_matching: bool=True, enable_prefix_pruning: bool=True, data_dir: str=None, time_limit: int=86400):
        self.graph = graph
        self.training_data = training_data
        self.ml_cache = ml_cache
        self.max_pattern_nodes = max_pattern_nodes
        self.max_predicates = max_predicates
        min_predicates = 8
        if self.max_predicates < min_predicates:
            logger.warning(f'  max_predicates={self.max_predicates} 过小，调整为 {min_predicates}')
            self.max_predicates = min_predicates
        self.min_support = min_support
        self.min_confidence = min_confidence
        self.time_limit = time_limit
        self.user_label = user_label
        self.item_label = item_label
        self.conclusion_edge_label = self._detect_conclusion_edge_label()
        self.data_dir = data_dir
        self.attr_config = None
        if data_dir is not None:
            self.attr_config = load_attribute_config(data_dir, self.graph)
        self.use_parallel = use_parallel
        if n_workers is None:
            n_workers = max(1, int(multiprocessing.cpu_count() * 0.75))
        self.n_workers = n_workers
        self.predicate_gen = PredicateGenerator(graph=graph, ml_cache=ml_cache, gini_top_k=5, max_candidate_predicates=20, max_attribute_samples=3, use_incremental_matching=use_incremental_matching, enable_prefix_pruning=enable_prefix_pruning, attr_config=self.attr_config)
        self.predicate_gen.conclusion_edge_label = self.conclusion_edge_label
        self.discovered_rules = []
        self.training_index: Dict[Any, Set[Any]] = {}
        for u, v in training_data.keys():
            self.training_index.setdefault(u, set()).add(v)
        self.predicate_gen.training_index = self.training_index
        self.rule_metrics_cache: Dict[str, Any] = {}
        self._rule_metrics_cache_lock = Lock()
        self.realtime_save_dir: Optional[str] = None
        self.realtime_log_path: Optional[str] = None
        self.rule_counter = 0
        self._saved_rule_sigs: Set[str] = set()

    def _detect_conclusion_edge_label(self) -> Optional[str]:
        esl = getattr(self.graph, 'edge_source_label', None) or {}
        etl = getattr(self.graph, 'edge_target_label', None) or {}
        candidates = [el for el in esl if esl.get(el) == self.user_label and etl.get(el) == self.item_label]
        if len(candidates) == 1:
            label = candidates[0]
            return label
        if len(candidates) == 0:
            logger.warning(f"  ⚠ No edge from '{self.user_label}'→'{self.item_label}' found in schema")
            return None
        positive_pairs = [(u, v) for (u, v), lbl in self.training_data.items() if lbl]
        sample = positive_pairs[:200]
        edge_hits = {el: 0 for el in candidates}
        for u, v in sample:
            for el in candidates:
                if self.graph.has_edge(u, el, v):
                    edge_hits[el] += 1
        best = max(edge_hits, key=edge_hits.get)
        return best

    def setup_realtime_saving(self, dataset_name: str, base_dir: str='rules'):
        timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
        self.realtime_save_dir = os.path.join(base_dir, dataset_name, timestamp)
        os.makedirs(self.realtime_save_dir, exist_ok=True)
        self.realtime_log_path = os.path.join(self.realtime_save_dir, 'mining_progress.log')
        with open(self.realtime_log_path, 'w') as f:
            f.write('TIE Rule Mining Progress Log\n')
            f.write(f'Dataset: {dataset_name}\n')
            f.write(f'Start Time: {timestamp}\n')
            f.write('=' * 70 + '\n\n')
        self.rule_counter = 0

    def _save_rule_realtime(self, rule, metrics=None):
        if metrics is None or (metrics.support > 0 and (not metrics.positive_pivots)):
            metrics = compute_rule_metrics(self.graph, rule, self.training_data, self.ml_cache, training_index=self.training_index)
        pred_sig = '|'.join(sorted((str(p) for p in rule.preconditions)))
        with self._rule_metrics_cache_lock:
            if pred_sig in self._saved_rule_sigs:
                logger.debug(f'  ⚠️ 规则已存在（跨模式去重），跳过保存: {pred_sig[:80]}...')
                return
            self._saved_rule_sigs.add(pred_sig)
            self.discovered_rules.append(rule)
            self.rule_counter += 1
            current_counter = self.rule_counter
            cache_key = self._get_rule_cache_key(rule)
            self.rule_metrics_cache[cache_key] = metrics
        if self.realtime_save_dir is None:
            return
        rule_filename = f'rule_{current_counter:04d}.json'
        rule_path = os.path.join(self.realtime_save_dir, rule_filename)
        rule_data = {'rule_id': current_counter, 'support': metrics.support, 'confidence': round(metrics.confidence, 4), 'matched_count': metrics.matched_count, 'pattern': {'user_center': rule.pattern.user_center, 'item_center': rule.pattern.item_center, 'num_nodes': len(rule.pattern.nodes), 'num_user_paths': len(rule.pattern.user_paths), 'num_item_paths': len(rule.pattern.item_paths), 'total_edges': sum((len(p.edges) for p in rule.pattern.user_paths + rule.pattern.item_paths)), 'nodes': {var: {'label': node.label, 'attributes': sorted(node.attributes) if isinstance(node.attributes, set) else list(node.attributes)} for var, node in rule.pattern.nodes.items()}, 'user_paths': [{'sequence': path.path_sequence, 'length': len(path.edges), 'edges': [{'source_var': edge.source_var, 'target_var': edge.target_var, 'label': edge.label, 'is_forward': edge.is_forward} for edge in path.edges]} for path in rule.pattern.user_paths], 'item_paths': [{'sequence': path.path_sequence, 'length': len(path.edges), 'edges': [{'source_var': edge.source_var, 'target_var': edge.target_var, 'label': edge.label, 'is_forward': edge.is_forward} for edge in path.edges]} for path in rule.pattern.item_paths]}, 'preconditions': [str(pred) for pred in rule.preconditions], 'num_preconditions': len(rule.preconditions), 'conclusion': str(rule.conclusion), 'timestamp': datetime.now().isoformat()}
        with open(rule_path, 'w') as f:
            json.dump(rule_data, f, indent=2)
        logger.debug(f'  ✓ Saved rule #{current_counter} to {rule_filename} (support={metrics.support}, conf={metrics.confidence:.3f})')

    def _update_progress_log(self, coverage: Optional[float]=None):
        if self.realtime_log_path is None:
            return
        if coverage is None:
            coverage = self._compute_coverage()
        with open(self.realtime_log_path, 'a') as f:
            timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
            f.write(f'[{timestamp}] Rules Discovered: {self.rule_counter}\n')
            f.write(f'[{timestamp}] Validation Coverage: {coverage:.2f}%\n')
            f.write('-' * 70 + '\n')

    def learn(self, ml_predicates: List[MLPredicate], patterns_file: str=None, data_dir: str=None, convergence_threshold: float=0.95, time_limit: int=None) -> List[TIERule]:
        if time_limit is not None:
            self.time_limit = time_limit
        total_start = time.time()
        if patterns_file is None:
            import os
            if data_dir is None:
                data_dir = 'data/steam'
            dataset_name = os.path.basename(os.path.normpath(data_dir))
            rl_patterns_file = os.path.join(data_dir, f'{dataset_name}_rl_patterns.pkl')
            regular_patterns_file = os.path.join(data_dir, f'{dataset_name}_patterns.pkl')
            if os.path.exists(rl_patterns_file):
                patterns_file = rl_patterns_file
            elif os.path.exists(regular_patterns_file):
                patterns_file = regular_patterns_file
            else:
                patterns_file = rl_patterns_file
        import os
        assert os.path.exists(patterns_file), f'Pattern file not found: {patterns_file}\n\nPlease generate patterns first using:\n  (Recommended) RL-based generator:\n    python rl_pattern_generator.py --dataset steam --num-patterns 3000\n\n  Or legacy GGM-based generator:\n    python ggm_trainer.py --data-dir data/steam --dataset steam --num-patterns 3000'
        with open(patterns_file, 'rb') as f:
            import pickle
            all_patterns = pickle.load(f)
        anchor_idx = getattr(self.graph, 'path_anchor_index', None)
        if anchor_idx is not None:
            pass
        else:
            logger.warning('  ⚠ PathAnchorIndex not available (build during load_dataset)')
        node_counts = [len(p.nodes) for p in all_patterns]
        if node_counts:
            from collections import Counter
            count_dist = Counter(node_counts)
            for n in sorted(count_dist.keys()):
                pass
        filtered_patterns = [p for p in all_patterns if len(p.nodes) <= self.max_pattern_nodes]
        if len(filtered_patterns) < len(all_patterns):
            if len(filtered_patterns) == 0:
                min_nodes = min(node_counts) if node_counts else 0
                logger.error(f'  ✗ ALL patterns filtered out! Minimum node count: {min_nodes}')
                logger.error(f'    Consider increasing --max-pattern-nodes (current: {self.max_pattern_nodes})')
        mining_start = time.time()
        patterns_processed = 0
        current_coverage = 0.0
        last_coverage_check = 0
        coverage_check_interval = max(1, len(filtered_patterns) // 20)
        sorted_patterns = sorted(filtered_patterns, key=lambda p: (len(p.nodes), len(p.edges), random.random()))
        import gc
        global _SHARED_GRAPH, _SHARED_TRAINING_DATA, _SHARED_TRAINING_INDEX
        global _SHARED_ML_CACHE, _SHARED_ML_PREDICATES, _SHARED_ATTR_CONFIG
        global _SHARED_PATTERNS, _SHARED_MINING_CONFIG
        _SHARED_GRAPH = self.graph
        _SHARED_TRAINING_DATA = self.training_data
        _SHARED_TRAINING_INDEX = self.training_index
        _SHARED_ML_CACHE = self.ml_cache
        _SHARED_ML_PREDICATES = ml_predicates
        _SHARED_ATTR_CONFIG = self.attr_config
        _SHARED_PATTERNS = sorted_patterns
        _SHARED_MINING_CONFIG = {'min_support': self.min_support, 'min_confidence': self.min_confidence, 'max_predicates': self.max_predicates, 'gini_top_k': self.predicate_gen.gini_top_k, 'max_candidate_predicates': self.predicate_gen.max_candidate_predicates, 'max_attribute_samples': self.predicate_gen.max_attribute_samples, 'use_incremental_matching': self.predicate_gen.use_incremental_matching, 'enable_prefix_pruning': self.predicate_gen.enable_prefix_pruning, 'conclusion_edge_label': self.conclusion_edge_label}
        if sorted_patterns:
            import pickle as _pkl_test
            try:
                _pkl_test.dumps(sorted_patterns[0])
            except Exception as _pkl_err:
                logger.error(f'  ✗ Pattern 不可 pickle: {_pkl_err}')
                logger.error('    请检查 DualStarPattern 及其成员的可序列化性')
                raise
        gc.disable()
        gc.freeze()
        mp_ctx = _get_parallel_context()
        n_patterns = len(sorted_patterns)
        chunksize = max(1, min(8, n_patterns // (self.n_workers * 4)))
        pool = mp_ctx.Pool(processes=self.n_workers, initializer=_init_worker, initargs=(self.graph, self.training_data, self.training_index, self.ml_cache, ml_predicates, self.attr_config, sorted_patterns, dict(_SHARED_MINING_CONFIG)))
        try:
            async_results = {}
            for idx in range(n_patterns):
                ar = pool.apply_async(_worker_mine_pattern, (idx,))
                async_results[idx] = ar
            with tqdm(total=n_patterns, desc='    Mining rules') as pbar:
                stop_mining = False
                while async_results and (not stop_mining):
                    elapsed = time.time() - mining_start
                    if elapsed >= self.time_limit:
                        pool.terminate()
                        break
                    done_indices = [idx for idx, ar in async_results.items() if ar.ready()]
                    if not done_indices:
                        time.sleep(1)
                        continue
                    for idx in done_indices:
                        ar = async_results.pop(idx)
                        patterns_processed += 1
                        pbar.update(1)
                        try:
                            worker_result = ar.get(timeout=5)
                        except Exception as e:
                            logger.error(f'    Worker error (pattern {idx}): {e}')
                            continue
                        if worker_result:
                            for rule, metrics in worker_result:
                                self._save_rule_realtime(rule, metrics=metrics)
                            last_coverage_check += len(worker_result)
                    if last_coverage_check >= coverage_check_interval and len(self.discovered_rules) > 0:
                        current_coverage = self._compute_coverage()
                        last_coverage_check = 0
                        elapsed = time.time() - mining_start
                        if self.realtime_save_dir is not None:
                            self._update_progress_log(coverage=current_coverage)
                        if current_coverage >= convergence_threshold * 100:
                            pool.terminate()
                            stop_mining = True
        except KeyboardInterrupt:
            logger.warning('\n  ⚠️ Interrupted, terminating workers...')
            pool.terminate()
        finally:
            pool.close()
            pool.join()
            gc.unfreeze()
            gc.enable()
            gc.collect()
            _SHARED_GRAPH = None
            _SHARED_TRAINING_DATA = None
            _SHARED_TRAINING_INDEX = None
            _SHARED_ML_CACHE = None
            _SHARED_ML_PREDICATES = None
            _SHARED_ATTR_CONFIG = None
            _SHARED_PATTERNS = None
            _SHARED_MINING_CONFIG = None
        coverage = self._compute_coverage()
        total_time = time.time() - total_start
        self._print_profiling_stats(total_time)
        return self.discovered_rules

    def _compute_coverage(self) -> float:
        covered = set()
        positive_pairs = {k for k, v in self.training_data.items() if v}
        cache_hits = 0
        cache_misses = 0
        if self.use_parallel and len(self.discovered_rules) >= self.n_workers:

            def compute_rule_coverage(rule):
                cache_key = self._get_rule_cache_key(rule)
                with self._rule_metrics_cache_lock:
                    if cache_key in self.rule_metrics_cache:
                        return (self.rule_metrics_cache[cache_key].positive_pivots, True)
                from tie_matcher import compute_rule_metrics
                metrics = compute_rule_metrics(self.graph, rule, self.training_data, self.ml_cache, training_index=self.training_index)
                with self._rule_metrics_cache_lock:
                    self.rule_metrics_cache[cache_key] = metrics
                return (metrics.positive_pivots, False)
            with ThreadPoolExecutor(max_workers=self.n_workers) as executor:
                futures = {executor.submit(compute_rule_coverage, rule): rule for rule in self.discovered_rules}
                for future in tqdm(as_completed(futures), total=len(futures), desc='  Computing coverage (parallel)', leave=False):
                    try:
                        positive_pivots, cache_hit = future.result()
                        if cache_hit:
                            cache_hits += 1
                        else:
                            cache_misses += 1
                        covered.update(positive_pivots)
                    except Exception as e:
                        logger.error(f'Error computing coverage: {e}')
        else:
            for rule in tqdm(self.discovered_rules, desc='  Computing coverage', leave=False):
                cache_key = self._get_rule_cache_key(rule)
                if cache_key in self.rule_metrics_cache:
                    metrics = self.rule_metrics_cache[cache_key]
                    cache_hits += 1
                else:
                    from tie_matcher import compute_rule_metrics
                    metrics = compute_rule_metrics(self.graph, rule, self.training_data, self.ml_cache, training_index=self.training_index)
                    self.rule_metrics_cache[cache_key] = metrics
                    cache_misses += 1
                covered.update(metrics.positive_pivots)
        coverage = len(covered) / len(positive_pairs) * 100 if positive_pairs else 0.0
        if cache_hits + cache_misses > 0:
            logger.debug(f'  Coverage cache: {cache_hits} hits, {cache_misses} misses ({cache_hits / (cache_hits + cache_misses) * 100:.1f}% hit rate)')
        return coverage

    def _get_rule_cache_key(self, rule: TIERule) -> str:
        import hashlib
        pattern = rule.pattern
        pattern_key = f'n{len(pattern.nodes)}_e{len(pattern.edges)}'
        node_labels = '_'.join(sorted([f'{var}:{node.label}' for var, node in pattern.nodes.items()]))
        edge_labels = '_'.join(sorted([f'{e.source_var}-{e.label}{('>' if e.is_forward else '<')}{e.target_var}' for e in pattern.edges]))
        pred_strs = sorted([str(p) for p in rule.preconditions])
        pred_key = '_'.join(pred_strs)
        full_key = f'{pattern_key}:{node_labels}:{edge_labels}:{pred_key}'
        if len(full_key) > 200:
            hash_digest = hashlib.md5(full_key.encode()).hexdigest()
            return f'{pattern_key}:{hash_digest}'
        return full_key

    def _print_profiling_stats(self, total_time: float):
        stats = self.predicate_gen.get_profiling_stats()

