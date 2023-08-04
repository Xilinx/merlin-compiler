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

struct ::Foo_Inner 
{
}
;

struct ::Foo 
{
  int x;
}
;

struct Foo p_ZN3Foo5Inner1fEv(struct Foo_Inner *this_)
{
  struct Foo temp;
  return temp;
}
static void __merlin_dummy_SgClassDeclaration_struct_Foo_();

void id()
{
}
static void __merlin_dummy_kernel_pragma();

void g()
{
  struct Foo foo;
  id();
}
