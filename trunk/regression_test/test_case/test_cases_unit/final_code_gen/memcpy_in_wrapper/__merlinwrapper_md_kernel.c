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
#include <string.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include "merlin_type_define.h"
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_440();
void md_kernel(int *n_points,double *force_x,double *force_y,double *force_z,double *position_x,double *position_y,double *position_z);
static void __merlin_dummy_extern_int_merlin_include_L_();
#include "__merlinhead_kernel_top.h"
static void __merlin_dummy_extern_int_merlin_include_L_();

void __merlinwrapper_md_kernel(int n_points[4][4][4],dvector_t force[4][4][4][10],dvector_t position[4][4][4][10])
{
{
    
#pragma ACCEL wrapper variable=n_points port=n_points depth=4,4,4 max_depth=4,4,4 data_type=int io=RW continue=1 copy=true
    
#pragma ACCEL wrapper variable=force->x port=force_x depth=4,4,4,10 max_depth=4,4,4,10 data_type=double io=RW continue=1 copy=true
    
#pragma ACCEL wrapper variable=force->y port=force_y depth=4,4,4,10 max_depth=4,4,4,10 data_type=double io=RW continue=1 copy=true
    
#pragma ACCEL wrapper variable=force->z port=force_z depth=4,4,4,10 max_depth=4,4,4,10 data_type=double io=RW continue=1 copy=true
    
#pragma ACCEL wrapper variable=position->x port=position_x depth=4,4,4,10 max_depth=4,4,4,10 data_type=double io=RW continue=1 copy=true
    
#pragma ACCEL wrapper variable=position->y port=position_y depth=4,4,4,10 max_depth=4,4,4,10 data_type=double io=RW continue=1 copy=true
    
#pragma ACCEL wrapper variable=position->z port=position_z depth=4,4,4,10 max_depth=4,4,4,10 data_type=double io=RW continue=1 copy=true
  }
  static int __m_n_points[64];
  static double __m_force_x[640];
  static double __m_force_y[640];
  static double __m_force_z[640];
  static double __m_position_x[640];
  static double __m_position_y[640];
  static double __m_position_z[640];
{
    int _i0;
    int _i1;
    int _i2;
    int _i3;
/*
    static int __m_n_points[64];
    static double __m_force_x[640];
    static double __m_force_y[640];
    static double __m_force_z[640];
    static double __m_position_x[640];
    static double __m_position_y[640];
    static double __m_position_z[640];
*/
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "n_points", "4 * 4 * 4 * sizeof(int )");
      
#pragma ACCEL debug fflush(stdout);
      if (n_points != 0) {
        memcpy(((void *)(__m_n_points + (0 * 16 + 0 * 4))),((const void *)n_points[0][0]),(4 * 4 * 4) * sizeof(int ));
      }
       else {
        printf("Error! The external memory pointed by 'n_points[0][0]' is invalid.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "n_points");
      
#pragma ACCEL debug fflush(stdout);
    }
    static double *__ti_force_x;
    posix_memalign(((void **)(&__ti_force_x)),64,sizeof(double ) * 640);
    for (_i0 = 0; _i0 < 4; ++_i0) {
      for (_i1 = 0; _i1 < 4; ++_i1) {
        for (_i2 = 0; _i2 < 4; ++_i2) {
          for (_i3 = 0; _i3 < 10; ++_i3) {
            if (force != 0) {
              __ti_force_x[_i0 * 160 + _i1 * 40 + _i2 * 10 + _i3 * 1] = force[_i0][_i1][_i2][_i3] . x;
            }
             else {
              printf("Error! The external memory pointed by 'force[_i0][_i1][_i2][_i3] . x' is invalid.\n");
              exit(1);
            }
          }
        }
      }
    }
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "force_x", "4 * 4 * 4 * 10 * 1 * sizeof(double )");
    
#pragma ACCEL debug fflush(stdout);
    memcpy(((void *)__m_force_x),((const void *)__ti_force_x),(4 * 4 * 4 * 10 * 1) * sizeof(double ));
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "force_x");
    
#pragma ACCEL debug fflush(stdout);
    static double *__ti_force_y;
    posix_memalign(((void **)(&__ti_force_y)),64,sizeof(double ) * 640);
    for (_i0 = 0; _i0 < 4; ++_i0) {
      for (_i1 = 0; _i1 < 4; ++_i1) {
        for (_i2 = 0; _i2 < 4; ++_i2) {
          for (_i3 = 0; _i3 < 10; ++_i3) {
            if (force != 0) {
              __ti_force_y[_i0 * 160 + _i1 * 40 + _i2 * 10 + _i3 * 1] = force[_i0][_i1][_i2][_i3] . y;
            }
             else {
              printf("Error! The external memory pointed by 'force[_i0][_i1][_i2][_i3] . y' is invalid.\n");
              exit(1);
            }
          }
        }
      }
    }
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "force_y", "4 * 4 * 4 * 10 * 1 * sizeof(double )");
    
#pragma ACCEL debug fflush(stdout);
    memcpy(((void *)__m_force_y),((const void *)__ti_force_y),(4 * 4 * 4 * 10 * 1) * sizeof(double ));
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "force_y");
    
#pragma ACCEL debug fflush(stdout);
    static double *__ti_force_z;
    posix_memalign(((void **)(&__ti_force_z)),64,sizeof(double ) * 640);
    for (_i0 = 0; _i0 < 4; ++_i0) {
      for (_i1 = 0; _i1 < 4; ++_i1) {
        for (_i2 = 0; _i2 < 4; ++_i2) {
          for (_i3 = 0; _i3 < 10; ++_i3) {
            if (force != 0) {
              __ti_force_z[_i0 * 160 + _i1 * 40 + _i2 * 10 + _i3 * 1] = force[_i0][_i1][_i2][_i3] . z;
            }
             else {
              printf("Error! The external memory pointed by 'force[_i0][_i1][_i2][_i3] . z' is invalid.\n");
              exit(1);
            }
          }
        }
      }
    }
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "force_z", "4 * 4 * 4 * 10 * 1 * sizeof(double )");
    
#pragma ACCEL debug fflush(stdout);
    memcpy(((void *)__m_force_z),((const void *)__ti_force_z),(4 * 4 * 4 * 10 * 1) * sizeof(double ));
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "force_z");
    
#pragma ACCEL debug fflush(stdout);
    static double *__ti_position_x;
    posix_memalign(((void **)(&__ti_position_x)),64,sizeof(double ) * 640);
    for (_i0 = 0; _i0 < 4; ++_i0) {
      for (_i1 = 0; _i1 < 4; ++_i1) {
        for (_i2 = 0; _i2 < 4; ++_i2) {
          for (_i3 = 0; _i3 < 10; ++_i3) {
            if (position != 0) {
              __ti_position_x[_i0 * 160 + _i1 * 40 + _i2 * 10 + _i3 * 1] = position[_i0][_i1][_i2][_i3] . x;
            }
             else {
              printf("Error! The external memory pointed by 'position[_i0][_i1][_i2][_i3] . x' is invalid.\n");
              exit(1);
            }
          }
        }
      }
    }
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "position_x", "4 * 4 * 4 * 10 * 1 * sizeof(double )");
    
#pragma ACCEL debug fflush(stdout);
    memcpy(((void *)__m_position_x),((const void *)__ti_position_x),(4 * 4 * 4 * 10 * 1) * sizeof(double ));
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "position_x");
    
#pragma ACCEL debug fflush(stdout);
    static double *__ti_position_y;
    posix_memalign(((void **)(&__ti_position_y)),64,sizeof(double ) * 640);
    for (_i0 = 0; _i0 < 4; ++_i0) {
      for (_i1 = 0; _i1 < 4; ++_i1) {
        for (_i2 = 0; _i2 < 4; ++_i2) {
          for (_i3 = 0; _i3 < 10; ++_i3) {
            if (position != 0) {
              __ti_position_y[_i0 * 160 + _i1 * 40 + _i2 * 10 + _i3 * 1] = position[_i0][_i1][_i2][_i3] . y;
            }
             else {
              printf("Error! The external memory pointed by 'position[_i0][_i1][_i2][_i3] . y' is invalid.\n");
              exit(1);
            }
          }
        }
      }
    }
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "position_y", "4 * 4 * 4 * 10 * 1 * sizeof(double )");
    
#pragma ACCEL debug fflush(stdout);
    memcpy(((void *)__m_position_y),((const void *)__ti_position_y),(4 * 4 * 4 * 10 * 1) * sizeof(double ));
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "position_y");
    
#pragma ACCEL debug fflush(stdout);
    static double *__ti_position_z;
    posix_memalign(((void **)(&__ti_position_z)),64,sizeof(double ) * 640);
    for (_i0 = 0; _i0 < 4; ++_i0) {
      for (_i1 = 0; _i1 < 4; ++_i1) {
        for (_i2 = 0; _i2 < 4; ++_i2) {
          for (_i3 = 0; _i3 < 10; ++_i3) {
            if (position != 0) {
              __ti_position_z[_i0 * 160 + _i1 * 40 + _i2 * 10 + _i3 * 1] = position[_i0][_i1][_i2][_i3] . z;
            }
             else {
              printf("Error! The external memory pointed by 'position[_i0][_i1][_i2][_i3] . z' is invalid.\n");
              exit(1);
            }
          }
        }
      }
    }
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "position_z", "4 * 4 * 4 * 10 * 1 * sizeof(double )");
    
#pragma ACCEL debug fflush(stdout);
    memcpy(((void *)__m_position_z),((const void *)__ti_position_z),(4 * 4 * 4 * 10 * 1) * sizeof(double ));
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "position_z");
    
#pragma ACCEL debug fflush(stdout);
  }
  
#pragma ACCEL kernel
  md_kernel(__m_n_points,__m_force_x,__m_force_y,__m_force_z,__m_position_x,__m_position_y,__m_position_z);
{
    int _i0;
    int _i1;
    int _i2;
    int _i3;
/*
    static int __m_n_points[64];
    static double __m_force_x[640];
    static double __m_force_y[640];
    static double __m_force_z[640];
    static double __m_position_x[640];
    static double __m_position_y[640];
    static double __m_position_z[640];
*/
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "n_points", "4 * 4 * 4 * sizeof(int )");
      
#pragma ACCEL debug fflush(stdout);
      if (n_points != 0) {
        memcpy(((void *)n_points[0][0]),((const void *)(__m_n_points + (0 * 16 + 0 * 4))),(4 * 4 * 4) * sizeof(int ));
      }
       else {
        printf("Error! The external memory pointed by 'n_points[0][0]' is invalid.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "n_points");
      
#pragma ACCEL debug fflush(stdout);
    }
    static double *__to_force_x;
    posix_memalign(((void **)(&__to_force_x)),64,sizeof(double ) * 640);
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "force_x", "4 * 4 * 4 * 10 * 1 * sizeof(double )");
    
#pragma ACCEL debug fflush(stdout);
    memcpy(((void *)__to_force_x),((const void *)__m_force_x),(4 * 4 * 4 * 10 * 1) * sizeof(double ));
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "force_x");
    
#pragma ACCEL debug fflush(stdout);
    for (_i0 = 0; _i0 < 4; ++_i0) {
      for (_i1 = 0; _i1 < 4; ++_i1) {
        for (_i2 = 0; _i2 < 4; ++_i2) {
          for (_i3 = 0; _i3 < 10; ++_i3) {
            if (force != 0) {
              force[_i0][_i1][_i2][_i3] . x = __to_force_x[_i0 * 160 + _i1 * 40 + _i2 * 10 + _i3 * 1];
            }
             else {
              printf("Error! The external memory pointed by 'force[_i0][_i1][_i2][_i3] . x' is invalid.\n");
              exit(1);
            }
          }
        }
      }
    }
    static double *__to_force_y;
    posix_memalign(((void **)(&__to_force_y)),64,sizeof(double ) * 640);
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "force_y", "4 * 4 * 4 * 10 * 1 * sizeof(double )");
    
#pragma ACCEL debug fflush(stdout);
    memcpy(((void *)__to_force_y),((const void *)__m_force_y),(4 * 4 * 4 * 10 * 1) * sizeof(double ));
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "force_y");
    
#pragma ACCEL debug fflush(stdout);
    for (_i0 = 0; _i0 < 4; ++_i0) {
      for (_i1 = 0; _i1 < 4; ++_i1) {
        for (_i2 = 0; _i2 < 4; ++_i2) {
          for (_i3 = 0; _i3 < 10; ++_i3) {
            if (force != 0) {
              force[_i0][_i1][_i2][_i3] . y = __to_force_y[_i0 * 160 + _i1 * 40 + _i2 * 10 + _i3 * 1];
            }
             else {
              printf("Error! The external memory pointed by 'force[_i0][_i1][_i2][_i3] . y' is invalid.\n");
              exit(1);
            }
          }
        }
      }
    }
    static double *__to_force_z;
    posix_memalign(((void **)(&__to_force_z)),64,sizeof(double ) * 640);
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "force_z", "4 * 4 * 4 * 10 * 1 * sizeof(double )");
    
#pragma ACCEL debug fflush(stdout);
    memcpy(((void *)__to_force_z),((const void *)__m_force_z),(4 * 4 * 4 * 10 * 1) * sizeof(double ));
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "force_z");
    
#pragma ACCEL debug fflush(stdout);
    for (_i0 = 0; _i0 < 4; ++_i0) {
      for (_i1 = 0; _i1 < 4; ++_i1) {
        for (_i2 = 0; _i2 < 4; ++_i2) {
          for (_i3 = 0; _i3 < 10; ++_i3) {
            if (force != 0) {
              force[_i0][_i1][_i2][_i3] . z = __to_force_z[_i0 * 160 + _i1 * 40 + _i2 * 10 + _i3 * 1];
            }
             else {
              printf("Error! The external memory pointed by 'force[_i0][_i1][_i2][_i3] . z' is invalid.\n");
              exit(1);
            }
          }
        }
      }
    }
    static double *__to_position_x;
    posix_memalign(((void **)(&__to_position_x)),64,sizeof(double ) * 640);
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "position_x", "4 * 4 * 4 * 10 * 1 * sizeof(double )");
    
#pragma ACCEL debug fflush(stdout);
    memcpy(((void *)__to_position_x),((const void *)__m_position_x),(4 * 4 * 4 * 10 * 1) * sizeof(double ));
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "position_x");
    
#pragma ACCEL debug fflush(stdout);
    for (_i0 = 0; _i0 < 4; ++_i0) {
      for (_i1 = 0; _i1 < 4; ++_i1) {
        for (_i2 = 0; _i2 < 4; ++_i2) {
          for (_i3 = 0; _i3 < 10; ++_i3) {
            if (position != 0) {
              position[_i0][_i1][_i2][_i3] . x = __to_position_x[_i0 * 160 + _i1 * 40 + _i2 * 10 + _i3 * 1];
            }
             else {
              printf("Error! The external memory pointed by 'position[_i0][_i1][_i2][_i3] . x' is invalid.\n");
              exit(1);
            }
          }
        }
      }
    }
    static double *__to_position_y;
    posix_memalign(((void **)(&__to_position_y)),64,sizeof(double ) * 640);
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "position_y", "4 * 4 * 4 * 10 * 1 * sizeof(double )");
    
#pragma ACCEL debug fflush(stdout);
    memcpy(((void *)__to_position_y),((const void *)__m_position_y),(4 * 4 * 4 * 10 * 1) * sizeof(double ));
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "position_y");
    
#pragma ACCEL debug fflush(stdout);
    for (_i0 = 0; _i0 < 4; ++_i0) {
      for (_i1 = 0; _i1 < 4; ++_i1) {
        for (_i2 = 0; _i2 < 4; ++_i2) {
          for (_i3 = 0; _i3 < 10; ++_i3) {
            if (position != 0) {
              position[_i0][_i1][_i2][_i3] . y = __to_position_y[_i0 * 160 + _i1 * 40 + _i2 * 10 + _i3 * 1];
            }
             else {
              printf("Error! The external memory pointed by 'position[_i0][_i1][_i2][_i3] . y' is invalid.\n");
              exit(1);
            }
          }
        }
      }
    }
    static double *__to_position_z;
    posix_memalign(((void **)(&__to_position_z)),64,sizeof(double ) * 640);
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "position_z", "4 * 4 * 4 * 10 * 1 * sizeof(double )");
    
#pragma ACCEL debug fflush(stdout);
    memcpy(((void *)__to_position_z),((const void *)__m_position_z),(4 * 4 * 4 * 10 * 1) * sizeof(double ));
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "position_z");
    
#pragma ACCEL debug fflush(stdout);
    for (_i0 = 0; _i0 < 4; ++_i0) {
      for (_i1 = 0; _i1 < 4; ++_i1) {
        for (_i2 = 0; _i2 < 4; ++_i2) {
          for (_i3 = 0; _i3 < 10; ++_i3) {
            if (position != 0) {
              position[_i0][_i1][_i2][_i3] . z = __to_position_z[_i0 * 160 + _i1 * 40 + _i2 * 10 + _i3 * 1];
            }
             else {
              printf("Error! The external memory pointed by 'position[_i0][_i1][_i2][_i3] . z' is invalid.\n");
              exit(1);
            }
          }
        }
      }
    }
  }
}

void __merlin_md_kernel(int n_points[4][4][4],dvector_t force[4][4][4][10],dvector_t position[4][4][4][10])
{
  
#pragma ACCEL string __merlin_check_opencl();
  
#pragma ACCEL string __merlin_init_md_kernel();
  __merlinwrapper_md_kernel(n_points,force,position);
  
#pragma ACCEL string __merlin_release_md_kernel();
}

void __merlin_write_buffer_md_kernel(int n_points[4][4][4],dvector_t force[4][4][4][10],dvector_t position[4][4][4][10])
{
  int _i0;
  int _i1;
  int _i2;
  int _i3;
  static int __m_n_points[64];
  static double __m_force_x[640];
  static double __m_force_y[640];
  static double __m_force_z[640];
  static double __m_position_x[640];
  static double __m_position_y[640];
  static double __m_position_z[640];
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "n_points", "4 * 4 * 4 * sizeof(int )");
    
#pragma ACCEL debug fflush(stdout);
    if (n_points != 0) {
      memcpy(((void *)(__m_n_points + (0 * 16 + 0 * 4))),((const void *)n_points[0][0]),(4 * 4 * 4) * sizeof(int ));
    }
     else {
      printf("Error! The external memory pointed by 'n_points[0][0]' is invalid.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "n_points");
    
#pragma ACCEL debug fflush(stdout);
  }
  static double *__ti_force_x;
  posix_memalign(((void **)(&__ti_force_x)),64,sizeof(double ) * 640);
  for (_i0 = 0; _i0 < 4; ++_i0) {
    for (_i1 = 0; _i1 < 4; ++_i1) {
      for (_i2 = 0; _i2 < 4; ++_i2) {
        for (_i3 = 0; _i3 < 10; ++_i3) {
          if (force != 0) {
            __ti_force_x[_i0 * 160 + _i1 * 40 + _i2 * 10 + _i3 * 1] = force[_i0][_i1][_i2][_i3] . x;
          }
           else {
            printf("Error! The external memory pointed by 'force[_i0][_i1][_i2][_i3] . x' is invalid.\n");
            exit(1);
          }
        }
      }
    }
  }
  
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "force_x", "4 * 4 * 4 * 10 * 1 * sizeof(double )");
  
#pragma ACCEL debug fflush(stdout);
  memcpy(((void *)__m_force_x),((const void *)__ti_force_x),(4 * 4 * 4 * 10 * 1) * sizeof(double ));
  
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "force_x");
  
#pragma ACCEL debug fflush(stdout);
  static double *__ti_force_y;
  posix_memalign(((void **)(&__ti_force_y)),64,sizeof(double ) * 640);
  for (_i0 = 0; _i0 < 4; ++_i0) {
    for (_i1 = 0; _i1 < 4; ++_i1) {
      for (_i2 = 0; _i2 < 4; ++_i2) {
        for (_i3 = 0; _i3 < 10; ++_i3) {
          if (force != 0) {
            __ti_force_y[_i0 * 160 + _i1 * 40 + _i2 * 10 + _i3 * 1] = force[_i0][_i1][_i2][_i3] . y;
          }
           else {
            printf("Error! The external memory pointed by 'force[_i0][_i1][_i2][_i3] . y' is invalid.\n");
            exit(1);
          }
        }
      }
    }
  }
  
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "force_y", "4 * 4 * 4 * 10 * 1 * sizeof(double )");
  
#pragma ACCEL debug fflush(stdout);
  memcpy(((void *)__m_force_y),((const void *)__ti_force_y),(4 * 4 * 4 * 10 * 1) * sizeof(double ));
  
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "force_y");
  
#pragma ACCEL debug fflush(stdout);
  static double *__ti_force_z;
  posix_memalign(((void **)(&__ti_force_z)),64,sizeof(double ) * 640);
  for (_i0 = 0; _i0 < 4; ++_i0) {
    for (_i1 = 0; _i1 < 4; ++_i1) {
      for (_i2 = 0; _i2 < 4; ++_i2) {
        for (_i3 = 0; _i3 < 10; ++_i3) {
          if (force != 0) {
            __ti_force_z[_i0 * 160 + _i1 * 40 + _i2 * 10 + _i3 * 1] = force[_i0][_i1][_i2][_i3] . z;
          }
           else {
            printf("Error! The external memory pointed by 'force[_i0][_i1][_i2][_i3] . z' is invalid.\n");
            exit(1);
          }
        }
      }
    }
  }
  
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "force_z", "4 * 4 * 4 * 10 * 1 * sizeof(double )");
  
#pragma ACCEL debug fflush(stdout);
  memcpy(((void *)__m_force_z),((const void *)__ti_force_z),(4 * 4 * 4 * 10 * 1) * sizeof(double ));
  
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "force_z");
  
#pragma ACCEL debug fflush(stdout);
  static double *__ti_position_x;
  posix_memalign(((void **)(&__ti_position_x)),64,sizeof(double ) * 640);
  for (_i0 = 0; _i0 < 4; ++_i0) {
    for (_i1 = 0; _i1 < 4; ++_i1) {
      for (_i2 = 0; _i2 < 4; ++_i2) {
        for (_i3 = 0; _i3 < 10; ++_i3) {
          if (position != 0) {
            __ti_position_x[_i0 * 160 + _i1 * 40 + _i2 * 10 + _i3 * 1] = position[_i0][_i1][_i2][_i3] . x;
          }
           else {
            printf("Error! The external memory pointed by 'position[_i0][_i1][_i2][_i3] . x' is invalid.\n");
            exit(1);
          }
        }
      }
    }
  }
  
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "position_x", "4 * 4 * 4 * 10 * 1 * sizeof(double )");
  
#pragma ACCEL debug fflush(stdout);
  memcpy(((void *)__m_position_x),((const void *)__ti_position_x),(4 * 4 * 4 * 10 * 1) * sizeof(double ));
  
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "position_x");
  
#pragma ACCEL debug fflush(stdout);
  static double *__ti_position_y;
  posix_memalign(((void **)(&__ti_position_y)),64,sizeof(double ) * 640);
  for (_i0 = 0; _i0 < 4; ++_i0) {
    for (_i1 = 0; _i1 < 4; ++_i1) {
      for (_i2 = 0; _i2 < 4; ++_i2) {
        for (_i3 = 0; _i3 < 10; ++_i3) {
          if (position != 0) {
            __ti_position_y[_i0 * 160 + _i1 * 40 + _i2 * 10 + _i3 * 1] = position[_i0][_i1][_i2][_i3] . y;
          }
           else {
            printf("Error! The external memory pointed by 'position[_i0][_i1][_i2][_i3] . y' is invalid.\n");
            exit(1);
          }
        }
      }
    }
  }
  
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "position_y", "4 * 4 * 4 * 10 * 1 * sizeof(double )");
  
#pragma ACCEL debug fflush(stdout);
  memcpy(((void *)__m_position_y),((const void *)__ti_position_y),(4 * 4 * 4 * 10 * 1) * sizeof(double ));
  
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "position_y");
  
#pragma ACCEL debug fflush(stdout);
  static double *__ti_position_z;
  posix_memalign(((void **)(&__ti_position_z)),64,sizeof(double ) * 640);
  for (_i0 = 0; _i0 < 4; ++_i0) {
    for (_i1 = 0; _i1 < 4; ++_i1) {
      for (_i2 = 0; _i2 < 4; ++_i2) {
        for (_i3 = 0; _i3 < 10; ++_i3) {
          if (position != 0) {
            __ti_position_z[_i0 * 160 + _i1 * 40 + _i2 * 10 + _i3 * 1] = position[_i0][_i1][_i2][_i3] . z;
          }
           else {
            printf("Error! The external memory pointed by 'position[_i0][_i1][_i2][_i3] . z' is invalid.\n");
            exit(1);
          }
        }
      }
    }
  }
  
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "position_z", "4 * 4 * 4 * 10 * 1 * sizeof(double )");
  
#pragma ACCEL debug fflush(stdout);
  memcpy(((void *)__m_position_z),((const void *)__ti_position_z),(4 * 4 * 4 * 10 * 1) * sizeof(double ));
  
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "position_z");
  
#pragma ACCEL debug fflush(stdout);
}

void __merlin_read_buffer_md_kernel(int n_points[4][4][4],dvector_t force[4][4][4][10],dvector_t position[4][4][4][10])
{
  int _i0;
  int _i1;
  int _i2;
  int _i3;
  static int __m_n_points[64];
  static double __m_force_x[640];
  static double __m_force_y[640];
  static double __m_force_z[640];
  static double __m_position_x[640];
  static double __m_position_y[640];
  static double __m_position_z[640];
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "n_points", "4 * 4 * 4 * sizeof(int )");
    
#pragma ACCEL debug fflush(stdout);
    if (n_points != 0) {
      memcpy(((void *)n_points[0][0]),((const void *)(__m_n_points + (0 * 16 + 0 * 4))),(4 * 4 * 4) * sizeof(int ));
    }
     else {
      printf("Error! The external memory pointed by 'n_points[0][0]' is invalid.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "n_points");
    
#pragma ACCEL debug fflush(stdout);
  }
  static double *__to_force_x;
  posix_memalign(((void **)(&__to_force_x)),64,sizeof(double ) * 640);
  
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "force_x", "4 * 4 * 4 * 10 * 1 * sizeof(double )");
  
#pragma ACCEL debug fflush(stdout);
  memcpy(((void *)__to_force_x),((const void *)__m_force_x),(4 * 4 * 4 * 10 * 1) * sizeof(double ));
  
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "force_x");
  
#pragma ACCEL debug fflush(stdout);
  for (_i0 = 0; _i0 < 4; ++_i0) {
    for (_i1 = 0; _i1 < 4; ++_i1) {
      for (_i2 = 0; _i2 < 4; ++_i2) {
        for (_i3 = 0; _i3 < 10; ++_i3) {
          if (force != 0) {
            force[_i0][_i1][_i2][_i3] . x = __to_force_x[_i0 * 160 + _i1 * 40 + _i2 * 10 + _i3 * 1];
          }
           else {
            printf("Error! The external memory pointed by 'force[_i0][_i1][_i2][_i3] . x' is invalid.\n");
            exit(1);
          }
        }
      }
    }
  }
  static double *__to_force_y;
  posix_memalign(((void **)(&__to_force_y)),64,sizeof(double ) * 640);
  
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "force_y", "4 * 4 * 4 * 10 * 1 * sizeof(double )");
  
#pragma ACCEL debug fflush(stdout);
  memcpy(((void *)__to_force_y),((const void *)__m_force_y),(4 * 4 * 4 * 10 * 1) * sizeof(double ));
  
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "force_y");
  
#pragma ACCEL debug fflush(stdout);
  for (_i0 = 0; _i0 < 4; ++_i0) {
    for (_i1 = 0; _i1 < 4; ++_i1) {
      for (_i2 = 0; _i2 < 4; ++_i2) {
        for (_i3 = 0; _i3 < 10; ++_i3) {
          if (force != 0) {
            force[_i0][_i1][_i2][_i3] . y = __to_force_y[_i0 * 160 + _i1 * 40 + _i2 * 10 + _i3 * 1];
          }
           else {
            printf("Error! The external memory pointed by 'force[_i0][_i1][_i2][_i3] . y' is invalid.\n");
            exit(1);
          }
        }
      }
    }
  }
  static double *__to_force_z;
  posix_memalign(((void **)(&__to_force_z)),64,sizeof(double ) * 640);
  
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "force_z", "4 * 4 * 4 * 10 * 1 * sizeof(double )");
  
#pragma ACCEL debug fflush(stdout);
  memcpy(((void *)__to_force_z),((const void *)__m_force_z),(4 * 4 * 4 * 10 * 1) * sizeof(double ));
  
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "force_z");
  
#pragma ACCEL debug fflush(stdout);
  for (_i0 = 0; _i0 < 4; ++_i0) {
    for (_i1 = 0; _i1 < 4; ++_i1) {
      for (_i2 = 0; _i2 < 4; ++_i2) {
        for (_i3 = 0; _i3 < 10; ++_i3) {
          if (force != 0) {
            force[_i0][_i1][_i2][_i3] . z = __to_force_z[_i0 * 160 + _i1 * 40 + _i2 * 10 + _i3 * 1];
          }
           else {
            printf("Error! The external memory pointed by 'force[_i0][_i1][_i2][_i3] . z' is invalid.\n");
            exit(1);
          }
        }
      }
    }
  }
  static double *__to_position_x;
  posix_memalign(((void **)(&__to_position_x)),64,sizeof(double ) * 640);
  
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "position_x", "4 * 4 * 4 * 10 * 1 * sizeof(double )");
  
#pragma ACCEL debug fflush(stdout);
  memcpy(((void *)__to_position_x),((const void *)__m_position_x),(4 * 4 * 4 * 10 * 1) * sizeof(double ));
  
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "position_x");
  
#pragma ACCEL debug fflush(stdout);
  for (_i0 = 0; _i0 < 4; ++_i0) {
    for (_i1 = 0; _i1 < 4; ++_i1) {
      for (_i2 = 0; _i2 < 4; ++_i2) {
        for (_i3 = 0; _i3 < 10; ++_i3) {
          if (position != 0) {
            position[_i0][_i1][_i2][_i3] . x = __to_position_x[_i0 * 160 + _i1 * 40 + _i2 * 10 + _i3 * 1];
          }
           else {
            printf("Error! The external memory pointed by 'position[_i0][_i1][_i2][_i3] . x' is invalid.\n");
            exit(1);
          }
        }
      }
    }
  }
  static double *__to_position_y;
  posix_memalign(((void **)(&__to_position_y)),64,sizeof(double ) * 640);
  
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "position_y", "4 * 4 * 4 * 10 * 1 * sizeof(double )");
  
#pragma ACCEL debug fflush(stdout);
  memcpy(((void *)__to_position_y),((const void *)__m_position_y),(4 * 4 * 4 * 10 * 1) * sizeof(double ));
  
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "position_y");
  
#pragma ACCEL debug fflush(stdout);
  for (_i0 = 0; _i0 < 4; ++_i0) {
    for (_i1 = 0; _i1 < 4; ++_i1) {
      for (_i2 = 0; _i2 < 4; ++_i2) {
        for (_i3 = 0; _i3 < 10; ++_i3) {
          if (position != 0) {
            position[_i0][_i1][_i2][_i3] . y = __to_position_y[_i0 * 160 + _i1 * 40 + _i2 * 10 + _i3 * 1];
          }
           else {
            printf("Error! The external memory pointed by 'position[_i0][_i1][_i2][_i3] . y' is invalid.\n");
            exit(1);
          }
        }
      }
    }
  }
  static double *__to_position_z;
  posix_memalign(((void **)(&__to_position_z)),64,sizeof(double ) * 640);
  
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "position_z", "4 * 4 * 4 * 10 * 1 * sizeof(double )");
  
#pragma ACCEL debug fflush(stdout);
  memcpy(((void *)__to_position_z),((const void *)__m_position_z),(4 * 4 * 4 * 10 * 1) * sizeof(double ));
  
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "position_z");
  
#pragma ACCEL debug fflush(stdout);
  for (_i0 = 0; _i0 < 4; ++_i0) {
    for (_i1 = 0; _i1 < 4; ++_i1) {
      for (_i2 = 0; _i2 < 4; ++_i2) {
        for (_i3 = 0; _i3 < 10; ++_i3) {
          if (position != 0) {
            position[_i0][_i1][_i2][_i3] . z = __to_position_z[_i0 * 160 + _i1 * 40 + _i2 * 10 + _i3 * 1];
          }
           else {
            printf("Error! The external memory pointed by 'position[_i0][_i1][_i2][_i3] . z' is invalid.\n");
            exit(1);
          }
        }
      }
    }
  }
}

void __merlin_execute_md_kernel(int n_points[4][4][4],dvector_t force[4][4][4][10],dvector_t position[4][4][4][10])
{
}
