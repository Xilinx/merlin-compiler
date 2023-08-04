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
void sub_func2(char **p, int count)
{
    **p += count;
}

#pragma ACCEL kernel
void func2(char **b)
{
#pragma ACCEL interface variable=b depth=1,2
    sub_func2(b, 20);
}

void sub_sub_func5(char **b)
{
    (**b)--;
}

void sub_func5(char **b)
{
    sub_sub_func5(b);
}

#pragma ACCEL kernel
void func5(char **b)
{
#pragma ACCEL interface variable=b depth=1,2
    sub_func5(b);
}

#pragma ACCEL kernel
void func6(char *b)
{
#pragma ACCEL interface variable=b depth=1
    (*b) += 10;
}
char **g9 = (char **)0;

void sub_func9(char ***b)
{
    ***b = g9;
}

#pragma ACCEL kernel
void func9(char ***b)
{
#pragma ACCEL interface variable=b depth=1,2,3
    sub_func9(b);
}

