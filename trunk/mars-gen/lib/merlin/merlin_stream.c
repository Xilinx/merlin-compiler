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
#include <stdio.h>
#define DEBUG

int visitQueue(merlin_stream InQueue) {
  merlin_stream *pstTemp = &InQueue;

  if (pstTemp->head == NULL) {
    printf("visitQueue: this queue is empty\n");
    return -1;
  }

  while (pstTemp->head->next != NULL) {
    printf("%d ", pstTemp->head->data);
    pstTemp->head = pstTemp->head->next;
  }
  printf("%d \n", pstTemp->head->data);

  return 0;
}

int isEmptyQueue(merlin_stream InQueue) {
  if (InQueue.head == NULL) {
    return 0;
  }
  return 1;
}

void merlin_stream_init(merlin_stream *var, unsigned long buffer_size,
                        const char *type_name, unsigned long data_size) {
  var->head = NULL;
  var->tail = NULL;
  var->buffer_size = buffer_size;
  var->data_size = data_size;
  var->curr_read_pos = 0;
  var->curr_write_pos = 0;
  var->name = "Default Channel";
}

void merlin_stream_init_debug(merlin_stream *var, unsigned long buffer_size,
                              const char *type_name, unsigned long data_size,
                              const char *name) {
  var->head = NULL;
  var->tail = NULL;
  var->buffer_size = buffer_size;
  var->data_size = data_size;
  var->curr_read_pos = 0;
  var->curr_write_pos = 0;
  var->name = name;
}

void merlin_stream_reset(merlin_stream *var) {
  unsigned long remain_size = var->curr_write_pos - var->curr_read_pos;
  if (remain_size > 0) {
    printf("Channel = %s\n", var->name);
    printf("\tTotal write size = %ld\n", var->curr_write_pos);
    printf("\tTotal read size  = %ld\n", var->curr_read_pos);
    printf("\tRemaining size   = %ld\n", remain_size);
  }
  var->head = NULL;
  var->tail = NULL;
  var->buffer_size = 0;
  var->data_size = 0;
  var->curr_read_pos = 0;
  var->curr_write_pos = 0;
}

void merlin_stream_read(merlin_stream *var, void *data_out) {
  if (var->head == NULL) {
    printf("Channel %s read from empty queue\n", var->name);
    return;
  }
  int i;
  for (i = 0; i < var->data_size; i++) {
    char OutData = var->head->data;

    NODE *pstTemp = var->head;
    var->head = var->head->next;

    *((char *)data_out + i) = OutData;
    free(pstTemp);
  }
  var->curr_read_pos++;
}

void merlin_stream_write(merlin_stream *var, void *data_in) {
  int i;
  for (i = 0; i < var->data_size; i++) {
    NODE *pNewNode = (NODE *)malloc(NODE_LEN);
    if (pNewNode == NULL) {
      return;
    }

    pNewNode->data = *((char *)data_in + i);
    pNewNode->next = NULL;

    if (var->head == NULL) {
      var->head = pNewNode;
      var->tail = pNewNode;
    } else {
      var->tail->next = pNewNode;
      var->tail = pNewNode;
    }
  }
  var->curr_write_pos++;
}
