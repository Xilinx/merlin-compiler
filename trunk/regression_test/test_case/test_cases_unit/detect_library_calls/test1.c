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
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

extern void bar();

#pragma ACCEL kernel
double test1(double x,double xi,double tao)
{
  double ret = exp(((double )(- 1)) * (x - xi) * (x - xi) / (((double )2) * tao * tao));
  return ret;
}

#pragma ACCEL kernel
int test2(const char *x_dat,const char *y_dat,double (*theta)[2])
{
  double x[100][2];
{
    char *line = (char *)0L;
    long read;
    int i = 0;
    for (i = 0; i < 100; ) {
      x[i][0] = ((double )1);
      x[i++][1] = atof((const char *)line);
    }
    __builtin_strstr("abc","def");
  }
  printf("foo");
  getchar();
  return 1;
}

#pragma ACCEL kernel
void test3() {
  bar();
  malloc(16);
  __builtin_inff();
}
