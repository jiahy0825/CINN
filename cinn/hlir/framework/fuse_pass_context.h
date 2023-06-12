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

namespace cinn {
namespace hlir {
namespace framework {


class FusePassContext {
 public:
  FusePassContext() = default;

  virtual std::shared_ptr<OpGroupInterface> PickGroup() = 0;

  virtual void EnableRecompute(const OpGroupInterface& op_group) = 0;

  virtual void EnableVerticalFuse(const OpGroupInterface& first_op_group, const OpGroupInterface& second_op_group) = 0;

  virtual void EnableHorizontalFuse(const OpGroupInterface& first_op_group, const OpGroupInterface& second_op_group) = 0;

};

}  // namespace framework
}  // namespace hlir
}  // namespace cinn
