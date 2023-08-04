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

struct t1 {float a[4];} gvar0;

#pragma ACCEL kernel
void f0(struct t1 *gvar0)
{
#pragma ACCEL interface variable=gvar0 depth=1
   *gvar0 = *gvar0;
}

void __merlinwrapper_f0()
{
  f0(&gvar0);
}


int gvar1;
#pragma ACCEL kernel 
void f1(int *gvar1)
{
#pragma ACCEL interface variable=gvar1 depth=1
   *gvar1 = *gvar1;
}
void __merlinwrapper_f1()
{
  f1(&gvar1);
}



#pragma ACCEL kernel
void f2(struct t1 *gvar0)
{
#pragma ACCEL interface variable=gvar0 depth=1
   *gvar0 = *gvar0;
}

void __merlinwrapper_f2(struct t1 gvar0)
{
  f2(&gvar0);
}

