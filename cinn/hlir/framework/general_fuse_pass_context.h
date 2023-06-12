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

#include "cinn/hlir/framework/fuse_pass_context.h"
#include "cinn/hlir/framework/graph.h"

namespace cinn {
namespace hlir {
namespace framework {


class GeneralFusePassContext {
 public:
  GeneralFusePassContext(Graph* graph) = default;

  Graph* graph() {
    return graph;
  }

  std::shared_ptr<OpGroupInterface> PickGroup() {
    CHECK(0 <= current_group_index_ && current_group_index_ < op_groups_.size())
          << "Can't find group with current index: " << current_group_index_ << ", the groups size is " <<  op_groups_.size();
    return op_groups_[i++];
  }

  void EnableRecompute(const OpGroupInterface& op_group) {
    op_group_fuse_tag_[&op_group] = std::make_shared<Tag>(Tag::Recompute);
  }

  bool CanRecompute(const OpGroupInterface& op_group) const{
    auto iter = op_group_fuse_tag_.find(&op_group);
    if (iter != op_group_fuse_tag_.end()) {
      return *iter->second == Tag::Recompute;
    }
    return false;
  }

  void EnableVerticalFuse(const OpGroupInterface& first_op_group, const OpGroupInterface& second_op_group) {
    auto tag = std::make_shared<Tag>(Tag::VerticalFuse);
    op_group_fuse_tag_[&first_op_group] = tag;
    op_group_fuse_tag_[&second_op_group] = tag;
  }

  bool CanVerticalFuse(const OpGroupInterface& first_op_group, const OpGroupInterface& second_op_group) const{
    auto iter1 = op_group_fuse_tag_.find(&first_op_group);
    if (iter1 != op_group_fuse_tag_.end()) {
       auto iter2 = op_group_fuse_tag_.find(&second_op_group);
       if (iter2 != op_group_fuse_tag_.end()) {
         return iter1->second == iter2->second && *iter1->second == Tag::VerticalFuse;
       }
    }
    return false;
  }

  void EnableHorizontalFuse(const OpGroupInterface& first_op_group, const OpGroupInterface& second_op_group) {
    auto tag = std::make_shared<Tag>(Tag::HorizontalFuse);
    op_group_fuse_tag_[&first_op_group] = tag;
    op_group_fuse_tag_[&second_op_group] = tag;
  }

  bool CanHorizontalFuse(const OpGroupInterface& first_op_group, const OpGroupInterface& second_op_group) {
    auto iter1 = op_group_fuse_tag_.find(&first_op_group);
    if (iter1 != op_group_fuse_tag_.end()) {
       auto iter2 = op_group_fuse_tag_.find(&second_op_group);
       if (iter2 != op_group_fuse_tag_.end()) {
         return iter1->second == iter2->second && *iter1->second == Tag::HorizontalFuse;
       }
    }
    return false;
  }

  void InsertOpGroup(const std::shared_ptr<OpGroupInterface>& op_group) {
    op_groups_.push_back(op_group);
    op_groups_set_.insert(op_groups_.back().get());
  }

  void DeleteOpGroup(const std::shared_ptr<OpGroupInterface>& op_group) {
    op_groups_set_.erase(op_group.get());
    op_group_fuse_tag_.erase(op_group.get());
  }

 private:

  enum class Tag {
    Recompute,
    VerticalFuse,
    HorizontalFuse
  };

  Graph* graph_;

  std::vector<std::shared_ptr<OpGroupInterface>> op_groups_;
  int64_t current_group_index_ = -1;
  std::unordered_set<const OpGroupInterface*> op_groups_set_;
  std::unordered_map<const OpGroupInterface*, std::shared_ptr<Tag>> op_group_fuse_tag_;
};

}  // namespace framework
}  // namespace hlir
}  // namespace cinn
