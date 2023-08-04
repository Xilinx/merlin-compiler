//(C) Copyright 2016-2021 Xilinx, Inc.
//All Rights Reserved.
//
//Licensed to the Apache Software Foundation (ASF) under one
//or more contributor license agreements.  See the NOTICE file
//distributed with this work for additional information
//regarding copyright ownership.  The ASF licenses this file
//to you under the Apache License, Version 2.0 (the
//"License"); you may not use this file except in compliance
//with the License.  You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
//Unless required by applicable law or agreed to in writing,
//software distributed under the License is distributed on an
//"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
//KIND, either express or implied.  See the License for the
//specific language governing permissions and limitations
//under the License. (edited)
void foo(unsigned __int128 merlin_return_value[1], signed __int128 a[10], __int128 b[10]);
unsigned __int128 __merlinwrapper_foo(signed __int128 a[10], __int128 b[10])
{
  unsigned __int128 merlin_return_value;
  
#pragma ACCEL kernel name="foo"
  foo(&merlin_return_value, a, b);
  unsigned __int128 __merlinwrapper_return = merlin_return_value;
  return __merlinwrapper_return;
}

static const unsigned __int128 m126 = 18446744073709551615ULL;
void foo(unsigned __int128 merlin_return_value[1], signed __int128 a[10], __int128 b[10])
{
  
#pragma ACCEL interface variable=merlin_return_value io=WO scope_type=return
  merlin_return_value[0] = m126 + a[2] + b[2];
  return ;
}
