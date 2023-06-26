// Copyright (c) 2023 CINN Authors. All Rights Reserved.
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

#include "cinn/api/op_group_interface.h"
#include "cinn/api/op_node.h"
#include "cinn/hlir/framework/graph.h"

namespace cinn {
namespace api {

class OpGroup : public OpGroupInterface {
 public:
  OpGroup(std::shared_ptr<hlir::framework::Graph::Group> group) : group_(group) {}

  const OpInterfaceList& input_ops() const override {
    return input_ops_;
  }

  const OpInterfaceList& output_ops() const {
    return output_ops_;
  }

  const OpInterfaceList& all_ops() const {
    return all_ops_;
  }

  const std::unordered_set<std::shared_ptr<OpGroupInterface>>& producers() const override {
    return producers_;
  }

  const std::unordered_set<std::shared_ptr<OpGroupInterface>>& consumers() const override {
    return consumers_;
  }

  void set_input_ops(const OpInterfaceList& input_ops) {
    input_ops_ = input_ops;
  }

  void set_output_ops(const OpInterfaceList& output_ops) {
    output_ops_ = output_ops;
  }

  void set_all_ops(const OpInterfaceList& all_ops) {
    all_ops_ = all_ops;
  }

  void set_producers(std::unordered_set<std::shared_ptr<OpGroupInterface>> producers) {
    producers_ = std::move(producers);
  }

  void set_consumers(std::unordered_set<std::shared_ptr<OpGroupInterface>> consumers) {
    consumers_ = std::move(consumers);
  }

 private:
  std::shared_ptr<hlir::framework::Graph::Group> group_;

  OpInterfaceList input_ops_;
  OpInterfaceList output_ops_;
  OpInterfaceList all_ops_;

  std::unordered_set<std::shared_ptr<OpGroupInterface>> producers_;
  std::unordered_set<std::shared_ptr<OpGroupInterface>> consumers_;
};

// std::vector<std::shared_ptr<OpGroupInterface>> Graph2GeneralFuseGroups(const Graph& graph) {
//   const auto& fusion_groups = graph.fusion_groups;
//   return GroupLite2GeneralFuseGroups(fusion_groups)
// }

std::vector<std::shared_ptr<OpGroupInterface>> GroupLite2GeneralFuseGroups(const std::vector<std::shared_ptr<Group>>& fusion_groups) {
  std::vector<std::shared_ptr<OpGroupInterface>> result;
  result.reserve(fusion_groups.size());
  std::unordered_map<const Group*, std::shared_ptr<OpGroupInterface>> op_group_map;

  // 1. Create GeneralFuseGroup by Graph::Group
  for (const auto& group : fusion_groups) {
    result.push_back(std::make_shared<OpGroup>(group));
    op_group_map[group.get()] = result.back();
  }

  // 2. TODO: Set input and output tensors for OpGroup

  // 3. Set producers and consumers for OpGroup
  for (const auto& group : fusion_groups) {
    const auto& producer_groups = group->producer_groups;
    std::unordered_set<std::shared_ptr<OpGroupInterface>> producers;
    for (const auto& producer_group : producer_groups) {
      producers.insert(op_group_map[producer_group.get()]);
    }
    op_group_map[group.get()]->set_producers(producers);

    const auto& consumer_groups = group->consumer_groups;
    std::unordered_set<std::shared_ptr<OpGroupInterface>> consumers;
    for (const auto& consumer_group : consumer_groups) {
      consumers.insert(op_group_map[consumer_groups.get()]);
    }
    op_group_map[group.get()]->set_producers(consumers);
  }

  return result;
}

std::vector<std::shared_ptr<Group>> GeneralFuseGroups2GroupList(const std::vector<OpGroupInterface>& op_groups) {
  std::vector<std::shared_ptr<Group>> result;
  result.reserve(op_groups.size());

  // 1. Create Graph::Group by OpGroup
  for (const auto& op_group : op_groups) {
    result.push_back(op_group.group_);
  }

  return result;
}

}  // namespace api
}  // namespace cinn
