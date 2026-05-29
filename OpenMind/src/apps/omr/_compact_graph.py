import logging
from typing import Any, Dict, List, Tuple
from collections import defaultdict
logger = logging.getLogger(__name__)

class CompactGraph:
    __slots__ = ('id_to_int', 'int_to_id', 'node_labels', 'node_attrs', 'out_neighbors', 'in_neighbors', 'n_nodes')

    def __init__(self):
        self.id_to_int: Dict[Any, int] = {}
        self.int_to_id: List[Any] = []
        self.node_labels: List[str] = []
        self.node_attrs: List[Dict[str, Any]] = []
        self.out_neighbors: Dict[Tuple[int, str], List[int]] = {}
        self.in_neighbors: Dict[Tuple[int, str], List[int]] = {}
        self.n_nodes: int = 0

    def get_out_neighbor_ints(self, node_int: int, edge_label: str) -> List[int]:
        return self.out_neighbors.get((node_int, edge_label), [])

    def get_in_neighbor_ints(self, node_int: int, edge_label: str) -> List[int]:
        return self.in_neighbors.get((node_int, edge_label), [])

def build_compact_graph(graph) -> CompactGraph:
    cg = CompactGraph()
    for i, (node_id, node) in enumerate(graph._nodes.items()):
        cg.id_to_int[node_id] = i
        cg.int_to_id.append(node_id)
        cg.node_labels.append(node.label)
        cg.node_attrs.append(node.attributes)
    cg.n_nodes = len(cg.int_to_id)
    id_to_int = cg.id_to_int
    out_neighbors = {}
    in_neighbors = {}
    out_by_label = getattr(graph, '_out_edges_by_label', None)
    in_by_label = getattr(graph, '_in_edges_by_label', None)
    if out_by_label is not None and in_by_label is not None:
        for (src_id, elabel), edges in out_by_label.items():
            src_int = id_to_int.get(src_id)
            if src_int is None:
                continue
            tgt_ints = []
            for e in edges:
                tgt_int = id_to_int.get(e.target)
                if tgt_int is not None:
                    tgt_ints.append(tgt_int)
            if tgt_ints:
                out_neighbors[src_int, elabel] = tgt_ints
        for (tgt_id, elabel), edges in in_by_label.items():
            tgt_int = id_to_int.get(tgt_id)
            if tgt_int is None:
                continue
            src_ints = []
            for e in edges:
                src_int = id_to_int.get(e.source)
                if src_int is not None:
                    src_ints.append(src_int)
            if src_ints:
                in_neighbors[tgt_int, elabel] = src_ints
    else:
        out_tmp = defaultdict(list)
        in_tmp = defaultdict(list)
        for edge in graph._edges.values():
            src_int = id_to_int.get(edge.source)
            tgt_int = id_to_int.get(edge.target)
            if src_int is not None and tgt_int is not None:
                out_tmp[src_int, edge.label].append(tgt_int)
                in_tmp[tgt_int, edge.label].append(src_int)
        out_neighbors = dict(out_tmp)
        in_neighbors = dict(in_tmp)
    cg.out_neighbors = out_neighbors
    cg.in_neighbors = in_neighbors
    return cg
