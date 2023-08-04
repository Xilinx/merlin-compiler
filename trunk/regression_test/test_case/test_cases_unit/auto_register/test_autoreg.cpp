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


//#define FOCUS 0
#if !FOCUS

#pragma ACCEL kernel
void test5_5_cpp()  // insert offset variable - Initializer
{ 
  int test5_5_b[16][64];
  int (* test5_5_b1)[64](test5_5_b);
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test5_5_b[i];
  }
}


#else


#endif
