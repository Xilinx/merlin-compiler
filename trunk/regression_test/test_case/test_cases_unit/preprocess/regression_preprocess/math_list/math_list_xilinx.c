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
#include "__merlin_math.h"

#pragma ACCEL kernel
void top(long x, long y, long z, 
    float fx, float fy, float fz , int iy,
    double dx, double dy, double dz) {
  //opencl spec
  abs_diff(x, y);
  add_sat(x, y);
  hadd(x, y);
  rhadd(x, y);
  clamp(x, y, z);
  clz(x);
  mad_hi(x, y, z);
  mad_sat(x, y, z);
  max(x, y);
  min(x, y);
  mul_hi(x, y);
  rotate(x, y);
  sub_sat(x, y);
  popcount(x);


  //support float builtins
  finitef(fx);
  isinff(fx);
  isnanf(fx);
  fmaxf(fx, fy);
  fminf(fx, fy);
  fdimf(fx, fy);
  sqrtf(fx);
  cbrtf(fx);
  hypotf(fx, fy);
  fmodf(fx, fy);
  truncf(fx);
  roundf(fx);
  rintf(fx);
  modff(fx, &fy);
  powf(fx, fy);
  expf(fx);
  exp2f(fx);
  expm1f(fx);
  exp10f(fx);
  frexpf(fx, &iy);
  logf(fx);
  log10f(fx);
  ilogbf(fx);
 
  //support double builtins
  isfinite(dx);
  isinf(dx);
  isnan(dx);
  fmax(dx, dy);
  fmin(dx, dy);
  fdim(dx, dy);
  sqrt(dx);
  fabs(dx);
  fmod(dx, dy);
  ceil(dx);
  floor(dx);
  trunc(dx);
  round(dx);
  rint(dx);
  modf(dx, &dy);
  pow(dx, dy);
  exp(dx);
  exp2(dx);
  exp10(dx);
  frexp(dx, &iy);
  ldexp(dx, iy);
  log(dx);
  log2(dx);
  log10(dx);
  log1p(dx);
  ilogb(dx);
 
  //support trig builtins
  sinf(fx);
  cosf(fx);
  tanf(fx);
  asinf(fx);
  acosf(fx);
  atanf(fx);
  atan2f(fx, fy);
  sinhf(fx);
  coshf(fx);
  tanhf(fx);

  //double trig
  sin(dx);
  cos(dx);
  tan(dx);
  asin(dx);
  acos(dx);
  atan(dx);
  atan2(dx, dy);
  sinh(dx);
  cosh(dx);
  tanh(dx);
  return; 
}
