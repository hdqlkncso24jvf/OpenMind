import math
import re
import ast
from functools import lru_cache
from collections import Counter, defaultdict
from typing import Any, Dict, FrozenSet, Iterable, List, Optional, Set, Tuple
try:
    from tqdm.auto import tqdm
except Exception:

    def tqdm(iterable, **kwargs):
        return iterable
_ID_ATTRS = frozenset({'name', 'title', 'book_id', 'anime_id', 'game_id', 'username', 'node_id'})
DEFAULT_FBP_BETA = 0.5
OMR_AUC_ALPHA = 0.1
REPORT_CUTOFFS = (1, 5, 10, 15, 20)
_NON_DIVERSITY_ATTRS = frozenset({'score', 'positive_ratio', 'user_reviews', 'members', 'reviews', 'products', 'ratings_count', 'count', 'num_pages', 'release_year', 'keyword_profile', 'theme_profile', 'review_signal', 'author_profile', 'shelf_profile', 'format_profile', 'era', 'author_fame', 'price_tier', 'rating_tier', 'discount_tier', 'episode_tier', 'page_tier', 'popularity_tier', 'score_tier', 'story_tier', 'animation_tier', 'sound_tier', 'character_tier', 'enjoyment_tier'})
_MULTIVALUE_HAMMING_WEIGHT = 0.6
_CATEGORY_SPLIT_RE = re.compile('[|,/;]')

def _get_eval_cache(graph) -> Dict[str, Any]:
    cache = getattr(graph, '_eval_metrics_cache', None)
    if cache is None:
        cache = {'feature_groups': {}, 'topic_tokens': {}, 'hamming': {}, 'fbp_contexts': {}}
        setattr(graph, '_eval_metrics_cache', cache)
    return cache

@lru_cache(maxsize=4096)
def _normalize_div_token_str(text: str) -> str:
    return re.sub('\\s+', '_', text.strip().lower())

def _normalize_div_token(value: Any) -> str:
    if isinstance(value, str):
        return _normalize_div_token_str(value)
    return _normalize_div_token_str(str(value))

@lru_cache(maxsize=8192)
def _split_category_tokens_str(text: str) -> Tuple[str, ...]:
    text = text.strip()
    if not text:
        return ()
    if text.startswith('[') and text.endswith(']'):
        try:
            parsed = ast.literal_eval(text)
        except Exception:
            parsed = None
        if parsed is not None:
            return tuple(sorted(_split_category_tokens(parsed)))
    parts = [_normalize_div_token(part) for part in _CATEGORY_SPLIT_RE.split(text) if part.strip()]
    return tuple(sorted({part for part in parts if part}))

def _split_category_tokens(value: Any) -> Set[str]:
    if value is None:
        return set()
    if isinstance(value, (set, frozenset, list, tuple)):
        out: Set[str] = set()
        for item in value:
            out.update(_split_category_tokens(item))
        return out
    if isinstance(value, str):
        return set(_split_category_tokens_str(value))
    return {_normalize_div_token(value)}

def _neighbor_group_tokens(graph, item_id: Any, edge_label: str, group_name: str, attr_name: str='name') -> Set[str]:
    tokens = set()
    for node, _ in graph.get_out_neighbors(item_id, edge_label):
        value = node.attributes.get(attr_name, node.node_id)
        tokens.add(f'{group_name}:{_normalize_div_token(value)}')
    return tokens

def _item_topic_tokens(graph, item_id: Any, topic_attr: str='genre') -> Set[str]:
    cache = _get_eval_cache(graph)['topic_tokens']
    cached = cache.get(item_id)
    if cached is not None:
        return cached
    tokens: Set[str] = set()
    tokens.update(_neighbor_group_tokens(graph, item_id, 'has_genre', 'genre'))
    tokens.update(_neighbor_group_tokens(graph, item_id, 'has_topic', 'topic'))
    if not any((token.startswith('genre:') for token in tokens)):
        node = graph.get_node(item_id)
        if node is not None:
            for genre in _split_category_tokens(node.attributes.get(topic_attr)):
                tokens.add(f'genre:{genre}')
    if not any((token.startswith('topic:') for token in tokens)):
        for tag_node, _ in graph.get_out_neighbors(item_id, 'has_tag'):
            topic = tag_node.attributes.get('topic')
            if topic:
                tokens.add(f'topic:{_normalize_div_token(topic)}')
    cache[item_id] = tokens
    return tokens

def _item_feature_groups(graph, item_id: Any, topic_attr: str='genre') -> Dict[str, Set[str]]:
    cache = _get_eval_cache(graph)['feature_groups']
    cached = cache.get(item_id)
    if cached is not None:
        return cached
    node = graph.get_node(item_id)
    if node is None:
        return {}
    groups: Dict[str, Set[str]] = {}
    for attr_name, value in node.attributes.items():
        if attr_name in _ID_ATTRS or attr_name in _NON_DIVERSITY_ATTRS:
            continue
        if isinstance(value, (int, float)):
            continue
        tokens = _split_category_tokens(value)
        if tokens:
            groups[f'attr:{attr_name}'] = {f'{attr_name}:{token}' for token in tokens}
    genre_tokens = _neighbor_group_tokens(graph, item_id, 'has_genre', 'genre')
    if not genre_tokens:
        genre_tokens = {f'genre:{token}' for token in _split_category_tokens(node.attributes.get(topic_attr))}
    if genre_tokens:
        groups['edge:genre'] = genre_tokens
    topic_tokens = _neighbor_group_tokens(graph, item_id, 'has_topic', 'topic')
    if not topic_tokens:
        topic_tokens = {f'topic:{_normalize_div_token(tag_node.attributes.get('topic'))}' for tag_node, _ in graph.get_out_neighbors(item_id, 'has_tag') if tag_node.attributes.get('topic')}
    if topic_tokens:
        groups['edge:topic'] = topic_tokens
    author_tokens = _neighbor_group_tokens(graph, item_id, 'written_by', 'author')
    if author_tokens:
        groups['edge:author'] = author_tokens
    facet_by_type: Dict[str, Set[str]] = defaultdict(set)
    for facet_node, _ in graph.get_out_neighbors(item_id, 'has_facet'):
        facet_type = facet_node.attributes.get('facet_type', 'facet')
        facet_by_type[str(facet_type)].add(f'{facet_type}:{_normalize_div_token(facet_node.attributes.get('name', facet_node.node_id))}')
    for facet_type, tokens in facet_by_type.items():
        groups[f'edge:facet:{facet_type}'] = tokens
    cache[item_id] = groups
    return groups

def _normalize_raw_hamming_value(value: Any) -> Optional[Tuple[str, ...]]:
    if value is None:
        return None
    if isinstance(value, bool):
        return (str(int(value)),)
    if isinstance(value, (int, float)):
        return None
    tokens = tuple(sorted(_split_category_tokens(value)))
    return tokens or None

def _slot_hamming_distance(value1: Optional[Tuple[str, ...]], value2: Optional[Tuple[str, ...]], *, multivalue: bool=False) -> float:
    if value1 is None and value2 is None:
        return 0.0
    if value1 is None or value2 is None:
        return _MULTIVALUE_HAMMING_WEIGHT if multivalue else 1.0
    if value1 == value2:
        return 0.0
    if not multivalue:
        return 1.0
    set1 = set(value1)
    set2 = set(value2)
    if not set1 and (not set2):
        return 0.0
    union = len(set1 | set2)
    if union == 0:
        return 0.0
    intersection = len(set1 & set2)
    return _MULTIVALUE_HAMMING_WEIGHT * (1.0 - intersection / float(union))

def _get_raw_hamming_label_space(graph, label: str) -> Dict[str, Any]:
    cache = _get_eval_cache(graph).setdefault('raw_hamming_label_spaces', {})
    cached = cache.get(label)
    if cached is not None:
        return cached
    item_attrs: Dict[Any, Dict[str, Tuple[str, ...]]] = {}
    multivalue_attrs: Set[str] = set()
    schema: Set[str] = set()
    for node_id in graph._nodes_by_label.get(label, set()):
        node = graph.get_node(node_id)
        attrs = getattr(node, 'attributes', None)
        if not isinstance(attrs, dict):
            item_attrs[node_id] = {}
            continue
        normalized: Dict[str, Tuple[str, ...]] = {}
        for attr_name, value in attrs.items():
            if attr_name in _ID_ATTRS or attr_name in _NON_DIVERSITY_ATTRS:
                continue
            norm_value = _normalize_raw_hamming_value(value)
            if norm_value is None:
                continue
            normalized[attr_name] = norm_value
            schema.add(attr_name)
            if len(norm_value) > 1:
                multivalue_attrs.add(attr_name)
        item_attrs[node_id] = normalized
    space = {'item_attrs': item_attrs, 'multivalue_attrs': frozenset(multivalue_attrs), 'schema': tuple(sorted(schema))}
    cache[label] = space
    return space

def _pair_cache_key(id1: Any, id2: Any) -> Tuple[Any, Any]:
    if id1 == id2:
        return (id1, id2)
    return (id1, id2) if repr(id1) <= repr(id2) else (id2, id1)

def hamming_distance(graph, id1: Any, id2: Any) -> float:
    cache = _get_eval_cache(graph)['hamming']
    key = _pair_cache_key(id1, id2)
    cached = cache.get(key)
    if cached is not None:
        return cached
    node1 = graph.get_node(id1)
    node2 = graph.get_node(id2)
    if node1 is None or node2 is None:
        return 0.0
    if node1.label != node2.label:
        return 1.0
    space1 = _get_raw_hamming_label_space(graph, node1.label)
    attrs1 = space1['item_attrs'].get(id1, {})
    attrs2 = space1['item_attrs'].get(id2, {})
    multivalue_attrs = space1.get('multivalue_attrs', frozenset())
    schema = space1['schema']
    if not schema:
        return 0.0
    diffs = sum((_slot_hamming_distance(attrs1.get(attr_name), attrs2.get(attr_name), multivalue=attr_name in multivalue_attrs) for attr_name in schema))
    result = max(0.0, min(1.0, diffs / float(len(schema))))
    cache[key] = result
    return result

def hamming_ild(rec_list: List[Any], graph, max_sample: Optional[int]=None) -> float:
    sample = rec_list if max_sample is None else rec_list[:max_sample]
    n_items = len(sample)
    if n_items < 2:
        return 0.0
    pair_sum = sum((hamming_distance(graph, sample[i], sample[j]) for i in range(n_items) for j in range(i + 1, n_items)))
    return 2.0 * pair_sum / (n_items * n_items - 1.0)

def _pairwise_auc(pos_scores: List[float], neg_scores: List[float]) -> Optional[float]:
    if not pos_scores or not neg_scores:
        return None
    n_correct = sum((1 for a in pos_scores for b in neg_scores if a > b))
    n_ties = sum((1 for a in pos_scores for b in neg_scores if a == b))
    return (n_correct + 0.5 * n_ties) / (len(pos_scores) * len(neg_scores))

def _rank_items(item_scores: Dict[Any, float], limit: Optional[int]=None) -> List[Any]:
    ranked = [item_id for item_id, _ in sorted(item_scores.items(), key=lambda item: (-float(item[1]), repr(item[0])))]
    if limit is None:
        return ranked
    return ranked[:max(limit, 0)]

def _dedupe_items(items: Iterable[Any]) -> List[Any]:
    out: List[Any] = []
    seen = set()
    for item in items:
        if item in seen:
            continue
        out.append(item)
        seen.add(item)
    return out

def _cutoff_metric_key(prefix: str, metric_name: str) -> str:
    return f'{prefix}_{metric_name}_At' if prefix else f'{metric_name}_At'

def _make_cutoff_metric_sums(cutoffs: Iterable[int]=REPORT_CUTOFFS) -> Dict[str, Dict[str, float]]:
    ordered = tuple(sorted({int(c) for c in cutoffs if int(c) > 0}))
    return {'precision': {str(c): 0.0 for c in ordered}, 'recall': {str(c): 0.0 for c in ordered}, 'ndcg': {str(c): 0.0 for c in ordered}, 'ild': {str(c): 0.0 for c in ordered}, 'fbp': {str(c): 0.0 for c in ordered}}

def _accumulate_cutoff_rank_metrics(metric_sums: Dict[str, Dict[str, float]], rec_list: List[Any], test_pos: Set[Any], graph, topic_attr: str, user_history: Optional[Set[Any]]=None, fbp_beta: Optional[float]=None, fbp_context: Optional[Dict[str, Any]]=None) -> None:
    for cutoff_key in metric_sums['precision'].keys():
        cutoff = int(cutoff_key)
        topk = rec_list[:cutoff]
        if not topk:
            continue
        hits = set(topk) & test_pos
        metric_sums['precision'][cutoff_key] += len(hits) / len(topk) if topk else 0.0
        metric_sums['recall'][cutoff_key] += len(hits) / len(test_pos) if test_pos else 0.0
        metric_sums['ndcg'][cutoff_key] += _ndcg(topk, test_pos)
        metric_sums['ild'][cutoff_key] += hamming_ild(topk, graph)
        metric_sums['fbp'][cutoff_key] += compute_fbp(topk, user_history or set(), graph, beta=fbp_beta, fbp_context=fbp_context)

def _accumulate_combined_cutoff_metrics(metric_sums: Dict[str, Dict[str, float]], anchor_items: List[Any], psi: Dict[Any, List[Tuple[float, int, Any]]], test_pos: Set[Any], graph, topic_attr: str, user_history: Optional[Set[Any]]=None, fbp_beta: Optional[float]=None, fbp_context: Optional[Dict[str, Any]]=None) -> None:
    for cutoff_key in metric_sums['precision'].keys():
        cutoff = int(cutoff_key)
        cutoff_views = build_recommendation_views(anchor_items=anchor_items, psi=psi, k=cutoff)
        topk = cutoff_views['combined_topk']
        if not topk:
            continue
        hits = set(topk) & test_pos
        metric_sums['precision'][cutoff_key] += len(hits) / len(topk) if topk else 0.0
        metric_sums['recall'][cutoff_key] += len(hits) / len(test_pos) if test_pos else 0.0
        metric_sums['ndcg'][cutoff_key] += _ndcg(topk, test_pos)
        metric_sums['ild'][cutoff_key] += hamming_ild(topk, graph)
        metric_sums['fbp'][cutoff_key] += compute_fbp(topk, user_history or set(), graph, beta=fbp_beta, fbp_context=fbp_context)

def _normalize_cutoff_metric_sums(metric_sums: Dict[str, Dict[str, float]], n_users: int, prefix: str) -> Dict[str, Dict[str, float]]:
    den = max(n_users, 1)
    return {_cutoff_metric_key(prefix, 'Precision'): {cutoff: value / den for cutoff, value in metric_sums['precision'].items()}, _cutoff_metric_key(prefix, 'Recall'): {cutoff: value / den for cutoff, value in metric_sums['recall'].items()}, _cutoff_metric_key(prefix, 'NDCG'): {cutoff: value / den for cutoff, value in metric_sums['ndcg'].items()}, _cutoff_metric_key(prefix, 'ILD'): {cutoff: value / den for cutoff, value in metric_sums['ild'].items()}, _cutoff_metric_key(prefix, 'FBP'): {cutoff: value / den for cutoff, value in metric_sums['fbp'].items()}}

def format_cutoff_series(metric_map: Dict[str, float], precision: int=4, cutoffs: Iterable[int]=REPORT_CUTOFFS) -> str:
    ordered = [str(int(c)) for c in cutoffs if str(int(c)) in metric_map]
    fmt = f'{{:.{precision}f}}'
    return ' / '.join((f'@{cutoff}={fmt.format(metric_map[cutoff])}' for cutoff in ordered))

def build_item_popularity(train_pos_index: Dict[Any, Set[Any]]) -> Counter:
    item_pop: Counter = Counter()
    for items in train_pos_index.values():
        item_pop.update(items)
    return item_pop

def compute_novelty(rec_list: List[Any], item_popularity: Counter, *, n_users: int) -> float:
    if not rec_list:
        return 0.0
    n_users = max(int(n_users), 1)
    denom = math.log(n_users + 1.0)
    if denom <= 0.0:
        return 0.0
    novelty_sum = 0.0
    for item_id in rec_list:
        pop = float(item_popularity.get(item_id, 0))
        prob = (pop + 1.0) / (n_users + 1.0)
        novelty_sum += -math.log(prob) / denom
    return novelty_sum / len(rec_list)

def _tie_fires_for_pair(graph, rule, uid: Any, vid: Any, ml_cache) -> bool:
    from tie_matcher import compute_compact_match, verify_variable_predicates
    pw_preds = rule.get_point_wise_predicates()
    unode = graph.get_node(uid)
    vnode = graph.get_node(vid)
    if unode is None or vnode is None:
        return False
    if unode.label != rule.pattern.nodes[rule.pattern.user_center].label:
        return False
    if vnode.label != rule.pattern.nodes[rule.pattern.item_center].label:
        return False
    for pred in pw_preds.get(rule.pattern.user_center, []):
        if not pred.evaluate(unode):
            return False
    for pred in pw_preds.get(rule.pattern.item_center, []):
        if not pred.evaluate(vnode):
            return False
    for ml_pred in rule.get_ml_predicates():
        score = ml_cache.get_prediction(uid, vid)
        if not ml_pred.evaluate(score):
            return False
    compact = compute_compact_match(graph, rule.pattern, uid, vid, pw_preds)
    if compact is None:
        return False
    pair_wise = rule.get_pair_wise_predicates()
    if pair_wise and (not verify_variable_predicates(graph, compact, pair_wise)):
        return False
    return True

def effective_score(uid: Any, vid: Any, ml_cache, tie_rules, graph, delta_L: float, delta_H: float) -> float:
    raw = ml_cache.get_prediction(uid, vid) or 0.0
    if raw >= delta_H or raw < delta_L:
        return raw
    for rule in tie_rules:
        if _tie_fires_for_pair(graph, rule, uid, vid, ml_cache):
            return delta_H
    return delta_L

def _omr_auc_score(uid: Any, item_id: Any, ml_cache, tie_score: float, alt_best_scores: Dict[Any, float], delta_L: float, delta_H: float, omr_alpha: float) -> float:
    raw = float(ml_cache.get_prediction(uid, item_id) or 0.0)
    if raw < delta_L or raw >= delta_H:
        return tie_score
    return tie_score + omr_alpha * float(alt_best_scores.get(item_id, 0.0))

def _calibrate_fbp_beta(user_history: Iterable[Any], graph) -> float:
    hist = _dedupe_items(user_history)
    if len(hist) < 2:
        return DEFAULT_FBP_BETA
    sims = [1.0 - hamming_distance(graph, hist[i], hist[j]) for i in range(len(hist)) for j in range(i + 1, len(hist))]
    if not sims:
        return DEFAULT_FBP_BETA
    beta = sum(sims) / len(sims)
    return min(1.0, max(0.0, beta))

def _normalize_history_items(user_history: Iterable[Any], max_history: Optional[int]=None) -> Tuple[Any, ...]:
    hist = _dedupe_items(sorted(user_history, key=repr))
    if max_history is not None:
        hist = hist[:max(max_history, 0)]
    return tuple(hist)

def build_fbp_context(user_history: Iterable[Any], graph, beta: Optional[float]=None, max_history: Optional[int]=None) -> Dict[str, Any]:
    hist = _normalize_history_items(user_history, max_history=max_history)
    beta_key = None if beta is None else float(beta)
    cache_key = (hist, beta_key)
    cache = _get_eval_cache(graph)['fbp_contexts']
    cached = cache.get(cache_key)
    if cached is not None:
        return cached
    context = {'history': hist, 'beta': _calibrate_fbp_beta(hist, graph) if beta is None else beta, 'avg_sim': {}}
    cache[cache_key] = context
    return context

def _average_similarity_to_history(item_id: Any, graph, fbp_context: Dict[str, Any]) -> float:
    avg_cache = fbp_context['avg_sim']
    cached = avg_cache.get(item_id)
    if cached is not None:
        return cached
    hist = fbp_context['history']
    if not hist:
        avg_cache[item_id] = 0.0
        return 0.0
    sims = [1.0 - hamming_distance(graph, item_id, hid) for hid in hist]
    avg_sim = sum(sims) / len(sims) if sims else 0.0
    avg_cache[item_id] = avg_sim
    return avg_sim

def compute_fbp(rec_list: List[Any], user_history: Set[Any], graph, beta: Optional[float]=None, max_history: Optional[int]=None, fbp_context: Optional[Dict[str, Any]]=None) -> float:
    if not rec_list:
        return 0.0
    context = fbp_context if fbp_context is not None else build_fbp_context(user_history, graph, beta=beta, max_history=max_history)
    hist = context['history']
    if not hist:
        return 0.0
    beta_value = context['beta']
    out_count = 0
    for vid in rec_list:
        avg_sim = _average_similarity_to_history(vid, graph, context)
        if avg_sim < beta_value:
            out_count += 1
    return out_count / len(rec_list)

def compute_reliability(rec_set: Set[Any], test_pos: Set[Any], test_neg: Set[Any]) -> float:
    test_all = test_pos | test_neg
    if not test_all:
        return 0.0
    tp = len(rec_set & test_pos)
    tn = len(test_neg - rec_set)
    return (tp + tn) / len(test_all)

def compute_fp_rate(rec_set: Set[Any], test_neg: Set[Any]) -> float:
    if not test_neg:
        return 0.0
    return len(rec_set & test_neg) / len(test_neg)

def compute_fn_rate(rec_set: Set[Any], test_pos: Set[Any]) -> float:
    if not test_pos:
        return 0.0
    return len(test_pos - rec_set) / len(test_pos)

def _ndcg(rec_list: List[Any], pos_set: Set[Any]) -> float:
    if not rec_list or not pos_set:
        return 0.0
    dcg = sum((1.0 / math.log2(rank + 2) for rank, item in enumerate(rec_list) if item in pos_set))
    ideal_n = min(len(pos_set), len(rec_list))
    idcg = sum((1.0 / math.log2(rank + 2) for rank in range(ideal_n)))
    return dcg / idcg if idcg > 0 else 0.0

def _count_topics(rec_list: List[Any], graph, topic_attr: str='genre') -> float:
    topics = set()
    for item_id in rec_list:
        topics.update(_item_topic_tokens(graph, item_id, topic_attr))
    return len(topics)

def build_recommendation_views(anchor_items: List[Any], psi: Dict[Any, List[Tuple[float, int, Any]]], k: Optional[int], ml_cache=None, uid: Any=None, candidate_items: Optional[Iterable[Any]]=None, delta_L: float=0.55, delta_H: float=0.8, tie_rules=None, graph=None, omr_alpha: float=OMR_AUC_ALPHA) -> Dict[str, Any]:
    if tie_rules is None:
        tie_rules = []
    anchors = _dedupe_items(anchor_items)
    alt_best_scores: Dict[Any, float] = {}
    alt_best_anchor: Dict[Any, Any] = {}
    for anchor in anchors:
        for entry in sorted(psi.get(anchor, []), reverse=True):
            if not entry:
                continue
            score = float(entry[0])
            alt_item = entry[-1]
            best_score = alt_best_scores.get(alt_item)
            if best_score is None or score > best_score:
                alt_best_scores[alt_item] = score
                alt_best_anchor[alt_item] = anchor
    alt_ranked = [alt_item for alt_item, _ in sorted(alt_best_scores.items(), key=lambda item: (-item[1], repr(item[0])))]
    all_union = []
    all_seen = set()
    for item in anchors + alt_ranked:
        if item in all_seen:
            continue
        all_union.append(item)
        all_seen.add(item)
    if k is None:
        anchor_top = list(anchors)
        alt_top = [item for item in alt_ranked if item not in set(anchor_top)]
    else:
        from toy_openmind_config import TOY_BASELINE_ANCHOR_SPLIT, TOY_BASELINE_HEAD_ANCHOR_SPLIT, TOY_BASELINE_HEAD_K
        split_ratio = TOY_BASELINE_HEAD_ANCHOR_SPLIT if int(k) <= int(TOY_BASELINE_HEAD_K) else TOY_BASELINE_ANCHOR_SPLIT
        anchor_budget = max(int(round(k * split_ratio)), 0)
        if k > 0:
            anchor_budget = min(max(anchor_budget, 1), k)
        anchor_top = anchors[:anchor_budget]
        top_seen = set(anchor_top)
        alt_top = []
        alt_target = max(k - len(anchor_top), 0)
        if alt_target > 0:
            for item in alt_ranked:
                if item in top_seen:
                    continue
                alt_top.append(item)
                top_seen.add(item)
                if len(alt_top) >= alt_target:
                    break
    combined_topk = anchor_top + alt_top
    if k is None:
        combined_full = list(all_union)
    else:
        combined_full = _dedupe_items(combined_topk)
    candidate_list = _dedupe_items(candidate_items or [])
    ml_baseline_scores: Dict[Any, float] = {}
    tie_adjusted_scores: Dict[Any, float] = {}
    omr_boost_scores: Dict[Any, float] = {}
    if ml_cache is not None and uid is not None and candidate_list:
        ml_baseline_scores = {item_id: float(ml_cache.get_prediction(uid, item_id) or 0.0) for item_id in candidate_list}
        if graph is not None:
            tie_adjusted_scores = {item_id: effective_score(uid, item_id, ml_cache, tie_rules, graph, delta_L, delta_H) for item_id in candidate_list}
        else:
            tie_adjusted_scores = dict(ml_baseline_scores)
        omr_boost_scores = {item_id: _omr_auc_score(uid=uid, item_id=item_id, ml_cache=ml_cache, tie_score=tie_adjusted_scores.get(item_id, 0.0), alt_best_scores=alt_best_scores, delta_L=delta_L, delta_H=delta_H, omr_alpha=omr_alpha) for item_id in candidate_list}
    return {'anchor_topk': anchor_top, 'anchor_full_set': set(anchors), 'alt_topk': alt_top, 'combined_topk': combined_topk, 'combined_full': combined_full, 'combined_full_set': set(combined_full), 'combined_all': all_union, 'combined_all_set': set(all_union), 'alt_best_scores': alt_best_scores, 'alt_best_anchor': alt_best_anchor, 'ml_baseline_scores': ml_baseline_scores, 'tie_adjusted_scores': tie_adjusted_scores, 'omr_boost_scores': omr_boost_scores}

def compute_auc_three_layer(uid: Any, test_pos: Set[Any], test_neg: Set[Any], ml_cache, tie_rules, graph, delta_L: float, delta_H: float, alt_best_scores: Dict[Any, float], omr_alpha: float=OMR_AUC_ALPHA, ml_scores: Optional[Dict[Any, float]]=None, tie_scores: Optional[Dict[Any, float]]=None, omr_scores: Optional[Dict[Any, float]]=None) -> Dict[str, Optional[float]]:
    if not test_pos or not test_neg:
        return {'AUC_ML': None, 'AUC_TIE': None, 'AUC_OMR': None}
    if ml_scores is None:
        ml_scores = {item_id: float(ml_cache.get_prediction(uid, item_id) or 0.0) for item_id in test_pos | test_neg}
    if tie_scores is None:
        tie_scores = {item_id: effective_score(uid, item_id, ml_cache, tie_rules, graph, delta_L, delta_H) for item_id in test_pos | test_neg}
    if omr_scores is None:
        omr_scores = {item_id: _omr_auc_score(uid=uid, item_id=item_id, ml_cache=ml_cache, tie_score=tie_scores.get(item_id, 0.0), alt_best_scores=alt_best_scores, delta_L=delta_L, delta_H=delta_H, omr_alpha=omr_alpha) for item_id in test_pos | test_neg}
    ml_auc = _pairwise_auc([ml_scores[item] for item in test_pos], [ml_scores[item] for item in test_neg])
    tie_auc = _pairwise_auc([tie_scores[item] for item in test_pos], [tie_scores[item] for item in test_neg])
    omr_auc = _pairwise_auc([omr_scores[item] for item in test_pos], [omr_scores[item] for item in test_neg])
    return {'AUC_ML': ml_auc, 'AUC_TIE': tie_auc, 'AUC_OMR': omr_auc}

def evaluate_openmind_metrics(fx, user_results, k: int=5, tie_rules=None, delta_L: float=0.55, delta_H: float=0.8, topic_attr: str='genre', omr_auc_alpha: float=OMR_AUC_ALPHA, fbp_beta: Optional[float]=None) -> Dict[str, float]:
    if tie_rules is None:
        tie_rules = []
    n_users = 0
    n_with_recs = 0
    n_with_anchors = 0
    n_with_alts = 0
    n_auc = 0
    ml_s = defaultdict(float)
    tie_s = defaultdict(float)
    anchor_s = defaultdict(float)
    alt_s = defaultdict(float)
    comb_s = defaultdict(float)
    anchor_at = _make_cutoff_metric_sums()
    alt_at = _make_cutoff_metric_sums()
    comb_at = _make_cutoff_metric_sums()
    anchor_hit_at = _make_cutoff_metric_sums()
    alt_hit_at = _make_cutoff_metric_sums()
    comb_hit_at = _make_cutoff_metric_sums()
    item_popularity = build_item_popularity(fx.train_pos_index)
    novelty_user_den = len(getattr(fx, 'all_users', ()) or fx.train_pos_index)
    progress = tqdm(fx.test_users, total=len(fx.test_users), desc=f'{getattr(fx, 'dataset_name', 'dataset')} eval', unit='users', dynamic_ncols=True)
    for uid in progress:
        test_pos = fx.test_pos_index.get(uid, set())
        if not test_pos:
            continue
        test_neg = fx.test_neg_index.get(uid, set())
        test_all = _dedupe_items(sorted(test_pos | test_neg, key=repr))
        train_pos = fx.train_pos_index.get(uid, set())
        user_fbp_context = build_fbp_context(train_pos, fx.graph, beta=fbp_beta)
        n_users += 1
        if hasattr(progress, 'set_postfix'):
            progress.set_postfix_str(f'eligible={n_users}, recs={n_with_recs}', refresh=False)
        K_u, Psi = user_results.get(uid, ([], {}))
        views = build_recommendation_views(anchor_items=list(K_u), psi=Psi, k=k, ml_cache=fx.ml_cache, uid=uid, candidate_items=test_all, delta_L=delta_L, delta_H=delta_H, tie_rules=tie_rules, graph=fx.graph, omr_alpha=omr_auc_alpha)
        anchor_recs = views['anchor_topk']
        alt_recs = views['alt_topk']
        combined_topk = views['combined_topk']
        combined_full = views['combined_full']
        combined_full_set = views['combined_full_set']
        alt_best_scores = views['alt_best_scores']
        alt_best_anchor = views['alt_best_anchor']
        ml_scores = views['ml_baseline_scores']
        tie_scores = views['tie_adjusted_scores']
        omr_scores = views['omr_boost_scores']
        ml_topk = _rank_items(ml_scores, limit=k)
        ml_hits = set(ml_topk) & test_pos
        ml_s['precision'] += len(ml_hits) / len(ml_topk) if ml_topk else 0.0
        ml_s['recall'] += len(ml_hits) / len(test_pos)
        ml_s['ndcg'] += _ndcg(ml_topk, test_pos)
        tie_rec_set = {item_id for item_id, score in tie_scores.items() if score >= delta_H}
        tie_s['fp_rate'] += compute_fp_rate(tie_rec_set, test_neg)
        tie_s['fn_rate'] += compute_fn_rate(tie_rec_set, test_pos)
        if combined_topk:
            n_with_recs += 1
        if anchor_recs:
            n_with_anchors += 1
            anchor_set = set(anchor_recs)
            anchor_hits = anchor_set & test_pos
            anchor_s['hit'] += 1.0 if anchor_hits else 0.0
            anchor_s['click'] += len(anchor_hits)
            anchor_s['prec'] += len(anchor_hits) / len(anchor_recs)
            anchor_s['recall'] += len(anchor_hits) / len(test_pos)
            anchor_s['ndcg'] += _ndcg(anchor_recs, test_pos)
            anchor_s['novelty'] += compute_novelty(anchor_recs, item_popularity, n_users=novelty_user_den)
            anchor_s['divs'] += _count_topics(anchor_recs, fx.graph, topic_attr)
        _accumulate_cutoff_rank_metrics(anchor_at, anchor_recs, test_pos, fx.graph, topic_attr, user_history=train_pos, fbp_beta=fbp_beta, fbp_context=user_fbp_context)
        _accumulate_cutoff_rank_metrics(anchor_hit_at, anchor_recs, test_pos, fx.graph, topic_attr, user_history=train_pos, fbp_beta=fbp_beta, fbp_context=user_fbp_context)
        if alt_recs:
            n_with_alts += 1
            alt_set = set(alt_recs)
            alt_hits = alt_set & test_pos
            alt_s['hit'] += 1.0 if alt_hits else 0.0
            alt_s['click'] += len(alt_hits)
            alt_s['prec'] += len(alt_hits) / len(alt_recs)
            alt_s['recall'] += len(alt_hits) / len(test_pos)
            alt_s['ndcg'] += _ndcg(alt_recs, test_pos)
            alt_s['divs'] += _count_topics(alt_recs, fx.graph, topic_attr)
            alt_s['ild'] += hamming_ild(alt_recs, fx.graph)
            alt_div_values = [fx.dis_model.compute(alt_best_anchor[item], item) for item in alt_recs if item in alt_best_anchor]
            if alt_div_values:
                alt_s['alt_div'] += sum(alt_div_values) / len(alt_div_values)
        _accumulate_cutoff_rank_metrics(alt_at, alt_recs, test_pos, fx.graph, topic_attr, user_history=train_pos, fbp_beta=fbp_beta, fbp_context=user_fbp_context)
        _accumulate_cutoff_rank_metrics(alt_hit_at, alt_recs, test_pos, fx.graph, topic_attr, user_history=train_pos, fbp_beta=fbp_beta, fbp_context=user_fbp_context)
        if combined_topk:
            topk_set = set(combined_topk)
            hits = topk_set & test_pos
            comb_s['hit'] += 1.0 if hits else 0.0
            comb_s['click'] += len(hits)
            comb_s['prec'] += len(hits) / len(combined_topk)
            comb_s['recall'] += len(hits) / len(test_pos)
            comb_s['ndcg'] += _ndcg(combined_topk, test_pos)
            comb_s['novelty'] += compute_novelty(combined_topk, item_popularity, n_users=novelty_user_den)
            comb_s['divs'] += _count_topics(combined_topk, fx.graph, topic_attr)
            comb_s['ild'] += hamming_ild(combined_topk, fx.graph)
        _accumulate_combined_cutoff_metrics(comb_at, list(K_u), Psi, test_pos, fx.graph, topic_attr, user_history=train_pos, fbp_beta=fbp_beta, fbp_context=user_fbp_context)
        _accumulate_combined_cutoff_metrics(comb_hit_at, list(K_u), Psi, test_pos, fx.graph, topic_attr, user_history=train_pos, fbp_beta=fbp_beta, fbp_context=user_fbp_context)
        comb_s['fbp'] += compute_fbp(combined_full, train_pos, fx.graph, beta=fbp_beta, fbp_context=user_fbp_context)
        comb_s['reliability'] += compute_reliability(combined_full_set, test_pos, test_neg)
        comb_s['fp_rate'] += compute_fp_rate(combined_full_set, test_neg)
        comb_s['fn_rate'] += compute_fn_rate(combined_full_set, test_pos)
        aucs = compute_auc_three_layer(uid=uid, test_pos=test_pos, test_neg=test_neg, ml_cache=fx.ml_cache, tie_rules=tie_rules, graph=fx.graph, delta_L=delta_L, delta_H=delta_H, alt_best_scores=alt_best_scores, omr_alpha=omr_auc_alpha, ml_scores=ml_scores, tie_scores=tie_scores, omr_scores=omr_scores)
        if aucs['AUC_ML'] is not None:
            n_auc += 1
            ml_s['auc'] += aucs['AUC_ML']
            tie_s['auc'] += aucs['AUC_TIE']
            comb_s['auc_ml'] += aucs['AUC_ML']
            comb_s['auc_tie'] += aucs['AUC_TIE']
            comb_s['auc_omr'] += aucs['AUC_OMR']
    if hasattr(progress, 'close'):
        progress.close()
    if n_users == 0:
        return {'error': 'no test users'}
    auc_den = n_auc if n_auc else 1
    auc_ml = comb_s['auc_ml'] / auc_den if n_auc else 0.0
    auc_tie = comb_s['auc_tie'] / auc_den if n_auc else 0.0
    auc_omr = comb_s['auc_omr'] / auc_den if n_auc else 0.0
    cutoff_metrics = {}
    cutoff_metrics.update(_normalize_cutoff_metric_sums(anchor_at, n_users, 'Anchor'))
    cutoff_metrics.update(_normalize_cutoff_metric_sums(alt_at, n_users, 'Alt'))
    cutoff_metrics.update(_normalize_cutoff_metric_sums(comb_at, n_users, ''))
    return {'n_test_users': n_users, 'n_with_recs': n_with_recs, 'n_auc_users': n_auc, 'Metric_Cutoffs': [str(c) for c in REPORT_CUTOFFS], 'AUC_User_Coverage': n_auc / n_users, 'OMR_AUC_Alpha': omr_auc_alpha, 'ML_AUC': ml_s['auc'] / auc_den if n_auc else 0.0, 'ML_Precision': ml_s['precision'] / n_users, 'ML_Recall': ml_s['recall'] / n_users, 'ML_NDCG': ml_s['ndcg'] / n_users, 'TIE_AUC': tie_s['auc'] / auc_den if n_auc else 0.0, 'TIE_FP_Rate': tie_s['fp_rate'] / n_users, 'TIE_FN_Rate': tie_s['fn_rate'] / n_users, 'Anchor_Coverage': n_with_anchors / n_users, 'Anchor_HitRate': anchor_s['hit'] / n_users, 'Anchor_Click': anchor_s['click'] / n_users, 'Anchor_Precision': anchor_s['prec'] / n_users, 'Anchor_Recall': anchor_s['recall'] / n_users, 'Anchor_NDCG': anchor_s['ndcg'] / n_users, 'Anchor_Novelty': anchor_s['novelty'] / n_users, 'Anchor_Divs': anchor_s['divs'] / n_users, 'Alt_Coverage': n_with_alts / n_users, 'Alt_HitRate': alt_s['hit'] / n_users, 'Alt_Click': alt_s['click'] / n_users, 'Alt_Precision': alt_s['prec'] / n_users, 'Alt_Recall': alt_s['recall'] / n_users, 'Alt_NDCG': alt_s['ndcg'] / n_users, 'Alt_ILD': alt_s['ild'] / n_users, 'Alt_Diversity': alt_s['alt_div'] / n_users, 'Alt_Divs': alt_s['divs'] / n_users, 'Coverage': n_with_recs / n_users, 'HitRate': comb_s['hit'] / n_users, 'Click': comb_s['click'] / n_users, 'Precision': comb_s['prec'] / n_users, 'Recall': comb_s['recall'] / n_users, 'NDCG': comb_s['ndcg'] / n_users, 'Novelty': comb_s['novelty'] / n_users, 'Divs': comb_s['divs'] / n_users, 'ILD': comb_s['ild'] / n_users, 'FBP': comb_s['fbp'] / n_users, 'Reliability': comb_s['reliability'] / n_users, 'FP_Rate': comb_s['fp_rate'] / n_users, 'FN_Rate': comb_s['fn_rate'] / n_users, 'AUC_ML': auc_ml, 'AUC_TIE': auc_tie, 'AUC_OMR': auc_omr, 'AUC': auc_omr, **cutoff_metrics}

def evaluate_openmind_ctr_metrics(fx, user_results, *, tie_rules=None, delta_L: float=0.55, delta_H: float=0.8, topic_attr: str='genre', omr_auc_alpha: float=OMR_AUC_ALPHA, fbp_beta: Optional[float]=None) -> Dict[str, float]:
    if tie_rules is None:
        tie_rules = []
    n_users = 0
    n_with_recs = 0
    n_auc = 0
    auc_ml_sum = 0.0
    auc_tie_sum = 0.0
    auc_omr_sum = 0.0
    precision_sum = 0.0
    ild_sum = 0.0
    fbp_sum = 0.0
    progress = tqdm(fx.test_users, total=len(fx.test_users), desc=f'{getattr(fx, 'dataset_name', 'dataset')} ctr-eval', unit='users', dynamic_ncols=True)
    for uid in progress:
        test_pos = fx.test_pos_index.get(uid, set())
        if not test_pos:
            continue
        test_neg = fx.test_neg_index.get(uid, set())
        test_all = _dedupe_items(sorted(test_pos | test_neg, key=repr))
        train_pos = fx.train_pos_index.get(uid, set())
        user_fbp_context = build_fbp_context(train_pos, fx.graph, beta=fbp_beta)
        n_users += 1
        K_u, Psi = user_results.get(uid, ([], {}))
        views = build_recommendation_views(anchor_items=list(K_u), psi=Psi, k=None, ml_cache=fx.ml_cache, uid=uid, candidate_items=test_all, delta_L=delta_L, delta_H=delta_H, tie_rules=tie_rules, graph=fx.graph, omr_alpha=omr_auc_alpha)
        displayed = views['combined_full']
        displayed_set = views['combined_full_set']
        if displayed:
            n_with_recs += 1
            hits = displayed_set & test_pos
            precision_sum += len(hits) / len(displayed)
            ild_sum += hamming_ild(displayed, fx.graph)
            fbp_sum += compute_fbp(displayed, train_pos, fx.graph, beta=fbp_beta, fbp_context=user_fbp_context)
        aucs = compute_auc_three_layer(uid=uid, test_pos=test_pos, test_neg=test_neg, ml_cache=fx.ml_cache, tie_rules=tie_rules, graph=fx.graph, delta_L=delta_L, delta_H=delta_H, alt_best_scores=views['alt_best_scores'], omr_alpha=omr_auc_alpha, ml_scores=views['ml_baseline_scores'], tie_scores=views['tie_adjusted_scores'], omr_scores=views['omr_boost_scores'])
        if aucs['AUC_ML'] is not None:
            n_auc += 1
            auc_ml_sum += aucs['AUC_ML']
            auc_tie_sum += aucs['AUC_TIE']
            auc_omr_sum += aucs['AUC_OMR']
    if hasattr(progress, 'close'):
        progress.close()
    if n_users == 0:
        return {'error': 'no test users'}
    auc_den = n_auc if n_auc else 1
    return {'n_test_users': n_users, 'n_with_recs': n_with_recs, 'n_auc_users': n_auc, 'AUC_ML': auc_ml_sum / auc_den if n_auc else 0.0, 'AUC_TIE': auc_tie_sum / auc_den if n_auc else 0.0, 'AUC_OMR': auc_omr_sum / auc_den if n_auc else 0.0, 'AUC': auc_omr_sum / auc_den if n_auc else 0.0, 'Precision': precision_sum / n_users, 'ILD': ild_sum / n_users, 'FBP': fbp_sum / n_users}

def evaluate_openmind_topk_metrics(fx, user_results, *, k: int=10, tie_rules=None, delta_L: float=0.55, delta_H: float=0.8, topic_attr: str='genre', omr_auc_alpha: float=OMR_AUC_ALPHA, fbp_beta: Optional[float]=None) -> Dict[str, float]:
    if tie_rules is None:
        tie_rules = []
    n_users = 0
    n_with_recs = 0
    recall_sum = 0.0
    ndcg_sum = 0.0
    ild_sum = 0.0
    fbp_sum = 0.0
    progress = tqdm(fx.test_users, total=len(fx.test_users), desc=f'{getattr(fx, 'dataset_name', 'dataset')} topk-eval', unit='users', dynamic_ncols=True)
    for uid in progress:
        test_pos = fx.test_pos_index.get(uid, set())
        if not test_pos:
            continue
        test_neg = fx.test_neg_index.get(uid, set())
        test_all = _dedupe_items(sorted(test_pos | test_neg, key=repr))
        train_pos = fx.train_pos_index.get(uid, set())
        user_fbp_context = build_fbp_context(train_pos, fx.graph, beta=fbp_beta)
        n_users += 1
        K_u, Psi = user_results.get(uid, ([], {}))
        views = build_recommendation_views(anchor_items=list(K_u), psi=Psi, k=k, ml_cache=fx.ml_cache, uid=uid, candidate_items=test_all, delta_L=delta_L, delta_H=delta_H, tie_rules=tie_rules, graph=fx.graph, omr_alpha=omr_auc_alpha)
        topk = views['combined_topk']
        if topk:
            n_with_recs += 1
            hits = set(topk) & test_pos
            recall_sum += len(hits) / len(test_pos)
            ndcg_sum += _ndcg(topk, test_pos)
            ild_sum += hamming_ild(topk, fx.graph)
            fbp_sum += compute_fbp(topk, train_pos, fx.graph, beta=fbp_beta, fbp_context=user_fbp_context)
    if hasattr(progress, 'close'):
        progress.close()
    if n_users == 0:
        return {'error': 'no test users'}
    cutoff = str(int(k))
    recall = recall_sum / n_users
    ndcg = ndcg_sum / n_users
    ild = ild_sum / n_users
    fbp = fbp_sum / n_users
    return {'n_test_users': n_users, 'n_with_recs': n_with_recs, 'Metric_Cutoffs': [cutoff], 'Recall': recall, 'NDCG': ndcg, 'ILD': ild, 'FBP': fbp, 'Recall_At': {cutoff: recall}, 'NDCG_At': {cutoff: ndcg}, 'ILD_At': {cutoff: ild}, 'FBP_At': {cutoff: fbp}}
