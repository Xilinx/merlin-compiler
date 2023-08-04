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

struct st 
{
  int a;
  short b;
}
;

int index()
{
  static int i = 0;
  ++i;
  return i;
}
#pragma ACCEL kernel

void top(int *a_a,short *a_b)
{
  
#pragma ACCEL INTERFACE DEPTH=100 VARIABLE=a_b max_depth=100
  
#pragma ACCEL INTERFACE DEPTH=100 VARIABLE=a_a max_depth=100
  int i = 0;
  short tmp_b;
  int tmp_a;
  tmp_a = 0;
  tmp_b = ((short )1);
  while(i < 100){
    int rose_temp = i++;
    a_a[rose_temp] = tmp_a;
    a_b[rose_temp] = tmp_b;
  }
  i = 0;
  while(i < 100){
    int rose_temp_0 = index();
    a_a[rose_temp_0] = tmp_a;
    a_b[rose_temp_0] = tmp_b;
  }
  while(i < 100){
    int rose_temp_7 = i++;
    a_a[rose_temp_7]++;
    int rose_temp_6 = i++;
    a_b[rose_temp_6]++;
  }
  i = 0;
  while(i < 100){
    _Bool rose__temp = 1;
    int rose_temp_5 = i++;
    rose__temp = ((_Bool )(a_a[rose_temp_5] == tmp_a));
    if (rose__temp) 
      break; 
  }
  i = 0;
  while(1){
    int rose_temp_3;
    if (i < 100) {
      int rose_temp_4 = i++;
      rose_temp_3 = a_a[rose_temp_4] == tmp_a;
    }
     else {
      rose_temp_3 = 0;
    }
    _Bool rose__temp = (_Bool )rose_temp_3;
    if (!rose__temp) {
      break; 
    }
  }
{
    _Bool rose__temp = 1;
    do {{
      }
      rose_label__1:
{
      }
      int rose_temp_2 = i++;
      rose__temp = ((_Bool )(a_a[rose_temp_2] == ((int )tmp_b)));
    }while (rose__temp);
  }
  for (i = 0; 1; ) {
    int rose_temp_1 = i++;
    _Bool rose__temp = (_Bool )(((int )a_b[rose_temp_1]) != ((int )tmp_b));
    if (!rose__temp) {
      break; 
    }
{
    }
  }
}
