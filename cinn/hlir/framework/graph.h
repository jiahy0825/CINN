// Copyright (c) 2021 CINN Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once
#include <absl/container/flat_hash_map.h>
#include <absl/types/any.h>

#include <atomic>
#include <memory>
#include <string>
#include <vector>

#include "cinn/common/graph_utils.h"
#include "cinn/frontend/syntax.h"
#include "cinn/hlir/framework/node.h"
#include "cinn/hlir/framework/op_group_interface.h"
#include "cinn/hlir/framework/tensor_interface_list.h"

namespace cinn {
namespace hlir {
namespace framework {

/**
 * \brief Symbolic computation graph.
 *  This is the intermediate representation for optimization pass.
 */
class Graph : public cinn::common::Graph {
 public:
  Graph(const frontend::Program& prog, const Target& target) {
    std::unordered_set<std::string> fetch_var_ids;
    Initialize(prog, fetch_var_ids, target);
  }
  Graph(const frontend::Program& prog, const std::unordered_set<std::string>& fetch_var_ids, const Target& target) {
    Initialize(prog, fetch_var_ids, target);
  }

  void Initialize(const frontend::Program& prog,
                  const std::unordered_set<std::string>& fetch_var_ids,
                  const Target& target);

  Target target_;
  /** \brief outputs of the computation graph. */
  std::vector<NodeData*> outputs;

  /** \brief attributes of a graph */
  absl::flat_hash_map<std::string, std::shared_ptr<absl::any>> attrs;

  std::vector<std::vector<Node*>> groups;
  class Group final : public OpGroupInterface {
   public:
    // distance to last group.
    int depth{0};
    int max_depth{0};
    int min_depth{INT_MAX};
    // group id, consisted of node's id.
    std::string group_id{""};
    // global unique id.
    std::string unique_id{UniqName("")};
    // node in this group
    std::vector<Node*> nodes;
    std::unordered_set<Node*> nodes_set;
    // input nodes of the group.
    std::unordered_map<Node*, int> input_nodes;
    // output nodes of the group.
    std::unordered_set<Node*> output_nodes;
    // op pattern kind.
    framework::OpPatternKind op_pattern_kind{framework::kElementWise};
    // internal node, the output is used by multi-node.
    // internal node can't use compute inline, should use buffer.
    std::unordered_set<Node*> internal_nodes;
    // master node for schedule
    std::unordered_set<Node*> master_nodes;

    // fused sub-groups, used for fusion merge pass
    std::vector<std::shared_ptr<Group>> fused_sub_groups;
    // if as sub-group, used for belong groups.
    std::unordered_set<std::shared_ptr<Group>> belong_groups;

    // for op lowering.
    std::vector<std::string> input_names;
    std::vector<std::string> output_names;

    // const std::unordered_map<std::shared_ptr<OpGroupInterface>, TensorInterfaceList>& producer_groups() const
    // override {
    //   return producer_groups_;
    // }
    // std::unordered_map<std::shared_ptr<OpGroupInterface>, TensorInterfaceList>* mut_producer_groups() {
    //   return &producer_groups_;
    // }

    const TensorInterfaceList& input_tensors() const override {}

    const TensorInterfaceList& output_tensors() const override {}

    const std::unordered_set<std::shared_ptr<OpGroupInterface>> producers() const override {}

    const std::unordered_set<std::shared_ptr<OpGroupInterface>> consumers() const override {}

    std::unordered_set<std::shared_ptr<Group>> CollectConsumerGroups() {
      std::unordered_set<std::shared_ptr<Group>> groups;
      for (const auto& consumer_and_list : consumer_groups) {
        groups.insert(consumer_and_list.first);
      }
      return groups;
    }

    std::vector<Node*> CollectNodes() {
      if (fused_sub_groups.size()) {
        std::vector<Node*> tmp_nodes;
        for (auto& group : fused_sub_groups) {
          tmp_nodes.insert(tmp_nodes.end(), group->nodes.begin(), group->nodes.end());
        }
        return tmp_nodes;
      } else {
        return nodes;
      }
    }

    std::unordered_set<Node*> NodeSet() {
      std::unordered_set<Node*> node_set;
      for (auto node : CollectNodes()) {
        node_set.insert(node);
      }
      return node_set;
    }

    std::unordered_set<NodeData*> GetInputNodeDatas();
    std::unordered_set<NodeData*> GetOutputNodeDatas();

    std::string GetFuncName() { return "fn_" + group_id + unique_id; }

    //  private:
    // input groups
    std::unordered_map<std::shared_ptr<Group>, TensorInterfaceList> producer_groups;
    // output grous
    std::unordered_map<std::shared_ptr<Group>, TensorInterfaceList> consumer_groups;
  };
  std::vector<std::shared_ptr<Group>> fusion_groups;

  void RegisterNode(size_t key, Node* node) { this->common::Graph::RegisterNode(key, node->as<common::GraphNode>()); }
  void RegisterNode(size_t key, NodeData* node) {
    this->common::Graph::RegisterNode(key, node->as<common::GraphNode>());
  }
  void RegisterNode(const std::string& key, Node* node) {
    this->common::Graph::RegisterNode(key, node->as<common::GraphNode>());
  }
  void RegisterNode(const std::string& key, NodeData* node) {
    this->common::Graph::RegisterNode(key, node->as<common::GraphNode>());
  }

  /**
   * \brief Get the immutable attribute from attrs.
   * @param attr_name the name of the attribute
   * @return the reference to corresponding attribute
   * @tparam T the type of the attribute.
   */
  template <typename T>
  inline const T& GetAttrs(const std::string& attr_name) const {
    auto it = attrs.find(attr_name);
    CHECK(it != attrs.end()) << "Cannot find attribute [" << attr_name << "] in the graph";
    return absl::any_cast<const T&>(*it->second);
  }

  /**
   * \brief Get the mutable attribute from attrs.
   * @param attr_name the name of the attribute
   * @return the reference to corresponding attribute
   * @tparam T the type of the attribute.
   */
  template <typename T>
  inline T& GetMutableAttrs(const std::string& attr_name) {
    auto it = attrs.find(attr_name);
    CHECK(it != attrs.end()) << "Cannot find attribute [" << attr_name << "] in the graph";
    return absl::any_cast<T&>(*it->second);
  }

  /**
   * \brief Check whether has a specific attribute.
   * @param attr_name the name of the attribute
   * @return a boolean result
   */
  inline bool HasAttr(const std::string& attr_name) const {
    auto it = attrs.find(attr_name);
    return it != attrs.end();
  }

  /**
   * \brief Debug the grouped graph according to fusion_groups.
   */
  std::string DebugGroupedGraph(const std::unordered_set<std::string>& fetch_var_ids = {});
  std::string DebugGroupedGraph(const std::vector<Node*>& group,
                                const std::unordered_set<std::string>& fetch_var_ids = {});

  /**
   * \brief Debug the grouped graph with GraphViz dot format according to fusion_groups.
   */
  std::string VisualizeGraph(const std::unordered_set<std::string>& fetch_var_ids = {});
  std::vector<std::string> VisualizeGroups(const std::unordered_set<std::string>& fetch_var_ids = {});

  /**
   * \brief Genereate the python test code for group test
   */
  std::string GenerateGroupPythonCode(const std::vector<Node*>& group,
                                      const std::unordered_set<std::string>& fetch_var_ids = {});

  /**
   * \brief Visualize the grouped graph according to fusion_groups.
   */
  void VisualizeGroupedGraph(const std::unordered_set<std::string>& fetch_var_ids = {});

  /**
   * \brief Visualize the grouped graph according to user specified groups.
   */
  void VisualizeGroupedGraph(const std::vector<std::vector<Node*>>& groups,
                             const std::unordered_set<std::string>& fetch_var_ids = {});

  void SaveSourceCode(const std::string& code);
  void SavePTXCode(const std::string& ptx);

 private:
  std::string DebugGroupedGraph(const std::vector<std::vector<Node*>>& groups,
                                const std::unordered_set<std::string>& fetch_var_ids = {});

  std::string VisualizeGraph(const std::vector<std::vector<Node*>>& groups,
                             const std::unordered_set<std::string>& fetch_var_ids = {});

  std::vector<std::string> VisualizeGroups(const std::vector<std::vector<Node*>>& groups,
                                           const std::unordered_set<std::string>& fetch_var_ids = {});

  std::vector<std::vector<Node*>> FusionGroupsToGroups();

  std::string viz_path_;
  static std::atomic_size_t viz_count_;

  CINN_DISALLOW_COPY_AND_ASSIGN(Graph);
};

}  // namespace framework
}  // namespace hlir
}  // namespace cinn
