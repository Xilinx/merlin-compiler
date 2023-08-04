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
// Testing multi-level loops

#pragma ACCEL kernel
void func_loop() {
  int a[10];
  int b[10];
  int c[10];

  int i = 0;
  int j = 0;
  int k = 0;
  int x, y, z;
  int N = 100;

  for (i = N, j = 3; i >= 0; i--)
    for (j = N; i >= 0; i--) {
      /* block 1*/
      c[i] = a[i] + b[i];
    }

  for (i = N, j = 3; i >= 0; i--)
    for (j = N; i >= 0; i--)
      /* block 2*/
      c[i] = a[i] + b[i];

  int i0, i1, i2, i3, i4;
  for (i0 = N, j = 0; i0 >= 0; i0--) {
    for (i1 = N, j = 1; i1 >= 0; i1--) {
      for (i2 = N, j = 2; i2 >= 0; i2--) {
        for (i3 = N, j = 3; i3 >= 0; i3--) {
          for (i4 = N, j = 4; i4 >= 0; i4--) {
            /* block 2*/
            c[i0 + i1 + i2 + i3 + i4] =
                a[i0 + i1 + i2 + i3 + i4] + b[i0 + i1 + i2 + i3 + i4];
          }
        }
      }
    }
  }

  for (int i0 = N, j = 0; i0 >= 0; i0--) {
    for (int i1 = N, j = 1; i1 >= 0; i1--) {
      for (int i2 = N, j = 2; i2 >= 0; i2--) {
        for (int i3 = N, j = 3; i3 >= 0; i3--) {
          for (int i4 = N, j = 4; i4 >= 0; i4--) {
            /* block 2.1*/
            c[j + i0 + i1 + i2 + i3 + i4] =
                a[i0 + i1 + i2 + i3 + i4] + b[i0 + i1 + i2 + i3 + i4];
          }
        }
      }
    }
  }

  for (int i0 = N, j0 = 0; i0 >= 0; i0--) {
    for (int i1 = N, j1 = 1; i1 >= 0; i1--) {
      for (int i2 = N, j2 = 2; i2 >= 0; i2--) {
        for (int i3 = N, j3 = 3; i3 >= 0; i3--) {
          for (int i4 = N, j4 = 4; i4 >= 0; i4--) {
            /* block 2.2*/
            c[j0 + i0 + i1 + i2 + i3 + i4] =
                a[j3 + i0 + i1 + i2 + i3 + i4] + b[i0 + i1 + i2 + i3 + i4];
          }
        }
      }
    }
  }

  for (i = N; i >= 0; i--) {
    j = i;
    for (; j < 10; j++) {
      /* block 3*/
      c[i] = a[j] + b[j];
    }
  }

  for (i = N; i >= 0; i--) {
    for (j = i; j < 100; j += 2)
      if (j < 10) {
        for (k = 3; k < 10; k++) {
          /* block 4*/
          c[i] = a[k] + b[j];
        }
      }
  }

  for (i = N; i >= 0; i--) {
    for (j = i0; j < 100; j += 2)
      if (j < 10) {
        for (k = 3; k < 10; k++) {
          /* block 5*/
          c[i] = a[k] + b[j];
        }
      }
    break;
  }

  for (i = N; i >= 0; i--) {
    for (j = i, k = 3; j < 100; j += 2)
      if (j < 10) {
        for (; k < 10; k++) {
          c[i] = a[k] + b[j];
        }
      }
  }

  for (i = N; i >= 0; i--) {
  loop1:
    for (j = i; j < 100; j += 2)
      if (j < 10) {
        for (k = 3; k < 10; k++) {
          c[i] = a[k] + b[j];
        }
      }
    if (c[0] == 1)
      goto loop1;
  }
}

int main() {

  func_loop();

  return 0;
}
