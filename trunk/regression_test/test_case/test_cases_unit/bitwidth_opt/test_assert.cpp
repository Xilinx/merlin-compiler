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
#include<assert.h>
//1d dimension
#pragma ACCEL kernel
void top(int *a, char *b, unsigned int *c, float *d, size_t n, size_t m, size_t l, size_t k, size_t g, size_t h) {

   int a_buf[1000];
   assert(n < 1000);
   memcpy(&a_buf[100], a, sizeof(int) * n);

   char b_buf[100][6400];
   assert(m < 6400);
   memcpy(&b_buf[0][64], b, sizeof(char) * m);

   unsigned int c_buf[2][20][40];
   assert(l < 800);
   memcpy(&c_buf[0][2][4], c, sizeof(unsigned int) * l);

   float d_buf[100][4][20][20];
   assert(k < 500);
   memcpy(&d_buf[0][2][10][10], d, sizeof(float) * k);

   char b_buf_1[9][640];
   assert(g < 640);
   memcpy(b, &b_buf_1[1][64], sizeof(char) * g);


   float d_buf_1[32][20][20][2];
   assert(h < 800);
   memcpy(d  + 8, &d_buf_1[0][2][2][0], sizeof(float) * h);
}

int main() {
  int *a;
  char *b;
  unsigned int *c;
  float *d;
  int n, m , l, k , g, h;
  top(a, b, c, d, n, m, l, k, g, h);
  return 0;

}
