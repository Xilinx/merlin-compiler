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
void top(int *a) {  
  for (int i = 0; i < 10000; ++i) {   
    if (i & 1) {  
      for (int j = 0; j < 1000; ++j) {  
        a[i * 1000 + j] = 0;    
      } 
      //expected write-only burst here    
    } 
  }   
}   
                        
#pragma ACCEL kernel
void top2(int *a) {
  for (int i = 0; i < 10000; ++i) {
    if (i & 1) {
      for (int j = 0; j < 1000; ++j) {
        a[i * 1000 + j] = 0;
      }
    } else {
      for (int j = 0; j < 1000; ++j) {
        a[i * 1000 + j] = 0;
      }
    }
    //expected write-only burst here
  }
}

#pragma ACCEL kernel    
void top3(int *a) {    
  for (int i = 0; i < 10000; ++i) {  
    //expected read burst here   
    if (i & 1) {   
      for (int j = 0; j < 100; ++j) {  
        a[i * 1000 + j] = 0; 
      }  
    } else {   
      for (int j = 0; j < 1000; ++j) { 
        a[i * 1000 + j] = 0; 
      }  
    }  
    //expected write burst here    
  }    
}
