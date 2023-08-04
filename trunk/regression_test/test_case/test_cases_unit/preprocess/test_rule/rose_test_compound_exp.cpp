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
#pragma ACCEL kernel

void top(int *sad)
{
  int thresh;
  int gskip_val;
  int gmind;
  int sweep;
  int gskip;
  int _s_d = 0;
  unsigned char _l_d = 0;
  loop_unique_search_1:
// Canonicalized from: for(unsigned char d =(unsigned char )0;((int )_s_d) < 2;_s_d++) {...}
  for (_s_d = ((unsigned char )0); _s_d <= 1; ++_s_d) {
    bool rose__temp = true;
    int rose_temp;
    if (sad[_s_d] <= thresh && sad[_s_d] < gskip_val) {
      int rose_temp_0;
      if (gmind == ((int )sweep) * 2 - 1) {
        rose_temp_0 = ((int )(((int )sweep) * 2 + ((int )_s_d) > gmind + 1));
      }
       else {
        rose_temp_0 = 1;
      }
      rose_temp = ((bool )rose_temp_0);
    }
     else {
      rose_temp = false;
    }
    rose__temp = ((bool )rose_temp);
    if (rose__temp) {
      gskip = ((int )((bool )1));
      gskip_val = sad[_s_d];
    }
  }
}
