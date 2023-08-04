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
struct st {
  int a;
  char b;
};
union ut {
  int a;
  double b;
};
struct st3 {
  int *a;
  int *b;
};
struct st4 {
  int a;
  int b;
  struct {
    int c;
    int d;
  };
  struct st5 {
    float e;
    float f;
  };
};
typedef struct st2 {
  int a;
} st2;
typedef int int_t;
enum et { P = 0 };
int g;
#pragma ACCEL kernel
void test(double b[10], int j) {
  int a[100];
  int aa[100][50];
  int bb[] = {0, 1, 2, 3};
  int i;
  int *p;
  sizeof(p);
  sizeof(union ut);
  sizeof(struct st);
  sizeof(enum et);
  sizeof(st2);
  sizeof(int [i]);
  sizeof(int [j]);
  sizeof(int [g]);
  sizeof(struct st [i]);
  sizeof(union ut [10]);
  sizeof(enum et [10]);
  sizeof (int (*)[100]);
  sizeof(int_t);
  sizeof(const char);
  sizeof(struct st3);
  sizeof(struct st4);
  for (i = 0; i < sizeof(a) / sizeof(int); ++i) {}
  for (i = 0; i < sizeof(aa) / sizeof(int); ++i) {}
  for (i = 0; i < sizeof(b) / sizeof(double); ++i) {}
  for (i = 0; i < sizeof(bb) / sizeof(bb[0]); ++i) {}
}

