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
void func_top_0() {
  int a[100][100];
  int b[100][100];
  int k;
  int i;
  int j;
  for (i = 0; i < 100; i++) {
    int *A = a[i];
    for (k = 0; k < 100; k++) {
      int *B = b[k];
      for (j = 0; j < 99; j++) {
#pragma ACCEL parallel complete
        A[j] = B[j + 1] + B[j];
      }
    }
  }
}

#pragma ACCEL kernel
void func_top_0_1(int b[100][100]) {
  int *A;
  int k;
  int i;
  int j;
  for (i = 0; i < 100; i++) {
    for (k = 0; k < 100; k++) {
      int *B = b[k];
      for (j = 0; j < 99; j++) {
#pragma ACCEL parallel complete
        A[j] = B[j + 1] + B[j];
      }
    }
  }
}

static void mm_store_si11(unsigned char (*dst_data)[16],unsigned char a_data[16])
{
  for (int i = 0; i < 16; i++) {

#pragma ACCEL PARALLEL COMPLETE
    ( *dst_data)[i] = a_data[i];
  }
}
static void mm_store_si12(unsigned char (*dst_data)[16],unsigned char a_data[16])
{
  for (int i = 0; i < 16; i++) {

#pragma ACCEL PARALLEL COMPLETE
  }
}

static void mm_store_si13(unsigned char (*dst_data)[16],unsigned char a_data[16])
{
  for (int j = 0; j < 3; j++) {
    for (int i = 0; i < 16; i++) {

#pragma ACCEL PARALLEL COMPLETE
      dst_data[j][i] = a_data[i];
    }
  }
}

static void mm_store_si14(unsigned char (*dst_data)[16],unsigned char a_data[16])
{
  unsigned char *dst_data1 = &dst_data[5][8];
    for (int i = 0; i < 8; i++) {

#pragma ACCEL PARALLEL COMPLETE
      dst_data1[i] = a_data[i];
    }
}

static void mm_store_si15(unsigned char (*dst_data)[16],unsigned char a_data[16])
{
  unsigned char *dst_data2 = (*dst_data);
    for (int i = 0; i < 8; i++) {

#pragma ACCEL PARALLEL COMPLETE
      dst_data2[i] = a_data[i];
    }
}


//test for MER-393
#pragma ACCEL kernel
void func_top_1() {
  unsigned char vScore1_data[10][6][16];
  unsigned char vScore2_data[10][6][16];
  unsigned char vScore3_data[10][60][16];
  unsigned char vScore4_data[10][6][16];
  unsigned char vScore5_data[10][6][16];
  unsigned char* vScore6_data[10][6][16];
  unsigned char profbuf_data[10][36][16];
  unsigned char vf_n_data[3][16];
  unsigned char vf_n_data1[4][16];
  const int rff_ = 1035;

  for (unsigned long i = 0; i < rff_; i++) {
    int ping_pong = (int )(i % ((unsigned long )2));
    for (int k = 0; k < 10; k++) {
#pragma ACCEL PIPELINE

      for (int j = 0; j < 3; j++) {
#pragma ACCEL PARALLEL COMPLETE
        mm_store_si11(&vScore1_data[k][3 * (1 - ping_pong) + j],vf_n_data[j]);
      }
      for (int j = 0; j < 3; j++) {
#pragma ACCEL PARALLEL COMPLETE
        mm_store_si12(&vScore2_data[k][3 * (1 - ping_pong) + j],vf_n_data[j]);
      }
      for (int j = 0; j < 3; j++) {
#pragma ACCEL PARALLEL COMPLETE
        mm_store_si13(&vScore3_data[k][3 * (1 - ping_pong) + j],vf_n_data[j]);
      }
      for (int j = 0; j < 3; j++) {
#pragma ACCEL PARALLEL COMPLETE
        mm_store_si14(&vScore4_data[k][3 * (1 - ping_pong) + j],vf_n_data[j]);
      }

      for (int j = 0; j < 4; j++) {
#pragma ACCEL PARALLEL COMPLETE
        mm_store_si15(&vScore5_data[k][3 * (1 - ping_pong) + j],vf_n_data1[j]);
      }
      for (int j = 0; j < 3; j++) {
#pragma ACCEL PARALLEL COMPLETE
        mm_store_si11(&profbuf_data[k][j * ((unsigned long )2) + ((unsigned long )0)],vf_n_data[j]);
        mm_store_si11(&profbuf_data[k][j * ((unsigned long )2) + ((unsigned long )1)],vf_n_data[j]);
      }

      for (int j = 0; j < 3; j++) {
#pragma ACCEL PARALLEL COMPLETE
          *vScore6_data[k][j][0];
      }
    }
  }
}


