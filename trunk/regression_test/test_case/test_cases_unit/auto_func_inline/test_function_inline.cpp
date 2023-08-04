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


//#define FOCUS 0
#if !FOCUS

class c6_5
{
public: 
void sub6_5(int * sub6_5_a) {}
static void sub6_5s(int * sub6_5_a) {}
void sub6_5t() {
#pragma HLS array_partition variable=c6_5_a dim=1
}
protected:
int c6_5_a[16];
};

#pragma ACCEL kernel
void test6_5()
{
  c6_5 test6_5_a[16];
#pragma HLS array_partition variable=test6_5_a dim=1
  test6_5_a[0].sub6_5(0); 

  int test6_5_b[16];
#pragma HLS array_partition variable=test6_5_b dim=1
  test6_5_a[0].sub6_5(test6_5_b); 
  test6_5_a[0].sub6_5s(test6_5_b); 
  test6_5_a[0].sub6_5t(); 
}

#else

//void sub1_1(int * sub1_1_a) {}
//
//#pragma ACCEL kernel
//void test1_1()
//{
//  int test1_1_a[16];
//#pragma HLS array_partition variable=test1_1_a dim=1
//  sub1_1(test1_1_a); 
//}

#endif
