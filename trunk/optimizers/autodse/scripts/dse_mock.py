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
Python Script in Merlin Compiler

Filename    : optimizers/autodse/scripts/dse_mock.py
Description : This is the mocked methods for the DSE testing.
Owner(s)    : Cody Hao Yu (cody@falcon-computing.com)
Version     : 02-21-2019 First commit (Cody Hao Yu)
"""
import fcntl
import errno
import os
import pickle
import time

from typing import Dict, Any, Optional, Tuple # pylint: disable=unused-import
from dse_type import CFGType # pylint: disable=unused-import
import merlin_logger

MLOGGER = merlin_logger.get_logger('DSE_Mock')

# Mocking mode in the unit test
DSE_MOCKING = (None if 'MERLIN_DSE_MOCK' not in os.environ
               else os.environ['MERLIN_DSE_MOCK'])

def gen_mock_key(func_name, key):
    # type: (str, str) -> str
    """
    Concate function name and the key to store in the mock DB
    """
    return '{0}-{1}'.format(func_name, key)

def mock_load(func_name, key='default'):
    # type: (str, str) -> Tuple[bool, Optional[Any]]
    """
    Load the mocked result for the function if available
    """
    if not DSE_MOCKING or not DSE_MOCKING.startswith('mock'):
        return (False, None)

    db_name = DSE_MOCKING[DSE_MOCKING.find('@') + 1:]
    with open(db_name, 'rb') as filep:
        mock_db = pickle.load(filep)

    db_key = gen_mock_key(func_name, key)
    if db_key in mock_db:
        MLOGGER.fcs_lv2('You are using mocking result for %s!', func_name)
        return (True, mock_db[db_key])
    else:
        MLOGGER.debug('Key %s not found in mock DB', db_key)
    return (False, None)

def mock_save(func_name, key='default', obj=None):
    # type: (str, str, Optional[Any]) -> None
    """
    Dump the object to the pickle file as the mocking reference
    """
    if not DSE_MOCKING or not DSE_MOCKING.startswith('save'):
        return

    db_path = DSE_MOCKING[DSE_MOCKING.find('@') + 1:]
    db_name = '{0}/mock_db.pkl'.format(db_path)
    db_key = gen_mock_key(func_name, key)
    MLOGGER.fcs_lv2('dump key %s to %s', db_key, db_name)
    locker = open('{0}/dump_for_mocking.lock'.format(db_path), 'w')
    while True: # Attempt to get the lock
        try:
            fcntl.flock(locker, fcntl.LOCK_EX | fcntl.LOCK_NB)
            break
        except IOError as err:
            if err.errno != errno.EAGAIN:
                MLOGGER.error('Error when trying to write mocking db')
                raise
            else:
                time.sleep(0.1)

    mock_db = {} # type: Dict[str, Any]
    if os.path.exists(db_name):
        filep = open(db_name, 'rb')
        mock_db = pickle.load(filep)
        filep.close()

    mock_db[db_key] = obj
    filep = open(db_name, 'wb')
    pickle.dump(mock_db, filep, pickle.HIGHEST_PROTOCOL)
    filep.close()
    fcntl.flock(locker, fcntl.LOCK_UN)
