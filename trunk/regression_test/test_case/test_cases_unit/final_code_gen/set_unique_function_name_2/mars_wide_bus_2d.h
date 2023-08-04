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
#include <assert.h>
#include <stdlib.h>
#define BUS_WIDTH LARGE_BUS / 8
#define tmp2(tmp, x, y) c_get_range(tmp, x, y)
#define tmp3(tmp, x, y, val) c_set_range(tmp, x, y, val)

#define memcpy_wide_bus(access, type, size, bus)                               \
  instant_size(access, type, size, bus)
#define instant_size(access, type, size, bus)                                  \
  memcpy_wide_bus_##access##_##type##_2d_##size##_##bus

static void memcpy_wide_bus(read, char, SIZE_1, LARGE_BUS)(
    char a_buf[][SIZE_1], size_t index2_offset, size_t index1_offset,
    MARS_WIDE_BUS_TYPE *a, size_t offset_byte, size_t size_byte,
    const size_t const_buf_size) {
#pragma HLS inline self
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
  size_t index2 = index2_offset, index1 = index1_offset;
  int aligned = 0;
  if ((SIZE_1 % num_elements) == 0 && (index1_offset % num_elements) == 0) {
    aligned = 1;
    index1 = index1_offset / num_elements;
  } else if (((num_elements % SIZE_1) == 0) &&
             ((index2_offset * SIZE_1) % num_elements) == 0 && !index1_offset) {
    aligned = -1;
    index2 = index2_offset * SIZE_1 / num_elements;
  }
  const size_t bound = SIZE_1 / num_elements;
  const size_t bound0 = num_elements / SIZE_1;
  const size_t index_offset = index2_offset * SIZE_1 + index1_offset;
  int len = end - start;
  assert(len <= const_buf_size / num_elements + 2);
  assert(len >= buf_size / num_elements);
  if (1 == len) {
    for (j = 0; j < num_elements; ++j) {
      if (j < head_align || j > tail_align)
        continue;
      size_t buf_index = j - head_align + index_offset;
      a_buf[buf_index / SIZE_1][buf_index % SIZE_1] =
          tmp2(a[start], data_width, (j * data_width));
    }
    return;
  }

  for (i = 0; i < len; ++i) {
#pragma HLS pipeline
    if (head_align == 0 && aligned) {
      if (aligned == 1) {
        for (j = 0; j < num_elements; ++j) {
          if (i == end - start - 1 && j > tail_align)
            continue;
          a_buf[index2][index1 * num_elements + j] =
              tmp2(a[i + start], data_width, (j * data_width));
        }
        index1++;
        if (index1 == bound) {
          index1 = 0;
          ++index2;
        }
      } else if (aligned == -1) {
        int j0, j1;
        for (j0 = 0; j0 < bound0; ++j0) {
          for (j1 = 0; j1 < SIZE_1; ++j1) {
            j = j0 * SIZE_1 + j1;
            if (i == end - start - 1 && j > tail_align)
              continue;
            a_buf[index2 * bound0 + j0][j1] =
                tmp2(a[i + start], data_width, (j * data_width));
          }
        }
        index2++;
      }
    } else {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < head_align)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buf_index = (i * num_elements + j - head_align) + index_offset;
        a_buf[buf_index / SIZE_1][buf_index % SIZE_1] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }
  }
}

static void memcpy_wide_bus(write, char, SIZE_1, LARGE_BUS)(
    MARS_WIDE_BUS_TYPE *c, char c_buf[][SIZE_1], size_t index2_offset,
    size_t index1_offset, size_t offset_byte, size_t size_byte,
    const size_t const_buf_size) {
#pragma HLS inline self
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
  size_t index_offset = index2_offset * SIZE_1 + index1_offset;
  size_t i, j;
  if (head_align == 0)
    len = (buf_size + num_elements - 1) / num_elements;
  size_t align = 0;
  if (len == 1) {
    for (j = 0; j < num_elements; ++j) {
      if (j < head_align)
        continue;
      if (j > tail_align)
        continue;
      size_t buf_index = j - head_align + index_offset;
      tmp3(c[start], data_width, (j * data_width),
           c_buf[buf_index / SIZE_1][buf_index % SIZE_1]);
    }
    return;
  }
  if (head_align != 0) {
    for (j = 0; j < num_elements; ++j) {
      if (j < head_align)
        continue;
      size_t buf_index = j - head_align + index_offset;
      tmp3(c[start], data_width, (j * data_width),
           c_buf[buf_index / SIZE_1][buf_index % SIZE_1]);
    }
    start++;
    align++;
  }
  if (tail_align != (num_elements - 1))
    align++;
  size_t index2 = index2_offset, index1 = index1_offset;
  int aligned = 0;
  if ((SIZE_1 % num_elements) == 0 && (index1_offset % num_elements) == 0) {
    aligned = 1;
    index1 = index1_offset / num_elements;
  } else if ((num_elements % SIZE_1) == 0 &&
             ((index2_offset * SIZE_1) % num_elements) == 0 && !index1_offset) {
    aligned = -1;
    index2 = index2_offset * SIZE_1 / num_elements;
  }
  const size_t bound = SIZE_1 / num_elements;
  const size_t bound0 = num_elements / SIZE_1;
  int burst_len = len - align;
  assert(burst_len <= const_buf_size / num_elements);
  for (i = 0; i < burst_len; ++i) {
#pragma HLS pipeline
    if (head_align == 0 && aligned) {
      if (aligned == 1) {
        for (j = 0; j < num_elements; ++j) {
          int val = c_buf[index2][index1 * num_elements + j];
          tmp3(c[i + start], data_width, (j * data_width), val);
        }
        index1++;
        if (index1 == bound) {
          index1 = 0;
          ++index2;
        }
      } else if (aligned == -1) {
        int j0, j1;
        for (j0 = 0; j0 < bound0; ++j0) {
          for (j1 = 0; j1 < SIZE_1; ++j1) {
            j = j0 * SIZE_1 + j1;
            int val = c_buf[index2 * bound0 + j0][j1];
            tmp3(c[i + start], data_width, (j * data_width), val);
          }
        }
        index2++;
      }
    } else {
      for (j = 0; j < num_elements; ++j) {
        size_t buf_index =
            (i * num_elements + j + num_elements - head_align) + index_offset;
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[buf_index / SIZE_1][index_offset % SIZE_1]);
      }
    }
  }
  if (tail_align != num_elements - 1) {
    size_t pos = (len - align) * num_elements;
    pos += (num_elements - head_align) % num_elements + index_offset;
    for (j = 0; j < num_elements; ++j) {
      if (j > tail_align)
        continue;
      tmp3(c[end - 1], data_width, (j * data_width),
           c_buf[(pos + j) / SIZE_1][(pos + j) % SIZE_1]);
    }
  }
}

static void memcpy_wide_bus(read, short, SIZE_1, LARGE_BUS)(
    short a_buf[][SIZE_1], size_t index2_offset, size_t index1_offset,
    MARS_WIDE_BUS_TYPE *a, size_t offset_byte, size_t size_byte,
    const size_t const_buf_size) {
#pragma HLS inline self
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
  size_t index2 = index2_offset, index1 = index1_offset;
  int aligned = 0;
  if ((SIZE_1 % num_elements) == 0 && (index1_offset % num_elements) == 0) {
    aligned = 1;
    index1 = index1_offset / num_elements;
  } else if (((num_elements % SIZE_1) == 0) &&
             ((index2_offset * SIZE_1) % num_elements) == 0 && !index1_offset) {
    aligned = -1;
    index2 = index2_offset * SIZE_1 / num_elements;
  }
  const size_t bound = SIZE_1 / num_elements;
  const size_t bound0 = num_elements / SIZE_1;
  const size_t index_offset = index2_offset * SIZE_1 + index1_offset;
  int len = end - start;
  assert(len <= const_buf_size / num_elements + 2);
  assert(len >= buf_size / num_elements);
  if (1 == len) {
    for (j = 0; j < num_elements; ++j) {
      if (j < head_align || j > tail_align)
        continue;
      size_t buf_index = j - head_align + index_offset;
      a_buf[buf_index / SIZE_1][buf_index % SIZE_1] =
          tmp2(a[start], data_width, (j * data_width));
    }
    return;
  }

  for (i = 0; i < len; ++i) {
#pragma HLS pipeline
    if (head_align == 0 && aligned) {
      if (aligned == 1) {
        for (j = 0; j < num_elements; ++j) {
          if (i == end - start - 1 && j > tail_align)
            continue;
          a_buf[index2][index1 * num_elements + j] =
              tmp2(a[i + start], data_width, (j * data_width));
        }
        index1++;
        if (index1 == bound) {
          index1 = 0;
          ++index2;
        }
      } else if (aligned == -1) {
        int j0, j1;
        for (j0 = 0; j0 < bound0; ++j0) {
          for (j1 = 0; j1 < SIZE_1; ++j1) {
            j = j0 * SIZE_1 + j1;
            if (i == end - start - 1 && j > tail_align)
              continue;
            a_buf[index2 * bound0 + j0][j1] =
                tmp2(a[i + start], data_width, (j * data_width));
          }
        }
        index2++;
      }
    } else {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < head_align)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buf_index = (i * num_elements + j - head_align) + index_offset;
        a_buf[buf_index / SIZE_1][buf_index % SIZE_1] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }
  }
}

static void memcpy_wide_bus(write, short, SIZE_1, LARGE_BUS)(
    MARS_WIDE_BUS_TYPE *c, short c_buf[][SIZE_1], size_t index2_offset,
    size_t index1_offset, size_t offset_byte, size_t size_byte,
    const size_t const_buf_size) {
#pragma HLS inline self
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
  size_t index_offset = index2_offset * SIZE_1 + index1_offset;
  size_t i, j;
  if (head_align == 0)
    len = (buf_size + num_elements - 1) / num_elements;
  size_t align = 0;
  if (len == 1) {
    for (j = 0; j < num_elements; ++j) {
      if (j < head_align)
        continue;
      if (j > tail_align)
        continue;
      size_t buf_index = j - head_align + index_offset;
      tmp3(c[start], data_width, (j * data_width),
           c_buf[buf_index / SIZE_1][buf_index % SIZE_1]);
    }
    return;
  }
  if (head_align != 0) {
    for (j = 0; j < num_elements; ++j) {
      if (j < head_align)
        continue;
      size_t buf_index = j - head_align + index_offset;
      tmp3(c[start], data_width, (j * data_width),
           c_buf[buf_index / SIZE_1][buf_index % SIZE_1]);
    }
    start++;
    align++;
  }
  if (tail_align != (num_elements - 1))
    align++;
  size_t index2 = index2_offset, index1 = index1_offset;
  int aligned = 0;
  if ((SIZE_1 % num_elements) == 0 && (index1_offset % num_elements) == 0) {
    aligned = 1;
    index1 = index1_offset / num_elements;
  } else if ((num_elements % SIZE_1) == 0 &&
             ((index2_offset * SIZE_1) % num_elements) == 0 && !index1_offset) {
    aligned = -1;
    index2 = index2_offset * SIZE_1 / num_elements;
  }
  const size_t bound = SIZE_1 / num_elements;
  const size_t bound0 = num_elements / SIZE_1;
  int burst_len = len - align;
  assert(burst_len <= const_buf_size / num_elements);
  for (i = 0; i < burst_len; ++i) {
#pragma HLS pipeline
    if (head_align == 0 && aligned) {
      if (aligned == 1) {
        for (j = 0; j < num_elements; ++j) {
          int val = c_buf[index2][index1 * num_elements + j];
          tmp3(c[i + start], data_width, (j * data_width), val);
        }
        index1++;
        if (index1 == bound) {
          index1 = 0;
          ++index2;
        }
      } else if (aligned == -1) {
        int j0, j1;
        for (j0 = 0; j0 < bound0; ++j0) {
          for (j1 = 0; j1 < SIZE_1; ++j1) {
            j = j0 * SIZE_1 + j1;
            int val = c_buf[index2 * bound0 + j0][j1];
            tmp3(c[i + start], data_width, (j * data_width), val);
          }
        }
        index2++;
      }
    } else {
      for (j = 0; j < num_elements; ++j) {
        size_t buf_index =
            (i * num_elements + j + num_elements - head_align) + index_offset;
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[buf_index / SIZE_1][index_offset % SIZE_1]);
      }
    }
  }
  if (tail_align != num_elements - 1) {
    size_t pos = (len - align) * num_elements;
    pos += (num_elements - head_align) % num_elements + index_offset;
    for (j = 0; j < num_elements; ++j) {
      if (j > tail_align)
        continue;
      tmp3(c[end - 1], data_width, (j * data_width),
           c_buf[(pos + j) / SIZE_1][(pos + j) % SIZE_1]);
    }
  }
}

static void memcpy_wide_bus(read, long, SIZE_1, LARGE_BUS)(
    long a_buf[][SIZE_1], size_t index2_offset, size_t index1_offset,
    MARS_WIDE_BUS_TYPE *a,

    size_t offset_byte, size_t size_byte, const size_t const_buf_size) {
#pragma HLS inline self
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
  size_t index2 = index2_offset, index1 = index1_offset;
  int aligned = 0;
  if ((SIZE_1 % num_elements) == 0 && (index1_offset % num_elements) == 0) {
    aligned = 1;
    index1 = index1_offset / num_elements;
  } else if ((num_elements % SIZE_1) == 0 &&
             (index2_offset * SIZE_1 % num_elements) == 0 && !index1_offset) {
    aligned = -1;
    index2 = index2_offset * SIZE_1 / num_elements;
  }
  const size_t bound = SIZE_1 / num_elements;
  const size_t bound0 = num_elements / SIZE_1;
  const size_t index_offset = index2_offset * SIZE_1 + index1_offset;
  int len = end - start;
  assert(len <= const_buf_size / num_elements + 2);
  assert(len >= buf_size / num_elements);
  if (1 == len) {
    for (j = 0; j < num_elements; ++j) {
      if (j < head_align || j > tail_align)
        continue;
      size_t buf_index = j - head_align + index_offset;
      a_buf[buf_index / SIZE_1][buf_index % SIZE_1] =
          tmp2(a[start], data_width, (j * data_width));
    }
    return;
  }

  for (i = 0; i < len; ++i) {
#pragma HLS pipeline
    if (head_align == 0 && aligned) {
      if (aligned == 1) {
        for (j = 0; j < num_elements; ++j) {
          if (i == end - start - 1 && j > tail_align)
            continue;
          a_buf[index2][index1 * num_elements + j] =
              tmp2(a[i + start], data_width, (j * data_width));
        }
        index1++;
        if (index1 == bound) {
          index1 = 0;
          ++index2;
        }
      } else if (aligned == -1) {
        int j0, j1;
        for (j0 = 0; j0 < bound0; ++j0) {
          for (j1 = 0; j1 < SIZE_1; ++j1) {
            j = j0 * SIZE_1 + j1;
            if (i == end - start - 1 && j > tail_align)
              continue;
            a_buf[index2 * bound0 + j0][j1] =
                tmp2(a[i + start], data_width, (j * data_width));
          }
        }
        index2++;
      }

    } else {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < head_align)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buf_index = (i * num_elements + j - head_align) + index_offset;
        a_buf[buf_index / SIZE_1][buf_index % SIZE_1] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }
  }
}

static void memcpy_wide_bus(write, long, SIZE_1, LARGE_BUS)(
    MARS_WIDE_BUS_TYPE *c, long c_buf[][SIZE_1], size_t index2_offset,
    size_t index1_offset, size_t offset_byte, size_t size_byte,
    const size_t const_buf_size) {
#pragma HLS inline self
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
  size_t index_offset = index2_offset * SIZE_1 + index1_offset;
  if (head_align == 0)
    len = (buf_size + num_elements - 1) / num_elements;
  size_t align = 0;
  if (len == 1) {
    for (j = 0; j < num_elements; ++j) {
      if (j < head_align)
        continue;
      if (j > tail_align)
        continue;
      size_t buf_index = (j - head_align) + index_offset;
      tmp3(c[start], data_width, (j * data_width),
           c_buf[buf_index / SIZE_1][buf_index % SIZE_1]);
    }
    return;
  }
  if (head_align != 0) {
    for (j = 0; j < num_elements; ++j) {
      if (j < head_align)
        continue;
      size_t buf_index = (j - head_align) + index_offset;
      tmp3(c[start], data_width, (j * data_width),
           c_buf[buf_index / SIZE_1][buf_index % SIZE_1]);
    }
    start++;
    align++;
  }
  if (tail_align != (num_elements - 1))
    align++;
  size_t index2 = index2_offset, index1 = index1_offset;
  int aligned = 0;
  if ((SIZE_1 % num_elements) == 0 && (index1_offset % num_elements) == 0) {
    aligned = 1;
    index1 = index1_offset / num_elements;
  } else if (((num_elements % SIZE_1) == 0) &&
             ((index2_offset * SIZE_1) % num_elements) == 0 && !index1_offset) {
    aligned = -1;
    index2 = index2_offset * SIZE_1 / num_elements;
  }
  const size_t bound = SIZE_1 / num_elements;
  const size_t bound0 = num_elements / SIZE_1;
  int burst_len = len - align;
  assert(burst_len <= const_buf_size / num_elements);
  for (i = 0; i < burst_len; ++i) {
#pragma HLS pipeline
    if (head_align == 0 && aligned) {
      if (aligned == 1) {
        for (j = 0; j < num_elements; ++j) {
          long val = c_buf[index2][index1 * num_elements + j];
          tmp3(c[i + start], data_width, (j * data_width), val);
        }
        index1++;
        if (index1 == bound) {
          index1 = 0;
          ++index2;
        }
      } else if (aligned == -1) {
        int j0, j1;
        for (j0 = 0; j0 < bound0; ++j0) {
          for (j1 = 0; j1 < SIZE_1; ++j1) {
            j = j0 * SIZE_1 + j1;
            long val = c_buf[index2 * bound0 + j0][j1];
            tmp3(c[i + start], data_width, (j * data_width), val);
          }
        }
        index2++;
      }
    } else {
      for (j = 0; j < num_elements; ++j) {
        size_t buf_index =
            (i * num_elements + j + num_elements - head_align) + index_offset;
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[buf_index / SIZE_1][buf_index % SIZE_1]);
      }
    }
  }
  if (tail_align != num_elements - 1) {
    size_t pos = (len - align) * num_elements + index_offset;
    pos += (num_elements - head_align) % num_elements;
    for (j = 0; j < num_elements; ++j) {
      if (j > tail_align)
        continue;
      tmp3(c[end - 1], data_width, (j * data_width),
           c_buf[(pos + j) / SIZE_1][(pos + j) % SIZE_1]);
    }
  }
}

static void memcpy_wide_bus(read, int, SIZE_1, LARGE_BUS)(
    int a_buf[][SIZE_1], size_t index2_offset, size_t index1_offset,
    MARS_WIDE_BUS_TYPE *a,

    size_t offset_byte, size_t size_byte, const size_t const_buf_size) {
#pragma HLS inline self
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
  size_t index2 = index2_offset, index1 = index1_offset;
  int aligned = 0;
  if ((SIZE_1 % num_elements) == 0 && (index1_offset % num_elements) == 0) {
    aligned = 1;
    index1 = index1_offset / num_elements;
  } else if ((num_elements % SIZE_1) == 0 &&
             (index2_offset * SIZE_1 % num_elements) == 0 && !index1_offset) {
    aligned = -1;
    index2 = index2_offset * SIZE_1 / num_elements;
  }
  const size_t bound = SIZE_1 / num_elements;
  const size_t bound0 = num_elements / SIZE_1;
  const size_t index_offset = index2_offset * SIZE_1 + index1_offset;
  int len = end - start;
  assert(len <= const_buf_size / num_elements + 2);
  assert(len >= buf_size / num_elements);
  if (1 == len) {
    for (j = 0; j < num_elements; ++j) {
      if (j < head_align || j > tail_align)
        continue;
      size_t buf_index = j - head_align + index_offset;
      a_buf[buf_index / SIZE_1][buf_index % SIZE_1] =
          tmp2(a[start], data_width, (j * data_width));
    }
    return;
  }

  for (i = 0; i < len; ++i) {
#pragma HLS pipeline
    if (head_align == 0 && aligned) {
      if (aligned == 1) {
        for (j = 0; j < num_elements; ++j) {
          if (i == end - start - 1 && j > tail_align)
            continue;
          a_buf[index2][index1 * num_elements + j] =
              tmp2(a[i + start], data_width, (j * data_width));
        }
        index1++;
        if (index1 == bound) {
          index1 = 0;
          ++index2;
        }
      } else if (aligned == -1) {
        int j0, j1;
        for (j0 = 0; j0 < bound0; ++j0) {
          for (j1 = 0; j1 < SIZE_1; ++j1) {
            j = j0 * SIZE_1 + j1;
            if (i == end - start - 1 && j > tail_align)
              continue;
            a_buf[index2 * bound0 + j0][j1] =
                tmp2(a[i + start], data_width, (j * data_width));
          }
        }
        index2++;
      }

    } else {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < head_align)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buf_index = (i * num_elements + j - head_align) + index_offset;
        a_buf[buf_index / SIZE_1][buf_index % SIZE_1] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }
  }
}

static void memcpy_wide_bus(write, long_long, SIZE_1, LARGE_BUS)(
    MARS_WIDE_BUS_TYPE *c, long c_buf[][SIZE_1], size_t index2_offset,
    size_t index1_offset, size_t offset_byte, size_t size_byte,
    const size_t const_buf_size) {
#pragma HLS inline self
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
  size_t index_offset = index2_offset * SIZE_1 + index1_offset;
  if (head_align == 0)
    len = (buf_size + num_elements - 1) / num_elements;
  size_t align = 0;
  if (len == 1) {
    for (j = 0; j < num_elements; ++j) {
      if (j < head_align)
        continue;
      if (j > tail_align)
        continue;
      size_t buf_index = (j - head_align) + index_offset;
      tmp3(c[start], data_width, (j * data_width),
           c_buf[buf_index / SIZE_1][buf_index % SIZE_1]);
    }
    return;
  }
  if (head_align != 0) {
    for (j = 0; j < num_elements; ++j) {
      if (j < head_align)
        continue;
      size_t buf_index = (j - head_align) + index_offset;
      tmp3(c[start], data_width, (j * data_width),
           c_buf[buf_index / SIZE_1][buf_index % SIZE_1]);
    }
    start++;
    align++;
  }
  if (tail_align != (num_elements - 1))
    align++;
  size_t index2 = index2_offset, index1 = index1_offset;
  int aligned = 0;
  if ((SIZE_1 % num_elements) == 0 && (index1_offset % num_elements) == 0) {
    aligned = 1;
    index1 = index1_offset / num_elements;
  } else if (((num_elements % SIZE_1) == 0) &&
             ((index2_offset * SIZE_1) % num_elements) == 0 && !index1_offset) {
    aligned = -1;
    index2 = index2_offset * SIZE_1 / num_elements;
  }
  const size_t bound = SIZE_1 / num_elements;
  const size_t bound0 = num_elements / SIZE_1;
  int burst_len = len - align;
  assert(burst_len <= const_buf_size / num_elements);
  for (i = 0; i < burst_len; ++i) {
#pragma HLS pipeline
    if (head_align == 0 && aligned) {
      if (aligned == 1) {
        for (j = 0; j < num_elements; ++j) {
          long val = c_buf[index2][index1 * num_elements + j];
          tmp3(c[i + start], data_width, (j * data_width), val);
        }
        index1++;
        if (index1 == bound) {
          index1 = 0;
          ++index2;
        }
      } else if (aligned == -1) {
        int j0, j1;
        for (j0 = 0; j0 < bound0; ++j0) {
          for (j1 = 0; j1 < SIZE_1; ++j1) {
            j = j0 * SIZE_1 + j1;
            long val = c_buf[index2 * bound0 + j0][j1];
            tmp3(c[i + start], data_width, (j * data_width), val);
          }
        }
        index2++;
      }
    } else {
      for (j = 0; j < num_elements; ++j) {
        size_t buf_index =
            (i * num_elements + j + num_elements - head_align) + index_offset;
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[buf_index / SIZE_1][buf_index % SIZE_1]);
      }
    }
  }
  if (tail_align != num_elements - 1) {
    size_t pos = (len - align) * num_elements + index_offset;
    pos += (num_elements - head_align) % num_elements;
    for (j = 0; j < num_elements; ++j) {
      if (j > tail_align)
        continue;
      tmp3(c[end - 1], data_width, (j * data_width),
           c_buf[(pos + j) / SIZE_1][(pos + j) % SIZE_1]);
    }
  }
}

static void memcpy_wide_bus(read, long_long, SIZE_1, LARGE_BUS)(
    long a_buf[][SIZE_1], size_t index2_offset, size_t index1_offset,
    MARS_WIDE_BUS_TYPE *a,

    size_t offset_byte, size_t size_byte, const size_t const_buf_size) {
#pragma HLS inline self
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
  size_t index2 = index2_offset, index1 = index1_offset;
  int aligned = 0;
  if ((SIZE_1 % num_elements) == 0 && (index1_offset % num_elements) == 0) {
    aligned = 1;
    index1 = index1_offset / num_elements;
  } else if ((num_elements % SIZE_1) == 0 &&
             (index2_offset * SIZE_1 % num_elements) == 0 && !index1_offset) {
    aligned = -1;
    index2 = index2_offset * SIZE_1 / num_elements;
  }
  const size_t bound = SIZE_1 / num_elements;
  const size_t bound0 = num_elements / SIZE_1;
  const size_t index_offset = index2_offset * SIZE_1 + index1_offset;
  int len = end - start;
  assert(len <= const_buf_size / num_elements + 2);
  assert(len >= buf_size / num_elements);
  if (1 == len) {
    for (j = 0; j < num_elements; ++j) {
      if (j < head_align || j > tail_align)
        continue;
      size_t buf_index = j - head_align + index_offset;
      a_buf[buf_index / SIZE_1][buf_index % SIZE_1] =
          tmp2(a[start], data_width, (j * data_width));
    }
    return;
  }

  for (i = 0; i < len; ++i) {
#pragma HLS pipeline
    if (head_align == 0 && aligned) {
      if (aligned == 1) {
        for (j = 0; j < num_elements; ++j) {
          if (i == end - start - 1 && j > tail_align)
            continue;
          a_buf[index2][index1 * num_elements + j] =
              tmp2(a[i + start], data_width, (j * data_width));
        }
        index1++;
        if (index1 == bound) {
          index1 = 0;
          ++index2;
        }
      } else if (aligned == -1) {
        int j0, j1;
        for (j0 = 0; j0 < bound0; ++j0) {
          for (j1 = 0; j1 < SIZE_1; ++j1) {
            j = j0 * SIZE_1 + j1;
            if (i == end - start - 1 && j > tail_align)
              continue;
            a_buf[index2 * bound0 + j0][j1] =
                tmp2(a[i + start], data_width, (j * data_width));
          }
        }
        index2++;
      }

    } else {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < head_align)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t buf_index = (i * num_elements + j - head_align) + index_offset;
        a_buf[buf_index / SIZE_1][buf_index % SIZE_1] =
            tmp2(a[i + start], data_width, (j * data_width));
      }
    }
  }
}

static void memcpy_wide_bus(write, int, SIZE_1, LARGE_BUS)(
    MARS_WIDE_BUS_TYPE *c, int c_buf[][SIZE_1], size_t index2_offset,
    size_t index1_offset, size_t offset_byte, size_t size_byte,
    const size_t const_buf_size) {
#pragma HLS inline self
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
  size_t index_offset = index2_offset * SIZE_1 + index1_offset;
  if (head_align == 0)
    len = (buf_size + num_elements - 1) / num_elements;
  size_t align = 0;
  if (len == 1) {
    for (j = 0; j < num_elements; ++j) {
      if (j < head_align)
        continue;
      if (j > tail_align)
        continue;
      size_t buf_index = (j - head_align) + index_offset;
      tmp3(c[start], data_width, (j * data_width),
           c_buf[buf_index / SIZE_1][buf_index % SIZE_1]);
    }
    return;
  }
  if (head_align != 0) {
    for (j = 0; j < num_elements; ++j) {
      if (j < head_align)
        continue;
      size_t buf_index = (j - head_align) + index_offset;
      tmp3(c[start], data_width, (j * data_width),
           c_buf[buf_index / SIZE_1][buf_index % SIZE_1]);
    }
    start++;
    align++;
  }
  if (tail_align != (num_elements - 1))
    align++;
  size_t index2 = index2_offset, index1 = index1_offset;
  int aligned = 0;
  if ((SIZE_1 % num_elements) == 0 && (index1_offset % num_elements) == 0) {
    aligned = 1;
    index1 = index1_offset / num_elements;
  } else if (((num_elements % SIZE_1) == 0) &&
             ((index2_offset * SIZE_1) % num_elements) == 0 && !index1_offset) {
    aligned = -1;
    index2 = index2_offset * SIZE_1 / num_elements;
  }
  const size_t bound = SIZE_1 / num_elements;
  const size_t bound0 = num_elements / SIZE_1;
  int burst_len = len - align;
  assert(burst_len <= const_buf_size / num_elements);
  for (i = 0; i < burst_len; ++i) {
#pragma HLS pipeline
    if (head_align == 0 && aligned) {
      if (aligned == 1) {
        for (j = 0; j < num_elements; ++j) {
          int val = c_buf[index2][index1 * num_elements + j];
          tmp3(c[i + start], data_width, (j * data_width), val);
        }
        index1++;
        if (index1 == bound) {
          index1 = 0;
          ++index2;
        }
      } else if (aligned == -1) {
        int j0, j1;
        for (j0 = 0; j0 < bound0; ++j0) {
          for (j1 = 0; j1 < SIZE_1; ++j1) {
            j = j0 * SIZE_1 + j1;
            int val = c_buf[index2 * bound0 + j0][j1];
            tmp3(c[i + start], data_width, (j * data_width), val);
          }
        }
        index2++;
      }
    } else {
      for (j = 0; j < num_elements; ++j) {
        size_t buf_index =
            (i * num_elements + j + num_elements - head_align) + index_offset;
        tmp3(c[i + start], data_width, (j * data_width),
             c_buf[buf_index / SIZE_1][buf_index % SIZE_1]);
      }
    }
  }
  if (tail_align != num_elements - 1) {
    size_t pos = (len - align) * num_elements + index_offset;
    pos += (num_elements - head_align) % num_elements;
    for (j = 0; j < num_elements; ++j) {
      if (j > tail_align)
        continue;
      tmp3(c[end - 1], data_width, (j * data_width),
           c_buf[(pos + j) / SIZE_1][(pos + j) % SIZE_1]);
    }
  }
}

static void memcpy_wide_bus(read, float, SIZE_1, LARGE_BUS)(
    float a_buf[][SIZE_1], size_t index2_offset, size_t index1_offset,
    MARS_WIDE_BUS_TYPE *a, size_t offset_byte, size_t size_byte,
    const size_t const_buf_size) {
#pragma HLS inline self
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
  size_t index2 = index2_offset, index1 = index1_offset;
  const size_t index_offset = SIZE_1 * index2_offset + index1_offset;
  const size_t bound = SIZE_1 / num_elements;
  const size_t bound0 = num_elements / SIZE_1;
  int aligned = 0;
  if ((SIZE_1 % num_elements) == 0 && (index1_offset % num_elements) == 0) {
    aligned = 1;
    index1 = index1_offset / num_elements;
  } else if (((num_elements % SIZE_1) == 0) &&
             ((index2_offset * SIZE_1) % num_elements) == 0 && !index1_offset) {
    aligned = -1;
    index2 = index2_offset * SIZE_1 / num_elements;
  }
  int len = end - start;

  assert(len <= const_buf_size / num_elements + 2);
  assert(len >= buf_size / num_elements);
  if (1 == len) {
    for (j = 0; j < num_elements; ++j) {
      if (j < head_align || j > tail_align)
        continue;
      size_t buf_index = j - head_align + index_offset;
      int raw_bits = tmp2(a[start], data_width, (j * data_width));
      a_buf[buf_index / SIZE_1][buf_index % SIZE_1] = *(float *)(&raw_bits);
    }
    return;
  }

  for (i = 0; i < len; ++i) {
#pragma HLS pipeline

    if (head_align == 0 && aligned) {
      if (aligned == 1) {
        for (j = 0; j < num_elements; ++j) {
          if (i == end - start - 1 && j > tail_align)
            continue;
          int raw_bits = tmp2(a[i + start], data_width, (j * data_width));
          a_buf[index2][index1 * num_elements + j] = *(float *)(&raw_bits);
        }
        index1++;
        if (index1 == bound) {
          index1 = 0;
          ++index2;
        }
      } else if (aligned == -1) {
        int j0, j1;
        for (j0 = 0; j0 < bound0; ++j0) {
          for (j1 = 0; j1 < SIZE_1; ++j1) {
            j = j0 * SIZE_1 + j1;
            if (i == end - start - 1 && j > tail_align)
              continue;
            int raw_bits = tmp2(a[i + start], data_width, (j * data_width));
            a_buf[index2 * bound0 + j0][j1] = *(float *)(&raw_bits);
          }
        }
        index2++;
      }
    } else {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < head_align)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        int raw_bits = tmp2(a[i + start], data_width, (j * data_width));
        size_t buf_index = (i * num_elements + j - head_align) + index_offset;
        a_buf[buf_index / SIZE_1][buf_index % SIZE_1] = *(float *)(&raw_bits);
      }
    }
  }
}

static void memcpy_wide_bus(write, float, SIZE_1, LARGE_BUS)(
    MARS_WIDE_BUS_TYPE *c, float c_buf[][SIZE_1], size_t index2_offset,
    size_t index1_offset, size_t offset_byte, size_t size_byte,
    const size_t const_buf_size) {
#pragma HLS inline self
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
  const size_t index_offset = index2_offset * SIZE_1 + index1_offset;
  size_t i, j;
  if (head_align == 0)
    len = (buf_size + num_elements - 1) / num_elements;
  if (len == 1) {
    for (j = 0; j < num_elements; ++j) {
      if (j < head_align)
        continue;
      if (j > tail_align)
        continue;
      size_t buf_index = (j - head_align) + index_offset;
      float buf_tmp = c_buf[buf_index / SIZE_1][buf_index % SIZE_1];
      int raw_bits = *(int *)&buf_tmp;
      tmp3(c[start], data_width, (j * data_width), raw_bits);
    }
    return;
  }
  unsigned align = 0;
  if (head_align != 0) {
    for (j = 0; j < num_elements; ++j) {
      if (j < head_align)
        continue;
      size_t buf_index = (j - head_align) + index_offset;
      float buf_tmp = c_buf[buf_index / SIZE_1][buf_index % SIZE_1];
      int raw_bits = *(int *)&buf_tmp;
      tmp3(c[start], data_width, (j * data_width), raw_bits);
    }
    start++;
    align++;
  }
  if (tail_align != (num_elements - 1))
    align++;
  size_t index2 = index2_offset, index1 = index1_offset;
  int aligned = 0;
  if ((SIZE_1 % num_elements) == 0 && (index1_offset % num_elements) == 0) {
    aligned = 1;
    index1 = index1_offset / num_elements;
  } else if (((num_elements % SIZE_1) == 0) &&
             ((index2_offset * SIZE_1) % num_elements) == 0 && !index1_offset) {
    aligned = -1;
    index2 = index2_offset * SIZE_1 / num_elements;
  }
  const size_t bound = SIZE_1 / num_elements;
  const size_t bound0 = num_elements / SIZE_1;
  int burst_len = len - align;
  assert(burst_len <= const_buf_size / num_elements);
  for (i = 0; i < burst_len; ++i) {
#pragma HLS pipeline
    if (head_align == 0 && aligned) {
      if (aligned == 1) {
        for (j = 0; j < num_elements; ++j) {
          float buf_tmp = c_buf[index2][index1 * num_elements + j];
          int raw_bits = *(int *)&buf_tmp;
          tmp3(c[i + start], data_width, (j * data_width), raw_bits);
        }
        index1++;
        if (index1 == bound) {
          index1 = 0;
          ++index2;
        }
      } else if (aligned == -1) {
        int j0, j1;
        for (j0 = 0; j0 < bound0; ++j0) {
          for (j1 = 0; j1 < SIZE_1; ++j1) {
            j = j0 * SIZE_1 + j1;
            float buf_tmp = c_buf[index2 * bound0 + j0][j1];
            int raw_bits = *(int *)&buf_tmp;
            tmp3(c[i + start], data_width, (j * data_width), raw_bits);
          }
        }
        index2++;
      }
    } else {
      for (j = 0; j < num_elements; ++j) {
        size_t index =
            i * num_elements + j + num_elements - head_align + index_offset;
        float buf_tmp = c_buf[index / SIZE_1][index % SIZE_1];
        int raw_bits = *(int *)&buf_tmp;
        tmp3(c[i + start], data_width, (j * data_width), raw_bits);
      }
    }
  }
  if (tail_align != num_elements - 1) {
    size_t pos = (len - align) * num_elements + index_offset;
    pos += (num_elements - head_align) % num_elements;
    for (j = 0; j < num_elements; ++j) {
      if (j > tail_align)
        continue;
      float buf_tmp = c_buf[(pos + j) / SIZE_1][(pos + j) % SIZE_1];
      int raw_bits = *(int *)&buf_tmp;
      tmp3(c[end - 1], data_width, (j * data_width), raw_bits);
    }
  }
}

static void memcpy_wide_bus(read, double, SIZE_1, LARGE_BUS)(
    double a_buf[][SIZE_1], size_t index2_offset, size_t index1_offset,
    MARS_WIDE_BUS_TYPE *a, size_t offset_byte, size_t size_byte,
    const size_t const_buf_size) {
#pragma HLS inline self
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
  size_t index2 = index2_offset, index1 = index1_offset;
  int aligned = 0;
  if ((SIZE_1 % num_elements) == 0 && (index1_offset % num_elements) == 0) {
    aligned = 1;
    index1 = index1_offset / num_elements;
  } else if (((num_elements % SIZE_1) == 0) &&
             ((index2_offset * SIZE_1) % num_elements) == 0 && !index1_offset) {
    aligned = -1;
    index2 = index2_offset * SIZE_1 / num_elements;
  }
  const size_t bound = SIZE_1 / num_elements;
  const size_t bound0 = num_elements / SIZE_1;
  const size_t index_offset = SIZE_1 * index2_offset + index1_offset;

  int len = end - start;
  assert(len <= const_buf_size / num_elements + 2);
  assert(len >= buf_size / num_elements);
  if (1 == len) {
    for (j = 0; j < num_elements; ++j) {
      if (j < head_align || j > tail_align)
        continue;
      long raw_bits = tmp2(a[start], data_width, (j * data_width));
      size_t buf_index = (j - head_align) + index_offset;
      a_buf[buf_index / SIZE_1][buf_index % SIZE_1] = *(double *)(&raw_bits);
    }
    return;
  }
  for (i = 0; i < len; ++i) {
#pragma HLS pipeline
    if (head_align == 0 && aligned) {
      if (aligned == 1) {
        for (j = 0; j < num_elements; ++j) {
          if (i == end - start - 1 && j > tail_align)
            continue;
          long raw_bits = tmp2(a[i + start], data_width, (j * data_width));
          a_buf[index2][index1 * num_elements + j] = *(double *)(&raw_bits);
        }
        index1++;
        if (index1 == bound) {
          index1 = 0;
          ++index2;
        }
      } else if (aligned == -1) {
        int j0, j1;
        for (j0 = 0; j0 < bound0; ++j0) {
          for (j1 = 0; j1 < SIZE_1; ++j1) {
            j = j0 * SIZE_1 + j1;
            if (i == end - start - 1 && j > tail_align)
              continue;
            long raw_bits =
                tmp2(a[i + start], data_width, (j * data_width));
            a_buf[index2 * bound0 + j0][j1] = *(double *)(&raw_bits);
          }
        }
        index2++;
      }
    } else {
      for (j = 0; j < num_elements; ++j) {
        if (i == 0 && j < head_align)
          continue;
        if (i == end - start - 1 && j > tail_align)
          continue;
        size_t index = i * num_elements + j - head_align + index_offset;
        long raw_bits = tmp2(a[i + start], data_width, (j * data_width));
        a_buf[index / SIZE_1][index % SIZE_1] = *(double *)(&raw_bits);
      }
    }
  }
}

static void memcpy_wide_bus(write, double, SIZE_1, LARGE_BUS)(
    MARS_WIDE_BUS_TYPE *c, double c_buf[][SIZE_1], size_t index2_offset,
    size_t index1_offset, size_t offset_byte, size_t size_byte,
    const size_t const_buf_size) {
#pragma HLS inline self
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
  size_t index_offset = index2_offset * SIZE_1 + index1_offset;
  size_t i, j;
  if (head_align == 0)
    len = (buf_size + num_elements - 1) / num_elements;
  if (len == 1) {
    for (j = 0; j < num_elements; ++j) {
      if (j < head_align)
        continue;
      if (j > tail_align)
        continue;
      size_t buf_index = (j - head_align) + index_offset;
      double buf_tmp = c_buf[buf_index / SIZE_1][buf_index % SIZE_1];
      long raw_bits = *(long *)&buf_tmp;
      tmp3(c[start], data_width, (j * data_width), raw_bits);
    }
    return;
  }
  size_t align = 0;
  if (head_align != 0) {
    for (j = 0; j < num_elements; ++j) {
      if (j < head_align)
        continue;
      size_t buf_index = (j - head_align) + index_offset;
      double buf_tmp = c_buf[buf_index / SIZE_1][buf_index % SIZE_1];
      long raw_bits = *(long *)&buf_tmp;
      tmp3(c[start], data_width, (j * data_width), raw_bits);
    }
    start++;
    align++;
  }
  if (tail_align != (num_elements - 1))
    align++;
  size_t index2 = index2_offset, index1 = index1_offset;
  const size_t alignment =
      (SIZE_1 % num_elements) == 0 && (index1_offset % num_elements) == 0;
  int aligned = 0;
  if ((SIZE_1 % num_elements) == 0 && (index1_offset % num_elements) == 0) {
    aligned = 1;
    index1 = index1_offset / num_elements;
  } else if (((num_elements % SIZE_1) == 0) &&
             ((index2_offset * SIZE_1) % num_elements) == 0 && !index1_offset) {
    aligned = -1;
    index2 = index2_offset * SIZE_1 / num_elements;
  }
  const size_t bound = SIZE_1 / num_elements;
  const size_t bound0 = num_elements / SIZE_1;
  int burst_len = len - align;
  assert(burst_len <= const_buf_size / num_elements);
  for (i = 0; i < burst_len; ++i) {
#pragma HLS pipeline
    if (head_align == 0 && aligned) {
      if (aligned == 1) {
        for (j = 0; j < num_elements; ++j) {
          double buf_tmp = c_buf[index2][index1 * num_elements + j];
          long raw_bits = *(long *)&buf_tmp;
          tmp3(c[i + start], data_width, (j * data_width), raw_bits);
        }
        index1++;
        if (index1 == bound) {
          index1 = 0;
          ++index2;
        }
      } else if (aligned == -1) {
        int j0, j1;
        for (j0 = 0; j0 < bound0; ++j0) {
          for (j1 = 0; j1 < SIZE_1; ++j1) {
            j = j0 * SIZE_1 + j1;
            double buf_tmp = c_buf[index2 * bound0 + j0][j1];
            long raw_bits = *(long *)&buf_tmp;
            tmp3(c[i + start], data_width, (j * data_width), raw_bits);
          }
        }
        index2++;
      }

    } else {
      for (j = 0; j < num_elements; ++j) {
        size_t index =
            i * num_elements + j + num_elements - head_align + index_offset;
        double buf_tmp = c_buf[index / SIZE_1][index % SIZE_1];
        long raw_bits = *(long *)&buf_tmp;
        tmp3(c[i + start], data_width, (j * data_width), raw_bits);
      }
    }
  }
  if (tail_align != num_elements - 1) {
    size_t pos = (len - align) * num_elements + index_offset;
    pos += (num_elements - head_align) % num_elements;
    for (j = 0; j < num_elements; ++j) {
      if (j > tail_align)
        continue;
      double buf_tmp = c_buf[(pos + j) / SIZE_1][(pos + j) % SIZE_1];
      long raw_bits = *(long *)&buf_tmp;
      tmp3(c[end - 1], data_width, (j * data_width), raw_bits);
    }
  }
}

#undef memcpy_wide_bus
#undef instant_size
#undef LARGE_BUS
#undef BUS_WIDTH
#undef cpp_get_range
#undef c_get_range
#undef cpp_set_range
#undef c_set_range
#undef tmp2
#undef tmp3
