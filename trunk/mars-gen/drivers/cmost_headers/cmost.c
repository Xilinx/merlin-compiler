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


#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "cmost.h"

#include <memory.h>
#include <sys/time.h>
#include <assert.h>
#include <math.h>
#define AOCL_ALIGNMENT 64
static int count = 0;
static void *map_pointer[256];
static int map_size[256];
typedef int data_type;

char data_file_prefix[1024];
void user_main_wrapper(int, char **, char *);
// int main(int argc, char ** argv)
//{
//    if (argc >= 3) {
//        strcpy(data_file_prefix, argv[2]);
//    }
//    else {
//        strcpy(data_file_prefix, "");
//    }
//    char * bitstream = argv[argc-1];
////    opencl_init("kernel_top.aocx");
////    if(strstr(bistream, ".aocx") != NULL) {
////    }
//    user_main_wrapper(argc, argv, bitstream);
//    report_timer();
//    return 0;
//}

#define NUM_TIMER 10
struct timeval g_t1[NUM_TIMER], g_t2[NUM_TIMER], g_tr[NUM_TIMER];
// void cmost_start_timer(int i)
//{
//    assert(i < 10);
//	gettimeofday(&g_t1[i], NULL);
//}
// float cmost_read_timer(int i)
//{
//    struct	timeval g_tr;
//	gettimeofday(&g_t2[i], NULL);
//	timersub(&g_t1[i], &g_t2[i], &g_tr);
//    float sec = fabs(g_tr.tv_sec+(double)g_tr.tv_usec/1000000.0);
//	return sec;
//}
// void cmost_read_timer_new(int i, float * sec)
//{
//    struct	timeval g_tr;
//	gettimeofday(&g_t2[i], NULL);
//	timersub(&g_t1[i], &g_t2[i], &g_tr);
//    *sec = fabs(g_tr.tv_sec+(double)g_tr.tv_usec/1000000.0);
//}

int cmost_free(void *a) {
  cmost_free_1d(a);
  return 1;
}
int cmost_dump(void *a, const char *filename) {
  cmost_dump_1d(a, filename);
  return 1;
}

int cmost_malloc(void **a, size_t size) {
  return cmost_malloc_1d(a, "", 4, size / 4);
}
void cmost_load_file(void *buffer, const char *file_name, size_t size) {
  if (strlen(file_name) == 0) {
    return;
  } else if (strcmp(file_name, "0") == 0) {
    memset(buffer, 0, size);
  } else if (strcmp(file_name, "+") == 0) {
    size_t i;
    for (i = 0; i < size / 4; i++)
      ((int *)buffer)[i] = i;
  } else if (strcmp(file_name, "&") == 0) {
    size_t i;
    for (i = 0; i < size / 4; i++)
      ((int *)buffer)[i] = rand();
  } else {

    FILE *fp;
    char fullname[1024];
    if (file_name[0] != '/') {
      strcpy(fullname, data_file_prefix);
      strcat(fullname, file_name);
    } else
      strcpy(fullname, file_name);
    printf("Open file -- %s\n", fullname);
    if (!(fp = fopen(fullname, "rb"))) {
      // FILE * fp;
      // if (!(fp = fopen(file_name, "rb"))) {
      printf("File %s can not be open for read.\n", file_name);
      exit;
    }

    fread(buffer, 1, size, fp);

    fclose(fp);
  }
}

void cmost_write_file(void *buffer, const char *file_name, size_t size) {
  FILE *fp;
  char fullname[1024];
  if (file_name[0] != '/') {
    strcpy(fullname, data_file_prefix);
    strcat(fullname, file_name);
  } else
    strcpy(fullname, file_name);
  if (!(fp = fopen(fullname, "wb"))) {
    // FILE * fp;
    // if (!(fp = fopen(file_name, "wb"))) {
    printf("File %s can not be open for write.\n", file_name);
    exit;
  }

  fwrite(buffer, 1, size, fp);

  fclose(fp);
}

int cmost_malloc_1d(void **a, const char *filename, int unit_size, size_t d0) {
  FILE *fp;

  /* allocate 3 more words to store dimensions d0, d1, d2, at the start of the
   * container */
  assert(
      sizeof(data_type) ==
      sizeof(
          int)); /* currently only support when data and int have same size */
  assert(
      unit_size ==
      sizeof(
          int)); /* currently only support when data and int have same size */
  // data_type * container = (data_type *)malloc((1+(d0)) * sizeof (data_type));
  data_type *container;
  posix_memalign((void **)(&container), AOCL_ALIGNMENT, d0 * sizeof(data_type));

  assert(container);

  if (container != 0) {
    // int * buffer = container+1;
    int *buffer = container;
    size_t size = sizeof(data_type) * d0;
    if (strlen(filename) == 0) {
    } else if (strcmp(filename, "0") == 0) {
      memset(buffer, 0, size);
    } else if (strcmp(filename, "+") == 0) {
      size_t i;
      for (i = 0; i < size / 4; i++)
        ((int *)buffer)[i] = i;
    } else if (strcmp(filename, "&") == 0) {
      size_t i;
      for (i = 0; i < size / 4; i++)
        ((int *)buffer)[i] = rand();
    } else if (strlen(filename) != 0) {
      char fullname[1024];
      if (filename[0] != '/') {
        strcpy(fullname, data_file_prefix);
        strcat(fullname, filename);
      } else
        strcpy(fullname, filename);
      fp = fopen(fullname, "rb");
      if (!fp)
        printf("ERROR: can not open %s for read.\n", filename);
      assert(fp != 0);
      /* fill the container with file data */
      int cnt = fread(container, sizeof(data_type), d0, fp);
      // int cnt = fread(container+1, sizeof(data_type), d0, fp);
      if (cnt != d0) {
        printf("WARNING: file size is smaller than required. (%s)\n", filename);
      }
      fclose(fp);
    }
  }

  data_type **a_tmp = (data_type **)a;
  (*a_tmp) = container;
  //	(*a_tmp) = 1 + container;
  //	container[0] = d0;
  map_pointer[count] = *a;
  map_size[count] = d0;
  // printf("point = %d  ",map_pointer[count]);
  // printf("size  = %d\n",map_size[count]);
  count++;

  return (((*a) != 0) && (container != 0));
}

// int cmost_malloc_2d(void *** a, char * filename, int unit_size, size_t d0,
// size_t d1 )
//{
//	int i, j;
//	FILE* fp;
//
//	/* allocate 3 more words to store dimensions d0, d1, d2, at the start of the
// container */ 	assert(sizeof(data_type) == sizeof(int));	/* currently
// only support when data and int have same size */ 	assert(unit_size ==
// sizeof(int));
///* currently only support when data and int have same size */ 	data_type *
// container = (data_type *)malloc((2+(d0*d1) )* sizeof (data_type));
//
//  assert(container);
//
//	if (container != 0) {
//		if (strcmp(filename, "0") == 0)
//		{
//			memset(container+2, 0, sizeof(data_type)*d0*d1);
//		}
//		else if (strlen(filename) != 0)
//		{
//			fp = fopen(filename, "r");
//			if (!fp) printf("ERROR: can not open %s for read.\n", filename);
//			assert (fp != 0);
//			/* fill the container with file data */
//			int cnt = fread(container+2, sizeof(data_type), d0*d1, fp);
//			if (cnt != d0*d1) {
//				printf("WARNING: file size is smaller than required. (%s)\n",
// filename);
//			}
//			fclose(fp);
//		}
//	}
//
//    data_type *** a_tmp = (data_type***) a;
//	(*a_tmp) = (data_type **)malloc(d0 * sizeof (data_type*));
//	for (i = 0; i < d0; i++)
//	{
//		(*a_tmp)[i] = 2 + container + i*d1;
//	}
//
//	container[0] = d0;
//	container[1] = d1;
//
//
//	return (((*a) != 0) && (container != 0));
//}
//
// int cmost_malloc_3d(void **** a, char * filename, int unit_size, size_t d0,
// size_t d1, size_t d2 )
//{
//	int i, j;
//	FILE* fp;
//
//	/* allocate 3 more words to store dimensions d0, d1, d2, at the start of the
// container */ 	assert(sizeof(data_type) == sizeof(int));	/* currently
// only support when data and int have same size */ 	assert(unit_size ==
// sizeof(int));
///* currently only support when data and int have same size */ 	data_type *
// container = (data_type *)malloc((3 + (d0*d1*d2)) * sizeof (data_type));
//
//  assert(container);
//
//	if (container != 0) {
//		if (strcmp(filename, "0") == 0)
//		{
//			memset(container+3, 0, sizeof(data_type)*d0*d1*d2);
//		}
//		else if (strlen(filename) != 0)
//		{
//			fp = fopen(filename, "r");
//			if (!fp) printf("ERROR: can not open %s for read.\n", filename);
//			assert (fp != 0);
//			/* fill the container with file data */
//			int cnt  = fread(container+3, sizeof(data_type), d0*d1*d2, fp);
//			if (cnt != d0*d1*d2) {
//				printf("WARNING: file size is smaller than required. (%s)\n",
// filename);
//			}
//			fclose(fp);
//		}
//	}
//
//    data_type **** a_tmp = (data_type****) a;
//	(*a_tmp) = (data_type ***)malloc(d0 * sizeof (data_type**));
//	for (i = 0; i < d0; i++)
//	{
//		(*a_tmp)[i] = (data_type **)malloc(d1 * sizeof (data_type *)) ;
//		for (j = 0; j < d1; j++)
//		{
//			(*a_tmp)[i][j] = 3 + container + i*d1*d2 + j*d2 ;
//		}
//	}
//
//	container[0] = d0;
//	container[1] = d1;
//	container[2] = d2;
//
//	return (((*a) != 0) && (container != 0));
//}

int cmost_free_1d(void *a) {
  data_type *base = (data_type *)a;

  /* free storage */
  free(base);
  // free(base-1);

  return 1;
}

// int cmost_free_2d(void ** a)
//{
//	int d0,i;
//    data_type ** a_tmp = (data_type **) a;
//	data_type * base = (data_type*) &(a_tmp[0][0]);
//
//	/* free level 1 pointer array */
//	free(a);
//
//	/* free storage */
//	free(base-2);
//
//	return 1;
//}
//
// int cmost_free_3d( void *** a)
//{
//	int d0,i;
//    data_type *** a_tmp = (data_type ***) a;
//	data_type * base = (data_type *) &(a_tmp[0][0][0]);
//
//
//	{
//		data_type * container = base - 3;
//		d0 = container[0];
//	}
//
//	/* free level 2 pointer arrays */
//	for (i=0; i<d0; i++)
//		free(a_tmp[d0]);
//
//	/* free level 1 pointer array */
//	free(a);
//
//	/* free storage */
//	free(base-3);
//
//	return 1;
//}

int cmost_dump_1d(void *a, const char *filename) {
  int d0 = 0;
  FILE *fp;
  int i;

  for (i = 0; i < 256; i++) {
    if (map_pointer[i] == a) {
      d0 = map_size[i];
      // printf("count = %d, d0 = %d\n",i,d0);
    }
  }

  char fullname[1024];
  if (filename[0] != '/') {
    strcpy(fullname, data_file_prefix);
    strcat(fullname, filename);
  } else
    strcpy(fullname, filename);
  fp = fopen(fullname, "w");
  assert(fp != 0);
  // fwrite(a[0][0], sizeof(data_type), g_test_info[index].length, fp);

  //	{
  //		data_type * base = (data_type *) a; //(data_type *)&(a[0]);
  //		data_type * container = base - 1;
  //		d0 = container[0];
  //	}

  {
    unsigned int addr = FPGA_DDR_USER_BASE;
    for (i = 0; i < d0; i++) {
      int *a_tmp = (int *)a;
      int *int_pointer = (int *)&(a_tmp[i]);
      fprintf(fp, "%08X:   %08X\n", addr, *int_pointer);
      addr += 4;
    }
  }

  fclose(fp);

  return 1;
}
// int cmost_dump_2d(void ** a, char * filename)
//{
//	int d0, d1, d2;
//	FILE* fp;
//	int i;
//
//    char fullname[1024];
//    if (filename[0] != '/') { strcpy(fullname, data_file_prefix);
//    strcat(fullname, filename);
//    }
//    else strcpy(fullname, filename);
//	fp = fopen(fullname, "w");
//	assert(fp != 0);
//	//fwrite(a[0][0], sizeof(data_type), g_test_info[index].length, fp);
//
//
//    data_type ** a_tmp = (data_type **)a;
//
//	{
//		data_type * base = (data_type *)&(a_tmp[0][0]);
//		data_type * container = base - 2;
//		d0 = container[0];
//		d1 = container[1];
//	}
//
//	{
//		unsigned int addr = FPGA_DDR_USER_BASE;
//		for (i = 0; i < d0*d1; i++)
//		{
//			int * int_pointer =(int*) &((a_tmp[0])[i]);
//			fprintf(fp, "%08X:   %08X\n", addr, *int_pointer);
//			addr += 4;
//		}
//	}
//
//	fclose(fp);
//
//	return 1;
//}
//
// int cmost_dump_3d(void *** a, char * filename)
//{
//	int d0, d1, d2;
//	FILE* fp;
//	int i;
//
//	fp = fopen(filename, "w");
//	assert(fp != 0);
//	//fwrite(a[0][0], sizeof(data_type), g_test_info[index].length, fp);
//    data_type *** a_tmp = (data_type ***)a;
//
//	{
//		data_type * base = (data_type*) &(a_tmp[0][0][0]);
//		data_type * container = base - 3;
//		d0 = container[0];
//		d1 = container[1];
//		d2 = container[2];
//	}
//
//	{
//		unsigned int addr = FPGA_DDR_USER_BASE;
//		for (i = 0; i < d0*d1*d2; i++)
//		{
//			int * int_pointer =(int*) &((a_tmp[0][0])[i]);
//			fprintf(fp, "%08X:   %08X\n", addr, *int_pointer);
//			addr += 4;
//		}
//	}
//
//	fclose(fp);
//
//	return 1;
//}

void cmost_break_point() { return; }
