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
class A
{
  public: typedef class A typedef_A;
          typedef int typedef_int;
}
;
#pragma ACCEL kernel name="foo"

void foo()
{
  // Name qualification required to reference types in another class.
  typedef A::typedef_A innerClass_A;
  typedef A::typedef_int innerClass_int;
  innerClass_A innerClassObject_A;
  innerClass_int innerClassObject_int;
}
