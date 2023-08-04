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
// RUN: %clang_cc1 %s -triple x86_64-unknown-linux-gnu -emit-llvm -target-cpu x86-64 -o - |opt -instnamer -S |FileCheck %s --check-prefix SSE
// RUN: %clang_cc1 %s -triple x86_64-unknown-linux-gnu -emit-llvm -target-cpu skylake -D AVX -o -|opt -instnamer -S  | FileCheck %s --check-prefixes AVX,SSE
// RUN: %clang_cc1 %s -triple x86_64-unknown-linux-gnu -emit-llvm -target-cpu skylake-avx512 -D AVX512 -D AVX -o -|opt -instnamer -S  | FileCheck %s --check-prefixes AVX512,AVX,SSE
// RUN: %clang_cc1 %s -triple x86_64-unknown-linux-gnu -emit-llvm -target-cpu knl -D AVX -D AVX512 -o - |opt -instnamer -S  | FileCheck %s --check-prefixes AVX512,AVX,SSE
typedef float m128;
typedef float m256;
typedef float m512;
// SSE: call <4 x float> asm "vmovhlps $1, $2, $0", "=v,v,v,~{dirflag},~{fpsr},~{flags}"(i64 %tmp, <4 x float> %tmp1)

m128 testXMM(m128 _xmm0,long _l)
{
  __asm__ ("vmovhlps %1, %2, %0" : "=v" (_xmm0) : "v" (_l), "v" (_xmm0));
  return _xmm0;
}
// AVX: call <8 x float> asm "vmovsldup $1, $0", "=v,v,~{dirflag},~{fpsr},~{flags}"(<8 x float> %tmp)
#pragma ACCEL kernel name="testYMM"

m256 vec_add_kernel(m256 _ymm0)
{


  return _ymm0;
}
// AVX512: call <16 x float> asm "vpternlogd $$0, $1, $2, $0", "=v,v,v,~{dirflag},~{fpsr},~{flags}"(<16 x float> %tmp, <16 x float> %tmp1)

m512 testZMM(m512 _zmm0,m512 _zmm1)
{


  return _zmm0;
}
