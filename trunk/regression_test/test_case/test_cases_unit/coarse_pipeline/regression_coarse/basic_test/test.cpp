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


// 1. loop structure
// 2. function structure
// 3. access pattern
// 4. pragma modes
void sub_top_0_0(int * a) {
    int i;
    for (i = 2; i < 100; i+=2) 
        a[i] += i;
}

#pragma ACCEL kernel
void func_top_0(int * a, int * b)
{
    int i;
    int j;
    int a_buf[100];
    for (j = 0; j < 100; ++j) {
#pragma ACCEL pipeline
        //FIXME:
        //int a_buf[100];
        for (i = 1; i < 100; i++)
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
void func_top_1(int a[100][100], int b[100][100])
{
    int i, j;
    int a_buf[100][100];
    for (i = 0; i < 100; i++) {
#pragma ACCEL pipeline
        for (j = 0; j < 100; j++)   //bracelet
        {
            a_buf[i][j] = b[i][j];
        }
        //FIXME:
        //L2: for (j = 0; j < 100; ++j) 
        for (j = 0; j < 100; ++j) 
            a[i][j] = j + a_buf[i][j];
    }
}

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
        j = 100;
        while(j < 100) {
            a[i][j] = a_buf[i][j];
            --j;
        }
    }
}

#pragma ACCEL kernel
void func_top_2_1(int a[100][100], int b[100][100])
{

    int i, j;
    int a_buf[100][100];
    for (i = 0; i < 100; i++) {
#pragma ACCEL pipeline
        for (j = 0; j < 100; j++)
        {
            a_buf[i][j] = b[i][j];
        }
				{
        j = 100;
        while(j < 100) {
            a[i][j] = a_buf[i][j];
            --j;
        }
				}
    }
}

void func_top_2_2(int a[100][100], int b[100][100])
{

    int i, j;
    int a_buf[100][100];
    for (i = 0; i < 100; i++) {
#pragma ACCEL pipeline
        for (j = 0; j < 99; j++)
        {
            a_buf[i][j] = b[i][j];
        }
        while(j < 100) {
            a[i][j] = a_buf[i][j];
            --j;
        }
    }
}

//Loop structure 
//Won't apply coarse pipeline because the loop iterator is used outside the for loop
#pragma ACCEL kernel
void func_top_3(int a[100][100], int b[100][100], 
        int c, int d)
{

    int i, j;
    int a_buf[100][100];
    for (j = 0; j < 100; j++) {
#pragma ACCEL pipeline
        if (c) 
            for (i = 0; i < 100; i++) {
                a_buf[i][j] = b[i][j];
            }
        else 
            a_buf[i][50] = b[i][50];
        if (d) {
            for (i = 0; i < 100; i++)
                a[i][j] = a_buf[i][j];
        }
    }
}

#pragma ACCEL kernel
void func_top_3_0(int a[100][100],int b[100][100],int c,int d)
{
  int i;
  int j;
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

void func_top_4(float a[100][100], float b[100][100])
{

    int i, j;
    float a_buf[100][100];
    for (i = 0; i < 100; i++)
    {
#pragma ACCEL pipeline
        for (j = 0; j < 100; j++)
        {
            a_buf[i][j] = b[i][j] * b[i][j];
        }
        for (j = 0; j < 99; j++)
        {
            a[i+1][j] += a_buf[i][j+1];
        }
    }
}

void func_top_4_0(float a[100][100], float b[100][100])
{

    int i, j;
    float a_buf[100][100];
    for (i = 0; i < 100; i++)
    {
#pragma ACCEL pipeline
        for (j = 0; j < 100; j++)
        {
            a_buf[i][j] = b[i][j] * b[i][j];
        }
        for (j = 0; j < 100; j++)
        {
            a[i][j] += a_buf[i][j];
        }
    }
}

#pragma ACCEL kernel
void func_top_5(int a[100][100],int b[100][100], int c[100][100])
{

    int i=0, j=0;

    int a_buf[100][100];
    int c_buf[100][100];
    for (i = 0; i < 100; i++)
    {
#pragma ACCEL pipeline
        for (j = 0; j < 100; j++)
        {
            a_buf[i][j] = b[i][j];
            c_buf[i][j] = b[j][i];
        }
        for (j = 0; j < 100; j++)
        {
            a[i+2][j] = a_buf[i+2][j];
        }
        for (j = 0; j < 100; j++)
        {
            c[i+1][j] = c_buf[i][j+1] + a_buf[i][j];
        }
    }
    for (i = 0; i < 100; ++i) {
#pragma ACCEL pipeline
        for (j = 0; j < 100; j++)
        {
            a_buf[i+1][j] = b[i][j+1];
        }

        for (j = 0; j < 100; j++)
        {
            a[i+1][j] = a_buf[i][j+1];
        }
    }

}

//ifelse



#pragma ACCEL kernel
void func_top_6(int a[100][100], int b[100][100])
{

    int i, j;
    int a_buf[100][100];
    for (i = 0; i < 100; i++) {
#pragma ACCEL pipeline
        if (b[0][0]) 
            for (j = 0; j < 100; j++) {
                a_buf[i][j] = b[i][j];
            }
        if (a_buf[1][1])
            for (j = 0; j < 100; j++)
                a[i][j] = a_buf[i][j];
    }
}

// sub function
void sub_0(int a[100][100], int b[100][100])
{

    int i, j;
    for (i = 0; i < 100; i++) {
        if (a[0][0]) {for (j = 0; j < 100; j++) 
            a[i][j] += b[i][j];

        }
    }
}

#pragma ACCEL kernel
void func_top_7(int a[100][100],
        int b[100][100],
        int c[100][100]) {

    int i;
    int a_buf[100][100];
    int c_buf[100][100];
    for (i = 0; i < 2; ++i) {
#pragma ACCEL pipeline
        sub_0(a_buf, b);
        sub_0(a, a_buf);
        sub_0(c_buf, a_buf);
        sub_0(c, c_buf);
    }
}

int sub_0_0(int a[100][100], int b[100][100], int c)
{
    int i, j;
    for (i = 0; i < 100; i++) {
        a[c][i] += b[i][j] + c;
    }
    return 1024;
}

#pragma ACCEL kernel
void func_top_8(int a[100][100],
        int b[100][100],
        int c[100][100]) {



    int sum = 0;
    int i = 0, j;
    int a_buf[100][100];
    for (i = 0; i < 100; ++i) {
#pragma ACCEL pipeline
        sub_0(a_buf, b);
        sum = sub_0_0(c, a_buf, i);
        for (j = 0; j < 50; ++j)
            a[j][i] = sum + a_buf[i][j];
    }


}



void sub_1(int *a, int *b) 
{
    int i, j;
    for (i = 0; i < 100; i++) {
        if (a[0]) for (j = 0; j < 100; j++) {
#pragma ACCEL pipeline
            a[i* 100 + j] = b[i * 100 + j];
        }
    }
}

void func_top_9(int *a, int *b) 
{
    int aa[10000];
    int bb[10000];
    int i;
    for (i = 0; i < 100; ++i) {
#pragma ACCEL pipeline
        sub_1(aa, a);
        sub_1(bb, aa);
        sub_1(b, bb);
    }
}

#pragma ACCEL kernel
void func_top_10(int *a, int *b) {
    int i;
    int a_buf[10000];
    for (i = 0; i < 10; ++i) {
#pragma ACCEL pipeline
       func_top_9(a, a_buf);
       func_top_9(a_buf, b);
    }
}

#pragma ACCEL kernel
void func_top_11(float a[100][100], float b[100][100])
{

    int i, j;
    float a_buf[100][100];
    for (i = 0; i < 100; i++)
    {
//#pragma ACCEL pipeline
        for (j = 0; j < 100; j++)
        {
#pragma ACCEL pipeline
            func_top_4(a_buf, a);
            func_top_4(b, a_buf);
        }
        for (j = 0; j < 100; j++)
        {
            a[i+1][j] = a_buf[i][j+1];
        }
    }
}

#pragma ACCEL kernel
void func_top_11_0(float a[100][100], float b[100][100])
{

    int i, j;
    float a_buf[100][100];
    for (i = 0; i < 100; i++)
    {
//#pragma ACCEL pipeline
        for (j = 0; j < 100; j++)
        {
#pragma ACCEL pipeline
            func_top_4_0(a_buf, a);
            func_top_4_0(b, a_buf);
        }
        for (j = 0; j < 100; j++)
        {
            a[i+1][j] = a_buf[i][j+1];
        }
    }
}

#pragma ACCEL kernel
void func_top_12(int * a, int * b, int *c)
{
    int i;
    int j;
    int a_buf[100];
    for (j = 0; j < 100; ++j) {
#pragma ACCEL pipeline
        for (i = 0; i < 100; i++)
        {
            a_buf[i] = b[i] + *c;
        }
        sub_top_0_0(a_buf);
        for (i = 100; i >= 0; --i)
            a[i] = a_buf[i];

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

  for (i = 0; i < 100; i++)
  for (j = 0; j < 100; j++)
	{
			a[i][j]=i;
	}
	int c;
	for (i = 0; i < 100; i++)
  {
#pragma ACCEL pipeline
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

#pragma ACCEL kernel
void func_top_14(int a[100][100],int b[100][100], int c[100][100])
{
		int k;
    int i=0, j=0;

    int a_buf[100][100];
    int c_buf[100][100];
	 for ( k= 0; k < 100; k++)
    {
#pragma ACCEL pipeline
			for (j = 0; j < 100; j++)
        {
            a_buf[k][j] = b[k][j];
            c_buf[k][j] = b[j][k];
        }
    }

    for (i = 0; i < 100; ++i) {
#pragma ACCEL pipeline
       for (j = 0; j < 100; j++)
        {
            a_buf[i+1][j] = b[i][j+1];
        }

        for (j = 0; j < 100; j++)
        {
						k=0;
            a[i+1][j] = a_buf[i+k][j];
        }
    }
}


#pragma ACCEL kernel
void func_top_14_1(int a[100][100],int b[100][100], int c[100][100])
{
		int k;
    int i=0, j=0;

    int a_buf[100][100];
    int c_buf[100][100];
	 for ( k= 0; k < 100; k++)
    {
#pragma ACCEL pipeline
			for (j = 0; j < 100; j++)
        {
            a_buf[k][j] = b[k][j];
            c_buf[k][j] = b[j][k];
        }
    }

    for (i = 0; i < 100; ++i) {
#pragma ACCEL pipeline
       for (j = 0; j < 100; j++)
        {
            a_buf[i+1][j] = b[i][j+1];
        }

        for (j = 0; j < 100; j++)
        {
						int k=0;
            a[i+1][j] = a_buf[i+k][j];
        }
    }
}

#pragma ACCEL kernel
void func_top_14_2(int a[100][100],int b[100][100], int c[100][100])
{
		int k;
    int i=0, j=0;

    int a_buf[100][100];
    int c_buf[100][100];
	 for ( k= 0; k < 100; k++)
    {
			for (j = 0; j < 100; j++)
        {
            a_buf[k][j] = b[k][j];
            c_buf[k][j] = b[j][k];
        }
    }

    for (i = 0; i < 100; ++i) {
#pragma ACCEL pipeline
       for (j = 0; j < 100; j++)
        {
            a_buf[i+1][j] = b[i][j+1];
        }

        for (j = 0; j < 100; j++)
        {
            a[i+1][j] = a_buf[i+k][j];
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

  func_top_0(a, b);
  func_top_0_1(a, b);
  func_top_0_0(a, b);
  func_top_1(aa, bb);
  func_top_2(aa, bb);
  func_top_2_1(aa, bb);
  func_top_3(aa, bb, c_s, d_s);
  func_top_3_0(aa, bb, c_s, d_s);
//  func_top_4(aa_f, bb_f);
  func_top_5(aa, bb, cc);
  func_top_6(aa, bb);
  func_top_7(aa, bb, cc);
  func_top_8(aa, bb, cc);
//  func_top_9(a, b);
  func_top_10(a, b);
  func_top_11(aa_f, bb_f);
  func_top_11_0(aa_f, bb_f);
  func_top_12(a, b, c);
  func_top_13();
  func_top_14(aa, bb, cc);

  func_top_14_1(aa, bb, cc);
  func_top_14_2(aa, bb, cc);
}
