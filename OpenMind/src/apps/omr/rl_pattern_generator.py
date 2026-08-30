import argparse
import pickle
import logging
import os
import random
from pathlib import Path
from tqdm import tqdm
import torch
from typing import List
from collections import defaultdict
from data_structure import DualStarPattern
from rl_training_suite import DualStarRLEnvironment, PolicyNetwork, REINFORCETrainer, BudgetAttributeAssigner, validate_pattern_quality
from attribute_config import load_attribute_config
logger = logging.getLogger(__name__)

def get_pattern_attribute_signature(pattern: DualStarPattern) -> frozenset:
    parts = set()
    for var, node in pattern.nodes.items():
        if node.attributes:
            for attr in node.attributes:
                parts.add((var, attr))
    return frozenset(parts)

def _get_topology_signature(pattern: DualStarPattern) -> str:

    def _path_sig(p) -> str:
        parts = []
        seq = p.path_sequence
        for i in range(len(seq) - 1):
            sl = pattern.nodes[seq[i]].label
            tl = pattern.nodes[seq[i + 1]].label
            edge = p.edges[i]
            d = 'F' if edge.is_forward else 'R'
            parts.append(f'{sl}-{edge.label}:{d}-{tl}')
        return '|'.join(parts) if parts else pattern.nodes[seq[0]].label if seq else ''
    user_sigs = sorted((_path_sig(p) for p in pattern.user_paths))
    item_sigs = sorted((_path_sig(p) for p in pattern.item_paths))
    return f'U[{';'.join(user_sigs)}]|I[{';'.join(item_sigs)}]'

def _get_attribute_signature(pattern: DualStarPattern) -> str:
    topo_sig = _get_topology_signature(pattern)
    attr_parts = []
    for var in sorted(pattern.nodes.keys()):
        node = pattern.nodes[var]
        attrs = sorted(node.attributes) if node.attributes else []
        attr_parts.append(f'{var}:{','.join(attrs)}')
    return f'{topo_sig}#{'|'.join(attr_parts)}'

def deduplicate_attribute_combinations(patterns: List[DualStarPattern], keep_minimal: bool=True) -> List[DualStarPattern]:
    topology_groups = defaultdict(list)
    for pattern in patterns:
        topo_sig = _get_topology_signature(pattern)
        topology_groups[topo_sig].append(pattern)
    deduplicated = []
    for topo_sig, group in topology_groups.items():
        if len(group) == 1:
            deduplicated.extend(group)
            continue
        pattern_attr_sigs = [(p, get_pattern_attribute_signature(p)) for p in group]
        kept = []
        for i, (pattern_i, attrs_i) in enumerate(pattern_attr_sigs):
            is_subsumed = False
            for j, (pattern_j, attrs_j) in enumerate(pattern_attr_sigs):
                if i == j:
                    continue
                if keep_minimal:
                    if attrs_i > attrs_j:
                        is_subsumed = True
                        break
                elif attrs_i < attrs_j:
                    is_subsumed = True
                    break
            if not is_subsumed:
                kept.append(pattern_i)
        deduplicated.extend(kept)
    removed = len(patterns) - len(deduplicated)
    return deduplicated

def sort_patterns_by_complexity(patterns: List[DualStarPattern], mode: str='simple_first') -> List[DualStarPattern]:
    if mode == 'random':
        random.shuffle(patterns)
        return patterns

    def complexity_score(pattern: DualStarPattern) -> float:
        num_paths = len(pattern.user_paths) + len(pattern.item_paths)
        num_edges = sum((len(path.path_sequence) - 1 for path in pattern.user_paths + pattern.item_paths))
        return num_paths * 10 + num_edges + random.random() * 0.1
    reverse = mode == 'complex_first'
    return sorted(patterns, key=complexity_score, reverse=reverse)

