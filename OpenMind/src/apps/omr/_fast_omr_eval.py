def filter_and_aggregate(v_results, aggr_pass_set, dis_model, dis_threshold, dis_operator, use_dis_index, users_per_v, valid_users):
    filtered_v_results = {}
    all_w_found = set()
    all_v_with_w = set()
    dis_score_sum = 0.0
    dis_count = 0
    user_to_vs = {}
    for v_id, (surv_ws, v_bvc, per_w_dis) in v_results.items():
        if aggr_pass_set is not None:
            ws = surv_ws & aggr_pass_set
        else:
            ws = surv_ws
        if not ws:
            continue
        effective_dis = per_w_dis
        if use_dis_index and dis_model is not None:
            ws = dis_model.get_items_above_threshold(v_id, dis_threshold, ws, dis_operator)
        if not ws:
            continue
        filtered_v_results[v_id] = (ws, v_bvc, effective_dis)
        v_users = users_per_v.get(v_id, set()) & valid_users
        n_users = len(v_users)
        if not n_users:
            continue
        all_v_with_w.add(v_id)
        all_w_found.update(ws)
        v_dis_sum = 0.0
        v_dis_count = 0
        for w_id in ws:
            dv = per_w_dis.get(w_id) if per_w_dis else None
            if dv is not None:
                v_dis_sum += dv
                v_dis_count += 1
        dis_score_sum += v_dis_sum * n_users
        dis_count += v_dis_count * n_users
        for u_id in v_users:
            if u_id not in user_to_vs:
                user_to_vs[u_id] = [v_id]
            else:
                user_to_vs[u_id].append(v_id)
    return (filtered_v_results, all_w_found, all_v_with_w, dis_score_sum, dis_count, user_to_vs)

def compute_support_from_training_index(user_to_vs, filtered_v_results, training_index, training_data):
    positive_uw_pairs = set()
    matched_uw_in_T = set()
    if training_index:
        for u_id, u_vs in user_to_vs.items():
            u_train = training_index.get(u_id)
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
    return (positive_uw_pairs, matched_uw_in_T)
