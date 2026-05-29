import heapq
from data_structure import ComparisonOp, _safe_compare_predicate_values

def match_path_compact_fast(compact_graph, path_sequence, pivot_int, edge_lookup, node_label_map, skip_label_checks, prefilter_sets, pw_eval_fn=None):
    if not path_sequence:
        return {}
    if len(path_sequence) == 1:
        return {path_sequence[0]: {pivot_int}}
    result = {path_sequence[0]: {pivot_int}}
    cg_out = compact_graph.out_neighbors
    cg_in = compact_graph.in_neighbors
    cg_labels = compact_graph.node_labels
    for i in range(len(path_sequence) - 1):
        current_var = path_sequence[i]
        next_var = path_sequence[i + 1]
        edge_info = edge_lookup.get((current_var, next_var))
        if edge_info is None:
            return None
        edge_label, is_forward = edge_info
        expected_label = node_label_map.get(next_var)
        do_label_check = not skip_label_checks.get(i, False)
        current_set = result[current_var]
        next_set = set()
        prefilter = prefilter_sets.get(next_var)
        has_prefilter = prefilter is not None
        has_pw_eval = pw_eval_fn is not None
        for current_int in current_set:
            neighbors = (cg_out if is_forward else cg_in).get((current_int, edge_label))
            if neighbors is None:
                continue
            for n_id in neighbors:
                if do_label_check and cg_labels[n_id] != expected_label:
                    continue
                if has_prefilter:
                    if n_id not in prefilter:
                        continue
                elif has_pw_eval:
                    if not pw_eval_fn(next_var, n_id):
                        continue
                next_set.add(n_id)
        if not next_set:
            return None
        result[next_var] = next_set
    return result

def match_star_paths_batch_fast(compact_graph, fast_path_plans, pivot_int, int_prefilter, pw_eval_fn=None):
    merged = {}
    for seq, el, node_label_map, skip_checks, needs_pw_eval in fast_path_plans:
        path_result = match_path_compact_fast(compact_graph, seq, pivot_int, el, node_label_map, skip_checks, int_prefilter, pw_eval_fn=pw_eval_fn if needs_pw_eval else None)
        if path_result is None:
            return None
        for var, int_set in path_result.items():
            if var in merged:
                merged[var] &= int_set
                if not merged[var]:
                    return None
            else:
                merged[var] = set(int_set)
    return merged

def match_bridge_batch(compact_graph, path_seq, pivot_ints, edge_lookup, node_label_map, skip_label_checks, int_prefilter, pw_eval_fn=None, w_candidate_ints=None):
    if not path_seq or not pivot_ints:
        return {}
    step_count = len(path_seq) - 1
    if step_count == 0:
        return {p: {path_seq[0]: {p}} for p in pivot_ints}
    cg_out = compact_graph.out_neighbors
    cg_in = compact_graph.in_neighbors
    cg_labels = compact_graph.node_labels
    hop_meta = []
    for i in range(step_count):
        current_var = path_seq[i]
        next_var = path_seq[i + 1]
        edge_info = edge_lookup.get((current_var, next_var))
        if edge_info is None:
            return {p: None for p in pivot_ints}
        hop_meta.append((current_var, next_var, edge_info[0], edge_info[1], node_label_map.get(next_var), not skip_label_checks.get(i, False), int_prefilter.get(next_var)))
    has_pw_eval = pw_eval_fn is not None
    last_var = path_seq[step_count]
    has_w_filter = w_candidate_ints is not None
    out = {}
    for pivot_int in pivot_ints:
        result = {path_seq[0]: {pivot_int}}
        failed = False
        for i in range(step_count):
            current_var, next_var, edge_label, is_forward, expected_label, do_label_check, prefilter = hop_meta[i]
            has_prefilter = prefilter is not None
            current_set = result[current_var]
            next_set = set()
            for current_int in current_set:
                neighbors = (cg_out if is_forward else cg_in).get((current_int, edge_label))
                if neighbors is None:
                    continue
                for n_id in neighbors:
                    if do_label_check and cg_labels[n_id] != expected_label:
                        continue
                    if has_prefilter:
                        if n_id not in prefilter:
                            continue
                    elif has_pw_eval:
                        if not pw_eval_fn(next_var, n_id):
                            continue
                    next_set.add(n_id)
            if not next_set:
                failed = True
                break
            result[next_var] = next_set
        if failed:
            out[pivot_int] = None
            continue
        if has_w_filter and last_var in result:
            filtered = result[last_var] & w_candidate_ints
            if not filtered:
                out[pivot_int] = None
                continue
            result[last_var] = filtered
        out[pivot_int] = result
    return out

def derive_bridge_vw_pairs_fast(y0_cands, w0_cands, bridge_specs, alt_matches_per_w, F_phi):
    if not bridge_specs:
        return None
    if not y0_cands or not w0_cands:
        return {v: set() for v in y0_cands}
    bridge_vw = {}
    for v in y0_cands:
        valid_ws = None
        for terminal_var, shared_vars, per_v_matches, w0_var in bridge_specs:
            pivot_match = per_v_matches.get(v)
            if not pivot_match:
                path_ws = set()
            elif terminal_var == w0_var:
                path_ws = pivot_match.get(w0_var, set()) & w0_cands
            elif not shared_vars:
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

def propagate_candidates_dag_fast(compact_graph, forward_steps, backward_steps, F_int):
    if not forward_steps and (not backward_steps):
        return F_int
    cg_out = compact_graph.out_neighbors
    cg_in = compact_graph.in_neighbors
    for parent_var, child_var, edges_info in forward_steps:
        parent_ints = F_int.get(parent_var)
        child_ints = F_int.get(child_var)
        if not parent_ints or not child_ints:
            return None
        reachable_child_ints = set()
        n_child = len(child_ints)
        for edge_label, is_fwd in edges_info:
            if len(parent_ints) <= len(child_ints):
                neighbor_map = cg_out if is_fwd else cg_in
                for p_int in parent_ints:
                    for n_int in neighbor_map.get((p_int, edge_label), ()):
                        if n_int in child_ints:
                            reachable_child_ints.add(n_int)
                    if len(reachable_child_ints) == n_child:
                        break
            else:
                reverse_neighbor_map = cg_in if is_fwd else cg_out
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
            return None
        F_int[child_var] = reachable_child_ints
    for parent_var, child_var, edges_info in backward_steps:
        child_ints = F_int.get(child_var)
        parent_ints = F_int.get(parent_var)
        if not child_ints or not parent_ints:
            return None
        reachable_parent_ints = set()
        n_parent = len(parent_ints)
        for edge_label, is_fwd in edges_info:
            if len(child_ints) <= len(parent_ints):
                reverse_neighbor_map = cg_in if is_fwd else cg_out
                for c_int in child_ints:
                    for n_int in reverse_neighbor_map.get((c_int, edge_label), ()):
                        if n_int in parent_ints:
                            reachable_parent_ints.add(n_int)
                    if len(reachable_parent_ints) == n_parent:
                        break
            else:
                forward_neighbor_map = cg_out if is_fwd else cg_in
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
            return None
        F_int[parent_var] = reachable_parent_ints
    return F_int

def prune_pairwise_values_fast(vals1, vals2, operator):
    valid_z1 = set()
    valid_z2 = set()
    if not vals1 or not vals2:
        return (valid_z1, valid_z2)
    if operator == ComparisonOp.EQ:
        val_to_c2 = {}
        for c2, v2 in vals2.items():
            val_to_c2.setdefault(v2, set()).add(c2)
        for c1, v1 in vals1.items():
            matching_c2s = val_to_c2.get(v1)
            if matching_c2s:
                valid_z1.add(c1)
                valid_z2 |= matching_c2s
        return (valid_z1, valid_z2)
    if operator == ComparisonOp.NE:
        unique_vals2 = set(vals2.values())
        unique_vals1 = set(vals1.values())
        for c1, v1 in vals1.items():
            if len(unique_vals2) > 1 or (unique_vals2 and v1 not in unique_vals2):
                valid_z1.add(c1)
        for c2, v2 in vals2.items():
            if len(unique_vals1) > 1 or (unique_vals1 and v2 not in unique_vals1):
                valid_z2.add(c2)
        return (valid_z1, valid_z2)
    if operator in (ComparisonOp.LT, ComparisonOp.LE):
        try:
            max_v2 = max(vals2.values())
            min_v1 = min(vals1.values())
            if operator == ComparisonOp.LT:
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
            return (valid_z1, valid_z2)
        except TypeError:
            pass
    if operator in (ComparisonOp.GT, ComparisonOp.GE):
        try:
            min_v2 = min(vals2.values())
            max_v1 = max(vals1.values())
            if operator == ComparisonOp.GT:
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
            return (valid_z1, valid_z2)
        except TypeError:
            pass
    for c1, v1 in vals1.items():
        for c2, v2 in vals2.items():
            try:
                if _safe_compare_predicate_values(v1, v2, operator):
                    valid_z1.add(c1)
                    valid_z2.add(c2)
            except (TypeError, ValueError):
                continue
    return (valid_z1, valid_z2)

def filter_dis_items_fast(dis_cache, anchor_id, operator, threshold, candidate_set):
    result = set()
    if operator == '>=':
        for w in candidate_set:
            score = dis_cache.get((anchor_id, w))
            if score is None or score >= threshold:
                result.add(w)
        return result
    if operator == '>':
        for w in candidate_set:
            score = dis_cache.get((anchor_id, w))
            if score is None or score > threshold:
                result.add(w)
        return result
    if operator == '<=':
        for w in candidate_set:
            score = dis_cache.get((anchor_id, w))
            if score is None or score <= threshold:
                result.add(w)
        return result
    if operator == '<':
        for w in candidate_set:
            score = dis_cache.get((anchor_id, w))
            if score is None or score < threshold:
                result.add(w)
        return result
    return set(candidate_set)

def verify_pivot_pairwise_binding_fast(user_attrs, item_attrs, pairwise_plans):
    for lhs_is_user, attr1, rhs_is_user, attr2, operator in pairwise_plans:
        value1 = user_attrs.get(attr1) if lhs_is_user else item_attrs.get(attr1)
        value2 = user_attrs.get(attr2) if rhs_is_user else item_attrs.get(attr2)
        if value1 is None or value2 is None:
            return False
        if not _safe_compare_predicate_values(value1, value2, operator):
            return False
    return True

def cleanup_lazy_heap_fast(heap, active_scores):
    while heap and active_scores.get(heap[0][2]) != heap[0][0]:
        heapq.heappop(heap)

def update_unique_topk_heap_fast(item, score, heap, active_scores, k, counter_state):
    k = max(int(k), 1)

    def _next_counter():
        counter_state[0] += 1
        return counter_state[0]
    current = active_scores.get(item)
    if current is not None:
        if score <= current:
            return
        active_scores[item] = score
        heapq.heappush(heap, (score, _next_counter(), item))
    elif len(active_scores) < k:
        active_scores[item] = score
        heapq.heappush(heap, (score, _next_counter(), item))
    else:
        cleanup_lazy_heap_fast(heap, active_scores)
        if not heap:
            active_scores[item] = score
            heapq.heappush(heap, (score, _next_counter(), item))
        else:
            root_score, _, root_item = heap[0]
            if score <= root_score:
                return
            heapq.heappop(heap)
            active_scores.pop(root_item, None)
            active_scores[item] = score
            heapq.heappush(heap, (score, _next_counter(), item))
    if len(heap) > max(32, 4 * k):
        rebuilt = [(active_score, _next_counter(), active_item) for active_item, active_score in active_scores.items()]
        heap[:] = rebuilt
        heapq.heapify(heap)

def update_heap_and_tau_fast(anchor, item, score, heap, tau_dict, k, active_scores, counter_state):
    update_unique_topk_heap_fast(item, score, heap, active_scores, k, counter_state)
    cleanup_lazy_heap_fast(heap, active_scores)
    if len(active_scores) == max(int(k), 1) and heap:
        tau_dict[anchor] = heap[0][0]
    else:
        tau_dict[anchor] = 0.0

def update_global_best_and_tau_fast(item, score, best_scores, global_heap, global_heap_scores, alt_k, counter_state):
    current = best_scores.get(item)
    if current is not None and score <= current:
        cleanup_lazy_heap_fast(global_heap, global_heap_scores)
        if len(global_heap_scores) == max(int(alt_k), 1) and global_heap:
            return global_heap[0][0]
        return 0.0
    best_scores[item] = score
    update_unique_topk_heap_fast(item, score, global_heap, global_heap_scores, alt_k, counter_state)
    cleanup_lazy_heap_fast(global_heap, global_heap_scores)
    if len(global_heap_scores) == max(int(alt_k), 1) and global_heap:
        return global_heap[0][0]
    return 0.0
