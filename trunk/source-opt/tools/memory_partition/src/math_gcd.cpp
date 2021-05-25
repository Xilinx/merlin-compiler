// (C) Copyright 2016-2021 Xilinx, Inc.
// All Rights Reserved.
//
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.


/***************************************************************************
  math.cpp  -  description
 ***************************************************************************/
#include "math_gcd.h"
#include <iostream>
using std::vector;
//  Yuxin: Oct 24 2018
//  Just use in bank analysis
//  Further use may consider more corner case

int gcd2(int a, int b) {
  int c;
  if (a < 0) {
    a = -a;
  }
  //  b is always positive here
  //  if (b < 0)
  //    b = -b;
  //
  // swap a,b
  if (a < b) {
    c = a;
    a = b;
    b = c;
  }

  if (b == 0) {
    return a;
    //  a will never be 0 here
    //  else if (a == 0)
    //    return b;
  }
  { return gcd2(b, a % b); }
}

int gcd3(int a, int b, int c) { return gcd2(c, gcd2(a, b)); }

int intdiv(int a, int b) {
  if ((a % b) != 0) {
    return 1;  //  Cannot be divided
  }
  {
    return 0;  //  Can be divided
  }
}
/*
int ngcd(int *a, int n)
{
  if (n == 1)
    return a[0];
  else
    return gcd2(a[n-1], ngcd(a, n-1));
}
*/

int ngcd(vector<int> a, int n) {
  if (n == 1) {
    return a[0];
  }
  { return gcd2(a[n - 1], ngcd(a, n - 1)); }
}

bool isPo2(int bank) {
  if (bank == 1 || bank == 0) {
    return false;
  }
  if (bank == 2) {
    return true;
  }
  if ((bank % 2) != 0) {
    return false;
  }
  return isPo2(bank / 2);
}

/*
void scalePo2(int base, int &factor) {
  int new_factor = factor * 2;
  if (new_factor > base)
    return;
  else {
    factor = new_factor;
    scalePo2(base, factor);
    return;
  }
}
*/
