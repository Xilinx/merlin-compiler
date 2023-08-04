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
void func_top_2_0(int a[100][100], int b[100][100])
{

    int i, j;
    int sum, sum1[100];
    for (i = 0; i < 100; i++) {
    #pragma ACCEL pipeline
        if(i<50)
          continue;
        for (j = 0; j < 100; j++){
            sum += b[i][j];
        }
    }
}


#pragma ACCEL kernel
void func_top_2_1(int a[100][100], int b[100][100])
{

    int i, j;
    int sum, sum1[100];
    sum=0;
    for (i = 0; i < 100; i++) {
    #pragma ACCEL pipeline
        for (j = 0; j < 100; j++){
            sum += b[i][j];
        }
        while(1) {
            if(sum >98)
              break;
            sum1[i] = sum;
        }
    }
}

#pragma ACCEL kernel
void func_top_4(float a[100], float b[100], float c[100])
{

    int i, j;
    float a_buf[100];
    float b_buf[100];
    float c_buf[100];
    for (i = 0; i < 100; i++)
    {
#pragma ACCEL pipeline
        for (j = 0; j < 100; j++)
        {
            a_buf[j] = b[j] * b[j];
        }
        j = 0;
        while (j < 100) {
            c_buf[j] = c[j] * 2;
						j++;
        }
        for (j = 0; j < 100; j+=2) {
            b_buf[j] = a_buf[j] + c_buf[j];
        }
        if(j>50)
          break;
    }
}

#pragma ACCEL kernel
void func_top_3(int a[100][100], int b[100][100])
{

    int i, j;
    int sum, sum1[100];
    for (i = 0; i < 100; i++) {
    #pragma ACCEL pipeline
        for (j = 0; j < 100; j++){
            sum += b[i][j];
        }
        switch(sum) {
          case 100: break;
          default: {
            sum1[i] = sum;
            break;
                   }
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
  func_top_2_1(aa, bb);
  func_top_4(a_f, b_f, c_f);
  func_top_3(aa, bb);


}
