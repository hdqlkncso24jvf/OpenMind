from __future__ import annotations
from typing import TYPE_CHECKING, Any, Dict, List, Optional, Set, Tuple, Union
from collections import defaultdict, OrderedDict
import logging
import os
import time
try:
    from tqdm import tqdm
except ImportError:
    tqdm = None
from data_structure import Graph, Node, Path, PointWisePredicate, PairWisePredicate, OMRPattern, AggrPredicate, ComparisonOp, PathAnchorIndex, TIERule, OMRule, JointPattern, RootedDAG, canonical_tie_rule_signature, _safe_compare_predicate_values, _normalize_predicate_equality_value
from tie_matcher import get_pivot_candidates, apply_path_anchor_filters, match_path_compact, _match_star_paths, build_star_match_context, _prefilter_by_attribute_index, _collect_pairwise_attr_data, _verify_pairwise_fast, _pairwise_vars_independent
try:
    from tie_matcher import _PREFILTER_NA
except ImportError:
    _PREFILTER_NA = '__PREFILTER_NOT_APPLICABLE__'
from models import MLPredicateCache, DisModel, AggrModel, TopicProximityModel
if TYPE_CHECKING:
    from _compact_graph import CompactGraph
    from indexing import OfflineIndex
    from models import RankingScoreComputer
try:
    from _fast_match import match_bridge_batch as _cy_bridge_batch
    from _fast_match import derive_bridge_vw_pairs_fast as _cy_derive_bridge_vw_pairs
    from _fast_match import propagate_candidates_dag_fast as _cy_propagate_dag
    _HAS_BRIDGE_BATCH = True
    _HAS_FAST_DAG = True
except ImportError:
    _HAS_BRIDGE_BATCH = False
    _HAS_FAST_DAG = False
    _cy_derive_bridge_vw_pairs = None
logger = logging.getLogger(__name__)
_RULE_GROUP_TIMING_HOOK = None
_OMR_RULE_TIMING_HOOK = None
_CTR_PHASE_TIMING_HOOK = None
_DAG_PROPAGATION_DIAGNOSTICS: Dict[str, int] = defaultdict(int)
__all__ = ['OMRule', 'JointPattern', 'RootedDAG', 'construct_rule_group', 'init_candidate_map', 'local_refinement', 'group_refinement', 'coupled_ctr', '_merge_candidate_maps']
_STATIC_VAR_CAND_CACHE_MAX = 2048
_STATIC_VAR_CAND_CACHE: 'OrderedDict[Tuple[int, str, Tuple[int, ...], Tuple[Tuple[str, bool], ...]], Tuple[Any, frozenset]]' = OrderedDict()
_GROUP_INIT_STATIC_BLUEPRINT_CACHE_MAX = 256
_GROUP_INIT_STATIC_BLUEPRINT_CACHE: 'OrderedDict[Tuple[int, Any, Tuple[Any, ...]], Tuple[Any, Any, Tuple[Any, ...], Dict[str, Any]]]' = OrderedDict()
_GROUP_INIT_BLUEPRINT_CACHE_MAX = 256
_GROUP_INIT_BLUEPRINT_CACHE: 'OrderedDict[Tuple[int, int, Tuple[int, ...]], Tuple[Any, Any, Tuple[Any, ...], Dict[str, Any]]]' = OrderedDict()
_OMR_CHILDREN_CACHE: "OrderedDict[int, Tuple[Any, Dict[int, List['OMRule']]]]" = OrderedDict()
_RULE_GROUP_SHAPE_CACHE_MAX = 256
_RULE_GROUP_SHAPE_CACHE: 'OrderedDict[Tuple[int, str, Tuple[int, ...]], Tuple[Any, Tuple[Any, ...], JointPattern, RootedDAG]]' = OrderedDict()
_RULE_GROUP_RUNTIME_CACHE_MAX = 256
_RULE_GROUP_RUNTIME_CACHE: 'OrderedDict[Tuple[int, int, Tuple[int, ...]], Tuple[Any, Any, Tuple[Any, ...], Dict[str, Any]]]' = OrderedDict()

def _record_timing(timing_out: Optional[Dict[str, float]], key: str, started_at: float) -> None:
    if timing_out is not None:
        timing_out[key] = timing_out.get(key, 0.0) + (time.perf_counter() - started_at)

def _env_flag(name: str) -> bool:
    raw = str(os.getenv(name, '')).strip().lower()
    return raw in {'1', 'true', 'yes', 'on'}

def _dag_diagnostics_enabled() -> bool:
    return _env_flag('OPENMIND_DAG_DIAGNOSTICS')

def _dag_skip_no_join_enabled() -> bool:
    return _env_flag('OPENMIND_DAG_SKIP_NO_JOIN')

def reset_dag_diagnostics() -> None:
    _DAG_PROPAGATION_DIAGNOSTICS.clear()

def get_dag_diagnostics_snapshot() -> Dict[str, int]:
    return {key: int(value) for key, value in _DAG_PROPAGATION_DIAGNOSTICS.items()}

def _increment_counter(cache_stats_out: Optional[Dict[str, int]], key: str, amount: int=1) -> None:
    if cache_stats_out is not None:
        cache_stats_out[key] = int(cache_stats_out.get(key, 0)) + int(amount)

def _rule_cache_identity(rule: Union[TIERule, 'OMRule']) -> Any:
    cache_identity = getattr(rule, '_cache_identity', None)
    if cache_identity is not None:
        return cache_identity
    return ('rule_id', id(rule))

def _rule_source_ref(rule: Union[TIERule, 'OMRule']) -> Union[TIERule, 'OMRule']:
    return getattr(rule, '_source_rule_ref', rule)

def _rule_source_identity(rule: Union[TIERule, 'OMRule']) -> Any:
    source_ref = getattr(rule, '_source_rule_ref', None)
    if source_ref is not None:
        return ('source_rule_id', id(source_ref))
    source_rule_id = getattr(rule, '_source_rule_id', None)
    if source_rule_id is not None:
        return ('source_rule_id', source_rule_id)
    return ('source_rule_id', id(rule))

def _rename_var_candidate_map(source_candidates: Dict[str, frozenset], source_var_map: Optional[Dict[str, str]]) -> Dict[str, frozenset]:
    if not source_var_map:
        return dict(source_candidates)
    renamed: Dict[str, Set[Any]] = {}
    for source_var, cands in source_candidates.items():
        target_var = source_var_map.get(source_var, source_var)
        if target_var not in renamed:
            renamed[target_var] = set(cands)
        else:
            renamed[target_var] |= set(cands)
    return {var: frozenset(cands) for var, cands in renamed.items()}

def _get_offline_omr_index(offline_index: Optional['OfflineIndex'], rule: 'OMRule') -> Optional[int]:
    if offline_index is None:
        return None
    omr_idx = offline_index._omr_id_to_idx.get(id(rule))
    if omr_idx is not None:
        return omr_idx
    source_rule_id = getattr(rule, '_source_rule_id', None)
    if source_rule_id is None:
        return None
    return offline_index._omr_id_to_idx.get(source_rule_id)

def _get_required_edge_groups_cached(pattern) -> Dict[str, Set[Tuple[str, bool]]]:
    cached = getattr(pattern, '_required_edge_groups_cache', None)
    if cached is None:
        cached = _required_edge_groups(pattern)
        try:
            setattr(pattern, '_required_edge_groups_cache', cached)
        except Exception:
            pass
    return cached

def _get_static_var_candidates(graph: Graph, label: str, preds: List[PointWisePredicate], required_groups: Set[Tuple[str, bool]], timing_out: Optional[Dict[str, float]]=None) -> Set[Any]:
    pred_ids = tuple(sorted((id(p) for p in preds))) if preds else ()
    groups_key = tuple(sorted(required_groups)) if required_groups else ()
    cache_key = (id(graph), label, pred_ids, groups_key)
    cached = _STATIC_VAR_CAND_CACHE.get(cache_key)
    if cached is not None:
        cached_graph, cached_values = cached
        if cached_graph is graph:
            _STATIC_VAR_CAND_CACHE.move_to_end(cache_key)
            return set(cached_values)
        _STATIC_VAR_CAND_CACHE.pop(cache_key, None)
    if preds:
        _t_phase = time.perf_counter()
        prefiltered = _prefilter_by_attribute_index(graph, label, preds)
        _record_timing(timing_out, 'attribute_prefilter', _t_phase)
        if prefiltered is not None:
            candidates = set(prefiltered)
        else:
            _t_phase = time.perf_counter()
            label_view = graph.get_node_ids_by_label_view(label)
            candidates = {nid for nid in label_view if all((p.evaluate(graph.get_node(nid)) for p in preds))}
            _record_timing(timing_out, 'label_scan_fallback', _t_phase)
    else:
        _t_phase = time.perf_counter()
        candidates = set(graph.get_node_ids_by_label_view(label))
        _record_timing(timing_out, 'label_lookup', _t_phase)
    if required_groups:
        _t_phase = time.perf_counter()
        candidates = _nodes_with_required_edge_groups(graph, candidates, required_groups)
        _record_timing(timing_out, 'required_groups_filter', _t_phase)
    frozen = frozenset(candidates)
    _STATIC_VAR_CAND_CACHE[cache_key] = (graph, frozen)
    _STATIC_VAR_CAND_CACHE.move_to_end(cache_key)
    while len(_STATIC_VAR_CAND_CACHE) > _STATIC_VAR_CAND_CACHE_MAX:
        _STATIC_VAR_CAND_CACHE.popitem(last=False)
    return set(frozen)

def _get_group_init_static_blueprint(graph: Graph, tie_rule: TIERule, omr_rules: List['OMRule'], timing_out: Optional[Dict[str, float]]=None, cache_stats_out: Optional[Dict[str, int]]=None) -> Dict[str, Any]:
    tie_rule_key = _rule_source_identity(tie_rule)
    omr_rule_keys = tuple((_rule_source_identity(rule) for rule in omr_rules))
    key = (id(graph), tie_rule_key, omr_rule_keys)
    cached = _GROUP_INIT_STATIC_BLUEPRINT_CACHE.get(key)
    if cached is not None:
        cached_graph, cached_tie_key, cached_omr_keys, cached_blueprint = cached
        if cached_graph is graph and cached_tie_key == tie_rule_key and (cached_omr_keys == omr_rule_keys):
            _increment_counter(cache_stats_out, 'blueprint_static_hits')
            _GROUP_INIT_STATIC_BLUEPRINT_CACHE.move_to_end(key)
            return cached_blueprint
        _GROUP_INIT_STATIC_BLUEPRINT_CACHE.pop(key, None)
    _increment_counter(cache_stats_out, 'blueprint_static_misses')
    source_tie_rule = _rule_source_ref(tie_rule)
    source_omr_rules = [_rule_source_ref(rule) for rule in omr_rules]
    tie_pattern = source_tie_rule.pattern
    x0 = tie_pattern.user_center
    y0 = tie_pattern.item_center
    tie_pw = source_tie_rule.get_point_wise_predicates()
    pivot_timing: Optional[Dict[str, float]] = {} if timing_out is not None else None
    _t_phase = time.perf_counter()
    _, item_candidates = get_pivot_candidates(graph, tie_pattern, tie_pw, timing_out=pivot_timing)
    _record_timing(timing_out, 'pivot_candidates_total', _t_phase)
    if timing_out is not None and pivot_timing is not None:
        for phase_name, elapsed in pivot_timing.items():
            timing_out[f'pivot_candidates.{phase_name}'] = timing_out.get(f'pivot_candidates.{phase_name}', 0.0) + float(elapsed)
    tie_required_groups = _get_required_edge_groups_cached(tie_pattern)
    tie_var_candidates: Dict[str, frozenset] = {}
    _t_phase = time.perf_counter()
    for var in tie_pattern.nodes:
        if var in (x0, y0):
            continue
        pnode = tie_pattern.nodes[var]
        tie_var_candidates[var] = frozenset(_get_static_var_candidates(graph, pnode.label, tie_pw.get(var, []), tie_required_groups.get(var, set()), timing_out=timing_out))
    _record_timing(timing_out, 'tie_static_var_candidates_total', _t_phase)
    omr_var_candidates_by_rule: Dict[Any, Dict[str, frozenset]] = {}
    _t_phase = time.perf_counter()
    for source_omr in source_omr_rules:
        omr_key = _rule_source_identity(source_omr)
        omr_pat = source_omr.pattern
        omr_pw = source_omr.get_point_wise_predicates()
        omr_required_groups = _get_required_edge_groups_cached(omr_pat)
        per_rule_candidates: Dict[str, frozenset] = {}
        for var in omr_pat.get_omr_only_variables():
            pnode = omr_pat.nodes.get(var)
            if pnode is None:
                continue
            per_rule_candidates[var] = frozenset(_get_static_var_candidates(graph, pnode.label, omr_pw.get(var, []), omr_required_groups.get(var, set()), timing_out=timing_out))
        omr_var_candidates_by_rule[omr_key] = per_rule_candidates
    _record_timing(timing_out, 'omr_blueprint_build_total', _t_phase)
    blueprint = {'item_candidates': frozenset(item_candidates), 'tie_var_candidates': tie_var_candidates, 'omr_var_candidates_by_rule': omr_var_candidates_by_rule}
    _GROUP_INIT_STATIC_BLUEPRINT_CACHE[key] = (graph, tie_rule_key, omr_rule_keys, blueprint)
    while len(_GROUP_INIT_STATIC_BLUEPRINT_CACHE) > _GROUP_INIT_STATIC_BLUEPRINT_CACHE_MAX:
        _GROUP_INIT_STATIC_BLUEPRINT_CACHE.popitem(last=False)
    return blueprint

def _get_omr_children_by_parent(sigma_omr: List['OMRule']) -> Dict[Tuple[str, Any], List['OMRule']]:
    cache_key = id(sigma_omr)
    cached = _OMR_CHILDREN_CACHE.get(cache_key)
    if cached is not None:
        cached_sigma, cached_value = cached
        if cached_sigma is sigma_omr:
            _OMR_CHILDREN_CACHE.move_to_end(cache_key)
            return cached_value
        _OMR_CHILDREN_CACHE.pop(cache_key, None)
    by_parent: Dict[Tuple[str, Any], List['OMRule']] = defaultdict(list)
    for omr in sigma_omr:
        parent_sig = canonical_tie_rule_signature(omr.parent_tie)
        by_parent['id', id(omr.parent_tie)].append(omr)
        by_parent['sig', parent_sig].append(omr)
    materialized = dict(by_parent)
    _OMR_CHILDREN_CACHE[cache_key] = (sigma_omr, materialized)
    while len(_OMR_CHILDREN_CACHE) > 32:
        _OMR_CHILDREN_CACHE.popitem(last=False)
    return materialized

def _get_rule_group_shape_cached(graph: Graph, rules: List[Union[TIERule, 'OMRule']], root_var: str, cache_stats_out: Optional[Dict[str, int]]=None) -> Tuple[JointPattern, RootedDAG]:
    rule_keys = tuple((_rule_cache_identity(rule) for rule in rules))
    key = (id(graph), root_var, rule_keys)
    cached = _RULE_GROUP_SHAPE_CACHE.get(key)
    if cached is not None:
        cached_graph, cached_rule_keys, cached_jp, cached_dag = cached
        if cached_graph is graph and cached_rule_keys == rule_keys:
            _increment_counter(cache_stats_out, 'shape_hits')
            _RULE_GROUP_SHAPE_CACHE.move_to_end(key)
            return (cached_jp, cached_dag)
        _RULE_GROUP_SHAPE_CACHE.pop(key, None)
    _increment_counter(cache_stats_out, 'shape_misses')
    jp = JointPattern.from_rule_group(rules)
    dag = RootedDAG.from_joint_pattern(jp, root_var, graph)
    _RULE_GROUP_SHAPE_CACHE[key] = (graph, rule_keys, jp, dag)
    while len(_RULE_GROUP_SHAPE_CACHE) > _RULE_GROUP_SHAPE_CACHE_MAX:
        _RULE_GROUP_SHAPE_CACHE.popitem(last=False)
    return (jp, dag)

def _get_rule_group_runtime_cached(graph: Graph, rules: List[Union[TIERule, 'OMRule']], compact_graph: Optional['CompactGraph']=None, per_rule_timing: Optional[Dict[int, float]]=None, cache_stats_out: Optional[Dict[str, int]]=None) -> Dict[str, Any]:
    compact_id = id(compact_graph) if compact_graph is not None else 0
    rule_keys = tuple((_rule_cache_identity(rule) for rule in rules))
    key = (id(graph), compact_id, rule_keys)
    cached = _RULE_GROUP_RUNTIME_CACHE.get(key)
    if cached is not None:
        cached_graph, cached_compact, cached_rule_keys, cached_value = cached
        if cached_graph is graph and cached_compact is compact_graph and (cached_rule_keys == rule_keys):
            _increment_counter(cache_stats_out, 'runtime_hits')
            _RULE_GROUP_RUNTIME_CACHE.move_to_end(key)
            return cached_value
        _RULE_GROUP_RUNTIME_CACHE.pop(key, None)
    _increment_counter(cache_stats_out, 'runtime_misses')
    prefilter_caches: Dict[int, Dict[str, Any]] = {}
    edge_lookups_per_rule: Dict[int, Dict[str, List[Dict]]] = {}
    star_contexts_per_rule: Dict[int, Dict[str, Any]] = {}
    rule_runtime_per_rule: Dict[int, Dict[str, Any]] = {}
    for rule_idx, rule in enumerate(rules):
        _t_rule = time.perf_counter()
        rule_pw = rule.get_point_wise_predicates()
        if isinstance(rule, TIERule):
            pat = rule.pattern
            pvars = {pat.user_center, pat.item_center}
            edge_lookups_per_rule[rule_idx] = {'user': _build_edge_lookups(pat.user_paths), 'item': _build_edge_lookups(pat.item_paths)}
            prefilter_caches[rule_idx] = _build_prefilter_cache(graph, pat, rule_pw, pvars)
            star_contexts_per_rule[rule_idx] = {'user': build_star_match_context(graph, pat, pat.user_paths, edge_lookups_per_rule[rule_idx]['user'], rule_pw, prefilter_caches[rule_idx], compact_graph) if pat.user_paths else {'compact_graph': compact_graph}, 'item': build_star_match_context(graph, pat, pat.item_paths, edge_lookups_per_rule[rule_idx]['item'], rule_pw, prefilter_caches[rule_idx], compact_graph) if pat.item_paths else {'compact_graph': compact_graph}}
            rule_runtime_per_rule[rule_idx] = {}
        else:
            pat = rule.pattern
            tie_pat = pat.tie_pattern
            pvars = {pat.user_center, pat.anchor_center, pat.alt_center}
            alt_paths = tuple(pat.alt_star.paths if hasattr(pat.alt_star, 'paths') else ())
            edge_lookups_per_rule[rule_idx] = {'user': _build_edge_lookups(tie_pat.user_paths), 'item': _build_edge_lookups(tie_pat.item_paths), 'alt': _build_edge_lookups(alt_paths)}
            prefilter_caches[rule_idx] = _build_prefilter_cache(graph, pat, rule_pw, pvars)
            star_contexts_per_rule[rule_idx] = {'user': build_star_match_context(graph, tie_pat, tie_pat.user_paths, edge_lookups_per_rule[rule_idx]['user'], rule_pw, prefilter_caches[rule_idx], compact_graph) if tie_pat.user_paths else {'compact_graph': compact_graph}, 'item': build_star_match_context(graph, tie_pat, tie_pat.item_paths, edge_lookups_per_rule[rule_idx]['item'], rule_pw, prefilter_caches[rule_idx], compact_graph) if tie_pat.item_paths else {'compact_graph': compact_graph}, 'alt': build_star_match_context(graph, pat, alt_paths, edge_lookups_per_rule[rule_idx]['alt'], rule_pw, prefilter_caches[rule_idx], compact_graph) if alt_paths else {'compact_graph': compact_graph}}
            omr_extra_pw = rule.get_omr_only_point_wise()
            tie_star_vars = set(tie_pat.nodes.keys()) - {pat.user_center, pat.anchor_center}
            safe_tie_leaf_vars = _get_safe_tie_star_leaf_vars(tie_pat)
            safe_tie_leaf_extra_pw = {var: tuple(preds) for var, preds in omr_extra_pw.items() if var in safe_tie_leaf_vars}
            unsafe_tie_star_extra_vars = {var for var in omr_extra_pw if var in tie_star_vars and var not in safe_tie_leaf_vars}
            bridge_paths = tuple(pat.bridge_paths)
            bridge_origins = tuple(pat.bridge_origins)
            rule_runtime_per_rule[rule_idx] = {'omr_extra_pw': omr_extra_pw, 'has_extra_on_tie_stars': bool(any((var in tie_star_vars for var in omr_extra_pw))), 'has_unsafe_extra_on_tie_stars': bool(unsafe_tie_star_extra_vars), 'safe_tie_leaf_extra_pw': safe_tie_leaf_extra_pw, 'y0_extra_preds': tuple(omr_extra_pw.get(pat.anchor_center, [])), 'w0_extra_preds': tuple(omr_extra_pw.get(pat.alt_center, [])), 'y0_extra_prefilter': _prefilter_by_attribute_index(graph, pat.nodes[pat.anchor_center].label, list(omr_extra_pw.get(pat.anchor_center, []))) if omr_extra_pw.get(pat.anchor_center) else None, 'w0_extra_prefilter': _prefilter_by_attribute_index(graph, pat.nodes[pat.alt_center].label, list(omr_extra_pw.get(pat.alt_center, []))) if omr_extra_pw.get(pat.alt_center) else None, 'anchor_ml_preds': tuple((pred for pred in rule.get_ml_predicates() if pred.user_var == pat.user_center and pred.item_var == pat.anchor_center)), 'alt_ml_preds': tuple((pred for pred in rule.get_ml_predicates() if pred.user_var == pat.user_center and pred.item_var == pat.alt_center)), 'bridge_paths': bridge_paths, 'bridge_origins': bridge_origins, 'bridge_edge_lookups': [{(e.source_var, e.target_var): (e.label, e.is_forward) for e in bridge_path.edges} for bridge_path in bridge_paths], 'bridge_contexts': [build_bridge_match_context(graph, pat, bridge_path, rule_pw, prefilter_caches[rule_idx], compact_graph) for bridge_path in bridge_paths], 'alt_paths': alt_paths, 'alt_edge_lookups': edge_lookups_per_rule[rule_idx]['alt']}
        if per_rule_timing is not None:
            per_rule_timing[rule_idx] = float(time.perf_counter() - _t_rule)
    runtime = {'prefilter_caches': prefilter_caches, 'edge_lookups_per_rule': edge_lookups_per_rule, 'star_contexts_per_rule': star_contexts_per_rule, 'rule_runtime_per_rule': rule_runtime_per_rule}
    _RULE_GROUP_RUNTIME_CACHE[key] = (graph, compact_graph, rule_keys, runtime)
    while len(_RULE_GROUP_RUNTIME_CACHE) > _RULE_GROUP_RUNTIME_CACHE_MAX:
        _RULE_GROUP_RUNTIME_CACHE.popitem(last=False)
    return runtime

def _get_group_init_blueprint(graph: Graph, tie_rule: TIERule, omr_rules: List['OMRule'], timing_out: Optional[Dict[str, float]]=None, cache_stats_out: Optional[Dict[str, int]]=None) -> Dict[str, Any]:
    tie_rule_key = _rule_cache_identity(tie_rule)
    omr_rule_keys = tuple((_rule_cache_identity(rule) for rule in omr_rules))
    key = (id(graph), tie_rule_key, omr_rule_keys)
    cached = _GROUP_INIT_BLUEPRINT_CACHE.get(key)
    if cached is not None:
        cached_graph, cached_tie_key, cached_omr_keys, cached_blueprint = cached
        if cached_graph is graph and cached_tie_key == tie_rule_key and (cached_omr_keys == omr_rule_keys):
            _increment_counter(cache_stats_out, 'blueprint_hits')
            _GROUP_INIT_BLUEPRINT_CACHE.move_to_end(key)
            return cached_blueprint
        _GROUP_INIT_BLUEPRINT_CACHE.pop(key, None)
    _increment_counter(cache_stats_out, 'blueprint_misses')
    tie_pattern = tie_rule.pattern
    x0 = tie_pattern.user_center
    y0 = tie_pattern.item_center
    tie_pw = tie_rule.get_point_wise_predicates()
    static_blueprint = _get_group_init_static_blueprint(graph, tie_rule, omr_rules, timing_out=timing_out, cache_stats_out=cache_stats_out)
    all_pw: Dict[str, List[PointWisePredicate]] = {}
    _t_phase = time.perf_counter()
    for rule in [tie_rule] + list(omr_rules):
        for var, preds in rule.get_point_wise_predicates().items():
            all_pw.setdefault(var, []).extend(preds)
    for var in all_pw:
        seen_ids = set()
        deduped: List[PointWisePredicate] = []
        for pred in all_pw[var]:
            pid = id(pred)
            if pid in seen_ids:
                continue
            seen_ids.add(pid)
            deduped.append(pred)
        all_pw[var] = deduped
    _record_timing(timing_out, 'merge_pointwise_preds', _t_phase)
    _t_phase = time.perf_counter()
    item_candidates = set(static_blueprint['item_candidates'])
    tie_var_candidates = _rename_var_candidate_map(static_blueprint['tie_var_candidates'], getattr(tie_rule, '_source_var_map', None))
    omr_var_candidates: Dict[str, frozenset] = {}
    path_anchor_jobs: List[Tuple['OMRule', Dict[str, List[PointWisePredicate]], Tuple[str, ...], Tuple[Path, ...], Tuple[Path, ...], str]] = []
    w0_aggr_constraints: Dict[str, List[AggrPredicate]] = {}
    for omr in omr_rules:
        source_var_map = getattr(omr, '_source_var_map', None)
        source_rule_key = _rule_source_identity(omr)
        source_rule_candidates = static_blueprint['omr_var_candidates_by_rule'].get(source_rule_key, {})
        renamed_rule_candidates = _rename_var_candidate_map(source_rule_candidates, source_var_map)
        for var, rule_cands in renamed_rule_candidates.items():
            if var in omr_var_candidates:
                omr_var_candidates[var] = frozenset(set(omr_var_candidates[var]) | set(rule_cands))
            else:
                omr_var_candidates[var] = frozenset(rule_cands)
        omr_pat = omr.pattern
        omr_pw = omr.get_point_wise_predicates()
        pivots_this: Set[str] = set(omr_pat.bridge_origins)
        pivots_this.add(omr_pat.alt_center)
        alt_paths = tuple(omr_pat.alt_star.paths if hasattr(omr_pat.alt_star, 'paths') else ())
        path_anchor_jobs.append((omr, omr_pw, tuple(sorted(pivots_this)), tuple(omr_pat.bridge_paths), alt_paths, omr_pat.alt_center))
        aggr_preds = omr.get_aggr_predicates()
        if aggr_preds:
            w0_aggr_constraints.setdefault(omr_pat.alt_center, []).extend(aggr_preds)
    _record_timing(timing_out, 'blueprint_static_copy_and_jobs', _t_phase)
    blueprint = {'all_pw': all_pw, 'item_candidates': frozenset(item_candidates), 'tie_var_candidates': tie_var_candidates, 'omr_var_candidates': omr_var_candidates, 'path_anchor_jobs': path_anchor_jobs, 'w0_aggr_constraints': w0_aggr_constraints}
    _GROUP_INIT_BLUEPRINT_CACHE[key] = (graph, tie_rule_key, omr_rule_keys, blueprint)
    while len(_GROUP_INIT_BLUEPRINT_CACHE) > _GROUP_INIT_BLUEPRINT_CACHE_MAX:
        _GROUP_INIT_BLUEPRINT_CACHE.popitem(last=False)
    return blueprint

def _merge_candidate_maps(dst: Dict[str, Set[Any]], src: Dict[str, Set[Any]]) -> bool:
    for var, cands in src.items():
        cand_set = set(cands)
        if var in dst:
            merged = dst[var] & cand_set
            if not merged:
                return False
            dst[var] = merged
        else:
            dst[var] = cand_set
    return True

def _build_direct_bridge_index_for_rule(graph: Graph, rule: OMRule, *, origin: str, origin_ids: Set[Any], training_items: Optional[Set[Any]]=None) -> Optional[Any]:
    if not origin_ids:
        return None
    omr_pattern = rule.pattern
    w0 = omr_pattern.alt_center
    pw_predicates = rule.get_point_wise_predicates()
    try:
        from omr_miner import BridgeReachabilityIndex
    except Exception:
        return None
    direct_indexes = []
    for bridge_path, bridge_origin in zip(omr_pattern.bridge_paths, omr_pattern.bridge_origins):
        if bridge_origin != origin:
            continue
        path_seq = getattr(bridge_path, 'path_sequence', ())
        if not path_seq or path_seq[-1] != w0:
            return None
        direct_indexes.append(BridgeReachabilityIndex.build(graph, bridge_path, origin_ids, w_var=w0, pw_predicates=pw_predicates, training_items=training_items))
    if not direct_indexes:
        return None
    if len(direct_indexes) == 1:
        return direct_indexes[0]

    class _MergedBridgeReachabilityIndex:

        def __init__(self, indexes):
            self._indexes = tuple(indexes)

        def get_reachable(self, origin_id: Any) -> frozenset:
            merged: Set[Any] = set()
            for index in self._indexes:
                merged |= set(index.get_reachable(origin_id))
            return frozenset(merged)

        def get_bvc(self, origin_id: Any) -> Dict[str, Set[Any]]:
            merged: Dict[str, Set[Any]] = {}
            for index in self._indexes:
                for var, cands in index.get_bvc(origin_id).items():
                    merged.setdefault(var, set()).update(cands)
            return merged
    return _MergedBridgeReachabilityIndex(direct_indexes)

def _build_ctr_precompute_w_candidates_by_anchor(*, graph: Graph, user_id: Any, omr_rules_in_H: List[OMRule], y0_candidates: Set[Any], w0_candidates: Set[Any], bridge_idx_per_rule: Dict[Any, Any]) -> Dict[Any, Set[Any]]:
    if not y0_candidates or not w0_candidates or (not omr_rules_in_H):
        return {v: set(w0_candidates) for v in y0_candidates}
    global_x0_reachable = set()
    requires_full_w_space = False
    per_rule_y0_indexes: Dict[int, Any] = {}
    user_origin_ids = {user_id}
    for local_idx, rule in enumerate(omr_rules_in_H):
        if not rule.pattern.bridge_paths:
            requires_full_w_space = True
            break
        y0_index = bridge_idx_per_rule.get(local_idx)
        if y0_index is None:
            y0_index = _build_direct_bridge_index_for_rule(graph, rule, origin=rule.pattern.anchor_center, origin_ids=y0_candidates, training_items=w0_candidates)
            if y0_index is not None:
                bridge_idx_per_rule[local_idx] = y0_index
        if y0_index is not None:
            per_rule_y0_indexes[local_idx] = y0_index
        x0_key = ('x0', local_idx)
        x0_index = bridge_idx_per_rule.get(x0_key)
        if x0_index is None:
            x0_index = _build_direct_bridge_index_for_rule(graph, rule, origin=rule.pattern.user_center, origin_ids=user_origin_ids, training_items=w0_candidates)
            if x0_index is not None:
                bridge_idx_per_rule[x0_key] = x0_index
        if x0_index is not None:
            global_x0_reachable |= set(x0_index.get_reachable(user_id))
        has_anchor_bridge = any((origin == rule.pattern.anchor_center for origin in rule.pattern.bridge_origins))
        has_user_bridge = any((origin == rule.pattern.user_center for origin in rule.pattern.bridge_origins))
        if has_anchor_bridge and local_idx not in per_rule_y0_indexes:
            requires_full_w_space = True
            break
        if has_user_bridge and x0_index is None:
            requires_full_w_space = True
            break
    if requires_full_w_space:
        return {v: set(w0_candidates) for v in y0_candidates}
    by_anchor: Dict[Any, Set[Any]] = {}
    for v in y0_candidates:
        reachable = set(global_x0_reachable)
        for index in per_rule_y0_indexes.values():
            reachable |= set(index.get_reachable(v))
        by_anchor[v] = reachable & w0_candidates if reachable else set(w0_candidates)
    return by_anchor

def _candidate_map_signature(candidate_map: Dict[str, Set[Any]]) -> Tuple[Tuple[str, frozenset[Any]], ...]:
    return tuple(((var, frozenset(candidate_map.get(var, set()))) for var in sorted(candidate_map)))

def _get_dag_propagation_steps(rooted_dag: Optional[RootedDAG]) -> Tuple[List[Tuple[str, str, Tuple[Tuple[str, bool], ...]]], List[Tuple[str, str, Tuple[Tuple[str, bool], ...]]]]:
    if rooted_dag is None:
        return ([], [])
    cached = getattr(rooted_dag, '_propagation_steps_cache', None)
    if cached is not None:
        return cached
    forward_steps: List[Tuple[str, str, Tuple[Tuple[str, bool], ...]]] = []
    for var in rooted_dag.get_topological_order():
        for child_var in rooted_dag.children.get(var, []):
            edges_info = rooted_dag.edge_info.get((var, child_var), [])
            if edges_info:
                forward_steps.append((var, child_var, tuple(edges_info)))
    backward_steps: List[Tuple[str, str, Tuple[Tuple[str, bool], ...]]] = []
    for var in rooted_dag.get_reverse_topological_order():
        for parent_var in rooted_dag.parents.get(var, []):
            edges_info = rooted_dag.edge_info.get((parent_var, var), [])
            if edges_info:
                backward_steps.append((parent_var, var, tuple(edges_info)))
    cached = (forward_steps, backward_steps)
    try:
        setattr(rooted_dag, '_propagation_steps_cache', cached)
    except Exception:
        pass
    return cached

def _get_dag_runtime_cached(rooted_dag: Optional[RootedDAG], rule_vars: Optional[Set[str]]=None, allowed_edge_info: Optional[Dict[Tuple[str, str], Set[Tuple[str, bool]]]]=None) -> Tuple[List[Tuple[str, str, Tuple[Tuple[str, bool], ...]]], List[Tuple[str, str, Tuple[Tuple[str, bool], ...]]], Tuple[str, ...], Dict[str, int]]:
    if rooted_dag is None:
        return ([], [], (), {'step_count': 0, 'join_node_count': 0, 'skip_candidate': 0})
    allowed_edge_key = None
    if allowed_edge_info is not None:
        allowed_edge_key = tuple(sorted(((parent_var, child_var, tuple(sorted(edges_info))) for (parent_var, child_var), edges_info in allowed_edge_info.items())))
    rule_vars_key = (None if rule_vars is None else frozenset(rule_vars), allowed_edge_key)
    cache_store = getattr(rooted_dag, '_propagation_runtime_cache', None)
    if cache_store is None or not isinstance(cache_store, dict):
        cache_store = {}
        try:
            setattr(rooted_dag, '_propagation_runtime_cache', cache_store)
        except Exception:
            cache_store = {}
    cached = cache_store.get(rule_vars_key)
    if cached is not None:
        return cached
    forward_steps, backward_steps = _get_dag_propagation_steps(rooted_dag)
    if rule_vars is not None:
        forward_steps = [(parent_var, child_var, edges_info) for parent_var, child_var, edges_info in forward_steps if parent_var in rule_vars and child_var in rule_vars]
        backward_steps = [(parent_var, child_var, edges_info) for parent_var, child_var, edges_info in backward_steps if parent_var in rule_vars and child_var in rule_vars]
    if allowed_edge_info is not None:
        filtered_forward_steps = []
        for parent_var, child_var, edges_info in forward_steps:
            allowed = allowed_edge_info.get((parent_var, child_var), set())
            kept = tuple((edge for edge in edges_info if edge in allowed))
            if kept:
                filtered_forward_steps.append((parent_var, child_var, kept))
        forward_steps = filtered_forward_steps
        filtered_backward_steps = []
        for parent_var, child_var, edges_info in backward_steps:
            allowed = allowed_edge_info.get((parent_var, child_var), set())
            kept = tuple((edge for edge in edges_info if edge in allowed))
            if kept:
                filtered_backward_steps.append((parent_var, child_var, kept))
        backward_steps = filtered_backward_steps
    dag_vars: List[str] = []
    seen: Set[str] = set()
    for parent_var, child_var, _edges_info in forward_steps:
        if parent_var not in seen:
            seen.add(parent_var)
            dag_vars.append(parent_var)
        if child_var not in seen:
            seen.add(child_var)
            dag_vars.append(child_var)
    for parent_var, child_var, _edges_info in backward_steps:
        if parent_var not in seen:
            seen.add(parent_var)
            dag_vars.append(parent_var)
        if child_var not in seen:
            seen.add(child_var)
            dag_vars.append(child_var)
    child_counts: Dict[str, int] = defaultdict(int)
    for _parent_var, child_var, _edges_info in forward_steps:
        child_counts[child_var] += 1
    join_node_count = sum((1 for count in child_counts.values() if count > 1))
    step_count = len(forward_steps) + len(backward_steps)
    runtime_meta = {'step_count': int(step_count), 'join_node_count': int(join_node_count), 'skip_candidate': int(join_node_count == 0 and step_count <= 2)}
    cached = (forward_steps, backward_steps, tuple(dag_vars), runtime_meta)
    try:
        cache_store[rule_vars_key] = cached
    except Exception:
        pass
    return cached

def _required_edge_groups(pattern) -> Dict[str, Set[Tuple[str, bool]]]:
    groups: Dict[str, Set[Tuple[str, bool]]] = defaultdict(set)
    for edge in pattern.edges:
        groups[edge.source_var].add((edge.label, bool(edge.is_forward)))
        groups[edge.target_var].add((edge.label, not bool(edge.is_forward)))
    return groups

def _get_rule_dag_edge_filter(pattern) -> Dict[Tuple[str, str], Set[Tuple[str, bool]]]:
    cached = getattr(pattern, '_dag_edge_filter_cache', None)
    if cached is not None:
        return cached
    edge_filter: Dict[Tuple[str, str], Set[Tuple[str, bool]]] = defaultdict(set)
    for edge in pattern.edges:
        edge_filter[edge.source_var, edge.target_var].add((edge.label, bool(edge.is_forward)))
        edge_filter[edge.target_var, edge.source_var].add((edge.label, not bool(edge.is_forward)))
    result = {key: set(values) for key, values in edge_filter.items()}
    try:
        setattr(pattern, '_dag_edge_filter_cache', result)
    except Exception:
        pass
    return result

def _nodes_with_required_edge_groups(graph: Graph, candidates: Set[Any], required_groups: Set[Tuple[str, bool]]) -> Set[Any]:
    if not candidates or not required_groups:
        return set(candidates)
    surviving = set(candidates)
    for edge_label, needs_outgoing in required_groups:
        supported = graph.get_source_nodes_of_edge(edge_label) if needs_outgoing else graph.get_target_nodes_of_edge(edge_label)
        surviving &= supported
        if not surviving:
            break
    return surviving

def _get_safe_tie_star_leaf_vars(tie_pattern) -> Set[str]:
    degree_by_var: Dict[str, int] = defaultdict(int)
    for edge in tie_pattern.edges:
        degree_by_var[edge.source_var] += 1
        degree_by_var[edge.target_var] += 1
    safe_leaf_vars: Set[str] = set()
    for path in list(getattr(tie_pattern, 'user_paths', ())) + list(getattr(tie_pattern, 'item_paths', ())):
        path_seq = getattr(path, 'path_sequence', ())
        if len(path_seq) < 2:
            continue
        leaf_var = path_seq[-1]
        if degree_by_var.get(leaf_var, 0) == 1:
            safe_leaf_vars.add(leaf_var)
    return safe_leaf_vars

def _post_filter_leaf_star_match(graph: Graph, star_match: Dict[str, Set[Any]], extra_leaf_preds: Dict[str, Tuple[PointWisePredicate, ...]]) -> Optional[Dict[str, Set[Any]]]:
    if not extra_leaf_preds:
        return star_match
    node_cache: Dict[Any, Optional[Node]] = {}
    filtered_match: Optional[Dict[str, Set[Any]]] = None
    for var, preds in extra_leaf_preds.items():
        cands = star_match.get(var)
        if not cands:
            continue
        filtered = {nid for nid in cands if node_cache.setdefault(nid, graph.get_node(nid)) is not None and all((pred.evaluate(node_cache[nid]) for pred in preds))}
        if not filtered:
            return None
        if filtered != cands:
            if filtered_match is None:
                filtered_match = {match_var: set(match_cands) for match_var, match_cands in star_match.items()}
            filtered_match[var] = filtered
    return filtered_match if filtered_match is not None else star_match

def _propagate_candidates_with_dag(graph: Graph, rooted_dag: Optional[RootedDAG], F: Dict[str, Set[Any]], compact_graph: Optional['CompactGraph']=None, rule_vars: Optional[Set[str]]=None, allowed_edge_info: Optional[Dict[Tuple[str, str], Set[Tuple[str, bool]]]]=None) -> Optional[Dict[str, Set[Any]]]:
    if rooted_dag is None:
        return F
    forward_steps, backward_steps, dag_vars, runtime_meta = _get_dag_runtime_cached(rooted_dag, rule_vars=rule_vars, allowed_edge_info=allowed_edge_info)
    if not forward_steps and (not backward_steps):
        return F
    diag_enabled = _dag_diagnostics_enabled()
    if diag_enabled:
        _DAG_PROPAGATION_DIAGNOSTICS['calls_total'] += 1
        _DAG_PROPAGATION_DIAGNOSTICS['calls_step_count_total'] += int(runtime_meta.get('step_count', 0))
        if runtime_meta.get('join_node_count', 0) == 0:
            _DAG_PROPAGATION_DIAGNOSTICS['calls_no_join'] += 1
        if runtime_meta.get('skip_candidate', 0):
            _DAG_PROPAGATION_DIAGNOSTICS['calls_skip_candidate'] += 1
    total_before = 0
    if diag_enabled:
        total_before = sum((len(F.get(var, set())) for var in dag_vars))
        _DAG_PROPAGATION_DIAGNOSTICS['nodes_before_total'] += int(total_before)
    if runtime_meta.get('skip_candidate', 0) and _dag_skip_no_join_enabled():
        if diag_enabled:
            _DAG_PROPAGATION_DIAGNOSTICS['calls_skipped'] += 1
            _DAG_PROPAGATION_DIAGNOSTICS['nodes_after_total'] += int(total_before)
        return F

    def _record_diag_exit(returned_none: bool=False) -> None:
        if not diag_enabled:
            return
        if returned_none:
            _DAG_PROPAGATION_DIAGNOSTICS['calls_return_none'] += 1
            return
        total_after = sum((len(F.get(var, set())) for var in dag_vars))
        _DAG_PROPAGATION_DIAGNOSTICS['nodes_after_total'] += int(total_after)
        pruned = max(int(total_before - total_after), 0)
        _DAG_PROPAGATION_DIAGNOSTICS['nodes_pruned_total'] += pruned
        if pruned > 0:
            _DAG_PROPAGATION_DIAGNOSTICS['calls_pruned'] += 1
            if runtime_meta.get('skip_candidate', 0):
                _DAG_PROPAGATION_DIAGNOSTICS['calls_pruned_skip_candidate'] += 1
    for parent_var, child_var, _edges_info in forward_steps:
        if not F.get(parent_var) or not F.get(child_var):
            _record_diag_exit(returned_none=True)
            return None
    for parent_var, child_var, _edges_info in backward_steps:
        if not F.get(parent_var) or not F.get(child_var):
            _record_diag_exit(returned_none=True)
            return None
    if compact_graph is not None:
        id_to_int = compact_graph.id_to_int
        int_to_id = compact_graph.int_to_id
        F_int: Dict[str, Set[int]] = {}
        for var in dag_vars:
            cands = F.get(var)
            if not cands:
                continue
            F_int[var] = {id_to_int[nid] for nid in cands if nid in id_to_int}
        if _HAS_FAST_DAG:
            propagated = _cy_propagate_dag(compact_graph, forward_steps, backward_steps, F_int)
            if propagated is None:
                _record_diag_exit(returned_none=True)
                return None
            for var, cands in propagated.items():
                F[var] = {int_to_id[n_int] for n_int in cands}
            _record_diag_exit()
            return F
        out_neighbors = compact_graph.out_neighbors
        in_neighbors = compact_graph.in_neighbors
        for parent_var, child_var, edges_info in forward_steps:
            parent_ints = F_int.get(parent_var)
            child_ints = F_int.get(child_var)
            if not parent_ints or not child_ints:
                _record_diag_exit(returned_none=True)
                return None
            reachable_child_ints: Set[int] = set()
            n_child = len(child_ints)
            for edge_label, is_fwd in edges_info:
                if len(parent_ints) <= len(child_ints):
                    neighbor_map = out_neighbors if is_fwd else in_neighbors
                    for p_int in parent_ints:
                        for n_int in neighbor_map.get((p_int, edge_label), ()):
                            if n_int in child_ints:
                                reachable_child_ints.add(n_int)
                        if len(reachable_child_ints) == n_child:
                            break
                else:
                    reverse_neighbor_map = in_neighbors if is_fwd else out_neighbors
                    for c_int in child_ints:
                        for n_int in reverse_neighbor_map.get((c_int, edge_label), ()):
                            if n_int in parent_ints:
                                reachable_child_ints.add(c_int)
                                break
                        if len(reachable_child_ints) == n_child:
                            break
                if len(reachable_child_ints) == n_child:
                    break
            if not reachable_child_ints:
                _record_diag_exit(returned_none=True)
                return None
            F_int[child_var] = reachable_child_ints
        for parent_var, child_var, edges_info in backward_steps:
            child_ints = F_int.get(child_var)
            parent_ints = F_int.get(parent_var)
            if not child_ints or not parent_ints:
                _record_diag_exit(returned_none=True)
                return None
            reachable_parent_ints: Set[int] = set()
            n_parent = len(parent_ints)
            for edge_label, is_fwd in edges_info:
                if len(child_ints) <= len(parent_ints):
                    reverse_neighbor_map = in_neighbors if is_fwd else out_neighbors
                    for c_int in child_ints:
                        for n_int in reverse_neighbor_map.get((c_int, edge_label), ()):
                            if n_int in parent_ints:
                                reachable_parent_ints.add(n_int)
                        if len(reachable_parent_ints) == n_parent:
                            break
                else:
                    forward_neighbor_map = out_neighbors if is_fwd else in_neighbors
                    for p_int in parent_ints:
                        for n_int in forward_neighbor_map.get((p_int, edge_label), ()):
                            if n_int in child_ints:
                                reachable_parent_ints.add(p_int)
                                break
                        if len(reachable_parent_ints) == n_parent:
                            break
                if len(reachable_parent_ints) == n_parent:
                    break
            if not reachable_parent_ints:
                _record_diag_exit(returned_none=True)
                return None
            F_int[parent_var] = reachable_parent_ints
        for var, cands in F_int.items():
            F[var] = {int_to_id[n_int] for n_int in cands}
        _record_diag_exit()
        return F
    out_by_label = getattr(graph, '_out_edges_by_label', {})
    in_by_label = getattr(graph, '_in_edges_by_label', {})
    for parent_var, child_var, edges_info in forward_steps:
        parent_cands = F.get(parent_var)
        child_cands = F.get(child_var)
        if not parent_cands or not child_cands:
            _record_diag_exit(returned_none=True)
            return None
        reachable_children = set()
        n_child = len(child_cands)
        for edge_label, is_fwd in edges_info:
            if len(parent_cands) <= len(child_cands):
                edge_view = out_by_label if is_fwd else in_by_label
                for p_id in parent_cands:
                    for e in edge_view.get((p_id, edge_label), ()):
                        n_id = e.target if is_fwd else e.source
                        if n_id in child_cands:
                            reachable_children.add(n_id)
                    if len(reachable_children) == n_child:
                        break
            else:
                reverse_edge_view = in_by_label if is_fwd else out_by_label
                for c_id in child_cands:
                    for e in reverse_edge_view.get((c_id, edge_label), ()):
                        n_id = e.source if is_fwd else e.target
                        if n_id in parent_cands:
                            reachable_children.add(c_id)
                            break
                    if len(reachable_children) == n_child:
                        break
            if len(reachable_children) == n_child:
                break
        F[child_var] = reachable_children
        if not F[child_var]:
            _record_diag_exit(returned_none=True)
            return None
    for parent_var, child_var, edges_info in backward_steps:
        child_cands = F.get(child_var)
        parent_cands = F.get(parent_var)
        if not child_cands or not parent_cands:
            _record_diag_exit(returned_none=True)
            return None
        reachable_parents = set()
        n_parent = len(parent_cands)
        for edge_label, is_fwd in edges_info:
            if len(child_cands) <= len(parent_cands):
                reverse_edge_view = in_by_label if is_fwd else out_by_label
                for c_id in child_cands:
                    for e in reverse_edge_view.get((c_id, edge_label), ()):
                        n_id = e.source if is_fwd else e.target
                        if n_id in parent_cands:
                            reachable_parents.add(n_id)
                    if len(reachable_parents) == n_parent:
                        break
            else:
                forward_edge_view = out_by_label if is_fwd else in_by_label
                for p_id in parent_cands:
                    for e in forward_edge_view.get((p_id, edge_label), ()):
                        n_id = e.target if is_fwd else e.source
                        if n_id in child_cands:
                            reachable_parents.add(p_id)
                            break
                    if len(reachable_parents) == n_parent:
                        break
            if len(reachable_parents) == n_parent:
                break
        F[parent_var] = reachable_parents
        if not F[parent_var]:
            _record_diag_exit(returned_none=True)
            return None
    _record_diag_exit()
    return F

def construct_rule_group(user_id: Any, tie_rule: TIERule, sigma_omr: List[OMRule], graph: Graph) -> Tuple[List[Union[TIERule, OMRule]], JointPattern, RootedDAG]:
    H: List[Union[TIERule, OMRule]] = [tie_rule]
    user_node = graph.get_node(user_id)
    if user_node is None:
        jp, dag = _get_rule_group_shape_cached(graph, H, tie_rule.pattern.user_center)
        return (H, jp, dag)
    parent_registry = _get_omr_children_by_parent(sigma_omr)
    child_omrs = list(parent_registry.get(('id', id(tie_rule)), []))
    if not child_omrs:
        child_omrs = list(parent_registry.get(('sig', canonical_tie_rule_signature(tie_rule)), []))
    for omr in child_omrs:
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
        H.append(omr)
    jp, dag = _get_rule_group_shape_cached(graph, H, tie_rule.pattern.user_center)
    return (H, jp, dag)

def _apply_path_anchor_for_path(graph: Graph, anchor_index: PathAnchorIndex, path: Path, pivot_var: str, all_pw: Dict[str, List[PointWisePredicate]], candidate_map: Dict[str, Set[Any]], compute_sig_fn) -> None:
    from data_structure import ComparisonOp
    path_seq = path.path_sequence
    if not path_seq or path_seq[0] != pivot_var:
        return
    _edge_lookup: Dict[Tuple[str, str], Tuple[str, bool]] = {}
    for edge in path.edges:
        _edge_lookup[edge.source_var, edge.target_var] = (edge.label, edge.is_forward)
    edge_labels = []
    node_labels = [path.nodes[path_seq[0]].label]
    is_forward_list = []
    for i in range(len(path_seq) - 1):
        src_var = path_seq[i]
        tgt_var = path_seq[i + 1]
        edge_info = _edge_lookup.get((src_var, tgt_var))
        if edge_info is not None:
            edge_labels.append(edge_info[0])
            is_forward_list.append(edge_info[1])
        else:
            edge_info = _edge_lookup.get((tgt_var, src_var))
            if edge_info is not None:
                edge_labels.append(edge_info[0])
                is_forward_list.append(not edge_info[1])
            else:
                return
        tgt_label = path.nodes[tgt_var].label if tgt_var in path.nodes else '?'
        node_labels.append(tgt_label)
        var_at_hop = tgt_var
        preds = all_pw.get(var_at_hop, [])
        if not preds:
            continue
        prefix_sig = compute_sig_fn(edge_labels[:i + 1], node_labels[:i + 2], is_forward_list[:i + 1])
        if not anchor_index.has_entry(prefix_sig):
            continue
        pivot_cands = candidate_map.get(pivot_var)
        if pivot_cands is None or not pivot_cands:
            continue
        for pred in preds:
            attr_name = pred.attribute
            attr_value = pred.constant
            op = pred.operator
            if op == ComparisonOp.EQ:
                pivots_from_index = anchor_index.query(prefix_sig, attr_name, attr_value)
            elif op == ComparisonOp.NE:
                pivots_from_index = anchor_index.query_ne(prefix_sig, attr_name, attr_value)
            else:
                groups = anchor_index.query_grouped(prefix_sig, attr_name)
                pivots_from_index = set()
                try:
                    const_f = float(attr_value)
                    for val, pivots in groups.items():
                        try:
                            val_f = float(val)
                        except (TypeError, ValueError):
                            continue
                        if op == ComparisonOp.LT and val_f < const_f:
                            pivots_from_index |= pivots
                        elif op == ComparisonOp.LE and val_f <= const_f:
                            pivots_from_index |= pivots
                        elif op == ComparisonOp.GT and val_f > const_f:
                            pivots_from_index |= pivots
                        elif op == ComparisonOp.GE and val_f >= const_f:
                            pivots_from_index |= pivots
                except (TypeError, ValueError):
                    continue
            if pivots_from_index is not None:
                candidate_map[pivot_var] = pivot_cands & pivots_from_index
                pivot_cands = candidate_map[pivot_var]
                if not pivot_cands:
                    return

def init_candidate_map(graph: Graph, joint_pattern: JointPattern, user_id: Any, tie_rule: TIERule, omr_rules: List[OMRule], ml_cache: Optional[MLPredicateCache]=None, aggr_model: Optional[AggrModel]=None, dis_model: Optional[DisModel]=None, offline_index: Optional['OfflineIndex']=None, timing_out: Optional[Dict[str, float]]=None, cache_stats_out: Optional[Dict[str, int]]=None) -> Tuple[Dict[str, Set[Any]], Dict[str, List[PointWisePredicate]]]:
    tie_pattern = tie_rule.pattern
    x0 = tie_pattern.user_center
    y0 = tie_pattern.item_center
    _t_phase = time.perf_counter()
    blueprint = _get_group_init_blueprint(graph, tie_rule, omr_rules, timing_out=timing_out, cache_stats_out=cache_stats_out)
    _record_timing(timing_out, 'blueprint_lookup', _t_phase)
    all_pw: Dict[str, List[PointWisePredicate]] = blueprint['all_pw']
    candidate_map: Dict[str, Set[Any]] = {}
    _t_phase = time.perf_counter()
    candidate_map[x0] = {user_id}
    _record_timing(timing_out, 'x0_patch', _t_phase)
    _t_phase = time.perf_counter()
    item_candidates = set(blueprint['item_candidates'])
    _record_timing(timing_out, 'y0_copy', _t_phase)
    user_cands_for_anchor = {user_id}
    anchor_index: Optional[PathAnchorIndex] = getattr(graph, 'path_anchor_index', None)
    if anchor_index is not None:
        _t_phase = time.perf_counter()
        user_cands_for_anchor, item_candidates, _, _ = apply_path_anchor_filters(graph, tie_pattern, tie_rule.preconditions, user_cands_for_anchor, item_candidates)
        _record_timing(timing_out, 'path_anchor_tie', _t_phase)
    _t_phase = time.perf_counter()
    candidate_map[y0] = set(item_candidates)
    _record_timing(timing_out, 'y0_patch', _t_phase)
    tie_ml_preds = tie_rule.get_ml_predicates()
    if tie_ml_preds and ml_cache is not None:
        _t_phase = time.perf_counter()
        candidate_map[y0] = {v for v in candidate_map[y0] if all((pred.evaluate(ml_cache.get_prediction(user_id, v)) for pred in tie_ml_preds))}
        _record_timing(timing_out, 'tie_ml_filter', _t_phase)
    _t_phase = time.perf_counter()
    for var, cands in blueprint['tie_var_candidates'].items():
        candidate_map[var] = set(cands)
    _record_timing(timing_out, 'tie_var_copy', _t_phase)
    _t_phase = time.perf_counter()
    for var, cands in blueprint['omr_var_candidates'].items():
        if var not in joint_pattern.nodes:
            logger.warning('[init_candidate_map] OMR variable %s is missing from joint_pattern.nodes; skipping it. This usually indicates inconsistent JointPattern construction.', var)
            continue
        if var not in candidate_map:
            candidate_map[var] = set(cands)
    _record_timing(timing_out, 'omr_var_copy', _t_phase)
    if anchor_index is not None and omr_rules:
        from data_structure import compute_path_anchor_signature
        pivot_survivors: Dict[str, Set[Any]] = {}
        _t_phase = time.perf_counter()
        for omr, omr_pw, pivots_this_tuple, bridge_paths, alt_paths, w0 in blueprint['path_anchor_jobs']:
            cm_local = dict(candidate_map)
            pivots_this = set(pivots_this_tuple)
            for pv in pivots_this:
                if pv in cm_local:
                    cm_local[pv] = set(cm_local[pv])
            for bp_idx, bridge_path in enumerate(bridge_paths):
                origin = omr.pattern.bridge_origins[bp_idx]
                _apply_path_anchor_for_path(graph, anchor_index, bridge_path, origin, omr_pw, cm_local, compute_path_anchor_signature)
            for alt_path in alt_paths:
                _apply_path_anchor_for_path(graph, anchor_index, alt_path, w0, omr_pw, cm_local, compute_path_anchor_signature)
            for pv in pivots_this:
                if pv in cm_local:
                    if pv not in pivot_survivors:
                        pivot_survivors[pv] = set(cm_local[pv])
                    else:
                        pivot_survivors[pv] |= cm_local[pv]
        for pv, survivors in pivot_survivors.items():
            if pv in candidate_map:
                candidate_map[pv] &= survivors
        _record_timing(timing_out, 'path_anchor_omr', _t_phase)
    if aggr_model is not None:
        offline_aggr_union: Dict[str, Set[Any]] = {}
        if offline_index is not None:
            for omr in omr_rules:
                omr_idx = _get_offline_omr_index(offline_index, omr)
                if omr_idx is None:
                    continue
                valid_ws = offline_index.aggr_pass.get(omr_idx)
                if not valid_ws:
                    continue
                w0 = omr.pattern.alt_center
                offline_aggr_union.setdefault(w0, set()).update(valid_ws)
        for w0, all_preds in blueprint['w0_aggr_constraints'].items():
            if w0 not in candidate_map:
                continue
            if w0 in offline_aggr_union:
                _t_phase = time.perf_counter()
                candidate_map[w0] &= offline_aggr_union[w0]
                _record_timing(timing_out, 'aggr_filter', _t_phase)
                continue
            w0_cands = candidate_map[w0]
            _t_phase = time.perf_counter()
            scores = aggr_model.compute_batch(w0_cands)
            if ml_cache is not None:
                for w_id, score in scores.items():
                    ml_cache.set_aggr_score(w_id, score)
            filtered = set()
            for w_id, score in scores.items():
                if any((p.evaluate(score) for p in all_preds)):
                    filtered.add(w_id)
            candidate_map[w0] = filtered
            _record_timing(timing_out, 'aggr_filter', _t_phase)
    return (candidate_map, all_pw)

def _has_ambiguous_multi_shared_bridge_join(rule: OMRule, bridge_paths: List[Path]) -> bool:
    omr_pattern = rule.pattern
    x0 = omr_pattern.user_center
    y0 = omr_pattern.anchor_center
    w0 = omr_pattern.alt_center
    alt_star_vars = set(omr_pattern.alt_star.nodes.keys())
    for bridge_path in bridge_paths:
        bp_seq = bridge_path.path_sequence if hasattr(bridge_path, 'path_sequence') else []
        terminal_var = bp_seq[-1] if bp_seq else None
        if terminal_var == w0:
            continue
        shared_vars = (set(bridge_path.nodes.keys()) & alt_star_vars) - {x0, y0, w0}
        if len(shared_vars) > 1:
            return True
    return False

def _derive_bridge_vw_pairs_from_matches_py(rule: OMRule, F_phi: Dict[str, Set[Any]], y0_bridge_matches: List[Tuple[Path, Dict[Any, Dict[str, Set[Any]]]]], alt_matches_per_w: Dict[Any, Dict[str, Set[Any]]]) -> Optional[Dict[Any, Set[Any]]]:
    if not y0_bridge_matches:
        return None
    if _has_ambiguous_multi_shared_bridge_join(rule, [bridge_path for bridge_path, _ in y0_bridge_matches]):
        return None
    omr_pattern = rule.pattern
    x0 = omr_pattern.user_center
    y0 = omr_pattern.anchor_center
    w0 = omr_pattern.alt_center
    y0_cands = F_phi.get(y0, set())
    w0_cands = F_phi.get(w0, set())
    if not y0_cands or not w0_cands:
        return {v: set() for v in y0_cands}
    alt_star_vars = set(omr_pattern.alt_star.nodes.keys())
    bridge_vw: Dict[Any, Set[Any]] = {}
    for v in y0_cands:
        valid_ws: Optional[Set[Any]] = None
        for bridge_path, per_v_matches in y0_bridge_matches:
            pivot_match = per_v_matches.get(v)
            if not pivot_match:
                path_ws: Set[Any] = set()
            else:
                bp_seq = bridge_path.path_sequence if hasattr(bridge_path, 'path_sequence') else []
                terminal_var = bp_seq[-1] if bp_seq else None
                if terminal_var == w0:
                    path_ws = pivot_match.get(w0, set()) & w0_cands
                else:
                    shared_vars = (set(bridge_path.nodes.keys()) & alt_star_vars) - {x0, y0, w0}
                    if not shared_vars:
                        path_ws = set()
                    else:
                        path_ws = set()
                        for w in w0_cands:
                            w_match = alt_matches_per_w.get(w)
                            if w_match is None:
                                continue
                            join_ok = True
                            for shared_var in shared_vars:
                                bridge_cands = set(pivot_match.get(shared_var, set()))
                                alt_cands = set(w_match.get(shared_var, set()))
                                if shared_var in F_phi:
                                    bridge_cands &= F_phi[shared_var]
                                    alt_cands &= F_phi[shared_var]
                                if not bridge_cands & alt_cands:
                                    join_ok = False
                                    break
                            if join_ok:
                                path_ws.add(w)
            valid_ws = path_ws if valid_ws is None else valid_ws & path_ws
            if not valid_ws:
                break
        bridge_vw[v] = valid_ws if valid_ws is not None else set()
    return bridge_vw

def _derive_bridge_vw_pairs_from_matches(rule: OMRule, F_phi: Dict[str, Set[Any]], y0_bridge_matches: List[Tuple[Path, Dict[Any, Dict[str, Set[Any]]]]], alt_matches_per_w: Dict[Any, Dict[str, Set[Any]]]) -> Optional[Dict[Any, Set[Any]]]:
    if not y0_bridge_matches:
        return None
    if _has_ambiguous_multi_shared_bridge_join(rule, [bridge_path for bridge_path, _ in y0_bridge_matches]):
        return None
    if _cy_derive_bridge_vw_pairs is None:
        return _derive_bridge_vw_pairs_from_matches_py(rule, F_phi, y0_bridge_matches, alt_matches_per_w)
    omr_pattern = rule.pattern
    y0 = omr_pattern.anchor_center
    w0 = omr_pattern.alt_center
    y0_cands = F_phi.get(y0, set())
    w0_cands = F_phi.get(w0, set())
    alt_star_vars = set(omr_pattern.alt_star.nodes.keys())
    bridge_specs = []
    for bridge_path, per_v_matches in y0_bridge_matches:
        bp_seq = bridge_path.path_sequence if hasattr(bridge_path, 'path_sequence') else []
        terminal_var = bp_seq[-1] if bp_seq else None
        shared_vars = tuple((set(bridge_path.nodes.keys()) & alt_star_vars) - {omr_pattern.user_center, y0, w0}) if terminal_var != w0 else ()
        bridge_specs.append((terminal_var, shared_vars, per_v_matches, w0))
    return _cy_derive_bridge_vw_pairs(y0_cands, w0_cands, bridge_specs, alt_matches_per_w, F_phi)

def _build_edge_lookups(paths: List[Path]) -> List[Dict[Tuple[str, str], Tuple[str, bool]]]:
    return [{(e.source_var, e.target_var): (e.label, e.is_forward) for e in path.edges} for path in paths]

def _build_prefilter_cache(graph: Graph, pattern, point_wise_preds: Dict[str, List[PointWisePredicate]], pivot_vars: Set[str]) -> Dict[str, Any]:
    cache: Dict[str, Any] = {}
    for var, preds in point_wise_preds.items():
        if preds and var not in pivot_vars and (var in pattern.nodes):
            var_label = pattern.nodes[var].label
            prefiltered = _prefilter_by_attribute_index(graph, var_label, preds)
            cache[var] = prefiltered if prefiltered is not None else _PREFILTER_NA
    return cache

def build_bridge_match_context(graph: Graph, omr_pattern: OMRPattern, bridge_path: Path, point_wise_preds: Dict[str, List[PointWisePredicate]], prefilter_cache: Optional[Dict[str, Any]]=None, compact_graph: Optional['CompactGraph']=None) -> Dict[str, Any]:
    ctx: Dict[str, Any] = {'compact_graph': compact_graph}
    if compact_graph is None or not _HAS_BRIDGE_BATCH:
        return ctx
    bp_seq = bridge_path.path_sequence
    bp_edge_lookup = {(e.source_var, e.target_var): (e.label, e.is_forward) for e in bridge_path.edges}
    node_label_map: Dict[str, str] = {}
    for var in bp_seq:
        pn = omr_pattern.nodes.get(var)
        if pn is not None:
            node_label_map[var] = pn.label
    _edge_tgt_label = getattr(graph, 'edge_target_label', {}) or {}
    _edge_src_label = getattr(graph, 'edge_source_label', {}) or {}
    skip_checks: Dict[int, bool] = {}
    for i in range(len(bp_seq) - 1):
        edge_info = bp_edge_lookup.get((bp_seq[i], bp_seq[i + 1]))
        if edge_info:
            elabel, fwd = edge_info
            next_label = node_label_map.get(bp_seq[i + 1])
            if fwd:
                skip_checks[i] = _edge_tgt_label.get(elabel) == next_label
            else:
                skip_checks[i] = _edge_src_label.get(elabel) == next_label
    int_prefilter: Dict[str, set] = {}
    if prefilter_cache:
        _id_to_int = compact_graph.id_to_int
        for var, pf in prefilter_cache.items():
            if pf is _PREFILTER_NA or pf is None:
                continue
            int_prefilter[var] = {_id_to_int[nid] for nid in pf if nid in _id_to_int}
    needs_pw_eval = any((v in point_wise_preds and v not in int_prefilter for v in bp_seq[1:]))
    _node_attrs = compact_graph.node_attrs

    def pw_eval(var_name, n_int):
        preds = point_wise_preds.get(var_name)
        if not preds:
            return True
        attrs = _node_attrs[n_int]
        for p in preds:
            val = attrs.get(p.attribute)
            if val is None:
                return False
            if not _safe_compare_predicate_values(val, p.constant, p.operator):
                return False
        return True
    ctx.update({'path_sequence': bp_seq, 'edge_lookup': bp_edge_lookup, 'node_label_map': node_label_map, 'skip_checks': skip_checks, 'int_prefilter': int_prefilter, 'needs_pw_eval': needs_pw_eval, 'pw_eval_fn': pw_eval})
    return ctx

def _match_bridge_paths_batch(graph: Graph, compact_graph, omr_pattern: OMRPattern, bridge_path: Path, origin_candidates: Set[Any], point_wise_preds: Dict[str, List[PointWisePredicate]], prefilter_cache: Optional[Dict[str, Any]]=None, w0_candidates: Optional[Set[Any]]=None, bridge_match_ctx: Optional[Dict[str, Any]]=None) -> Dict[Any, Optional[Dict[str, Set[Any]]]]:
    cg = compact_graph
    _id_to_int = cg.id_to_int
    _int_to_id = cg.int_to_id
    pivot_ints = []
    pivot_map = {}
    for oid in origin_candidates:
        oi = _id_to_int.get(oid)
        if oi is not None:
            pivot_ints.append(oi)
            pivot_map[oi] = oid
    if not pivot_ints:
        return {oid: None for oid in origin_candidates}
    if bridge_match_ctx is None:
        bridge_match_ctx = build_bridge_match_context(graph, omr_pattern, bridge_path, point_wise_preds, prefilter_cache, compact_graph)
    bp_seq = bridge_match_ctx.get('path_sequence', bridge_path.path_sequence)
    bp_edge_lookup = bridge_match_ctx.get('edge_lookup')
    if bp_edge_lookup is None:
        bp_edge_lookup = {(e.source_var, e.target_var): (e.label, e.is_forward) for e in bridge_path.edges}
    node_label_map = bridge_match_ctx.get('node_label_map', {})
    skip_checks = bridge_match_ctx.get('skip_checks', {})
    int_prefilter = bridge_match_ctx.get('int_prefilter', {})
    needs_pw_eval = bridge_match_ctx.get('needs_pw_eval', False)
    pw_eval = bridge_match_ctx.get('pw_eval_fn')
    w_candidate_ints = None
    if w0_candidates is not None:
        w_candidate_ints = {_id_to_int[w] for w in w0_candidates if w in _id_to_int}
    int_results = _cy_bridge_batch(cg, bp_seq, pivot_ints, bp_edge_lookup, node_label_map, skip_checks, int_prefilter, pw_eval_fn=pw_eval if needs_pw_eval else None, w_candidate_ints=w_candidate_ints)
    out: Dict[Any, Optional[Dict[str, Set[Any]]]] = {}
    for int_pivot, int_result in int_results.items():
        orig_id = pivot_map[int_pivot]
        if int_result is None:
            out[orig_id] = None
        else:
            out[orig_id] = {var: {_int_to_id[i] for i in int_set} for var, int_set in int_result.items()}
    for oid in origin_candidates:
        if oid not in out:
            out[oid] = None
    return out

def local_refinement(graph: Graph, candidate_map: Dict[str, Set[Any]], rooted_dag: RootedDAG, rule: Union[TIERule, OMRule], point_wise_preds: Dict[str, List[PointWisePredicate]], compact_graph: Optional['CompactGraph']=None, star_cache_user: Optional[Dict[Any, Dict[str, Set[Any]]]]=None, star_cache_item: Optional[Dict[Any, Dict[str, Set[Any]]]]=None, star_cache_alt: Optional[Dict[Any, Dict[str, Set[Any]]]]=None, current_tie_anchors: Optional[Set[Any]]=None, ml_cache: Optional[MLPredicateCache]=None, prefilter_cache: Optional[Dict[str, Any]]=None, edge_lookups: Optional[Dict[str, List[Dict]]]=None, bridge_vw_out: Optional[Dict[Any, Set[Any]]]=None, dis_precomputed: bool=True, offline_bridge_idx: Optional[Any]=None, offline_dis_pass: Optional[Dict[Any, frozenset]]=None, star_contexts: Optional[Dict[str, Any]]=None, rule_runtime: Optional[Dict[str, Any]]=None) -> Optional[Dict[str, Set[Any]]]:
    if isinstance(rule, TIERule):
        return _local_refinement_tie(graph, candidate_map, rooted_dag, rule, point_wise_preds, compact_graph, star_cache_user, star_cache_item, prefilter_cache=prefilter_cache, edge_lookups=edge_lookups, star_contexts=star_contexts, rule_runtime=rule_runtime)
    else:
        return _local_refinement_omr(graph, candidate_map, rooted_dag, rule, point_wise_preds, compact_graph, star_cache_user, star_cache_item, star_cache_alt, current_tie_anchors=current_tie_anchors, ml_cache=ml_cache, prefilter_cache=prefilter_cache, edge_lookups=edge_lookups, star_contexts=star_contexts, bridge_vw_out=bridge_vw_out, dis_precomputed=dis_precomputed, offline_bridge_idx=offline_bridge_idx, offline_dis_pass=offline_dis_pass, rule_runtime=rule_runtime)

def _local_refinement_tie(graph: Graph, candidate_map: Dict[str, Set[Any]], rooted_dag: Optional[RootedDAG], rule: TIERule, point_wise_preds: Dict[str, List[PointWisePredicate]], compact_graph: Optional['CompactGraph']=None, star_cache_user: Optional[Dict[Any, Dict[str, Set[Any]]]]=None, star_cache_item: Optional[Dict[Any, Dict[str, Set[Any]]]]=None, prefilter_cache: Optional[Dict[str, Any]]=None, edge_lookups: Optional[Dict[str, List[Dict]]]=None, star_contexts: Optional[Dict[str, Any]]=None, rule_runtime: Optional[Dict[str, Any]]=None) -> Optional[Dict[str, Set[Any]]]:
    pattern = rule.pattern
    x0 = pattern.user_center
    y0 = pattern.item_center
    x0_preds = point_wise_preds.get(x0, [])
    y0_preds = point_wise_preds.get(y0, [])
    user_paths = pattern.user_paths
    item_paths = pattern.item_paths
    if edge_lookups is not None:
        user_edge_lookups = edge_lookups.get('user', _build_edge_lookups(user_paths))
        item_edge_lookups = edge_lookups.get('item', _build_edge_lookups(item_paths))
    else:
        user_edge_lookups = _build_edge_lookups(user_paths)
        item_edge_lookups = _build_edge_lookups(item_paths)
    pivot_vars = {x0, y0}
    if prefilter_cache is None:
        prefilter_cache = _build_prefilter_cache(graph, pattern, point_wise_preds, pivot_vars)
    user_star_ctx = (star_contexts or {}).get('user')
    if user_star_ctx is None:
        user_star_ctx = build_star_match_context(graph, pattern, user_paths, user_edge_lookups, point_wise_preds, prefilter_cache, compact_graph) if user_paths else {'compact_graph': compact_graph}
    item_star_ctx = (star_contexts or {}).get('item')
    if item_star_ctx is None:
        item_star_ctx = build_star_match_context(graph, pattern, item_paths, item_edge_lookups, point_wise_preds, prefilter_cache, compact_graph) if item_paths else {'compact_graph': compact_graph}
    F: Dict[str, Set[Any]] = {x0: set(), y0: set()}
    x0_candidates = candidate_map.get(x0, set())
    if x0_preds:
        x0_candidates = {u for u in x0_candidates if graph.get_node(u) is not None and all((p.evaluate(graph.get_node(u)) for p in x0_preds))}
    for u in x0_candidates:
        if star_cache_user is not None and u in star_cache_user:
            u_match = star_cache_user[u]
        else:
            u_match = _match_star_paths(graph, pattern, u, user_paths, point_wise_preds, user_edge_lookups, prefilter_cache, compact_graph, star_match_ctx=user_star_ctx)
            if star_cache_user is not None and u_match is not None:
                star_cache_user[u] = u_match
        if u_match is None:
            continue
        F[x0].add(u)
        for var, cands in u_match.items():
            if var == x0:
                continue
            if var not in F:
                F[var] = set(cands)
            else:
                F[var] |= cands
    if not F[x0]:
        return None
    y0_candidates = candidate_map.get(y0, set())
    if y0_preds:
        y0_candidates = {v for v in y0_candidates if graph.get_node(v) is not None and all((p.evaluate(graph.get_node(v)) for p in y0_preds))}
    for v in y0_candidates:
        if star_cache_item is not None and v in star_cache_item:
            v_match = star_cache_item[v]
        else:
            v_match = _match_star_paths(graph, pattern, v, item_paths, point_wise_preds, item_edge_lookups, prefilter_cache, compact_graph, star_match_ctx=item_star_ctx)
            if star_cache_item is not None and v_match is not None:
                star_cache_item[v] = v_match
        if v_match is None:
            continue
        F[y0].add(v)
        for var, cands in v_match.items():
            if var == y0:
                continue
            if var not in F:
                F[var] = set(cands)
            else:
                F[var] |= cands
    if not F[y0]:
        return None
    for var in F:
        if var in candidate_map:
            F[var] &= candidate_map[var]
            if not F[var]:
                return None
    F = _propagate_candidates_with_dag(graph, rooted_dag, F, compact_graph=compact_graph, rule_vars=set(rule.pattern.nodes.keys()), allowed_edge_info=_get_rule_dag_edge_filter(rule.pattern))
    if F is None:
        return None
    pair_wise = rule.get_pair_wise_predicates()
    if pair_wise:
        _prune_pairwise(graph, F, pair_wise)
        for var in F:
            if not F[var]:
                return None
    return F

def _local_refinement_omr(graph: Graph, candidate_map: Dict[str, Set[Any]], rooted_dag: Optional[RootedDAG], rule: OMRule, point_wise_preds: Dict[str, List[PointWisePredicate]], compact_graph: Optional['CompactGraph']=None, star_cache_user: Optional[Dict[Any, Dict[str, Set[Any]]]]=None, star_cache_item: Optional[Dict[Any, Dict[str, Set[Any]]]]=None, star_cache_alt: Optional[Dict[Any, Dict[str, Set[Any]]]]=None, current_tie_anchors: Optional[Set[Any]]=None, ml_cache: Optional[MLPredicateCache]=None, prefilter_cache: Optional[Dict[str, Any]]=None, edge_lookups: Optional[Dict[str, List[Dict]]]=None, bridge_vw_out: Optional[Dict[Any, Set[Any]]]=None, dis_precomputed: bool=True, offline_bridge_idx: Optional[Any]=None, offline_dis_pass: Optional[Dict[Any, frozenset]]=None, star_contexts: Optional[Dict[str, Any]]=None, rule_runtime: Optional[Dict[str, Any]]=None) -> Optional[Dict[str, Set[Any]]]:
    omr_pattern = rule.pattern
    tie_pattern = omr_pattern.tie_pattern
    x0 = omr_pattern.user_center
    y0 = omr_pattern.anchor_center
    w0 = omr_pattern.alt_center
    _st = getattr(_local_refinement_omr, '_step_times', None)
    pivot_vars = {x0, y0, w0}
    if prefilter_cache is None:
        prefilter_cache = _build_prefilter_cache(graph, omr_pattern, point_wise_preds, pivot_vars)
    omr_extra_pw = (rule_runtime or {}).get('omr_extra_pw')
    if omr_extra_pw is None:
        omr_extra_pw = rule.get_omr_only_point_wise()
    _safe_tie_leaf_extra_pw = dict((rule_runtime or {}).get('safe_tie_leaf_extra_pw', {}))
    _has_unsafe_extra_on_tie_stars = (rule_runtime or {}).get('has_unsafe_extra_on_tie_stars')
    if _has_unsafe_extra_on_tie_stars is None:
        tie_star_vars = set(tie_pattern.nodes.keys()) - {x0, y0}
        safe_tie_leaf_vars = _get_safe_tie_star_leaf_vars(tie_pattern)
        _has_unsafe_extra_on_tie_stars = any((var in tie_star_vars and var not in safe_tie_leaf_vars for var in omr_extra_pw))
        if not _safe_tie_leaf_extra_pw:
            _safe_tie_leaf_extra_pw = {var: tuple(preds) for var, preds in omr_extra_pw.items() if var in safe_tie_leaf_vars}
    _use_shared_tie_star_cache = not _has_unsafe_extra_on_tie_stars
    _y0_extra_preds = list((rule_runtime or {}).get('y0_extra_preds', omr_extra_pw.get(y0, [])))
    _w0_extra_preds = list((rule_runtime or {}).get('w0_extra_preds', omr_extra_pw.get(w0, [])))
    _y0_extra_prefilter = (rule_runtime or {}).get('y0_extra_prefilter')
    _w0_extra_prefilter = (rule_runtime or {}).get('w0_extra_prefilter')
    _anchor_ml_preds = list((rule_runtime or {}).get('anchor_ml_preds', [pred for pred in rule.get_ml_predicates() if pred.user_var == x0 and pred.item_var == y0]))
    _alt_ml_preds = list((rule_runtime or {}).get('alt_ml_preds', [pred for pred in rule.get_ml_predicates() if pred.user_var == x0 and pred.item_var == w0]))
    _resolved_user_id = None
    _x0_candidates = candidate_map.get(x0, set())
    if len(_x0_candidates) == 1:
        _resolved_user_id = next(iter(_x0_candidates))
    _t_step = time.perf_counter()
    user_paths = tie_pattern.user_paths
    item_paths = tie_pattern.item_paths
    if edge_lookups is not None:
        user_edge_lookups = edge_lookups.get('user', _build_edge_lookups(user_paths))
        item_edge_lookups = edge_lookups.get('item', _build_edge_lookups(item_paths))
    else:
        user_edge_lookups = _build_edge_lookups(user_paths)
        item_edge_lookups = _build_edge_lookups(item_paths)
    user_star_ctx = (star_contexts or {}).get('user')
    if user_star_ctx is None:
        user_star_ctx = build_star_match_context(graph, tie_pattern, user_paths, user_edge_lookups, point_wise_preds, prefilter_cache, compact_graph) if user_paths else {'compact_graph': compact_graph}
    item_star_ctx = (star_contexts or {}).get('item')
    if item_star_ctx is None:
        item_star_ctx = build_star_match_context(graph, tie_pattern, item_paths, item_edge_lookups, point_wise_preds, prefilter_cache, compact_graph) if item_paths else {'compact_graph': compact_graph}
    F: Dict[str, Set[Any]] = {x0: set(), y0: set()}
    for u in candidate_map.get(x0, set()):
        if _use_shared_tie_star_cache and star_cache_user is not None and (u in star_cache_user):
            u_match = _post_filter_leaf_star_match(graph, star_cache_user[u], _safe_tie_leaf_extra_pw)
        else:
            u_match = _match_star_paths(graph, tie_pattern, u, user_paths, point_wise_preds, user_edge_lookups, prefilter_cache, compact_graph, star_match_ctx=user_star_ctx)
            if _use_shared_tie_star_cache and star_cache_user is not None and (u_match is not None) and (not _safe_tie_leaf_extra_pw):
                star_cache_user[u] = u_match
        if u_match is None:
            continue
        F[x0].add(u)
        for var, cands in u_match.items():
            if var == x0:
                continue
            if var not in F:
                F[var] = set(cands)
            else:
                F[var] |= cands
    if not F[x0]:
        return None
    if _st is not None:
        _st['1_user_star'] += time.perf_counter() - _t_step
        _t_step = time.perf_counter()
    _y0_iter_cands = candidate_map.get(y0, set())
    if _y0_extra_preds:
        if _y0_extra_prefilter is not None:
            _y0_iter_cands = _y0_iter_cands & _y0_extra_prefilter
        else:
            _y0_iter_cands = {v for v in _y0_iter_cands if all((p.evaluate(graph.get_node(v)) for p in _y0_extra_preds))}
    if _anchor_ml_preds and ml_cache is not None and (_resolved_user_id is not None):
        _y0_iter_cands = {v for v in _y0_iter_cands if all((pred.evaluate(ml_cache.get_prediction(_resolved_user_id, v)) for pred in _anchor_ml_preds))}
    for v in _y0_iter_cands:
        if _use_shared_tie_star_cache and star_cache_item is not None and (v in star_cache_item):
            v_match = _post_filter_leaf_star_match(graph, star_cache_item[v], _safe_tie_leaf_extra_pw)
        else:
            v_match = _match_star_paths(graph, tie_pattern, v, item_paths, point_wise_preds, item_edge_lookups, prefilter_cache, compact_graph, star_match_ctx=item_star_ctx)
            if _use_shared_tie_star_cache and star_cache_item is not None and (v_match is not None) and (not _safe_tie_leaf_extra_pw):
                star_cache_item[v] = v_match
        if v_match is None:
            continue
        F[y0].add(v)
        for var, cands in v_match.items():
            if var == y0:
                continue
            if var not in F:
                F[var] = set(cands)
            else:
                F[var] |= cands
    if not F[y0]:
        return None
    if _st is not None:
        _st['1_item_star'] += time.perf_counter() - _t_step
        _t_step = time.perf_counter()
    rec_preds = rule.get_rec_predicates()
    if rec_preds and current_tie_anchors is not None:
        F[y0] &= current_tie_anchors
        if not F[y0]:
            return None
    if _anchor_ml_preds and ml_cache is not None and (_resolved_user_id is not None):
        F[y0] = {v for v in F[y0] if all((pred.evaluate(ml_cache.get_prediction(_resolved_user_id, v)) for pred in _anchor_ml_preds))}
        if not F[y0]:
            return None
    if _st is not None:
        _st['1.5_rec'] += time.perf_counter() - _t_step
        _t_step = time.perf_counter()
    bridge_paths = list((rule_runtime or {}).get('bridge_paths', omr_pattern.bridge_paths))
    bridge_origins = list((rule_runtime or {}).get('bridge_origins', omr_pattern.bridge_origins))
    bridge_edge_lookups = list((rule_runtime or {}).get('bridge_edge_lookups', []))
    bridge_contexts = list((rule_runtime or {}).get('bridge_contexts', []))
    _use_batch_bridge = _HAS_BRIDGE_BATCH and compact_graph is not None
    _can_use_offline_bridge = offline_bridge_idx is not None and len(bridge_paths) == 1 and (len(bridge_origins) == 1) and (bridge_origins[0] == y0)
    dis_preds = rule.get_dis_predicates()
    if dis_preds and ml_cache is not None and dis_precomputed:
        y0_candidates = F.get(y0, set())
        w0_init_candidates = candidate_map.get(w0, set())
        if w0 in F and F[w0]:
            w0_init_candidates = w0_init_candidates & F[w0]
        valid_w0 = set()
        for v in y0_candidates:
            if offline_dis_pass is not None:
                w_valid_for_v = w0_init_candidates & offline_dis_pass.get(v, frozenset())
                if not w_valid_for_v:
                    continue
                valid_w0 |= w_valid_for_v
                continue
            if _can_use_offline_bridge:
                w_valid_for_v = w0_init_candidates & offline_bridge_idx.get_reachable(v)
                if not w_valid_for_v:
                    continue
            else:
                w_valid_for_v = set(w0_init_candidates)
            for dp in dis_preds:
                filtered = ml_cache.get_dis_filtered_items(v, dp.operator, dp.threshold, w_valid_for_v)
                w_valid_for_v &= filtered
                if not w_valid_for_v:
                    break
            valid_w0 |= w_valid_for_v
        if w0 in F:
            F[w0] = F[w0] & valid_w0 if F[w0] else valid_w0
        else:
            F[w0] = valid_w0
        if not F.get(w0, set()):
            return None
    if _st is not None:
        _st['2.5_dis_prune'] += time.perf_counter() - _t_step
        _t_step = time.perf_counter()
    y0_bridge_matches: List[Tuple[Path, Dict[Any, Dict[str, Set[Any]]]]] = []
    for bp_idx, bridge_path in enumerate(bridge_paths):
        origin = bridge_origins[bp_idx]
        origin_candidates = F.get(origin, set())
        if not origin_candidates:
            return None
        bp_seq = bridge_path.path_sequence if hasattr(bridge_path, 'path_sequence') else []
        track_y0_bridge = bridge_vw_out is not None and origin == y0
        per_v_bridge: Optional[Dict[Any, Dict[str, Set[Any]]]] = {} if track_y0_bridge else None
        bp_result: Dict[str, Set[Any]] = {}
        if _can_use_offline_bridge and origin == y0:
            for pivot_id in origin_candidates:
                bri_bvc = offline_bridge_idx.get_bvc(pivot_id)
                if not bri_bvc:
                    if per_v_bridge is not None:
                        per_v_bridge[pivot_id] = {}
                    continue
                for var, cands in bri_bvc.items():
                    if var not in bp_result:
                        bp_result[var] = set(cands)
                    else:
                        bp_result[var] |= cands
                if per_v_bridge is not None:
                    per_v_bridge[pivot_id] = {var: set(cands) for var, cands in bri_bvc.items()}
        elif _use_batch_bridge:
            w0_filter = F.get(w0, None) if bp_seq and bp_seq[-1] == w0 else None
            per_pivot_results = _match_bridge_paths_batch(graph, compact_graph, omr_pattern, bridge_path, origin_candidates, point_wise_preds, prefilter_cache=prefilter_cache, w0_candidates=w0_filter, bridge_match_ctx=bridge_contexts[bp_idx] if bp_idx < len(bridge_contexts) else None)
            for pivot_id, pmatch in per_pivot_results.items():
                if pmatch is None:
                    if per_v_bridge is not None:
                        per_v_bridge[pivot_id] = {}
                    continue
                for var, cands in pmatch.items():
                    if var not in bp_result:
                        bp_result[var] = set(cands)
                    else:
                        bp_result[var] |= cands
                if per_v_bridge is not None:
                    per_v_bridge[pivot_id] = {var: set(cands) for var, cands in pmatch.items()}
        else:
            bp_edge_lookup = bridge_edge_lookups[bp_idx] if bp_idx < len(bridge_edge_lookups) else {(e.source_var, e.target_var): (e.label, e.is_forward) for e in bridge_path.edges}
            for pivot_id in origin_candidates:
                bp_match = match_path_compact(graph, bridge_path, pivot_id, omr_pattern, point_wise_preds, edge_lookup=bp_edge_lookup, prefilter_cache=prefilter_cache)
                if bp_match is None:
                    if per_v_bridge is not None:
                        per_v_bridge[pivot_id] = {}
                    continue
                for var, cands in bp_match.items():
                    if var not in bp_result:
                        bp_result[var] = set(cands)
                    else:
                        bp_result[var] |= cands
                if per_v_bridge is not None:
                    per_v_bridge[pivot_id] = {var: set(cands) for var, cands in bp_match.items()}
        if not bp_result:
            return None
        if per_v_bridge is not None:
            y0_bridge_matches.append((bridge_path, per_v_bridge))
        for var, cands in bp_result.items():
            if var in F:
                F[var] &= cands
            else:
                F[var] = cands
            if not F[var]:
                return None
    if _st is not None:
        _st['2_bridge'] += time.perf_counter() - _t_step
        _t_step = time.perf_counter()
    alt_paths = list((rule_runtime or {}).get('alt_paths', omr_pattern.alt_star.paths if hasattr(omr_pattern.alt_star, 'paths') else []))
    alt_matches_per_w: Dict[Any, Dict[str, Set[Any]]] = {}
    if alt_paths:
        if edge_lookups is not None:
            alt_edge_lookups = edge_lookups.get('alt', _build_edge_lookups(alt_paths))
        else:
            alt_edge_lookups = (rule_runtime or {}).get('alt_edge_lookups', _build_edge_lookups(alt_paths))
        alt_star_ctx = (star_contexts or {}).get('alt')
        if alt_star_ctx is None:
            alt_star_ctx = build_star_match_context(graph, omr_pattern, alt_paths, alt_edge_lookups, point_wise_preds, prefilter_cache, compact_graph)
        w0_candidates = candidate_map.get(w0, set())
        if w0 in F and F[w0]:
            w0_candidates = w0_candidates & F[w0]
        if _w0_extra_preds:
            if _w0_extra_prefilter is not None:
                w0_candidates = w0_candidates & _w0_extra_prefilter
            else:
                w0_candidates = {w for w in w0_candidates if all((p.evaluate(graph.get_node(w)) for p in _w0_extra_preds))}
        if _alt_ml_preds and ml_cache is not None and (_resolved_user_id is not None):
            w0_candidates = {w for w in w0_candidates if all((pred.evaluate(ml_cache.get_prediction(_resolved_user_id, w)) for pred in _alt_ml_preds))}
        alt_results: Dict[str, Set[Any]] = {}
        for w in w0_candidates:
            if star_cache_alt is not None and w in star_cache_alt:
                w_match = star_cache_alt[w]
            else:
                w_match = _match_star_paths(graph, omr_pattern, w, alt_paths, point_wise_preds, alt_edge_lookups, prefilter_cache, compact_graph, star_match_ctx=alt_star_ctx)
                if star_cache_alt is not None and w_match is not None:
                    star_cache_alt[w] = w_match
            if w_match is None:
                continue
            alt_matches_per_w[w] = w_match
            for var, cands in w_match.items():
                if var not in alt_results:
                    alt_results[var] = set(cands)
                else:
                    alt_results[var] |= cands
        for var, cands in alt_results.items():
            if var in F:
                F[var] &= cands
            else:
                F[var] = cands
            if not F[var]:
                return None
    if _st is not None:
        _st['3_alt_star'] += time.perf_counter() - _t_step
        _t_step = time.perf_counter()
    F = _propagate_candidates_with_dag(graph, rooted_dag, F, compact_graph=compact_graph, rule_vars=set(rule.pattern.nodes.keys()), allowed_edge_info=_get_rule_dag_edge_filter(rule.pattern))
    if F is None:
        return None
    if _st is not None:
        _st['3.5_dag'] += time.perf_counter() - _t_step
        _t_step = time.perf_counter()
    for var in F:
        if var in candidate_map:
            F[var] &= candidate_map[var]
            if not F[var]:
                return None
    if _alt_ml_preds and ml_cache is not None and (_resolved_user_id is not None) and (w0 in F):
        F[w0] = {w for w in F[w0] if all((pred.evaluate(ml_cache.get_prediction(_resolved_user_id, w)) for pred in _alt_ml_preds))}
        if not F[w0]:
            return None
    if bridge_vw_out is not None and y0_bridge_matches:
        bridge_vw_out.clear()
        bridge_vw_out.update(_derive_bridge_vw_pairs_from_matches(rule, F, y0_bridge_matches, alt_matches_per_w) or {})
    pair_wise = rule.get_pair_wise_predicates()
    if pair_wise:
        _prune_pairwise(graph, F, pair_wise)
        for var in F:
            if not F[var]:
                return None
    if _st is not None:
        _st['4_intersect_pw'] += time.perf_counter() - _t_step
    return F

def _prune_pairwise(graph: Graph, F: Dict[str, Set[Any]], pair_wise_preds: List[PairWisePredicate]) -> None:
    attr_batch_cache: Dict[Tuple[str, str, int, int], Dict[Any, Any]] = {}

    def _get_attr_values(var: str, attr: str, cands: Set[Any]) -> Dict[Any, Any]:
        cache_key = (var, attr, id(cands), len(cands))
        cached = attr_batch_cache.get(cache_key)
        if cached is not None:
            return cached
        values = graph.extract_attributes_batch(cands, attr)
        attr_batch_cache[cache_key] = values
        return values
    for pred in pair_wise_preds:
        z1, z2 = (pred.var1, pred.var2)
        if z1 not in F or z2 not in F:
            continue
        cands1 = F[z1]
        cands2 = F[z2]
        if not cands1 or not cands2:
            continue
        if z1 == z2:
            filtered = set()
            for nid in cands1:
                node = graph.get_node(nid)
                if node is None:
                    continue
                try:
                    if pred.evaluate(node, node):
                        filtered.add(nid)
                except (TypeError, ValueError):
                    continue
            F[z1] = filtered
            continue
        vals1 = _get_attr_values(z1, pred.attr1, cands1)
        vals2 = _get_attr_values(z2, pred.attr2, cands2)
        if not vals1 or not vals2:
            F[z1] = set()
            F[z2] = set()
            continue
        op = pred.operator
        valid_z1 = set()
        valid_z2 = set()
        if op == ComparisonOp.EQ:
            val_to_c2: Dict[Any, Set[Any]] = {}
            for c2, v2 in vals2.items():
                nv2 = _normalize_predicate_equality_value(v2)
                if nv2 not in val_to_c2:
                    val_to_c2[nv2] = set()
                val_to_c2[nv2].add(c2)
            for c1, v1 in vals1.items():
                matching_c2s = val_to_c2.get(_normalize_predicate_equality_value(v1))
                if matching_c2s:
                    valid_z1.add(c1)
                    valid_z2 |= matching_c2s
        elif op == ComparisonOp.NE:
            unique_vals2 = {_normalize_predicate_equality_value(v) for v in vals2.values()}
            unique_vals1 = {_normalize_predicate_equality_value(v) for v in vals1.values()}
            for c1, v1 in vals1.items():
                nv1 = _normalize_predicate_equality_value(v1)
                if len(unique_vals2) > 1 or (unique_vals2 and nv1 not in unique_vals2):
                    valid_z1.add(c1)
            for c2, v2 in vals2.items():
                nv2 = _normalize_predicate_equality_value(v2)
                if len(unique_vals1) > 1 or (unique_vals1 and nv2 not in unique_vals1):
                    valid_z2.add(c2)
        elif op in (ComparisonOp.LT, ComparisonOp.LE):
            try:
                max_v2 = max(vals2.values())
                min_v1 = min(vals1.values())
                if op == ComparisonOp.LT:
                    for c1, v1 in vals1.items():
                        if v1 < max_v2:
                            valid_z1.add(c1)
                    for c2, v2 in vals2.items():
                        if min_v1 < v2:
                            valid_z2.add(c2)
                else:
                    for c1, v1 in vals1.items():
                        if v1 <= max_v2:
                            valid_z1.add(c1)
                    for c2, v2 in vals2.items():
                        if min_v1 <= v2:
                            valid_z2.add(c2)
            except TypeError:
                _prune_pairwise_fallback(graph, pred, vals1, vals2, valid_z1, valid_z2)
        elif op in (ComparisonOp.GT, ComparisonOp.GE):
            try:
                min_v2 = min(vals2.values())
                max_v1 = max(vals1.values())
                if op == ComparisonOp.GT:
                    for c1, v1 in vals1.items():
                        if v1 > min_v2:
                            valid_z1.add(c1)
                    for c2, v2 in vals2.items():
                        if max_v1 > v2:
                            valid_z2.add(c2)
                else:
                    for c1, v1 in vals1.items():
                        if v1 >= min_v2:
                            valid_z1.add(c1)
                    for c2, v2 in vals2.items():
                        if max_v1 >= v2:
                            valid_z2.add(c2)
            except TypeError:
                _prune_pairwise_fallback(graph, pred, vals1, vals2, valid_z1, valid_z2)
        else:
            _prune_pairwise_fallback(graph, pred, vals1, vals2, valid_z1, valid_z2)
        F[z1] = cands1 & valid_z1 if valid_z1 else set()
        F[z2] = cands2 & valid_z2 if valid_z2 else set()

def _prune_pairwise_fallback(graph: Graph, pred: PairWisePredicate, vals1: Dict[Any, Any], vals2: Dict[Any, Any], valid_z1: Set[Any], valid_z2: Set[Any]) -> None:
    nodes_cache: Dict[Any, Optional[Node]] = {}
    for c1 in vals1:
        if c1 not in nodes_cache:
            nodes_cache[c1] = graph.get_node(c1)
        node1 = nodes_cache[c1]
        if node1 is None:
            continue
        for c2 in vals2:
            if c2 not in nodes_cache:
                nodes_cache[c2] = graph.get_node(c2)
            node2 = nodes_cache[c2]
            if node2 is None:
                continue
            try:
                if pred.evaluate(node1, node2):
                    valid_z1.add(c1)
                    valid_z2.add(c2)
            except (TypeError, ValueError):
                pass

def group_refinement(local_results: Dict[int, Dict[str, Set[Any]]]) -> Dict[str, Set[Any]]:
    result: Dict[str, Set[Any]] = {}
    for rule_idx, local_map in local_results.items():
        for var, candidates in local_map.items():
            if var not in result:
                result[var] = set(candidates)
            else:
                result[var] |= candidates
    return result

def _pick_best_scored_item(item_scores: Dict[Any, float]) -> Tuple[Any, float]:
    best_item, best_score = min(item_scores.items(), key=lambda item: (-float(item[1]), repr(item[0])))
    return (best_item, float(best_score))

def coupled_ctr(graph: Graph, user_id: Any, sigma_tie: List[TIERule], sigma_omr: List[OMRule], dis_model: DisModel, aggr_model: AggrModel, ml_cache: MLPredicateCache, lambda_param: float=0.6, training_data: Optional[Dict]=None, max_iterations: int=100, topic_model: Optional[TopicProximityModel]=None, progress: bool=False, offline_index: Optional['OfflineIndex']=None) -> Tuple[Set[Any], Dict[Any, Tuple[Any, float]]]:
    from models import RankingScoreComputer
    _use_tqdm = progress and tqdm is not None
    t_start = time.perf_counter()
    if not sigma_tie:
        return (set(), {})
    user_ml_cache = ml_cache.get_user_view(user_id) if hasattr(ml_cache, 'get_user_view') else ml_cache
    if any((rule.get_dis_predicates() for rule in sigma_omr)) and hasattr(dis_model, 'ensure_threshold_index'):
        dis_model.ensure_threshold_index()
    score_accumulators: Dict[Any, Dict[Any, float]] = {}
    anchor_items: Set[Any] = set()
    compact_graph: Optional['CompactGraph'] = getattr(graph, '_compact', None)
    sigma_omr_size = len(sigma_omr)
    scorer = RankingScoreComputer(graph=graph, dis_model=dis_model, aggr_model=aggr_model, ml_cache=user_ml_cache, lambda_param=lambda_param, sigma_omr_size=max(sigma_omr_size, 1), topic_model=topic_model)
    if hasattr(scorer, 'prepare_user'):
        scorer.prepare_user(user_id)
    dis_score_cache: Dict[Tuple[Any, Any], float] = {}
    _ctr_phase_hook = _CTR_PHASE_TIMING_HOOK
    _track_phase_times = progress or callable(_ctr_phase_hook)
    _phase_times: Optional[Dict[str, float]] = defaultdict(float) if _track_phase_times else None
    if progress:
        print(f'  [coupled_ctr] user={user_id}, {len(sigma_tie)} TIEs, {len(sigma_omr)} OMRs')
    _tie_iter = enumerate(sigma_tie)
    if _use_tqdm:
        _tie_iter = tqdm(list(_tie_iter), desc='  TIE groups', unit='grp')
    for tie_idx, tie_rule in _tie_iter:
        logger.debug(f'[coupled_ctr] Processing TIE {tie_idx + 1}/{len(sigma_tie)}')
        user_node = graph.get_node(user_id)
        if user_node is None:
            continue
        tie_x0 = tie_rule.pattern.user_center
        tie_x0_node = tie_rule.pattern.nodes.get(tie_x0)
        if tie_x0_node is not None and tie_x0_node.label and (user_node.label != tie_x0_node.label):
            continue
        tie_x0_preds = tie_rule.get_point_wise_predicates().get(tie_x0, [])
        if tie_x0_preds and (not all((pred.evaluate(user_node) for pred in tie_x0_preds))):
            continue
        _t_grp = time.perf_counter()
        _t0 = time.perf_counter()
        H, joint_pattern, rooted_dag = construct_rule_group(user_id, tie_rule, sigma_omr, graph)
        if not H:
            if _use_tqdm:
                _tie_iter.set_postfix_str('skip(empty H)')
            continue
        omr_rules_in_H = [r for r in H if isinstance(r, OMRule)]
        if _phase_times is not None:
            _phase_times['construct_group'] += time.perf_counter() - _t0
        _t0 = time.perf_counter()
        omr_rule_indices_in_H = [rule_idx for rule_idx, rule in enumerate(H) if isinstance(rule, OMRule)]
        candidate_map, all_pw = init_candidate_map(graph, joint_pattern, user_id, tie_rule, omr_rules_in_H, ml_cache=user_ml_cache, aggr_model=aggr_model, dis_model=dis_model, offline_index=offline_index)
        if _phase_times is not None:
            _phase_times['init_candidate_map'] += time.perf_counter() - _t0
        if not candidate_map:
            continue
        x0 = tie_rule.pattern.user_center
        if x0 not in candidate_map or not candidate_map[x0]:
            continue
        y0 = tie_rule.pattern.item_center
        if y0 not in candidate_map or not candidate_map[y0]:
            continue
        if progress:
            _cand_sizes = {v: len(s) for v, s in candidate_map.items()}
            print(f'    TIE-{tie_idx + 1}: H={len(H)} rules ({1 + len(omr_rules_in_H)} = 1 TIE + {len(omr_rules_in_H)} OMR), C_H sizes: {dict(sorted(_cand_sizes.items(), key=lambda x: -x[1])[:5])}')
        star_cache_user: Dict[Any, Dict[str, Set[Any]]] = {}
        star_cache_item: Dict[Any, Dict[str, Set[Any]]] = {}
        star_cache_alt_per_rule: Dict[int, Dict[Any, Dict[str, Set[Any]]]] = {}
        bridge_idx_per_rule: Dict[int, Any] = {}
        dis_pass_per_rule: Dict[int, Dict[Any, frozenset]] = {}
        omr_rule_elapsed: Dict[int, float] = defaultdict(float)
        if offline_index is not None:
            _ofc_item = offline_index.item_star.get(tie_idx, {})
            for _v in candidate_map.get(y0, set()):
                if _v in _ofc_item:
                    _val = _ofc_item[_v]
                    if _val is not None:
                        star_cache_item[_v] = _val
            for _ri, _rule in enumerate(H):
                if isinstance(_rule, OMRule):
                    _gi = _get_offline_omr_index(offline_index, _rule)
                    if _gi is not None:
                        _ofa = offline_index.alt_star.get(_gi, {})
                        _bri = offline_index.bridge_idx.get(_gi)
                        _odp = offline_index.dis_pass.get(_gi)
                        if _bri is not None:
                            bridge_idx_per_rule[_ri] = _bri
                        if _odp:
                            dis_pass_per_rule[_ri] = _odp
                        if _ofa:
                            _w0v = _rule.pattern.alt_center
                            _w0s = candidate_map.get(_w0v, set())
                            _ac = {}
                            for _w in _w0s:
                                if _w in _ofa and _ofa[_w] is not None:
                                    _ac[_w] = _ofa[_w]
                            if _ac:
                                star_cache_alt_per_rule[_ri] = _ac
        _t0 = time.perf_counter()
        _dis_precomputed = False
        if omr_rules_in_H:
            w0_var = omr_rules_in_H[0].pattern.alt_center
            y0_cands = candidate_map.get(y0, set())
            w0_cands = candidate_map.get(w0_var, set())
            _product = len(y0_cands) * len(w0_cands)
            if _product < 500000:
                precompute_bridge_idx_by_rule = {local_idx: bridge_idx_per_rule[h_rule_idx] for local_idx, h_rule_idx in enumerate(omr_rule_indices_in_H) if h_rule_idx in bridge_idx_per_rule}
                precompute_ws_by_v = _build_ctr_precompute_w_candidates_by_anchor(graph=graph, user_id=user_id, omr_rules_in_H=omr_rules_in_H, y0_candidates=y0_cands, w0_candidates=w0_cands, bridge_idx_per_rule=precompute_bridge_idx_by_rule)
                for local_idx, h_rule_idx in enumerate(omr_rule_indices_in_H):
                    bridge_idx = precompute_bridge_idx_by_rule.get(local_idx)
                    if bridge_idx is not None:
                        bridge_idx_per_rule.setdefault(h_rule_idx, bridge_idx)
                if offline_index is not None and offline_index.dis_scores:
                    for v in y0_cands:
                        precompute_ws = precompute_ws_by_v.get(v, w0_cands)
                        _batch = {}
                        for w in precompute_ws:
                            _ds = offline_index.dis_scores.get((v, w))
                            if _ds is not None:
                                _batch[w] = _ds
                        if _batch:
                            user_ml_cache.batch_set_dis_scores(v, _batch)
                        _uncached = precompute_ws - set(_batch.keys())
                        if _uncached:
                            _extra = dis_model.compute_batch(v, _uncached)
                            user_ml_cache.batch_set_dis_scores(v, _extra)
                else:
                    if _use_tqdm:
                        _dis_iter = tqdm(y0_cands, desc='    Precompute Mdis', unit='v', leave=False)
                    else:
                        _dis_iter = y0_cands
                    for v in _dis_iter:
                        precompute_ws = precompute_ws_by_v.get(v, w0_cands)
                        batch_scores = dis_model.compute_batch(v, precompute_ws)
                        user_ml_cache.batch_set_dis_scores(v, batch_scores)
                _dis_precomputed = True
            elif progress:
                print(f'    ⚠ Mdis full precompute too large: |y₀|×|w₀| = {len(y0_cands)}×{len(w0_cands)} = {_product:,} > 500k → dis pruning deferred to scoring')
            user_ml_cache.precompute_aggr(aggr_model, w0_cands)
        if _phase_times is not None:
            _phase_times['precompute_ml'] += time.perf_counter() - _t0
        F_H = {var: set(cands) for var, cands in candidate_map.items()}
        prev_F_H_signature = _candidate_map_signature(F_H)
        refined_once = False
        runtime = _get_rule_group_runtime_cached(graph, H, compact_graph)
        prefilter_caches = runtime['prefilter_caches']
        edge_lookups_per_rule = runtime['edge_lookups_per_rule']
        star_contexts_per_rule = runtime['star_contexts_per_rule']
        rule_runtime_per_rule = runtime['rule_runtime_per_rule']
        scored_pairs_per_rule: Dict[int, Dict[Any, Set[Any]]] = {}
        iteration_limit = max(int(max_iterations), len(joint_pattern.nodes), 5)
        for iteration in range(iteration_limit):
            _t_iter = time.perf_counter()
            local_results: Dict[int, Dict[str, Set[Any]]] = {}
            current_tie_anchors: Set[Any] = set()
            _t0 = time.perf_counter()
            for rule_idx, rule in enumerate(H):
                if not isinstance(rule, TIERule):
                    continue
                rule_pw = rule.get_point_wise_predicates()
                F_phi = local_refinement(graph, F_H, rooted_dag, rule, rule_pw, compact_graph, star_cache_user, star_cache_item, None, prefilter_cache=prefilter_caches.get(rule_idx), edge_lookups=edge_lookups_per_rule.get(rule_idx), star_contexts=star_contexts_per_rule.get(rule_idx), rule_runtime=rule_runtime_per_rule.get(rule_idx))
                if F_phi is not None:
                    local_results[rule_idx] = F_phi
                    current_tie_anchors |= F_phi.get(y0, set())
            if _phase_times is not None:
                _phase_times['local_tie'] += time.perf_counter() - _t0
            _ml_preds = tie_rule.get_ml_predicates()
            if _ml_preds and ml_cache is not None and current_tie_anchors:
                _verified = set()
                for _v in current_tie_anchors:
                    _score = user_ml_cache.get_prediction(user_id, _v)
                    if all((mp.evaluate(_score) for mp in _ml_preds)):
                        _verified.add(_v)
                current_tie_anchors = _verified
            _t0 = time.perf_counter()
            bridge_vw_per_rule: Dict[int, Optional[Dict[Any, Set[Any]]]] = {}
            _omr_rules_in_iter = [(rule_idx, rule) for rule_idx, rule in enumerate(H) if isinstance(rule, OMRule)]
            _omr_iter = _omr_rules_in_iter
            if _use_tqdm and iteration == 0 and (len(_omr_rules_in_iter) > 3):
                _omr_iter = tqdm(_omr_rules_in_iter, desc=f'    OMR rules (iter {iteration})', unit='rule', leave=False)
            if progress and iteration == 0:
                _local_refinement_omr._step_times = defaultdict(float)
            for rule_idx, rule in _omr_iter:
                _t_rule = time.perf_counter()
                rule_pw = rule.get_point_wise_predicates()
                _bvw: Dict[Any, Set[Any]] = {}
                if rule_idx not in star_cache_alt_per_rule:
                    star_cache_alt_per_rule[rule_idx] = {}
                _rule_alt_cache = star_cache_alt_per_rule[rule_idx]
                F_phi = local_refinement(graph, F_H, rooted_dag, rule, rule_pw, compact_graph, star_cache_user, star_cache_item, _rule_alt_cache, current_tie_anchors=current_tie_anchors, ml_cache=user_ml_cache, prefilter_cache=prefilter_caches.get(rule_idx), edge_lookups=edge_lookups_per_rule.get(rule_idx), star_contexts=star_contexts_per_rule.get(rule_idx), rule_runtime=rule_runtime_per_rule.get(rule_idx), bridge_vw_out=_bvw, dis_precomputed=_dis_precomputed, offline_bridge_idx=bridge_idx_per_rule.get(rule_idx), offline_dis_pass=dis_pass_per_rule.get(rule_idx))
                if F_phi is not None:
                    local_results[rule_idx] = F_phi
                    bridge_vw_per_rule[rule_idx] = _bvw if _bvw else None
                omr_rule_elapsed[rule_idx] += time.perf_counter() - _t_rule
            if _phase_times is not None:
                _phase_times['local_omr'] += time.perf_counter() - _t0
                _omr_st = getattr(_local_refinement_omr, '_step_times', None)
                if progress and _omr_st and (iteration == 0):
                    _omr_total = sum(_omr_st.values())
                    print(f'      OMR step breakdown (iter 0, {len(_omr_rules_in_iter)} rules, total {_omr_total:.2f}s):')
                    for step, st in sorted(_omr_st.items()):
                        pct = 100 * st / _omr_total if _omr_total > 0 else 0
                        print(f'        {step:20s}: {st:8.2f}s  ({pct:5.1f}%)')
                    _local_refinement_omr._step_times = None
            if not local_results:
                break
            _t0 = time.perf_counter()
            scored_omr_results = [(rule_idx, rule) for rule_idx, rule in _omr_rules_in_iter if rule_idx in local_results]
            if scored_omr_results:
                _shared_aggr: Optional[Dict[Any, float]] = None
                _w0_var = omr_rules_in_H[0].pattern.alt_center
                _w0_cands = F_H.get(_w0_var, set())
                if _w0_cands:
                    _shared_aggr = aggr_model.compute_batch(_w0_cands)
                for rule_idx, rule in scored_omr_results:
                    _t_rule = time.perf_counter()
                    bridge_vw = bridge_vw_per_rule.get(rule_idx)
                    if bridge_vw is None and any((origin == rule.pattern.anchor_center for origin in rule.pattern.bridge_origins)):
                        bridge_vw = _compute_bridge_vw_pairs(graph, rule, local_results[rule_idx], rule.get_point_wise_predicates(), prefilter_cache=prefilter_caches.get(rule_idx))
                    if bridge_vw is None and rule.pattern.bridge_paths:
                        bridge_vw = _compute_vw_pairs_by_anchor_refinement(graph=graph, user_id=user_id, rooted_dag=rooted_dag, rule=rule, F_phi=local_results[rule_idx], compact_graph=compact_graph, star_cache_user=star_cache_user, star_cache_item=star_cache_item, star_cache_alt=star_cache_alt_per_rule.get(rule_idx), ml_cache=user_ml_cache, prefilter_cache=prefilter_caches.get(rule_idx), edge_lookups=edge_lookups_per_rule.get(rule_idx), dis_precomputed=_dis_precomputed, offline_bridge_idx=bridge_idx_per_rule.get(rule_idx), offline_dis_pass=dis_pass_per_rule.get(rule_idx), star_contexts=star_contexts_per_rule.get(rule_idx), rule_runtime=rule_runtime_per_rule.get(rule_idx))
                    _update_omr_scores(user_id, rule, local_results[rule_idx], score_accumulators, scorer, dis_model, aggr_model, dis_score_cache, bridge_vw_pairs=bridge_vw, aggr_scores_shared=_shared_aggr, scored_pairs=scored_pairs_per_rule.setdefault(rule_idx, {}))
                    omr_rule_elapsed[rule_idx] += time.perf_counter() - _t_rule
            if _phase_times is not None:
                _phase_times['scoring'] += time.perf_counter() - _t0
            new_F_H = group_refinement(local_results)
            refined_once = True
            for var in candidate_map:
                if var not in new_F_H:
                    new_F_H[var] = set()
            new_signature = _candidate_map_signature(new_F_H)
            converged = new_signature == prev_F_H_signature
            prev_F_H_signature = new_signature
            F_H = new_F_H
            if not converged:
                _invalidate_star_caches(star_cache_user, F_H.get(x0, set()), star_cache_item, F_H.get(y0, set()), {}, set())
                for _rc_idx, _rc in star_cache_alt_per_rule.items():
                    _rc_rule = H[_rc_idx]
                    _valid_alts = F_H.get(_rc_rule.pattern.alt_center, set()) if isinstance(_rc_rule, OMRule) else set()
                    _invalidate_star_caches({}, set(), {}, set(), _rc, _valid_alts)
            if progress:
                _iter_el = time.perf_counter() - _t_iter
                _fh_total = sum((len(s) for s in F_H.values()))
                print(f'      iter {iteration}: {_iter_el:.2f}s, |F_H|={_fh_total}, local={len(local_results)}/{len(H)} rules ok' + (' → CONVERGED' if converged else ''))
            if converged:
                logger.debug(f'  Converged after {iteration + 1} iterations')
                break
        else:
            logger.warning(f'  [coupled_ctr] Max iterations ({iteration_limit}) reached for TIE {tie_idx} without convergence')
        if not refined_once:
            continue
        if progress:
            _grp_el = time.perf_counter() - _t_grp
            print(f'    TIE-{tie_idx + 1} total: {_grp_el:.2f}s, anchors so far: {len(anchor_items)}')
        tie_anchors = F_H.get(y0, set())
        ml_preds = tie_rule.get_ml_predicates()
        if ml_preds:
            verified_anchors = set()
            for v in tie_anchors:
                ml_ok = True
                for mp in ml_preds:
                    score = user_ml_cache.get_prediction(user_id, v)
                    if not mp.evaluate(score):
                        ml_ok = False
                        break
                if ml_ok:
                    verified_anchors.add(v)
            tie_anchors = verified_anchors
        anchor_items |= tie_anchors
        _group_hook = _RULE_GROUP_TIMING_HOOK
        if callable(_group_hook):
            try:
                _group_hook(tie_rule, time.perf_counter() - _t_grp)
            except Exception:
                logger.debug('[coupled_ctr] rule timing hook failed', exc_info=True)
        _omr_hook = _OMR_RULE_TIMING_HOOK
        if callable(_omr_hook):
            for _rule_idx, _elapsed in omr_rule_elapsed.items():
                _rule = H[_rule_idx]
                if not isinstance(_rule, OMRule):
                    continue
                try:
                    _omr_hook(_rule, float(_elapsed))
                except Exception:
                    logger.debug('[coupled_ctr] omr timing hook failed', exc_info=True)
    alternatives: Dict[Any, Tuple[Any, float]] = {}
    for v, w_scores in score_accumulators.items():
        if v not in anchor_items:
            continue
        if not w_scores:
            continue
        best_w, best_score = _pick_best_scored_item(w_scores)
        best_score = max(0.0, min(1.0, best_score))
        alternatives[v] = (best_w, best_score)
    elapsed = time.perf_counter() - t_start
    if callable(_ctr_phase_hook) and _phase_times is not None:
        try:
            _ctr_phase_hook({'task': 'ctr', 'user_id': user_id, 'total_seconds': float(elapsed), 'n_tie_rules': len(sigma_tie), 'n_omr_rules': len(sigma_omr), 'n_anchor_items': len(anchor_items), 'n_alternatives': len(alternatives), 'phases': {phase: float(t) for phase, t in _phase_times.items()}})
        except Exception:
            logger.debug('[coupled_ctr] phase timing hook failed', exc_info=True)
    if progress and _phase_times:
        print(f'  ── Phase timing breakdown (total {elapsed:.2f}s) ──')
        for phase, t in sorted(_phase_times.items(), key=lambda x: -x[1]):
            pct = 100 * t / elapsed if elapsed > 0 else 0
            print(f'    {phase:25s}: {t:8.2f}s  ({pct:5.1f}%)')
    return (anchor_items, alternatives)

def _compute_bridge_vw_pairs(graph: Graph, rule: OMRule, F_phi: Dict[str, Set[Any]], point_wise_preds: Dict[str, List[PointWisePredicate]], prefilter_cache: Optional[Dict[str, Any]]=None) -> Optional[Dict[Any, Set[Any]]]:
    omr_pattern = rule.pattern
    y0 = omr_pattern.anchor_center
    w0 = omr_pattern.alt_center
    bridge_paths = omr_pattern.bridge_paths
    bridge_origins = omr_pattern.bridge_origins
    y0_cands = F_phi.get(y0, set())
    w0_cands = F_phi.get(w0, set())
    if not y0_cands or not w0_cands:
        return None
    y0_bridge_matches: List[Tuple[Path, Dict[Any, Dict[str, Set[Any]]]]] = []
    alt_matches_per_w: Dict[Any, Dict[str, Set[Any]]] = {}
    for bp_idx, bridge_path in enumerate(bridge_paths):
        origin = bridge_origins[bp_idx]
        if origin != y0:
            continue
        bp_seq = bridge_path.path_sequence if hasattr(bridge_path, 'path_sequence') else []
        if not bp_seq:
            continue
        bp_edge_lookup = {(e.source_var, e.target_var): (e.label, e.is_forward) for e in bridge_path.edges}
        per_v_match: Dict[Any, Dict[str, Set[Any]]] = {}
        for v in y0_cands:
            bp_match = match_path_compact(graph, bridge_path, v, omr_pattern, point_wise_preds, edge_lookup=bp_edge_lookup, prefilter_cache=prefilter_cache)
            per_v_match[v] = {var: set(cands) & F_phi.get(var, set()) if var in F_phi else set(cands) for var, cands in (bp_match or {}).items()}
        y0_bridge_matches.append((bridge_path, per_v_match))
    if not y0_bridge_matches:
        return None
    alt_paths = omr_pattern.alt_star.paths if hasattr(omr_pattern.alt_star, 'paths') else []
    if alt_paths:
        alt_edge_lookups = _build_edge_lookups(alt_paths)
        for w in w0_cands:
            w_match = _match_star_paths(graph, omr_pattern, w, alt_paths, point_wise_preds, alt_edge_lookups, prefilter_cache)
            if w_match is not None:
                alt_matches_per_w[w] = {var: set(cands) & F_phi.get(var, set()) if var in F_phi else set(cands) for var, cands in w_match.items()}
    return _derive_bridge_vw_pairs_from_matches(rule, F_phi, y0_bridge_matches, alt_matches_per_w)

def _build_needed_pairwise_attrs(pair_wise_preds: List[PairWisePredicate]) -> Dict[str, Set[str]]:
    needed_attrs: Dict[str, Set[str]] = {}
    for pred in pair_wise_preds:
        needed_attrs.setdefault(pred.var1, set()).add(pred.attr1)
        needed_attrs.setdefault(pred.var2, set()).add(pred.attr2)
    return needed_attrs

def _get_x0_bridge_prefilter_candidates(graph: Graph, user_id: Any, rule: OMRule, F_phi: Dict[str, Set[Any]], compact_graph: Optional['CompactGraph']=None, star_cache_user: Optional[Dict[Any, Dict[str, Set[Any]]]]=None, star_cache_item: Optional[Dict[Any, Dict[str, Set[Any]]]]=None, star_cache_alt: Optional[Dict[Any, Dict[str, Set[Any]]]]=None, prefilter_cache: Optional[Dict[str, Any]]=None, edge_lookups: Optional[Dict[str, List[Dict]]]=None, star_contexts: Optional[Dict[str, Any]]=None, rule_runtime: Optional[Dict[str, Any]]=None) -> Tuple[Optional[Dict[Any, Set[Any]]], Dict[Any, Set[Any]]]:
    omr_pattern = rule.pattern
    tie_pattern = omr_pattern.tie_pattern
    x0 = omr_pattern.user_center
    y0 = omr_pattern.anchor_center
    w0 = omr_pattern.alt_center
    bridge_paths = list((rule_runtime or {}).get('bridge_paths', omr_pattern.bridge_paths))
    bridge_origins = list((rule_runtime or {}).get('bridge_origins', omr_pattern.bridge_origins))
    if not bridge_paths or any((origin != x0 for origin in bridge_origins)):
        return (None, {})
    if _has_ambiguous_multi_shared_bridge_join(rule, bridge_paths):
        return (None, {})
    y0_cands = F_phi.get(y0, set())
    w0_cands = F_phi.get(w0, set())
    if not y0_cands or not w0_cands:
        return ({}, {})
    point_wise_preds = rule.get_point_wise_predicates()
    omr_extra_pw = (rule_runtime or {}).get('omr_extra_pw')
    if omr_extra_pw is None:
        omr_extra_pw = rule.get_omr_only_point_wise()
    safe_tie_leaf_extra_pw = dict((rule_runtime or {}).get('safe_tie_leaf_extra_pw', {}))
    has_unsafe_extra_on_tie_stars = (rule_runtime or {}).get('has_unsafe_extra_on_tie_stars')
    if has_unsafe_extra_on_tie_stars is None:
        tie_star_vars = set(tie_pattern.nodes.keys()) - {x0, y0}
        safe_tie_leaf_vars = _get_safe_tie_star_leaf_vars(tie_pattern)
        has_unsafe_extra_on_tie_stars = any((var in tie_star_vars and var not in safe_tie_leaf_vars for var in omr_extra_pw))
        if not safe_tie_leaf_extra_pw:
            safe_tie_leaf_extra_pw = {var: tuple(preds) for var, preds in omr_extra_pw.items() if var in safe_tie_leaf_vars}
    use_shared_tie_star_cache = not has_unsafe_extra_on_tie_stars
    if edge_lookups is not None:
        user_edge_lookups = edge_lookups.get('user', _build_edge_lookups(tie_pattern.user_paths))
        item_edge_lookups = edge_lookups.get('item', _build_edge_lookups(tie_pattern.item_paths))
        alt_edge_lookups = edge_lookups.get('alt', _build_edge_lookups(omr_pattern.alt_star.paths if hasattr(omr_pattern.alt_star, 'paths') else ()))
    else:
        user_edge_lookups = _build_edge_lookups(tie_pattern.user_paths)
        item_edge_lookups = _build_edge_lookups(tie_pattern.item_paths)
        alt_edge_lookups = _build_edge_lookups(omr_pattern.alt_star.paths if hasattr(omr_pattern.alt_star, 'paths') else ())
    user_star_ctx = (star_contexts or {}).get('user')
    item_star_ctx = (star_contexts or {}).get('item')
    alt_star_ctx = (star_contexts or {}).get('alt')
    user_match = None
    if star_cache_user is not None and use_shared_tie_star_cache and (user_id in star_cache_user):
        user_match = _post_filter_leaf_star_match(graph, star_cache_user[user_id], safe_tie_leaf_extra_pw)
    else:
        user_match = _match_star_paths(graph, tie_pattern, user_id, tie_pattern.user_paths, point_wise_preds, user_edge_lookups, prefilter_cache, compact_graph, star_match_ctx=user_star_ctx) if tie_pattern.user_paths else {x0: {user_id}}
        if user_match is not None and star_cache_user is not None and use_shared_tie_star_cache and (not safe_tie_leaf_extra_pw):
            star_cache_user[user_id] = user_match
    if user_match is None:
        return ({}, {})
    bridge_matches: List[Tuple[Path, Dict[str, Set[Any]]]] = []
    bridge_edge_lookups = list((rule_runtime or {}).get('bridge_edge_lookups', []))
    for bp_idx, bridge_path in enumerate(bridge_paths):
        bp_edge_lookup = bridge_edge_lookups[bp_idx] if bp_idx < len(bridge_edge_lookups) else {(e.source_var, e.target_var): (e.label, e.is_forward) for e in bridge_path.edges}
        bp_match = match_path_compact(graph, bridge_path, user_id, omr_pattern, point_wise_preds, edge_lookup=bp_edge_lookup, prefilter_cache=prefilter_cache)
        if bp_match is None:
            return ({}, {})
        bridge_matches.append((bridge_path, {var: set(cands) & F_phi.get(var, set()) if var in F_phi else set(cands) for var, cands in bp_match.items()}))
    alt_paths = list((rule_runtime or {}).get('alt_paths', omr_pattern.alt_star.paths if hasattr(omr_pattern.alt_star, 'paths') else []))
    alt_matches_per_w: Dict[Any, Dict[str, Set[Any]]] = {}
    for w in w0_cands:
        if star_cache_alt is not None and w in star_cache_alt:
            w_match = star_cache_alt[w]
        else:
            w_match = _match_star_paths(graph, omr_pattern, w, alt_paths, point_wise_preds, alt_edge_lookups, prefilter_cache, compact_graph, star_match_ctx=alt_star_ctx) if alt_paths else {w0: {w}}
            if star_cache_alt is not None and w_match is not None:
                star_cache_alt[w] = w_match
        if w_match is None:
            continue
        alt_matches_per_w[w] = {var: set(cands) & F_phi.get(var, set()) if var in F_phi else set(cands) for var, cands in w_match.items()}
    if not alt_matches_per_w:
        return ({}, {})
    alt_star_vars = set(omr_pattern.alt_star.nodes.keys()) if hasattr(omr_pattern.alt_star, 'nodes') else set()
    globally_valid_ws = set(alt_matches_per_w.keys())
    merged_bridge_match: Dict[str, Set[Any]] = {x0: {user_id}}
    for bridge_path, bp_match in bridge_matches:
        for var, cands in bp_match.items():
            if var in merged_bridge_match:
                merged_bridge_match[var] &= set(cands)
            else:
                merged_bridge_match[var] = set(cands)
            if var in F_phi:
                merged_bridge_match[var] &= F_phi[var]
            if not merged_bridge_match[var]:
                return ({}, {})
        terminal_var = bridge_path.path_sequence[-1]
        if terminal_var == w0:
            path_ws = set(bp_match.get(w0, set())) & globally_valid_ws
        else:
            shared_vars = (set(bridge_path.nodes.keys()) & alt_star_vars) - {x0, y0, w0}
            if not shared_vars:
                return {}
            path_ws: Set[Any] = set()
            for w, w_match in alt_matches_per_w.items():
                ok = True
                for shared_var in shared_vars:
                    bridge_cands = set(bp_match.get(shared_var, set()))
                    alt_cands = set(w_match.get(shared_var, set()))
                    if shared_var in F_phi:
                        bridge_cands &= F_phi[shared_var]
                        alt_cands &= F_phi[shared_var]
                    if not bridge_cands or not alt_cands or bridge_cands.isdisjoint(alt_cands):
                        ok = False
                        break
                if ok:
                    path_ws.add(w)
        globally_valid_ws &= path_ws
        if not globally_valid_ws:
            return ({}, {})
    pair_wise_preds = rule.get_pair_wise_predicates()
    if not pair_wise_preds:
        exact_pairs = {v: set(globally_valid_ws) for v in y0_cands}
        return (exact_pairs, exact_pairs)
    needed_attrs = _build_needed_pairwise_attrs(pair_wise_preds)
    anchor_attr_cache: Dict[Any, Tuple[Dict[Tuple[str, str], frozenset], Dict[Tuple[str, str], Tuple]]] = {}
    item_match_cache: Dict[Any, Optional[Dict[str, Set[Any]]]] = {}
    for v in y0_cands:
        if star_cache_item is not None and use_shared_tie_star_cache and (v in star_cache_item):
            v_match = star_cache_item[v]
        elif v in item_match_cache:
            v_match = item_match_cache[v]
        else:
            v_match = _match_star_paths(graph, tie_pattern, v, tie_pattern.item_paths, point_wise_preds, item_edge_lookups, prefilter_cache, compact_graph, star_match_ctx=item_star_ctx) if tie_pattern.item_paths else {y0: {v}}
            item_match_cache[v] = v_match
            if v_match is not None and star_cache_item is not None and use_shared_tie_star_cache:
                star_cache_item[v] = v_match
        if v_match is None:
            continue
        anchor_match: Dict[str, Set[Any]] = {x0: {user_id}, y0: {v}}
        for component in (user_match, v_match):
            for var, cands in component.items():
                cands_set = set(cands)
                if var in F_phi:
                    cands_set &= F_phi[var]
                if var in anchor_match:
                    anchor_match[var] &= cands_set
                else:
                    anchor_match[var] = cands_set
        if any((not cands for cands in anchor_match.values())):
            continue
        anchor_attr_cache[v] = _collect_pairwise_attr_data(graph, anchor_match, needed_attrs)
    w_attr_cache: Dict[Any, Tuple[Dict[Tuple[str, str], frozenset], Dict[Tuple[str, str], Tuple]]] = {}
    for w in globally_valid_ws:
        w_match = alt_matches_per_w[w]
        combined_w_match: Dict[str, Set[Any]] = {w0: {w}}
        for component in (merged_bridge_match, w_match):
            for var, cands in component.items():
                cands_set = set(cands)
                if var in F_phi:
                    cands_set &= F_phi[var]
                if var in combined_w_match:
                    combined_w_match[var] &= cands_set
                else:
                    combined_w_match[var] = cands_set
        if any((not cands for cands in combined_w_match.values())):
            continue
        w_attr_cache[w] = _collect_pairwise_attr_data(graph, combined_w_match, needed_attrs)
    pw_fast_exact = _pairwise_vars_independent(pair_wise_preds)
    per_anchor_ws: Dict[Any, Set[Any]] = {}
    exact_ws_per_anchor: Dict[Any, Set[Any]] = {}
    for v, (anchor_vals, anchor_bounds) in anchor_attr_cache.items():
        valid_ws: Set[Any] = set()
        exact_ws: Set[Any] = set()
        for w in globally_valid_ws:
            w_data = w_attr_cache.get(w)
            if w_data is None:
                continue
            quick = _verify_pairwise_fast(anchor_vals, anchor_bounds, w_data[0], w_data[1], pair_wise_preds)
            if quick is False:
                continue
            valid_ws.add(w)
            if pw_fast_exact and quick is True:
                exact_ws.add(w)
        if valid_ws:
            per_anchor_ws[v] = valid_ws
        if exact_ws:
            exact_ws_per_anchor[v] = exact_ws
    return (per_anchor_ws, exact_ws_per_anchor)

def _compute_vw_pairs_by_anchor_refinement_reference(graph: Graph, user_id: Any, rooted_dag: Optional[RootedDAG], rule: OMRule, F_phi: Dict[str, Set[Any]], compact_graph: Optional['CompactGraph']=None, star_cache_user: Optional[Dict[Any, Dict[str, Set[Any]]]]=None, star_cache_item: Optional[Dict[Any, Dict[str, Set[Any]]]]=None, star_cache_alt: Optional[Dict[Any, Dict[str, Set[Any]]]]=None, ml_cache: Optional[MLPredicateCache]=None, prefilter_cache: Optional[Dict[str, Any]]=None, edge_lookups: Optional[Dict[str, List[Dict]]]=None, dis_precomputed: bool=True, offline_bridge_idx: Optional[Any]=None, offline_dis_pass: Optional[Dict[Any, frozenset]]=None, star_contexts: Optional[Dict[str, Any]]=None, rule_runtime: Optional[Dict[str, Any]]=None, candidate_ws_per_anchor: Optional[Dict[Any, Set[Any]]]=None) -> Optional[Dict[Any, Set[Any]]]:
    x0 = rule.pattern.user_center
    y0 = rule.pattern.anchor_center
    w0 = rule.pattern.alt_center
    y0_cands = F_phi.get(y0, set())
    w0_cands = F_phi.get(w0, set())
    if not y0_cands or not w0_cands:
        return None
    rule_pw = rule.get_point_wise_predicates()
    base_candidate_map = {var: set(cands) for var, cands in F_phi.items()}
    base_candidate_map[x0] = {user_id}
    per_anchor_ws: Dict[Any, Set[Any]] = {}
    for v in y0_cands:
        valid_ws: Set[Any] = set()
        iter_w0_cands = candidate_ws_per_anchor.get(v, set()) if candidate_ws_per_anchor is not None else w0_cands
        if not iter_w0_cands:
            continue
        for w in iter_w0_cands:
            candidate_map = {var: {v} if var == y0 else {w} if var == w0 else set(cands) for var, cands in base_candidate_map.items()}
            candidate_map[y0] = {v}
            candidate_map[w0] = {w}
            F_vw = local_refinement(graph, candidate_map, rooted_dag, rule, rule_pw, compact_graph, star_cache_user, star_cache_item, star_cache_alt, current_tie_anchors={v}, ml_cache=ml_cache, prefilter_cache=prefilter_cache, edge_lookups=edge_lookups, star_contexts=star_contexts, rule_runtime=rule_runtime, dis_precomputed=dis_precomputed, offline_bridge_idx=offline_bridge_idx, offline_dis_pass=offline_dis_pass)
            if F_vw is not None and w in F_vw.get(w0, set()):
                valid_ws.add(w)
        if valid_ws:
            per_anchor_ws[v] = valid_ws
    return per_anchor_ws or None

def _compute_vw_pairs_by_anchor_refinement(graph: Graph, user_id: Any, rooted_dag: Optional[RootedDAG], rule: OMRule, F_phi: Dict[str, Set[Any]], compact_graph: Optional['CompactGraph']=None, star_cache_user: Optional[Dict[Any, Dict[str, Set[Any]]]]=None, star_cache_item: Optional[Dict[Any, Dict[str, Set[Any]]]]=None, star_cache_alt: Optional[Dict[Any, Dict[str, Set[Any]]]]=None, ml_cache: Optional[MLPredicateCache]=None, prefilter_cache: Optional[Dict[str, Any]]=None, edge_lookups: Optional[Dict[str, List[Dict]]]=None, dis_precomputed: bool=True, offline_bridge_idx: Optional[Any]=None, offline_dis_pass: Optional[Dict[Any, frozenset]]=None, star_contexts: Optional[Dict[str, Any]]=None, rule_runtime: Optional[Dict[str, Any]]=None) -> Optional[Dict[Any, Set[Any]]]:
    candidate_ws_per_anchor, exact_ws_per_anchor = _get_x0_bridge_prefilter_candidates(graph=graph, user_id=user_id, rule=rule, F_phi=F_phi, compact_graph=compact_graph, star_cache_user=star_cache_user, star_cache_item=star_cache_item, star_cache_alt=star_cache_alt, prefilter_cache=prefilter_cache, edge_lookups=edge_lookups, star_contexts=star_contexts, rule_runtime=rule_runtime)
    exact_pairs = {v: set(ws) for v, ws in exact_ws_per_anchor.items() if ws}
    remaining_candidates: Optional[Dict[Any, Set[Any]]] = None
    if candidate_ws_per_anchor is not None:
        remaining_candidates = {}
        for v, ws in candidate_ws_per_anchor.items():
            residual = set(ws) - exact_pairs.get(v, set())
            if residual:
                remaining_candidates[v] = residual
    refined_pairs = _compute_vw_pairs_by_anchor_refinement_reference(graph=graph, user_id=user_id, rooted_dag=rooted_dag, rule=rule, F_phi=F_phi, compact_graph=compact_graph, star_cache_user=star_cache_user, star_cache_item=star_cache_item, star_cache_alt=star_cache_alt, ml_cache=ml_cache, prefilter_cache=prefilter_cache, edge_lookups=edge_lookups, dis_precomputed=dis_precomputed, offline_bridge_idx=offline_bridge_idx, offline_dis_pass=offline_dis_pass, star_contexts=star_contexts, rule_runtime=rule_runtime, candidate_ws_per_anchor=remaining_candidates)
    if not exact_pairs:
        return refined_pairs
    if refined_pairs is None:
        return exact_pairs
    merged = {v: set(ws) for v, ws in exact_pairs.items()}
    for v, ws in refined_pairs.items():
        if v in merged:
            merged[v] |= set(ws)
        else:
            merged[v] = set(ws)
    return merged

def _update_omr_scores(user_id: Any, rule: OMRule, F_phi: Dict[str, Set[Any]], score_accumulators: Dict[Any, Dict[Any, float]], scorer: 'RankingScoreComputer', dis_model: DisModel, aggr_model: AggrModel, dis_score_cache: Dict[Tuple[Any, Any], float], bridge_vw_pairs: Optional[Dict[Any, Set[Any]]]=None, aggr_scores_shared: Optional[Dict[Any, float]]=None, scored_pairs: Optional[Dict[Any, Set[Any]]]=None) -> None:
    w0_var = rule.pattern.alt_center
    y0_var = rule.pattern.anchor_center
    x0_var = rule.pattern.user_center
    w0_candidates = F_phi.get(w0_var, set())
    y0_candidates = F_phi.get(y0_var, set())
    if not w0_candidates or not y0_candidates:
        return
    rule_matched_vars = F_phi
    dis_preds = rule.get_dis_predicates()
    aggr_preds = rule.get_aggr_predicates()
    rule_dis_indexed = hasattr(dis_model, '_cotag_scores') and dis_preds and (len(dis_preds) == 1) and (dis_preds[0].operator in ('>=', '>'))
    fixed_pw_plans = getattr(rule, '_fixed_pairwise_plan_cache', None)
    if fixed_pw_plans is None:
        fixed_pw_plans = []
        for pred in rule.get_pair_wise_predicates():
            if pred.var1 not in {x0_var, y0_var, w0_var} or pred.var2 not in {x0_var, y0_var, w0_var}:
                continue
            fixed_pw_plans.append((pred.var1, pred.attr1, pred.var2, pred.attr2, pred.operator))
        try:
            setattr(rule, '_fixed_pairwise_plan_cache', tuple(fixed_pw_plans))
            fixed_pw_plans = getattr(rule, '_fixed_pairwise_plan_cache')
        except Exception:
            fixed_pw_plans = tuple(fixed_pw_plans)
    graph = scorer.graph
    x0_node = graph.get_node(user_id)
    x0_attrs = x0_node.attributes if x0_node is not None else None
    delta_phi = scorer.get_rule_delta_phi(rule, graph, rule_matched_vars)
    acceptability_weight = 1.0 - scorer.lambda_param
    diversity_increment_scale = scorer.lambda_param * delta_phi / max(scorer.sigma_omr_size, 1)
    v_attr_cache: Dict[Any, Optional[Dict[str, Any]]] = {}
    w_attr_cache: Dict[Any, Optional[Dict[str, Any]]] = {}

    def _filter_fixed_pairwise(v_id: Any, ws: Set[Any]) -> Set[Any]:
        if not fixed_pw_plans:
            return ws
        v_attrs = v_attr_cache.get(v_id)
        if v_attrs is None:
            v_node = graph.get_node(v_id)
            v_attrs = v_node.attributes if v_node is not None else None
            v_attr_cache[v_id] = v_attrs
        if v_attrs is None or x0_attrs is None:
            return set()
        surviving = set()
        for w_id in ws:
            w_attrs = w_attr_cache.get(w_id)
            if w_attrs is None:
                w_node = graph.get_node(w_id)
                w_attrs = w_node.attributes if w_node is not None else None
                w_attr_cache[w_id] = w_attrs
            if w_attrs is None:
                continue
            ok = True
            for lhs_var, lhs_attr, rhs_var, rhs_attr, operator in fixed_pw_plans:
                lhs_present = False
                rhs_present = False
                if lhs_var == x0_var:
                    lhs_present = lhs_attr in x0_attrs
                    value1 = x0_attrs.get(lhs_attr)
                elif lhs_var == y0_var:
                    lhs_present = lhs_attr in v_attrs
                    value1 = v_attrs.get(lhs_attr)
                else:
                    lhs_present = lhs_attr in w_attrs
                    value1 = w_attrs.get(lhs_attr)
                if rhs_var == x0_var:
                    rhs_present = rhs_attr in x0_attrs
                    value2 = x0_attrs.get(rhs_attr)
                elif rhs_var == y0_var:
                    rhs_present = rhs_attr in v_attrs
                    value2 = v_attrs.get(rhs_attr)
                else:
                    rhs_present = rhs_attr in w_attrs
                    value2 = w_attrs.get(rhs_attr)
                if not lhs_present or not rhs_present:
                    ok = False
                    break
                if not _safe_compare_predicate_values(value1, value2, operator):
                    ok = False
                    break
            if ok:
                surviving.add(w_id)
        return surviving
    aggr_scores: Dict[Any, float] = {}
    aggr_pass_ws: Optional[Set[Any]] = None
    if aggr_preds:
        if aggr_scores_shared is not None:
            aggr_scores = aggr_scores_shared
        else:
            aggr_scores = aggr_model.compute_batch(w0_candidates)
        aggr_pass_ws = {w for w in w0_candidates if all((ap.evaluate(aggr_scores.get(w, aggr_model.compute(w))) for ap in aggr_preds))}
        if not aggr_pass_ws:
            return
    for v in y0_candidates:
        v_w_candidates = w0_candidates
        if bridge_vw_pairs is not None and v in bridge_vw_pairs:
            v_w_candidates = bridge_vw_pairs[v] & w0_candidates
        if scored_pairs is not None and v in scored_pairs:
            v_w_candidates = v_w_candidates - scored_pairs[v]
        v_w_candidates = _filter_fixed_pairwise(v, v_w_candidates)
        if rule_dis_indexed:
            dp0 = dis_preds[0]
            v_w_candidates = dis_model.get_items_above_threshold(v, dp0.threshold, v_w_candidates, dp0.operator)
        uncached_ws = {w for w in v_w_candidates if w != v and (v, w) not in dis_score_cache}
        if uncached_ws:
            batch_scores = dis_model.compute_batch(v, uncached_ws)
            for w, s in batch_scores.items():
                score = float(s)
                dis_score_cache[v, w] = score
                scorer._mdis_cache[v, w] = score
    for v in y0_candidates:
        v_w_candidates = w0_candidates
        if bridge_vw_pairs is not None and v in bridge_vw_pairs:
            v_w_candidates = bridge_vw_pairs[v] & w0_candidates
        if scored_pairs is not None and v in scored_pairs:
            v_w_candidates = v_w_candidates - scored_pairs[v]
        v_w_candidates = _filter_fixed_pairwise(v, v_w_candidates)
        if aggr_pass_ws is not None:
            v_w_candidates = v_w_candidates & aggr_pass_ws
        if rule_dis_indexed:
            dp0 = dis_preds[0]
            v_w_candidates = dis_model.get_items_above_threshold(v, dp0.threshold, v_w_candidates, dp0.operator)
        if not v_w_candidates:
            continue
        ws_to_score = []
        for w in v_w_candidates:
            if v == w:
                continue
            vw_key = (v, w)
            dis_score = dis_score_cache.get(vw_key)
            if dis_score is None:
                dis_score = float(dis_model.compute(v, w))
                dis_score_cache[vw_key] = dis_score
            scorer._mdis_cache[vw_key] = float(dis_score)
            if dis_preds and (not rule_dis_indexed) and (not all((dp.evaluate(dis_score) for dp in dis_preds))):
                continue
            ws_to_score.append(w)
        if not ws_to_score:
            continue
        _, gamma_scores = scorer._get_gamma_batch(v, ws_to_score)
        A_v = score_accumulators.setdefault(v, {})
        for idx, w in enumerate(ws_to_score):
            increment = diversity_increment_scale * float(gamma_scores[idx])
            current_score = A_v.get(w)
            if current_score is None:
                score = acceptability_weight * scorer._get_s_acc(user_id, w) + increment
            else:
                score = current_score + increment
            A_v[w] = max(0.0, min(1.0, score))
            if scored_pairs is not None:
                scored_pairs.setdefault(v, set()).add(w)

def _invalidate_star_caches(cache_user: Dict[Any, Any], valid_users: Set[Any], cache_item: Dict[Any, Any], valid_items: Set[Any], cache_alt: Dict[Any, Any], valid_alts: Set[Any]) -> None:
    for cache, valid in [(cache_user, valid_users), (cache_item, valid_items), (cache_alt, valid_alts)]:
        to_remove = [k for k in cache if k not in valid]
        for k in to_remove:
            del cache[k]
