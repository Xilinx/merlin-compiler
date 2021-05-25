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


#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#define BUS_WIDTH LARGE_BUS / 8
#define tmp2(tmp, len, s) c_get_range(tmp, len, s)
#define tmp3(tmp, len, s, val) c_set_range(tmp, len, s, val)
#define memcpy_wide_bus(access, type, bus) instant_size(access, type, bus)
#define instant_size(access, type, bus)                                        \
  memcpy_wide_bus_##access##_##type##_##bus
#define memcpy_wide_bus_single(access, type, bus)                              \
  instant_single_size(access, type, bus)
#define instant_single_size(access, type, bus)                                 \
  memcpy_wide_bus_single_##access##_##type##_##bus
#define memcpy_read(dst, src, s, T)                                            \
  {                                                                            \
    MARS_WIDE_BUS_TYPE src_tmp = src;                                          \
    T dst_tmp = *((T*)&src_tmp);                                               \
    dst = dst_tmp;                                                             \
  }
#define memcpy_write(dst, src, s, T)                                           \
  {                                                                            \
    MARS_WIDE_BUS_TYPE dst_tmp;                                                \
    T src_tmp = src;                                                           \
    dst_tmp = *((MARS_WIDE_BUS_TYPE *) &src_tmp);                              \
    dst = dst_tmp;                                                             \
  }
static char memcpy_wide_bus_single(read, char, LARGE_BUS)(MARS_WIDE_BUS_TYPE *a,
                                                          size_t offset_byte) {
#pragma HLS inline 
  const size_t data_width = sizeof(char);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t start = offset / num_elements;
  char ret = tmp2(a[start], data_width, (head_align * data_width));
  return ret;
}

static void memcpy_wide_bus_single(write, char,
                                   LARGE_BUS)(MARS_WIDE_BUS_TYPE *c, char c_buf,
                                              size_t offset_byte) {
#pragma HLS inline 
  const size_t data_width = sizeof(char);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t start = offset / num_elements;
  tmp3(c[start], data_width, (head_align * data_width), c_buf);
  return;
}

static short memcpy_wide_bus_single(read, short,
                                    LARGE_BUS)(MARS_WIDE_BUS_TYPE *a,
                                               size_t offset_byte) {
#pragma HLS inline 
  const size_t data_width = sizeof(short);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t start = offset / num_elements;
  short ret = tmp2(a[start], data_width, (head_align * data_width));
  return ret;
}

static void memcpy_wide_bus_single(write, short,
                                   LARGE_BUS)(MARS_WIDE_BUS_TYPE *c,
                                              short c_buf, size_t offset_byte) {
#pragma HLS inline 
  const size_t data_width = sizeof(short);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t start = offset / num_elements;
  tmp3(c[start], data_width, (head_align * data_width), c_buf);
  return;
}

static int memcpy_wide_bus_single(read, int, LARGE_BUS)(MARS_WIDE_BUS_TYPE *a,
                                                        size_t offset_byte) {
#pragma HLS inline 
  const size_t data_width = sizeof(int);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t start = offset / num_elements;
  int ret = tmp2(a[start], data_width, (head_align * data_width));
  return ret;
}

static void memcpy_wide_bus_single(write, int, LARGE_BUS)(MARS_WIDE_BUS_TYPE *c,
                                                          int c_buf,
                                                          size_t offset_byte) {
#pragma HLS inline 
  const size_t data_width = sizeof(int);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t start = offset / num_elements;
  tmp3(c[start], data_width, (head_align * data_width), c_buf);
  return;
}

static long memcpy_wide_bus_single(read, long, LARGE_BUS)(MARS_WIDE_BUS_TYPE *a,
                                                          size_t offset_byte) {
#pragma HLS inline 
  const size_t data_width = sizeof(long);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t start = offset / num_elements;
  long ret = tmp2(a[start], data_width, (head_align * data_width));
  return ret;
}

static void memcpy_wide_bus_single(write, long,
                                   LARGE_BUS)(MARS_WIDE_BUS_TYPE *c, long c_buf,
                                              size_t offset_byte) {
#pragma HLS inline 
  const size_t data_width = sizeof(long);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t start = offset / num_elements;
  tmp3(c[start], data_width, (head_align * data_width), c_buf);
  return;
}

static long long memcpy_wide_bus_single(read, long_long,
                                        LARGE_BUS)(MARS_WIDE_BUS_TYPE *a,
                                                   size_t offset_byte) {
#pragma HLS inline 
  const size_t data_width = sizeof(long long);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t start = offset / num_elements;
  long long ret = tmp2(a[start], data_width, (head_align * data_width));
  return ret;
}

static void memcpy_wide_bus_single(write, long_long,
                                   LARGE_BUS)(MARS_WIDE_BUS_TYPE *c,
                                              long long c_buf,
                                              size_t offset_byte) {
#pragma HLS inline 
  const size_t data_width = sizeof(long long);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t start = offset / num_elements;
  tmp3(c[start], data_width, (head_align * data_width), c_buf);
  return;
}

static float memcpy_wide_bus_single(read, float,
                                    LARGE_BUS)(MARS_WIDE_BUS_TYPE *a,
                                               size_t offset_byte) {
#pragma HLS inline 
  const size_t data_width = sizeof(float);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t start = offset / num_elements;

  int raw_bits = tmp2(a[start], data_width, (head_align * data_width));
  float ret = *(float *)(&raw_bits);
  return ret;
}

static void memcpy_wide_bus_single(write, float,
                                   LARGE_BUS)(MARS_WIDE_BUS_TYPE *c,
                                              float c_buf, size_t offset_byte) {
#pragma HLS inline 
  const size_t data_width = sizeof(float);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t start = offset / num_elements;
  float buf_tmp = c_buf;
  int raw_bits = *(int *)&buf_tmp;
  tmp3(c[start], data_width, (head_align * data_width), raw_bits);
  return;
}

static double memcpy_wide_bus_single(read, double,
                                     LARGE_BUS)(MARS_WIDE_BUS_TYPE *a,
                                                size_t offset_byte) {
#pragma HLS inline 
  const size_t data_width = sizeof(double);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t start = offset / num_elements;

  int64_t raw_bits = tmp2(a[start], data_width, (head_align * data_width));
  double ret = *(double *)(&raw_bits);
  return ret;
}

static void memcpy_wide_bus_single(write, double,
                                   LARGE_BUS)(MARS_WIDE_BUS_TYPE *c,
                                              double c_buf,
                                              size_t offset_byte) {
#pragma HLS inline 
  const size_t data_width = sizeof(double);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t start = offset / num_elements;
  double buf_tmp = c_buf;
  int64_t raw_bits = *(int64_t *)&buf_tmp;
  tmp3(c[start], data_width, (head_align * data_width), raw_bits);
  return;
}

static void memcpy_wide_bus(read, long,
                            LARGE_BUS)(long *a_buf, MARS_WIDE_BUS_TYPE *a,
                                       size_t offset_byte, size_t size_byte,
                                       const size_t const_buf_size) {
#pragma HLS inline 
  const size_t data_width = sizeof(long);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t buf_size = size_byte / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t new_offset = offset + buf_size;
  size_t tail_align = (new_offset - 1) & (num_elements - 1);
  size_t start = offset / num_elements;
  size_t end = (offset + buf_size + num_elements - 1) / num_elements;
  // MARS_WIDE_BUS_TYPE *a_offset = a + start;
  size_t i, j;
  long len = end - start;
  if (head_align == 0)
    len = (buf_size + num_elements - 1) / num_elements;
  const size_t max_trip = const_buf_size / num_elements + 2;
  assert(len <= const_buf_size / num_elements + 2);
  assert(len >= buf_size / num_elements);
  if (1 == len) {
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align || j > tail_align)
        continue;
      a_buf[j - head_align] = tmp2(a[start], data_width, (j * data_width));
    }
    return;
  }

L2:
  for (i = 0; i < len; ++i) {
#pragma HLS loop_tripcount max = max_trip
#ifdef PARALLEL_COALESCING
#pragma HLS unroll
#else
#pragma HLS pipeline
#endif

    if (head_align == 0) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 0] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 1) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 1)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 1] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 2) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 2)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 2] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 3) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 3)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 3] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 4) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 4)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 4] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 5) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 5)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 5] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 6) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 6)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 6] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (data_width == 4 && head_align == 7) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 7)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 7] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (data_width == 4 && head_align == 8) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 8)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 8] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (data_width == 4 && head_align == 9) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 9)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 9] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (data_width == 4 && head_align == 10) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 10)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 10] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (data_width == 4 && head_align == 11) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 11)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 11] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (data_width == 4 && head_align == 12) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 12)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 12] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (data_width == 4 && head_align == 13) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 13)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 13] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (data_width == 4 && head_align == 14) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 14)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 14] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 15)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 15] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }
  }
}

static void memcpy_wide_bus(write, long,
                            LARGE_BUS)(MARS_WIDE_BUS_TYPE *c, long *c_buf,
                                       size_t offset_byte, size_t size_byte,
                                       const size_t const_buf_size) {
#pragma HLS inline 
  const size_t data_width = sizeof(long);
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
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align)
        continue;
      if (j > tail_align)
        continue;
      tmp3(c[start], data_width, (j * data_width), c_buf[j - head_align]);
    }
    return;
  }
  if (head_align != 0) {
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align)
        continue;
      tmp3(c[start], data_width, (j * data_width), c_buf[j - head_align]);
    }
    start++;
    align++;
  }
  if (tail_align != (num_elements - 1))
    align++;
  int burst_len = len - align;
  const size_t max_trip = const_buf_size / num_elements;
  assert(burst_len <= const_buf_size / num_elements);
L3:
  for (i = 0; i < burst_len; ++i) {
#pragma HLS loop_tripcount max = max_trip
#ifdef PARALLEL_COALESCING
#pragma HLS unroll
#else
#pragma HLS pipeline
#endif

    if (head_align == 0) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j - 0]);
      }

    }

    else if (head_align == 1) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 15]);
      }
    }

    else if (head_align == 2) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 14]);
      }
    }

    else if (head_align == 3) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 13]);
      }
    }

    else if (head_align == 4) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 12]);
      }
    }

    else if (head_align == 5) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 11]);
      }
    }

    else if (head_align == 6) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 10]);
      }
    }

    else if (data_width == 4 && head_align == 7) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 9]);
      }
    }

    else if (data_width == 4 && head_align == 8) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 8]);
      }
    }

    else if (data_width == 4 && head_align == 9) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 7]);
      }
    }

    else if (data_width == 4 && head_align == 10) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 6]);
      }
    }

    else if (data_width == 4 && head_align == 11) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 5]);
      }
    }

    else if (data_width == 4 && head_align == 12) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 4]);
      }
    }

    else if (data_width == 4 && head_align == 13) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 3]);
      }
    }

    else if (data_width == 4 && head_align == 14) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 2]);
      }
    }

    else {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 1]);
      }
    }
  }
  if (tail_align != num_elements - 1) {
    size_t pos = (len - align) * num_elements;
    pos += (num_elements - head_align) % num_elements;
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j > tail_align)
        continue;
      tmp3(c[end - 1], data_width, (j * data_width), c_buf[pos + j]);
    }
  }
}

static void memcpy_wide_bus(read, long_long,
                            LARGE_BUS)(long long *a_buf, MARS_WIDE_BUS_TYPE *a,
                                       size_t offset_byte, size_t size_byte,
                                       const size_t const_buf_size) {
#pragma HLS inline 
  const size_t data_width = sizeof(long long);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t buf_size = size_byte / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t new_offset = offset + buf_size;
  size_t tail_align = (new_offset - 1) & (num_elements - 1);
  size_t start = offset / num_elements;
  size_t end = (offset + buf_size + num_elements - 1) / num_elements;
  // MARS_WIDE_BUS_TYPE *a_offset = a + start;
  size_t i, j;
  int len = end - start;
  if (head_align == 0)
    len = (buf_size + num_elements - 1) / num_elements;
  const size_t max_trip = const_buf_size / num_elements + 2;
  assert(len <= const_buf_size / num_elements + 2);
  assert(len >= buf_size / num_elements);
  if (1 == len) {
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align || j > tail_align)
        continue;
      a_buf[j - head_align] = tmp2(a[start], data_width, (j * data_width));
    }
    return;
  }

L2:
  for (i = 0; i < len; ++i) {
#pragma HLS loop_tripcount max = max_trip
#ifdef PARALLEL_COALESCING
#pragma HLS unroll
#else
#pragma HLS pipeline
#endif

    if (head_align == 0) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 0] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 1) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 1)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 1] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 2) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 2)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 2] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 3) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 3)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 3] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 4) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 4)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 4] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 5) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 5)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 5] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 6) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 6)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 6] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 15)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 15] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }
  }
}

static void memcpy_wide_bus(write, long_long,
                            LARGE_BUS)(MARS_WIDE_BUS_TYPE *c, long long *c_buf,
                                       size_t offset_byte, size_t size_byte,
                                       const size_t const_buf_size) {
#pragma HLS inline 
  const size_t data_width = sizeof(long long);
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
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align)
        continue;
      if (j > tail_align)
        continue;
      tmp3(c[start], data_width, (j * data_width), c_buf[j - head_align]);
    }
    return;
  }
  if (head_align != 0) {
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align)
        continue;
      tmp3(c[start], data_width, (j * data_width), c_buf[j - head_align]);
    }
    start++;
    align++;
  }
  if (tail_align != (num_elements - 1))
    align++;
  int burst_len = len - align;
  const size_t max_trip = const_buf_size / num_elements;
  assert(burst_len <= const_buf_size / num_elements);
L3:
  for (i = 0; i < burst_len; ++i) {
#pragma HLS loop_tripcount max = max_trip
#ifdef PARALLEL_COALESCING
#pragma HLS unroll
#else
#pragma HLS pipeline
#endif

    if (head_align == 0) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j - 0]);
      }

    }

    else if (head_align == 1) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 15]);
      }
    }

    else if (head_align == 2) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 14]);
      }
    }

    else if (head_align == 3) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 13]);
      }
    }

    else if (head_align == 4) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 12]);
      }
    }

    else if (head_align == 5) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 11]);
      }
    }

    else if (head_align == 6) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 10]);
      }
    }

    else {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 1]);
      }
    }
  }
  if (tail_align != num_elements - 1) {
    size_t pos = (len - align) * num_elements;
    pos += (num_elements - head_align) % num_elements;
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j > tail_align)
        continue;
      tmp3(c[end - 1], data_width, (j * data_width), c_buf[pos + j]);
    }
  }
}

static void memcpy_wide_bus(read, int,
                            LARGE_BUS)(int *a_buf, MARS_WIDE_BUS_TYPE *a,
                                       size_t offset_byte, size_t size_byte,
                                       const size_t const_buf_size) {
#pragma HLS inline 
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
  // MARS_WIDE_BUS_TYPE *a_offset = a + start;
  size_t i, j;
  int len = end - start;
  if (head_align == 0)
    len = (buf_size + num_elements - 1) / num_elements;
  const size_t max_trip = const_buf_size / num_elements + 2;
  assert(len <= const_buf_size / num_elements + 2);
  assert(len >= buf_size / num_elements);
  if (1 == len) {
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align || j > tail_align)
        continue;
      a_buf[j - head_align] = tmp2(a[start], data_width, (j * data_width));
    }
    return;
  }

L2:
  for (i = 0; i < len; ++i) {
#pragma HLS loop_tripcount max = max_trip
#ifdef PARALLEL_COALESCING
#pragma HLS unroll
#else
#pragma HLS pipeline
#endif

    if (head_align == 0) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 0] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 1) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 1)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 1] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 2) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 2)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 2] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 3) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 3)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 3] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 4) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 4)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 4] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 5) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 5)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 5] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 6) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 6)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 6] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 7) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 7)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 7] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 8) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 8)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 8] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 9) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 9)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 9] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 10) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 10)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 10] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 11) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 11)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 11] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 12) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 12)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 12] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 13) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 13)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 13] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 14) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 14)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 14] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 15)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 15] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }
  }
}

static void memcpy_wide_bus(write, int,
                            LARGE_BUS)(MARS_WIDE_BUS_TYPE *c, int *c_buf,
                                       size_t offset_byte, size_t size_byte,
                                       const size_t const_buf_size) {
#pragma HLS inline 
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
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align)
        continue;
      if (j > tail_align)
        continue;
      tmp3(c[start], data_width, (j * data_width), c_buf[j - head_align]);
    }
    return;
  }
  if (head_align != 0) {
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align)
        continue;
      tmp3(c[start], data_width, (j * data_width), c_buf[j - head_align]);
    }
    start++;
    align++;
  }
  if (tail_align != (num_elements - 1))
    align++;
  int burst_len = len - align;
  const size_t max_trip = const_buf_size / num_elements;
  assert(burst_len <= const_buf_size / num_elements);
L3:
  for (i = 0; i < burst_len; ++i) {
#pragma HLS loop_tripcount max = max_trip
#ifdef PARALLEL_COALESCING
#pragma HLS unroll
#else
#pragma HLS pipeline
#endif

    if (head_align == 0) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j - 0]);
      }

    }

    else if (head_align == 1) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 15]);
      }
    }

    else if (head_align == 2) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 14]);
      }
    }

    else if (head_align == 3) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 13]);
      }
    }

    else if (head_align == 4) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 12]);
      }
    }

    else if (head_align == 5) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 11]);
      }
    }

    else if (head_align == 6) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 10]);
      }
    }

    else if (head_align == 7) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 9]);
      }
    }

    else if (head_align == 8) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 8]);
      }
    }

    else if (head_align == 9) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 7]);
      }
    }

    else if (head_align == 10) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 6]);
      }
    }

    else if (head_align == 11) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 5]);
      }
    }

    else if (head_align == 12) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 4]);
      }
    }

    else if (head_align == 13) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 3]);
      }
    }

    else if (head_align == 14) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 2]);
      }
    }

    else {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 1]);
      }
    }
  }
  if (tail_align != num_elements - 1) {
    size_t pos = (len - align) * num_elements;
    pos += (num_elements - head_align) % num_elements;
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j > tail_align)
        continue;
      tmp3(c[end - 1], data_width, (j * data_width), c_buf[pos + j]);
    }
  }
}

static void memcpy_wide_bus(read, short,
                            LARGE_BUS)(short *a_buf, MARS_WIDE_BUS_TYPE *a,
                                       size_t offset_byte, size_t size_byte,
                                       const size_t const_buf_size) {
#pragma HLS inline 
  const size_t data_width = sizeof(short);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t buf_size = size_byte / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t new_offset = offset + buf_size;
  size_t tail_align = (new_offset - 1) & (num_elements - 1);
  size_t start = offset / num_elements;
  size_t end = (offset + buf_size + num_elements - 1) / num_elements;
  // MARS_WIDE_BUS_TYPE *a_offset = a + start;
  size_t i, j;
  int len = end - start;
  if (head_align == 0)
    len = (buf_size + num_elements - 1) / num_elements;
  const size_t max_trip = const_buf_size / num_elements + 2;
  assert(len <= const_buf_size / num_elements + 2);
  assert(len >= buf_size / num_elements);
  if (1 == len) {
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align || j > tail_align)
        continue;
      a_buf[j - head_align] = tmp2(a[start], data_width, (j * data_width));
    }
    return;
  }

L3:
  for (i = 0; i < len; ++i) {
#pragma HLS loop_tripcount max = max_trip
#ifdef PARALLEL_COALESCING
#pragma HLS unroll
#else
#pragma HLS pipeline
#endif

    if (head_align == 0) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 0] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 1) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 1)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 1] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 2) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 2)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 2] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 3) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 3)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 3] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 4) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 4)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 4] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 5) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 5)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 5] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 6) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 6)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 6] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 7) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 7)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 7] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 8) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 8)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 8] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 9) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 9)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 9] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 10) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 10)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 10] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 11) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 11)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 11] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 12) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 12)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 12] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 13) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 13)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 13] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 14) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 14)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 14] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 15) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 15)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 15] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 16) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 16)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 16] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 17) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 17)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 17] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 18) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 18)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 18] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 19) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 19)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 19] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 20) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 20)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 20] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 21) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 21)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 21] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 22) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 22)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 22] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 23) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 23)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 23] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 24) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 24)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 24] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 25) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 25)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 25] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 26) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 26)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 26] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 27) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 27)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 27] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 28) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 28)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 28] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 29) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 29)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 29] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 30) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 30)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 30] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 31)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 31] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }
  }
}

static void memcpy_wide_bus(write, short,
                            LARGE_BUS)(MARS_WIDE_BUS_TYPE *c, short *c_buf,
                                       size_t offset_byte, size_t size_byte,
                                       const size_t const_buf_size) {
#pragma HLS inline 
  const size_t data_width = sizeof(short);
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
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align)
        continue;
      if (j > tail_align)
        continue;
      tmp3(c[start], data_width, (j * data_width), c_buf[j - head_align]);
    }
    return;
  }
  if (head_align != 0) {
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align)
        continue;
      tmp3(c[start], data_width, (j * data_width), c_buf[j - head_align]);
    }
    start++;
    align++;
  }
  if (tail_align != (num_elements - 1))
    align++;
  int burst_len = len - align;
  const size_t max_trip = const_buf_size / num_elements;
  assert(burst_len <= const_buf_size / num_elements);
L3:
  for (i = 0; i < burst_len; ++i) {
#pragma HLS loop_tripcount max = max_trip
#ifdef PARALLEL_COALESCING
#pragma HLS unroll
#else
#pragma HLS pipeline
#endif

    if (head_align == 0) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j - 0]);
      }

    } else if (head_align == 1) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 31]);
      }
    }

    else if (head_align == 2) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 30]);
      }
    }

    else if (head_align == 3) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 29]);
      }
    }

    else if (head_align == 4) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 28]);
      }
    }

    else if (head_align == 5) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 27]);
      }
    }

    else if (head_align == 6) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 26]);
      }
    }

    else if (head_align == 7) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 25]);
      }
    }

    else if (head_align == 8) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 24]);
      }
    }

    else if (head_align == 9) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 23]);
      }
    }

    else if (head_align == 10) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 22]);
      }
    }

    else if (head_align == 11) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 21]);
      }
    }

    else if (head_align == 12) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 20]);
      }
    }

    else if (head_align == 13) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 19]);
      }
    }

    else if (head_align == 14) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 18]);
      }
    }

    else if (head_align == 15) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 17]);
      }
    }

    else if (head_align == 16) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 16]);
      }
    }

    else if (head_align == 17) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 15]);
      }
    }

    else if (head_align == 18) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 14]);
      }
    }

    else if (head_align == 19) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 13]);
      }
    }

    else if (head_align == 20) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 12]);
      }
    }

    else if (head_align == 21) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 11]);
      }
    }

    else if (head_align == 22) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 10]);
      }
    }

    else if (head_align == 23) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 9]);
      }
    }

    else if (head_align == 24) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 8]);
      }
    }

    else if (head_align == 25) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 7]);
      }
    }

    else if (head_align == 26) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 6]);
      }
    }

    else if (head_align == 27) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 5]);
      }
    }

    else if (head_align == 28) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 4]);
      }
    }

    else if (head_align == 29) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 3]);
      }
    }

    else if (head_align == 30) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 2]);
      }
    }

    else {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 1]);
      }
    }
  }
  if (tail_align != num_elements - 1) {
    size_t pos = (len - align) * num_elements;
    pos += (num_elements - head_align) % num_elements;
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j > tail_align)
        continue;
      tmp3(c[end - 1], data_width, (j * data_width), c_buf[pos + j]);
    }
  }
}

static void memcpy_wide_bus(read, char,
                            LARGE_BUS)(char *a_buf, MARS_WIDE_BUS_TYPE *a,
                                       size_t offset_byte, size_t size_byte,
                                       const size_t const_buf_size) {
#pragma HLS inline 
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
  // MARS_WIDE_BUS_TYPE *a_offset = a + start;
  size_t i, j;
  int len = end - start;
  if (head_align == 0)
    len = (buf_size + num_elements - 1) / num_elements;
  const size_t max_trip = const_buf_size / num_elements + 2;
  assert(len <= const_buf_size / num_elements + 2);
  assert(len >= buf_size / num_elements);
  if (1 == len) {
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align || j > tail_align)
        continue;
      a_buf[j - head_align] = tmp2(a[start], data_width, (j * data_width));
    }
    return;
  }

L2:
  for (i = 0; i < len; ++i) {
#pragma HLS loop_tripcount max = max_trip
#ifdef PARALLEL_COALESCING
#pragma HLS unroll
#else
#pragma HLS pipeline
#endif

    if (head_align == 0) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 0] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 1) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 1)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 1] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 2) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 2)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 2] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 3) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 3)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 3] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 4) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 4)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 4] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 5) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 5)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 5] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 6) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 6)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 6] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 7) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 7)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 7] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 8) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 8)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 8] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 9) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 9)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 9] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 10) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 10)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 10] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 11) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 11)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 11] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 12) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 12)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 12] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 13) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 13)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 13] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }

    else if (head_align == 14) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 14)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 14] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 15) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 15)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 15] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 16) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 16)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 16] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 17) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 17)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 17] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 18) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 18)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 18] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 19) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 19)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 19] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 20) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 20)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 20] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 21) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 21)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 21] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 22) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 22)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 22] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 23) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 23)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 23] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 24) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 24)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 24] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 25) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 25)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 25] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 26) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 26)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 26] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 27) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 27)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 27] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 28) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 28)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 28] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 29) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 29)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 29] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 30) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 30)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 30] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 31) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 31)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 31] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 32) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 32)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 32] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 33) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 33)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 33] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 34) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 34)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 34] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 35) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 35)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 35] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 36) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 36)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 36] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 37) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 37)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 37] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 38) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 38)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 38] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 39) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 39)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 39] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 40) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 40)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 40] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 41) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 41)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 41] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 42) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 42)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 42] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 43) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 43)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 43] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 44) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 44)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 44] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 45) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 45)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 45] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 46) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 46)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 46] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 47) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 47)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 47] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 48) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 48)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 48] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 49) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 49)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 49] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 50) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 50)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 50] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 51) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 51)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 51] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 52) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 52)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 52] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 53) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 53)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 53] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 54) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 54)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 54] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 55) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 55)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 55] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 56) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 56)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 56] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 57) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 57)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 57] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 58) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 58)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 58] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 59) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 59)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 59] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 60) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 60)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 60] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 61) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 61)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 61] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else if (head_align == 62) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 62)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 62] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    } else {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 63)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        a_buf[i * num_elements + j - 63] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }
  }
}

static void memcpy_wide_bus(write, char,
                            LARGE_BUS)(MARS_WIDE_BUS_TYPE *c, char *c_buf,
                                       size_t offset_byte, size_t size_byte,
                                       const size_t const_buf_size) {
#pragma HLS inline 
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
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align)
        continue;
      if (j > tail_align)
        continue;
      tmp3(c[start], data_width, (j * data_width), c_buf[j - head_align]);
    }
    return;
  }
  if (head_align != 0) {
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align)
        continue;
      tmp3(c[start], data_width, (j * data_width), c_buf[j - head_align]);
    }
    start++;
    align++;
  }
  if (tail_align != (num_elements - 1))
    align++;
  int burst_len = len - align;
  const size_t max_trip = const_buf_size / num_elements;
  assert(burst_len <= const_buf_size / num_elements);
L3:
  for (i = 0; i < burst_len; ++i) {
#pragma HLS loop_tripcount max = max_trip
#ifdef PARALLEL_COALESCING
#pragma HLS unroll
#else
#pragma HLS pipeline
#endif

    if (head_align == 0) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j - 0]);
      }

    } else if (head_align == 1) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 63]);
      }
    }

    else if (head_align == 2) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 62]);
      }
    }

    else if (head_align == 3) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 61]);
      }
    }

    else if (head_align == 4) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 60]);
      }
    }

    else if (head_align == 5) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 59]);
      }
    }

    else if (head_align == 6) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 58]);
      }
    }

    else if (head_align == 7) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 57]);
      }
    }

    else if (head_align == 8) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 56]);
      }
    }

    else if (head_align == 9) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 55]);
      }
    }

    else if (head_align == 10) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 54]);
      }
    }

    else if (head_align == 11) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 53]);
      }
    }

    else if (head_align == 12) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 52]);
      }
    }

    else if (head_align == 13) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 51]);
      }
    }

    else if (head_align == 14) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 50]);
      }
    }

    else if (head_align == 15) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 49]);
      }
    }

    else if (head_align == 16) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 48]);
      }
    }

    else if (head_align == 17) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 47]);
      }
    }

    else if (head_align == 18) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 46]);
      }
    }

    else if (head_align == 19) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 45]);
      }
    }

    else if (head_align == 20) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 44]);
      }
    }

    else if (head_align == 21) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 43]);
      }
    }

    else if (head_align == 22) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 42]);
      }
    }

    else if (head_align == 23) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 41]);
      }
    }

    else if (head_align == 24) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 40]);
      }
    }

    else if (head_align == 25) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 39]);
      }
    }

    else if (head_align == 26) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 38]);
      }
    }

    else if (head_align == 27) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 37]);
      }
    }

    else if (head_align == 28) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 36]);
      }
    }

    else if (head_align == 29) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 35]);
      }
    }

    else if (head_align == 30) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 34]);
      }
    }

    else if (head_align == 31) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 33]);
      }
    }

    else if (head_align == 32) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 32]);
      }
    }

    else if (head_align == 33) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 31]);
      }
    }

    else if (head_align == 34) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 30]);
      }
    }

    else if (head_align == 35) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 29]);
      }
    }

    else if (head_align == 36) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 28]);
      }
    }

    else if (head_align == 37) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 27]);
      }
    }

    else if (head_align == 38) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 26]);
      }
    }

    else if (head_align == 39) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 25]);
      }
    }

    else if (head_align == 40) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 24]);
      }
    }

    else if (head_align == 41) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 23]);
      }
    }

    else if (head_align == 42) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 22]);
      }
    }

    else if (head_align == 43) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 21]);
      }
    }

    else if (head_align == 44) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 20]);
      }
    }

    else if (head_align == 45) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 19]);
      }
    }

    else if (head_align == 46) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 18]);
      }
    }

    else if (head_align == 47) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 17]);
      }
    }

    else if (head_align == 48) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 16]);
      }
    }

    else if (head_align == 49) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 15]);
      }
    }

    else if (head_align == 50) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 14]);
      }
    }

    else if (head_align == 51) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 13]);
      }
    }

    else if (head_align == 52) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 12]);
      }
    }

    else if (head_align == 53) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 11]);
      }
    }

    else if (head_align == 54) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 10]);
      }
    }

    else if (head_align == 55) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 9]);
      }
    }

    else if (head_align == 56) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 8]);
      }
    }

    else if (head_align == 57) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 7]);
      }
    }

    else if (head_align == 58) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 6]);
      }
    }

    else if (head_align == 59) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 5]);
      }
    }

    else if (head_align == 60) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 4]);
      }
    }

    else if (head_align == 61) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 3]);
      }
    }

    else if (head_align == 62) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 2]);
      }
    }

    else {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[i * num_elements + j + 1]);
      }
    }
  }
  if (tail_align != num_elements - 1) {
    size_t pos = (len - align) * num_elements;
    pos += (num_elements - head_align) % num_elements;
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j > tail_align)
        continue;
      tmp3(c[end - 1], data_width, (j * data_width), c_buf[pos + j]);
    }
  }
}

static void memcpy_wide_bus(read, float,
                            LARGE_BUS)(float *a_buf, MARS_WIDE_BUS_TYPE *a,
                                       size_t offset_byte, size_t size_byte,
                                       const size_t const_buf_size) {
#pragma HLS inline 
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
  // MARS_WIDE_BUS_TYPE *a_offset = a + start;
  size_t i, j;
  int len = end - start;
  if (head_align == 0) {
    len = (buf_size + num_elements - 1) / num_elements;
  }
  const size_t max_trip = const_buf_size / num_elements + 2;
  assert(len <= const_buf_size / num_elements + 2);
  assert(len >= buf_size / num_elements);
  if (1 == len) {
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align || j > tail_align)
        continue;
      int raw_bits = tmp2(a[start], data_width, (j * data_width));
      a_buf[j - head_align] = *(float *)(&raw_bits);
    }
    return;
  }

L2:
  for (i = 0; i < len; ++i) {
#pragma HLS loop_tripcount max = max_trip
#ifdef PARALLEL_COALESCING
#pragma HLS unroll
#else
#pragma HLS pipeline
#endif

    if (head_align == 0) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits = tmp2(a[i + start], data_width, (j * data_width));
        a_buf[i * num_elements + j - 0] = *(float *)(&raw_bits);
      }
    }

    else if (head_align == 1) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 1)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits = tmp2(a[i + start], data_width, (j * data_width));
        a_buf[i * num_elements + j - 1] = *(float *)(&raw_bits);
      }
    }

    else if (head_align == 2) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 2)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits = tmp2(a[i + start], data_width, (j * data_width));
        a_buf[i * num_elements + j - 2] = *(float *)(&raw_bits);
      }
    }

    else if (head_align == 3) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 3)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits = tmp2(a[i + start], data_width, (j * data_width));
        a_buf[i * num_elements + j - 3] = *(float *)(&raw_bits);
      }
    }

    else if (head_align == 4) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 4)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits = tmp2(a[i + start], data_width, (j * data_width));
        a_buf[i * num_elements + j - 4] = *(float *)(&raw_bits);
      }
    }

    else if (head_align == 5) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 5)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits = tmp2(a[i + start], data_width, (j * data_width));
        a_buf[i * num_elements + j - 5] = *(float *)(&raw_bits);
      }
    }

    else if (head_align == 6) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 6)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits = tmp2(a[i + start], data_width, (j * data_width));
        a_buf[i * num_elements + j - 6] = *(float *)(&raw_bits);
      }
    }

    else if (head_align == 7) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 7)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits = tmp2(a[i + start], data_width, (j * data_width));
        a_buf[i * num_elements + j - 7] = *(float *)(&raw_bits);
      }
    }

    else if (head_align == 8) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 8)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits = tmp2(a[i + start], data_width, (j * data_width));
        a_buf[i * num_elements + j - 8] = *(float *)(&raw_bits);
      }
    }

    else if (head_align == 9) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 9)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits = tmp2(a[i + start], data_width, (j * data_width));
        a_buf[i * num_elements + j - 9] = *(float *)(&raw_bits);
      }
    }

    else if (head_align == 10) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 10)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits = tmp2(a[i + start], data_width, (j * data_width));
        a_buf[i * num_elements + j - 10] = *(float *)(&raw_bits);
      }
    }

    else if (head_align == 11) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 11)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits = tmp2(a[i + start], data_width, (j * data_width));
        a_buf[i * num_elements + j - 11] = *(float *)(&raw_bits);
      }
    }

    else if (head_align == 12) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 12)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits = tmp2(a[i + start], data_width, (j * data_width));
        a_buf[i * num_elements + j - 12] = *(float *)(&raw_bits);
      }
    }

    else if (head_align == 13) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 13)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits = tmp2(a[i + start], data_width, (j * data_width));
        a_buf[i * num_elements + j - 13] = *(float *)(&raw_bits);
      }
    }

    else if (head_align == 14) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 14)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits = tmp2(a[i + start], data_width, (j * data_width));
        a_buf[i * num_elements + j - 14] = *(float *)(&raw_bits);
      }
    }

    else {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 15)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits = tmp2(a[i + start], data_width, (j * data_width));
        a_buf[i * num_elements + j - 15] = *(float *)(&raw_bits);
      }
    }
  }
}

static void memcpy_wide_bus(write, float,
                            LARGE_BUS)(MARS_WIDE_BUS_TYPE *c, float *c_buf,
                                       size_t offset_byte, size_t size_byte,
                                       const size_t const_buf_size) {
#pragma HLS inline 
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
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align)
        continue;
      if (j > tail_align)
        continue;
      float buf_tmp = c_buf[j - head_align];
      int raw_bits = *(int *)&buf_tmp;
      tmp3(c[start], data_width, (j * data_width), raw_bits);
    }
    return;
  }
  unsigned align = 0;
  if (head_align != 0) {
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align)
        continue;
      float buf_tmp = c_buf[j - head_align];
      int raw_bits = *(int *)&buf_tmp;
      tmp3(c[start], data_width, (j * data_width), raw_bits);
    }
    start++;
    align++;
  }
  if (tail_align != (num_elements - 1))
    align++;
  int burst_len = len - align;
  const size_t max_trip = const_buf_size / num_elements;
  assert(burst_len <= const_buf_size / num_elements);
L3:
  for (i = 0; i < burst_len; ++i) {
#pragma HLS loop_tripcount max = max_trip
#ifdef PARALLEL_COALESCING
#pragma HLS unroll
#else
#pragma HLS pipeline
#endif

    if (head_align == 0) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        float buf_tmp = c_buf[i * num_elements + j - 0];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(c[i + start], data_width, (j * data_width), raw_bits);
      }

    }

    else if (head_align == 1) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        float buf_tmp = c_buf[i * num_elements + j + 15];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(c[i + start], data_width, (j * data_width), raw_bits);
      }
    }

    else if (head_align == 2) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        float buf_tmp = c_buf[i * num_elements + j + 14];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(c[i + start], data_width, (j * data_width), raw_bits);
      }
    }

    else if (head_align == 3) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        float buf_tmp = c_buf[i * num_elements + j + 13];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(c[i + start], data_width, (j * data_width), raw_bits);
      }
    }

    else if (head_align == 4) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        float buf_tmp = c_buf[i * num_elements + j + 12];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(c[i + start], data_width, (j * data_width), raw_bits);
      }
    }

    else if (head_align == 5) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        float buf_tmp = c_buf[i * num_elements + j + 11];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(c[i + start], data_width, (j * data_width), raw_bits);
      }
    }

    else if (head_align == 6) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        float buf_tmp = c_buf[i * num_elements + j + 10];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(c[i + start], data_width, (j * data_width), raw_bits);
      }
    }

    else if (head_align == 7) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        float buf_tmp = c_buf[i * num_elements + j + 9];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(c[i + start], data_width, (j * data_width), raw_bits);
      }
    }

    else if (head_align == 8) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        float buf_tmp = c_buf[i * num_elements + j + 8];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(c[i + start], data_width, (j * data_width), raw_bits);
      }
    }

    else if (head_align == 9) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        float buf_tmp = c_buf[i * num_elements + j + 7];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(c[i + start], data_width, (j * data_width), raw_bits);
      }
    }

    else if (head_align == 10) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        float buf_tmp = c_buf[i * num_elements + j + 6];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(c[i + start], data_width, (j * data_width), raw_bits);
      }
    }

    else if (head_align == 11) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        float buf_tmp = c_buf[i * num_elements + j + 5];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(c[i + start], data_width, (j * data_width), raw_bits);
      }
    }

    else if (head_align == 12) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        float buf_tmp = c_buf[i * num_elements + j + 4];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(c[i + start], data_width, (j * data_width), raw_bits);
      }
    }

    else if (head_align == 13) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        float buf_tmp = c_buf[i * num_elements + j + 3];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(c[i + start], data_width, (j * data_width), raw_bits);
      }
    }

    else if (head_align == 14) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        float buf_tmp = c_buf[i * num_elements + j + 2];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(c[i + start], data_width, (j * data_width), raw_bits);
      }
    }

    else {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        float buf_tmp = c_buf[i * num_elements + j + 1];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(c[i + start], data_width, (j * data_width), raw_bits);
      }
    }
  }
  if (tail_align != num_elements - 1) {
    size_t pos = (len - align) * num_elements;
    pos += (num_elements - head_align) % num_elements;
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j > tail_align)
        continue;
      float buf_tmp = c_buf[pos + j];
      int raw_bits = *(int *)&buf_tmp;
      tmp3(c[end - 1], data_width, (j * data_width), raw_bits);
    }
  }
}

static void memcpy_wide_bus(read, double,
                            LARGE_BUS)(double *a_buf, MARS_WIDE_BUS_TYPE *a,
                                       size_t offset_byte, size_t size_byte,
                                       const size_t const_buf_size) {
#pragma HLS inline 
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
  // MARS_WIDE_BUS_TYPE *a_offset = a + start;
  size_t i, j;
  int len = end - start;
  if (head_align == 0)
    len = (buf_size + num_elements - 1) / num_elements;
  const size_t max_trip = const_buf_size / num_elements + 2;
  assert(len <= const_buf_size / num_elements + 2);
  assert(len >= buf_size / num_elements);
  if (1 == len) {
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align || j > tail_align)
        continue;
      int64_t raw_bits = tmp2(a[start], data_width, (j * data_width));
      a_buf[j - head_align] = *(double *)(&raw_bits);
    }
    return;
  }

L2:
  for (i = 0; i < len; ++i) {
#pragma HLS loop_tripcount max = max_trip
#ifdef PARALLEL_COALESCING
#pragma HLS unroll
#else
#pragma HLS pipeline
#endif

    if (head_align == 0) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == end - start - 1 && j > tail_align)
          continue;
        int64_t raw_bits = tmp2(a[i + start], data_width, (j * data_width));
        a_buf[i * num_elements + j - 0] = *(double *)(&raw_bits);
      }
    }

    else if (head_align == 1) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 1)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int64_t raw_bits = tmp2(a[i + start], data_width, (j * data_width));
        a_buf[i * num_elements + j - 1] = *(double *)(&raw_bits);
      }
    }

    else if (head_align == 2) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 2)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int64_t raw_bits = tmp2(a[i + start], data_width, (j * data_width));
        a_buf[i * num_elements + j - 2] = *(double *)(&raw_bits);
      }
    }

    else if (head_align == 3) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 3)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int64_t raw_bits = tmp2(a[i + start], data_width, (j * data_width));
        a_buf[i * num_elements + j - 3] = *(double *)(&raw_bits);
      }
    }

    else if (head_align == 4) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 4)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int64_t raw_bits = tmp2(a[i + start], data_width, (j * data_width));
        a_buf[i * num_elements + j - 4] = *(double *)(&raw_bits);
      }
    }

    else if (head_align == 5) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 5)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int64_t raw_bits = tmp2(a[i + start], data_width, (j * data_width));
        a_buf[i * num_elements + j - 5] = *(double *)(&raw_bits);
      }
    }

    else if (head_align == 6) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 6)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int64_t raw_bits = tmp2(a[i + start], data_width, (j * data_width));
        a_buf[i * num_elements + j - 6] = *(double *)(&raw_bits);
      }
    }

    else {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 7)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int64_t raw_bits = tmp2(a[i + start], data_width, (j * data_width));
        a_buf[i * num_elements + j - 7] = *(double *)(&raw_bits);
      }
    }
  }
}

static void memcpy_wide_bus(write, double,
                            LARGE_BUS)(MARS_WIDE_BUS_TYPE *c, double *c_buf,
                                       size_t offset_byte, size_t size_byte,
                                       const size_t const_buf_size) {
#pragma HLS inline 
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
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align)
        continue;
      if (j > tail_align)
        continue;
      double buf_tmp = c_buf[j - head_align];
      int64_t raw_bits = *(int64_t *)&buf_tmp;
      tmp3(c[start], data_width, (j * data_width), raw_bits);
    }
    return;
  }
  size_t align = 0;
  if (head_align != 0) {
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align)
        continue;
      double buf_tmp = c_buf[j - head_align];
      int64_t raw_bits = *(int64_t *)&buf_tmp;
      tmp3(c[start], data_width, (j * data_width), raw_bits);
    }
    start++;
    align++;
  }
  if (tail_align != (num_elements - 1))
    align++;
  int burst_len = len - align;
  const size_t max_trip = const_buf_size / num_elements;
  assert(burst_len <= const_buf_size / num_elements);
L3:
  for (i = 0; i < burst_len; ++i) {
#pragma HLS loop_tripcount max = max_trip
#ifdef PARALLEL_COALESCING
#pragma HLS unroll
#else
#pragma HLS pipeline
#endif

    if (head_align == 0) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        double buf_tmp = c_buf[i * num_elements + j - 0];
        int64_t raw_bits = *(int64_t *)&buf_tmp;
        tmp3(c[i + start], data_width, (j * data_width), raw_bits);
      }

    }

    else if (head_align == 1) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        double buf_tmp = c_buf[i * num_elements + j + 7];
        int64_t raw_bits = *(int64_t *)&buf_tmp;
        tmp3(c[i + start], data_width, (j * data_width), raw_bits);
      }
    }

    else if (head_align == 2) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        double buf_tmp = c_buf[i * num_elements + j + 6];
        int64_t raw_bits = *(int64_t *)&buf_tmp;
        tmp3(c[i + start], data_width, (j * data_width), raw_bits);
      }
    }

    else if (head_align == 3) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        double buf_tmp = c_buf[i * num_elements + j + 5];
        int64_t raw_bits = *(int64_t *)&buf_tmp;
        tmp3(c[i + start], data_width, (j * data_width), raw_bits);
      }
    }

    else if (head_align == 4) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        double buf_tmp = c_buf[i * num_elements + j + 4];
        int64_t raw_bits = *(int64_t *)&buf_tmp;
        tmp3(c[i + start], data_width, (j * data_width), raw_bits);
      }
    }

    else if (head_align == 5) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        double buf_tmp = c_buf[i * num_elements + j + 3];
        int64_t raw_bits = *(int64_t *)&buf_tmp;
        tmp3(c[i + start], data_width, (j * data_width), raw_bits);
      }
    }

    else if (head_align == 6) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        double buf_tmp = c_buf[i * num_elements + j + 2];
        int64_t raw_bits = *(int64_t *)&buf_tmp;
        tmp3(c[i + start], data_width, (j * data_width), raw_bits);
      }
    }

    else {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        double buf_tmp = c_buf[i * num_elements + j + 1];
        int64_t raw_bits = *(int64_t *)&buf_tmp;
        tmp3(c[i + start], data_width, (j * data_width), raw_bits);
      }
    }
  }
  if (tail_align != num_elements - 1) {
    size_t pos = (len - align) * num_elements;
    pos += (num_elements - head_align) % num_elements;
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j > tail_align)
        continue;
      double buf_tmp = c_buf[pos + j];
      int64_t raw_bits = *(int64_t *)&buf_tmp;
      tmp3(c[end - 1], data_width, (j * data_width), raw_bits);
    }
  }
}
#ifdef __cplusplus
template <typename T>
static void memcpy_wide_bus(read, struct,
                            LARGE_BUS)(T *a_buf, MARS_WIDE_BUS_TYPE *a,
                                       size_t offset_byte, size_t size_byte,
                                       const size_t const_buf_size) {
#pragma HLS inline 
  const size_t data_width = sizeof(T);
  const size_t bus_width = BUS_WIDTH;
  const size_t num_elements = bus_width / data_width;
  size_t buf_size = size_byte / data_width;
  size_t offset = offset_byte / data_width;
  size_t head_align = offset & (num_elements - 1);
  size_t new_offset = offset + buf_size;
  size_t tail_align = (new_offset - 1) & (num_elements - 1);
  size_t start = offset / num_elements;
  size_t end = (offset + buf_size + num_elements - 1) / num_elements;
  // MARS_WIDE_BUS_TYPE *a_offset = a + start;
  size_t i, j;
  int len = end - start;
  if (head_align == 0)
    len = (buf_size + num_elements - 1) / num_elements;
  const size_t max_trip = const_buf_size / num_elements + 2;
  assert(len <= const_buf_size / num_elements + 2);
  assert(len >= buf_size / num_elements);
  if (1 == len) {
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align || j > tail_align)
        continue;
      memcpy_read(a_buf[j - head_align],
                  tmp2(a[start], data_width, (j * data_width)), data_width, T);
    }
    return;
  }

L2:
  for (i = 0; i < len; ++i) {
#pragma HLS loop_tripcount max = max_trip
#ifdef PARALLEL_COALESCING
#pragma HLS unroll
#else
#pragma HLS pipeline
#endif

    if (head_align == 0) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 0],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    }

    else if (head_align == 1) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 1)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 1],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    }

    else if (head_align == 2) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 2)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 2],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    }

    else if (head_align == 3) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 3)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 3],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    }

    else if (head_align == 4) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 4)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 4],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    }

    else if (head_align == 5) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 5)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 5],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    }

    else if (head_align == 6) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 6)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 6],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    }

    else if (head_align == 7) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 7)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 7],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    }

    else if (head_align == 8) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 8)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 8],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    }

    else if (head_align == 9) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 9)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 9],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    }

    else if (head_align == 10) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 10)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 10],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    }

    else if (head_align == 11) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 11)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 11],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    }

    else if (head_align == 12) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 12)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 12],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    }

    else if (head_align == 13) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 13)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 13],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    }

    else if (head_align == 14) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 14)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 14],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    } else if (head_align == 15) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 15)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 15],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    } else if (head_align == 16) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 16)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 16],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    } else if (head_align == 17) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 17)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 17],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    } else if (head_align == 18) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 18)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 18],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    } else if (head_align == 19) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 19)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 19],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    } else if (head_align == 20) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 20)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 20],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    } else if (head_align == 21) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 21)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 21],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    } else if (head_align == 22) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 22)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 22],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    } else if (head_align == 23) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 23)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 23],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    } else if (head_align == 24) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 24)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 24],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    } else if (head_align == 25) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 25)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 25],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    } else if (head_align == 26) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 26)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 26],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    } else if (head_align == 27) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 27)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 27],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    } else if (head_align == 28) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 28)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 28],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    } else if (head_align == 29) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 29)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 29],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    } else if (head_align == 30) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 30)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 30],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    } else if (head_align == 31) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 31)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 31],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    } else if (head_align == 32) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 32)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 32],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    } else if (head_align == 33) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 33)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 33],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    } else if (head_align == 34) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 34)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 34],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    } else if (head_align == 35) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 35)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 35],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    } else if (head_align == 36) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 36)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 36],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    } else if (head_align == 37) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 37)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 37],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    } else if (head_align == 38) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 38)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 38],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    } else if (head_align == 39) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 39)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 39],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    } else if (head_align == 40) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 40)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 40],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    } else if (head_align == 41) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 41)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 41],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    } else if (head_align == 42) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 42)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 42],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    } else if (head_align == 43) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 43)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 43],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    } else if (head_align == 44) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 44)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 44],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    } else if (head_align == 45) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 45)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 45],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    } else if (head_align == 46) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 46)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 46],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    } else if (head_align == 47) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 47)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 47],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    } else if (head_align == 48) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 48)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 48],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    } else if (head_align == 49) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 49)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 49],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    } else if (head_align == 50) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 50)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 50],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    } else if (head_align == 51) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 51)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 51],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    } else if (head_align == 52) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 52)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 52],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    } else if (head_align == 53) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 53)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 53],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    } else if (head_align == 54) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 54)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 54],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    } else if (head_align == 55) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 55)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 55],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    } else if (head_align == 56) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 56)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 56],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    } else if (head_align == 57) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 57)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 57],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    } else if (head_align == 58) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 58)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 58],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    } else if (head_align == 59) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 59)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 59],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    } else if (head_align == 60) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 60)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 60],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    } else if (head_align == 61) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 61)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 61],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    } else if (head_align == 62) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 62)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 62],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    } else {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        if (i == 0 && j < 63)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        memcpy_read(a_buf[i * num_elements + j - 63],
                    tmp2(a[i + start], data_width, (j * data_width)),
                    data_width, T);
      }
    }
  }
}

template <typename T>
static void memcpy_wide_bus(write, struct,
                            LARGE_BUS)(MARS_WIDE_BUS_TYPE *c, T *c_buf,
                                       size_t offset_byte, size_t size_byte,
                                       const size_t const_buf_size) {
#pragma HLS inline 
  const size_t data_width = sizeof(T);
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
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align)
        continue;
      if (j > tail_align)
        continue;
      memcpy_write(tmp2(c[start], data_width, (j * data_width)),
                   c_buf[j - head_align], data_width, T);
    }
    return;
  }
  if (head_align != 0) {
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j < head_align)
        continue;
      memcpy_write(tmp2(c[start], data_width, (j * data_width)),
                   c_buf[j - head_align], data_width, T);
    }
    start++;
    align++;
  }
  if (tail_align != (num_elements - 1))
    align++;
  int burst_len = len - align;
  const size_t max_trip = const_buf_size / num_elements;
  assert(burst_len <= const_buf_size / num_elements);
L3:
  for (i = 0; i < burst_len; ++i) {
#pragma HLS loop_tripcount max = max_trip
#ifdef PARALLEL_COALESCING
#pragma HLS unroll
#else
#pragma HLS pipeline
#endif

    if (head_align == 0) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j - 0], data_width, T);
      }

    } else if (head_align == 1) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 63], data_width, T);
      }
    }

    else if (head_align == 2) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 62], data_width, T);
      }
    }

    else if (head_align == 3) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 61], data_width, T);
      }
    }

    else if (head_align == 4) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 60], data_width, T);
      }
    }

    else if (head_align == 5) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 59], data_width, T);
      }
    }

    else if (head_align == 6) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 58], data_width, T);
      }
    }

    else if (head_align == 7) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 57], data_width, T);
      }
    }

    else if (head_align == 8) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 56], data_width, T);
      }
    }

    else if (head_align == 9) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 55], data_width, T);
      }
    }

    else if (head_align == 10) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 54], data_width, T);
      }
    }

    else if (head_align == 11) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 53], data_width, T);
      }
    }

    else if (head_align == 12) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 52], data_width, T);
      }
    }

    else if (head_align == 13) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 51], data_width, T);
      }
    }

    else if (head_align == 14) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 50], data_width, T);
      }
    }

    else if (head_align == 15) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 49], data_width, T);
      }
    }

    else if (head_align == 16) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 48], data_width, T);
      }
    }

    else if (head_align == 17) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 47], data_width, T);
      }
    }

    else if (head_align == 18) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 46], data_width, T);
      }
    }

    else if (head_align == 19) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 45], data_width, T);
      }
    }

    else if (head_align == 20) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 44], data_width, T);
      }
    }

    else if (head_align == 21) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 43], data_width, T);
      }
    }

    else if (head_align == 22) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 42], data_width, T);
      }
    }

    else if (head_align == 23) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 41], data_width, T);
      }
    }

    else if (head_align == 24) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 40], data_width, T);
      }
    }

    else if (head_align == 25) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 39], data_width, T);
      }
    }

    else if (head_align == 26) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 38], data_width, T);
      }
    }

    else if (head_align == 27) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 37], data_width, T);
      }
    }

    else if (head_align == 28) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 36], data_width, T);
      }
    }

    else if (head_align == 29) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 35], data_width, T);
      }
    }

    else if (head_align == 30) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 34], data_width, T);
      }
    }

    else if (head_align == 31) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 33], data_width, T);
      }
    }

    else if (head_align == 32) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 32], data_width, T);
      }
    }

    else if (head_align == 33) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 31], data_width, T);
      }
    }

    else if (head_align == 34) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 30], data_width, T);
      }
    }

    else if (head_align == 35) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 29], data_width, T);
      }
    }

    else if (head_align == 36) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 28], data_width, T);
      }
    }

    else if (head_align == 37) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 27], data_width, T);
      }
    }

    else if (head_align == 38) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 26], data_width, T);
      }
    }

    else if (head_align == 39) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 25], data_width, T);
      }
    }

    else if (head_align == 40) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 24], data_width, T);
      }
    }

    else if (head_align == 41) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 23], data_width, T);
      }
    }

    else if (head_align == 42) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 22], data_width, T);
      }
    }

    else if (head_align == 43) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 21], data_width, T);
      }
    }

    else if (head_align == 44) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 20], data_width, T);
      }
    }

    else if (head_align == 45) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 19], data_width, T);
      }
    }

    else if (head_align == 46) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 18], data_width, T);
      }
    }

    else if (head_align == 47) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 17], data_width, T);
      }
    }

    else if (head_align == 48) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 16], data_width, T);
      }
    }

    else if (head_align == 49) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 15], data_width, T);
      }
    }

    else if (head_align == 50) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 14], data_width, T);
      }
    }

    else if (head_align == 51) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 13], data_width, T);
      }
    }

    else if (head_align == 52) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 12], data_width, T);
      }
    }

    else if (head_align == 53) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 11], data_width, T);
      }
    }

    else if (head_align == 54) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 10], data_width, T);
      }
    }

    else if (head_align == 55) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 9], data_width, T);
      }
    }

    else if (head_align == 56) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 8], data_width, T);
      }
    }

    else if (head_align == 57) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 7], data_width, T);
      }
    }

    else if (head_align == 58) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 6], data_width, T);
      }
    }

    else if (head_align == 59) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 5], data_width, T);
      }
    }

    else if (head_align == 60) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 4], data_width, T);
      }
    }

    else if (head_align == 61) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 3], data_width, T);
      }
    }

    else if (head_align == 62) {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 2], data_width, T);
      }
    }

    else {
      for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
        memcpy_write(tmp2(c[i + start], data_width, (j * data_width)),
                     c_buf[i * num_elements + j + 1], data_width, T);
      }
    }
  }
  if (tail_align != num_elements - 1) {
    size_t pos = (len - align) * num_elements;
    pos += (num_elements - head_align) % num_elements;
    for (j = 0; j < num_elements; ++j) {
#pragma HLS unroll
      if (j > tail_align)
        continue;
      memcpy_write(tmp2(c[end - 1], data_width, (j * data_width)),
                   c_buf[pos + j], data_width, T);
    }
  }
}
#endif
#undef memcpy_wide_bus
#undef instant_size
#undef memcpy_wide_bus_single
#undef instant_single_size
#undef LARGE_BUS
#undef BUS_WIDTH
#undef tmp2
#undef tmp3
#undef memcpy_read
#undef memcpy_write
