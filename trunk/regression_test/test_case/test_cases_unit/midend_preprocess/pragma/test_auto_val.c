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
#pragma accel kernel
void top(int *a) {
  for (int i = 0; i < 100; ++i) {
#pragma ACCEL parallel auto{options: R=["", "reduction=a"]; default:""} factor=auto{options: P1=range(1, 101); \
    default: 1}
#pragma ACCEL tile factor=10 parallel_factor=auto
#pragma ACCEL tile factor=auto{options: T1=[2**x for x in range(7) if x==0 or (2**x)*T2<100]; default: 1} \
                   parallel_factor=4
#pragma ACCEL tile factor=auto{options: T2=[2**x for x in range(7) if x==0 or (2**x)*T1 < 100]; default:1}
    *a += i;
  }
}

#pragma accel kernel
void top1(int *a) {
  for (int i = 0; i < 100; ++i) {
#pragma ACCEL parallel factor=1
    a[i] = 0;
  }
}
