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

//Won't apply coarse pipeline because the loop iterator is used outside the for loop
#pragma ACCEL kernel
void func_top_2(int a[100][100], int b[100][100])
{

    int i, j;
    int a_buf[100][100];
    for (i = 0; i < 100; i++) {
#pragma ACCEL pipeline
        for (j = 0; j < 100; j++)
        {
            a_buf[i][j] = b[i][j];
        }
        j = 0;
        for (; j < 100; j++)
        {
            a[i][j] = a_buf[i][j];
        }
    }
}

int sub_13(int a[100][100])
{
	if(a[0][0])
	return 1;
}

#pragma ACCEL kernel
void func_top_13()
{
  int a[100][100];
	int b[100][100];
  int i=0, j=0;

 	int c;
	for (i = 0; i < 100; i++)
  {
#pragma ACCEL pipeline
		for (j = 0; j < 100; j++)
		{
			a[i][j]=i;
		}
    {
			a[0][0]=1;
			a[0][1]=1;
		}
		c=sub_13(a);
    for (j = 0; j < 100; j++)
    {
        b[i][j] = a[i][j]+c;
    }
 }
}

int sub_14(int a)
{
	if(a)
	return 1;
}

#pragma ACCEL kernel
void func_top_14()
{
  int a[100][100];
	int b[100][100];
  int i=0, j=0;

 	int c;
	for (i = 1; i < 100; i++)
  {
#pragma ACCEL pipeline
		for (j = 0; j < 100; j++)
		{
			a[i][j]=i;
		}
		c=sub_14(a[0][0]);
    for (j = 0; j < 100; j++)
    {
        b[i][j] = a[i][j]+c;
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
  func_top_2(aa, bb);
  func_top_13();
  func_top_14();


}
