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
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#pragma ACCEL kernel
void func_top(double a[100], double *val)
{
	int j, j_sub;
  double sum=*val;
	for (j = 0; j < 10; j++) 
	{
#pragma ACCEL pipeline
#pragma ACCEL REDUCTION VARIABLE=sum
		for (j_sub = 0; j_sub < 10; ++j_sub) 
		{
#pragma ACCEL PARALLEL
			sum *= a[j * 10 + j_sub]/5;
		}
	}
  *val = sum;
}

void func_top_sw(double a[100], double *val)
{
	int j, j_sub;
  double sum=*val;
	for (j = 0; j < 10; j++) 
	{
		for (j_sub = 0; j_sub < 10; ++j_sub) 
		{
			sum *= a[j * 10 + j_sub]/5;
		}
	}
  *val = sum;
}

int main() {

  double c[100];
  for (int i = 0; i < 100; i++)
  {
    c[i] = rand()%10+1;
  }
  double sum=1;
  double sum_sw=1;
  func_top(c, &sum);
  func_top_sw(c, &sum_sw);
  int err = 0;
  double larger = fmax(fabs(sum), fabs(sum_sw)); 
  if (larger > 0)
    if (fabs((sum-sum_sw)) / larger > 1e-5) 
    {   
      printf("[ERROR] out=%lf exp=%lf\n", sum, sum_sw);
      printf("Error.\n");
      return 1 ;
    }   
  printf("Success.\n");
  return 0;
}
