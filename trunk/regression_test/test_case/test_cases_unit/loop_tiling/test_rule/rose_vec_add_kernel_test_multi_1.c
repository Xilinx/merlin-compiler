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
#define VEC_SIZE 10000

void f1()
{
  int i;
  int x;
  for (i = 0; i < 160; ++i) {
    
#pragma ACCEL TILED_TAG
    for (int i_sub = 0; i_sub < 4; ++i_sub) {
      
#pragma ACCEL TILED_TAG
      for (int i_sub_sub = 0; i_sub_sub < 8; ++i_sub_sub) {
        
#pragma ACCEL TILED_TAG
        for (int i_sub_sub_sub = 0; i_sub_sub_sub < 2; ++i_sub_sub_sub) {
          x = x + (i * 64 + (i_sub * 16 + (i_sub_sub * 2 + i_sub_sub_sub)));
        }
      }
    }
  }
{
    for (i = 10240L; i < 10250; ++i) {
      x = x + i;
    }
  }
}

void f2()
{
  int i;
  int x;
  
#pragma ACCEL pipeline
  for (i = 0; i < 160; ++i) {
    
#pragma ACCEL TILED_TAG
    for (int i_sub = 0; i_sub < 4; ++i_sub) {
      
#pragma ACCEL TILED_TAG
      for (int i_sub_sub = 0; i_sub_sub < 8; ++i_sub_sub) {
        
#pragma ACCEL TILED_TAG
        for (int i_sub_sub_sub = 0; i_sub_sub_sub < 2; ++i_sub_sub_sub) {
          x = x + (i * 64 + (i_sub * 16 + (i_sub_sub * 2 + i_sub_sub_sub)));
        }
      }
    }
  }
{
    for (i = 10240L; i < 10250; ++i) {
      x = x + i;
    }
  }
}

void f3()
{
  int i;
  int x;
  
#pragma ACCEL pipeline
  for (i = 0; i < 160; ++i) {
    
#pragma ACCEL TILED_TAG
    for (int i_sub = 0; i_sub < 4; ++i_sub) {
      
#pragma ACCEL TILED_TAG
      for (int i_sub_sub = 0; i_sub_sub < 8; ++i_sub_sub) {
        
#pragma ACCEL TILED_TAG
        for (int i_sub_sub_sub = 0; i_sub_sub_sub < 2; ++i_sub_sub_sub) {
          x = x + (i * 64 + (i_sub * 16 + (i_sub_sub * 2 + i_sub_sub_sub)));
        }
      }
    }
  }
{
    for (i = 10240L; i < 10250; ++i) {
      x = x + i;
    }
  }
}

void f4()
{
  int i;
  int x;
  
#pragma ACCEL pipeline
  for (i = 0; i < 160; ++i) {
    
#pragma ACCEL TILED_TAG
    for (int i_sub = 0; i_sub < 4; ++i_sub) {
      
#pragma ACCEL TILED_TAG
      for (int i_sub_sub = 0; i_sub_sub < 8; ++i_sub_sub) {
        
#pragma ACCEL TILED_TAG
        for (int i_sub_sub_sub = 0; i_sub_sub_sub < 2; ++i_sub_sub_sub) {
          x = x + (i * 64 + (i_sub * 16 + (i_sub_sub * 2 + i_sub_sub_sub)));
        }
      }
    }
  }
{
    for (i = 10240L; i < 10250; ++i) {
      x = x + i;
    }
  }
}

void f5()
{
  int i;
  int x;
  for (i = 0; i < 160; ++i) {
    
#pragma ACCEL PARALLEL 
    
#pragma ACCEL PARTIAL_PARALLEL_TAG
    for (int i_sub = 0; i_sub < 4; ++i_sub) {
      
#pragma ACCEL TILED_TAG
      for (int i_sub_sub = 0; i_sub_sub < 8; ++i_sub_sub) {
        
#pragma ACCEL TILED_TAG
        for (int i_sub_sub_sub = 0; i_sub_sub_sub < 2; ++i_sub_sub_sub) {
          x = x + (i * 64 + (i_sub * 16 + (i_sub_sub * 2 + i_sub_sub_sub)));
        }
      }
    }
  }
{
    for (i = 10240L; i < 10250; ++i) {
      x = x + i;
    }
  }
}

void f6()
{
  int i;
  int x;
  for (i = 0; i < 160; ++i) {
    
#pragma ACCEL TILED_TAG
    for (int i_sub = 0; i_sub < 4; ++i_sub) {
      
#pragma ACCEL PARALLEL 
      
#pragma ACCEL PARTIAL_PARALLEL_TAG
      for (int i_sub_sub = 0; i_sub_sub < 8; ++i_sub_sub) {
        
#pragma ACCEL TILED_TAG
        for (int i_sub_sub_sub = 0; i_sub_sub_sub < 2; ++i_sub_sub_sub) {
          x = x + (i * 64 + (i_sub * 16 + (i_sub_sub * 2 + i_sub_sub_sub)));
        }
      }
    }
  }
{
    for (i = 10240L; i < 10250; ++i) {
      x = x + i;
    }
  }
}

void f7()
{
  int i;
  int x;
  for (i = 0; i < 160; ++i) {
    
#pragma ACCEL TILED_TAG
    for (int i_sub = 0; i_sub < 4; ++i_sub) {
      
#pragma ACCEL TILED_TAG
      for (int i_sub_sub = 0; i_sub_sub < 8; ++i_sub_sub) {
        
#pragma ACCEL PARALLEL 
        
#pragma ACCEL PARTIAL_PARALLEL_TAG
        for (int i_sub_sub_sub = 0; i_sub_sub_sub < 2; ++i_sub_sub_sub) {
          x = x + (i * 64 + (i_sub * 16 + (i_sub_sub * 2 + i_sub_sub_sub)));
        }
      }
    }
  }
{
    for (i = 10240L; i < 10250; ++i) {
      x = x + i;
    }
  }
}

void f8()
{
  int i;
  int x;
  
#pragma ACCEL pipeline
  for (i = 0; i < 160; ++i) {
    
#pragma ACCEL PARALLEL 
    
#pragma ACCEL PARTIAL_PARALLEL_TAG
    for (int i_sub = 0; i_sub < 4; ++i_sub) {
      
#pragma ACCEL TILED_TAG
      for (int i_sub_sub = 0; i_sub_sub < 8; ++i_sub_sub) {
        
#pragma ACCEL TILED_TAG
        for (int i_sub_sub_sub = 0; i_sub_sub_sub < 2; ++i_sub_sub_sub) {
          x = x + (i * 64 + (i_sub * 16 + (i_sub_sub * 2 + i_sub_sub_sub)));
        }
      }
    }
  }
{
    for (i = 10240L; i < 10250; ++i) {
      x = x + i;
    }
  }
}

void f9()
{
  int i;
  int x;
  
#pragma ACCEL pipeline
  for (i = 0; i < 160; ++i) {
    
#pragma ACCEL TILED_TAG
    for (int i_sub = 0; i_sub < 4; ++i_sub) {
      
#pragma ACCEL PARALLEL 
      
#pragma ACCEL PARTIAL_PARALLEL_TAG
      for (int i_sub_sub = 0; i_sub_sub < 8; ++i_sub_sub) {
        
#pragma ACCEL TILED_TAG
        for (int i_sub_sub_sub = 0; i_sub_sub_sub < 2; ++i_sub_sub_sub) {
          x = x + (i * 64 + (i_sub * 16 + (i_sub_sub * 2 + i_sub_sub_sub)));
        }
      }
    }
  }
{
    for (i = 10240L; i < 10250; ++i) {
      x = x + i;
    }
  }
}

void f10()
{
  int i;
  int x;
  
#pragma ACCEL pipeline
  for (i = 0; i < 160; ++i) {
    
#pragma ACCEL TILED_TAG
    for (int i_sub = 0; i_sub < 4; ++i_sub) {
      
#pragma ACCEL TILED_TAG
      for (int i_sub_sub = 0; i_sub_sub < 8; ++i_sub_sub) {
        
#pragma ACCEL PARALLEL 
        
#pragma ACCEL PARTIAL_PARALLEL_TAG
        for (int i_sub_sub_sub = 0; i_sub_sub_sub < 2; ++i_sub_sub_sub) {
          x = x + (i * 64 + (i_sub * 16 + (i_sub_sub * 2 + i_sub_sub_sub)));
        }
      }
    }
  }
{
    for (i = 10240L; i < 10250; ++i) {
      x = x + i;
    }
  }
}

void f11()
{
  int i;
  int x;
  
#pragma ACCEL pipeline
  for (i = 0; i < 3; ++i) {
    
#pragma ACCEL TILED_TAG
    for (int i_sub = 0; i_sub < 8; ++i_sub) {
      
#pragma ACCEL PARALLEL 
      
#pragma ACCEL PARTIAL_PARALLEL_TAG
      for (int i_sub_sub = 0; i_sub_sub < 2; ++i_sub_sub) {
        x = x + (i * 16 + (i_sub * 2 + i_sub_sub));
      }
    }
  }
{
    for (i = 48L; i < 63; ++i) {
      x = x + i;
    }
  }
}
#pragma ACCEL kernel

void vec_add_kernel(int *a,int *b,int *c,int inc,int n)
{
  int j;
  
#pragma ACCEL pipeline
  mylabel:
  for (j = 0; j < 128; j++) {
    
#pragma ACCEL TILED_TAG
    for (int j_sub = 0; j_sub < 5; ++j_sub) {
      
#pragma ACCEL PARALLEL 
      
#pragma ACCEL PARTIAL_PARALLEL_TAG
      for (int j_sub_sub = 0; j_sub_sub < 8; ++j_sub_sub) {
        
#pragma ACCEL TILED_TAG
        for (int j_sub_sub_sub = 0; j_sub_sub_sub < 2; ++j_sub_sub_sub) {
          c[j * 80 + (j_sub * 16 + (j_sub_sub * 2 + j_sub_sub_sub))] = a[j * 80 + (j_sub * 16 + (j_sub_sub * 2 + j_sub_sub_sub))] + b[j * 80 + (j_sub * 16 + (j_sub_sub * 2 + j_sub_sub_sub))] + inc;
        }
      }
    }
  }
{
    for (j = 10240L; j < 10250; j++) {
      c[j] = a[j] + b[j] + inc;
    }
  }
  f1();
  f2();
  f3();
  f4();
  f5();
  f6();
  f7();
  f8();
  f9();
  f10();
  f11();
}
