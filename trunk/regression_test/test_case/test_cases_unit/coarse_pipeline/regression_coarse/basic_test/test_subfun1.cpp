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
void sub_top_0_0(int * a) {
    int i;
    for (i = 2; i < 100; i+=2) 
        a[i] += i;
}

#pragma ACCEL kernel
void func_top_0_3(int *a, int *b) {
  int i;
  int j;
  int a_buf[100];
  for (j = 0; j < 100; ++j) {
#pragma ACCEL pipeline
    if (j % 2 == 0)
      a_buf[0] = 1;
    for (i = 1; i < 100; i++) {
      a_buf[i] = b[i];
    }
    sub_top_0_0(a_buf);
    for (i = 100 - 1; i >= 0; --i)
      a[i] = a_buf[i];
  }
}
#pragma ACCEL kernel
void func_top_0(int * a, int * b)
{
    int i;
    int j;
    int a_buf[100];
    for (j = 0; j < 100; ++j) {
#pragma ACCEL pipeline
        for (i = 1; i < 100; i++)
        {
            a_buf[i] = b[i];
        }
        sub_top_0_0(a_buf);
        for (i = 100-1; i >= 0; --i)
            a[i] = a_buf[i];

    }
}

#pragma ACCEL kernel
void func_top_1(int * a, int * b)
{
    int i;
    int j;
    for (j = 0; j < 100; ++j) {
#pragma ACCEL pipeline
    		int a_buf[100];
        for (i = 1; i < 98; i++)
        {
            a_buf[i] = b[i];
        }
        sub_top_0_0(a_buf);
        for (i = 100-1; i >= 0; --i)
            a[i] = a_buf[i];

    }
}


void sub_top_0_0_1(int * a) {
    int i;
    for (i = 0; i < 100; i+=2) 
        a[i] += i;
}

#pragma ACCEL kernel
void func_top_0_1(int * a, int * b)
{
    int i;
    int j;
    int a_buf[100];
    for (j = 0; j < 100; ++j) {
#pragma ACCEL pipeline
        //FIXME:
        //int a_buf[100];
        for (i = 0; i < 100; i++)
        {
            a_buf[i] = b[i];
        }
#pragma HLS array_partition variable=a_buf complete
        sub_top_0_0_1(a_buf);
        for (i = 100-1; i >= 0; --i)
            a[i] = a_buf[i];

    }
}
void sub_top_0_1(int * a) {
    int i;
    for (i = 0; i < 100; i++) 
        a[i] += i;
}

#pragma ACCEL kernel
void func_top_0_0(int * a, int * b)
{
    int i;
    int j;
    int a_buf[100];
    for (j = 0; j < 100; ++j) {
#pragma ACCEL pipeline
        //FIXME:
        //int a_buf[100];
        for (i = 0; i < 100; i++)
				{   a_buf[i] = b[i]; }
        
				sub_top_0_1(a_buf);
        for (i = 100-1; i >= 0; --i)
            a[i] = a_buf[i];

    }
}

#pragma ACCEL kernel
void func_top_0_2(int * a, int * b)
{
    int i;
    int j;
    int a_buf[100];
    for (j = 100-1; j >= 0; --j) {
#pragma ACCEL pipeline
        for (i = 0; i < 100; i++)
				{   a_buf[i] = b[i]; }
        
				sub_top_0_1(a_buf);
        for (i = 100-1; i >= 0; --i)
            a[i] = a_buf[i];

    }
}

int main() {
  int aa[100][100];
  int bb[100][100];
  int cc[100][100];
  int *a, *b, *c;
  int a_s, b_s, c_s, d_s;
  float aa_f[100][100];
  float bb_f[100][100];
  float cc_f[100][100];
  float *a_f, *b_f, *c_f;
  float a_s_f, b_s_f, c_s_f, d_s_f;

  func_top_0(a, b);
  func_top_0_1(a, b);
  func_top_0_0(a, b);
  func_top_1(a, b);

}
