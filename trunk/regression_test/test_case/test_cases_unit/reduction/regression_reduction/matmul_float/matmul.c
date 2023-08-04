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
/*Step1: loop exchange*/


//#include "cmost.h"
//#include <stdio.h>

#define VEC_SIZE 512 
#define VEC_flat_SIZE (512*512) 

void matmul_kernel(float * a, float * b, float * c);

int main()
{   
    float * a; cmost_malloc( &a, 4*VEC_flat_SIZE);
    float * b; cmost_malloc( &b, 4*VEC_flat_SIZE);
    float * c; cmost_malloc( &c, 4*VEC_flat_SIZE);
    
    cmost_load_file(a, "float_i.dat" , VEC_flat_SIZE * 4);
    cmost_load_file(b, "float_i2.dat", VEC_flat_SIZE * 4);
    cmost_load_file(c, "0"           , VEC_flat_SIZE * 4);

    if (!(a && b && c)) while(1);


    matmul_kernel(a,b,c);

    cmost_write_file(c, "c_out.dat"  , VEC_flat_SIZE * 4);

    cmost_dump(c, "out.dat");
    cmost_free(a);
    cmost_free(b);
    cmost_free(c);

    return 0;
}

