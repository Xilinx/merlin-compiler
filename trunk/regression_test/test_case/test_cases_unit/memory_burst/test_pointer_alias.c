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
#include "cmost.h"
#include <math.h>
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_kernel_pragma();
/* Original: #pragma ACCEL kernel name="advance" */

void advance(int nbodies,double *bodies_x,double *bodies_y,double *bodies_z,double *bodies_vx,double *bodies_vy,double *bodies_vz,double *bodies_mass,double dt)
{
  __merlin_access_range(bodies_mass,0,4UL);
  __merlin_access_range(bodies_vz,0,4UL);
  __merlin_access_range(bodies_vy,0,4UL);
  __merlin_access_range(bodies_vx,0,4UL);
  __merlin_access_range(bodies_z,0,4UL);
  __merlin_access_range(bodies_y,0,4UL);
  __merlin_access_range(bodies_x,0,4UL);
  
#pragma ACCEL INTERFACE VARIABLE=bodies_mass max_depth=5 DEPTH=5
  
#pragma ACCEL INTERFACE VARIABLE=bodies_vz max_depth=5 DEPTH=5
  
#pragma ACCEL INTERFACE VARIABLE=bodies_vy max_depth=5 DEPTH=5
  
#pragma ACCEL INTERFACE VARIABLE=bodies_vx max_depth=5 DEPTH=5
  
#pragma ACCEL INTERFACE VARIABLE=bodies_z max_depth=5 DEPTH=5
  
#pragma ACCEL INTERFACE VARIABLE=bodies_y max_depth=5 DEPTH=5
  
#pragma ACCEL INTERFACE VARIABLE=bodies_x max_depth=5 DEPTH=5
  int i;
  int j;
  for (i = 0; i < nbodies; i++) {
    for (j = i + 1; j < nbodies; j++) {
      
#pragma ACCEL PIPELINE AUTO
      double *b2_vz = &bodies_vz[j];
      double *b2_vy = &bodies_vy[j];
      double *b2_vx = &bodies_vx[j];
      double dx =  *(&bodies_x[i]) -  *(&bodies_x[j]);
      double dy =  *(&bodies_y[i]) -  *(&bodies_y[j]);
      double dz =  *(&bodies_z[i]) -  *(&bodies_z[j]);
      double distance = sqrt(dx * dx + dy * dy + dz * dz);
      double mag = dt / (distance * distance * distance);
       *(&bodies_vx[i]) -= dx *  *(&bodies_mass[j]) * mag;
       *(&bodies_vy[i]) -= dy *  *(&bodies_mass[j]) * mag;
       *(&bodies_vz[i]) -= dz *  *(&bodies_mass[j]) * mag;
       *b2_vx += dx *  *(&bodies_mass[i]) * mag;
       *b2_vy += dy *  *(&bodies_mass[i]) * mag;
       *b2_vz += dz *  *(&bodies_mass[i]) * mag;
    }
  }
  for (i = 0; i < nbodies; i++) {
    
#pragma ACCEL PIPELINE AUTO
     *(&bodies_x[i]) += dt *  *(&bodies_vx[i]);
     *(&bodies_y[i]) += dt *  *(&bodies_vy[i]);
     *(&bodies_z[i]) += dt *  *(&bodies_vz[i]);
  }
}
