def filter_and_aggregate(
    dict v_results,
    object aggr_pass_set,
    object dis_model,
    double dis_threshold,
    str dis_operator,
    bint use_dis_index,
    dict users_per_v,
    set valid_users,
):
    cdef dict filtered_v_results = {}
    cdef set all_w_found = set()
    cdef set all_v_with_w = set()
    cdef double dis_score_sum = 0.0
    cdef long dis_count = 0
    cdef dict user_to_vs = {}

    cdef object v_id, w_id, u_id
    cdef set ws, v_users
    cdef tuple v_entry
    cdef set surv_ws
    cdef dict v_bvc, per_w_dis, effective_dis
    cdef long n_users
    cdef double v_dis_sum, dv
    cdef long v_dis_count

    for v_id, v_entry in v_results.items():
        surv_ws = <set>v_entry[0]
        v_bvc = <dict>v_entry[1]
        per_w_dis = <dict>v_entry[2]

        if aggr_pass_set is not None:
            ws = surv_ws & aggr_pass_set
        else:
            ws = surv_ws

        if not ws:
            continue

        effective_dis = per_w_dis
        if use_dis_index and dis_model is not None:
            ws = dis_model.get_items_above_threshold(
                v_id, dis_threshold, ws, dis_operator)

        if not ws:
            continue

        filtered_v_results[v_id] = (ws, v_bvc, effective_dis)

        v_users = users_per_v.get(v_id, set()) & valid_users
        n_users = len(v_users)
        if n_users == 0:
            continue

        all_v_with_w.add(v_id)
        all_w_found.update(ws)

        v_dis_sum = 0.0
        v_dis_count = 0
        for w_id in ws:
            dv_obj = per_w_dis.get(w_id) if per_w_dis else None
            if dv_obj is not None:
                dv = <double>dv_obj
                v_dis_sum += dv
                v_dis_count += 1

        dis_score_sum += v_dis_sum * n_users
        dis_count += v_dis_count * n_users

        for u_id in v_users:
            if u_id not in user_to_vs:
                user_to_vs[u_id] = [v_id]
            else:
                (<list>user_to_vs[u_id]).append(v_id)

    return (filtered_v_results, all_w_found, all_v_with_w,
            dis_score_sum, dis_count, user_to_vs)

def compute_support_from_training_index(
    dict user_to_vs,
    dict filtered_v_results,
    dict training_index,
    dict training_data,
):
    cdef set positive_uw_pairs = set()
    cdef set matched_uw_in_T = set()
    cdef object u_id, w_id, v_id
    cdef list u_vs
    cdef set u_train
    cdef tuple v_data
    cdef set v_ws

    if training_index:
        for u_id, u_vs in user_to_vs.items():
            u_train = training_index.get(u_id)
            if not u_train:
                continue
            for w_id in u_train:
                for v_id in u_vs:
                    v_data = filtered_v_results.get(v_id)
                    if v_data is not None:
                        v_ws = <set>v_data[0]
                        if w_id in v_ws:
                            matched_uw_in_T.add((u_id, w_id))
                            if training_data[(u_id, w_id)]:
                                positive_uw_pairs.add((u_id, w_id))
                            break
    else:
        _user_set = frozenset(user_to_vs)
        for uw_key in training_data:
            u_id = uw_key[0]
            w_id = uw_key[1]
            if u_id not in _user_set:
                continue
            u_vs = user_to_vs[u_id]
            for v_id in u_vs:
                v_data = filtered_v_results.get(v_id)
                if v_data is not None:
                    v_ws = <set>v_data[0]
                    if w_id in v_ws:
                        matched_uw_in_T.add((u_id, w_id))
                        if training_data[uw_key]:
                            positive_uw_pairs.add((u_id, w_id))
                        break

    return positive_uw_pairs, matched_uw_in_T
