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
#include "cinn/hlir/framework/general_fuse_group.h"
#include "cinn/hlir/framework/graph.h"

namespace cinn {
namespace hlir {
namespace framework {

class GeneralFusePassContext : public FusePassContext {
 public:
  GeneralFusePassContext(const Graph& graph);

  std::shared_ptr<GeneralFuseGroup> PickGroup();

  void EnableRecompute(const GeneralFuseGroup& op_group);

  void EnableVerticalFuse(const GeneralFuseGroup& first_op_group, const GeneralFuseGroup& second_op_group);

  void EnableHorizontalFuse(const GeneralFuseGroup& first_op_group, const GeneralFuseGroup& second_op_group);

  bool CanHorizontalFuse()
};

}  // namespace framework
}  // namespace hlir
}  // namespace cinn
