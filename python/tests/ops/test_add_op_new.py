# Copyright (c) 2023 CINN Authors. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import unittest
import numpy as np
from op_test import OpTest, OpTestTool
from op_test_helper import TestCaseHelper
import paddle
import cinn
from cinn.frontend import *
from cinn.common import *


@OpTestTool.skip_if(not is_compiled_with_cuda(),
                    "x86 test will be skipped due to timeout.")
class TestElementwiseAddOp(OpTest):
    def setUp(self):
        print(f"\nRunning {self.__class__.__name__}: {self.case}")
        self.prepare_inputs()

    def prepare_inputs(self):
        self.x_np = self.random(
            shape=self.case["x_shape"],
            dtype=self.case["x_dtype"],
            low=-10,
            high=10)
        self.y_np = self.random(
            shape=self.case["y_shape"],
            dtype=self.case["y_dtype"],
            low=-10,
            high=10)
        self.dout_np = self.random(
            self.case["dout_shape"], dtype=self.case["dout_dtype"])

    def build_paddle_program(self, target):
        x = paddle.to_tensor(self.x_np, stop_gradient=False)
        y = paddle.to_tensor(self.y_np, stop_gradient=False)

        def get_unsqueeze_axis(x_rank, y_rank, axis):
            self.assertTrue(
                x_rank >= y_rank,
                "The rank of x should be greater or equal to that of y.")
            axis = axis if axis >= 0 else x_rank - y_rank
            unsqueeze_axis = np.arange(0, axis).tolist() + np.arange(
                axis + y_rank, x_rank).tolist()
            return unsqueeze_axis

        unsqueeze_axis = get_unsqueeze_axis(
            len(x.shape), len(y.shape), self.case["axis"])
        y_t = paddle.unsqueeze(
            y, axis=unsqueeze_axis) if len(unsqueeze_axis) > 0 else y
        out = paddle.add(x, y_t)

        self.paddle_outputs = [out]
        self.paddle_grads = self.get_paddle_grads([out], [x, y],
                                                  [self.dout_np])

    def build_cinn_program(self, target):
        builder = NetBuilder("add")
        x = builder.create_input(
            self.nptype2cinntype(self.case["x_dtype"]), self.case["x_shape"],
            "x")
        y = builder.create_input(
            self.nptype2cinntype(self.case["y_dtype"]), self.case["y_shape"],
            "y")
        out = builder.add(x, y, axis=self.case["axis"])

        dout = builder.create_input(
            self.nptype2cinntype(self.case["dout_dtype"]),
            self.case["dout_shape"], "dout")
        x_grad, y_grad = builder.elementwise_add_grad(
            dout, x, y, axis=self.case["axis"])

        prog = builder.build()
        res = self.get_cinn_output(prog, target, [x, y, dout],
                                   [self.x_np, self.y_np, self.dout_np],
                                   [out, x_grad, y_grad])

        self.cinn_outputs = [res[0]]
        self.cinn_grads = [res[1], res[2]]

    def test_check_results(self):
        self.check_outputs_and_grads()


class TestAddAll(TestCaseHelper):
    def init_attrs(self):
        self.class_name = "TestElementwiseAddOpCase"
        self.cls = TestElementwiseAddOp
        self.inputs = [
            {
                "x_shape": [3],
                "y_shape": [3],
                "dout_shape": [3],
                "axis": -1
            },
            {
                "x_shape": [3, 3],
                "y_shape": [1],
                "dout_shape": [3, 3],
                "axis": -1
            },
            {
                "x_shape": [3, 3, 3],
                "y_shape": [3, 3, 3],
                "dout_shape": [3, 3, 3],
                "axis": 0
            },
        ]
        self.dtypes = [
            {
                "x_dtype": "int32",
                "y_dtype": "int32",
                "dout_dtype": "int32"
            },
            {
                "x_dtype": "float32",
                "y_dtype": "float32",
                "dout_dtype": "float32"
            },
        ]
        self.attrs = {
            # "axis": [-1, 0],
        }


if __name__ == "__main__":
    TestAddAll().run()
