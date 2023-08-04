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

int func(int *a, int aa)
{
    return *a + aa;
}

void func2(int a[100]) {
}
int g_asdf = 0;

int main()
{
    int c[10]={0};
    int a = 0+c[2];
    int b = a+2;
    int aa[100];
    int cc[100];
    func2(aa);
    func(&cc[0], b);
    a = 1+g_asdf;

    for (int i = 0; i < 100; i++) 
    {
        int d; //test case for bug155
        c[i] =a;         
        a = 2;
        i;
    }
    g_asdf = 12;

    a = 3;
    func(&b, a);

    return c[0];
}
