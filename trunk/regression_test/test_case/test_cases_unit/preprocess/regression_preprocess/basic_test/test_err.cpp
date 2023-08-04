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

#pragma ACCEL kernel
void func_top_3(int &a, int * b)
{
    int k;
    int i;
    int j;
    for (j = 0; j < 100; ++j) {
    for (k= 0; k < 100; ++k) {
#pragma ACCEL pipeline
        for (i = 0; i < 100; i++)
        {
            a+= b[i+100*k+j];
        }
		}
   }
}
#if 0
#pragma ACCEL kernel
void func_top_0_1(int * a, int * b)
{
    int i;
    int j;
    int a_buf[100];
  for (i = 0; i < 100; i++) {
    for (j = 0; j < 100; ++j) {
#pragma ACCEL pipeline flatten
            a_buf[i]+= b[i];
        }
    }
}

#pragma ACCEL kernel
void func_top_0_2(int * a, int * b)
{
    int i;
    int j;
    for (j = 0; j < 100; ++j) {
#pragma ACCEL pipeline flatten
        int a_buf[100];
        for (i = 0; i < 100; i++)
        {
            a_buf[i]+= b[i];
        }
    for (i = 100; i >= 0; --i)
        a[i] += a_buf[i];
    }
}

#pragma ACCEL kernel
void func_top_0_3(int * a, int * b)
{
	int k;
    int i;
    int j;
    for (j = 0; j < 100; ++j) {
        int a_buf[100];
    for (k= 0; k < 100; ++k) {
#pragma ACCEL pipeline flatten
        //FIXME:
        for (i = 0; i < 100; i++)
        {
            a_buf[i]+= b[i];
        }
    }

    for (i = 100; i >= 0; --i)
        a[i] += a_buf[i];

		}
}


#pragma ACCEL kernel
void func_top_1(int * a, int * b)
{
    int i;
    int j;
    for (j = 0; j < 100; ++j) {
#pragma ACCEL pipeline flatten
        for (i = 0; i < 100; i++)
        {
            a[2*i]+= b[100*i+j];
        }
    }
}

#pragma ACCEL kernel
void func_top_2(int * a, int * b)
{
    int i;
    int j;
    for (j = 0; j < 100; ++j) {
#pragma ACCEL pipeline flatten
        for (i = 0; i < 100; i++)
        {
            a[j]+= b[100*i+j];
        }
    }
}


#pragma ACCEL kernel
void func_top_4(int a, int * b, int * c)
{
		int k;
    int i;
    int j;
    for (j = 0; j < 100; ++j) {
    for (k= 0; k < 100; ++k) {
        for (i = 0; i < 100; i++)
        {
#pragma ACCEL pipeline
            a+= b[i+100*k+j];
        }
		}
     c[j] = a;
   }
}
#endif
int main() {
     int a;
     int *aa;
     int *b;
     int *c;
//
//   func_top_0_1(aa, b);
//
//   func_top_0_2(aa, b);
//
//   func_top_0_3(aa, b);
//
//   func_top_1(aa, b);
//
//   func_top_2(aa, b);

   func_top_3(a, b);
//
//
//   func_top_4(a, b, c);

}
