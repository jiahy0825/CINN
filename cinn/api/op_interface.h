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

#include <vector>

#include "cinn/api/tensor_interface.h"
#include "cinn/api/tensor_interface_list.h"
#include "cinn/utils/type_defs.h"
#include "cinn/hlir/framework/op.h"

namespace cinn {
namespace api {

using OpPatternKind = cinn::hlir::framework::OpPatternKind;
using Attribute = cinn::utils::Attribute;

class OpInterface {
 public:
  virtual OpPatternKind kind () const = 0;

  virtual const TensorInterfaceList& inputs() const = 0;
  virtual const TensorInterfaceList& outputs() const = 0;

  template <typename T>
  const T& GetAttr(const std::string& attr_name) const {
    return absl::get<T>(GetAttr(attr_name));
  }

 protected:
  OpInterface()                       = default;
  OpInterface(const OpInterface&) = delete;
  OpInterface(OpInterface&&)      = delete;

  virtual const Attribute& GetAttr(const std::string& attr_name) const = 0;
};

}  // namespace api
}  // namespace cinn
