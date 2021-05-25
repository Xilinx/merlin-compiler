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


#include <stdlib.h>
#include <assert.h>
#include "merlin_stream.h"

void merlin_stream_init(merlin_stream *var, unsigned long buffer_size,
                        unsigned long data_size) {
  var->buffer_size = buffer_size;
  var->data_size = data_size;
  var->curr_read_pos = 0;
  var->curr_write_pos = 0;
  var->buffer = malloc(buffer_size * data_size);
}

void merlin_stream_reset(merlin_stream *var) {
  var->buffer_size = 0;
  var->data_size = 0;
  var->curr_read_pos = 0;
  var->curr_write_pos = 0;
  free(var->buffer);
  var->buffer = NULL;
}

void merlin_stream_read(merlin_stream *var, void *data_out) {
  if (var->curr_read_pos >= var->curr_write_pos) {
    assert(0 && "access empty channel buffer");
    return;
  }

  switch (var->data_size) {
  case 1:
    *(char *)data_out = *(((char *)var->buffer) + var->curr_read_pos);
    break;
  case 2:
    *(short *)data_out = *(((short *)var->buffer) + var->curr_read_pos);
    break;
  case 4:
    *(int *)data_out = *(((int *)var->buffer) + var->curr_read_pos);
    break;
  case 8:
    *(long long *)data_out = *(((long long *)var->buffer) + var->curr_read_pos);
    break;
  default:
    assert(0 && "cannot support data type");
    break;
  }
  var->curr_read_pos++;
}

void merlin_stream_write(merlin_stream *var, void *data_in) {
  if (var->curr_write_pos >= var->buffer_size) {
    var->buffer_size <<= 1;
    var->buffer = realloc(var->buffer, var->buffer_size * var->data_size);
    // assert(0 && "access full channel buffer");
    return;
  }

  switch (var->data_size) {
  case 1:
    *(((char *)var->buffer) + var->curr_write_pos) = *(char *)data_in;
    break;
  case 2:
    *(((short *)var->buffer) + var->curr_write_pos) = *(short *)data_in;
    break;
  case 4:
    *(((int *)var->buffer) + var->curr_write_pos) = *(int *)data_in;
    break;
  case 8:
    *(((long long *)var->buffer) + var->curr_write_pos) = *(long long *)data_in;
    break;
  default:
    assert(0 && "cannot support data type");
    break;
  }
  var->curr_write_pos++;
}
