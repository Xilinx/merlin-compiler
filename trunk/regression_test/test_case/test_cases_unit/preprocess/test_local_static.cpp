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
void sub(int *a) {
  static int i = 0;
  a[i++] = 2;
}

#pragma ACCEL kernel
void top(int *a) {
  {
    static int i = 0;
    a[++i] = 0;
  }
  {
    static int i = 100;
    a[--i] = 1;
  }
  sub(a);
}

#pragma ACCEL kernel
void top2(int *a) {
    static int s0 = 1;
    static int s1 = s0 + 1;
    static int s2 = s1 + s0;

    int dim = s0 + s1 + s2;
}

int sub3() {
    return 10;
}

#pragma ACCEL kernel
void top3(int *a) {
    static int s0 = sub3();

    int dim = s0 * 2;
}


#pragma ACCEL kernel
void top4(int *a) {
    int b = 10;
    static int s0 = 1;
    static int s1 = s0 + b;

    int dim = s0 * 2 + s1 * 2;
}

#pragma ACCEL kernel
void top5(int *a) {
    int b = *a;
    int c = *(a + 1);
    static int s0 = b;
    static int s1 = s0 + c;

    int dim = s0 * 2 + s1 * 2;
}
