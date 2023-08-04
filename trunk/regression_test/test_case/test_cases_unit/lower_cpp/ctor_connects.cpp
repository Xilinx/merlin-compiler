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
class cls1 {
public:
  float x;
  cls1(){ 
    x= -1;
  }
  cls1(const cls1 &foo) {
    this->x = foo.x;
  }
  //cls1& operator=(cls1& other) {
  //  this->x = other.x;
  //  return *this;
  //}
};

struct st1 { 
  cls1 m1;
  int y;
};

struct st2 {
  st1 m2;
  st2() {
    m2.m1.x = 2;
  }
};

struct st3 {
  st2 m3;
};

#pragma ACCEL kernel
void foo () {
  st3 m4;
  static st3 m5;
  m5 = m4;
  cls1 m6, m7;
  m6 = m7;
  st3 m8[20];
  static st3 m9[20];
  static st3 m10 = m4;
  st3 m11 = m4;
  const st3 m12 = m4;
}
