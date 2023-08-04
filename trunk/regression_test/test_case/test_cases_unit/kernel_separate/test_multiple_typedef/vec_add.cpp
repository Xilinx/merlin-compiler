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
typedef enum {ONE} my_em;
typedef my_em my_em_1;
typedef enum {TWO} other_em;
typedef other_em other_em_1;
typedef struct {int a;} my_st;
typedef my_st my_st_1;
typedef class {int a;} my_cl;
typedef my_cl my_cl_1;
typedef union {int a; float b;} my_un;
typedef my_un my_un_1;
#pragma ACCEL kernel
void top(my_em_1 *a, my_st_1 *b, my_cl_1 *c, my_un_1 *d) {
  b->a = TWO;
}
