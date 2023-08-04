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
import sys
import json
from test_util import eprint

def check_dict(ref_data, res_data, ignore_values, is_ignored=False):
    if not isinstance(res_data, dict):
        eprint('Data type mismatch. Expect dict but {0}: {1}'
               .format(type(res_data), res_data))
        return False

    if is_ignored: # Only check keys if the dict is ignored
        for key in ref_data:
            if key not in res_data:
                eprint('Key missing. Expect {0}'.format(key))
                return False
        return True

    for key in ref_data:
        ignore = key in ignore_values
        if key not in res_data: # Check key
            eprint('Key missing. Expect {0}'.format(key))
            return False
        elif isinstance(ref_data[key], list):
            if not check_list(ref_data[key], res_data[key], ignore_values, ignore):
                return False
        elif isinstance(ref_data[key], dict):
            if not check_dict(ref_data[key], res_data[key], ignore_values, ignore):
                return False
        elif ref_data[key] != res_data[key]:
            if ignore and res_data[key] != 'ignore':
                print('Ignore data mismatch. Expect {0} but {1} for key {2} in {3}'
                      .format(ref_data[key], res_data[key], key, res_data))
            else:
                eprint('Data mismatch. Expect {0} but {1} for key {2} in {3}'
                       .format(ref_data[key], res_data[key], key, res_data))
            return False

    return True

def check_list(ref_data, res_data, ignore_values, is_ignored=False):
    if not isinstance(res_data, list):
        eprint('Data type mismatch. Expect list but {0}: {1}'
               .format(type(res_data), res_data))
        return False

    if is_ignored:
        return True

    for (ref, res) in zip(ref_data, res_data):
        if isinstance(ref, list):
            if not check_list(ref, res, ignore_values):
                return False
        elif isinstance(ref, dict):
            if not check_dict(ref, res, ignore_values):
                return False
        elif ref != res: # Element
            eprint('Data mismatch. Expect {0} but {1} in {2}'
                   .format(ref, res, res_data))
            return False
            
    return True

def check_json(result_json, ref_json, ignore_values=[]):   
    filep = open(result_json, 'r')
    res_data = json.loads(filep.read())
    filep.close()

    filep = open(ref_json, 'r')
    ref_data = json.loads(filep.read())
    filep.close()

    if isinstance(ref_data, list):
        return check_list(ref_data, res_data, ignore_values)
    elif isinstance(ref_data, dict):
        return check_dict(ref_data, res_data, ignore_values)

    eprint('Unknown type in {0}: {1}'.format(ref_json, type(ref_data)))
    return False

if __name__ == "__main__":
    if not check_json(sys.argv[2], sys.argv[1]):
        eprint('JSON file {0} and {1} differ'.format(sys.argv[2], sys.argv[1]))
        sys.exit(1)
