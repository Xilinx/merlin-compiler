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
#include "logistic.h"

int main()
{   

    int inc1 = 0, inc2 = 0;
    int n_samples = TOTAL_SIZE;
    float * global_weights; cmost_malloc_1d( &global_weights, "0" , 4, WEIGHTS_SIZE);
    float * global_data; cmost_malloc_1d( &global_data, "global.dat", 4, DATA_SIZE);
    float * global_gradient; cmost_malloc_1d( &global_gradient, "0"  , 4, WEIGHTS_SIZE);
    //float * global_gradient_gold; cmost_malloc_1d( &global_gradient_gold, "gradient_gold.dat"  , 4, WEIGHTS_SIZE);
	if (!(global_weights && global_data && global_gradient)) while (1);


    logistic_regression_kernel( n_samples, global_weights, global_data, global_gradient );

    int i;
    int err=0;
    double epsilon = 1.0e-16;

    //for (i = 0 ; i < WEIGHTS_SIZE; i++) {
    //    if (fabs(global_gradient[i] - global_gradient_gold[i]) > epsilon) {
    //        err++;
    //        printf("Test FAIL at %d: %f (expected %f)\n", i, global_gradient[i], global_gradient_gold[i]);
    //    }
    //}

    //cmost_write_file(global_gradient, "gradient.dat", 4*WEIGHTS_SIZE); // added
    cmost_dump(global_gradient, "gradient_out.dat");
    cmost_free_1d(global_weights);
    cmost_free_1d(global_data);
    cmost_free_1d(global_gradient);
    //cmost_free_1d(global_gradient_gold);

    if (err != 0) return 1;
    return 0;
}

