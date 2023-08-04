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
#ifndef __MERLIN_STREAM_H__
#define __MERLIN_STREAM_H__
struct merlin_stream {
  unsigned long buffer_size;
  unsigned long data_size;
  unsigned long curr_read_pos;
  unsigned long curr_write_pos;
  void *buffer;
};
typedef struct merlin_stream merlin_stream;

#ifdef __cplusplus
extern "C" {
#endif
void merlin_stream_init(merlin_stream *var, unsigned long buffer_size,
                        unsigned long data_size);

void merlin_stream_reset(merlin_stream *var);

void merlin_stream_write(merlin_stream *var, void *data_in);

void merlin_stream_read(merlin_stream *var, void *data_out);
#ifdef __cplusplus
}
#endif
#endif
