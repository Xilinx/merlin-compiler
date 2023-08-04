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

struct t1 {float a[4];} ;
struct t2 {float a; float b[4]; float c[8];} ;
struct t1 gvar0;
struct t1 gvar1[100];

double gvar2;
double gvar3[4];
struct t2 gvar4;
struct t2 gvar5[4];

#pragma ACCEL kernel
void f0(struct t1 *gvar0, struct t1 (* gvar1)[100], 
    double *gvar2, double (*gvar3)[4], struct t2 * gvar4, struct t2 (* gvar5)[4])
{
#pragma ACCEL interface variable=gvar0 has_addrop=1 scope_type=global depth=1
#pragma ACCEL interface variable=gvar1 scope_type=global depth=1,100
#pragma ACCEL interface variable=gvar2 has_addrop=1 scope_type=global depth=1
#pragma ACCEL interface variable=gvar3 scope_type=global depth=1,4
#pragma ACCEL interface variable=gvar4 has_addrop=1 scope_type=global depth=1
#pragma ACCEL interface variable=gvar5 scope_type=global depth=1,4 
  int i; 
  *gvar0 = *gvar0;
  for (i = 0; i < 100; ++i) {
    (*gvar1)[i] = (*gvar1)[i];
  }
  *gvar2 = *gvar2;
  for (i = 0; i < 4; ++i)
    (*gvar3)[i] = (*gvar3)[i];
  *gvar4 = *gvar4;
  for (i = 0; i < 4; ++i) {
    (*gvar5)[i] = (*gvar5)[i];
  }
}

void __merlinwrapper_f0()
{
  f0(&gvar0, &gvar1, &gvar2, &gvar3, &gvar4, &gvar5);
}


