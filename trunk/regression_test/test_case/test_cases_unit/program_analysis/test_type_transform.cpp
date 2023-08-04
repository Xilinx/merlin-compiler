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
void remove_modifier(const int a[10], const volatile double b[][10]) {
  const char *ptr;
  const int &ref = a[0];
  const int s = 10;
  volatile double a2[s];
  int s2;
  const float (*a3)[s2];
}
struct st {
  int a;
};

const int ss = 10;
void replace_vars_ref(const int a[ss], double b[][ss]) {
  volatile double a2[ss];
  const float (*a3)[ss];
}

