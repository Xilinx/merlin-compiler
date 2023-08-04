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

#define VEC_SIZE 64
#define VEC_flat_SIZE 4096

#define aa(p, n) (a[(p)*VEC_SIZE + (n)])
#define bb(p, n) (b[(p)*VEC_SIZE + (n)])
#define cc(p, n) (c[(p)*VEC_SIZE + (n)])
#define a_buf(p, n) (abuf[(p)*VEC_SIZE + (n)])
#pragma ACCEL kernel
void matmul_kernel(float * a, float * b, float * c)
{
    int i, j, k;
//    for (i = 0 ;i < VEC_SIZE; i++) {
//        for (j = 0 ;j < VEC_SIZE; j++) {
//             cc(i,j)=0;
//        }
//    }

    for (k = 0 ;k < VEC_SIZE; k++) {
        for (i = 0 ;i < VEC_SIZE; i++) {
#pragma ACCEL pipeline
            for (j = 0 ;j < VEC_SIZE; j++) {
#pragma ACCEL parallel complete
                 cc(i,j) += aa(i,k)*bb(k,j);
            }
        }
    }
}

int main() {
   float *a;
   float *b;
   float *c;
   matmul_kernel(a, b, c);
   return 0;


}
