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
#include "cmost.h"

void top(int *a)
{
  __merlin_access_range(a,0,999999UL);
  
#pragma ACCEL interface variable=a stream_prefetch=on max_depth=1000000 depth=1000000
  int i;
  int j;
  for (i = 0; i < 10000 / 100; ++i) {
    for (j = 0; j < 100; ++j) 
// Original: #pragma ACCEL parallel
// Original: #pragma ACCEL PARALLEL COMPLETE
{
      
#pragma ACCEL PARALLEL COMPLETE
      int index = j;
	  a[index] = 0;
    }
  }
}
