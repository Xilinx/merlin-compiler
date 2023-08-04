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
/*
Implemenataion based on:
A. Danalis, G. Marin, C. McCurdy, J. S. Meredith, P. C. Roth, K. Spafford, V. Tipparaju, and J. S. Vetter.
The scalable heterogeneous computing (shoc) benchmark suite.
In Proceedings of the 3rd Workshop on General-Purpose Computation on Graphics Processing Units, 2010.
*/

#include "md.h"

void md(TYPE force_x[nAtoms],
               TYPE force_y[nAtoms],
               TYPE force_z[nAtoms],
               TYPE position_x[nAtoms],
               TYPE position_y[nAtoms],
               TYPE position_z[nAtoms],
               int32_t NL[nAtoms*maxNeighbors])
{
    TYPE delx, dely, delz, r2inv;
    TYPE r6inv, potential, force, j_x, j_y, j_z;
    TYPE i_x, i_y, i_z, fx, fy, fz;

    int32_t i, j, jidx;


loop_i : for (i = 0; i < nAtoms; i++){
             i_x = position_x[i];
             i_y = position_y[i];
             i_z = position_z[i];
             fx = 0;
             fy = 0;
             fz = 0;
loop_j : for( j = 0; j < maxNeighbors; j++){
             // Get neighbor
             jidx = NL[i*maxNeighbors + j];
             // Look up x,y,z positions
             j_x = position_x[jidx];
             j_y = position_y[jidx];
             j_z = position_z[jidx];
             // Calc distance
             delx = i_x - j_x;
             dely = i_y - j_y;
             delz = i_z - j_z;
             r2inv = 1.0/( delx*delx + dely*dely + delz*delz );
             // Assume no cutoff and aways account for all nodes in area
             r6inv = r2inv * r2inv * r2inv;
             potential = r6inv*(lj1*r6inv - lj2);
             // Sum changes in force
             force = r2inv*potential;
             fx += delx * force;
             fy += dely * force;
             fz += delz * force;
         }
         //Update forces after all neighbors accounted for.
         force_x[i] = fx;
         force_y[i] = fy;
         force_z[i] = fz;
         //printf("dF=%lf,%lf,%lf\n", fx, fy, fz);
         }
}

#pragma ACCEL kernel
void md_kernel(TYPE *force_x,
               TYPE *force_y,
               TYPE *force_z,
               TYPE *position_x,
               TYPE *position_y,
               TYPE *position_z,
               int32_t *NL){

	#pragma ACCEL interface variable=force_x depth=2560
    #pragma ACCEL interface variable=force_y depth=2560
    #pragma ACCEL interface variable=force_z depth=2560
    #pragma ACCEL interface variable=position_x depth=2560
    #pragma ACCEL interface variable=position_y depth=2560
    #pragma ACCEL interface variable=position_z depth=2560
    #pragma ACCEL interface variable=NL depth=40960

    int num_jobs = 10;
//#pragma ACCEL parallel copy_in=position_x,position_y,position_z
    for (int i=0; i<num_jobs; i++) {
			md(force_x + i*nAtoms,force_y + i*nAtoms,force_z + i*nAtoms,
			   position_x + i*nAtoms,position_y + i*nAtoms,position_z + i*nAtoms,NL + i*nAtoms*maxNeighbors);
    }
    return;
}

