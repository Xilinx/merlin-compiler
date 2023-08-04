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
class float2{
public:
  float x;
  float y;
  float2() { x= 0, y = 0;}
  float2(float a){
    x = a;
    y = a;
  }
  float2& operator=(const float2& other){
    x = other.x;
    y = other.y;
    return *this;
  }
};

struct float2x8{
   float2 i0;
   float2 i1;
};

#pragma ACCEL kernel
void foo (/*float2x8 in*/) {
  float2 a, b;
  b = a;
  float2x8 data, data1;
  data1 = data;
}
