from typing import Any, Dict, List, Set, Tuple, Optional, Union
from dataclasses import dataclass, field
from collections import defaultdict
from enum import Enum

class ComparisonOp(Enum):
    EQ = '='
    NE = '!='
    LT = '<'
    LE = '<='
    GT = '>'
    GE = '>='

@dataclass
class GraphIndexConfig:
    enable_path_signature_index: bool = True
    max_path_length: int = 4
    use_numpy_acceleration: bool = False
    cache_size_limit_mb: int = 1024

@dataclass
class PathSignature:
    edge_labels: Tuple[str, ...]
    node_labels: Tuple[str, ...]
    predicates_hash: int

    def __hash__(self):
        return hash((self.edge_labels, self.node_labels, self.predicates_hash))

    def __eq__(self, other):
        if not isinstance(other, PathSignature):
            return False
        return self.edge_labels == other.edge_labels and self.node_labels == other.node_labels and (self.predicates_hash == other.predicates_hash)

@dataclass
class PathSignatureIndex:
    signature_to_pairs: Dict[PathSignature, Set[Tuple[Any, Any]]] = field(default_factory=dict)
    pair_to_signatures: Dict[Tuple[Any, Any], Set[PathSignature]] = field(default_factory=dict)

@dataclass
class Node:
    node_id: Any
    label: str
    attributes: Dict[str, Any] = field(default_factory=dict)

    def __hash__(self):
        return hash(self.node_id)

    def __eq__(self, other):
        if not isinstance(other, Node):
            return False
        return self.node_id == other.node_id

@dataclass
class Edge:
    edge_id: Any
    source: Any
    target: Any
    label: str
    attributes: Dict[str, Any] = field(default_factory=dict)

    def __hash__(self):
        return hash(self.edge_id)

    def __eq__(self, other):
        if not isinstance(other, Edge):
            return False
        return self.edge_id == other.edge_id

class Graph:

    def __init__(self, config: Optional[GraphIndexConfig]=None):
        self._nodes: Dict[Any, Node] = {}
        self._edges: Dict[Any, Edge] = {}
        self._out_edges: Dict[Any, List[Edge]] = defaultdict(list)
        self._in_edges: Dict[Any, List[Edge]] = defaultdict(list)
        self._out_edges_by_label: Dict[Tuple[Any, str], List[Edge]] = defaultdict(list)
        self._in_edges_by_label: Dict[Tuple[Any, str], List[Edge]] = defaultdict(list)
        self._edge_existence: Set[Tuple[Any, str, Any]] = set()
        self._nodes_by_label: Dict[str, Set[Any]] = defaultdict(set)
        self._edges_by_label: Dict[str, Set[Any]] = defaultdict(set)
        self._edge_attr_index: Dict[str, Dict[Any, Set[Any]]] = {}
        self.edge_sources: Dict[str, Set[Any]] = defaultdict(set)
        self.edge_targets: Dict[str, Set[Any]] = defaultdict(set)
        self.attr_value_index: Dict[Tuple[str, Any], Set[Any]] = {}
        self.config = config or GraphIndexConfig(enable_path_signature_index=False)
        self.path_signature_index: Optional[PathSignatureIndex] = PathSignatureIndex() if self.config.enable_path_signature_index else None

    def add_node(self, node_id: Any, label: str, attributes: Optional[Dict[str, Any]]=None) -> Node:
        if node_id in self._nodes:
            raise ValueError(f'Node {node_id} already exists')
        node = Node(node_id=node_id, label=label, attributes=attributes or {})
        self._nodes[node_id] = node
        self._nodes_by_label[label].add(node_id)
        for attr_name, attr_value in node.attributes.items():
            key = (attr_name, attr_value)
            if key not in self.attr_value_index:
                self.attr_value_index[key] = set()
            self.attr_value_index[key].add(node_id)
        return node

    def add_edge(self, edge_id: Any, source: Any, target: Any, label: str, attributes: Optional[Dict[str, Any]]=None) -> Edge:
        if edge_id in self._edges:
            raise ValueError(f'Edge {edge_id} already exists')
        if source not in self._nodes:
            raise ValueError(f'Source node {source} does not exist')
        if target not in self._nodes:
            raise ValueError(f'Target node {target} does not exist')
        edge = Edge(edge_id=edge_id, source=source, target=target, label=label, attributes=attributes or {})
        self._edges[edge_id] = edge
        self._edges_by_label[label].add(edge_id)
        self._out_edges[source].append(edge)
        self._in_edges[target].append(edge)
        self._out_edges_by_label[source, label].append(edge)
        self._in_edges_by_label[target, label].append(edge)
        self._edge_existence.add((source, label, target))
        self.edge_sources[label].add(source)
        self.edge_targets[label].add(target)
        for attr_name, attr_value in edge.attributes.items():
            if attr_name not in self._edge_attr_index:
                self._edge_attr_index[attr_name] = {}
            if attr_value not in self._edge_attr_index[attr_name]:
                self._edge_attr_index[attr_name][attr_value] = set()
            self._edge_attr_index[attr_name][attr_value].add(edge_id)
        return edge

    def get_node(self, node_id: Any) -> Optional[Node]:
        return self._nodes.get(node_id)

    def get_edge(self, edge_id: Any) -> Optional[Edge]:
        return self._edges.get(edge_id)

    def get_nodes_by_label(self, label: str) -> List[Node]:
        node_ids = self._nodes_by_label.get(label, set())
        return [self._nodes[nid] for nid in node_ids]

    def get_edges_by_label(self, label: str) -> List[Edge]:
        edge_ids = self._edges_by_label.get(label, set())
        return [self._edges[eid] for eid in edge_ids]

    def get_nodes_by_attribute(self, attr_name: str, attr_value: Any) -> List[Node]:
        node_ids = self.attr_value_index.get((attr_name, attr_value), set())
        return [self._nodes[nid] for nid in node_ids]

    def get_edges_by_attribute(self, attr_name: str, attr_value: Any) -> List[Edge]:
        edge_ids = self._edge_attr_index.get(attr_name, {}).get(attr_value, set())
        return [self._edges[eid] for eid in edge_ids]

    def get_out_neighbors(self, node_id: Any, edge_label: Optional[str]=None) -> List[Tuple[Node, Edge]]:
        if node_id not in self._nodes:
            return []
        if edge_label:
            edges = self._out_edges_by_label.get((node_id, edge_label), [])
        else:
            edges = self._out_edges[node_id]
        return [(self._nodes[e.target], e) for e in edges]

    def get_in_neighbors(self, node_id: Any, edge_label: Optional[str]=None) -> List[Tuple[Node, Edge]]:
        if node_id not in self._nodes:
            return []
        if edge_label:
            edges = self._in_edges_by_label.get((node_id, edge_label), [])
        else:
            edges = self._in_edges[node_id]
        return [(self._nodes[e.source], e) for e in edges]

    def get_out_neighbor_nodes(self, node_id: Any, edge_label: str) -> List[Node]:
        edges = self._out_edges_by_label.get((node_id, edge_label), [])
        return [self._nodes[e.target] for e in edges]

    def iter_out_neighbor_nodes(self, node_id: Any, edge_label: str):
        for e in self._out_edges_by_label.get((node_id, edge_label), []):
            yield self._nodes[e.target]

    def get_in_neighbor_nodes(self, node_id: Any, edge_label: str) -> List[Node]:
        edges = self._in_edges_by_label.get((node_id, edge_label), [])
        return [self._nodes[e.source] for e in edges]

    def iter_in_neighbor_nodes(self, node_id: Any, edge_label: str):
        for e in self._in_edges_by_label.get((node_id, edge_label), []):
            yield self._nodes[e.source]

    def get_out_edges(self, node_id: Any) -> List[Edge]:
        return self._out_edges.get(node_id, [])

    def get_in_edges(self, node_id: Any) -> List[Edge]:
        return self._in_edges.get(node_id, [])

    def all_nodes(self) -> List[Node]:
        return list(self._nodes.values())

    def all_edges(self) -> List[Edge]:
        return list(self._edges.values())

    def node_count(self) -> int:
        return len(self._nodes)

    def edge_count(self) -> int:
        return len(self._edges)

    def get_source_nodes_of_edge(self, edge_label: str) -> Set[Any]:
        return self.edge_sources.get(edge_label, set())

    def get_target_nodes_of_edge(self, edge_label: str) -> Set[Any]:
        return self.edge_targets.get(edge_label, set())

    def has_edge(self, source: Any, edge_label: str, target: Any) -> bool:
        return (source, edge_label, target) in self._edge_existence

    def get_node_ids_by_label(self, label: str) -> Set[Any]:
        return self._nodes_by_label.get(label, set()).copy()

    def get_node_ids_by_label_view(self, label: str) -> Set[Any]:
        return self._nodes_by_label.get(label, set())

    def get_nodes_batch(self, node_ids) -> Dict[Any, Any]:
        return {nid: self._nodes[nid] for nid in node_ids if nid in self._nodes}

    def extract_attributes_batch(self, node_ids, attr_name: str) -> Dict[Any, Any]:
        result = {}
        for nid in node_ids:
            node = self._nodes.get(nid)
            if node is not None and attr_name in node.attributes:
                result[nid] = node.attributes[attr_name]
        return result

class Predicate:
    pass

def _normalize_predicate_equality_value(value: Any) -> Any:
    if value is None:
        return None
    if isinstance(value, bool):
        return float(value)
    if isinstance(value, (int, float)):
        return float(value)
    if isinstance(value, str):
        try:
            return float(value)
        except (TypeError, ValueError):
            return value
    return str(value)

def _safe_compare_predicate_values(value1: Any, value2: Any, operator: ComparisonOp) -> bool:
    if operator == ComparisonOp.EQ:
        if value1 is None or value2 is None:
            return value1 == value2
    elif operator == ComparisonOp.NE:
        if value1 is None or value2 is None:
            return value1 != value2
    elif value1 is None or value2 is None:
        return False
    try:
        if operator == ComparisonOp.EQ:
            return value1 == value2
        if operator == ComparisonOp.NE:
            return value1 != value2
        if operator == ComparisonOp.LT:
            return value1 < value2
        if operator == ComparisonOp.LE:
            return value1 <= value2
        if operator == ComparisonOp.GT:
            return value1 > value2
        if operator == ComparisonOp.GE:
            return value1 >= value2
    except TypeError:
        pass

    def _to_number(value: Any):
        if isinstance(value, bool):
            return float(value)
        if isinstance(value, (int, float)):
            return float(value)
        if isinstance(value, str):
            return float(value)
        raise TypeError
    if operator in (ComparisonOp.LT, ComparisonOp.LE, ComparisonOp.GT, ComparisonOp.GE):
        try:
            lhs = _to_number(value1)
            rhs = _to_number(value2)
        except (TypeError, ValueError):
            return False
        if operator == ComparisonOp.LT:
            return lhs < rhs
        if operator == ComparisonOp.LE:
            return lhs <= rhs
        if operator == ComparisonOp.GT:
            return lhs > rhs
        return lhs >= rhs
    if operator in (ComparisonOp.EQ, ComparisonOp.NE):
        lhs = _normalize_predicate_equality_value(value1)
        rhs = _normalize_predicate_equality_value(value2)
        return lhs == rhs if operator == ComparisonOp.EQ else lhs != rhs
    return False

class PointWisePredicate(Predicate):

    def __init__(self, variable: str, attribute: str, operator: ComparisonOp, constant: Any):
        self.variable = variable
        self.attribute = attribute
        self.operator = operator
        self.constant = constant

    def evaluate(self, node: Node) -> bool:
        if self.attribute not in node.attributes:
            return False
        value = node.attributes[self.attribute]
        return _safe_compare_predicate_values(value, self.constant, self.operator)

    def __repr__(self):
        return f'{self.variable}.{self.attribute} {self.operator.value} {self.constant}'

class PairWisePredicate(Predicate):

    def __init__(self, var1: str, attr1: str, operator: ComparisonOp, var2: str, attr2: str):
        self.var1 = var1
        self.attr1 = attr1
        self.operator = operator
        self.var2 = var2
        self.attr2 = attr2

    def evaluate(self, node1: Node, node2: Node) -> bool:
        if self.attr1 not in node1.attributes or self.attr2 not in node2.attributes:
            return False
        value1 = node1.attributes[self.attr1]
        value2 = node2.attributes[self.attr2]
        return _safe_compare_predicate_values(value1, value2, self.operator)

    def __repr__(self):
        return f'{self.var1}.{self.attr1} {self.operator.value} {self.var2}.{self.attr2}'

class MLPredicate(Predicate):

    def __init__(self, user_var: str, item_var: str, operator: str, threshold: float):
        self.user_var = user_var
        self.item_var = item_var
        self.operator = operator
        self.threshold = threshold

    def evaluate(self, ml_score) -> bool:
        if ml_score is None:
            return False
        if self.operator == '=':
            return ml_score == self.threshold
        if self.operator == '!=':
            return ml_score != self.threshold
        if self.operator == '<':
            return ml_score < self.threshold
        if self.operator == '<=':
            return ml_score <= self.threshold
        if self.operator == '>':
            return ml_score > self.threshold
        if self.operator == '>=':
            return ml_score >= self.threshold
        return False

    def __repr__(self):
        return f'M({self.user_var}, {self.item_var}) {self.operator} {self.threshold}'

@dataclass
class EdgePredicate:
    source_var: str
    target_var: str
    edge_label: str

    def __repr__(self):
        label = self.edge_label if self.edge_label is not None else 'PREDICTS'
        return f'({self.source_var}, {label}, {self.target_var})'

@dataclass
class PatternNode:
    variable: str
    label: str
    attributes: Set[str] = field(default_factory=set)

    def __hash__(self):
        return hash(self.variable)

    def __eq__(self, other):
        if not isinstance(other, PatternNode):
            return False
        return self.variable == other.variable

@dataclass
class PatternEdge:
    source_var: str
    target_var: str
    label: str
    is_forward: bool = True

    def __hash__(self):
        return hash((self.source_var, self.target_var, self.label, self.is_forward))

class Pattern:

    def __init__(self):
        self.nodes: Dict[str, PatternNode] = {}
        self.edges: List[PatternEdge] = []

    def add_node(self, variable: str, label: str, attributes: Optional[Set[str]]=None) -> PatternNode:
        if variable in self.nodes:
            raise ValueError(f'Variable {variable} already exists in pattern')
        node = PatternNode(variable=variable, label=label, attributes=attributes or set())
        self.nodes[variable] = node
        return node

    def add_edge(self, source_var: str, target_var: str, label: str, is_forward: bool=True) -> PatternEdge:
        if source_var not in self.nodes:
            raise ValueError(f'Source variable {source_var} not in pattern')
        if target_var not in self.nodes:
            raise ValueError(f'Target variable {target_var} not in pattern')
        edge = PatternEdge(source_var=source_var, target_var=target_var, label=label, is_forward=is_forward)
        self.edges.append(edge)
        return edge

    def get_node(self, variable: str) -> Optional[PatternNode]:
        return self.nodes.get(variable)

    def get_all_nodes(self) -> List[PatternNode]:
        return list(self.nodes.values())

    def get_all_edges(self) -> List[PatternEdge]:
        return self.edges

class Path(Pattern):

    def __init__(self, path_nodes: List[Tuple[str, str, Optional[Set[str]]]]):
        super().__init__()
        self.path_sequence: List[str] = []
        for i, (variable, label, attributes) in enumerate(path_nodes):
            self.add_node(variable, label, attributes)
            self.path_sequence.append(variable)

    def add_path_edge(self, source_var: str, target_var: str, label: str, is_forward: bool=True):
        self.add_edge(source_var, target_var, label, is_forward=is_forward)

    def get_path_length(self) -> int:
        return len(self.path_sequence)

    def get_start_node(self) -> Optional[PatternNode]:
        if self.path_sequence:
            return self.nodes[self.path_sequence[0]]
        return None

    def get_end_node(self) -> Optional[PatternNode]:
        if self.path_sequence:
            return self.nodes[self.path_sequence[-1]]
        return None

class StarPattern(Pattern):

    def __init__(self, center_var: str, center_label: str, center_attributes: Optional[Set[str]]=None):
        super().__init__()
        self.center_var = center_var
        self.add_node(center_var, center_label, center_attributes)
        self.paths: List[Path] = []

    def add_path_from_center(self, path: Path):
        if path.get_start_node().variable != self.center_var:
            raise ValueError(f'Path must start from center node {self.center_var}')
        for node in path.get_all_nodes():
            if node.variable not in self.nodes:
                self.nodes[node.variable] = node
        for edge in path.get_all_edges():
            if edge not in self.edges:
                self.edges.append(edge)
        self.paths.append(path)

    def get_center(self) -> PatternNode:
        return self.nodes[self.center_var]

    def get_leaves(self) -> List[PatternNode]:
        leaves = []
        for path in self.paths:
            end_node = path.get_end_node()
            if end_node and end_node.variable != self.center_var:
                leaves.append(end_node)
        return leaves

class DualStarPattern(Pattern):

    def __init__(self, user_center: str, user_label: str, item_center: str, item_label: str, user_attributes: Optional[Set[str]]=None, item_attributes: Optional[Set[str]]=None):
        super().__init__()
        self.user_center = user_center
        self.item_center = item_center
        self.user_star = StarPattern(user_center, user_label, user_attributes)
        self.item_star = StarPattern(item_center, item_label, item_attributes)
        self.nodes[user_center] = self.user_star.get_center()
        self.nodes[item_center] = self.item_star.get_center()
        self.user_paths: List[Path] = []
        self.item_paths: List[Path] = []

    def add_user_path(self, path: Path):
        self.user_star.add_path_from_center(path)
        self.user_paths.append(path)
        for node in path.get_all_nodes():
            if node.variable not in self.nodes:
                self.nodes[node.variable] = node
        for edge in path.get_all_edges():
            if edge not in self.edges:
                self.edges.append(edge)

    def add_item_path(self, path: Path):
        self.item_star.add_path_from_center(path)
        self.item_paths.append(path)
        for node in path.get_all_nodes():
            if node.variable not in self.nodes:
                self.nodes[node.variable] = node
        for edge in path.get_all_edges():
            if edge not in self.edges:
                self.edges.append(edge)

    def get_all_nodes(self) -> List[PatternNode]:
        return list(self.nodes.values())

    def get_leaves(self) -> List[PatternNode]:
        user_leaves = self.user_star.get_leaves()
        item_leaves = self.item_star.get_leaves()
        return user_leaves + item_leaves

    def get_user_center(self) -> PatternNode:
        return self.nodes[self.user_center]

    def get_item_center(self) -> PatternNode:
        return self.nodes[self.item_center]

@dataclass
class TIERule:
    pattern: DualStarPattern
    preconditions: List[Predicate]
    conclusion: Optional[EdgePredicate] = None

    def __post_init__(self):
        self._point_wise_cache: Dict[str, List[PointWisePredicate]] = {}
        self._pair_wise_cache: List[PairWisePredicate] = []
        self._ml_cache: List[MLPredicate] = []
        for pred in self.preconditions:
            if isinstance(pred, PointWisePredicate):
                self._point_wise_cache.setdefault(pred.variable, []).append(pred)
            elif isinstance(pred, PairWisePredicate):
                self._pair_wise_cache.append(pred)
            elif isinstance(pred, MLPredicate):
                self._ml_cache.append(pred)

    def get_y_predicate(self) -> Optional[EdgePredicate]:
        return self.conclusion

    def get_point_wise_predicates(self) -> Dict[str, List[PointWisePredicate]]:
        return self._point_wise_cache

    def get_pair_wise_predicates(self) -> List[PairWisePredicate]:
        return self._pair_wise_cache

    def get_ml_predicates(self) -> List[MLPredicate]:
        return self._ml_cache

def canonical_pattern_signature(pattern: Pattern) -> Tuple[Any, ...]:
    node_sig = tuple(sorted(((var, node.label, tuple(sorted(node.attributes)) if node.attributes else ()) for var, node in pattern.nodes.items())))
    edge_sig = tuple(sorted(((edge.source_var, edge.target_var, edge.label, bool(edge.is_forward)) for edge in pattern.edges)))
    centers = (getattr(pattern, 'user_center', None), getattr(pattern, 'item_center', None), getattr(pattern, 'anchor_center', None), getattr(pattern, 'alt_center', None))
    return (type(pattern).__name__, centers, node_sig, edge_sig)

def canonical_tie_rule_signature(rule: TIERule) -> Tuple[Any, ...]:
    return (canonical_pattern_signature(rule.pattern), tuple(sorted((repr(pred) for pred in rule.preconditions))), repr(rule.conclusion) if rule.conclusion is not None else None)

@dataclass
class RuleMetrics:
    support: int = 0
    confidence: float = 0.0
    matched_count: int = 0
    matched_pivots: Set[Tuple[Any, Any]] = field(default_factory=set)
    positive_pivots: Set[Tuple[Any, Any]] = field(default_factory=set)

    @property
    def support_pivots(self):
        return self.matched_pivots

    @property
    def confidence_pivots(self):
        return self.positive_pivots

@dataclass
class PairWiseAnchorInfo:
    pred: PairWisePredicate
    var1_sig: str
    var1_star: str
    var2_sig: str
    var2_star: str

@dataclass
class PathAnchorIndex:
    anchor_to_pivots: Dict[Tuple[str, str, Any], Set[Any]] = field(default_factory=dict)
    pivot_to_anchors: Dict[Any, Set[Tuple[str, str, Any]]] = field(default_factory=dict)
    known_signatures: Set[str] = field(default_factory=set)
    all_pivots_by_sig_attr: Dict[Tuple[str, str], Set[Any]] = field(default_factory=dict)
    grouped_by_value: Dict[Tuple[str, str], Dict[Any, Set[Any]]] = field(default_factory=dict)

    def __getattr__(self, name):
        if name == 'grouped_by_value':
            grouped = {}
            atp = self.__dict__.get('anchor_to_pivots', {})
            for (sig, attr, val), pivots in atp.items():
                grouped.setdefault((sig, attr), {}).setdefault(val, set()).update(pivots)
            self.__dict__['grouped_by_value'] = grouped
            return grouped
        raise AttributeError(f"'{type(self).__name__}' has no attribute '{name}'")

    def query(self, path_sig: str, attr_name: str, attr_value: Any) -> Set[Any]:
        return self.anchor_to_pivots.get((path_sig, attr_name, attr_value), set())

    def query_ne(self, path_sig: str, attr_name: str, attr_value: Any) -> Set[Any]:
        grouped = self.grouped_by_value.get((path_sig, attr_name), {})
        if not grouped:
            return set()
        result: Set[Any] = set()
        for value, pivots in grouped.items():
            if value != attr_value:
                result |= pivots
        return result

    def has_entry(self, path_sig: str) -> bool:
        return path_sig in self.known_signatures

    def query_grouped(self, path_sig: str, attr_name: str) -> Dict[Any, Set[Any]]:
        return self.grouped_by_value.get((path_sig, attr_name), {})

def compute_path_anchor_signature(edge_labels: List[str], node_labels: List[str], is_forward: List[bool]) -> str:
    parts = [node_labels[0]]
    for i, (el, fwd) in enumerate(zip(edge_labels, is_forward)):
        parts.append(el)
        parts.append('F' if fwd else 'R')
        parts.append(node_labels[i + 1])
    return '|'.join(parts)

def build_path_anchor_index(graph: 'Graph', path_templates: List[Tuple[List[str], List[str], List[bool]]], target_attrs: List[str], max_cardinality: int=1000) -> PathAnchorIndex:
    import logging
    logger = logging.getLogger(__name__)
    index = PathAnchorIndex()
    attr_owner = getattr(graph, 'attr_owner_label', {}) or {}
    cardinality_counter: Dict[Tuple[str, str], int] = {}
    cardinality_exceeded: set = set()
    for template_idx, (edge_labels, node_labels, is_forward) in enumerate(path_templates):
        if len(edge_labels) != len(is_forward) or len(node_labels) != len(edge_labels) + 1:
            raise ValueError('path_templates: len(edge_labels) must equal len(is_forward) and len(node_labels) must equal len(edge_labels)+1')
        full_sig = compute_path_anchor_signature(edge_labels, node_labels, is_forward)
        pivot_label = node_labels[0]
        L = len(edge_labels)
        hop_effective_attrs: List[List[str]] = []
        hop_prefix_sigs: List[str] = []
        for step in range(L):
            hop_label = node_labels[step + 1]
            eff = [a for a in target_attrs if attr_owner.get(a) == hop_label or a not in attr_owner]
            hop_effective_attrs.append(eff)
            prefix_sig = compute_path_anchor_signature(edge_labels[:step + 1], node_labels[:step + 2], is_forward[:step + 1])
            hop_prefix_sigs.append(prefix_sig)
        pivot_count = 0
        entry_count = 0
        for pivot_node in graph.get_nodes_by_label(pivot_label):
            pivot_id = pivot_node.node_id
            pivot_count += 1
            current_level: Set[Any] = {pivot_id}
            for step in range(L):
                edge_label = edge_labels[step]
                fwd = is_forward[step]
                expected_label = node_labels[step + 1]
                next_level: Set[Any] = set()
                for node_id in current_level:
                    if fwd:
                        neighbors = graph.iter_out_neighbor_nodes(node_id, edge_label)
                    else:
                        neighbors = graph.iter_in_neighbor_nodes(node_id, edge_label)
                    for neighbor_node in neighbors:
                        if neighbor_node.label == expected_label:
                            next_level.add(neighbor_node.node_id)
                current_level = next_level
                if not current_level:
                    break
                prefix_sig = hop_prefix_sigs[step]
                effective_attrs = hop_effective_attrs[step]
                for reached_id in current_level:
                    reached_node = graph._nodes[reached_id]
                    for attr_name in effective_attrs:
                        sig_attr_key = (prefix_sig, attr_name)
                        if sig_attr_key in cardinality_exceeded:
                            continue
                        attr_value = reached_node.attributes.get(attr_name)
                        if attr_value is None:
                            continue
                        key = (prefix_sig, attr_name, attr_value)
                        grouped_for_sa = index.grouped_by_value.get(sig_attr_key)
                        if grouped_for_sa is None:
                            grouped_for_sa = {}
                            index.grouped_by_value[sig_attr_key] = grouped_for_sa
                        if attr_value not in grouped_for_sa:
                            cnt = cardinality_counter.get(sig_attr_key, 0) + 1
                            cardinality_counter[sig_attr_key] = cnt
                            if cnt > max_cardinality:
                                cardinality_exceeded.add(sig_attr_key)
                                for purge_val, purge_pivots in grouped_for_sa.items():
                                    purge_key = (prefix_sig, attr_name, purge_val)
                                    index.anchor_to_pivots.pop(purge_key, None)
                                    for pid in purge_pivots:
                                        pa = index.pivot_to_anchors.get(pid)
                                        if pa:
                                            pa.discard(purge_key)
                                del index.grouped_by_value[sig_attr_key]
                                index.all_pivots_by_sig_attr.pop(sig_attr_key, None)
                                continue
                        index.anchor_to_pivots.setdefault(key, set()).add(pivot_id)
                        index.pivot_to_anchors.setdefault(pivot_id, set()).add(key)
                        index.all_pivots_by_sig_attr.setdefault(sig_attr_key, set()).add(pivot_id)
                        grouped_for_sa.setdefault(attr_value, set()).add(pivot_id)
                        entry_count += 1
    index.known_signatures = {sig for sig, _, _ in index.anchor_to_pivots}
    total_entries = len(index.anchor_to_pivots)
    return index

@dataclass
class PathInConstruction:
    nodes: List[PatternNode] = field(default_factory=list)
    edges: List[str] = field(default_factory=list)
    edge_directions: List[bool] = field(default_factory=list)

    def length(self) -> int:
        return len(self.edges)

    def last_node(self) -> Optional[PatternNode]:
        return self.nodes[-1] if self.nodes else None

    def to_path(self) -> Path:
        path_nodes = [(n.variable, n.label, set()) for n in self.nodes]
        path = Path(path_nodes)
        for i, edge_label in enumerate(self.edges):
            src_var = self.nodes[i].variable
            tgt_var = self.nodes[i + 1].variable
            fwd = self.edge_directions[i] if i < len(self.edge_directions) else True
            path.add_path_edge(src_var, tgt_var, edge_label, is_forward=fwd)
        return path

    def copy(self):
        return PathInConstruction(nodes=[PatternNode(n.variable, n.label, set(n.attributes)) for n in self.nodes], edges=list(self.edges), edge_directions=list(self.edge_directions))

@dataclass
class RLState:
    user_center_id: Any
    user_center_label: str
    item_center_id: Any
    item_center_label: str
    user_paths: List[PathInConstruction] = field(default_factory=list)
    item_paths: List[PathInConstruction] = field(default_factory=list)
    next_user_var_idx: int = 1
    next_item_var_idx: int = 1

    def num_user_paths(self) -> int:
        return len(self.user_paths)

    def num_item_paths(self) -> int:
        return len(self.item_paths)

    def total_paths(self) -> int:
        return self.num_user_paths() + self.num_item_paths()

    def total_edges(self) -> int:
        return sum((p.length() for p in self.user_paths + self.item_paths))

    def to_pattern(self) -> DualStarPattern:
        pattern = DualStarPattern(user_center='x', user_label=self.user_center_label, item_center='y', item_label=self.item_center_label)
        for path_in_construction in self.user_paths:
            pattern.add_user_path(path_in_construction.to_path())
        for path_in_construction in self.item_paths:
            pattern.add_item_path(path_in_construction.to_path())
        return pattern

    def get_signature(self) -> str:

        def path_sig(path: PathInConstruction) -> str:
            if not path.edges:
                return path.nodes[0].label if path.nodes else ''
            parts = []
            for i, edge_label in enumerate(path.edges):
                src_label = path.nodes[i].label
                tgt_label = path.nodes[i + 1].label
                direction = 'F' if i < len(path.edge_directions) and path.edge_directions[i] or i >= len(path.edge_directions) else 'R'
                parts.append(f'{src_label}-{edge_label}:{direction}-{tgt_label}')
            return '|'.join(parts)
        user_sigs = sorted((path_sig(p) for p in self.user_paths))
        item_sigs = sorted((path_sig(p) for p in self.item_paths))
        return f'U[{';'.join(user_sigs)}]|I[{';'.join(item_sigs)}]'

    def copy(self):
        return RLState(user_center_id=self.user_center_id, user_center_label=self.user_center_label, item_center_id=self.item_center_id, item_center_label=self.item_center_label, user_paths=[p.copy() for p in self.user_paths], item_paths=[p.copy() for p in self.item_paths], next_user_var_idx=self.next_user_var_idx, next_item_var_idx=self.next_item_var_idx)

@dataclass
class AddEdgeAction:
    from_var: str
    edge_label: str
    to_label: str
    star_type: str
    is_forward: bool = True

    def __repr__(self):
        arrow = '--' if self.is_forward else '<-'
        return f'{self.from_var} {arrow}{self.edge_label}{arrow}> ({self.to_label})'

    def is_new_path(self) -> bool:
        return self.from_var in ['x', 'y']

@dataclass
class StopAction:

    def __repr__(self):
        return 'STOP'

class RecPredicate(Predicate):

    def __init__(self, user_var: str, item_var: str):
        self.user_var = user_var
        self.item_var = item_var

    def evaluate(self, is_recommended: bool) -> bool:
        return bool(is_recommended)

    def __repr__(self):
        return f'Rec({self.user_var}, {self.item_var})'

class DisPredicate(Predicate):

    def __init__(self, anchor_var: str, alt_var: str, operator: str, threshold: float):
        self.anchor_var = anchor_var
        self.alt_var = alt_var
        self.operator = operator
        self.threshold = threshold

    def evaluate(self, dis_score: float) -> bool:
        if dis_score is None:
            return False
        if self.operator == '>=':
            return dis_score >= self.threshold
        if self.operator == '>':
            return dis_score > self.threshold
        if self.operator == '<=':
            return dis_score <= self.threshold
        if self.operator == '<':
            return dis_score < self.threshold
        return False

    def __repr__(self):
        return f'Mdis({self.anchor_var}, {self.alt_var}) {self.operator} {self.threshold}'

class AggrPredicate(Predicate):

    def __init__(self, item_var: str, operator: str, threshold: float):
        self.item_var = item_var
        self.operator = operator
        self.threshold = threshold

    def evaluate(self, aggr_score: float) -> bool:
        if aggr_score is None:
            return False
        if self.operator == '>=':
            return aggr_score >= self.threshold
        if self.operator == '>':
            return aggr_score > self.threshold
        if self.operator == '<=':
            return aggr_score <= self.threshold
        if self.operator == '<':
            return aggr_score < self.threshold
        return False

    def __repr__(self):
        return f'Maggr({self.item_var}) {self.operator} {self.threshold}'

class OMRPattern(Pattern):

    def __init__(self, user_center: str, anchor_center: str, alt_center: str, alt_center_label: str, tie_pattern: 'DualStarPattern', alt_star: Optional['StarPattern']=None, bridge_paths: Optional[List['Path']]=None, bridge_origins: Optional[List[str]]=None):
        super().__init__()
        self.user_center = user_center
        self.anchor_center = anchor_center
        self.alt_center = alt_center
        self.alt_center_label = alt_center_label
        self.tie_pattern = tie_pattern
        self.alt_star = alt_star or StarPattern(alt_center, alt_center_label)
        self.bridge_paths: List[Path] = bridge_paths or []
        self.bridge_origins: List[str] = bridge_origins or []
        self._rebuild_merged()

    def _rebuild_merged(self):
        self.nodes.clear()
        self.edges.clear()

        def _merge_node(var: str, pnode: 'PatternNode'):
            if var not in self.nodes:
                self.nodes[var] = pnode
            else:
                existing = self.nodes[var]
                if existing.label != pnode.label:
                    raise ValueError(f"Variable '{var}' has conflicting labels: '{existing.label}' vs '{pnode.label}'")
                if pnode.attributes:
                    if existing.attributes is None:
                        existing.attributes = set(pnode.attributes)
                    else:
                        existing.attributes |= pnode.attributes
        for var, pnode in self.tie_pattern.nodes.items():
            _merge_node(var, pnode)
        for edge in self.tie_pattern.edges:
            if edge not in self.edges:
                self.edges.append(edge)
        for var, pnode in self.alt_star.nodes.items():
            _merge_node(var, pnode)
        for edge in self.alt_star.edges:
            if edge not in self.edges:
                self.edges.append(edge)
        for bridge_path in self.bridge_paths:
            for var, pnode in bridge_path.nodes.items():
                _merge_node(var, pnode)
            for edge in bridge_path.edges:
                if edge not in self.edges:
                    self.edges.append(edge)

    def add_bridge_path(self, path: 'Path', origin: str):
        if origin not in (self.anchor_center, self.user_center):
            raise ValueError(f"Bridge origin must be '{self.anchor_center}' or '{self.user_center}', got '{origin}'")
        self.bridge_paths.append(path)
        self.bridge_origins.append(origin)
        for var, pnode in path.nodes.items():
            if var in self.nodes:
                if self.nodes[var].label != pnode.label:
                    raise ValueError(f"Variable '{var}' has conflicting labels: '{self.nodes[var].label}' vs '{pnode.label}'")
                if pnode.attributes and self.nodes[var].attributes is not None:
                    self.nodes[var].attributes |= pnode.attributes
            else:
                self.nodes[var] = pnode
        for edge in path.edges:
            if edge not in self.edges:
                self.edges.append(edge)

    def add_alt_path(self, path: 'Path'):
        self.alt_star.add_path_from_center(path)
        for var, pnode in path.nodes.items():
            if var not in self.nodes:
                self.nodes[var] = pnode
        for edge in path.edges:
            if edge not in self.edges:
                self.edges.append(edge)

    def get_user_center(self) -> PatternNode:
        return self.nodes[self.user_center]

    def get_anchor_center(self) -> PatternNode:
        return self.nodes[self.anchor_center]

    def get_alt_center(self) -> PatternNode:
        return self.nodes[self.alt_center]

    def get_tie_variables(self) -> Set[str]:
        return set(self.tie_pattern.nodes.keys())

    def get_omr_only_variables(self) -> Set[str]:
        return set(self.nodes.keys()) - set(self.tie_pattern.nodes.keys())

    def get_bridge_paths_from(self, origin: str) -> List['Path']:
        return [p for p, o in zip(self.bridge_paths, self.bridge_origins) if o == origin]

    def degree_of(self, variable: str) -> int:
        count = 0
        for edge in self.edges:
            if edge.source_var == variable or edge.target_var == variable:
                count += 1
        return count

@dataclass
class OMRule:
    pattern: OMRPattern
    parent_tie: TIERule
    preconditions: List[Predicate]
    conclusion: EdgePredicate

    def __post_init__(self):
        self._point_wise_cache: Dict[str, List[PointWisePredicate]] = {}
        self._pair_wise_cache: List[PairWisePredicate] = []
        self._ml_cache: List[MLPredicate] = []
        self._dis_cache: List[DisPredicate] = []
        self._aggr_cache: List[AggrPredicate] = []
        self._rec_cache: List[RecPredicate] = []
        parent_pred_ids = {id(p) for p in self.parent_tie.preconditions}
        self._omr_only_point_wise: Dict[str, List[PointWisePredicate]] = {}
        self._omr_only_pair_wise: List[PairWisePredicate] = []
        for pred in self.preconditions:
            if isinstance(pred, PointWisePredicate):
                self._point_wise_cache.setdefault(pred.variable, []).append(pred)
                if id(pred) not in parent_pred_ids:
                    self._omr_only_point_wise.setdefault(pred.variable, []).append(pred)
            elif isinstance(pred, PairWisePredicate):
                self._pair_wise_cache.append(pred)
                if id(pred) not in parent_pred_ids:
                    self._omr_only_pair_wise.append(pred)
            elif isinstance(pred, MLPredicate):
                self._ml_cache.append(pred)
            elif isinstance(pred, DisPredicate):
                self._dis_cache.append(pred)
            elif isinstance(pred, AggrPredicate):
                self._aggr_cache.append(pred)
            elif isinstance(pred, RecPredicate):
                self._rec_cache.append(pred)

    def get_point_wise_predicates(self) -> Dict[str, List[PointWisePredicate]]:
        return self._point_wise_cache

    def get_pair_wise_predicates(self) -> List[PairWisePredicate]:
        return self._pair_wise_cache

    def get_ml_predicates(self) -> List[MLPredicate]:
        return self._ml_cache

    def get_dis_predicates(self) -> List[DisPredicate]:
        return self._dis_cache

    def get_aggr_predicates(self) -> List[AggrPredicate]:
        return self._aggr_cache

    def get_rec_predicates(self) -> List[RecPredicate]:
        return self._rec_cache

    def get_omr_only_point_wise(self) -> Dict[str, List[PointWisePredicate]]:
        return self._omr_only_point_wise

    def get_omr_only_pair_wise(self) -> List[PairWisePredicate]:
        return self._omr_only_pair_wise

class JointPattern:

    def __init__(self):
        self.nodes: Dict[str, PatternNode] = {}
        self.edges: List[PatternEdge] = []
        self.variable_to_rules: Dict[str, Set[int]] = defaultdict(set)
        self._edge_set: Set[Tuple[str, str, str, bool]] = set()

    @classmethod
    def from_rule_group(cls, rules: List[Union[TIERule, 'OMRule']]) -> 'JointPattern':
        jp = cls()
        for rule_idx, rule in enumerate(rules):
            pattern = rule.pattern
            for var, pnode in pattern.nodes.items():
                if var not in jp.nodes:
                    jp.nodes[var] = pnode
                jp.variable_to_rules[var].add(rule_idx)
            for edge in pattern.edges:
                edge_key = (edge.source_var, edge.target_var, edge.label, edge.is_forward)
                if edge_key not in jp._edge_set:
                    jp._edge_set.add(edge_key)
                    jp.edges.append(edge)
        return jp

    def get_rules_for_var(self, variable: str) -> Set[int]:
        return self.variable_to_rules.get(variable, set())

class RootedDAG:

    def __init__(self):
        self.children: Dict[str, List[str]] = defaultdict(list)
        self.parents: Dict[str, List[str]] = defaultdict(list)
        self.levels: Dict[str, int] = {}
        self.order: List[str] = []
        self.edge_info: Dict[Tuple[str, str], List[Tuple[str, bool]]] = {}

    @classmethod
    def from_joint_pattern(cls, joint_pattern: JointPattern, root_var: str, graph: Optional[Graph]=None) -> 'RootedDAG':
        dag = cls()
        adj: Dict[str, List[Tuple[str, str, bool]]] = defaultdict(list)
        for edge in joint_pattern.edges:
            adj[edge.source_var].append((edge.target_var, edge.label, edge.is_forward))
            adj[edge.target_var].append((edge.source_var, edge.label, not edge.is_forward))
        visited = {root_var}
        queue = [root_var]
        dag.levels[root_var] = 0
        dag.order.append(root_var)

        def _record_edge(parent_var: str, child_var: str, edge_label: str, is_fwd: bool) -> None:
            if child_var not in dag.children[parent_var]:
                dag.children[parent_var].append(child_var)
            if parent_var not in dag.parents[child_var]:
                dag.parents[child_var].append(parent_var)
            key = (parent_var, child_var)
            if key not in dag.edge_info:
                dag.edge_info[key] = []
            entry = (edge_label, is_fwd)
            if entry not in dag.edge_info[key]:
                dag.edge_info[key].append(entry)

        def _bfs_expand(queue_init):
            q = queue_init
            while q:
                next_queue = []
                for parent_var in q:
                    parent_level = dag.levels[parent_var]
                    new_children: Set[str] = set()
                    for neighbor_var, edge_label, is_fwd in adj.get(parent_var, []):
                        if neighbor_var in visited:
                            if dag.levels.get(neighbor_var) == parent_level + 1:
                                _record_edge(parent_var, neighbor_var, edge_label, is_fwd)
                            continue
                        visited.add(neighbor_var)
                        new_children.add(neighbor_var)
                        _record_edge(parent_var, neighbor_var, edge_label, is_fwd)
                        dag.levels[neighbor_var] = parent_level + 1
                        dag.order.append(neighbor_var)
                        next_queue.append(neighbor_var)
                if graph is not None and next_queue:
                    next_queue = _sort_by_selectivity(next_queue, joint_pattern, graph)
                q = next_queue
        _bfs_expand(queue)
        all_vars = set(joint_pattern.nodes.keys())
        unreached = all_vars - visited
        if unreached:
            import logging
            _logger = logging.getLogger(__name__)
            _logger.debug(f"[RootedDAG] {len(unreached)} vars unreachable from root '{root_var}': {unreached}. Grafting to root.")
            while unreached:
                seed = min(unreached)
                visited.add(seed)
                dag.children[root_var].append(seed)
                dag.parents[seed].append(root_var)
                dag.levels[seed] = dag.levels[root_var] + 1
                dag.order.append(seed)
                _bfs_expand([seed])
                unreached = all_vars - visited
        return dag

    def get_topological_order(self) -> List[str]:
        return self.order

    def get_reverse_topological_order(self) -> List[str]:
        return list(reversed(self.order))

    def get_leaves(self) -> List[str]:
        return [v for v in self.order if not self.children.get(v)]

def _sort_by_selectivity(variables: List[str], joint_pattern: JointPattern, graph: Graph) -> List[str]:

    def sort_key(var: str):
        pnode = joint_pattern.nodes.get(var)
        if pnode is None:
            return (float('inf'), 0)
        label_count = len(graph.get_node_ids_by_label_view(pnode.label))
        degree = sum((1 for edge in joint_pattern.edges if edge.source_var == var or edge.target_var == var))
        return (label_count, -degree)
    return sorted(variables, key=sort_key)
__all__ = ['ComparisonOp', 'GraphIndexConfig', 'PathSignature', 'PathSignatureIndex', 'Node', 'Edge', 'Graph', 'Predicate', 'PointWisePredicate', 'PairWisePredicate', 'MLPredicate', 'EdgePredicate', 'PatternNode', 'PatternEdge', 'Pattern', 'Path', 'StarPattern', 'DualStarPattern', 'TIERule', 'canonical_pattern_signature', 'canonical_tie_rule_signature', 'RuleMetrics', 'PairWiseAnchorInfo', 'PathAnchorIndex', 'compute_path_anchor_signature', 'build_path_anchor_index', 'PathInConstruction', 'RLState', 'AddEdgeAction', 'StopAction', 'RecPredicate', 'DisPredicate', 'AggrPredicate', 'OMRPattern', 'OMRule', 'JointPattern', 'RootedDAG', '_safe_compare_predicate_values', '_normalize_predicate_equality_value']
