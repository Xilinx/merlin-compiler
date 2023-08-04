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
#include "math.h"

#define SIZE_I 21250
#define SIZE_J 2

void vec_multi_kernel(int *a, int *b, int*c, int *d)
{
    int i,j,k;
    int a_buf[SIZE_I];
    int b_buf[SIZE_I];
    int c_buf[SIZE_I];
    int d_buf[SIZE_I];
    int tmp1,tmp2;

    for (j = 0 ;j < SIZE_J; j++)
    {
        memcpy(a_buf, a+SIZE_I*j, SIZE_I*sizeof(int)); 
        memcpy(b_buf, b+SIZE_I*j, SIZE_I*sizeof(int)); 
        for (i = 0 ;i < SIZE_I; i++)
        {
            #pragma cmost pipeline_parallel factor=425
            c_buf[i] = a_buf[i]*b_buf[i];
            tmp1 = a_buf[i]+b_buf[i];
            if((tmp1 == 1)||(tmp1 == 11)||(tmp1 == 111)||(tmp1 == 1111)||(tmp1 == 11111)||(tmp1 == 111111))
                tmp2 = tmp1 + 1;
            else if((tmp1 == 2)||(tmp1 == 22)||(tmp1 == 222)||(tmp1 == 2222)||(tmp1 == 22222)||(tmp1 == 222222))
                tmp2 = tmp1 + 2;
            else if((tmp1 == 3)||(tmp1 == 33)||(tmp1 == 333)||(tmp1 == 3333)||(tmp1 == 33333)||(tmp1 == 333333))
                tmp2 = tmp1 + 3;
            else if((tmp1 == 4)||(tmp1 == 44)||(tmp1 == 444)||(tmp1 == 4444)||(tmp1 == 44444)||(tmp1 == 444444))
                tmp2 = tmp1 + 4;
            else if((tmp1 == 5)||(tmp1 == 55)||(tmp1 == 555)||(tmp1 == 5555)||(tmp1 == 55555)||(tmp1 == 555555))
                tmp2 = tmp1 + 5;
            else if((tmp1 == 6)||(tmp1 == 66)||(tmp1 == 666)||(tmp1 == 6666)||(tmp1 == 66666)||(tmp1 == 666666))
                tmp2 = tmp1 + 6;
            else if((tmp1 == 7)||(tmp1 == 77)||(tmp1 == 777)||(tmp1 == 7777)||(tmp1 == 77777)||(tmp1 == 777777))
                tmp2 = tmp1 + 7;
            else if((tmp1 == 8)||(tmp1 == 88)||(tmp1 == 888)||(tmp1 == 8888)||(tmp1 == 88888)||(tmp1 == 888888))
                tmp2 = tmp1 + 8;
            else
                tmp2 = tmp1 + 9;
            d_buf[i] = tmp2 + tmp1;
        }
        memcpy(c+SIZE_I*j, c_buf, SIZE_I*sizeof(int)); 
        memcpy(d+SIZE_I*j, d_buf, SIZE_I*sizeof(int)); 
    }
}
