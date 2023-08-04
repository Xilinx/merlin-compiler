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
#include<stdlib.h>
#define N 100

union tag 
{
  int ival;
  float fval;
}
;
#pragma ACCEL kernel

void top(union tag *a)
{
  
#pragma ACCEL interface variable=a depth=100
  int i;
  union tag b;
  b . ival = a[0] . ival;
  for (i = 0; i < 100; ++i) {
    a[i] . ival = ((a[i] . ival + 10) + b . fval);
  }
  return ;
}

int main()
{
  
#pragma ACCEL wrapper VARIABLE=a port=a data_type="union tag"
  int i;
  union tag *a;
  a = ((union tag *)(malloc(sizeof(union tag ) * 100)));
  for (i = 0; i < 100; ++i) 
    a[i] . ival = i;
  top(a);
  return 0;
}
