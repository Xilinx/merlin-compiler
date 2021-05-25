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


#define __kernel
#define __global

#if __STDC_VERSION__ >= 199901L
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 500
#endif /* __STDC_VERSION__ */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
//#include <sys/time.h>

#include <time.h>
#include "__merlin_timer.h"
#define DEBUG_PCIE_PRINT_TIME 1

struct timespec t0, t1;

struct timespec w_start;
struct timespec w_end;
struct timespec timer_start[100];
struct timespec timer_end[100];
struct timespec t_start;
struct timespec t_end;
struct timespec t_betw;
float t_pcie = 0.0;
float t_comp = 0.0;
float t_total = 0.0;
int count = 0;
double timer_data[100];
double wrapper_time = 0.0;

void timer_begin(int index) {
  if (DEBUG_PCIE_PRINT_TIME) {
    clock_gettime(CLOCK_MONOTONIC, &timer_start[index]);
    //	    if(index == 0) {
    //            printf("Program execute begin\n");
    //	        char print_info[1024];
    //	        sprintf(print_info, "echo 'Program execute begin' >>
    // timer.rpt"); 	        system(print_info);
    //	    }
  }
}
void timer_comp_end(int index) {
  //    if(DEBUG_PCIE_PRINT_TIME)	{
  //        clock_gettime(CLOCK_MONOTONIC, &timer_end[index]);
  //        double delta = ((double) (timer_end[index].tv_sec -
  //        timer_start[index].tv_sec))+((double) (timer_end[index].tv_nsec -
  //        timer_start[index].tv_nsec))*1.0e-9;
  //
  //        float t_local;
  //        t_local = delta;
  //	timer_data[count] = delta;
  //	t_comp += t_local;
  //    }
  //    count++;
}

void timer_total(int index) {
  if (DEBUG_PCIE_PRINT_TIME) {
    clock_gettime(CLOCK_MONOTONIC, &timer_end[index]);
    double delta =
        ((double)(timer_end[index].tv_sec - timer_start[index].tv_sec)) +
        ((double)(timer_end[index].tv_nsec - timer_start[index].tv_nsec)) *
            1.0e-9;

    float t_local;
    t_local = delta;
    //        printf("Program execute current time: %lf second(s)\n", t_local);
    //	    {
    //	        char print_info[1024];
    //	        sprintf(print_info, "echo 'Program execute current time: %lf
    // second(s)' >> timer.rpt", t_local); 	        system(print_info);
    //	    }
    t_total = t_local;
  }
}

void start_timer() {
  if (DEBUG_PCIE_PRINT_TIME) {
    // gettimeofday(&t_start, 0);
    clock_gettime(CLOCK_MONOTONIC, &t_start);
  }
}

double read_timer() {
  if (DEBUG_PCIE_PRINT_TIME) {
    // gettimeofday(&t_end, 0);
    clock_gettime(CLOCK_MONOTONIC, &t_end);
    // timersub(&t_end, &t_start, &t_betw);
    double delta = ((double)(t_end.tv_sec - t_start.tv_sec)) +
                   ((double)(t_end.tv_nsec - t_start.tv_nsec)) * 1.0e-9;

    return delta;
  }
}

void acc_pcie_timer() {
  if (DEBUG_PCIE_PRINT_TIME) {
    // gettimeofday(&t_end, 0);
    clock_gettime(CLOCK_MONOTONIC, &t_end);
    // timersub(&t_end, &t_start, &t_betw);
    double delta = ((double)(t_end.tv_sec - t_start.tv_sec)) +
                   ((double)(t_end.tv_nsec - t_start.tv_nsec)) * 1.0e-9;

    t_pcie += delta;
  }
}

void acc_comp_timer() {
  if (DEBUG_PCIE_PRINT_TIME) {
    clock_gettime(CLOCK_MONOTONIC, &t_end);
    // timersub(&t_end, &t_start, &t_betw);
    double delta = ((double)(t_end.tv_sec - t_start.tv_sec)) +
                   ((double)(t_end.tv_nsec - t_start.tv_nsec)) * 1.0e-9;

    float t_local;
    // gettimeofday(&t_end, 0);
    // timersub(&t_end, &t_start, &t_betw);
    // t_local = t_betw.tv_sec + t_betw.tv_usec/1000000.0;
    t_local = delta;
    //        printf("Kernel time of the graph: %lf second(s)\n", t_local);
    //        	{
    //		char print_info[1024];
    //		sprintf(print_info, "echo 'Kernel time of the graph: %lf second(s)'
    //>> timer.rpt", t_local); 		system(print_info);
    //		}

    //		t_comp += t_local;
    t_comp = t_local;
  }
}

void report_timer(const char *kernel_name) {
  if (DEBUG_PCIE_PRINT_TIME) {
    //        printf("Total Kernel time: %lf second(s)\n", (double) t_comp);
    // int i;
    // for(i=0; i<count; i++) {
    //	printf("Kernel time of the graph%d: %lf second(s)\n",i, timer_data[i]);
    //    char print_info[1024];
    //    sprintf(print_info, "echo 'Kernel time of the graph%d: %lf second(s)'
    //    >> timer.rpt",i, timer_data[i]); system(print_info);
    //}
    if (NULL != kernel_name) {
      printf("%s kernel compute time: %lf second(s)\n", kernel_name,
             (double)t_comp);
      printf("%s PCIe transfer  time: %lf second(s)\n", kernel_name,
             (double)(t_total - t_comp));
    } else {
      printf("kernel compute time: %lf second(s)\n", (double)t_comp);
      printf("PCIe transfer  time: %lf second(s)\n",
             (double)(t_total - t_comp));
    }
    //        {
    //		    char print_info[1024];
    //            sprintf("echo '%s kernel compute time: %lf second(s)\n' >>
    //            timer.rpt", kernel_name, t_total-t_pcie);
    //		    system(print_info);
    //            sprintf("echo '%s PCIe transfer  time: %lf second(s)\n' >>
    //            timer.rpt", kernel_name, t_pcie);
    //		    system(print_info);
    //		}
    //		    sprintf(print_info, "echo 'Total Kernel   time : %lf second(s)'
    //>> timer.rpt", t_comp); 		    sprintf(print_info, "echo 'Total Kernel
    // time : %lf second(s)' >> timer.rpt", t_total-t_pcie);
    // sprintf(print_info, "echo 'Total Transfer time : %lf second(s)' >>
    // timer.rpt", t_pcie);
  }
}

void wrapper_start() { clock_gettime(CLOCK_MONOTONIC, &w_start); }
void wrapper_end() { clock_gettime(CLOCK_MONOTONIC, &w_end); }
void print_time() {
  double wrapper_time = ((double)(w_end.tv_sec - w_start.tv_sec)) +
                        ((double)(w_end.tv_nsec - w_start.tv_nsec)) * 1.0e-9;
  printf("Wrapper execute time: %lf second(s)\n", wrapper_time);
}
