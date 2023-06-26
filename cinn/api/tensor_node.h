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

class ShapeInfo : public ShapeInterface {
 public:
  ShapeInfo(const ShapeInfo& other) = default;
  ShapeInfo(const hlir::framework::shape_t& other) : data_(other.begin(), other.end()) {}

  bool operator == (const ShapeInterface& other) const override {
    if (this->size() != other.size() || this->numel() != other.numel()) {
        return false;
    }
    for (int i = 0; i < data_.size(); i++) {
      if (data_[i] != other.at(i)) {
          return false;
      }
    }
    return true;
  }

  size_t& operator[] (size_t index) override{
    return data_[index];
  }

  size_t at(size_t index) const override {
    return data_.at(index);
  }

  size_t size() const override {
    return data_.size();
  }

  size_t numel() const {
    return std::accumulate(data_.begin(), data_.end(), 1, std::multiplies<int>());
  }

 private:
  std::vector<size_t> data_;
};

class TensorNode : public TensorInterface {
 public:
  TensorNode(const hlir::framework::NodeData& node_data, const ShapeInfo& shape) : tensor_node_(node_data), shape_(shape) {}

  // Get the shape of tensor.
  const ShapeInterface& shape() const override{
    return shape_;
  }

  const std::weak_ptr<OpInterface>& producer() const override {
    return producer_;
  }

  const std::vector<std::weak_ptr<OpInterface>>& consumers() const {
    return consumers_;
  }

  void set_procuder(const std::shared_ptr<OpInterface>& producer){
    producer_ = producer;
  }

  void set_consumers(const std::vector<std::shared_ptr<OpInterface>>& consumers){
    consumers_.reserve(consumers.size());
    for(const auto& consumer : consumers) {
      consumers_.push_back(consumer);
    }
  }

  const hlir::framework::NodeData& GetNodeData() const {
    return tensor_node_;
  }

 private:
  ShapeInfo shape_;
  const hlir::framework::NodeData& tensor_node_;
  std::weak_ptr<OpInterface> producer_;
  std::vector<std::weak_ptr<OpInterface>> consumers_;
};


class TensorNodeFactory {
  public:
   TensorNodeFactory(const absl::flat_hash_map<std::string, hlir::framework::shape_t>& shape_dict) : shape_dict_(shape_dict) {}

   std::shared_ptr<TensorNode> Create(const hlir::framework::NodeData& node_data) const{
    const hlir::framework::shape_t& shape = shape_dict_.at(node_data.id());
    ShapeInfo tensor_shape(shape);
    return std::make_shared<TensorNode>(node_data, tensor_shape);
   }

 private:
  // shape dict
  const absl::flat_hash_map<std::string, hlir::framework::shape_t>& shape_dict_;
};

}  // namespace api
}  // namespace cinn
