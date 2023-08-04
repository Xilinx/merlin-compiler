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
void top(char *a,const char &b,const char c[10],const char d[30],char ee[],char fa[10]);

void __merlinwrapper_top(bool *a,const bool &b,const bool c[10],const bool d[30],bool e[],bool f[10])
{
  
#pragma ACCEL wrapper variable=a port=a data_type=char
  char *m_a;
  char m_b;
  char m_c[10];
  char m_d[30];
  char m_e[10];
  char m_f[10];
  top(m_a,(bool )m_b,m_c,m_d,m_e,m_f);
}
