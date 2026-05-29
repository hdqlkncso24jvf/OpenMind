from collections import Counter, defaultdict
from typing import Any, Dict, Iterable, List, Optional, Set, Tuple
import numpy as np
from data_structure import Graph, OMRPattern
from omr_models import DisModel, AggrModel, TopicProximityModel
from ml_cache import MLPredicateCache

class RankingScoreComputer:

    def __init__(self, graph: Graph, dis_model: DisModel, aggr_model: AggrModel, ml_cache: MLPredicateCache, lambda_param: float=0.6, sigma_omr_size: int=1, topic_model: Optional[TopicProximityModel]=None):
        self.graph = graph
        self.dis_model = dis_model
        self.aggr_model = aggr_model
        self.ml_cache = ml_cache
        self.lambda_param = lambda_param
        self.sigma_omr_size = max(sigma_omr_size, 1)
        self.topic_model = topic_model
        self._sim_t_cache: Dict[Tuple[Any, Any], float] = {}
        self._mdis_cache: Dict[Tuple[Any, Any], float] = {}
        self._s_acc_cache: Dict[Tuple[Any, Any], float] = {}
        self._maggr_cache: Dict[Any, float] = {}
        self._dense_s_acc_user: Any = None
        self._dense_s_acc_row: Optional[np.ndarray] = None
        self._dense_item_index = getattr(ml_cache, '_dense_item_index', None)

    def _get_sim_t(self, v: Any, w: Any) -> float:
        key = (v, w)
        if key not in self._sim_t_cache:
            if self.topic_model is not None:
                self._sim_t_cache[key] = self.topic_model.compute(v, w)
            else:
                mdis = self._get_mdis(v, w)
                self._sim_t_cache[key] = 1.0 - mdis
        return self._sim_t_cache[key]

    def _get_mdis(self, v: Any, w: Any) -> float:
        key = (v, w)
        if key not in self._mdis_cache:
            self._mdis_cache[key] = self.dis_model.compute(v, w)
        return self._mdis_cache[key]

    def _get_mdis_batch(self, v: Any, ws: Iterable[Any]) -> Tuple[List[Any], np.ndarray]:
        ws_list = list(ws)
        if not ws_list:
            return ([], np.empty(0, dtype=np.float32))
        scores = np.empty(len(ws_list), dtype=np.float32)
        missing_ws: List[Any] = []
        missing_pos: List[int] = []
        for pos, w in enumerate(ws_list):
            cached = self._mdis_cache.get((v, w))
            if cached is None:
                missing_ws.append(w)
                missing_pos.append(pos)
            else:
                scores[pos] = float(cached)
        if missing_ws:
            batch_scores = self.dis_model.compute_batch(v, set(missing_ws))
            for pos, w in zip(missing_pos, missing_ws):
                score = float(batch_scores.get(w, self.dis_model.compute(v, w)))
                self._mdis_cache[v, w] = score
                scores[pos] = score
        return (ws_list, scores)

    def _get_sim_t_batch(self, v: Any, ws: Iterable[Any], *, mdis_scores: Optional[np.ndarray]=None) -> Tuple[List[Any], np.ndarray]:
        ws_list = list(ws)
        if not ws_list:
            return ([], np.empty(0, dtype=np.float32))
        if self.topic_model is None:
            if mdis_scores is None:
                ws_list, mdis_scores = self._get_mdis_batch(v, ws_list)
            return (ws_list, np.asarray(1.0 - mdis_scores, dtype=np.float32))
        scores = np.empty(len(ws_list), dtype=np.float32)
        missing_ws: List[Any] = []
        missing_pos: List[int] = []
        for pos, w in enumerate(ws_list):
            cached = self._sim_t_cache.get((v, w))
            if cached is None:
                missing_ws.append(w)
                missing_pos.append(pos)
            else:
                scores[pos] = float(cached)
        if missing_ws:
            if hasattr(self.topic_model, 'compute_batch'):
                batch_scores = self.topic_model.compute_batch(v, set(missing_ws))
            else:
                batch_scores = {w: self.topic_model.compute(v, w) for w in missing_ws}
            for pos, w in zip(missing_pos, missing_ws):
                score = float(batch_scores.get(w, 0.0))
                self._sim_t_cache[v, w] = score
                scores[pos] = score
        return (ws_list, scores)

    def _get_gamma_batch(self, v: Any, ws: Iterable[Any]) -> Tuple[List[Any], np.ndarray]:
        ws_list, mdis_scores = self._get_mdis_batch(v, ws)
        if not ws_list:
            return (ws_list, np.empty(0, dtype=np.float32))
        _ws_list, sim_scores = self._get_sim_t_batch(v, ws_list, mdis_scores=mdis_scores)
        return (_ws_list, np.multiply(sim_scores, mdis_scores, dtype=np.float32))

    def _get_s_acc(self, u: Any, w: Any) -> float:
        key = (u, w)
        if key not in self._s_acc_cache:
            dense_row = None
            if u == self._dense_s_acc_user:
                dense_row = self._dense_s_acc_row
            else:
                dense_row = self._get_dense_s_acc_row(u)
            if dense_row is not None and self._dense_item_index is not None:
                item_idx = self._dense_item_index.get(w)
                if item_idx is not None:
                    self._s_acc_cache[key] = float(dense_row[item_idx])
                    return self._s_acc_cache[key]
            mrec = self.ml_cache.get_prediction(u, w)
            maggr = self._get_maggr(w)
            self._s_acc_cache[key] = mrec * maggr
        return self._s_acc_cache[key]

    def _get_maggr(self, w: Any) -> float:
        if w not in self._maggr_cache:
            self._maggr_cache[w] = self.aggr_model.compute(w)
        return self._maggr_cache[w]

    def _get_dense_s_acc_row(self, u: Any) -> Optional[np.ndarray]:
        if self._dense_s_acc_user == u and self._dense_s_acc_row is not None:
            return self._dense_s_acc_row
        if self._dense_item_index is None:
            return None
        get_dense_user_row = getattr(self.ml_cache, 'get_dense_user_row', None)
        if not callable(get_dense_user_row):
            return None
        dense_user_row = get_dense_user_row(u)
        if dense_user_row is None:
            return None
        dense_item_ids = getattr(self.ml_cache, 'dense_item_ids', None)
        if not dense_item_ids:
            return None
        cache_key = id(dense_item_ids)
        dense_aggr_cache_key = getattr(self.aggr_model, '_dense_aggr_vector_cache_key', None)
        dense_aggr_vector = getattr(self.aggr_model, '_dense_aggr_vector_cache', None)
        if dense_aggr_cache_key != cache_key or dense_aggr_vector is None:
            dense_aggr_vector = np.fromiter((self.aggr_model.compute(item_id) for item_id in dense_item_ids), dtype=np.float32, count=len(dense_item_ids))
            try:
                setattr(self.aggr_model, '_dense_aggr_vector_cache_key', cache_key)
                setattr(self.aggr_model, '_dense_aggr_vector_cache', dense_aggr_vector)
            except Exception:
                pass
        self._dense_s_acc_user = u
        self._dense_s_acc_row = np.multiply(np.asarray(dense_user_row, dtype=np.float32), np.asarray(dense_aggr_vector, dtype=np.float32), dtype=np.float32)
        return self._dense_s_acc_row

    def prepare_user(self, u: Any) -> None:
        _ = self._get_dense_s_acc_row(u)

    @staticmethod
    def _flatten_attr_values(value: Any) -> Tuple[Any, ...]:
        if value is None:
            return ()
        if isinstance(value, (list, tuple, set, frozenset)):
            flattened = []
            for part in value:
                flattened.extend(RankingScoreComputer._flatten_attr_values(part))
            return tuple(flattened)
        return (value,)

    @staticmethod
    def _get_feature_stats(graph: Graph) -> Dict[str, Dict[str, Dict[str, Any]]]:
        cached = getattr(graph, '_ranking_delta_feature_stats_cache', None)
        if cached is not None:
            return cached
        stats: Dict[str, Dict[str, Dict[str, Any]]] = {}
        for label, node_ids in getattr(graph, '_nodes_by_label', {}).items():
            numeric_values: Dict[str, list] = defaultdict(list)
            categorical_values: Dict[str, Counter] = defaultdict(Counter)
            for node_id in node_ids:
                node = graph.get_node(node_id)
                attrs = getattr(node, 'attributes', None)
                if not isinstance(attrs, dict):
                    continue
                for attr, value in attrs.items():
                    if value is None:
                        continue
                    if isinstance(value, bool):
                        numeric_values[attr].append(1.0 if value else 0.0)
                    elif isinstance(value, (int, float)):
                        numeric_values[attr].append(float(value))
                    else:
                        for token in RankingScoreComputer._flatten_attr_values(value):
                            categorical_values[attr][repr(token)] += 1
            label_stats: Dict[str, Dict[str, Any]] = {}
            for attr, values in numeric_values.items():
                label_stats[attr] = {'kind': 'numeric', 'min': min(values) if values else 0.0, 'max': max(values) if values else 0.0}
            for attr, counts in categorical_values.items():
                label_stats[attr] = {'kind': 'categorical', 'counts': dict(counts), 'max_count': max(counts.values()) if counts else 1}
            stats[label] = label_stats
        try:
            setattr(graph, '_ranking_delta_feature_stats_cache', stats)
        except Exception:
            pass
        return stats

    @staticmethod
    def _rule_attr_refs(pattern: OMRPattern, preconditions: Optional[list]) -> Tuple[Tuple[str, str], ...]:
        from data_structure import PairWisePredicate as PWP2
        from data_structure import PointWisePredicate as PWP1
        pivots = {pattern.user_center, pattern.anchor_center, pattern.alt_center}
        refs_by_var: Dict[str, Set[str]] = defaultdict(set)
        for pred in preconditions or ():
            if isinstance(pred, PWP1):
                if pred.variable in pattern.nodes and pred.variable not in pivots:
                    refs_by_var[pred.variable].add(pred.attribute)
            elif isinstance(pred, PWP2):
                if pred.var1 in pattern.nodes and pred.var1 not in pivots:
                    refs_by_var[pred.var1].add(pred.attr1)
                if pred.var2 in pattern.nodes and pred.var2 not in pivots:
                    refs_by_var[pred.var2].add(pred.attr2)
        return tuple(sorted(((var, attr) for var, attrs in refs_by_var.items() for attr in sorted(attrs))))

    @staticmethod
    def get_rule_attr_refs(rule: Any) -> Tuple[Tuple[str, str], ...]:
        cached = getattr(rule, '_cached_attr_refs', None)
        if cached is not None:
            return cached
        refs = RankingScoreComputer._rule_attr_refs(rule.pattern, getattr(rule, 'preconditions', None))
        try:
            setattr(rule, '_cached_attr_refs', refs)
        except Exception:
            pass
        return refs

    @staticmethod
    def _value_importance_for_node(graph: Graph, node_id: Any, attr: str, *, upper_bound: bool) -> Optional[float]:
        node = graph.get_node(node_id)
        if node is None:
            return None
        attrs = getattr(node, 'attributes', None)
        if not isinstance(attrs, dict) or attr not in attrs:
            return None
        stats = RankingScoreComputer._get_feature_stats(graph)
        attr_stats = stats.get(node.label, {}).get(attr)
        if not attr_stats:
            return None
        raw_value = attrs.get(attr)
        if attr_stats['kind'] == 'numeric':
            values = []
            for token in RankingScoreComputer._flatten_attr_values(raw_value):
                if isinstance(token, bool):
                    values.append(1.0 if token else 0.0)
                elif isinstance(token, (int, float)):
                    values.append(float(token))
            if not values:
                return None
            lo = float(attr_stats['min'])
            hi = float(attr_stats['max'])
            if hi <= lo:
                return 1.0
            normalized = [max(0.0, min(1.0, (value - lo) / (hi - lo))) for value in values]
            return max(normalized) if upper_bound else sum(normalized) / len(normalized)
        counts = attr_stats.get('counts', {})
        max_count = max(int(attr_stats.get('max_count', 1)), 1)
        tokens = RankingScoreComputer._flatten_attr_values(raw_value)
        if not tokens:
            return None
        normalized = [counts.get(repr(token), 0) / max_count for token in tokens]
        return max(normalized) if upper_bound else sum(normalized) / len(normalized)

    @staticmethod
    def compute_delta_phi(pattern: OMRPattern, matched_vars: Optional[Dict[str, Set[Any]]]=None, preconditions: Optional[list]=None, graph: Optional[Graph]=None, upper_bound: bool=False, attr_refs: Optional[Tuple[Tuple[str, str], ...]]=None) -> float:
        if attr_refs is None:
            attr_refs = RankingScoreComputer._rule_attr_refs(pattern, preconditions)
        if not attr_refs:
            return 0.0
        total = 0.0
        for var, attr in attr_refs:
            if graph is None:
                total += 0.5
                continue
            if matched_vars is None:
                candidates = graph._nodes_by_label.get(pattern.nodes[var].label, set())
            else:
                candidates = matched_vars.get(var, set())
            if not candidates:
                continue
            scores = []
            for node_id in candidates:
                beta = RankingScoreComputer._value_importance_for_node(graph, node_id, attr, upper_bound=upper_bound)
                if beta is not None:
                    scores.append(beta)
            if not scores:
                continue
            total += max(scores) if upper_bound else sum(scores) / len(scores)
        return max(0.0, min(total, 1.0))

    @staticmethod
    def get_rule_delta_phi(rule: Any, graph: Optional[Graph]=None, matched_vars: Optional[Dict[str, Set[Any]]]=None) -> float:
        if matched_vars is None:
            cached = getattr(rule, '_cached_delta_phi_exact', None)
            if cached is not None:
                cached_graph_id, cached_value = cached
                if graph is None or cached_graph_id == id(graph):
                    return float(cached_value)
        delta = RankingScoreComputer.compute_delta_phi(rule.pattern, matched_vars=matched_vars, preconditions=getattr(rule, 'preconditions', None), graph=graph, attr_refs=RankingScoreComputer.get_rule_attr_refs(rule))
        if matched_vars is None:
            try:
                setattr(rule, '_cached_delta_phi_exact', (id(graph), float(delta)))
            except Exception:
                pass
        return delta

    @staticmethod
    def get_rule_delta_upper_bound(rule: Any, graph: Optional[Graph]=None, matched_vars: Optional[Dict[str, Set[Any]]]=None) -> float:
        if matched_vars is None:
            cached = getattr(rule, '_cached_delta_phi_upper', None)
            if cached is not None:
                cached_graph_id, cached_value = cached
                if graph is None or cached_graph_id == id(graph):
                    return float(cached_value)
        delta = RankingScoreComputer.compute_delta_phi(rule.pattern, matched_vars=matched_vars, preconditions=getattr(rule, 'preconditions', None), graph=graph, upper_bound=True, attr_refs=RankingScoreComputer.get_rule_attr_refs(rule))
        if matched_vars is None:
            try:
                setattr(rule, '_cached_delta_phi_upper', (id(graph), float(delta)))
            except Exception:
                pass
        return delta

    @staticmethod
    def get_rule_presence_weights(rule: Any) -> Tuple[Tuple[str, ...], Tuple[float, ...]]:
        cached = getattr(rule, '_cached_delta_presence_weights', None)
        if cached is not None:
            return cached
        from data_structure import PointWisePredicate as PWP
        pattern = rule.pattern
        pivots = {pattern.user_center, pattern.anchor_center, pattern.alt_center}
        non_pivot_vars = tuple((v for v in pattern.nodes if v not in pivots))
        if not non_pivot_vars:
            cached = ((), ())
            try:
                setattr(rule, '_cached_delta_presence_weights', cached)
            except Exception:
                pass
            return cached
        pred_count: Dict[str, int] = {}
        preconditions = getattr(rule, 'preconditions', None) or ()
        for pred in preconditions:
            if isinstance(pred, PWP) and pred.variable in pattern.nodes:
                pred_count[pred.variable] = pred_count.get(pred.variable, 0) + 1
        beta_base = 1.0 / max(len(non_pivot_vars), 1)
        weights = tuple((max(pred_count.get(var, 0), 1) * beta_base for var in non_pivot_vars))
        cached = (non_pivot_vars, weights)
        try:
            setattr(rule, '_cached_delta_presence_weights', cached)
        except Exception:
            pass
        return cached

    def compute_full(self, u: Any, v: Any, w: Any, omr_rules: list, matched_results: Optional[Dict[int, Dict[str, Set[Any]]]]=None) -> float:
        lam = self.lambda_param
        s_acc = self._get_s_acc(u, w)
        if not omr_rules:
            return (1.0 - lam) * s_acc
        sim_t = self._get_sim_t(v, w)
        mdis = self._get_mdis(v, w)
        s_dis_total = 0.0
        for idx, phi in enumerate(omr_rules):
            if matched_results is None:
                delta = self.get_rule_delta_phi(phi, self.graph)
            else:
                matched = matched_results.get(idx)
                delta = self.get_rule_delta_phi(phi, self.graph, matched)
            s_dis_total += mdis * delta
        s_dis = sim_t * s_dis_total / self.sigma_omr_size
        score = lam * s_dis + (1.0 - lam) * s_acc
        return max(0.0, min(1.0, score))

    def compute_initial(self, u: Any, v: Any, w: Any, phi, matched_vars: Optional[Dict[str, Set[Any]]]=None) -> float:
        lam = self.lambda_param
        s_acc = self._get_s_acc(u, w)
        sim_t = self._get_sim_t(v, w)
        mdis = self._get_mdis(v, w)
        delta = self.get_rule_delta_phi(phi, self.graph, matched_vars)
        s_dis = sim_t * mdis * delta / self.sigma_omr_size
        score = (1.0 - lam) * s_acc + lam * s_dis
        return max(0.0, min(1.0, score))

    def update_incremental(self, current_score: float, u: Any, v: Any, w: Any, phi, matched_vars: Optional[Dict[str, Set[Any]]]=None) -> float:
        lam = self.lambda_param
        sim_t = self._get_sim_t(v, w)
        mdis = self._get_mdis(v, w)
        gamma = sim_t * mdis
        delta = self.get_rule_delta_phi(phi, self.graph, matched_vars)
        increment = lam * gamma * delta / self.sigma_omr_size
        score = current_score + increment
        return max(0.0, min(1.0, score))

    def clear_caches(self):
        self._sim_t_cache.clear()
        self._mdis_cache.clear()
        self._s_acc_cache.clear()
        self._maggr_cache.clear()
        self._dense_s_acc_user = None
        self._dense_s_acc_row = None
