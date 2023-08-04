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

//Loop structure 
#pragma ACCEL kernel
void func_top_3(int a[100][100], int b[100][100], 
        int c, int d)
{

    int i, j, k;
    int a_buf[100][100];
    for (j = 0; j < 100; j++) {
#pragma ACCEL pipeline
        if (c) 
            for (i = 0; i < 100; i++) {
                a_buf[i][j] = b[i][j];
            }
        else 
        if (d) {
            for (k = 0; k < 100; k++)
                a[k][j] = a_buf[k][j];
        }
    }
}

#pragma ACCEL kernel
void func_top_3_0(int a[100][100],int b[100][100],int c,int d)
{
  int i;
  int j;
	int k;
  int a_buf[100][100];
  for (j = 0; j < 100; j++) 
{
#pragma ACCEL pipeline
      for (i = 0; i < 100; i++) 
			{
        a_buf[i][j] = b[i][j];
      }
      for (i = 0; i < 100; i++) 
			{
        a[i][j] = a_buf[i][j];
 	    }
  }
}

#pragma ACCEL kernel
void func_top_3_1(int a[100][100],int b[100][100],int c,int d)
{
  int i;
  int j;
	int k;
  int a_buf[100][100];
  for (j = 0; j < 100; j++) 
{
#pragma ACCEL pipeline
      for (i = 0; i < 100; i++) 
			{
        a_buf[i][j] = b[i][j];
      }
      for (; i < 100; i++) 
			{
        a[i][j] = a_buf[i][j];
 	    }
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
  func_top_3(aa, bb, c_s, d_s);
  func_top_3_0(aa, bb, c_s, d_s);
  func_top_3_1(aa, bb, c_s, d_s);


}
