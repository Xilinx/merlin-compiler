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

void func_top_0(int *a,int *b)
{
  int i;
  for (i = 0; i < 100; i++) {
    
#pragma ACCEL pipeline_parallel factor = 10
    a[i] = b[i];
  }
}

void func_top_1(int a[100][100],int b[100][100])
{
  int i;
  int j;
  for (i = 0; i < 100; i++) 
//bracelet
    for (j = 0; j < 100; j++) {
      
#pragma ACCEL pipeline
      a[i][j] = b[i][j];
    }
}

void func_top_2(int a[100][100],int b[100][100])
{
  int i;
  int j;
  for (i = 0; i < 100; i++) {
    for (j = 0; j < 100; j++) {
      
#pragma ACCEL pipeline_parallel factor = 10
      a[i][j] = b[i][j];
    }
  }
}
//Loop structure 

void func_top_3(int a[100][100],int b[100][100])
{
  int i;
  int j;
  for (j = 0; j < 100; j++) 
    for (i = 0; i < 100; i++) {
      
#pragma ACCEL pipeline
      a[i][j] = b[i][j];
    }
}

void func_top_4(int a[100][100],int b[100][100])
{
  int i;
  int j;
  for (i = 0; i < 100; i++) {
    for (j = 0; j < 100; j++) {
      
#pragma ACCEL pipeline
      a[i][j] = b[i][j];
    }
    for (j = 0; j < 100; j++) {
      
#pragma ACCEL pipeline_parallel factor = 10
      a[i + 1][j] = b[i][j + 1];
    }
  }
}

void func_top_4_0(int a[100][100],int b[100][100])
{
  int i;
  int j;
  for (j = 0; j < 100; j++) {
    for (i = 0; i < 100; i++) {
      
#pragma ACCEL pipeline
      a[i][j] = b[i][j];
    }
    for (i = 0; i < 100; i++) {
      
#pragma ACCEL pipeline_parallel factor = 10
      a[i + 1][j] = b[i][j + 1];
    }
  }
}

void func_top_5(int a[100][100],int b[100][100])
{
  int i = 0;
  int j = 0;
  for (j = 0; j < 100; j++) {
    
#pragma ACCEL pipeline_parallel factor = 10
    a[i + 1][j] = b[i][j + 1];
  }
  for (i = 0; i < 100; i++) {
    for (j = 0; j < 100; j++) {
      
#pragma ACCEL pipeline
      a[i][j] = b[i][j];
    }
    for (j = 0; j < 100; j++) {
      a[i + 2][j] = b[i + 2][j];
    }
    for (j = 0; j < 100; j++) {
      
#pragma ACCEL pipeline
      a[i + 1][j] = b[i][j + 1];
    }
  }
  for (j = 0; j < 100; j++) {
    
#pragma ACCEL parallel factor = 10
    a[i + 1][j] = b[i][j + 1];
  }
}
//ifelse

void func_top_6(int a[100][100],int b[100][100])
{
  int i;
  int j;
  for (i = 0; i < 100; i++) {
    for (j = 0; j < 100; j++) {
      
#pragma ACCEL pipeline_parallel factor = 10
      if (a[0][0]) 
        a[i][j] = b[i][j];
    }
  }
}

void func_top_7(int a[100][100],int b[100][100])
{
  int i;
  int j;
  for (i = 0; i < 100; i++) {
    if (a[0][0]) 
      for (j = 0; j < 100; j++) {
        
#pragma ACCEL pipeline_parallel factor = 10
        a[i][j] = b[i][j];
      }
  }
}

void func_top_8(int a[100][100],int b[100][100])
{
  int i;
  int j;
  for (i = 0; i < 100; i++) {
    if (a[0][0]) 
      for (j = 0; j < 100; j++) {
        
#pragma ACCEL pipeline
        a[i][j] = b[i][j];
      }
  }
}
// sub function

void sub_0(int a[100][100],int b[100][100])
{
  int i;
  int j;
  for (i = 0; i < 100; i++) {
    if (a[0][0]) {
      for (j = 0; j < 100; j++) {
        
#pragma ACCEL pipeline
        a[i][j] = b[i][j];
      }
    }
  }
}

void func_top_9()
{
  int a[100][100];
  int b[100][100];
  sub_0(a,b);
}

int sub_0_0(int a[100],int b[100][100],int c)
{
  int i;
  int j;
  for (i = 0; i < 100; i++) {
    for (j = 0; j < 100; j++) {
      
#pragma ACCEL pipeline_parallel factor = 10
      a[i] = b[i][j] + c;
    }
  }
  return 1024;
}

void func_top_10()
{
  int a[100][100];
  int b[100][100];
  int c = 1;
  int sum = sub_0_0(a[0],b,c);
}

void sub_1(int a[100][100],int b[100][100])
{
  int i;
  int j;
  for (i = 0; i < 100; i++) {
    if (a[0][0]) 
      for (j = 0; j < 100; j++) {
        
#pragma ACCEL pipeline_parallel factor = 10
        a[i][j] = b[i][j];
      }
  }
}

void func_top_11()
{
  int a[100][100];
  int b[100][100];
  sub_1(a,b);
  sub_1(a,b);
}

int sub_2(int b[100][100],int i,int j)
{
  return b[i][j] + b[i][j + 1];
}

int sub_00(int a[100],int b[100][100],int c)
{
  int i;
  int j;
  for (i = 0; i < 100; i++) {
    for (j = 0; j < 100; j++) {
      
#pragma ACCEL pipeline_parallel factor = 10
      a[i] = sub_2(b,i,j);
    }
  }
  return 1024;
}

void func_top_12()
{
  int a[100][100];
  int b[100][100];
  int c = 1;
  int sum = sub_00(a[0],b,c);
}
// Acccess pattern

void func_top_13(int a[100][100],int b[1000000])
{
  int i;
  int j;
  int k;
  for (i = 0; i < 100; i++) 
    for (k = 0; k < 100; k++) {
      for (j = 0; j < 100; j++) {
        
#pragma ACCEL pipeline_parallel factor = 10
        a[i][j] = b[100 * i + j];
      }
    }
}

void func_top_14(int a[100][100],int b[100][100])
{
  int i;
  int j;
  for (j = 0; j < 100; j++) 
    for (i = 0; i < 100; i++) {
      
#pragma ACCEL pipeline_parallel factor = 4
      a[i][j] = b[i][j] + b[i + 1][j] + b[i - 1][j];
    }
}

void func_top_15(int a[100][100],int b[100][100])
{
  int i;
  int j;
  for (i = 0; i < 100; i++) 
    for (j = 0; j < 100; j++) {
      
#pragma ACCEL pipeline
      a[i][j] = b[2 * i][j] + b[2 * i + 1][j] + b[2 * i - 1][j];
    }
}

void func_top_16(int a[100][100],int b[100][100])
{
  int i;
  int j;
  for (i = 0; i < 100; i++) 
    for (j = 0; j < 100; j++) {
      
#pragma ACCEL pipeline_parallel factor = 10
      a[i][j] = b[16 * i][j] + b[16 * i + 1][j] + b[16 * i - 1][j];
    }
}

void func_top_17(int a[100][100],int b[100][100])
{
  int i;
  int j;
  for (i = 0; i < 100; i++) 
    for (j = 0; j < 100; j++) {
      
#pragma ACCEL pipeline
      a[i][j] = b[10587946 * i][j] + b[10587946 * i + 1][j] + b[10587946 * i - 1][j];
    }
}

void func_top_18(int a[100][100],int b[100][100])
{
  int i;
  int j;
  for (i = 0; i < 100; i++) 
    for (j = 0; j < 100; j++) {
      
#pragma ACCEL pipeline_parallel factor = 10
      a[i][j] = b[10587946 * i][j] + b[35935221 * i + 1][j] + b[43453045 * i - 1][j];
    }
}

void func_top_19(int a[100][100],int b[1024])
{
  int i;
  int j;
  for (j = 0; j < 100; j++) 
    for (i = 0; i < 100; i++) {
      
#pragma ACCEL pipeline_parallel factor = 10
      a[i][j] = b[1024 * i + j] + b[1024 + j] + b[1024 * i - 1024 + j];
    }
}

void func_top_20(int a[100][100],int b[100][100])
{
  int i;
  int j;
  for (i = 0; i < 100; i++) 
    for (j = 0; j < 100; j++) {
      
#pragma ACCEL pipeline_parallel factor = 10
      a[i][j] = b[i][j] + b[i + 1][j] + b[i + 2][j] + b[i + 3][j] + b[i + 4][j] + b[i + 5][j] + b[i + 6][j] + b[i + 7][j] + b[i + 8][j] + b[i + 9][j] + b[i + 10][j] + b[i + 11][j] + b[i + 12][j] + b[i + 13][j] + b[i + 14][j] + b[i + 15][j] + b[i + 16][j] + b[i + 17][j] + b[i + 18][j] + b[i + 19][j] + b[i + 20][j] + b[i + 21][j] + b[i + 22][j] + b[i + 23][j] + b[i + 24][j] + b[i + 25][j] + b[i + 26][j] + b[i + 27][j] + b[i + 28][j] + b[i + 29][j] + b[i + 30][j];
    }
}

void func_top_21(int a[100][100],int b[1000000])
{
  int i;
  int j;
  for (j = 0; j < 100; j++) 
    for (i = 0; i < 100; i++) {
      
#pragma ACCEL pipeline_parallel factor = 10
      a[i][j] = b[10587946 * i + j];
    }
}

void func_top_22(int a[100][100],int b[100][100])
{
  int i;
  int j;
  for (i = 0; i < 100; i++) 
    for (j = 0; j < 100; j++) {
      
#pragma ACCEL pipeline_parallel factor = 10
      a[i][j] = b[3 * i][j] + b[2 * i][j] + b[1 * i - 1][j];
    }
}

void func_top_23(int a[100][100],int b[100][100])
{
  int i;
  int j;
  for (i = 0; i < 100; i++) 
    for (j = 0; j < 100; j++) {
      
#pragma ACCEL pipeline_parallel factor = 10
      a[i][j] = b[3 * i][j];
    }
  for (j = 0; j < 100; j++) 
    for (i = 0; i < 100; i++) {
      
#pragma ACCEL pipeline_parallel factor = 10
      a[i][j] = b[3 * i][j];
    }
}

void func_top_24(int b[100][100])
{
  int a[100][100];
  int i;
  int j;
  for (i = 0; i < 100; i++) {
    for (j = 0; j < 100; j++) {
      
#pragma ACCEL pipeline_parallel factor = 10
      a[i][j] += b[i][j] * b[i - 1][j];
    }
  }
}

void func_top_25(int *a,int *b)
{
  int i;
  int j;
  for (i = 0; i < 100; i++) 
    for (j = 0; j < 100; j++) {
      
#pragma ACCEL pipeline
      a[100 * i + 2 * j] = b[100 * i + j + 1] * b[100 * i - 100 + j - 1];
    }
}
// pragma modes

void func_top_26(int *a,int *b)
{
  int i;
  int j;
  for (i = 0; i < 100; i++) 
    for (j = 0; j < 100; j++) {
      
#pragma ACCEL pipeline
      a[100 * i + j] += b[100 * i + 100 + j];
    }
}

void func_top_26_1(int *a,int *b)
{
  int i;
  int j;
  for (i = 0; i < 100; i++) 
    for (j = 0; j < 100; j++) {
      
#pragma ACCEL pipeline
      a[100 * i + j] += b[100 * i + 100 + 2 * j];
    }
}

void func_top_26_2(int *a,int *b)
{
  int i;
  int j;
  for (i = 0; i < 100; i++) 
    for (j = 0; j < 100; j++) {
      
#pragma ACCEL pipeline
      a[100 * i + 2 * j] = b[100 * i + 2 * j - 1];
    }
}
// pragma modes

void func_top_27(int a[100][100])
{
  int i;
  int j;
  for (j = 0; j < 100; j++) 
    for (i = 0; i < 100; i++) {
      
#pragma ACCEL parallel factor = 10
      a[i][j] += a[i + 1][j];
    }
}

void func_top_28(int a[100][100],int b[100][100])
{
  int i;
  int j;
  for (i = 0; i < 100; i++) 
    for (j = 0; j < 100; j++) {
      
#pragma ACCEL pipeline
      a[i][j] += a[i + 1][j] * a[i - 1][j];
    }
}
// pragma modes

void func_top_29(int a[100][100],int b[100][100])
{
  int i;
  int j;
  for (i = 0; i < 100; i++) 
    for (j = 0; j < 100; j++) {
      
#pragma ACCEL parallel complete
      a[i][j] += a[i + 1][j] * a[i - 1][j];
    }
}
// pragma modes

void func_top_30(int a[100][100],int b[100][100])
{
  int i;
  int j;
  for (i = 0; i < 100; i++) 
    for (j = 0; j < 100; j++) {
      
#pragma ACCEL pipeline_parallel complete priority = logic
      a[i][j] = b[i - 1][j];
    }
  for (i = 0; i < 100; i++) 
    for (j = 0; j < 100; j++) {
      
#pragma ACCEL pipeline
      a[i][j] = b[i + 1][j];
    }
  for (i = 0; i < 100; i++) 
    for (j = 0; j < 100; j++) {
      
#pragma ACCEL pipeline_parallel complete priority = logic
      a[i][j] = b[i][j];
    }
}
// test priority

void func_top_31(int a[100][100],int b[100][100])
{
  int i;
  int j;
  for (i = 0; i < 100; i++) 
    for (j = 0; j < 100; j++) {
      
#pragma ACCEL pipeline_parallel factor=4 priority=logic
      a[i][j] = b[i - 1][j] + b[i + 1][j] + b[i][j];
    }
}
//test pragma parameter

void func_top_32(int a[100],int b[100])
{
  int i;
  for (i = 0; i < 100; i++) {
    
#pragma ACCEL pipeline
{
      a[i] = b[i];
    }
  }
}

void func_top_33(int *a,int *b)
{
  int i;
  for (i = 0; i < 100; i++) {
    
#pragma ACCEL pipeline_parallel factor = - 1
    a[i] = b[i];
  }
}

void func_top_33_1(int *a,int *b)
{
  int i;
  int NN = 100;
  for (i = 0; i < 100; i++) {
    
#pragma ACCEL pipeline_parallel factor = NN
    a[i] = b[i];
  }
}

void func_top_34(int a[100],int b[100])
{
  int i;
  int NN = 100;
  for (i = 0; i < NN; i++) {
    
#pragma ACCEL pipeline_parallel factor = NN
    a[i] = b[i];
  }
}
//test loop range

void func_top_35(int *a,int *b)
{
  int i;
  for (i = - 1; i < 9; i++) {
    
#pragma ACCEL pipeline_parallel factor = 10
    a[i] = b[i];
  }
}

void func_top_35_1(int *a,int *b)
{
  int i;
  int j;
  for (j = - 1; j < 10 - 1; j++) {
    
#pragma ACCEL pipeline_parallel complete priority = logic
    a[j] = b[j];
  }
}

void func_top_36(int a[100],int b[100])
{
  int i;
  for (i = 0; i < 100; i++) {
    
#pragma ACCEL pipeline
    a[i] = b[i];
  }
}

void func_top_37(int a[10000],int b[10000])
{
  int i;
  for (i = 0; i < 100; i++) {
    
#pragma ACCEL pipeline_parallel factor = 100
    a[i] = b[i];
  }
}

void func_top_38(int a[100][100],int b[100][100])
{
  int i;
  int j;
  for (j = 0; j < 100; j++) {
    
#pragma ACCEL pipeline
    for (i = 0; i < 100; i++) {
      
#pragma ACCEL parallel complete
      a[i][j] = b[i][j];
    }
  }
}

void func_top_39(int a[100][100],int b[100][100])
{
  int i;
  int j;
  for (i = 0; i < 100; i++) {
    
#pragma ACCEL pipeline
    for (j = 0; j < 100; j++) {
      
#pragma ACCEL parallel factor=10
      a[i][j] = b[i][j];
    }
  }
}

void func_top_40(int a[100][100],int b[100][100])
{
  int i;
  int j;
  for (i = 0; i < 100; i++) {
    for (j = 0; j < 100; j++) {
      
#pragma ACCEL pipeline
      
#pragma ACCEL parallel factor=complete
      a[i][j] = b[i][j];
    }
  }
}

void func_top_41(int a[100][100],int b[100][100])
{
  int i;
  int j;
  int k;
  for (k = 0; k < 100; k++) {
    
#pragma ACCEL pipeline
    for (i = 0; i < 100; i++) {
      for (j = 0; j < 100; j++) {
        a[i][j] = b[i][j];
      }
    }
  }
}

void func_top_42(int a[100][100],int b[100][100])
{
  int i;
  int j;
  int k;
  for (k = 0; k < 100; k++) {
    
#pragma ACCEL parallel factor = 10
    for (i = 0; i < 100; i++) {
      for (j = 0; j < 100; j++) {
        a[i][j] = b[i][j];
      }
    }
  }
}

void func_top_43(int a[100],int b[100])
{
  int i;
  for (i = 0; i < 100; i++) {
    b[i] = i;
  }
  for (i = 0; i < 100; i++) {
    
#pragma ACCEL pipeline_parallel factor = 10
    a[i] = b[i];
  }
}

void func_top_44(int a[100][100],int b[100][100])
{
  int i;
  int j;
  for (i = 0; i < 100; i++) 
    for (j = 0; j < 100; j++) {
      
#pragma ACCEL pipeline_parallel factor = 10
      a[i][j] = b[3 * i][j] + b[2 * i][j] + b[1 * i - 1][j];
    }
  for (i = 0; i < 100; i++) {
    int k;
    for (j = 0; j < 100; j++) {
      
#pragma ACCEL pipeline_parallel factor = 10
      a[i][j] = b[3 * i][i + j] + b[2 * i][i + j] + b[1 * i - 1][i + j + 1];
    }
  }
}
#define _PB_N 5
#define n 5

void func_top_45(int A_out[5],int k)
{
  int A[5][5][5][5][5];
  int i0;
  int i1;
  int i2;
  int i3;
  int i4;
  int j;
//	for( i0 = 0 ; i0 <  n-1; i0++){
  for (i1 = 0; i1 < 5 - 1; i1++) {
    for (i2 = 0; i2 < 5 - 1; i2++) {
      for (i3 = 0; i3 < 5 - 1; i3++) {
        for (i4 = 0; i4 < 5 - 1; i4++) {
          for (j = 0; j < 5 - 1; j++) {
            A[i1][i2][i3][i4][j] = j;
          }
        }
      }
    }
  }
  for (i1 = 0; i1 < 5 - 1; i1++) {
    for (i2 = 0; i2 < 5 - 1; i2++) {
      for (i3 = 0; i3 < 5 - 1; i3++) {
        for (i4 = 0; i4 < 5 - 1; i4++) {
          for (j = 0; j < 5 - 1; j++) {
            
#pragma ACCEL parallel factor=4
            int r0 = A[i1][i2][i3][i4][j];
            A_out[j] = r0 / 8;
          }
        }
      }
    }
  }
}

void func_top_46(int a[100][100])
{
  int b[100][100];
  int i;
  int j;
  int k;
  for (i = 0; i < 100; i++) {
    for (j = 0; j < 100; j++) {
      int var1 = i + 1;
//        int var0=i;
      for (k = 0; k < 100; k++) {
        
#pragma ACCEL pipeline_parallel factor=2
        a[var1][k] += b[var1][j + k];
//            a[var0][k] += b[var0][j+k];
      }
    }
  }
}

void func_top_47(int *a,int sum)
{
  int i;
  int j;
  int k;
  for (j = - 1; j < 100; j++) {
    for (k = - 1; k < 100; k++) {
      
#pragma ACCEL pipeline_parallel factor=2
      sum += a[100 * j + k];
      sum += a[100 * j + 100 + k];
      sum += a[100 * j + k + 1];
    }
  }
}
#define N_size -2

void func_top_48(int *a,int sum)
{
  int i;
  int j;
  int k;
  for (j = - 2; j < - 2 + 100; j++) {
    for (k = - 2; k < - 2 + 100; k++) {
      
#pragma ACCEL pipeline_parallel factor=2
      sum += a[100 * j + k];
      sum += a[100 * j + 100 + k];
      sum += a[100 * j + k + 1];
    }
  }
}

void func_top_49(int *a,int sum,int N)
{
  int i;
  int j;
  int k;
  for (j = N; j < N + 100; j++) {
    for (k = N; k < N + 100; k++) {
      
#pragma ACCEL pipeline_parallel factor=2
      sum += a[100 * j + k];
      sum += a[100 * j + 100 + k];
      sum += a[100 * j + k + 1];
    }
  }
}

void func_top_50(int *a,int sum,int N)
{
  int i;
  int j;
  int k;
  int i0;
  int i1;
  int i2;
  int i3;
  int i4;
  for (i1 = 0; i1 < 551; i1++) 
    for (i2 = 0; i2 < 551; i2++) 
      for (i3 = 0; i3 < 551; i3++) 
        for (i4 = 0; i4 < 551; i4++) 
          for (j = 0; j < 100; j++) {
            for (k = 0; k < 100; k++) {
              
#pragma ACCEL pipeline_parallel factor=2
              sum += a[100 * j + k];
              sum += a[100 * j + 100 + k];
              sum += a[100 * j + k + 1];
            }
          }
}

void func_top_51(int *a,int sum,int N)
{
  int i;
  int j;
  int k;
  int i0;
  int i1;
  int i2;
  int i3;
  int i4;
  for (j = 0; j < 100; j++) {
    for (k = 0; k < 100; k++) {
      for (i1 = 0; i1 < 551; i1++) 
        for (i2 = 0; i2 < 551; i2++) 
          for (i3 = 0; i3 < 551; i3++) 
            for (i4 = 0; i4 < 551; i4++) {
              
#pragma ACCEL pipeline_parallel factor=2
              sum += a[100 * j + k];
            }
      sum += a[100 * j + 100 + k];
      sum += a[100 * j + k + 1];
    }
  }
}

void func_top_52(int *sum,int a[100][100],int b[100])
{
  int i;
  int j;
  for (i = 0; i < 100; i++) {
    for (j = 0; j < 100; j++) {
      
#pragma ACCEL pipeline_parallel factor = 10
      a[i][j] = b[i];
    }
  }
  for (j = 0; j < 100; j++) {
    
#pragma ACCEL pipeline_parallel factor = 10
    sum[j] = b[i];
  }
}

void func_top_53(int a[100][100],int b[100])
{
  int i;
  int j;
  for (i = 0; i < 100; i++) {
    for (j = 0; j < 100; j++) {
      
#pragma ACCEL pipeline_parallel factor = 10
      a[i][j] = b[i];
    }
  }
  for (j = 0; j < 100; j++) {
  }
}
