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

import argparse
import itertools
import unittest

parser = argparse.ArgumentParser(description="Argparse for op test helper")
parser.add_argument(
    "--case",
    type=int,
    help="Which case you want to test, default -1 for all cases.",
    default=-1)
args = parser.parse_args()


class TestCaseHelper():
    """
    Helper class for constructing test cases.
    """

    def init_attrs(self):
        """
        Initialize attributes for op
        """
        raise Exception("Not implemented.")

    def _flatten_tuple(self, cur_tuple):
        """
        Expand the nested dict in tuple
        """
        new_dict = []
        for cur_dict in cur_tuple:
            for k, v in cur_dict.items():
                new_dict.append((k, v))
        return dict(new_dict)

    def _init_cases(self):
        """
        Generate all test cases
        """
        self.all_cases = []
        attrs_cases = (dict(zip(self.attrs.keys(), values))
                       for values in itertools.product(*self.attrs.values()))
        for case in itertools.product(self.inputs, self.dtypes, attrs_cases):
            self.all_cases.append(self._flatten_tuple(case))

    def _make_all_classes(self):
        """
        Generate test classes
        """
        self.init_attrs()
        self._init_cases()
        self.all_classes = []
        if args.case >= 0:
            self.all_classes.append(
                type(f'{self.class_name}{args.case}', (self.cls, ),
                     {"case": self.all_cases[args.case]}))
        else:
            for i, case in enumerate(self.all_cases):
                self.all_classes.append(
                    type(f'{self.class_name}{i}', (self.cls, ),
                         {"case": case}))

    def run(self):
        """
        Run all test classes
        """
        self._make_all_classes()
        test_suite = unittest.TestSuite()
        test_loader = unittest.TestLoader()
        for x in self.all_classes:
            test_suite.addTests(test_loader.loadTestsFromTestCase(x))
        runner = unittest.TextTestRunner()
        runner.run(test_suite)
