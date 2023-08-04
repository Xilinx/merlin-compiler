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
#include<ap_int.h>
//1d dimension
#pragma ACCEL kernel
void top(ap_int<40> *a, ap_int<10> *b, ap_uint<4> *c, ap_int<63> *d, ap_int<140> *f, ap_int<20> *g, ap_uint<16> *h) {

   ap_int<40> a_buf[1000];
   memcpy(a_buf, a, sizeof(ap_int<40>) * 1000);

   ap_int<10> b_buf[100][64];
   memcpy(b_buf[0], b, sizeof(ap_int<10>) * 100 * 64);

   ap_uint<4> c_buf[100][200][16];
   memcpy(c_buf[0][0], c, sizeof(ap_uint<4>) * 100 * 200 * 16);

   ap_int<63> d_buf[100][200][300][16];
   memcpy(d_buf[0][0][0], d, sizeof(ap_int<63>) * 100 * 200 * 300 * 16);

   ap_int<140> f_buf[100][200];
   memcpy(f_buf[0], f, sizeof(ap_int<140>) * 100 * 200);

   ap_int<20> g_buf[2000];
   memcpy(g_buf, g, sizeof(ap_int<20>) * 2000);

   ap_uint<16>  h_buf[25][4][128][8][32];
   memcpy(h_buf, h, sizeof(ap_uint<16>) * 25 * 4 * 128 * 8 * 32);
}

int main() {
ap_int<40> *a; ap_int<10> *b; ap_uint<4> *c; ap_int<63> *d; ap_int<140> *f; ap_int<20> *g; ap_uint<16> *h;
  top(a, b, c, d, f, g, h);


}
