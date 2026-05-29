from typing import TYPE_CHECKING, Any, Dict, Iterable, List, Optional, Set, Tuple, TypeAlias
import logging
import os
import time
from collections import defaultdict
from concurrent.futures import ThreadPoolExecutor
import numpy as np
try:
    import torch
except Exception:
    torch = None
if TYPE_CHECKING:
    from torch import Tensor as TorchTensor
else:
    TorchTensor: TypeAlias = Any
from data_structure import AggrPredicate, DisPredicate, EdgePredicate, Graph, TIERule, MLPredicate, OMRule, OMRPattern, PairWisePredicate, Path, PointWisePredicate, RecPredicate, JointPattern, RootedDAG, DualStarPattern, canonical_tie_rule_signature
from tie_matcher import _match_star_paths, build_star_match_context, verify_variable_predicates, _prefilter_by_attribute_index, _PREFILTER_NA
from models import MLPredicateCache, DisModel, AggrModel, TopicProximityModel, RankingScoreComputer
import heapq
from omr_matcher import init_candidate_map, local_refinement, group_refinement, _get_omr_children_by_parent, _get_required_edge_groups_cached, _get_rule_group_shape_cached, _get_rule_group_runtime_cached, _invalidate_star_caches, _build_prefilter_cache, _build_edge_lookups, _compute_bridge_vw_pairs, _compute_vw_pairs_by_anchor_refinement, _nodes_with_required_edge_groups
from omr_miner import BridgeReachabilityIndex
from indexing import OfflineIndex
try:
    from _fast_match import cleanup_lazy_heap_fast as _cy_cleanup_lazy_heap, update_global_best_and_tau_fast as _cy_update_global_best_and_tau, update_heap_and_tau_fast as _cy_update_heap_and_tau, update_unique_topk_heap_fast as _cy_update_unique_topk_heap
    _HAS_FAST_HEAP = True
except ImportError:
    _HAS_FAST_HEAP = False
logger = logging.getLogger(__name__)
_DEFAULT_TOPK_MAX_WORKERS = max(1, os.cpu_count() or 1)
_TOPK_PAR_MIN_V = 8
_TOPK_PAR_MIN_PAIRS = 2000
_TOPK_BUILD_GROUP_PAR_MIN_GROUPS = 4
_DEFAULT_TOPK_DENSE_GPU_MAX_MB = 512
_TOPK_PHASE_TIMING_HOOK = None
_RENAMED_OFFLINE_ALT_CACHE: Dict[Tuple[int, int, Tuple[Tuple[str, str], ...]], Dict[Any, Dict[str, Set[Any]]]] = {}
_RENAMED_BRIDGE_INDEX_CACHE: Dict[Tuple[int, Tuple[Tuple[str, str], ...]], '_RenamedBridgeReachabilityIndex'] = {}
_TOPK_GROUP_TEMPLATE_CACHE: Dict[Tuple[int, int, Tuple[int, ...], str, str, str], Dict[str, Any]] = {}
_TOPK_GAMMA_GLOBAL_UPPER_CACHE: Dict[Tuple[int, int, int, int, Any], float] = {}

def _env_flag(name: str) -> bool:
    raw = str(os.getenv(name, '')).strip().lower()
    return raw in {'1', 'true', 'yes', 'on'}

def _topk_build_group_profile_enabled() -> bool:
    return _env_flag('OPENMIND_TOPK_PROFILE_BUILD_GROUP')

def _topk_build_group_parallel_enabled() -> bool:
    return _env_flag('OPENMIND_TOPK_BUILD_GROUP_PARALLEL')

def _topk_omr_parallel_enabled() -> bool:
    return _env_flag('OPENMIND_TOPK_OMR_PARALLEL')

def _record_timing(timing_out: Optional[Dict[str, float]], key: str, started_at: float) -> None:
    if timing_out is not None:
        timing_out[key] = timing_out.get(key, 0.0) + (time.perf_counter() - started_at)

def _increment_counter(cache_stats_out: Optional[Dict[str, int]], key: str, amount: int=1) -> None:
    if cache_stats_out is not None:
        cache_stats_out[key] = int(cache_stats_out.get(key, 0)) + int(amount)

def _record_parallel_bucket(parallel_stats: Optional[Dict[str, Any]], bucket: str, key: Any, elapsed: float) -> None:
    if parallel_stats is None:
        return
    bucket_map = parallel_stats.setdefault(bucket, {})
    bucket_map[key] = float(bucket_map.get(key, 0.0)) + float(elapsed)

def _build_parallel_proxy_summary(total_seconds: float, parallel_stats: Optional[Dict[str, Any]]) -> Dict[str, Any]:
    if not parallel_stats:
        return {}
    sections: List[Dict[str, Any]] = []
    actual_replaced = 0.0
    proxy_replaced = 0.0

    def _add_max_bucket(bucket_name: str, label: str) -> None:
        nonlocal actual_replaced, proxy_replaced
        bucket = parallel_stats.get(bucket_name, {})
        if not bucket:
            return
        actual = float(sum(bucket.values()))
        proxy = float(max(bucket.values())) if bucket else 0.0
        actual_replaced += actual
        proxy_replaced += proxy
        sections.append({'section': label, 'mode': 'max_bucket', 'n_buckets': len(bucket), 'actual_seconds': round(actual, 6), 'proxy_seconds': round(proxy, 6), 'saved_seconds': round(max(actual - proxy, 0.0), 6)})

    def _add_iter_max_bucket(bucket_name: str, label: str) -> None:
        nonlocal actual_replaced, proxy_replaced
        iterations = parallel_stats.get(bucket_name, [])
        if not iterations:
            return
        actual = 0.0
        proxy = 0.0
        n_buckets = 0
        for per_iter in iterations:
            if not per_iter:
                continue
            actual += float(sum(per_iter.values()))
            proxy += float(max(per_iter.values()))
            n_buckets += len(per_iter)
        if actual <= 0.0 and proxy <= 0.0:
            return
        actual_replaced += actual
        proxy_replaced += proxy
        sections.append({'section': label, 'mode': 'sum_iter_max', 'n_iterations': len(iterations), 'n_buckets': n_buckets, 'actual_seconds': round(actual, 6), 'proxy_seconds': round(proxy, 6), 'saved_seconds': round(max(actual - proxy, 0.0), 6)})
    _add_max_bucket('anchor_rule_index_by_anchor', 'anchor_rule_index_by_anchor')
    _add_max_bucket('anchor_select_user_rule_filter_by_rule', 'anchor_select_user_rule_filter_by_rule')
    _add_max_bucket('anchor_select_dense_lookup_by_item', 'anchor_select_dense_lookup_by_item')
    _add_max_bucket('anchor_select_dense_materialize_by_item', 'anchor_select_dense_materialize_by_item')
    _add_max_bucket('anchor_select_sparse_score_by_item', 'anchor_select_sparse_score_by_item')
    _add_max_bucket('anchor_select_candidate_rule_by_item', 'anchor_select_candidate_rule_by_item')
    _add_iter_max_bucket('anchor_select_window_rule_total_by_iteration', 'anchor_select_window_rule_total_by_iteration')
    _add_max_bucket('active_omr_index_by_tie', 'active_omr_index_by_tie')
    _add_max_bucket('build_global_group_by_tie', 'build_global_group_by_tie')
    _add_max_bucket('runtime_prepare_by_tie', 'runtime_prepare_by_tie')
    _add_max_bucket('offline_bridge_prepare_by_tie', 'offline_bridge_prepare_by_tie')
    _add_max_bucket('global_bridge_index_by_tie', 'global_bridge_index_by_tie')
    _add_max_bucket('candidate_ws_init_by_anchor', 'candidate_ws_init_by_anchor')
    _add_iter_max_bucket('omr_group_total_by_iteration', 'omr_group_total_by_iteration')
    estimated = max(float(total_seconds) - actual_replaced + proxy_replaced, 0.0)
    return {'estimated_parallel_elapsed_seconds': round(estimated, 6), 'actual_replaced_seconds': round(actual_replaced, 6), 'proxy_replaced_seconds': round(proxy_replaced, 6), 'saved_seconds': round(max(actual_replaced - proxy_replaced, 0.0), 6), 'sections': sections}
_heap_counter = 0

class TIEInvertedIndex:

    def __init__(self, graph: Graph, tie_rules: List[TIERule]):
        self.graph = graph
        self.tie_rules = tie_rules
        self.rule_preds = []
        self.rule_item_labels = []
        self.rule_item_degrees = []
        self._degree_cache: Dict[Any, int] = {}
        for rule in tie_rules:
            y0 = rule.pattern.item_center
            y0_node = rule.pattern.nodes.get(y0)
            preds = rule.get_point_wise_predicates().get(rule.pattern.item_center, [])
            self.rule_preds.append(preds)
            self.rule_item_labels.append(y0_node.label if y0_node is not None else None)
            self.rule_item_degrees.append(sum((1 for edge in rule.pattern.edges if edge.source_var == y0 or edge.target_var == y0)))

    def _node_degree(self, node_id: Any) -> int:
        if node_id not in self._degree_cache:
            self._degree_cache[node_id] = len(self.graph.get_out_edges(node_id)) + len(self.graph.get_in_edges(node_id))
        return self._degree_cache[node_id]

    def get_candidate_rules(self, v_node: Any) -> List[int]:
        if not v_node:
            return []
        valid_indices = []
        for i, preds in enumerate(self.rule_preds):
            expected_label = self.rule_item_labels[i]
            if expected_label and v_node.label != expected_label:
                continue
            min_degree = self.rule_item_degrees[i]
            if min_degree > 0 and self._node_degree(v_node.node_id) < min_degree:
                continue
            if all((p.evaluate(v_node) for p in preds)):
                valid_indices.append(i)
        return valid_indices

def _build_tie_runtime_template(graph: Graph, rule: TIERule, compact_graph: Any) -> Dict[str, Any]:
    point_wise_preds = rule.get_point_wise_predicates()
    x0 = rule.pattern.user_center
    y0 = rule.pattern.item_center
    prefilter_cache = _build_prefilter_cache(graph, rule.pattern, point_wise_preds, {x0, y0})
    edge_lookups = {'user': _build_edge_lookups(rule.pattern.user_paths), 'item': _build_edge_lookups(rule.pattern.item_paths)}
    star_contexts = {'user': build_star_match_context(graph, rule.pattern, rule.pattern.user_paths, edge_lookups['user'], point_wise_preds, prefilter_cache, compact_graph) if rule.pattern.user_paths else {'compact_graph': compact_graph}, 'item': build_star_match_context(graph, rule.pattern, rule.pattern.item_paths, edge_lookups['item'], point_wise_preds, prefilter_cache, compact_graph) if rule.pattern.item_paths else {'compact_graph': compact_graph}}
    return {'prefilter_cache': prefilter_cache, 'edge_lookups': edge_lookups, 'star_contexts': star_contexts}

def prepare_topk_anchor_runtime(graph: Graph, tie_rules: List[TIERule], ml_cache: Optional[MLPredicateCache]=None, candidate_items: Optional[Iterable[Any]]=None, omr_rules: Optional[List[OMRule]]=None, offline_index: Optional[OfflineIndex]=None) -> Dict[str, Any]:
    compact_graph = getattr(graph, '_compact', None)
    rule_templates = {rule_idx: _build_tie_runtime_template(graph, rule, compact_graph) for rule_idx, rule in enumerate(tie_rules)}
    runtime = {'inv_index': TIEInvertedIndex(graph, tie_rules), 'rule_templates': rule_templates, 'group_cache': {}}
    dense_backend = _build_dense_catalog_backend(ml_cache=ml_cache, candidate_items=candidate_items)
    if dense_backend is not None:
        runtime['dense_backend'] = dense_backend
    _prepare_topk_offline_views(offline_index=offline_index, tie_rules=tie_rules, omr_rules=omr_rules)
    return runtime

def _dense_gpu_mode() -> str:
    return str(os.getenv('OPENMIND_TOPK_DENSE_DEVICE', 'auto')).strip().lower()

def _dense_gpu_max_bytes() -> int:
    raw = os.getenv('OPENMIND_TOPK_DENSE_GPU_MAX_MB')
    if raw:
        try:
            return max(int(raw), 1) * 1024 * 1024
        except ValueError:
            pass
    return _DEFAULT_TOPK_DENSE_GPU_MAX_MB * 1024 * 1024

def _should_prepare_dense_gpu_backend(dense_scores: Optional[np.ndarray]) -> bool:
    if torch is None or dense_scores is None or (not torch.cuda.is_available()):
        return False
    mode = _dense_gpu_mode()
    if mode in {'cpu', 'off', 'false', '0', 'disabled'}:
        return False
    if mode in {'cuda', 'gpu', 'true', '1', 'force'}:
        return True
    return int(dense_scores.nbytes) <= _dense_gpu_max_bytes()

def _get_dense_scores_cuda_cached(ml_cache: Optional[MLPredicateCache]) -> Optional[TorchTensor]:
    if ml_cache is None or torch is None:
        return None
    cached = getattr(ml_cache, '_topk_dense_scores_cuda', None)
    if cached is not None:
        return cached
    dense_scores = getattr(ml_cache, 'dense_scores', None)
    if dense_scores is None or not _should_prepare_dense_gpu_backend(dense_scores):
        return None
    try:
        dense_scores_cuda = torch.as_tensor(dense_scores, device='cuda')
    except Exception:
        logger.debug('[topk_dense] failed to cache dense_scores on CUDA', exc_info=True)
        return None
    setattr(ml_cache, '_topk_dense_scores_cuda', dense_scores_cuda)
    return dense_scores_cuda

def _warm_dense_scores_cuda_cached(dense_scores_cuda: Optional[TorchTensor]) -> None:
    if dense_scores_cuda is None or torch is None or (not dense_scores_cuda.is_cuda):
        return
    if getattr(dense_scores_cuda, '_openmind_topk_warmed', False):
        return
    try:
        warm_n = min(int(dense_scores_cuda.shape[1]), 256)
        if warm_n <= 0:
            setattr(dense_scores_cuda, '_openmind_topk_warmed', True)
            return
        warm_idx = torch.arange(warm_n, device=dense_scores_cuda.device, dtype=torch.long)
        warm_row = dense_scores_cuda[0].index_select(0, warm_idx)
        warm_mask = warm_row >= warm_row.new_tensor(0.0)
        _ = torch.nonzero(warm_mask, as_tuple=False)
        torch.cuda.synchronize(dense_scores_cuda.device)
        setattr(dense_scores_cuda, '_openmind_topk_warmed', True)
    except Exception:
        logger.debug('[topk_dense] failed to warm CUDA dense backend', exc_info=True)

def _build_dense_catalog_backend(*, ml_cache: Optional[MLPredicateCache], candidate_items: Optional[Iterable[Any]]=None) -> Optional[Dict[str, Any]]:
    if ml_cache is None:
        return None
    dense_scores = getattr(ml_cache, 'dense_scores', None)
    dense_item_ids = list(getattr(ml_cache, 'dense_item_ids', ()))
    dense_item_index = getattr(ml_cache, '_dense_item_index', None)
    if dense_scores is None or dense_item_index is None or (not dense_item_ids):
        return None
    if candidate_items is None:
        catalog_item_ids = list(dense_item_ids)
        catalog_dense_indices = np.arange(len(catalog_item_ids), dtype=np.int64)
        catalog_is_full_dense_row = True
    else:
        candidate_item_set = set(candidate_items)
        catalog_item_ids = [item_id for item_id in dense_item_ids if item_id in candidate_item_set]
        if not catalog_item_ids:
            return None
        catalog_is_full_dense_row = len(catalog_item_ids) == len(dense_item_ids)
        if catalog_is_full_dense_row:
            catalog_dense_indices = np.arange(len(catalog_item_ids), dtype=np.int64)
        else:
            catalog_dense_indices = np.fromiter((dense_item_index[item_id] for item_id in catalog_item_ids), dtype=np.int64, count=len(catalog_item_ids))
    backend: Dict[str, Any] = {'catalog_item_ids': tuple(catalog_item_ids), 'catalog_item_set': set(catalog_item_ids), 'catalog_pos_by_item': {item_id: pos for pos, item_id in enumerate(catalog_item_ids)}, 'catalog_dense_indices_np': catalog_dense_indices, 'catalog_is_full_dense_row': catalog_is_full_dense_row, 'catalog_size': len(catalog_item_ids)}
    dense_scores_cuda = _get_dense_scores_cuda_cached(ml_cache)
    if dense_scores_cuda is not None:
        _warm_dense_scores_cuda_cached(dense_scores_cuda)
        backend['dense_scores_cuda'] = dense_scores_cuda
        if not catalog_is_full_dense_row:
            backend['catalog_dense_indices_cuda'] = torch.as_tensor(catalog_dense_indices, device=dense_scores_cuda.device, dtype=torch.long)
    return backend

def _get_default_dense_catalog_backend(ml_cache: Optional[MLPredicateCache]) -> Optional[Dict[str, Any]]:
    if ml_cache is None:
        return None
    cached = getattr(ml_cache, '_topk_dense_full_backend', None)
    if cached is not None:
        return cached
    cached = _build_dense_catalog_backend(ml_cache=ml_cache)
    setattr(ml_cache, '_topk_dense_full_backend', cached)
    return cached

def _resolve_dense_catalog_backend(ml_cache: MLPredicateCache, prepared_runtime: Optional[Dict[str, Any]]) -> Optional[Dict[str, Any]]:
    if prepared_runtime is not None:
        dense_backend = prepared_runtime.get('dense_backend')
        if dense_backend is not None:
            return dense_backend
    return _get_default_dense_catalog_backend(ml_cache)

def _select_dense_catalog_subset(dense_backend: Dict[str, Any], candidate_items: Set[Any]) -> Optional[Dict[str, Any]]:
    dense_covered = candidate_items & dense_backend['catalog_item_set']
    if not dense_covered:
        return None
    catalog_size = int(dense_backend['catalog_size'])
    if len(dense_covered) == catalog_size:
        return {'mode': 'full_catalog', 'dense_covered': dense_covered, 'catalog_positions': None, 'exclude_positions': None}
    if len(dense_covered) * 2 >= catalog_size:
        excluded_positions = np.fromiter((dense_backend['catalog_pos_by_item'][item_id] for item_id in dense_backend['catalog_item_set'] - dense_covered), dtype=np.int64, count=max(catalog_size - len(dense_covered), 0))
        if excluded_positions.size > 1:
            excluded_positions.sort()
        return {'mode': 'full_catalog', 'dense_covered': dense_covered, 'catalog_positions': None, 'exclude_positions': excluded_positions}
    catalog_positions = np.fromiter((dense_backend['catalog_pos_by_item'][item_id] for item_id in dense_covered), dtype=np.int64, count=len(dense_covered))
    if catalog_positions.size > 1:
        catalog_positions.sort()
    return {'mode': 'catalog_positions', 'dense_covered': dense_covered, 'catalog_positions': catalog_positions, 'exclude_positions': None}

def _resolve_topk_parallel_workers(task_count: int, total_pairs: int) -> int:
    if task_count < _TOPK_PAR_MIN_V or total_pairs < _TOPK_PAR_MIN_PAIRS:
        return 1
    raw = os.getenv('OPENMIND_TOPK_MAX_WORKERS')
    if raw:
        try:
            cap = max(1, int(raw))
        except ValueError:
            cap = _DEFAULT_TOPK_MAX_WORKERS
    else:
        cap = _DEFAULT_TOPK_MAX_WORKERS
    return max(1, min(cap, task_count))

def _resolve_topk_group_workers(group_count: int) -> int:
    if group_count <= 1:
        return 1
    raw = os.getenv('OPENMIND_TOPK_GROUP_MAX_WORKERS')
    if raw:
        try:
            cap = max(1, int(raw))
        except ValueError:
            cap = _DEFAULT_TOPK_MAX_WORKERS
    else:
        cap = _DEFAULT_TOPK_MAX_WORKERS
    return max(2, min(cap, group_count))

def _resolve_topk_build_group_workers(group_count: int) -> int:
    if group_count < _TOPK_BUILD_GROUP_PAR_MIN_GROUPS or not _topk_build_group_parallel_enabled():
        return 1
    raw = os.getenv('OPENMIND_TOPK_BUILD_GROUP_MAX_WORKERS')
    if not raw:
        raw = os.getenv('OPENMIND_TOPK_GROUP_MAX_WORKERS')
    if raw:
        try:
            cap = max(1, int(raw))
        except ValueError:
            cap = _DEFAULT_TOPK_MAX_WORKERS
    else:
        cap = _DEFAULT_TOPK_MAX_WORKERS
    return max(2, min(cap, group_count))

def _resolve_topk_omr_workers(group_count: int) -> int:
    if group_count <= 1 or not _topk_omr_parallel_enabled():
        return 1
    raw = os.getenv('OPENMIND_TOPK_OMR_MAX_WORKERS')
    if not raw:
        raw = os.getenv('OPENMIND_TOPK_GROUP_MAX_WORKERS')
    if raw:
        try:
            cap = max(1, int(raw))
        except ValueError:
            cap = _DEFAULT_TOPK_MAX_WORKERS
    else:
        cap = _DEFAULT_TOPK_MAX_WORKERS
    return max(2, min(cap, group_count))

def _chunked(values: List[Any], n_chunks: int) -> List[List[Any]]:
    if n_chunks <= 1 or len(values) <= 1:
        return [values]
    chunk_size = max(1, (len(values) + n_chunks - 1) // n_chunks)
    return [values[i:i + chunk_size] for i in range(0, len(values), chunk_size)]

def _dense_item_scores(*, user_id: Any, candidate_items: Set[Any], ml_cache: MLPredicateCache, prepared_runtime: Optional[Dict[str, Any]]=None, min_score: Optional[float]=None, parallel_stats: Optional[Dict[str, Any]]=None) -> Tuple[Optional[List[Tuple[float, Any]]], Set[Any]]:
    dense_scores = getattr(ml_cache, 'dense_scores', None)
    dense_user_index = getattr(ml_cache, '_dense_user_index', None)
    dense_item_index = getattr(ml_cache, '_dense_item_index', None)
    if dense_scores is None or dense_user_index is None or dense_item_index is None:
        return (None, set())
    user_idx = dense_user_index.get(user_id)
    if user_idx is None:
        return (None, set())
    dense_backend = _resolve_dense_catalog_backend(ml_cache, prepared_runtime)
    if dense_backend is not None:
        selection = _select_dense_catalog_subset(dense_backend, candidate_items)
        if selection is not None:
            dense_covered = selection['dense_covered']
            catalog_item_ids = dense_backend['catalog_item_ids']
            threshold = None
            if min_score is not None:
                threshold = np.asarray(min_score, dtype=dense_scores.dtype).item()
            dense_scores_cuda = dense_backend.get('dense_scores_cuda')
            if dense_scores_cuda is not None and torch is not None:
                try:
                    if selection['mode'] == 'full_catalog':
                        if dense_backend['catalog_is_full_dense_row']:
                            score_tensor = dense_scores_cuda[user_idx]
                        else:
                            score_tensor = dense_scores_cuda[user_idx].index_select(0, dense_backend['catalog_dense_indices_cuda'])
                        if threshold is None:
                            keep_mask = torch.ones_like(score_tensor, dtype=torch.bool)
                        else:
                            threshold_tensor = torch.tensor(threshold, device=score_tensor.device, dtype=score_tensor.dtype)
                            keep_mask = score_tensor >= threshold_tensor
                        exclude_positions = selection['exclude_positions']
                        if exclude_positions is not None and exclude_positions.size:
                            exclude_tensor = torch.as_tensor(exclude_positions, device=score_tensor.device, dtype=torch.long)
                            keep_mask[exclude_tensor] = False
                        kept_positions = torch.nonzero(keep_mask, as_tuple=False).flatten()
                        if kept_positions.numel() == 0:
                            return ([], dense_covered)
                        kept_scores = score_tensor.index_select(0, kept_positions)
                    else:
                        catalog_positions = selection['catalog_positions']
                        if catalog_positions is None or catalog_positions.size == 0:
                            return (None, set())
                        catalog_positions_t = torch.as_tensor(catalog_positions, device=dense_scores_cuda.device, dtype=torch.long)
                        if dense_backend['catalog_is_full_dense_row']:
                            dense_cols_t = catalog_positions_t
                        else:
                            dense_cols_t = dense_backend['catalog_dense_indices_cuda'].index_select(0, catalog_positions_t)
                        kept_positions = catalog_positions_t
                        kept_scores = dense_scores_cuda[user_idx].index_select(0, dense_cols_t)
                        if threshold is not None:
                            threshold_tensor = torch.tensor(threshold, device=kept_scores.device, dtype=kept_scores.dtype)
                            keep_mask = kept_scores >= threshold_tensor
                            kept_positions = kept_positions[keep_mask]
                            kept_scores = kept_scores[keep_mask]
                            if kept_positions.numel() == 0:
                                return ([], dense_covered)
                    kept_positions_np = kept_positions.detach().cpu().numpy()
                    kept_scores_np = kept_scores.detach().float().cpu().numpy()
                    order = np.argsort(-kept_scores_np, kind='stable')
                    return ([(float(kept_scores_np[pos]), catalog_item_ids[int(kept_positions_np[pos])]) for pos in order.tolist()], dense_covered)
                except Exception:
                    logger.debug('[topk_dense] CUDA dense path failed; falling back to CPU', exc_info=True)
            if selection['mode'] == 'full_catalog':
                if dense_backend['catalog_is_full_dense_row']:
                    score_row = dense_scores[user_idx]
                else:
                    score_row = dense_scores[user_idx, dense_backend['catalog_dense_indices_np']]
                if threshold is None:
                    keep_mask = np.ones(score_row.shape[0], dtype=bool)
                else:
                    keep_mask = score_row >= threshold
                exclude_positions = selection['exclude_positions']
                if exclude_positions is not None and exclude_positions.size:
                    keep_mask[exclude_positions] = False
                kept_positions = np.flatnonzero(keep_mask)
                if kept_positions.size == 0:
                    return ([], dense_covered)
                kept_scores = score_row[kept_positions]
            else:
                catalog_positions = selection['catalog_positions']
                if catalog_positions is None or catalog_positions.size == 0:
                    return (None, set())
                dense_columns = dense_backend['catalog_dense_indices_np'][catalog_positions]
                kept_scores = dense_scores[user_idx, dense_columns]
                kept_positions = catalog_positions
                if threshold is not None:
                    keep_mask = kept_scores >= threshold
                    if not np.any(keep_mask):
                        return ([], dense_covered)
                    kept_positions = kept_positions[keep_mask]
                    kept_scores = kept_scores[keep_mask]
            kept_scores = np.asarray(kept_scores, dtype=np.float32)
            order = np.argsort(-kept_scores, kind='stable')
            return ([(float(kept_scores[pos]), catalog_item_ids[int(kept_positions[pos])]) for pos in order.tolist()], dense_covered)
    if parallel_stats is None:
        item_ids = [item_id for item_id in candidate_items if item_id in dense_item_index]
    else:
        item_ids = []
        for item_id in candidate_items:
            _t_item = time.perf_counter()
            dense_idx = dense_item_index.get(item_id)
            if dense_idx is not None:
                item_ids.append(item_id)
            _record_parallel_bucket(parallel_stats, 'anchor_select_dense_lookup_by_item', item_id, time.perf_counter() - _t_item)
    if not item_ids:
        return (None, set())
    item_indices = np.asarray([dense_item_index[item_id] for item_id in item_ids], dtype=np.int64)
    score_row = dense_scores[user_idx, item_indices]
    dense_covered = set(item_ids)
    if min_score is not None:
        threshold = np.asarray(min_score, dtype=score_row.dtype).item()
        keep_mask = score_row >= threshold
        if not np.any(keep_mask):
            return ([], dense_covered)
        kept_positions = np.flatnonzero(keep_mask)
        score_row = score_row[kept_positions]
        item_ids = [item_ids[pos] for pos in kept_positions.tolist()]
    score_row = np.asarray(score_row, dtype=np.float32)
    order = np.argsort(-score_row, kind='stable')
    return ([(float(score_row[pos]), item_ids[pos]) for pos in order], dense_covered)

def _compute_gamma_max_worker(vs: List[Any], *, candidate_ws_by_v: Dict[Any, Set[Any]], dis_model: DisModel, topic_model: Optional[TopicProximityModel], offline_dis_scores: Optional[Dict[Tuple[Any, Any], float]]) -> Tuple[Dict[Any, Dict[Any, float]], Dict[Any, float]]:
    batch_updates: Dict[Any, Dict[Any, float]] = {}
    gamma_chunk: Dict[Any, float] = {}
    for v in vs:
        ws = candidate_ws_by_v.get(v, set())
        if not ws:
            gamma_chunk[v] = 0.0
            continue
        local_dis: Dict[Any, float] = {}
        if offline_dis_scores:
            for w in ws:
                score = offline_dis_scores.get((v, w))
                if score is not None:
                    local_dis[w] = score
        uncached_ws = ws - set(local_dis.keys())
        if uncached_ws:
            local_dis.update(dis_model.compute_batch(v, uncached_ws))
        if local_dis:
            batch_updates[v] = local_dis
        if topic_model is not None:
            if len(local_dis) < 16:
                gamma_chunk[v] = max((topic_model.compute(v, w) * local_dis[w] for w in ws if w in local_dis), default=0.0)
            elif hasattr(topic_model, 'compute_batch'):
                sim_scores = topic_model.compute_batch(v, set(local_dis))
                gamma_chunk[v] = max((sim_scores.get(w, 0.0) * local_dis[w] for w in ws if w in local_dis), default=0.0)
            else:
                sim_scores = {w: topic_model.compute(v, w) for w in local_dis}
                gamma_chunk[v] = max((sim_scores.get(w, 0.0) * local_dis[w] for w in ws if w in local_dis), default=0.0)
        elif len(local_dis) < 16:
            gamma_chunk[v] = max(((1.0 - local_dis[w]) * local_dis[w] for w in ws if w in local_dis), default=0.0)
        elif local_dis:
            dis_arr = np.fromiter((local_dis[w] for w in ws if w in local_dis), dtype=np.float32, count=len(local_dis))
            gamma_chunk[v] = float(np.max((1.0 - dis_arr) * dis_arr)) if dis_arr.size else 0.0
        else:
            gamma_chunk[v] = 0.0
    return (batch_updates, gamma_chunk)

def _run_topk_tie_group_worker(*, graph: Graph, F_H: Dict[str, Set[Any]], rooted_dag: RootedDAG, H: List[Any], group_state: Dict[str, Any], compact_graph: Any, prefilter_caches: Dict[int, Dict[str, Any]], edge_lookups_per_rule: Dict[int, Dict[str, List[Dict]]], star_contexts_per_rule: Dict[int, Dict[str, Any]], rule_runtime_per_rule: Dict[int, Dict[str, Any]]) -> Tuple[int, Optional[Dict[str, Set[Any]]]]:
    rule_idx = group_state['tie_rule_idx']
    rule = H[rule_idx]
    rule_pw = rule.get_point_wise_predicates()
    F_phi = local_refinement(graph, F_H, rooted_dag, rule, rule_pw, compact_graph, group_state['star_cache_user'], group_state['star_cache_item'], None, prefilter_cache=prefilter_caches.get(rule_idx), edge_lookups=edge_lookups_per_rule.get(rule_idx), star_contexts=star_contexts_per_rule.get(rule_idx), rule_runtime=rule_runtime_per_rule.get(rule_idx))
    return (rule_idx, F_phi)

def _run_topk_omr_group_refinement_worker(*, graph: Graph, rooted_dag: RootedDAG, H: List[Any], group_state: Dict[str, Any], compact_graph: Any, current_tie_anchors: Set[Any], ml_cache: MLPredicateCache, candidate_maps_by_rule: Dict[int, Dict[str, Set[Any]]], prefilter_caches: Dict[int, Dict[str, Any]], edge_lookups_per_rule: Dict[int, Dict[str, List[Dict]]], star_contexts_per_rule: Dict[int, Dict[str, Any]], rule_runtime_per_rule: Dict[int, Dict[str, Any]], dis_precomputed: bool, bridge_indexes: Dict[int, BridgeReachabilityIndex]) -> Dict[str, Any]:
    local_results: Dict[int, Dict[str, Set[Any]]] = {}
    bridge_vw_per_rule: Dict[int, Optional[Dict[Any, Set[Any]]]] = {}
    started_at = time.perf_counter()
    for rule_idx in group_state['omr_rule_indices']:
        if rule_idx not in candidate_maps_by_rule:
            continue
        rule = H[rule_idx]
        candidate_map = candidate_maps_by_rule[rule_idx]
        rule_pw = rule.get_point_wise_predicates()
        bridge_vw_local: Dict[Any, Set[Any]] = {}
        if rule_idx not in group_state['star_cache_alt_per_rule']:
            group_state['star_cache_alt_per_rule'][rule_idx] = {}
        rule_alt_cache = group_state['star_cache_alt_per_rule'][rule_idx]
        F_phi = local_refinement(graph, candidate_map, rooted_dag, rule, rule_pw, compact_graph, group_state['star_cache_user'], group_state['star_cache_item'], rule_alt_cache, current_tie_anchors=current_tie_anchors, ml_cache=ml_cache, prefilter_cache=prefilter_caches.get(rule_idx), edge_lookups=edge_lookups_per_rule.get(rule_idx), star_contexts=star_contexts_per_rule.get(rule_idx), rule_runtime=rule_runtime_per_rule.get(rule_idx), bridge_vw_out=bridge_vw_local, dis_precomputed=dis_precomputed, offline_bridge_idx=bridge_indexes.get(rule_idx), offline_dis_pass=group_state['dis_pass_per_rule'].get(rule_idx))
        if F_phi is None:
            continue
        local_results[rule_idx] = F_phi
        bridge_vw_per_rule[rule_idx] = bridge_vw_local if bridge_vw_local else None
    return {'local_results': local_results, 'bridge_vw_per_rule': bridge_vw_per_rule, 'elapsed_seconds': time.perf_counter() - started_at}

class TIEMatchChecker:

    def __init__(self, graph: Graph, tie_rules: List[TIERule], ml_cache: Optional[MLPredicateCache]=None):
        self.graph = graph
        self.tie_rules = tie_rules
        self.ml_cache = ml_cache
        self.compact_graph = getattr(graph, '_compact', None)
        self._user_star_cache: Dict[Tuple[int, Any], Optional[Dict[str, Set[Any]]]] = {}
        self.rule_data = []
        for rule in tie_rules:
            pw_preds = rule.get_point_wise_predicates()
            pivot_vars = {rule.pattern.user_center, rule.pattern.item_center}
            prefilter_cache = {}
            for var, preds in pw_preds.items():
                if preds and var not in pivot_vars:
                    var_label = rule.pattern.nodes[var].label
                    prefiltered = _prefilter_by_attribute_index(graph, var_label, preds)
                    prefilter_cache[var] = prefiltered if prefiltered is not None else _PREFILTER_NA
            all_paths = rule.pattern.user_paths + rule.pattern.item_paths
            edge_lookups_all = []
            for path in all_paths:
                edge_lookups_all.append({(edge.source_var, edge.target_var): (edge.label, edge.is_forward) for edge in path.edges})
            user_edge_lookups = edge_lookups_all[:len(rule.pattern.user_paths)]
            item_edge_lookups = edge_lookups_all[len(rule.pattern.user_paths):]
            self.rule_data.append({'pw_preds': pw_preds, 'prefilter_cache': prefilter_cache, 'user_edge_lookups': user_edge_lookups, 'item_edge_lookups': item_edge_lookups, 'user_star_ctx': build_star_match_context(graph, rule.pattern, rule.pattern.user_paths, user_edge_lookups, pw_preds, prefilter_cache, self.compact_graph) if rule.pattern.user_paths else {'compact_graph': self.compact_graph}, 'item_star_ctx': build_star_match_context(graph, rule.pattern, rule.pattern.item_paths, item_edge_lookups, pw_preds, prefilter_cache, self.compact_graph) if rule.pattern.item_paths else {'compact_graph': self.compact_graph}, 'pair_wise': rule.get_pair_wise_predicates(), 'ml_preds': rule.get_ml_predicates()})

    def check_match(self, rule_idx: int, u: Any, v: Any, u_node: Any=None, v_node: Any=None) -> bool:
        rule = self.tie_rules[rule_idx]
        data = self.rule_data[rule_idx]
        pw_preds = data['pw_preds']
        u_preds = pw_preds.get(rule.pattern.user_center)
        if u_preds:
            if u_node is None:
                u_node = self.graph.get_node(u)
            if not u_node or not all((p.evaluate(u_node) for p in u_preds)):
                return False
        v_preds = pw_preds.get(rule.pattern.item_center)
        if v_preds:
            if v_node is None:
                v_node = self.graph.get_node(v)
            if not v_node or not all((p.evaluate(v_node) for p in v_preds)):
                return False
        ml_preds = data['ml_preds']
        if ml_preds and self.ml_cache is not None:
            score = self.ml_cache.get_prediction(u, v)
            for mp in ml_preds:
                if not mp.evaluate(score):
                    return False
        cache_key = (rule_idx, u)
        if cache_key in self._user_star_cache:
            u_match = self._user_star_cache[cache_key]
        else:
            u_match = {rule.pattern.user_center: {u}}
            if rule.pattern.user_paths:
                u_match = _match_star_paths(self.graph, rule.pattern, u, rule.pattern.user_paths, pw_preds, data['user_edge_lookups'], data['prefilter_cache'], compact_graph=self.compact_graph, star_match_ctx=data['user_star_ctx'])
            self._user_star_cache[cache_key] = u_match
        if u_match is None:
            return False
        v_match = {rule.pattern.item_center: {v}}
        if rule.pattern.item_paths:
            v_match = _match_star_paths(self.graph, rule.pattern, v, rule.pattern.item_paths, pw_preds, data['item_edge_lookups'], data['prefilter_cache'], compact_graph=self.compact_graph, star_match_ctx=data['item_star_ctx'])
        if v_match is None:
            return False
        compact_match = {}
        for var, cands in u_match.items():
            compact_match[var] = cands
        for var, cands in v_match.items():
            if var in compact_match:
                compact_match[var] = compact_match[var] & cands
                if not compact_match[var]:
                    return False
            else:
                compact_match[var] = cands
        if data['pair_wise']:
            if not verify_variable_predicates(self.graph, compact_match, data['pair_wise']):
                return False
        return True

def _get_anchor_candidate_rule_indices(tie_rules: List[TIERule], inv_index: TIEInvertedIndex, v_node: Any, mrec_score: float, applicable_rule_indices: Optional[Set[int]]=None) -> List[int]:
    candidate_rule_indices = inv_index.get_candidate_rules(v_node)
    if not candidate_rule_indices:
        return []
    valid_indices = []
    for rule_idx in candidate_rule_indices:
        if applicable_rule_indices is not None and rule_idx not in applicable_rule_indices:
            continue
        ml_preds = tie_rules[rule_idx].get_ml_predicates()
        if ml_preds and (not all((pred.evaluate(mrec_score) for pred in ml_preds))):
            continue
        valid_indices.append(rule_idx)
    return valid_indices

def _init_tie_window_candidate_map(graph: Graph, user_id: Any, joint_pattern: JointPattern, window_items: List[Any], x0_var: str, y0_var: str) -> Dict[str, Set[Any]]:
    candidate_map: Dict[str, Set[Any]] = {}
    for var, pnode in joint_pattern.nodes.items():
        if var == x0_var:
            candidate_map[var] = {user_id}
        elif var == y0_var:
            candidate_map[var] = set(window_items)
        else:
            candidate_map[var] = set(graph.get_node_ids_by_label(pnode.label))
    degree_requirements: Dict[str, int] = defaultdict(int)
    for edge in joint_pattern.edges:
        degree_requirements[edge.source_var] += 1
        degree_requirements[edge.target_var] += 1
    degree_cache: Dict[Any, int] = {}

    def _node_degree(node_id: Any) -> int:
        if node_id not in degree_cache:
            degree_cache[node_id] = len(graph.get_out_edges(node_id)) + len(graph.get_in_edges(node_id))
        return degree_cache[node_id]
    for var, min_degree in degree_requirements.items():
        if min_degree <= 0 or var not in candidate_map:
            continue
        candidate_map[var] = {nid for nid in candidate_map[var] if _node_degree(nid) >= min_degree}
    return candidate_map

def _candidate_map_signature(candidate_map: Dict[str, Set[Any]]) -> Tuple[Tuple[str, frozenset[Any]], ...]:
    return tuple(((var, frozenset(candidate_map.get(var, set()))) for var in sorted(candidate_map)))

def _validate_anchor_window_batched(graph: Graph, user_id: Any, window_items: List[Any], candidate_rule_indices: Dict[Any, List[int]], tie_rules: List[TIERule], runtime_cache: Dict[int, Dict[str, Any]], compact_graph: Any, group_cache: Optional[Dict[Tuple[int, ...], Tuple[JointPattern, RootedDAG]]]=None, offline_index: Optional[OfflineIndex]=None, max_iterations_cap: Optional[int]=None, parallel_stats: Optional[Dict[str, Any]]=None) -> Tuple[List[Any], Set[int]]:
    sigma_s = sorted({rule_idx for v in window_items for rule_idx in candidate_rule_indices.get(v, [])})
    if not sigma_s:
        return ([], set())
    selected_rules = [tie_rules[rule_idx] for rule_idx in sigma_s]
    x0_var = selected_rules[0].pattern.user_center
    y0_var = selected_rules[0].pattern.item_center
    sigma_key = tuple(sigma_s)
    if group_cache is not None and sigma_key in group_cache:
        joint_pattern, rooted_dag = group_cache[sigma_key]
    else:
        joint_pattern = JointPattern.from_rule_group(selected_rules)
        rooted_dag = RootedDAG.from_joint_pattern(joint_pattern, x0_var, graph)
        if group_cache is not None:
            group_cache[sigma_key] = (joint_pattern, rooted_dag)
    F_H = _init_tie_window_candidate_map(graph=graph, user_id=user_id, joint_pattern=joint_pattern, window_items=window_items, x0_var=x0_var, y0_var=y0_var)
    if not F_H.get(x0_var) or not F_H.get(y0_var):
        return ([], set())
    rule_local_y0: Dict[int, Set[Any]] = {}
    for local_idx, orig_rule_idx in enumerate(sigma_s):
        allowed = {v for v in window_items if orig_rule_idx in candidate_rule_indices.get(v, [])}
        if allowed:
            rule_local_y0[local_idx] = allowed
    prev_signature = None
    max_iterations = max(2, len(joint_pattern.nodes))
    if max_iterations_cap is not None:
        max_iterations = max(1, min(max_iterations, int(max_iterations_cap)))
    for _ in range(max_iterations):
        local_results: Dict[int, Dict[str, Set[Any]]] = {}
        iteration_rule_totals: Dict[int, float] = {}
        for local_idx, orig_rule_idx in enumerate(sigma_s):
            _t_rule = time.perf_counter()
            rule = selected_rules[local_idx]
            y0_allowed = F_H.get(y0_var, set()) & rule_local_y0.get(local_idx, set())
            if not y0_allowed:
                iteration_rule_totals[orig_rule_idx] = iteration_rule_totals.get(orig_rule_idx, 0.0) + (time.perf_counter() - _t_rule)
                continue
            candidate_map_rule = {var: set(cands) for var, cands in F_H.items()}
            candidate_map_rule[x0_var] = {user_id}
            candidate_map_rule[y0_var] = y0_allowed
            cache = runtime_cache[orig_rule_idx]
            if offline_index is not None:
                offline_item_cache = offline_index.item_star.get(orig_rule_idx, {})
                if offline_item_cache:
                    item_star_cache = cache['item_star_cache']
                    for v in y0_allowed:
                        if v in item_star_cache:
                            continue
                        offline_match = offline_item_cache.get(v)
                        if offline_match is not None:
                            item_star_cache[v] = offline_match
            F_phi = local_refinement(graph=graph, candidate_map=candidate_map_rule, rooted_dag=rooted_dag, rule=rule, point_wise_preds=rule.get_point_wise_predicates(), compact_graph=compact_graph, star_cache_user=cache['user_star_cache'], star_cache_item=cache['item_star_cache'], prefilter_cache=cache['prefilter_cache'], edge_lookups=cache['edge_lookups'], star_contexts=cache.get('star_contexts'))
            if F_phi is not None:
                local_results[local_idx] = F_phi
            iteration_rule_totals[orig_rule_idx] = iteration_rule_totals.get(orig_rule_idx, 0.0) + (time.perf_counter() - _t_rule)
        if parallel_stats is not None and iteration_rule_totals:
            parallel_stats.setdefault('anchor_select_window_rule_total_by_iteration', []).append({rule_idx: float(elapsed) for rule_idx, elapsed in iteration_rule_totals.items()})
        if not local_results:
            return ([], set())
        new_F_H = group_refinement(local_results)
        for var in F_H:
            new_F_H.setdefault(var, set())
        signature = _candidate_map_signature(new_F_H)
        F_H = new_F_H
        if signature == prev_signature:
            break
        prev_signature = signature
    valid_items = [v for v in window_items if v in F_H.get(y0_var, set())]
    sigma_prime_local: Set[int] = set()
    for v in valid_items:
        sigma_prime_local.update(candidate_rule_indices.get(v, []))
    return (valid_items, sigma_prime_local)

def _user_applicable_tie_rule_indices(u_node: Any, tie_rules: List[TIERule], parallel_stats: Optional[Dict[str, Any]]=None) -> List[int]:
    applicable_indices: List[int] = []
    for orig_idx, rule in enumerate(tie_rules):
        _t_rule = time.perf_counter()
        pw_preds = rule.get_point_wise_predicates().get(rule.pattern.user_center, [])
        if all((p.evaluate(u_node) for p in pw_preds)):
            applicable_indices.append(orig_idx)
        _record_parallel_bucket(parallel_stats, 'anchor_select_user_rule_filter_by_rule', orig_idx, time.perf_counter() - _t_rule)
    return applicable_indices

def anchor_select(graph: Graph, user_id: Any, candidate_items: Set[Any], tie_rules: List[TIERule], ml_cache: MLPredicateCache, delta_L: float, delta_H: float, k: int, offline_index: Optional[OfflineIndex]=None, prepared_runtime: Optional[Dict[str, Any]]=None, max_window_iterations: Optional[int]=None, parallel_stats: Optional[Dict[str, Any]]=None, detail_stats: Optional[Dict[str, float]]=None) -> Tuple[List[Any], Set[int]]:
    t_start = time.perf_counter()

    def _record_detail(name: str, started_at: float) -> None:
        if detail_stats is not None:
            detail_stats[name] = detail_stats.get(name, 0.0) + (time.perf_counter() - started_at)
    user_ml_cache = ml_cache.get_user_view(user_id) if hasattr(ml_cache, 'get_user_view') else ml_cache
    u_node = graph.get_node(user_id)
    if not u_node:
        logger.warning(f'[anchor_select] User {user_id} not found in graph.')
        return ([], set())
    _t_detail = time.perf_counter()
    applicable_rule_indices = _user_applicable_tie_rule_indices(u_node, tie_rules, parallel_stats=parallel_stats)
    _record_detail('user_rule_filter', _t_detail)
    if not applicable_rule_indices:
        return ([], set())
    applicable_rule_index_set = set(applicable_rule_indices)
    compact_graph = getattr(graph, '_compact', None)
    if prepared_runtime is not None:
        inv_index = prepared_runtime['inv_index']
        rule_templates = prepared_runtime['rule_templates']
        window_group_cache = prepared_runtime.setdefault('group_cache', {})
    else:
        inv_index = TIEInvertedIndex(graph, tie_rules)
        rule_templates = {rule_idx: _build_tie_runtime_template(graph, rule, compact_graph) for rule_idx, rule in enumerate(tie_rules)}
        window_group_cache = {}
    rule_runtime_cache: Dict[int, Dict[str, Any]] = {}
    for rule_idx in applicable_rule_indices:
        template = rule_templates[rule_idx]
        rule_runtime_cache[rule_idx] = {'user_star_cache': {}, 'item_star_cache': {}, 'prefilter_cache': template['prefilter_cache'], 'edge_lookups': template['edge_lookups'], 'star_contexts': template['star_contexts']}
    sigma_prime_tie: Set[int] = set()
    item_nodes: Dict[Any, Any] = {}
    _t_detail = time.perf_counter()
    item_scores, dense_covered = _dense_item_scores(user_id=user_id, candidate_items=set(candidate_items), ml_cache=ml_cache, prepared_runtime=prepared_runtime, min_score=delta_L, parallel_stats=parallel_stats)
    _record_detail('dense_score_lookup', _t_detail)
    if item_scores is None:
        item_scores = []
    filtered_item_scores: List[Tuple[float, Any]] = []
    _t_detail = time.perf_counter()
    for score, v in item_scores:
        _t_item = time.perf_counter()
        v_node = graph.get_node(v)
        if v_node is None:
            _record_parallel_bucket(parallel_stats, 'anchor_select_dense_materialize_by_item', v, time.perf_counter() - _t_item)
            continue
        item_nodes[v] = v_node
        filtered_item_scores.append((score, v))
        _record_parallel_bucket(parallel_stats, 'anchor_select_dense_materialize_by_item', v, time.perf_counter() - _t_item)
    _record_detail('dense_item_materialize', _t_detail)
    _t_detail = time.perf_counter()
    for v in set(candidate_items) - dense_covered:
        _t_item = time.perf_counter()
        score = user_ml_cache.get_prediction(user_id, v)
        if score is None or score < delta_L:
            _record_parallel_bucket(parallel_stats, 'anchor_select_sparse_score_by_item', v, time.perf_counter() - _t_item)
            continue
        v_node = graph.get_node(v)
        if v_node is None:
            _record_parallel_bucket(parallel_stats, 'anchor_select_sparse_score_by_item', v, time.perf_counter() - _t_item)
            continue
        item_nodes[v] = v_node
        filtered_item_scores.append((score, v))
        _record_parallel_bucket(parallel_stats, 'anchor_select_sparse_score_by_item', v, time.perf_counter() - _t_item)
    _record_detail('sparse_item_score_lookup', _t_detail)
    _t_detail = time.perf_counter()
    item_scores = filtered_item_scores
    item_scores.sort(key=lambda x: x[0], reverse=True)
    _record_detail('sort_scored_items', _t_detail)
    K_u: List[Any] = []
    candidate_rule_cache: Dict[Any, List[int]] = {}

    def _log_selection_summary(elapsed: float) -> None:
        logger.debug(f'[anchor_select] User {user_id}: Selected {len(K_u)} anchors out of {len(candidate_items)} candidates in {elapsed:.4f}s.')

    def _candidate_rules_for_item(v: Any, score: float) -> List[int]:
        if v not in candidate_rule_cache:
            _t_item = time.perf_counter()
            candidate_rule_cache[v] = _get_anchor_candidate_rule_indices(tie_rules, inv_index, item_nodes[v], score, applicable_rule_indices=applicable_rule_index_set)
            _record_parallel_bucket(parallel_stats, 'anchor_select_candidate_rule_by_item', v, time.perf_counter() - _t_item)
        return candidate_rule_cache[v]
    _t_detail = time.perf_counter()
    i_b = len(item_scores)
    for idx, (score, v) in enumerate(item_scores):
        if len(K_u) >= k:
            break
        if score >= delta_H:
            K_u.append(v)
            for rule_idx in _candidate_rules_for_item(v, score):
                sigma_prime_tie.add(rule_idx)
            i_b = idx + 1
            continue
        i_b = idx
        break
    _record_detail('high_conf_prefix_scan', _t_detail)
    if len(K_u) >= k or i_b >= len(item_scores):
        elapsed = time.perf_counter() - t_start
        _log_selection_summary(elapsed)
        return (K_u[:k], sigma_prime_tie)
    fuzzy_end = i_b
    while fuzzy_end < len(item_scores) and item_scores[fuzzy_end][0] >= delta_L:
        fuzzy_end += 1
    if i_b >= fuzzy_end:
        elapsed = time.perf_counter() - t_start
        _log_selection_summary(elapsed)
        return (K_u[:k], sigma_prime_tie)
    i_e = i_b + (k - len(K_u)) - 1
    while len(K_u) < k and i_b < fuzzy_end:
        actual_end = min(i_e, fuzzy_end - 1)
        window = item_scores[i_b:actual_end + 1]
        if not window:
            break
        window_items = [v for _, v in window]
        window_rule_indices: Dict[Any, List[int]] = {}
        _t_detail = time.perf_counter()
        for score, v in window:
            window_rule_indices[v] = _candidate_rules_for_item(v, score)
        _record_detail('window_rule_prefilter', _t_detail)
        _t_detail = time.perf_counter()
        valid_items, _ = _validate_anchor_window_batched(graph=graph, user_id=user_id, window_items=window_items, candidate_rule_indices=window_rule_indices, tie_rules=tie_rules, runtime_cache=rule_runtime_cache, compact_graph=compact_graph, group_cache=window_group_cache, offline_index=offline_index, max_iterations_cap=max_window_iterations, parallel_stats=parallel_stats)
        _record_detail('window_batch_validate', _t_detail)
        remaining = k - len(K_u)
        selected_items = valid_items[:remaining]
        K_u.extend(selected_items)
        for v in selected_items:
            for rule_idx in window_rule_indices.get(v, []):
                sigma_prime_tie.add(rule_idx)
        i_b = actual_end + 1
        remaining = k - len(K_u)
        if remaining <= 0:
            break
        i_e = i_b + remaining - 1
    elapsed = time.perf_counter() - t_start
    _log_selection_summary(elapsed)
    return (K_u, sigma_prime_tie)

def _relevant_tie_rules_for_selected_anchors(graph: Graph, user_id: Any, selected_anchors: List[Any], tie_rules: List[TIERule], ml_cache: MLPredicateCache, prepared_runtime: Optional[Dict[str, Any]]=None) -> Set[int]:
    anchor_rule_map = _selected_anchor_rule_indices(graph=graph, user_id=user_id, selected_anchors=selected_anchors, tie_rules=tie_rules, ml_cache=ml_cache, prepared_runtime=prepared_runtime)
    sigma_prime_tie: Set[int] = set()
    for rule_indices in anchor_rule_map.values():
        sigma_prime_tie.update(rule_indices)
    return sigma_prime_tie

def _selected_anchor_rule_indices(graph: Graph, user_id: Any, selected_anchors: List[Any], tie_rules: List[TIERule], ml_cache: MLPredicateCache, prepared_runtime: Optional[Dict[str, Any]]=None, parallel_stats: Optional[Dict[str, Any]]=None) -> Dict[Any, Set[int]]:
    u_node = graph.get_node(user_id)
    if not u_node or not selected_anchors:
        return {}
    applicable_rule_indices = _user_applicable_tie_rule_indices(u_node, tie_rules)
    if not applicable_rule_indices:
        return {}
    applicable_rule_index_set = set(applicable_rule_indices)
    user_ml_cache = ml_cache.get_user_view(user_id) if hasattr(ml_cache, 'get_user_view') else ml_cache
    inv_index = prepared_runtime['inv_index'] if prepared_runtime is not None else TIEInvertedIndex(graph, tie_rules)
    anchor_rule_map: Dict[Any, Set[int]] = {}
    for v in selected_anchors:
        _t_anchor = time.perf_counter()
        v_node = graph.get_node(v)
        if v_node is None:
            _record_parallel_bucket(parallel_stats, 'anchor_rule_index_by_anchor', v, time.perf_counter() - _t_anchor)
            continue
        score = user_ml_cache.get_prediction(user_id, v)
        if score is None:
            score = 0.0
        rule_indices = _get_anchor_candidate_rule_indices(tie_rules, inv_index, v_node, score, applicable_rule_indices=applicable_rule_index_set)
        if rule_indices:
            anchor_rule_map[v] = set(rule_indices)
        _record_parallel_bucket(parallel_stats, 'anchor_rule_index_by_anchor', v, time.perf_counter() - _t_anchor)
    return anchor_rule_map

def _prime_group_offline_caches(offline_index: Optional[OfflineIndex], H: List[Any], tie_idx: int, candidate_map: Dict[str, Set[Any]], y0_var: str) -> Tuple[Dict[Any, Dict[str, Set[Any]]], Dict[int, Dict[Any, Dict[str, Set[Any]]]], Dict[int, Any], Dict[int, Dict[Any, frozenset]]]:
    star_cache_item: Dict[Any, Dict[str, Set[Any]]] = {}
    star_cache_alt_per_rule: Dict[int, Dict[Any, Dict[str, Set[Any]]]] = {}
    bridge_idx_per_rule: Dict[int, Any] = {}
    dis_pass_per_rule: Dict[int, Dict[Any, frozenset]] = {}
    if offline_index is None:
        return (star_cache_item, star_cache_alt_per_rule, bridge_idx_per_rule, dis_pass_per_rule)
    offline_item_cache = offline_index.item_star.get(tie_idx, {})
    for v in candidate_map.get(y0_var, set()):
        if v in offline_item_cache and offline_item_cache[v] is not None:
            star_cache_item[v] = offline_item_cache[v]
    for rule_idx, rule in enumerate(H):
        if not isinstance(rule, OMRule):
            continue
        omr_global_idx = offline_index._omr_id_to_idx.get(id(rule))
        if omr_global_idx is None:
            continue
        bridge_idx = offline_index.bridge_idx.get(omr_global_idx)
        if bridge_idx is not None:
            bridge_idx_per_rule[rule_idx] = bridge_idx
        offline_dis_pass = offline_index.dis_pass.get(omr_global_idx)
        if offline_dis_pass:
            dis_pass_per_rule[rule_idx] = offline_dis_pass
        alt_cache = offline_index.alt_star.get(omr_global_idx, {})
        if not alt_cache:
            continue
        w0_var = rule.pattern.alt_center
        seeded_alt_cache = {}
        for w in candidate_map.get(w0_var, set()):
            if w in alt_cache and alt_cache[w] is not None:
                seeded_alt_cache[w] = alt_cache[w]
        if seeded_alt_cache:
            star_cache_alt_per_rule[rule_idx] = seeded_alt_cache
    return (star_cache_item, star_cache_alt_per_rule, bridge_idx_per_rule, dis_pass_per_rule)

def compute_rule_upper_bound(max_dis_v: float, delta_max_phi: float, lambda_param: float) -> float:
    return lambda_param * max_dis_v * delta_max_phi

def _omr_matches_any_selected_anchor(graph: Graph, user_id: Any, anchor_items: Set[Any], rule: OMRule, ml_cache: Optional[MLPredicateCache]) -> bool:
    if not anchor_items:
        return False
    pat = rule.pattern
    x0 = pat.user_center
    y0 = pat.anchor_center
    y0_node = pat.nodes.get(y0)
    y0_preds = rule.get_point_wise_predicates().get(y0, [])
    anchor_ml_preds = [pred for pred in rule.get_ml_predicates() if pred.user_var == x0 and pred.item_var == y0]
    min_y0_degree = sum((1 for edge in pat.edges if edge.source_var == y0 or edge.target_var == y0))
    for v in anchor_items:
        vnode = graph.get_node(v)
        if vnode is None:
            continue
        if y0_node is not None and y0_node.label and (vnode.label != y0_node.label):
            continue
        if min_y0_degree > 0:
            degree = len(graph.get_out_edges(v)) + len(graph.get_in_edges(v))
            if degree < min_y0_degree:
                continue
        if y0_preds and (not all((pred.evaluate(vnode) for pred in y0_preds))):
            continue
        if anchor_ml_preds and ml_cache is not None:
            score = ml_cache.get_prediction(user_id, v)
            if not all((pred.evaluate(score) for pred in anchor_ml_preds)):
                continue
        return True
    return False

def _active_topk_omrs_by_tie_index(graph: Graph, user_id: Any, selected_tie_indices: Iterable[int], tie_rules: List[TIERule], omr_rules: List[OMRule], anchor_items: Set[Any], ml_cache: Optional[MLPredicateCache], parallel_stats: Optional[Dict[str, Any]]=None) -> Tuple[Dict[int, List[OMRule]], int]:
    if not selected_tie_indices or not omr_rules or (not anchor_items):
        return ({}, 0)
    user_node = graph.get_node(user_id)
    if user_node is None:
        return ({}, 0)
    parent_registry = _get_omr_children_by_parent(omr_rules)
    by_tie_idx: Dict[int, List[OMRule]] = {}
    active_omr_ids: Set[int] = set()
    for tie_idx in selected_tie_indices:
        _t_tie = time.perf_counter()
        tie_rule = tie_rules[tie_idx]
        children = list(parent_registry.get(('id', id(tie_rule)), []))
        if not children:
            children = list(parent_registry.get(('sig', canonical_tie_rule_signature(tie_rule)), []))
        if not children:
            _record_parallel_bucket(parallel_stats, 'active_omr_index_by_tie', tie_idx, time.perf_counter() - _t_tie)
            continue
        active_children: List[OMRule] = []
        for omr in children:
            x0_var = omr.pattern.user_center
            x0_node = omr.pattern.nodes.get(x0_var)
            if x0_node is not None and x0_node.label and (user_node.label != x0_node.label):
                continue
            x0_preds = omr.get_point_wise_predicates().get(x0_var, [])
            if x0_preds and (not all((pred.evaluate(user_node) for pred in x0_preds))):
                continue
            required_x0 = _get_required_edge_groups_cached(omr.pattern).get(x0_var, set())
            if required_x0 and user_id not in _nodes_with_required_edge_groups(graph, {user_id}, required_x0):
                continue
            if not _omr_matches_any_selected_anchor(graph=graph, user_id=user_id, anchor_items=anchor_items, rule=omr, ml_cache=ml_cache):
                continue
            active_children.append(omr)
            active_omr_ids.add(id(omr))
        if active_children:
            by_tie_idx[tie_idx] = active_children
        _record_parallel_bucket(parallel_stats, 'active_omr_index_by_tie', tie_idx, time.perf_counter() - _t_tie)
    return (by_tie_idx, len(active_omr_ids))

class _RenamedBridgeReachabilityIndex:

    def __init__(self, base_index: BridgeReachabilityIndex, var_map: Dict[str, str]):
        self._base = base_index
        self._var_map = dict(var_map)

    def get_reachable(self, origin_id: Any) -> frozenset:
        return self._base.get_reachable(origin_id)

    def get_bvc(self, origin_id: Any) -> Dict[str, Set[Any]]:
        renamed: Dict[str, Set[Any]] = {}
        for var, cands in self._base.get_bvc(origin_id).items():
            target_var = self._var_map.get(var, var)
            if target_var not in renamed:
                renamed[target_var] = set(cands)
            else:
                renamed[target_var] |= set(cands)
        return renamed

def _rename_cached_match_vars(cached_match: Dict[str, Set[Any]], var_map: Dict[str, str]) -> Dict[str, Set[Any]]:
    renamed: Dict[str, Set[Any]] = {}
    for var, cands in cached_match.items():
        target_var = var_map.get(var, var)
        if target_var not in renamed:
            renamed[target_var] = set(cands)
        else:
            renamed[target_var] |= set(cands)
    return renamed

def _var_map_cache_key(var_map: Dict[str, str]) -> Tuple[Tuple[str, str], ...]:
    return tuple(sorted(var_map.items()))

def _get_renamed_bridge_index_cached(base_index: BridgeReachabilityIndex, var_map: Dict[str, str]) -> '_RenamedBridgeReachabilityIndex':
    cache_key = (id(base_index), _var_map_cache_key(var_map))
    cached = _RENAMED_BRIDGE_INDEX_CACHE.get(cache_key)
    if cached is not None:
        return cached
    renamed = _RenamedBridgeReachabilityIndex(base_index, var_map)
    _RENAMED_BRIDGE_INDEX_CACHE[cache_key] = renamed
    return renamed

def _get_renamed_offline_alt_cache_cached(offline_index: OfflineIndex, omr_global_idx: int, var_map: Dict[str, str]) -> Dict[Any, Dict[str, Set[Any]]]:
    cache_key = (id(offline_index), int(omr_global_idx), _var_map_cache_key(var_map))
    cached = _RENAMED_OFFLINE_ALT_CACHE.get(cache_key)
    if cached is not None:
        return cached
    base_alt_cache = offline_index.alt_star.get(omr_global_idx, {})
    renamed_alt_cache: Dict[Any, Dict[str, Set[Any]]] = {}
    for w, cached_match in base_alt_cache.items():
        if cached_match is None:
            continue
        renamed_alt_cache[w] = _rename_cached_match_vars(cached_match, var_map)
    _RENAMED_OFFLINE_ALT_CACHE[cache_key] = renamed_alt_cache
    return renamed_alt_cache

def _topk_global_pivot_vars(tie_rules: List[TIERule], omr_rules: Optional[List[OMRule]]) -> Tuple[str, str, str]:
    x0_var = tie_rules[0].pattern.user_center if tie_rules else 'x0'
    y0_var = tie_rules[0].pattern.item_center if tie_rules else 'y0'
    first_omr = next((rule for rule in omr_rules or [] if rule is not None), None)
    w0_var = first_omr.pattern.alt_center if first_omr is not None else 'w0'
    return (x0_var, y0_var, w0_var)

def _build_stable_topk_tie_var_map(tie_rule: TIERule, tie_idx: int, *, x0_var: str, y0_var: str) -> Dict[str, str]:
    tie_var_map = {tie_rule.pattern.user_center: x0_var, tie_rule.pattern.item_center: y0_var}
    for var in tie_rule.pattern.nodes:
        if var not in tie_var_map:
            tie_var_map[var] = f'tie{tie_idx}__{var}'
    return tie_var_map

def _build_stable_topk_omr_var_map(omr_rule: OMRule, tie_idx: int, omr_slot: int, tie_var_map: Dict[str, str], *, x0_var: str, y0_var: str, w0_var: str) -> Dict[str, str]:
    omr_var_map = dict(tie_var_map)
    omr_var_map[omr_rule.pattern.user_center] = x0_var
    omr_var_map[omr_rule.pattern.anchor_center] = y0_var
    omr_var_map[omr_rule.pattern.alt_center] = w0_var
    for var in omr_rule.pattern.nodes:
        if var not in omr_var_map:
            omr_var_map[var] = f'tie{tie_idx}_omr{omr_slot}__{var}'
    return omr_var_map

def _prepare_topk_offline_views(*, offline_index: Optional[OfflineIndex], tie_rules: List[TIERule], omr_rules: Optional[List[OMRule]]) -> None:
    if offline_index is None or not tie_rules or (not omr_rules):
        return
    if getattr(offline_index, '_topk_offline_views_prepared', False):
        return
    x0_var, y0_var, w0_var = _topk_global_pivot_vars(tie_rules, omr_rules)
    parent_registry = _get_omr_children_by_parent(omr_rules)
    omr_global_idx_by_id = {id(rule): idx for idx, rule in enumerate(omr_rules)}
    for tie_idx, tie_rule in enumerate(tie_rules):
        children = list(parent_registry.get(('id', id(tie_rule)), []))
        if not children:
            children = list(parent_registry.get(('sig', canonical_tie_rule_signature(tie_rule)), []))
        if not children:
            continue
        tie_var_map = _build_stable_topk_tie_var_map(tie_rule, tie_idx, x0_var=x0_var, y0_var=y0_var)
        for omr in children:
            omr_global_idx = omr_global_idx_by_id.get(id(omr))
            if omr_global_idx is None:
                continue
            omr_var_map = _build_stable_topk_omr_var_map(omr, tie_idx, omr_global_idx, tie_var_map, x0_var=x0_var, y0_var=y0_var, w0_var=w0_var)
            bridge_idx = offline_index.bridge_idx.get(omr_global_idx)
            if bridge_idx is not None:
                _get_renamed_bridge_index_cached(bridge_idx, omr_var_map)
            cache_key = (id(offline_index), int(omr_global_idx), _var_map_cache_key(omr_var_map))
            if cache_key in _RENAMED_OFFLINE_ALT_CACHE:
                continue
            base_alt_cache = offline_index.alt_star.get(omr_global_idx, {})
            renamed_alt_cache: Dict[Any, Dict[str, Set[Any]]] = {}
            for w, cached_match in base_alt_cache.items():
                if cached_match is None:
                    continue
                renamed_alt_cache[w] = _rename_cached_match_vars(cached_match, omr_var_map)
            _RENAMED_OFFLINE_ALT_CACHE[cache_key] = renamed_alt_cache
    setattr(offline_index, '_topk_offline_views_prepared', True)

def _get_or_build_topk_group_template(*, tie_idx: int, original_tie: TIERule, original_omrs: List[OMRule], omr_global_idx_by_id: Dict[int, int], x0_var: str, y0_var: str, w0_var: str, timing_out: Optional[Dict[str, float]]=None, cache_stats_out: Optional[Dict[str, int]]=None) -> Dict[str, Any]:
    omr_slots = tuple((int(omr_global_idx_by_id.get(id(omr), local_pos)) for local_pos, omr in enumerate(original_omrs)))
    cache_key = (id(original_tie), int(tie_idx), omr_slots, x0_var, y0_var, w0_var)
    cached = _TOPK_GROUP_TEMPLATE_CACHE.get(cache_key)
    if cached is not None:
        _increment_counter(cache_stats_out, 'template_hits')
        return cached
    _increment_counter(cache_stats_out, 'template_misses')
    _t_phase = time.perf_counter()
    tie_var_map = _build_stable_topk_tie_var_map(original_tie, tie_idx, x0_var=x0_var, y0_var=y0_var)
    cloned_tie = _clone_tie_rule_with_var_map(original_tie, tie_var_map)
    cloned_omrs: List[OMRule] = []
    omr_var_map_by_omr_id: Dict[int, Dict[str, str]] = {}
    for omr_slot, original_omr in zip(omr_slots, original_omrs):
        omr_var_map = _build_stable_topk_omr_var_map(original_omr, tie_idx, int(omr_slot), tie_var_map, x0_var=x0_var, y0_var=y0_var, w0_var=w0_var)
        cloned_omrs.append(_clone_omr_rule_with_var_map(original_omr, cloned_parent_tie=cloned_tie, var_map=omr_var_map))
        omr_var_map_by_omr_id[id(original_omr)] = dict(omr_var_map)
    template = {'cloned_tie': cloned_tie, 'cloned_omrs': tuple(cloned_omrs), 'tie_var_map': dict(tie_var_map), 'omr_var_map_by_omr_id': omr_var_map_by_omr_id, 'omr_slots': omr_slots}
    _record_timing(timing_out, 'clone_rules', _t_phase)
    _TOPK_GROUP_TEMPLATE_CACHE[cache_key] = template
    return template

def _gamma_max_from_scorer(scorer: RankingScoreComputer, v: Any, ws: Iterable[Any]) -> float:
    ws_list = list(ws)
    if not ws_list:
        return 0.0
    if len(ws_list) < 16:
        return max((scorer._get_sim_t(v, w) * scorer._get_mdis(v, w) for w in ws_list), default=0.0)
    ws_list, gamma_scores = scorer._get_gamma_batch(v, ws_list)
    if not ws_list or gamma_scores.size == 0:
        return 0.0
    return float(np.max(gamma_scores))

def _global_gamma_upper_bound(scorer: RankingScoreComputer, v: Any, item_universe: Iterable[Any]) -> float:
    universe_id = id(item_universe)
    cache_key = (id(scorer.graph), id(scorer.dis_model), id(scorer.topic_model) if scorer.topic_model is not None else 0, universe_id, v)
    cached = _TOPK_GAMMA_GLOBAL_UPPER_CACHE.get(cache_key)
    if cached is not None:
        return float(cached)
    ws = [w for w in item_universe if w != v]
    if not ws:
        _TOPK_GAMMA_GLOBAL_UPPER_CACHE[cache_key] = 0.0
        return 0.0
    gamma = _gamma_max_from_scorer(scorer, v, ws)
    _TOPK_GAMMA_GLOBAL_UPPER_CACHE[cache_key] = float(gamma)
    return float(gamma)

def _clone_path_with_var_map(path: Path, var_map: Dict[str, str]) -> Path:
    cloned = Path([(var_map.get(var, var), path.nodes[var].label, set(path.nodes[var].attributes) if path.nodes[var].attributes else set()) for var in path.path_sequence])
    for edge in path.edges:
        cloned.add_path_edge(var_map.get(edge.source_var, edge.source_var), var_map.get(edge.target_var, edge.target_var), edge.label, is_forward=edge.is_forward)
    return cloned

def _rename_predicate_vars(pred: Any, var_map: Dict[str, str]) -> Any:
    if isinstance(pred, PointWisePredicate):
        return PointWisePredicate(var_map.get(pred.variable, pred.variable), pred.attribute, pred.operator, pred.constant)
    if isinstance(pred, PairWisePredicate):
        return PairWisePredicate(var_map.get(pred.var1, pred.var1), pred.attr1, pred.operator, var_map.get(pred.var2, pred.var2), pred.attr2)
    if isinstance(pred, MLPredicate):
        return MLPredicate(var_map.get(pred.user_var, pred.user_var), var_map.get(pred.item_var, pred.item_var), pred.operator, pred.threshold)
    if isinstance(pred, RecPredicate):
        return RecPredicate(var_map.get(pred.user_var, pred.user_var), var_map.get(pred.item_var, pred.item_var))
    if isinstance(pred, DisPredicate):
        return DisPredicate(var_map.get(pred.anchor_var, pred.anchor_var), var_map.get(pred.alt_var, pred.alt_var), pred.operator, pred.threshold)
    if isinstance(pred, AggrPredicate):
        return AggrPredicate(var_map.get(pred.item_var, pred.item_var), pred.operator, pred.threshold)
    raise TypeError(f'Unsupported predicate type for TopK cloning: {type(pred)!r}')

def _clone_edge_predicate(conclusion: Optional[EdgePredicate], var_map: Dict[str, str]) -> Optional[EdgePredicate]:
    if conclusion is None:
        return None
    return EdgePredicate(source_var=var_map.get(conclusion.source_var, conclusion.source_var), target_var=var_map.get(conclusion.target_var, conclusion.target_var), edge_label=conclusion.edge_label)

def _clone_tie_rule_with_var_map(rule: TIERule, var_map: Dict[str, str]) -> TIERule:
    pattern = rule.pattern
    cloned_pattern = DualStarPattern(user_center=var_map.get(pattern.user_center, pattern.user_center), user_label=pattern.nodes[pattern.user_center].label, item_center=var_map.get(pattern.item_center, pattern.item_center), item_label=pattern.nodes[pattern.item_center].label, user_attributes=set(pattern.nodes[pattern.user_center].attributes), item_attributes=set(pattern.nodes[pattern.item_center].attributes))
    for user_path in pattern.user_paths:
        cloned_pattern.add_user_path(_clone_path_with_var_map(user_path, var_map))
    for item_path in pattern.item_paths:
        cloned_pattern.add_item_path(_clone_path_with_var_map(item_path, var_map))
    cloned_rule = TIERule(pattern=cloned_pattern, preconditions=[_rename_predicate_vars(pred, var_map) for pred in rule.preconditions], conclusion=_clone_edge_predicate(rule.conclusion, var_map))
    try:
        setattr(cloned_rule, '_cache_identity', ('topk_tie_clone', id(rule), tuple(sorted(var_map.items()))))
        setattr(cloned_rule, '_source_rule_id', id(rule))
        setattr(cloned_rule, '_source_rule_ref', rule)
        setattr(cloned_rule, '_source_var_map', dict(var_map))
    except Exception:
        pass
    return cloned_rule

def _clone_omr_rule_with_var_map(rule: OMRule, cloned_parent_tie: TIERule, var_map: Dict[str, str]) -> OMRule:
    pattern = rule.pattern
    cloned_pattern = OMRPattern(user_center=var_map.get(pattern.user_center, pattern.user_center), anchor_center=var_map.get(pattern.anchor_center, pattern.anchor_center), alt_center=var_map.get(pattern.alt_center, pattern.alt_center), alt_center_label=pattern.alt_center_label, tie_pattern=cloned_parent_tie.pattern)
    for bridge_path, origin in zip(pattern.bridge_paths, pattern.bridge_origins):
        cloned_pattern.add_bridge_path(_clone_path_with_var_map(bridge_path, var_map), var_map.get(origin, origin))
    for alt_path in getattr(pattern.alt_star, 'paths', ()):
        cloned_pattern.add_alt_path(_clone_path_with_var_map(alt_path, var_map))
    cloned_rule = OMRule(pattern=cloned_pattern, parent_tie=cloned_parent_tie, preconditions=[_rename_predicate_vars(pred, var_map) for pred in rule.preconditions], conclusion=_clone_edge_predicate(rule.conclusion, var_map))
    try:
        setattr(cloned_rule, '_cache_identity', ('topk_omr_clone', id(rule), tuple(sorted(var_map.items()))))
        setattr(cloned_rule, '_source_rule_id', id(rule))
        setattr(cloned_rule, '_source_rule_ref', rule)
        setattr(cloned_rule, '_source_var_map', dict(var_map))
    except Exception:
        pass
    return cloned_rule

def _build_topk_group_tie_worker(*, graph: Graph, user_id: Any, tie_idx: int, anchor_items: List[Any], candidate_items: Set[Any], tie_rules: List[TIERule], anchor_rule_indices: Dict[Any, Set[int]], active_omrs_by_tie_idx: Dict[int, List[OMRule]], omr_global_idx_by_id: Dict[int, int], x0_var: str, y0_var: str, w0_var: str, ml_cache: MLPredicateCache, aggr_model: AggrModel, dis_model: DisModel, offline_index: Optional[OfflineIndex], collect_detail: bool, collect_cache_stats: bool) -> Dict[str, Any]:
    _t_group = time.perf_counter()
    original_omrs = active_omrs_by_tie_idx.get(tie_idx, [])
    row_phase: Optional[Dict[str, float]] = {} if collect_detail else None
    row_init: Optional[Dict[str, float]] = {} if collect_detail else None
    row_cache_stats: Optional[Dict[str, int]] = {} if collect_cache_stats else None
    row: Optional[Dict[str, Any]] = None
    if collect_detail:
        row = {'tie_idx': int(tie_idx), 'n_active_omr_slots': int(len(original_omrs)), 'n_group_anchor_items': 0, 'status': 'started'}
    if not original_omrs:
        if row is not None:
            row['status'] = 'skip_no_active_omr'
        return {'tie_idx': tie_idx, 'kept': False, 'elapsed_seconds': float(time.perf_counter() - _t_group), 'row': row, 'row_phase': row_phase, 'row_init': row_init, 'row_cache_stats': row_cache_stats}
    group_anchor_items = {v for v in anchor_items if tie_idx in anchor_rule_indices.get(v, set())}
    if row is not None:
        row['n_group_anchor_items'] = int(len(group_anchor_items))
    if not group_anchor_items:
        if row is not None:
            row['status'] = 'skip_no_group_anchors'
        return {'tie_idx': tie_idx, 'kept': False, 'elapsed_seconds': float(time.perf_counter() - _t_group), 'row': row, 'row_phase': row_phase, 'row_init': row_init, 'row_cache_stats': row_cache_stats}
    original_tie = tie_rules[tie_idx]
    _t_phase = time.perf_counter()
    group_template = _get_or_build_topk_group_template(tie_idx=tie_idx, original_tie=original_tie, original_omrs=original_omrs, omr_global_idx_by_id=omr_global_idx_by_id, x0_var=x0_var, y0_var=y0_var, w0_var=w0_var, timing_out=row_phase, cache_stats_out=row_cache_stats)
    _record_timing(row_phase, 'template_lookup_total', _t_phase)
    tie_var_map = dict(group_template['tie_var_map'])
    cloned_tie = group_template['cloned_tie']
    cloned_omrs = tuple(group_template['cloned_omrs'])
    omr_var_map_by_local_idx: Dict[int, Dict[str, str]] = {}
    for omr_pos, original_omr in enumerate(original_omrs):
        omr_var_map_by_local_idx[omr_pos] = dict(group_template['omr_var_map_by_omr_id'][id(original_omr)])
    H_group = [cloned_tie] + list(cloned_omrs)
    _t_phase = time.perf_counter()
    group_joint_pattern, _ = _get_rule_group_shape_cached(graph, H_group, x0_var, cache_stats_out=row_cache_stats)
    _record_timing(row_phase, 'shape_cache_query', _t_phase)
    _t_phase = time.perf_counter()
    group_candidate_map, _ = init_candidate_map(graph, group_joint_pattern, user_id, cloned_tie, list(cloned_omrs), ml_cache=ml_cache, aggr_model=aggr_model, dis_model=dis_model, offline_index=offline_index, timing_out=row_init, cache_stats_out=row_cache_stats)
    _record_timing(row_phase, 'init_candidate_map_total', _t_phase)
    if not group_candidate_map:
        if row is not None:
            row['status'] = 'skip_empty_candidate_map'
        return {'tie_idx': tie_idx, 'kept': False, 'elapsed_seconds': float(time.perf_counter() - _t_group), 'row': row, 'row_phase': row_phase, 'row_init': row_init, 'row_cache_stats': row_cache_stats}
    if not group_candidate_map.get(x0_var):
        if row is not None:
            row['status'] = 'skip_empty_x0'
        return {'tie_idx': tie_idx, 'kept': False, 'elapsed_seconds': float(time.perf_counter() - _t_group), 'row': row, 'row_phase': row_phase, 'row_init': row_init, 'row_cache_stats': row_cache_stats}
    _t_phase = time.perf_counter()
    group_candidate_map[y0_var] = group_candidate_map.get(y0_var, set()) & group_anchor_items
    if not group_candidate_map.get(y0_var):
        _record_timing(row_phase, 'pivot_intersections', _t_phase)
        if row is not None:
            row['status'] = 'skip_empty_y0_after_anchor_intersection'
        return {'tie_idx': tie_idx, 'kept': False, 'elapsed_seconds': float(time.perf_counter() - _t_group), 'row': row, 'row_phase': row_phase, 'row_init': row_init, 'row_cache_stats': row_cache_stats}
    if w0_var in group_candidate_map:
        group_candidate_map[w0_var] &= candidate_items
        if not group_candidate_map[w0_var]:
            _record_timing(row_phase, 'pivot_intersections', _t_phase)
            if row is not None:
                row['status'] = 'skip_empty_w0_after_candidate_intersection'
            return {'tie_idx': tie_idx, 'kept': False, 'elapsed_seconds': float(time.perf_counter() - _t_group), 'row': row, 'row_phase': row_phase, 'row_init': row_init, 'row_cache_stats': row_cache_stats}
    _record_timing(row_phase, 'pivot_intersections', _t_phase)
    if row is not None:
        row['status'] = 'kept'
    return {'tie_idx': tie_idx, 'kept': True, 'elapsed_seconds': float(time.perf_counter() - _t_group), 'row': row, 'row_phase': row_phase, 'row_init': row_init, 'row_cache_stats': row_cache_stats, 'group_anchor_items': set(group_anchor_items), 'original_tie': original_tie, 'original_omrs': tuple(original_omrs), 'tie_var_map': tie_var_map, 'cloned_tie': cloned_tie, 'cloned_omrs': cloned_omrs, 'group_candidate_map': {var: set(cands) for var, cands in group_candidate_map.items()}, 'omr_var_map_by_local_idx': omr_var_map_by_local_idx}

def _build_global_topk_rule_group(graph: Graph, user_id: Any, anchor_items: List[Any], candidate_items: Set[Any], tie_rules: List[TIERule], omr_rules: List[OMRule], anchor_rule_indices: Dict[Any, Set[int]], active_omrs_by_tie_idx: Dict[int, List[OMRule]], ml_cache: MLPredicateCache, aggr_model: AggrModel, dis_model: DisModel, offline_index: Optional[OfflineIndex]=None, parallel_stats: Optional[Dict[str, Any]]=None, detail_stats: Optional[Dict[str, Any]]=None, cache_stats_out: Optional[Dict[str, int]]=None) -> Optional[Dict[str, Any]]:
    if not anchor_items or not active_omrs_by_tie_idx:
        return None
    sorted_ties = sorted(active_omrs_by_tie_idx)
    if not omr_rules:
        return None
    x0_var, y0_var, w0_var = _topk_global_pivot_vars(tie_rules, omr_rules)
    omr_global_idx_by_id = {id(rule): idx for idx, rule in enumerate(omr_rules)}
    local_cache_stats: Optional[Dict[str, int]] = {} if detail_stats is not None or cache_stats_out is not None else None
    detail_phase_totals: Optional[Dict[str, float]] = defaultdict(float) if detail_stats is not None else None
    detail_init_totals: Optional[Dict[str, float]] = defaultdict(float) if detail_stats is not None else None
    detail_rows: Optional[List[Dict[str, Any]]] = [] if detail_stats is not None else None
    global_candidate_map: Dict[str, Set[Any]] = {x0_var: {user_id}, y0_var: set(anchor_items)}
    H_global: List[Any] = []
    rule_to_group: Dict[int, int] = {}
    rule_to_original: Dict[int, Any] = {}
    group_states: Dict[int, Dict[str, Any]] = {}

    def _finalize_detail_row(row_obj: Optional[Dict[str, Any]], *, row_phase_obj: Optional[Dict[str, float]], row_init_obj: Optional[Dict[str, float]], row_cache_obj: Optional[Dict[str, int]], started_at: Optional[float]=None, elapsed_seconds: Optional[float]=None) -> None:
        if elapsed_seconds is None:
            row_elapsed = float(time.perf_counter() - started_at) if started_at is not None else 0.0
        else:
            row_elapsed = float(elapsed_seconds)
        if row_obj is not None and detail_rows is not None:
            row_obj['elapsed_seconds'] = row_elapsed
            row_obj['phase_seconds'] = dict(row_phase_obj or {})
            row_obj['init_candidate_map_detail_seconds'] = dict(row_init_obj or {})
            row_obj['cache_stats'] = dict(row_cache_obj or {})
            detail_rows.append(row_obj)
        if detail_phase_totals is not None and row_phase_obj is not None:
            for name, elapsed in row_phase_obj.items():
                detail_phase_totals[name] += float(elapsed)
        if detail_init_totals is not None and row_init_obj is not None:
            for name, elapsed in row_init_obj.items():
                detail_init_totals[name] += float(elapsed)
        if local_cache_stats is not None and row_cache_obj is not None:
            for name, value in row_cache_obj.items():
                _increment_counter(local_cache_stats, name, value)
    build_group_worker_count = _resolve_topk_build_group_workers(len(sorted_ties))
    per_tie_outputs: Dict[int, Dict[str, Any]] = {}
    if build_group_worker_count > 1:
        with ThreadPoolExecutor(max_workers=build_group_worker_count) as executor:
            futures = {tie_idx: executor.submit(_build_topk_group_tie_worker, graph=graph, user_id=user_id, tie_idx=tie_idx, anchor_items=anchor_items, candidate_items=candidate_items, tie_rules=tie_rules, anchor_rule_indices=anchor_rule_indices, active_omrs_by_tie_idx=active_omrs_by_tie_idx, omr_global_idx_by_id=omr_global_idx_by_id, x0_var=x0_var, y0_var=y0_var, w0_var=w0_var, ml_cache=ml_cache, aggr_model=aggr_model, dis_model=dis_model, offline_index=offline_index, collect_detail=detail_stats is not None, collect_cache_stats=local_cache_stats is not None) for tie_idx in sorted_ties}
            for tie_idx, future in futures.items():
                per_tie_outputs[tie_idx] = future.result()
    else:
        for tie_idx in sorted_ties:
            per_tie_outputs[tie_idx] = _build_topk_group_tie_worker(graph=graph, user_id=user_id, tie_idx=tie_idx, anchor_items=anchor_items, candidate_items=candidate_items, tie_rules=tie_rules, anchor_rule_indices=anchor_rule_indices, active_omrs_by_tie_idx=active_omrs_by_tie_idx, omr_global_idx_by_id=omr_global_idx_by_id, x0_var=x0_var, y0_var=y0_var, w0_var=w0_var, ml_cache=ml_cache, aggr_model=aggr_model, dis_model=dis_model, offline_index=offline_index, collect_detail=detail_stats is not None, collect_cache_stats=local_cache_stats is not None)
    for tie_idx in sorted_ties:
        worker_output = per_tie_outputs[tie_idx]
        row = worker_output.get('row')
        row_phase = worker_output.get('row_phase')
        row_init = worker_output.get('row_init')
        row_cache_stats = worker_output.get('row_cache_stats')
        reduce_started_at = time.perf_counter()
        if worker_output.get('kept'):
            original_tie = worker_output['original_tie']
            original_omrs = list(worker_output['original_omrs'])
            tie_var_map = dict(worker_output['tie_var_map'])
            cloned_tie = worker_output['cloned_tie']
            cloned_omrs = list(worker_output['cloned_omrs'])
            group_candidate_map = {var: set(cands) for var, cands in worker_output['group_candidate_map'].items()}
            group_anchor_items = set(worker_output['group_anchor_items'])
            omr_var_map_by_global_idx = {}
            original_omr_by_global_idx: Dict[int, OMRule] = {}
            H_group = [cloned_tie] + cloned_omrs
            start_idx = len(H_global)
            H_global.extend(H_group)
            tie_rule_idx = start_idx
            omr_rule_indices = list(range(start_idx + 1, start_idx + 1 + len(cloned_omrs)))
            rule_to_group[tie_rule_idx] = tie_idx
            rule_to_original[tie_rule_idx] = original_tie
            for local_pos, (global_rule_idx, original_omr) in enumerate(zip(omr_rule_indices, original_omrs)):
                rule_to_group[global_rule_idx] = tie_idx
                rule_to_original[global_rule_idx] = original_omr
                original_omr_by_global_idx[global_rule_idx] = original_omr
                omr_var_map_by_global_idx[global_rule_idx] = dict(worker_output['omr_var_map_by_local_idx'][local_pos])
            _t_phase = time.perf_counter()
            for var, cands in group_candidate_map.items():
                if var == x0_var:
                    global_candidate_map[x0_var] &= set(cands)
                elif var == y0_var:
                    continue
                else:
                    global_candidate_map.setdefault(var, set()).update(cands)
            _record_timing(row_phase, 'merge_global_candidate_map', _t_phase)
            _t_phase = time.perf_counter()
            group_states[tie_idx] = {'tie_rule_idx': tie_rule_idx, 'tie_var_map': dict(tie_var_map), 'omr_rule_indices': omr_rule_indices, 'star_cache_user': {}, 'star_cache_item': {}, 'star_cache_alt_per_rule': {}, 'bridge_idx_per_rule': {}, 'dis_pass_per_rule': {}, 'original_omr_by_global_idx': original_omr_by_global_idx, 'omr_var_map_by_global_idx': omr_var_map_by_global_idx, 'anchor_items': set(group_anchor_items)}
            _record_timing(row_phase, 'group_state_assemble', _t_phase)
        total_elapsed = float(worker_output.get('elapsed_seconds', 0.0)) + float(time.perf_counter() - reduce_started_at)
        _record_parallel_bucket(parallel_stats, 'build_global_group_by_tie', tie_idx, total_elapsed)
        _finalize_detail_row(row, row_phase_obj=row_phase, row_init_obj=row_init, row_cache_obj=row_cache_stats, elapsed_seconds=total_elapsed)
    if not H_global or not global_candidate_map.get(x0_var) or (not global_candidate_map.get(y0_var)):
        if cache_stats_out is not None and local_cache_stats is not None:
            for name, value in local_cache_stats.items():
                _increment_counter(cache_stats_out, name, value)
        if detail_stats is not None:
            detail_stats['per_tie_group'] = list(detail_rows or [])
            detail_stats['totals_seconds'] = {name: float(value) for name, value in (detail_phase_totals or {}).items()}
            detail_stats['init_candidate_map_detail_seconds'] = {name: float(value) for name, value in (detail_init_totals or {}).items()}
            detail_stats['cache_stats'] = {name: int(value) for name, value in (local_cache_stats or {}).items()}
            detail_stats['n_kept_tie_groups'] = int(len(group_states))
        return None
    if w0_var in global_candidate_map and (not global_candidate_map[w0_var]):
        if cache_stats_out is not None and local_cache_stats is not None:
            for name, value in local_cache_stats.items():
                _increment_counter(cache_stats_out, name, value)
        if detail_stats is not None:
            detail_stats['per_tie_group'] = list(detail_rows or [])
            detail_stats['totals_seconds'] = {name: float(value) for name, value in (detail_phase_totals or {}).items()}
            detail_stats['init_candidate_map_detail_seconds'] = {name: float(value) for name, value in (detail_init_totals or {}).items()}
            detail_stats['cache_stats'] = {name: int(value) for name, value in (local_cache_stats or {}).items()}
            detail_stats['n_kept_tie_groups'] = int(len(group_states))
        return None
    final_shape_cache_stats: Optional[Dict[str, int]] = {} if detail_stats is not None or cache_stats_out is not None else None
    _t_phase = time.perf_counter()
    joint_pattern, rooted_dag = _get_rule_group_shape_cached(graph, H_global, x0_var, cache_stats_out=final_shape_cache_stats)
    if detail_phase_totals is not None:
        detail_phase_totals['final_global_shape'] += float(time.perf_counter() - _t_phase)
    if local_cache_stats is not None and final_shape_cache_stats is not None:
        for name, value in final_shape_cache_stats.items():
            _increment_counter(local_cache_stats, name, value)
    if cache_stats_out is not None and local_cache_stats is not None:
        for name, value in local_cache_stats.items():
            _increment_counter(cache_stats_out, name, value)
    if detail_stats is not None:
        detail_stats['per_tie_group'] = list(detail_rows or [])
        detail_stats['totals_seconds'] = {name: float(value) for name, value in (detail_phase_totals or {}).items()}
        detail_stats['init_candidate_map_detail_seconds'] = {name: float(value) for name, value in (detail_init_totals or {}).items()}
        detail_stats['cache_stats'] = {name: int(value) for name, value in (local_cache_stats or {}).items()}
        detail_stats['n_kept_tie_groups'] = int(len(group_states))
        detail_stats['n_candidate_anchor_items'] = int(len(anchor_items))
        detail_stats['n_sorted_ties'] = int(len(sorted_ties))
    return {'rules': H_global, 'joint_pattern': joint_pattern, 'rooted_dag': rooted_dag, 'candidate_map': global_candidate_map, 'group_states': group_states, 'rule_to_group': rule_to_group, 'rule_to_original': rule_to_original, 'x0_var': x0_var, 'y0_var': y0_var, 'w0_var': w0_var}

def _remaining_rule_upper_bound_for_pair(v: Any, w: Any, omr_rule_indices: List[int], ub_per_rv: Dict[int, Dict[Any, float]], scored_pairs: Dict[int, Dict[Any, Set[Any]]], blocked_v_by_rule: Dict[int, Set[Any]], total_ub_by_v: Optional[Dict[Any, float]]=None, scored_rule_ids_by_pair: Optional[Dict[Any, Dict[Any, Set[int]]]]=None) -> float:
    if total_ub_by_v is not None and scored_rule_ids_by_pair is not None:
        remaining = float(total_ub_by_v.get(v, 0.0))
        if remaining <= 0.0:
            return 0.0
        for r_idx in scored_rule_ids_by_pair.get(v, {}).get(w, set()):
            if v in blocked_v_by_rule.get(r_idx, set()):
                continue
            remaining -= float(ub_per_rv[r_idx].get(v, 0.0))
        return max(remaining, 0.0)
    remaining = 0.0
    for r_idx in omr_rule_indices:
        if v in blocked_v_by_rule.get(r_idx, set()):
            continue
        if w in scored_pairs[r_idx].get(v, set()):
            continue
        remaining += ub_per_rv[r_idx].get(v, 0.0)
    return remaining

def _build_total_remaining_rule_upper_bound_by_v(*, y0_candidates: Set[Any], omr_rule_indices: List[int], ub_per_rv: Dict[int, Dict[Any, float]], blocked_v_by_rule: Dict[int, Set[Any]]) -> Dict[Any, float]:
    totals: Dict[Any, float] = {}
    for v in y0_candidates:
        total = 0.0
        for r_idx in omr_rule_indices:
            if v in blocked_v_by_rule.get(r_idx, set()):
                continue
            total += float(ub_per_rv[r_idx].get(v, 0.0))
        totals[v] = total
    return totals

def _compute_globally_prunable_alternatives(user_id: Any, y0_candidates: Set[Any], w0_candidates: Set[Any], omr_rule_indices: List[int], score_accumulators: Dict[Any, Dict[Any, float]], best_alt_scores: Dict[Any, float], ub_per_rv: Dict[int, Dict[Any, float]], scored_pairs: Dict[int, Dict[Any, Set[Any]]], blocked_v_by_rule: Dict[int, Set[Any]], scorer: RankingScoreComputer, lambda_param: float, bound_sigma_size: int, tau_global: float, scored_rule_ids_by_pair: Optional[Dict[Any, Dict[Any, Set[int]]]]=None) -> Set[Any]:
    if tau_global <= 0.0 or not y0_candidates or (not w0_candidates):
        return set()
    total_ub_by_v = _build_total_remaining_rule_upper_bound_by_v(y0_candidates=y0_candidates, omr_rule_indices=omr_rule_indices, ub_per_rv=ub_per_rv, blocked_v_by_rule=blocked_v_by_rule)
    prunable: Set[Any] = set()
    for w in w0_candidates:
        global_ub = best_alt_scores.get(w, 0.0)
        if global_ub >= tau_global:
            continue
        s_acc_component = (1.0 - lambda_param) * scorer._get_s_acc(user_id, w)
        for v in y0_candidates:
            s_current = score_accumulators.get(v, {}).get(w, 0.0)
            sum_rem_ub = _remaining_rule_upper_bound_for_pair(v=v, w=w, omr_rule_indices=omr_rule_indices, ub_per_rv=ub_per_rv, scored_pairs=scored_pairs, blocked_v_by_rule=blocked_v_by_rule, total_ub_by_v=total_ub_by_v, scored_rule_ids_by_pair=scored_rule_ids_by_pair)
            if s_current == 0.0:
                pair_ub = s_acc_component + sum_rem_ub / max(bound_sigma_size, 1)
            else:
                pair_ub = s_current + sum_rem_ub / max(bound_sigma_size, 1)
            if pair_ub > global_ub:
                global_ub = pair_ub
            if global_ub >= tau_global:
                break
        if global_ub < tau_global:
            prunable.add(w)
    return prunable

def _init_rule_delta_upper_bound(rule: OMRule, graph: Graph) -> float:
    return RankingScoreComputer.get_rule_delta_upper_bound(rule, graph)

def _next_heap_counter(counter_state: Optional[List[int]]=None) -> int:
    global _heap_counter
    if counter_state is None:
        _heap_counter += 1
        return _heap_counter
    counter_state[0] += 1
    return counter_state[0]

def _cleanup_lazy_heap(heap: List[Tuple[float, int, Any]], active_scores: Dict[Any, float]) -> None:
    if _HAS_FAST_HEAP:
        _cy_cleanup_lazy_heap(heap, active_scores)
        return
    while heap and active_scores.get(heap[0][2]) != heap[0][0]:
        heapq.heappop(heap)

def _compact_lazy_heap(heap: List[Tuple[float, int, Any]], active_scores: Dict[Any, float], counter_state: Optional[List[int]]=None) -> None:
    rebuilt = [(score, _next_heap_counter(counter_state), item) for item, score in active_scores.items()]
    heap[:] = rebuilt
    heapq.heapify(heap)

def _update_unique_topk_heap(item: Any, score: float, heap: List[Tuple[float, int, Any]], active_scores: Dict[Any, float], k: int, counter_state: Optional[List[int]]=None) -> None:
    if _HAS_FAST_HEAP and counter_state is not None:
        _cy_update_unique_topk_heap(item, float(score), heap, active_scores, int(k), counter_state)
        return
    k = max(int(k), 1)
    current = active_scores.get(item)
    if current is not None:
        if score <= current:
            return
        active_scores[item] = score
        heapq.heappush(heap, (score, _next_heap_counter(counter_state), item))
    elif len(active_scores) < k:
        active_scores[item] = score
        heapq.heappush(heap, (score, _next_heap_counter(counter_state), item))
    else:
        _cleanup_lazy_heap(heap, active_scores)
        if not heap:
            active_scores[item] = score
            heapq.heappush(heap, (score, _next_heap_counter(counter_state), item))
        else:
            root_score, _, root_item = heap[0]
            if score <= root_score:
                return
            heapq.heappop(heap)
            active_scores.pop(root_item, None)
            active_scores[item] = score
            heapq.heappush(heap, (score, _next_heap_counter(counter_state), item))
    if len(heap) > max(32, 4 * k):
        _compact_lazy_heap(heap, active_scores, counter_state)

def _update_global_best_and_tau(w: Any, score: float, best_scores: Dict[Any, float], global_heap: List[Tuple[float, int, Any]], global_heap_scores: Dict[Any, float], alt_k: int, counter_state: Optional[List[int]]=None) -> float:
    if _HAS_FAST_HEAP and counter_state is not None:
        return float(_cy_update_global_best_and_tau(w, float(score), best_scores, global_heap, global_heap_scores, int(alt_k), counter_state))
    current = best_scores.get(w)
    if current is not None and score <= current:
        _cleanup_lazy_heap(global_heap, global_heap_scores)
        return global_heap[0][0] if len(global_heap_scores) == max(int(alt_k), 1) and global_heap else 0.0
    best_scores[w] = score
    _update_unique_topk_heap(item=w, score=score, heap=global_heap, active_scores=global_heap_scores, k=alt_k, counter_state=counter_state)
    _cleanup_lazy_heap(global_heap, global_heap_scores)
    if len(global_heap_scores) == max(int(alt_k), 1) and global_heap:
        return global_heap[0][0]
    return 0.0

def _prune_global_alternative_state(*, valid_items: Set[Any], best_scores: Dict[Any, float], best_anchors: Dict[Any, Any], global_heap: List[Tuple[float, int, Any]], global_heap_scores: Dict[Any, float], alt_k: int, counter_state: Optional[List[int]]=None) -> float:
    if not valid_items:
        best_scores.clear()
        best_anchors.clear()
        global_heap.clear()
        global_heap_scores.clear()
        return 0.0
    stale_any = False
    for w in list(best_scores):
        if w not in valid_items:
            best_scores.pop(w, None)
            best_anchors.pop(w, None)
            stale_any = True
    for w in list(global_heap_scores):
        if w not in valid_items:
            global_heap_scores.pop(w, None)
            stale_any = True
    if stale_any:
        _compact_lazy_heap(global_heap, global_heap_scores, counter_state)
    else:
        _cleanup_lazy_heap(global_heap, global_heap_scores)
    if len(global_heap_scores) == max(int(alt_k), 1) and global_heap:
        return global_heap[0][0]
    return 0.0

def _rebuild_global_alternative_state_from_scores(*, valid_items: Set[Any], score_accumulators: Dict[Any, Dict[Any, float]], best_scores: Dict[Any, float], best_anchors: Dict[Any, Any], global_heap: List[Tuple[float, int, Any]], global_heap_scores: Dict[Any, float], alt_k: int, counter_state: Optional[List[int]]=None) -> float:
    if not valid_items:
        best_scores.clear()
        best_anchors.clear()
        global_heap.clear()
        global_heap_scores.clear()
        return 0.0
    rebuilt_best_scores: Dict[Any, float] = {}
    rebuilt_best_anchors: Dict[Any, Any] = {}
    for v, per_w_scores in score_accumulators.items():
        for w, score in per_w_scores.items():
            if w not in valid_items or v == w:
                continue
            current_best = rebuilt_best_scores.get(w)
            if current_best is None or score > current_best or (score == current_best and _prefer_tied_anchor(v, rebuilt_best_anchors.get(w))):
                rebuilt_best_scores[w] = float(score)
                rebuilt_best_anchors[w] = v
    best_scores.clear()
    best_scores.update(rebuilt_best_scores)
    best_anchors.clear()
    best_anchors.update(rebuilt_best_anchors)
    global_heap.clear()
    global_heap_scores.clear()
    for w, score in rebuilt_best_scores.items():
        _update_unique_topk_heap(item=w, score=score, heap=global_heap, active_scores=global_heap_scores, k=alt_k, counter_state=counter_state)
    _cleanup_lazy_heap(global_heap, global_heap_scores)
    if len(global_heap_scores) == max(int(alt_k), 1) and global_heap:
        return global_heap[0][0]
    return 0.0

def _build_global_output_psi(*, anchor_items: Iterable[Any], global_heap_scores: Dict[Any, float], best_anchors: Dict[Any, Any]) -> Dict[Any, List[Tuple[float, int, Any]]]:
    psi: Dict[Any, List[Tuple[float, int, Any]]] = {anchor: [] for anchor in anchor_items}
    ranked_global = sorted(global_heap_scores.items(), key=lambda item: (-item[1], repr(item[0])))
    for rank, (w, score) in enumerate(ranked_global):
        anchor = best_anchors.get(w)
        if anchor not in psi or w == anchor:
            continue
        psi[anchor].append((float(score), -rank, w))
    return psi

def _prefer_tied_anchor(candidate_anchor: Any, current_anchor: Optional[Any]) -> bool:
    if current_anchor is None:
        return True
    return repr(candidate_anchor) < repr(current_anchor)

def update_psi_and_tau(v: Any, w: Any, score: float, Psi_v: List[Tuple[float, int, Any]], tau_dict: Dict[Any, float], k: int, heap_scores: Optional[Dict[Any, float]]=None, counter_state: Optional[List[int]]=None):
    if heap_scores is not None:
        if _HAS_FAST_HEAP and counter_state is not None:
            _cy_update_heap_and_tau(v, w, float(score), Psi_v, tau_dict, int(k), heap_scores, counter_state)
            return
        _update_unique_topk_heap(item=w, score=score, heap=Psi_v, active_scores=heap_scores, k=k, counter_state=counter_state)
        _cleanup_lazy_heap(Psi_v, heap_scores)
        tau_dict[v] = Psi_v[0][0] if len(heap_scores) == max(int(k), 1) and Psi_v else 0.0
        return
    global _heap_counter
    found = False
    for i, entry in enumerate(Psi_v):
        if entry[-1] == w:
            _heap_counter += 1
            Psi_v[i] = (score, _heap_counter, w)
            found = True
            break
    if found:
        heapq.heapify(Psi_v)
    else:
        _heap_counter += 1
        if len(Psi_v) < k:
            heapq.heappush(Psi_v, (score, _heap_counter, w))
        elif score > Psi_v[0][0]:
            heapq.heapreplace(Psi_v, (score, _heap_counter, w))
    if len(Psi_v) == k:
        tau_dict[v] = Psi_v[0][0]
    else:
        tau_dict[v] = 0.0

def coupled_topk(graph: Graph, user_id: Any, candidate_items: Set[Any], tie_rules: List[TIERule], omr_rules: List[OMRule], dis_model: DisModel, aggr_model: AggrModel, ml_cache: MLPredicateCache, delta_L: float, delta_H: float, k: int, alt_k: Optional[int]=None, lambda_param: float=0.6, max_iterations: int=10, topic_model: Optional[TopicProximityModel]=None, offline_index: Optional[OfflineIndex]=None, preselected_anchors: Optional[List[Any]]=None, preselected_sigma_prime_tie: Optional[Set[int]]=None, prepared_anchor_runtime: Optional[Dict[str, Any]]=None, anchor_max_iterations: Optional[int]=None, gamma_max_mode: str='exact', gamma_upper_bound_items: Optional[Iterable[Any]]=None) -> Tuple[List[Any], Dict[Any, List[Tuple[float, Any]]]]:
    t_start = time.perf_counter()
    _topk_phase_hook = _TOPK_PHASE_TIMING_HOOK
    _phase_times: Optional[Dict[str, float]] = defaultdict(float) if callable(_topk_phase_hook) else None
    _parallel_stats: Optional[Dict[str, Any]] = {} if callable(_topk_phase_hook) else None
    _anchor_select_detail: Optional[Dict[str, float]] = defaultdict(float) if callable(_topk_phase_hook) else None
    _cache_stats: Optional[Dict[str, int]] = {} if callable(_topk_phase_hook) else None
    _build_global_group_detail: Optional[Dict[str, Any]] = {} if callable(_topk_phase_hook) and _topk_build_group_profile_enabled() else None
    iteration_count = 0

    def _record_phase(phase: str, started_at: float) -> None:
        if _phase_times is not None:
            _phase_times[phase] += time.perf_counter() - started_at
    candidate_items_set = set(candidate_items)
    user_ml_cache = ml_cache.get_user_view(user_id) if hasattr(ml_cache, 'get_user_view') else ml_cache
    gamma_mode = str(gamma_max_mode).strip().lower()
    if gamma_mode not in {'exact', 'unit', 'global_upper_bound'}:
        raise ValueError(f'Unsupported gamma_max_mode: {gamma_max_mode!r}')
    gamma_upper_items = gamma_upper_bound_items if gamma_upper_bound_items is not None else candidate_items_set
    _t_phase = time.perf_counter()
    if preselected_anchors is not None:
        K_u_list = list(preselected_anchors[:k])
        if preselected_sigma_prime_tie is not None:
            sigma_prime_tie = set(preselected_sigma_prime_tie)
        else:
            sigma_prime_tie = _relevant_tie_rules_for_selected_anchors(graph=graph, user_id=user_id, selected_anchors=K_u_list, tie_rules=tie_rules, ml_cache=user_ml_cache, prepared_runtime=prepared_anchor_runtime)
    else:
        K_u_list, sigma_prime_tie = anchor_select(graph=graph, user_id=user_id, candidate_items=candidate_items_set, tie_rules=tie_rules, ml_cache=ml_cache, delta_L=delta_L, delta_H=delta_H, k=k, offline_index=offline_index, prepared_runtime=prepared_anchor_runtime, max_window_iterations=anchor_max_iterations, parallel_stats=_parallel_stats, detail_stats=_anchor_select_detail)
    _record_phase('anchor_select', _t_phase)
    _t_phase = time.perf_counter()
    K_u = set(K_u_list)
    anchor_rule_indices = _selected_anchor_rule_indices(graph=graph, user_id=user_id, selected_anchors=K_u_list, tie_rules=tie_rules, ml_cache=user_ml_cache, prepared_runtime=prepared_anchor_runtime, parallel_stats=_parallel_stats)
    _record_phase('anchor_rule_index', _t_phase)
    _t_phase = time.perf_counter()
    active_omrs_by_tie_idx, _ = _active_topk_omrs_by_tie_index(graph=graph, user_id=user_id, selected_tie_indices=sorted(sigma_prime_tie), tie_rules=tie_rules, omr_rules=omr_rules, anchor_items=K_u, ml_cache=user_ml_cache, parallel_stats=_parallel_stats)
    _record_phase('active_omr_index', _t_phase)
    score_accumulators: Dict[Any, Dict[Any, float]] = {v: {} for v in K_u}
    best_anchor_scores: Dict[Any, float] = {v: 0.0 for v in K_u}
    heap_counter_state = [0]
    global_heap: List[Tuple[float, int, Any]] = []
    global_heap_scores: Dict[Any, float] = {}
    best_alt_scores: Dict[Any, float] = {}
    best_alt_anchor: Dict[Any, Any] = {}
    tau_global = 0.0
    score_sigma_size = max(len(omr_rules), 1)
    compact_graph = getattr(graph, '_compact', None)
    scorer = RankingScoreComputer(graph=graph, dis_model=dis_model, aggr_model=aggr_model, ml_cache=user_ml_cache, lambda_param=lambda_param, sigma_omr_size=score_sigma_size, topic_model=topic_model)
    if hasattr(scorer, 'prepare_user'):
        scorer.prepare_user(user_id)
    if offline_index is not None:
        scorer._mdis_cache.update(offline_index.dis_scores)
        scorer._maggr_cache.update(offline_index.aggr_scores)
        logger.debug('[coupled_topk] OfflineIndex provided: using cache-accelerated coupled refinement path.')
    user_s_acc_cache: Dict[Any, float] = {}

    def _get_user_s_acc(item_id: Any) -> float:
        cached = user_s_acc_cache.get(item_id)
        if cached is not None:
            return cached
        score = scorer._get_s_acc(user_id, item_id)
        user_s_acc_cache[item_id] = score
        return score

    def _gamma_bound_for(v: Any, ws: Iterable[Any]) -> float:
        ws_list = list(ws)
        if not ws_list:
            return 0.0
        if gamma_mode == 'unit':
            return 1.0
        if gamma_mode == 'global_upper_bound':
            return _global_gamma_upper_bound(scorer, v, gamma_upper_items)
        return _gamma_max_from_scorer(scorer, v, ws_list)
    global_bridge_index_cache: Dict[int, Optional[BridgeReachabilityIndex]] = {}

    def _get_global_bridge_index(rule: OMRule) -> Optional[BridgeReachabilityIndex]:
        cache_key = id(rule)
        if cache_key in global_bridge_index_cache:
            return global_bridge_index_cache[cache_key]
        if not K_u:
            global_bridge_index_cache[cache_key] = None
            return None
        omr_pat = rule.pattern
        built_index: Optional[BridgeReachabilityIndex] = None
        for bp_idx, bridge_path in enumerate(omr_pat.bridge_paths):
            origin = omr_pat.bridge_origins[bp_idx]
            if origin != omr_pat.anchor_center:
                continue
            bp_seq = getattr(bridge_path, 'path_sequence', [])
            if not bp_seq or bp_seq[-1] != omr_pat.alt_center:
                continue
            built_index = BridgeReachabilityIndex.build(graph, bridge_path, K_u, w_var=omr_pat.alt_center, pw_predicates=rule.get_point_wise_predicates())
            break
        global_bridge_index_cache[cache_key] = built_index
        return built_index
    alt_heap_k = max(int(alt_k if alt_k is not None else k), 1)
    w0_var: Optional[str] = None
    F_H: Dict[str, Set[Any]] = {}
    _t_phase = time.perf_counter()
    global_group = _build_global_topk_rule_group(graph=graph, user_id=user_id, anchor_items=K_u_list, candidate_items=candidate_items_set, tie_rules=tie_rules, omr_rules=omr_rules, anchor_rule_indices=anchor_rule_indices, active_omrs_by_tie_idx=active_omrs_by_tie_idx, ml_cache=user_ml_cache, aggr_model=aggr_model, dis_model=dis_model, offline_index=offline_index, parallel_stats=_parallel_stats, detail_stats=_build_global_group_detail, cache_stats_out=_cache_stats)
    _record_phase('build_global_group', _t_phase)
    if global_group is not None:
        H = global_group['rules']
        joint_pattern = global_group['joint_pattern']
        rooted_dag = global_group['rooted_dag']
        candidate_map = {var: set(cands) for var, cands in global_group['candidate_map'].items()}
        group_states = global_group['group_states']
        rule_to_group = global_group['rule_to_group']
        rule_to_original = global_group['rule_to_original']
        x0 = global_group['x0_var']
        y0 = global_group['y0_var']
        w0_var = global_group['w0_var']
        _t_phase = time.perf_counter()
        runtime_prepare_rule_times: Optional[Dict[int, float]] = {} if _parallel_stats is not None else None
        if runtime_prepare_rule_times is not None:
            runtime = _get_rule_group_runtime_cached(graph, H, compact_graph, per_rule_timing=runtime_prepare_rule_times, cache_stats_out=_cache_stats)
        else:
            runtime = _get_rule_group_runtime_cached(graph, H, compact_graph, cache_stats_out=_cache_stats)
        prefilter_caches = runtime['prefilter_caches']
        edge_lookups_per_rule = runtime['edge_lookups_per_rule']
        star_contexts_per_rule = runtime['star_contexts_per_rule']
        rule_runtime_per_rule = runtime['rule_runtime_per_rule']
        if runtime_prepare_rule_times:
            for rule_idx, elapsed in runtime_prepare_rule_times.items():
                _record_parallel_bucket(_parallel_stats, 'runtime_prepare_by_tie', rule_to_group.get(rule_idx, rule_idx), elapsed)
        _record_phase('runtime_prepare', _t_phase)
        omr_rule_indices = [rule_idx for rule_idx, rule in enumerate(H) if isinstance(rule, OMRule)]
        rule_delta_max = {rule_idx: _init_rule_delta_upper_bound(H[rule_idx], graph) for rule_idx in omr_rule_indices}
        bound_sigma_size = max(len(omr_rule_indices), 1)
        prev_iteration_state = None
        F_H = {var: set(cands) for var, cands in candidate_map.items()}
        iteration_limit = max(int(max_iterations), 1)
        inapplicable_v_by_rule = {rule_idx: set(K_u) - set(group_states[rule_to_group[rule_idx]]['anchor_items']) for rule_idx in omr_rule_indices}
        scored_pairs: Dict[int, Dict[Any, Set[Any]]] = {rule_idx: {} for rule_idx in omr_rule_indices}
        scored_rule_ids_by_pair: Dict[Any, Dict[Any, Set[int]]] = {}
        globally_pruned_w: Set[Any] = set()
        aggr_scores_cache = {}
        _t_phase = time.perf_counter()
        for tie_idx, group_state in group_states.items():
            _t_group = time.perf_counter()
            if offline_index is None:
                _record_parallel_bucket(_parallel_stats, 'offline_bridge_prepare_by_tie', tie_idx, time.perf_counter() - _t_group)
                continue
            offline_item_cache = offline_index.item_star.get(tie_idx, {})
            if offline_item_cache:
                tie_var_map = group_state.get('tie_var_map', {})
                for v in group_state['anchor_items']:
                    offline_match = offline_item_cache.get(v)
                    if offline_match is not None:
                        group_state['star_cache_item'][v] = _rename_cached_match_vars(offline_match, tie_var_map)
            for rule_idx, original_omr in group_state['original_omr_by_global_idx'].items():
                omr_global_idx = offline_index._omr_id_to_idx.get(id(original_omr))
                if omr_global_idx is None:
                    continue
                bridge_idx = offline_index.bridge_idx.get(omr_global_idx)
                if bridge_idx is not None:
                    var_map = group_state['omr_var_map_by_global_idx'].get(rule_idx, {})
                    group_state['bridge_idx_per_rule'][rule_idx] = _get_renamed_bridge_index_cached(bridge_idx, var_map)
                offline_dis_pass = offline_index.dis_pass.get(omr_global_idx)
                if offline_dis_pass:
                    group_state['dis_pass_per_rule'][rule_idx] = offline_dis_pass
                var_map = group_state['omr_var_map_by_global_idx'].get(rule_idx, {})
                renamed_alt_cache = _get_renamed_offline_alt_cache_cached(offline_index, omr_global_idx, var_map)
                if renamed_alt_cache:
                    candidate_w0 = candidate_map.get(w0_var, set())
                    if candidate_w0 and len(candidate_w0) < len(renamed_alt_cache):
                        group_state['star_cache_alt_per_rule'][rule_idx] = {w: renamed_alt_cache[w] for w in candidate_w0 if w in renamed_alt_cache}
                    else:
                        group_state['star_cache_alt_per_rule'][rule_idx] = renamed_alt_cache
            _record_parallel_bucket(_parallel_stats, 'offline_bridge_prepare_by_tie', tie_idx, time.perf_counter() - _t_group)
        _record_phase('offline_bridge_prepare', _t_phase)
        _t_phase = time.perf_counter()
        _bridge_indexes: Dict[int, BridgeReachabilityIndex] = {}
        for rule_idx in omr_rule_indices:
            _t_rule = time.perf_counter()
            group_state = group_states[rule_to_group[rule_idx]]
            if rule_idx in group_state['bridge_idx_per_rule']:
                _bridge_indexes[rule_idx] = group_state['bridge_idx_per_rule'][rule_idx]
            else:
                original_rule = rule_to_original.get(rule_idx)
                if isinstance(original_rule, OMRule):
                    cached_bridge_idx = _get_global_bridge_index(original_rule)
                    if cached_bridge_idx is not None:
                        _bridge_indexes[rule_idx] = _get_renamed_bridge_index_cached(cached_bridge_idx, group_state['omr_var_map_by_global_idx'].get(rule_idx, {}))
            _record_parallel_bucket(_parallel_stats, 'global_bridge_index_by_tie', rule_to_group[rule_idx], time.perf_counter() - _t_rule)
        _record_phase('global_bridge_index', _t_phase)
        _t_phase = time.perf_counter()
        w0_cands = candidate_map.get(w0_var, set()) if w0_var else set()
        y0_cands = candidate_map.get(y0, set())
        candidate_ws_by_v: Dict[Any, Set[Any]] = {}
        for v in y0_cands:
            _t_anchor = time.perf_counter()
            applicable_rule_indices = [rule_idx for rule_idx in omr_rule_indices if v not in inapplicable_v_by_rule.get(rule_idx, set())]
            if not applicable_rule_indices:
                candidate_ws_by_v[v] = set()
            else:
                applicable_indexed_rules = [rule_idx for rule_idx in applicable_rule_indices if rule_idx in _bridge_indexes]
                if not w0_cands or len(applicable_indexed_rules) != len(applicable_rule_indices):
                    candidate_ws_by_v[v] = set(w0_cands)
                else:
                    reachable_union: Set[Any] = set()
                    for rule_idx in applicable_indexed_rules:
                        reachable_union |= _bridge_indexes[rule_idx].get_reachable(v)
                    candidate_ws_by_v[v] = w0_cands & reachable_union
            _record_parallel_bucket(_parallel_stats, 'candidate_ws_init_by_anchor', v, time.perf_counter() - _t_anchor)
        _record_phase('candidate_ws_init', _t_phase)
        _t_phase = time.perf_counter()
        gamma_max: Dict[Any, float] = {}
        max_s_acc_val = 0.0
        _dis_precomputed = False
        if w0_cands:
            max_s_acc_val = max((_get_user_s_acc(w) for w in w0_cands), default=0.0)
            if gamma_mode == 'exact':
                workload_pairs = sum((len(candidate_ws_by_v.get(v, set())) for v in y0_cands))
                worker_count = _resolve_topk_parallel_workers(len(y0_cands), workload_pairs)
                offline_dis_scores = offline_index.dis_scores if offline_index is not None else None
                if worker_count > 1:
                    y0_chunks = _chunked(list(y0_cands), worker_count)
                    with ThreadPoolExecutor(max_workers=worker_count) as executor:
                        futures = [executor.submit(_compute_gamma_max_worker, chunk, candidate_ws_by_v=candidate_ws_by_v, dis_model=dis_model, topic_model=topic_model, offline_dis_scores=offline_dis_scores) for chunk in y0_chunks if chunk]
                        for future in futures:
                            batch_updates, gamma_chunk = future.result()
                            for v, batch_dis in batch_updates.items():
                                if batch_dis:
                                    ml_cache.batch_set_dis_scores(v, batch_dis)
                                    for w, score in batch_dis.items():
                                        scorer._mdis_cache[v, w] = score
                            gamma_max.update(gamma_chunk)
                    _dis_precomputed = True
                else:
                    if workload_pairs < 500000:
                        for v in y0_cands:
                            ws = candidate_ws_by_v.get(v, set())
                            if not ws:
                                gamma_max[v] = 0.0
                                continue
                            batch_dis = {}
                            if offline_dis_scores:
                                batch_dis = {w: offline_dis_scores[v, w] for w in ws if (v, w) in offline_dis_scores}
                                if batch_dis:
                                    ml_cache.batch_set_dis_scores(v, batch_dis)
                            uncached_ws = ws - set(batch_dis.keys())
                            if uncached_ws:
                                extra_dis = dis_model.compute_batch(v, uncached_ws)
                                if extra_dis:
                                    ml_cache.batch_set_dis_scores(v, extra_dis)
                                    batch_dis.update(extra_dis)
                            for w, score in batch_dis.items():
                                scorer._mdis_cache[v, w] = score
                        _dis_precomputed = True
                    for v in y0_cands:
                        ws = candidate_ws_by_v.get(v, set())
                        if not ws:
                            gamma_max[v] = 0.0
                            continue
                        uncached_ws = {w for w in ws if (v, w) not in scorer._mdis_cache}
                        if uncached_ws:
                            batch_dis = dis_model.compute_batch(v, uncached_ws)
                            for w, score in batch_dis.items():
                                scorer._mdis_cache[v, w] = score
                        gamma_max[v] = _gamma_max_from_scorer(scorer, v, ws)
            else:
                for v in y0_cands:
                    gamma_max[v] = _gamma_bound_for(v, candidate_ws_by_v.get(v, set()))
        _record_phase('precompute_dis_gamma', _t_phase)
        _dis_has_index = dis_model is not None and hasattr(dis_model, '_cotag_scores')
        for _iteration in range(iteration_limit):
            iteration_count += 1
            local_results = {}
            current_tie_anchors = set()
            V_not_phi = {rule_idx: set() for rule_idx in omr_rule_indices}
            omr_rules_skipped_by_bound: Set[int] = set()
            iteration_omr_group_totals: Dict[int, float] = defaultdict(float)

            def _blocked_v_by_rule_snapshot() -> Dict[int, Set[Any]]:
                return {rule_idx: inapplicable_v_by_rule.get(rule_idx, set()) | V_not_phi.get(rule_idx, set()) for rule_idx in omr_rule_indices}
            if w0_var and globally_pruned_w:
                F_H[w0_var] = F_H.get(w0_var, set()) - globally_pruned_w
                if not F_H[w0_var]:
                    break
            _t_phase = time.perf_counter()
            group_worker_count = _resolve_topk_group_workers(len(group_states))
            if group_worker_count > 1:
                with ThreadPoolExecutor(max_workers=group_worker_count) as executor:
                    futures = [executor.submit(_run_topk_tie_group_worker, graph=graph, F_H=F_H, rooted_dag=rooted_dag, H=H, group_state=group_state, compact_graph=compact_graph, prefilter_caches=prefilter_caches, edge_lookups_per_rule=edge_lookups_per_rule, star_contexts_per_rule=star_contexts_per_rule, rule_runtime_per_rule=rule_runtime_per_rule) for _tie_idx, group_state in sorted(group_states.items())]
                    for future in futures:
                        rule_idx, F_phi = future.result()
                        if F_phi is not None:
                            local_results[rule_idx] = F_phi
                            current_tie_anchors |= F_phi.get(y0, set())
            else:
                for _tie_idx, group_state in sorted(group_states.items()):
                    rule_idx, F_phi = _run_topk_tie_group_worker(graph=graph, F_H=F_H, rooted_dag=rooted_dag, H=H, group_state=group_state, compact_graph=compact_graph, prefilter_caches=prefilter_caches, edge_lookups_per_rule=edge_lookups_per_rule, star_contexts_per_rule=star_contexts_per_rule, rule_runtime_per_rule=rule_runtime_per_rule)
                    if F_phi is not None:
                        local_results[rule_idx] = F_phi
                        current_tie_anchors |= F_phi.get(y0, set())
            _record_phase('local_tie', _t_phase)
            _y0_cands_iter = F_H.get(y0, set())
            _t_phase = time.perf_counter()
            ub_per_rv: Dict[int, Dict[Any, float]] = {}
            for r_idx in omr_rule_indices:
                _t_rule = time.perf_counter()
                ub_per_rv[r_idx] = {v: compute_rule_upper_bound(gamma_max.get(v, 1.0), rule_delta_max.get(r_idx, 1.0), lambda_param) for v in _y0_cands_iter}
                iteration_omr_group_totals[rule_to_group[r_idx]] += time.perf_counter() - _t_rule
            _record_phase('upper_bound_prepare', _t_phase)
            if _topk_omr_parallel_enabled():
                candidate_maps_by_group: Dict[int, Dict[int, Dict[str, Set[Any]]]] = defaultdict(dict)
                omr_candidate_base = {var: frozenset(cands) for var, cands in F_H.items()}
                for rule_idx in omr_rule_indices:
                    group_idx = rule_to_group[rule_idx]
                    _t_phase = time.perf_counter()
                    for v in F_H.get(y0, set()):
                        if v in inapplicable_v_by_rule.get(rule_idx, set()) or v in V_not_phi[rule_idx]:
                            continue
                        sum_rem_ub = 0.0
                        for r_idx in omr_rule_indices:
                            if v in inapplicable_v_by_rule.get(r_idx, set()) or v in V_not_phi[r_idx]:
                                continue
                            scored_for_v = scored_pairs[r_idx].get(v, set())
                            if not w0_cands.issubset(scored_for_v):
                                sum_rem_ub += ub_per_rv[r_idx].get(v, 0.0)
                        max_s_current_v = best_anchor_scores.get(v, 0.0)
                        new_w_bound = (1.0 - lambda_param) * max_s_acc_val + 1.0 / bound_sigma_size * sum_rem_ub
                        if max_s_current_v == 0.0:
                            max_possible_score = new_w_bound
                        else:
                            max_possible_score = max(max_s_current_v + 1.0 / bound_sigma_size * sum_rem_ub, new_w_bound)
                        if max_possible_score < tau_global:
                            V_not_phi[rule_idx].add(v)
                    _elapsed_rule = time.perf_counter() - _t_phase
                    if _phase_times is not None:
                        _phase_times['omr_bound_screen'] += _elapsed_rule
                    iteration_omr_group_totals[group_idx] += _elapsed_rule
                    blocked_y0 = inapplicable_v_by_rule.get(rule_idx, set()) | V_not_phi[rule_idx]
                    if blocked_y0:
                        C_prime = dict(omr_candidate_base)
                        C_prime[y0] = omr_candidate_base.get(y0, set()) - blocked_y0
                        if w0_var and w0_var in C_prime and globally_pruned_w:
                            C_prime[w0_var] = C_prime[w0_var] - globally_pruned_w
                        if not C_prime[y0]:
                            omr_rules_skipped_by_bound.add(rule_idx)
                            continue
                    else:
                        C_prime = omr_candidate_base
                        if w0_var and globally_pruned_w:
                            C_prime = dict(omr_candidate_base)
                            C_prime[w0_var] = C_prime.get(w0_var, set()) - globally_pruned_w
                    if w0_var and (not C_prime.get(w0_var, set())):
                        continue
                    candidate_maps_by_group[group_idx][rule_idx] = C_prime
                omr_local_results: Dict[int, Dict[str, Set[Any]]] = {}
                bridge_vw_per_rule: Dict[int, Optional[Dict[Any, Set[Any]]]] = {}
                active_omr_groups = {group_idx: group_candidate_maps for group_idx, group_candidate_maps in sorted(candidate_maps_by_group.items()) if group_candidate_maps}
                omr_worker_count = _resolve_topk_omr_workers(len(active_omr_groups))
                if omr_worker_count > 1:
                    with ThreadPoolExecutor(max_workers=omr_worker_count) as executor:
                        futures = {group_idx: executor.submit(_run_topk_omr_group_refinement_worker, graph=graph, rooted_dag=rooted_dag, H=H, group_state=group_states[group_idx], compact_graph=compact_graph, current_tie_anchors=set(current_tie_anchors), ml_cache=user_ml_cache, candidate_maps_by_rule=group_candidate_maps, prefilter_caches=prefilter_caches, edge_lookups_per_rule=edge_lookups_per_rule, star_contexts_per_rule=star_contexts_per_rule, rule_runtime_per_rule=rule_runtime_per_rule, dis_precomputed=_dis_precomputed, bridge_indexes=_bridge_indexes) for group_idx, group_candidate_maps in active_omr_groups.items()}
                        for group_idx, future in futures.items():
                            worker_output = future.result()
                            omr_local_results.update(worker_output['local_results'])
                            bridge_vw_per_rule.update(worker_output['bridge_vw_per_rule'])
                            _elapsed_group = float(worker_output.get('elapsed_seconds', 0.0))
                            if _phase_times is not None:
                                _phase_times['local_omr'] += _elapsed_group
                            iteration_omr_group_totals[group_idx] += _elapsed_group
                else:
                    for group_idx, group_candidate_maps in active_omr_groups.items():
                        worker_output = _run_topk_omr_group_refinement_worker(graph=graph, rooted_dag=rooted_dag, H=H, group_state=group_states[group_idx], compact_graph=compact_graph, current_tie_anchors=set(current_tie_anchors), ml_cache=user_ml_cache, candidate_maps_by_rule=group_candidate_maps, prefilter_caches=prefilter_caches, edge_lookups_per_rule=edge_lookups_per_rule, star_contexts_per_rule=star_contexts_per_rule, rule_runtime_per_rule=rule_runtime_per_rule, dis_precomputed=_dis_precomputed, bridge_indexes=_bridge_indexes)
                        omr_local_results.update(worker_output['local_results'])
                        bridge_vw_per_rule.update(worker_output['bridge_vw_per_rule'])
                        _elapsed_group = float(worker_output.get('elapsed_seconds', 0.0))
                        if _phase_times is not None:
                            _phase_times['local_omr'] += _elapsed_group
                        iteration_omr_group_totals[group_idx] += _elapsed_group
                for rule_idx in omr_rule_indices:
                    if rule_idx not in omr_local_results:
                        continue
                    rule = H[rule_idx]
                    group_idx = rule_to_group[rule_idx]
                    group_state = group_states[group_idx]
                    rule_pw = rule.get_point_wise_predicates()
                    _rule_alt_cache = group_state['star_cache_alt_per_rule'].setdefault(rule_idx, {})
                    F_phi = omr_local_results[rule_idx]
                    _bvw = bridge_vw_per_rule.get(rule_idx)
                    _t_phase = time.perf_counter()
                    local_results[rule_idx] = F_phi
                    delta_exact = RankingScoreComputer.get_rule_delta_phi(rule, graph, F_phi)
                    tightened_delta = RankingScoreComputer.get_rule_delta_upper_bound(rule, graph, F_phi)
                    if tightened_delta < rule_delta_max.get(rule_idx, tightened_delta):
                        rule_delta_max[rule_idx] = tightened_delta
                        ub_per_rv[rule_idx] = {v: compute_rule_upper_bound(gamma_max.get(v, 1.0), tightened_delta, lambda_param) for v in F_H.get(y0, set())}
                    w0_candidates = F_phi.get(w0_var, set())
                    y0_candidates = F_phi.get(y0, set())
                    if not w0_candidates or not y0_candidates:
                        continue
                    _rule_bridge_idx = _bridge_indexes.get(rule_idx)
                    _fallback_bvw = _bvw if not _rule_bridge_idx and _bvw else None
                    if _fallback_bvw is None and (not _rule_bridge_idx):
                        _fallback_bvw = _compute_bridge_vw_pairs(graph, rule, F_phi, rule_pw, prefilter_cache=prefilter_caches.get(rule_idx))
                    if _fallback_bvw is None and (not _rule_bridge_idx) and rule.pattern.bridge_paths:
                        _fallback_bvw = _compute_vw_pairs_by_anchor_refinement(graph=graph, user_id=user_id, rooted_dag=rooted_dag, rule=rule, F_phi=F_phi, compact_graph=compact_graph, star_cache_user=group_state['star_cache_user'], star_cache_item=group_state['star_cache_item'], star_cache_alt=_rule_alt_cache, ml_cache=user_ml_cache, prefilter_cache=prefilter_caches.get(rule_idx), edge_lookups=edge_lookups_per_rule.get(rule_idx), dis_precomputed=_dis_precomputed, offline_bridge_idx=_bridge_indexes.get(rule_idx), offline_dis_pass=group_state['dis_pass_per_rule'].get(rule_idx), star_contexts=star_contexts_per_rule.get(rule_idx), rule_runtime=rule_runtime_per_rule.get(rule_idx))
                    dis_preds = rule.get_dis_predicates()
                    aggr_preds = rule.get_aggr_predicates()
                    _rule_dis_indexed = _dis_has_index and dis_preds and (len(dis_preds) == 1) and (dis_preds[0].operator in ('>=', '>'))
                    _dis_pass_per_v: Dict[Any, Set[Any]] = {}
                    if _rule_dis_indexed:
                        _dp0 = dis_preds[0]
                        for v in y0_candidates:
                            _dis_pass_per_v[v] = dis_model.get_items_above_threshold(v, _dp0.threshold, w0_candidates, _dp0.operator)
                    blocked_v_by_rule = _blocked_v_by_rule_snapshot()
                    total_ub_by_v = _build_total_remaining_rule_upper_bound_by_v(y0_candidates=y0_candidates, omr_rule_indices=omr_rule_indices, ub_per_rv=ub_per_rv, blocked_v_by_rule=blocked_v_by_rule)
                    for v in y0_candidates:
                        v_w_cands = w0_candidates
                        if _rule_bridge_idx:
                            _reachable = _rule_bridge_idx.get_reachable(v)
                            if _reachable:
                                v_w_cands = w0_candidates & _reachable
                            else:
                                continue
                        elif _fallback_bvw is not None and v in _fallback_bvw:
                            v_w_cands = _fallback_bvw[v] & w0_candidates
                        if dis_preds and (not _rule_dis_indexed):
                            uncached_ws = {w for w in v_w_cands if w != v and (v, w) not in scorer._mdis_cache}
                            if uncached_ws:
                                batch_scores = dis_model.compute_batch(v, uncached_ws)
                                for w, score in batch_scores.items():
                                    scorer._mdis_cache[v, w] = score
                        for w in v_w_cands:
                            if v == w:
                                continue
                            if w in scored_pairs[rule_idx].get(v, set()):
                                continue
                            sum_rem_ub = _remaining_rule_upper_bound_for_pair(v=v, w=w, omr_rule_indices=omr_rule_indices, ub_per_rv=ub_per_rv, scored_pairs=scored_pairs, blocked_v_by_rule=blocked_v_by_rule, total_ub_by_v=total_ub_by_v, scored_rule_ids_by_pair=scored_rule_ids_by_pair)
                            s_current = score_accumulators[v].get(w, 0.0)
                            if s_current == 0.0:
                                s_acc = _get_user_s_acc(w)
                                s_hat = (1.0 - lambda_param) * s_acc + 1.0 / bound_sigma_size * sum_rem_ub
                            else:
                                s_hat = s_current + 1.0 / bound_sigma_size * sum_rem_ub
                            if max(best_alt_scores.get(w, 0.0), s_hat) < tau_global:
                                continue
                            if _rule_dis_indexed:
                                if w not in _dis_pass_per_v.get(v, set()):
                                    continue
                            elif dis_preds:
                                dis_score = scorer._mdis_cache.get((v, w))
                                if dis_score is None:
                                    dis_score = scorer._get_mdis(v, w)
                                if not all((dp.evaluate(dis_score) for dp in dis_preds)):
                                    continue
                            if aggr_preds:
                                if w not in aggr_scores_cache:
                                    aggr_scores_cache[w] = aggr_model.compute(w)
                                if not all((ap.evaluate(aggr_scores_cache[w]) for ap in aggr_preds)):
                                    continue
                            vw = (v, w)
                            mdis_vw = scorer._mdis_cache.get(vw)
                            if mdis_vw is None:
                                mdis_vw = scorer._get_mdis(v, w)
                            sim_t_vw = scorer._get_sim_t(v, w)
                            gamma_vw = sim_t_vw * mdis_vw
                            if w in score_accumulators[v]:
                                new_score = score_accumulators[v][w] + lambda_param * gamma_vw * delta_exact / score_sigma_size
                            else:
                                s_acc = _get_user_s_acc(w)
                                new_score = (1.0 - lambda_param) * s_acc + lambda_param * gamma_vw * delta_exact / score_sigma_size
                            new_score = max(0.0, min(1.0, new_score))
                            score_accumulators[v][w] = new_score
                            if new_score > best_anchor_scores.get(v, 0.0):
                                best_anchor_scores[v] = new_score
                            scored_pairs[rule_idx].setdefault(v, set()).add(w)
                            scored_rule_ids_by_pair.setdefault(v, {}).setdefault(w, set()).add(rule_idx)
                            current_best_for_w = best_alt_scores.get(w)
                            if current_best_for_w is None or new_score > current_best_for_w or (new_score == current_best_for_w and _prefer_tied_anchor(v, best_alt_anchor.get(w))):
                                best_alt_anchor[w] = v
                            tau_global = _update_global_best_and_tau(w, new_score, best_alt_scores, global_heap, global_heap_scores, alt_heap_k, counter_state=heap_counter_state)
                    _elapsed_rule = time.perf_counter() - _t_phase
                    if _phase_times is not None:
                        _phase_times['score_update'] += _elapsed_rule
                    iteration_omr_group_totals[group_idx] += _elapsed_rule
            else:
                for rule_idx in omr_rule_indices:
                    rule = H[rule_idx]
                    group_idx = rule_to_group[rule_idx]
                    group_state = group_states[group_idx]
                    _t_phase = time.perf_counter()
                    for v in F_H.get(y0, set()):
                        if v in inapplicable_v_by_rule.get(rule_idx, set()) or v in V_not_phi[rule_idx]:
                            continue
                        sum_rem_ub = 0.0
                        for r_idx in omr_rule_indices:
                            if v in inapplicable_v_by_rule.get(r_idx, set()) or v in V_not_phi[r_idx]:
                                continue
                            scored_for_v = scored_pairs[r_idx].get(v, set())
                            if not w0_cands.issubset(scored_for_v):
                                sum_rem_ub += ub_per_rv[r_idx].get(v, 0.0)
                        max_s_current_v = best_anchor_scores.get(v, 0.0)
                        new_w_bound = (1.0 - lambda_param) * max_s_acc_val + 1.0 / bound_sigma_size * sum_rem_ub
                        if max_s_current_v == 0.0:
                            max_possible_score = new_w_bound
                        else:
                            max_possible_score = max(max_s_current_v + 1.0 / bound_sigma_size * sum_rem_ub, new_w_bound)
                        if max_possible_score < tau_global:
                            V_not_phi[rule_idx].add(v)
                    _elapsed_rule = time.perf_counter() - _t_phase
                    if _phase_times is not None:
                        _phase_times['omr_bound_screen'] += _elapsed_rule
                    iteration_omr_group_totals[group_idx] += _elapsed_rule
                    blocked_y0 = inapplicable_v_by_rule.get(rule_idx, set()) | V_not_phi[rule_idx]
                    if blocked_y0:
                        C_prime = dict(F_H)
                        C_prime[y0] = F_H.get(y0, set()) - blocked_y0
                        if w0_var and w0_var in C_prime and globally_pruned_w:
                            C_prime[w0_var] = C_prime[w0_var] - globally_pruned_w
                        if not C_prime[y0]:
                            omr_rules_skipped_by_bound.add(rule_idx)
                            continue
                    else:
                        C_prime = F_H
                        if w0_var and globally_pruned_w:
                            C_prime = dict(F_H)
                            C_prime[w0_var] = C_prime.get(w0_var, set()) - globally_pruned_w
                    if w0_var and (not C_prime.get(w0_var, set())):
                        continue
                    rule_pw = rule.get_point_wise_predicates()
                    _bvw: Dict[Any, Set[Any]] = {}
                    if rule_idx not in group_state['star_cache_alt_per_rule']:
                        group_state['star_cache_alt_per_rule'][rule_idx] = {}
                    _rule_alt_cache = group_state['star_cache_alt_per_rule'][rule_idx]
                    _t_phase = time.perf_counter()
                    F_phi = local_refinement(graph, C_prime, rooted_dag, rule, rule_pw, compact_graph, group_state['star_cache_user'], group_state['star_cache_item'], _rule_alt_cache, current_tie_anchors=current_tie_anchors, ml_cache=user_ml_cache, prefilter_cache=prefilter_caches.get(rule_idx), edge_lookups=edge_lookups_per_rule.get(rule_idx), star_contexts=star_contexts_per_rule.get(rule_idx), rule_runtime=rule_runtime_per_rule.get(rule_idx), bridge_vw_out=_bvw, dis_precomputed=_dis_precomputed, offline_bridge_idx=_bridge_indexes.get(rule_idx), offline_dis_pass=group_state['dis_pass_per_rule'].get(rule_idx))
                    _elapsed_rule = time.perf_counter() - _t_phase
                    if _phase_times is not None:
                        _phase_times['local_omr'] += _elapsed_rule
                    iteration_omr_group_totals[group_idx] += _elapsed_rule
                    if F_phi is None:
                        continue
                    _t_phase = time.perf_counter()
                    local_results[rule_idx] = F_phi
                    delta_exact = RankingScoreComputer.get_rule_delta_phi(rule, graph, F_phi)
                    tightened_delta = RankingScoreComputer.get_rule_delta_upper_bound(rule, graph, F_phi)
                    if tightened_delta < rule_delta_max.get(rule_idx, tightened_delta):
                        rule_delta_max[rule_idx] = tightened_delta
                        ub_per_rv[rule_idx] = {v: compute_rule_upper_bound(gamma_max.get(v, 1.0), tightened_delta, lambda_param) for v in F_H.get(y0, set())}
                    w0_candidates = F_phi.get(w0_var, set())
                    y0_candidates = F_phi.get(y0, set())
                    if not w0_candidates or not y0_candidates:
                        continue
                    _rule_bridge_idx = _bridge_indexes.get(rule_idx)
                    _fallback_bvw = _bvw if not _rule_bridge_idx and _bvw else None
                    if _fallback_bvw is None and (not _rule_bridge_idx):
                        _fallback_bvw = _compute_bridge_vw_pairs(graph, rule, F_phi, rule_pw, prefilter_cache=prefilter_caches.get(rule_idx))
                    if _fallback_bvw is None and (not _rule_bridge_idx) and rule.pattern.bridge_paths:
                        _fallback_bvw = _compute_vw_pairs_by_anchor_refinement(graph=graph, user_id=user_id, rooted_dag=rooted_dag, rule=rule, F_phi=F_phi, compact_graph=compact_graph, star_cache_user=group_state['star_cache_user'], star_cache_item=group_state['star_cache_item'], star_cache_alt=_rule_alt_cache, ml_cache=user_ml_cache, prefilter_cache=prefilter_caches.get(rule_idx), edge_lookups=edge_lookups_per_rule.get(rule_idx), dis_precomputed=_dis_precomputed, offline_bridge_idx=_bridge_indexes.get(rule_idx), offline_dis_pass=group_state['dis_pass_per_rule'].get(rule_idx), star_contexts=star_contexts_per_rule.get(rule_idx), rule_runtime=rule_runtime_per_rule.get(rule_idx))
                    dis_preds = rule.get_dis_predicates()
                    aggr_preds = rule.get_aggr_predicates()
                    _rule_dis_indexed = _dis_has_index and dis_preds and (len(dis_preds) == 1) and (dis_preds[0].operator in ('>=', '>'))
                    _dis_pass_per_v: Dict[Any, Set[Any]] = {}
                    if _rule_dis_indexed:
                        _dp0 = dis_preds[0]
                        for v in y0_candidates:
                            _dis_pass_per_v[v] = dis_model.get_items_above_threshold(v, _dp0.threshold, w0_candidates, _dp0.operator)
                    blocked_v_by_rule = _blocked_v_by_rule_snapshot()
                    total_ub_by_v = _build_total_remaining_rule_upper_bound_by_v(y0_candidates=y0_candidates, omr_rule_indices=omr_rule_indices, ub_per_rv=ub_per_rv, blocked_v_by_rule=blocked_v_by_rule)
                    for v in y0_candidates:
                        v_w_cands = w0_candidates
                        if _rule_bridge_idx:
                            _reachable = _rule_bridge_idx.get_reachable(v)
                            if _reachable:
                                v_w_cands = w0_candidates & _reachable
                            else:
                                continue
                        elif _fallback_bvw is not None and v in _fallback_bvw:
                            v_w_cands = _fallback_bvw[v] & w0_candidates
                        if dis_preds and (not _rule_dis_indexed):
                            uncached_ws = {w for w in v_w_cands if w != v and (v, w) not in scorer._mdis_cache}
                            if uncached_ws:
                                batch_scores = dis_model.compute_batch(v, uncached_ws)
                                for w, score in batch_scores.items():
                                    scorer._mdis_cache[v, w] = score
                        for w in v_w_cands:
                            if v == w:
                                continue
                            if w in scored_pairs[rule_idx].get(v, set()):
                                continue
                            sum_rem_ub = _remaining_rule_upper_bound_for_pair(v=v, w=w, omr_rule_indices=omr_rule_indices, ub_per_rv=ub_per_rv, scored_pairs=scored_pairs, blocked_v_by_rule=blocked_v_by_rule, total_ub_by_v=total_ub_by_v, scored_rule_ids_by_pair=scored_rule_ids_by_pair)
                            s_current = score_accumulators[v].get(w, 0.0)
                            if s_current == 0.0:
                                s_acc = _get_user_s_acc(w)
                                s_hat = (1.0 - lambda_param) * s_acc + 1.0 / bound_sigma_size * sum_rem_ub
                            else:
                                s_hat = s_current + 1.0 / bound_sigma_size * sum_rem_ub
                            if max(best_alt_scores.get(w, 0.0), s_hat) < tau_global:
                                continue
                            if _rule_dis_indexed:
                                if w not in _dis_pass_per_v.get(v, set()):
                                    continue
                            elif dis_preds:
                                dis_score = scorer._mdis_cache.get((v, w))
                                if dis_score is None:
                                    dis_score = scorer._get_mdis(v, w)
                                if not all((dp.evaluate(dis_score) for dp in dis_preds)):
                                    continue
                            if aggr_preds:
                                if w not in aggr_scores_cache:
                                    aggr_scores_cache[w] = aggr_model.compute(w)
                                if not all((ap.evaluate(aggr_scores_cache[w]) for ap in aggr_preds)):
                                    continue
                            vw = (v, w)
                            mdis_vw = scorer._mdis_cache.get(vw)
                            if mdis_vw is None:
                                mdis_vw = scorer._get_mdis(v, w)
                            sim_t_vw = scorer._get_sim_t(v, w)
                            gamma_vw = sim_t_vw * mdis_vw
                            if w in score_accumulators[v]:
                                new_score = score_accumulators[v][w] + lambda_param * gamma_vw * delta_exact / score_sigma_size
                            else:
                                s_acc = _get_user_s_acc(w)
                                new_score = (1.0 - lambda_param) * s_acc + lambda_param * gamma_vw * delta_exact / score_sigma_size
                            new_score = max(0.0, min(1.0, new_score))
                            score_accumulators[v][w] = new_score
                            if new_score > best_anchor_scores.get(v, 0.0):
                                best_anchor_scores[v] = new_score
                            scored_pairs[rule_idx].setdefault(v, set()).add(w)
                            scored_rule_ids_by_pair.setdefault(v, {}).setdefault(w, set()).add(rule_idx)
                            current_best_for_w = best_alt_scores.get(w)
                            if current_best_for_w is None or new_score > current_best_for_w or (new_score == current_best_for_w and _prefer_tied_anchor(v, best_alt_anchor.get(w))):
                                best_alt_anchor[w] = v
                            tau_global = _update_global_best_and_tau(w, new_score, best_alt_scores, global_heap, global_heap_scores, alt_heap_k, counter_state=heap_counter_state)
                    _elapsed_rule = time.perf_counter() - _t_phase
                    if _phase_times is not None:
                        _phase_times['score_update'] += _elapsed_rule
                    iteration_omr_group_totals[group_idx] += _elapsed_rule
            if _parallel_stats is not None and iteration_omr_group_totals:
                _parallel_stats.setdefault('omr_group_total_by_iteration', []).append({group_idx: float(elapsed) for group_idx, elapsed in iteration_omr_group_totals.items()})
            if not local_results:
                break
            _t_phase = time.perf_counter()
            bound_pruned_rules = tuple(sorted((rule_idx for rule_idx, blocked_vs in V_not_phi.items() if blocked_vs)))
            new_F_H = group_refinement(local_results)
            for var in candidate_map:
                if var not in new_F_H:
                    new_F_H[var] = set()
            if w0_var and bound_pruned_rules and (w0_var in F_H):
                new_F_H[w0_var] = set(new_F_H.get(w0_var, set())) | set(F_H.get(w0_var, set()))
            F_H = new_F_H
            _record_phase('group_refine', _t_phase)
            _t_phase = time.perf_counter()
            w0_surviving = F_H.get(w0_var, set()) if w0_var else set()
            if w0_surviving:
                for v in F_H.get(y0, set()):
                    surviving_ws = candidate_ws_by_v.get(v, w0_surviving) & w0_surviving
                    gamma_max[v] = _gamma_bound_for(v, surviving_ws)
                max_s_acc_val = max((_get_user_s_acc(w) for w in w0_surviving), default=0.0)
                w0_cands = w0_surviving
            for r_idx in omr_rule_indices:
                if r_idx in local_results:
                    tightened_delta = RankingScoreComputer.get_rule_delta_upper_bound(H[r_idx], graph, local_results[r_idx])
                    rule_delta_max[r_idx] = min(rule_delta_max[r_idx], tightened_delta)
            _record_phase('state_refresh', _t_phase)
            _t_phase = time.perf_counter()
            if w0_var and F_H.get(w0_var):
                blocked_v_by_rule = {rule_idx: set(inapplicable_v_by_rule.get(rule_idx, set())) for rule_idx in omr_rule_indices}
                newly_pruned_w = _compute_globally_prunable_alternatives(user_id=user_id, y0_candidates=F_H.get(y0, set()), w0_candidates=F_H.get(w0_var, set()), omr_rule_indices=omr_rule_indices, score_accumulators=score_accumulators, best_alt_scores=best_alt_scores, ub_per_rv=ub_per_rv, scored_pairs=scored_pairs, blocked_v_by_rule=blocked_v_by_rule, scored_rule_ids_by_pair=scored_rule_ids_by_pair, scorer=scorer, lambda_param=lambda_param, bound_sigma_size=bound_sigma_size, tau_global=tau_global)
                if newly_pruned_w:
                    globally_pruned_w |= newly_pruned_w
                    F_H[w0_var] = F_H.get(w0_var, set()) - newly_pruned_w
                    w0_surviving = F_H.get(w0_var, set())
            _record_phase('global_remove_w', _t_phase)
            _t_phase = time.perf_counter()
            tau_before_cleanup = tau_global
            if w0_var and w0_surviving:
                tau_global = _prune_global_alternative_state(valid_items=w0_surviving, best_scores=best_alt_scores, best_anchors=best_alt_anchor, global_heap=global_heap, global_heap_scores=global_heap_scores, alt_k=alt_heap_k, counter_state=heap_counter_state)
                if tau_global < tau_before_cleanup and globally_pruned_w:
                    F_H[w0_var] = set(F_H.get(w0_var, set())) | set(globally_pruned_w)
                    globally_pruned_w.clear()
                    w0_surviving = F_H.get(w0_var, set())
                    if w0_surviving:
                        _rebuild_global_alternative_state_from_scores(valid_items=w0_surviving, score_accumulators=score_accumulators, best_scores=best_alt_scores, best_anchors=best_alt_anchor, global_heap=global_heap, global_heap_scores=global_heap_scores, alt_k=alt_heap_k, counter_state=heap_counter_state)
                        for v in F_H.get(y0, set()):
                            surviving_ws = candidate_ws_by_v.get(v, w0_surviving) & w0_surviving
                            gamma_max[v] = _gamma_bound_for(v, surviving_ws)
                        max_s_acc_val = max((_get_user_s_acc(w) for w in w0_surviving), default=0.0)
                        w0_cands = w0_surviving
            new_signature = _candidate_map_signature(F_H)
            iteration_state = (new_signature, float(tau_global), bound_pruned_rules, tuple(sorted(omr_rules_skipped_by_bound)))
            converged = prev_iteration_state is not None and iteration_state == prev_iteration_state
            prev_iteration_state = iteration_state
            if not converged:
                for group_state in group_states.values():
                    _invalidate_star_caches(group_state['star_cache_user'], F_H.get(x0, set()), group_state['star_cache_item'], F_H.get(y0, set()), {}, set())
                    for rule_idx, alt_cache in group_state['star_cache_alt_per_rule'].items():
                        valid_alts = F_H.get(H[rule_idx].pattern.alt_center, set())
                        _invalidate_star_caches({}, set(), {}, set(), alt_cache, valid_alts)
            _record_phase('heap_cleanup_and_convergence', _t_phase)
            if converged:
                break
    else:
        logger.warning('[coupled_topk] User %s: no active global Top-k rule group; returning anchors with empty alternatives.', user_id)
    _t_phase = time.perf_counter()
    valid_w0_final = F_H.get(w0_var, set()) if w0_var else set()
    tau_global = _prune_global_alternative_state(valid_items=valid_w0_final if valid_w0_final else set(global_heap_scores), best_scores=best_alt_scores, best_anchors=best_alt_anchor, global_heap=global_heap, global_heap_scores=global_heap_scores, alt_k=alt_heap_k, counter_state=heap_counter_state)
    Psi = _build_global_output_psi(anchor_items=K_u_list, global_heap_scores=global_heap_scores, best_anchors=best_alt_anchor)
    _record_phase('finalize_output', _t_phase)
    elapsed = time.perf_counter() - t_start
    if callable(_topk_phase_hook) and _phase_times is not None:
        try:
            _topk_phase_hook({'task': 'topk', 'user_id': user_id, 'total_seconds': float(elapsed), 'iterations': int(iteration_count), 'gamma_max_mode': gamma_mode, 'n_selected_anchors': len(K_u_list), 'n_active_tie_rules': len(sigma_prime_tie), 'n_active_tie_groups': len(active_omrs_by_tie_idx), 'n_active_omr_rules': sum((len(rules) for rules in active_omrs_by_tie_idx.values())), 'phases': {phase: float(t) for phase, t in _phase_times.items()}, 'anchor_select_detail': {phase: float(t) for phase, t in _anchor_select_detail.items()} if _anchor_select_detail is not None else {}, 'cache_stats': {name: int(value) for name, value in _cache_stats.items()} if _cache_stats is not None else {}, 'build_global_group_detail': _build_global_group_detail if _build_global_group_detail is not None else {}, 'parallel_proxy': _build_parallel_proxy_summary(elapsed, _parallel_stats)})
        except Exception:
            logger.debug('[coupled_topk] phase timing hook failed', exc_info=True)
    logger.debug(f'[coupled_topk] Completed in {elapsed:.3f}s for user {user_id}')
    return (K_u_list, Psi)
