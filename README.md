# Overview

This repository is the official code of "**OpenMind: Query-Time Diversification via Logical Rules**".

Recommender systems are a major driver of filter bubbles, in which users are repeatedly exposed to opinions aligned with their existing views. We propose OpenMind, a lightweight, query-time framework that extends recommendation queries with diverse, alternative and even opposing viewpoints. Rather than replacing or retraining existing recommendation models, OpenMind augments query results by returning complementary items as optional extended answers, preserving both efficiency and accuracy. At the core of OpenMind is a class of logical rules, called OMRs (Open Mind Rules), which operate over a user-item interaction graph to identify high-quality content that differs in perspective while remaining relevant and engaging. We develop efficient algorithms that integrate OMRs into both click-through-rate (CTR) and top-𝑘 query processing, executing in the same inference pass as the underlying recommender. Using real-world datasets, we empirically verify that OpenMind improves exposure to diverse views at negligible query processing cost without loss in accuracy.

![Exp](https://anonymous.4open.science/r/OpenMind-F735/OpenMind.png)

* The **full version** of the paper can be accessed at this file: [`OpenMind_full_version.pdf`](https://anonymous.4open.science/r/OpenMind-F735/OpenMind_full_version.pdf)
* The **dataset** are available at this link: [Google Drive](https://drive.google.com/drive/folders/1OgKpzwj1nf_T5BhXLAsIOPe4mW3OHcin)

## Software requirements

```shell
python >= 3.12
numpy==1.26.4
scipy==1.13.1
scikit-learn==1.4.2
PyYAML==6.0.1
tqdm==4.66.4
torch==2.6.0
transformers==4.46.3
datasets==4.8.4
FlagEmbedding==1.3.5
sentencepiece==0.2.1
safetensors==0.7.0
accelerate==1.13.0
einops>=0.7.0
openpyxl==3.1.2
Cython==3.2.4
setuptools>=68
pytest>=8.0
```

## Installing dependencies on Ubuntu

GCC version: 7.4.0 or above, support of c++17 standard required.

Install mpi:

```shell
sudo apt-get install openmpi-bin openmpi-doc libopenmpi-dev
```

Install glog:

```shell
sudo apt-get install libgoogle-glog-dev
```

Install gflags:

```shell
sudo apt-get install libgflags-dev
```

Install yaml:

```shell
sudo apt-get install libyaml-cpp-dev
```

## Compile

```shell
mkdir build && cd ./build
cmake ../
make all -j
```

## Run

We use libgrape-lite for multi-process parallelism and openmp for multi-thread parallelism.

For LR discovery, to run with single machine, occupying all threads:

```shell
./build/gar_discover --yaml_file ${yaml_file_name}
```

To run with single machine, occupying a specified number of threads:

```shell
mpirun -n 1 -map-by slot:pe=core_num ./build/gar_discover --yaml_file ${yaml_file_name}
```

To run with multiple machines:

```shell
mpirun -N xxx -n yyy -c zzz ./build/gar_discover --yaml_file ${yaml_file_name}
```

For rule match, to run with single machine, occupying all threads:

```shell
./build/rule_match --yaml_file ${yaml_file_name}
```

The others are same as discovery.

The main loop of raw level-wise discovery can be found in folder LMiner/src/apps/rule_discover/, and the main loop of LR match, pattern matching can be found in folder LMiner/src/apps/rule_match/.

The ER folder contains code for computing 1-WL, where you can employ any feature (such as SimCSE or GloVe) embedding to determine whether a pair of points, as well as all pairs within a graph. 

Detailed running examples can be viewed within this folder. 

If you want to run LR discovery algorithm, you may need to fill a yaml file in this format:

```yaml
DataGraphPath: # the path for the data graphs
  - VFile: the vertex file for the first data graph
    EFile: the edge file for the first data graph
    MlLiteralEdgesFile: (optional) the edges that are added by the well-trained ml model for the first data graph
  ...
ExpandRound: number of expand round, i.e. total edges to be added
J: depth of the literal tree for horizontal spawning.
SupportBound: the support bound for the gar to be discovered
OutputGarDir: the directory for the discovered gar to export
TimeLimit: time limit for evaluating the support bound of each gar or graph pattern
TimeLimitPerSupp: time limit for it to complete the match of the entire pattern of gar at each support
ConstantFreqBound: the frequency bound for the constant, only the value appear larger than this frequence would be considered
PatternVertexLimit: the limit of pattern vertex
DiameterLimit: the limit of the diameter of the graph pattern
LiteralTypes: # the literal types to be considered
  - constant_literal
  - variable_literal
  - edge_literal
Restrictions: # the restrictions for the gar
  - variable_literal_only_between_connected_vertexes
  - edge_literal_only_between_2_hop_connected_vertexes
  - literals_connected
  - pattern_without_loop
SpecifiedRhsLiteralSet:
  - Type: variable_literal
    XLabel: label of x
    YLabel: label of y
    XAttrKey: attr of x
    YAttrKey: attr of y
TimeLogFile: the path for the time log file
```

An example yaml file for LR discovery may like this:

```yaml
DataGraphPath:
  VFile : dataset/v.csv
  EFile : dataset/e.csv
ExpandRound: 15
J: 3
LiteralTypes:
  - constant_literal
  - variable_literal
  - edge_literal

SupportBound: 1
ConfidenceBound: 0.4

Rule:
  Type: gcr
  PathNumLimit: 3
  PathLengthLimit: 5
  
SpecifiedRhsLiteralSet:
  - Type: variable_literal
    XLabel: 3
    YLabel: 3
    XAttrKey: year
    YAttrKey: year
  
TimeLogFile:  dataset/lr.log
OutputGarDir: dataset/lr

TimeLimit: 3000
TimeLimitPerSupp: 0.5
ConstantFreqBound: 0.09
```

If you want to run LR pattern matching algorithm, you may need to fill a yaml file in this format:

```yaml
DataGraphPath: 
  VFile : vertex file of the data graph
  EFile : edge file of the data graph
  
PatternPath:
  VFile : vertex file of the pattern
  EFile : edge file of the pattern
  XFile : X (lhs) literal file of the pattern
  YFile : Y (rhs) literal file of the pattern
  PivotId : (optional) specify the pivot vertex id, needs to be contained in the Y literals of the pattern

TimeLogFile: time log file
```

## Literal CSV format

### Literals

| type | x_id | x_attr | y_id | y_attr | edge_label |  c   |
| :--: | :--: | :----: | :--: | :----: | :--------: | :--: |

Different kinds of literals would use different columns.

### Attribute literal

Format:

|   type    | x_id | x_attr | y_id | y_attr | edge_label |  c   |
| :-------: | :--: | :----: | :--: | :----: | :--------: | :--: |
| Attribute |  x   |   A    |  -   |   -    |     -      |  -   |

Semantics:

```
x.A
```

Vertex *x* has attribute *A*.

### Variable literal

Format:

|   type   | x_id | x_attr | y_id | y_attr | edge_label |  c   |
| :------: | :--: | :----: | :--: | :----: | :--------: | :--: |
| Variable |  x   |   A    |  y   |   B    |     -      |  -   |

Semantics:

```
x.A = y.B
```

The attribute *A* of *x* is the same as attribute *B* of *y*.

### Constant literal

Format:

|   type   | x_id | x_attr | y_id | y_attr | edge_label |  c   |
| :------: | :--: | :----: | :--: | :----: | :--------: | :--: |
| Constant |  x   |   A    |  -   |   -    |     -      |  c   |

Semantics:

```
x.A = c
```

The attribute *A* of *x* is equal to *c*.

More detailly, the data type of *c* in constant literal needs to be specified as the following example:

> |   type   | x_id | x_attr | y_id | y_attr | edge_label |        c        |
> | :------: | :--: | :----: | :--: | :----: | :--------: | :-------------: |
> | Constant |  4   | genres |  -   |   -    |     -      | \|Comedy;string |

Which specifies that the attribute *genres* of vertex with id *4* is equal to *|Comedy* in string.

### Edge literal

Format:

| type | x_id | x_attr | y_id | y_attr | edge_label |  c   |
| :--: | :--: | :----: | :--: | :----: | :--------: | :--: |
| Edge |  x   |   -    |  y   |   -    |     l      |  -   |

Semantics:

```
x -(l)-> y
```

There is an edge with label *l* from vertex *x* to *y*.

### ML literal

Format:

| type | x_id | x_attr | y_id | y_attr | edge_label |  c   |
| :--: | :--: | :----: | :--: | :----: | :--------: | :--: |
|  Ml  |  x   |   -    |  y   |   -    |     l      |  -   |

Semantics:

```
x - ml(l) -> y
```

The ML model can predict an edge with label *l* from vertex *x* to *y*.

## Set of literals

The above *literal format* allow users to store multiply literals in the same file, and the *literal set format* further allows to divide the literals into different sets by an additional column *gar_id* :

| type | x_id | x_attr | y_id | y_attr | edge_label |  c   | **gar_id** |
| :--: | :--: | :----: | :--: | :----: | :--------: | :--: | :--------: |

As an example, the following example shows that there are two literals in the *example_x.csv*:

> |   type   | x_id | x_attr | y_id | y_attr | edge_label |  c   |
> | :------: | :--: | :----: | :--: | :----: | :--------: | :--: |
> | Constant |  x   |   A    |  -   |   -    |     -      |  c   |
> |   Edge   |  x   |   -    |  y   |   -    |     l      |  -   |

By adding the additional column *gar_id*, the following *example_x_set.csv* represent there are two single literal:

> |   type   | x_id | x_attr | y_id | y_attr | edge_label |  c   | gar_id |
> | :------: | :--: | :----: | :--: | :----: | :--------: | :--: | :----: |
> | Constant |  x   |   A    |  -   |   -    |     -      |  c   |   0    |
> |   Edge   |  x   |   -    |  y   |   -    |     l      |  -   |   1\   |

[head file](/include/gar/csv_gar.h):

```
/include/gar/csv_gar.h
```

## CSV file format

Both a single GAR and a set of GARs are stored in four files seperately:

* v.csv / v_set.csv

  > The vertexes of Gar / Gar set, see [csv format of graph](/include/gundam/doc/user_doc/csv_format.md) in GUNDAM.

* e.csv / e_set.csv

  > The edges of Gar / Gar set, see [csv format of graph](/include/gundam/doc/user_doc/csv_format.md) in GUNDAM.

* x.csv / x_set.csv

  > The set of literals contained in X of Gar / Gar set, see [csv format of literal](/doc/user_doc/literal_csv_format.md).

* y.csv / y_set.csv

  > The set of literals contained in Y of Gar / Gar set, see [csv format of literal](/doc/user_doc/literal_csv_format.md).

## Useful method

### Read

```c++
template <typename PatternType, typename DataGraphType>
int ReadGAR(GraphAssociationRule<PatternType, DataGraphType> &gar,
            const std::string &v_file, const std::string &e_file,
            const std::string &x_file, const std::string &y_file);
```

### ReadSet

```c++
template <typename PatternType, typename DataGraphType>
int ReadGARSet(
    std::vector<GraphAssociationRule<PatternType, DataGraphType>> &gar_set,
    const std::string &v_set_file, const std::string &e_set_file,
    const std::string &x_set_file, const std::string &y_set_file);

template <typename PatternType, typename DataGraphType>
int ReadGARSet(
    std::vector<GraphAssociationRule<PatternType, DataGraphType>> &gar_set,
    std::vector<std::string> &gar_name_set, 
    const std::string &v_set_file, const std::string &e_set_file, 
    const std::string &x_set_file, const std::string &y_set_file);
```

### Write

```c++
template <typename PatternType, typename DataGraphType>
int WriteGAR(
    const GraphAssociationRule<PatternType, DataGraphType> &gar,
    const std::string &v_file, const std::string &e_file,
    const std::string &x_file, const std::string &y_file);
```

### WriteSet

```c++
template <typename PatternType, typename DataGraphType>
int WriteGARSet(
    const std::vector<GraphAssociationRule<PatternType, DataGraphType>> &gar_set,
    const std::string &v_file, const std::string &e_file,
    const std::string &x_file, const std::string &y_file);

template <typename PatternType, typename DataGraphType>
int WriteGARSet(
    const std::vector<GraphAssociationRule<PatternType, DataGraphType>> &gar_set,
    const std::vector<std::string> &gar_name_list, 
    const std::string &v_file, const std::string &e_file, 
    const std::string &x_file, const std::string &y_file);
```

### RL

By default, all commands are assumed to be executed from the repository root. In most ordinary situations, the RL-related workflow begins from an already prepared dataset directory rather than from a raw-data reconstruction step, and this is generally the most natural way to think about it. 

In practice, RL is mainly used here as the upstream pattern-generation stage that precedes later mining stages. This makes it conceptually early in the workflow even if users do not always interact with it directly every time. The important thing to keep in mind is that the output of RL is not usually treated as an end in itself, but rather as an artifact that is subsequently consumed elsewhere. In other words, RL training serves as a preparatory stage whose results are meant to be handed off to the phases that follow.

```bash
python rl_pattern_generator.py \
  --dataset steam \
  --num-patterns 300 \
  --num-episodes 500
```

```python
from rl_training_suite import DualStarRLEnvironment, PolicyNetwork, REINFORCETrainer

env = DualStarRLEnvironment(graph, training_data)
policy = PolicyNetwork()
trainer = REINFORCETrainer(env, policy, device="cuda")
trainer.train(num_episodes=500)
trainer.save_checkpoint("checkpoints/rl_policy_steam.pt")
```

If one wishes to say something slightly more paper-like about what is going on internally, the real computational heart of this stage is not especially mysterious: trajectories are sampled, discounted returns are computed, and policy gradients are pushed in the direction of trajectories that turned out to be more rewarding. 

```python
def _compute_returns(self, rewards: List[float]) -> torch.Tensor:
    returns = []
    R = 0
    for r in reversed(rewards):
        R = r + self.gamma * R
        returns.insert(0, R)
    ...
    return returns

def _update_policy(self, trajectory: Dict) -> float:
    log_probs = trajectory['log_probs']
    returns = trajectory['returns'].to(self.device)
    loss = -(log_probs * returns).mean()
    ...
```

What this is doing, stated in a deliberately ceremonial way, is assigning retrospective credit to a sequence of sampled decisions after the episode has already revealed whether those decisions were collectively useful. In other words, the code is not judging a single move in isolation so much as it is judging a sampled path through the action space as a whole and then nudging the policy toward paths of the sort that tended to work out better. This is the familiar REINFORCE story, merely written here in the exact conventions of the current repository rather than in the more sanitized notation one would encounter in a formal description.

Once this stage has run, the practical expectation is that a pattern artifact becomes available for downstream use. In the standard layout, this commonly appears as a file such as `data/steam/steam_rl_patterns.pkl`. The exact naming convention is less important than the general idea that a concrete pattern file now exists and can be handed off to TIE mining, which is the next thing most users care about once the RL phase has completed in a satisfactory way.

### TIE

By default, all commands should be executed from the repository root directory. These artifacts are the result of prior processing steps, and the intent of making them available in this form is to allow users to proceed with downstream tasks without first needing to regenerate them from scratch. 

Before any of the downstream tasks can be meaningfully initiated, it is customary to first load the dataset into memory. This loading step serves as a foundational prerequisite for essentially everything that follows, and as such, it is generally advisable to perform it at the very beginning of any session. The loaded bundle will be referenced repeatedly in later stages, and keeping it in scope throughout the workflow is typically the most convenient arrangement, though users with more specialized needs may choose to adapt this pattern to their own preferences as appropriate.

```python
from data_loader import load_dataset

bundle = load_dataset("data/steam", user_label="user", item_label="game")
graph = bundle.graph
training_data = bundle.training_data
test_data = bundle.test_data
ml_cache = bundle.ml_cache
```

The resulting `bundle` exposes several components, each of which plays a role at some point in the broader pipeline. Some of these components are used immediately, while others come into play only at later stages, and the exact pattern of usage will depend on which entry points the user ultimately chooses to invoke. It is generally considered good practice to retain access to all of these components for the duration of the session, since situations occasionally arise in which a component that seemed unnecessary at first turns out to be relevant later on.

TIE-related functionality can be thought of as falling into two broad groups, namely mining and matching. These two groups are conceptually distinct but are frequently used in conjunction with one another as part of a larger workflow. In the usual course of events, mining is performed first, producing a set of rules, and matching is then performed afterward, consuming those rules in one way or another. This ordering reflects the natural progression of the pipeline and is the pattern that most users will want to follow in practice, although the overall system is designed with enough flexibility that variations on this theme are also possible depending on specific circumstances.

Before proceeding with TIE mining, it is generally assumed that some upstream pattern generation has already taken place, and that the resulting pattern file is available at a known location on disk. TIE mining then consumes this pattern file directly, which is the standard and recommended convention. Users who are following along for the first time are encouraged to verify that the expected pattern file is indeed present before continuing, as its absence would otherwise become apparent only at the point of invocation.

```bash
python tie_miner.py \
  --data-dir data/steam \
  --patterns-file data/steam/steam_rl_patterns.pkl \
  --min-support 1000 \
  --min-confidence 0.8
```

```python
from data_structure import MLPredicate
from tie_miner import TIELearner

learner = TIELearner(
    graph=graph,
    training_data=training_data,
    ml_cache=ml_cache,
    user_label="user",
    item_label="game",
)

tie_rules = learner.learn(
    ml_predicates=[MLPredicate("x0", "y0", ">=", 0.5)],
    patterns_file="data/steam/steam_rl_patterns.pkl",
)
```

If one now shifts from the interface level to a more algorithmic description, TIE mining can be viewed as a progressive search over a candidate predicate set, beginning from seed groups and extending them as long as the intermediate rule remains worth extending. 

```python
delta = self._build_predicate_set(pattern)
seed_groups = self._select_seed_groups(pattern, delta, max_groups=3)
...
initial_matches = self._compute_all_compact_matches(
    pattern, initial_preconditions, training_data
)
initial_metrics = self._compute_metrics_from_compact_matches(...)
...
for level in range(remaining_budget):
    X_next = self._expand_level_parallel(...)
    for new_X_tuple, (new_X, child_matches, is_valid, exp_metrics) in X_next.items():
        if is_valid:
            valid_rules.append((rule, exp_metrics))
```

The logic here is, broadly speaking, that the miner does not attempt to jump directly to a finished rule in one motion. Instead, it first constructs a universe of plausible predicates, chooses a few reasonable seeds, evaluates whether those seeds already support something nontrivial, and then expands outward level by level. This has the rhetorical flavor of a search procedure over the space of symbolic descriptions, but operationally it is simply a disciplined way of saying: start from something small, keep what remains promising, and avoid spending too much effort on branches that are already drifting toward irrelevance.

The same idea becomes even more visible in the incremental expansion path, where the implementation reuses parent matches rather than recomputing the world from scratch each time a predicate is added. The corresponding condensed fragment looks like this.

```python
if self.use_gini and candidates:
    gini_scores = self._compute_gini_batch_optimized(
        pattern, current_preconditions, candidates, training_data,
        parent_matches=parent_matches
    )
    gini_scores.sort(key=lambda x: x[1])
    selected = [pred for pred, _ in gini_scores[:self.gini_top_k]]
...
for (u, v), compact_match in parent_matches.items():
    filtered_match = self._filter_compact_match_by_predicate(
        compact_match, pred, u, v
    )
    if filtered_match is not None:
        child_matches[u, v] = filtered_match
metrics = self._compute_metrics_from_compact_matches(
    child_matches, training_data, ...
)
```

In somewhat grander language than the code itself requires, this is the point at which the miner behaves less like an uninformed enumerator and more like a guided search routine. Gini is being used as a ranking signal so that not every conceivable next predicate has to be treated with equal seriousness, while `parent_matches` function as a compact witness space that can be narrowed incrementally. 

Once mining has completed and a set of TIE rules has been produced, the natural next step for most workflows is to move on to matching. Matching itself encompasses two related but distinct concerns: single-pair matching on the one hand, and overall verification on the other. These two concerns are typically addressed through two separate entry points, and although they can in principle be invoked independently of one another, they are very often used together in practice as part of a combined routine. The exact way in which they are composed is ultimately left to the caller, and reasonable people may arrive at slightly different conventions depending on the particular demands of their situation.

```python
from tie_matcher import compute_compact_match, verify_variable_predicates, compute_rule_metrics

rule = tie_rules[0]

compact_match = compute_compact_match(
    graph,
    rule.pattern,
    uid,
    vid,
    rule.get_point_wise_predicates(),
)

is_match = compact_match is not None and verify_variable_predicates(
    graph,
    compact_match,
    rule.get_pair_wise_predicates(),
)

metrics = compute_rule_metrics(graph, rule, training_data, ml_cache)
```

At the level of the underlying logic, `compute_rule_metrics(...)` is essentially where a symbolic rule stops being merely a declarative object and starts being treated as something that has to survive contact with the actual graph. 

```python
user_candidates, item_candidates = get_pivot_candidates(
    graph, rule.pattern, point_wise_preds, debug=debug
)
...
user_candidates, item_candidates, ..., ... = apply_path_anchor_filters(
    graph, rule.pattern, rule.preconditions,
    user_candidates, item_candidates,
    training_index=training_index,
    training_data=training_data,
    ...
)
...
matched_pivots.add((u, v))
if _check_conclusion_edge(graph, u, v, rule, training_data=training_data):
    positive_pivots.add((u, v))
...
support = len(matched_pivots)
confidence = len(positive_pivots) / support if support > 0 else 0.0
```

The conceptual story here is fairly straightforward even if the implementation contains a great deal of engineering detail around it. First, the candidate space is narrowed as much as possible before expensive matching is allowed to happen in earnest. Then, among the remaining pivot pairs, the system distinguishes between pairs that satisfy the structural and predicate-side conditions of the rule and pairs that additionally satisfy the target conclusion edge. In this sense, support is telling us how many witnesses the rule can gather, while confidence is telling us how often those witnesses also line up with the relation the rule claims to explain. That is the high-level semantics, even though the real implementation understandably contains many optimizations around how those witnesses are found.

The general expectation is that users will invoke these entry points in roughly the order shown above, though as noted earlier, the orchestration is ultimately a matter for the caller to determine. Users working with unfamiliar datasets are encouraged to inspect intermediate results as they go, since doing so tends to build confidence that the overall workflow is proceeding as intended.

### OMR

OMR is conceptually layered on top of TIE, and in the usual sequence of operations the former is built directly upon rules that have already been produced by the latter. This layered relationship is fairly characteristic of how the system is intended to be used, and being mindful of it helps ensure that the various stages of the pipeline fit together in the manner that was originally envisioned. Users who have not yet worked through the preceding TIE section may find it helpful to do so before continuing, as several of the conventions introduced earlier continue to apply here in broadly similar form.

OMR mining is initiated in a manner that mirrors the general shape of TIE mining, with the principal difference being the specific collection of inputs that need to be assembled in advance. As was the case earlier, both a command-line entry point and a corresponding Python-level interface are available, and users may choose whichever of the two is more convenient for their particular use case. Neither choice carries any substantive implication beyond convenience itself, and in most practical settings the two produce results that are aligned with one another.

```bash
python omr_miner.py \
  --data-dir data/steam \
  --tie-rules-file rules/steam/rules_xxx.pkl \
  --min-support 100 \
  --min-confidence 0.7
```

The Python-level interface offers somewhat more granular control over the various components involved, which can be useful in contexts where the user wishes to customize specific aspects of the procedure. For the great majority of straightforward use cases, however, the command-line interface is entirely sufficient, and reaching for the Python-level interface should generally be regarded as an option to be exercised when the situation specifically calls for it.

```python
from attribute_config import load_attribute_config
from tie_matcher import compute_rule_metrics
from omr_models import DisModel, AggrModel, TopicProximityModel
from omr_miner import OMRLearner

attr_config = load_attribute_config("data/steam", graph)
tie_metrics = [compute_rule_metrics(graph, r, training_data, ml_cache) for r in tie_rules]

dis_model = DisModel(graph, edge_label="has_tag")
aggr_model = AggrModel(graph, interaction_edge_label="plays")
topic_model = TopicProximityModel(graph, category_attr="genre")

omr_learner = OMRLearner(
    graph=graph,
    training_data=training_data,
    ml_cache=ml_cache,
    tie_rules=tie_rules,
    tie_metrics=tie_metrics,
    dis_model=dis_model,
    aggr_model=aggr_model,
    user_label="user",
    item_label="game",
    min_support=100,
    min_confidence=0.7,
    max_bridge_hops=2,
    max_alt_paths=2,
    max_predicates=5,
    dis_thresholds=[0.3, 0.5, 0.7],
    aggr_thresholds=[0.1, 0.3, 0.5],
    max_omr_per_tie=10,
    n_workers=8,
    time_limit=3600,
    data_dir="data/steam",
    attr_config=attr_config,
)

omr_rules = omr_learner.learn()
```

If one wants to narrate OMR mining in a more paper-facing tone, the main point is that an OMR rule is not treated as an entirely new discovery problem divorced from TIE. Rather, it is explicitly evaluated as an extension of an already matched parent TIE rule. 

```python
if parent_tie_metrics is None or not parent_tie_metrics.positive_pivots:
    parent_tie_metrics = compute_rule_metrics(
        graph, omr_rule.parent_tie, training_data, ml_cache, ...
    )
...
matched_pivots = parent_tie_metrics.matched_pivots
...
w_preds = omr_pw.get(w_var, [])
if w_preds:
    prefiltered = _prefilter_by_attribute_index(graph, w_label, w_preds)
    ...
else:
    w_candidates_base = graph.get_node_ids_by_label(w_label)
...
if aggr_preds and w_candidates_base:
    ...
```

What is happening here, in practical rather than theatrical terms, is that the OMR stage begins from the parent TIE's already established anchor-side evidence and then asks whether a usable alternative item can be justified on top of that anchor. The alternative side is filtered by its own predicates, then by dissimilarity- and popularity-oriented constraints, and only then is it considered a serious recommendation candidate. So although OMR is often discussed as an additional layer, it is perhaps more accurate to say that it is a conditional extension layer whose entire meaning depends on the parent TIE having already carved out a credible anchor space.

After OMR rules have been produced, they become available for consumption by the subsequent matching stages. These matching stages support two distinct recommendation settings, namely CTR and TopK, and both are covered by dedicated entry points. Users may optionally choose to construct an offline index before invoking these entry points, or they may proceed without doing so. Both approaches are fully supported, and the decision of whether to build such an index is ultimately left to the user's own discretion based on whatever considerations happen to be most relevant to the task at hand.

```python
from offline_index import build_offline_index
from omr_matcher import coupled_ctr
from topk_matcher import coupled_topk

offline_index = build_offline_index(
    graph,
    tie_rules,
    omr_rules,
    dis_model=dis_model,
    aggr_model=aggr_model,
)

anchors, alternatives = coupled_ctr(
    graph=graph,
    user_id=uid,
    sigma_tie=tie_rules,
    sigma_omr=omr_rules,
    dis_model=dis_model,
    aggr_model=aggr_model,
    ml_cache=ml_cache,
    lambda_param=0.6,
    topic_model=topic_model,
    offline_index=offline_index,
)

K_u, Psi = coupled_topk(
    graph=graph,
    user_id=uid,
    candidate_items=candidate_items,
    tie_rules=tie_rules,
    omr_rules=omr_rules,
    dis_model=dis_model,
    aggr_model=aggr_model,
    ml_cache=ml_cache,
    delta_L=0.6,
    delta_H=0.9,
    k=10,
    alt_k=11,
    lambda_param=0.6,
    topic_model=topic_model,
    offline_index=offline_index,
)
```

For CTR, the algorithmic picture is that TIE and OMR are not run as two completely isolated passes that happen never to speak to each other. Rather, they are coupled inside a shared candidate space and coordinated by a unified scoring routine. 

```python
user_ml_cache = (
    ml_cache.get_user_view(user_id)
    if hasattr(ml_cache, "get_user_view")
    else ml_cache
)
score_accumulators: Dict[Any, Dict[Any, float]] = {}
anchor_items: Set[Any] = set()
...
scorer = RankingScoreComputer(
    graph=graph,
    dis_model=dis_model,
    aggr_model=aggr_model,
    ml_cache=user_ml_cache,
    lambda_param=lambda_param,
    sigma_omr_size=max(sigma_omr_size, 1),
    topic_model=topic_model,
)
...
for tie_idx, tie_rule in enumerate(sigma_tie):
    H, joint_pattern, rooted_dag = construct_rule_group(user_id, tie_rule, sigma_omr, graph)
    candidate_map, all_pw = init_candidate_map(...)
    ...
```

The slightly more literary summary would be that CTR here is implemented as a coordinated refinement process rather than a naive chaining of independent modules. A user-specific ML view is prepared, a shared scorer is instantiated, TIE groups are formed, and OMR rules are then evaluated in the context created by those groups rather than in some detached universe of their own. This is why the implementation reads less like a simple pipeline and more like a coupled procedure that keeps several moving parts in conversation with one another until a stable recommendation picture emerges.

TopK adds another layer of organization. The implementation first decides which anchors deserve to occupy the visible TopK positions and then manages alternatives under a more global budgeted structure. 

```python
K_u_list, sigma_prime_tie = anchor_select(
    graph=graph,
    user_id=user_id,
    candidate_items=candidate_items_set,
    tie_rules=tie_rules,
    ml_cache=ml_cache,
    delta_L=delta_L,
    delta_H=delta_H,
    k=k,
    ...
)
...
active_omrs_by_tie_idx, _ = _active_topk_omrs_by_tie_index(...)
...
score_accumulators: Dict[Any, Dict[Any, float]] = {v: {} for v in K_u}
global_heap: List[Tuple[float, int, Any]] = []
best_alt_scores: Dict[Any, float] = {}
tau_global = 0.0
scorer = RankingScoreComputer(...)
```

The logic here is that anchor selection is treated as a first-class phase rather than as an incidental by-product of alternative scoring. Once the anchor set is fixed, the algorithm keeps track of alternative competition in a global structure, which is why one sees objects such as `global_heap`, `best_alt_scores`, and `tau_global` appear so early in the routine. In a paper-style paraphrase, one might say that the method is trying to maintain a disciplined separation between anchor selection and alternative diversification, while still allowing the scoring machinery underneath to remain unified.

For reference, the common output shapes associated with these entry points are summarized as follows. The `coupled_ctr(...)` call returns a pair of the form `(anchors, alternatives)`, while the `coupled_topk(...)` call returns a pair of the form `(K_u, Psi)`. In addition, the `tie_miner.py` script produces files matching the pattern `rules_*.pkl`, and the `omr_miner.py` script produces files matching the pattern `omr_rules_*.pkl`. These output conventions are consistent across datasets and should generally behave as one would expect.

When switching to a different dataset, the set of adjustments required is typically limited in scope and involves synchronizing `data_dir` and `item_label` with the new dataset, along with updating the edge label configuration of `DisModel` and `AggrModel` accordingly. Beyond these synchronizations, most of the rest of the workflow tends to carry over in a straightforward manner, though as always, users are encouraged to verify that the expected outputs are produced at each stage before proceeding further.

