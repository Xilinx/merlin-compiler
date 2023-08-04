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
template<int I>
int sub(int c) {
#pragma hls tripcount max=I
  return c;
}

class util {
  public:
template<int I>
int sub2(int c) {
#pragma hls tripcount max=I
  return c;
}
};

template<int I> 
class tc {
  public:
    template<int II>
    int sub3(int c) {
#pragma HLS tripcount max=I+II
      return c;
    }
};

#pragma ACCEL kernel
int top(int c) {
  util tmp;
  tc<30> tmp2;
  return sub<10>(c) + tmp.sub2<20>(c) + tmp2.sub3<40>(c);
}
