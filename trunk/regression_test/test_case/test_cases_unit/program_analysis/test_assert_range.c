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
#include<assert.h>
#include<stdlib.h>

void func(int M, int N, int K, unsigned char uc, unsigned short us, unsigned int ui, unsigned long ul, /*unsigned long long ull,*/ size_t ust)
{
    assert(N > 100);
    assert(N < 200);
    assert(uc < 100);
    assert(us < 100);
    assert(ui < 100);
    assert(ul < 100);
    //assert(ull < 100);
    assert(ust < 100);
    int a[100];
    int i, j;
    for(i = 0; i < uc; ++i) {i;}
    for(i = 0; i < us; ++i) {i;}
    for(i = 0; i < ui; ++i) {i;}
    for(i = 0; i < ul; ++i) {i;}
    //for(i = 0; i < ull; ++i) {i;}
    for(i = 0; i < ust; ++i) {i;}
    for(i = 0; i < N; ++i)
      for (j = i; j < 2 * i; ++j)
        ;

    for (i = 0, j=23; i <= N; i++)
    for (j = -50; j < 1; j++)
    {
        a[i] = i;
        i;
        i+j;
        i*j;
        2*i - 2*j;
        -i+j;
    }
    0;
    0;
    assert(K > 50 && K < 100);
    for (i = 100; i <= K; i++)
    for (j = 100; j <= K + 2 * i; j++)
    {
        a[i] = i;
        i;
        i+j;
        i*j;
        2*i - 2*j;
        -i+j;
    }
    K = a[1];
    assert(K < 200);
    M = a[0];
    assert(M > 50);
    for (i = M ; i < K; i++)
    {
        i+1;
    }
    if (M) {
      K = a[2];
    } else
      K = a[3];
    assert(K > 10 && K < 30);
    for (i = 0; i < K; i++) {
      i + 3;
      i * 2;
    }
    K = a[4];
    assert(K > 20/2 + 10 && K < 30);
    for (i = 0; i < K; i++) {
      i + 3;
      i * 2;
    }

    {
        int b;
        assert(b <= 1024);
        b+1;
        b/2;
    }
    {
        int c;
        assert(c <= 5);
        int d;
        assert(d >= -5);
        int e;
        assert(e <= -5);
        c+1;
        c/2;
        c/-2;
        d/2;
        d/-2;
        e/2;
        e/-2;
    }
    {
      //bug1676
      int bb;
      assert(bb == 100);
      bb + 1;
    }


}

void f() {
  (char) 88;
  (signed char) 88;
  (short) 88;
  (int) 88;
  (long) 88;
  //(long long) 88;

  (char) 0x1ffffffffLL;
  (signed char) 0x1ffffffffLL;
  (short) 0x1ffffffffLL;
  (int) 0x1ffffffffLL;
  (long) 0x1ffffffffLL;
  //(long long) 0x1ffffffffLL;

  (char) -1;
  (signed char) -1;
  (short) -1;
  (int) -1;
  (long) -1;
  //(long long) -1;

  (unsigned char) 88;
  (unsigned short) 88;
  (unsigned int) 88;
  (unsigned long) 88;
  //(unsigned long long) 88;

  (unsigned char) 0x1ffffffffLL;
  (unsigned short) 0x1ffffffffLL;
  (unsigned int) 0x1ffffffffLL;
  (unsigned long) 0x1ffffffffLL;
  //(unsigned long long) 0x1ffffffffLL;

  (unsigned char) -1;
  (unsigned short) -1;
  (unsigned int) -1;
  (unsigned long) -1;
  //(unsigned long long) -1;
}
#if 0
void g() {
  long long x8lOut, x8uOut, x8In;
  long long x16lOut, x16uOut, x16In;
  long long x32lOut, x32uOut, x32In;
  long long x64lOut, x64uOut, x64In;

  unsigned long long ux8uOut, ux8In;
  unsigned long long ux16uOut, ux16In;
  unsigned long long ux32uOut, ux32In;
  unsigned long long ux64In;

  long long ux8lOut;
  long long ux16lOut;
  long long ux32lOut;
  long long ux64lOut;

  assert(x8In >= -128 && x8In <= 127);
  assert(x8lOut >= -129 && x8lOut <= 127);
  assert(x8uOut >= -128 && x8uOut <= 128);
  assert(ux8In <= 255);
  assert(ux8lOut >= -1 && ux8lOut <= 255);
  assert(ux8uOut <= 256);

  (signed char) x8In;
  (signed char) x8lOut;
  (signed char) x8uOut;
  (unsigned char) ux8In;
  (unsigned char) ux8lOut;
  (unsigned char) ux8uOut;

  assert(x16In >= -32768 && x16In <= 32767);
  assert(x16lOut >= -32769 && x16lOut <= 32767);
  assert(x16uOut >= -32768 && x16uOut <= 32768);
  assert(ux16In <= 65535);
  assert(ux16lOut >= -1 && ux16lOut <= 65535);
  assert(ux16uOut <= 65536);

  (signed short) x16In;
  (signed short) x16lOut;
  (signed short) x16uOut;
  (unsigned short) ux16In;
  (unsigned short) ux16lOut;
  (unsigned short) ux16uOut;

  assert(x32In >= -2147483648L && x32In <= 2147483647L);
  assert(x32lOut >= -2147483649L && x32lOut <= 2147483647L);
  assert(x32uOut >= -2147483648L && x32uOut <= 2147483648L);
  assert(ux32In <= 4294967295L);
  assert(ux32lOut >= -1 && ux32lOut <= 4294967295L);
  assert(ux32uOut <= 4294967296L);

  (signed int) x32In;
  (signed int) x32lOut;
  (signed int) x32uOut;
  (unsigned int) ux32In;
  (unsigned int) ux32lOut;
  (unsigned int) ux32uOut;

  assert(x64In >= -9223372036854775807L && x64In <= 9223372036854775806L);
  (signed long) x64In;

}
#endif
void f1() {
  int i;
  int x;

  for (i = -1; i <= -1; i++) {
    i;
  }
}

void f2() {
  int i;
  int x;

  for (i = -1; i <= -1; ++i) {
    i;
  }
}

void f3() {
  int i;
  int x;

  for (i = -1; i >= -1; i--) {
    i;
  }
}

void f4() {
  int i;
  int x;

  for (i = -1; i >= -1; --i) {
    i;
  }
}

void f5() {
  int i;
  int x;

  for (i = -1; i >= -1; i += -1) {
    i;
  }
}

void f6() {
  int i;
  int x;

  for (i = -1; i <= -1; i += 1) {
    i;
  }
}

void f7() {
  int i;
  int x;

  for (i = -1; i <= -1; i -= -1) {
    i;
  }
}

void f8() {
  int i;
  int x;

  for (i = -1; i >= -1; i -= 1) {
    i;
  }
}

void f9() {
  int i;
  int x;

  for (i = -1; i >= -1; i = i - 1) {
    i;
  }
}

void f10() {
  int i;
  int x;

  for (i = -1; i <= -1; i = i + 1) {
    i;
  }
}

void g1() {
  int i;
  int x;

  for (i = -1; i >= -1; i++) {
    i;
  }
}

void g2() {
  int i;
  int x;

  for (i = -1; i >= -1; ++i) {
    i;
  }
}

void g3() {
  int i;
  int x;

  for (i = -1; i <= -1; i--) {
    i;
  }
}

void g4() {
  int i;
  int x;

  for (i = -1; i <= -1; --i) {
    i;
  }
}

void g5() {
  int i;
  int x;

  for (i = -1; i <= -1; i += -1) {
    i;
  }
}

void g6() {
  int i;
  int x;

  for (i = -1; i >= -1; i += 1) {
    i;
  }
}

void g7() {
  int i;
  int x;

  for (i = -1; i >= -1; i -= -1) {
    i;
  }
}

void g8() {
  int i;
  int x;

  for (i = -1; i <= -1; i -= 1) {
    i;
  }
}

void g9() {
  int i;
  int x;

  for (i = -1; i <= -1; i = i - 1) {
    i;
  }
}

void g10() {
  int i;
  int x;

  for (i = -1; i >= -1; i = i + 1) {
    i;
  }
}
