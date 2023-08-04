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
#include<stdlib.h>
#define N 100

struct st 
{
  int a;
  char b;
}
;

void inc_a(char *m_b)
{
  int i;
  for (i = 0; i < 100; ++i) {
    m_b[i] = ((char )(((int )m_b[i]) + '0'));
  }
}

void inc_b(int *m_a)
{
  int i;
  for (i = 0; i < 100; ++i) {
    m_a[i] = m_a[i] + 10;
  }
}

void inc(int *m_a,char *m_b)
{
  inc_a(m_b);
  inc_b(m_a);
}

void dec(int *m_a,char *m_b)
{
  int i;
  for (i = 0; i < 100; ++i) {
    m_a[i] = m_a[i] - 9;
    m_b[i] = ((char )(((int )m_b[i]) - '1'));
  }
}
#pragma ACCEL kernel

void top(int *m_a,char *m_b)
{
  
#pragma ACCEL INTERFACE DEPTH=100 VARIABLE=m_b max_depth=100
  
#pragma ACCEL INTERFACE DEPTH=100 VARIABLE=m_a max_depth=100
  inc(m_a,m_b);
  dec(m_a,m_b);
  return ;
}

int main()
{
  char *m_b;
  int *m_a;
  
#pragma ACCEL wrapper VARIABLE=m->a port=m_a data_type=int
  
#pragma ACCEL wrapper VARIABLE=m->b port=m_b data_type=char
  
#pragma ACCEL wrapper VARIABLE=m port=m data_type="struct st"
  int i;
  struct st *m;
  m = ((struct st *)(malloc(sizeof(struct st ) * 100)));
  for (i = 0; i < 100; ++i) 
    m[i] . a = i;
  top(m_a,m_b);
  return 0;
}
