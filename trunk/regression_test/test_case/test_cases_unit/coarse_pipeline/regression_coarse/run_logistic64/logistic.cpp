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
void logistic_regression_kernel(int n_samples,float *global_weights,float *global_data,float *global_gradient);

int main()
{
  int n_samples = 1875;
// input  
  float global_weights[64 * 785];
// input 
  float global_data[1875 * 848];
// output
  float global_gradient[64 * 785];
  int i = 0;
  FILE *fp;
  if (!(fp = fopen("global.dat","rb"))) {
    printf("File can not be open for read.\n");
    return - 1;
  }
  fread(global_data,4,(1875 * (64 + 784)),fp);
  fclose(fp);
  for (i = 0; i < 64 * (784 + 1); i++) {
    global_weights[i] = 0;
  }
  
  logistic_regression_kernel(n_samples,global_weights,global_data,global_gradient);
// TODO: Compare the resuls of global_gradient between FPGA and CPU
  if (!(fp = fopen("gradient.dat","wb"))) {
    printf("File can not be open for write.\n");
    return - 1;
  }
  fwrite(global_gradient,4,(64 * (784 + 1)),fp);
  fclose(fp);
  printf("Finished.\n");
  return 0;
}
