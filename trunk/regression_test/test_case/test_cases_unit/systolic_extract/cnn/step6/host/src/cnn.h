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



#define IMG_IN_NUM  32 //256   BSIZE_K
#define IMG_OUT_NUM 16 //256   BNUM_I * PE_ROWS
#define IMG_ROW 64  //56       BSIZE_J * PE_COLS
#define IMG_COL 2 //56         BNUM_J

#define KERNEL  4   //3        BSIZE_I=K*K
#define IMG_IN_ROW (IMG_ROW + KERNEL-1)
#define IMG_IN_COL (IMG_COL + KERNEL-1)



