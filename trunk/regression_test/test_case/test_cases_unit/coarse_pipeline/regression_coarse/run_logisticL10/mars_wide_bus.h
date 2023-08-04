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
#ifndef __MERLING_LARGE_BITWIDTH_INTERFACE_H_
#define __MERLING_LARGE_BITWIDTH_INTERFACE_H_
#include<assert.h>
#include<stdlib.h>
#define BUS_WIDTH LARGE_BUS / 8
#define FACTOR BUS_WIDTH / 4
#define FACTOR_L BUS_WIDTH / 8
#define cpp_get_range(tmp, x, y) tmp(x, y)
#define c_get_range(tmp, x, y) apint_get_range(tmp, x, y)
#define cpp_set_range(tmp, x, y, val) tmp(x, y) = val
#define c_set_range(tmp, x, y, val) tmp = apint_set_range(tmp, x, y, val)
#ifdef __cplusplus
#define tmp2(x, y) cpp_get_range(tmp, x, y)
#define tmp3(x, y, val) cpp_set_range(tmp, x, y, val)
#else
#define tmp2(x, y) c_get_range(tmp, x, y)
#define tmp3(x, y, val) c_set_range(tmp, x, y, val)
#endif
inline static char memcpy_wide_bus_single_read_char(MARS_WIDE_BUS_TYPE *a,
                                                  size_t offset_byte) {
#pragma HLS inline self
  //#pragma HLS array_partition variable = a_buf cyclic factor = 16
  const size_t data_width = sizeof(char);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t start = offset / num_elements;
#ifdef __cplusplus
  MARS_WIDE_BUS_TYPE tmp(a[start]);
#else
  MARS_WIDE_BUS_TYPE tmp = a[start];
#endif
  char ret = tmp2(((head_align + 1) * data_width * 8 - 1),
                 (head_align * data_width * 8));
  return ret;
}

inline static void memcpy_wide_bus_single_write_char(MARS_WIDE_BUS_TYPE *c,
                                                    char c_buf,
                                                    size_t offset_byte) {
#pragma HLS inline self
  //#pragma HLS array_partition variable = c_buf cyclic factor = 16
  const size_t data_width = sizeof(char);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t start = offset / num_elements;
#ifdef __cplusplus
  MARS_WIDE_BUS_TYPE tmp(c[start]);
#else
  MARS_WIDE_BUS_TYPE tmp = c[start];
#endif
  tmp3(((head_align + 1) * data_width * 8 - 1), (head_align * data_width * 8),
       c_buf);
  c[start] = tmp;
  return;
}

inline static int memcpy_wide_bus_single_read_int(MARS_WIDE_BUS_TYPE *a,
                                                  size_t offset_byte) {
#pragma HLS inline self
  //#pragma HLS array_partition variable = a_buf cyclic factor = 16
  const size_t data_width = sizeof(int);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t start = offset / num_elements;
#ifdef __cplusplus
  MARS_WIDE_BUS_TYPE tmp(a[start]);
#else
  MARS_WIDE_BUS_TYPE tmp = a[start];
#endif
  int ret = tmp2(((head_align + 1) * data_width * 8 - 1),
                 (head_align * data_width * 8));
  return ret;
}

inline static void memcpy_wide_bus_single_write_int(MARS_WIDE_BUS_TYPE *c,
                                                    int c_buf,
                                                    size_t offset_byte) {
#pragma HLS inline self
  //#pragma HLS array_partition variable = c_buf cyclic factor = 16
  const size_t data_width = sizeof(int);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t start = offset / num_elements;
#ifdef __cplusplus
  MARS_WIDE_BUS_TYPE tmp(c[start]);
#else
  MARS_WIDE_BUS_TYPE tmp = c[start];
#endif
  tmp3(((head_align + 1) * data_width * 8 - 1), (head_align * data_width * 8),
       c_buf);
  c[start] = tmp;
  return;
}

inline static float
memcpy_wide_bus_single_read_float(MARS_WIDE_BUS_TYPE *a,
                                  size_t offset_byte) {
//#pragma HLS array_partition variable = a_buf cyclic factor = 16
#pragma HLS inline self
  const size_t data_width = sizeof(float);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t start = offset / num_elements;
#ifdef __cplusplus
  MARS_WIDE_BUS_TYPE tmp(a[start]);
#else
  MARS_WIDE_BUS_TYPE tmp = a[start];
#endif

  int raw_bits = tmp2(((head_align + 1) * data_width * 8 - 1),
                      (head_align * data_width * 8));
  float ret = *(float *)(&raw_bits);
  return ret;
}

inline static void
memcpy_wide_bus_single_write_float(MARS_WIDE_BUS_TYPE *c, float c_buf,
                                   size_t offset_byte) {
#pragma HLS inline self
  //#pragma HLS array_partition variable = c_buf cyclic factor = 16
  const size_t data_width = sizeof(float);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t start = offset / num_elements;
#ifdef __cplusplus
  MARS_WIDE_BUS_TYPE tmp(c[start]);
#else
  MARS_WIDE_BUS_TYPE tmp = c[start];
#endif
  float buf_tmp = c_buf;
  long raw_bits = *(long *)&buf_tmp;
  tmp3(((head_align + 1) * data_width * 8 - 1), (head_align * data_width * 8),
       raw_bits);
  c[start] = tmp;
  return;
}

inline static double
memcpy_wide_bus_single_read_double(MARS_WIDE_BUS_TYPE *a,
                                   size_t offset_byte) {
#pragma HLS inline self
  //#pragma HLS array_partition variable = a_buf cyclic factor = 16
  const size_t data_width = sizeof(double);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t start = offset / num_elements;
#ifdef __cplusplus
  MARS_WIDE_BUS_TYPE tmp(a[start]);
#else
  MARS_WIDE_BUS_TYPE tmp = a[start];
#endif

  long raw_bits = tmp2(((head_align + 1) * data_width * 8 - 1),
                            (head_align * data_width * 8));
  double ret = *(double *)(&raw_bits);
  return ret;
}

inline static void
memcpy_wide_bus_single_write_double(MARS_WIDE_BUS_TYPE *c, double c_buf,
                                    size_t offset_byte) {
#pragma HLS inline self
  //#pragma HLS array_partition variable = c_buf cyclic factor = 16
  const size_t data_width = sizeof(double);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t start = offset / num_elements;
#ifdef __cplusplus
  MARS_WIDE_BUS_TYPE tmp(c[start]);
#else
  MARS_WIDE_BUS_TYPE tmp = c[start];
#endif
  double buf_tmp = c_buf;
  long raw_bits = *(long *)&buf_tmp;
  tmp3(((head_align + 1) * data_width * 8 - 1), (head_align * data_width * 8),
       raw_bits);
  c[start] = tmp;
  return;
}

inline static void memcpy_wide_bus_read_int(int *a_buf, MARS_WIDE_BUS_TYPE *a,
                                            size_t offset_byte,
                                            size_t size_byte) {
#pragma HLS inline self
  //#pragma HLS array_partition variable = a_buf cyclic factor = 16
  const size_t data_width = sizeof(int);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t buf_size = size_byte / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t new_offset = offset + buf_size;
  size_t tail_align = (new_offset - 1) & (num_elements - 1);
  size_t start = offset / num_elements;
  size_t end = (offset + buf_size + num_elements - 1) / num_elements;
  //MARS_WIDE_BUS_TYPE *a_offset = a + start;
  size_t i, j;
  int len = end - start;
  assert(len <= buf_size / num_elements + 2);
  assert(len >= buf_size / num_elements);
  if (1 == len) {
#ifdef __cplusplus
    MARS_WIDE_BUS_TYPE tmp(a[start]);
#else
    MARS_WIDE_BUS_TYPE tmp = a[start];
#endif
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
       if (j < head_align || j > tail_align)
         continue;
        a_buf[j - head_align] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
    }
    return;
  }

  for (i = 0; i < len; ++i) {
#pragma HLS pipeline
#ifdef __cplusplus
    MARS_WIDE_BUS_TYPE tmp(a[i + start]);
#else
    MARS_WIDE_BUS_TYPE tmp = a[i + start];
#endif
    if (head_align == 0) {
      for (j = 0; j < num_elements; ++j) {
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 0] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 1) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 1)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 2) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 2)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 2] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 3) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 3)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 3] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 4) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 4)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 4] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 5) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 5)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 5] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 6) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 6)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 6] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 7) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 7)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 7] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 8) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 8)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 8] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 9) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 9)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 9] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 10) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 10)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 10] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 11) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 11)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 11] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 12) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 12)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 12] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 13) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 13)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 13] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 14) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 14)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 14] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 15)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 15] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }
  }
}

inline static void memcpy_wide_bus_write_int(MARS_WIDE_BUS_TYPE *c, int *c_buf,
                                             size_t offset_byte,
                                             size_t size_byte) {
#pragma HLS inline self
  //#pragma HLS array_partition variable = c_buf cyclic factor = 16
  const size_t data_width = sizeof(int);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t buf_size = size_byte / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t new_offset = offset + buf_size;
  size_t tail_align = (new_offset - 1) & (num_elements - 1);
  size_t start = offset / num_elements;
  size_t end = (offset + buf_size + num_elements - 1) / num_elements;
  size_t len = end - start;
  size_t i, j;
  if (head_align == 0)
    len = (buf_size + num_elements - 1) / num_elements;
  size_t align = 0;
  if (len == 1) {
    MARS_WIDE_BUS_TYPE tmp;
    if (head_align != 0 || tail_align != (num_elements - 1))
      tmp = c[start];
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align)
        continue;
      if (j > tail_align)
        continue;
      tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
           c_buf[j - head_align]);
    }
    c[start] = tmp;
    return;
  }
  if (head_align != 0) {
    MARS_WIDE_BUS_TYPE tmp = c[start];
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align)
        continue;
      tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
           c_buf[j - head_align]);
    }
    c[start] = tmp;
    start++;
    align++;
  }
  if (tail_align != (num_elements - 1))
    align++;
  int burst_len = len - align;
  assert(burst_len <= buf_size / num_elements);
  for (i = 0; i < burst_len; ++i) {
#pragma HLS pipeline
    MARS_WIDE_BUS_TYPE tmp;
    if (head_align == 0) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[i * num_elements + j - 0]);
      }

    }

    else if (head_align == 1) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[i * num_elements + j + 15]);
      }
    }

    else if (head_align == 2) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[i * num_elements + j + 14]);
      }
    }

    else if (head_align == 3) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[i * num_elements + j + 13]);
      }
    }

    else if (head_align == 4) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[i * num_elements + j + 12]);
      }
    }

    else if (head_align == 5) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[i * num_elements + j + 11]);
      }
    }

    else if (head_align == 6) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[i * num_elements + j + 10]);
      }
    }

    else if (head_align == 7) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[i * num_elements + j + 9]);
      }
    }

    else if (head_align == 8) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[i * num_elements + j + 8]);
      }
    }

    else if (head_align == 9) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[i * num_elements + j + 7]);
      }
    }

    else if (head_align == 10) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[i * num_elements + j + 6]);
      }
    }

    else if (head_align == 11) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[i * num_elements + j + 5]);
      }
    }

    else if (head_align == 12) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[i * num_elements + j + 4]);
      }
    }

    else if (head_align == 13) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[i * num_elements + j + 3]);
      }
    }

    else if (head_align == 14) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[i * num_elements + j + 2]);
      }
    }

    else {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[i * num_elements + j + 1]);
      }
    }

    c[i + start] = tmp;
  }
  if (tail_align != num_elements - 1) {
    MARS_WIDE_BUS_TYPE tmp = c[end - 1];
    size_t pos = (len - align) * num_elements;
    pos += (num_elements - head_align) % num_elements;
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j > tail_align)
        continue;
      tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
           c_buf[pos + j]);
    }
    c[end - 1] = tmp;
  }
}

inline static void memcpy_wide_bus_read_char(char *a_buf, MARS_WIDE_BUS_TYPE *a,
                                            size_t offset_byte,
                                            size_t size_byte) {
#pragma HLS inline self
  //#pragma HLS array_partition variable = a_buf cyclic factor = 16
  const size_t data_width = sizeof(char);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t buf_size = size_byte / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t new_offset = offset + buf_size;
  size_t tail_align = (new_offset - 1) & (num_elements - 1);
  size_t start = offset / num_elements;
  size_t end = (offset + buf_size + num_elements - 1) / num_elements;
  //MARS_WIDE_BUS_TYPE *a_offset = a + start;
  size_t i, j;
  int len = end - start;
  assert(len <= buf_size / num_elements + 2);
  assert(len >= buf_size / num_elements);
  if (1 == len) {
#ifdef __cplusplus
    MARS_WIDE_BUS_TYPE tmp(a[start]);
#else
    MARS_WIDE_BUS_TYPE tmp = a[start];
#endif
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
       if (j < head_align || j > tail_align)
         continue;
        a_buf[j - head_align] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
    }
    return;
  }

  for (i = 0; i < len; ++i) {
#pragma HLS pipeline
#ifdef __cplusplus
    MARS_WIDE_BUS_TYPE tmp(a[i + start]);
#else
    MARS_WIDE_BUS_TYPE tmp = a[i + start];
#endif
    if (head_align == 0) {
      for (j = 0; j < num_elements; ++j) {
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 0] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 1) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 1)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 2) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 2)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 2] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 3) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 3)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 3] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 4) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 4)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 4] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 5) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 5)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 5] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 6) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 6)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 6] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 7) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 7)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 7] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 8) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 8)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 8] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 9) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 9)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 9] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 10) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 10)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 10] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 11) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 11)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 11] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 12) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 12)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 12] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 13) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 13)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 13] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 14) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 14)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 14] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 15)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 15] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }
  }
}

inline static void memcpy_wide_bus_write_char(MARS_WIDE_BUS_TYPE *c, char *c_buf,
                                             size_t offset_byte,
                                             size_t size_byte) {
#pragma HLS inline self
  //#pragma HLS array_partition variable = c_buf cyclic factor = 16
  const size_t data_width = sizeof(char);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t buf_size = size_byte / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t new_offset = offset + buf_size;
  size_t tail_align = (new_offset - 1) & (num_elements - 1);
  size_t start = offset / num_elements;
  size_t end = (offset + buf_size + num_elements - 1) / num_elements;
  size_t len = end - start;
  size_t i, j;
  if (head_align == 0)
    len = (buf_size + num_elements - 1) / num_elements;
  size_t align = 0;
  if (len == 1) {
    MARS_WIDE_BUS_TYPE tmp;
    if (head_align != 0 || tail_align != (num_elements - 1))
      tmp = c[start];
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align)
        continue;
      if (j > tail_align)
        continue;
      tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
           c_buf[j - head_align]);
    }
    c[start] = tmp;
    return;
  }
  if (head_align != 0) {
    MARS_WIDE_BUS_TYPE tmp = c[start];
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align)
        continue;
      tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
           c_buf[j - head_align]);
    }
    c[start] = tmp;
    start++;
    align++;
  }
  if (tail_align != (num_elements - 1))
    align++;
  int burst_len = len - align;
  assert(burst_len <= buf_size / num_elements);
  for (i = 0; i < burst_len; ++i) {
#pragma HLS pipeline
    MARS_WIDE_BUS_TYPE tmp;
    if (head_align == 0) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[i * num_elements + j - 0]);
      }

    }

    else if (head_align == 1) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[i * num_elements + j + 15]);
      }
    }

    else if (head_align == 2) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[i * num_elements + j + 14]);
      }
    }

    else if (head_align == 3) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[i * num_elements + j + 13]);
      }
    }

    else if (head_align == 4) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[i * num_elements + j + 12]);
      }
    }

    else if (head_align == 5) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[i * num_elements + j + 11]);
      }
    }

    else if (head_align == 6) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[i * num_elements + j + 10]);
      }
    }

    else if (head_align == 7) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[i * num_elements + j + 9]);
      }
    }

    else if (head_align == 8) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[i * num_elements + j + 8]);
      }
    }

    else if (head_align == 9) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[i * num_elements + j + 7]);
      }
    }

    else if (head_align == 10) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[i * num_elements + j + 6]);
      }
    }

    else if (head_align == 11) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[i * num_elements + j + 5]);
      }
    }

    else if (head_align == 12) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[i * num_elements + j + 4]);
      }
    }

    else if (head_align == 13) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[i * num_elements + j + 3]);
      }
    }

    else if (head_align == 14) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[i * num_elements + j + 2]);
      }
    }

    else {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[i * num_elements + j + 1]);
      }
    }

    c[i + start] = tmp;
  }
  if (tail_align != num_elements - 1) {
    MARS_WIDE_BUS_TYPE tmp = c[end - 1];
    size_t pos = (len - align) * num_elements;
    pos += (num_elements - head_align) % num_elements;
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j > tail_align)
        continue;
      tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
           c_buf[pos + j]);
    }
    c[end - 1] = tmp;
  }
}

inline static void memcpy_wide_bus_read_float(float *a_buf,
                                              MARS_WIDE_BUS_TYPE *a,
                                              size_t offset_byte,
                                              size_t size_byte) {
#pragma HLS inline self
  //#pragma HLS array_partition variable = a_buf cyclic factor = 16
  const size_t data_width = sizeof(float);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t buf_size = size_byte / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t new_offset = offset + buf_size;
  size_t tail_align = (new_offset - 1) & (num_elements - 1);
  size_t start = offset / num_elements;
  size_t end = (offset + buf_size + num_elements - 1) / num_elements;
  //MARS_WIDE_BUS_TYPE *a_offset = a + start;
  size_t i, j;
  int len = end - start;
  assert(len <= buf_size / num_elements + 2);
  assert(len >= buf_size / num_elements);
  if (1 == len) {
#ifdef __cplusplus
    MARS_WIDE_BUS_TYPE tmp(a[start]);
#else
    MARS_WIDE_BUS_TYPE tmp = a[start];
#endif
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
       if (j < head_align || j > tail_align)
         continue;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[j - head_align] = *(float *)(&raw_bits);
    }
    return;
  }

  for (i = 0; i < len; ++i) {
#pragma HLS pipeline
#ifdef __cplusplus
    MARS_WIDE_BUS_TYPE tmp(a[i + start]);
#else
    MARS_WIDE_BUS_TYPE tmp = a[i + start];
#endif

    if (head_align == 0) {
      for (j = 0; j < num_elements; ++j) {
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[i * num_elements + j - 0] = *(float *)(&raw_bits);
      }
    }

    else if (head_align == 1) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 1)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[i * num_elements + j - 1] = *(float *)(&raw_bits);
      }
    }

    else if (head_align == 2) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 2)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[i * num_elements + j - 2] = *(float *)(&raw_bits);
      }
    }

    else if (head_align == 3) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 3)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[i * num_elements + j - 3] = *(float *)(&raw_bits);
      }
    }

    else if (head_align == 4) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 4)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[i * num_elements + j - 4] = *(float *)(&raw_bits);
      }
    }

    else if (head_align == 5) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 5)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[i * num_elements + j - 5] = *(float *)(&raw_bits);
      }
    }

    else if (head_align == 6) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 6)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[i * num_elements + j - 6] = *(float *)(&raw_bits);
      }
    }

    else if (head_align == 7) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 7)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[i * num_elements + j - 7] = *(float *)(&raw_bits);
      }
    }

    else if (head_align == 8) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 8)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[i * num_elements + j - 8] = *(float *)(&raw_bits);
      }
    }

    else if (head_align == 9) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 9)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[i * num_elements + j - 9] = *(float *)(&raw_bits);
      }
    }

    else if (head_align == 10) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 10)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[i * num_elements + j - 10] = *(float *)(&raw_bits);
      }
    }

    else if (head_align == 11) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 11)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[i * num_elements + j - 11] = *(float *)(&raw_bits);
      }
    }

    else if (head_align == 12) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 12)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[i * num_elements + j - 12] = *(float *)(&raw_bits);
      }
    }

    else if (head_align == 13) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 13)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[i * num_elements + j - 13] = *(float *)(&raw_bits);
      }
    }

    else if (head_align == 14) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 14)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[i * num_elements + j - 14] = *(float *)(&raw_bits);
      }
    }

    else {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 15)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[i * num_elements + j - 15] = *(float *)(&raw_bits);
      }
    }
  }
}

inline static void memcpy_wide_bus_write_float(MARS_WIDE_BUS_TYPE *c,
                                               float *c_buf,
                                               size_t offset_byte,
                                               size_t size_byte) {
#pragma HLS inline self
  //#pragma HLS array_partition variable = c_buf cyclic factor = 16
  const size_t data_width = sizeof(float);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t buf_size = size_byte / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t new_offset = offset + buf_size;
  size_t tail_align = (new_offset - 1) & (num_elements - 1);
  size_t start = offset / num_elements;
  size_t end = (offset + buf_size + num_elements - 1) / num_elements;
  size_t len = end - start;
  size_t i, j;
  if (head_align == 0)
    len = (buf_size + num_elements - 1) / num_elements;
  if (len == 1) {
    MARS_WIDE_BUS_TYPE tmp;
    if (head_align != 0 || tail_align != (num_elements - 1))
      tmp = c[start];
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align)
        continue;
      if (j > tail_align)
        continue;
      float buf_tmp = c_buf[j - head_align];
      int raw_bits = *(int *)&buf_tmp;
      tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
    }
    c[start] = tmp;
    return;
  }
  unsigned align = 0;
  if (head_align != 0) {
    MARS_WIDE_BUS_TYPE tmp = c[start];
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align)
        continue;
      float buf_tmp = c_buf[j - head_align];
      int raw_bits = *(int *)&buf_tmp;
      tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
    }
    c[start] = tmp;
    start++;
    align++;
  }
  if (tail_align != (num_elements - 1))
    align++;
  int burst_len = len - align;
  assert(burst_len <= buf_size / num_elements);
  for (i = 0; i < burst_len; ++i) {
#pragma HLS pipeline
    MARS_WIDE_BUS_TYPE tmp;
    if (head_align == 0) {
      for (j = 0; j < num_elements; ++j) {
        float buf_tmp = c_buf[i * num_elements + j - 0];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }

    }

    else if (head_align == 1) {
      for (j = 0; j < num_elements; ++j) {
        float buf_tmp = c_buf[i * num_elements + j + 15];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 2) {
      for (j = 0; j < num_elements; ++j) {
        float buf_tmp = c_buf[i * num_elements + j + 14];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 3) {
      for (j = 0; j < num_elements; ++j) {
        float buf_tmp = c_buf[i * num_elements + j + 13];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 4) {
      for (j = 0; j < num_elements; ++j) {
        float buf_tmp = c_buf[i * num_elements + j + 12];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 5) {
      for (j = 0; j < num_elements; ++j) {
        float buf_tmp = c_buf[i * num_elements + j + 11];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 6) {
      for (j = 0; j < num_elements; ++j) {
        float buf_tmp = c_buf[i * num_elements + j + 10];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 7) {
      for (j = 0; j < num_elements; ++j) {
        float buf_tmp = c_buf[i * num_elements + j + 9];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 8) {
      for (j = 0; j < num_elements; ++j) {
        float buf_tmp = c_buf[i * num_elements + j + 8];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 9) {
      for (j = 0; j < num_elements; ++j) {
        float buf_tmp = c_buf[i * num_elements + j + 7];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 10) {
      for (j = 0; j < num_elements; ++j) {
        float buf_tmp = c_buf[i * num_elements + j + 6];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 11) {
      for (j = 0; j < num_elements; ++j) {
        float buf_tmp = c_buf[i * num_elements + j + 5];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 12) {
      for (j = 0; j < num_elements; ++j) {
        float buf_tmp = c_buf[i * num_elements + j + 4];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 13) {
      for (j = 0; j < num_elements; ++j) {
        float buf_tmp = c_buf[i * num_elements + j + 3];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 14) {
      for (j = 0; j < num_elements; ++j) {
        float buf_tmp = c_buf[i * num_elements + j + 2];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else {
      for (j = 0; j < num_elements; ++j) {
        float buf_tmp = c_buf[i * num_elements + j + 1];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    c[i + start] = tmp;
  }
  if (tail_align != num_elements - 1) {
    MARS_WIDE_BUS_TYPE tmp = c[end - 1];
    size_t pos = (len - align) * num_elements;
    pos += (num_elements - head_align) % num_elements;
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j > tail_align)
        continue;
      float buf_tmp = c_buf[pos + j];
      int raw_bits = *(int *)&buf_tmp;
      tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
    }
    c[end - 1] = tmp;
  }
}

inline static void memcpy_wide_bus_read_double(double *a_buf,
                                               MARS_WIDE_BUS_TYPE *a,
                                               size_t offset_byte,
                                               size_t size_byte) {
#pragma HLS inline self
  //#pragma HLS array_partition variable = a_buf cyclic factor = 8
  const size_t data_width = sizeof(double);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t buf_size = size_byte / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t new_offset = offset + buf_size;
  size_t tail_align = (new_offset - 1) & (num_elements - 1);
  size_t start = offset / num_elements;
  size_t end = (offset + buf_size + num_elements - 1) / num_elements;
  //MARS_WIDE_BUS_TYPE *a_offset = a + start;
  size_t i, j;
  int len = end - start;
  assert(len <= buf_size / num_elements + 2);
  assert(len >= buf_size / num_elements);
  if (1 == len) {
#ifdef __cplusplus
    MARS_WIDE_BUS_TYPE tmp(a[start]);
#else
    MARS_WIDE_BUS_TYPE tmp = a[start];
#endif
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
       if (j < head_align || j > tail_align)
         continue;
        long raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[j - head_align] = *(double *)(&raw_bits);
    }
    return;
  }

  for (i = 0; i < len; ++i) {
#pragma HLS pipeline
#ifdef __cplusplus
    MARS_WIDE_BUS_TYPE tmp(a[i + start]);
#else
    MARS_WIDE_BUS_TYPE tmp = a[i + start];
#endif
    if (head_align == 0) {
      for (j = 0; j < num_elements; ++j) {
        if (i == end - start - 1 && j > tail_align)
          continue;
        long raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[i * num_elements + j - 0] = *(double *)(&raw_bits);
      }
    }

    else if (head_align == 1) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 1)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        long raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[i * num_elements + j - 1] = *(double *)(&raw_bits);
      }
    }

    else if (head_align == 2) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 2)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        long raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[i * num_elements + j - 2] = *(double *)(&raw_bits);
      }
    }

    else if (head_align == 3) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 3)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        long raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[i * num_elements + j - 3] = *(double *)(&raw_bits);
      }
    }

    else if (head_align == 4) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 4)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        long raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[i * num_elements + j - 4] = *(double *)(&raw_bits);
      }
    }

    else if (head_align == 5) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 5)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        long raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[i * num_elements + j - 5] = *(double *)(&raw_bits);
      }
    }

    else if (head_align == 6) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 6)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        long raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[i * num_elements + j - 6] = *(double *)(&raw_bits);
      }
    }

    else {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 7)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        long raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[i * num_elements + j - 7] = *(double *)(&raw_bits);
      }
    }
  }
}

inline static void memcpy_wide_bus_write_double(MARS_WIDE_BUS_TYPE *c,
                                                double *c_buf,
                                                size_t offset_byte,
                                                size_t size_byte) {
#pragma HLS inline self
  //#pragma HLS array_partition variable = c_buf cyclic factor = 8
  const size_t data_width = sizeof(double);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t buf_size = size_byte / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t new_offset = offset + buf_size;
  size_t tail_align = (new_offset - 1) & (num_elements - 1);
  size_t start = offset / num_elements;
  size_t end = (offset + buf_size + num_elements - 1) / num_elements;
  size_t len = end - start;
  size_t i, j;
  if (head_align == 0)
    len = (buf_size + num_elements - 1) / num_elements;
  if (len == 1) {
    MARS_WIDE_BUS_TYPE tmp;
    if (head_align != 0 || tail_align != (num_elements - 1))
      tmp = c[start];
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align)
        continue;
      if (j > tail_align)
        continue;
      double buf_tmp = c_buf[j - head_align];
      long raw_bits = *(long *)&buf_tmp;
      tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
    }
    c[start] = tmp;
    return;
  }
  size_t align = 0;
  if (head_align != 0) {
    MARS_WIDE_BUS_TYPE tmp = c[start];
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align)
        continue;
      double buf_tmp = c_buf[j - head_align];
      long raw_bits = *(long *)&buf_tmp;
      tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
    }
    c[start] = tmp;
    start++;
    align++;
  }
  if (tail_align != (num_elements - 1))
    align++;
  int burst_len = len - align;
  assert(burst_len <= buf_size / num_elements);
  for (i = 0; i < burst_len; ++i) {
#pragma HLS pipeline
    MARS_WIDE_BUS_TYPE tmp;
    if (head_align == 0) {
      for (j = 0; j < num_elements; ++j) {
        double buf_tmp = c_buf[i * num_elements + j - 0];
        long raw_bits = *(long *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }

    }

    else if (head_align == 1) {
      for (j = 0; j < num_elements; ++j) {
        double buf_tmp = c_buf[i * num_elements + j + 7];
        long raw_bits = *(long *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 2) {
      for (j = 0; j < num_elements; ++j) {
        double buf_tmp = c_buf[i * num_elements + j + 6];
        long raw_bits = *(long *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 3) {
      for (j = 0; j < num_elements; ++j) {
        double buf_tmp = c_buf[i * num_elements + j + 5];
        long raw_bits = *(long *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 4) {
      for (j = 0; j < num_elements; ++j) {
        double buf_tmp = c_buf[i * num_elements + j + 4];
        long raw_bits = *(long *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 5) {
      for (j = 0; j < num_elements; ++j) {
        double buf_tmp = c_buf[i * num_elements + j + 3];
        long raw_bits = *(long *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 6) {
      for (j = 0; j < num_elements; ++j) {
        double buf_tmp = c_buf[i * num_elements + j + 2];
        long raw_bits = *(long *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else {
      for (j = 0; j < num_elements; ++j) {
        double buf_tmp = c_buf[i * num_elements + j + 1];
        long raw_bits = *(long *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    c[i + start] = tmp;
  }
  if (tail_align != num_elements - 1) {
    MARS_WIDE_BUS_TYPE tmp = c[end - 1];
    size_t pos = (len - align) * num_elements;
    pos += (num_elements - head_align) % num_elements;
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j > tail_align)
        continue;
      double buf_tmp = c_buf[pos + j];
      long raw_bits = *(long *)&buf_tmp;
      tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
    }
    c[end - 1] = tmp;
  }
}

#ifdef __cplusplus
template <int SIZE>
inline static void
memcpy_wide_bus_read_char_2d(char a_buf[][SIZE], size_t index2_offset, 
                             size_t index1_offset, MARS_WIDE_BUS_TYPE *a,
                            size_t offset_byte, size_t size_byte) {
#pragma HLS inline self
  //#pragma HLS array_partition variable = a_buf cyclic factor = 16
  const size_t data_width = sizeof(char);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t buf_size = size_byte / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t new_offset = offset + buf_size;
  size_t tail_align = (new_offset - 1) & (num_elements - 1);
  size_t start = offset / num_elements;
  size_t end = (offset + buf_size + num_elements - 1) / num_elements;
  //MARS_WIDE_BUS_TYPE *a_offset = a + start;
  size_t i, j;
  size_t index2 = index2_offset, index1 = index1_offset;
  const size_t alignment = (SIZE % num_elements) == 0 &&
                            (index1_offset % num_elements) == 0;
  const size_t bound = SIZE / num_elements;
  const size_t index_offset = index2_offset * SIZE + index1_offset;
  int len = end - start;
  assert(len <= buf_size / num_elements + 2);
  assert(len >= buf_size / num_elements);
  if (1 == len) {
#ifdef __cplusplus
    MARS_WIDE_BUS_TYPE tmp(a[start]);
#else
    MARS_WIDE_BUS_TYPE tmp = a[start];
#endif
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
       if (j < head_align || j > tail_align)
         continue;
       size_t buf_index = j - head_align + index_offset;
        a_buf[buf_index / SIZE][buf_index % SIZE] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
    }
    return;
  }

  for (i = 0; i < len; ++i) {
#pragma HLS pipeline
#ifdef __cplusplus
    MARS_WIDE_BUS_TYPE tmp(a[i + start]);
#else
    MARS_WIDE_BUS_TYPE tmp = a[i + start];
#endif
    if (head_align == 0) {
      for (j = 0; j < num_elements; ++j) {
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buf_index = (i * num_elements + j - 0) + index_offset; 
        if (alignment) {
          a_buf[index2][index1 * num_elements + j] =
              tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        } else
          a_buf[buf_index / SIZE][ buf_index %
                                                   SIZE] =
              tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
      if (alignment) {
        index1++;
        if (index1 == bound) {
          index1 = 0;
          ++index2;
        }
      }

    }
#if 0
    else if (head_align == 1) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 1)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[(i * num_elements + j - 1) / SIZE][(i * num_elements + j - 1) %
                                                 SIZE] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 2) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 2)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[(i * num_elements + j - 2) / SIZE][(i * num_elements + j - 2) %
                                                 SIZE] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 3) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 3)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[(i * num_elements + j - 3) / SIZE][(i * num_elements + j - 3) %
                                                 SIZE] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 4) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 4)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[(i * num_elements + j - 4) / SIZE][(i * num_elements + j - 4) %
                                                 SIZE] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 5) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 5)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[(i * num_elements + j - 5) / SIZE][(i * num_elements + j - 5) %
                                                 SIZE] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 6) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 6)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[(i * num_elements + j - 6) / SIZE][(i * num_elements + j - 6) %
                                                 SIZE] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 7) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 7)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[(i * num_elements + j - 7) / SIZE][(i * num_elements + j - 7) %
                                                 SIZE] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 8) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 8)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[(i * num_elements + j - 8) / SIZE][(i * num_elements + j - 8) %
                                                 SIZE] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 9) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 9)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[(i * num_elements + j - 9) / SIZE][(i * num_elements + j - 9) %
                                                 SIZE] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 10) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 10)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[(i * num_elements + j - 10) / SIZE][(i * num_elements + j - 10) %
                                                  SIZE] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 11) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 11)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[(i * num_elements + j - 11) / SIZE][(i * num_elements + j - 11) %
                                                  SIZE] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 12) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 12)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[(i * num_elements + j - 12) / SIZE][(i * num_elements + j - 12) %
                                                  SIZE] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 13) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 13)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[(i * num_elements + j - 13) / SIZE][(i * num_elements + j - 13) %
                                                  SIZE] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 14) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 14)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[(i * num_elements + j - 14) / SIZE][(i * num_elements + j - 14) %
                                                  SIZE] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 15)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[(i * num_elements + j - 15) / SIZE][(i * num_elements + j - 15) %
                                                  SIZE] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }
#else
    else {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < head_align)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buf_index =(i * num_elements + j - head_align) + index_offset; 
        a_buf[buf_index / SIZE][buf_index % SIZE] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }
#endif

  }
}

template <int SIZE>
inline static void
memcpy_wide_bus_write_char_2d(MARS_WIDE_BUS_TYPE *c, char c_buf[][SIZE],
                             size_t index2_offset, size_t index1_offset,
                             size_t offset_byte, size_t size_byte) {
#pragma HLS inline self
  //#pragma HLS array_partition variable = c_buf cyclic factor = 16
  const size_t data_width = sizeof(char);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t buf_size = size_byte / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t new_offset = offset + buf_size;
  size_t tail_align = (new_offset - 1) & (num_elements - 1);
  size_t start = offset / num_elements;
  size_t end = (offset + buf_size + num_elements - 1) / num_elements;
  size_t len = end - start;
  size_t index_offset = index2_offset * SIZE + index1_offset;
  size_t i, j;
  if (head_align == 0)
    len = (buf_size + num_elements - 1) / num_elements;
  size_t align = 0;
  if (len == 1) {
    MARS_WIDE_BUS_TYPE tmp;
    if (head_align != 0 || tail_align != (num_elements - 1))
      tmp = c[start];
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align)
        continue;
      if (j > tail_align)
        continue;
      size_t buf_index = j - head_align + index_offset;
      tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
           c_buf[buf_index / SIZE][buf_index % SIZE]);
    }
    c[start] = tmp;
    return;
  }
  if (head_align != 0) {
    MARS_WIDE_BUS_TYPE tmp = c[start];
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align)
        continue;
      size_t buf_index = j - head_align + index_offset;
      tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
           c_buf[buf_index / SIZE][ buf_index % SIZE]);
    }
    c[start] = tmp;
    start++;
    align++;
  }
  if (tail_align != (num_elements - 1))
    align++;
  size_t index2 = index2_offset, index1 = index1_offset;
  const size_t alignment = (SIZE % num_elements) == 0 &&
                           (index1_offset % num_elements) == 0;
  const size_t bound = SIZE / num_elements;
  int burst_len = len - align;
  assert(burst_len <= buf_size / num_elements);
  for (i = 0; i < burst_len; ++i) {
#pragma HLS pipeline
    MARS_WIDE_BUS_TYPE tmp;
    if (head_align == 0) {
      for (j = 0; j < num_elements; ++j) {
        int val = alignment ? c_buf[index2][index1 * num_elements + j]
                            : c_buf[(i * num_elements + j - 0) /
                                    SIZE][(i * num_elements + j - 0) % SIZE];
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), val);
      }
      if (alignment) {
        index1++;
        if (index1 == bound) {
          index1 = 0;
          ++index2;
        }
      }
    }
#if 0
    else if (head_align == 1) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[(i * num_elements + j + 15) /
                   SIZE][(i * num_elements + j + 15) % SIZE]);
      }
    }

    else if (head_align == 2) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[(i * num_elements + j + 14) /
                   SIZE][(i * num_elements + j + 14) % SIZE]);
      }
    }

    else if (head_align == 3) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[(i * num_elements + j + 13) /
                   SIZE][(i * num_elements + j + 13) % SIZE]);
      }
    }

    else if (head_align == 4) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[(i * num_elements + j + 12) /
                   SIZE][(i * num_elements + j + 12) % SIZE]);
      }
    }

    else if (head_align == 5) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[(i * num_elements + j + 11) /
                   SIZE][(i * num_elements + j + 11) % SIZE]);
      }
    }

    else if (head_align == 6) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[(i * num_elements + j + 10) /
                   SIZE][(i * num_elements + j + 10) % SIZE]);
      }
    }

    else if (head_align == 7) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[(i * num_elements + j + 9) /
                   SIZE][(i * num_elements + j + 9) % SIZE]);
      }
    }

    else if (head_align == 8) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[(i * num_elements + j + 8) /
                   SIZE][(i * num_elements + j + 8) % SIZE]);
      }
    }

    else if (head_align == 9) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[(i * num_elements + j + 7) /
                   SIZE][(i * num_elements + j + 7) % SIZE]);
      }
    }

    else if (head_align == 10) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[(i * num_elements + j + 6) /
                   SIZE][(i * num_elements + j + 6) % SIZE]);
      }
    }

    else if (head_align == 11) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[(i * num_elements + j + 5) /
                   SIZE][(i * num_elements + j + 5) % SIZE]);
      }
    }

    else if (head_align == 12) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[(i * num_elements + j + 4) /
                   SIZE][(i * num_elements + j + 4) % SIZE]);
      }
    }

    else if (head_align == 13) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[(i * num_elements + j + 3) /
                   SIZE][(i * num_elements + j + 3) % SIZE]);
      }
    }

    else if (head_align == 14) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[(i * num_elements + j + 2) /
                   SIZE][(i * num_elements + j + 2) % SIZE]);
      }
    }

    else {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[(i * num_elements + j + 1) /
                   SIZE][(i * num_elements + j + 1) % SIZE]);
      }
    }
#else
    else {
      for (j = 0; j < num_elements; ++j) {
        size_t buf_index = (i * num_elements + j + num_elements - head_align) + index_offset; 
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buf_index / SIZE][index_offset % SIZE]);
      }
    }
#endif

    c[i + start] = tmp;
  }
  if (tail_align != num_elements - 1) {
    MARS_WIDE_BUS_TYPE tmp = c[end - 1];
    size_t pos = (len - align) * num_elements;
    pos += (num_elements - head_align) % num_elements + index_offset;
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j > tail_align)
        continue;
      tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
           c_buf[(pos + j) / SIZE][(pos + j) % SIZE]);
    }
    c[end - 1] = tmp;
  }
}

template <int SIZE>
inline static void
memcpy_wide_bus_read_int_2d(int a_buf[][SIZE], size_t index2_offset, size_t index1_offset,
                             MARS_WIDE_BUS_TYPE *a,

                            size_t offset_byte, size_t size_byte) {
#pragma HLS inline self
  //#pragma HLS array_partition variable = a_buf cyclic factor = 16
  const size_t data_width = sizeof(int);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t buf_size = size_byte / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t new_offset = offset + buf_size;
  size_t tail_align = (new_offset - 1) & (num_elements - 1);
  size_t start = offset / num_elements;
  size_t end = (offset + buf_size + num_elements - 1) / num_elements;
  //MARS_WIDE_BUS_TYPE *a_offset = a + start;
  size_t i, j;
  size_t index2 = index2_offset, index1 = index1_offset;
  const size_t alignment = (SIZE % num_elements) == 0 &&
                           (index1_offset % num_elements) == 0;
  const size_t bound = SIZE / num_elements;
  const size_t index_offset = index2_offset * SIZE + index1_offset;
  int len = end - start;
  assert(len <= buf_size / num_elements + 2);
  assert(len >= buf_size / num_elements);
  if (1 == len) {
#ifdef __cplusplus
    MARS_WIDE_BUS_TYPE tmp(a[start]);
#else
    MARS_WIDE_BUS_TYPE tmp = a[start];
#endif
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
       if (j < head_align || j > tail_align)
         continue;
       size_t buf_index = j - head_align + index_offset;
        a_buf[buf_index / SIZE][buf_index % SIZE] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
    }
    return;
  }

  for (i = 0; i < len; ++i) {
#pragma HLS pipeline
#ifdef __cplusplus
    MARS_WIDE_BUS_TYPE tmp(a[i + start]);
#else
    MARS_WIDE_BUS_TYPE tmp = a[i + start];
#endif
    if (head_align == 0) {
      for (j = 0; j < num_elements; ++j) {
        if (i == end - start - 1 && j > tail_align)
          continue;
        if (alignment) {
          a_buf[index2][index1 * num_elements + j] =
              tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        } else {
          size_t buf_index = (i * num_elements + j - 0) + index_offset;
          a_buf[ buf_index / SIZE][buf_index % SIZE] =
              tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        }
      }
      if (alignment) {
        index1++;
        if (index1 == bound) {
          index1 = 0;
          ++index2;
        }
      }

    }
#if 0
    else if (head_align == 1) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 1)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[(i * num_elements + j - 1) / SIZE][(i * num_elements + j - 1) %
                                                 SIZE] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 2) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 2)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[(i * num_elements + j - 2) / SIZE][(i * num_elements + j - 2) %
                                                 SIZE] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 3) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 3)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[(i * num_elements + j - 3) / SIZE][(i * num_elements + j - 3) %
                                                 SIZE] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 4) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 4)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[(i * num_elements + j - 4) / SIZE][(i * num_elements + j - 4) %
                                                 SIZE] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 5) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 5)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[(i * num_elements + j - 5) / SIZE][(i * num_elements + j - 5) %
                                                 SIZE] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 6) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 6)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[(i * num_elements + j - 6) / SIZE][(i * num_elements + j - 6) %
                                                 SIZE] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 7) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 7)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[(i * num_elements + j - 7) / SIZE][(i * num_elements + j - 7) %
                                                 SIZE] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 8) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 8)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[(i * num_elements + j - 8) / SIZE][(i * num_elements + j - 8) %
                                                 SIZE] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 9) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 9)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[(i * num_elements + j - 9) / SIZE][(i * num_elements + j - 9) %
                                                 SIZE] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 10) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 10)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[(i * num_elements + j - 10) / SIZE][(i * num_elements + j - 10) %
                                                  SIZE] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 11) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 11)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[(i * num_elements + j - 11) / SIZE][(i * num_elements + j - 11) %
                                                  SIZE] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 12) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 12)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[(i * num_elements + j - 12) / SIZE][(i * num_elements + j - 12) %
                                                  SIZE] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 13) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 13)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[(i * num_elements + j - 13) / SIZE][(i * num_elements + j - 13) %
                                                  SIZE] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 14) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 14)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[(i * num_elements + j - 14) / SIZE][(i * num_elements + j - 14) %
                                                  SIZE] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 15)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[(i * num_elements + j - 15) / SIZE][(i * num_elements + j - 15) %
                                                  SIZE] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }
#else
    else {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < head_align)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buf_index = (i * num_elements + j - head_align) + index_offset;
        a_buf[ buf_index / SIZE][ buf_index % SIZE] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }
#endif

  }
}

template <int SIZE>
inline static void
memcpy_wide_bus_write_int_2d(MARS_WIDE_BUS_TYPE *c, int c_buf[][SIZE],
                             size_t index2_offset, size_t index1_offset,
                             size_t offset_byte, size_t size_byte) {
#pragma HLS inline self
  //#pragma HLS array_partition variable = c_buf cyclic factor = 16
  const size_t data_width = sizeof(int);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t buf_size = size_byte / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t new_offset = offset + buf_size;
  size_t tail_align = (new_offset - 1) & (num_elements - 1);
  size_t start = offset / num_elements;
  size_t end = (offset + buf_size + num_elements - 1) / num_elements;
  size_t len = end - start;
  size_t i, j;
  size_t index_offset = index2_offset * SIZE + index1_offset;
  if (head_align == 0)
    len = (buf_size + num_elements - 1) / num_elements;
  size_t align = 0;
  if (len == 1) {
    MARS_WIDE_BUS_TYPE tmp;
    if (head_align != 0 || tail_align != (num_elements - 1))
      tmp = c[start];
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align)
        continue;
      if (j > tail_align)
        continue;
      size_t buf_index = (j - head_align) + index_offset;
      tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
           c_buf[buf_index / SIZE][ buf_index % SIZE]);
    }
    c[start] = tmp;
    return;
  }
  if (head_align != 0) {
    MARS_WIDE_BUS_TYPE tmp = c[start];
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align)
        continue;
      size_t buf_index = (j - head_align) + index_offset;
      tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
           c_buf[buf_index / SIZE][buf_index % SIZE]);
    }
    c[start] = tmp;
    start++;
    align++;
  }
  if (tail_align != (num_elements - 1))
    align++;
  size_t index2 = index2_offset, index1 = index1_offset;
  const size_t alignment = (SIZE % num_elements) == 0 &&
                           (index1_offset % num_elements) == 0;
  const size_t bound = SIZE / num_elements;
  int burst_len = len - align;
  assert(burst_len <= buf_size / num_elements);
  for (i = 0; i < burst_len; ++i) {
#pragma HLS pipeline
    MARS_WIDE_BUS_TYPE tmp;
    if (head_align == 0) {
      for (j = 0; j < num_elements; ++j) {
        size_t buf_index = (i * num_elements + j - 0) + index_offset; 
        int val = alignment ? c_buf[index2][index1 * num_elements + j]
                            : c_buf[buf_index / SIZE][buf_index % SIZE];
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), val);
      }
      if (alignment) {
        index1++;
        if (index1 == bound) {
          index1 = 0;
          ++index2;
        }
      }
    }
#if 0
    else if (head_align == 1) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[(i * num_elements + j + 15) /
                   SIZE][(i * num_elements + j + 15) % SIZE]);
      }
    }

    else if (head_align == 2) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[(i * num_elements + j + 14) /
                   SIZE][(i * num_elements + j + 14) % SIZE]);
      }
    }

    else if (head_align == 3) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[(i * num_elements + j + 13) /
                   SIZE][(i * num_elements + j + 13) % SIZE]);
      }
    }

    else if (head_align == 4) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[(i * num_elements + j + 12) /
                   SIZE][(i * num_elements + j + 12) % SIZE]);
      }
    }

    else if (head_align == 5) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[(i * num_elements + j + 11) /
                   SIZE][(i * num_elements + j + 11) % SIZE]);
      }
    }

    else if (head_align == 6) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[(i * num_elements + j + 10) /
                   SIZE][(i * num_elements + j + 10) % SIZE]);
      }
    }

    else if (head_align == 7) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[(i * num_elements + j + 9) /
                   SIZE][(i * num_elements + j + 9) % SIZE]);
      }
    }

    else if (head_align == 8) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[(i * num_elements + j + 8) /
                   SIZE][(i * num_elements + j + 8) % SIZE]);
      }
    }

    else if (head_align == 9) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[(i * num_elements + j + 7) /
                   SIZE][(i * num_elements + j + 7) % SIZE]);
      }
    }

    else if (head_align == 10) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[(i * num_elements + j + 6) /
                   SIZE][(i * num_elements + j + 6) % SIZE]);
      }
    }

    else if (head_align == 11) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[(i * num_elements + j + 5) /
                   SIZE][(i * num_elements + j + 5) % SIZE]);
      }
    }

    else if (head_align == 12) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[(i * num_elements + j + 4) /
                   SIZE][(i * num_elements + j + 4) % SIZE]);
      }
    }

    else if (head_align == 13) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[(i * num_elements + j + 3) /
                   SIZE][(i * num_elements + j + 3) % SIZE]);
      }
    }

    else if (head_align == 14) {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[(i * num_elements + j + 2) /
                   SIZE][(i * num_elements + j + 2) % SIZE]);
      }
    }

    else {
      for (j = 0; j < num_elements; ++j) {
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[(i * num_elements + j + 1) /
                   SIZE][(i * num_elements + j + 1) % SIZE]);
      }
    }
#else
    else {
      for (j = 0; j < num_elements; ++j) {
        size_t buf_index = (i * num_elements + j + num_elements - head_align) + index_offset;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[ buf_index / SIZE][ buf_index % SIZE]);
      }
    }
#endif

    c[i + start] = tmp;
  }
  if (tail_align != num_elements - 1) {
    MARS_WIDE_BUS_TYPE tmp = c[end - 1];
    size_t pos = (len - align) * num_elements + index_offset;
    pos += (num_elements - head_align) % num_elements;
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j > tail_align)
        continue;
      tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
           c_buf[(pos + j) / SIZE][(pos + j) % SIZE]);
    }
    c[end - 1] = tmp;
  }
}

template <int SIZE>
inline static void memcpy_wide_bus_read_float_2d(float a_buf[][SIZE],
                                                 size_t index2_offset,
                                                 size_t index1_offset,
                                                 MARS_WIDE_BUS_TYPE *a,
                                                 size_t offset_byte,
                                                 size_t size_byte) {
#pragma HLS inline self
  //#pragma HLS array_partition variable = a_buf cyclic factor = 16
  const size_t data_width = sizeof(float);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t buf_size = size_byte / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t new_offset = offset + buf_size;
  size_t tail_align = (new_offset - 1) & (num_elements - 1);
  size_t start = offset / num_elements;
  size_t end = (offset + buf_size + num_elements - 1) / num_elements;
  //MARS_WIDE_BUS_TYPE *a_offset = a + start;
  size_t i, j;
  size_t index2 = index2_offset, index1 = index1_offset;
  const size_t alignment = (SIZE % num_elements) == 0 &&
                            (index1_offset % SIZE) == 0;
  const size_t index_offset = SIZE * index2_offset + index1_offset;
  const size_t bound = SIZE / num_elements;
  int len = end - start;

  assert(len <= buf_size / num_elements + 2);
  assert(len >= buf_size / num_elements);
  if (1 == len) {
#ifdef __cplusplus
    MARS_WIDE_BUS_TYPE tmp(a[start]);
#else
    MARS_WIDE_BUS_TYPE tmp = a[start];
#endif
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
       if (j < head_align || j > tail_align)
         continue;
       size_t buf_index = j - head_align + index_offset;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buf_index / SIZE][buf_index % SIZE] = *(float *)(&raw_bits);
    }
    return;
  }

  for (i = 0; i < len; ++i) {
#pragma HLS pipeline
#ifdef __cplusplus
    MARS_WIDE_BUS_TYPE tmp(a[i + start]);
#else
    MARS_WIDE_BUS_TYPE tmp = a[i + start];
#endif

    if (head_align == 0) {
      for (j = 0; j < num_elements; ++j) {
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        if (alignment)
          a_buf[index2][index1 * num_elements + j] = *(float *)(&raw_bits);
        else {
          size_t buf_index = (i * num_elements + j - 0) + index_offset; 
          a_buf[buf_index / SIZE][ buf_index % SIZE] =
              *(float *)(&raw_bits);
        }
      }
      if (alignment) {
        index1++;
        if (index1 == bound) {
          index1 = 0;
          ++index2;
        }
      }
    }
#if 0
    else if (head_align == 1) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 1)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[(i * num_elements + j - 1) / SIZE][(i * num_elements + j - 1) %
                                                 SIZE] = *(float *)(&raw_bits);
      }
    }

    else if (head_align == 2) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 2)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[(i * num_elements + j - 2) / SIZE][(i * num_elements + j - 2) %
                                                 SIZE] = *(float *)(&raw_bits);
      }
    }

    else if (head_align == 3) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 3)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[(i * num_elements + j - 3) / SIZE][(i * num_elements + j - 3) %
                                                 SIZE] = *(float *)(&raw_bits);
      }
    }

    else if (head_align == 4) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 4)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[(i * num_elements + j - 4) / SIZE][(i * num_elements + j - 4) %
                                                 SIZE] = *(float *)(&raw_bits);
      }
    }

    else if (head_align == 5) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 5)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[(i * num_elements + j - 5) / SIZE][(i * num_elements + j - 5) %
                                                 SIZE] = *(float *)(&raw_bits);
      }
    }

    else if (head_align == 6) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 6)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[(i * num_elements + j - 6) / SIZE][(i * num_elements + j - 6) %
                                                 SIZE] = *(float *)(&raw_bits);
      }
    }

    else if (head_align == 7) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 7)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[(i * num_elements + j - 7) / SIZE][(i * num_elements + j - 7) %
                                                 SIZE] = *(float *)(&raw_bits);
      }
    }

    else if (head_align == 8) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 8)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[(i * num_elements + j - 8) / SIZE][(i * num_elements + j - 8) %
                                                 SIZE] = *(float *)(&raw_bits);
      }
    }

    else if (head_align == 9) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 9)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[(i * num_elements + j - 9) / SIZE][(i * num_elements + j - 9) %
                                                 SIZE] = *(float *)(&raw_bits);
      }
    }

    else if (head_align == 10) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 10)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[(i * num_elements + j - 10) / SIZE][(i * num_elements + j - 10) %
                                                  SIZE] = *(float *)(&raw_bits);
      }
    }

    else if (head_align == 11) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 11)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[(i * num_elements + j - 11) / SIZE][(i * num_elements + j - 11) %
                                                  SIZE] = *(float *)(&raw_bits);
      }
    }

    else if (head_align == 12) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 12)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[(i * num_elements + j - 12) / SIZE][(i * num_elements + j - 12) %
                                                  SIZE] = *(float *)(&raw_bits);
      }
    }

    else if (head_align == 13) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 13)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[(i * num_elements + j - 13) / SIZE][(i * num_elements + j - 13) %
                                                  SIZE] = *(float *)(&raw_bits);
      }
    }

    else if (head_align == 14) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 14)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[(i * num_elements + j - 14) / SIZE][(i * num_elements + j - 14) %
                                                  SIZE] = *(float *)(&raw_bits);
      }
    }

    else {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 15)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[(i * num_elements + j - 15) / SIZE][(i * num_elements + j - 15) %
                                                  SIZE] = *(float *)(&raw_bits);
      }
    }
#else
    else {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < head_align)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        size_t buf_index = (i * num_elements + j - head_align) + index_offset; 
        a_buf[ buf_index / SIZE][ buf_index  %  SIZE] = *(float *)(&raw_bits);
      }
    }

#endif
  }
}

template <int SIZE>
inline static void memcpy_wide_bus_write_float_2d(MARS_WIDE_BUS_TYPE *c,
                                                  float c_buf[][SIZE],
                                                  size_t index2_offset,
                                                  size_t index1_offset,
                                                  size_t offset_byte,
                                                  size_t size_byte) {
#pragma HLS inline self
  //#pragma HLS array_partition variable = c_buf cyclic factor = 16
  const size_t data_width = sizeof(float);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t buf_size = size_byte / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t new_offset = offset + buf_size;
  size_t tail_align = (new_offset - 1) & (num_elements - 1);
  size_t start = offset / num_elements;
  size_t end = (offset + buf_size + num_elements - 1) / num_elements;
  size_t len = end - start;
  const size_t index_offset = index2_offset * SIZE + index1_offset;
  size_t i, j;
  if (head_align == 0)
    len = (buf_size + num_elements - 1) / num_elements;
  if (len == 1) {
    MARS_WIDE_BUS_TYPE tmp;
    if (head_align != 0 || tail_align != (num_elements - 1))
      tmp = c[start];
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align)
        continue;
      if (j > tail_align)
        continue;
      size_t buf_index = (j - head_align) + index_offset;
      float buf_tmp = c_buf[ buf_index / SIZE][ buf_index % SIZE];
      int raw_bits = *(int *)&buf_tmp;
      tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
    }
    c[start] = tmp;
    return;
  }
  unsigned align = 0;
  if (head_align != 0) {
    MARS_WIDE_BUS_TYPE tmp = c[start];
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align)
        continue;
      size_t buf_index = (j - head_align) + index_offset;
      float buf_tmp = c_buf[ buf_index / SIZE][ buf_index % SIZE];
      int raw_bits = *(int *)&buf_tmp;
      tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
    }
    c[start] = tmp;
    start++;
    align++;
  }
  if (tail_align != (num_elements - 1))
    align++;
  size_t index2 = index2_offset, index1 = index1_offset;
  const size_t alignment = (SIZE % num_elements) == 0 &&
                           (index1_offset % num_elements) == 0;
  const size_t bound = SIZE / num_elements;
  int burst_len = len - align;
  assert(burst_len <= buf_size / num_elements);
  for (i = 0; i < burst_len; ++i) {
#pragma HLS pipeline
    MARS_WIDE_BUS_TYPE tmp;
    if (head_align == 0) {
      for (j = 0; j < num_elements; ++j) {
        size_t buf_index = (i * num_elements + j - 0) + index_offset; 
        float buf_tmp = alignment
                            ? c_buf[index2][index1 * num_elements + j]
                            : c_buf[ buf_index / SIZE][ buf_index % SIZE];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
      if (alignment) {
        index1++;
        if (index1 == bound) {
          index1 = 0;
          ++index2;
        }
      }

    }
#if 0
    else if (head_align == 1) {
      for (j = 0; j < num_elements; ++j) {
        float buf_tmp = c_buf[(i * num_elements + j + 15) /
                              SIZE][(i * num_elements + j + 15) % SIZE];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 2) {
      for (j = 0; j < num_elements; ++j) {
        float buf_tmp = c_buf[(i * num_elements + j + 14) /
                              SIZE][(i * num_elements + j + 14) % SIZE];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 3) {
      for (j = 0; j < num_elements; ++j) {
        float buf_tmp = c_buf[(i * num_elements + j + 13) /
                              SIZE][(i * num_elements + j + 13) % SIZE];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 4) {
      for (j = 0; j < num_elements; ++j) {
        float buf_tmp = c_buf[(i * num_elements + j + 12) /
                              SIZE][(i * num_elements + j + 12) % SIZE];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 5) {
      for (j = 0; j < num_elements; ++j) {
        float buf_tmp = c_buf[(i * num_elements + j + 11) /
                              SIZE][(i * num_elements + j + 11) % SIZE];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 6) {
      for (j = 0; j < num_elements; ++j) {
        float buf_tmp = c_buf[(i * num_elements + j + 10) /
                              SIZE][(i * num_elements + j + 10) % SIZE];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 7) {
      for (j = 0; j < num_elements; ++j) {
        float buf_tmp = c_buf[(i * num_elements + j + 9) /
                              SIZE][(i * num_elements + j + 9) % SIZE];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 8) {
      for (j = 0; j < num_elements; ++j) {
        float buf_tmp = c_buf[(i * num_elements + j + 8) /
                              SIZE][(i * num_elements + j + 8) % SIZE];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 9) {
      for (j = 0; j < num_elements; ++j) {
        float buf_tmp = c_buf[(i * num_elements + j + 7) /
                              SIZE][(i * num_elements + j + 7) % SIZE];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 10) {
      for (j = 0; j < num_elements; ++j) {
        float buf_tmp = c_buf[(i * num_elements + j + 6) /
                              SIZE][(i * num_elements + j + 6) % SIZE];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 11) {
      for (j = 0; j < num_elements; ++j) {
        float buf_tmp = c_buf[(i * num_elements + j + 5) /
                              SIZE][(i * num_elements + j + 5) % SIZE];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 12) {
      for (j = 0; j < num_elements; ++j) {
        float buf_tmp = c_buf[(i * num_elements + j + 4) /
                              SIZE][(i * num_elements + j + 4) % SIZE];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 13) {
      for (j = 0; j < num_elements; ++j) {
        float buf_tmp = c_buf[(i * num_elements + j + 3) /
                              SIZE][(i * num_elements + j + 3) % SIZE];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 14) {
      for (j = 0; j < num_elements; ++j) {
        float buf_tmp = c_buf[(i * num_elements + j + 2) /
                              SIZE][(i * num_elements + j + 2) % SIZE];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else {
      for (j = 0; j < num_elements; ++j) {
        float buf_tmp = c_buf[(i * num_elements + j + 1) /
                              SIZE][(i * num_elements + j + 1) % SIZE];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }
#else
    else {
      for (j = 0; j < num_elements; ++j) {
        size_t index = i * num_elements + j + num_elements - 
            head_align + index_offset;
        float buf_tmp = c_buf[index / SIZE][index % SIZE];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

#endif
    c[i + start] = tmp;
  }
  if (tail_align != num_elements - 1) {
    MARS_WIDE_BUS_TYPE tmp = c[end - 1];
    size_t pos = (len - align) * num_elements + index_offset;
    pos += (num_elements - head_align) % num_elements;
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j > tail_align)
        continue;
      float buf_tmp = c_buf[(pos + j) / SIZE][(pos + j) % SIZE];
      int raw_bits = *(int *)&buf_tmp;
      tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
    }
    c[end - 1] = tmp;
  }
}

template <int SIZE>
inline static void memcpy_wide_bus_read_double_2d(double a_buf[][SIZE],
                                                  size_t index2_offset,
                                                  size_t index1_offset,
                                                  MARS_WIDE_BUS_TYPE *a,
                                                  size_t offset_byte,
                                                  size_t size_byte) {
#pragma HLS inline self
  //#pragma HLS array_partition variable = a_buf cyclic factor = 8
  const size_t data_width = sizeof(double);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t buf_size = size_byte / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t new_offset = offset + buf_size;
  size_t tail_align = (new_offset - 1) & (num_elements - 1);
  size_t start = offset / num_elements;
  size_t end = (offset + buf_size + num_elements - 1) / num_elements;
  //MARS_WIDE_BUS_TYPE *a_offset = a + start;
  size_t i, j;
  size_t index2 = index2_offset, index1 = index1_offset;
  const size_t alignment = (SIZE % num_elements) == 0 &&
                           (index1_offset % num_elements) == 0;
  const size_t bound = SIZE / num_elements;
  const size_t index_offset = SIZE * index2_offset + index1_offset;

  int len = end - start;
  assert(len <= buf_size / num_elements + 2);
  assert(len >= buf_size / num_elements);
  if (1 == len) {
#ifdef __cplusplus
    MARS_WIDE_BUS_TYPE tmp(a[start]);
#else
    MARS_WIDE_BUS_TYPE tmp = a[start];
#endif
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
       if (j < head_align || j > tail_align)
         continue;
       long raw_bits =
           tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
       size_t buf_index = (j - head_align) + index_offset;
       a_buf[ buf_index / SIZE][buf_index % SIZE] = 
         *(double *)(&raw_bits);
    }
    return;
  }
  for (i = 0; i < len; ++i) {
#pragma HLS pipeline
#ifdef __cplusplus
    MARS_WIDE_BUS_TYPE tmp(a[i + start]);
#else
    MARS_WIDE_BUS_TYPE tmp = a[i + start];
#endif
    if (head_align == 0) {
      for (j = 0; j < num_elements; ++j) {
        if (i == end - start - 1 && j > tail_align)
          continue;
        long raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        size_t buf_index = (i * num_elements + j - 0) + index_offset;
        if (alignment)
          a_buf[index1][index2 * num_elements + j] = *(double *)(&raw_bits);
        else
          a_buf[ buf_index  / SIZE][buf_index %
                                                   SIZE] =
              *(double *)(&raw_bits);
      }
      if (alignment) {
        index1++;
        if (index1 == bound) {
          index1 = 0;
          ++index2;
        }
      }
    }
#if 0
    else if (head_align == 1) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 1)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        long raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[(i * num_elements + j - 1) / SIZE][(i * num_elements + j - 1) %
                                                 SIZE] = *(double *)(&raw_bits);
      }
    }

    else if (head_align == 2) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 2)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        long raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[(i * num_elements + j - 2) / SIZE][(i * num_elements + j - 2) %
                                                 SIZE] = *(double *)(&raw_bits);
      }
    }

    else if (head_align == 3) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 3)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        long raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[(i * num_elements + j - 3) / SIZE][(i * num_elements + j - 3) %
                                                 SIZE] = *(double *)(&raw_bits);
      }
    }

    else if (head_align == 4) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 4)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        long raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[(i * num_elements + j - 4) / SIZE][(i * num_elements + j - 4) %
                                                 SIZE] = *(double *)(&raw_bits);
      }
    }

    else if (head_align == 5) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 5)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        long raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[(i * num_elements + j - 5) / SIZE][(i * num_elements + j - 5) %
                                                 SIZE] = *(double *)(&raw_bits);
      }
    }

    else if (head_align == 6) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 6)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        long raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[(i * num_elements + j - 6) / SIZE][(i * num_elements + j - 6) %
                                                 SIZE] = *(double *)(&raw_bits);
      }
    }

    else {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 7)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        long raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[(i * num_elements + j - 7) / SIZE][(i * num_elements + j - 7) %
                                                 SIZE] = *(double *)(&raw_bits);
      }
    }
#else
    else {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < head_align)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t index = i * num_elements + j - head_align + index_offset;
        long raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[index / SIZE][index % SIZE] = *(double *)(&raw_bits);
      }
    }
#endif
  }
}

template <int SIZE>
inline static void memcpy_wide_bus_write_double_2d(MARS_WIDE_BUS_TYPE *c,
                                                   double c_buf[][SIZE],
                                                   size_t index2_offset,
                                                   size_t index1_offset,
                                                   size_t offset_byte,
                                                   size_t size_byte) {
#pragma HLS inline self
  //#pragma HLS array_partition variable = c_buf cyclic factor = 8
  const size_t data_width = sizeof(double);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t buf_size = size_byte / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t new_offset = offset + buf_size;
  size_t tail_align = (new_offset - 1) & (num_elements - 1);
  size_t start = offset / num_elements;
  size_t end = (offset + buf_size + num_elements - 1) / num_elements;
  size_t len = end - start;
  size_t index_offset = index2_offset * SIZE + index1_offset;
  size_t i, j;
  if (head_align == 0)
    len = (buf_size + num_elements - 1) / num_elements;
  if (len == 1) {
    MARS_WIDE_BUS_TYPE tmp;
    if (head_align != 0 || tail_align != (num_elements - 1))
      tmp = c[start];
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align)
        continue;
      if (j > tail_align)
        continue;
      size_t buf_index = (j - head_align) + index_offset;
      double buf_tmp = c_buf[ buf_index / SIZE][buf_index % SIZE];
      long raw_bits = *(long *)&buf_tmp;
      tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
    }
    c[start] = tmp;
    return;
  }
  size_t align = 0;
  if (head_align != 0) {
    MARS_WIDE_BUS_TYPE tmp = c[start];
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align)
        continue;
      size_t buf_index = (j - head_align) + index_offset;
      double buf_tmp = c_buf[ buf_index / SIZE][buf_index % SIZE];
      long raw_bits = *(long *)&buf_tmp;
      tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
    }
    c[start] = tmp;
    start++;
    align++;
  }
  if (tail_align != (num_elements - 1))
    align++;
  size_t index2 = index2_offset, index1 = index1_offset;
  const size_t alignment = (SIZE % num_elements) == 0 &&
                           (index1_offset % num_elements) == 0;
  const size_t bound = SIZE / num_elements;
  int burst_len = len - align;
  assert(burst_len <= buf_size / num_elements);
  for (i = 0; i < burst_len; ++i) {
#pragma HLS pipeline
    MARS_WIDE_BUS_TYPE tmp;
    if (head_align == 0) {
      for (j = 0; j < num_elements; ++j) {
        size_t buf_index = (i * num_elements + j - 0) + index_offset;
        double buf_tmp = alignment
                             ? c_buf[index2][index1 * num_elements + j]
                             : c_buf[ buf_index / SIZE][ buf_index % SIZE];
        long raw_bits = *(long *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
      if (alignment) {
        index1++;
        if (index1 == bound) {
          index1 = 0;
          ++index2;
        }
      }

    }
#if 0
    else if (head_align == 1) {
      for (j = 0; j < num_elements; ++j) {
        double buf_tmp = c_buf[(i * num_elements + j + 7) /
                               SIZE][(i * num_elements + j + 7) % SIZE];
        long raw_bits = *(long *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 2) {
      for (j = 0; j < num_elements; ++j) {
        double buf_tmp = c_buf[(i * num_elements + j + 6) /
                               SIZE][(i * num_elements + j + 6) % SIZE];
        long raw_bits = *(long *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 3) {
      for (j = 0; j < num_elements; ++j) {
        double buf_tmp = c_buf[(i * num_elements + j + 5) /
                               SIZE][(i * num_elements + j + 5) % SIZE];
        long raw_bits = *(long *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 4) {
      for (j = 0; j < num_elements; ++j) {
        double buf_tmp = c_buf[(i * num_elements + j + 4) /
                               SIZE][(i * num_elements + j + 4) % SIZE];
        long raw_bits = *(long *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 5) {
      for (j = 0; j < num_elements; ++j) {
        double buf_tmp = c_buf[(i * num_elements + j + 3) /
                               SIZE][(i * num_elements + j + 3) % SIZE];
        long raw_bits = *(long *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 6) {
      for (j = 0; j < num_elements; ++j) {
        double buf_tmp = c_buf[(i * num_elements + j + 2) /
                               SIZE][(i * num_elements + j + 2) % SIZE];
        long raw_bits = *(long *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else {
      for (j = 0; j < num_elements; ++j) {
        double buf_tmp = c_buf[(i * num_elements + j + 1) /
                               SIZE][(i * num_elements + j + 1) % SIZE];
        long raw_bits = *(long *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }
#else
    else {
      for (j = 0; j < num_elements; ++j) {
        size_t index = i * num_elements + j + num_elements - head_align + index_offset;
        double buf_tmp = c_buf[ index / SIZE][ index % SIZE];
        long raw_bits = *(long *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }
#endif
    c[i + start] = tmp;
  }
  if (tail_align != num_elements - 1) {
    MARS_WIDE_BUS_TYPE tmp = c[end - 1];
    size_t pos = (len - align) * num_elements + index_offset;
    pos += (num_elements - head_align) % num_elements;
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j > tail_align)
        continue;
      double buf_tmp = c_buf[(pos + j) / SIZE][(pos + j) % SIZE];
      long raw_bits = *(long *)&buf_tmp;
      tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
    }
    c[end - 1] = tmp;
  }
}

template <int SIZE_2, int SIZE_1>
inline static void
memcpy_wide_bus_read_char_3d(char a_buf[][SIZE_2][SIZE_1], 
                            size_t index3_offset, size_t index2_offset,
                            size_t index1_offset, MARS_WIDE_BUS_TYPE *a,
                            size_t offset_byte, size_t size_byte) {
#pragma HLS inline self
  //#pragma HLS array_partition variable = a_buf cyclic factor = 16
  const size_t data_width = sizeof(char);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t buf_size = size_byte / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t new_offset = offset + buf_size;
  size_t tail_align = (new_offset - 1) & (num_elements - 1);
  size_t start = offset / num_elements;
  size_t end = (offset + buf_size + num_elements - 1) / num_elements;
  //MARS_WIDE_BUS_TYPE *a_offset = a + start;
  size_t i, j;
  size_t index3 = index3_offset, index2 = index2_offset, index1 = index1_offset;
  const size_t alignment = (SIZE_1 % num_elements) == 0 &&
                            (index1_offset % num_elements) == 0;
  const size_t bound = SIZE_1 / num_elements;
  const size_t SIZE = SIZE_1 * SIZE_2;
  const size_t index_offset = index3_offset * SIZE + index2_offset * 
                              SIZE_1 + index1_offset;
  int len = end - start;
  assert(len <= buf_size / num_elements + 2);
  assert(len >= buf_size / num_elements);
  if (1 == len) {
#ifdef __cplusplus
    MARS_WIDE_BUS_TYPE tmp(a[start]);
#else
    MARS_WIDE_BUS_TYPE tmp = a[start];
#endif
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
       if (j < head_align || j > tail_align)
         continue;
       size_t buffer_index = j - head_align + index_offset;
       a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
           tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
    }
    return;
  }
  for (i = 0; i < len; ++i) {
#pragma HLS pipeline
#ifdef __cplusplus
    MARS_WIDE_BUS_TYPE tmp(a[i + start]);
#else
    MARS_WIDE_BUS_TYPE tmp = a[i + start];
#endif
    if (head_align == 0) {
      for (j = 0; j < num_elements; ++j) {
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 0;
        if (alignment) {
          a_buf[index3][index2][index1 * num_elements + j] =
              tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        } else
          a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                          SIZE_1] =
              tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
      if (alignment) {
        index1++;
        if (index1 == bound) {
          index1 = 0;
          ++index2;
        }
        if (index2 == SIZE_2) {
          index2 = 0;
          ++index3;
        }
      }

    }
#if 0
    else if (head_align == 1) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 1)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 1;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 2) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 2)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 2;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 3) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 3)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 3;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 4) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 4)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 4;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 5) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 5)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 5;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 6) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 6)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 6;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 7) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 7)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 7;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 8) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 8)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 8;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 9) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 9)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 9;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 10) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 10)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 10;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 11) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 11)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 11;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 12) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 12)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 12;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 13) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 13)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 13;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 14) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 14)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 14;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 15)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 15;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }
#else
    else {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < head_align)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - head_align + index_offset;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }
#endif
  }
}

template <int SIZE_1, int SIZE_2>
inline static void
memcpy_wide_bus_write_char_3d(MARS_WIDE_BUS_TYPE *c, char c_buf[][SIZE_2][SIZE_1],
                             size_t index3_offset, size_t index2_offset, 
                             size_t index1_offset,
                             size_t offset_byte, size_t size_byte) {
#pragma HLS inline self
  //#pragma HLS array_partition variable = c_buf cyclic factor = 16
  const size_t data_width = sizeof(char);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t buf_size = size_byte / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t new_offset = offset + buf_size;
  size_t tail_align = (new_offset - 1) & (num_elements - 1);
  size_t start = offset / num_elements;
  size_t end = (offset + buf_size + num_elements - 1) / num_elements;
  size_t len = end - start;
  size_t i, j;
  const size_t SIZE = SIZE_1 * SIZE_2;
  size_t index_offset = SIZE * index3_offset + SIZE_1 * index2_offset +
                        index1_offset;
  if (head_align == 0)
    len = (buf_size + num_elements - 1) / num_elements;
  size_t align = 0;
  if (len == 1) {
    MARS_WIDE_BUS_TYPE tmp;
    if (head_align != 0 || tail_align != (num_elements - 1))
      tmp = c[start];
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align)
        continue;
      if (j > tail_align)
        continue;
      size_t buffer_index = j - head_align + index_offset;
      tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
           c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                           SIZE_1]);
    }
    c[start] = tmp;
    return;
  }
  if (head_align != 0) {
    MARS_WIDE_BUS_TYPE tmp = c[start];
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align)
        continue;
      size_t buffer_index = j - head_align + index_offset;
      tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
           c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][
                 (buffer_index % SIZE) % SIZE_1]);
    }
    c[start] = tmp;
    start++;
    align++;
  }
  if (tail_align != (num_elements - 1))
    align++;
  size_t index3 = index3_offset, index2 = index2_offset, index1 = index1_offset;
  const size_t alignment = (SIZE_1 % num_elements) == 0 &&
                           (index1_offset % num_elements) == 0;
  const size_t bound = SIZE_1 / num_elements;
  int burst_len = len - align;
  assert(burst_len <= buf_size / num_elements);
  for (i = 0; i < burst_len; ++i) {
#pragma HLS pipeline
    MARS_WIDE_BUS_TYPE tmp;
    if (head_align == 0) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j - 0 + index_offset;
        int val = alignment
                      ? c_buf[index3][index2][index1 * num_elements + j]
                      : c_buf[buffer_index / SIZE][
                              (buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1];
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), val);
      }
      if (alignment) {
        index1++;
        if (index1 == bound) {
          index1 = 0;
          ++index2;
        }
        if (index2 == SIZE_2) {
          index2 = 0;
          ++index3;
        }
      }
    }
#if 0
    else if (head_align == 1) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 15;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 2) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 14;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 3) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 13;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 4) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 12;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 5) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 11;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 6) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 10;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 7) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 9;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 8) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 8;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 9) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 7;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 10) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 6;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 11) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 5;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 12) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 4;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 13) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 3;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 14) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 2;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 1;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }
#else
    else {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + num_elements - head_align + index_offset;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }
#endif
    c[i + start] = tmp;
  }
  if (tail_align != num_elements - 1) {
    MARS_WIDE_BUS_TYPE tmp = c[end - 1];
    size_t pos = (len - align) * num_elements + index_offset;
    pos += (num_elements - head_align) % num_elements;
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j > tail_align)
        continue;
      size_t buffer_index = pos + j;
      tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
           c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                           SIZE_1]);
    }
    c[end - 1] = tmp;
  }
}

template <int SIZE_2, int SIZE_1>
inline static void
memcpy_wide_bus_read_int_3d(int a_buf[][SIZE_2][SIZE_1], 
                                                   size_t index3_offset,
                                                   size_t index2_offset,
                                                   size_t index1_offset,
                            MARS_WIDE_BUS_TYPE *a,
                            size_t offset_byte, size_t size_byte) {
#pragma HLS inline self
  //#pragma HLS array_partition variable = a_buf cyclic factor = 16
  const size_t data_width = sizeof(int);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t buf_size = size_byte / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t new_offset = offset + buf_size;
  size_t tail_align = (new_offset - 1) & (num_elements - 1);
  size_t start = offset / num_elements;
  size_t end = (offset + buf_size + num_elements - 1) / num_elements;
  //MARS_WIDE_BUS_TYPE *a_offset = a + start;
  size_t i, j;
  size_t index3 = index3_offset, index2 = index2_offset, index1 = index1_offset;
  const size_t alignment = (SIZE_1 % num_elements) == 0 &&
                            (index1_offset % num_elements) == 0;
  const size_t bound = SIZE_1 / num_elements;
  const size_t SIZE = SIZE_1 * SIZE_2;
  const size_t index_offset = index3_offset * SIZE + index2_offset * SIZE_1 +
                              index1_offset;
  int len = end - start;
  assert(len <= buf_size / num_elements + 2);
  assert(len >= buf_size / num_elements);
  if (1 == len) {
#ifdef __cplusplus
    MARS_WIDE_BUS_TYPE tmp(a[start]);
#else
    MARS_WIDE_BUS_TYPE tmp = a[start];
#endif
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
       if (j < head_align || j > tail_align)
         continue;
       size_t buffer_index = j - head_align + index_offset;
       a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
           tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
    }
    return;
  }
  for (i = 0; i < len; ++i) {
#pragma HLS pipeline
#ifdef __cplusplus
    MARS_WIDE_BUS_TYPE tmp(a[i + start]);
#else
    MARS_WIDE_BUS_TYPE tmp = a[i + start];
#endif
    if (head_align == 0) {
      for (j = 0; j < num_elements; ++j) {
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 0 + index_offset;
        if (alignment) {
          a_buf[index3][index2][index1 * num_elements + j] =
              tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        } else
          a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                          SIZE_1] =
              tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
      if (alignment) {
        index1++;
        if (index1 == bound) {
          index1 = 0;
          ++index2;
        }
        if (index2 == SIZE_2) {
          index2 = 0;
          ++index3;
        }
      }

    }
#if 0
    else if (head_align == 1) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 1)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 1;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 2) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 2)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 2;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 3) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 3)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 3;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 4) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 4)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 4;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 5) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 5)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 5;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 6) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 6)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 6;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 7) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 7)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 7;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 8) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 8)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 8;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 9) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 9)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 9;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 10) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 10)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 10;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 11) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 11)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 11;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 12) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 12)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 12;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 13) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 13)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 13;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 14) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 14)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 14;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 15)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 15;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }
#else
    else {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < head_align)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - head_align + index_offset;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }
#endif
  }
}

template <int SIZE_1, int SIZE_2>
inline static void
memcpy_wide_bus_write_int_3d(MARS_WIDE_BUS_TYPE *c, int c_buf[][SIZE_2][SIZE_1],
                                                   size_t index3_offset,
                                                   size_t index2_offset,
                                                   size_t index1_offset,
                             size_t offset_byte, size_t size_byte) {
#pragma HLS inline self
  //#pragma HLS array_partition variable = c_buf cyclic factor = 16
  const size_t data_width = sizeof(int);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t buf_size = size_byte / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t new_offset = offset + buf_size;
  size_t tail_align = (new_offset - 1) & (num_elements - 1);
  size_t start = offset / num_elements;
  size_t end = (offset + buf_size + num_elements - 1) / num_elements;
  size_t len = end - start;
  size_t i, j;
  const size_t SIZE = SIZE_1 * SIZE_2;
  const size_t index_offset = index3_offset * SIZE + index2_offset * SIZE_1 +
                              index1_offset;
  if (head_align == 0)
    len = (buf_size + num_elements - 1) / num_elements;
  size_t align = 0;
  if (len == 1) {
    MARS_WIDE_BUS_TYPE tmp;
    if (head_align != 0 || tail_align != (num_elements - 1))
      tmp = c[start];
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align)
        continue;
      if (j > tail_align)
        continue;
      size_t buffer_index = j - head_align + index_offset ;
      tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
           c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                           SIZE_1]);
    }
    c[start] = tmp;
    return;
  }
  if (head_align != 0) {
    MARS_WIDE_BUS_TYPE tmp = c[start];
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align)
        continue;
      size_t buffer_index = j - head_align + index_offset;
      tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
           c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][
                 (buffer_index % SIZE) % SIZE_1]);
    }
    c[start] = tmp;
    start++;
    align++;
  }
  if (tail_align != (num_elements - 1))
    align++;
  size_t index3 = index3_offset, index2 = index2_offset, index1 = index1_offset;
  const size_t alignment = (SIZE_1 % num_elements) == 0 &&
                            (index1_offset % num_elements) == 0;
  const size_t bound = SIZE_1 / num_elements;
  int burst_len = len - align;
  assert(burst_len <= buf_size / num_elements);
  for (i = 0; i < burst_len; ++i) {
#pragma HLS pipeline
    MARS_WIDE_BUS_TYPE tmp;
    if (head_align == 0) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j - 0 + index_offset;
        int val = alignment
                      ? c_buf[index3][index2][index1 * num_elements + j]
                      : c_buf[buffer_index / SIZE][
                              (buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1];
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), val);
      }
      if (alignment) {
        index1++;
        if (index1 == bound) {
          index1 = 0;
          ++index2;
        }
        if (index2 == SIZE_2) {
          index2 = 0;
          ++index3;
        }
      }
    }
#if 0
    else if (head_align == 1) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 15;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 2) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 14;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 3) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 13;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 4) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 12;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 5) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 11;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 6) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 10;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 7) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 9;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 8) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 8;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 9) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 7;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 10) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 6;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 11) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 5;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 12) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 4;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 13) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 3;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 14) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 2;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 1;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }
#else
    else {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + num_elements - head_align + index_offset;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }
#endif
    c[i + start] = tmp;
  }
  if (tail_align != num_elements - 1) {
    MARS_WIDE_BUS_TYPE tmp = c[end - 1];
    size_t pos = (len - align) * num_elements+ index_offset;
    pos += (num_elements - head_align) % num_elements;
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j > tail_align)
        continue;
      size_t buffer_index = pos + j;
      tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
           c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                           SIZE_1]);
    }
    c[end - 1] = tmp;
  }
}

template <int SIZE_1, int SIZE_2>
inline static void memcpy_wide_bus_read_float_3d(float a_buf[][SIZE_2][SIZE_1],
                                                   size_t index3_offset,
                                                   size_t index2_offset,
                                                   size_t index1_offset,
                                                 MARS_WIDE_BUS_TYPE *a,
                                                 size_t offset_byte,
                                                 size_t size_byte) {
#pragma HLS inline self
  //#pragma HLS array_partition variable = a_buf cyclic factor = 16
  const size_t data_width = sizeof(float);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t buf_size = size_byte / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t new_offset = offset + buf_size;
  size_t tail_align = (new_offset - 1) & (num_elements - 1);
  size_t start = offset / num_elements;
  size_t end = (offset + buf_size + num_elements - 1) / num_elements;
  //MARS_WIDE_BUS_TYPE *a_offset = a + start;
  size_t i, j;
  size_t index3 = index3_offset, index2 = index2_offset, index1 = index1_offset;
  const size_t alignment = (SIZE_1 % num_elements) == 0 &&
                            (index1_offset % num_elements) == 0;
  const size_t bound = SIZE_1 / num_elements;
  const size_t SIZE = SIZE_2 * SIZE_1;
  const size_t index_offset = index3_offset * SIZE + index2_offset * SIZE_1 +
                              index1_offset;
  int len = end - start;
  assert(len <= buf_size / num_elements + 2);
  assert(len >= buf_size / num_elements);
  if (1 == len) {
#ifdef __cplusplus
    MARS_WIDE_BUS_TYPE tmp(a[start]);
#else
    MARS_WIDE_BUS_TYPE tmp = a[start];
#endif
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align || j > tail_align)
         continue;
      size_t buffer_index = j - head_align + index_offset;
      int raw_bits =
          tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
          *(float *)(&raw_bits);
    }
    return;
  }
  for (i = 0; i < len; ++i) {
#pragma HLS pipeline
#ifdef __cplusplus
    MARS_WIDE_BUS_TYPE tmp(a[i + start]);
#else
    MARS_WIDE_BUS_TYPE tmp = a[i + start];
#endif

    if (head_align == 0) {
      for (j = 0; j < num_elements; ++j) {
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j + 0 + index_offset;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        if (alignment)
          a_buf[index3][index2][index1 * num_elements + j] =
              *(float *)(&raw_bits);
        else
          a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                          SIZE_1] =
              *(float *)(&raw_bits);
      }
      if (alignment) {
        index1++;
        if (index1 == bound) {
          index1 = 0;
          ++index2;
        }
        if (index2 == SIZE_2) {
          index2 = 0;
          ++index3;
        }
      }
    }
#if 0
    else if (head_align == 1) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 1)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 1;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            *(float *)(&raw_bits);
      }
    }

    else if (head_align == 2) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 2)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 2;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            *(float *)(&raw_bits);
      }
    }

    else if (head_align == 3) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 3)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 3;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            *(float *)(&raw_bits);
      }
    }

    else if (head_align == 4) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 4)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 4;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            *(float *)(&raw_bits);
      }
    }

    else if (head_align == 5) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 5)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 5;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            *(float *)(&raw_bits);
      }
    }

    else if (head_align == 6) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 6)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 6;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            *(float *)(&raw_bits);
      }
    }

    else if (head_align == 7) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 7)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 7;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            *(float *)(&raw_bits);
      }
    }

    else if (head_align == 8) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 8)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 8;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            *(float *)(&raw_bits);
      }
    }

    else if (head_align == 9) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 9)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 9;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            *(float *)(&raw_bits);
      }
    }

    else if (head_align == 10) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 10)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 10;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            *(float *)(&raw_bits);
      }
    }

    else if (head_align == 11) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 11)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 11;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            *(float *)(&raw_bits);
      }
    }

    else if (head_align == 12) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 12)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 12;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            *(float *)(&raw_bits);
      }
    }

    else if (head_align == 13) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 13)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 13;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            *(float *)(&raw_bits);
      }
    }

    else if (head_align == 14) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 14)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 14;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            *(float *)(&raw_bits);
      }
    }

    else {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 15)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 15;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            *(float *)(&raw_bits);
      }
    }
#else
    else {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < head_align)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - head_align + index_offset;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            *(float *)(&raw_bits);
      }
    }
#endif
  }
}

template <int SIZE_1, int SIZE_2>
inline static void memcpy_wide_bus_write_float_3d(MARS_WIDE_BUS_TYPE *c,
                                                  float c_buf[][SIZE_2][SIZE_1],
                                                   size_t index3_offset,
                                                   size_t index2_offset,
                                                   size_t index1_offset,
                                                  size_t offset_byte,
                                                  size_t size_byte) {
#pragma HLS inline self
  //#pragma HLS array_partition variable = c_buf cyclic factor = 16
  const size_t data_width = sizeof(float);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t buf_size = size_byte / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t new_offset = offset + buf_size;
  size_t tail_align = (new_offset - 1) & (num_elements - 1);
  size_t start = offset / num_elements;
  size_t end = (offset + buf_size + num_elements - 1) / num_elements;
  size_t len = end - start;
  size_t i, j;
  const size_t SIZE = SIZE_1 * SIZE_2;
  const size_t index_offset = index3_offset * SIZE + index2_offset * SIZE_1 +
                              index1_offset;
  if (head_align == 0)
    len = (buf_size + num_elements - 1) / num_elements;
  if (len == 1) {
    MARS_WIDE_BUS_TYPE tmp;
    if (head_align != 0 || tail_align != (num_elements - 1))
      tmp = c[start];
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align)
        continue;
      if (j > tail_align)
        continue;
      size_t buffer_index = j - head_align + index_offset;
      float buf_tmp = c_buf[buffer_index / SIZE][
        (buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1];
      int raw_bits = *(int *)&buf_tmp;
      tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
    }
    c[start] = tmp;
    return;
  }
  unsigned align = 0;
  if (head_align != 0) {
    MARS_WIDE_BUS_TYPE tmp = c[start];
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align)
        continue;
      size_t buffer_index = j - head_align + index_offset;
      float buf_tmp = c_buf[buffer_index / SIZE][
        (buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1];
      int raw_bits = *(int *)&buf_tmp;
      tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
    }
    c[start] = tmp;
    start++;
    align++;
  }
  if (tail_align != (num_elements - 1))
    align++;
  size_t index3 = index3_offset, index2 = index2_offset, index1 = index1_offset;
  const size_t alignment = (SIZE_1 % num_elements) == 0 &&
                            (index1_offset % num_elements) == 0;
  const size_t bound = SIZE_1 / num_elements;
  int burst_len = len - align;
  assert(burst_len <= buf_size / num_elements);
  for (i = 0; i < burst_len; ++i) {
#pragma HLS pipeline
    MARS_WIDE_BUS_TYPE tmp;
    if (head_align == 0) {
      for (j = 0; j < num_elements; ++j) {

        size_t buffer_index = i * num_elements + j + index_offset;
        float buf_tmp =
            alignment ? c_buf[index3][index2][index1 * num_elements + j]
                      : c_buf[buffer_index / SIZE][
                        (buffer_index % SIZE) /SIZE_1][(buffer_index % SIZE) % SIZE_1];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
      if (alignment) {
        index1++;
        if (index1 == bound) {
          index1 = 0;
          ++index2;
        }
        if (index2 == SIZE_2) {
          index2 = 0;
          ++index3;
        }
      }

    }
#if 0
    else if (head_align == 1) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 15;
        float buf_tmp = c_buf[buffer_index / SIZE][(buffer_index % SIZE) /SIZE_1][(buffer_index % SIZE) % SIZE_1];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 2) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 14;
        float buf_tmp = c_buf[buffer_index / SIZE][(buffer_index % SIZE) /SIZE_1][(buffer_index % SIZE) % SIZE_1];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 3) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 13;
        float buf_tmp = c_buf[buffer_index / SIZE][(buffer_index % SIZE) /SIZE_1][(buffer_index % SIZE) % SIZE_1];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 4) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 12;
        float buf_tmp = c_buf[buffer_index / SIZE][(buffer_index % SIZE) /SIZE_1][(buffer_index % SIZE) % SIZE_1];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 5) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 11;
        float buf_tmp = c_buf[buffer_index / SIZE][(buffer_index % SIZE) /SIZE_1][(buffer_index % SIZE) % SIZE_1];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 6) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 10;
        float buf_tmp = c_buf[buffer_index / SIZE][(buffer_index % SIZE) /SIZE_1][(buffer_index % SIZE) % SIZE_1];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 7) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 9;
        float buf_tmp = c_buf[buffer_index / SIZE][(buffer_index % SIZE) /SIZE_1][(buffer_index % SIZE) % SIZE_1];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 8) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 8;
        float buf_tmp = c_buf[buffer_index / SIZE][(buffer_index %SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 9) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 7;
        float buf_tmp = c_buf[buffer_index / SIZE][(buffer_index % SIZE) /SIZE_1][(buffer_index % SIZE) % SIZE_1];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 10) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 6;
        float buf_tmp = c_buf[buffer_index / SIZE][(buffer_index % SIZE) /SIZE_1][(buffer_index % SIZE) % SIZE_1];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 11) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 5;
        float buf_tmp = c_buf[buffer_index / SIZE][(buffer_index % SIZE) /SIZE_1][(buffer_index % SIZE) % SIZE_1];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 12) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 4;
        float buf_tmp = c_buf[buffer_index / SIZE][(buffer_index % SIZE) /SIZE_1][(buffer_index % SIZE) % SIZE_1];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 13) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 3;
        float buf_tmp = c_buf[buffer_index / SIZE][(buffer_index % SIZE) /SIZE_1][(buffer_index % SIZE) % SIZE_1];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 14) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 2;
        float buf_tmp = c_buf[buffer_index / SIZE][(buffer_index % SIZE) /SIZE_1][(buffer_index % SIZE) % SIZE_1];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 1;
        float buf_tmp = c_buf[buffer_index / SIZE][(buffer_index % SIZE) /SIZE_1][(buffer_index % SIZE) % SIZE_1];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }
#else
    else {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + num_elements - head_align + index_offset;
        float buf_tmp = c_buf[buffer_index / SIZE][(buffer_index % SIZE) /SIZE_1][(buffer_index % SIZE) % SIZE_1];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }
#endif
    c[i + start] = tmp;
  }
  if (tail_align != num_elements - 1) {
    MARS_WIDE_BUS_TYPE tmp = c[end - 1];
    size_t pos = (len - align) * num_elements + index_offset;
    pos += (num_elements - head_align) % num_elements;
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j > tail_align)
        continue;
      size_t buffer_index = pos + j;
      float buf_tmp = c_buf[buffer_index / SIZE][(buffer_index % SIZE) /SIZE_1][(buffer_index % SIZE) % SIZE_1];
      int raw_bits = *(int *)&buf_tmp;
      tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
    }
    c[end - 1] = tmp;
  }
}

template <int SIZE_1, int SIZE_2>
inline static void
memcpy_wide_bus_read_double_3d(double a_buf[][SIZE_2][SIZE_1],
                                                   size_t index3_offset,
                                                   size_t index2_offset,
                                                   size_t index1_offset,
                               MARS_WIDE_BUS_TYPE *a, size_t offset_byte,
                               size_t size_byte) {
#pragma HLS inline self
  //#pragma HLS array_partition variable = a_buf cyclic factor = 8
  const size_t data_width = sizeof(double);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t buf_size = size_byte / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t new_offset = offset + buf_size;
  size_t tail_align = (new_offset - 1) & (num_elements - 1);
  size_t start = offset / num_elements;
  size_t end = (offset + buf_size + num_elements - 1) / num_elements;
  //MARS_WIDE_BUS_TYPE *a_offset = a + start;
  size_t i, j;
  size_t index3 = index3_offset, index2 = index2_offset, index1 = index1_offset;
  const size_t alignment = (SIZE_1 % num_elements) == 0 &&
                            (index1_offset % num_elements) == 0;
  const size_t bound = SIZE_1 / num_elements;
  const size_t SIZE = SIZE_2 * SIZE_1;
  const size_t index_offset = index3_offset * SIZE + index2_offset * SIZE_1 +
                              index1_offset;
  int len = end - start;
  assert(len <= buf_size / num_elements + 2);
  assert(len >= buf_size / num_elements);
  if (1 == len) {
#ifdef __cplusplus
    MARS_WIDE_BUS_TYPE tmp(a[start]);
#else
    MARS_WIDE_BUS_TYPE tmp = a[start];
#endif
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
       if (j < head_align || j > tail_align)
         continue;

        size_t buffer_index = j - head_align + index_offset;
        long raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            *(double *)(&raw_bits);
    }
    return;
  }
  for (i = 0; i < len; ++i) {
#pragma HLS pipeline
#ifdef __cplusplus
    MARS_WIDE_BUS_TYPE tmp(a[i + start]);
#else
    MARS_WIDE_BUS_TYPE tmp = a[i + start];
#endif
    if (head_align == 0) {
      for (j = 0; j < num_elements; ++j) {
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j + index_offset;
        long raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        if (alignment)
          a_buf[index3][index2][index1 * num_elements + j] =
              *(double *)(&raw_bits);
        else
          a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %SIZE_1] =
              *(double *)(&raw_bits);
      }
      if (alignment) {
        index1++;
        if (index1 == bound) {
          index1 = 0;
          ++index2;
        }
        if (index2 == SIZE_2) {
          index2 = 0;
          ++index3;
        }
      }
    }
#if 0
    else if (head_align == 1) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 1)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 1;
        long raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            *(double *)(&raw_bits);
      }
    }

    else if (head_align == 2) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 2)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 2;
        long raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            *(double *)(&raw_bits);
      }
    }

    else if (head_align == 3) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 3)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 3;
        long raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            *(double *)(&raw_bits);
      }
    }

    else if (head_align == 4) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 4)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 4;
        long raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            *(double *)(&raw_bits);
      }
    }

    else if (head_align == 5) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 5)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 5;
        long raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            *(double *)(&raw_bits);
      }
    }

    else if (head_align == 6) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 6)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 6;
        long raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            *(double *)(&raw_bits);
      }
    }

    else {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 7)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 7;
        long raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            *(double *)(&raw_bits);
      }
    }
#else
    else {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < head_align)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - head_align + index_offset;
        long raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            *(double *)(&raw_bits);
      }
    }
#endif
  }
}

template <int SIZE_1, int SIZE_2>
inline static void memcpy_wide_bus_write_double_3d(
    MARS_WIDE_BUS_TYPE *c, double c_buf[][SIZE_2][SIZE_1],
                                                   size_t index3_offset,
                                                   size_t index2_offset,
                                                   size_t index1_offset,
    size_t offset_byte, size_t size_byte) {
#pragma HLS inline self
  //#pragma HLS array_partition variable = c_buf cyclic factor = 8
  const size_t data_width = sizeof(double);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t buf_size = size_byte / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t new_offset = offset + buf_size;
  size_t tail_align = (new_offset - 1) & (num_elements - 1);
  size_t start = offset / num_elements;
  size_t end = (offset + buf_size + num_elements - 1) / num_elements;
  size_t len = end - start;
  size_t i, j;
  const size_t SIZE = SIZE_1 * SIZE_2;
  const size_t index_offset = index3_offset * SIZE + index2_offset * SIZE_1 +
                              index1_offset;
  if (head_align == 0)
    len = (buf_size + num_elements - 1) / num_elements;
  if (len == 1) {
    MARS_WIDE_BUS_TYPE tmp;
    if (head_align != 0 || tail_align != (num_elements - 1))
      tmp = c[start];
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align)
        continue;
      if (j > tail_align)
        continue;
      size_t buffer_index = j - head_align + index_offset;
      double buf_tmp = c_buf[buffer_index / SIZE][(buffer_index % SIZE) /SIZE_1][(buffer_index % SIZE) % SIZE_1];
      long raw_bits = *(long *)&buf_tmp;
      tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
    }
    c[start] = tmp;
    return;
  }
  size_t align = 0;
  if (head_align != 0) {
    MARS_WIDE_BUS_TYPE tmp = c[start];
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align)
        continue;
      size_t buffer_index = j - head_align + index_offset;
      double buf_tmp = c_buf[buffer_index / SIZE][(buffer_index % SIZE) /SIZE_1][(buffer_index % SIZE) % SIZE_1];
      long raw_bits = *(long *)&buf_tmp;
      tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
    }
    c[start] = tmp;
    start++;
    align++;
  }
  if (tail_align != (num_elements - 1))
    align++;
  size_t index3 = index3_offset, index2 = index2_offset, index1 = index1_offset;
  const size_t alignment = (SIZE_1 % num_elements) == 0 &&
                            (index1_offset % num_elements) == 0;
  const size_t bound = SIZE_1 / num_elements;
  int burst_len = len - align;
  assert(burst_len <= buf_size / num_elements);
  for (i = 0; i < burst_len; ++i) {
#pragma HLS pipeline
    MARS_WIDE_BUS_TYPE tmp;
    if (head_align == 0) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + index_offset;
        double buf_tmp =
            alignment ? c_buf[index3][index2][index1 * num_elements + j]
                      : c_buf[buffer_index / SIZE][(buffer_index % SIZE) /SIZE_1][(buffer_index % SIZE) % SIZE_1];
        long raw_bits = *(long *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
      if (alignment) {
        index1++;
        if (index1 == bound) {
          index1 = 0;
          ++index2;
        }
        if (index2 == SIZE_2) {
          index2 = 0;
          ++index3;
        }
      }
    }
#if 0
    else if (head_align == 1) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 7;
        double buf_tmp =
            c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %SIZE_1];
        long raw_bits = *(long *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 2) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 6;
        double buf_tmp =
            c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %SIZE_1];
        long raw_bits = *(long *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 3) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 5;
        double buf_tmp =
            c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %SIZE_1];
        long raw_bits = *(long *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 4) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 4;
        double buf_tmp =
            c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %SIZE_1];
        long raw_bits = *(long *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 5) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 3;
        double buf_tmp =
            c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %SIZE_1];
        long raw_bits = *(long *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 6) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 2;
        double buf_tmp =
            c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %SIZE_1];
        long raw_bits = *(long *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 1;
        double buf_tmp =
            c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %SIZE_1];
        long raw_bits = *(long *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }
#else
    else {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + num_elements - head_align + index_offset;
        double buf_tmp =
            c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %SIZE_1];
        long raw_bits = *(long *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }
#endif
    c[i + start] = tmp;
  }
  if (tail_align != num_elements - 1) {
    MARS_WIDE_BUS_TYPE tmp = c[end - 1];
    size_t pos = (len - align) * num_elements + index_offset;
    pos += (num_elements - head_align) % num_elements;
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j > tail_align)
        continue;
      size_t buffer_index = pos + j;
      double buf_tmp = c_buf[buffer_index / SIZE][(buffer_index % SIZE) /SIZE_1][(buffer_index % SIZE) % SIZE_1];
      long raw_bits = *(long *)&buf_tmp;
      tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
    }
    c[end - 1] = tmp;
  }
}

template <int SIZE_3, int SIZE_2, int SIZE_1>
inline static void
memcpy_wide_bus_read_char_4d(char a_buf[][SIZE_3][SIZE_2][SIZE_1], 
                                                   size_t index4_offset,
                                                   size_t index3_offset,
                                                   size_t index2_offset,
                                                   size_t index1_offset,
                            MARS_WIDE_BUS_TYPE *a,
                            size_t offset_byte, size_t size_byte) {
#pragma HLS inline self
  //#pragma HLS array_partition variable = a_buf cyclic factor = 16
  const size_t data_width = sizeof(char);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t buf_size = size_byte / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t new_offset = offset + buf_size;
  size_t tail_align = (new_offset - 1) & (num_elements - 1);
  size_t start = offset / num_elements;
  size_t end = (offset + buf_size + num_elements - 1) / num_elements;
  //MARS_WIDE_BUS_TYPE *a_offset = a + start;
  size_t i, j;
  size_t index4 = index4_offset, index3 = index3_offset, index2 = index2_offset, index1 = index1_offset;
  const size_t alignment = (SIZE_1 % num_elements) == 0 &&
                           (index1_offset % num_elements) == 0;
  const size_t bound = SIZE_1 / num_elements;
  const size_t T_SIZE = SIZE_1 * SIZE_2 * SIZE_3;
  const size_t T_SIZE_1 = SIZE_1 * SIZE_2;
  const size_t index_offset = T_SIZE * index4_offset + T_SIZE_1 * index3_offset +
                              SIZE_1 * index2_offset + index1_offset;
  int len = end - start;
  assert(len <= buf_size / num_elements + 2);
  assert(len >= buf_size / num_elements);
  if (1 == len) {
#ifdef __cplusplus
    MARS_WIDE_BUS_TYPE tmp(a[start]);
#else
    MARS_WIDE_BUS_TYPE tmp = a[start];
#endif
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
       if (j < head_align || j > tail_align)
         continue;
       size_t buffer_index = j - head_align + index_offset;
       a_buf[buffer_index / T_SIZE]
                            [(buffer_index % T_SIZE) / T_SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) / SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) % SIZE_1] =
           tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
    }
    return;
  }
  for (i = 0; i < len; ++i) {
#pragma HLS pipeline
#ifdef __cplusplus
    MARS_WIDE_BUS_TYPE tmp(a[i + start]);
#else
    MARS_WIDE_BUS_TYPE tmp = a[i + start];
#endif
    if (head_align == 0) {
      for (j = 0; j < num_elements; ++j) {
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 0 + index_offset;
        if (alignment) {
          a_buf[index4][index3][index2][index1 * num_elements + j] =
              tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        } else
          a_buf[buffer_index / T_SIZE]
                            [(buffer_index % T_SIZE) / T_SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) / SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) % SIZE_1] =
              tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
      if (alignment) {
        index1++;
        if (index1 == bound) {
          index1 = 0;
          ++index2;
        }
        if (index2 == SIZE_2) {
          index2 = 0;
          ++index3;
        }
        if (index3 == SIZE_3) {
          index3 = 0;
          ++index4;
        }
      }

    }
#if 0
    else if (head_align == 1) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 1)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 1;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 2) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 2)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 2;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 3) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 3)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 3;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 4) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 4)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 4;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 5) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 5)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 5;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 6) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 6)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 6;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 7) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 7)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 7;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 8) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 8)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 8;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 9) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 9)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 9;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 10) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 10)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 10;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 11) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 11)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 11;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 12) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 12)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 12;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 13) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 13)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 13;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 14) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 14)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 14;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 15)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 15;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }
#else
    else {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < head_align)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - head_align + index_offset;
        a_buf[buffer_index / T_SIZE]
                            [(buffer_index % T_SIZE) / T_SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) / SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }
#endif
  }
}

template <int SIZE_1, int SIZE_2, int SIZE_3>
inline static void
memcpy_wide_bus_write_char_4d(MARS_WIDE_BUS_TYPE *c, 
                             char c_buf[][SIZE_3][SIZE_2][SIZE_1],
                                                   size_t index4_offset,
                                                   size_t index3_offset,
                                                   size_t index2_offset,
                                                   size_t index1_offset,
                             size_t offset_byte, size_t size_byte) {
#pragma HLS inline self
  //#pragma HLS array_partition variable = c_buf cyclic factor = 16
  const size_t data_width = sizeof(char);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t buf_size = size_byte / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t new_offset = offset + buf_size;
  size_t tail_align = (new_offset - 1) & (num_elements - 1);
  size_t start = offset / num_elements;
  size_t end = (offset + buf_size + num_elements - 1) / num_elements;
  size_t len = end - start;
  size_t i, j;
  const size_t T_SIZE_1 = SIZE_1 * SIZE_2 ;
  const size_t T_SIZE = SIZE_1 * SIZE_2 * SIZE_3;
  const size_t index_offset = T_SIZE * index4_offset + T_SIZE_1 * index3_offset +
                              SIZE_1 * index2_offset + index1_offset;

  if (head_align == 0)
    len = (buf_size + num_elements - 1) / num_elements;
  size_t align = 0;
  if (len == 1) {
    MARS_WIDE_BUS_TYPE tmp;
    if (head_align != 0 || tail_align != (num_elements - 1))
      tmp = c[start];
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align)
        continue;
      if (j > tail_align)
        continue;
      size_t buffer_index = j - head_align + index_offset;
      tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
            c_buf[buffer_index / T_SIZE]
                            [(buffer_index % T_SIZE) / T_SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) / SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) % SIZE_1]
                                                           );
    }
    c[start] = tmp;
    return;
  }
  if (head_align != 0) {
    MARS_WIDE_BUS_TYPE tmp = c[start];
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align)
        continue;
      size_t buffer_index = j - head_align + index_offset;
      tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
            c_buf[buffer_index / T_SIZE]
                            [(buffer_index % T_SIZE) / T_SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) / SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) % SIZE_1]
                                                           );
    }
    c[start] = tmp;
    start++;
    align++;
  }
  if (tail_align != (num_elements - 1))
    align++;
  size_t index4 = index4_offset, index3 = index3_offset, index2 = index2_offset, index1 = index1_offset;
  const size_t alignment = (SIZE_1 % num_elements) == 0 &&
                           (index1_offset % num_elements) == 0;
  const size_t bound = SIZE_1 / num_elements;
  int burst_len = len - align;
  assert(burst_len <= buf_size / num_elements);
  for (i = 0; i < burst_len; ++i) {
#pragma HLS pipeline
    MARS_WIDE_BUS_TYPE tmp;
    if (head_align == 0) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j - 0 + index_offset;
        int val = alignment
                      ? c_buf[index4][index3][index2][index1 * num_elements + j]
                        :c_buf[buffer_index / T_SIZE]
                            [(buffer_index % T_SIZE) / T_SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) / SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) % SIZE_1];
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), val);
      }
      if (alignment) {
        index1++;
        if (index1 == bound) {
          index1 = 0;
          ++index2;
        }
        if (index2 == SIZE_2) {
          index2 = 0;
          ++index3;
        }
        if (index3 == SIZE_3) {
          index3 = 0;
          ++index4;
        }
      }
    }
#if 0
    else if (head_align == 1) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 15;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 2) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 14;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 3) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 13;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 4) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 12;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 5) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 11;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 6) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 10;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 7) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 9;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 8) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 8;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 9) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 7;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 10) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 6;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 11) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 5;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 12) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 4;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 13) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 3;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 14) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 2;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 1;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }
#else
    else {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + num_elements - head_align + index_offset;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
            c_buf[buffer_index / T_SIZE]
                            [(buffer_index % T_SIZE) / T_SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) / SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) % SIZE_1]
                                                           );
      }
    }
#endif
    c[i + start] = tmp;
  }
  if (tail_align != num_elements - 1) {
    MARS_WIDE_BUS_TYPE tmp = c[end - 1];
    size_t pos = (len - align) * num_elements + index_offset;
    pos += (num_elements - head_align) % num_elements;
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j > tail_align)
        continue;
      size_t buffer_index = pos + j;
      tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
            c_buf[buffer_index / T_SIZE]
                            [(buffer_index % T_SIZE) / T_SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) / SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) % SIZE_1]
                                                           );
    }
    c[end - 1] = tmp;
  }
}

template <int SIZE_3, int SIZE_2, int SIZE_1>
inline static void
memcpy_wide_bus_read_int_4d(int a_buf[][SIZE_3][SIZE_2][SIZE_1], 
                                                   size_t index4_offset,
                                                   size_t index3_offset,
                                                   size_t index2_offset,
                                                   size_t index1_offset,
                            MARS_WIDE_BUS_TYPE *a,
                            size_t offset_byte, size_t size_byte) {
#pragma HLS inline self
  //#pragma HLS array_partition variable = a_buf cyclic factor = 16
  const size_t data_width = sizeof(int);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t buf_size = size_byte / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t new_offset = offset + buf_size;
  size_t tail_align = (new_offset - 1) & (num_elements - 1);
  size_t start = offset / num_elements;
  size_t end = (offset + buf_size + num_elements - 1) / num_elements;
  //MARS_WIDE_BUS_TYPE *a_offset = a + start;
  size_t i, j;
  size_t index4 = index4_offset, index3 = index3_offset, index2 = index2_offset, index1 = index1_offset;
  const size_t alignment = (SIZE_1 % num_elements) == 0 &&
                           (index1_offset % num_elements) == 0;
  const size_t bound = SIZE_1 / num_elements;
  const size_t T_SIZE = SIZE_1 * SIZE_2 * SIZE_3;
  const size_t T_SIZE_1 = SIZE_1 * SIZE_2;
  const size_t index_offset = T_SIZE * index4_offset + T_SIZE_1 * index3_offset +
                              SIZE_1 * index2_offset + index1_offset;

  int len = end - start;
  assert(len <= buf_size / num_elements + 2);
  assert(len >= buf_size / num_elements);
  if (1 == len) {
#ifdef __cplusplus
    MARS_WIDE_BUS_TYPE tmp(a[start]);
#else
    MARS_WIDE_BUS_TYPE tmp = a[start];
#endif
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
       if (j < head_align || j > tail_align)
         continue;
       size_t buffer_index = j - head_align + index_offset;
       a_buf[buffer_index / T_SIZE]
                            [(buffer_index % T_SIZE) / T_SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) / SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) % SIZE_1] =
           tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
    }
    return;
  }
  for (i = 0; i < len; ++i) {
#pragma HLS pipeline
#ifdef __cplusplus
    MARS_WIDE_BUS_TYPE tmp(a[i + start]);
#else
    MARS_WIDE_BUS_TYPE tmp = a[i + start];
#endif
    if (head_align == 0) {
      for (j = 0; j < num_elements; ++j) {
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 0 + index_offset;
        if (alignment) {
          a_buf[index4][index3][index2][index1 * num_elements + j] =
              tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        } else
          a_buf[buffer_index / T_SIZE]
                            [(buffer_index % T_SIZE) / T_SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) / SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) % SIZE_1] =
              tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
      if (alignment) {
        index1++;
        if (index1 == bound) {
          index1 = 0;
          ++index2;
        }
        if (index2 == SIZE_2) {
          index2 = 0;
          ++index3;
        }
        if (index3 == SIZE_3) {
          index3 = 0;
          ++index4;
        }
      }

    }
#if 0
    else if (head_align == 1) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 1)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 1;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 2) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 2)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 2;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 3) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 3)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 3;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 4) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 4)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 4;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 5) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 5)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 5;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 6) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 6)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 6;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 7) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 7)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 7;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 8) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 8)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 8;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 9) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 9)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 9;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 10) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 10)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 10;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 11) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 11)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 11;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 12) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 12)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 12;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 13) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 13)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 13;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else if (head_align == 14) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 14)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 14;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }

    else {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 15)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 15;
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }
#else
    else {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < head_align)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - head_align + index_offset;
        a_buf[buffer_index / T_SIZE]
                            [(buffer_index % T_SIZE) / T_SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) / SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) % SIZE_1] =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      }
    }
#endif
  }
}

template <int SIZE_1, int SIZE_2, int SIZE_3>
inline static void
memcpy_wide_bus_write_int_4d(MARS_WIDE_BUS_TYPE *c, 
                             int c_buf[][SIZE_3][SIZE_2][SIZE_1],
                                                   size_t index4_offset,
                                                   size_t index3_offset,
                                                   size_t index2_offset,
                                                   size_t index1_offset,
                             size_t offset_byte, size_t size_byte) {
#pragma HLS inline self
  //#pragma HLS array_partition variable = c_buf cyclic factor = 16
  const size_t data_width = sizeof(int);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t buf_size = size_byte / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t new_offset = offset + buf_size;
  size_t tail_align = (new_offset - 1) & (num_elements - 1);
  size_t start = offset / num_elements;
  size_t end = (offset + buf_size + num_elements - 1) / num_elements;
  size_t len = end - start;
  size_t i, j;
  const size_t T_SIZE_1 = SIZE_1 * SIZE_2 ;
  const size_t T_SIZE = SIZE_1 * SIZE_2 * SIZE_3;
  const size_t index_offset = T_SIZE * index4_offset + T_SIZE_1 * index3_offset +
                              SIZE_1 * index2_offset + index1_offset;
  if (head_align == 0)
    len = (buf_size + num_elements - 1) / num_elements;
  size_t align = 0;
  if (len == 1) {
    MARS_WIDE_BUS_TYPE tmp;
    if (head_align != 0 || tail_align != (num_elements - 1))
      tmp = c[start];
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align)
        continue;
      if (j > tail_align)
        continue;
      size_t buffer_index = j - head_align + index_offset;
      tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
            c_buf[buffer_index / T_SIZE]
                            [(buffer_index % T_SIZE) / T_SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) / SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) % SIZE_1]
                                                           );
    }
    c[start] = tmp;
    return;
  }
  if (head_align != 0) {
    MARS_WIDE_BUS_TYPE tmp = c[start];
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align)
        continue;
      size_t buffer_index = j - head_align + index_offset;
      tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
            c_buf[buffer_index / T_SIZE]
                            [(buffer_index % T_SIZE) / T_SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) / SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) % SIZE_1]
                                                           );
    }
    c[start] = tmp;
    start++;
    align++;
  }
  if (tail_align != (num_elements - 1))
    align++;
  size_t index4 = index4_offset, index3 = index3_offset, index2 = index2_offset, index1 = 0;
  const size_t alignment = (SIZE_1 % num_elements) == 0 &&
                           (index1_offset % num_elements) == 0;
  const size_t bound = SIZE_1 / num_elements;
  int burst_len = len - align;
  assert(burst_len <= buf_size / num_elements);
  for (i = 0; i < burst_len; ++i) {
#pragma HLS pipeline
    MARS_WIDE_BUS_TYPE tmp;
    if (head_align == 0) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j - 0 + index_offset;
        int val = alignment
                      ? c_buf[index4][index3][index2][index1 * num_elements + j]
                        :c_buf[buffer_index / T_SIZE]
                            [(buffer_index % T_SIZE) / T_SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) / SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) % SIZE_1];
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), val);
      }
      if (alignment) {
        index1++;
        if (index1 == bound) {
          index1 = 0;
          ++index2;
        }
        if (index2 == SIZE_2) {
          index2 = 0;
          ++index3;
        }
        if (index3 == SIZE_3) {
          index3 = 0;
          ++index4;
        }
      }
    }
#if 0
    else if (head_align == 1) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 15;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 2) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 14;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 3) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 13;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 4) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 12;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 5) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 11;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 6) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 10;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 7) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 9;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 8) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 8;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 9) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 7;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 10) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 6;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 11) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 5;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 12) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 4;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 13) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 3;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else if (head_align == 14) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 2;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }

    else {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 1;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
             c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %
                                                             SIZE_1]);
      }
    }
#else
    else {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + num_elements - head_align + index_offset;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
            c_buf[buffer_index / T_SIZE]
                            [(buffer_index % T_SIZE) / T_SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) / SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) % SIZE_1]
                                                           );
      }
    }
#endif
    c[i + start] = tmp;
  }
  if (tail_align != num_elements - 1) {
    MARS_WIDE_BUS_TYPE tmp = c[end - 1];
    size_t pos = (len - align) * num_elements + index_offset;
    pos += (num_elements - head_align) % num_elements;
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j > tail_align)
        continue;
      size_t buffer_index = pos + j;
      tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8),
            c_buf[buffer_index / T_SIZE]
                            [(buffer_index % T_SIZE) / T_SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) / SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) % SIZE_1]
                                                           );
    }
    c[end - 1] = tmp;
  }
}

template <int SIZE_1, int SIZE_2, int SIZE_3>
inline static void memcpy_wide_bus_read_float_4d(
    float a_buf[][SIZE_3][SIZE_2][SIZE_1],
                                                   size_t index4_offset,
                                                   size_t index3_offset,
                                                   size_t index2_offset,
                                                   size_t index1_offset,
                                                 MARS_WIDE_BUS_TYPE *a,
                                                 size_t offset_byte,
                                                 size_t size_byte) {
#pragma HLS inline self
  //#pragma HLS array_partition variable = a_buf cyclic factor = 16
  const size_t data_width = sizeof(float);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t buf_size = size_byte / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t new_offset = offset + buf_size;
  size_t tail_align = (new_offset - 1) & (num_elements - 1);
  size_t start = offset / num_elements;
  size_t end = (offset + buf_size + num_elements - 1) / num_elements;
  //MARS_WIDE_BUS_TYPE *a_offset = a + start;
  size_t i, j;
  size_t index4 = index4_offset, index3 = index3_offset, index2 = index2_offset, index1 = index1_offset;
  const size_t alignment = (SIZE_1 % num_elements) == 0 &&
                           (index1_offset % num_elements) == 0;
  const size_t bound = SIZE_1 / num_elements;
  const size_t T_SIZE_1 = SIZE_2 * SIZE_1;
  const size_t T_SIZE = SIZE_3 * SIZE_2 * SIZE_1;
  const size_t index_offset = T_SIZE * index4_offset + T_SIZE_1 * index3_offset +
                              SIZE_1 * index2_offset + index1_offset;
  int len = end - start;
  assert(len <= buf_size / num_elements + 2);
  assert(len >= buf_size / num_elements);
  if (1 == len) {
#ifdef __cplusplus
    MARS_WIDE_BUS_TYPE tmp(a[start]);
#else
    MARS_WIDE_BUS_TYPE tmp = a[start];
#endif
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align || j > tail_align)
         continue;
      size_t buffer_index = j - head_align + index_offset;
      int raw_bits =
          tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
      a_buf[buffer_index / T_SIZE]
                            [(buffer_index % T_SIZE) / T_SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) / SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) % SIZE_1] =
          *(float *)(&raw_bits);
    }
    return;
  }
  for (i = 0; i < len; ++i) {
#pragma HLS pipeline
#ifdef __cplusplus
    MARS_WIDE_BUS_TYPE tmp(a[i + start]);
#else
    MARS_WIDE_BUS_TYPE tmp = a[i + start];
#endif

    if (head_align == 0) {
      for (j = 0; j < num_elements; ++j) {
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j + 0 + index_offset;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        if (alignment)
          a_buf[index4][index3][index2][index1 * num_elements + j] =
              *(float *)(&raw_bits);
        else
          a_buf[buffer_index / T_SIZE]
                            [(buffer_index % T_SIZE) / T_SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) / SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) % SIZE_1] =
              *(float *)(&raw_bits);
      }
      if (alignment) {
        index1++;
        if (index1 == bound) {
          index1 = 0;
          ++index2;
        }
        if (index2 == SIZE_2) {
          index2 = 0;
          ++index3;
        }
        if (index3 == SIZE_3) {
          index3 = 0;
          ++index4;
        }
      }
    }
#if 0
    else if (head_align == 1) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 1)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 1;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            *(float *)(&raw_bits);
      }
    }

    else if (head_align == 2) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 2)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 2;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            *(float *)(&raw_bits);
      }
    }

    else if (head_align == 3) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 3)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 3;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            *(float *)(&raw_bits);
      }
    }

    else if (head_align == 4) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 4)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 4;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            *(float *)(&raw_bits);
      }
    }

    else if (head_align == 5) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 5)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 5;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            *(float *)(&raw_bits);
      }
    }

    else if (head_align == 6) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 6)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 6;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            *(float *)(&raw_bits);
      }
    }

    else if (head_align == 7) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 7)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 7;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            *(float *)(&raw_bits);
      }
    }

    else if (head_align == 8) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 8)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 8;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            *(float *)(&raw_bits);
      }
    }

    else if (head_align == 9) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 9)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 9;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            *(float *)(&raw_bits);
      }
    }

    else if (head_align == 10) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 10)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 10;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            *(float *)(&raw_bits);
      }
    }

    else if (head_align == 11) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 11)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 11;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            *(float *)(&raw_bits);
      }
    }

    else if (head_align == 12) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 12)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 12;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            *(float *)(&raw_bits);
      }
    }

    else if (head_align == 13) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 13)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 13;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            *(float *)(&raw_bits);
      }
    }

    else if (head_align == 14) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 14)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 14;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            *(float *)(&raw_bits);
      }
    }

    else {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 15)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 15;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            *(float *)(&raw_bits);
      }
    }
#else
    else {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < head_align)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - head_align + index_offset;
        int raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / T_SIZE]
                            [(buffer_index % T_SIZE) / T_SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) / SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) % SIZE_1] =
            *(float *)(&raw_bits);
      }
    }
#endif
  }
}

template <int SIZE_1, int SIZE_2, int SIZE_3>
inline static void memcpy_wide_bus_write_float_4d(MARS_WIDE_BUS_TYPE *c,
                                         float c_buf[][SIZE_3][SIZE_2][SIZE_1],
                                                   size_t index4_offset,
                                                   size_t index3_offset,
                                                   size_t index2_offset,
                                                   size_t index1_offset,
                                         size_t offset_byte,
                                         size_t size_byte) {
#pragma HLS inline self
  //#pragma HLS array_partition variable = c_buf cyclic factor = 16
  const size_t data_width = sizeof(float);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t buf_size = size_byte / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t new_offset = offset + buf_size;
  size_t tail_align = (new_offset - 1) & (num_elements - 1);
  size_t start = offset / num_elements;
  size_t end = (offset + buf_size + num_elements - 1) / num_elements;
  size_t len = end - start;
  size_t i, j;
  const size_t T_SIZE_1 = SIZE_1 * SIZE_2;
  const size_t T_SIZE = SIZE_1 * SIZE_2 * SIZE_3;
  const size_t index_offset = T_SIZE * index4_offset + T_SIZE_1 * index3_offset +
                              SIZE_1 * index2_offset + index1_offset;
 if (head_align == 0)
    len = (buf_size + num_elements - 1) / num_elements;
  if (len == 1) {
    MARS_WIDE_BUS_TYPE tmp;
    if (head_align != 0 || tail_align != (num_elements - 1))
      tmp = c[start];
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align)
        continue;
      if (j > tail_align)
        continue;
      size_t buffer_index = j - head_align + index_offset;
      float buf_tmp = c_buf[buffer_index / T_SIZE]
                            [(buffer_index % T_SIZE) / T_SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) / SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) % SIZE_1];
      int raw_bits = *(int *)&buf_tmp;
      tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
    }
    c[start] = tmp;
    return;
  }
  unsigned align = 0;
  if (head_align != 0) {
    MARS_WIDE_BUS_TYPE tmp = c[start];
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align)
        continue;
      size_t buffer_index = j - head_align + index_offset;
      float buf_tmp = c_buf[buffer_index / T_SIZE]
                            [(buffer_index % T_SIZE) / T_SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) / SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) % SIZE_1];
      int raw_bits = *(int *)&buf_tmp;
      tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
    }
    c[start] = tmp;
    start++;
    align++;
  }
  if (tail_align != (num_elements - 1))
    align++;
  size_t index4 = index4_offset, index3 = index3_offset, index2 = index2_offset, index1 = index1_offset;
  const size_t alignment = (SIZE_1 % num_elements) == 0 &&
                           (index1_offset % num_elements) == 0;
  const size_t bound = SIZE_1 / num_elements;
  int burst_len = len - align;
  assert(burst_len <= buf_size / num_elements);
  for (i = 0; i < burst_len; ++i) {
#pragma HLS pipeline
    MARS_WIDE_BUS_TYPE tmp;
    if (head_align == 0) {
      for (j = 0; j < num_elements; ++j) {

        size_t buffer_index = i * num_elements + j + index_offset;
        float buf_tmp =
            alignment ? c_buf[index4][index3][index2][index1 * num_elements + j]
                      : c_buf[buffer_index / T_SIZE]
                            [(buffer_index % T_SIZE) / T_SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) / SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) % SIZE_1];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
      if (alignment) {
        index1++;
        if (index1 == bound) {
          index1 = 0;
          ++index2;
        }
        if (index2 == SIZE_2) {
          index2 = 0;
          ++index3;
        }
        if (index3 == SIZE_3) {
          index3 = 0;
          ++index4;
        }
      }

    }
#if 0
    else if (head_align == 1) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 15;
        float buf_tmp = c_buf[buffer_index / SIZE][(buffer_index % SIZE) /SIZE_1][(buffer_index % SIZE) % SIZE_1];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 2) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 14;
        float buf_tmp = c_buf[buffer_index / SIZE][(buffer_index % SIZE) /SIZE_1][(buffer_index % SIZE) % SIZE_1];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 3) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 13;
        float buf_tmp = c_buf[buffer_index / SIZE][(buffer_index % SIZE) /SIZE_1][(buffer_index % SIZE) % SIZE_1];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 4) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 12;
        float buf_tmp = c_buf[buffer_index / SIZE][(buffer_index % SIZE) /SIZE_1][(buffer_index % SIZE) % SIZE_1];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 5) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 11;
        float buf_tmp = c_buf[buffer_index / SIZE][(buffer_index % SIZE) /SIZE_1][(buffer_index % SIZE) % SIZE_1];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 6) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 10;
        float buf_tmp = c_buf[buffer_index / SIZE][(buffer_index % SIZE) /SIZE_1][(buffer_index % SIZE) % SIZE_1];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 7) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 9;
        float buf_tmp = c_buf[buffer_index / SIZE][(buffer_index % SIZE) /SIZE_1][(buffer_index % SIZE) % SIZE_1];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 8) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 8;
        float buf_tmp = c_buf[buffer_index / SIZE][(buffer_index %SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 9) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 7;
        float buf_tmp = c_buf[buffer_index / SIZE][(buffer_index % SIZE) /SIZE_1][(buffer_index % SIZE) % SIZE_1];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 10) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 6;
        float buf_tmp = c_buf[buffer_index / SIZE][(buffer_index % SIZE) /SIZE_1][(buffer_index % SIZE) % SIZE_1];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 11) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 5;
        float buf_tmp = c_buf[buffer_index / SIZE][(buffer_index % SIZE) /SIZE_1][(buffer_index % SIZE) % SIZE_1];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 12) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 4;
        float buf_tmp = c_buf[buffer_index / SIZE][(buffer_index % SIZE) /SIZE_1][(buffer_index % SIZE) % SIZE_1];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 13) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 3;
        float buf_tmp = c_buf[buffer_index / SIZE][(buffer_index % SIZE) /SIZE_1][(buffer_index % SIZE) % SIZE_1];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 14) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 2;
        float buf_tmp = c_buf[buffer_index / SIZE][(buffer_index % SIZE) /SIZE_1][(buffer_index % SIZE) % SIZE_1];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 1;
        float buf_tmp = c_buf[buffer_index / SIZE][(buffer_index % SIZE) /SIZE_1][(buffer_index % SIZE) % SIZE_1];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }
#else
    else {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + num_elements - head_align;
        float buf_tmp = c_buf[buffer_index / T_SIZE]
                            [(buffer_index % T_SIZE) / T_SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) / SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) % SIZE_1];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }
#endif
    c[i + start] = tmp;
  }
  if (tail_align != num_elements - 1) {
    MARS_WIDE_BUS_TYPE tmp = c[end - 1];
    size_t pos = (len - align) * num_elements;
    pos += (num_elements - head_align) % num_elements + index_offset;
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j > tail_align)
        continue;
      size_t buffer_index = pos + j;
      float buf_tmp = c_buf[buffer_index / T_SIZE]
                            [(buffer_index % T_SIZE) / T_SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) / SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) % SIZE_1];
      int raw_bits = *(int *)&buf_tmp;
      tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
    }
    c[end - 1] = tmp;
  }
}

template <int SIZE_1, int SIZE_2, int SIZE_3>
inline static void
memcpy_wide_bus_read_double_4d(double a_buf[][SIZE_3][SIZE_2][SIZE_1],
                                                   size_t index4_offset,
                                                   size_t index3_offset,
                                                   size_t index2_offset,
                                                   size_t index1_offset,
                               MARS_WIDE_BUS_TYPE *a, size_t offset_byte,
                               size_t size_byte) {
#pragma HLS inline self
  //#pragma HLS array_partition variable = a_buf cyclic factor = 8
  const size_t data_width = sizeof(double);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t buf_size = size_byte / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t new_offset = offset + buf_size;
  size_t tail_align = (new_offset - 1) & (num_elements - 1);
  size_t start = offset / num_elements;
  size_t end = (offset + buf_size + num_elements - 1) / num_elements;
  //MARS_WIDE_BUS_TYPE *a_offset = a + start;
  size_t i, j;
  size_t index4 = index4_offset, index3 = index3_offset, index2 = index2_offset, index1 = index1_offset;
  const size_t alignment = (SIZE_1 % num_elements) == 0 &&
                           (index1_offset % num_elements) == 0;
  const size_t bound = SIZE_1 / num_elements;
  const size_t T_SIZE_1 = SIZE_2 * SIZE_1;
  const size_t T_SIZE = SIZE_3 * SIZE_2 * SIZE_1;
  const size_t index_offset = T_SIZE * index4_offset + T_SIZE_1 * index3_offset +
                              SIZE_1 * index2_offset + index1_offset;
  int len = end - start;
  assert(len <= buf_size / num_elements + 2);
  assert(len >= buf_size / num_elements);
  if (1 == len) {
#ifdef __cplusplus
    MARS_WIDE_BUS_TYPE tmp(a[start]);
#else
    MARS_WIDE_BUS_TYPE tmp = a[start];
#endif
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
       if (j < head_align || j > tail_align)
         continue;

        size_t buffer_index = j - head_align + index_offset;
        long raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / T_SIZE]
                            [(buffer_index % T_SIZE) / T_SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) / SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) % SIZE_1] =
            *(double *)(&raw_bits);
    }
    return;
  }
  for (i = 0; i < len; ++i) {
#pragma HLS pipeline
#ifdef __cplusplus
    MARS_WIDE_BUS_TYPE tmp(a[i + start]);
#else
    MARS_WIDE_BUS_TYPE tmp = a[i + start];
#endif
    if (head_align == 0) {
      for (j = 0; j < num_elements; ++j) {
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j + index_offset;
        long raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        if (alignment)
          a_buf[index4][index3][index2][index1 * num_elements + j] =
              *(double *)(&raw_bits);
        else
          a_buf[buffer_index / T_SIZE]
                            [(buffer_index % T_SIZE) / T_SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) / SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) % SIZE_1] =
              *(double *)(&raw_bits);
      }
      if (alignment) {
        index1++;
        if (index1 == bound) {
          index1 = 0;
          ++index2;
        }
        if (index2 == SIZE_2) {
          index2 = 0;
          ++index3;
        }
        if (index3 == SIZE_3) {
          index3 = 0;
          ++index4;
        }
      }
    }
#if 0
    else if (head_align == 1) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 1)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 1;
        long raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            *(double *)(&raw_bits);
      }
    }

    else if (head_align == 2) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 2)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 2;
        long raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            *(double *)(&raw_bits);
      }
    }

    else if (head_align == 3) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 3)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 3;
        long raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            *(double *)(&raw_bits);
      }
    }

    else if (head_align == 4) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 4)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 4;
        long raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            *(double *)(&raw_bits);
      }
    }

    else if (head_align == 5) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 5)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 5;
        long raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) % SIZE_1] =
            *(double *)(&raw_bits);
      }
    }

    else if (head_align == 6) {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 6)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 6;
        long raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / T_SIZE]
                            [(buffer_index % T_SIZE) / T_SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) / SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) % SIZE_1] =
            *(double *)(&raw_bits);
      }
    }

    else {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < 7)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - 7;
        long raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / T_SIZE]
                            [(buffer_index % T_SIZE) / T_SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) / SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) % SIZE_1] =
            *(double *)(&raw_bits);
      }
    }
#else
    else {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < head_align)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buffer_index = i * num_elements + j - head_align + index_offset;
        long raw_bits =
            tmp2(((j + 1) * data_width * 8 - 1), (j * data_width * 8));
        a_buf[buffer_index / T_SIZE]
                            [(buffer_index % T_SIZE) / T_SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) / SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) % SIZE_1] =
            *(double *)(&raw_bits);
      }
    }
#endif
  }
}

template <int SIZE_1, int SIZE_2, int SIZE_3>
inline static void memcpy_wide_bus_write_double_4d(
    MARS_WIDE_BUS_TYPE *c, double c_buf[][SIZE_3][SIZE_2][SIZE_1],
                                                   size_t index4_offset,
                                                   size_t index3_offset,
                                                   size_t index2_offset,
                                                   size_t index1_offset,
    size_t offset_byte, size_t size_byte) {
#pragma HLS inline self
  //#pragma HLS array_partition variable = c_buf cyclic factor = 8
  const size_t data_width = sizeof(double);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t buf_size = size_byte / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t new_offset = offset + buf_size;
  size_t tail_align = (new_offset - 1) & (num_elements - 1);
  size_t start = offset / num_elements;
  size_t end = (offset + buf_size + num_elements - 1) / num_elements;
  size_t len = end - start;
  size_t i, j;
  const size_t T_SIZE = SIZE_1 * SIZE_2 * SIZE_3;
  const size_t T_SIZE_1 = SIZE_1 * SIZE_2;
  const size_t index_offset = T_SIZE * index4_offset + T_SIZE_1 * index3_offset +
                              SIZE_1 * index2_offset + index1_offset;
  if (head_align == 0)
    len = (buf_size + num_elements - 1) / num_elements;
  if (len == 1) {
    MARS_WIDE_BUS_TYPE tmp;
    if (head_align != 0 || tail_align != (num_elements - 1))
      tmp = c[start];
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align)
        continue;
      if (j > tail_align)
        continue;
      size_t buffer_index = j - head_align + index_offset;
      double buf_tmp = c_buf[buffer_index / T_SIZE]
                            [(buffer_index % T_SIZE) / T_SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) / SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) % SIZE_1];
      long raw_bits = *(long *)&buf_tmp;
      tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
    }
    c[start] = tmp;
    return;
  }
  size_t align = 0;
  if (head_align != 0) {
    MARS_WIDE_BUS_TYPE tmp = c[start];
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align)
        continue;
      size_t buffer_index = j - head_align + index_offset;
      double buf_tmp = c_buf[buffer_index / T_SIZE]
                            [(buffer_index % T_SIZE) / T_SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) / SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) % SIZE_1];
      long raw_bits = *(long *)&buf_tmp;
      tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
    }
    c[start] = tmp;
    start++;
    align++;
  }
  if (tail_align != (num_elements - 1))
    align++;
  size_t index4 = index4_offset, index3 = index3_offset, index2 = index2_offset, index1 = index1_offset;
  const size_t alignment = (SIZE_1 % num_elements) == 0 &&
                           (index1_offset % num_elements) == 0;
  const size_t bound = SIZE_1 / num_elements;
  int burst_len = len - align;
  assert(burst_len <= buf_size / num_elements);
  for (i = 0; i < burst_len; ++i) {
#pragma HLS pipeline
    MARS_WIDE_BUS_TYPE tmp;
    if (head_align == 0) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + index_offset;
        double buf_tmp =
            alignment ? c_buf[index4][index3][index2][index1 * num_elements + j]
                      : c_buf[buffer_index / T_SIZE]
                            [(buffer_index % T_SIZE) / T_SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) / SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) % SIZE_1];
        long raw_bits = *(long *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
      if (alignment) {
        index1++;
        if (index1 == bound) {
          index1 = 0;
          ++index2;
        }
        if (index2 == SIZE_2) {
          index2 = 0;
          ++index3;
        }
        if (index3 == SIZE_3) {
          index3 = 0;
          ++index4;
        }
      }
    }
#if 0
    else if (head_align == 1) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 7;
        double buf_tmp =
            c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %SIZE_1];
        long raw_bits = *(long *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 2) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 6;
        double buf_tmp =
            c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %SIZE_1];
        long raw_bits = *(long *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 3) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 5;
        double buf_tmp =
            c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %SIZE_1];
        long raw_bits = *(long *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 4) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 4;
        double buf_tmp =
            c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %SIZE_1];
        long raw_bits = *(long *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 5) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 3;
        double buf_tmp =
            c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %SIZE_1];
        long raw_bits = *(long *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else if (head_align == 6) {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 2;
        double buf_tmp =
            c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %SIZE_1];
        long raw_bits = *(long *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }

    else {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + 1;
        double buf_tmp =
            c_buf[buffer_index / SIZE][(buffer_index % SIZE) / SIZE_1][(buffer_index % SIZE) %SIZE_1];
        long raw_bits = *(long *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }
#else
    else {
      for (j = 0; j < num_elements; ++j) {
        size_t buffer_index = i * num_elements + j + num_elements - head_align + index_offset;
        double buf_tmp = c_buf[buffer_index / T_SIZE]
                            [(buffer_index % T_SIZE) / T_SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) / SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) % SIZE_1];
        long raw_bits = *(long *)&buf_tmp;
        tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
      }
    }
#endif
    c[i + start] = tmp;
  }
  if (tail_align != num_elements - 1) {
    MARS_WIDE_BUS_TYPE tmp = c[end - 1];
    size_t pos = (len - align) * num_elements + index_offset;
    pos += (num_elements - head_align) % num_elements;
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j > tail_align)
        continue;
      size_t buffer_index = pos + j;
      double buf_tmp = c_buf[buffer_index / T_SIZE]
                            [(buffer_index % T_SIZE) / T_SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) / SIZE_1]
                            [((buffer_index % T_SIZE) % T_SIZE_1) % SIZE_1];
      long raw_bits = *(long *)&buf_tmp;
      tmp3(((j + 1) * data_width * 8 - 1), (j * data_width * 8), raw_bits);
    }
    c[end - 1] = tmp;
  }
}
#endif

#undef LARGE_BUS
#undef BUS_WIDTH
#undef cpp_get_range
#undef c_get_range
#undef cpp_set_range
#undef c_set_range
#undef tmp2
#undef tmp3
#endif
