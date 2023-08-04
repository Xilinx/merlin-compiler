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
#include<string.h>
#pragma ACCEL kernel
DATA_T vec_read_sum(DATA_T *input, DATA_T *output) {
  DATA_T input_buff[SIZE_1][SIZE_2];
  memcpy(input_buff, input, sizeof(DATA_T) *SIZE);
  DATA_T sum = 0;
  for (int i0 = 0; i0 < SIZE_1; ++i0)
  for (int i1 = 0; i1 < SIZE_2; ++i1)
	sum += input_buff[i0][i1];
  memcpy(output, input_buff, sizeof(DATA_T) * SIZE);
  return sum;
}

int main() {
  DATA_T input[SIZE];
  DATA_T output[SIZE];
  int i = 0;
  DATA_T golden_sum = 0;
  for (i = 0; i < SIZE; ++i) {
	input[i] = i;
    golden_sum += i;                    
  }
  DATA_T sum = vec_read_sum(input, output);
  DATA_T diff = golden_sum - sum;
  if (diff < 0)
	diff = 0 - diff;
  if (diff > 1) {
	printf("Mismatch\n");
	return 1;
  }
  DATA_T output_sum = 0;
  for (i = 0; i < SIZE; ++i) {
    output_sum += output[i];
  }
  diff = golden_sum - output_sum;
  if (diff < 0) 
    diff = 0 - diff;
  if (diff > 0) {
	printf("Mismatch\n");
	return 1;
  }
  printf("Pass\n");
  return 0;
}
  

