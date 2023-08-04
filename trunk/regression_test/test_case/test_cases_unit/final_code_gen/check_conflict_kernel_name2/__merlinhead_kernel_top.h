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
extern int __merlin_init(const char * bitstream);
extern int __merlin_release();
extern int __merlin_init_begin();
extern int __merlin_release_begin();
extern int __merlin_wait_kernel_begin();
extern int __merlin_wait_write_begin();
extern int __merlin_wait_read_begin();
extern void __merlinwrapper_begin(int a[4096],int b[4096],int c[4096],int inc);
extern void __merlin_write_buffer_begin(int a[4096],int b[4096],int c[4096],int inc);
extern void __merlin_read_buffer_begin(int a[4096],int b[4096],int c[4096],int inc);
extern void __merlin_execute_begin(int a[4096],int b[4096],int c[4096],int inc);
extern void __merlin_begin(int a[4096],int b[4096],int c[4096],int inc);
