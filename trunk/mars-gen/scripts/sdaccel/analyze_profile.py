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

#!/usr/bin/python

import sys
import os

def formatOutput(number):
    #print number
    return str(format(round(number, 9), '.9f'))

def formatTime(times, shift):
    i = 0
    while i < len(times):
        #print float(times[0]) / 10**shift
        times.append(formatOutput(float(times[0]) / 10**shift))
        del times[0]
        i += 1

def formatTimeTuple(times, shift):
    i = 0
    while i < len(times):
        times.append((formatOutput(float(times[0][0]) / 10**shift), formatOutput(float(times[0][1]) / 10**shift)))
        del times[0]
        i += 1

def formatType(types):
    i = 0
    while i < len(types):
        temp = list(types[i])
        if temp[0] == '.':
            del temp[0]
        types[i] = "".join(temp)
        i += 1

## Returns index of the kernel if it exists, -1 otherwise.
def isNewKernel(name, kernels):
    i = 0
    while i < len(kernels):
        if (name == kernels[i]):
            return i
        i += 1
    return -1

## Returns length of overlap of intervals t1 and t2
def overlap(t1, t2):
    length = min(float(t1[1]), float(t2[1])) - max(float(t1[0]), float(t2[0]))
    if (length >= 0):
        return length
    else:
        return 0

def is_number(s):
    try:
        float(s)
        return True
    except ValueError:
        return False

def xilinx_main(inputfile):
    types = []
    execute_times = []
    with open(inputfile) as file:
        lineList = file.readlines()
        for line in lineList:
            data = line.rstrip('\n').split(",")
            if len(data) >= 3 :
                if typeindex == 1 :
                    if is_number(data[4]) or data[4]=="N/A" :
                        types.append(tmptype)
                        #execute_times.append(float(data[4]))
                        if data[4]=="N/A" :
                            print tmptype, ' '*(50-len(tmptype)), data[4]
                        else :
                            print tmptype, ' '*(50-len(tmptype)), formatOutput(float(data[4]) / 1000)
                elif typeindex == 2 :
                    if is_number(data[6]) :
                        types.append(data[1])
                        execute_times.append(float(data[6]))
                        print data[1], ' '*(50-len(data[1])), formatOutput(float(data[6]) / 1000)
                elif typeindex == 3 :
                    if is_number(data[7]) :
                        types.append(tmptype)
                        execute_times.append(float(data[7]))
                        print tmptype, ' '*(50-len(tmptype)), formatOutput(float(data[7]) / 1000)
                elif typeindex == 4 :
                    if is_number(data[6]) :
                        temptype = tmptype + ' (' + data[1] + ')'
                        types.append(temptype)
                        execute_times.append(float(data[6]))
                        print temptype, ' '*(50-len(temptype)), formatOutput(float(data[6]) / 1000)
                elif typeindex == 5 :
                    if is_number(data[6]) :
                        types.append(tmptype)
                        execute_times.append(float(data[6]))
                        print tmptype, ' '*(50-len(tmptype)), formatOutput(float(data[6]) / 1000)
                else :
                    if is_number(data[2]) :
                        types.append(data[0])
                        execute_times.append(float(data[2]))
                        print data[0], ' '*(50-len(data[0])), formatOutput(float(data[2]) / 1000)
            elif len(data) == 1 and data[0] != "" :
                tmptype = data[0]
                if data[0]=="Top Buffer Writes" or data[0]=="Top Buffer Reads" :
                    typeindex = 1
                elif data[0]=="Top Kernel Execution" :
                    typeindex = 2
                elif data[0]=="Top Data Transfer: Kernels and Global Memory" :
                    typeindex = 0
                elif data[0]=="Data Transfer: Kernels and Global Memory" :
                    typeindex = 3
                elif data[0]=="Data Transfer: Host and Global Memory" :
                    typeindex = 4
                elif data[0]=="Compute Unit Utilization" :
                    typeindex = 5
                else :
                    typeindex = 6
                #print data[0]


def main():
    if len(sys.argv) == 2 :
        filename, fileext = os.path.splitext(sys.argv[1])
        if fileext == '.csv' :
            xilinx_main(sys.argv[1])
        else :
            print "Analysis file extension: *.mon or *.csv"
    else :
        print "Usage: <app_name> *.mon|*.csv"

if __name__ == "__main__":
    main()
