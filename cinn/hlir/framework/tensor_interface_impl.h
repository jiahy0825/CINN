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

#include "cinn/hlir/framework/tensor_interface.h"
#include "cinn/hlir/framework/node.h"

namespace cinn {
namespace hlir {
namespace framework {

class ShapeInfo : public ShapeInterface, public std::vector<int32_t> {}

class TensorNode : public TensorInterface {
 public:
  TensorNode(const std::shared_ptr<NodeData>& node_data, const ShapeInfo& shape) : tensor_node_(node_data), shape_(shape) {}

  // Get the shape of tensor.
  const ShapeInterface& shape() const {
    return shape_;
  }

 private:
  std::shared_ptr<NodeData> tensor_node_;
  ShapeInfo shape_;
};


class TensorNodeFactory {
  public:
   TensorNodeFactory(const absl::flat_hash_map<std::string, shape_t>& shape_dict) : shape_dict_(shape_dict) {}

   std::shared_ptr<TensorNode> Create(const std::shared_ptr<NodeData>& node_data) {
    ShapeInfo shape = shape_dict_.at(node_data->id());
    return std::make_shared<TensorNode>(node_data, shape);
   }

 private:
  // shape dict
  const absl::flat_hash_map<std::string, shape_t>& shape_dict_;
}

}  // namespace framework
}  // namespace hlir
}  // namespace cinn
