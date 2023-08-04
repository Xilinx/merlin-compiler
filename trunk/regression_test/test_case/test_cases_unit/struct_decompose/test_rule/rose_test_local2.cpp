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
const int N = 100;

class Test 
{
  private: int a;
  char b;
  

  public: inline void set_a(int n)
{
    (this) -> a = n;
  }
  

  inline void set_b(char c)
{
    (this) -> b = c;
  }
  

  inline int get_a()
{
    return (this) -> a;
  }
  

  inline int get_b()
{
    return ((this) -> b);
  }
}
;

#pragma ACCEL kernel
void top(class Test *a)
{
  int i;
  class Test b;
  b .  set_a (1);
  b .  set_b ('b');
  for (i = 0; i < N; ++i) {
    a[i] .  set_a ((a[i] .  get_a () + b .  get_a ()));
    a[i] .  set_b ((a[i] .  get_b () + b .  get_b ()));
  }
  return ;
}

int main()
{
  
#pragma ACCEL wrapper variable=a port=a
  int i;
  class Test *a = new Test [N];
  for (i = 0; i < N; ++i) 
    a[i] .  set_a (i);
  
  top(a);
  delete []a;
  return 0;
}
