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
namespace st {
  enum my_em {ONE};
  struct A {};
  typedef int integer;
};

#pragma ACCEL kernel
void k()
{
  int f = 1;
  enum st::my_em a = (enum st::my_em)f;
  int s = sizeof(struct st::A);
  int s1 = sizeof(struct st::A **);
  int s2 = sizeof(struct st::A *);
  int s3 = sizeof(struct st::A [2]);
  int s4 = sizeof(const struct st::A [2][3]);
  int t = sizeof(st::integer);
  int t2 = sizeof(st::integer [2]);
  int t3 = sizeof(st::integer *);
  const struct st::A arr[2];
  struct st::A *p = (struct st::A *)arr;
}
