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
#define SIMPLE 1
void OUT__1__7816__(void **__out_argv);
namespace N
{

class A
{
public: friend void ::OUT__1__7816__(void **__out_argv);

private: inline int foo() const
{
  return 7;
}

public: inline int biz() const
{
// A declaration for this pointer
  const class A *this__ptr__ = this;
// Build a reference to test the AST copy mechanism.
  (this) -> foo ();
  int result = 0;
  typedef class A *pointerToA;
{
    void *__out_argv1__1527__[2];
    __out_argv1__1527__[0] = ((void *)(&this__ptr__));
    __out_argv1__1527__[1] = ((void *)(&result));
    OUT__1__7816__(__out_argv1__1527__);
  }
  return result;
}
}
;
}

// I don't think the use of extern "C" is an issue.
// extern "C" void OUT__1__7816__(void **__out_argv)
#pragma ACCEL kernel name="OUT__1__7816__"

void OUT__1__7816__(void **__out_argv)
{
int &result = *((int *)__out_argv[1]);
const class N::A *&this__ptr__ = *((const class N::A **)__out_argv[0]);
for (int i = 1; i <= this__ptr__ -> foo (); i++)
result += i;
}
