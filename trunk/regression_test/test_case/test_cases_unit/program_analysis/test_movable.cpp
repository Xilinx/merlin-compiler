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



void func1(int _a){}
void func2(int & _a){ _a = 1; }
void func3(int *_a){ *_a = 1;}

int c[10] = {0};

int main(int M)
{
    int a ;
    int b[10] = {0};
    int c;
    int * pos;
    a = 0;
    a+123;

    // expressions
    {
        func2(a);
        a = 0;
        a;
        a+M;
        b[0];
        func1(a);
    }

    //positions
    pos[0];
    a = c;
    pos[1];
    a = b[0];
    pos[2];
    a++;
    pos[3];
    b[1] = b[2];
    pos[4];
    b[a] = b[2];
    pos[41];
    func3(b);
    pos[42];
    func1(a);
    pos[5];
    func2(a);
    pos[6];
    func3(&a);
    pos[7];
}
