from typing import Dict, Set, Tuple, List, Optional, Any
import logging
import os
import time
from data_structure import Graph, DualStarPattern, Path, PointWisePredicate, PairWisePredicate, EdgePredicate, TIERule, RuleMetrics, PairWiseAnchorInfo, _safe_compare_predicate_values, _normalize_predicate_equality_value
from models import MLPredicateCache
from _compact_graph import CompactGraph
try:
    from _fast_match import match_path_compact_fast as _cy_match, match_star_paths_batch_fast as _cy_match_star_batch, verify_pivot_pairwise_binding_fast as _cy_verify_pivot_pairwise_binding
    _HAS_FAST_MATCH = True
except ImportError:
    _HAS_FAST_MATCH = False
    _cy_match_star_batch = None
    _cy_verify_pivot_pairwise_binding = None
_PREFILTER_NA = '__PREFILTER_NOT_APPLICABLE__'
_LOGGER = logging.getLogger(__name__)
__all__ = ['TIERule', 'RuleMetrics', 'PairWiseAnchorInfo', 'get_pivot_candidates', 'match_path_compact', 'build_star_match_context', '_match_star_paths', 'verify_variable_predicates', 'compute_rule_metrics', 'apply_path_anchor_filters', '_check_conclusion_edge', '_get_prefix_sig_for_var']

def _debug_enabled(debug: bool) -> bool:
    return debug or os.environ.get('DEBUG_MATCHER') == '1'

def _get_graph_matcher_cache(graph: Graph) -> Dict[str, Any]:
    cache = getattr(graph, '_tie_matcher_runtime_cache', None)
    if cache is None:
        cache = {}
        setattr(graph, '_tie_matcher_runtime_cache', cache)
    return cache

def _get_rule_matcher_cache(rule: 'TIERule') -> Dict[str, Any]:
    cache = getattr(rule, '_tie_matcher_runtime_cache', None)
    if cache is None:
        cache = {}
        setattr(rule, '_tie_matcher_runtime_cache', cache)
    return cache

def _get_cached_training_index(graph: Graph, training_data: Dict[Tuple[Any, Any], bool]) -> Dict[Any, Set[Any]]:
    graph_cache = _get_graph_matcher_cache(graph)
    training_index_cache = graph_cache.setdefault('training_index_cache', {})
    cache_key = id(training_data)
    cached = training_index_cache.get(cache_key)
    if cached is not None and cached[0] is training_data:
        return cached[1]
    training_index: Dict[Any, Set[Any]] = {}
    for u, v in training_data.keys():
        training_index.setdefault(u, set()).add(v)
    training_index_cache[cache_key] = (training_data, training_index)
    return training_index

def _execute_comparison(value1: Any, value2: Any, operator) -> bool:
    from data_structure import ComparisonOp
    if operator == ComparisonOp.EQ:
        return value1 == value2
    elif operator == ComparisonOp.NE:
        return value1 != value2
    elif operator == ComparisonOp.LT:
        return value1 < value2
    elif operator == ComparisonOp.LE:
        return value1 <= value2
    elif operator == ComparisonOp.GT:
        return value1 > value2
    elif operator == ComparisonOp.GE:
        return value1 >= value2
    raise ValueError(f'Unknown operator: {operator}')

def _safe_compare(value1: Any, value2: Any, operator) -> Optional[bool]:
    from data_structure import ComparisonOp
    if value1 is None or value2 is None:
        if operator in [ComparisonOp.EQ, ComparisonOp.NE]:
            result = value1 == value2
            return result if operator == ComparisonOp.EQ else not result
        return None
    if type(value1) == type(value2):
        try:
            return _execute_comparison(value1, value2, operator)
        except TypeError:
            return None
    if isinstance(value1, (int, float)) and isinstance(value2, (int, float)):
        try:
            return _execute_comparison(value1, value2, operator)
        except TypeError:
            return None
    if isinstance(value1, (int, float, str)) and isinstance(value2, (int, float, str)):
        try:
            num1 = float(value1)
            num2 = float(value2)
            return _execute_comparison(num1, num2, operator)
        except (ValueError, TypeError):
            if operator in [ComparisonOp.EQ, ComparisonOp.NE]:
                try:
                    str1 = str(value1)
                    str2 = str(value2)
                    result = str1 == str2
                    return result if operator == ComparisonOp.EQ else not result
                except:
                    return None
            else:
                return None
    if operator in [ComparisonOp.EQ, ComparisonOp.NE]:
        try:
            str1 = str(value1)
            str2 = str(value2)
            result = str1 == str2
            return result if operator == ComparisonOp.EQ else not result
        except:
            return None
    return None

def get_pivot_candidates(graph: Graph, pattern: DualStarPattern, point_wise_preds: Dict[str, List[PointWisePredicate]], debug: bool=False, timing_out: Optional[Dict[str, float]]=None) -> Tuple[Set[Any], Set[Any]]:
    logger = _LOGGER

    def _record(name: str, started_at: float) -> None:
        if timing_out is not None:
            timing_out[name] = timing_out.get(name, 0.0) + (time.perf_counter() - started_at)
    user_label = pattern.nodes[pattern.user_center].label
    item_label = pattern.nodes[pattern.item_center].label
    _t_phase = time.perf_counter()
    user_candidates = graph.get_node_ids_by_label_view(user_label)
    item_candidates = graph.get_node_ids_by_label_view(item_label)
    _record('label_lookup', _t_phase)
    if pattern.user_center in point_wise_preds:
        user_preds = point_wise_preds[pattern.user_center]
        before = len(user_candidates)
        _t_phase = time.perf_counter()
        prefiltered = _prefilter_by_attribute_index(graph, user_label, user_preds)
        if prefiltered is not None:
            user_candidates = user_candidates & prefiltered
        else:
            filtered_users = set()
            for uid in user_candidates:
                node = graph.get_node(uid)
                if node and all((pred.evaluate(node) for pred in user_preds)):
                    filtered_users.add(uid)
            user_candidates = filtered_users
        _record('user_pointwise_filter', _t_phase)
    if pattern.item_center in point_wise_preds:
        item_preds = point_wise_preds[pattern.item_center]
        before = len(item_candidates)
        _t_phase = time.perf_counter()
        prefiltered = _prefilter_by_attribute_index(graph, item_label, item_preds)
        if prefiltered is not None:
            item_candidates = item_candidates & prefiltered
        else:
            filtered_items = set()
            for iid in item_candidates:
                node = graph.get_node(iid)
                if node and all((pred.evaluate(node) for pred in item_preds)):
                    filtered_items.add(iid)
            item_candidates = filtered_items
        _record('item_pointwise_filter', _t_phase)

    def _required_groups(var_name: str) -> Set[Tuple[str, bool]]:
        groups: Set[Tuple[str, bool]] = set()
        for edge in pattern.edges:
            if edge.source_var == var_name:
                groups.add((edge.label, bool(edge.is_forward)))
            if edge.target_var == var_name:
                groups.add((edge.label, not bool(edge.is_forward)))
        return groups

    def _filter_by_groups(candidates: Set[Any], groups: Set[Tuple[str, bool]]) -> Set[Any]:
        surviving = set(candidates)
        for edge_label, needs_outgoing in groups:
            supported = graph.get_source_nodes_of_edge(edge_label) if needs_outgoing else graph.get_target_nodes_of_edge(edge_label)
            surviving &= supported
            if not surviving:
                break
        return surviving
    user_groups = _required_groups(pattern.user_center)
    if user_groups:
        before = len(user_candidates)
        _t_phase = time.perf_counter()
        user_candidates = _filter_by_groups(user_candidates, user_groups)
        _record('user_structural_filter', _t_phase)
    item_groups = _required_groups(pattern.item_center)
    if item_groups:
        before = len(item_candidates)
        _t_phase = time.perf_counter()
        item_candidates = _filter_by_groups(item_candidates, item_groups)
        _record('item_structural_filter', _t_phase)
    if pattern.user_center not in point_wise_preds:
        user_candidates = set(user_candidates)
    if pattern.item_center not in point_wise_preds:
        item_candidates = set(item_candidates)
    return (user_candidates, item_candidates)

def _prefilter_by_attribute_index(graph: Graph, node_label: str, predicates: List[PointWisePredicate]) -> Optional[Set[Any]]:
    from data_structure import ComparisonOp
    if not predicates:
        return None
    if not hasattr(graph, 'attr_value_index') or not graph.attr_value_index:
        return None
    attr_owner = getattr(graph, 'attr_owner_label', {})
    attr_to_values = getattr(graph, '_attr_to_values', None)
    if attr_to_values is None:
        attr_to_values = {}
        for (attr_name, attr_value), node_ids in graph.attr_value_index.items():
            if attr_value is None:
                continue
            attr_to_values.setdefault(attr_name, {})[attr_value] = node_ids
        graph._attr_to_values = attr_to_values
    label_set = None
    handled_any = False
    fallback_preds: List[PointWisePredicate] = []
    result: Optional[Set[Any]] = None

    def _restrict_to_label(nodes: Set[Any], attr_name: str) -> Set[Any]:
        nonlocal label_set
        if attr_owner.get(attr_name) == node_label:
            return set(nodes)
        if label_set is None:
            label_set = graph._nodes_by_label.get(node_label, set())
        return set(nodes) & label_set
    for pred in predicates:
        candidates: Optional[Set[Any]] = None
        if pred.operator == ComparisonOp.EQ:
            nodes_with_value = graph.attr_value_index.get((pred.attribute, pred.constant))
            candidates = set() if nodes_with_value is None else _restrict_to_label(nodes_with_value, pred.attribute)
            handled_any = True
        else:
            value_groups = attr_to_values.get(pred.attribute)
            if value_groups:
                matched_nodes: Set[Any] = set()
                for attr_value, node_ids in value_groups.items():
                    if attr_value is None:
                        continue
                    if _safe_compare(attr_value, pred.constant, pred.operator) is True:
                        matched_nodes |= node_ids
                candidates = _restrict_to_label(matched_nodes, pred.attribute)
                handled_any = True
        if candidates is None:
            fallback_preds.append(pred)
            continue
        result = candidates if result is None else result & candidates
        if not result:
            return result
    if not handled_any:
        return None
    if result is None:
        return set()
    if fallback_preds:
        result = {nid for nid in result if all((p.evaluate(graph.get_node(nid)) for p in fallback_preds))}
    return result

def _verify_same_variable_predicate(graph: Graph, candidates: Set[Any], pred: PairWisePredicate) -> bool:
    for node_id in candidates:
        node = graph.get_node(node_id)
        if node is None:
            continue
        if pred.evaluate(node, node):
            return True
    return False

def _narrow_same_variable(graph: Graph, candidates: Set[Any], pred: PairWisePredicate) -> Tuple[Set[Any], Set[Any]]:
    filtered = set()
    for node_id in candidates:
        node = graph.get_node(node_id)
        if node is None:
            continue
        try:
            if pred.evaluate(node, node):
                filtered.add(node_id)
        except (TypeError, ValueError):
            continue
    return (filtered, filtered)

def match_path_compact(graph: Graph, path: Path, pivot_node: Any, pattern: DualStarPattern, point_wise_preds: Dict[str, List[PointWisePredicate]], edge_lookup: Optional[Dict[Tuple[str, str], Tuple[str, bool]]]=None, debug: bool=False, bidirectional: bool=True, prefilter_cache: Optional[Dict[str, Set[Any]]]=None) -> Optional[Dict[str, Set[Any]]]:
    debug = _debug_enabled(debug)
    logger = _LOGGER
    path_seq = path.path_sequence
    if debug:
        logger.debug('[match_path_compact] Starting path match')
        logger.debug(f'  Path: {' -> '.join(path_seq)}')
        logger.debug(f'  Pivot: {pivot_node}')
        logger.debug(f'  Edge lookup size: {(len(edge_lookup) if edge_lookup else 0)}')
    if not path_seq:
        return {}
    if len(path_seq) == 1:
        return {path_seq[0]: {pivot_node}}
    if edge_lookup is None:
        edge_lookup = {}
        for edge in path.edges:
            edge_lookup[edge.source_var, edge.target_var] = (edge.label, edge.is_forward)
    result = {path_seq[0]: {pivot_node}}
    _edge_tgt_label = getattr(graph, 'edge_target_label', {}) or {}
    _edge_src_label = getattr(graph, 'edge_source_label', {}) or {}
    for i in range(len(path_seq) - 1):
        current_var = path_seq[i]
        next_var = path_seq[i + 1]
        edge_info = edge_lookup.get((current_var, next_var))
        if edge_info is None:
            return None
        edge_label, is_forward = edge_info
        next_label = pattern.nodes[next_var].label
        next_preds = point_wise_preds.get(next_var, [])
        next_candidates_prefiltered = None
        if next_preds:
            if prefilter_cache is not None and next_var in prefilter_cache:
                cached = prefilter_cache[next_var]
                next_candidates_prefiltered = None if cached is _PREFILTER_NA else cached
            else:
                next_candidates_prefiltered = _prefilter_by_attribute_index(graph, next_label, next_preds)
            if debug and next_candidates_prefiltered is not None:
                logger.debug(f'  Step {i}: Prefilter for {next_var} returned {len(next_candidates_prefiltered)} candidates')
        current_candidates = result[current_var]
        next_candidates = set()
        if is_forward:
            skip_label_check = _edge_tgt_label.get(edge_label) == next_label
        else:
            skip_label_check = _edge_src_label.get(edge_label) == next_label
        for current_node_id in current_candidates:
            if is_forward:
                neighbors = graph.iter_out_neighbor_nodes(current_node_id, edge_label)
            else:
                neighbors = graph.iter_in_neighbor_nodes(current_node_id, edge_label)
            for neighbor_node in neighbors:
                nid = neighbor_node.node_id
                if not skip_label_check and neighbor_node.label != next_label:
                    continue
                if next_candidates_prefiltered is not None:
                    if nid not in next_candidates_prefiltered:
                        continue
                elif next_preds and (not all((pred.evaluate(neighbor_node) for pred in next_preds))):
                    continue
                next_candidates.add(nid)
        if not next_candidates:
            if debug:
                logger.debug(f'  Step {i}: {current_var} -> {next_var} via {edge_label} => EMPTY (match failed)')
            return None
        if debug:
            logger.debug(f'  Step {i}: {current_var} -> {next_var} via {edge_label}')
            logger.debug(f'    Current candidates: {len(current_candidates)}')
            logger.debug(f'    Next candidates: {len(next_candidates)}')
        result[next_var] = next_candidates
    if debug:
        logger.debug('[match_path_compact] Result: SUCCESS')
        logger.debug(f'  Variables matched: {list(result.keys())}')
    return result

def compute_compact_match(graph: Graph, pattern: DualStarPattern, pivot_u: Any, pivot_v: Any, point_wise_preds: Dict[str, List[PointWisePredicate]], prefilter_cache: Optional[Dict[str, Set[Any]]]=None, debug: bool=False) -> Optional[Dict[str, Set[Any]]]:
    all_paths = pattern.user_paths + pattern.item_paths
    edge_lookups = {}
    for idx, path in enumerate(all_paths):
        edge_lookups[idx] = {(edge.source_var, edge.target_var): (edge.label, edge.is_forward) for edge in path.edges}
    result = {pattern.user_center: {pivot_u}, pattern.item_center: {pivot_v}}
    for idx, path in enumerate(pattern.user_paths):
        path_match = match_path_compact(graph, path, pivot_u, pattern, point_wise_preds, edge_lookup=edge_lookups[idx], prefilter_cache=prefilter_cache)
        if path_match is None:
            return None
        for var, candidates in path_match.items():
            if var in result:
                result[var] = result[var] & candidates
                if not result[var]:
                    return None
            else:
                result[var] = candidates
    user_path_count = len(pattern.user_paths)
    for idx, path in enumerate(pattern.item_paths):
        path_match = match_path_compact(graph, path, pivot_v, pattern, point_wise_preds, edge_lookup=edge_lookups[user_path_count + idx], prefilter_cache=prefilter_cache)
        if path_match is None:
            return None
        for var, candidates in path_match.items():
            if var in result:
                result[var] = result[var] & candidates
                if not result[var]:
                    return None
            else:
                result[var] = candidates
    if any((len(candidates) == 0 for candidates in result.values())):
        return None
    return result

def _verify_equality_predicate(graph: Graph, candidates_z1: Set[Any], candidates_z2: Set[Any], pred: PairWisePredicate) -> bool:
    z2_attr_map = {}
    for n2_id in candidates_z2:
        n2 = graph.get_node(n2_id)
        if n2 and pred.attr2 in n2.attributes:
            value = n2.attributes[pred.attr2]
            if value is None:
                continue
            value = _normalize_predicate_equality_value(value)
            if value not in z2_attr_map:
                z2_attr_map[value] = []
            z2_attr_map[value].append(n2_id)
    for n1_id in candidates_z1:
        n1 = graph.get_node(n1_id)
        if n1 and pred.attr1 in n1.attributes:
            value = n1.attributes[pred.attr1]
            if value is not None and _normalize_predicate_equality_value(value) in z2_attr_map:
                return True
    return False

def _verify_equality_predicate_optimized(graph: Graph, candidates_z1: Set[Any], candidates_z2: Set[Any], pred: PairWisePredicate) -> bool:
    if not candidates_z1 or not candidates_z2:
        return False
    vals1 = graph.extract_attributes_batch(candidates_z1, pred.attr1)
    vals2 = graph.extract_attributes_batch(candidates_z2, pred.attr2)
    if not vals1 or not vals2:
        return False
    normalized2 = {_normalize_predicate_equality_value(value) for value in vals2.values() if value is not None}
    if not normalized2:
        return False
    for value in vals1.values():
        if value is None:
            continue
        if _normalize_predicate_equality_value(value) in normalized2:
            return True
    return False

def _verify_comparison_predicate(graph: Graph, candidates_z1: Set[Any], candidates_z2: Set[Any], pred: PairWisePredicate) -> bool:
    from data_structure import ComparisonOp
    if not candidates_z1 or not candidates_z2:
        return False
    z1_values = []
    for n1_id in candidates_z1:
        n1 = graph.get_node(n1_id)
        if n1 and pred.attr1 in n1.attributes:
            value = n1.attributes[pred.attr1]
            if value is not None:
                try:
                    z1_values.append(value)
                except:
                    pass
    z2_values = []
    for n2_id in candidates_z2:
        n2 = graph.get_node(n2_id)
        if n2 and pred.attr2 in n2.attributes:
            value = n2.attributes[pred.attr2]
            if value is not None:
                try:
                    z2_values.append(value)
                except:
                    pass
    if not z1_values or not z2_values:
        return False
    try:
        z1_values.sort()
        z2_values.sort()
    except TypeError:
        return _verify_predicate_fallback_safe(graph, candidates_z1, candidates_z2, pred)
    except:
        return _verify_predicate_fallback_safe(graph, candidates_z1, candidates_z2, pred)
    if pred.operator == ComparisonOp.LT:
        return z1_values[0] < z2_values[-1]
    elif pred.operator == ComparisonOp.LE:
        return z1_values[0] <= z2_values[-1]
    elif pred.operator == ComparisonOp.GT:
        return z1_values[-1] > z2_values[0]
    elif pred.operator == ComparisonOp.GE:
        return z1_values[-1] >= z2_values[0]
    return False

def _verify_ne_predicate(graph: Graph, candidates_z1: Set[Any], candidates_z2: Set[Any], pred: PairWisePredicate) -> bool:
    vals1 = set()
    for nid in candidates_z1:
        node = graph.get_node(nid)
        if node and pred.attr1 in node.attributes:
            v = node.attributes[pred.attr1]
            if v is not None:
                vals1.add(_normalize_predicate_equality_value(v))
                if len(vals1) > 1:
                    for nid2 in candidates_z2:
                        node2 = graph.get_node(nid2)
                        if node2 and pred.attr2 in node2.attributes and (node2.attributes[pred.attr2] is not None):
                            return True
                    return False
    if not vals1:
        return False
    sole_val1 = next(iter(vals1))
    for nid in candidates_z2:
        node = graph.get_node(nid)
        if node and pred.attr2 in node.attributes:
            v = node.attributes[pred.attr2]
            if v is not None and _normalize_predicate_equality_value(v) != sole_val1:
                return True
    return False

def _verify_predicate_fallback_safe(graph: Graph, candidates_z1: Set[Any], candidates_z2: Set[Any], pred: PairWisePredicate) -> bool:
    if not candidates_z1 or not candidates_z2:
        return False
    if len(candidates_z1) > len(candidates_z2):
        outer_set = candidates_z2
        inner_set = candidates_z1
        outer_is_z1 = False
    else:
        outer_set = candidates_z1
        inner_set = candidates_z2
        outer_is_z1 = True
    for outer_node_id in outer_set:
        outer_node = graph.get_node(outer_node_id)
        if outer_node is None:
            continue
        for inner_node_id in inner_set:
            inner_node = graph.get_node(inner_node_id)
            if inner_node is None:
                continue
            if outer_is_z1:
                n1 = outer_node
                n2 = inner_node
            else:
                n1 = inner_node
                n2 = outer_node
            if pred.attr1 not in n1.attributes:
                continue
            if pred.attr2 not in n2.attributes:
                continue
            value1 = n1.attributes[pred.attr1]
            value2 = n2.attributes[pred.attr2]
            result = _safe_compare(value1, value2, pred.operator)
            if result is True:
                return True
    return False

def _verify_predicate_direct_enumeration(graph: Graph, candidates_z1: Set[Any], candidates_z2: Set[Any], pred: PairWisePredicate) -> bool:
    for node1_id in candidates_z1:
        node1 = graph.get_node(node1_id)
        if not node1 or pred.attr1 not in node1.attributes:
            continue
        value1 = node1.attributes[pred.attr1]
        for node2_id in candidates_z2:
            node2 = graph.get_node(node2_id)
            if not node2 or pred.attr2 not in node2.attributes:
                continue
            value2 = node2.attributes[pred.attr2]
            result = _safe_compare(value1, value2, pred.operator)
            if result is True:
                return True
    return False

def verify_variable_predicates(graph: Graph, compact_match: Dict[str, Set[Any]], variable_preds: List[PairWisePredicate], debug: bool=False) -> bool:
    from data_structure import ComparisonOp
    logger = _LOGGER
    debug = _debug_enabled(debug)
    if debug:
        logger.debug(f'[verify_variable_predicates] Checking {len(variable_preds)} predicates')
    working_match = {k: v.copy() for k, v in compact_match.items()}
    variable_preds = sorted(variable_preds, key=lambda p: min(len(working_match.get(p.var1, set())), len(working_match.get(p.var2, set()))))
    if debug:
        selectivity_info = [(p.var1, p.var2, min(len(working_match.get(p.var1, set())), len(working_match.get(p.var2, set())))) for p in variable_preds]
        logger.debug(f'  Predicate order (by selectivity): {selectivity_info}')
    for idx, pred in enumerate(variable_preds):
        candidates_z1 = working_match.get(pred.var1, set())
        candidates_z2 = working_match.get(pred.var2, set())
        if not candidates_z1 or not candidates_z2:
            if debug:
                logger.debug(f'  [{idx + 1}/{len(variable_preds)}] {pred} => FAIL (empty candidates)')
            return False
        if pred.var1 == pred.var2:
            strategy = 'same_var'
        elif len(candidates_z1) <= 5 or len(candidates_z2) <= 5:
            strategy = 'direct_enum'
        elif pred.operator == ComparisonOp.EQ:
            strategy = 'equality_optimized'
        elif pred.operator == ComparisonOp.NE:
            strategy = 'ne_optimized'
        elif pred.operator in [ComparisonOp.LT, ComparisonOp.LE, ComparisonOp.GT, ComparisonOp.GE]:
            strategy = 'comparison_optimized'
        else:
            strategy = 'fallback'
        if debug:
            logger.debug(f'  [{idx + 1}/{len(variable_preds)}] {pred}')
            logger.debug(f'    |C({pred.var1})| = {len(candidates_z1)}, |C({pred.var2})| = {len(candidates_z2)}')
            logger.debug(f'    Strategy: {strategy}')
        if strategy == 'same_var':
            result = _verify_same_variable_predicate(graph, candidates_z1, pred)
            if debug:
                logger.debug(f'    Result: {result}')
            if not result:
                return False
        elif strategy == 'direct_enum':
            result = _verify_predicate_direct_enumeration(graph, candidates_z1, candidates_z2, pred)
            if debug:
                logger.debug(f'    Result: {result}')
            if not result:
                return False
        elif strategy == 'equality_optimized':
            result = _verify_equality_predicate_optimized(graph, candidates_z1, candidates_z2, pred)
            if debug:
                logger.debug(f'    Result: {result}')
            if not result:
                return False
        elif strategy == 'ne_optimized':
            result = _verify_ne_predicate(graph, candidates_z1, candidates_z2, pred)
            if debug:
                logger.debug(f'    Result: {result}')
            if not result:
                return False
        elif strategy == 'comparison_optimized':
            result = _verify_comparison_predicate(graph, candidates_z1, candidates_z2, pred)
            if debug:
                logger.debug(f'    Result: {result}')
            if not result:
                return False
        else:
            result = _verify_predicate_fallback_safe(graph, candidates_z1, candidates_z2, pred)
            if debug:
                logger.debug(f'    Result: {result}')
            if not result:
                return False
        if idx < len(variable_preds) - 1 and (len(candidates_z1) > 1 or len(candidates_z2) > 1):
            narrowed1, narrowed2 = _narrow_pairwise_candidates(graph, candidates_z1, candidates_z2, pred)
            working_match[pred.var1] = narrowed1
            working_match[pred.var2] = narrowed2
            if not narrowed1 or not narrowed2:
                return False
            if debug:
                logger.debug(f'    Propagation: |C({pred.var1})| {len(candidates_z1)}->{len(narrowed1)}, |C({pred.var2})| {len(candidates_z2)}->{len(narrowed2)}')
    if debug:
        logger.debug(f'[verify_variable_predicates] All {len(variable_preds)} predicates satisfied')
    if _needs_joint_pairwise_check(variable_preds):
        return _has_joint_pairwise_witness(graph, working_match, variable_preds)
    return True

def _needs_joint_pairwise_check(variable_preds: List[PairWisePredicate]) -> bool:
    if len(variable_preds) <= 1:
        return False
    var_degree: Dict[str, int] = {}
    for pred in variable_preds:
        var_degree[pred.var1] = var_degree.get(pred.var1, 0) + 1
        var_degree[pred.var2] = var_degree.get(pred.var2, 0) + 1
    return any((degree > 1 for degree in var_degree.values()))

def _has_joint_pairwise_witness(graph: Graph, compact_match: Dict[str, Set[Any]], variable_preds: List[PairWisePredicate]) -> bool:
    involved_vars = sorted({pred.var1 for pred in variable_preds} | {pred.var2 for pred in variable_preds})
    domains: Dict[str, Tuple[Any, ...]] = {}
    for var in involved_vars:
        cands = compact_match.get(var, set())
        if not cands:
            return False
        domains[var] = tuple(sorted(cands, key=str))
    preds_by_var: Dict[str, List[PairWisePredicate]] = {}
    for pred in variable_preds:
        preds_by_var.setdefault(pred.var1, []).append(pred)
        if pred.var2 != pred.var1:
            preds_by_var.setdefault(pred.var2, []).append(pred)
    attr_cache: Dict[Tuple[Any, str], Any] = {}

    def _get_attr(node_id: Any, attr: str) -> Any:
        key = (node_id, attr)
        if key in attr_cache:
            return attr_cache[key]
        node = graph.get_node(node_id)
        value = None
        if node is not None:
            value = node.attributes.get(attr)
        attr_cache[key] = value
        return value
    assigned: Dict[str, Any] = {}

    def _pred_holds(pred: PairWisePredicate, left_id: Any, right_id: Any) -> bool:
        value1 = _get_attr(left_id, pred.attr1)
        value2 = _get_attr(right_id, pred.attr2)
        return _safe_compare(value1, value2, pred.operator) is True

    def _supports_unassigned(var: str, node_id: Any) -> bool:
        for pred in preds_by_var.get(var, []):
            if pred.var1 == pred.var2:
                if not _pred_holds(pred, node_id, node_id):
                    return False
                continue
            other_var = pred.var2 if pred.var1 == var else pred.var1
            if other_var in assigned:
                other_id = assigned[other_var]
                left_id = node_id if pred.var1 == var else other_id
                right_id = other_id if pred.var2 == other_var else node_id
                if not _pred_holds(pred, left_id, right_id):
                    return False
                continue
            has_partner = False
            for other_id in domains[other_var]:
                left_id = node_id if pred.var1 == var else other_id
                right_id = other_id if pred.var2 == other_var else node_id
                if _pred_holds(pred, left_id, right_id):
                    has_partner = True
                    break
            if not has_partner:
                return False
        return True

    def _choose_var() -> str:
        best_var = None
        best_key = None
        for var in involved_vars:
            if var in assigned:
                continue
            viable = 0
            for node_id in domains[var]:
                if _supports_unassigned(var, node_id):
                    viable += 1
            key = (viable, len(domains[var]), -len(preds_by_var.get(var, ())), var)
            if best_key is None or key < best_key:
                best_key = key
                best_var = var
        return best_var

    def _backtrack() -> bool:
        if len(assigned) == len(involved_vars):
            return True
        var = _choose_var()
        if var is None:
            return True
        for node_id in domains[var]:
            if not _supports_unassigned(var, node_id):
                continue
            assigned[var] = node_id
            if _backtrack():
                return True
            assigned.pop(var, None)
        return False
    return _backtrack()

def _narrow_pairwise_candidates(graph: Graph, candidates_z1: Set[Any], candidates_z2: Set[Any], pred: PairWisePredicate) -> Tuple[Set[Any], Set[Any]]:
    from data_structure import ComparisonOp
    if pred.var1 == pred.var2:
        return _narrow_same_variable(graph, candidates_z1, pred)
    if pred.operator == ComparisonOp.EQ:
        return _narrow_equality(graph, candidates_z1, candidates_z2, pred)
    if pred.operator in (ComparisonOp.LT, ComparisonOp.LE, ComparisonOp.GT, ComparisonOp.GE):
        return _narrow_comparison(graph, candidates_z1, candidates_z2, pred)
    if pred.operator == ComparisonOp.NE:
        return _narrow_ne(graph, candidates_z1, candidates_z2, pred)
    return _narrow_brute_force(graph, candidates_z1, candidates_z2, pred)

def _narrow_equality(graph: Graph, candidates_z1: Set[Any], candidates_z2: Set[Any], pred: PairWisePredicate) -> Tuple[Set[Any], Set[Any]]:
    val_to_z1 = {}
    for n_id in candidates_z1:
        node = graph.get_node(n_id)
        if node is None or pred.attr1 not in node.attributes:
            continue
        v = node.attributes[pred.attr1]
        if v is not None:
            val_to_z1.setdefault(_normalize_predicate_equality_value(v), set()).add(n_id)
    val_to_z2 = {}
    for n_id in candidates_z2:
        node = graph.get_node(n_id)
        if node is None or pred.attr2 not in node.attributes:
            continue
        v = node.attributes[pred.attr2]
        if v is not None:
            val_to_z2.setdefault(_normalize_predicate_equality_value(v), set()).add(n_id)
    common_vals = set(val_to_z1.keys()) & set(val_to_z2.keys())
    filtered1 = set()
    filtered2 = set()
    for cv in common_vals:
        filtered1.update(val_to_z1[cv])
        filtered2.update(val_to_z2[cv])
    return (filtered1, filtered2)

def _narrow_ne(graph: Graph, candidates_z1: Set[Any], candidates_z2: Set[Any], pred: PairWisePredicate) -> Tuple[Set[Any], Set[Any]]:
    z2_vals = {}
    z2_distinct = set()
    z2_with_attr = set()
    for nid in candidates_z2:
        node = graph.get_node(nid)
        if node is None:
            continue
        v = node.attributes.get(pred.attr2)
        if v is not None:
            z2_with_attr.add(nid)
            nv = _normalize_predicate_equality_value(v)
            z2_distinct.add(nv)
            z2_vals.setdefault(nv, set()).add(nid)
    z1_vals = {}
    z1_distinct = set()
    z1_with_attr = set()
    for nid in candidates_z1:
        node = graph.get_node(nid)
        if node is None:
            continue
        v = node.attributes.get(pred.attr1)
        if v is not None:
            z1_with_attr.add(nid)
            nv = _normalize_predicate_equality_value(v)
            z1_distinct.add(nv)
            z1_vals.setdefault(nv, set()).add(nid)
    if len(z2_distinct) >= 2:
        filtered1 = z1_with_attr
    elif len(z2_distinct) == 1:
        sole_z2_val = next(iter(z2_distinct))
        filtered1 = set()
        for v, nids in z1_vals.items():
            if v != sole_z2_val:
                filtered1.update(nids)
    else:
        filtered1 = set()
    if len(z1_distinct) >= 2:
        filtered2 = z2_with_attr
    elif len(z1_distinct) == 1:
        sole_z1_val = next(iter(z1_distinct))
        filtered2 = set()
        for v, nids in z2_vals.items():
            if v != sole_z1_val:
                filtered2.update(nids)
    else:
        filtered2 = set()
    return (filtered1, filtered2)

def _narrow_comparison(graph: Graph, candidates_z1: Set[Any], candidates_z2: Set[Any], pred: PairWisePredicate) -> Tuple[Set[Any], Set[Any]]:
    from data_structure import ComparisonOp
    import bisect

    def _collect_raw(attr: str, candidates: Set[Any]) -> List[Tuple[Any, Any]]:
        collected = []
        for n_id in candidates:
            node = graph.get_node(n_id)
            if node and attr in node.attributes:
                v = node.attributes[attr]
                if v is not None:
                    collected.append((v, n_id))
        return collected
    vals1 = _collect_raw(pred.attr1, candidates_z1)
    vals2 = _collect_raw(pred.attr2, candidates_z2)
    if not vals1 or not vals2:
        return (set(), set())
    try:
        vals1.sort(key=lambda x: x[0])
        vals2.sort(key=lambda x: x[0])
    except TypeError:
        try:
            vals1 = [(float(v), nid) for v, nid in vals1]
            vals2 = [(float(v), nid) for v, nid in vals2]
            vals1.sort(key=lambda x: x[0])
            vals2.sort(key=lambda x: x[0])
        except (TypeError, ValueError):
            return _narrow_brute_force(graph, candidates_z1, candidates_z2, pred)
    sorted_v2 = [v for v, _ in vals2]
    sorted_v1 = [v for v, _ in vals1]
    filtered1 = set()
    filtered2 = set()
    if pred.operator == ComparisonOp.LT:
        for v1, nid in vals1:
            idx = bisect.bisect_right(sorted_v2, v1)
            if idx < len(sorted_v2):
                filtered1.add(nid)
        for v2, nid in vals2:
            idx = bisect.bisect_left(sorted_v1, v2)
            if idx > 0:
                filtered2.add(nid)
    elif pred.operator == ComparisonOp.LE:
        for v1, nid in vals1:
            idx = bisect.bisect_left(sorted_v2, v1)
            if idx < len(sorted_v2):
                filtered1.add(nid)
        for v2, nid in vals2:
            idx = bisect.bisect_right(sorted_v1, v2)
            if idx > 0:
                filtered2.add(nid)
    elif pred.operator == ComparisonOp.GT:
        for v1, nid in vals1:
            idx = bisect.bisect_left(sorted_v2, v1)
            if idx > 0:
                filtered1.add(nid)
        for v2, nid in vals2:
            idx = bisect.bisect_right(sorted_v1, v2)
            if idx < len(sorted_v1):
                filtered2.add(nid)
    elif pred.operator == ComparisonOp.GE:
        for v1, nid in vals1:
            idx = bisect.bisect_right(sorted_v2, v1)
            if idx > 0:
                filtered1.add(nid)
        for v2, nid in vals2:
            idx = bisect.bisect_left(sorted_v1, v2)
            if idx < len(sorted_v1):
                filtered2.add(nid)
    return (filtered1, filtered2)

def _narrow_brute_force(graph: Graph, candidates_z1: Set[Any], candidates_z2: Set[Any], pred: PairWisePredicate) -> Tuple[Set[Any], Set[Any]]:
    filtered1 = set()
    for n1_id in candidates_z1:
        node1 = graph.get_node(n1_id)
        if node1 is None or pred.attr1 not in node1.attributes:
            continue
        v1 = node1.attributes[pred.attr1]
        for n2_id in candidates_z2:
            node2 = graph.get_node(n2_id)
            if node2 is None or pred.attr2 not in node2.attributes:
                continue
            if _safe_compare(v1, node2.attributes[pred.attr2], pred.operator) is True:
                filtered1.add(n1_id)
                break
    filtered2 = set()
    for n2_id in candidates_z2:
        node2 = graph.get_node(n2_id)
        if node2 is None or pred.attr2 not in node2.attributes:
            continue
        v2 = node2.attributes[pred.attr2]
        for n1_id in filtered1:
            node1 = graph.get_node(n1_id)
            if node1 is None or pred.attr1 not in node1.attributes:
                continue
            if _safe_compare(node1.attributes[pred.attr1], v2, pred.operator) is True:
                filtered2.add(n2_id)
                break
    return (filtered1, filtered2)

def _extract_anchor_predicates(rule: 'TIERule') -> List[Tuple[str, str, Any, str]]:
    from data_structure import ComparisonOp
    _OP_MAP = {ComparisonOp.EQ: 'EQ', ComparisonOp.NE: 'NE', ComparisonOp.LT: 'LT', ComparisonOp.LE: 'LE', ComparisonOp.GT: 'GT', ComparisonOp.GE: 'GE'}
    result = []
    for pred in rule.preconditions:
        if not isinstance(pred, PointWisePredicate):
            continue
        if pred.variable in (rule.pattern.user_center, rule.pattern.item_center):
            continue
        op_str = _OP_MAP.get(pred.operator)
        if op_str:
            result.append((pred.variable, pred.attribute, pred.constant, op_str))
    return result

def _get_prefix_sig_for_var(pattern: 'DualStarPattern', var_name: str) -> Tuple[Optional[str], Optional[str]]:
    from data_structure import compute_path_anchor_signature
    for path in pattern.user_paths:
        seq = path.path_sequence
        for i, var in enumerate(seq):
            if var == var_name and i > 0:
                node_labels = [pattern.nodes[v].label for v in seq[:i + 1]]
                edge_labels = [e.label for e in path.edges[:i]]
                is_fwd = [e.is_forward for e in path.edges[:i]]
                return (compute_path_anchor_signature(edge_labels, node_labels, is_fwd), 'user')
    for path in pattern.item_paths:
        seq = path.path_sequence
        for i, var in enumerate(seq):
            if var == var_name and i > 0:
                node_labels = [pattern.nodes[v].label for v in seq[:i + 1]]
                edge_labels = [e.label for e in path.edges[:i]]
                is_fwd = [e.is_forward for e in path.edges[:i]]
                return (compute_path_anchor_signature(edge_labels, node_labels, is_fwd), 'item')
    return (None, None)

def _extract_pairwise_anchor_info(rule: TIERule, anchor_index) -> List[PairWiseAnchorInfo]:
    result = []
    for pred in rule.get_pair_wise_predicates():
        if pred.var1 == pred.var2:
            continue
        sig1, star1 = _get_prefix_sig_for_var(rule.pattern, pred.var1)
        sig2, star2 = _get_prefix_sig_for_var(rule.pattern, pred.var2)
        if sig1 is None or sig2 is None:
            continue
        has1 = (sig1, pred.attr1) in anchor_index.grouped_by_value
        has2 = (sig2, pred.attr2) in anchor_index.grouped_by_value
        if not has1 or not has2:
            continue
        result.append(PairWiseAnchorInfo(pred=pred, var1_sig=sig1, var1_star=star1, var2_sig=sig2, var2_star=star2))
    return result

def _build_int_prefilter_cache(compact_graph: CompactGraph, prefilter_cache: Optional[Dict[str, Set[Any]]]=None) -> Dict[str, set]:
    int_prefilter: Dict[str, set] = {}
    if not prefilter_cache:
        return int_prefilter
    _id_to_int = compact_graph.id_to_int
    for var, pf in prefilter_cache.items():
        if pf is _PREFILTER_NA or pf is None:
            continue
        int_prefilter[var] = {_id_to_int[nid] for nid in pf if nid in _id_to_int}
    return int_prefilter

def build_star_match_context(graph: Graph, pattern: DualStarPattern, paths: list, edge_lookups: list, point_wise_preds: Dict[str, List[PointWisePredicate]], prefilter_cache: Optional[Dict[str, Set[Any]]]=None, compact_graph: Optional[CompactGraph]=None) -> Dict[str, Any]:
    ctx: Dict[str, Any] = {'compact_graph': compact_graph}
    if compact_graph is None or not _HAS_FAST_MATCH or (not paths):
        return ctx
    int_prefilter = _build_int_prefilter_cache(compact_graph, prefilter_cache)
    ctx['int_prefilter'] = int_prefilter
    ctx['fast_path_plans'] = _build_fast_star_plans(graph, pattern, paths, edge_lookups, point_wise_preds, int_prefilter)
    ctx['id_to_int'] = compact_graph.id_to_int
    return ctx

def _build_fast_star_plans(graph: Graph, pattern: DualStarPattern, paths: list, edge_lookups: list, point_wise_preds: Dict[str, List[PointWisePredicate]], int_prefilter: Dict[str, set]) -> List[Tuple[list, dict, dict, dict, bool]]:
    _edge_tgt_label = getattr(graph, 'edge_target_label', {}) or {}
    _edge_src_label = getattr(graph, 'edge_source_label', {}) or {}
    plans: List[Tuple[list, dict, dict, dict, bool]] = []
    for idx, path in enumerate(paths):
        seq = path.path_sequence
        el = edge_lookups[idx]
        skip_checks: Dict[int, bool] = {}
        node_label_map = {v: pattern.nodes[v].label for v in seq}
        needs_pw_eval = False
        for i in range(len(seq) - 1):
            edge_info = el.get((seq[i], seq[i + 1]))
            if edge_info:
                elabel, fwd = edge_info
                next_label = node_label_map[seq[i + 1]]
                if fwd:
                    skip_checks[i] = _edge_tgt_label.get(elabel) == next_label
                else:
                    skip_checks[i] = _edge_src_label.get(elabel) == next_label
        for var in seq[1:]:
            if var in point_wise_preds and var not in int_prefilter:
                needs_pw_eval = True
                break
        plans.append((seq, el, node_label_map, skip_checks, needs_pw_eval))
    return plans

def _verify_pivot_pairwise_binding(user_attrs: Dict[str, Any], item_attrs: Dict[str, Any], pairwise_plans: List[Tuple[bool, str, bool, str, Any]]) -> bool:
    if _cy_verify_pivot_pairwise_binding is not None:
        return bool(_cy_verify_pivot_pairwise_binding(user_attrs, item_attrs, pairwise_plans))
    for lhs_is_user, attr1, rhs_is_user, attr2, operator in pairwise_plans:
        value1 = user_attrs.get(attr1) if lhs_is_user else item_attrs.get(attr1)
        value2 = user_attrs.get(attr2) if rhs_is_user else item_attrs.get(attr2)
        if value1 is None or value2 is None:
            return False
        if not _safe_compare_predicate_values(value1, value2, operator):
            return False
    return True

def _build_pivot_pairwise_plans(pair_wise_preds: List[PairWisePredicate], user_var: str, item_var: str) -> Optional[List[Tuple[bool, str, bool, str, Any]]]:
    plans: List[Tuple[bool, str, bool, str, Any]] = []
    for pred in pair_wise_preds:
        if pred.var1 == user_var:
            lhs_is_user = True
        elif pred.var1 == item_var:
            lhs_is_user = False
        else:
            return None
        if pred.var2 == user_var:
            rhs_is_user = True
        elif pred.var2 == item_var:
            rhs_is_user = False
        else:
            return None
        plans.append((lhs_is_user, pred.attr1, rhs_is_user, pred.attr2, pred.operator))
    return plans

def _match_star_paths(graph: Graph, pattern: DualStarPattern, pivot_id: Any, paths: list, point_wise_preds: Dict[str, List[PointWisePredicate]], edge_lookups: list, prefilter_cache: Optional[Dict[str, Set[Any]]]=None, compact_graph: Optional[CompactGraph]=None, star_match_ctx: Optional[Dict[str, Any]]=None) -> Optional[Dict[str, Set[Any]]]:
    if star_match_ctx is not None and compact_graph is None:
        compact_graph = star_match_ctx.get('compact_graph')
    if compact_graph is not None and _HAS_FAST_MATCH:
        if star_match_ctx is None:
            star_match_ctx = build_star_match_context(graph, pattern, paths, edge_lookups, point_wise_preds, prefilter_cache, compact_graph)
        return _match_star_paths_fast(graph, compact_graph, pattern, pivot_id, paths, point_wise_preds, edge_lookups, prefilter_cache, star_match_ctx=star_match_ctx)
    result = {}
    for idx, path in enumerate(paths):
        path_match = match_path_compact(graph, path, pivot_id, pattern, point_wise_preds, edge_lookup=edge_lookups[idx], prefilter_cache=prefilter_cache)
        if path_match is None:
            return None
        for var, candidates in path_match.items():
            if var in result:
                result[var] = result[var] & candidates
                if not result[var]:
                    return None
            else:
                result[var] = candidates
    return result

def _match_star_paths_fast(graph: Graph, compact_graph: CompactGraph, pattern: DualStarPattern, pivot_id: Any, paths: list, point_wise_preds: Dict[str, List[PointWisePredicate]], edge_lookups: list, prefilter_cache: Optional[Dict[str, Set[Any]]]=None, star_match_ctx: Optional[Dict[str, Any]]=None) -> Optional[Dict[str, Set[Any]]]:
    cg = compact_graph
    if star_match_ctx is None:
        star_match_ctx = build_star_match_context(graph, pattern, paths, edge_lookups, point_wise_preds, prefilter_cache, compact_graph)
    _id_to_int = star_match_ctx.get('id_to_int')
    pivot_int = _id_to_int.get(pivot_id) if _id_to_int is not None else None
    if pivot_int is None:
        return None
    int_prefilter = star_match_ctx.get('int_prefilter') or {}
    fast_path_plans = star_match_ctx.get('fast_path_plans') or []
    _node_attrs = cg.node_attrs

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
    if _cy_match_star_batch is not None:
        merged = _cy_match_star_batch(cg, fast_path_plans, pivot_int, int_prefilter, pw_eval_fn=pw_eval)
        if merged is None:
            return None
        _int_to_id = cg.int_to_id
        return {var: {_int_to_id[i] for i in int_set} for var, int_set in merged.items()}
    merged: Dict[str, set] = {}
    for seq, el, node_label_map, skip_checks, needs_pw_eval in fast_path_plans:
        path_result = _cy_match(cg, seq, pivot_int, el, node_label_map, skip_checks, int_prefilter, pw_eval_fn=pw_eval if needs_pw_eval else None)
        if path_result is None:
            return None
        for var, int_set in path_result.items():
            if var in merged:
                merged[var] = merged[var] & int_set
                if not merged[var]:
                    return None
            else:
                merged[var] = int_set
    _int_to_id = cg.int_to_id
    return {var: {_int_to_id[i] for i in int_set} for var, int_set in merged.items()}

def _analyze_pairwise_needed_attrs(pair_wise_preds: List[PairWisePredicate]) -> Dict[str, Set[str]]:
    needed: Dict[str, Set[str]] = {}
    for pred in pair_wise_preds:
        needed.setdefault(pred.var1, set()).add(pred.attr1)
        needed.setdefault(pred.var2, set()).add(pred.attr2)
    return needed

def _collect_pairwise_attr_data(graph: Graph, star_match: Dict[str, Set[Any]], needed_attrs: Dict[str, Set[str]]) -> Tuple[Dict[Tuple[str, str], frozenset], Dict[Tuple[str, str], Tuple]]:
    _nodes = graph._nodes
    attr_values: Dict[Tuple[str, str], frozenset] = {}
    attr_bounds: Dict[Tuple[str, str], Tuple] = {}
    for var, attrs in needed_attrs.items():
        cands = star_match.get(var)
        if cands is None:
            continue
        for attr in attrs:
            vals = set()
            lo = None
            hi = None
            for nid in cands:
                node = _nodes.get(nid)
                if node is None:
                    continue
                v = node.attributes.get(attr)
                if v is None:
                    continue
                vals.add(v)
                if isinstance(v, (int, float)):
                    if lo is None or v < lo:
                        lo = v
                    if hi is None or v > hi:
                        hi = v
                else:
                    try:
                        fv = float(v)
                        if lo is None or fv < lo:
                            lo = fv
                        if hi is None or fv > hi:
                            hi = fv
                    except (TypeError, ValueError):
                        pass
            if vals:
                attr_values[var, attr] = frozenset(vals)
            if lo is not None:
                attr_bounds[var, attr] = (lo, hi)
    return (attr_values, attr_bounds)

def _pairwise_vars_independent(pair_wise_preds: List[PairWisePredicate]) -> bool:
    seen: Set[str] = set()
    for pred in pair_wise_preds:
        if pred.var1 == pred.var2:
            return False
        if pred.var1 in seen or pred.var2 in seen:
            return False
        seen.add(pred.var1)
        seen.add(pred.var2)
    return True

def _verify_pairwise_fast(u_attr_values: Dict[Tuple[str, str], frozenset], u_attr_bounds: Dict[Tuple[str, str], Tuple], v_attr_values: Dict[Tuple[str, str], frozenset], v_attr_bounds: Dict[Tuple[str, str], Tuple], pair_wise_preds: List[PairWisePredicate]) -> Optional[bool]:
    from data_structure import ComparisonOp
    for pred in pair_wise_preds:
        if pred.var1 == pred.var2:
            return None
        key1 = (pred.var1, pred.attr1)
        key2 = (pred.var2, pred.attr2)
        vals1 = u_attr_values.get(key1) or v_attr_values.get(key1)
        vals2 = u_attr_values.get(key2) or v_attr_values.get(key2)
        if pred.operator == ComparisonOp.EQ:
            if vals1 is None or vals2 is None:
                return False
            norm1 = {_normalize_predicate_equality_value(v) for v in vals1}
            norm2 = {_normalize_predicate_equality_value(v) for v in vals2}
            if norm1.isdisjoint(norm2):
                return False
        elif pred.operator == ComparisonOp.NE:
            if vals1 is None or vals2 is None:
                return False
            norm1 = {_normalize_predicate_equality_value(v) for v in vals1}
            norm2 = {_normalize_predicate_equality_value(v) for v in vals2}
            if len(norm1) >= 2 or len(norm2) >= 2:
                continue
            if norm1 == norm2:
                return False
        elif pred.operator in (ComparisonOp.LT, ComparisonOp.LE, ComparisonOp.GT, ComparisonOp.GE):
            bounds1 = u_attr_bounds.get(key1) or v_attr_bounds.get(key1)
            bounds2 = u_attr_bounds.get(key2) or v_attr_bounds.get(key2)
            if bounds1 is None or bounds2 is None:
                return None
            min1, max1 = bounds1
            min2, max2 = bounds2
            if pred.operator == ComparisonOp.LT:
                if not min1 < max2:
                    return False
            elif pred.operator == ComparisonOp.LE:
                if not min1 <= max2:
                    return False
            elif pred.operator == ComparisonOp.GT:
                if not max1 > min2:
                    return False
            elif pred.operator == ComparisonOp.GE:
                if not max1 >= min2:
                    return False
        else:
            return None
    return True

def apply_path_anchor_filters(graph: Graph, pattern: 'DualStarPattern', preconditions: list, user_candidates: Set[Any], item_candidates: Set[Any], training_index: Optional[Dict[Any, Set[Any]]]=None, training_data: Optional[Dict[Tuple[Any, Any], bool]]=None, debug: bool=False, anchor_preds: Optional[List[Tuple[str, str, Any, Any]]]=None, pw_anchor_infos: Optional[List['PairWiseAnchorInfo']]=None) -> Tuple[Set[Any], Set[Any], Set[int], Optional[Set[Tuple[Any, Any]]]]:
    from data_structure import PathAnchorIndex, ComparisonOp
    anchor_index: Optional[PathAnchorIndex] = getattr(graph, 'path_anchor_index', None)
    pw_handled_ids: set = set()
    cross_star_eq_viable_pairs: Optional[set] = None
    _temp_rule = None
    if anchor_index is None:
        if debug:
            _LOGGER.info('  [PathAnchorIndex] Available: NO')
        return (user_candidates, item_candidates, pw_handled_ids, cross_star_eq_viable_pairs)
    if anchor_preds is None:
        _temp_rule = TIERule(pattern=pattern, preconditions=list(preconditions), conclusion=EdgePredicate(pattern.user_center, pattern.item_center, None))
        anchor_preds = _extract_anchor_predicates(_temp_rule)
    if debug:
        _LOGGER.info('  [PathAnchorIndex] Available: YES')
        _LOGGER.info(f'    Anchor predicates extracted: {len(anchor_preds)}')
        for var, attr, val, op in anchor_preds:
            _LOGGER.info(f'      {var}.{attr} {op} {val}')
    user_candidates_before = len(user_candidates)
    item_candidates_before = len(item_candidates)
    if anchor_preds:
        var_sig_cache = {}
        for var_name, _, _, _ in anchor_preds:
            if var_name not in var_sig_cache:
                var_sig_cache[var_name] = _get_prefix_sig_for_var(pattern, var_name)
        for var_name, attr_name, attr_value, op in anchor_preds:
            path_sig, star_type = var_sig_cache[var_name]
            if path_sig is None:
                if debug:
                    _LOGGER.info(f'    Skipping {var_name}.{attr_name}: no path signature')
                continue
            has_index_data = False
            if op == 'EQ':
                pivots_from_index = anchor_index.query(path_sig, attr_name, attr_value)
                has_index_data = (path_sig, attr_name) in anchor_index.grouped_by_value
            elif op == 'NE':
                pivots_from_index = anchor_index.query_ne(path_sig, attr_name, attr_value)
                has_index_data = (path_sig, attr_name) in anchor_index.all_pivots_by_sig_attr
            elif op in ('LT', 'LE', 'GT', 'GE'):
                groups = anchor_index.query_grouped(path_sig, attr_name)
                has_index_data = bool(groups)
                pivots_from_index = set()
                try:
                    const_f = float(attr_value)
                    for val, pivots in groups.items():
                        try:
                            val_f = float(val)
                        except (TypeError, ValueError):
                            continue
                        if op == 'LT' and val_f < const_f:
                            pivots_from_index |= pivots
                        elif op == 'LE' and val_f <= const_f:
                            pivots_from_index |= pivots
                        elif op == 'GT' and val_f > const_f:
                            pivots_from_index |= pivots
                        elif op == 'GE' and val_f >= const_f:
                            pivots_from_index |= pivots
                except (TypeError, ValueError):
                    pivots_from_index = set()
            else:
                continue
            if debug:
                _LOGGER.info(f'    Query {var_name}.{attr_name} {op} {attr_value} (star={star_type}): {len(pivots_from_index)} pivots (index_data={has_index_data})')
            if pivots_from_index or has_index_data:
                if star_type == 'user':
                    user_candidates = user_candidates & pivots_from_index
                    if not user_candidates:
                        if debug:
                            _LOGGER.info('    User candidates after index: 0 (early exit)')
                        return (user_candidates, item_candidates, pw_handled_ids, cross_star_eq_viable_pairs)
                else:
                    item_candidates = item_candidates & pivots_from_index
                    if not item_candidates:
                        if debug:
                            _LOGGER.info('    Item candidates after index: 0 (early exit)')
                        return (user_candidates, item_candidates, pw_handled_ids, cross_star_eq_viable_pairs)
    if debug:
        if user_candidates_before > 0:
            u_reduction = (1 - len(user_candidates) / user_candidates_before) * 100
            _LOGGER.info(f'    User candidates: {user_candidates_before} -> {len(user_candidates)} ({u_reduction:.1f}% reduction)')
        if item_candidates_before > 0:
            i_reduction = (1 - len(item_candidates) / item_candidates_before) * 100
            _LOGGER.info(f'    Item candidates: {item_candidates_before} -> {len(item_candidates)} ({i_reduction:.1f}% reduction)')
    if pw_anchor_infos is None:
        if _temp_rule is None:
            _temp_rule = TIERule(pattern=pattern, preconditions=list(preconditions), conclusion=EdgePredicate(pattern.user_center, pattern.item_center, None))
        pw_anchor_infos = _extract_pairwise_anchor_info(_temp_rule, anchor_index)
    elif _temp_rule is None and anchor_preds is None:
        _temp_rule = TIERule(pattern=pattern, preconditions=list(preconditions), conclusion=EdgePredicate(pattern.user_center, pattern.item_center, None))
    if debug:
        _LOGGER.info(f'  [PathAnchor PairWise] Accelerable predicates: {len(pw_anchor_infos)}')
    for info in pw_anchor_infos:
        pred = info.pred
        if pred.operator != ComparisonOp.EQ:
            continue
        if info.var1_star != info.var2_star:
            continue
        group1 = anchor_index.query_grouped(info.var1_sig, pred.attr1)
        group2 = anchor_index.query_grouped(info.var2_sig, pred.attr2)
        common_values = set(group1.keys()) & set(group2.keys())
        valid_pivots = set()
        for val in common_values:
            valid_pivots |= group1[val] & group2[val]
        if info.var1_star == 'user':
            before = len(user_candidates)
            user_candidates = user_candidates & valid_pivots
            if debug:
                _LOGGER.info(f'    Same-star EQ {pred.var1}.{pred.attr1}={pred.var2}.{pred.attr2} (user): {before} -> {len(user_candidates)}')
            if not user_candidates:
                return (user_candidates, item_candidates, pw_handled_ids, cross_star_eq_viable_pairs)
        else:
            before = len(item_candidates)
            item_candidates = item_candidates & valid_pivots
            if debug:
                _LOGGER.info(f'    Same-star EQ {pred.var1}.{pred.attr1}={pred.var2}.{pred.attr2} (item): {before} -> {len(item_candidates)}')
            if not item_candidates:
                return (user_candidates, item_candidates, pw_handled_ids, cross_star_eq_viable_pairs)
        pw_handled_ids.add(id(pred))
    cross_star_ne_infos = [info for info in pw_anchor_infos if info.pred.operator == ComparisonOp.NE and info.var1_star != info.var2_star]
    for info in cross_star_ne_infos:
        pred = info.pred
        if info.var1_star == 'user':
            u_sig, u_attr = (info.var1_sig, pred.attr1)
            v_sig, v_attr = (info.var2_sig, pred.attr2)
        else:
            u_sig, u_attr = (info.var2_sig, pred.attr2)
            v_sig, v_attr = (info.var1_sig, pred.attr1)
        u_groups = anchor_index.query_grouped(u_sig, u_attr)
        v_groups = anchor_index.query_grouped(v_sig, v_attr)
        if not u_groups or not v_groups:
            continue
        if len(u_groups) == 1:
            sole_val = next(iter(u_groups))
            valid_items = set()
            for val, pivots in v_groups.items():
                if val != sole_val:
                    valid_items |= pivots
            if valid_items:
                before = len(item_candidates)
                item_candidates = item_candidates & valid_items
                if debug:
                    _LOGGER.info(f'    Cross-star NE (user sole={sole_val}): items {before}->{len(item_candidates)}')
                if not item_candidates:
                    return (user_candidates, item_candidates, pw_handled_ids, cross_star_eq_viable_pairs)
            else:
                item_candidates = set()
                return (user_candidates, item_candidates, pw_handled_ids, cross_star_eq_viable_pairs)
            pw_handled_ids.add(id(pred))
        if len(v_groups) == 1 and id(pred) not in pw_handled_ids:
            sole_val = next(iter(v_groups))
            valid_users = set()
            for val, pivots in u_groups.items():
                if val != sole_val:
                    valid_users |= pivots
            if valid_users:
                before = len(user_candidates)
                user_candidates = user_candidates & valid_users
                if debug:
                    _LOGGER.info(f'    Cross-star NE (item sole={sole_val}): users {before}->{len(user_candidates)}')
                if not user_candidates:
                    return (user_candidates, item_candidates, pw_handled_ids, cross_star_eq_viable_pairs)
            else:
                user_candidates = set()
                return (user_candidates, item_candidates, pw_handled_ids, cross_star_eq_viable_pairs)
            pw_handled_ids.add(id(pred))
    cross_star_cmp_infos = [info for info in pw_anchor_infos if info.pred.operator in (ComparisonOp.LT, ComparisonOp.LE, ComparisonOp.GT, ComparisonOp.GE) and info.var1_star != info.var2_star]
    for info in cross_star_cmp_infos:
        pred = info.pred
        if info.var1_star == 'user':
            u_sig, u_attr = (info.var1_sig, pred.attr1)
            v_sig, v_attr = (info.var2_sig, pred.attr2)
        else:
            u_sig, u_attr = (info.var2_sig, pred.attr2)
            v_sig, v_attr = (info.var1_sig, pred.attr1)
        u_groups = anchor_index.query_grouped(u_sig, u_attr)
        v_groups = anchor_index.query_grouped(v_sig, v_attr)
        try:
            u_all_vals = sorted(set(u_groups.keys()), key=float)
            v_all_vals = sorted(set(v_groups.keys()), key=float)
        except (TypeError, ValueError):
            continue
        if not u_all_vals or not v_all_vals:
            continue
        op = pred.operator
        if info.var1_star == 'item':
            flip_map = {ComparisonOp.LT: ComparisonOp.GT, ComparisonOp.LE: ComparisonOp.GE, ComparisonOp.GT: ComparisonOp.LT, ComparisonOp.GE: ComparisonOp.LE}
            op = flip_map[op]
        u_before = len(user_candidates)
        v_before = len(item_candidates)
        if op == ComparisonOp.LT:
            v_max = float(v_all_vals[-1])
            valid_u = set()
            for val in u_all_vals:
                if float(val) < v_max:
                    valid_u |= u_groups[val] & user_candidates
            user_candidates = user_candidates & valid_u if valid_u else set()
            u_min = float(u_all_vals[0])
            valid_v = set()
            for val in v_all_vals:
                if float(val) > u_min:
                    valid_v |= v_groups[val] & item_candidates
            item_candidates = item_candidates & valid_v if valid_v else set()
        elif op == ComparisonOp.LE:
            v_max = float(v_all_vals[-1])
            valid_u = set()
            for val in u_all_vals:
                if float(val) <= v_max:
                    valid_u |= u_groups[val] & user_candidates
            user_candidates = user_candidates & valid_u if valid_u else set()
            u_min = float(u_all_vals[0])
            valid_v = set()
            for val in v_all_vals:
                if float(val) >= u_min:
                    valid_v |= v_groups[val] & item_candidates
            item_candidates = item_candidates & valid_v if valid_v else set()
        elif op == ComparisonOp.GT:
            v_min = float(v_all_vals[0])
            valid_u = set()
            for val in u_all_vals:
                if float(val) > v_min:
                    valid_u |= u_groups[val] & user_candidates
            user_candidates = user_candidates & valid_u if valid_u else set()
            u_max = float(u_all_vals[-1])
            valid_v = set()
            for val in v_all_vals:
                if float(val) < u_max:
                    valid_v |= v_groups[val] & item_candidates
            item_candidates = item_candidates & valid_v if valid_v else set()
        elif op == ComparisonOp.GE:
            v_min = float(v_all_vals[0])
            valid_u = set()
            for val in u_all_vals:
                if float(val) >= v_min:
                    valid_u |= u_groups[val] & user_candidates
            user_candidates = user_candidates & valid_u if valid_u else set()
            u_max = float(u_all_vals[-1])
            valid_v = set()
            for val in v_all_vals:
                if float(val) <= u_max:
                    valid_v |= v_groups[val] & item_candidates
            item_candidates = item_candidates & valid_v if valid_v else set()
        if debug:
            _LOGGER.info(f'    Cross-star CMP {pred}: users {u_before}->{len(user_candidates)}, items {v_before}->{len(item_candidates)}')
        if not user_candidates or not item_candidates:
            return (user_candidates, item_candidates, pw_handled_ids, cross_star_eq_viable_pairs)
    cross_star_eq_infos = [info for info in pw_anchor_infos if info.pred.operator == ComparisonOp.EQ and info.var1_star != info.var2_star]
    if cross_star_eq_infos and training_data is not None:
        viable_pairs_sets = []
        for info in cross_star_eq_infos:
            pred = info.pred
            if info.var1_star == 'user':
                u_sig, u_attr = (info.var1_sig, pred.attr1)
                v_sig, v_attr = (info.var2_sig, pred.attr2)
            else:
                u_sig, u_attr = (info.var2_sig, pred.attr2)
                v_sig, v_attr = (info.var1_sig, pred.attr1)
            u_groups = anchor_index.query_grouped(u_sig, u_attr)
            v_groups = anchor_index.query_grouped(v_sig, v_attr)
            common_vals = set(u_groups.keys()) & set(v_groups.keys())
            viable = set()
            for val in common_vals:
                valid_users = u_groups[val] & user_candidates
                valid_items = v_groups[val] & item_candidates
                if not valid_users or not valid_items:
                    continue
                for u in valid_users:
                    if training_index is not None:
                        matched_items = training_index.get(u, set()) & valid_items
                    else:
                        matched_items = {vi for vi in valid_items if (u, vi) in training_data}
                    for vi in matched_items:
                        viable.add((u, vi))
            viable_pairs_sets.append(viable)
            pw_handled_ids.add(id(pred))
            if debug:
                _LOGGER.info(f'    Cross-star EQ join {pred.var1}.{pred.attr1}={pred.var2}.{pred.attr2}: {len(common_vals)} common values, {len(viable)} viable pairs')
        if viable_pairs_sets:
            cross_star_eq_viable_pairs = viable_pairs_sets[0]
            for s in viable_pairs_sets[1:]:
                cross_star_eq_viable_pairs &= s
    return (user_candidates, item_candidates, pw_handled_ids, cross_star_eq_viable_pairs)

def _check_conclusion_edge(graph: Graph, u: Any, v: Any, rule: TIERule, training_data: Optional[Dict[Tuple[Any, Any], bool]]=None) -> bool:
    if training_data is not None and (u, v) in training_data:
        return bool(training_data[u, v])
    conclusion = rule.conclusion
    if conclusion is None or not getattr(conclusion, 'edge_label', None):
        return False
    pattern = rule.pattern
    var_to_node = {pattern.user_center: u, pattern.item_center: v}
    src = var_to_node.get(conclusion.source_var)
    tgt = var_to_node.get(conclusion.target_var)
    if src is None or tgt is None:
        return False
    return graph.has_edge(src, conclusion.edge_label, tgt)

def compute_rule_metrics(graph: Graph, rule: TIERule, training_data: Dict[Tuple[Any, Any], bool], ml_cache: MLPredicateCache, debug: bool=False, training_index: Optional[Dict[Any, Set[Any]]]=None) -> RuleMetrics:
    debug = _debug_enabled(debug)
    if debug:
        _LOGGER.info(f'\n{'=' * 70}')
        _LOGGER.info('[compute_rule_metrics] Starting rule evaluation')
        _LOGGER.info(f'  Pattern: {rule.pattern.user_center} ({len(rule.pattern.user_paths)} user paths), {rule.pattern.item_center} ({len(rule.pattern.item_paths)} item paths)')
        _LOGGER.info(f'  Preconditions: {len(rule.preconditions)} ({len(rule.get_point_wise_predicates())} point-wise, {len(rule.get_pair_wise_predicates())} pair-wise, {len(rule.get_ml_predicates())} ML)')
    rule_cache = _get_rule_matcher_cache(rule)
    graph_key = id(graph)
    rule_graph_cache = rule_cache.setdefault(graph_key, {})
    point_wise_preds = rule_cache.get('point_wise_preds')
    if point_wise_preds is None:
        point_wise_preds = rule.get_point_wise_predicates()
        rule_cache['point_wise_preds'] = point_wise_preds
    pair_wise_preds = rule_cache.get('pair_wise_preds')
    if pair_wise_preds is None:
        pair_wise_preds = rule.get_pair_wise_predicates()
        rule_cache['pair_wise_preds'] = pair_wise_preds
    ml_preds = rule_cache.get('ml_preds')
    if ml_preds is None:
        ml_preds = rule.get_ml_predicates()
        rule_cache['ml_preds'] = ml_preds
    user_candidates, item_candidates = get_pivot_candidates(graph, rule.pattern, point_wise_preds, debug=debug)
    if debug:
        _LOGGER.info(f'  Initial candidates: {len(user_candidates)} users, {len(item_candidates)} items')
    from data_structure import PathAnchorIndex
    anchor_index: Optional[PathAnchorIndex] = getattr(graph, 'path_anchor_index', None)
    if training_index is None and len(training_data) > 100:
        training_index = _get_cached_training_index(graph, training_data)
    if anchor_index is not None:
        anchor_preds = rule_cache.get('anchor_preds')
        if anchor_preds is None:
            anchor_preds = _extract_anchor_predicates(rule)
            rule_cache['anchor_preds'] = anchor_preds
        pw_anchor_infos = rule_graph_cache.get('pw_anchor_infos')
        if pw_anchor_infos is None:
            pw_anchor_infos = _extract_pairwise_anchor_info(rule, anchor_index)
            rule_graph_cache['pw_anchor_infos'] = pw_anchor_infos
    else:
        anchor_preds = []
        pw_anchor_infos = None
    user_candidates, item_candidates, pw_handled_by_anchor, cross_star_eq_viable_pairs = apply_path_anchor_filters(graph, rule.pattern, rule.preconditions, user_candidates, item_candidates, training_index=training_index, training_data=training_data, debug=debug, anchor_preds=anchor_preds, pw_anchor_infos=pw_anchor_infos)
    if not user_candidates or not item_candidates:
        return RuleMetrics()
    pivot_vars = {rule.pattern.user_center, rule.pattern.item_center}
    prefilter_cache = rule_graph_cache.get('prefilter_cache')
    if prefilter_cache is None:
        prefilter_cache = {}
        for var, preds in point_wise_preds.items():
            if preds and var not in pivot_vars:
                var_label = rule.pattern.nodes[var].label
                prefiltered = _prefilter_by_attribute_index(graph, var_label, preds)
                prefilter_cache[var] = prefiltered if prefiltered is not None else _PREFILTER_NA
        rule_graph_cache['prefilter_cache'] = prefilter_cache
    import time
    t0 = time.perf_counter()
    if cross_star_eq_viable_pairs is not None:
        training_pairs = [p for p in cross_star_eq_viable_pairs if p in training_data]
        if debug:
            _LOGGER.info(f'    [PathAnchor EQ Join] Viable pairs: {len(cross_star_eq_viable_pairs)} -> training pairs: {len(training_pairs)}')
    elif training_index is not None:
        training_pairs = [(u, v) for u in user_candidates for v in training_index.get(u, set()) & item_candidates]
    else:
        training_pairs = [(u, v) for u in user_candidates for v in item_candidates if (u, v) in training_data]
    pair_filter_time = time.perf_counter() - t0
    if debug:
        _LOGGER.info(f'    [性能] 训练对筛选: {pair_filter_time * 1000:.2f}ms')
        _LOGGER.info(f'    [性能] 待检查样本数: {len(training_pairs)}')
        _LOGGER.info(f'    [PathAnchor] Handled PairWise preds: {len(pw_handled_by_anchor)}/{len(rule.get_pair_wise_predicates())}')
    matched_pivots = set()
    positive_pivots = set()
    checked_pairs = 0
    matched_pairs = 0
    remaining_pw_preds = [p for p in pair_wise_preds if id(p) not in pw_handled_by_anchor]
    t0 = time.perf_counter()
    if anchor_index is not None and (not remaining_pw_preds) and (not ml_preds):
        _all_anchor_covered = True
        for var in point_wise_preds:
            if var in pivot_vars:
                continue
            psig, _ = _get_prefix_sig_for_var(rule.pattern, var)
            if psig is None or not anchor_index.has_entry(psig):
                _all_anchor_covered = False
                break
            for pred in point_wise_preds[var]:
                if (psig, pred.attribute) not in anchor_index.grouped_by_value:
                    _all_anchor_covered = False
                    break
            if not _all_anchor_covered:
                break
        if _all_anchor_covered:
            _anchor_covered_vars = set()
            for var in point_wise_preds:
                if var in pivot_vars:
                    continue
                var_preds = point_wise_preds[var]
                if not var_preds:
                    continue
                if len(var_preds) > 1:
                    continue
                psig, _ = _get_prefix_sig_for_var(rule.pattern, var)
                if psig is not None and all(((psig, p.attribute) in anchor_index.grouped_by_value for p in var_preds)):
                    _anchor_covered_vars.add(var)
            for path in rule.pattern.user_paths + rule.pattern.item_paths:
                if hasattr(path, 'path_sequence') and path.path_sequence:
                    pred_vars_on_path = [v for v in path.path_sequence if v not in pivot_vars and v in _anchor_covered_vars]
                    non_pred_vars_on_path = [v for v in path.path_sequence if v not in pivot_vars and v not in _anchor_covered_vars]
                    if not pred_vars_on_path or non_pred_vars_on_path or len(pred_vars_on_path) > 1:
                        _all_anchor_covered = False
                        break
        if _all_anchor_covered:
            for u, v in training_pairs:
                matched_pivots.add((u, v))
                if _check_conclusion_edge(graph, u, v, rule, training_data=training_data):
                    positive_pivots.add((u, v))
            skip_time = time.perf_counter() - t0
            if debug:
                _LOGGER.info('    [Fast Path] Skipped star matching — PathAnchor covers all predicates')
                _LOGGER.info(f'        [匹配结果] 检查了 {len(training_pairs)} 对，匹配 {len(matched_pivots)} 对')
                _LOGGER.info('    [性能总结]')
                _LOGGER.info(f'      Skip fast path: {skip_time * 1000:.2f}ms')
            support = len(matched_pivots)
            matched_count = support
            confidence = len(positive_pivots) / support if support > 0 else 0.0
            return RuleMetrics(support=support, confidence=confidence, matched_count=matched_count, matched_pivots=matched_pivots, positive_pivots=positive_pivots)
    pattern = rule.pattern
    _compact: Optional[CompactGraph] = getattr(graph, '_compact', None)
    edge_lookup_cache = rule_cache.get('edge_lookup_cache')
    if edge_lookup_cache is None:
        all_paths = pattern.user_paths + pattern.item_paths
        edge_lookups_all = []
        for path in all_paths:
            edge_lookups_all.append({(edge.source_var, edge.target_var): (edge.label, edge.is_forward) for edge in path.edges})
        edge_lookup_cache = (edge_lookups_all[:len(pattern.user_paths)], edge_lookups_all[len(pattern.user_paths):])
        rule_cache['edge_lookup_cache'] = edge_lookup_cache
    user_edge_lookups, item_edge_lookups = edge_lookup_cache
    _anchor_covered_vars_sps = rule_graph_cache.get('anchor_covered_vars_sps')
    if _anchor_covered_vars_sps is None:
        _anchor_covered_vars_sps = set()
        if anchor_index is not None and anchor_preds:
            _var_preds_map: Dict[str, List[Tuple[str, str]]] = {}
            _var_sig_cache: Dict[str, Optional[str]] = {}
            for var, attr, val, op in anchor_preds:
                _var_preds_map.setdefault(var, []).append(attr)
                if var not in _var_sig_cache:
                    psig, _ = _get_prefix_sig_for_var(rule.pattern, var)
                    _var_sig_cache[var] = psig
            for var, attrs in _var_preds_map.items():
                psig = _var_sig_cache.get(var)
                if psig is None:
                    continue
                if all(((psig, a) in anchor_index.grouped_by_value for a in attrs)):
                    _anchor_covered_vars_sps.add(var)
        rule_graph_cache['anchor_covered_vars_sps'] = _anchor_covered_vars_sps
    remaining_pw_key = tuple((id(p) for p in remaining_pw_preds))
    path_plan_cache = rule_graph_cache.setdefault('path_plan_cache', {})
    cached_path_plan = path_plan_cache.get(remaining_pw_key)
    if cached_path_plan is None:
        _pw_referenced_vars: Set[str] = set()
        for _pw in remaining_pw_preds:
            _pw_referenced_vars.add(_pw.var1)
            _pw_referenced_vars.add(_pw.var2)

        def _path_anchor_skippable(path) -> bool:
            if not hasattr(path, 'path_sequence') or not path.path_sequence:
                return False
            pred_vars = [v for v in path.path_sequence if v not in pivot_vars and v in _anchor_covered_vars_sps]
            uncovered_vars = [v for v in path.path_sequence if v not in pivot_vars and v not in _anchor_covered_vars_sps]
            if len(pred_vars) != 1 or len(uncovered_vars) != 0:
                return False
            covered_var = pred_vars[0]
            if covered_var in _pw_referenced_vars:
                return False
            return True
        _user_remaining_idxs = tuple((i for i, p in enumerate(pattern.user_paths) if not _path_anchor_skippable(p)))
        _item_remaining_idxs = tuple((i for i, p in enumerate(pattern.item_paths) if not _path_anchor_skippable(p)))
        pivot_only_pw = not remaining_pw_preds or all((pred.var1 in pivot_vars and pred.var2 in pivot_vars for pred in remaining_pw_preds))
        pivot_pairwise_plans = _build_pivot_pairwise_plans(remaining_pw_preds, pattern.user_center, pattern.item_center) if remaining_pw_preds and pivot_only_pw else None
        cached_path_plan = (_user_remaining_idxs, _item_remaining_idxs, pivot_only_pw, pivot_pairwise_plans)
        path_plan_cache[remaining_pw_key] = cached_path_plan
    _user_remaining_idxs, _item_remaining_idxs, pivot_only_pw, pivot_pairwise_plans = cached_path_plan
    _user_remaining_paths = [pattern.user_paths[i] for i in _user_remaining_idxs]
    _user_remaining_lookups = [user_edge_lookups[i] for i in _user_remaining_idxs]
    _item_remaining_paths = [pattern.item_paths[i] for i in _item_remaining_idxs]
    _item_remaining_lookups = [item_edge_lookups[i] for i in _item_remaining_idxs]
    _user_skipped = len(pattern.user_paths) - len(_user_remaining_paths)
    _item_skipped = len(pattern.item_paths) - len(_item_remaining_paths)
    if debug and (_user_skipped or _item_skipped):
        _LOGGER.info(f'    [Selective Path Skip] User paths: {_user_skipped}/{len(pattern.user_paths)} skipped, Item paths: {_item_skipped}/{len(pattern.item_paths)} skipped')
    _has_conclusion = bool(rule.conclusion and getattr(rule.conclusion, 'edge_label', None))
    if not _user_remaining_paths and (not _item_remaining_paths) and pivot_only_pw:
        _nodes = graph._nodes
        _get_prediction = ml_cache.get_prediction
        user_attr_cache: Dict[Any, Optional[Dict[str, Any]]] = {}
        item_attr_cache: Dict[Any, Optional[Dict[str, Any]]] = {}
        for u, v in training_pairs:
            checked_pairs += 1
            if u in user_attr_cache:
                u_attrs = user_attr_cache[u]
            else:
                u_node = _nodes.get(u)
                u_attrs = u_node.attributes if u_node is not None else None
                user_attr_cache[u] = u_attrs
            if v in item_attr_cache:
                v_attrs = item_attr_cache[v]
            else:
                v_node = _nodes.get(v)
                v_attrs = v_node.attributes if v_node is not None else None
                item_attr_cache[v] = v_attrs
            if u_attrs is None or v_attrs is None:
                continue
            if pivot_pairwise_plans and (not _verify_pivot_pairwise_binding(u_attrs, v_attrs, pivot_pairwise_plans)):
                continue
            ml_satisfied = True
            for ml_pred in ml_preds:
                score = _get_prediction(u, v)
                if not ml_pred.evaluate(score):
                    ml_satisfied = False
                    break
            if not ml_satisfied:
                continue
            matched_pivots.add((u, v))
            matched_pairs += 1
            if _has_conclusion and _check_conclusion_edge(graph, u, v, rule, training_data=training_data):
                positive_pivots.add((u, v))
        support = len(matched_pivots)
        matched_count = support
        confidence = len(positive_pivots) / support if support > 0 else 0.0
        return RuleMetrics(support=support, confidence=confidence, matched_count=matched_count, matched_pivots=matched_pivots, positive_pivots=positive_pivots)
    unique_users = set()
    unique_items = set()
    for u, v in training_pairs:
        unique_users.add(u)
        unique_items.add(v)
    _user_star_ctx = None
    _item_star_ctx = None
    if _compact is not None and _HAS_FAST_MATCH:
        star_ctx_cache = rule_graph_cache.setdefault('star_ctx_cache', {})
        star_ctx_key = (id(_compact), _user_remaining_idxs, _item_remaining_idxs)
        cached_star_ctx = star_ctx_cache.get(star_ctx_key)
        if cached_star_ctx is None:
            _user_star_ctx = build_star_match_context(graph, pattern, _user_remaining_paths, _user_remaining_lookups, point_wise_preds, prefilter_cache, _compact) if _user_remaining_paths else {'compact_graph': _compact}
            _item_star_ctx = build_star_match_context(graph, pattern, _item_remaining_paths, _item_remaining_lookups, point_wise_preds, prefilter_cache, _compact) if _item_remaining_paths else {'compact_graph': _compact}
            cached_star_ctx = (_user_star_ctx, _item_star_ctx)
            star_ctx_cache[star_ctx_key] = cached_star_ctx
        else:
            _user_star_ctx, _item_star_ctx = cached_star_ctx
    user_star_cache: Dict[Any, Optional[Dict[str, Set[Any]]]] = {}
    for u in unique_users:
        if _user_remaining_paths:
            user_star_cache[u] = _match_star_paths(graph, pattern, u, _user_remaining_paths, point_wise_preds, _user_remaining_lookups, prefilter_cache, compact_graph=_compact, star_match_ctx=_user_star_ctx)
        else:
            user_star_cache[u] = {pattern.user_center: {u}}
    item_star_cache: Dict[Any, Optional[Dict[str, Set[Any]]]] = {}
    for v in unique_items:
        if _item_remaining_paths:
            item_star_cache[v] = _match_star_paths(graph, pattern, v, _item_remaining_paths, point_wise_preds, _item_remaining_lookups, prefilter_cache, compact_graph=_compact, star_match_ctx=_item_star_ctx)
        else:
            item_star_cache[v] = {pattern.item_center: {v}}
    if debug:
        u_matched = sum((1 for m in user_star_cache.values() if m is not None))
        v_matched = sum((1 for m in item_star_cache.values() if m is not None))
        _LOGGER.info(f'    [Star Cache] Users: {u_matched}/{len(unique_users)} matched, Items: {v_matched}/{len(unique_items)} matched')
    has_remaining_pw = bool(remaining_pw_preds)
    pw_needed_attrs: Dict[str, Set[str]] = {}
    pw_vars_indep = True
    user_pw_attr_cache: Dict[Any, Tuple[Dict, Dict]] = {}
    item_pw_attr_cache: Dict[Any, Tuple[Dict, Dict]] = {}
    if has_remaining_pw:
        pw_needed_attrs = _analyze_pairwise_needed_attrs(remaining_pw_preds)
        pw_vars_indep = _pairwise_vars_independent(remaining_pw_preds)
        for u, u_match in user_star_cache.items():
            if u_match is not None:
                user_pw_attr_cache[u] = _collect_pairwise_attr_data(graph, u_match, pw_needed_attrs)
        for v, v_match in item_star_cache.items():
            if v_match is not None:
                item_pw_attr_cache[v] = _collect_pairwise_attr_data(graph, v_match, pw_needed_attrs)
    star_cache_time = time.perf_counter() - t0
    if debug:
        _LOGGER.info(f'    [性能] Star caching + attr extraction: {star_cache_time * 1000:.2f}ms')
    t1 = time.perf_counter()
    _empty_vals: Dict[Tuple[str, str], frozenset] = {}
    _empty_bounds: Dict[Tuple[str, str], Tuple] = {}
    _get_prediction = ml_cache.get_prediction
    for u, v in training_pairs:
        checked_pairs += 1
        u_match = user_star_cache.get(u)
        v_match = item_star_cache.get(v)
        if u_match is None or v_match is None:
            continue
        compact_match = {}
        compact_match[pattern.user_center] = {u}
        compact_match[pattern.item_center] = {v}
        _merge_failed = False
        for var, cands in u_match.items():
            if var == pattern.user_center:
                continue
            if var in compact_match:
                compact_match[var] = compact_match[var] & cands
                if not compact_match[var]:
                    _merge_failed = True
                    break
            else:
                compact_match[var] = cands
        if _merge_failed:
            continue
        for var, cands in v_match.items():
            if var == pattern.item_center:
                continue
            if var in compact_match:
                compact_match[var] = compact_match[var] & cands
                if not compact_match[var]:
                    _merge_failed = True
                    break
            else:
                compact_match[var] = cands
        if _merge_failed:
            continue
        if has_remaining_pw:
            u_vals, u_bounds = user_pw_attr_cache.get(u, (_empty_vals, _empty_bounds))
            v_vals, v_bounds = item_pw_attr_cache.get(v, (_empty_vals, _empty_bounds))
            fast_result = _verify_pairwise_fast(u_vals, u_bounds, v_vals, v_bounds, remaining_pw_preds)
            if fast_result is False:
                continue
            if fast_result is None or not pw_vars_indep:
                if not verify_variable_predicates(graph, compact_match, remaining_pw_preds):
                    continue
        ml_satisfied = True
        for ml_pred in ml_preds:
            score = _get_prediction(u, v)
            if not ml_pred.evaluate(score):
                ml_satisfied = False
                break
        if not ml_satisfied:
            continue
        matched_pivots.add((u, v))
        matched_pairs += 1
        if _has_conclusion and _check_conclusion_edge(graph, u, v, rule, training_data=training_data):
            positive_pivots.add((u, v))
    pivot_checking_time = time.perf_counter() - t1
    total_time = time.perf_counter() - t0
    if debug:
        _LOGGER.info(f'        [匹配结果] 检查了 {checked_pairs} 对，匹配 {matched_pairs} 对')
        if checked_pairs == 0:
            _LOGGER.warning('        ⚠️ 候选集与训练数据无交集！')
        avg_per_pair = pivot_checking_time / checked_pairs * 1000 if checked_pairs > 0 else 0
        _LOGGER.info('    [性能总结]')
        _LOGGER.info(f'      Star cache phase: {star_cache_time * 1000:.2f}ms')
        _LOGGER.info(f'      Merge+verify phase: {pivot_checking_time:.2f}s')
        _LOGGER.info(f'      总耗时: {total_time:.2f}s')
        _LOGGER.info(f'      检查对数: {checked_pairs}')
        _LOGGER.info(f'      匹配对数: {matched_pairs}')
        _LOGGER.info(f'      平均耗时: {avg_per_pair:.3f}ms/对')
    support = len(matched_pivots)
    matched_count = support
    confidence = len(positive_pivots) / support if support > 0 else 0.0
    return RuleMetrics(support=support, confidence=confidence, matched_count=matched_count, matched_pivots=matched_pivots, positive_pivots=positive_pivots)
