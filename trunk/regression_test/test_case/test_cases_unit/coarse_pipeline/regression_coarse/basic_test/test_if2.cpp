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
void func_top_3_0(int a[100][100],int b[100][100],int c[50],int c1,int c2)
{
  int i;
  int j;
  int a_buf[100][100];
  int c_buf[50];
  for (j = 0; j < 100; j++) 
{
#pragma ACCEL pipeline
      for (i = 0; i < 100; i++) {
        a_buf[i][j] = b[i][j];
        c_buf[i / 2] = b[i][j];
      }
    if (c2) {
      for (i = 0; i < 100; i++) {
        a[i][j] = a_buf[i][j];
      }
    }
    for (i = 0; i < 50; i++) {
      c[i] = c_buf[i];
    }
  }
}


//Loop structure 
#pragma ACCEL kernel
void func_top_3(int a[100][100], int b[100][100], 
        int c[50], int c1, int c2)
{

    int i, j;
    int a_buf[100][100];
    int c_buf[50];
    for (j = 0; j < 100; j++) {
#pragma ACCEL pipeline
        if (c1) 
            for (i = 0; i < 100; i++) {
                a_buf[i][j] = b[i][j];
                c_buf[i/ 2] = b[i][j];
            }
        else 
            a_buf[i][50] = b[i][50];
        if (c2) {
            for (i = 0; i < 100; i++)
                a[i][j] = a_buf[i][j];
        }
        for (i = 0; i < 50; i++) {
            c[i] = c_buf[i];
        }
    }
}

int main() {
  int aa[100][100];
  int bb[100][100];
  int cc[100][100];
  int *a, *b, *c;
  int a_s, b_s;
	int c_s=1;
	int d_s=1;
  float aa_f[100][100];
  float bb_f[100][100];
  float cc_f[100][100];
  float *a_f, *b_f, *c_f;
  float a_s_f, b_s_f, c_s_f, d_s_f;
  func_top_3_0(aa, bb, c, c_s, d_s);
  func_top_3(aa, bb, c, c_s, d_s);

}

