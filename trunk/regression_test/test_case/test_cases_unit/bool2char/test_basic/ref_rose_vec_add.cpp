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

void f(char *a,char b,const char c[10],const char *d,const char e[10][12],char *f[10],char (*g)[20])
{
}
typedef bool *bool_p;
const int s = 10;
#include<string.h>
#pragma ACCEL kernel

void top(char *a,const char &b,const char c[10],const char d[30],char ee[],char fa[10])
{
  char tmp;
  char tmp_2[100];
  char *tmp_3;
  char e[10][12];
  char *ff[10];
  char g[20];
  (bool )tmp;
  (bool )tmp_2[0];
  tmp_3;
  char d_buf[30];
  memcpy(d_buf,d,sizeof(bool ) * 30);
  for (char i = true; ((bool )i) != false; i = ((char )((bool )(1 - ((int )((bool )i)))))) {
  }
  f(a,(bool )b,c,&tmp,e,ff,&g);
  return ;
}
