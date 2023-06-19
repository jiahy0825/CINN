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

#include "cinn/api/tensor_interface.h"
#include "cinn/hlir/framework/node.h"

namespace cinn {
namespace api {

class OpNode final : public OpInterface {
 public:
  OpNode(const hlir::framework::Node& node,  OpPatternKind kind) : op_node_(node), kind_(kind) {} ;

  OpPatternKind kind () const override {
    return kind_;
  }

  const TensorInterfaceList& inputs() const override {
    return inputs_;
  }
  const TensorInterfaceList& outputs() const override {
    return outputs_;
  }

  void set_inputs(const TensorInterfaceList& inputs) {
    inputs_ = inputs;
  }

  void set_outputs(const TensorInterfaceList& outputs) {
    outputs_ = outputs;
  }

  const hlir::framework::Node& GetNode() const {
    return op_node_;
  }

 protected:
  const Attribute& GetAttr(const std::string& attr_name) const {
    return op_node_.attrs.attr_store.at(attr_name);
  }

 private:
  const hlir::framework::Node& op_node_;
  OpPatternKind kind_;
  TensorInterfaceList inputs_;
  TensorInterfaceList outputs_;
};

class OpNodeFactory {
  public:
   OpNodeFactory(const hlir::framework::OpValueType<OpPatternKind>& op_pattern_dict) : op_pattern_dict_(op_pattern_dict) {}

   std::shared_ptr<OpNode> Create(const hlir::framework::Node& node) const {
    OpPatternKind kind = op_pattern_dict_[node.op()];
    if (kind == hlir::framework::kBroadcast) {
      // As binary op was defined as broadcast, actually it should be element-wise.
      if (node.op()->name != "broadcast_to") {
        kind = hlir::framework::kElementWise;
      }
    }
    return std::make_shared<OpNode>(node, kind);
   }

 private:
  // op pattern dict
  const hlir::framework::OpValueType<OpPatternKind>& op_pattern_dict_;
};

}  // namespace api
}  // namespace cinn
