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
template<typename T, int size>
class Array {
    T m_arr[size];
public:
    T& operator[](int i) { return m_arr[i]; }
    int getSize() { return size; }
};

template<int size>
class Array<char, size> {
    char m_arr[size+1];
public:
    char& operator[](int i) { return m_arr[i]; }
    int getSize();
};

template<int size>
int Array<char, size>::getSize()
{
    return size+1;
}

template<int size>
void print_char_array(Array<char, size> &arr)
{
}

#pragma ACCEL kernel
void k()
{
    Array<char, 5> ac5;
    Array<char, 6> ac6;
    print_char_array(ac5);
    print_char_array(ac6);
}
