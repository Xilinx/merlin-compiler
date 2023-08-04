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
#pragma ACCEL kernel
void func_top_6(int a[100][100], int b[100][100])
{

    int i, j;
    int a_buf[100][100];
    for (i = 0; i < 100; i++) {
#pragma ACCEL pipeline
      j = 10;
      do {
          a_buf[i][j] = b[i][j];
          ++j;
      } while (j <= 100);

      j = 100;
      while (j >= 10) {
          a[i][j] = a_buf[i][j];
          --j;
      }
    }
}

#pragma ACCEL kernel
void func_top_6_1(int a[100][100], int b[100][100])
{

    int i, j;
    int a_buf[100][100];
    for (i = 0; i < 100; i++) {
#pragma ACCEL pipeline
      j = 10;
      do {
          a_buf[i][0] = b[i][0];
          ++j;
      } while (j <= 100);

      j = 100;
      while (j >= 10) {
          a[i][0] = a_buf[i][0];
          --j;
      }
    }
}

#pragma ACCEL kernel
void func_top_5(int a[100],int b[100], int c[100])
{

    int i=0, j=0;

    int a_buf[100];
    int c_buf[100];
    while (i < 100)
    {
#pragma ACCEL pipeline
        for (j = 0; j < 100; j++)
        {
            a_buf[j] = b[j];
            c_buf[j] = b[j];
        }
        for (j = 0; j < 100; j++)
        {
            a[j] = a_buf[j];
        }
        for (j = 0; j < 100; j++)
        {
            c[j] = c_buf[j+1] + a_buf[j];
        }
        ++i;
    }
    i = 0;
    do {
#pragma ACCEL pipeline
        for (j = 0; j < 100; j++)
        {
            a_buf[j] = b[j+1];
        }

        for (j = 0; j < 100; j++)
        {
            a[j] = a_buf[j+1];
        }
        ++i;
    } while (i < 100);

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

  func_top_6(aa, bb);
  func_top_6_1(aa, bb);
  func_top_5(a, b, c);
}
