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
#include "cmost.h"
void idct_kernel(int img,int mb_width,int mb_height,int *dequant_data,int *row2col_data,int *text_data);

int main()
{
  int mbx;
  int mby;
  int img;
  int blk;
  int mb_height = 18;
  int mb_width = 22;
  int IMG_NUM = 2;
#define TOTAL_MEM_SIZE 600000 //(3*352*288)
  int *row2col_data;
  cmost_malloc_1d((&row2col_data),"",4,600000);
  int *dequant_data;
  cmost_malloc_1d((&dequant_data),"+",4,600000);
  int *text_data;
  cmost_malloc_1d((&text_data),"0",4,600000);
  for (img = 0; img < 1; img++) {
    
    idct_kernel(img,mb_width,mb_height,dequant_data,row2col_data,text_data);
  }
  cmost_dump_1d(text_data,"text_out.dat");
  return 0;
}
