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
#define GAP_PENALTY -1
#define TRANSITION_PENALTY -2
#define TRANSVERSION_PENALTY -4
#define MATCH 2
//extern const char alignment_score_matrix[5][5];
#include <stdio.h>
#include <math.h>
signed char *string_1;
signed char *string_2;
enum Nucleotide {GAP=0,ADENINE=1,CYTOSINE=2,GUANINE=3,THYMINE=4} ;

signed char char_mapping(char c)
{
  signed char to_be_returned = (- 1);
  switch(c){
    case '_':
{
      to_be_returned = GAP;
      break; 
    }
    case 'A':
{
      to_be_returned = ADENINE;
      break; 
    }
    case 'C':
{
      to_be_returned = CYTOSINE;
      break; 
    }
    case 'G':
{
      to_be_returned = GUANINE;
      break; 
    }
    case 'T':
{
      to_be_returned = THYMINE;
      break; 
    }
  }
  return to_be_returned;
}
#define alignment_score_matrix(x,y) ((x==0||y==0) ? GAP_PENALTY : (x==y)? MATCH: TRANSITION_PENALTY)
#define VEC_SIZE 100

void smithwaterman_kernel(int *string_1,int *string_2,int *score)
{
  int string_2_1_0_buf[99];
  int string_1_1_0_buf[99];
  int i;
  int j;
  int score_buf[2][100];
  memcpy(score_buf[0],score,100 * 4);
  memcpy(&string_1_1_0_buf[0],&string_1[0],sizeof(int ) * 99);
  memcpy(&string_2_1_0_buf[0],&string_2[0],sizeof(int ) * 99);
  for (i = 1; i < 100; ++i) {
    memcpy(score_buf[i % 2],score + i * 100,100 * 4);
    for (j = 1; j < 100; ++j) {
      
#pragma ACCEL pipeline_parallel factor = 8
      signed char char_from_1 = (string_1_1_0_buf[j - 1] % 4 + 1);
      signed char char_from_2 = (string_2_1_0_buf[i - 1] % 4 + 1);
      int diag_score = score_buf[(i - 1) % 2][j - 1] + ((char_from_2 == 0 || char_from_1 == 0?- 1 : ((char_from_2 == char_from_1?2 : - 2))));
      int left_score = score_buf[i % 2][j - 1] + ((char_from_1 == 0 || GAP == 0?- 1 : ((char_from_1 == GAP?2 : - 2))));
      int top_score = score_buf[(i - 1) % 2][j] + ((GAP == 0 || char_from_2 == 0?- 1 : ((GAP == char_from_2?2 : - 2))));
      int bigger_of_left_top = left_score > top_score?left_score : top_score;
      score_buf[i % 2][j] = (bigger_of_left_top > diag_score?bigger_of_left_top : diag_score);
    }
    memcpy(score + i * 100,score_buf[i % 2],100 * 4);
  }
}
