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
#include <string.h> 
#include "__merlinheadwrapper_idct_kernel.h"
void idct_kernel(int *dequant_data,int *row2col_data,int *text_data);
/*       int img,
        int mb_width,
        int mb_height,*/

void __merlinwrapper_idct_kernel(
/*       int img,
        int mb_width,
        int mb_height,*/
int *dequant_data,int *row2col_data,int *text_data)
{
  
#pragma ACCEL wrapper variable=dequant_data port=dequant_data depth=600000 max_depth=600000 data_type=int io=RO copy=false
  
#pragma ACCEL wrapper variable=row2col_data port=row2col_data depth=600000 max_depth=600000 data_type=int io=NO copy=false
  
#pragma ACCEL wrapper variable=text_data port=text_data depth=600000 max_depth=600000 data_type=int io=RW copy=false
  int __m_dequant_data[600000];
  int __m_row2col_data[600000];
  int __m_text_data[600000];
{
    memcpy((__m_dequant_data + 0),dequant_data,600000 * sizeof(int ));
  }
{
    memcpy((__m_text_data + 0),text_data,600000 * sizeof(int ));
  }
  
#pragma ACCEL kernel
  idct_kernel(__m_dequant_data,__m_row2col_data,__m_text_data);
{
    memcpy(text_data,(__m_text_data + 0),600000 * sizeof(int ));
  }
}
