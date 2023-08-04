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
typedef struct st2 {
int b[4];
int c[5][6];
int d[7][8][9];}st2;
typedef struct st {
st2 a[1][2][3];}st;
#pragma ACCEL kernel

void top(int (*a_a_b)[1][2][3][4],int (*a_a_c)[1][2][3][5][6],int (*a_a_d)[1][2][3][7][8][9])
{
  
#pragma ACCEL INTERFACE DEPTH=1,1,2,3,7,8,9 VARIABLE=a_a_d max_depth=1,1,2,3,7,8,9
  
#pragma ACCEL INTERFACE DEPTH=1,1,2,3,5,6 VARIABLE=a_a_c max_depth=1,1,2,3,5,6
  
#pragma ACCEL INTERFACE DEPTH=1,1,2,3,4 VARIABLE=a_a_b max_depth=1,1,2,3,4
  int i_3_2_0;
  int i_3_1_0;
  int i_3_0_0;
  int i_2_1_0;
  int i_2_0_0;
  int i_1_0_2;
  int i_1_2_0;
  int i_1_1_0;
  int i_1_0_1;
  int i_3_2;
  int i_3_1;
  int i_3_0;
  int i_2_1;
  int i_2_0;
  int i_1_0_0;
  int i_1_2;
  int i_1_1;
  int i_1_0;
  int tmp_a_d[1][2][3][7][8][9];
  int tmp_a_c[1][2][3][5][6];
  int tmp_a_b[1][2][3][4];
  for (i_1_2 = 0; i_1_2 < 1UL; ++i_1_2) {
    for (i_1_1 = 0; i_1_1 < 2UL; ++i_1_1) {
      for (i_1_0 = 0; i_1_0 < 3UL; ++i_1_0) {
        for (i_1_0_0 = 0; i_1_0_0 < 4UL; ++i_1_0_0) {
          tmp_a_b[i_1_2][i_1_1][i_1_0][i_1_0_0] = a_a_b[0][i_1_2][i_1_1][i_1_0][i_1_0_0];
        }
        for (i_2_1 = 0; i_2_1 < 5UL; ++i_2_1) {
          for (i_2_0 = 0; i_2_0 < 6UL; ++i_2_0) {
            tmp_a_c[i_1_2][i_1_1][i_1_0][i_2_1][i_2_0] = a_a_c[0][i_1_2][i_1_1][i_1_0][i_2_1][i_2_0];
          }
        }
        for (i_3_2 = 0; i_3_2 < 7UL; ++i_3_2) {
          for (i_3_1 = 0; i_3_1 < 8UL; ++i_3_1) {
            for (i_3_0 = 0; i_3_0 < 9UL; ++i_3_0) {
              tmp_a_d[i_1_2][i_1_1][i_1_0][i_3_2][i_3_1][i_3_0] = a_a_d[0][i_1_2][i_1_1][i_1_0][i_3_2][i_3_1][i_3_0];
            }
          }
        }
      }
    }
  }
  for (i_1_2_0 = 0; i_1_2_0 < 1UL; ++i_1_2_0) {
    for (i_1_1_0 = 0; i_1_1_0 < 2UL; ++i_1_1_0) {
      for (i_1_0_1 = 0; i_1_0_1 < 3UL; ++i_1_0_1) {
        for (i_1_0_2 = 0; i_1_0_2 < 4UL; ++i_1_0_2) {
          a_a_b[1][i_1_2_0][i_1_1_0][i_1_0_1][i_1_0_2] = tmp_a_b[i_1_2_0][i_1_1_0][i_1_0_1][i_1_0_2];
        }
        for (i_2_1_0 = 0; i_2_1_0 < 5UL; ++i_2_1_0) {
          for (i_2_0_0 = 0; i_2_0_0 < 6UL; ++i_2_0_0) {
            a_a_c[1][i_1_2_0][i_1_1_0][i_1_0_1][i_2_1_0][i_2_0_0] = tmp_a_c[i_1_2_0][i_1_1_0][i_1_0_1][i_2_1_0][i_2_0_0];
          }
        }
        for (i_3_2_0 = 0; i_3_2_0 < 7UL; ++i_3_2_0) {
          for (i_3_1_0 = 0; i_3_1_0 < 8UL; ++i_3_1_0) {
            for (i_3_0_0 = 0; i_3_0_0 < 9UL; ++i_3_0_0) {
              a_a_d[1][i_1_2_0][i_1_1_0][i_1_0_1][i_3_2_0][i_3_1_0][i_3_0_0] = tmp_a_d[i_1_2_0][i_1_1_0][i_1_0_1][i_3_2_0][i_3_1_0][i_3_0_0];
            }
          }
        }
      }
    }
  }
}
