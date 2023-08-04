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
typedef my_em_1 my_em_2;
typedef my_em_2 my_em_3;
typedef my_em_3 my_em_4;
typedef enum {TWO} other_em;
typedef other_em other_em_1;
typedef other_em_1 other_em_2;
typedef other_em_2 other_em_3;
typedef other_em_3 other_em_4;
#pragma ACCEL kernel
void top(my_em_2 *a, other_em_4 * b) {
}
