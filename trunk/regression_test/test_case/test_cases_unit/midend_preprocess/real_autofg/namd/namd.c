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


#define NUM_CELLS	700
//#define NA_BRAM_SIZE    27*500*4        // 27 neighbors , 500 max atoms in each cell, 4 floats each atom
#define NA_BRAM_SIZE    27*512*4        // 27 neighbors , 500 max atoms in each cell, 4 floats each atom
#define DIM_X	10
#define DIM_Y	10
#define DIM_Z	7



#define DATA_SIZE	1400000	// 700*500*4	700 cells, 500 atoms, 4 floats


void namd_kernel(int cell_to_run, int neighbour_to_run, int cell_size,
                 float* i_cell_pos, float* i_cell_vel, float* o_cell_pos, float* o_cell_vel );

int main ()
{
	//long int cycle_cnt = 0;
    int cell_to_run = 7; //700; // NUM_CELLS;
    int neighbour_to_run = 27;
    int cell_size =-1; // -1; //-1;

    float* i_cell_pos; cmost_malloc_1d( &i_cell_pos, "i_cell_pos.dat" , 4, DATA_SIZE);	// 32'h0600 0000
    float* i_cell_vel; cmost_malloc_1d( &i_cell_vel, "i_cell_vel.dat" , 4, DATA_SIZE);	// 32'h0680 0000
    float* o_cell_pos; cmost_malloc_1d( &o_cell_pos, "0" , 4, DATA_SIZE);	// 32'h0700 0000
    float* o_cell_vel; cmost_malloc_1d( &o_cell_vel, "0" , 4, DATA_SIZE);	// 32'h0780 0000

    float* i_cell_pos_fake; cmost_malloc_1d( &i_cell_pos_fake, "0" , 4, DATA_SIZE);	// 32'h

    namd_kernel(cell_to_run, neighbour_to_run, cell_size, i_cell_pos, i_cell_vel, o_cell_pos, o_cell_vel);

    cmost_dump_1d(o_cell_pos, "o_cell_pos_out.dat");
    cmost_dump_1d(o_cell_vel, "o_cell_vel_out.dat");

    cmost_free_1d(i_cell_pos);
    cmost_free_1d(i_cell_vel);
    cmost_free_1d(o_cell_pos);
    cmost_free_1d(o_cell_vel);

	return 0;
}


