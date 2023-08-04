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
int func1(int a) {
  if (a < 0) return 0;
  if (a == 0) return 1;
  return a * func1(a - 1);
}
int func2_sub(int a) {
  return a * a;
}

int func2(int a) {
  int ret;
  ret = func2_sub(a) * func2_sub(a - 1);
  return ret;
}

int func3(int a);

int func3_sub(int a) {
  if (a % 2 )
    return 1;
  return func3(a / 2);
}

int func3(int a) {
  if (a < 0) 
    return func3_sub(-a);
  if (a == 0)
    return 1;
  return func3_sub(a - 1);
}

int func4_sub(int a) {
  return a;
}

int func4_1(int a) {
  return  func4_sub(a);
}

int func4_2(int a) {
  return func4_sub(a);
}

int func4(int a) {
  if (a > 0)
    return func4_1(a - 1);
  if (a < 0)
    return func4_2(-a);
  return 1;
}
