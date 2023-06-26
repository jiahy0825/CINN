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

#include <absl/types/any.h>
#include "cinn/api/op_group_interface.h"

namespace cinn {
namespace api {

using any = absl::any;

class FusePassContext {
 public:
  virtual void EnableFuse(const OpGroupInterface& first_op_group, const OpGroupInterface& second_op_group) = 0;

  // User can cache some group info in context by using this function.
  // The group info can be any data and need to create by create_fn.
  virtual any* FindOrCreateCachedGroupInfo(const OpGroupInterface& op_group, const std::function<any(const OpGroupInterface& op_group)>& create_fn) = 0;

 protected:
  FusePassContext() = default;
};

}  // namespace api
}  // namespace cinn
