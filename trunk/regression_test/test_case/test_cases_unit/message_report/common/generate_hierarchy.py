#(C) Copyright 2016-2021 Xilinx, Inc.
#All Rights Reserved.
#
#Licensed to the Apache Software Foundation (ASF) under one
#or more contributor license agreements.  See the NOTICE file
#distributed with this work for additional information
#regarding copyright ownership.  The ASF licenses this file
#to you under the Apache License, Version 2.0 (the
#"License"); you may not use this file except in compliance
#with the License.  You may obtain a copy of the License at
#
#  http://www.apache.org/licenses/LICENSE-2.0
#
#Unless required by applicable law or agreed to in writing,
#software distributed under the License is distributed on an
#"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
#KIND, either express or implied.  See the License for the
#specific language governing permissions and limitations
#under the License. (edited)
import os
import re
import json
import argparse
from copy import deepcopy

# the first several lines start with X_ is interface
# then followed with F_ which is function
def get_separate_lines(lines, type = "hierarchy"):
    # skip lines to get interface info
    lines_interface = []
    lines_hierarchy = []
    is_interface = True
    for one_line in lines:
        if is_interface:
            if one_line.startswith("X_"):
                lines_interface.append(one_line)
            elif one_line.startswith("F_"):
                is_interface = False
        if not is_interface:
            lines_hierarchy.append(one_line)
    if type == "interface":
        return lines_interface
    elif type == "hierarchy":
        return lines_hierarchy
    else:
        return lines_hierarchy

def get_hierarchy_lines(lines):
    for one_line in lines:
        print(one_line)
    return lines_hierarchy

list_tmp = locals()
# map config file to simple json
def json_gen_structure(lines):
    # when update dict, will update data in list, but not dict
    lines = get_separate_lines(lines)
    print("hierarchy lines")
    print(lines)
    for i in range(100):
        list_tmp['list%s'%i] = []

    first_line = True
    indent = 0
    pre_indent = 0
    res_list = []
    index = 0
    # create top list
    if len(list_tmp['list%s'%index]) == 0:
        list_tmp['list%s'%index] = []
    for line in lines:
        exp1 = (r"^(\s*)(\S+)(\s*)(.*)$")
        match = re.search(exp1, line)
        if match:
            pre, name, post, info= match.groups()
#            print("name = " + str(name) + ", indent = " + str(indent) + ", pre_indent = " + str(pre_indent))
            # get indent indenG
            indent = pre.count(" ")
            index = indent//4 + 1

            # create child list for current list and append to dict
            list_tmp['list%s'%index] = []
            new_dict = {name: list_tmp['list%s'%index]}
            if first_line:
                new_dict["type"] = "kernel"
            else:
                if name.startswith("L_"):
                    new_dict["type"] = "loop"
                elif name.startswith("F_"):
                    new_dict["type"] = "function"
                elif name.startswith("X_"):
                    new_dict["type"] = "callfunction"
            first_line = False
            #print(new_dict)

            # append dict to parent list
            list_tmp['list%s'%(index-1)].append(new_dict)
#            print(list_tmp['list%s'%(index-1)])
            pre_indent = indent
#    print(list_tmp['list%s'%(0)])
#    with open("test.json", "w") as fp:
#        json.dump(list_tmp['list%s'%(0)], fp, sort_keys=True, indent=4)
    return list_tmp['list%s'%(0)]

def get_interface_list(lines, id_name):
    interface_list = []
    for line in lines:
        exp1 = (r"^(\s*)(\S+)(\s*)\((.*)\)$")
        match = re.search(exp1, line)
        if match:
            pre, name, post, info= match.groups()
            # create child list for current list and append to dict
            output = {}
            output[id_name] = name
            output["type"] = "interface"

            #print("info = " + info)
            data_list = info.split(",")
            #print(data_list)
            node_output = {}
            for one_data in data_list:
                key_value = one_data.split(":")
                if len(key_value) != 2:
                    continue
                key = key_value[0].strip()
                value = key_value[1].strip()
                output[key] = value
            interface_list.append(output)
    print("interface list = ")
    print(interface_list)
    return interface_list
                    
def scan_node(data, lines, level, type):
    if type == "dst":
        id_name = "topo_id"
    elif type == "src":
        id_name = "src_topo_id"
    else:
        id_name = "topo_id"
    output_list = []
    output_dict = {}
    output_sub_functions = []
    for node in data:
#        print("node = ")
#        print(node)
        node_output = {}
        next_node = []
        id = ""
        node_output["childs"] = []
        node_type = ""
        for key in node:
            if key == "type":
                node_output["type"] = node[key]
            if key == "type" and node[key] == "kernel":
                node_type = "kernel"
        for key in node:
            if key == "type":
                continue
            id = key
            next_node = node[id]
            node_output[id_name] = id
            # create top list
            for line in lines:
                exp1 = (r"^(\s*)(\S+)(\s*)\((.*)\)$")
                match = re.search(exp1, line)
                if match:
                    pre, name, post, info= match.groups()
                    #print("name = " + name + ", id = " + id)
                    if name == id:
                        #print("info = " + info)
                        data_list = info.split(",")
                        #print(data_list)
                        for one_data in data_list:
                            key_value = one_data.split(":")
                            if len(key_value) != 2:
                                continue
                            key = key_value[0].strip()
                            value = key_value[1].strip()
                            node_output[key] = value
                            #print("key = " + key + ", value = " + value)
            next_output_list = scan_node(next_node, lines, level+1, type)
            for one_next in next_output_list:
                node_output["childs"].append(one_next)
        if level > 0:
            output_list.append(node_output)
        else:
            if node_type == "kernel":
                interface_list = get_interface_list(get_separate_lines(lines, "interface"), id_name)
                node_output["interfaces"] = interface_list
                output_list.append(node_output)
            else:
                output_sub_functions.append(node_output)
#        print("node_output = ")
#        print(node_output)
    if level == 0:
        output_list[0]["sub_functions"] = output_sub_functions
        
    return output_list

# generate real json file for testing
def json_gen_hierarchy(data, lines, type):
    output_list = []
    output_list = scan_node(data, lines, 0, type)
    return output_list

# generate flattened json files
def json_gen_flatten(lines):
    # when update dict, will update data in list, but not dict
    output = {}
    first_line = True
    for line in lines:
        exp1 = (r"^(\s*)(\S+)(\s*)\((.*)\)$")
        match = re.search(exp1, line)
        if match:
            pre, name, post, info= match.groups()
#            print("name = " + str(name) + ", indent = " + str(indent) + ", pre_indent = " + str(pre_indent))
            # create child list for current list and append to dict
            output[name] = {}
            if first_line:
                output[name]["type"] = "kernel"
            else:
                if name.startswith("L_"):
                    output[name]["type"] = "loop"
                elif name.startswith("F_"):
                    output[name]["type"] = "function"
                elif name.startswith("X_"):
                    output[name]["type"] = "callfunction"
            first_line = False

            #print("info = " + info)
            data_list = info.split(",")
            #print(data_list)
            node_output = {}
            for one_data in data_list:
                key_value = one_data.split(":")
                if len(key_value) != 2:
                    continue
                key = key_value[0].strip()
                value = key_value[1].strip()
                output[name][key] = value
                #print("key = " + key + ", value = " + value)
    return output

def parse_from_text(text, flatten=False, cpp_type="dst"):
    assert type(text) == str
    lines = list(filter(lambda x: not x.startswith("#"), text.splitlines()))
    if flatten:
        return json_gen_flatten(lines)
    else:
        return json_gen_hierarchy(json_gen_structure(lines), lines, cpp_type)

#FIXME do we need to add callfunctions
if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("--src", required=True, help="input configuration file")
    parser.add_argument("--dst", required=False, help="output configuration file")
    parser.add_argument("--type", required=False, help="generate dst node or src node")
    args = parser.parse_args()
    file_name = args.src
    if os.path.exists(file_name):
        lines = []
        with open(file_name, "r") as f:
            lines = f.readlines()
        remain_lines = []
        for line in lines:
            if line.startswith("#"):
                print("ignore line with comment " + line)
            else:
                remain_lines.append(line)
        data = json_gen_structure(remain_lines)
#        print("\n\n")
        output_list = json_gen_hierarchy(data, remain_lines, args.type)
#        print(output_list)
        if args.dst:
            file_name = args.dst + os.path.basename(file_name)
        with open(file_name + ".json", "w") as fp:
            json.dump(output_list, fp, sort_keys=True, indent=4)
        output_flatten = json_gen_flatten(remain_lines)
        with open(file_name + "_flatten.json", "w") as fp:
            json.dump(output_flatten, fp, sort_keys=True, indent=4)
    else:
        print(file_name + " not exist")
