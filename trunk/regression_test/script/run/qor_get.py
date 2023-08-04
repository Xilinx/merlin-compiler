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
#!/usr/bin/python

import sys
import os
import fnmatch
import re
import numbers
import time
from datetime import datetime, date

file_path = None
key_name = ""
number_of_lines = 0
which_field = 0
separator = " "

def parsing_options():
    global file_path, key_name, number_of_lines, which_field, separator

    for option in sys.argv :
        #print (option)
        words = option.split("=")
        if words[0] == "file" :
            file_path = words[1]
        elif words[0] == "key" :
            key_name = words[1]
        elif words[0] == "line" :
            number_of_lines  = int(words[1].strip())
        elif words[0] == "field" :
            which_field = int(words[1].strip())
        elif words[0] == "sep" :
            separator = words[1]
    #print ("file_path = " + file_path)
    #print ("key_name = " + key_name)
    #print ("number_of_lines = " + str(number_of_lines))
    #print ("which_field = " + str(which_field))
    #print ("separator = " + separator)

def parsing_key(file_name):
    value = []
    linenumber = 0
    file = open(file_name)
    lines = file.readlines()
    file.close
    file = open(file_name)
    for line in file:
        if re.search(key_name, line) != None :
            if len(lines) >= (linenumber+number_of_lines+1) :
                targetline = lines[linenumber+number_of_lines]
                #print ("targetline=" + targetline)
            else :
                break
            if separator == " " :
                words = targetline.strip().split()
            else :
                words = targetline.strip().split(separator)
            #print (words)
            if len(words) >= which_field :
                value.append(words[which_field-1].strip())
        linenumber += 1
    file.close()
    return (value)
            
def main():
    if len(sys.argv) >= 2 :
        parsing_options()
        if os.path.isfile(file_path) == True :
            value = parsing_key(file_path)
            if len(value) > 0 :
                for i in value :
                    print ("value = " + i)
            else :
                print ("value = None")
        else :
            print ("Error: file not existed")
    else :
        print ("Usage:")
        print (sys.argv[0] + " file=<file_path> key=\"<key_name>\" line=<number_of_lines> field=<which_field> sep=\"<separator>\"")
        print ("")
        print ("file_path : the path of the file which key and value is at")
        print ("key_name : name of the key")
        print ("number_of_lines : number of lines between key and value")
        print ("which_field : location of value that the fields are separated by separator")
        print ("separator : field separator, default is whitespace")

if __name__ == "__main__" :
    main()

