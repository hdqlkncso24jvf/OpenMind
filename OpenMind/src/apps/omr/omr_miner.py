import os
import gc
import sys
import time
import json
import pickle
import logging
import argparse
import math
import multiprocessing
from typing import List, Set, Dict, Tuple, Optional, Any
from dataclasses import dataclass, field
from collections import defaultdict
from datetime import datetime
from itertools import combinations
from tqdm import tqdm
from data_structure import Graph, DualStarPattern, Path, PointWisePredicate, PairWisePredicate, ComparisonOp, EdgePredicate, OMRPattern, RecPredicate, DisPredicate, AggrPredicate, TIERule, RuleMetrics, OMRule, canonical_tie_rule_signature
from tie_matcher import compute_rule_metrics, match_path_compact, _prefilter_by_attribute_index, verify_variable_predicates, _get_prefix_sig_for_var
try:
    from tie_matcher import _PREFILTER_NA
except ImportError:
    _PREFILTER_NA = '__PREFILTER_NOT_APPLICABLE__'
from omr_matcher import _merge_candidate_maps
from models import DisModel, AggrModel, MLPredicateCache
from data_loader import load_dataset, load_tie_rules, load_omr_models
from attribute_config import load_attribute_config
from tie_miner import MetricsCache, ProfilingStats, _are_contradictory
logger = logging.getLogger(__name__)
_MATERIALIZE_MATCHED_TRIPLES_LIMIT = 200000
try:
    from _fast_omr_eval import filter_and_aggregate as _cy_filter_and_aggregate
    from _fast_omr_eval import compute_support_from_training_index as _cy_compute_support
    _HAS_FAST_OMR = True
except ImportError:
    _HAS_FAST_OMR = False

def _get_parallel_context() -> multiprocessing.context.BaseContext:
    methods = set(multiprocessing.get_all_start_methods())
    if 'forkserver' in methods:
        return multiprocessing.get_context('forkserver')
    if 'spawn' in methods:
        return multiprocessing.get_context('spawn')
    return multiprocessing.get_context()

def _pw_are_contradictory(a: 'PointWisePredicate', b: 'PointWisePredicate') -> bool:
    if a.variable != b.variable or a.attribute != b.attribute:
        return False
    oa, ob = (a.operator, b.operator)
    va, vb = (a.constant, b.constant)
    if oa == ComparisonOp.EQ and ob == ComparisonOp.EQ:
        return va != vb
    if {oa, ob} == {ComparisonOp.EQ, ComparisonOp.NE}:
        return va == vb
    if oa == ComparisonOp.EQ and ob == ComparisonOp.LT:
        try:
            return va >= vb
        except TypeError:
            return False
    if oa == ComparisonOp.EQ and ob == ComparisonOp.GT:
        try:
            return va <= vb
        except TypeError:
            return False
    if oa == ComparisonOp.LT and ob == ComparisonOp.EQ:
        try:
            return vb >= va
        except TypeError:
            return False
    if oa == ComparisonOp.GT and ob == ComparisonOp.EQ:
        try:
            return vb <= va
        except TypeError:
            return False
    if oa == ComparisonOp.LT and ob == ComparisonOp.GT:
        try:
            return vb >= va
        except TypeError:
            return False
    if oa == ComparisonOp.GT and ob == ComparisonOp.LT:
        try:
            return va >= vb
        except TypeError:
            return False
    return False

@dataclass
class OMRMetrics:
    support: int = 0
    confidence: float = 0.0
    matched_count: int = 0
    diversity_score: float = 0.0
    acceptability_score: float = 0.0
    alternative_count: int = 0
    anchor_count: int = 0
    matched_triples: Set[Tuple] = field(default_factory=set)
    positive_pivots: Set[Tuple] = field(default_factory=set)

def _min_positive_required(min_support: int, min_confidence: float) -> int:
    return int(math.ceil(min_support * min_confidence - 1e-12))

def _omr_branch_impossible(metrics: OMRMetrics, min_support: int, min_confidence: float) -> bool:
    if metrics.support < min_support:
        return True
    if metrics.confidence >= min_confidence:
        return False
    if metrics.matched_count < min_support:
        return True
    return metrics.support < _min_positive_required(min_support, min_confidence)

def _compute_mean_acceptability(uw_pairs: Set[Tuple[Any, Any]], ml_cache, aggr_model, aggr_scores: Optional[Dict[Any, float]]=None) -> float:
    if not uw_pairs or ml_cache is None:
        return 0.0
    aggr_cache: Dict[Any, float] = {}
    score_sum = 0.0
    for u_id, w_id in uw_pairs:
        rec_score = float(ml_cache.get_prediction(u_id, w_id) or 0.0)
        aggr_score = None
        if aggr_scores is not None:
            aggr_score = aggr_scores.get(w_id)
        if aggr_score is None:
            aggr_score = ml_cache.get_aggr_score(w_id)
        if aggr_score is None:
            if w_id not in aggr_cache:
                aggr_cache[w_id] = float(aggr_model.compute(w_id)) if aggr_model is not None else 1.0
                if ml_cache is not None:
                    ml_cache.set_aggr_score(w_id, aggr_cache[w_id])
            aggr_score = aggr_cache[w_id]
        score_sum += rec_score * float(aggr_score)
    return score_sum / len(uw_pairs)

class BridgeReachabilityIndex:

    def __init__(self):
        self._reachable: Dict[Any, frozenset] = {}
        self._bvc: Dict[Any, Dict[str, Set]] = {}

    @classmethod
    def build(cls, graph: 'Graph', bridge_path: 'Path', origin_ids: Set, w_var: str, pw_predicates: List=None, training_items: Set=None) -> 'BridgeReachabilityIndex':
        import time
        _t0 = time.time()
        idx = cls()
        seq = bridge_path.path_sequence
        edges = bridge_path.edges
        if len(seq) < 2:
            return idx
        hops = []
        for i, edge in enumerate(edges):
            hops.append((edge.label, edge.is_forward, seq[i + 1]))
        pat_nodes = getattr(bridge_path, 'nodes', {}) or {}
        pw_by_var: Dict[str, List[PointWisePredicate]] = {}
        if isinstance(pw_predicates, dict):
            for var, preds in pw_predicates.items():
                pw_by_var[var] = [p for p in preds if isinstance(p, PointWisePredicate)]
        elif pw_predicates:
            for pred in pw_predicates:
                if isinstance(pred, PointWisePredicate):
                    pw_by_var.setdefault(pred.variable, []).append(pred)
        n_built = 0
        for origin_id in origin_ids:
            current_set = {origin_id}
            bvc: Dict[str, Set] = {seq[0]: {origin_id}}
            failed = False
            for hop_idx, (edge_label, is_forward, next_var) in enumerate(hops):
                next_set = set()
                for node_id in current_set:
                    if is_forward:
                        neighbors = graph._out_edges_by_label.get((node_id, edge_label), [])
                        for e in neighbors:
                            next_set.add(e.target)
                    else:
                        neighbors = graph._in_edges_by_label.get((node_id, edge_label), [])
                        for e in neighbors:
                            next_set.add(e.source)
                if not next_set:
                    failed = True
                    break
                if pat_nodes:
                    pn = pat_nodes.get(next_var)
                    if pn and hasattr(pn, 'label') and pn.label:
                        expected_label = pn.label
                        next_set = {n for n in next_set if n in graph._nodes and graph._nodes[n].label == expected_label}
                next_preds = pw_by_var.get(next_var, [])
                if next_preds and next_set:
                    next_set = {n for n in next_set if n in graph._nodes and all((pred.evaluate(graph._nodes[n]) for pred in next_preds))}
                if not next_set:
                    failed = True
                    break
                bvc[next_var] = next_set
                current_set = next_set
            if failed or not current_set:
                continue
            reachable = set(current_set)
            if training_items is not None:
                reachable &= training_items
            reachable.discard(origin_id)
            if seq[-1] in bvc:
                bvc[seq[-1]] = set(reachable)
            if reachable:
                idx._reachable[origin_id] = frozenset(reachable)
                idx._bvc[origin_id] = bvc
            n_built += 1
        _dt = time.time() - _t0
        logger.debug(f'  [BridgeIndex] built for {n_built}/{len(origin_ids)} origins in {_dt:.2f}s, avg_reach={sum((len(s) for s in idx._reachable.values())) / max(1, len(idx._reachable)):.0f}')
        return idx

    def get_reachable(self, origin_id: Any) -> frozenset:
        return self._reachable.get(origin_id, frozenset())

    def get_bvc(self, origin_id: Any) -> Dict:
        return self._bvc.get(origin_id, {})

@dataclass
class OMRTopologyCache:
    valid_users: Set = field(default_factory=set)
    u_bridge_data: Dict = field(default_factory=dict)
    user_av_map: Dict = field(default_factory=dict)
    v_results: Dict = field(default_factory=dict)
    users_per_v: Dict = field(default_factory=dict)
    has_u_dep: bool = False
    alt_result_cache: Dict = field(default_factory=dict)
    aggr_scores: Dict = field(default_factory=dict)
    anchor_pred_infos: List = field(default_factory=list)
    fallback_preds: List = field(default_factory=list)
    user_anchor_preds: List = field(default_factory=list)
    item_anchor_preds: List = field(default_factory=list)
    x0_bp_indices: List = field(default_factory=list)
    y0_narrow_info: List = field(default_factory=list)
    x0_narrow_info: List = field(default_factory=list)
    v_only_fallbacks: List = field(default_factory=list)
    x0_var: str = ''
    y0_var: str = ''
    w_var: str = ''
    matched_pivots: Set = field(default_factory=set)
    training_items: Set = field(default_factory=set)
    training_index: Dict = field(default_factory=dict)
    ml_cache_ref: Any = None
    dis_model_ref: Any = None
    bridge_match_cache: Dict = field(default_factory=dict)
    dis_scores_per_v: Dict = field(default_factory=dict)
    bridge_reachability_indexes: Dict = field(default_factory=dict)

def _check_tie_pw_for_pivot(anchor_index, pivot_id, checks):
    for _tvar, tpreds, tsig, _tstar in checks:
        for tp in tpreds:
            grouped = anchor_index.query_grouped(tsig, tp.attribute)
            if tp.operator == ComparisonOp.EQ:
                if pivot_id not in grouped.get(tp.constant, set()):
                    return False
            else:
                found = False
                for val, pids in grouped.items():
                    if pivot_id not in pids:
                        continue
                    try:
                        op = tp.operator
                        c = tp.constant
                        if op == ComparisonOp.LT and val < c or (op == ComparisonOp.LE and val <= c) or (op == ComparisonOp.GT and val > c) or (op == ComparisonOp.GE and val >= c) or (op == ComparisonOp.NE and val != c):
                            found = True
                            break
                    except TypeError:
                        pass
                if not found:
                    return False
    return True

def _apply_narrow_info(graph, compact_match, narrow_info, w_id):
    for int_var, edge_label, is_fwd in narrow_info:
        if int_var not in compact_match:
            continue
        if is_fwd:
            connected = {e.source for e in graph._in_edges_by_label.get((w_id, edge_label), [])}
        else:
            connected = {e.target for e in graph._out_edges_by_label.get((w_id, edge_label), [])}
        narrowed = compact_match[int_var] & connected
        if not narrowed:
            return False
        compact_match[int_var] = narrowed
    return True

def _apply_anchor_filter(graph, compact_match, anchor_preds, valid_vals_list):
    for ap_idx, (_, info) in enumerate(anchor_preds):
        pred, omr_var_name, omr_attr = (info[0], info[1], info[2])
        valid_vals = valid_vals_list[ap_idx]
        if not valid_vals:
            return False
        omr_cands = compact_match.get(omr_var_name, set())
        if not omr_cands:
            return False
        surviving = set()
        op = pred.operator
        omr_is_var1 = pred.var1 == omr_var_name
        for cand_id in omr_cands:
            node = graph.get_node(cand_id)
            if node is None:
                continue
            val = node.attributes.get(omr_attr)
            if val is None:
                continue
            if op == ComparisonOp.EQ:
                if val in valid_vals:
                    surviving.add(cand_id)
            elif op == ComparisonOp.NE:
                if any((tv != val for tv in valid_vals)):
                    surviving.add(cand_id)
            else:
                for tv in valid_vals:
                    try:
                        if omr_is_var1:
                            sat = op == ComparisonOp.LT and val < tv or (op == ComparisonOp.LE and val <= tv) or (op == ComparisonOp.GT and val > tv) or (op == ComparisonOp.GE and val >= tv)
                        else:
                            sat = op == ComparisonOp.LT and tv < val or (op == ComparisonOp.LE and tv <= val) or (op == ComparisonOp.GT and tv > val) or (op == ComparisonOp.GE and tv >= val)
                        if sat:
                            surviving.add(cand_id)
                            break
                    except TypeError:
                        pass
        if not surviving:
            return False
        compact_match[omr_var_name] = surviving
    return True

def evaluate_omr_from_cache(cache: OMRTopologyCache, training_data: Dict[Tuple, bool], dis_preds: List=None, aggr_preds: List=None, graph: 'Graph'=None) -> OMRMetrics:
    if not cache.v_results:
        return OMRMetrics()
    dis_preds = dis_preds or []
    aggr_preds = aggr_preds or []
    valid_users = cache.valid_users
    users_per_v = cache.users_per_v
    has_u_dep = cache.has_u_dep
    x0_var = cache.x0_var
    y0_var = cache.y0_var
    w_var = cache.w_var
    _dis_model = cache.dis_model_ref
    _lazy_dis = cache.dis_scores_per_v

    def _materialize_dis_scores_for_v(v_id: Any, ws: Set[Any], per_w_dis: Dict[Any, float]) -> Dict[Any, float]:
        if not ws:
            return {}
        if _dis_model is None:
            return dict(per_w_dis)
        v_cache = _lazy_dis.setdefault(v_id, {})
        if per_w_dis:
            v_cache.update(per_w_dis)
        missing = [w_id for w_id in ws if w_id not in v_cache]
        unresolved = []
        if missing and hasattr(_dis_model, 'get_score'):
            for w_id in missing:
                score = _dis_model.get_score(v_id, w_id)
                if score is None:
                    unresolved.append(w_id)
                else:
                    v_cache[w_id] = score
        else:
            unresolved = missing
        if unresolved:
            fresh_scores = _dis_model.compute_batch(v_id, set(unresolved))
            if fresh_scores:
                v_cache.update(fresh_scores)
        return {w_id: v_cache[w_id] for w_id in ws if w_id in v_cache}

    def _aggregate_dis_scores(filtered_v_results: Dict) -> Tuple[float, int]:
        total_sum = 0.0
        total_count = 0
        for v_id, (surv_ws, _v_bvc, pw_dis) in filtered_v_results.items():
            n_users = len(users_per_v.get(v_id, set()) & valid_users)
            if not n_users:
                continue
            v_sum = 0.0
            v_count = 0
            for w_id in surv_ws:
                dv = pw_dis.get(w_id)
                if dv is not None:
                    v_sum += dv
                    v_count += 1
            total_sum += v_sum * n_users
            total_count += v_count * n_users
        return (total_sum, total_count)
    _t_filter = time.time()
    if aggr_preds:
        aggr_pass_set = frozenset((w for w, score in cache.aggr_scores.items() if all((ap.evaluate(score) for ap in aggr_preds))))
    else:
        aggr_pass_set = None
    _t_aggr_pre = time.time()
    logger.debug(f'  [eval_cache] aggr_pass_set: {(len(aggr_pass_set) if aggr_pass_set is not None else 'ALL')} in {_t_aggr_pre - _t_filter:.3f}s')
    _use_dis_index = False
    _dis_threshold = 0.0
    _dis_operator = '>='
    if dis_preds and _dis_model is not None and hasattr(_dis_model, '_cotag_scores'):
        if len(dis_preds) == 1:
            dp0 = dis_preds[0]
            if dp0.operator in ('>=', '>'):
                _use_dis_index = True
                _dis_threshold = dp0.threshold
                _dis_operator = dp0.operator
    _use_combined = not has_u_dep and (not dis_preds or _use_dis_index) and _HAS_FAST_OMR
    if _use_combined:
        _t_combined = time.time()
        filtered_v_results, all_w_found, all_v_with_w, dis_score_sum, dis_count, user_to_vs = _cy_filter_and_aggregate(cache.v_results, aggr_pass_set, _dis_model, _dis_threshold, _dis_operator, _use_dis_index, users_per_v, valid_users)
        _t_combined_filt = time.time()
        if not filtered_v_results:
            return OMRMetrics()
        if dis_preds and _use_dis_index and (_dis_model is not None):
            rematerialized_v_results: Dict = {}
            for v_id, (surv_ws, v_bvc, per_w_dis) in filtered_v_results.items():
                effective_dis = _materialize_dis_scores_for_v(v_id, surv_ws, per_w_dis)
                rematerialized_v_results[v_id] = (surv_ws, v_bvc, effective_dis)
            filtered_v_results = rematerialized_v_results
            dis_score_sum, dis_count = _aggregate_dis_scores(filtered_v_results)
        _ti = cache.training_index
        positive_uw_pairs, matched_uw_in_T = _cy_compute_support(user_to_vs, filtered_v_results, _ti, training_data)
        _t_combined_sup = time.time()
        positive_per_u: Dict = {}
        for u_id, w_id in positive_uw_pairs:
            positive_per_u.setdefault(u_id, set()).add(w_id)
        positive_pivots_set: Set = set()
        if positive_per_u:
            for v_id, (surv_ws, _, _) in filtered_v_results.items():
                v_u = users_per_v[v_id] & valid_users
                for u_id in v_u:
                    u_pos = positive_per_u.get(u_id)
                    if u_pos and (not u_pos.isdisjoint(surv_ws)):
                        positive_pivots_set.add((u_id, v_id))
        _t_combined_end = time.time()
        matched_triples: Set = set()
    else:
        filtered_v_results: Dict = {}
        _lazy_computed = 0
        _n_v = len(cache.v_results)
        for v_id, (surv_ws, v_bvc, per_w_dis) in tqdm(cache.v_results.items(), total=_n_v, desc='  [eval_cache] filter v', disable=_n_v < 100, leave=False):
            if aggr_pass_set is not None:
                ws = surv_ws & aggr_pass_set
            else:
                ws = surv_ws
            if not ws:
                continue
            effective_dis: Dict = {}
            if dis_preds:
                if _use_dis_index:
                    ws = _dis_model.get_items_above_threshold(v_id, _dis_threshold, ws, _dis_operator)
                    effective_dis = _materialize_dis_scores_for_v(v_id, ws, per_w_dis)
                else:
                    v_dis = _lazy_dis.get(v_id)
                    if v_dis is None:
                        if per_w_dis:
                            v_dis = dict(per_w_dis)
                            _lazy_dis[v_id] = v_dis
                    if v_dis is None and _dis_model is not None:
                        v_dis = _dis_model.compute_batch(v_id, ws)
                        _lazy_dis[v_id] = v_dis
                        _lazy_computed += len(ws)
                    elif v_dis is not None and _dis_model is not None:
                        uncached = ws - v_dis.keys()
                        if uncached:
                            new_scores = _dis_model.compute_batch(v_id, uncached)
                            v_dis.update(new_scores)
                            _lazy_computed += len(uncached)
                    if v_dis:
                        ws_filtered = set()
                        for w in ws:
                            score = v_dis.get(w)
                            if score is not None and all((dp.evaluate(score) for dp in dis_preds)):
                                ws_filtered.add(w)
                        ws = ws_filtered
                        effective_dis = v_dis
                    else:
                        ws = set()
            else:
                effective_dis = per_w_dis
            if ws:
                filtered_v_results[v_id] = (ws, v_bvc, effective_dis)
        _t_filter_end = time.time()
        logger.debug(f'  [eval_cache filter] {_t_filter_end - _t_filter:.3f}s (aggr_pre={_t_aggr_pre - _t_filter:.3f}s filter={_t_filter_end - _t_aggr_pre:.3f}s) v_in={_n_v} v_out={len(filtered_v_results)} dis_index={('yes' if _use_dis_index else 'no')} lazy_computed={_lazy_computed}')
        if not filtered_v_results:
            return OMRMetrics()
        _t_p3 = time.time()
        all_w_found: Set = set()
        all_v_with_w: Set = set()
        dis_score_sum = 0.0
        dis_count = 0
        if not has_u_dep:
            for v_id, (surv_ws, _v_bvc, pw_dis) in filtered_v_results.items():
                v_users = users_per_v[v_id] & valid_users
                n_users = len(v_users)
                if not n_users:
                    continue
                all_v_with_w.add(v_id)
                all_w_found.update(surv_ws)
                _v_dis_sum = 0.0
                _v_dis_count = 0
                for w_id in surv_ws:
                    dv = pw_dis.get(w_id)
                    if dv is not None:
                        _v_dis_sum += dv
                        _v_dis_count += 1
                dis_score_sum += _v_dis_sum * n_users
                dis_count += _v_dis_count * n_users
            _t_pA = time.time()
            user_to_vs: Dict = {}
            for v_id in filtered_v_results:
                for u_id in users_per_v.get(v_id, set()) & valid_users:
                    if u_id not in user_to_vs:
                        user_to_vs[u_id] = [v_id]
                    else:
                        user_to_vs[u_id].append(v_id)
            _t_pB = time.time()
            _ti = cache.training_index
            positive_uw_pairs: Set = set()
            matched_uw_in_T: Set = set()
            if _HAS_FAST_OMR:
                positive_uw_pairs, matched_uw_in_T = _cy_compute_support(user_to_vs, filtered_v_results, _ti, training_data)
            elif _ti:
                for u_id, u_vs in user_to_vs.items():
                    u_train = _ti.get(u_id)
                    if not u_train:
                        continue
                    for w_id in u_train:
                        for v_id in u_vs:
                            v_data = filtered_v_results.get(v_id)
                            if v_data is not None and w_id in v_data[0]:
                                matched_uw_in_T.add((u_id, w_id))
                                if training_data[u_id, w_id]:
                                    positive_uw_pairs.add((u_id, w_id))
                                break
            else:
                _user_set = frozenset(user_to_vs)
                for u_id, w_id in training_data:
                    if u_id not in _user_set:
                        continue
                    for v_id in user_to_vs[u_id]:
                        v_data = filtered_v_results.get(v_id)
                        if v_data is not None and w_id in v_data[0]:
                            matched_uw_in_T.add((u_id, w_id))
                            if training_data[u_id, w_id]:
                                positive_uw_pairs.add((u_id, w_id))
                            break
            _t_pC = time.time()
            positive_per_u: Dict = {}
            for u_id, w_id in positive_uw_pairs:
                positive_per_u.setdefault(u_id, set()).add(w_id)
            positive_pivots_set: Set = set()
            if positive_per_u:
                for v_id, (surv_ws, _, _) in filtered_v_results.items():
                    v_u = users_per_v[v_id] & valid_users
                    for u_id in v_u:
                        u_pos = positive_per_u.get(u_id)
                        if u_pos and (not u_pos.isdisjoint(surv_ws)):
                            positive_pivots_set.add((u_id, v_id))
            _t_pD = time.time()
            matched_triples: Set = set()
        else:
            u_bridge_data = cache.u_bridge_data
            x0_bp_indices = cache.x0_bp_indices
            anchor_pred_infos = cache.anchor_pred_infos
            fallback_preds = cache.fallback_preds
            user_anchor_preds = cache.user_anchor_preds
            item_anchor_preds = cache.item_anchor_preds
            user_av_map = cache.user_av_map
            y0_narrow_info = cache.y0_narrow_info
            x0_narrow_info = cache.x0_narrow_info
            v_only_fallbacks = cache.v_only_fallbacks
            alt_result_cache = cache.alt_result_cache
            anchor_index = getattr(graph, 'path_anchor_index', None) if graph else None
            need_cm = bool(anchor_pred_infos or fallback_preds)
            all_uw_pairs: Set = set()
            positive_uw_pairs: Set = set()
            matched_uw_in_T: Set = set()
            positive_pivots_set: Set = set()
            matched_triples: Set = set()
            _n_fv = len(filtered_v_results)
            if not need_cm:
                _t_med = time.time()
                user_to_vs: Dict = {}
                for v_id, (surv_ws, v_bvc, pw_dis) in tqdm(filtered_v_results.items(), total=_n_fv, desc='  [eval_cache] P3 medium A', disable=_n_fv < 100, leave=False):
                    v_users = users_per_v[v_id] & valid_users
                    if not v_users:
                        continue
                    all_v_with_w.add(v_id)
                    n_users = len(v_users)
                    _v_dis_sum = 0.0
                    _v_dis_count = 0
                    for w_id in surv_ws:
                        all_w_found.add(w_id)
                        dv = pw_dis.get(w_id)
                        if dv is not None:
                            _v_dis_sum += dv
                            _v_dis_count += 1
                    dis_score_sum += _v_dis_sum * n_users
                    dis_count += _v_dis_count * n_users
                    for u_id in v_users:
                        effective = surv_ws
                        if x0_bp_indices:
                            u_data = u_bridge_data.get(u_id)
                            if u_data is None:
                                continue
                            effective = surv_ws & u_data[0]
                        if not effective:
                            continue
                        user_to_vs.setdefault(u_id, []).append(v_id)
                _t_medA = time.time()
                _ti = cache.training_index
                if _ti:
                    for u_id, u_vs in tqdm(user_to_vs.items(), total=len(user_to_vs), desc='  [eval_cache] P3 medium B', disable=len(user_to_vs) < 100, leave=False):
                        u_train = _ti.get(u_id)
                        if not u_train:
                            continue
                        for w_id in u_train:
                            u_data = u_bridge_data.get(u_id) if x0_bp_indices else None
                            for v_id in u_vs:
                                v_data = filtered_v_results.get(v_id)
                                if v_data is None:
                                    continue
                                v_ws = v_data[0]
                                if w_id not in v_ws:
                                    continue
                                if u_data is not None and w_id not in u_data[0]:
                                    continue
                                matched_uw_in_T.add((u_id, w_id))
                                if training_data[u_id, w_id]:
                                    positive_uw_pairs.add((u_id, w_id))
                                    positive_pivots_set.add((u_id, v_id))
                                break
                else:
                    _user_set = frozenset(user_to_vs)
                    for u_id, w_id in training_data:
                        if u_id not in _user_set:
                            continue
                        u_data = u_bridge_data.get(u_id) if x0_bp_indices else None
                        for v_id in user_to_vs[u_id]:
                            v_data = filtered_v_results.get(v_id)
                            if v_data is None:
                                continue
                            if w_id not in v_data[0]:
                                continue
                            if u_data is not None and w_id not in u_data[0]:
                                continue
                            matched_uw_in_T.add((u_id, w_id))
                            if training_data[u_id, w_id]:
                                positive_uw_pairs.add((u_id, w_id))
                                positive_pivots_set.add((u_id, v_id))
                            break
                _t_medB = time.time()
            else:
                for v_id, (surv_ws, v_bvc, pw_dis) in tqdm(filtered_v_results.items(), total=_n_fv, desc='  [eval_cache] P3 slow', disable=_n_fv < 100, leave=False):
                    v_users = users_per_v[v_id] & valid_users
                    if not v_users:
                        continue
                    v_valid_ws: Optional[Set] = None
                    if item_anchor_preds or v_only_fallbacks or y0_narrow_info:
                        v_valid_ws = set()
                        for w_id in surv_ws:
                            cm = {}
                            if not _merge_candidate_maps(cm, v_bvc):
                                continue
                            _, alt_vc = alt_result_cache.get(w_id, (True, {}))
                            if not _merge_candidate_maps(cm, alt_vc):
                                continue
                            cm[y0_var] = {v_id}
                            cm[w_var] = {w_id}
                            if y0_narrow_info:
                                if not _apply_narrow_info(graph, cm, y0_narrow_info, w_id):
                                    continue
                            if item_anchor_preds and anchor_index:
                                item_avs_local = []
                                for _, info in item_anchor_preds:
                                    grouped = anchor_index.query_grouped(info[3], info[4])
                                    item_avs_local.append(frozenset((val for val, pids in grouped.items() if v_id in pids)))
                                if not _apply_anchor_filter(graph, cm, item_anchor_preds, item_avs_local):
                                    continue
                            if v_only_fallbacks:
                                if not verify_variable_predicates(graph, cm, v_only_fallbacks):
                                    continue
                            v_valid_ws.add(w_id)
                        if not v_valid_ws:
                            continue
                    effective_surv = v_valid_ws if v_valid_ws is not None else surv_ws
                    v_had_w = False
                    for u_id in v_users:
                        ws = effective_surv
                        u_bvc_local: Dict = {}
                        if x0_bp_indices:
                            u_data = u_bridge_data.get(u_id)
                            if u_data is None:
                                continue
                            u_ws_set, u_bvc_local = u_data
                            ws = ws & u_ws_set
                            if not ws:
                                continue
                        if user_anchor_preds or x0_narrow_info:
                            for w_id in ws:
                                cm = {x0_var: {u_id}, y0_var: {v_id}, w_var: {w_id}}
                                if x0_narrow_info:
                                    if not _merge_candidate_maps(cm, u_bvc_local):
                                        continue
                                    if not _apply_narrow_info(graph, cm, x0_narrow_info, w_id):
                                        continue
                                if user_anchor_preds:
                                    uav = user_av_map.get(u_id, [])
                                    if not _apply_anchor_filter(graph, cm, user_anchor_preds, uav):
                                        continue
                                all_w_found.add(w_id)
                                v_had_w = True
                                uw = (u_id, w_id)
                                if uw not in all_uw_pairs:
                                    all_uw_pairs.add(uw)
                                    if uw in training_data:
                                        matched_uw_in_T.add(uw)
                                        if training_data[uw]:
                                            positive_uw_pairs.add(uw)
                                            positive_pivots_set.add((u_id, v_id))
                                dv = pw_dis.get(w_id)
                                if dv is not None:
                                    dis_score_sum += dv
                                    dis_count += 1
                        else:
                            for w_id in ws:
                                all_w_found.add(w_id)
                                v_had_w = True
                                uw = (u_id, w_id)
                                if uw not in all_uw_pairs:
                                    all_uw_pairs.add(uw)
                                    if uw in training_data:
                                        matched_uw_in_T.add(uw)
                                        if training_data[uw]:
                                            positive_uw_pairs.add(uw)
                                            positive_pivots_set.add((u_id, v_id))
                                dv = pw_dis.get(w_id)
                                if dv is not None:
                                    dis_score_sum += dv
                                    dis_count += 1
                    if v_had_w:
                        all_v_with_w.add(v_id)
    support = len(positive_uw_pairs)
    matched_count = len(matched_uw_in_T)
    confidence = support / matched_count if matched_count > 0 else 0.0
    diversity_score = dis_score_sum / dis_count if dis_count > 0 else 0.0
    acceptability_score = _compute_mean_acceptability(matched_uw_in_T, cache.ml_cache_ref, None, aggr_scores=cache.aggr_scores)
    return OMRMetrics(support=support, confidence=confidence, matched_count=matched_count, diversity_score=diversity_score, acceptability_score=acceptability_score, alternative_count=len(all_w_found), anchor_count=len(all_v_with_w), matched_triples=matched_triples, positive_pivots=positive_pivots_set)

def filter_topology_cache(cache: OMRTopologyCache, new_preds: List, graph: 'Graph') -> Optional[OMRTopologyCache]:
    if not new_preds:
        return cache
    new_pw = [p for p in new_preds if isinstance(p, PointWisePredicate)]
    new_pair = [p for p in new_preds if isinstance(p, PairWisePredicate)]
    if not new_pw and (not new_pair):
        return cache
    w_var = cache.w_var
    y0_var = cache.y0_var
    x0_var = cache.x0_var
    available_vars = {w_var, y0_var, x0_var}
    for _vid, (_ws, v_bvc, _dis) in cache.v_results.items():
        available_vars.update(v_bvc.keys())
        break
    for _wid, (_ok, alt_vc) in cache.alt_result_cache.items():
        available_vars.update(alt_vc.keys())
        break
    for p in new_pw:
        if p.variable not in available_vars:
            return None
    for p in new_pair:
        if p.var1 not in available_vars or p.var2 not in available_vars:
            return None
    pw_on_w: List = [p for p in new_pw if p.variable == w_var]
    pw_on_y0: List = [p for p in new_pw if p.variable == y0_var]
    pw_on_x0: List = [p for p in new_pw if p.variable == x0_var]
    pw_on_intermediate: Dict[str, List] = {}
    for p in new_pw:
        if p.variable not in (w_var, y0_var, x0_var):
            pw_on_intermediate.setdefault(p.variable, []).append(p)
    new_valid_users = cache.valid_users
    if pw_on_x0:
        new_valid_users = {u for u in new_valid_users if all((p.evaluate(graph.get_node(u)) for p in pw_on_x0))}
        if not new_valid_users:
            return OMRTopologyCache()
    y0_pass: Optional[Set] = None
    if pw_on_y0:
        y0_pass = set()
        for v_id in cache.v_results:
            node = graph.get_node(v_id)
            if node is not None and all((p.evaluate(node) for p in pw_on_y0)):
                y0_pass.add(v_id)
    _alt_fail_ws: Optional[Set] = None
    new_alt_result_cache = cache.alt_result_cache
    if pw_on_intermediate:
        _alt_fail_ws = set()
        narrowed_alt_cache: Dict[Any, Tuple[bool, Dict[str, Set[Any]]]] = {}
        for w_id, (alt_ok, alt_vc) in cache.alt_result_cache.items():
            narrowed_ok = bool(alt_ok)
            narrowed_vc = {var: set(cands) for var, cands in alt_vc.items()}
            if alt_ok:
                for var, preds in pw_on_intermediate.items():
                    if var not in narrowed_vc:
                        continue
                    filtered = {n for n in narrowed_vc[var] if all((p.evaluate(graph.get_node(n)) for p in preds))}
                    narrowed_vc[var] = filtered
                    if not filtered:
                        narrowed_ok = False
                        _alt_fail_ws.add(w_id)
                        break
            narrowed_alt_cache[w_id] = (narrowed_ok, narrowed_vc)
        new_alt_result_cache = narrowed_alt_cache
    new_v_results: Dict = {}
    for v_id, (surv_ws, v_bvc, per_w_dis) in cache.v_results.items():
        if y0_pass is not None and v_id not in y0_pass:
            continue
        if not cache.users_per_v[v_id] & new_valid_users:
            continue
        new_v_bvc = {k: set(s) for k, s in v_bvc.items()}
        bvc_ok = True
        for var, preds in pw_on_intermediate.items():
            if var in new_v_bvc:
                filtered = {n for n in new_v_bvc[var] if all((p.evaluate(graph.get_node(n)) for p in preds))}
                if not filtered:
                    bvc_ok = False
                    break
                new_v_bvc[var] = filtered
        if not bvc_ok:
            continue
        ws = set(surv_ws)
        if pw_on_w:
            ws = {w for w in ws if all((p.evaluate(graph.get_node(w)) for p in pw_on_w))}
        if _alt_fail_ws:
            ws -= _alt_fail_ws
        if new_pair and ws:
            ws_remove = set()
            for w_id in ws:
                cm: Dict[str, Set] = {}
                if not _merge_candidate_maps(cm, new_v_bvc):
                    ws_remove.add(w_id)
                    continue
                if w_id in new_alt_result_cache:
                    _, alt_vc = new_alt_result_cache[w_id]
                    if not _merge_candidate_maps(cm, alt_vc):
                        ws_remove.add(w_id)
                        continue
                cm[y0_var] = {v_id}
                cm[w_var] = {w_id}
                if not verify_variable_predicates(graph, cm, new_pair):
                    ws_remove.add(w_id)
            ws -= ws_remove
        if ws:
            filtered_dis = {w: per_w_dis[w] for w in ws if w in per_w_dis}
            new_v_results[v_id] = (ws, new_v_bvc, filtered_dis)
    if not new_v_results:
        return OMRTopologyCache()
    return OMRTopologyCache(valid_users=new_valid_users, u_bridge_data=cache.u_bridge_data, user_av_map=cache.user_av_map, v_results=new_v_results, users_per_v=cache.users_per_v, has_u_dep=cache.has_u_dep, alt_result_cache=new_alt_result_cache, aggr_scores=cache.aggr_scores, anchor_pred_infos=cache.anchor_pred_infos, fallback_preds=cache.fallback_preds, user_anchor_preds=cache.user_anchor_preds, item_anchor_preds=cache.item_anchor_preds, x0_bp_indices=cache.x0_bp_indices, y0_narrow_info=cache.y0_narrow_info, x0_narrow_info=cache.x0_narrow_info, v_only_fallbacks=cache.v_only_fallbacks, x0_var=cache.x0_var, y0_var=cache.y0_var, w_var=cache.w_var, matched_pivots=cache.matched_pivots, training_items=cache.training_items, training_index=cache.training_index, ml_cache_ref=cache.ml_cache_ref, dis_model_ref=cache.dis_model_ref, bridge_match_cache=cache.bridge_match_cache, dis_scores_per_v=cache.dis_scores_per_v, bridge_reachability_indexes=cache.bridge_reachability_indexes)

def compute_omr_metrics(graph: Graph, omr_rule: OMRule, training_data: Dict[Tuple, bool], ml_cache: MLPredicateCache, dis_model: DisModel, aggr_model: AggrModel, parent_tie_metrics: RuleMetrics=None, training_index: Dict=None, _no_dis_aggr_filter: bool=False, _bridge_match_cache_in: Dict=None, _dis_scores_per_v_in: Dict=None, _bridge_reachability_indexes_in: Dict=None):
    omr_pattern = omr_rule.pattern
    tie_pattern = omr_rule.parent_tie.pattern
    if parent_tie_metrics is None or not parent_tie_metrics.positive_pivots:
        parent_tie_metrics = compute_rule_metrics(graph, omr_rule.parent_tie, training_data, ml_cache, training_index=training_index)
    if parent_tie_metrics.support == 0:
        return OMRMetrics()
    matched_pivots = parent_tie_metrics.matched_pivots
    if not matched_pivots:
        return OMRMetrics()
    _training_items = {w for _, w in training_data}
    dis_preds = omr_rule.get_dis_predicates()
    aggr_preds = omr_rule.get_aggr_predicates()
    omr_pw = omr_rule.get_omr_only_point_wise()
    omr_pair = omr_rule.get_omr_only_pair_wise()
    all_pw = omr_rule.get_point_wise_predicates()
    w_var = omr_pattern.alt_center
    w_label = omr_pattern.alt_center_label
    w_preds = omr_pw.get(w_var, [])
    if w_preds:
        prefiltered = _prefilter_by_attribute_index(graph, w_label, w_preds)
        if prefiltered is not None:
            w_candidates_base = prefiltered
        else:
            all_w = graph.get_node_ids_by_label(w_label)
            w_candidates_base = {nid for nid in all_w if all((p.evaluate(graph.get_node(nid)) for p in w_preds))}
    else:
        w_candidates_base = graph.get_node_ids_by_label(w_label)
    _aggr_scores_cache: Dict[Any, float] = {}
    if _no_dis_aggr_filter:
        if aggr_model is not None:
            for w_id in w_candidates_base:
                _aggr_scores_cache[w_id] = aggr_model.compute(w_id)
    elif aggr_preds and w_candidates_base:
        aggr_filtered = set()
        for w_id in w_candidates_base:
            score = aggr_model.compute(w_id)
            if all((ap.evaluate(score) for ap in aggr_preds)):
                aggr_filtered.add(w_id)
        w_candidates_base = aggr_filtered
    if not w_candidates_base:
        return OMRMetrics()
    bridge_paths = omr_pattern.bridge_paths
    bridge_origins = omr_pattern.bridge_origins
    alt_paths = omr_pattern.alt_star.paths
    _has_alt_paths = bool(alt_paths)
    anchor_index = getattr(graph, 'path_anchor_index', None)
    if anchor_index is not None and bridge_paths:
        from data_structure import compute_path_anchor_signature
        from omr_matcher import _apply_path_anchor_for_path
        origin_vars = set(bridge_origins)
        origin_cands = {}
        for ov in origin_vars:
            if ov == omr_pattern.anchor_center:
                origin_cands[ov] = {v for u, v in matched_pivots}
            elif ov == omr_pattern.user_center:
                origin_cands[ov] = {u for u, v in matched_pivots}
        if origin_cands:
            cm_tmp = dict(origin_cands)
            for bp_idx, bridge_path in enumerate(bridge_paths):
                origin = bridge_origins[bp_idx]
                _apply_path_anchor_for_path(graph, anchor_index, bridge_path, origin, all_pw, cm_tmp, compute_path_anchor_signature)
            anchor_cands = cm_tmp.get(omr_pattern.anchor_center)
            user_cands = cm_tmp.get(omr_pattern.user_center)
            if anchor_cands is not None or user_cands is not None:
                filtered_pivots = set()
                for u, v in matched_pivots:
                    keep = True
                    if anchor_cands is not None and v not in anchor_cands:
                        keep = False
                    if user_cands is not None and u not in user_cands:
                        keep = False
                    if keep:
                        filtered_pivots.add((u, v))
                if len(filtered_pivots) < len(matched_pivots):
                    logger.debug(f'  [PathAnchor] Bridge pre-filter: {len(matched_pivots)} → {len(filtered_pivots)} pivots')
                    matched_pivots = filtered_pivots
    if anchor_index is not None and alt_paths and w_candidates_base:
        from data_structure import compute_path_anchor_signature
        from omr_matcher import _apply_path_anchor_for_path
        cm_w = {w_var: set(w_candidates_base)}
        w_before = len(w_candidates_base)
        for alt_path in alt_paths:
            ap_seq = alt_path.path_sequence
            if ap_seq and ap_seq[0] == w_var:
                _apply_path_anchor_for_path(graph, anchor_index, alt_path, w_var, all_pw, cm_w, compute_path_anchor_signature)
        w_after_anchor = cm_w.get(w_var, set())
        if len(w_after_anchor) < w_before:
            logger.debug(f'  [PathAnchor] Alt-star pre-filter: {w_before} → {len(w_after_anchor)} w candidates')
            w_candidates_base = w_after_anchor
        if not w_candidates_base:
            return OMRMetrics()
    matched_triples = set()
    all_w_found = set()
    all_v_with_w = set()
    dis_score_sum = 0.0
    dis_count = 0
    bridge_edge_lookups = []
    for bridge_path in bridge_paths:
        edge_lookup = {}
        for edge in bridge_path.edges:
            edge_lookup[edge.source_var, edge.target_var] = (edge.label, edge.is_forward)
        bridge_edge_lookups.append(edge_lookup)
    alt_edge_lookups = []
    for alt_path in alt_paths:
        edge_lookup = {}
        for edge in alt_path.edges:
            edge_lookup[edge.source_var, edge.target_var] = (edge.label, edge.is_forward)
        alt_edge_lookups.append(edge_lookup)
    y0_narrow_info: List[Tuple[str, str, bool]] = []
    x0_narrow_info: List[Tuple[str, str, bool]] = []
    for bp_idx, bridge_path in enumerate(bridge_paths):
        bp_seq = bridge_path.path_sequence
        if len(bp_seq) <= 2:
            continue
        last_edge = bridge_path.edges[-1]
        int_var = bp_seq[-2]
        entry = (int_var, last_edge.label, last_edge.is_forward)
        if bridge_origins[bp_idx] == omr_pattern.anchor_center:
            y0_narrow_info.append(entry)
        else:
            x0_narrow_info.append(entry)
    x0_var = omr_pattern.user_center
    y0_var = omr_pattern.anchor_center
    _pivot_vars_pf = {x0_var, y0_var, w_var}
    _prefilter_cache: Dict[str, Optional[Set[Any]]] = {}
    for var, preds in all_pw.items():
        if preds and var not in _pivot_vars_pf:
            var_label = omr_pattern.nodes.get(var)
            if var_label is not None:
                var_label = var_label.label
                prefiltered = _prefilter_by_attribute_index(graph, var_label, preds)
                _prefilter_cache[var] = prefiltered if prefiltered is not None else _PREFILTER_NA
            else:
                _prefilter_cache[var] = _PREFILTER_NA
    available_vars = {x0_var, y0_var, w_var}
    for bp in bridge_paths:
        available_vars.update(bp.path_sequence)
    for ap in alt_paths:
        available_vars.update(ap.path_sequence)
    anchor_pred_infos: List[Tuple] = []
    fallback_preds: List[PairWisePredicate] = []
    if omr_pair:
        for pred in omr_pair:
            v1_avail = pred.var1 in available_vars
            v2_avail = pred.var2 in available_vars
            if v1_avail and v2_avail:
                fallback_preds.append(pred)
                continue
            if not v1_avail and v2_avail:
                tie_var, tie_attr = (pred.var1, pred.attr1)
                omr_var_name, omr_attr = (pred.var2, pred.attr2)
            elif v1_avail and (not v2_avail):
                tie_var, tie_attr = (pred.var2, pred.attr2)
                omr_var_name, omr_attr = (pred.var1, pred.attr1)
            else:
                raise RuntimeError(f'[OMR INVARIANT VIOLATION] PairWise predicate {pred} references two unavailable variables ({pred.var1}, {pred.var2}). _build_omr_predicate_set must not generate such predicates.')
            if anchor_index is None:
                raise RuntimeError(f"[OMR INVARIANT VIOLATION] PairWise predicate {pred} involves TIE intermediate '{tie_var}' but PathAnchorIndex is None. _build_omr_predicate_set must gate on index availability.")
            sig, star = _get_prefix_sig_for_var(tie_pattern, tie_var)
            if sig is None or not anchor_index.has_entry(sig):
                raise RuntimeError(f"[OMR INVARIANT VIOLATION] PairWise predicate {pred} involves TIE intermediate '{tie_var}' but PathAnchor has no entry for sig={sig}. _build_omr_predicate_set must gate on index coverage.")
            anchor_pred_infos.append((pred, omr_var_name, omr_attr, sig, tie_attr, star))
    path_covered_vars = set()
    for bp in bridge_paths:
        path_covered_vars.update(bp.path_sequence)
    for ap in alt_paths:
        path_covered_vars.update(ap.path_sequence)
    path_covered_vars.add(w_var)
    pivot_pw_checks: List[Tuple[str, List]] = []
    for pvar in (x0_var, y0_var):
        if pvar not in path_covered_vars and pvar in omr_pw:
            preds = omr_pw[pvar]
            if preds:
                pivot_pw_checks.append((pvar, preds))
    tie_intermediate_pw_checks: List[Tuple[str, List, str, str]] = []
    tie_intermediate_vars = set(tie_pattern.nodes.keys()) - {tie_pattern.user_center, tie_pattern.item_center}
    for tvar in sorted(tie_intermediate_vars):
        if tvar in path_covered_vars:
            continue
        if tvar not in omr_pw:
            continue
        preds = omr_pw[tvar]
        if not preds:
            continue
        if anchor_index is None:
            continue
        sig_val, star_val = _get_prefix_sig_for_var(tie_pattern, tvar)
        if sig_val is None:
            continue
        tie_intermediate_pw_checks.append((tvar, preds, sig_val, star_val))
    _bridge_match_cache: Dict[Tuple[int, Any], Optional[Dict[str, Set[Any]]]] = dict(_bridge_match_cache_in) if _bridge_match_cache_in is not None else {}
    _bridge_reachability_indexes: Dict[int, BridgeReachabilityIndex] = dict(_bridge_reachability_indexes_in) if _bridge_reachability_indexes_in is not None else {}
    _alt_result_cache: Dict[Any, Tuple[bool, Dict[str, Set[Any]]]] = {}
    logger.debug(f'  [OMR-metrics] pivots={len(matched_pivots)}, unique_v={len({v for _, v in matched_pivots})}, unique_u={len({u for u, _ in matched_pivots})}, w_base={len(w_candidates_base)}, has_dis={bool(dis_preds)}, has_aggr={bool(aggr_preds)}, pw_checks={len(pivot_pw_checks)}, tie_pw={len(tie_intermediate_pw_checks)}, anchor_pairs={len(anchor_pred_infos)}, fallback={len(fallback_preds)}')
    x0_pw_checks = [(pv, pp) for pv, pp in pivot_pw_checks if pv == x0_var]
    y0_pw_checks = [(pv, pp) for pv, pp in pivot_pw_checks if pv == y0_var]
    user_tie_pw = [t for t in tie_intermediate_pw_checks if t[3] == 'user']
    item_tie_pw = [t for t in tie_intermediate_pw_checks if t[3] == 'item']
    y0_bp_indices = [i for i, o in enumerate(bridge_origins) if o == omr_pattern.anchor_center]
    x0_bp_indices = [i for i, o in enumerate(bridge_origins) if o == omr_pattern.user_center]
    _unique_vs = {v for _, v in matched_pivots}
    if y0_bp_indices and _unique_vs and (len(_unique_vs) >= 50):
        for bp_idx in y0_bp_indices:
            if bp_idx in _bridge_reachability_indexes:
                continue
            bp = bridge_paths[bp_idx]
            if len(bp.path_sequence) < 2:
                continue
            _bridge_reachability_indexes[bp_idx] = BridgeReachabilityIndex.build(graph, bp, _unique_vs, w_var, pw_predicates=all_pw, training_items=_training_items)
    user_anchor_preds = [(i, info) for i, info in enumerate(anchor_pred_infos) if info[5] == 'user']
    item_anchor_preds = [(i, info) for i, info in enumerate(anchor_pred_infos) if info[5] == 'item']
    u_vars = {x0_var}
    for idx in x0_bp_indices:
        u_vars.update(bridge_paths[idx].path_sequence)
    u_dep_fallbacks = [p for p in fallback_preds if p.var1 in u_vars or p.var2 in u_vars]
    v_only_fallbacks = [p for p in fallback_preds if p.var1 not in u_vars and p.var2 not in u_vars]
    has_u_dep = bool(x0_bp_indices or user_anchor_preds or u_dep_fallbacks)
    users_per_v: Dict[Any, Set[Any]] = {}
    for u, v in matched_pivots:
        users_per_v.setdefault(v, set()).add(u)
    _t_phase1 = time.time()
    unique_users = {u for u, _v in matched_pivots}
    valid_users: Set[Any] = set()
    u_bridge_data: Dict[Any, Tuple[Set, Dict]] = {}
    user_av_map: Dict[Any, List] = {}
    for u_id in unique_users:
        if x0_pw_checks:
            fail = False
            for _pvar, ppreds in x0_pw_checks:
                node = graph.get_node(u_id)
                if node is None or not all((p.evaluate(node) for p in ppreds)):
                    fail = True
                    break
            if fail:
                continue
        if user_tie_pw and (not _check_tie_pw_for_pivot(anchor_index, u_id, user_tie_pw)):
            continue
        if x0_bp_indices:
            u_ws = set(w_candidates_base)
            u_bvc: Dict[str, Set[Any]] = {}
            bp_fail = False
            for bp_idx in x0_bp_indices:
                bp = bridge_paths[bp_idx]
                if len(bp.path_sequence) < 2:
                    continue
                key = (bp_idx, u_id)
                if key not in _bridge_match_cache:
                    _bridge_match_cache[key] = match_path_compact(graph, bp, u_id, omr_pattern, all_pw, edge_lookup=bridge_edge_lookups[bp_idx], prefilter_cache=_prefilter_cache)
                bm = _bridge_match_cache[key]
                if bm is None:
                    bp_fail = True
                    break
                if not _merge_candidate_maps(u_bvc, bm):
                    bp_fail = True
                    break
                if bp.path_sequence[-1] == w_var and w_var in bm:
                    u_ws &= bm[w_var]
            if bp_fail or not u_ws:
                continue
            u_bridge_data[u_id] = (u_ws, u_bvc)
        if user_anchor_preds:
            uav = []
            for _, info in user_anchor_preds:
                grouped = anchor_index.query_grouped(info[3], info[4])
                uav.append(frozenset((val for val, pids in grouped.items() if u_id in pids)))
            user_av_map[u_id] = uav
        valid_users.add(u_id)
    if not valid_users:
        return OMRMetrics()
    _t_phase1_end = time.time()
    logger.debug(f'  [Phase1] {_t_phase1_end - _t_phase1:.3f}s — valid_users={len(valid_users)}/{len(unique_users)}, has_u_dep={has_u_dep}, x0_bridges={len(x0_bp_indices)}')
    v_results: Dict[Any, Tuple[Set, Dict, Dict]] = {}
    _total_v = len(users_per_v)
    _v_processed = 0
    _total_w_surviving = 0
    _dis_scores_per_v: Dict[Any, Dict[Any, float]] = {}
    for v_id in tqdm(users_per_v, total=_total_v, desc='  [Phase2] per-v', disable=_total_v < 100, leave=False):
        if not users_per_v[v_id] & valid_users:
            continue
        if y0_pw_checks:
            fail = False
            for _pvar, ppreds in y0_pw_checks:
                node = graph.get_node(v_id)
                if node is None or not all((p.evaluate(node) for p in ppreds)):
                    fail = True
                    break
            if fail:
                continue
        if item_tie_pw and (not _check_tie_pw_for_pivot(anchor_index, v_id, item_tie_pw)):
            continue
        w_reachable = set(w_candidates_base)
        v_bvc: Dict[str, Set[Any]] = {}
        bp_fail = False
        for bp_idx in y0_bp_indices:
            bp = bridge_paths[bp_idx]
            if len(bp.path_sequence) < 2:
                continue
            _bri = _bridge_reachability_indexes.get(bp_idx)
            if _bri is not None:
                bri_reach = _bri.get_reachable(v_id)
                if not bri_reach:
                    bp_fail = True
                    break
                bri_bvc = _bri.get_bvc(v_id)
                if not _merge_candidate_maps(v_bvc, bri_bvc):
                    bp_fail = True
                    break
                if bp.path_sequence[-1] == w_var:
                    w_reachable &= bri_reach
            else:
                key = (bp_idx, v_id)
                if key not in _bridge_match_cache:
                    _bridge_match_cache[key] = match_path_compact(graph, bp, v_id, omr_pattern, all_pw, edge_lookup=bridge_edge_lookups[bp_idx], prefilter_cache=_prefilter_cache)
                bm = _bridge_match_cache[key]
                if bm is None:
                    bp_fail = True
                    break
                if not _merge_candidate_maps(v_bvc, bm):
                    bp_fail = True
                    break
                if bp.path_sequence[-1] == w_var and w_var in bm:
                    w_reachable &= bm[w_var]
        if bp_fail or not w_reachable:
            continue
        w_reachable.discard(v_id)
        if not w_reachable:
            continue
        w_reachable &= _training_items
        if not w_reachable:
            continue
        dis_scores: Dict[Any, float] = {}
        if _no_dis_aggr_filter:
            if _dis_scores_per_v_in is not None and v_id in _dis_scores_per_v_in:
                cached = _dis_scores_per_v_in[v_id]
                dis_scores = {w: cached[w] for w in w_reachable if w in cached}
                _dis_scores_per_v[v_id] = dis_scores
        elif dis_preds:
            if _dis_scores_per_v_in is not None and v_id in _dis_scores_per_v_in:
                cached = _dis_scores_per_v_in[v_id]
                dis_scores = {w: cached[w] for w in w_reachable if w in cached}
            else:
                dis_scores = dis_model.compute_batch(v_id, w_reachable)
            _dis_scores_per_v[v_id] = dis_scores
        item_avs: List = []
        if item_anchor_preds:
            for _, info in item_anchor_preds:
                grouped = anchor_index.query_grouped(info[3], info[4])
                item_avs.append(frozenset((val for val, pids in grouped.items() if v_id in pids)))
        surviving_ws: Set[Any] = set()
        per_w_dis: Dict[Any, float] = {}
        for w_id in w_reachable:
            dis_val = dis_scores.get(w_id) if _no_dis_aggr_filter or dis_preds else None
            if dis_preds and (not _no_dis_aggr_filter):
                if dis_val is None:
                    continue
                if not all((dp.evaluate(dis_val) for dp in dis_preds)):
                    continue
            if not _has_alt_paths:
                alt_ok = True
                alt_var_cands = {}
                _alt_result_cache.setdefault(w_id, (True, {}))
            elif w_id in _alt_result_cache:
                alt_ok, alt_var_cands = _alt_result_cache[w_id]
            else:
                alt_ok = True
                alt_var_cands: Dict[str, Set[Any]] = {}
                for ap_idx, alt_path in enumerate(alt_paths):
                    ap_seq = alt_path.path_sequence
                    if not ap_seq or ap_seq[0] != w_var:
                        continue
                    alt_match = match_path_compact(graph, alt_path, w_id, omr_pattern, all_pw, edge_lookup=alt_edge_lookups[ap_idx], prefilter_cache=_prefilter_cache)
                    if alt_match is None:
                        alt_ok = False
                        break
                    if not _merge_candidate_maps(alt_var_cands, alt_match):
                        alt_ok = False
                        break
                _alt_result_cache[w_id] = (alt_ok, alt_var_cands)
            if not alt_ok:
                continue
            if not has_u_dep and (item_anchor_preds or v_only_fallbacks):
                cm: Dict[str, Set[Any]] = {}
                if not _merge_candidate_maps(cm, v_bvc):
                    continue
                if _has_alt_paths and (not _merge_candidate_maps(cm, alt_var_cands)):
                    continue
                cm[y0_var] = {v_id}
                cm[w_var] = {w_id}
                if y0_narrow_info:
                    if not _apply_narrow_info(graph, cm, y0_narrow_info, w_id):
                        continue
                if item_anchor_preds:
                    if not _apply_anchor_filter(graph, cm, item_anchor_preds, item_avs):
                        continue
                if v_only_fallbacks:
                    if not verify_variable_predicates(graph, cm, v_only_fallbacks):
                        continue
            surviving_ws.add(w_id)
            if dis_val is not None:
                per_w_dis[w_id] = dis_val
        if surviving_ws:
            v_results[v_id] = (surviving_ws, v_bvc, per_w_dis)
            _total_w_surviving += len(surviving_ws)
        _v_processed += 1
        if _v_processed % 200 == 0 or _v_processed == _total_v:
            logger.debug(f"  [Phase2] {_v_processed}/{_total_v} v's done, {len(v_results)} with w, total_w={_total_w_surviving}")
    _t_phase2_end = time.time()
    logger.debug(f'  [Phase2] {_t_phase2_end - _t_phase1_end:.3f}s — v_with_results={len(v_results)}/{_total_v}, total_surviving_w={_total_w_surviving}')
    if not v_results:
        return OMRTopologyCache() if _no_dis_aggr_filter else OMRMetrics()
    if _no_dis_aggr_filter:
        anchor_index = getattr(graph, 'path_anchor_index', None)
        return OMRTopologyCache(valid_users=valid_users, u_bridge_data=u_bridge_data, user_av_map=user_av_map, v_results=v_results, users_per_v=users_per_v, has_u_dep=has_u_dep, alt_result_cache=_alt_result_cache, aggr_scores=_aggr_scores_cache, anchor_pred_infos=anchor_pred_infos, fallback_preds=fallback_preds, user_anchor_preds=user_anchor_preds, item_anchor_preds=item_anchor_preds, x0_bp_indices=x0_bp_indices, y0_narrow_info=y0_narrow_info, x0_narrow_info=x0_narrow_info, v_only_fallbacks=v_only_fallbacks, x0_var=x0_var, y0_var=y0_var, w_var=w_var, matched_pivots=matched_pivots, training_items=_training_items, training_index=training_index if training_index is not None else {}, ml_cache_ref=ml_cache, dis_model_ref=dis_model, bridge_match_cache=_bridge_match_cache, dis_scores_per_v=_dis_scores_per_v, bridge_reachability_indexes=_bridge_reachability_indexes)
    _t_phase3 = time.time()
    all_w_found: Set[Any] = set()
    all_v_with_w: Set[Any] = set()
    dis_score_sum = 0.0
    dis_count = 0
    _triple_count = 0
    if not has_u_dep:
        for v_id, (surv_ws, _v_bvc, pw_dis) in v_results.items():
            v_users = users_per_v[v_id] & valid_users
            n_users = len(v_users)
            if not n_users:
                continue
            all_v_with_w.add(v_id)
            all_w_found.update(surv_ws)
            _v_dis_sum = 0.0
            _v_dis_count = 0
            for w_id in surv_ws:
                dv = pw_dis.get(w_id)
                if dv is not None:
                    _v_dis_sum += dv
                    _v_dis_count += 1
                _triple_count += n_users
            dis_score_sum += _v_dis_sum * n_users
            dis_count += _v_dis_count * n_users
        user_to_vs: Dict[Any, List] = {}
        for v_id in v_results:
            for u_id in users_per_v.get(v_id, set()) & valid_users:
                if u_id not in user_to_vs:
                    user_to_vs[u_id] = [v_id]
                else:
                    user_to_vs[u_id].append(v_id)
        _ti = training_index
        positive_uw_pairs: Set[Tuple] = set()
        matched_uw_in_T: Set[Tuple] = set()
        if _ti:
            for u_id, u_vs in user_to_vs.items():
                u_train = _ti.get(u_id)
                if not u_train:
                    continue
                for w_id in u_train:
                    for v_id in u_vs:
                        v_data = v_results.get(v_id)
                        if v_data is not None and w_id in v_data[0]:
                            matched_uw_in_T.add((u_id, w_id))
                            if training_data[u_id, w_id]:
                                positive_uw_pairs.add((u_id, w_id))
                            break
        else:
            _user_set = frozenset(user_to_vs)
            for u_id, w_id in training_data:
                if u_id not in _user_set:
                    continue
                for v_id in user_to_vs[u_id]:
                    v_data = v_results.get(v_id)
                    if v_data is not None and w_id in v_data[0]:
                        matched_uw_in_T.add((u_id, w_id))
                        if training_data[u_id, w_id]:
                            positive_uw_pairs.add((u_id, w_id))
                        break
        positive_per_u: Dict[Any, Set[Any]] = {}
        for u_id, w_id in positive_uw_pairs:
            positive_per_u.setdefault(u_id, set()).add(w_id)
        all_uw_pairs: Set[Tuple] = set()
        positive_pivots_set: Set[Tuple] = set()
        if positive_per_u:
            for v_id, (surv_ws, _, _) in v_results.items():
                v_u = users_per_v[v_id] & valid_users
                for u_id in v_u:
                    u_pos = positive_per_u.get(u_id)
                    if u_pos and (not u_pos.isdisjoint(surv_ws)):
                        positive_pivots_set.add((u_id, v_id))
        matched_triples: Set[Tuple] = set()
        if _triple_count <= _MATERIALIZE_MATCHED_TRIPLES_LIMIT:
            for u_id, w_id in matched_uw_in_T:
                for v_id in user_to_vs.get(u_id, []):
                    v_data = v_results.get(v_id)
                    if v_data is not None and w_id in v_data[0]:
                        matched_triples.add((u_id, v_id, w_id))
    else:
        need_cm = bool(anchor_pred_infos or fallback_preds)
        all_uw_pairs: Set[Tuple] = set()
        positive_uw_pairs: Set[Tuple] = set()
        matched_uw_in_T: Set[Tuple] = set()
        positive_pivots_set: Set[Tuple] = set()
        matched_triples: Set[Tuple] = set()
        _triple_estimate = sum((len(users_per_v.get(v_id, set()) & valid_users) * len(surv_ws) for v_id, (surv_ws, _v_bvc, _pw_dis) in v_results.items()))
        _materialize_triples = _triple_estimate <= _MATERIALIZE_MATCHED_TRIPLES_LIMIT
        _n_vr = len(v_results)
        if not need_cm:
            _cm_user_to_vs: Dict[Any, List] = {}
            for v_id, (surv_ws, v_bvc, pw_dis) in tqdm(v_results.items(), total=_n_vr, desc='  [Phase3] medium A', disable=_n_vr < 100, leave=False):
                v_users = users_per_v[v_id] & valid_users
                n_users = len(v_users)
                if not n_users:
                    continue
                all_v_with_w.add(v_id)
                all_w_found.update(surv_ws)
                _v_dis_sum = 0.0
                _v_dis_count = 0
                for w_id in surv_ws:
                    dv = pw_dis.get(w_id)
                    if dv is not None:
                        _v_dis_sum += dv
                        _v_dis_count += 1
                    _triple_count += n_users
                dis_score_sum += _v_dis_sum * n_users
                dis_count += _v_dis_count * n_users
                for u_id in v_users:
                    effective = surv_ws
                    if x0_bp_indices:
                        u_data = u_bridge_data.get(u_id)
                        if u_data is None:
                            continue
                        effective = surv_ws & u_data[0]
                    if not effective:
                        continue
                    _cm_user_to_vs.setdefault(u_id, []).append(v_id)
            _ti = training_index
            if _ti:
                for u_id, u_vs in _cm_user_to_vs.items():
                    u_train = _ti.get(u_id)
                    if not u_train:
                        continue
                    for w_id in u_train:
                        u_data = u_bridge_data.get(u_id) if x0_bp_indices else None
                        for v_id in u_vs:
                            v_data = v_results.get(v_id)
                            if v_data is None:
                                continue
                            if w_id not in v_data[0]:
                                continue
                            if u_data is not None and w_id not in u_data[0]:
                                continue
                            matched_uw_in_T.add((u_id, w_id))
                            if training_data[u_id, w_id]:
                                positive_uw_pairs.add((u_id, w_id))
                                positive_pivots_set.add((u_id, v_id))
                            break
            else:
                _user_set = frozenset(_cm_user_to_vs)
                for u_id, w_id in training_data:
                    if u_id not in _user_set:
                        continue
                    u_data = u_bridge_data.get(u_id) if x0_bp_indices else None
                    for v_id in _cm_user_to_vs[u_id]:
                        v_data = v_results.get(v_id)
                        if v_data is None:
                            continue
                        if w_id not in v_data[0]:
                            continue
                        if u_data is not None and w_id not in u_data[0]:
                            continue
                        matched_uw_in_T.add((u_id, w_id))
                        if training_data[u_id, w_id]:
                            positive_uw_pairs.add((u_id, w_id))
                            positive_pivots_set.add((u_id, v_id))
                        break
        else:
            for v_id, (surv_ws, v_bvc, pw_dis) in tqdm(v_results.items(), total=_n_vr, desc='  [Phase3] slow', disable=_n_vr < 100, leave=False):
                v_users = users_per_v[v_id] & valid_users
                if not v_users:
                    continue
                v_valid_ws: Optional[Set[Any]] = None
                if item_anchor_preds or v_only_fallbacks or y0_narrow_info:
                    v_valid_ws = set()
                    for w_id in surv_ws:
                        cm = {}
                        cm.update({k: set(s) for k, s in v_bvc.items()})
                        _, alt_vc = _alt_result_cache.get(w_id, (True, {}))
                        cm.update({k: set(s) for k, s in alt_vc.items()})
                        cm[y0_var] = {v_id}
                        cm[w_var] = {w_id}
                        if y0_narrow_info:
                            if not _apply_narrow_info(graph, cm, y0_narrow_info, w_id):
                                continue
                        if item_anchor_preds:
                            item_avs_local = []
                            for _, info in item_anchor_preds:
                                grouped = anchor_index.query_grouped(info[3], info[4])
                                item_avs_local.append(frozenset((val for val, pids in grouped.items() if v_id in pids)))
                            if not _apply_anchor_filter(graph, cm, item_anchor_preds, item_avs_local):
                                continue
                        if v_only_fallbacks:
                            if not verify_variable_predicates(graph, cm, v_only_fallbacks):
                                continue
                        v_valid_ws.add(w_id)
                    if not v_valid_ws:
                        continue
                effective_surv = v_valid_ws if v_valid_ws is not None else surv_ws
                v_had_w = False
                for u_id in v_users:
                    ws = effective_surv
                    u_bvc_local: Dict[str, Set[Any]] = {}
                    if x0_bp_indices:
                        u_data = u_bridge_data.get(u_id)
                        if u_data is None:
                            continue
                        u_ws_set, u_bvc_local = u_data
                        ws = ws & u_ws_set
                        if not ws:
                            continue
                    if user_anchor_preds or x0_narrow_info:
                        for w_id in ws:
                            cm = {x0_var: {u_id}, y0_var: {v_id}, w_var: {w_id}}
                            if x0_narrow_info:
                                cm.update({k: set(s) for k, s in u_bvc_local.items()})
                                if not _apply_narrow_info(graph, cm, x0_narrow_info, w_id):
                                    continue
                            if user_anchor_preds:
                                uav = user_av_map.get(u_id, [])
                                if not _apply_anchor_filter(graph, cm, user_anchor_preds, uav):
                                    continue
                            all_w_found.add(w_id)
                            v_had_w = True
                            uw = (u_id, w_id)
                            if uw not in all_uw_pairs:
                                all_uw_pairs.add(uw)
                                if uw in training_data:
                                    matched_uw_in_T.add(uw)
                                    if _materialize_triples:
                                        matched_triples.add((u_id, v_id, w_id))
                                    if training_data[uw]:
                                        positive_uw_pairs.add(uw)
                                        positive_pivots_set.add((u_id, v_id))
                            dv = pw_dis.get(w_id)
                            if dv is not None:
                                dis_score_sum += dv
                                dis_count += 1
                            _triple_count += 1
                    else:
                        for w_id in ws:
                            all_w_found.add(w_id)
                            v_had_w = True
                            uw = (u_id, w_id)
                            if uw not in all_uw_pairs:
                                all_uw_pairs.add(uw)
                                if uw in training_data:
                                    matched_uw_in_T.add(uw)
                                    if _materialize_triples:
                                        matched_triples.add((u_id, v_id, w_id))
                                    if training_data[uw]:
                                        positive_uw_pairs.add(uw)
                                        positive_pivots_set.add((u_id, v_id))
                            dv = pw_dis.get(w_id)
                            if dv is not None:
                                dis_score_sum += dv
                                dis_count += 1
                            _triple_count += 1
                if v_had_w:
                    all_v_with_w.add(v_id)
    _t_phase3_end = time.time()
    logger.debug(f'  [Phase3] {_t_phase3_end - _t_phase3:.3f}s — triples={_triple_count}, uw_pairs={len(all_uw_pairs)}, v_with_w={len(all_v_with_w)}, unique_w={len(all_w_found)}, fast_path={not has_u_dep}')
    support = len(positive_uw_pairs)
    matched_count = len(matched_uw_in_T)
    confidence = support / matched_count if matched_count > 0 else 0.0
    diversity_score = dis_score_sum / dis_count if dis_count > 0 else 0.0
    acceptability_score = _compute_mean_acceptability(matched_uw_in_T, ml_cache, aggr_model)
    return OMRMetrics(support=support, confidence=confidence, matched_count=matched_count, diversity_score=diversity_score, acceptability_score=acceptability_score, alternative_count=len(all_w_found), anchor_count=len(all_v_with_w), matched_triples=matched_triples, positive_pivots=positive_pivots_set)
_SHARED_GRAPH = None
_SHARED_TRAINING_DATA = None
_SHARED_TRAINING_INDEX = None
_SHARED_ML_CACHE = None
_SHARED_ATTR_CONFIG = None
_SHARED_TIE_RULES = None
_SHARED_TIE_METRICS = None
_SHARED_DIS_MODEL = None
_SHARED_AGGR_MODEL = None
_SHARED_MINING_CONFIG = None
_SHARED_REALTIME_SAVE_DIR = None
_WORKER_OMR_GEN = None
_WORKER_RULE_COUNTER = 0

def _save_omr_rule_realtime_worker(save_dir: Optional[str], tie_rule_idx: int, omr_rule: OMRule, metrics: OMRMetrics, tie_rules: List[TIERule], local_idx: int) -> None:
    if save_dir is None:
        return
    parent_tie_idx = -1
    parent_sig = canonical_tie_rule_signature(omr_rule.parent_tie)
    for i, tr in enumerate(tie_rules):
        if tr is omr_rule.parent_tie or canonical_tie_rule_signature(tr) == parent_sig:
            parent_tie_idx = i
            break
    rule_filename = f'omr_rule_t{tie_rule_idx:05d}_p{os.getpid()}_{local_idx:04d}.json'
    rule_path = os.path.join(save_dir, rule_filename)
    rule_data = {'rule_id': f't{tie_rule_idx:05d}_p{os.getpid()}_{local_idx:04d}', 'parent_tie_index': parent_tie_idx, 'parent_tie_conclusion': str(omr_rule.parent_tie.conclusion) if omr_rule.parent_tie.conclusion else 'N/A', 'support': metrics.support, 'confidence': round(metrics.confidence, 4), 'diversity_score': round(metrics.diversity_score, 4), 'acceptability_score': round(metrics.acceptability_score, 4), 'alternative_count': metrics.alternative_count, 'anchor_count': metrics.anchor_count, 'matched_count': metrics.matched_count, 'pattern': {'user_center': omr_rule.pattern.user_center, 'anchor_center': omr_rule.pattern.anchor_center, 'alt_center': omr_rule.pattern.alt_center, 'num_nodes': len(omr_rule.pattern.nodes), 'bridge_paths': [{'sequence': bp.path_sequence, 'edges': [{'source_var': e.source_var, 'target_var': e.target_var, 'label': e.label, 'is_forward': e.is_forward} for e in bp.edges]} for bp in omr_rule.pattern.bridge_paths], 'alt_star_paths': [{'sequence': ap.path_sequence, 'edges': [{'source_var': e.source_var, 'target_var': e.target_var, 'label': e.label, 'is_forward': e.is_forward} for e in ap.edges]} for ap in omr_rule.pattern.alt_star.paths]}, 'preconditions': [str(p) for p in omr_rule.preconditions], 'conclusion': str(omr_rule.conclusion), 'timestamp': datetime.now().isoformat(), 'saved_from_worker': True}
    with open(rule_path, 'w') as f:
        json.dump(rule_data, f, indent=2, default=str)

def _init_omr_worker(graph, training_data, training_index, ml_cache, attr_config, tie_rules, tie_metrics, dis_model, aggr_model, mining_config, realtime_save_dir):
    global _SHARED_GRAPH, _SHARED_TRAINING_DATA, _SHARED_TRAINING_INDEX
    global _SHARED_ML_CACHE, _SHARED_ATTR_CONFIG
    global _SHARED_TIE_RULES, _SHARED_TIE_METRICS
    global _SHARED_DIS_MODEL, _SHARED_AGGR_MODEL, _SHARED_MINING_CONFIG
    global _SHARED_REALTIME_SAVE_DIR, _WORKER_OMR_GEN, _WORKER_RULE_COUNTER
    _SHARED_GRAPH = graph
    _SHARED_TRAINING_DATA = training_data
    _SHARED_TRAINING_INDEX = training_index
    _SHARED_ML_CACHE = ml_cache
    _SHARED_ATTR_CONFIG = attr_config
    _SHARED_TIE_RULES = tie_rules
    _SHARED_TIE_METRICS = tie_metrics
    _SHARED_DIS_MODEL = dis_model
    _SHARED_AGGR_MODEL = aggr_model
    _SHARED_MINING_CONFIG = mining_config
    _SHARED_REALTIME_SAVE_DIR = realtime_save_dir
    cfg = mining_config
    _WORKER_RULE_COUNTER = 0
    _WORKER_OMR_GEN = OMRPredicateGenerator(graph=_SHARED_GRAPH, ml_cache=_SHARED_ML_CACHE, dis_model=_SHARED_DIS_MODEL, aggr_model=_SHARED_AGGR_MODEL, attr_config=_SHARED_ATTR_CONFIG, dis_thresholds=cfg['dis_thresholds'], aggr_thresholds=cfg['aggr_thresholds'], max_bridge_hops=cfg['max_bridge_hops'], max_alt_paths=cfg['max_alt_paths'], max_predicates=cfg['max_predicates'], min_support=cfg['min_support'], min_confidence=cfg['min_confidence'], max_omr_per_tie=cfg['max_omr_per_tie'], gini_top_k=cfg['gini_top_k'], omr_predicates=cfg.get('omr_predicates', 2), tie_predicates=cfg.get('tie_predicates', 1))
    _WORKER_OMR_GEN.training_index = _SHARED_TRAINING_INDEX

def _worker_mine_omr(tie_rule_idx: int) -> List[Tuple[OMRule, OMRMetrics]]:
    try:
        tie_rule = _SHARED_TIE_RULES[tie_rule_idx]
        parent_metrics = _SHARED_TIE_METRICS[tie_rule_idx] if _SHARED_TIE_METRICS else None
        _WORKER_OMR_GEN.current_tie_rule_idx = tie_rule_idx
        _WORKER_OMR_GEN.worker_save_dir = _SHARED_REALTIME_SAVE_DIR
        results = _WORKER_OMR_GEN.generate_omr_rules(tie_rule=tie_rule, parent_tie_metrics=parent_metrics, training_data=_SHARED_TRAINING_DATA)
        return results
    except Exception as e:
        import traceback
        logger.error(f'Worker error on TIE rule {tie_rule_idx}: {e}')
        logger.debug(traceback.format_exc())
        return []

class OMRPredicateGenerator:

    def __init__(self, graph, ml_cache, dis_model, aggr_model, attr_config, dis_thresholds, aggr_thresholds, max_bridge_hops, max_alt_paths, max_predicates, min_support, min_confidence, max_omr_per_tie, gini_top_k, omr_predicates=2, tie_predicates=1):
        self.graph = graph
        self.ml_cache = ml_cache
        self.dis_model = dis_model
        self.aggr_model = aggr_model
        self.attr_config = attr_config
        self.dis_thresholds = dis_thresholds
        self.aggr_thresholds = aggr_thresholds
        self.max_bridge_hops = max_bridge_hops
        self.max_alt_paths = max_alt_paths
        self.max_predicates = max_predicates
        self.min_support = min_support
        self.min_confidence = min_confidence
        self.max_omr_per_tie = max_omr_per_tie
        self.gini_top_k = gini_top_k
        self.omr_predicates = omr_predicates
        self.tie_predicates = tie_predicates
        self.metrics_cache = MetricsCache()
        self.stats = ProfilingStats()
        self.training_index = None
        self.current_tie_rule_idx = -1
        self.worker_save_dir: Optional[str] = None
        self.edge_source_label = getattr(graph, 'edge_source_label', {}) or {}
        self.edge_target_label = getattr(graph, 'edge_target_label', {}) or {}

    def _prefix_signature(self, predicates: List) -> frozenset:
        return frozenset((str(p) for p in predicates))

    def _is_pruned_prefix(self, pruned_prefixes: Optional[Set[frozenset]], predicates: List) -> bool:
        if not pruned_prefixes:
            return False
        sig = self._prefix_signature(predicates)
        return any((prefix.issubset(sig) for prefix in pruned_prefixes))

    def _add_pruned_prefix(self, pruned_prefixes: Optional[Set[frozenset]], predicates: List) -> None:
        if pruned_prefixes is None:
            return
        pruned_prefixes.add(self._prefix_signature(predicates))

    def _record_discovered_rule(self, discovered: List[Tuple[OMRule, OMRMetrics]], seen_sigs: Set[str], omr_rule: OMRule, metrics: OMRMetrics) -> bool:
        global _WORKER_RULE_COUNTER
        rule_sig = self._get_omr_signature(omr_rule)
        if rule_sig in seen_sigs:
            return False
        seen_sigs.add(rule_sig)
        discovered.append((omr_rule, metrics))
        if self.worker_save_dir is not None:
            _WORKER_RULE_COUNTER += 1
            _save_omr_rule_realtime_worker(self.worker_save_dir, self.current_tie_rule_idx, omr_rule, metrics, _SHARED_TIE_RULES or [omr_rule.parent_tie], _WORKER_RULE_COUNTER)
        return True

    def generate_omr_rules(self, tie_rule: TIERule, parent_tie_metrics: Optional[RuleMetrics], training_data: Dict[Tuple, bool]) -> List[Tuple[OMRule, OMRMetrics]]:
        if parent_tie_metrics is None or not parent_tie_metrics.positive_pivots:
            parent_tie_metrics = compute_rule_metrics(self.graph, tie_rule, training_data, self.ml_cache, training_index=self.training_index)
        if parent_tie_metrics.support == 0:
            return []
        tie_pattern = tie_rule.pattern
        item_label = tie_pattern.nodes[tie_pattern.item_center].label
        bridge_candidates = self._enumerate_bridge_paths(tie_pattern)
        if not bridge_candidates:
            logger.debug('  No bridge paths found for TIE pattern')
            return []
        alt_candidates = self._enumerate_alt_paths(item_label, tie_rule.pattern)
        tie_frequent_values = self._extract_tie_frequent_values(tie_rule)
        total_combos = len(bridge_candidates) * len(alt_candidates)
        discovered = []
        seen_sigs = set()
        _combo_idx = 0
        _eval_count = 0
        _t_gen_start = time.time()

        def _evaluate_topology(omr_pattern: OMRPattern, bridge_cache_shared: Optional[Dict], dis_cache_shared: Optional[Dict], bridge_reachability_shared: Optional[Dict]) -> Tuple[Optional[Dict], Optional[Dict], Optional[Dict]]:
            nonlocal _combo_idx, _eval_count
            _combo_idx += 1
            delta_omr = self._build_omr_predicate_set(omr_pattern, tie_rule, tie_frequent_values=tie_frequent_values)
            if not delta_omr:
                return (bridge_cache_shared, dis_cache_shared, bridge_reachability_shared)
            seed_groups = self._build_seed_groups(delta_omr)
            if not seed_groups:
                return (bridge_cache_shared, dis_cache_shared, bridge_reachability_shared)
            base_seed = seed_groups[0]
            base_preds = list(tie_rule.preconditions) + base_seed
            base_rule = OMRule(pattern=omr_pattern, parent_tie=tie_rule, preconditions=base_preds, conclusion=EdgePredicate(omr_pattern.user_center, omr_pattern.alt_center, 'rec_d'))
            _t_cache = time.time()
            topology_cache = compute_omr_metrics(self.graph, base_rule, training_data, self.ml_cache, self.dis_model, self.aggr_model, parent_tie_metrics=parent_tie_metrics, training_index=self.training_index, _no_dis_aggr_filter=True, _bridge_match_cache_in=bridge_cache_shared, _dis_scores_per_v_in=dis_cache_shared, _bridge_reachability_indexes_in=bridge_reachability_shared)
            _dt_cache = time.time() - _t_cache
            cache_valid = isinstance(topology_cache, OMRTopologyCache) and topology_cache.v_results
            _n_cache_v = len(topology_cache.v_results) if cache_valid else 0
            if cache_valid and bridge_cache_shared is None:
                bridge_cache_shared = topology_cache.bridge_match_cache
                dis_cache_shared = topology_cache.dis_scores_per_v
                bridge_reachability_shared = topology_cache.bridge_reachability_indexes
            if not cache_valid:
                return (bridge_cache_shared, dis_cache_shared, bridge_reachability_shared)
            pruned_prefixes: Set[frozenset] = set()
            for seed in seed_groups:
                if len(discovered) >= self.max_omr_per_tie:
                    break
                if self._is_pruned_prefix(pruned_prefixes, list(tie_rule.preconditions) + seed):
                    continue
                all_preds = list(tie_rule.preconditions) + seed
                omr_rule = OMRule(pattern=omr_pattern, parent_tie=tie_rule, preconditions=all_preds, conclusion=EdgePredicate(omr_pattern.user_center, omr_pattern.alt_center, 'rec_d'))
                rule_sig = self._get_omr_signature(omr_rule)
                if rule_sig in seen_sigs:
                    continue
                _t_eval = time.time()
                seed_dis = omr_rule.get_dis_predicates()
                seed_aggr = omr_rule.get_aggr_predicates()
                metrics = evaluate_omr_from_cache(topology_cache, training_data, dis_preds=seed_dis, aggr_preds=seed_aggr, graph=self.graph)
                _eval_count += 1
                _dt_eval = time.time() - _t_eval
                branch_impossible = _omr_branch_impossible(metrics, self.min_support, self.min_confidence)
                if metrics.support >= self.min_support and metrics.confidence >= self.min_confidence:
                    self._record_discovered_rule(discovered, seen_sigs, omr_rule, metrics)
                elif metrics.support >= self.min_support and (not branch_impossible):
                    expanded = self._expand_omr_rule(omr_rule, delta_omr, seed, tie_rule, training_data, parent_tie_metrics, seen_sigs, topology_cache=topology_cache, pruned_prefixes=pruned_prefixes)
                    discovered.extend(expanded)
                elif branch_impossible:
                    self._add_pruned_prefix(pruned_prefixes, all_preds)
            return (bridge_cache_shared, dis_cache_shared, bridge_reachability_shared)
        for bridge_path, bridge_origin in bridge_candidates:
            if len(discovered) >= self.max_omr_per_tie:
                break
            _bridge_cache_shared: Optional[Dict] = None
            _dis_cache_shared: Optional[Dict] = None
            _bridge_reachability_shared: Optional[Dict] = None
            for alt_paths in alt_candidates:
                if len(discovered) >= self.max_omr_per_tie:
                    break
                try:
                    omr_pattern = self._assemble_omr_pattern(tie_rule, bridge_path, bridge_origin, alt_paths, item_label)
                except (ValueError, KeyError) as e:
                    logger.debug(f'  Failed to assemble OMRPattern: {e}')
                    continue
                _bridge_cache_shared, _dis_cache_shared, _bridge_reachability_shared = _evaluate_topology(omr_pattern, _bridge_cache_shared, _dis_cache_shared, _bridge_reachability_shared)
        return discovered

    def _build_bridge_path(self, origin_var: str, origin_label: str, hops: List[Tuple[str, str, str, bool]], item_label: str) -> Path:
        path_nodes = [(origin_var, origin_label, None)]
        for hop_idx, (_el, _src_label, tgt_label, _fwd) in enumerate(hops):
            if hop_idx == len(hops) - 1:
                path_nodes.append(('w_omr', item_label, None))
            else:
                z_var = f'z_b{hop_idx}'
                path_nodes.append((z_var, tgt_label, self._get_label_attrs(tgt_label)))
        bp = Path(path_nodes)
        for hop_idx, (edge_label, _src_label, _tgt_label, is_forward) in enumerate(hops):
            src_var = path_nodes[hop_idx][0]
            tgt_var = path_nodes[hop_idx + 1][0]
            bp.add_path_edge(src_var, tgt_var, edge_label, is_forward=is_forward)
        return bp

    def _enumerate_bridge_paths(self, tie_pattern: DualStarPattern) -> List[Tuple[Path, str]]:
        results: List[Tuple[Path, str]] = []
        hop_counts: Dict[int, int] = defaultdict(int)
        item_center = tie_pattern.item_center
        user_center = tie_pattern.user_center
        item_label = tie_pattern.nodes[item_center].label
        user_label = tie_pattern.nodes[user_center].label

        def _edges_from_label(src_label):
            edges = []
            for el, sl in self.edge_source_label.items():
                tl = self.edge_target_label.get(el)
                if sl == src_label and tl is not None:
                    edges.append((el, tl, True))
                if tl == src_label and sl is not None:
                    edges.append((el, sl, False))
            return edges

        def _is_immediate_backtrack(hops_so_far, new_hop):
            if not hops_so_far:
                return False
            prev_el, prev_src_label, prev_tgt_label, prev_fwd = hops_so_far[-1]
            new_el, new_src_label, new_tgt_label, new_fwd = new_hop
            return new_el == prev_el and new_fwd == prev_fwd and (new_tgt_label == prev_src_label)

        def _enumerate_from_origin(origin_var: str, origin_label: str) -> None:
            queue: List[Tuple[str, List[Tuple[str, str, str, bool]]]] = [(origin_label, [])]
            q_idx = 0
            while q_idx < len(queue):
                current_label, hops_so_far = queue[q_idx]
                q_idx += 1
                if len(hops_so_far) >= self.max_bridge_hops:
                    continue
                for el, tl, fwd in _edges_from_label(current_label):
                    new_hop = (el, current_label, tl, fwd)
                    if _is_immediate_backtrack(hops_so_far, new_hop):
                        continue
                    if tl != item_label:
                        intermediate_count = len(self.graph._nodes_by_label.get(tl, set()))
                        if intermediate_count > 10000:
                            continue
                    new_hops = hops_so_far + [new_hop]
                    if tl == item_label:
                        bp = self._build_bridge_path(origin_var, origin_label, new_hops, item_label)
                        results.append((bp, origin_var))
                        hop_counts[len(new_hops)] += 1
                    if len(new_hops) < self.max_bridge_hops:
                        queue.append((tl, new_hops))
        if self.max_bridge_hops >= 1:
            _enumerate_from_origin(item_center, item_label)
            _enumerate_from_origin(user_center, user_label)
        seen = set()
        unique = []
        for bp, origin in results:
            key = (origin, tuple(((e.source_var, e.target_var, e.label, e.is_forward) for e in bp.edges)))
            if key not in seen:
                seen.add(key)
                unique.append((bp, origin))
        if hop_counts:
            hop_summary = ', '.join((f'{count}x{hop}-hop' for hop, count in sorted(hop_counts.items())))
        else:
            hop_summary = 'none'
        logger.debug(f'  Enumerated {len(unique)} bridge paths: {hop_summary}')
        return unique

    def _enumerate_reusable_alt_paths(self, item_label: str, tie_pattern: DualStarPattern) -> List[Path]:
        reusable_paths: List[Path] = []
        seen: Set[Tuple[str, str, str, bool]] = set()
        anchor = tie_pattern.item_center
        for path in tie_pattern.item_paths:
            for edge in path.edges:
                neighbor_var = None
                alt_is_forward = None
                if edge.source_var == anchor:
                    neighbor_var = edge.target_var
                    alt_is_forward = edge.is_forward
                elif edge.target_var == anchor:
                    neighbor_var = edge.source_var
                    alt_is_forward = not edge.is_forward
                if neighbor_var is None:
                    continue
                neighbor_node = tie_pattern.nodes.get(neighbor_var)
                if neighbor_node is None:
                    continue
                key = (neighbor_var, neighbor_node.label, edge.label, bool(alt_is_forward))
                if key in seen:
                    continue
                seen.add(key)
                alt_path = Path([('w_omr', item_label, None), (neighbor_var, neighbor_node.label, set(neighbor_node.attributes or set()))])
                alt_path.add_path_edge('w_omr', neighbor_var, edge.label, is_forward=alt_is_forward)
                reusable_paths.append(alt_path)
        return reusable_paths

    def _enumerate_alt_paths(self, item_label: str, tie_pattern: Optional[DualStarPattern]=None) -> List[List[Path]]:
        single_paths = []
        alt_var_counter = [0]

        def _next_alt_var():
            v = f'z_a{alt_var_counter[0]}'
            alt_var_counter[0] += 1
            return v
        if tie_pattern is not None:
            single_paths.extend(self._enumerate_reusable_alt_paths(item_label, tie_pattern))
        existing_keys = {tuple(((e.source_var, e.target_var, e.label, bool(e.is_forward)) for e in p.edges)) for p in single_paths}
        for el, sl in self.edge_source_label.items():
            tl = self.edge_target_label.get(el)
            if sl == item_label and tl is not None:
                alt_var_counter[0] = len(single_paths)
                z_var = _next_alt_var()
                z_attrs = self._get_label_attrs(tl)
                p = Path([('w_omr', item_label, None), (z_var, tl, z_attrs)])
                p.add_path_edge('w_omr', z_var, el, is_forward=True)
                key = tuple(((e.source_var, e.target_var, e.label, bool(e.is_forward)) for e in p.edges))
                if key not in existing_keys:
                    single_paths.append(p)
                    existing_keys.add(key)
            if tl == item_label and sl is not None:
                alt_var_counter[0] = len(single_paths)
                z_var = _next_alt_var()
                z_attrs = self._get_label_attrs(sl)
                p = Path([('w_omr', item_label, None), (z_var, sl, z_attrs)])
                p.add_path_edge('w_omr', z_var, el, is_forward=False)
                key = tuple(((e.source_var, e.target_var, e.label, bool(e.is_forward)) for e in p.edges))
                if key not in existing_keys:
                    single_paths.append(p)
                    existing_keys.add(key)
        combos = [[]]
        max_paths = max(int(self.max_alt_paths), 0)
        for size in range(1, min(max_paths, len(single_paths)) + 1):
            for selected in combinations(single_paths, size):
                combos.append(list(selected))
        return combos

    def _assemble_omr_pattern(self, tie_rule: TIERule, bridge_path: Path, bridge_origin: str, alt_paths: List[Path], item_label: str) -> OMRPattern:
        tie_pattern = tie_rule.pattern
        omr_pattern = OMRPattern(user_center=tie_pattern.user_center, anchor_center=tie_pattern.item_center, alt_center='w_omr', alt_center_label=item_label, tie_pattern=tie_pattern)
        omr_pattern.add_bridge_path(bridge_path, bridge_origin)
        for alt_path in alt_paths:
            omr_pattern.add_alt_path(alt_path)
        return omr_pattern

    def _build_omr_predicate_set(self, omr_pattern: OMRPattern, tie_rule: TIERule, tie_frequent_values: Optional[Dict[Tuple[str, str], List[Any]]]=None) -> List:
        delta = []
        tie_frequent_values = tie_frequent_values or {}
        delta.append(RecPredicate(omr_pattern.user_center, omr_pattern.anchor_center))
        for threshold in self.dis_thresholds:
            delta.append(DisPredicate(omr_pattern.anchor_center, omr_pattern.alt_center, '>=', threshold))
        for threshold in self.aggr_thresholds:
            delta.append(AggrPredicate(omr_pattern.alt_center, '>=', threshold))
        tie_existing_pw: set = set()
        for _var, _preds in tie_rule.get_point_wise_predicates().items():
            for _p in _preds:
                tie_existing_pw.add((_p.variable, _p.attribute, _p.operator, _p.constant))
        tie_existing_pair: set = set()
        for p in tie_rule.get_pair_wise_predicates():
            tie_existing_pair.add((p.var1, p.attr1, p.operator, p.var2, p.attr2))
            rev_op = {ComparisonOp.EQ: ComparisonOp.EQ, ComparisonOp.NE: ComparisonOp.NE, ComparisonOp.LT: ComparisonOp.GT, ComparisonOp.GT: ComparisonOp.LT, ComparisonOp.LE: ComparisonOp.GE, ComparisonOp.GE: ComparisonOp.LE}.get(p.operator, p.operator)
            tie_existing_pair.add((p.var2, p.attr2, rev_op, p.var1, p.attr1))
        tie_pattern = tie_rule.pattern
        tie_pivot_vars = {tie_pattern.user_center, tie_pattern.item_center}
        tie_vars = set(tie_pattern.nodes.keys())
        anchor_index = getattr(self.graph, 'path_anchor_index', None)
        _tie_intermediate_sig: Dict[str, Optional[str]] = {}
        for var in tie_vars - tie_pivot_vars:
            sig, _ = _get_prefix_sig_for_var(tie_pattern, var)
            _tie_intermediate_sig[var] = sig

        def _pair_passes_anchor_gate(var: str, attr: str) -> bool:
            if var not in _tie_intermediate_sig:
                return True
            if anchor_index is None:
                return False
            sig = _tie_intermediate_sig[var]
            if sig is None:
                return False
            return (sig, attr) in anchor_index.grouped_by_value
        for var in sorted(omr_pattern.nodes.keys()):
            node = omr_pattern.nodes.get(var)
            if node is None:
                continue
            node_attrs = self._get_node_attrs(node)
            for attr in sorted(node_attrs):
                ops = self._get_allowed_ops(attr, node.label)
                sample_values = None
                if var in tie_pattern.nodes:
                    cached_values = tie_frequent_values.get((node.label, attr))
                    if cached_values:
                        sample_values = list(cached_values)
                if sample_values is None:
                    sample_values = self._get_frequent_attribute_values(node.label, attr, max_samples=3, min_frequency=0.01)
                for value in sample_values:
                    if ops['pointwise_eq']:
                        if (var, attr, ComparisonOp.EQ, value) not in tie_existing_pw:
                            delta.append(PointWisePredicate(var, attr, ComparisonOp.EQ, value))
                    if ops['pointwise_lt_gt'] and isinstance(value, (int, float)):
                        if (var, attr, ComparisonOp.LT, value) not in tie_existing_pw:
                            delta.append(PointWisePredicate(var, attr, ComparisonOp.LT, value))
                        if (var, attr, ComparisonOp.GT, value) not in tie_existing_pw:
                            delta.append(PointWisePredicate(var, attr, ComparisonOp.GT, value))
        _var_region: Dict[str, str] = {}
        _var_region[omr_pattern.user_center] = 'user'
        _var_region[omr_pattern.anchor_center] = 'item'
        _var_region[omr_pattern.alt_center] = 'alt'
        for path in tie_pattern.user_paths:
            for v in path.path_sequence:
                _var_region.setdefault(v, 'user')
        for path in tie_pattern.item_paths:
            for v in path.path_sequence:
                _var_region.setdefault(v, 'item')
        for path in omr_pattern.alt_star.paths if hasattr(omr_pattern.alt_star, 'paths') else []:
            for v in path.path_sequence:
                _var_region.setdefault(v, 'alt')
        for bp in omr_pattern.bridge_paths:
            for v in bp.path_sequence:
                if v not in (omr_pattern.user_center, omr_pattern.anchor_center, omr_pattern.alt_center):
                    _var_region[v] = 'bridge'
        _center_vars = {omr_pattern.user_center, omr_pattern.anchor_center, omr_pattern.alt_center}
        _runtime_available_vars = set(_center_vars)
        for bp in omr_pattern.bridge_paths:
            _runtime_available_vars.update(bp.path_sequence)
        for ap in omr_pattern.alt_star.paths if hasattr(omr_pattern.alt_star, 'paths') else []:
            _runtime_available_vars.update(ap.path_sequence)
        all_vars = sorted(omr_pattern.nodes.keys())
        for i, var1 in enumerate(all_vars):
            node1 = omr_pattern.nodes.get(var1)
            if node1 is None:
                continue
            attrs1 = self._get_node_attrs(node1)
            for var2 in all_vars[i + 1:]:
                node2 = omr_pattern.nodes.get(var2)
                if node2 is None:
                    continue
                if node1.label != node2.label:
                    continue
                if var1 not in _runtime_available_vars and var2 not in _runtime_available_vars:
                    continue
                attrs2 = self._get_node_attrs(node2)
                shared_attrs = attrs1 & attrs2
                same_region = _var_region.get(var1) == _var_region.get(var2)
                if not same_region:
                    r1, r2 = (_var_region.get(var1), _var_region.get(var2))
                    if var1 not in _center_vars and var2 not in _center_vars and (r1 in ('user', 'item')) and (r2 in ('user', 'item')):
                        continue
                if same_region:
                    shared_id_attrs = {a for a in shared_attrs if self._get_attr_type(a, node1.label) == 'id'}
                    for attr in sorted(shared_id_attrs):
                        if not _pair_passes_anchor_gate(var1, attr):
                            continue
                        if not _pair_passes_anchor_gate(var2, attr):
                            continue
                        if (var1, attr, ComparisonOp.NE, var2, attr) not in tie_existing_pair:
                            delta.append(PairWisePredicate(var1, attr, ComparisonOp.NE, var2, attr))
                    continue
                for attr in sorted(shared_attrs):
                    if not _pair_passes_anchor_gate(var1, attr):
                        continue
                    if not _pair_passes_anchor_gate(var2, attr):
                        continue
                    attr_type = self._get_attr_type(attr, node1.label)
                    if attr_type == 'id':
                        if (var1, attr, ComparisonOp.EQ, var2, attr) not in tie_existing_pair:
                            delta.append(PairWisePredicate(var1, attr, ComparisonOp.EQ, var2, attr))
                    else:
                        ops = self._get_allowed_ops(attr, node1.label)
                        if ops['pairwise_eq']:
                            if (var1, attr, ComparisonOp.EQ, var2, attr) not in tie_existing_pair:
                                delta.append(PairWisePredicate(var1, attr, ComparisonOp.EQ, var2, attr))
                        if ops['pairwise_lt_gt']:
                            if (var1, attr, ComparisonOp.LT, var2, attr) not in tie_existing_pair:
                                delta.append(PairWisePredicate(var1, attr, ComparisonOp.LT, var2, attr))
                            if (var1, attr, ComparisonOp.GT, var2, attr) not in tie_existing_pair:
                                delta.append(PairWisePredicate(var1, attr, ComparisonOp.GT, var2, attr))
        return delta

    def _build_seed_groups(self, delta: List) -> List[List]:
        rec_preds = [p for p in delta if isinstance(p, RecPredicate)]
        dis_preds = [p for p in delta if isinstance(p, DisPredicate)]
        aggr_preds = [p for p in delta if isinstance(p, AggrPredicate)]
        if not rec_preds:
            return []
        rec = rec_preds[0]
        groups = []
        if not dis_preds:
            dis_preds = [None]
        if not aggr_preds:
            aggr_preds = [None]
        for dp in dis_preds:
            for ap in aggr_preds:
                seed = [rec]
                if dp is not None:
                    seed.append(dp)
                if ap is not None:
                    seed.append(ap)
                groups.append(seed)
        return groups

    def _expand_omr_rule(self, omr_rule: OMRule, delta: List, seed: List, tie_rule: TIERule, training_data: Dict, parent_tie_metrics: RuleMetrics, seen_sigs: Set[str], depth: int=0, max_depth: int=None, omr_budget: int=None, tie_budget: int=None, topology_cache: Optional[OMRTopologyCache]=None, pruned_prefixes: Optional[Set[frozenset]]=None) -> List[Tuple[OMRule, OMRMetrics]]:
        if omr_budget is None:
            omr_budget = self.omr_predicates
        if tie_budget is None:
            tie_budget = self.tie_predicates
        if max_depth is None:
            max_depth = self.omr_predicates + self.tie_predicates
        results = []
        if depth >= max_depth or (omr_budget <= 0 and tie_budget <= 0):
            return results
        omr_only_vars = omr_rule.pattern.get_omr_only_variables()

        def _is_omr_pred(p) -> bool:
            if isinstance(p, PointWisePredicate):
                return p.variable in omr_only_vars
            if isinstance(p, PairWisePredicate):
                return p.var1 in omr_only_vars or p.var2 in omr_only_vars
            return False
        current_preds = list(omr_rule.preconditions)
        current_strs = {str(p) for p in current_preds}
        remaining = []
        for p in delta:
            if str(p) in current_strs:
                continue
            if isinstance(p, (RecPredicate, DisPredicate, AggrPredicate)):
                continue
            if _is_omr_pred(p) and omr_budget <= 0:
                continue
            if not _is_omr_pred(p) and tie_budget <= 0:
                continue
            contradicts = False
            for cp in current_preds:
                if isinstance(p, PairWisePredicate) and isinstance(cp, PairWisePredicate):
                    if _are_contradictory(p, cp):
                        contradicts = True
                        break
                if isinstance(p, PointWisePredicate) and isinstance(cp, PointWisePredicate):
                    if _pw_are_contradictory(p, cp):
                        contradicts = True
                        break
            if not contradicts:
                remaining.append(p)
        pw_candidates = [p for p in remaining if isinstance(p, PointWisePredicate)]
        pair_candidates = [p for p in remaining if isinstance(p, PairWisePredicate)]
        expansion_candidates = pw_candidates + pair_candidates
        if not expansion_candidates:
            return results
        scored = []
        for pred in expansion_candidates:
            score = self._estimate_selectivity(pred)
            scored.append((score, pred))
        scored.sort(key=lambda x: x[0], reverse=True)
        for _score, pred in scored[:self.gini_top_k]:
            if len(results) + len(seen_sigs) >= self.max_omr_per_tie:
                break
            extended_preds = current_preds + [pred]
            if self._is_pruned_prefix(pruned_prefixes, extended_preds):
                continue
            test_rule = OMRule(pattern=omr_rule.pattern, parent_tie=tie_rule, preconditions=extended_preds, conclusion=omr_rule.conclusion)
            rule_sig = self._get_omr_signature(test_rule)
            if rule_sig in seen_sigs:
                continue
            metrics = None
            filtered_cache = None
            next_topology_cache = None
            if topology_cache is not None:
                filtered_cache = filter_topology_cache(topology_cache, [pred], self.graph)
                if filtered_cache is not None and filtered_cache.v_results:
                    seed_dis = test_rule.get_dis_predicates()
                    seed_aggr = test_rule.get_aggr_predicates()
                    metrics = evaluate_omr_from_cache(filtered_cache, training_data, dis_preds=seed_dis, aggr_preds=seed_aggr, graph=self.graph)
                    next_topology_cache = filtered_cache
            if metrics is None:
                metrics = compute_omr_metrics(self.graph, test_rule, training_data, self.ml_cache, self.dis_model, self.aggr_model, parent_tie_metrics=parent_tie_metrics, training_index=self.training_index)
                if filtered_cache is None and topology_cache is not None and (metrics.support >= self.min_support) and (depth + 1 < max_depth):
                    rebuilt_cache = compute_omr_metrics(self.graph, test_rule, training_data, self.ml_cache, self.dis_model, self.aggr_model, parent_tie_metrics=parent_tie_metrics, training_index=self.training_index, _no_dis_aggr_filter=True, _bridge_match_cache_in=topology_cache.bridge_match_cache, _bridge_reachability_indexes_in=topology_cache.bridge_reachability_indexes)
                    if isinstance(rebuilt_cache, OMRTopologyCache) and rebuilt_cache.v_results:
                        next_topology_cache = rebuilt_cache
            branch_impossible = _omr_branch_impossible(metrics, self.min_support, self.min_confidence)
            if metrics.support >= self.min_support and metrics.confidence >= self.min_confidence:
                self._record_discovered_rule(results, seen_sigs, test_rule, metrics)
            elif metrics.support >= self.min_support and (not branch_impossible) and (depth + 1 < max_depth):
                is_omr = _is_omr_pred(pred)
                deeper = self._expand_omr_rule(test_rule, delta, seed, tie_rule, training_data, parent_tie_metrics, seen_sigs, depth=depth + 1, max_depth=max_depth, omr_budget=omr_budget - (1 if is_omr else 0), tie_budget=tie_budget - (0 if is_omr else 1), topology_cache=next_topology_cache, pruned_prefixes=pruned_prefixes)
                results.extend(deeper)
            elif branch_impossible:
                self._add_pruned_prefix(pruned_prefixes, extended_preds)
        return results

    def _estimate_selectivity(self, pred) -> float:
        if isinstance(pred, PointWisePredicate):
            if pred.operator == ComparisonOp.EQ:
                _by_attr = getattr(self.graph, '_attr_to_values', None)
                if _by_attr is not None and pred.attribute in _by_attr:
                    value_map = _by_attr[pred.attribute]
                    node_set = value_map.get(pred.constant, set())
                    total = sum((len(s) for s in value_map.values()))
                    if total > 0:
                        freq = len(node_set) / total
                        return 1.0 - freq
            return 0.5
        elif isinstance(pred, PairWisePredicate):
            return 0.7
        return 0.3

    def _get_omr_signature(self, omr_rule: OMRule) -> str:
        import hashlib
        pattern = omr_rule.pattern
        pattern_key = f'n{len(pattern.nodes)}_e{len(pattern.edges)}'
        edge_sig = '_'.join(sorted((f'{e.source_var}-{e.label}{('>' if e.is_forward else '<')}{e.target_var}' for e in pattern.edges)))
        pred_strs = sorted((str(p) for p in omr_rule.preconditions))
        pred_sig = '|'.join(pred_strs)
        full = f'{pattern_key}:{edge_sig}::{pred_sig}'
        if len(full) > 200:
            return f'{pattern_key}::{hashlib.md5(full.encode()).hexdigest()}'
        return full

    def _extract_tie_frequent_values(self, tie_rule: TIERule) -> Dict[Tuple[str, str], List[Any]]:
        grouped: Dict[Tuple[str, str], List[Any]] = {}
        for var, preds in tie_rule.get_point_wise_predicates().items():
            node = tie_rule.pattern.nodes.get(var)
            if node is None:
                continue
            for pred in preds:
                key = (node.label, pred.attribute)
                values = grouped.setdefault(key, [])
                if pred.constant not in values:
                    values.append(pred.constant)
        return grouped

    def _get_label_attrs(self, label: str) -> Optional[set]:
        label_nodes = self.graph._nodes_by_label.get(label, set())
        for nid in label_nodes:
            node = self.graph._nodes.get(nid)
            if node and isinstance(node.attributes, dict) and node.attributes:
                return set(node.attributes.keys())
        return set()

    def _get_node_attrs(self, node) -> set:
        if isinstance(node.attributes, dict):
            return set(node.attributes.keys())
        elif isinstance(node.attributes, (set, frozenset)):
            return set(node.attributes)
        return set()

    def _get_attr_type(self, attr: str, node_label: str) -> str:
        if self.attr_config is not None:
            by_label = getattr(self.attr_config, 'by_label', {})
            flat = getattr(self.attr_config, 'flat', {})
            if node_label and node_label in by_label and (attr in by_label[node_label]):
                return by_label[node_label][attr]
            if attr in flat:
                return flat[attr]
        a_lower = attr.lower()
        if a_lower == 'id' or a_lower.endswith('_id') or a_lower.startswith('id_'):
            return 'id'
        return 'category'

    def _get_cardinality(self, attr: str, node_label: str) -> int:
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

    def _get_allowed_ops(self, attr: str, node_label: str) -> dict:
        cardinality = self._get_cardinality(attr, node_label)
        attr_type = self._get_attr_type(attr, node_label)
        if attr_type == 'id':
            return {'pointwise_eq': False, 'pointwise_lt_gt': False, 'pairwise_eq': True, 'pairwise_lt_gt': False}
        if attr_type == 'temporal':
            return {'pointwise_eq': True, 'pointwise_lt_gt': True, 'pairwise_eq': True, 'pairwise_lt_gt': True}
        if attr_type == 'ordinal':
            if cardinality <= 10:
                return {'pointwise_eq': True, 'pointwise_lt_gt': True, 'pairwise_eq': True, 'pairwise_lt_gt': True}
            return {'pointwise_eq': False, 'pointwise_lt_gt': True, 'pairwise_eq': False, 'pairwise_lt_gt': True}
        return {'pointwise_eq': True, 'pointwise_lt_gt': True, 'pairwise_eq': True, 'pairwise_lt_gt': True}

    def _get_frequent_attribute_values(self, label: str, attr: str, max_samples: int, min_frequency: float=0.0) -> List[Any]:
        label_nodes = self.graph._nodes_by_label.get(label, set())
        total_nodes = len(label_nodes)
        if total_nodes == 0:
            return []
        _by_attr = getattr(self.graph, '_attr_to_values', None)
        if _by_attr is not None and attr in _by_attr:
            value_counts = {}
            for v, node_set in _by_attr[attr].items():
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
        frequent = [(value, count) for value, count in value_counts.items() if count / total_nodes >= min_frequency]
        frequent.sort(key=lambda x: x[1], reverse=True)
        return [value for value, _ in frequent[:max_samples]]

class OMRLearner:

    def __init__(self, graph, training_data, ml_cache, tie_rules, tie_metrics, dis_model, aggr_model, user_label, item_label, min_support, min_confidence, max_bridge_hops, max_alt_paths, max_predicates, dis_thresholds, aggr_thresholds, max_omr_per_tie, n_workers, time_limit, data_dir, attr_config, gini_top_k=5, omr_predicates=2, tie_predicates=1):
        self.graph = graph
        self.training_data = training_data
        self.ml_cache = ml_cache
        self.tie_rules = tie_rules
        self.tie_metrics = tie_metrics
        self.dis_model = dis_model
        self.aggr_model = aggr_model
        self.user_label = user_label
        self.item_label = item_label
        self.min_support = min_support
        self.min_confidence = min_confidence
        self.max_bridge_hops = max_bridge_hops
        self.max_alt_paths = max_alt_paths
        self.max_predicates = max_predicates
        self.dis_thresholds = dis_thresholds
        self.aggr_thresholds = aggr_thresholds
        self.max_omr_per_tie = max_omr_per_tie
        self.gini_top_k = gini_top_k
        self.omr_predicates = omr_predicates
        self.tie_predicates = tie_predicates
        self.n_workers = n_workers
        self.time_limit = time_limit
        self.data_dir = data_dir
        self.attr_config = attr_config
        self.training_index: Dict[Any, Set[Any]] = {}
        for u, v in training_data.keys():
            self.training_index.setdefault(u, set()).add(v)
        self.discovered_rules: List[OMRule] = []
        self._discovered_with_metrics: List[Tuple[OMRule, OMRMetrics]] = []
        self._saved_rule_sigs: Set[str] = set()
        self.realtime_save_dir: Optional[str] = None
        self.realtime_log_path: Optional[str] = None
        self.rule_counter = 0
        self._ensure_tie_paths_indexed()

    def _ensure_tie_paths_indexed(self) -> None:
        from data_structure import build_path_anchor_index, compute_path_anchor_signature
        anchor_index = getattr(self.graph, 'path_anchor_index', None)
        tie_templates_set: set = set()
        tie_attrs: set = set()
        for rule in self.tie_rules:
            pattern = rule.pattern
            for path in pattern.user_paths + pattern.item_paths:
                if not hasattr(path, 'edges') or not path.edges:
                    continue
                try:
                    edge_labels = tuple((e.label for e in path.edges))
                    node_labels = tuple((pattern.nodes[v].label for v in path.path_sequence))
                    is_forward = tuple((e.is_forward for e in path.edges))
                    tie_templates_set.add((edge_labels, node_labels, is_forward))
                    for var in path.path_sequence:
                        node = pattern.nodes.get(var)
                        if node is None:
                            continue
                        if hasattr(node, 'attributes') and node.attributes:
                            if isinstance(node.attributes, dict):
                                tie_attrs.update(node.attributes.keys())
                            elif isinstance(node.attributes, (set, list, frozenset)):
                                tie_attrs.update(node.attributes)
                except (KeyError, IndexError, AttributeError):
                    continue
        if not tie_templates_set:
            return
        needed_sigs: set = set()
        for el_tup, nl_tup, fw_tup in tie_templates_set:
            for step in range(len(el_tup)):
                sig = compute_path_anchor_signature(list(el_tup[:step + 1]), list(nl_tup[:step + 2]), list(fw_tup[:step + 1]))
                needed_sigs.add(sig)
        if anchor_index is not None:
            missing_sigs = needed_sigs - anchor_index.known_signatures
            if not missing_sigs:
                return
        existing_templates: set = set()
        existing_attrs: set = set()
        dataset_name = os.path.basename(os.path.normpath(self.data_dir))
        for suffix in (f'{dataset_name}_rl_patterns.pkl', f'{dataset_name}_patterns.pkl'):
            candidate = os.path.join(self.data_dir, suffix)
            if os.path.exists(candidate):
                try:
                    with open(candidate, 'rb') as f:
                        patterns = pickle.load(f)
                    for pattern in patterns[:500]:
                        for path in pattern.user_paths + pattern.item_paths:
                            if not hasattr(path, 'edges') or not path.edges:
                                continue
                            try:
                                edge_labels = tuple((e.label for e in path.edges))
                                node_labels = tuple((pattern.nodes[v].label for v in path.path_sequence))
                                is_forward = tuple((e.is_forward for e in path.edges))
                                existing_templates.add((edge_labels, node_labels, is_forward))
                                for var in path.path_sequence:
                                    node = pattern.nodes.get(var)
                                    if node and hasattr(node, 'attributes') and node.attributes:
                                        if isinstance(node.attributes, dict):
                                            existing_attrs.update(node.attributes.keys())
                                        elif isinstance(node.attributes, (set, list, frozenset)):
                                            existing_attrs.update(node.attributes)
                            except (KeyError, IndexError, AttributeError):
                                continue
                except Exception:
                    pass
                break
        combined_templates = existing_templates | tie_templates_set
        combined_attrs = sorted(existing_attrs | tie_attrs)
        if not combined_attrs:
            combined_attrs = sorted(set((a for node in self.graph._nodes.values() for a in node.attributes)))
        path_templates_list = [(list(el), list(nl), list(fw)) for el, nl, fw in combined_templates]
        t0 = time.time()
        try:
            self.graph.path_anchor_index = build_path_anchor_index(self.graph, path_templates=path_templates_list, target_attrs=combined_attrs, max_cardinality=1000)
            elapsed = time.time() - t0
        except Exception as e:
            logger.error(f'  ✗ PathAnchor-TIE build failed: {e}')
            raise RuntimeError(f'PathAnchorIndex rebuild failed — cannot guarantee predicate coverage: {e}') from e

    def setup_realtime_saving(self, dataset_name: str, base_dir: str='rules'):
        timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
        self.realtime_save_dir = os.path.join(base_dir, dataset_name, f'omr_{timestamp}')
        os.makedirs(self.realtime_save_dir, exist_ok=True)
        self.realtime_log_path = os.path.join(self.realtime_save_dir, 'mining_progress.log')
        with open(self.realtime_log_path, 'w') as f:
            f.write('OMR Rule Mining Progress Log\n')
            f.write(f'Dataset: {dataset_name}\n')
            f.write(f'Start Time: {timestamp}\n')
            f.write(f'TIE Rules: {len(self.tie_rules)}\n')
            f.write(f'{'=' * 70}\n\n')
        self.rule_counter = 0

    def _record_rule_result(self, omr_rule: OMRule, metrics: OMRMetrics) -> bool:
        pred_sig = '|'.join(sorted((str(p) for p in omr_rule.preconditions)))
        if pred_sig in self._saved_rule_sigs:
            return False
        self._saved_rule_sigs.add(pred_sig)
        self.discovered_rules.append(omr_rule)
        self._discovered_with_metrics.append((omr_rule, metrics))
        self.rule_counter += 1
        return True

    def _update_progress_log(self, coverage: float):
        if self.realtime_log_path is None:
            return
        with open(self.realtime_log_path, 'a') as f:
            ts = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
            f.write(f'[{ts}] Rules: {self.rule_counter}, Coverage: {coverage:.2f}%\n')

    def _compute_coverage(self) -> float:
        all_positive_pairs = set()
        if self.tie_metrics:
            for m in self.tie_metrics:
                if m and hasattr(m, 'positive_pivots') and m.positive_pivots:
                    all_positive_pairs.update(m.positive_pivots)
        if not all_positive_pairs:
            return 0.0
        covered_pairs = set()
        for omr_rule, omr_metrics in self._discovered_with_metrics:
            covered_pairs.update(omr_metrics.positive_pivots)
        return len(covered_pairs & all_positive_pairs) / len(all_positive_pairs) * 100

    def learn(self, convergence_threshold: float=0.8, time_limit: int=None) -> List[OMRule]:
        if time_limit is not None:
            self.time_limit = time_limit
        total_start = time.time()
        tie_rules = self.tie_rules
        n_ties = len(tie_rules)
        if self.tie_metrics:
            indexed = list(zip(range(n_ties), tie_rules, self.tie_metrics))
            indexed.sort(key=lambda x: x[2].support if x[2] else 0, reverse=True)
        else:
            indexed = [(i, r, None) for i, r in enumerate(tie_rules)]
        sorted_rules = [r for _, r, _ in indexed]
        sorted_metrics = [m for _, _, m in indexed]
        self.tie_rules = sorted_rules
        self.tie_metrics = sorted_metrics
        global _SHARED_GRAPH, _SHARED_TRAINING_DATA, _SHARED_TRAINING_INDEX
        global _SHARED_ML_CACHE, _SHARED_ATTR_CONFIG
        global _SHARED_TIE_RULES, _SHARED_TIE_METRICS
        global _SHARED_DIS_MODEL, _SHARED_AGGR_MODEL, _SHARED_MINING_CONFIG
        global _SHARED_REALTIME_SAVE_DIR
        _SHARED_GRAPH = self.graph
        _SHARED_TRAINING_DATA = self.training_data
        _SHARED_TRAINING_INDEX = self.training_index
        _SHARED_ML_CACHE = self.ml_cache
        _SHARED_ATTR_CONFIG = self.attr_config
        _SHARED_TIE_RULES = sorted_rules
        _SHARED_TIE_METRICS = sorted_metrics
        _SHARED_DIS_MODEL = self.dis_model
        _SHARED_AGGR_MODEL = self.aggr_model
        _SHARED_REALTIME_SAVE_DIR = self.realtime_save_dir
        _SHARED_MINING_CONFIG = {'dis_thresholds': self.dis_thresholds, 'aggr_thresholds': self.aggr_thresholds, 'max_bridge_hops': self.max_bridge_hops, 'max_alt_paths': self.max_alt_paths, 'max_predicates': self.max_predicates, 'min_support': self.min_support, 'min_confidence': self.min_confidence, 'max_omr_per_tie': self.max_omr_per_tie, 'gini_top_k': self.gini_top_k, 'omr_predicates': self.omr_predicates, 'tie_predicates': self.tie_predicates}
        gc.disable()
        gc.freeze()
        mp_ctx = _get_parallel_context()
        pool = mp_ctx.Pool(processes=self.n_workers, initializer=_init_omr_worker, initargs=(self.graph, self.training_data, self.training_index, self.ml_cache, self.attr_config, sorted_rules, sorted_metrics, self.dis_model, self.aggr_model, dict(_SHARED_MINING_CONFIG), self.realtime_save_dir))
        mining_start = time.time()
        ties_processed = 0
        current_coverage = 0.0
        last_coverage_check = 0
        coverage_check_interval = max(1, n_ties // 20)
        try:
            async_results = {}
            for idx in range(n_ties):
                ar = pool.apply_async(_worker_mine_omr, (idx,))
                async_results[idx] = ar
            with tqdm(total=n_ties, desc='    Mining OMR rules') as pbar:
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
                        ties_processed += 1
                        pbar.update(1)
                        try:
                            worker_result = ar.get(timeout=5)
                        except Exception as e:
                            logger.error(f'    Worker error (TIE {idx}): {e}')
                            continue
                        if worker_result:
                            for omr_rule, metrics in worker_result:
                                if self._record_rule_result(omr_rule, metrics):
                                    last_coverage_check += 1
                    if last_coverage_check >= coverage_check_interval and len(self.discovered_rules) > 0:
                        current_coverage = self._compute_coverage()
                        last_coverage_check = 0
                        elapsed = time.time() - mining_start
                        if self.realtime_save_dir:
                            self._update_progress_log(current_coverage)
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
            _SHARED_ATTR_CONFIG = None
            _SHARED_TIE_RULES = None
            _SHARED_TIE_METRICS = None
            _SHARED_DIS_MODEL = None
            _SHARED_AGGR_MODEL = None
            _SHARED_MINING_CONFIG = None
            _SHARED_REALTIME_SAVE_DIR = None
        coverage = self._compute_coverage()
        total_time = time.time() - total_start
        return self.discovered_rules
