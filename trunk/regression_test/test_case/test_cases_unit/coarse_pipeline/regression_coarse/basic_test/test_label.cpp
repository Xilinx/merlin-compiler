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
#define VEC_SIZE 10000
#define SUB_SIZE 100

#pragma ACCEl kernel
void func_top_2_1(int a[100][100], int b[100][100])
{

    int i, j;
    int a_buf[100][100];
loop1:    for (i = 0; i < 100; i++) {
#pragma ACCEL pipeline
loop2:        for (j = 0; j < 100; j++)
        {
            a_buf[i][j] = b[i][j];
						if(j==5)
							goto loop1;
        }
				
    }
}

#pragma ACCEl kernel
void func_top_2_2(int a[100][100], int b[100][100])
{

    int i, j;
    int a_buf[100][100];
loop1:    for (i = 0; i < 100; i++) {
#pragma ACCEL pipeline
loop2:        for (j = 0; j < 100; j++)
        {
            a_buf[i][j] = b[i][j];
						if(j==5)
							goto r1;
        }
r1:			break;
    }
}

#pragma ACCEl kernel
void func_top_2_3(int a[100][100], int b[100][100])
{

    int i, j;
		int sum=0;
    int a_buf[100][100];
loop1:    for (i = 0; i < 100; i++) {
#pragma ACCEL pipeline
loop2:        for (j = 0; j < 100; j++)
        {
            a_buf[i][j] = b[i][j];
        }
							sum+= a_buf[i][0];
    }
}

#pragma ACCEl kernel
void func_top_2_4(int a[100][100], int b[100][100], int c)
{
		int sum=0;
    int i, j;
    int a_buf[100][100];
loop1:    for (i = 0; i < 100; i++) {
#pragma ACCEL pipeline
loop2:        for (j = 0; j < 100; j++)
        {
            a_buf[i][j] = b[i][j];
        }
							sum+= a_buf[i][0];
    }
	  if(c>0) 
			goto loop2;
}

#pragma ACCEL kernel
void func_top_2_5(int a[100][100], int b[100][100])
{

    int i, j;
    int a_buf[100][100];
loop1:    for (i = 0; i < 100; i++) {
#pragma ACCEL pipeline
loop2:        for (j = 0; j < 100; j++)
        {
            a_buf[i][j] = b[i][j];
        }
			  goto loop2;
    }
}

#pragma ACCEL kernel
void func_top_2_6(int a[100][100], int b[100][100])
{

  int i, j;
  int a_buf[100][100];
loop1:    for (i = 0; i < 100; i++) {
#pragma ACCEL pipeline
loop2:        for (j = 0; j < 100; j++)
              {
                a_buf[i][j] = b[i][j];
                if(i<5)
                  goto loop2;
              }
              a[0][i]=0;
          }
}

#pragma ACCEL kernel
void vec_add_kernel(int *a, int *b, int *c, int inc) {
	for (int i = 0; i < SUB_SIZE; i++) {
#pragma ACCEL  pipeline
		int local_buf[SUB_SIZE];
		for (int j = 0 ;j < SUB_SIZE; j++) 
		{
			local_buf[j]  = a[i * SUB_SIZE + j] + b[i * SUB_SIZE+ j]+ c[i * SUB_SIZE + j];
		}

		for (int j = 0; j < SUB_SIZE; j++) {
			c[i * SUB_SIZE + j]+= local_buf[j];
		}
		if(i>inc) {
			goto L0;
		}
	}
L0:
  return;
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
	int d;
  func_top_2_1(aa, bb);
  func_top_2_2(aa, bb);
  func_top_2_3(aa, bb);
  func_top_2_4(aa, bb, d);

}
