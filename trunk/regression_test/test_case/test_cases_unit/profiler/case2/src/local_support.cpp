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
#include "backprop.h"
#include <string.h>
#if TIMER
#include "__merlin_timer.h"
#else
#include <sys/time.h>
#endif 

int INPUT_SIZE = sizeof(struct bench_args_t);

#ifdef MCC_ACC
#include MCC_ACC_H_FILE
#else
void workload(float delta[17], float ly[65537], float w[65537 * 17], float oldw[65537 * 17]);
#endif


void run_benchmark( void *vargs ) {
    struct bench_args_t *args = (struct bench_args_t *)vargs;

    #if TIMER
    start_timer();
    #else
    //------------------------------------------------------
    printf("INFO: Starting processing ...\n");
    struct timeval tv_start, tv_end;
    double kernel_time;
    gettimeofday (&tv_start, NULL);
    #endif 
              
    float *delta = (float *) malloc (sizeof(args->delta)); 
    float *ly = (float *) malloc (sizeof(args->ly)); 
    float *w = (float *) malloc (sizeof(args->w)); 
    float *oldw = (float *) malloc (sizeof(args->oldw)); 

    memcpy(delta, args->delta, sizeof(args->delta)); 
    memcpy(ly, args->ly, sizeof(args->ly)); 
    memcpy(w, args->w, sizeof(args->w)); 
    memcpy(oldw, args->oldw, sizeof(args->oldw)); 

    #ifdef MCC_ACC
    __merlin_workload( delta,ly,w,oldw);
    #else  
    workload(delta,ly,w,oldw);
    #endif
                                  
    memcpy(args->ly, ly, sizeof(args->ly)); 
    memcpy(args->w, w, sizeof(args->w)); 
    memcpy(args->oldw, oldw, sizeof(args->oldw)); 

    #if TIMER
    printf("Kernel time: %lf\n", read_timer());
    #else
    //stop the timer
    gettimeofday (&tv_end, NULL);
    printf("INFO: Kernel execution completed.\n");
    kernel_time = (tv_end.tv_sec - tv_start.tv_sec) * 1000.0 +
      (tv_end.tv_usec - tv_start.tv_usec)/1000.0;

    printf("INFO: Kernel execution time=%f ms\n", kernel_time);
    //------------------------------------------------------
    #endif 
}

void input_to_data(int fd, void *vdata) {
    struct bench_args_t *data = (struct bench_args_t *)vdata;
    char *p, *s;
    // Zero-out everything.
    memset(vdata,0,sizeof(struct bench_args_t));
    // Load input string
    p = readfile(fd);
    s = find_section_start(p,1);
    STAC(parse_,float,_array)(s, data->delta, 17);
    s = find_section_start(p,2);
    STAC(parse_,float,_array)(s, data->ly, 65537);
    s = find_section_start(p,3);
    STAC(parse_,float,_array)(s, data->oldw, 65537 * 17);
    //printf("+++++++++++++++++++++++++++++++++++input_to_data: ");
    //printf("%d\n",data->J[0]);
}

void data_to_input(int fd, void *vdata) {
    struct bench_args_t *data = (struct bench_args_t *)vdata;

    write_section_header(fd);
    STAC(write_, float, _array)(fd, data->delta, 17);
    write_section_header(fd);
    STAC(write_, float, _array)(fd, data->ly, 65537);
    write_section_header(fd);
    STAC(write_, float, _array)(fd, data->oldw, 65537 * 17);
}

void output_to_data(int fd, void *vdata) {
    struct bench_args_t *data = (struct bench_args_t *)vdata;
    char *p, *s;
    // Zero-out everything.
    memset(vdata,0,sizeof(struct bench_args_t));
    // Load input string
    p = readfile(fd);

    s = find_section_start(p,1);
    STAC(parse_,float,_array)(s, data->oldw, 65537 * 17);
    s = find_section_start(p,2);
    STAC(parse_,float,_array)(s, data->w, 65537 * 17);
}

void data_to_output(int fd, void *vdata) {
    struct bench_args_t *data = (struct bench_args_t *)vdata;

    //printf("+++++++++++++++++++++++++++++++++++data_to_output: ");
    //printf("%d\n",data->Jout[0]);

    write_section_header(fd);
    STAC(write_,float,_array)(fd, data->oldw, 65537 * 17);
    write_section_header(fd);
    STAC(write_,float,_array)(fd, data->w, 65537 * 17);
}

int check_data( void *vdata, void *vref ) {
    //printf("starting check data\n");
    struct bench_args_t *data = (struct bench_args_t *)vdata;
    //printf("converting data\n");
    struct bench_args_t *ref = (struct bench_args_t *)vref;
    //printf("converting ref\n");
    int has_errors = 0;

    has_errors |= memcmp(data->oldw, ref->oldw, 65537 * 17);
    has_errors |= memcmp(data->w, ref->w, 65537 * 17);
    //printf("finished comparing\n");
              
    // Return true if it's correct.
    return !has_errors;
}
