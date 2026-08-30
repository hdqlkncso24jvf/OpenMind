import os
import yaml
import logging
from typing import Dict, Set
from collections import defaultdict
logger = logging.getLogger(__name__)
VALID_ATTR_TYPES = {'id', 'category', 'temporal', 'ordinal'}

class AttributeConfig:

    def __init__(self, flat_config: Dict[str, str], label_config: Dict[str, Dict[str, str]]):
        self.flat = flat_config
        self.by_label = label_config

    def get_type(self, attr_name: str, label: str=None) -> str:
        if label and label in self.by_label:
            if attr_name in self.by_label[label]:
                return self.by_label[label][attr_name]
        if attr_name in self.flat:
            return self.flat[attr_name]
        return 'category'

    def is_id(self, attr_name: str, label: str=None) -> bool:
        return self.get_type(attr_name, label) == 'id'

    def get_id_attributes(self, label: str=None) -> Set[str]:
        if label and label in self.by_label:
            return {a for a, t in self.by_label[label].items() if t == 'id'}
        return {a for a, t in self.flat.items() if t == 'id'}

    def get_allowed_operations(self, attr_name: str, label: str=None, cardinality: int=999) -> dict:
        attr_type = self.get_type(attr_name, label)
        if attr_type == 'id':
            return {'pointwise_eq': False, 'pointwise_lt_gt': False, 'pairwise_eq': True, 'pairwise_lt_gt': False}
        elif attr_type == 'category':
            return {'pointwise_eq': True, 'pointwise_lt_gt': False, 'pairwise_eq': True, 'pairwise_lt_gt': False}
        elif attr_type == 'temporal':
            return {'pointwise_eq': True, 'pointwise_lt_gt': True, 'pairwise_eq': True, 'pairwise_lt_gt': True}
        elif attr_type == 'ordinal':
            if cardinality <= 10:
                return {'pointwise_eq': True, 'pointwise_lt_gt': True, 'pairwise_eq': True, 'pairwise_lt_gt': True}
            else:
                return {'pointwise_eq': False, 'pointwise_lt_gt': True, 'pairwise_eq': False, 'pairwise_lt_gt': True}
        else:
            logger.warning(f"Unknown attribute type '{attr_type}' for {attr_name}, disabling all")
            return {'pointwise_eq': False, 'pointwise_lt_gt': False, 'pairwise_eq': False, 'pairwise_lt_gt': False}

def load_attribute_config(data_dir: str, graph=None) -> 'AttributeConfig':
    config_path = os.path.join(data_dir, 'attribute_config.yaml')
    if not os.path.exists(config_path):
        if graph is not None:
            _generate_template(config_path, graph)
        assert False, f'\n{'=' * 70}\n  MISSING: attribute_config.yaml\n{'=' * 70}\n  A template has been generated at:\n    {config_path}\n\n  Please edit it and classify each attribute as one of:\n    id       — unique identifier (PairWise EQ only, must appear in pairs)\n    category — categorical value (PointWise EQ, PairWise EQ)\n    temporal — date/time (all comparisons)\n    ordinal  — numeric with ordering (PointWise LT/GT, PairWise LT/GT;\n               cardinality<=10 also allows EQ)\n\n  Then re-run.\n{'=' * 70}'
    with open(config_path, 'r') as f:
        raw = yaml.safe_load(f)
    assert raw is not None, f'attribute_config.yaml is empty: {config_path}'
    assert isinstance(raw, dict), f'attribute_config.yaml must be a YAML dict, got {type(raw)}'
    flat_config = {}
    label_config = {}
    for section in ['nodes', 'edges']:
        section_data = raw.get(section, {})
        if section_data is None:
            continue
        assert isinstance(section_data, dict), f"'{section}' in attribute_config.yaml must be a dict, got {type(section_data)}"
        for label, attrs in section_data.items():
            if attrs is None:
                continue
            assert isinstance(attrs, dict), f"'{section}.{label}' must be a dict of attr_name: type, got {type(attrs)}"
            label_config[label] = {}
            for attr_name, attr_type in attrs.items():
                assert attr_type != 'FIXME', f'Unresolved FIXME for {section}.{label}.{attr_name}.\nPlease classify it as one of: {VALID_ATTR_TYPES}\nEdit {config_path} and re-run.'
                assert attr_type in VALID_ATTR_TYPES, f"Invalid type '{attr_type}' for {section}.{label}.{attr_name}.\nMust be one of: {VALID_ATTR_TYPES}\nPlease fix {config_path} and re-run."
                flat_config[attr_name] = attr_type
                label_config[label][attr_name] = attr_type
    assert len(flat_config) > 0, f'attribute_config.yaml has no attributes defined.\nPlease edit {config_path} and classify at least one attribute.'
    type_counts = defaultdict(int)
    for t in flat_config.values():
        type_counts[t] += 1
    return AttributeConfig(flat_config, label_config)

def _generate_template(config_path: str, graph):
    node_attrs = defaultdict(lambda: defaultdict(list))
    edge_attrs = defaultdict(lambda: defaultdict(list))
    label_counts = defaultdict(int)
    for node in graph.all_nodes():
        if label_counts[node.label] >= 200:
            continue
        label_counts[node.label] += 1
        attrs = node.attributes
        if isinstance(attrs, dict):
            for attr, val in attrs.items():
                node_attrs[node.label][attr].append(val)
        elif isinstance(attrs, (set, list)):
            for attr in attrs:
                node_attrs[node.label][attr].append(True)
    edge_count = defaultdict(int)
    for edge in graph.all_edges():
        if edge_count[edge.label] >= 200:
            continue
        edge_count[edge.label] += 1
        if hasattr(edge, 'attributes') and isinstance(edge.attributes, dict):
            for attr, val in edge.attributes.items():
                edge_attrs[edge.label][attr].append(val)
    lines = ['# attribute_config.yaml', '# Classify each attribute as: id | category | temporal | ordinal', '#', '# Types:', '#   id       - unique identifier (PairWise EQ only, appears in pairs in RL)', '#   category - categorical/enum (PointWise EQ, PairWise EQ)', '#   temporal - date/time related (all comparisons allowed)', '#   ordinal  - numeric with meaningful order (PointWise LT/GT, PairWise LT/GT;', '#              if <=10 unique values, also EQ comparisons)', '#', '# Replace FIXME with the correct type, then re-run.', '', 'nodes:']
    for label in sorted(node_attrs.keys()):
        lines.append(f'  {label}:')
        for attr in sorted(node_attrs[label].keys()):
            values = node_attrs[label][attr]
            suggested = _guess_attr_type(attr, values)
            lines.append(f'    {attr}: {suggested}')
        lines.append('')
    if edge_attrs:
        lines.append('edges:')
        for label in sorted(edge_attrs.keys()):
            lines.append(f'  {label}:')
            for attr in sorted(edge_attrs[label].keys()):
                values = edge_attrs[label][attr]
                suggested = _guess_attr_type(attr, values)
                lines.append(f'    {attr}: {suggested}')
            lines.append('')
    else:
        lines.append('edges: {}')
        lines.append('')
    with open(config_path, 'w') as f:
        f.write('\n'.join(lines))

def _guess_attr_type(attr_name: str, values: list) -> str:
    name_lower = attr_name.lower()
    if name_lower == 'id' or name_lower.endswith('_id') or name_lower.startswith('id_') or name_lower.endswith('_key') or name_lower.startswith('key_'):
        return 'id'
    temporal_keywords = ['date', 'time', 'year', 'month', 'day', 'hour', 'created', 'updated', 'timestamp', 'since', 'when']
    if any((kw in name_lower for kw in temporal_keywords)):
        return 'temporal'
    if not values:
        return 'FIXME'
    non_none = [v for v in values if v is not None]
    if not non_none:
        return 'FIXME'
    numeric_count = sum((1 for v in non_none if isinstance(v, (int, float))))
    if numeric_count >= len(non_none) * 0.8:
        return 'ordinal'
    string_count = sum((1 for v in non_none if isinstance(v, str)))
    if string_count >= len(non_none) * 0.8:
        unique = len(set(non_none))
        if unique <= 50:
            return 'category'
    return 'FIXME'
