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
extern int __merlin_init_needwun_kernel();
extern int __merlin_release_needwun_kernel();
extern int __merlin_wait_kernel_needwun_kernel();
extern int __merlin_wait_write_needwun_kernel();
extern int __merlin_wait_read_needwun_kernel();
extern void __merlinwrapper_needwun_kernel(char *SEQA,char *SEQB,char *alignedA,char *alignedB);
extern void __merlin_write_buffer_needwun_kernel(char *SEQA,char *SEQB,char *alignedA,char *alignedB);
extern void __merlin_read_buffer_needwun_kernel(char *SEQA,char *SEQB,char *alignedA,char *alignedB);
extern void __merlin_execute_needwun_kernel(char *SEQA,char *SEQB,char *alignedA,char *alignedB);
extern void __merlin_needwun_kernel(char *SEQA,char *SEQB,char *alignedA,char *alignedB);
