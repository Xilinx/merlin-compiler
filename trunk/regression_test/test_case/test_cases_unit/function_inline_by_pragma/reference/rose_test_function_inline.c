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

void sub0(int *b)
{
  int a =  *b;
}
#pragma ACCEL kernel

void test0(int *a)
{
  sub0(a);
// no inline
}
#pragma ACCEL inline

void sub1(int *b)
{
  int a =  *b;
}
#pragma ACCEL kernel

void test1(int *a)
{
  int *b__2 = a;
  int a =  *b__2;
  rose_inline_end__3:
  ;
}
#pragma ACCEL inline

void sub1_1(int *b)
{
  int a =  *b;
}

void test1_1(int *a)
{
  sub1_1(a);
}

int sub2(int *b)
{
  int a =  *b;
  return 0;
}
#pragma ACCEL kernel

void test2(int *a)
{
  sub2(a);
  
#pragma ACCEL inline
  int rose_temp__7;
{
    int *b__5 = a + 1;
    int a =  *b__5;
{
      rose_temp__7 = 0;
      goto rose_inline_end__6;
    }
    rose_inline_end__6:
    ;
  }
  a + rose_temp__7;
}
#pragma ACCEL inline

int sub3(int *b)
{
  int a =  *b;
  return 0;
}
#pragma ACCEL kernel

void test3(int *a)
{
{
    int *b__9 = a;
    int a =  *b__9;
{
      0;
      goto rose_inline_end__10;
    }
    rose_inline_end__10:
    ;
  }
  int rose_temp__14;
{
    int *b__12 = a + 1;
    int a =  *b__12;
{
      rose_temp__14 = 0;
      goto rose_inline_end__13;
    }
    rose_inline_end__13:
    ;
  }
  a + rose_temp__14;
}

int sub4(int *b)
{
  int a =  *b;
  return 0;
}
#pragma ACCEL kernel

void test4(int *a)
{
  sub4(a);
  a + sub4(a + 1);
  
#pragma ACCEL inline
{
    int *b__16 = a - 1;
    int a =  *b__16;
{
      0;
      goto rose_inline_end__17;
    }
    rose_inline_end__17:
    ;
  }
}
#pragma ACCEL inline

int sub5(int *b)
{
  int a =  *b;
  return 0;
}
#pragma ACCEL kernel

void test5(int *a)
{
{
    int *b__19 = a;
    int a =  *b__19;
{
      0;
      goto rose_inline_end__20;
    }
    rose_inline_end__20:
    ;
  }
  int rose_temp__24;
{
    int *b__22 = a + 1;
    int a =  *b__22;
{
      rose_temp__24 = 0;
      goto rose_inline_end__23;
    }
    rose_inline_end__23:
    ;
  }
  a + rose_temp__24;
  
#pragma ACCEL inline
{
    int *b__26 = a - 1;
    int a =  *b__26;
{
      0;
      goto rose_inline_end__27;
    }
    rose_inline_end__27:
    ;
  }
}

int sub6(int *b)
{
  int a =  *b;
{
    int *b__29 = a;
    int a =  *b__29;
{
      0;
      goto rose_inline_end__30;
    }
    rose_inline_end__30:
    ;
  }
  return 0;
}
#pragma ACCEL inline

int sub6_1(int *b)
{
  int a =  *b;
  return 0;
}
#pragma ACCEL kernel

void test6(int *a)
{
{
    int *b__32 = a - 1;
    int a =  *b__32;
{
      0;
      goto rose_inline_end__33;
    }
    rose_inline_end__33:
    ;
  }
  sub6(a);
}
void test7(int *a);
#pragma ACCEL inline

int sub7_s(int *b)
{
  int a =  *b;
  test7(b);
  return 0;
}

int sub7(int *b)
{
  int a =  *b;
  sub7_s(b);
  return 0;
}
#pragma ACCEL kernel

void test7(int *a)
{
  sub7(a);
}
#pragma ACCEL INLINE

int sub7_s1(int *b)
{
  int a =  *b;
  return 0;
}
#pragma ACCEL inline

int sub7_1(int *b)
{
  int a =  *b;
{
    int *b__35 = b + 1;
    int a =  *b__35;
{
      0;
      goto rose_inline_end__36;
    }
    rose_inline_end__36:
    ;
  }
  return 0;
}
#pragma ACCEL inline

int sub7_s2(int *b)
{
  int a =  *b;
{
    int *b__38 = b + 2;
    int a =  *b__38;
{
      0;
      goto rose_inline_end__39;
    }
    rose_inline_end__39:
    ;
  }
  return 0;
}
#pragma ACCEL kernel

void test7_1(int *a)
{
{
    int *b__44 = a;
    int a =  *b__44;
{
      int *b__35 = b__44 + 1;
      int a =  *b__35;
{
        0;
        goto rose_inline_end__36__1;
      }
      rose_inline_end__36__1:
      ;
    }
{
      0;
      goto rose_inline_end__45;
    }
    rose_inline_end__45:
    ;
  }
{
    int *b__41 = a;
    int a =  *b__41;
{
      0;
      goto rose_inline_end__42;
    }
    rose_inline_end__42:
    ;
  }
{
    int *b__47 = a;
    int a =  *b__47;
{
      int *b__38 = b__47 + 2;
      int a =  *b__38;
{
        0;
        goto rose_inline_end__39__2;
      }
      rose_inline_end__39__2:
      ;
    }
{
      0;
      goto rose_inline_end__48;
    }
    rose_inline_end__48:
    ;
  }
}
#pragma ACCEL INLINE

int sub8(int *b)
{
  int a =  *b;
  if (a) 
    return a;
   else if (b) 
    return 1;
  return 2;
}
#pragma ACCEL kernel

void test8(int *a)
{
  int rose_temp__52;
{
    int *b__50 = a - 1;
    int a =  *b__50;
    if (a) {
      rose_temp__52 = a;
      goto rose_inline_end__51;
    }
     else if (b__50) {
      rose_temp__52 = 1;
      goto rose_inline_end__51;
    }
{
      rose_temp__52 = 2;
      goto rose_inline_end__51;
    }
    rose_inline_end__51:
    ;
  }
  int c = rose_temp__52;
}
