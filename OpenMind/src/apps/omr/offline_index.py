from dataclasses import dataclass, field
from typing import Any, Dict, Optional, Set, Tuple
import logging
import time
from data_structure import canonical_tie_rule_signature
logger = logging.getLogger(__name__)
_ALT_STAR_FALLBACK_MAX = 5000

@dataclass
class OfflineIndex:
    item_star: Dict[int, Dict[Any, Optional[Dict[str, Set]]]] = field(default_factory=dict)
    alt_star: Dict[int, Dict[Any, Optional[Dict[str, Set]]]] = field(default_factory=dict)
    bridge_idx: Dict[int, Any] = field(default_factory=dict)
    dis_scores: Dict[Tuple, float] = field(default_factory=dict)
    dis_pass: Dict[int, Dict[Any, frozenset]] = field(default_factory=dict)
    aggr_scores: Dict[Any, float] = field(default_factory=dict)
    aggr_pass: Dict[int, frozenset] = field(default_factory=dict)
    _omr_id_to_idx: Dict[int, int] = field(default_factory=dict)
    build_time: float = 0.0
    n_item_star_entries: int = 0
    n_alt_star_entries: int = 0
    n_dis_pass_entries: int = 0
    n_aggr_pass_entries: int = 0

def build_offline_index(graph, tie_rules: list, omr_rules: list, dis_model=None, aggr_model=None) -> OfflineIndex:
    from tie_matcher import _match_star_paths, _prefilter_by_attribute_index, build_star_match_context
    from omr_matcher import _build_edge_lookups, _build_prefilter_cache
    t_start = time.time()
    compact_graph = getattr(graph, '_compact', None)
    idx = OfflineIndex()

    def _build_effective_bridge_index(omr_rule, origin_ids: Set[Any]):
        from omr_miner import BridgeReachabilityIndex
        omr_pat = omr_rule.pattern
        y0 = omr_pat.anchor_center
        w0 = omr_pat.alt_center
        rule_pw = omr_rule.get_point_wise_predicates()
        for bp_idx, bridge_path in enumerate(omr_pat.bridge_paths):
            origin = omr_pat.bridge_origins[bp_idx]
            if origin != y0:
                continue
            bp_seq = getattr(bridge_path, 'path_sequence', [])
            if not bp_seq:
                continue
            if bp_seq[-1] != w0:
                continue
            raw_idx = BridgeReachabilityIndex.build(graph, bridge_path, origin_ids, w_var=w0, pw_predicates=rule_pw)
            return (raw_idx, {})
        return (None, {})
    idx._omr_id_to_idx = {id(r): i for i, r in enumerate(omr_rules)}
    for tie_idx, tie_rule in enumerate(tie_rules):
        pattern = tie_rule.pattern
        pw_preds = tie_rule.get_point_wise_predicates()
        y0 = pattern.item_center
        item_paths = pattern.item_paths
        if not item_paths:
            idx.item_star[tie_idx] = {}
            continue
        item_edge_lookups = _build_edge_lookups(item_paths)
        pivot_vars = {pattern.user_center, y0}
        prefilter_cache = _build_prefilter_cache(graph, pattern, pw_preds, pivot_vars)
        item_star_ctx = build_star_match_context(graph, pattern, item_paths, item_edge_lookups, pw_preds, prefilter_cache, compact_graph)
        y0_label = pattern.nodes[y0].label
        all_items = graph._nodes_by_label.get(y0_label, set())
        y0_preds = pw_preds.get(y0, [])
        cache: Dict[Any, Optional[Dict[str, Set]]] = {}
        n_match = 0
        for v in all_items:
            if y0_preds:
                v_node = graph.get_node(v)
                if not v_node or not all((p.evaluate(v_node) for p in y0_preds)):
                    cache[v] = None
                    continue
            v_match = _match_star_paths(graph, pattern, v, item_paths, pw_preds, item_edge_lookups, prefilter_cache, compact_graph, star_match_ctx=item_star_ctx)
            cache[v] = v_match
            if v_match is not None:
                n_match += 1
        idx.item_star[tie_idx] = cache
        idx.n_item_star_entries += len(cache)
    _tie_id_to_idx = {id(r): i for i, r in enumerate(tie_rules)}
    _tie_sig_to_idx = {canonical_tie_rule_signature(r): i for i, r in enumerate(tie_rules)}
    for omr_idx, omr_rule in enumerate(omr_rules):
        omr_pat = omr_rule.pattern
        y0 = omr_pat.anchor_center
        w0 = omr_pat.alt_center
        dis_preds = omr_rule.get_dis_predicates()
        aggr_preds = omr_rule.get_aggr_predicates()
        parent_tie_idx = _tie_id_to_idx.get(id(omr_rule.parent_tie))
        if parent_tie_idx is None:
            parent_tie_idx = _tie_sig_to_idx.get(canonical_tie_rule_signature(omr_rule.parent_tie))
        if parent_tie_idx is None:
            continue
        parent_tie = omr_rule.parent_tie
        tie_item_cache = idx.item_star.get(parent_tie_idx, {})
        if tie_item_cache:
            v_candidates = {v for v, m in tie_item_cache.items() if m is not None}
        elif not parent_tie.pattern.item_paths:
            y0_var = parent_tie.pattern.item_center
            y0_label = parent_tie.pattern.nodes[y0_var].label
            y0_preds = parent_tie.get_point_wise_predicates().get(y0_var, [])
            if y0_preds:
                prefiltered = _prefilter_by_attribute_index(graph, y0_label, y0_preds)
                if prefiltered is not None:
                    v_candidates = set(prefiltered)
                else:
                    label_view = graph._nodes_by_label.get(y0_label, set())
                    v_candidates = {nid for nid in label_view if all((p.evaluate(graph.get_node(nid)) for p in y0_preds))}
            else:
                v_candidates = set(graph._nodes_by_label.get(y0_label, set()))
        else:
            v_candidates = set()
        if not v_candidates:
            continue
        effective_bridge_idx, precomputed_alt_cache = _build_effective_bridge_index(omr_rule, v_candidates)
        if effective_bridge_idx is not None:
            idx.bridge_idx[omr_idx] = effective_bridge_idx
        bri = idx.bridge_idx.get(omr_idx)
        all_ws: Set[Any] = set()
        if bri:
            for v in v_candidates:
                all_ws |= bri.get_reachable(v)
        alt_star = omr_pat.alt_star
        alt_paths = alt_star.paths if hasattr(alt_star, 'paths') else []
        if precomputed_alt_cache:
            idx.alt_star[omr_idx] = precomputed_alt_cache
            idx.n_alt_star_entries += len(precomputed_alt_cache)
        elif alt_paths:
            alt_seed_ws = set(all_ws)
            if not alt_seed_ws:
                w0_label = omr_pat.nodes[w0].label if w0 in omr_pat.nodes else getattr(omr_pat, 'alt_center_label', None)
                if w0_label is not None:
                    label_ws = graph._nodes_by_label.get(w0_label, set())
                    if len(label_ws) <= _ALT_STAR_FALLBACK_MAX:
                        alt_seed_ws = set(label_ws)
                        logger.debug("  [OfflineIndex] OMR-%d: alt-star fallback over all %d '%s' items", omr_idx, len(alt_seed_ws), w0_label)
            if alt_seed_ws:
                omr_pw = omr_rule.get_point_wise_predicates()
                alt_edge_lookups = _build_edge_lookups(alt_paths)
                pivot_vars = {omr_pat.user_center, omr_pat.anchor_center, w0}
                alt_prefilter = _build_prefilter_cache(graph, omr_pat, omr_pw, pivot_vars)
                alt_star_ctx = build_star_match_context(graph, omr_pat, alt_paths, alt_edge_lookups, omr_pw, alt_prefilter, compact_graph)
                alt_cache: Dict[Any, Optional[Dict[str, Set]]] = {}
                n_alt_match = 0
                for w in alt_seed_ws:
                    w_match = _match_star_paths(graph, omr_pat, w, alt_paths, omr_pw, alt_edge_lookups, alt_prefilter, compact_graph, star_match_ctx=alt_star_ctx)
                    alt_cache[w] = w_match
                    if w_match is not None:
                        n_alt_match += 1
                idx.alt_star[omr_idx] = alt_cache
                idx.n_alt_star_entries += len(alt_cache)
            else:
                idx.alt_star[omr_idx] = {}
        else:
            idx.alt_star[omr_idx] = {}
        if bri and dis_model is not None:
            dis_pass_per_v: Dict[Any, frozenset] = {}
            for v in v_candidates:
                ws = bri.get_reachable(v)
                if ws:
                    scores = dis_model.compute_batch(v, ws)
                    for w, s in scores.items():
                        idx.dis_scores[v, w] = s
                    if dis_preds:
                        valid_ws = {w for w, score in scores.items() if all((dp.evaluate(score) for dp in dis_preds))}
                        if valid_ws:
                            dis_pass_per_v[v] = frozenset(valid_ws)
            if dis_pass_per_v:
                idx.dis_pass[omr_idx] = dis_pass_per_v
                idx.n_dis_pass_entries += sum((len(ws) for ws in dis_pass_per_v.values()))
        if aggr_model is not None and aggr_preds:
            w0_label = omr_pat.nodes[w0].label if w0 in omr_pat.nodes else getattr(omr_pat, 'alt_center_label', None)
            if w0_label is not None:
                label_ws = graph._nodes_by_label.get(w0_label, set())
                if label_ws:
                    valid_ws = set()
                    for w in label_ws:
                        score = idx.aggr_scores.get(w)
                        if score is None:
                            score = aggr_model.compute(w)
                            idx.aggr_scores[w] = score
                        if all((ap.evaluate(score) for ap in aggr_preds)):
                            valid_ws.add(w)
                    if valid_ws:
                        idx.aggr_pass[omr_idx] = frozenset(valid_ws)
                        idx.n_aggr_pass_entries += len(valid_ws)
    if aggr_model is not None:
        all_item_ids: Set[Any] = set()
        for tie_rule in tie_rules:
            y0_label = tie_rule.pattern.nodes[tie_rule.pattern.item_center].label
            all_item_ids |= graph._nodes_by_label.get(y0_label, set())
        for item_id in all_item_ids:
            idx.aggr_scores[item_id] = aggr_model.compute(item_id)
    dt = time.time() - t_start
    idx.build_time = dt
    return idx
