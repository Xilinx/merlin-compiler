# (C) Copyright 2016-2021 Xilinx, Inc.
# All Rights Reserved.
#
# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License. 

"""
Module for data extraction from vivado_hls.log

"""

import os.path
import re
import utility
#from merlin_report_setting import MerlinReportSetting

#pylint: disable=invalid-name, fixme, too-many-public-methods
class LogParser:
    """ Parser for vivado_hls.log """
    def __init__(self, settings, path):
        self.settings = settings
        self.dumpfile = "%s.json" % os.path.basename(path).split(".")[0]
        self._id = 0
        self.flatten_loops = []
        with open(path, "r") as f:
            self.lines = f.readlines()

    def id(self):
        """ Returns a unique number for internal use """
        self._id += 1
        return self._id

    #pylint: disable=unused-variable
    def parse_1(self, curr):
        """ Search "Unrolling loop ..." """
        d = {}
        regexp = (r"Unrolling loop '(\S+?)' \((.*):(\d+?)\) in function "
                  r"'(\S+?)' completely with a factor of (\d+).")
        match = re.search(regexp, self.lines[curr])
        if match:
            name, path, line_num, function, factor = match.groups()
            d[self.settings.attr_name] = name
            # ? d['funcname'] = function
            d[self.settings.attr_filename] = os.path.basename(path)
            if line_num.isdigit():
                d[self.settings.attr_line] = int(line_num)
            d[self.settings.attr_unrolled] = 'yes'
            d[self.settings.attr_unroll_factor] = factor
            return {"L,%d" % self.id(): d}
        return {}

    #pylint: disable=unused-variable
    def parse_2(self, curr):
        """ Search "Unrolling small iteration loop ..." """
        d = {}
        regexp = (r"Unrolling small iteration loop '(\S+?)' \((.*):(\d+?)\) "
                  r"in function '(\S+?)' automatically.")
        match = re.search(regexp, self.lines[curr])
        if match:
            name, path, line_num, function = match.groups()
            d[self.settings.attr_name] = name
            #? d['funcname'] = function
            d[self.settings.attr_filename] = os.path.basename(path)
            if line_num.isdigit():
                d[self.settings.attr_line] = int(line_num)
            d[self.settings.attr_unrolled] = 'yes'
            return {"L,%d" % self.id(): d}
        return {}

    # FIXME: Unrolling all sub-loops, the loop it self is not unrolled, do we
    # need to check if "Unrolling loop"
    # parser can cover all loops
    #pylint: disable=unused-variable
    def parse_3(self, curr):
        """ Search "Unrolling all sub-loops inside loop ..." """
        d = {}
        regexp = (r"Unrolling all sub-loops inside loop '(\S+?)' "
                  r"\((.*):(\d+?)\) in function '(\S+?)' for pipelining.")
        match = re.search(regexp, self.lines[curr])
        if match:
            name, path, line_num, function = match.groups()
            d[self.settings.attr_name] = name
            #? d['funcname'] = function
            d[self.settings.attr_filename] = os.path.basename(path)
            if line_num.isdigit():
                d[self.settings.attr_line] = int(line_num)
            d[self.settings.attr_unrolled] = 'yes'
            d['flatten-sub-loop'] = 'yes'
            return {"L,%d" % self.id(): d}
        return {}

    #pylint: disable=unused-variable
    def parse_4(self, curr):
        """ Search "Inlining function ..." """
        d = {}
        regexp = r"Inlining function '(\S+?)' into '(\S+?)' \((.*):(\d+?)\)."
        match = re.search(regexp, self.lines[curr])
        if match:
            func_name, p_func_name, path, line_num = match.groups()
            d[self.settings.attr_name] = func_name
            #? d[self.settings.attr_filename] = os.path.basename(path)
            #? d[self.settings.attr_line] = int(line_num)
            d[self.settings.attr_inline] = 'yes'
            return {"F,%d" % self.id(): d}
        return {}

    def parse_5(self, curr):
        """ Search "Partitioning array ..." """
        d = {}
        regexp = (r"Partitioning array '(\S+?)' \((.*):(\d+?)\) in dimension "
                  r"(\d+) with a cyclic factor (\d+).")
        match = re.search(regexp, self.lines[curr])
        if match:
            name, path, line_num, dim, factor = match.groups()
            d[self.settings.attr_name] = name
            #? d['groupname'] = name
            d[self.settings.attr_filename] = os.path.basename(path)
            if line_num.isdigit():
                d[self.settings.attr_line] = int(line_num)
            d['partition'] = 'yes'
            d[self.settings.attr_partition_dim] = dim
            d[self.settings.attr_cyclic_factor] = factor
            return {"A,%d" % self.id(): d}
        return {}

    #pylint: disable=missing-docstring, unused-argument, no-self-use
    def parse_6(self, curr):
        regexp = r"Found false inter dependency for variable"
        return {}

    #pylint: disable=missing-docstring, no-self-use
    def parse_7(self, curr):
        return {}

    def parse_8(self, curr):
        d = {}
        regexp = r"Flattening a loop nest '(\S+?)' \((.*):(\d+?):\d+?\)"
        match = re.search(regexp, self.lines[curr])
        if match:
            name, path, line_num = match.groups()
            d['name'] = name
            self.flatten_loops.append(name)
            d[self.settings.attr_filename] = os.path.basename(path)
            if line_num.isdigit():
                d[self.settings.attr_line] = int(line_num)
            d['flatten'] = 'yes'
            return {"L,%d" % self.id(): d}
        return {}

    #pylint: disable=missing-docstring, no-self-use
    def parse_9(self, curr):
        return {}

    #pylint: disable=missing-docstring, no-self-use
    def parse_10(self, curr):
        return {}

    #pylint: disable=missing-docstring, no-self-use
    def parse_11(self, curr):
        return {}

    def try_to_match_12_1_1(self, curr, result):
        regexp = (
            r"between '(?P<opt1>\w+)' operation [(]'\w+', .+?:\d+(?:->.+?:\d+)*[)] of "
            r"variable '(?P<var>\w+)', (?P<st_arr_path>.+?):(?P<st_arr_line>\d+) on array "
            r"'(?P<st_arr>\S+)'"
            r" and '(?P<opt2>\w+)' operation [(]'\w+', (?P<ld_arr_path>.+?):(?P<ld_arr_line>\d+)[)]"
            r" on array '(?P<ld_arr>\S+)'\."
        )
        match = re.search(regexp, self.lines[curr])
        if match:
            (opt1, var, st_arr_path, st_arr_line, st_arr,
             opt2, ld_arr_path, ld_arr_line, ld_arr) = match.groups()

            result[self.settings.attr_ii_violation][opt1] = {
                var: "",
                st_arr.split("[", 1)[0]: "%s:%s" % (
                    os.path.basename(st_arr_path), st_arr_line
                )
            }

            result[self.settings.attr_ii_violation][opt2] = {
                ld_arr.split("[", 1)[0]: "%s:%s" % (
                    os.path.basename(ld_arr_path), ld_arr_line
                )
            }
            return True
        else:
            return False


    def try_to_match_12_1_4(self, curr, result):
        regexp = (
            r"between '(?P<opt1>\w+)' operation [(]'\w+', .+?:\d+(?:->.+?:\d+)*[)] of "
            r"variable '(?P<var>\w+).*', .* on local variable "
            r"'(?P<st_arr>\w+).*'"
            r" and '(?P<opt2>\w+)' operation .*"
            r" on local variable '(?P<ld_arr>\w+).*'\."
        )
        match = re.search(regexp, self.lines[curr])
        if match:
            (opt1, var, st_arr,
             opt2, ld_arr) = match.groups()

            result[self.settings.attr_ii_violation][opt1] = {
                var: "",
                st_arr.split("[", 1)[0]: "%s:%s" % (
                    "", ""
                )
            }

            result[self.settings.attr_ii_violation][opt2] = {
                ld_arr.split("[", 1)[0]: "%s:%s" % (
                    "", ""
                )
            }
            return True
        else:
            return False

    def try_to_match_12_1_3(self, curr, result):
        regexp = (
            r"between '(?P<opt1>\w+)' operation [(]'\w+', .+?:\d+(?:->.+?:\d+)*[)] of "
            r"variable '(?P<var>\w+)', .+?:\d+(?:->.+?:\d+)* on array "
            r"'(?P<st_arr>\S+)', (?P<st_arr_path>.+?):(?P<st_arr_line>\d+) "
            r"and '(?P<opt2>\w+)' operation [(]'\w+', .+?:\d+(?:->.+?:\d+)*[)] "
            r"on array '(?P<ld_arr>\S+)', "
            r"(?P<ld_arr_path>.+?):(?P<ld_arr_line>\d+)\."
        )
        match = re.search(regexp, self.lines[curr])
        if match:
            (opt1, var, st_arr, st_arr_path, st_arr_line,
             opt2, ld_arr, ld_arr_path, ld_arr_line) = match.groups()

            result[self.settings.attr_ii_violation][opt1] = {
                var: "",
                st_arr.split("[", 1)[0]: "%s:%s" % (
                    os.path.basename(st_arr_path), st_arr_line
                )
            }

            result[self.settings.attr_ii_violation][opt2] = {
                ld_arr.split("[", 1)[0]: "%s:%s" % (
                    os.path.basename(ld_arr_path), ld_arr_line
                )
            }
            return True
        else:
            return False

    def try_to_match_12_1_2(self, curr, result):
        regexp = (
            r"between bus access on port '(?P<a_port>\w+)' [(]"
            r"(?:.+?:\d+->)*"
            r"(?P<a_path>.+?):(?P<a_line>\d+)"
            r"[)] and bus request on port '(?P<r_port>\w+)' [(]"
            r"(?:.+?:\d+->)*"
            r"(?P<r_path>.+?):(?P<r_line>\d+)"
        )
        match = re.search(regexp, self.lines[curr])
        if match:
            (a_port, a_path, a_line,
             r_port, r_path, r_line) = match.groups()

            result[self.settings.attr_ii_violation]['bus_access'] = {
                a_port: "%s:%s" % (
                    os.path.basename(a_path),
                    a_line
                )
            }

            result[self.settings.attr_ii_violation]['bus_request'] = {
                r_port: "%s:%s" % (
                    os.path.basename(r_path),
                    r_line
                )
            }
            return True
        else:
            return False

    def try_to_match_12_1(self, curr, result):
        regexp = (
            r"The II Violation in module '\S+?' [(]Loop: "
            r"(?P<name>.+?)[)]: (?P<reason>[^()]+)"
            r" [(]"
        )
        match = re.search(regexp, self.lines[curr])
        if match:
            name, reason = match.groups()
            result['name'] = name
            result[self.settings.attr_ii_violation] = \
                    {self.settings.attr_ii_reason: reason}
            if self.try_to_match_12_1_1(curr+1, result):
                return True
            elif self.try_to_match_12_1_4(curr+1, result):
                return True
            elif self.try_to_match_12_1_3(curr+1, result):
                return True
            elif self.try_to_match_12_1_2(curr+1, result):
                return True
            else:
                return False
        else:
            return False

    def try_to_match_12_2(self, curr, result):
        regexp = (
            r"Unable to schedule '(?P<opt1>\w+)' operation [(]'\w+', "
            r".+?:\d+(?:->.+?:\d+)*[)] on array "
            r"'(?P<arr>\S+)', (?P<arr_path>.+?):(?P<arr_line>\d+) "
            r"due to (?P<reason>.+?)\. (?P<solution>.+?) \'.*\'\."
        )
        match = re.search(regexp, self.lines[curr])
        if match:
            opt1, arr, arr_path, arr_line, reason, solution = match.groups()
            result[self.settings.attr_ii_violation] = {
                self.settings.attr_ii_reason: reason,
                self.settings.attr_ii_solution: solution,
                opt1: {
                    arr.split("[", 1)[0]: "%s:%s" % (
                        os.path.basename(arr_path), arr_line
                    )
                }
            }
            return True
        else:
            return False

    def try_to_match_12_4(self, curr, result):
        regexp = (
            r"Unable to schedule bus (?P<opt1>\w+) on port '(?P<var1>\w+)' [(]"
            r"(?P<arr_path>.+?):(?P<arr_line>\d+)[)]"
            r" due to (?P<reason>.+?)\. (?P<solution>.+?)\."
        )
        match = re.search(regexp, self.lines[curr])
        if match:
            opt1, var1, arr_path, arr_line, reason, solution = match.groups()
            result[self.settings.attr_ii_violation] = {
                self.settings.attr_ii_reason: reason,
                self.settings.attr_ii_solution: solution,
                opt1: {
                    var1.split("[", 1)[0]: "%s:%s" % (
                        os.path.basename(arr_path), arr_line
                    )
                }
            }
            return True
        else:
            return False

    def try_to_match_13_2(self, curr, result):
        regexp = (
            r"Unable to schedule '(?P<opt1>\w+)' operation [(]'\w+', "
            r"(?P<arr_path>.+?):(?P<arr_line>\d+)[)] on array "
            r"'(?P<arr>\S+)' "
            r"due to (?P<reason>.+?)\. (?P<solution>.+?) \'.*\'\."
        )
        match = re.search(regexp, self.lines[curr])
        if match:
            opt1, arr, arr_path, arr_line, reason, solution = match.groups()
            result[self.settings.attr_ii_violation] = {
                self.settings.attr_ii_reason: reason,
                self.settings.attr_ii_solution: solution,
                opt1: {
                    arr.split("[", 1)[0]: "%s:%s" % (
                        os.path.basename(arr_path), arr_line
                    )
                }
            }
            return True
        else:
            return False

    def try_to_match_12_2_1(self, curr, result):
        regexp = (
            r"Unable to schedule '(?P<op>\w+)' operation [(]'\w+', "
            r".+?:\d+(?:->.+?:\d+)*[)] on array "
            r"'(?P<arr>\S+)' "
            r"due to (?P<reason>.+?)\. (?P<solution>.+?) \'.*\'\."
        )
        match = re.search(regexp, self.lines[curr])
        if match:
            op, arr, reason, solution = match.groups()
            arr_path = ""
            arr_line = ""
            result[self.settings.attr_ii_violation] = {
                self.settings.attr_ii_reason: reason,
                self.settings.attr_ii_solution: solution,
                op: {
                    arr.split("[", 1)[0]: "%s:%s" % (
                        os.path.basename(arr_path), arr_line
                    )
                }
            }
            return True
        else:
            return False

    def try_to_match_12_3_1(self, curr, result):
        regexp = (
            r"between '(?P<op1>\w+)' operation .*[(]"
            r"'(?P<var1>\S+)', (?P<path1>.+?):(?P<line1>\d+)"
            r"[)] and '(?P<op2>\w+)' operation .*[(]"
            r"'(?P<var2>\S+)', (?P<path2>.+?):(?P<line2>\d+)"
            r"[)].*\."
        )
        match = re.search(regexp, self.lines[curr])
        if match:
            (op1, var1, path1, line1,
             op2, var2, path2, line2) = match.groups()

            result[self.settings.attr_ii_violation][op1] = {
                var1.split("[", 1)[0]: "%s:%s" % (
                    os.path.basename(path1),
                    line1
                )
            }
            result[self.settings.attr_ii_violation][op2] = {
                var2.split("[", 1)[0]: "%s:%s" % (
                    os.path.basename(path2),
                    line2
                )
            }
            return True
        else:
            return False

    def try_to_match_12_3_2(self, curr, result):
        regexp = (
            r"between '(?P<op1>\w+)' operation .* on array '(?P<var1>\S+)'.*"
            r"and '(?P<op2>\w+)' operation .* on array '(?P<var2>\S+)'"
        )
        match = re.search(regexp, self.lines[curr])
        if match:
            (op1, var1,
             op2, var2,) = match.groups()
            path1 = ""
            line1 = ""
            path2 = ""
            line2 = ""

            result[self.settings.attr_ii_violation][op1] = {
                var1.split("[", 1)[0]: "%s:%s" % (
                    os.path.basename(path1),
                    line1
                )
            }
            result[self.settings.attr_ii_violation][op2] = {
                var2.split("[", 1)[0]: "%s:%s" % (
                    os.path.basename(path2),
                    line2
                )
            }
            return True
        else:
            return False

#    def try_to_match_12_3_3(self, curr, result):
#        regexp = (
#            r"between '(?P<op1>\w+)' operation .*[(]"
#            r"'(?P<var1>\S+)', (?P<path1>.+?):(?P<line1>\d+)"
#            r"[)] of variable '(?P<var2>\S+)', "
#            r"(?P<path2>.+?):(?P<line2>\d+)"
#            r" on local variable .*"
#            r"and '(?P<op2>\w+)' operation .*[(]"
#            r"'(?P<var3>\S+)'"
#            r"[)].*\."
#        )
#        match = re.search(regexp, self.lines[curr])
#        if match:
#            (op1, var1, path1, line1,
#             var2, path2, line2, op2, var3) = match.groups()
#
#            result[self.settings.attr_ii_violation][op1] = {
#                var1.split("[", 1)[0]: "%s:%s" % (
#                    os.path.basename(path1),
#                    line1
#                )
#            }
#            result[self.settings.attr_ii_violation][op2] = {
#                var2.split("[", 1)[0]: "%s:%s" % (
#                    os.path.basename(path2),
#                    line2
#                )
#            }
#            return True
#        else:
#            return False

    # example 1
    # The II Violation in module 'xxx': Unable to enforce a carried dependence constraint
    # between 'xxx' operation ('xxx', xxx) and 'xxx' operation ('xxx', xxx)
    # example 2
    # Unable to schedule 'xxx' operation on array 'xxx' due to limited memory ports.
    # Please consider using a memory core with more ports or partitioning the array 'xxx'.
    # example 3
    # The II Violation in module 'xxx': Unable to enforce a carried dependence constraint
    # between 'xxx' operation ('xxx') with incoming values : ('xxx', xxx:xxx) and
    # 'xxx' operation ('xxx', xxx:xxx).
    # example 3
    # The II Violation in module 'xxx': Unable to enforce a carried dependence constraint
    # between 'xxx' operation .* on array 'xxx' and 'xxx' operation .* on array 'xxx'.
    #pylint: disable=simplifiable-if-statement
    def try_to_match_12_3(self, curr, result):
        regexp = (
            r"(?P<reason>Unable to enforce a carried .*constraint)"
        )
        match = re.search(regexp, self.lines[curr])
        if match:
            (reason, ) = match.groups()
            result[self.settings.attr_ii_violation] = \
                    {self.settings.attr_ii_reason: reason}
            if self.try_to_match_12_3_1(curr+1, result):
                return True
            elif self.try_to_match_12_3_2(curr+1, result):
                return True
#            elif self.try_to_match_12_3_3(curr+1, result):
#                return True
            else:
                return False
        else:
            return False

    #pylint: disable=missing-docstring, no-self-use, too-many-return-statements
    def parse_12(self, curr):
        d = {}
        regexp = r"Pipelining loop '(?P<name>.+?)'\."
        match = re.search(regexp, self.lines[curr])
        if match:
            (name, ) = match.groups()
            # if flatten, mark to the outermost flatten loop
            first_name = name.rpartition("_")[0]
            if first_name in self.flatten_loops:
                d['name'] = first_name
            else:
                d['name'] = name
        else:
            return {}

        if self.try_to_match_12_1(curr+1, d):
            return {"L,%d" % self.id(): d}
        elif self.try_to_match_12_2(curr+1, d):
            return {"L,%d" % self.id(): d}
        elif self.try_to_match_12_2_1(curr+1, d):
            return {"L,%d" % self.id(): d}
        elif self.try_to_match_12_3(curr+1, d):
            return {"L,%d" % self.id(): d}
        elif self.try_to_match_12_4(curr+1, d):
            return {"L,%d" % self.id(): d}
        else:
            return {}

    #pylint: disable=missing-docstring, no-self-use
    def parse_13(self, curr):
        d = {}
        regexp = r"Pipelining function '(?P<name>.+?)'\."
        match = re.search(regexp, self.lines[curr])
        if match:
            (name, ) = match.groups()
            d['name'] = name
        else:
            return {}
        if self.try_to_match_13_2(curr+1, d):
            regexp = r"Pipelining result : \w, "
            regexp = r"Final II = (?P<II_value>\d+), .*\."
            match = re.search(regexp, self.lines[curr+2])
            if match:
                (II_value,) = match.groups()
            else:
                match = re.search(regexp, self.lines[curr+3])
                if match:
                    (II_value,) = match.groups()
                    II_info = {"II":II_value}
                    d.update(II_info)
            return {"F,%d" % self.id(): d}
        else:
            return {}

    #pylint: disable=missing-docstring, no-self-use
    def parse_14(self, curr):
        return {}

    #pylint: disable=missing-docstring, no-self-use
    def parse_15(self, curr):
        return {}

    #pylint: disable=missing-docstring, no-self-use
    def parse_16(self, curr):
        return {}

    #pylint: disable=missing-docstring, no-self-use
    def parse_17(self, curr):
        return {}

    #pylint: disable=missing-docstring, no-self-use
    def parse_18(self, curr):
        return {}

    @staticmethod
    def or_parsers(*parsers):
        def func(self, curr):
            for pa in parsers:
                d = pa(curr)
                if d:
                    return d
            return {}
        return func

    def parse(self):
        func = LogParser.or_parsers(
            self.parse_1, self.parse_2, self.parse_4,
            self.parse_5, self.parse_6, self.parse_7, self.parse_8,
            self.parse_9, self.parse_10, self.parse_11, self.parse_12,
            self.parse_13, self.parse_14, self.parse_15, self.parse_16,
            self.parse_17, self.parse_18
        )

        res = {}
        for i in range(len(self.lines)):
            res.update(func(self, i))
        utility.dump_to_json(self.dumpfile, res)
