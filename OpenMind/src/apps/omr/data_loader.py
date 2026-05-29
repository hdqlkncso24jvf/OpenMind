import os
import pickle
import json
import logging
from collections import defaultdict
from dataclasses import dataclass
from typing import Any, Dict, List, Tuple, Optional
from data_structure import Graph
from models import MLPredicateCache
logger = logging.getLogger(__name__)

@dataclass
class DatasetBundle:
    graph: Graph
    training_data: Dict[Tuple[Any, Any], bool]
    test_data: Optional[Dict[Tuple[Any, Any], bool]] = None
    ml_cache: Optional[MLPredicateCache] = None
    stats: Optional[Dict[str, Any]] = None
    dataset_name: str = ''
    data_dir: str = ''
    dis_model: Optional[Any] = None
    aggr_model: Optional[Any] = None
    topic_model: Optional[Any] = None
    tie_rules: Optional[List] = None
    tie_metrics: Optional[List] = None

def load_dataset(data_dir: str, user_label: str='user', item_label: str='game', validate: bool=True) -> DatasetBundle:
    dataset_name = os.path.basename(os.path.normpath(data_dir))
    graph_path = os.path.join(data_dir, 'graph.pkl')
    with open(graph_path, 'rb') as f:
        graph: Graph = pickle.load(f)
    _migrate_graph(graph)
    _build_path_anchor_index(graph, data_dir)
    train_path = os.path.join(data_dir, 'training_data.pkl')
    with open(train_path, 'rb') as f:
        training_data = pickle.load(f)
    pos = sum(training_data.values())
    neg = len(training_data) - pos
    test_data = None
    test_path = os.path.join(data_dir, 'test_data.pkl')
    if os.path.exists(test_path):
        with open(test_path, 'rb') as f:
            test_data = pickle.load(f)
        tp = sum(test_data.values())
        tn = len(test_data) - tp
    ml_cache = None
    ml_cache_path = os.path.join(data_dir, 'ml_cache.pkl')
    if os.path.exists(ml_cache_path):
        ml_cache = MLPredicateCache(cache_path=ml_cache_path)
    else:
        ml_cache = MLPredicateCache()
    stats = None
    stats_path = os.path.join(data_dir, 'stats.json')
    if os.path.exists(stats_path):
        with open(stats_path, 'r') as f:
            stats = json.load(f)
    if validate:
        _validate(graph, training_data, ml_cache, user_label, item_label)
    return DatasetBundle(graph=graph, training_data=training_data, test_data=test_data, ml_cache=ml_cache, stats=stats, dataset_name=dataset_name, data_dir=data_dir)

def _migrate_graph(graph: Graph) -> None:
    rebuilt = []
    if not hasattr(graph, '_out_edges_by_label') or not graph._out_edges_by_label:
        graph._out_edges_by_label = defaultdict(list)
        graph._in_edges_by_label = defaultdict(list)
        graph._edge_existence = set()
        graph.edge_sources = defaultdict(set)
        graph.edge_targets = defaultdict(set)
        for edge in graph._edges.values():
            graph._out_edges_by_label[edge.source, edge.label].append(edge)
            graph._in_edges_by_label[edge.target, edge.label].append(edge)
            graph._edge_existence.add((edge.source, edge.label, edge.target))
            graph.edge_sources[edge.label].add(edge.source)
            graph.edge_targets[edge.label].add(edge.target)
        rebuilt.append(f'per-label edge indexes ({len(graph._edges)} edges)')
    if not hasattr(graph, '_edge_existence'):
        graph._edge_existence = set()
        for edge in graph._edges.values():
            graph._edge_existence.add((edge.source, edge.label, edge.target))
        rebuilt.append('edge_existence set')
    if not hasattr(graph, 'edge_sources') or not graph.edge_sources:
        graph.edge_sources = defaultdict(set)
        graph.edge_targets = defaultdict(set)
        for edge in graph._edges.values():
            graph.edge_sources[edge.label].add(edge.source)
            graph.edge_targets[edge.label].add(edge.target)
        rebuilt.append(f'edge_sources/edge_targets ({len(graph.edge_sources)} labels)')
    if not hasattr(graph, 'attr_value_index') or not graph.attr_value_index:
        graph.attr_value_index = {}
        for node_id, node in graph._nodes.items():
            for attr_name, attr_value in node.attributes.items():
                if attr_value is None:
                    continue
                key = (attr_name, attr_value)
                if key not in graph.attr_value_index:
                    graph.attr_value_index[key] = set()
                graph.attr_value_index[key].add(node_id)
        rebuilt.append(f'attr_value_index ({len(graph.attr_value_index)} entries)')
    if not hasattr(graph, '_attr_to_values') or graph._attr_to_values is None:
        _by_attr = {}
        for (a, v), node_set in graph.attr_value_index.items():
            _by_attr.setdefault(a, {})[v] = node_set
        graph._attr_to_values = _by_attr
        rebuilt.append(f'_attr_to_values ({len(_by_attr)} attrs)')
    if not hasattr(graph, 'config'):
        from data_structure import GraphIndexConfig
        graph.config = GraphIndexConfig(enable_path_signature_index=False)
        rebuilt.append('GraphIndexConfig (defaults)')
    if not hasattr(graph, 'attr_owner_label') or graph.attr_owner_label is None:
        attr_label_occ = defaultdict(set)
        for node in graph._nodes.values():
            for attr_name in node.attributes:
                attr_label_occ[attr_name].add(node.label)
        graph.attr_owner_label = {a: next(iter(labels)) for a, labels in attr_label_occ.items() if len(labels) == 1}
        rebuilt.append(f'attr_owner_label ({len(graph.attr_owner_label)} attrs)')
    if not hasattr(graph, 'edge_target_label') or graph.edge_target_label is None:
        tgt_map = {}
        src_map = {}
        for edge in graph._edges.values():
            tl = graph._nodes[edge.target].label
            sl = graph._nodes[edge.source].label
            el = edge.label
            if el not in tgt_map:
                tgt_map[el] = tl
            elif tgt_map[el] != tl:
                tgt_map[el] = None
            if el not in src_map:
                src_map[el] = sl
            elif src_map[el] != sl:
                src_map[el] = None
        graph.edge_target_label = tgt_map
        graph.edge_source_label = src_map
        rebuilt.append(f'edge_schema ({len(tgt_map)} edge labels)')
    if hasattr(graph, '_out_edges_by_label') and isinstance(graph._out_edges_by_label, defaultdict):
        graph._out_edges_by_label = {k: tuple(v) for k, v in graph._out_edges_by_label.items()}
        graph._in_edges_by_label = {k: tuple(v) for k, v in graph._in_edges_by_label.items()}
    if not hasattr(graph, '_compact') or graph._compact is None:
        try:
            import time as _t
            _t0 = _t.time()
            from _compact_graph import build_compact_graph
            graph._compact = build_compact_graph(graph)
            _elapsed = _t.time() - _t0
            rebuilt.append(f'CompactGraph ({graph._compact.n_nodes} nodes, {_elapsed:.1f}s)')
        except Exception as e:
            logger.warning(f'  [CompactGraph] Build failed: {e}')
            graph._compact = None

def _build_path_anchor_index(graph: Graph, data_dir: str) -> None:
    import time
    import hashlib
    if hasattr(graph, 'path_anchor_index') and graph.path_anchor_index is not None:
        return
    from data_structure import build_path_anchor_index
    dataset_name = os.path.basename(os.path.normpath(data_dir))
    patterns = None
    for suffix in (f'{dataset_name}_rl_patterns.pkl', f'{dataset_name}_patterns.pkl'):
        candidate = os.path.join(data_dir, suffix)
        if os.path.exists(candidate):
            try:
                with open(candidate, 'rb') as f:
                    patterns = pickle.load(f)
            except Exception as e:
                logger.warning(f'  [PathAnchorIndex] Failed to load {suffix}: {e}')
            break
    path_templates_set: set = set()
    target_attrs: set = set()

    def _extract_from_path(path, nodes_dict):
        if not hasattr(path, 'edges') or not path.edges:
            return
        edge_labels = tuple((e.label for e in path.edges))
        node_labels = tuple((nodes_dict[v].label for v in path.path_sequence))
        is_forward = tuple((e.is_forward for e in path.edges))
        path_templates_set.add((edge_labels, node_labels, is_forward))
        for var in path.path_sequence:
            if var in nodes_dict:
                node = nodes_dict[var]
                if hasattr(node, 'attributes') and node.attributes:
                    attrs = node.attributes
                    if isinstance(attrs, dict):
                        target_attrs.update(attrs.keys())
                    elif isinstance(attrs, (set, list)):
                        target_attrs.update(attrs)
    if patterns:
        sample_size = min(500, len(patterns))
        failed = 0
        for pattern in patterns[:sample_size]:
            for path in pattern.user_paths + pattern.item_paths:
                try:
                    _extract_from_path(path, pattern.nodes)
                except (KeyError, IndexError, AttributeError):
                    failed += 1
    import glob
    omr_files = sorted(glob.glob(os.path.join(data_dir, 'omr_rules*.pkl')))
    omr_files += sorted(glob.glob(os.path.join(data_dir, 'rules', '**', 'omr_rules*.pkl'), recursive=True))
    omr_template_count_before = len(path_templates_set)
    for omr_file in omr_files:
        try:
            with open(omr_file, 'rb') as f:
                omr_rules = pickle.load(f)
            for omr_rule in omr_rules:
                omr_pat = getattr(omr_rule, 'pattern', None)
                if omr_pat is None:
                    continue
                for bp in getattr(omr_pat, 'bridge_paths', []):
                    try:
                        _extract_from_path(bp, omr_pat.nodes)
                    except (KeyError, IndexError, AttributeError):
                        pass
                alt_star = getattr(omr_pat, 'alt_star', None)
                if alt_star is not None:
                    for ap in getattr(alt_star, 'paths', []):
                        try:
                            _extract_from_path(ap, omr_pat.nodes)
                        except (KeyError, IndexError, AttributeError):
                            pass
        except Exception as e:
            logger.debug(f'  [PathAnchorIndex] Could not load {omr_file}: {e}')
    omr_new = len(path_templates_set) - omr_template_count_before
    if not path_templates_set:
        edge_target_label = getattr(graph, 'edge_target_label', {}) or {}
        edge_source_label = getattr(graph, 'edge_source_label', {}) or {}
        for el in set(list(edge_target_label.keys()) + list(edge_source_label.keys())):
            sl = edge_source_label.get(el)
            tl = edge_target_label.get(el)
            if sl and tl:
                path_templates_set.add(((el,), (sl, tl), (True,)))
                path_templates_set.add(((el,), (tl, sl), (False,)))
        all_attrs = set((a for node in graph._nodes.values() for a in node.attributes))
        target_attrs = all_attrs
    if not path_templates_set or not target_attrs:
        logger.warning('  [PathAnchorIndex] No templates or attrs — skipping build')
        graph.path_anchor_index = None
        return
    path_templates = [(list(el), list(nl), list(fw)) for el, nl, fw in path_templates_set]
    target_attrs_list = sorted(target_attrs)
    max_cardinality = 1000
    cache_content = str(sorted(path_templates_set)) + str(target_attrs_list) + str(max_cardinality)
    cache_hash = hashlib.md5(cache_content.encode()).hexdigest()[:12]
    cache_path = os.path.join(data_dir, f'path_anchor_index_{cache_hash}.pkl')
    if os.path.exists(cache_path):
        t0 = time.time()
        try:
            with open(cache_path, 'rb') as f:
                graph.path_anchor_index = pickle.load(f)
            elapsed = time.time() - t0
            return
        except Exception as e:
            logger.warning(f'  [PathAnchorIndex] Cache load failed ({e}), rebuilding...')
    t0 = time.time()
    try:
        graph.path_anchor_index = build_path_anchor_index(graph, path_templates=path_templates, target_attrs=target_attrs_list, max_cardinality=max_cardinality)
        elapsed = time.time() - t0
        with open(cache_path, 'wb') as fout:
            pickle.dump(graph.path_anchor_index, fout)
    except Exception as e:
        logger.error(f'  ✗ PathAnchorIndex build failed: {e}')
        graph.path_anchor_index = None

def load_tie_rules(rules_path: str) -> Tuple[List, Optional[List]]:
    with open(rules_path, 'rb') as f:
        data = pickle.load(f)
    if not data:
        return ([], None)
    if isinstance(data[0], tuple):
        rules = [r for r, m in data]
        metrics = [m for r, m in data]
        return (rules, metrics)
    else:
        return (data, None)

def load_omr_models(graph, ml_cache=None, dis_edge_label: str='has_tag', aggr_edge_label: str='plays'):
    if isinstance(ml_cache, str):
        legacy_dis_edge_label = ml_cache
        legacy_aggr_edge_label = dis_edge_label
        ml_cache = None
        dis_edge_label = legacy_dis_edge_label
        aggr_edge_label = legacy_aggr_edge_label
    from models import DisModel, AggrModel, TopicProximityModel
    dis_model = DisModel(graph, edge_label=dis_edge_label, ml_cache=ml_cache)
    aggr_model = AggrModel(graph, interaction_edge_label=aggr_edge_label)
    topic_model = TopicProximityModel(graph, ml_cache=ml_cache)
    return (dis_model, aggr_model, topic_model)

def _validate(graph: Graph, training_data: Dict[Tuple[Any, Any], bool], ml_cache: Optional[MLPredicateCache], user_label: str, item_label: str) -> None:
    errors = []
    warnings = []
    ml_missing = 0
    for (uid, iid), _ in training_data.items():
        if graph.get_node(uid) is None:
            errors.append(f'User node {uid} not in graph')
        if graph.get_node(iid) is None:
            errors.append(f'Item node {iid} not in graph')
        if ml_cache is not None:
            try:
                if not ml_cache.has_prediction(uid, iid):
                    ml_missing += 1
            except Exception:
                ml_missing += 1
    if ml_cache is not None:
        if ml_missing:
            warnings.append(f'{ml_missing} training pairs missing ML predictions')
    users = graph.get_nodes_by_label(user_label)
    items = graph.get_nodes_by_label(item_label)
    if len(training_data) < 20:
        warnings.append(f'Training samples ({len(training_data)}) < recommended minimum (20)')
    pos = sum(training_data.values())
    neg = len(training_data) - pos
    ratio = pos / neg if neg > 0 else float('inf')
    if ratio < 0.5 or ratio > 2.0:
        warnings.append(f'Pos/neg ratio ({ratio:.2f}) outside recommended range (0.5-2.0)')
    if errors:
        for e in errors:
            logger.error(f'    ✗ {e}')
        raise ValueError(f'Data validation failed with {len(errors)} error(s)')
    if warnings:
        for w in warnings:
            logger.warning(f'    ⚠ {w}')
