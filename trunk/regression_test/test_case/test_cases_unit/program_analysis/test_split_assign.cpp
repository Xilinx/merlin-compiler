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
struct st {
  int a;
  int b;
};
void f(int a);
int main() {
  int a, b, c;
  struct st sa, sb, sc;
  a = b = c = 42;
  ((a > b) ? a : b) = b = c;
  if (a = b = c) {}
  while (a = b = c) {}
  f(a = b = c);
  sa = sb = sc;
  sa = sb = sc, a, b ,c;
  for (a = b = c; a = b = c; a = b = c) {}

  int a1 = b = c = 99;
  int d = 100;
  int e = 101;
  a = (b = c = d) = e = 102;
  int arr[3] = {0, 1, 2};
  a = arr[0] = arr[1] = 103;
  a = arr[d = e = 2] = 104;
  return 0;
}
