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
void func_top_0()
{
  int a[100][100];
  int b[100][100];
  int k;
  int i;
  int j;
  for (i = 0; i < 100; i++) {
    for (k = 0; k < 100; k++) 
      for (j = 0; j < 100; j++) 
      {
        a[i][j] = b[i*k][j]+ b[i*k+1][j+1];
      }
  }
}

#pragma ACCEL kernel
void func_top_1()
{
  int a[100][100];
  int b[100][100];
  int k;
  int i;
  int j;
  for (i = 0; i < 100; i++) {
    for (k = 0; k < 100; k++) 
      for (j = 0; j < 2; j++) 
      {
        a[i][j] = b[i*k][j]+ b[i*k+1][j+1];
      }
  }
}

void foo_top_2() {
  int tmp;
  for (int i = 0; i < 50; i++) {
    tmp++;
  }
}

#pragma ACCEL kernel
void func_top_2()
{
  int a[100][100];
  int b[100][100];
  int k;
  int i;
  int j;
  for (i = 0; i < 100; i++) {
    for (k = 0; k < 100; k++) { 
      for (j = 0; j < 100; j++) 
      {
        a[i][j] = b[i*k][j]+ b[i*k+1][j+1];
      }
      foo_top_2();
    }
  }
}

void foo_top_3() {
  int tmp;
  for (int i = 0; i < 5; i++) {
    tmp++;
    for (int j = 0; j < 3; j++) {
      tmp++;
    }
  }
}

#pragma ACCEL kernel
void func_top_3()
{
  int a[100][100];
  int b[100][100];
  int k;
  int i;
  int j;
  for (i = 0; i < 100; i++) {
    for (k = 0; k < 100; k++) { 
      for (j = 0; j < 100; j++) 
      {
        a[i][j] = b[i*k][j]+ b[i*k+1][j+1];
      }
      foo_top_3();
    }
  }
}

void foo_top_4() {
  int tmp;
  for (unsigned long i = 0; i < 16; i++) {
    tmp++;
    for (unsigned long j = 0; j < 3; j++) {
      tmp++;
    }
  }
}

#pragma ACCEL kernel
void func_top_4()
{
  int a[100][100];
  int b[100][100];
  int k;
  int i;
  int j;
  int tmp;
  for (i = 0; i < 100; i++) {
    for (k = 0; k < 100; k++) { 
      for (j = 0; j < 100; j++) {
        a[i][j] = b[i*k][j]+ b[i*k+1][j+1];
      }
      foo_top_4();
      for (int m = 0 ; m < 50; m++) {
        tmp++;
      }
    }
  }
}

void foo_top_5() {
  int tmp;
  for (int i = 0; i < 16; i++) {
    tmp++;
    for (int j = 0; j < 3; j++) {
      tmp++;
    }
  }
}

#pragma ACCEL kernel
void func_top_5()
{
  int a[100][100];
  int b[100][100];
  int k;
  int i;
  int j;
  int tmp;
  for (i = 0; i < 100; i++) {
    for (k = 0; k < 100; k++) { 
      for (j = 0; j < 100; j++) {
        a[i][j] = b[i*k][j]+ b[i*k+1][j+1];
      }
      foo_top_5();
      for (int m = 0 ; m < 2; m++) {
        tmp++;
      }
    }
  }
}

void foo_top_6() {
  int tmp;
  for (int i = 0; i < 16; i++) {
    tmp++;
    for (int j = 0; j < 3; j++) {
      tmp++;
    }
  }
}

#pragma ACCEL kernel
void func_top_6()
{
  int a[100][100];
  int b[100][100];
  int k;
  int i;
  int j;
  int tmp;
  for (i = 0; i < 4096*4096; i++) {
    for (k = 0; k < 1024*16; k++) { 
      for (j = 0; j < 100; j++) {
        a[i][j] = b[i*k][j]+ b[i*k+1][j+1];
      }
      foo_top_6();
      for (int m = 0 ; m < 1024; m++) {
        tmp++;
      }
    }
  }
}

#pragma ACCEL kernel
void func_top_7()
{
  int a[100][100];
  int b[100][100];
  int k;
  int i;
  int j;
  int tmp;
  for (int i1= 0; i1<20000 ; i1++) {
    for (int i2= 0; i2< 16; i2++) {
      for (int i3= 0; i3< 3; i3++) {
        tmp++; 
      }
    }
    for (int i4= 0; i4< 16; i4++) {
      for (int i5= 0; i5< 3; i5++) {
        tmp++; 
      }
    }
    for (int i6= 0; i6< 16; i6++) {
      for (int i7= 0; i7< 3; i7++) {
        tmp++; 
      }
    }
    for (int i8= 0; i8< 16; i8++) {
      for (int i9= 0; i9< 3; i9++) {
        tmp++; 
      }
    }
    for (int i10= 0; i10< 16; i10++) {
      for (int i11= 0; i11< 16; i11++) {
        tmp++; 
      }
    }
    for (int i12= 0; i12< 16; i12++) {
      for (int i13= 0; i13< 3; i13++) {
        for (int i13= 0; i13< 3; i13++) {
          for (int i13= 0; i13< 3; i13++) {
            for (int i13= 0; i13< 3; i13++) {
              for (int i13= 0; i13< 3; i13++) {
                for (int i13= 0; i13< 3; i13++) {
                  for (int i13= 0; i13< 3; i13++) {
                    tmp++;
                  }
                }
              }
            }
          }
        } 
      }
    }
  }
}

void foo_top_8() {
  int tmp;
  for (int i = 0; i < 50; i++) {
    tmp++;
    for (int j = 0; j < 30; j++) {
      tmp++;
    }
  }
}

#pragma ACCEL kernel
void func_top_8()
{
  int a[100][100];
  int b[100][100];
  int k;
  int i;
  int j;
  for (int i = 0; i < 100; i++) {
    for (int k = 0; k < 100; k++) { 
      int j = 0;
      while (j < 50) 
      {
        a[i][j] = b[i*k][j]+ b[i*k+1][j+1];
        j++;
      }
      foo_top_8();
    }
  }
}

void foo_top_9() {
  int tmp;
  for (int i = 0; i < 50; i++) {
    tmp++;
    for (int j = 0; j < 30; j++) {
      tmp++;
    }
  }
}

#pragma ACCEL kernel
void func_top_9()
{
  int a[100][100];
  int b[100][100];
  int k;
  int i;
  int j;
  for (int i = 0; i < 100; i++) {
    for (int k = 0; k < 100; k++) { 
      int j = 0, n = 50;
      while (j < n) 
      {
#pragma ACCEL loop_tripcount max=50
        a[i][j] = b[i*k][j]+ b[i*k+1][j+1];
        j++;
      }
      foo_top_9();
    }
  }
}

#pragma ACCEL kernel
void func_top_10()
{
  int a[100][100];
  int b[100][100];
  int k;
  int i;
  int j;
  for (int i = 0; i < 100; i++) {
    for (int k = 0; k < 32; k++) { 
#pragma HLS PIPELINE II=1
      int j = 0, n = 50;
    }
  }
}

void foo_top_11() {
  int tmp;
  tmp++;
}
#pragma ACCEL kernel
void func_top_11()
{
  int tmp;
  for (int i1 = 0; i1 < 32; i1++) {
    for (int i2 = 0; i2 < 32; i2++) {
      for (int i3 = 0; i3 < 32; i3++) {
        for (int i4 = 0; i4 < 32; i4++) {
          for (int i5 = 0; i5 < 32; i5++) {
            tmp++;
            tmp++;
            tmp++;
            tmp++;
            tmp++;
            tmp++;
            tmp++;
            foo_top_11();
          }
        }
      }
    }
  }
}

#pragma ACCEL kernel
void func_top_12()
{
  int a[100][100];
  int b[100][100];
  int k;
  int i;
  int j;
  for (i = 0; i < 100; i++) {
    for (k = 0; k < 100; k++) {
#pragma HLS unroll
      for (j = 0; j < 50; j++)  {
        a[i][j] = b[i*k][j]+ b[i*k+1][j+1];
      }
    }
  }
}

#pragma ACCEL kernel
void func_top_13()
{
  int a[100][100];
  int b[100][100];
  int k;
  int i;
  int j;
  for (i = 0; i < 100; i++) {
#pragma HLS unroll
    for (k = 0; k < 100; k++) {
      for (j = 0; j < 50; j++)  {
        a[i][j] = b[i*k][j]+ b[i*k+1][j+1];
      }
    }
  }
}

void foo_top_14() {
  int tmp;
  for (int i = 0; i < 10; i++) {
    tmp++;
  }
}
#pragma ACCEL kernel
void func_top_14()
{
  int tmp;
  for (int i1 = 0; i1 < 32; i1++) {
    for (int i2 = 0; i2 < 32; i2++) {
      for (int i3 = 0; i3 < 32; i3++) {
#pragma ACCEL pipeline
        for (int i4 = 0; i4 < 32; i4++) {
          for (int i5 = 0; i5 < 32; i5++) {
            tmp++;
            tmp++;
            tmp++;
            tmp++;
            tmp++;
            tmp++;
            tmp++;
            foo_top_14();
          }
        }
      }
    }
  }
}

void foo_top_15() {
  int tmp;
  for (int i = 0; i < 10; i++) {
    tmp++;
  }
}
#pragma ACCEL kernel
void func_top_15()
{
  int tmp;
  for (int i1 = 0; i1 < 32; i1++) {
    for (int i2 = 0; i2 < 32; i2++) {
      for (int i3 = 0; i3 < 32; i3++) {
#pragma HLS pipeline
        for (int i4 = 0; i4 < 32; i4++) {
          for (int i5 = 0; i5 < 32; i5++) {
            tmp++;
            tmp++;
            tmp++;
            tmp++;
            tmp++;
            tmp++;
            tmp++;
            foo_top_15();
          }
        }
      }
    }
  }
}

void foo_top_16() {
  int tmp;
  for (int i = 0; i < 10; i++) {
    tmp++;
  }
}
#pragma ACCEL kernel
void func_top_16()
{
  int tmp;
  for (int i1 = 0; i1 < 32; i1++) {
    for (int i2 = 0; i2 < 32; i2++) {
      for (int i3 = 0; i3 < 32; i3++) {
#pragma ACCEL parallel
        for (int i4 = 0; i4 < 32; i4++) {
          for (int i5 = 0; i5 < 32; i5++) {
            tmp++;
            tmp++;
            tmp++;
            tmp++;
            tmp++;
            tmp++;
            tmp++;
            foo_top_16();
          }
        }
      }
    }
  }
}

void foo_top_17() {
  int tmp;
  for (int i = 0; i < 10; i++) {
    tmp++;
  }
}
#pragma ACCEL kernel
void func_top_17()
{
  int tmp;
  foo_top_17();
  for (int i1 = 0; i1 < 32; i1++) {
    foo_top_17();
    for (int i2 = 0; i2 < 32; i2++) {
      foo_top_17();
      for (int i3 = 0; i3 < 32; i3++) {
#pragma ACCEL parallel
        for (int i4 = 0; i4 < 32; i4++) {
          for (int i5 = 0; i5 < 32; i5++) {
            tmp++;
            tmp++;
            tmp++;
            tmp++;
            tmp++;
            tmp++;
            tmp++;
            foo_top_17();
          }
        }
      }
    }
  }
}

#pragma ACCEL kernel
void func_top_21(float a[64][64], float b[64][64], float c[64][64]) {
  for (int i0 = 0; i0 < 32; i0++) {
    for (int i1 = 0; i1 < 32; i1++) {
      for (int i2 = 0; i2 < 32; i2++) {
        for (int i3 = 0; i3 < 8; i3++) {
          if (i2 == 23) {
            c[i1][i2] += a[i2][i3] * b[i2][i3];
          } else if (i2 == 20) {
            c[i1][i2] += a[i2][i3] * b[i2][i3];
            c[i1][i2] += a[i2][i3] * b[i2][i3];
            c[i1][i2] += a[i2][i3] * b[i2][i3];
            c[i1][i2] += a[i2][i3] * b[i2][i3];
            c[i1][i2] += a[i2][i3] * b[i2][i3];
            c[i1][i2] += a[i2][i3] * b[i2][i3];
            c[i1][i2] += a[i2][i3] * b[i2][i3];
            c[i1][i2] += a[i2][i3] * b[i2][i3];
          } else {
            c[i1][i2] += a[i2][i3] * b[i2][i3];
            c[i1][i2] += a[i2][i3] * b[i2][i3];
          }
        }
      }
    }
  }
}

#pragma ACCEL kernel
void func_top_22(float a[64][64], float b[64][64], float c[64][64]) {
  for (int i0 = 0; i0 < 32; i0++) {
    for (int i1 = 0; i1 < 32; i1++) {
      for (int i2 = 0; i2 < 32; i2++) {
        for (int i3 = 0; i3 < 8; i3++) {
          if (i2 == 23) {
            c[i1][i2] += a[i2][i3] * b[i2][i3];
          } else {
            c[i1][i2] += a[i2][i3] * b[i2][i3];
            c[i1][i2] += a[i2][i3] * b[i2][i3];
          }
        }
      }
    }
  }
}


#pragma ACCEL kernel
void func_top_23(int a[64], int b[64]) {
  for (int i = 0; i < 64; i++) {
    b[i] = a[i] * 2;
  }
}
