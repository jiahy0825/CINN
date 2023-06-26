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

#include <memory>

namespace cinn {
namespace api {

class OpInterface;

class ShapeInterface {
 public:
  virtual bool operator == (const ShapeInterface& other) const = 0;

  virtual size_t& operator[] (size_t index) = 0;

  virtual size_t at(size_t index) const = 0;

  virtual size_t size() const = 0;

  // numel is the number of elements in the shape.
  // for example: shape is [2, 3, 4], numel in shape is 2 * 3 * 4 = 24.
  virtual size_t numel() const = 0;

 protected:
  ShapeInterface() = default;
  ShapeInterface(const ShapeInterface& other) = delete;
  ShapeInterface(ShapeInterface&& other) = delete;
  virtual ~ShapeInterface() = default;
}

class TensorInterface {
 public:
  // Get the shape of tensor.
  virtual const ShapeInterface& shape() const = 0;

  virtual const std::weak_ptr<OpInterface>& producer() const = 0;

  virtual const std::vector<std::weak_ptr<OpInterface>>& consumers() const = 0;

 protected:
  TensorInterface()                       = default;
  TensorInterface(const TensorInterface&) = delete;
  TensorInterface(TensorInterface&&)      = delete;
};

}  // namespace api
}  // namespace cinn
