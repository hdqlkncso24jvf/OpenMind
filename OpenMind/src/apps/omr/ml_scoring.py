from __future__ import annotations
import argparse
import ast
import csv
import json
import logging
import math
import os
import pickle
import random
import re
import sys
import time
from collections import Counter, defaultdict
from copy import deepcopy
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Dict, Iterable, List, Optional, Sequence, Set, Tuple
import numpy as np
import torch
import torch.nn as nn
import torch.nn.functional as F
from scipy import sparse
from sklearn.metrics.pairwise import cosine_similarity
from sklearn.metrics import average_precision_score, log_loss, precision_score, recall_score, roc_auc_score
from torch.utils.data import DataLoader, Dataset
from FlagEmbedding import FlagModel
from models import MLPredicateCache
logger = logging.getLogger('ml_scoring')
PROJECT_ROOT = Path(__file__).resolve().parent
DEFAULT_EMBED_MODEL = PROJECT_ROOT / 'Embed' / 'bge-small-en'
DEFAULT_TEXT_BATCH = 32
DEFAULT_TEXT_CHARS = 1800
DEFAULT_USER_REVIEW_SNIPPETS = 3
DEFAULT_ITEM_REVIEW_SNIPPETS = 3
DEFAULT_ITEM_NEIGHBOR_TOPK = 64
DEFAULT_RW_STEPS = 6
DEFAULT_RW_RESTART = 0.2
DEFAULT_DEEPFM_EMBED_DIM = 48
DEFAULT_DEEPFM_HIDDEN = (256, 128, 64)
DEFAULT_DEEPFM_CROSS_LAYERS = 3
DEFAULT_DIS_HIDDEN = 128
DEFAULT_EPOCHS_CTR = 20
DEFAULT_EPOCHS_DIS = 2
DEFAULT_BATCH_SIZE = 256
DEFAULT_SEED = 42
DEFAULT_CTR_EXTRA_NEG_RATIO = 0
DEFAULT_CTR_RANK_EPOCHS = 0
DEFAULT_CTR_HARD_NEG_TOPK = 96
DEFAULT_CTR_HARD_NEG_PER_POS = 2
DEFAULT_CTR_RANDOM_NEG_PER_POS = 1

def _set_seed(seed: int) -> None:
    random.seed(seed)
    np.random.seed(seed)
    torch.manual_seed(seed)
    if torch.cuda.is_available():
        torch.cuda.manual_seed_all(seed)

def _safe_float(value: Any, default: float=0.0) -> float:
    if value is None:
        return default
    if isinstance(value, bool):
        return float(value)
    if isinstance(value, (int, float)):
        return float(value)
    text = str(value).strip()
    if not text:
        return default
    try:
        return float(text)
    except ValueError:
        return default

def _normalize_text(text: Any) -> str:
    if text is None:
        return ''
    normalized = re.sub('\\s+', ' ', str(text)).strip()
    return normalized

def _clip_text(text: str, max_chars: int) -> str:
    text = _normalize_text(text)
    if len(text) <= max_chars:
        return text
    return text[:max_chars - 3].rstrip() + '...'

def _strip_prefix(node_id: Any) -> str:
    text = str(node_id)
    if ':' in text:
        return text.split(':', 1)[1]
    return text

def _prefixed(label: str, raw_id: Any) -> str:
    raw = str(raw_id)
    return raw if raw.startswith(f'{label}:') else f'{label}:{raw}'

def _split_literal_list(raw_value: Any) -> List[str]:
    if raw_value is None:
        return []
    if isinstance(raw_value, (list, tuple, set)):
        out: List[str] = []
        for value in raw_value:
            out.extend(_split_literal_list(value))
        return out
    text = str(raw_value).strip()
    if not text:
        return []
    if text.startswith('[') and text.endswith(']'):
        try:
            value = eval(text, {'__builtins__': {}}, {})
            if isinstance(value, (list, tuple, set)):
                return [str(v).strip() for v in value if str(v).strip()]
        except Exception:
            pass
    return [part.strip() for part in re.split('[|,/;+]', text) if part.strip()]

def _parse_literal_mapping(raw_value: Any) -> Dict[str, Any]:
    if raw_value is None:
        return {}
    if isinstance(raw_value, dict):
        return {str(k): v for k, v in raw_value.items()}
    text = str(raw_value).strip()
    if not text:
        return {}
    try:
        value = ast.literal_eval(text)
    except Exception:
        return {}
    if isinstance(value, dict):
        return {str(k): v for k, v in value.items()}
    return {}

def _top_tokens(values: Sequence[str], k: int) -> str:
    if not values:
        return ''
    counter = Counter((v for v in values if v))
    if not counter:
        return ''
    return ', '.join((token for token, _ in counter.most_common(k)))

@dataclass
class DatasetArtifacts:
    graph: Any
    training_data: Dict[Tuple[Any, Any], bool]
    valid_data: Dict[Tuple[Any, Any], bool]
    test_data: Dict[Tuple[Any, Any], bool]
    dataset_name: str
    item_label: str
    interaction_edge: str
    dis_edge: str

@dataclass
class TextCorpora:
    item_texts: Dict[Any, str]
    user_texts: Dict[Any, str]
    item_titles: Dict[Any, str]
    user_numeric: Dict[Any, np.ndarray]
    item_numeric: Dict[Any, np.ndarray]

class CTRDataset(Dataset):

    def __init__(self, samples: Sequence[Tuple[int, int, float]], user_text: np.ndarray, user_history: np.ndarray, item_text: np.ndarray, user_dense: np.ndarray, item_dense: np.ndarray):
        self.samples = list(samples)
        self.user_text = user_text
        self.user_history = user_history
        self.item_text = item_text
        self.user_dense = user_dense
        self.item_dense = item_dense

    def __len__(self) -> int:
        return len(self.samples)

    def __getitem__(self, idx: int) -> Dict[str, torch.Tensor]:
        u_idx, i_idx, label = self.samples[idx]
        return {'user_idx': torch.tensor(u_idx, dtype=torch.long), 'item_idx': torch.tensor(i_idx, dtype=torch.long), 'user_text': torch.tensor(self.user_text[u_idx], dtype=torch.float32), 'user_history': torch.tensor(self.user_history[u_idx], dtype=torch.float32), 'item_text': torch.tensor(self.item_text[i_idx], dtype=torch.float32), 'user_dense': torch.tensor(self.user_dense[u_idx], dtype=torch.float32), 'item_dense': torch.tensor(self.item_dense[i_idx], dtype=torch.float32), 'label': torch.tensor(label, dtype=torch.float32)}

class PairDistanceDataset(Dataset):

    def __init__(self, pairs: Sequence[Tuple[int, int, float, float]], item_text: np.ndarray):
        self.pairs = list(pairs)
        self.item_text = item_text

    def __len__(self) -> int:
        return len(self.pairs)

    def __getitem__(self, idx: int) -> Dict[str, torch.Tensor]:
        i_idx, j_idx, rw_dist, target = self.pairs[idx]
        return {'left': torch.tensor(self.item_text[i_idx], dtype=torch.float32), 'right': torch.tensor(self.item_text[j_idx], dtype=torch.float32), 'rw_dist': torch.tensor([rw_dist], dtype=torch.float32), 'target': torch.tensor([target], dtype=torch.float32), 'left_idx': torch.tensor(i_idx, dtype=torch.long), 'right_idx': torch.tensor(j_idx, dtype=torch.long)}

class CTRPairwiseDataset(Dataset):

    def __init__(self, triplets: Sequence[Tuple[int, int, int]], user_text: np.ndarray, user_history: np.ndarray, item_text: np.ndarray, user_dense: np.ndarray, item_dense: np.ndarray):
        self.triplets = list(triplets)
        self.user_text = user_text
        self.user_history = user_history
        self.item_text = item_text
        self.user_dense = user_dense
        self.item_dense = item_dense

    def __len__(self) -> int:
        return len(self.triplets)

    def __getitem__(self, idx: int) -> Dict[str, torch.Tensor]:
        u_idx, pos_idx, neg_idx = self.triplets[idx]
        return {'user_idx': torch.tensor(u_idx, dtype=torch.long), 'pos_item_idx': torch.tensor(pos_idx, dtype=torch.long), 'neg_item_idx': torch.tensor(neg_idx, dtype=torch.long), 'user_text': torch.tensor(self.user_text[u_idx], dtype=torch.float32), 'user_history': torch.tensor(self.user_history[u_idx], dtype=torch.float32), 'pos_item_text': torch.tensor(self.item_text[pos_idx], dtype=torch.float32), 'neg_item_text': torch.tensor(self.item_text[neg_idx], dtype=torch.float32), 'user_dense': torch.tensor(self.user_dense[u_idx], dtype=torch.float32), 'pos_item_dense': torch.tensor(self.item_dense[pos_idx], dtype=torch.float32), 'neg_item_dense': torch.tensor(self.item_dense[neg_idx], dtype=torch.float32)}

class CrossLayer(nn.Module):

    def __init__(self, dim: int):
        super().__init__()
        self.weight = nn.Parameter(torch.empty(dim))
        self.bias = nn.Parameter(torch.zeros(dim))
        nn.init.xavier_uniform_(self.weight.unsqueeze(0))

    def forward(self, x0: torch.Tensor, xl: torch.Tensor) -> torch.Tensor:
        cross = torch.sum(xl * self.weight, dim=1, keepdim=True)
        return x0 * cross + self.bias + xl

class RobustDeepFM(nn.Module):

    def __init__(self, n_users: int, n_items: int, text_dim: int, user_dense_dim: int, item_dense_dim: int, embed_dim: int=DEFAULT_DEEPFM_EMBED_DIM, hidden_dims: Sequence[int]=DEFAULT_DEEPFM_HIDDEN, cross_layers: int=DEFAULT_DEEPFM_CROSS_LAYERS, dropout: float=0.15):
        super().__init__()
        self.user_id_emb = nn.Embedding(max(n_users, 1), embed_dim)
        self.item_id_emb = nn.Embedding(max(n_items, 1), embed_dim)
        self.user_id_bias = nn.Embedding(max(n_users, 1), 1)
        self.item_id_bias = nn.Embedding(max(n_items, 1), 1)
        self.user_text_proj = nn.Sequential(nn.Linear(text_dim, embed_dim), nn.LayerNorm(embed_dim), nn.GELU())
        self.user_history_proj = nn.Sequential(nn.Linear(text_dim, embed_dim), nn.LayerNorm(embed_dim), nn.GELU())
        self.item_text_proj = nn.Sequential(nn.Linear(text_dim, embed_dim), nn.LayerNorm(embed_dim), nn.GELU())
        self.user_dense_proj = nn.Sequential(nn.Linear(user_dense_dim, embed_dim), nn.LayerNorm(embed_dim), nn.GELU())
        self.item_dense_proj = nn.Sequential(nn.Linear(item_dense_dim, embed_dim), nn.LayerNorm(embed_dim), nn.GELU())
        dense_pair_dim = 10 + user_dense_dim + item_dense_dim
        self.first_dense = nn.Linear(dense_pair_dim, 1)
        dnn_in = embed_dim * 7 + dense_pair_dim
        self.deep_in_norm = nn.LayerNorm(dnn_in)
        layers: List[nn.Module] = []
        cur_dim = dnn_in
        for hidden in hidden_dims:
            layers.extend([nn.Linear(cur_dim, hidden), nn.BatchNorm1d(hidden), nn.GELU(), nn.Dropout(dropout)])
            cur_dim = hidden
        self.deep = nn.Sequential(*layers)
        self.cross_layers = nn.ModuleList([CrossLayer(dnn_in) for _ in range(max(cross_layers, 1))])
        self.cross_out = nn.Linear(dnn_in, max(hidden_dims[-1], embed_dim))
        self.deep_out = nn.Linear(cur_dim + max(hidden_dims[-1], embed_dim), 1)
        nn.init.xavier_uniform_(self.user_id_emb.weight)
        nn.init.xavier_uniform_(self.item_id_emb.weight)

    @staticmethod
    def _fm(fields: torch.Tensor) -> torch.Tensor:
        summed = fields.sum(dim=1)
        summed_square = summed * summed
        square_summed = (fields * fields).sum(dim=1)
        return 0.5 * (summed_square - square_summed).sum(dim=1, keepdim=True)

    def forward(self, user_idx: torch.Tensor, item_idx: torch.Tensor, user_text: torch.Tensor, user_history: torch.Tensor, item_text: torch.Tensor, user_dense: torch.Tensor, item_dense: torch.Tensor) -> torch.Tensor:
        user_id_vec = self.user_id_emb(user_idx)
        item_id_vec = self.item_id_emb(item_idx)
        user_text_vec = self.user_text_proj(user_text)
        user_history_vec = self.user_history_proj(user_history)
        item_text_vec = self.item_text_proj(item_text)
        user_dense_vec = self.user_dense_proj(user_dense)
        item_dense_vec = self.item_dense_proj(item_dense)
        user_text_norm = F.normalize(user_text, dim=1)
        user_history_norm = F.normalize(user_history, dim=1)
        item_text_norm = F.normalize(item_text, dim=1)
        profile_item_cos = (user_text_norm * item_text_norm).sum(dim=1, keepdim=True)
        history_item_cos = (user_history_norm * item_text_norm).sum(dim=1, keepdim=True)
        profile_history_cos = (user_text_norm * user_history_norm).sum(dim=1, keepdim=True)
        text_l2 = torch.linalg.norm(user_text - item_text, dim=1, keepdim=True)
        history_l2 = torch.linalg.norm(user_history - item_text, dim=1, keepdim=True)
        profile_history_l2 = torch.linalg.norm(user_text - user_history, dim=1, keepdim=True)
        user_norm = torch.linalg.norm(user_text, dim=1, keepdim=True)
        history_norm = torch.linalg.norm(user_history, dim=1, keepdim=True)
        item_norm = torch.linalg.norm(item_text, dim=1, keepdim=True)
        dense_pair = torch.cat([profile_item_cos, history_item_cos, profile_history_cos, text_l2, history_l2, profile_history_l2, user_norm, history_norm, item_norm, torch.sum(user_dense, dim=1, keepdim=True), user_dense, item_dense], dim=1)
        fields = torch.stack([user_id_vec, item_id_vec, user_text_vec, user_history_vec, item_text_vec, user_dense_vec, item_dense_vec], dim=1)
        first = self.user_id_bias(user_idx) + self.item_id_bias(item_idx) + self.first_dense(dense_pair)
        fm = self._fm(fields)
        deep_input = self.deep_in_norm(torch.cat([fields.flatten(1), dense_pair], dim=1))
        deep_hidden = self.deep(deep_input)
        cross_hidden = deep_input
        for layer in self.cross_layers:
            cross_hidden = layer(deep_input, cross_hidden)
        cross_hidden = self.cross_out(cross_hidden)
        deep_logit = self.deep_out(torch.cat([deep_hidden, cross_hidden], dim=1))
        return (first + fm + deep_logit).squeeze(1)

class MDisRegressor(nn.Module):

    def __init__(self, text_dim: int, hidden_dim: int=DEFAULT_DIS_HIDDEN):
        super().__init__()
        self.proj = nn.Sequential(nn.Linear(text_dim, hidden_dim), nn.LayerNorm(hidden_dim), nn.GELU(), nn.Linear(hidden_dim, hidden_dim), nn.GELU())
        self.head = nn.Sequential(nn.Linear(hidden_dim * 3 + 4, hidden_dim), nn.ReLU(), nn.Linear(hidden_dim, hidden_dim // 2), nn.ReLU(), nn.Linear(hidden_dim // 2, 1))

    def forward(self, left: torch.Tensor, right: torch.Tensor, rw_dist: torch.Tensor) -> torch.Tensor:
        zl = F.normalize(self.proj(left), dim=1)
        zr = F.normalize(self.proj(right), dim=1)
        cos = (zl * zr).sum(dim=1, keepdim=True)
        l2 = torch.linalg.norm(zl - zr, dim=1, keepdim=True)
        raw_cos = F.cosine_similarity(left, right, dim=1, eps=1e-06).unsqueeze(1)
        features = torch.cat([zl, zr, torch.abs(zl - zr), cos, l2, raw_cos, rw_dist], dim=1)
        return torch.sigmoid(self.head(features)).squeeze(1)

def detect_dataset_schema(graph: Any) -> Tuple[str, str, str]:
    labels = set(graph._nodes_by_label.keys())
    edge_labels = set(graph._edges_by_label.keys())
    candidate_item_labels = labels - {'user', 'tag', 'genre', 'author', 'category', 'playlist'}
    if not candidate_item_labels:
        candidate_item_labels = labels - {'user'}
    item_label = max(candidate_item_labels, key=lambda l: len(graph._nodes_by_label.get(l, set())))
    interaction_edge = None
    for edge_label in edge_labels:
        edges = graph.get_edges_by_label(edge_label)
        if not edges:
            continue
        sample = edges[0]
        src = graph.get_node(sample.source)
        tgt = graph.get_node(sample.target)
        if src is not None and tgt is not None and (src.label == 'user') and (tgt.label == item_label):
            interaction_edge = edge_label
            break
    if interaction_edge is None:
        interaction_edge = 'plays'
    dis_edge = 'has_tag' if 'has_tag' in edge_labels else None
    if dis_edge is None:
        for edge_label in edge_labels:
            edges = graph.get_edges_by_label(edge_label)
            if not edges:
                continue
            sample = edges[0]
            src = graph.get_node(sample.source)
            tgt = graph.get_node(sample.target)
            if src is not None and tgt is not None and (src.label == item_label) and (tgt.label == 'tag'):
                dis_edge = edge_label
                break
    if dis_edge is None:
        dis_edge = 'has_tag'
    return (item_label, interaction_edge, dis_edge)

def load_artifacts(data_dir: Path) -> DatasetArtifacts:
    with open(data_dir / 'graph.pkl', 'rb') as f:
        graph = pickle.load(f)
    with open(data_dir / 'training_data.pkl', 'rb') as f:
        training_data = pickle.load(f)
    valid_data = {}
    test_data = {}
    if (data_dir / 'valid_data.pkl').exists():
        with open(data_dir / 'valid_data.pkl', 'rb') as f:
            valid_data = pickle.load(f)
    if (data_dir / 'test_data.pkl').exists():
        with open(data_dir / 'test_data.pkl', 'rb') as f:
            test_data = pickle.load(f)
    dataset_name = data_dir.name
    item_label, interaction_edge, dis_edge = detect_dataset_schema(graph)
    return DatasetArtifacts(graph=graph, training_data=training_data, valid_data=valid_data, test_data=test_data, dataset_name=dataset_name, item_label=item_label, interaction_edge=interaction_edge, dis_edge=dis_edge)

def select_entities(artifacts: DatasetArtifacts, max_users: Optional[int], max_items: Optional[int], max_train_pairs: Optional[int]) -> Tuple[List[Any], List[Any], List[Tuple[Any, Any, float]], Dict[Any, Set[Any]]]:
    pos_by_user: Dict[Any, Set[Any]] = defaultdict(set)
    item_freq: Counter = Counter()
    user_freq: Counter = Counter()
    sampled_pairs: List[Tuple[Any, Any, float]] = []
    for (uid, iid), label in artifacts.training_data.items():
        sampled_pairs.append((uid, iid, float(label)))
        if label:
            pos_by_user[uid].add(iid)
            item_freq[iid] += 1
            user_freq[uid] += 1
    sorted_users = sorted(artifacts.graph._nodes_by_label.get('user', set()), key=lambda uid: (-user_freq.get(uid, 0), repr(uid)))
    selected_users = sorted_users[:max_users] if max_users else sorted_users
    selected_user_set = set(selected_users)
    selected_item_set: Set[Any] = set()
    selected_user_pos_items: Set[Any] = set()
    for uid in selected_users:
        selected_user_pos_items.update(pos_by_user.get(uid, set()))
    prioritized_pos_items = sorted(selected_user_pos_items, key=lambda iid: (-item_freq.get(iid, 0), repr(iid)))
    for iid in prioritized_pos_items:
        if max_items and len(selected_item_set) >= max_items:
            break
        selected_item_set.add(iid)
    for iid, _ in item_freq.most_common():
        if max_items and len(selected_item_set) >= max_items:
            break
        selected_item_set.add(iid)
    if not max_items or len(selected_item_set) < max_items:
        all_items = sorted(artifacts.graph._nodes_by_label.get(artifacts.item_label, set()), key=repr)
        for iid in all_items:
            if max_items and len(selected_item_set) >= max_items:
                break
            selected_item_set.add(iid)
    selected_items = sorted(selected_item_set, key=repr)
    selected_item_set = set(selected_items)
    filtered_pairs = [(uid, iid, label) for uid, iid, label in sampled_pairs if uid in selected_user_set and iid in selected_item_set]
    if max_train_pairs and len(filtered_pairs) > max_train_pairs:
        positives = [row for row in filtered_pairs if row[2] > 0.5]
        negatives = [row for row in filtered_pairs if row[2] <= 0.5]
        keep_pos = positives[:max_train_pairs // 2]
        keep_neg = negatives[:max_train_pairs - len(keep_pos)]
        filtered_pairs = keep_pos + keep_neg
    return (selected_users, selected_items, filtered_pairs, pos_by_user)

def _row_text(parts: Iterable[str], max_chars: int) -> str:
    filtered = [_normalize_text(part) for part in parts if _normalize_text(part)]
    if not filtered:
        return ''
    return _clip_text(' [SEP] '.join(filtered), max_chars=max_chars)

def _load_steam_metadata_map(raw_dir: Path, selected_item_raw: Set[str]) -> Dict[str, Dict[str, Any]]:
    meta_map: Dict[str, Dict[str, Any]] = {}
    meta_path = raw_dir / 'games_metadata.json'
    if not meta_path.exists():
        return meta_map
    with open(meta_path, 'r', encoding='utf-8') as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            try:
                record = json.loads(line)
            except Exception:
                continue
            app_id = str(record.get('app_id', ''))
            if app_id in selected_item_raw:
                meta_map[app_id] = record
    return meta_map

def build_steam_corpora(raw_dir: Path, selected_users: List[Any], selected_items: List[Any], positive_history: Dict[Any, Set[Any]], max_chars: int) -> TextCorpora:
    selected_user_raw = {_strip_prefix(uid) for uid in selected_users}
    selected_item_raw = {_strip_prefix(iid) for iid in selected_items}
    item_meta = _load_steam_metadata_map(raw_dir, selected_item_raw)
    item_texts: Dict[Any, str] = {}
    item_titles: Dict[Any, str] = {}
    item_numeric: Dict[Any, np.ndarray] = {}
    with open(raw_dir / 'games.csv', newline='', encoding='utf-8') as f:
        for row in csv.DictReader(f):
            app_id = row.get('app_id', '')
            if app_id not in selected_item_raw:
                continue
            node_id = _prefixed('game', app_id)
            meta = item_meta.get(app_id, {})
            tags = meta.get('tags', []) if isinstance(meta.get('tags'), list) else []
            item_titles[node_id] = _normalize_text(row.get('title', '')) or f'game {app_id}'
            item_texts[node_id] = _row_text([row.get('title', ''), meta.get('description', ''), 'tags: ' + ', '.join(tags[:20]) if tags else '', 'rating: ' + _normalize_text(row.get('rating', '')), 'positive ratio: ' + _normalize_text(row.get('positive_ratio', '')), 'user reviews: ' + _normalize_text(row.get('user_reviews', '')), 'release: ' + _normalize_text(row.get('date_release', ''))], max_chars=max_chars)
            item_numeric[node_id] = np.asarray([math.log1p(_safe_float(row.get('user_reviews'))), _safe_float(row.get('positive_ratio')) / 100.0, math.log1p(_safe_float(row.get('price_final'))), float(str(row.get('win', '')).lower() == 'true'), float(str(row.get('mac', '')).lower() == 'true'), float(str(row.get('linux', '')).lower() == 'true')], dtype=np.float32)
    user_row_map: Dict[str, Dict[str, Any]] = {}
    with open(raw_dir / 'users.csv', newline='', encoding='utf-8') as f:
        for row in csv.DictReader(f):
            raw_id = row.get('user_id', '')
            if raw_id in selected_user_raw:
                user_row_map[raw_id] = row
    item_interaction_stats: Dict[str, Dict[str, float]] = defaultdict(lambda: {'count': 0.0, 'recommended': 0.0, 'hours': 0.0, 'helpful': 0.0, 'funny': 0.0})
    user_interaction_stats: Dict[str, Dict[str, float]] = defaultdict(lambda: {'count': 0.0, 'recommended': 0.0, 'hours': 0.0, 'helpful': 0.0, 'funny': 0.0})
    interaction_notes: Dict[str, List[str]] = defaultdict(list)
    with open(raw_dir / 'recommendations.csv', newline='', encoding='utf-8') as f:
        for row in csv.DictReader(f):
            raw_uid = row.get('user_id', '')
            raw_iid = row.get('app_id', '')
            if raw_uid not in selected_user_raw:
                continue
            if raw_iid not in selected_item_raw:
                continue
            title = item_titles.get(_prefixed('game', raw_iid), f'game {raw_iid}')
            hours = _safe_float(row.get('hours'))
            helpful = _safe_float(row.get('helpful'))
            funny = _safe_float(row.get('funny'))
            recommended = str(row.get('is_recommended', '')).strip().lower()
            recommended_score = float(recommended in {'true', '1', 'yes'})
            item_interaction_stats[raw_iid]['count'] += 1.0
            item_interaction_stats[raw_iid]['recommended'] += recommended_score
            item_interaction_stats[raw_iid]['hours'] += hours
            item_interaction_stats[raw_iid]['helpful'] += helpful
            item_interaction_stats[raw_iid]['funny'] += funny
            user_interaction_stats[raw_uid]['count'] += 1.0
            user_interaction_stats[raw_uid]['recommended'] += recommended_score
            user_interaction_stats[raw_uid]['hours'] += hours
            user_interaction_stats[raw_uid]['helpful'] += helpful
            user_interaction_stats[raw_uid]['funny'] += funny
            interaction_notes[raw_uid].append(_row_text([title, f'hours {hours:.1f}', f'recommended {recommended}', f'helpful {helpful:.0f}', f'funny {funny:.0f}'], max_chars=120))
    for iid in selected_items:
        raw_iid = _strip_prefix(iid)
        stats = item_interaction_stats.get(raw_iid, {})
        count = max(stats.get('count', 0.0), 1.0)
        item_texts[iid] = _row_text([item_texts.get(iid, ''), f'review interactions {int(stats.get('count', 0.0))}', f'recommended ratio {stats.get('recommended', 0.0) / count:.3f}', f'average hours {stats.get('hours', 0.0) / count:.2f}', f'helpful votes {stats.get('helpful', 0.0):.0f}', f'funny votes {stats.get('funny', 0.0):.0f}'], max_chars=max_chars)
        base_numeric = item_numeric.get(iid, np.zeros((6,), dtype=np.float32))
        item_numeric[iid] = np.concatenate([base_numeric, np.asarray([math.log1p(stats.get('count', 0.0)), stats.get('recommended', 0.0) / count, math.log1p(stats.get('hours', 0.0) / count), math.log1p(stats.get('helpful', 0.0)), math.log1p(stats.get('funny', 0.0))], dtype=np.float32)])
    user_texts: Dict[Any, str] = {}
    user_numeric: Dict[Any, np.ndarray] = {}
    for uid in selected_users:
        raw_uid = _strip_prefix(uid)
        row = user_row_map.get(raw_uid, {})
        stats = user_interaction_stats.get(raw_uid, {})
        count = max(stats.get('count', 0.0), 1.0)
        liked_titles = [item_titles.get(iid, _strip_prefix(iid)) for iid in sorted(positive_history.get(uid, set()), key=repr)[:12] if iid in item_titles]
        user_texts[uid] = _row_text([f'user {raw_uid}', f'products {row.get('products', '')}', f'reviews {row.get('reviews', '')}', f'recommended ratio {stats.get('recommended', 0.0) / count:.3f}', f'average hours {stats.get('hours', 0.0) / count:.2f}', 'liked games: ' + ', '.join(liked_titles[:12]) if liked_titles else '', 'interaction notes: ' + ' '.join(interaction_notes.get(raw_uid, [])[:8])], max_chars=max_chars)
        user_numeric[uid] = np.asarray([math.log1p(_safe_float(row.get('products'))), math.log1p(_safe_float(row.get('reviews'))), math.log1p(len(positive_history.get(uid, set()))), stats.get('recommended', 0.0) / count, math.log1p(stats.get('hours', 0.0) / count), math.log1p(stats.get('helpful', 0.0)), math.log1p(stats.get('funny', 0.0))], dtype=np.float32)
    return TextCorpora(item_texts, user_texts, item_titles, user_numeric, item_numeric)

def build_anime_corpora(raw_dir: Path, selected_users: List[Any], selected_items: List[Any], positive_history: Dict[Any, Set[Any]], max_chars: int, max_user_review_snippets: int, max_item_review_snippets: int) -> TextCorpora:
    selected_user_raw = {_strip_prefix(uid) for uid in selected_users}
    selected_item_raw = {_strip_prefix(iid) for iid in selected_items}
    item_texts: Dict[Any, str] = {}
    item_titles: Dict[Any, str] = {}
    item_numeric: Dict[Any, np.ndarray] = {}
    with open(raw_dir / 'animes.csv', newline='', encoding='utf-8') as f:
        for row in csv.DictReader(f):
            raw_iid = row.get('uid', '')
            if raw_iid not in selected_item_raw:
                continue
            iid = _prefixed('anime', raw_iid)
            title = _normalize_text(row.get('title', '')) or f'anime {raw_iid}'
            item_titles[iid] = title
            genres = _split_literal_list(row.get('genre', ''))
            item_texts[iid] = _row_text([title, row.get('synopsis', ''), 'genre: ' + ', '.join(genres[:12]) if genres else '', f'aired {row.get('aired', '')}', f'episodes {row.get('episodes', '')}', f'score {row.get('score', '')}', f'members {row.get('members', '')}', f'ranked {row.get('ranked', '')}', f'popularity {row.get('popularity', '')}'], max_chars=max_chars)
            item_numeric[iid] = np.asarray([math.log1p(_safe_float(row.get('members'))), _safe_float(row.get('score')) / 10.0, math.log1p(_safe_float(row.get('episodes'))), math.log1p(_safe_float(row.get('popularity'))), math.log1p(_safe_float(row.get('ranked')))], dtype=np.float32)
    user_profiles: Dict[str, Dict[str, Any]] = {}
    with open(raw_dir / 'profiles.csv', newline='', encoding='utf-8') as f:
        for row in csv.DictReader(f):
            raw_uid = row.get('profile', '')
            if raw_uid in selected_user_raw:
                user_profiles[raw_uid] = row
    user_reviews: Dict[str, List[str]] = defaultdict(list)
    item_reviews: Dict[str, List[str]] = defaultdict(list)
    item_review_aspects: Dict[str, Dict[str, float]] = defaultdict(lambda: defaultdict(float))
    item_review_counts: Dict[str, int] = defaultdict(int)
    with open(raw_dir / 'reviews.csv', newline='', encoding='utf-8') as f:
        for row in csv.DictReader(f):
            raw_uid = row.get('profile', '')
            raw_iid = row.get('anime_uid', '')
            scores_map = _parse_literal_mapping(row.get('scores', ''))
            aspect_summary = ' '.join((f'{key.lower()} {scores_map[key]}' for key in ('Overall', 'Story', 'Animation', 'Sound', 'Character', 'Enjoyment') if key in scores_map))
            text = _clip_text(_row_text([_normalize_text(row.get('text', '')), aspect_summary, f'overall score {row.get('score', '')}'], max_chars=max_chars // 3), max_chars // 3)
            if text:
                if raw_uid in selected_user_raw and len(user_reviews[raw_uid]) < max_user_review_snippets:
                    user_reviews[raw_uid].append(text)
                if raw_iid in selected_item_raw and len(item_reviews[raw_iid]) < max_item_review_snippets:
                    item_reviews[raw_iid].append(text)
            if raw_iid in selected_item_raw and scores_map:
                for key in ('Overall', 'Story', 'Animation', 'Sound', 'Character', 'Enjoyment'):
                    if key in scores_map:
                        item_review_aspects[raw_iid][key] += _safe_float(scores_map[key])
                item_review_counts[raw_iid] += 1
    for raw_iid, reviews in item_reviews.items():
        iid = _prefixed('anime', raw_iid)
        base = item_texts.get(iid, '')
        aspect_map = item_review_aspects.get(raw_iid, {})
        aspect_count = max(item_review_counts.get(raw_iid, 0), 1)
        aspect_text = ', '.join((f'{key.lower()} {aspect_map.get(key, 0.0) / aspect_count:.2f}' for key in ('Overall', 'Story', 'Animation', 'Sound', 'Character', 'Enjoyment') if key in aspect_map))
        item_texts[iid] = _row_text([base, 'review aspects: ' + aspect_text if aspect_text else '', 'reviews: ' + ' '.join(reviews)], max_chars=max_chars)
        base_numeric = item_numeric.get(iid, np.zeros((5,), dtype=np.float32))
        item_numeric[iid] = np.concatenate([base_numeric, np.asarray([aspect_map.get(key, 0.0) / aspect_count for key in ('Overall', 'Story', 'Animation', 'Sound', 'Character', 'Enjoyment')], dtype=np.float32)])
    user_texts: Dict[Any, str] = {}
    user_numeric: Dict[Any, np.ndarray] = {}
    for uid in selected_users:
        raw_uid = _strip_prefix(uid)
        profile = user_profiles.get(raw_uid, {})
        fav_ids = _split_literal_list(profile.get('favorites_anime', ''))
        fav_titles = [item_titles.get(_prefixed('anime', fav_id), fav_id) for fav_id in fav_ids[:10]]
        hist_titles = [item_titles.get(iid, _strip_prefix(iid)) for iid in sorted(positive_history.get(uid, set()), key=repr)[:12]]
        user_texts[uid] = _row_text([f'profile {raw_uid}', f'gender {profile.get('gender', '')}', f'birthday {profile.get('birthday', '')}', 'favorites: ' + ', '.join(fav_titles) if fav_titles else '', 'history: ' + ', '.join(hist_titles) if hist_titles else '', 'reviews: ' + ' '.join(user_reviews.get(raw_uid, []))], max_chars=max_chars)
        user_numeric[uid] = np.asarray([math.log1p(len(positive_history.get(uid, set()))), float(profile.get('gender', '').lower().startswith('male')), float(profile.get('gender', '').lower().startswith('female')), math.log1p(len(fav_ids)), math.log1p(len(user_reviews.get(raw_uid, [])))], dtype=np.float32)
    return TextCorpora(item_texts, user_texts, item_titles, user_numeric, item_numeric)

def _load_goodreads_author_map(raw_dir: Path) -> Dict[str, str]:
    author_map: Dict[str, str] = {}
    path = raw_dir / 'goodreads_book_authors.json'
    if not path.exists():
        return author_map
    with open(path, 'r', encoding='utf-8') as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            try:
                row = json.loads(line)
            except Exception:
                continue
            author_id = str(row.get('author_id', ''))
            if author_id:
                author_map[author_id] = _normalize_text(row.get('name', '')) or author_id
    return author_map

def _load_goodreads_genre_map(raw_dir: Path, selected_item_raw: Set[str]) -> Dict[str, List[str]]:
    genre_map: Dict[str, List[str]] = {}
    path = raw_dir / 'goodreads_book_genres_initial.json'
    if not path.exists():
        return genre_map
    with open(path, 'r', encoding='utf-8') as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            try:
                row = json.loads(line)
            except Exception:
                continue
            raw_iid = str(row.get('book_id', ''))
            if raw_iid not in selected_item_raw:
                continue
            genres = row.get('genres', {}) or {}
            if isinstance(genres, dict):
                genre_map[raw_iid] = [str(name).strip() for name in genres.keys() if str(name).strip()]
    return genre_map

def _load_goodreads_work_map(raw_dir: Path, work_ids: Set[str]) -> Dict[str, Dict[str, Any]]:
    work_map: Dict[str, Dict[str, Any]] = {}
    path = raw_dir / 'goodreads_book_works.json'
    if not path.exists() or not work_ids:
        return work_map
    with open(path, 'r', encoding='utf-8') as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            try:
                row = json.loads(line)
            except Exception:
                continue
            work_id = str(row.get('work_id', ''))
            if work_id in work_ids:
                work_map[work_id] = row
    return work_map

def build_goodread_corpora(raw_dir: Path, selected_users: List[Any], selected_items: List[Any], positive_history: Dict[Any, Set[Any]], max_chars: int, max_user_review_snippets: int, max_item_review_snippets: int) -> TextCorpora:
    selected_user_raw = {_strip_prefix(uid) for uid in selected_users}
    selected_item_raw = {_strip_prefix(iid) for iid in selected_items}
    author_map = _load_goodreads_author_map(raw_dir)
    genre_map = _load_goodreads_genre_map(raw_dir, selected_item_raw)
    item_texts: Dict[Any, str] = {}
    item_titles: Dict[Any, str] = {}
    item_numeric: Dict[Any, np.ndarray] = {}
    pending_work_ids: Set[str] = set()
    raw_book_rows: Dict[str, Dict[str, Any]] = {}
    with open(raw_dir / 'goodreads_books.json', 'r', encoding='utf-8') as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            try:
                row = json.loads(line)
            except Exception:
                continue
            raw_iid = str(row.get('book_id', ''))
            if raw_iid not in selected_item_raw:
                continue
            raw_book_rows[raw_iid] = row
            work_id = str(row.get('work_id', ''))
            if work_id:
                pending_work_ids.add(work_id)
    work_map = _load_goodreads_work_map(raw_dir, pending_work_ids)
    for raw_iid, row in raw_book_rows.items():
        iid = _prefixed('book', raw_iid)
        work_row = work_map.get(str(row.get('work_id', '')), {})
        title = _normalize_text(row.get('title') or row.get('title_without_series') or f'book {raw_iid}')
        item_titles[iid] = title
        authors = ', '.join((author_map.get(str(a.get('author_id', '')), str(a.get('author_id', ''))) for a in row.get('authors', [])[:5]))
        shelves = ', '.join((str(s.get('name', '')) for s in row.get('popular_shelves', [])[:10]))
        genres = ', '.join(genre_map.get(raw_iid, [])[:10])
        series = ', '.join((_normalize_text(entry.get('title', '')) for entry in row.get('series', [])[:5] if _normalize_text(entry.get('title', ''))))
        rating_dist = _normalize_text(work_row.get('rating_dist', ''))
        item_texts[iid] = _row_text([title, _normalize_text(work_row.get('original_title', '')), row.get('description', ''), f'authors {authors}', f'genres {genres}' if genres else '', f'series {series}' if series else '', f'shelves {shelves}', f'publisher {row.get('publisher', '')}', f'format {row.get('format', '')}', f'language {row.get('language_code', '')}', f'country {row.get('country_code', '')}', f'pages {row.get('num_pages', '')}', f'year {row.get('publication_year', '')}', f'rating {row.get('average_rating', '')}', f'rating distribution {rating_dist}' if rating_dist else ''], max_chars=max_chars)
        item_numeric[iid] = np.asarray([math.log1p(_safe_float(row.get('ratings_count'))), math.log1p(_safe_float(row.get('text_reviews_count'))), _safe_float(row.get('average_rating')) / 5.0, math.log1p(_safe_float(row.get('num_pages'))), math.log1p(_safe_float(work_row.get('ratings_count'))), math.log1p(_safe_float(work_row.get('reviews_count')))], dtype=np.float32)
    user_reviews: Dict[str, List[str]] = defaultdict(list)
    item_reviews: Dict[str, List[str]] = defaultdict(list)
    with open(raw_dir / 'goodreads_reviews_dedup.json', 'r', encoding='utf-8') as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            try:
                row = json.loads(line)
            except Exception:
                continue
            raw_uid = str(row.get('user_id', ''))
            raw_iid = str(row.get('book_id', ''))
            text = _clip_text(_row_text([_normalize_text(row.get('review_text', '')), f'rating {row.get('rating', '')}', f'votes {row.get('n_votes', '')}', f'comments {row.get('n_comments', '')}'], max_chars=max_chars // 3), max_chars // 3)
            if text:
                if raw_uid in selected_user_raw and len(user_reviews[raw_uid]) < max_user_review_snippets:
                    user_reviews[raw_uid].append(text)
                if raw_iid in selected_item_raw and len(item_reviews[raw_iid]) < max_item_review_snippets:
                    item_reviews[raw_iid].append(text)
            if len(user_reviews) >= len(selected_user_raw) and len(item_reviews) >= len(selected_item_raw) and all((len(v) >= max_user_review_snippets for v in user_reviews.values())) and all((len(v) >= max_item_review_snippets for v in item_reviews.values())):
                break
    for raw_iid, reviews in item_reviews.items():
        iid = _prefixed('book', raw_iid)
        base = item_texts.get(iid, '')
        item_texts[iid] = _row_text([base, 'reviews: ' + ' '.join(reviews)], max_chars=max_chars)
    interaction_rows: Dict[str, Dict[str, float]] = defaultdict(lambda: {'count': 0.0, 'rating_sum': 0.0})
    with open(raw_dir / 'goodreads_interactions.csv', newline='', encoding='utf-8') as f:
        for row in csv.DictReader(f):
            raw_uid = row.get('user_id', '')
            raw_iid = row.get('book_id', '')
            if raw_uid not in selected_user_raw or raw_iid not in selected_item_raw:
                continue
            if row.get('is_read') == '1':
                interaction_rows[raw_uid]['count'] += 1.0
            interaction_rows[raw_uid]['rating_sum'] += _safe_float(row.get('rating'))
            interaction_rows[raw_uid]['reviewed'] = interaction_rows[raw_uid].get('reviewed', 0.0) + float(row.get('is_reviewed', '0') == '1')
    user_texts: Dict[Any, str] = {}
    user_numeric: Dict[Any, np.ndarray] = {}
    for uid in selected_users:
        raw_uid = _strip_prefix(uid)
        hist_titles = [item_titles.get(iid, _strip_prefix(iid)) for iid in sorted(positive_history.get(uid, set()), key=repr)[:12]]
        row = interaction_rows.get(raw_uid, {})
        user_texts[uid] = _row_text([f'user {raw_uid}', 'history: ' + ', '.join(hist_titles) if hist_titles else '', 'reviews: ' + ' '.join(user_reviews.get(raw_uid, []))], max_chars=max_chars)
        user_numeric[uid] = np.asarray([math.log1p(len(positive_history.get(uid, set()))), math.log1p(row.get('count', 0.0)), row.get('rating_sum', 0.0) / max(row.get('count', 1.0), 1.0) / 5.0, math.log1p(row.get('reviewed', 0.0))], dtype=np.float32)
    return TextCorpora(item_texts, user_texts, item_titles, user_numeric, item_numeric)

def build_text_corpora(artifacts: DatasetArtifacts, raw_dir: Path, selected_users: List[Any], selected_items: List[Any], positive_history: Dict[Any, Set[Any]], max_chars: int, max_user_review_snippets: int, max_item_review_snippets: int) -> TextCorpora:
    dataset_name = artifacts.dataset_name.lower()
    if dataset_name == 'steam':
        return build_steam_corpora(raw_dir, selected_users, selected_items, positive_history, max_chars)
    if dataset_name == 'anime':
        return build_anime_corpora(raw_dir, selected_users, selected_items, positive_history, max_chars, max_user_review_snippets, max_item_review_snippets)
    if dataset_name == 'goodread':
        return build_goodread_corpora(raw_dir, selected_users, selected_items, positive_history, max_chars, max_user_review_snippets, max_item_review_snippets)
    raise ValueError(f'Unsupported dataset for raw text extraction: {artifacts.dataset_name}')

class FlagTextEmbedder:

    def __init__(self, model_path: Path, device: str):
        self.model_path = str(model_path)
        self.device = device
        self.model = FlagModel(self.model_path, use_fp16=device.startswith('cuda'))

    def encode(self, texts: Sequence[str], batch_size: int=DEFAULT_TEXT_BATCH) -> np.ndarray:
        normalized = [text if text and text.strip() else '[EMPTY]' for text in texts]
        try:
            vectors = self.model.encode(normalized, batch_size=batch_size)
        except TypeError:
            vectors = self.model.encode(normalized)
        return np.asarray(vectors, dtype=np.float32)

def _align_numeric(ids: Sequence[Any], values: Dict[Any, np.ndarray]) -> np.ndarray:
    if not values:
        return np.zeros((len(ids), 1), dtype=np.float32)
    dim = len(next(iter(values.values())))
    matrix = np.zeros((len(ids), dim), dtype=np.float32)
    for idx, node_id in enumerate(ids):
        vec = values.get(node_id)
        if vec is not None:
            matrix[idx] = vec
    return matrix

def _train_model(model: nn.Module, dataloader: DataLoader, loss_fn: Any, optimizer: torch.optim.Optimizer, device: str, epochs: int) -> None:
    model.train()
    for epoch in range(epochs):
        epoch_loss = 0.0
        n_rows = 0
        for batch in dataloader:
            optimizer.zero_grad()
            if 'user_history' in batch:
                logits = model(batch['user_idx'].to(device), batch['item_idx'].to(device), batch['user_text'].to(device), batch['user_history'].to(device), batch['item_text'].to(device), batch['user_dense'].to(device), batch['item_dense'].to(device))
                labels = batch['label'].to(device)
                loss = loss_fn(logits, labels)
            else:
                preds = model(batch['left'].to(device), batch['right'].to(device), batch['rw_dist'].to(device))
                targets = batch['target'].to(device).squeeze(1)
                loss = loss_fn(preds, targets)
            loss.backward()
            optimizer.step()
            batch_rows = next(iter(batch.values())).shape[0]
            epoch_loss += float(loss.item()) * batch_rows
            n_rows += int(batch_rows)

def _standardize_dense_matrix(matrix: np.ndarray) -> np.ndarray:
    if matrix.size == 0:
        return matrix.astype(np.float32, copy=False)
    mean = matrix.mean(axis=0, keepdims=True)
    std = matrix.std(axis=0, keepdims=True)
    std = np.where(std < 1e-06, 1.0, std)
    return ((matrix - mean) / std).astype(np.float32, copy=False)

def _build_user_history_embeddings(selected_users: List[Any], positive_history: Dict[Any, Set[Any]], selected_items: List[Any], item_text_emb: np.ndarray) -> np.ndarray:
    item_to_idx = {iid: idx for idx, iid in enumerate(selected_items)}
    user_history = np.zeros((len(selected_users), item_text_emb.shape[1]), dtype=np.float32)
    for user_idx, uid in enumerate(selected_users):
        hist_indices = [item_to_idx[iid] for iid in sorted(positive_history.get(uid, set()), key=repr) if iid in item_to_idx]
        if not hist_indices:
            continue
        hist_vec = item_text_emb[hist_indices].mean(axis=0)
        norm = float(np.linalg.norm(hist_vec))
        if norm > 1e-06:
            hist_vec = hist_vec / norm
        user_history[user_idx] = hist_vec.astype(np.float32, copy=False)
    return user_history

def _normalized_similarity_matrix(user_emb: np.ndarray, item_emb: np.ndarray) -> np.ndarray:
    user = np.asarray(user_emb, dtype=np.float32)
    item = np.asarray(item_emb, dtype=np.float32)
    user_norm = user / np.clip(np.linalg.norm(user, axis=1, keepdims=True), 1e-06, None)
    item_norm = item / np.clip(np.linalg.norm(item, axis=1, keepdims=True), 1e-06, None)
    sim = user_norm @ item_norm.T
    return np.clip(sim, -1.0, 1.0).astype(np.float32, copy=False)

def _build_user_label_sets(rows: Sequence[Tuple[int, int, float]]) -> Tuple[Dict[int, Set[int]], Dict[int, Set[int]]]:
    positives: Dict[int, Set[int]] = defaultdict(set)
    negatives: Dict[int, Set[int]] = defaultdict(set)
    for u_idx, i_idx, label in rows:
        if label > 0.5:
            positives[u_idx].add(i_idx)
        else:
            negatives[u_idx].add(i_idx)
    return (positives, negatives)

def _build_item_prior_scores(rows: Sequence[Tuple[int, int, float]], n_items: int) -> np.ndarray:
    counts = np.zeros((n_items,), dtype=np.float32)
    for _, i_idx, label in rows:
        if label > 0.5:
            counts[i_idx] += 1.0
    max_count = float(counts.max()) if counts.size else 0.0
    if max_count <= 0.0:
        return counts
    return (np.log1p(counts) / math.log1p(max_count)).astype(np.float32, copy=False)

def _top_negative_candidates_for_users(positive_by_user: Dict[int, Set[int]], n_items: int, history_sim: np.ndarray, profile_sim: np.ndarray, item_prior: np.ndarray, topk: int, base_scores: Optional[np.ndarray]=None) -> Dict[int, List[int]]:
    hard_negatives: Dict[int, List[int]] = {}
    prior_row = np.asarray(item_prior, dtype=np.float32)
    for u_idx, positives in positive_by_user.items():
        if not positives:
            continue
        hardness = 0.55 * ((history_sim[u_idx] + 1.0) * 0.5) + 0.2 * ((profile_sim[u_idx] + 1.0) * 0.5) + 0.1 * prior_row
        if base_scores is not None:
            hardness = hardness + 0.35 * np.asarray(base_scores[u_idx], dtype=np.float32)
        if positives:
            hardness = hardness.copy()
            hardness[np.fromiter(positives, dtype=np.int64, count=len(positives))] = -np.inf
        k = min(max(int(topk), 1), max(n_items - len(positives), 0))
        if k <= 0:
            hard_negatives[u_idx] = []
            continue
        candidate_idx = np.argpartition(hardness, -k)[-k:]
        candidate_idx = candidate_idx[np.argsort(hardness[candidate_idx])[::-1]]
        hard_negatives[u_idx] = [int(idx) for idx in candidate_idx.tolist() if np.isfinite(hardness[idx])]
    return hard_negatives

def _augment_train_pairs_with_hard_negatives(train_rows: Sequence[Tuple[int, int, float]], positive_by_user: Dict[int, Set[int]], hard_negatives: Dict[int, List[int]], n_items: int, extra_neg_ratio: int, seed: int) -> List[Tuple[int, int, float]]:
    if extra_neg_ratio <= 0:
        return list(train_rows)
    rng = random.Random(seed)
    augmented = list(train_rows)
    existing = {(u_idx, i_idx): float(label) for u_idx, i_idx, label in train_rows}
    all_items = list(range(n_items))
    for u_idx, positives in positive_by_user.items():
        if not positives:
            continue
        target_neg = int(max(len(positives) * extra_neg_ratio, 0))
        if target_neg <= 0:
            continue
        added = 0
        for neg_idx in hard_negatives.get(u_idx, []):
            if neg_idx in positives:
                continue
            if existing.get((u_idx, neg_idx), 1.0) > 0.5:
                continue
            if (u_idx, neg_idx) not in existing:
                augmented.append((u_idx, neg_idx, 0.0))
                existing[u_idx, neg_idx] = 0.0
                added += 1
            if added >= target_neg:
                break
        if added >= target_neg:
            continue
        remaining = [idx for idx in all_items if idx not in positives and (u_idx, idx) not in existing]
        rng.shuffle(remaining)
        for neg_idx in remaining[:max(target_neg - added, 0)]:
            augmented.append((u_idx, neg_idx, 0.0))
            existing[u_idx, neg_idx] = 0.0
    rng.shuffle(augmented)
    return augmented

def _build_pairwise_triplets(positive_by_user: Dict[int, Set[int]], hard_negatives: Dict[int, List[int]], n_items: int, hard_neg_per_pos: int, random_neg_per_pos: int, seed: int) -> List[Tuple[int, int, int]]:
    rng = random.Random(seed)
    all_items = list(range(n_items))
    triplets: List[Tuple[int, int, int]] = []
    for u_idx, positives in positive_by_user.items():
        if not positives:
            continue
        hard_pool = [idx for idx in hard_negatives.get(u_idx, []) if idx not in positives]
        random_pool = [idx for idx in all_items if idx not in positives]
        if not hard_pool and (not random_pool):
            continue
        pos_list = list(positives)
        rng.shuffle(pos_list)
        for pos_idx in pos_list:
            used: Set[int] = set()
            for neg_idx in hard_pool[:max(hard_neg_per_pos, 0)]:
                if neg_idx == pos_idx or neg_idx in used:
                    continue
                triplets.append((u_idx, pos_idx, neg_idx))
                used.add(neg_idx)
            if random_neg_per_pos > 0 and random_pool:
                sampled = rng.sample(random_pool, k=min(random_neg_per_pos, len(random_pool)))
                for neg_idx in sampled:
                    if neg_idx == pos_idx or neg_idx in used:
                        continue
                    triplets.append((u_idx, pos_idx, neg_idx))
                    used.add(neg_idx)
    rng.shuffle(triplets)
    return triplets

def _score_row_recall(scores_row: np.ndarray, positives: Set[int], seen: Set[int], k: int) -> float:
    if not positives:
        return 0.0
    candidate_idx = [idx for idx in range(scores_row.shape[0]) if idx not in seen]
    if not candidate_idx:
        return 0.0
    candidate_arr = np.asarray(candidate_idx, dtype=np.int64)
    k = min(max(int(k), 1), candidate_arr.shape[0])
    top_idx = np.argpartition(scores_row[candidate_arr], -k)[-k:]
    ranked = candidate_arr[top_idx[np.argsort(scores_row[candidate_arr][top_idx])[::-1]]]
    hits = sum((1 for idx in ranked.tolist() if idx in positives))
    return float(hits) / float(len(positives))

def _tune_ctr_ranking_blend(base_scores: np.ndarray, train_split: Sequence[Tuple[int, int, float]], val_split: Sequence[Tuple[int, int, float]], user_text_emb: np.ndarray, user_history_emb: np.ndarray, item_text_emb: np.ndarray, item_prior: np.ndarray) -> Dict[str, float]:
    train_positive_by_user, _ = _build_user_label_sets(train_split)
    val_positive_by_user, _ = _build_user_label_sets(val_split)
    if not val_positive_by_user:
        return {'model': 1.0, 'history': 0.0, 'profile': 0.0, 'prior': 0.0}
    val_users = sorted({u_idx for u_idx, _, _ in val_split})
    history_sim = ((_normalized_similarity_matrix(user_history_emb[val_users], item_text_emb) + 1.0) * 0.5).astype(np.float32, copy=False)
    profile_sim = ((_normalized_similarity_matrix(user_text_emb[val_users], item_text_emb) + 1.0) * 0.5).astype(np.float32, copy=False)
    val_user_to_local = {u_idx: pos for pos, u_idx in enumerate(val_users)}
    val_labels = np.asarray([label for _, _, label in val_split], dtype=np.float32)
    base_pair = np.asarray([base_scores[u_idx, i_idx] for u_idx, i_idx, _ in val_split], dtype=np.float32)
    history_pair = np.asarray([history_sim[val_user_to_local[u_idx], i_idx] for u_idx, i_idx, _ in val_split], dtype=np.float32)
    profile_pair = np.asarray([profile_sim[val_user_to_local[u_idx], i_idx] for u_idx, i_idx, _ in val_split], dtype=np.float32)
    prior_pair = np.asarray([item_prior[i_idx] for _, i_idx, _ in val_split], dtype=np.float32)
    best = {'objective': -1.0, 'model': 1.0, 'history': 0.0, 'profile': 0.0, 'prior': 0.0}
    for w_model in (0.65, 0.8, 1.0):
        for w_history in (0.0, 0.2, 0.35):
            for w_profile in (0.0, 0.05):
                for w_prior in (0.0, 0.03):
                    denom = max(w_model + w_history + w_profile + w_prior, 1e-06)
                    recall10_scores: List[float] = []
                    recall20_scores: List[float] = []
                    for u_idx in val_users:
                        local_u = val_user_to_local[u_idx]
                        row = (w_model * np.asarray(base_scores[u_idx], dtype=np.float32) + w_history * history_sim[local_u] + w_profile * profile_sim[local_u] + w_prior * item_prior) / denom
                        recall10_scores.append(_score_row_recall(row, val_positive_by_user[u_idx], train_positive_by_user.get(u_idx, set()), 10))
                        recall20_scores.append(_score_row_recall(row, val_positive_by_user[u_idx], train_positive_by_user.get(u_idx, set()), 20))
                    pair_scores = (w_model * base_pair + w_history * history_pair + w_profile * profile_pair + w_prior * prior_pair) / denom
                    pair_auc = float(roc_auc_score(val_labels, pair_scores)) if len(np.unique(val_labels)) > 1 else 0.5
                    objective = 1.5 * float(np.mean(recall20_scores)) + 1.0 * float(np.mean(recall10_scores)) + 0.05 * pair_auc
                    if objective > float(best['objective']):
                        best = {'objective': objective, 'model': w_model, 'history': w_history, 'profile': w_profile, 'prior': w_prior}
    return {key: float(best[key]) for key in ('model', 'history', 'profile', 'prior')}

def _split_ctr_pairs(encoded_pairs: Sequence[Tuple[int, int, float]], valid_pairs: Sequence[Tuple[int, int, float]], holdout_ratio: float, seed: int) -> Tuple[List[Tuple[int, int, float]], List[Tuple[int, int, float]]]:
    if valid_pairs:
        return (list(encoded_pairs), list(valid_pairs))
    if len(encoded_pairs) < 8:
        return (list(encoded_pairs), list(encoded_pairs))
    positives = [row for row in encoded_pairs if row[2] > 0.5]
    negatives = [row for row in encoded_pairs if row[2] <= 0.5]
    if not positives or not negatives:
        return (list(encoded_pairs), list(encoded_pairs))
    rng = random.Random(seed)
    positives = positives.copy()
    negatives = negatives.copy()
    rng.shuffle(positives)
    rng.shuffle(negatives)
    val_pos = max(1, int(len(positives) * holdout_ratio))
    val_neg = max(1, int(len(negatives) * holdout_ratio))
    train_split = positives[val_pos:] + negatives[val_neg:]
    val_split = positives[:val_pos] + negatives[:val_neg]
    if not train_split or not val_split:
        return (list(encoded_pairs), list(encoded_pairs))
    rng.shuffle(train_split)
    rng.shuffle(val_split)
    return (train_split, val_split)

def _ctr_metrics(labels: np.ndarray, scores: np.ndarray) -> Dict[str, float]:
    labels = np.asarray(labels, dtype=np.float32)
    scores = np.asarray(scores, dtype=np.float32)
    clipped = np.clip(scores, 1e-06, 1.0 - 1e-06)
    preds = (scores >= 0.5).astype(np.int64)
    out = {'loss': float(log_loss(labels, clipped, labels=[0, 1])), 'precision': float(precision_score(labels, preds, zero_division=0)), 'recall': float(recall_score(labels, preds, zero_division=0))}
    if len(np.unique(labels)) > 1:
        out['auc'] = float(roc_auc_score(labels, scores))
        out['pr_auc'] = float(average_precision_score(labels, scores))
    else:
        out['auc'] = 0.5
        out['pr_auc'] = float(labels.mean())
    return out

def _score_ctr_dataset(model: nn.Module, dataset: CTRDataset, device: str, batch_size: int) -> np.ndarray:
    model.eval()
    scores: List[np.ndarray] = []
    with torch.no_grad():
        for batch in DataLoader(dataset, batch_size=batch_size, shuffle=False, drop_last=False):
            logits = model(batch['user_idx'].to(device), batch['item_idx'].to(device), batch['user_text'].to(device), batch['user_history'].to(device), batch['item_text'].to(device), batch['user_dense'].to(device), batch['item_dense'].to(device))
            scores.append(torch.sigmoid(logits).cpu().numpy().astype(np.float32))
    if not scores:
        return np.zeros((0,), dtype=np.float32)
    return np.concatenate(scores, axis=0)

def _train_ctr_ranking(model: RobustDeepFM, triplets: Sequence[Tuple[int, int, int]], user_text_emb: np.ndarray, user_history_emb: np.ndarray, item_text_emb: np.ndarray, user_dense: np.ndarray, item_dense: np.ndarray, device: str, batch_size: int, lr: float, epochs: int) -> None:
    if not triplets or epochs <= 0:
        return
    dataset = CTRPairwiseDataset(triplets, user_text_emb, user_history_emb, item_text_emb, user_dense, item_dense)
    dataloader = DataLoader(dataset, batch_size=batch_size, shuffle=True, drop_last=False)
    optimizer = torch.optim.AdamW(model.parameters(), lr=lr, weight_decay=0.0001)
    bce = nn.BCEWithLogitsLoss()
    for epoch in range(epochs):
        model.train()
        epoch_loss = 0.0
        n_rows = 0
        for batch in dataloader:
            optimizer.zero_grad()
            pos_logits = model(batch['user_idx'].to(device), batch['pos_item_idx'].to(device), batch['user_text'].to(device), batch['user_history'].to(device), batch['pos_item_text'].to(device), batch['user_dense'].to(device), batch['pos_item_dense'].to(device))
            neg_logits = model(batch['user_idx'].to(device), batch['neg_item_idx'].to(device), batch['user_text'].to(device), batch['user_history'].to(device), batch['neg_item_text'].to(device), batch['user_dense'].to(device), batch['neg_item_dense'].to(device))
            rank_loss = F.softplus(-(pos_logits - neg_logits)).mean()
            aux_loss = 0.5 * (bce(pos_logits, torch.ones_like(pos_logits)) + bce(neg_logits, torch.zeros_like(neg_logits)))
            loss = rank_loss + 0.25 * aux_loss
            loss.backward()
            torch.nn.utils.clip_grad_norm_(model.parameters(), max_norm=2.0)
            optimizer.step()
            batch_rows = pos_logits.shape[0]
            epoch_loss += float(loss.item()) * batch_rows
            n_rows += int(batch_rows)

def train_ctr_model(selected_users: List[Any], selected_items: List[Any], train_pairs: Sequence[Tuple[Any, Any, float]], valid_pairs: Sequence[Tuple[Any, Any, float]], user_text_emb: np.ndarray, user_history_emb: np.ndarray, item_text_emb: np.ndarray, user_dense: np.ndarray, item_dense: np.ndarray, device: str, embed_dim: int, hidden_dims: Sequence[int], cross_layers: int, epochs: int, batch_size: int, lr: float, seed: int, early_stop_patience: int, holdout_ratio: float, extra_neg_ratio: int=DEFAULT_CTR_EXTRA_NEG_RATIO, ranking_epochs: int=DEFAULT_CTR_RANK_EPOCHS, hard_neg_topk: int=DEFAULT_CTR_HARD_NEG_TOPK, hard_neg_per_pos: int=DEFAULT_CTR_HARD_NEG_PER_POS, random_neg_per_pos: int=DEFAULT_CTR_RANDOM_NEG_PER_POS) -> Tuple[RobustDeepFM, Dict[Any, int], Dict[Any, int]]:
    user_to_idx = {uid: idx for idx, uid in enumerate(selected_users)}
    item_to_idx = {iid: idx for idx, iid in enumerate(selected_items)}
    encoded_train_pairs = [(user_to_idx[uid], item_to_idx[iid], label) for uid, iid, label in train_pairs if uid in user_to_idx and iid in item_to_idx]
    encoded_valid_pairs = [(user_to_idx[uid], item_to_idx[iid], label) for uid, iid, label in valid_pairs if uid in user_to_idx and iid in item_to_idx]
    if not encoded_train_pairs:
        raise ValueError('No training pairs available after filtering.')
    train_split, val_split = _split_ctr_pairs(encoded_pairs=encoded_train_pairs, valid_pairs=encoded_valid_pairs, holdout_ratio=holdout_ratio, seed=seed)
    train_positive_by_user, _ = _build_user_label_sets(train_split)
    item_prior = _build_item_prior_scores(train_split, len(selected_items))
    history_sim: Optional[np.ndarray] = None
    profile_sim: Optional[np.ndarray] = None
    bootstrap_hard_negatives: Dict[int, List[int]] = {}
    if extra_neg_ratio > 0 or ranking_epochs > 0:
        history_sim = _normalized_similarity_matrix(user_history_emb, item_text_emb)
        profile_sim = _normalized_similarity_matrix(user_text_emb, item_text_emb)
    if extra_neg_ratio > 0 and history_sim is not None and (profile_sim is not None):
        bootstrap_hard_negatives = _top_negative_candidates_for_users(train_positive_by_user, len(selected_items), history_sim, profile_sim, item_prior, topk=hard_neg_topk)
    augmented_train_split = _augment_train_pairs_with_hard_negatives(train_split, train_positive_by_user, bootstrap_hard_negatives, len(selected_items), extra_neg_ratio=extra_neg_ratio, seed=seed)
    train_dataset = CTRDataset(augmented_train_split, user_text_emb, user_history_emb, item_text_emb, user_dense, item_dense)
    val_dataset = CTRDataset(val_split, user_text_emb, user_history_emb, item_text_emb, user_dense, item_dense)
    train_loader = DataLoader(train_dataset, batch_size=batch_size, shuffle=True, drop_last=False)
    model = RobustDeepFM(n_users=len(selected_users), n_items=len(selected_items), text_dim=user_text_emb.shape[1], user_dense_dim=user_dense.shape[1], item_dense_dim=item_dense.shape[1], embed_dim=embed_dim, hidden_dims=hidden_dims, cross_layers=cross_layers).to(device)
    loss_fn = nn.BCEWithLogitsLoss()
    optimizer = torch.optim.AdamW(model.parameters(), lr=lr, weight_decay=0.0001)
    scheduler = torch.optim.lr_scheduler.ReduceLROnPlateau(optimizer, mode='max', factor=0.5, patience=max(1, early_stop_patience // 2), min_lr=1e-05)
    best_state = deepcopy(model.state_dict())
    best_auc = -1.0
    bad_epochs = 0
    for epoch in range(epochs):
        model.train()
        epoch_loss = 0.0
        n_rows = 0
        for batch in train_loader:
            optimizer.zero_grad()
            logits = model(batch['user_idx'].to(device), batch['item_idx'].to(device), batch['user_text'].to(device), batch['user_history'].to(device), batch['item_text'].to(device), batch['user_dense'].to(device), batch['item_dense'].to(device))
            labels = batch['label'].to(device)
            loss = loss_fn(logits, labels)
            loss.backward()
            torch.nn.utils.clip_grad_norm_(model.parameters(), max_norm=2.0)
            optimizer.step()
            batch_rows = labels.shape[0]
            epoch_loss += float(loss.item()) * batch_rows
            n_rows += int(batch_rows)
        train_scores = _score_ctr_dataset(model, train_dataset, device, batch_size=batch_size)
        val_scores = _score_ctr_dataset(model, val_dataset, device, batch_size=batch_size)
        train_labels = np.asarray([label for _, _, label in augmented_train_split], dtype=np.float32)
        val_labels = np.asarray([label for _, _, label in val_split], dtype=np.float32)
        train_metrics = _ctr_metrics(train_labels, train_scores)
        val_metrics = _ctr_metrics(val_labels, val_scores)
        val_auc = float(val_metrics.get('auc', 0.5))
        scheduler.step(val_auc)
        if val_auc > best_auc + 1e-05:
            best_auc = val_auc
            best_state = deepcopy(model.state_dict())
            bad_epochs = 0
        else:
            bad_epochs += 1
            if bad_epochs >= early_stop_patience:
                break
    model.load_state_dict(best_state)
    tuned_base_scores = score_dense_ctr(model=model, selected_users=selected_users, selected_items=selected_items, user_text_emb=user_text_emb, user_history_emb=user_history_emb, item_text_emb=item_text_emb, user_dense=user_dense, item_dense=item_dense, user_to_idx=user_to_idx, item_to_idx=item_to_idx, device=device, batch_size=batch_size, apply_blend=False).astype(np.float32)
    if ranking_epochs > 0 and history_sim is not None and (profile_sim is not None):
        ranking_hard_negatives = _top_negative_candidates_for_users(train_positive_by_user, len(selected_items), history_sim, profile_sim, item_prior, topk=hard_neg_topk, base_scores=tuned_base_scores)
        triplets = _build_pairwise_triplets(train_positive_by_user, ranking_hard_negatives, len(selected_items), hard_neg_per_pos=hard_neg_per_pos, random_neg_per_pos=random_neg_per_pos, seed=seed)
        _train_ctr_ranking(model=model, triplets=triplets, user_text_emb=user_text_emb, user_history_emb=user_history_emb, item_text_emb=item_text_emb, user_dense=user_dense, item_dense=item_dense, device=device, batch_size=batch_size, lr=max(lr * 0.35, 1e-05), epochs=ranking_epochs)
        tuned_base_scores = score_dense_ctr(model=model, selected_users=selected_users, selected_items=selected_items, user_text_emb=user_text_emb, user_history_emb=user_history_emb, item_text_emb=item_text_emb, user_dense=user_dense, item_dense=item_dense, user_to_idx=user_to_idx, item_to_idx=item_to_idx, device=device, batch_size=batch_size, apply_blend=False).astype(np.float32)
    ranking_blend = _tune_ctr_ranking_blend(tuned_base_scores, train_split, val_split, user_text_emb, user_history_emb, item_text_emb, item_prior)
    model._ranking_blend = ranking_blend
    model._item_prior_scores = np.asarray(item_prior, dtype=np.float32)
    return (model, user_to_idx, item_to_idx)

def score_dense_ctr(model: RobustDeepFM, selected_users: List[Any], selected_items: List[Any], user_text_emb: np.ndarray, user_history_emb: np.ndarray, item_text_emb: np.ndarray, user_dense: np.ndarray, item_dense: np.ndarray, user_to_idx: Dict[Any, int], item_to_idx: Dict[Any, int], device: str, batch_size: int, apply_blend: bool=True) -> np.ndarray:
    model.eval()
    rows: List[np.ndarray] = []
    with torch.no_grad():
        for uid in selected_users:
            u_idx = user_to_idx[uid]
            item_idx_arr = np.asarray([item_to_idx[iid] for iid in selected_items], dtype=np.int64)
            score_chunks: List[np.ndarray] = []
            for start in range(0, len(selected_items), batch_size):
                end = min(start + batch_size, len(selected_items))
                idx_slice = item_idx_arr[start:end]
                size = end - start
                logits = model(user_idx=torch.full((size,), u_idx, dtype=torch.long, device=device), item_idx=torch.tensor(idx_slice, dtype=torch.long, device=device), user_text=torch.tensor(np.repeat(user_text_emb[u_idx:u_idx + 1], size, axis=0), dtype=torch.float32, device=device), user_history=torch.tensor(np.repeat(user_history_emb[u_idx:u_idx + 1], size, axis=0), dtype=torch.float32, device=device), item_text=torch.tensor(item_text_emb[idx_slice], dtype=torch.float32, device=device), user_dense=torch.tensor(np.repeat(user_dense[u_idx:u_idx + 1], size, axis=0), dtype=torch.float32, device=device), item_dense=torch.tensor(item_dense[idx_slice], dtype=torch.float32, device=device))
                score_chunks.append(torch.sigmoid(logits).cpu().numpy().astype(np.float32))
            row = np.concatenate(score_chunks, axis=0).astype(np.float32, copy=False)
            rows.append(row)
    dense_scores = np.stack(rows, axis=0).astype(np.float32, copy=False)
    if apply_blend and hasattr(model, '_ranking_blend'):
        blend = getattr(model, '_ranking_blend', None) or {}
        w_model = float(blend.get('model', 1.0))
        w_history = float(blend.get('history', 0.0))
        w_profile = float(blend.get('profile', 0.0))
        w_prior = float(blend.get('prior', 0.0))
        denom = max(w_model + w_history + w_profile + w_prior, 1e-06)
        history_sim = ((_normalized_similarity_matrix(user_history_emb, item_text_emb) + 1.0) * 0.5).astype(np.float32, copy=False)
        profile_sim = ((_normalized_similarity_matrix(user_text_emb, item_text_emb) + 1.0) * 0.5).astype(np.float32, copy=False)
        item_prior = np.asarray(getattr(model, '_item_prior_scores', np.zeros((len(selected_items),), dtype=np.float32)), dtype=np.float32)
        dense_scores = (w_model * dense_scores + w_history * history_sim + w_profile * profile_sim + w_prior * item_prior[None, :]) / denom
    return np.clip(dense_scores, 0.0, 1.0).astype(np.float16)

def build_item_graph(artifacts: DatasetArtifacts, selected_items: List[Any], selected_users: List[Any], positive_history: Dict[Any, Set[Any]], rw_tag_max_df: int=256) -> Tuple[sparse.csr_matrix, Dict[Any, Set[Any]], Dict[Any, Set[Any]]]:
    item_to_idx = {iid: idx for idx, iid in enumerate(selected_items)}
    selected_item_set = set(selected_items)
    selected_user_set = set(selected_users)
    rows: List[int] = []
    cols: List[int] = []
    vals: List[float] = []
    item_tags: Dict[Any, Set[Any]] = defaultdict(set)
    for iid in selected_items:
        for edge in artifacts.graph._out_edges_by_label.get((iid, artifacts.dis_edge), []):
            item_tags[iid].add(edge.target)
    tag_to_items: Dict[Any, List[Any]] = defaultdict(list)
    for iid, tags in item_tags.items():
        for tag in tags:
            tag_to_items[tag].append(iid)
    for items in tag_to_items.values():
        if len(items) <= 1 or len(items) > rw_tag_max_df:
            continue
        weight = 1.0 / math.log2(len(items) + 1.0)
        for i in range(len(items)):
            for j in range(i + 1, len(items)):
                a = item_to_idx[items[i]]
                b = item_to_idx[items[j]]
                rows.extend([a, b])
                cols.extend([b, a])
                vals.extend([weight, weight])
    item_users: Dict[Any, Set[Any]] = defaultdict(set)
    for uid in selected_user_set:
        for iid in positive_history.get(uid, set()):
            if iid in selected_item_set:
                item_users[iid].add(uid)
    for uid, items in positive_history.items():
        if uid not in selected_user_set:
            continue
        item_list = [iid for iid in items if iid in selected_item_set]
        if len(item_list) <= 1:
            continue
        weight = 1.0 / math.sqrt(len(item_list))
        for i in range(len(item_list)):
            for j in range(i + 1, len(item_list)):
                a = item_to_idx[item_list[i]]
                b = item_to_idx[item_list[j]]
                rows.extend([a, b])
                cols.extend([b, a])
                vals.extend([weight, weight])
    n_items = len(selected_items)
    if not rows:
        adjacency = sparse.eye(n_items, dtype=np.float32, format='csr')
    else:
        adjacency = sparse.coo_matrix((vals, (rows, cols)), shape=(n_items, n_items), dtype=np.float32).tocsr()
        adjacency = adjacency + sparse.eye(n_items, dtype=np.float32, format='csr')
    return (adjacency, item_tags, item_users)

def compute_random_walk_matrix(adjacency: sparse.csr_matrix, restart_prob: float, steps: int) -> np.ndarray:
    degrees = np.asarray(adjacency.sum(axis=1)).reshape(-1)
    inv_degrees = np.divide(1.0, degrees, out=np.zeros_like(degrees), where=degrees > 0)
    transition = sparse.diags(inv_degrees).dot(adjacency).tocsr()
    n_items = adjacency.shape[0]
    result = np.zeros((n_items, n_items), dtype=np.float32)
    eye = np.eye(n_items, dtype=np.float32)
    for idx in range(n_items):
        prob = eye[idx].copy()
        restart = eye[idx]
        for _ in range(max(steps, 1)):
            prob = restart_prob * restart + (1.0 - restart_prob) * transition.T.dot(prob)
        result[idx] = prob
    row_max = np.maximum(result.max(axis=1, keepdims=True), 1e-06)
    return result / row_max

def build_mdis_training_pairs(item_embeddings: np.ndarray, rw_matrix: np.ndarray, item_tags: Dict[Any, Set[Any]], selected_items: List[Any], max_pair_count: int, neighbor_topk: int) -> List[Tuple[int, int, float, float]]:
    n_items = len(selected_items)
    if n_items <= 1:
        return []
    cosine = cosine_similarity(item_embeddings)
    pair_candidates: Set[Tuple[int, int]] = set()
    for i in range(n_items):
        top_sem = np.argsort(-cosine[i])[:neighbor_topk + 1]
        top_rw = np.argsort(-rw_matrix[i])[:neighbor_topk + 1]
        for j in np.concatenate([top_sem, top_rw]):
            if i == j:
                continue
            pair_candidates.add((min(i, int(j)), max(i, int(j))))
    if n_items <= 256:
        for i in range(n_items):
            for j in range(i + 1, n_items):
                pair_candidates.add((i, j))
    pair_list = sorted(pair_candidates)
    if max_pair_count and len(pair_list) > max_pair_count:
        pair_list = pair_list[:max_pair_count]
    train_pairs: List[Tuple[int, int, float, float]] = []
    for i, j in pair_list:
        iid = selected_items[i]
        jid = selected_items[j]
        tags_i = item_tags.get(iid, set())
        tags_j = item_tags.get(jid, set())
        if tags_i or tags_j:
            inter = len(tags_i & tags_j)
            union = len(tags_i | tags_j)
            target = 1.0 - inter / union if union else 0.0
        else:
            target = float(np.clip(1.0 - cosine[i, j], 0.0, 1.0))
        rw_prox = float(max(rw_matrix[i, j], rw_matrix[j, i]))
        rw_dist = float(np.clip(1.0 - rw_prox, 0.0, 1.0))
        train_pairs.append((i, j, rw_dist, target))
    return train_pairs

def train_mdis_model(item_embeddings: np.ndarray, rw_matrix: np.ndarray, item_tags: Dict[Any, Set[Any]], selected_items: List[Any], device: str, max_pair_count: int, neighbor_topk: int, epochs: int, batch_size: int, lr: float) -> Tuple[MDisRegressor, List[Tuple[int, int, float, float]]]:
    train_pairs = build_mdis_training_pairs(item_embeddings=item_embeddings, rw_matrix=rw_matrix, item_tags=item_tags, selected_items=selected_items, max_pair_count=max_pair_count, neighbor_topk=neighbor_topk)
    if not train_pairs:
        raise ValueError('No candidate item pairs available for Mdis training.')
    dataset = PairDistanceDataset(train_pairs, item_embeddings)
    loader = DataLoader(dataset, batch_size=batch_size, shuffle=True, drop_last=False)
    model = MDisRegressor(text_dim=item_embeddings.shape[1]).to(device)
    optimizer = torch.optim.Adam(model.parameters(), lr=lr, weight_decay=1e-05)
    loss_fn = nn.MSELoss()
    _train_model(model, loader, loss_fn, optimizer, device, epochs=epochs)
    return (model, train_pairs)

def score_mdis_pairs(model: MDisRegressor, item_embeddings: np.ndarray, rw_matrix: np.ndarray, pairs: Sequence[Tuple[int, int, float, float]], selected_items: List[Any], device: str, batch_size: int) -> Dict[Tuple[Any, Any], float]:
    model.eval()
    dis_cache: Dict[Tuple[Any, Any], float] = {}
    dataset = PairDistanceDataset(pairs, item_embeddings)
    loader = DataLoader(dataset, batch_size=batch_size, shuffle=False, drop_last=False)
    with torch.no_grad():
        for batch in loader:
            preds = model(batch['left'].to(device), batch['right'].to(device), batch['rw_dist'].to(device)).cpu().numpy()
            left_idx = batch['left_idx'].cpu().numpy()
            right_idx = batch['right_idx'].cpu().numpy()
            rw_dist = batch['rw_dist'].cpu().numpy().reshape(-1)
            for idx, score in enumerate(preds):
                i = int(left_idx[idx])
                j = int(right_idx[idx])
                iid = selected_items[i]
                jid = selected_items[j]
                blended = 0.65 * float(score) + 0.35 * float(rw_dist[idx])
                final_score = float(np.clip(blended, 0.0, 1.0))
                dis_cache[iid, jid] = final_score
                dis_cache[jid, iid] = final_score
    return dis_cache

def compute_maggr_scores(artifacts: DatasetArtifacts, selected_items: List[Any]) -> Dict[Any, float]:
    counts = Counter()
    selected_item_set = set(selected_items)
    for edge in artifacts.graph.get_edges_by_label(artifacts.interaction_edge):
        if edge.target in selected_item_set:
            counts[edge.target] += 1
    max_count = max(counts.values()) if counts else 1
    scores = {}
    for iid in selected_items:
        score = math.log1p(counts.get(iid, 0)) / math.log1p(max_count)
        scores[iid] = float(np.clip(score, 0.0, 1.0))
    return scores

def build_ml_cache(selected_users: List[Any], selected_items: List[Any], dense_scores: np.ndarray, dis_cache: Dict[Tuple[Any, Any], float], aggr_cache: Dict[Any, float]) -> MLPredicateCache:
    cache = MLPredicateCache()
    cache.cache = {}
    cache.dis_cache = dict(dis_cache)
    cache.aggr_cache = dict(aggr_cache)
    cache.set_dense_rec_scores(selected_users, selected_items, dense_scores)
    return cache

def run_pipeline(args: argparse.Namespace) -> Path:
    _set_seed(args.seed)
    data_dir = Path(args.data_dir).resolve()
    dataset_name = args.dataset or data_dir.name
    raw_dir = Path(args.raw_dir).resolve() if args.raw_dir else PROJECT_ROOT / 'raw_data' / dataset_name
    output_path = Path(args.output).resolve() if args.output else data_dir / 'ml_cache.pkl'
    device = args.device
    if device == 'auto':
        device = 'cuda' if torch.cuda.is_available() else 'cpu'
    artifacts = load_artifacts(data_dir)
    selected_users, selected_items, train_pairs, positive_history = select_entities(artifacts=artifacts, max_users=args.toy_max_users, max_items=args.toy_max_items, max_train_pairs=args.toy_max_train_pairs)
    corpora = build_text_corpora(artifacts=artifacts, raw_dir=raw_dir, selected_users=selected_users, selected_items=selected_items, positive_history=positive_history, max_chars=args.max_text_chars, max_user_review_snippets=args.max_user_review_snippets, max_item_review_snippets=args.max_item_review_snippets)
    user_texts = [corpora.user_texts.get(uid, f'user {_strip_prefix(uid)}') for uid in selected_users]
    item_texts = [corpora.item_texts.get(iid, corpora.item_titles.get(iid, _strip_prefix(iid))) for iid in selected_items]
    embedder = FlagTextEmbedder(Path(args.embedding_model_path).resolve(), device=device)
    user_text_emb = embedder.encode(user_texts, batch_size=args.text_batch_size)
    item_text_emb = embedder.encode(item_texts, batch_size=args.text_batch_size)
    user_history_emb = _build_user_history_embeddings(selected_users=selected_users, positive_history=positive_history, selected_items=selected_items, item_text_emb=item_text_emb)
    user_dense = _standardize_dense_matrix(_align_numeric(selected_users, corpora.user_numeric))
    item_dense = _standardize_dense_matrix(_align_numeric(selected_items, corpora.item_numeric))
    valid_pairs = [(uid, iid, float(label)) for (uid, iid), label in artifacts.valid_data.items() if uid in set(selected_users) and iid in set(selected_items)]
    ctr_model, user_to_idx, item_to_idx = train_ctr_model(selected_users=selected_users, selected_items=selected_items, train_pairs=train_pairs, valid_pairs=valid_pairs, user_text_emb=user_text_emb, user_history_emb=user_history_emb, item_text_emb=item_text_emb, user_dense=user_dense, item_dense=item_dense, device=device, embed_dim=args.deepfm_embed_dim, hidden_dims=[int(x) for x in args.deepfm_hidden.split(',') if x.strip()], cross_layers=args.deepfm_cross_layers, epochs=args.epochs_ctr, batch_size=args.batch_size, lr=args.lr_ctr, seed=args.seed, early_stop_patience=args.ctr_early_stop_patience, holdout_ratio=args.ctr_holdout_ratio, extra_neg_ratio=args.ctr_extra_neg_ratio, ranking_epochs=args.ctr_ranking_epochs, hard_neg_topk=args.ctr_hard_neg_topk, hard_neg_per_pos=args.ctr_hard_neg_per_pos, random_neg_per_pos=args.ctr_random_neg_per_pos)
    dense_scores = score_dense_ctr(model=ctr_model, selected_users=selected_users, selected_items=selected_items, user_text_emb=user_text_emb, user_history_emb=user_history_emb, item_text_emb=item_text_emb, user_dense=user_dense, item_dense=item_dense, user_to_idx=user_to_idx, item_to_idx=item_to_idx, device=device, batch_size=args.batch_size)
    adjacency, item_tags, _item_users = build_item_graph(artifacts=artifacts, selected_items=selected_items, selected_users=selected_users, positive_history=positive_history, rw_tag_max_df=args.rw_tag_max_df)
    rw_matrix = compute_random_walk_matrix(adjacency=adjacency, restart_prob=args.rw_restart, steps=args.rw_steps)
    mdis_model, mdis_pairs = train_mdis_model(item_embeddings=item_text_emb, rw_matrix=rw_matrix, item_tags=item_tags, selected_items=selected_items, device=device, max_pair_count=args.max_mdis_pairs, neighbor_topk=args.item_neighbor_topk, epochs=args.epochs_dis, batch_size=args.batch_size, lr=args.lr_dis)
    dis_cache = score_mdis_pairs(model=mdis_model, item_embeddings=item_text_emb, rw_matrix=rw_matrix, pairs=mdis_pairs, selected_items=selected_items, device=device, batch_size=args.batch_size)
    aggr_cache = compute_maggr_scores(artifacts=artifacts, selected_items=selected_items)
    ml_cache = build_ml_cache(selected_users=selected_users, selected_items=selected_items, dense_scores=dense_scores, dis_cache=dis_cache, aggr_cache=aggr_cache)
    ml_cache.save(str(output_path))
    return output_path
