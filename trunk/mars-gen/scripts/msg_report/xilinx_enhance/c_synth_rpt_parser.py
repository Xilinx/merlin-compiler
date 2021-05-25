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
Module for data extraction from csynth.rpt.xml

"""

import os.path
import re
import xmlparser
import utility

#pylint: disable=invalid-name, redefined-outer-name, no-self-use

#pylint: disable=super-init-not-called
class ReportParseError(Exception):
    """ Parse Error for exception """
    def __init__(self, msg):
        self.msg = msg
    def __str__(self):
        return "[%s] %s" % (self.__class__.__name__, self.msg)

class ReportParser:
    """ Parser for csynth.rpt.xml """
    def __init__(self, logger, path):
        self.logger = logger
        try:
            self.root = xmlparser.parse_xml(path)
        except xmlparser.XMLParseError as e:
            raise ReportParseError(str(e))

    @staticmethod
    def rename_duplicate(keys):
        """ make each key name unique """
        if len(keys) > 1:
            i = 0
            while i < len(keys)-1:
                order = 1
                j = i + 1
                while j < len(keys):
                    if keys[j] == keys[i]:
                        keys[j] += str(order)
                        order += 1
                    j += 1
                if order > 1:
                    keys[i] += str(0)
                i += 1
        return keys

    def parse_item(self, el):
        """ parse <item> node """
        res = None
        if len(el) == 0:
            res = el.text()
        else:
            res = self.parse(el[0])
        name = el.attr("name").strip()
        if name == "":
            raise ReportParseError(
                "Found <item> node without 'name' attribute value!"
            )
        else:
            return (name, res)

    def parse_section(self, el):
        """ parse <section> node """
        # <section> cannot directly follow another <section>,
        # i.e. <section><section> ...
        res = [self.parse(x) for x in el]
        name = el.attr("name").strip()
        if name == "":
            return dict(res)
        else:
            return {name: dict(res)}

    def parse_row(self, el):
        """ parse one row from <table> node """
        #assert len(el) == 0
        return list(filter(lambda x: x != "", (x.strip() for x in el.text().split(','))))

    def parse_table(self, el):
        """ parse <table> node """
        if len(el) == 2:
            keys = ReportParser.rename_duplicate(self.parse_row(el[0]))
            vals = self.parse_row(el[1])
            name = el[1].attr("name").strip()
            if name == "":
                return dict(zip(keys, vals))
            else:
                keys.pop(0)
                return {name: dict(zip(keys, vals))}

        elif len(el) > 2:
            keys = ReportParser.rename_duplicate(self.parse_row(el[0]))
            keys.pop(0)
            i = 1
            res = {}
            while i < len(el):
                vals = self.parse_row(el[i])
                name = el[i].attr("name").strip()
                if name == "":
                    raise ReportParseError(
                        "Found one row in <table> node "
                        "without 'name' attribute value!"
                    )
                else:
                    res[name] = dict(zip(keys, vals))
                    i += 1
            return res
        else:
            return {}

    def parse(self, el=None):
        """ parse the given node """
        if el is None:
            el = self.root

        tag = el.tag().strip()
        if tag == "item":
            return self.parse_item(el)

        elif tag == "section":
            return self.parse_section(el)

        elif tag == "table":
            return self.parse_table(el)

        elif tag == "profile":
            res = {}
            for x in el:
                res.update(self.parse_section(x))
            return res

class ReportExtractError(Exception):
    """ Report Extraction for exception """
    def __init__(self, msg):
        self.msg = msg
    def __str__(self):
        return "[%s] %s" % (self.__class__.__name__, self.msg)

class ReportExtractor:
    """ Extract data from csynth.rpt.xml """
    PE = "Performance Estimates"
    CC = "Latency (clock cycles)"
    CC2 = "Latency"
    TIMING = "Timing (ns)"
    TIMING2 = "Timing"
    SUMM = "Summary"
    DTAL = "Detail"
    INST = "Instance"
    LOOP = "Loop"
    UE = "Utilization Estimates"
    UP = "Utilization (%)"

    def __init__(self, spec, logger, path):
        self.res = ReportParser(logger, path).parse()
        self.spec = spec
        self.logger = logger
        for k in self.res:
            match = re.search("Vivado HLS Report for '([^']+)'", k)
            if match:
                self.subject = match.group(1)
            else:
                self.subject = os.path.basename(path).split(".")[0]

    @staticmethod
    def strip_loop_name(name):
        """ Returns the real loop name """
        while re.search(r"^\s*[-+]\s*", name):
            name = re.sub(r"^\s*[-+]\s*", "", name)
        return name

    @staticmethod
    def process_tilde(s):
        """ split the string """
        return [x.strip() for x in s.split("~", 1)]

    def get_to_dictionary(self, prev_path, curr, *path):
        """ get a dict from a dict value """
        d = curr
        try:
            for x in path:
                if x not in d:
                    if x == ReportExtractor.CC:
                        x = ReportExtractor.CC2
                    if x == ReportExtractor.TIMING:
                        x = ReportExtractor.TIMING2
                d = d[x]
            assert isinstance(d, dict)
        except:
            raise ReportExtractError(
                ("Unable to get to dictionary "
                 "'%s' for its data") % "/".join(prev_path+(list(path)))
            )
        return d

    def _perf_freq(self, result):
        path = [ReportExtractor.PE, ReportExtractor.TIMING, ReportExtractor.SUMM]
        d = self.get_to_dictionary([], self.res, *path)
        if self.spec.attr_ap_clk in d:
            t = {}
            estimate_period = d[self.spec.attr_ap_clk]["Estimated"]
            target_period = d[self.spec.attr_ap_clk]["Target"]
            uncertainty_period = d[self.spec.attr_ap_clk]["Uncertainty"]
            if target_period.endswith(" ns"):
                target_period = target_period[:-3]
            if estimate_period.endswith(" ns"):
                estimate_period = estimate_period[:-3]
            if uncertainty_period.endswith(" ns"):
                uncertainty_period = uncertainty_period[:-3]
            t[self.spec.attr_perf_estimate] = estimate_period
            t[self.spec.attr_perf_target] = target_period
            t[self.spec.attr_perf_uncertain] = uncertainty_period
            result[self.subject] = t
        return result

    def _perf_summ(self, result):
        path = [ReportExtractor.PE, ReportExtractor.CC, ReportExtractor.SUMM]
        d = self.get_to_dictionary([], self.res, *path)
        try:
            t = {}
            # 1
            t[self.spec.attr_latency_min] = d["min0"]
            t[self.spec.attr_latency_max] = d["max0"]
            # 2
            t[self.spec.attr_interval_min] = d["min1"]
            t[self.spec.attr_interval_max] = d["max1"]
            #result[self.subject] = t
            result[self.subject].update(t)
        except KeyError as e:
            raise ReportExtractError("No %s in %s" % (e, "/".join(path)))

        return result

    def _perf_instance(self, result):
        path = [ReportExtractor.PE, ReportExtractor.CC, ReportExtractor.DTAL,
                ReportExtractor.INST]
        try:
            d = self.get_to_dictionary([], self.res, *path)
        except ReportExtractError as e:
            self.logger.merlin_warning(str(e))
            return result

        for ins in d:
            t = {}
            t[self.spec.attr_instance_name] = ins
            #try:
                # 3
            d2 = self.get_to_dictionary(path, d, ins)
            #except ReportExtractError as e:
            #    self.logger.merlin_warning(str(e))
            #else:
            try:
                # 4
                t[self.spec.attr_module_name] = d2["Module"]
                # 5
                t[self.spec.attr_latency_min] = d2["min0"]
                t[self.spec.attr_latency_max] = d2["max0"]
                # 6
                t[self.spec.attr_interval_min] = d2["min1"]
                t[self.spec.attr_interval_max] = d2["max1"]
                result[d2["Module"]] = t
            except KeyError as e:
                self.logger.merlin_warning(
                    "No %s in %s" % (
                        e, "/".join(path+[ins])
                    )
                )

        return result
    #pylint: disable= line-too-long
    def _perf_loop(self, result):
        path = [ReportExtractor.PE, ReportExtractor.CC, ReportExtractor.DTAL,
                ReportExtractor.LOOP]
        try:
            d = self.get_to_dictionary([], self.res, *path)
        except ReportExtractError as e:
            self.logger.merlin_warning(str(e))
            return result

        for lp in d:
            t = {}
            # 7
            t[self.spec.attr_name] = ReportExtractor.strip_loop_name(lp)
            d2 = self.get_to_dictionary(path, d, lp)
            try:
                # 8
                t[self.spec.attr_latency_min] = d2["min"]
                t[self.spec.attr_latency_max] = d2["max"]
                # 9
                if "~" in d2["Latency"]:
                    (
                        t[self.spec.attr_iteration_latency_min],
                        t[self.spec.attr_iteration_latency]
                    ) = ReportExtractor.process_tilde(d2["Latency"])
                else:
                    t[self.spec.attr_iteration_latency_min] = d2["Latency"]
                    t[self.spec.attr_iteration_latency] = d2["Latency"]
                # 10
                t[self.spec.attr_iteration_interval] = d2["achieved"]
                # 11
                t[self.spec.attr_trip_count] = d2["Count"]
                match = re.search(r"(\S+)\s*~\s*(\S+)", d2["Count"])
                if match:
                    t[self.spec.attr_trip_count] = match.group(2)
                    self.logger.merlin_info("get max trip count value: " + t[self.spec.attr_trip_count])
                # 12
                t[self.spec.attr_pipelined] = d2["Pipelined"]
                result["L,%s-%s" % (self.subject,
                                    ReportExtractor.strip_loop_name(lp))] = t
            except KeyError as e:
                self.logger.merlin_warning(
                    "No %s in %s" % (
                        e, "/".join(path+[lp])
                    )
                )
        return result

    def _util_summ(self, result):
        path = [ReportExtractor.UE, ReportExtractor.SUMM]
        d = self.get_to_dictionary([], self.res, *path)

        t = {}
        # 13
        tup = ("DSP", "Expression", "FIFO", "Instance", "Memory",
               "Multiplexer", "Register")
        def to_int(k2):
            def func(k1):
                d2 = self.get_to_dictionary(path, d, k1)
                key = k2
                if key == "DSP48E" and key not in d2:
                    key = "DSP"
                try:
                    return 0 if d2[key] == "-" else int(d2[key])
                except KeyError as e:
                    raise ReportExtractError(
                        "No %s in %s" % (
                            e, "/".join(path+[k1])
                        )
                    )
            return func
        t[self.spec.attr_total_lut] = sum(map(to_int("LUT"), tup))
        t[self.spec.attr_total_ff] = sum(map(to_int("FF"), tup))
        t[self.spec.attr_total_bram] = sum(map(to_int("BRAM_18K"), tup))
        t[self.spec.attr_total_dsp] = sum(map(to_int("DSP48E"), tup))
        t[self.spec.attr_total_uram] = sum(map(to_int("URAM"), tup))

        # 14
        path.append(ReportExtractor.UP)
        d = self.get_to_dictionary([], self.res, *path)
        try:
            t[self.spec.attr_util_lut] = d["LUT"]
            t[self.spec.attr_util_ff] = d["FF"]
            t[self.spec.attr_util_bram] = d["BRAM_18K"]
            if "DSP48E" in d:
                t[self.spec.attr_util_dsp] = d["DSP48E"]
                #dsp_flag = 1
            else:
                t[self.spec.attr_util_dsp] = d["DSP"]
                #dsp_flag = 0
            t[self.spec.attr_util_uram] = d["URAM"]
            result[self.subject].update(t)
        except KeyError as e:
            raise ReportExtractError(
                "No %s in %s" % (
                    e, "/".join(path)
                )
            )
        return result

    def dump(self):
        """ Dump the extracted data into a JSON file """
        res = {}
        res = self._perf_freq(res)
        res = self._perf_summ(res)
        res = self._perf_instance(res)
        res = self._perf_loop(res)
        res = self._util_summ(res)
        dumpfile = "%s_csynth.json" % self.subject

        #pylint: disable=unused-variable
        utility.dump_to_json(dumpfile, res)
