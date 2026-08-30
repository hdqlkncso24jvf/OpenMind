import os
import pickle
from typing import TYPE_CHECKING, Optional, Dict, Tuple, Any, Set
import numpy as np
if TYPE_CHECKING:
    from omr_models import AggrModel

class _MLUserPredictionView:

    def __init__(self, base_cache: 'MLPredicateCache', user_id: Any):
        self._base_cache = base_cache
        self._user_id = user_id
        self._dense_row = None
        dense_scores = getattr(base_cache, 'dense_scores', None)
        dense_user_index = getattr(base_cache, '_dense_user_index', None)
        if dense_scores is not None and dense_user_index is not None:
            user_idx = dense_user_index.get(user_id)
            if user_idx is not None:
                self._dense_row = dense_scores[user_idx]
        self._item_prediction_cache: Dict[Any, float] = {}

    def get_prediction(self, user_id: Any, item_id: Any) -> float:
        if user_id != self._user_id:
            return self._base_cache.get_prediction(user_id, item_id)
        cached = self._item_prediction_cache.get(item_id)
        if cached is not None:
            return cached
        if self._dense_row is not None:
            item_idx = self._base_cache._dense_item_index.get(item_id)
            if item_idx is not None:
                score = float(self._dense_row[item_idx])
                self._item_prediction_cache[item_id] = score
                return score
        score = self._base_cache.get_prediction(user_id, item_id)
        self._item_prediction_cache[item_id] = score
        return score

    def get_dense_user_row(self, user_id: Any) -> Optional[np.ndarray]:
        if user_id != self._user_id:
            return self._base_cache.get_dense_user_row(user_id)
        return self._dense_row

    def has_prediction(self, user_id: Any, item_id: Any) -> bool:
        if user_id != self._user_id:
            return self._base_cache.has_prediction(user_id, item_id)
        if self._dense_row is not None:
            return item_id in self._base_cache._dense_item_index
        return self._base_cache.has_prediction(user_id, item_id)

    def __getattr__(self, name: str) -> Any:
        return getattr(self._base_cache, name)

class MLPredicateCache:

    def __init__(self, cache_path: Optional[str]=None):
        if cache_path and os.path.exists(cache_path):
            with open(cache_path, 'rb') as f:
                loaded = pickle.load(f)
            if isinstance(loaded, dict) and 'cache' in loaded and ('dis_cache' in loaded):
                self.cache = loaded['cache']
                self.dis_cache = loaded.get('dis_cache', {})
                self.aggr_cache = loaded.get('aggr_cache', {})
                self.dense_scores = loaded.get('dense_scores')
                self.dense_user_ids = loaded.get('dense_user_ids', [])
                self.dense_item_ids = loaded.get('dense_item_ids', [])
            elif isinstance(loaded, dict) and ('unary' in loaded or 'binary' in loaded):
                self.cache = loaded
            else:
                self.cache = loaded
        else:
            self.cache = {'unary': {}, 'binary': {}}
        if not hasattr(self, 'dis_cache'):
            self.dis_cache: Dict[Tuple[Any, Any], float] = {}
        if not hasattr(self, 'aggr_cache'):
            self.aggr_cache: Dict[Any, float] = {}
        if not hasattr(self, 'dense_scores'):
            self.dense_scores: Optional[np.ndarray] = None
        if not hasattr(self, 'dense_user_ids'):
            self.dense_user_ids = []
        if not hasattr(self, 'dense_item_ids'):
            self.dense_item_ids = []
        self._rebuild_dense_indices()

    def _rebuild_dense_indices(self):
        self._dense_user_index: Dict[Any, int] = {user_id: idx for idx, user_id in enumerate(self.dense_user_ids)}
        self._dense_item_index: Dict[Any, int] = {item_id: idx for idx, item_id in enumerate(self.dense_item_ids)}

    def get_user_view(self, user_id: Any):
        return _MLUserPredictionView(self, user_id)

    def get_dense_user_row(self, user_id: Any) -> Optional[np.ndarray]:
        if self.dense_scores is None:
            return None
        user_idx = self._dense_user_index.get(user_id)
        if user_idx is None:
            return None
        return self.dense_scores[user_idx]

    def set_dense_rec_scores(self, user_ids, item_ids, scores):
        arr = np.asarray(scores)
        if arr.ndim != 2:
            raise ValueError('dense Mrec scores must be a 2D array')
        if arr.shape != (len(user_ids), len(item_ids)):
            raise ValueError(f'dense Mrec shape mismatch: got {arr.shape}, expected {(len(user_ids), len(item_ids))}')
        for attr_name in ('_topk_dense_scores_cuda', '_topk_dense_full_backend'):
            if hasattr(self, attr_name):
                delattr(self, attr_name)
        self.dense_scores = arr.astype(np.float16, copy=False)
        self.dense_user_ids = list(user_ids)
        self.dense_item_ids = list(item_ids)
        self._rebuild_dense_indices()

    def query(self, model_name: str, user_id: Optional[Any]=None, item_id: Optional[Any]=None, is_binary: bool=True) -> bool:
        if is_binary:
            if user_id is None or item_id is None:
                return True
            key = (user_id, item_id)
            return self.cache['binary'].get(model_name, {}).get(key, True)
        else:
            node_id = user_id if user_id else item_id
            return self.cache['unary'].get(model_name, {}).get(node_id, True)

    def add_binary_prediction(self, model_name: str, user_id: Any, item_id: Any, result: bool):
        if model_name not in self.cache['binary']:
            self.cache['binary'][model_name] = {}
        self.cache['binary'][model_name][user_id, item_id] = result

    def add_unary_prediction(self, model_name: str, node_id: Any, result: bool):
        if model_name not in self.cache['unary']:
            self.cache['unary'][model_name] = {}
        self.cache['unary'][model_name][node_id] = result

    @staticmethod
    def _normalize_prediction_value(value: Any) -> float:
        if value is None:
            return 0.5
        if isinstance(value, bool):
            return 1.0 if value else 0.0
        if isinstance(value, (int, float)):
            return float(value)
        try:
            return float(value)
        except (TypeError, ValueError):
            return 1.0 if value else 0.0

    def get_prediction(self, user_id: Any, item_id: Any) -> float:
        key = (user_id, item_id)
        if isinstance(self.cache, dict) and key in self.cache:
            return self._normalize_prediction_value(self.cache[key])
        if self.dense_scores is not None:
            u_idx = self._dense_user_index.get(user_id)
            i_idx = self._dense_item_index.get(item_id)
            if u_idx is not None and i_idx is not None:
                return float(self.dense_scores[u_idx, i_idx])
        if 'binary' in self.cache:
            for model_name, predictions in self.cache['binary'].items():
                if key in predictions:
                    return self._normalize_prediction_value(predictions[key])
        return 0.5

    def has_prediction(self, user_id: Any, item_id: Any) -> bool:
        key = (user_id, item_id)
        if isinstance(self.cache, dict) and key in self.cache:
            return True
        if self.dense_scores is not None:
            return user_id in self._dense_user_index and item_id in self._dense_item_index
        if isinstance(self.cache, dict) and 'binary' in self.cache:
            for predictions in self.cache['binary'].values():
                if key in predictions:
                    return True
        return False

    def get_dis_score(self, item1_id: Any, item2_id: Any) -> Optional[float]:
        return self.dis_cache.get((item1_id, item2_id))

    def set_dis_score(self, item1_id: Any, item2_id: Any, score: float):
        self.dis_cache[item1_id, item2_id] = score

    def batch_set_dis_scores(self, anchor_id: Any, scores: Dict[Any, float]):
        for alt_id, score in scores.items():
            self.dis_cache[anchor_id, alt_id] = score

    def get_dis_filtered_items(self, anchor_id: Any, operator: str, threshold: float, candidate_set: Set[Any]) -> Set[Any]:
        dis_cache = self.dis_cache
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

    def get_aggr_score(self, item_id: Any) -> Optional[float]:
        return self.aggr_cache.get(item_id)

    def set_aggr_score(self, item_id: Any, score: float):
        self.aggr_cache[item_id] = score

    def precompute_aggr(self, aggr_model: 'AggrModel', item_ids: Set[Any]):
        for item_id in item_ids:
            if item_id not in self.aggr_cache:
                self.aggr_cache[item_id] = aggr_model.compute(item_id)

    def get_aggr_filtered_items(self, operator: str, threshold: float, candidate_set: Set[Any]) -> Set[Any]:
        result = set()
        for w in candidate_set:
            score = self.aggr_cache.get(w)
            if score is None:
                result.add(w)
                continue
            if operator == '>=' and score >= threshold:
                result.add(w)
            elif operator == '>' and score > threshold:
                result.add(w)
            elif operator == '<=' and score <= threshold:
                result.add(w)
            elif operator == '<' and score < threshold:
                result.add(w)
        return result

    def save(self, cache_path: str):
        payload = {'cache': self.cache, 'dis_cache': self.dis_cache, 'aggr_cache': self.aggr_cache, 'dense_scores': self.dense_scores, 'dense_user_ids': self.dense_user_ids, 'dense_item_ids': self.dense_item_ids}
        with open(cache_path, 'wb') as f:
            pickle.dump(payload, f)

    @classmethod
    def load(cls, cache_path: str) -> 'MLPredicateCache':
        instance = cls.__new__(cls)
        with open(cache_path, 'rb') as f:
            data = pickle.load(f)
        if isinstance(data, dict) and 'cache' in data and ('dis_cache' in data):
            instance.cache = data['cache']
            instance.dis_cache = data.get('dis_cache', {})
            instance.aggr_cache = data.get('aggr_cache', {})
            instance.dense_scores = data.get('dense_scores')
            instance.dense_user_ids = data.get('dense_user_ids', [])
            instance.dense_item_ids = data.get('dense_item_ids', [])
        else:
            instance.cache = data
            instance.dis_cache = {}
            instance.aggr_cache = {}
            instance.dense_scores = None
            instance.dense_user_ids = []
            instance.dense_item_ids = []
        instance._rebuild_dense_indices()
        return instance
