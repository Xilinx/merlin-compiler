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



void sub0(int * b)
{
  int a = *b;
}

#pragma ACCEL kernel
void test0(int * a)
{
  sub0(a); // no inline
}

#pragma ACCEL inline
void sub1(int * b)
{
  int a = *b;
}

#pragma ACCEL kernel
void test1(int * a)
{
  sub1(a);
}

#pragma ACCEL inline
void sub1_1(int * b)
{
  int a = *b;
}

void test1_1(int * a)
{
  sub1_1(a);
}

int sub2(int * b)
{
  int a = *b;
  return 0;
}

#pragma ACCEL kernel
void test2(int * a)
{
  sub2(a);

#pragma ACCEL inline
  a + sub2(a+1);
}

#pragma ACCEL inline
int sub3(int * b)
{
  int a = *b;
  return 0;
}

#pragma ACCEL kernel
void test3(int * a)
{
  sub3(a);
  a + sub3(a+1);
}

int sub4(int * b)
{
  int a = *b;
  return 0;
}

#pragma ACCEL kernel
void test4(int * a)
{
  sub4(a);
  a + sub4(a+1);
#pragma ACCEL inline
  sub4(a-1);
}

#pragma ACCEL inline
int sub5(int * b)
{
  int a = *b;
  return 0;
}

#pragma ACCEL kernel
void test5(int * a)
{
  sub5(a);
  a + sub5(a+1);
#pragma ACCEL inline
  sub5(a-1);
}

int sub6(int * b)
{
  int a = *b;
  sub6_1(a);
  return 0;
}

#pragma ACCEL inline
int sub6_1(int * b)
{
  int a = *b;
  return 0;
}

#pragma ACCEL kernel
void test6(int * a)
{
  sub6_1(a-1);
  sub6(a);
}


void test7(int * a);

#pragma ACCEL inline
int sub7_s(int * b)
{
  int a = *b;
  test7(b);
  return 0;
}

int sub7(int * b)
{
  int a = *b;
  sub7_s(b);
  return 0;
}

#pragma ACCEL kernel
void test7(int * a)
{
  sub7(a);
}


#pragma ACCEL INLINE 
int sub7_s1(int * b)
{
  int a = *b;
  return 0;
}

#pragma ACCEL inline
int sub7_1(int * b)
{
  int a = *b;
  sub7_s1(b+1);
  return 0;
}

#pragma ACCEL inline
int sub7_s2(int * b)
{
  int a = *b;
  sub7_s1(b+2);
  return 0;
}

#pragma ACCEL kernel
void test7_1(int * a)
{
  sub7_1(a);
  sub7_s1(a);
  sub7_s2(a);
}


#pragma ACCEL INLINE 
int sub8(int * b)
{
  int a = *b;
  if (a)
    return a;
  else if (b)
    return 1;

  return 2;
}

#pragma ACCEL kernel
void test8(int * a)
{
  int c = sub8(a-1);
}

