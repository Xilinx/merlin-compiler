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
#pragma ACCEL kernel
static int foo(unsigned int i)
{
  void *addrs[] = {(&&L1), (&&L2), (&&L3), (&&L4), (&&L5)};
  int res = 1;
  goto *addrs[i];
  L5:
  res *= 11;
  L4:
  res *= 7;
  L3:
  res *= 5;
  L2:
  res *= 3;
  L1:
  res *= 2;
  return res;
}
