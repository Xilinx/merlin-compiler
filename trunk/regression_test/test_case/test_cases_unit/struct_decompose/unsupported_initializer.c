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
// RUN: %clang_cc1 -emit-llvm %s  -o /dev/null
typedef struct {
int foo;}spinlock_t;
typedef struct wait_queue_head_t {
spinlock_t lock;}wait_queue_head_t;
struct wait_queue_head_t work2 = {.lock = (spinlock_t ){(0)}};
#pragma ACCEL kernel name="call_usermodehelper"

void call_usermodehelper()
{
  struct wait_queue_head_t work = {.lock = (spinlock_t ){(0)}};
  work2;
}
