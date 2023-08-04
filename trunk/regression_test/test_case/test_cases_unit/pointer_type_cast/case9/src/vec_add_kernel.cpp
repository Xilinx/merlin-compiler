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


#define VEC_SIZE 10000

#pragma ACCEL kernel
void vec_add_kernel(int a[VEC_SIZE], int b[VEC_SIZE], int c[VEC_SIZE], int inc, short* arr, char* str)
{
    #pragma ACCEL interface variable = arr depth = 100
    int j;
    for (j = 0 ;j < VEC_SIZE; j++) c[j] = a[j]+b[j]+inc;
    short* pdst = arr + inc % 4;
    short* pdst1 = arr + inc % 5;
    short* pdst2 = arr + inc % 6;
    *(long*)pdst = *(long *)(&a[j]);
    *(double*)pdst1 = *(double *)(&b[j]);
    *(char*)pdst2 = *(char *)(&c[j]);    
    const int pdst3[2] = {1,2};
    int *pdst4  = (int *)pdst3;
    (*pdst4) += 1;
    unsigned int *pdst5=(unsigned int *)str;
    char **pdst6 = (char **)&b; 
    char dst7;
    int dst8 = *((int *)(&dst7));
}
