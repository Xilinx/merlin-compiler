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




int g_idx0;
int g_idx1;
int g_idx2;
int g_idx3;

void set_global_id(int dim, int val) {
  if (0 == dim)
    g_idx0 = val;
  if (1 == dim)
    g_idx1 = val;
  if (2 == dim)
    g_idx2 = val;
  if (3 == dim)
    g_idx3 = val;
}

int get_global_id(int dim) {
  if (0 == dim)
    return g_idx0;
  if (1 == dim)
    return g_idx1;
  if (2 == dim)
    return g_idx2;
  if (3 == dim)
    return g_idx3;
}
