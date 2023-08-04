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
#include<string.h>
//1d dimension
#pragma ACCEL kernel
void top(int *a, char *b, unsigned int *c, float *d, int n) {

   int a_buf[1000];
   memcpy(a_buf, a, sizeof(int) * 1000);

   for (int i = 0; i < 64; ++i) {
     char b_buf[100][64];
     memcpy(b_buf[0], b, sizeof(char) * 100 * i);
   }

   unsigned int c_buf[2][20][4];
   memcpy(c_buf[0][0], c, sizeof(unsigned int) * 2 * 20 * 4);

   float d_buf[100][4][2][2];
   memcpy(d_buf[0][0][0], d, sizeof(float) * 100 * 4 * 2 * 2);

   char b_buf_1[9][64];
   memcpy(b_buf_1[0], b, sizeof(char) * n);


   float d_buf_1[32][2][2][2];
   for (int i = 0; i < 32; ++i) {
     memcpy(d_buf_1[i][0][0], d + i * 8 , sizeof(float) * i * 8);
   }
}

int main() {
  int *a;
  char *b;
  unsigned int *c;
  float *d;
  int n;
  top(a, b, c, d, n);


}
