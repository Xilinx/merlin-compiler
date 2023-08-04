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
//1d dimensio
#pragma ACCEL kernel
void top(int *a, char *b, unsigned int *c, float *d, double *f, signed short *g, signed long *h, signed long long *k) {

   int a_buf[1000];
   memcpy(a_buf, a, sizeof(int) * 1000);

   char b_buf[100][64];
   memcpy(b_buf[0], b, sizeof(char) * 100 * 64);

   unsigned int c_buf[100][200][16];
   memcpy(c_buf[0][0], c, sizeof(unsigned int) * 100 * 200 * 16);

   float d_buf[100][200][300][16];
   memcpy(d_buf[0][0][0], d, sizeof(float) * 100 * 200 * 300 * 16);

   signed long h_buf[100][200][300][16];
   memcpy(h_buf[0][0][0], d, sizeof(signed long) * 100 * 200 * 300 * 16);

   signed long long k_buf[100][200][300][16];
   memcpy(k_buf[0][0][0], d, sizeof(signed long long) * 100 * 200 * 300 * 16);

   double f_buf[100][200];
   memcpy(f_buf[0], f, sizeof(double) * 100 * 200);

   signed short g_buf[2000];
   memcpy(g_buf, g, sizeof(signed short) * 2000);
}

int main() {
  int *a;
  char *b;
  unsigned int *c;
  float *d;
  double *f;
  signed short *g;
  signed long *h;
  signed long long *k;
  top(a, b, c, d, f, g, h, k);


}
