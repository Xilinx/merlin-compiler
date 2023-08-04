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
#include "__merlinheadwrapper_gradient.h"
void gradient(int n_samples,int label_size,int feature_size,float *weights,float *data,float *output,float *output_last_element,float *feature_samples,float *label_samples);

void __merlinwrapper_gradient(int n_samples,int label_size,int feature_size,float *weights,float *data,float *output,float *output_last_element,float *feature_samples,float *label_samples)
{
  
#pragma ACCEL wrapper variable=n_samples port=n_samples
  
#pragma ACCEL wrapper variable=label_size port=label_size
  
#pragma ACCEL wrapper variable=feature_size port=feature_size
  
#pragma ACCEL wrapper variable=weights port=weights depth=7056 max_depth=7056 data_type=float io=RO copy=false
  
#pragma ACCEL wrapper variable=data port=data depth=2009600 max_depth=2009600 data_type=float io=NO copy=false
  
#pragma ACCEL wrapper variable=output port=output depth=7056 max_depth=7056 data_type=float io=RW copy=false
  
#pragma ACCEL wrapper variable=output_last_element port=output_last_element depth=1 max_depth=1 data_type=float io=RW copy=true
  
#pragma ACCEL wrapper variable=feature_samples port=feature_samples depth=2007040 max_depth=2007040 data_type=float io=RO copy=false
  
#pragma ACCEL wrapper variable=label_samples port=label_samples depth=2560 max_depth=2560 data_type=float io=RO copy=false
  static float __m_weights[7056];
  static float __m_data[2009600];
  static float __m_output[7056];
  static float __m_output_last_element[1];
  static float __m_feature_samples[2007040];
  static float __m_label_samples[2560];
{
    memcpy((__m_weights + 0),weights,7056 * sizeof(float ));
  }
{
    memcpy((__m_output + 0),output,7056 * sizeof(float ));
  }
{
    memcpy((__m_output_last_element + 0),output_last_element,1 * sizeof(float ));
  }
{
    memcpy((__m_feature_samples + 0),feature_samples,2007040 * sizeof(float ));
  }
{
    memcpy((__m_label_samples + 0),label_samples,2560 * sizeof(float ));
  }
  
  gradient(n_samples,label_size,feature_size,__m_weights,__m_data,__m_output,__m_output_last_element,__m_feature_samples,__m_label_samples);
{
    memcpy(output,(__m_output + 0),7056 * sizeof(float ));
  }
{
    memcpy(output_last_element,(__m_output_last_element + 0),1 * sizeof(float ));
  }
}
