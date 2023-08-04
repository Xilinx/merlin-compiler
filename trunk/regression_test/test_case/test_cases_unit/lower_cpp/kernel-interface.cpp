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
#include<hls_stream.h>
class globalID {
  public:
    friend class globalID operator+(const int &,const class globalID &);

    inline globalID(int i) : m_gid(i) { }
    inline int operator<(const class globalID &g) const {
      return m_gid < g.m_gid;
    }
  private:
    int m_gid;
};

#pragma ACCEL kernel
void k(globalID &g)
{
  if (g < globalID(1)) {
  }
}

#pragma ACCEL kernel
void k2(hls::stream<globalID> &out)
{
  globalID tmp(0);
  out.write(tmp);
}

#pragma ACCEL kernel
void k3(globalID *out)
{
  globalID tmp(0);
  *out = tmp;
}
