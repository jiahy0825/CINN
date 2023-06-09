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

class GeneralFusePassContext : public FusePassContext {
 public:
  GeneralFusePassContext(const Graph& graph);

  std::shared_ptr<OpGroupInterface> PickGroup();

  void EnableRecompute(const OpGroupInterface& op_group);

  void EnableVerticalFuse(const OpGroupInterface& first_op_group, const OpGroupInterface& second_op_group);

  void EnableHorizontalFuse(const OpGroupInterface& first_op_group, const OpGroupInterface& second_op_group);

  bool CanHorizontalFuse(const OpGroupInterface& iter1, const OpGroupInterface& iter2);

  void CleanGroup(const OpGroupInterface& group);

  void InsertGroup(const OpGroupInterface& group);
};

}  // namespace framework
}  // namespace hlir
}  // namespace cinn
