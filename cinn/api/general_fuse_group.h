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

#include "cinn/hlir/framework/op_group_interface.h"
#include "cinn/hlir/framework/graph.h"
#include "cinn/hlir/framework/tensor_interface_impl.h"


namespace cinn {
namespace hlir {
namespace framework {


class GeneralFuseGroup : public OpGroupInterface {
 public:
  GeneralFuseGroup(GroupPtr group) : group_(group) {
    TensorNodeFactory
  }

  const TensorInterfaceList& input_tensors() const {
    return input_tensors_;
  }

  const TensorInterfaceList& output_tensors() const {
    return output_tensors_;
  }

  const std::unordered_set<std::shared_ptr<OpGroupInterface>>& producers() const {
    return producers_;
  }

  const std::unordered_set<std::shared_ptr<OpGroupInterface>>& consumers() const {
    return consumers_;
  }

 private:
  bool set_producers(std::unordered_set<std::shared_ptr<OpGroupInterface>> producers) {
    producers_ = std::move(producers);
    return true;
  }

  bool set_consumers(std::unordered_set<std::shared_ptr<OpGroupInterface>> consumers) {
    consumers_ = std::move(consumers);
    return true;
  }

  friend std::vector<std::shared_ptr<OpGroupInterface>> GroupLite2GeneralFuseGroups(const std::vector<std::shared_ptr<Group>>& fusion_groups);
  friend std::vector<std::shared_ptr<Group>> GeneralFuseGroups2GroupList(const std::vector<OpGroupInterface>& op_groups)

 private:
  std::shared_ptr<Graph::Group> group_;

  TensorInterfaceList input_tensors_;
  TensorInterfaceList output_tensors_;

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



}  // namespace framework
}  // namespace hlir
}  // namespace cinn
