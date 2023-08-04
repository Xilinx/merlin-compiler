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
#pragma ACCEL kernel
void test(int * argv, int n)
{
    double a[1024][1024][1024];
    int i,j,k,l,m;
    m = argv[1];

    {
        a;
    }

    for (j = m; j < n; j++)
    {
        for (i = 1; i < 33; i++)
        {
#pragma ACCEL pipeline
            for (k = 0; k < 1024; k++)
            {
                m + n;
                a[i+m+6][n][j+n+1];    
            }
        }
    }
    for (int j = m; j < n; j++)
    {
        for (int i = 1; i < 33; i++)
        {
#pragma ACCEL pipeline
        }
    }
    //return 1;
}

int main(int *arg, int n) {
  test(arg, n);
  return 0;
}


