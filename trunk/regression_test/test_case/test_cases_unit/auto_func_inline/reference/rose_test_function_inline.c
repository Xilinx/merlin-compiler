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
//#define FOCUS 0
#if !FOCUS

void sub1_1(int *sub1_1_a)
{
  
#pragma HLS inline
}
#pragma ACCEL kernel

void test1_1()
{
  int test1_1_a[16];
  
#pragma HLS array_partition variable=test1_1_a dim=1
  sub1_1(test1_1_a);
}

void sub1_2(int *sub1_2_a)
{
  
#pragma HLS inline
}
#pragma ACCEL kernel

void test1_2()
{
  int test1_2_a[16];
  
#pragma hls ARRAY_PARTITION variable=test1_2_a dim=1
  sub1_2(test1_2_a);
}

void sub1_3(int *sub1_3_a)
{
}

void sub1_3s(int *sub1_3s_a)
{
}

void sub1_3t(int *sub1_3t_a)
{
  
#pragma HLS inline
}
#pragma ACCEL kernel

void test1_3()
{
  int test1_3_a[16];
  
#pragma hls array_partition variable=test1_3_wrong dim=1
  sub1_3(test1_3_a);
  int test1_3s_a[16];
  
#pragma hls array_partition variable=test1_3s_a dim=1
{
    int test1_3s_a[16];
    sub1_3s(test1_3s_a);
  }
  int test1_3t_a[16];
{
    int test1_3t_a[16];
    
#pragma hls array_partition variable=test1_3t_a dim=1
    sub1_3t(test1_3t_a);
  }
}

void sub1_4(int *sub1_4_a[16])
{
}

void sub1_4s(int *sub1_4s_a[16])
{
}

void sub1_4t(int *sub1_4t_a[16])
{
}

void sub1_4u(int *sub1_4u_a)
{
}
#pragma ACCEL kernel

void test1_4()
{
  int test1_4_a[16];
  
#pragma hls array_partition variable=test1_4_a
  sub1_4(test1_4_a);
  int test1_4s_a[16];
  
#pragma hls array_partition variable=test1_4s_a dim=2
  sub1_4s(test1_4s_a);
  int test1_4t_a[16];
  
#pragma hls array_partition variable=test1_4t_a dim=0
  sub1_4t(test1_4t_a);
  int test1_4u_a;
  
#pragma hls array_partition variable=test1_4u_a dim=1
  sub1_4u(&test1_4u_a);
}

void sub1_5(int *sub1_1_a)
{
  
#pragma HLS Dataflow
}
#pragma ACCEL kernel

void test1_5()
{
  int test1_5_a[16];
  sub1_5(test1_5_a);
}

void sub2_1(int *sub2_1_a[16][16])
{
  
#pragma HLS inline
}

void sub2_1s(int *sub2_1s_a[16][16])
{
  
#pragma HLS inline
}

void sub2_1t(int sub2_1t_a[16][16])
{
  
#pragma HLS inline
}

void sub2_1u(int sub2_1u_a[16][16])
{
  
#pragma HLS inline
}
#pragma ACCEL kernel

void test2_1()
{
  int test2_1_a[16][16][16];
  
#pragma hls array_partition variable=test2_1_a dim=1
  sub2_1(test2_1_a);
  int test2_1s_a[16][16][16];
  
#pragma hls array_partition variable=test2_1s_a dim=3
  sub2_1s(test2_1s_a);
  int test2_1t_a[16][16][16];
  
#pragma hls array_partition variable=test2_1t_a dim=3
  
#pragma hls array_partition variable=test2_1t_a dim=1
  sub2_1t(test2_1t_a[0]);
  int test2_1u_a[16][16][16];
  
#pragma hls array_partition variable=test2_1u_a dim=1
  
#pragma hls array_partition variable=test2_1u_a dim=3
  sub2_1u(test2_1u_a[0]);
}

void sub2_2(int *sub2_2_a[16][16])
{
  
#pragma HLS inline
}

void sub2_2s(int *sub2_2s_a[16][16])
{
  
#pragma HLS inline
}

void sub2_2t(int sub2_2t_a[16][16])
{
  
#pragma HLS inline
}

void sub2_2u(int sub2_2u_a[16][16])
{
}

void sub2_2v(int sub2_2v_a)
{
}

void sub2_2w(int *sub2_2w_a)
{
}
#pragma ACCEL kernel

void test2_2()
{
  int test2_2_a[16][16][16];
  
#pragma hls array_partition variable=test2_2_a dim=1
  sub2_2(test2_2_a);
  int test2_2s_a[16][16][16];
  
#pragma hls array_partition variable=test2_2s_a complete dim=3
  
#pragma hls array_partition variable=test2_2s_a dim=2 factor=4
  sub2_2s((test2_2s_a + 2));
  int test2_2t_a[16][16][16];
  
#pragma hls array_partition variable=test2_2t_a dim=2
  sub2_2t(&test2_2t_a[0][2] + 4);
  int test2_2u_a[16][16][16];
  
#pragma hls array_partition variable=test2_2u_a dim=1
  sub2_2u(&test2_2u_a[0][2] + 4);
  int test2_2v_a[16][16][16];
  
#pragma hls array_partition variable=test2_2v_a dim=1
  sub2_2v((test2_2v_a[0][2] + 4));
}

void sub2_3(int (*sub2_3_a)[16][16])
{
  
#pragma HLS inline
}

void sub2_3s(int (*sub2_3s_a)[16])
{
  
#pragma HLS inline
}

void sub2_3t(int sub2_3t_a[16])
{
}

void sub2_3u(int sub2_3u_n,int sub2_3u_a[16])
{
  
#pragma HLS inline
}

void sub2_3v(int sub2_3v_a[16],int sub2_3v_b[16])
{
  
#pragma HLS inline
}

void sub2_3w(int sub2_3w_a[16],int sub2_3w_b[16])
{
  
#pragma HLS inline
}

void sub2_3x(int sub2_3x_a[16],int sub2_3x_b[16])
{
}

void sub2_3y(int sub2_3y_a[16][16])
{
  
#pragma HLS inline
}

void sub2_3z(int *sub2_3z_a)
{
}

int *sub2_3z_wrap(int *sub2_3z_wrap_a)
{
  
#pragma HLS inline
  return sub2_3z_wrap_a;
}
#pragma ACCEL kernel

void test2_3()
{
  int test2_3_a[16][16][16];
  
#pragma hls array_partition variable=test2_3_a dim=2
  sub2_3(test2_3_a);
  int test2_3s_a[16][16][16];
  
#pragma hls array_partition variable=test2_3s_a dim=2
  sub2_3s(test2_3s_a[0]);
  int test2_3t_a[16][16][16];
  
#pragma hls array_partition variable=test2_3t_a dim=2
  sub2_3t(test2_3t_a[0][0] + 5);
  int test2_3u_a[16][16][16];
  
#pragma hls array_partition variable=test2_3u_a dim=3
  sub2_3u(1,test2_3u_a[0][0]);
  int test2_3v_a[16][16][16];
  
#pragma hls array_partition variable=test2_3v_a dim=2
  sub2_3v(test2_3u_a[0][0],test2_3v_a[0][0]);
  int test2_3w_a[16][16][16];
  
#pragma hls array_partition variable=test2_3w_a dim=2
  sub2_3w(test2_3w_a[0][0],test2_3u_a[0][0]);
  int test2_3x_a[16][16][16];
  
#pragma hls array_partition variable=test2_3x_a dim=2
  sub2_3x(test2_3x_a[0][0],test2_3w_a[0][0]);
  int test2_3y_a[16][16][16];
  
#pragma hls array_partition variable=test2_3y_a dim=2
  sub2_3y(&test2_3y_a[0][test2_3y_a[0][0][0]]);
  int test2_3z_a[16];
  
#pragma hls array_partition variable=test2_3z_a dim=1
  sub2_3z((sub2_3z_wrap(test2_3z_a)));
// Note that sub2_3x is not inlined currently
  sub2_3z_wrap((sub2_3z_wrap(test2_3z_a)));
}

void sub2_4(int *sub2_4_a)
{
  
#pragma HLS inline
}
int g_test2_4_a[16];
#pragma ACCEL kernel

void test2_4()
{
  
#pragma HLS array_partition variable=g_test2_4_a dim=1
  sub2_4(g_test2_4_a);
}

void sub2_4s(int *sub2_4s_a)
{
  
#pragma HLS inline
}
int g_test2_4s_a[16];
#pragma HLS array_partition variable=g_test2_4s_a dim=1
#pragma ACCEL kernel

void test2_4s()
{
  sub2_4s(g_test2_4s_a);
}

void sub2_4t()
{
  
#pragma HLS inline
  g_test2_4s_a[0];
}
#pragma ACCEL kernel

void test2_4t()
{
  sub2_4t();
}

void sub2_4us()
{
  
#pragma HLS inline
  g_test2_4s_a[0];
}

void sub2_4u()
{
  
#pragma HLS inline
  sub2_4us();
}
#pragma ACCEL kernel

void test2_4u()
{
  sub2_4u();
}

void sub3_1(int *sub3_1_a[16][16])
{
  
#pragma HLS inline
}

void sub3_1s(int sub3_1s_a[16][16])
{
}

void sub3_1t(int sub3_1t_a[16][16],int sub3_1t_b[16][16])
{
  
#pragma HLS inline
}

void sub3_1u(int sub3_1u_a[16][16],int sub3_1u_b[16][16])
{
}

void sub3_1v(int sub3_1v_a[16][16],int sub3_1v_b[16][16])
{
  
#pragma HLS inline
}

void sub3_1w(int sub3_1w_a[16][16],int sub3_1w_b[16][16])
{
  
#pragma HLS inline
}
#pragma ACCEL kernel

void test3_1()
{
  int test3_1_a[16][16][16];
  
#pragma hls array_partition variable=test3_1_a dim=2
  sub3_1(test3_1_a);
  sub3_1((test3_1_a + 1));
  int test3_1_b[16][16][16];
  sub3_1(test3_1_b);
  int test3_1s_a[16][16][16];
  
#pragma hls array_partition variable=test3_1s_a dim=1
  sub3_1s(test3_1s_a[0]);
  sub3_1s(test3_1s_a[0] + 1);
  int test3_1t_a[16][16][16];
  
#pragma hls array_partition variable=test3_1t_a dim=3
  sub3_1t(test3_1t_a[0],test3_1t_a[0] + 1);
  int test3_1u_a[16][16][16];
  
#pragma hls array_partition variable=test3_1u_a dim=1
  sub3_1u(test3_1u_a[0],test3_1u_a[0] + 1);
  int test3_1vw_a[16][16][16];
  
#pragma hls array_partition variable=test3_1vw_a dim=2
  int test3_1vw_dummy[16][16];
  sub3_1v(test3_1vw_dummy,test3_1vw_a[0]);
  sub3_1w(test3_1vw_a[0],test3_1vw_dummy);
}

void sub3_2(int *sub3_2_a[16][16])
{
  
#pragma HLS inline
}

void sub3_2s(int (*sub3_2s_a)[16][16])
{
  
#pragma HLS inline
}

void sub3_2t(int (*sub3_2t_a)[16][16])
{
  
#pragma HLS inline
  
#pragma HLS array_partition variable=sub3_2t_a dim=2
}

void sub3_2u(int (*sub3_2u_a)[16][16])
{
  
#pragma HLS inline
  
#pragma HLS array_partition variable=sub3_2u_a dim=2
}

void sub3_2vs(int (*sub3_2vs_a)[16][16])
{
  
#pragma HLS inline
}

void sub3_2v(int (*sub3_2v_a)[16][16])
{
  
#pragma HLS inline
  sub3_2vs(sub3_2v_a);
}

void sub3_2ws(int (*sub3_2ws_a)[16][16])
{
  
#pragma HLS inline
}

void sub3_2w(int (*sub3_2w_a)[16][16])
{
  
#pragma HLS inline
  
#pragma HLS array_partition variable=sub3_2w_a dim=2
  sub3_2ws(sub3_2w_a);
}

void sub3_2xs(int (*sub3_2xs_a)[16][16])
{
  
#pragma HLS inline
  
#pragma HLS array_partition variable=sub3_2xs_a dim=2
}

void sub3_2x(int (*sub3_2x_a)[16][16])
{
  
#pragma HLS inline
  sub3_2xs(sub3_2x_a);
}

void sub3_2ys(int sub3_2ys_a[16])
{
}

void sub3_2y(int sub3_2y_a[16][16])
{
  
#pragma HLS inline
  sub3_2ys(sub3_2y_a[0]);
}
#pragma ACCEL kernel

void test3_2()
{
  
#pragma hls array_partition variable=test3_2_a dim=2
  int test3_2_a[16][16][16];
  sub3_2(test3_2_a);
  int test3_2s_a[16][16][16];
  sub3_2s(test3_2s_a);
  
#pragma hls array_partition variable=test3_2s_a dim=2
  int test3_2t_a[16][16][16];
  sub3_2t(test3_2t_a);
  int test3_2u_a[16][16][16];
  
#pragma hls array_partition variable=test3_2u_a dim=1
  sub3_2u(test3_2u_a);
  int test3_2v_a[16][16][16];
  
#pragma hls array_partition variable=test3_2v_a dim=2
  sub3_2v(test3_2v_a);
  int test3_2w_a[16][16][16];
  sub3_2w(test3_2w_a);
  int test3_2x_a[16][16][16];
  sub3_2x(test3_2x_a);
  int test3_2y_a[16][16];
  
#pragma HLS array_partition variable=test3_2y_a dim=1
  sub3_2y(test3_2y_a);
}

void sub3_3(int *sub3_3_a)
{
  
#pragma HLS inline
}

void sub3_3sss(int *sub3_3ss_a)
{
  
#pragma HLS inline
}

void sub3_3ss(int *sub3_3ss_a)
{
  
#pragma HLS inline
  sub3_3sss(sub3_3ss_a);
}

void sub3_3s(int *sub3_3s_a)
{
  
#pragma HLS inline
  sub3_3ss(sub3_3s_a);
}

void sub3_3tt(int *sub3_3tt_a)
{
  
#pragma HLS inline
}

void sub3_3ts(int *sub3_3ts_a)
{
  
#pragma HLS inline
  sub3_3tt(sub3_3ts_a);
}

void sub3_3t(int *sub3_3t_a)
{
  
#pragma HLS inline
  sub3_3ts(sub3_3t_a);
  sub3_3tt(sub3_3t_a);
}

void sub3_3ut(int *sub3_3ut_a)
{
  
#pragma HLS inline
  
#pragma HLS array_partition variable=sub3_3ut_a dim=1
}

void sub3_3us(int *sub3_3us_a)
{
  
#pragma HLS inline
  sub3_3ut(sub3_3us_a);
}

void sub3_3u(int *sub3_3u_a)
{
  
#pragma HLS inline
  sub3_3us(sub3_3u_a);
  sub3_3ut(sub3_3u_a);
}

void sub3_3wt(int *sub3_3wt_a,int *sub3_3wt_b)
{
  
#pragma HLS inline
  
#pragma HLS array_partition variable=sub3_3wt_a dim=1
  
#pragma HLS array_partition variable=sub3_3wt_b dim=1
}

void sub3_3ws(int *sub3_3ws_a)
{
  
#pragma HLS inline
  sub3_3wt(0,sub3_3ws_a);
}

void sub3_3w(int *sub3_3w_a)
{
  
#pragma HLS inline
  sub3_3ws(0);
  sub3_3wt(sub3_3w_a,0);
}

void sub3_3xs(int *sub3_3xs_a)
{
  
#pragma HLS inline
}

void sub3_3xt(int **sub3_3xt_a)
{
  
#pragma HLS inline
  
#pragma HLS array_partition variable=sub3_3xt_a dim=2
}

void sub3_3x(int **sub3_3x_a)
{
  
#pragma HLS inline
  sub3_3xs(sub3_3x_a[0]);
  sub3_3xt(sub3_3x_a);
}

void sub3_3ys(int *sub3_3ys_a)
{
}

void sub3_3yt(int **sub3_3yt_a)
{
  
#pragma HLS inline
  
#pragma HLS array_partition variable=sub3_3yt_a dim=1
}

void sub3_3y(int **sub3_3y_a)
{
  
#pragma HLS inline
  sub3_3ys(sub3_3y_a[0]);
  sub3_3yt(sub3_3y_a);
}
#pragma ACCEL kernel

void test3_3()
{
  int test3_3_a[16];
  
#pragma HLS array_partition variable=test3_3_a dim=1
  sub3_3(test3_3_a);
  int test3_3s_a[16];
  
#pragma HLS array_partition variable=test3_3s_a dim=1
  sub3_3s(test3_3s_a);
  int test3_3t_a[16];
  
#pragma HLS array_partition variable=test3_3t_a dim=1
  sub3_3t(test3_3t_a);
  int test3_3u_a[16];
  sub3_3u(test3_3u_a);
  int test3_3w_a[16];
  sub3_3w(test3_3w_a);
  int test3_3x_a[16];
  sub3_3x(test3_3x_a);
  int test3_3y_a[16];
  sub3_3y(test3_3y_a);
}

void sub4_1(int *sub4_1_a)
{
  
#pragma HLS inline
}

void sub4_1s(int *sub4_1s_a)
{
  
#pragma HLS inline
}

void sub4_1ts(int **sub4_1ts_a)
{
  
#pragma HLS inline
}

void sub4_1t(int *sub4_1t_a)
{
  
#pragma HLS inline
  int **sub4_1t_b = &sub4_1t_a;
  sub4_1ts(sub4_1t_b);
}
#pragma ACCEL kernel

void test4_1()
{
  int test4_1_a[16];
  int *test4_1_pa = test4_1_a;
// this is ignored currently
  
#pragma HLS array_partition variable=test4_1_a dim=1
  sub4_1(test4_1_pa);
  int *test4_1s_pa = test4_1_pa;
// this is ignored currently
  sub4_1s(test4_1s_pa);
  int *test4_1t_pa = test4_1s_pa;
// this is ignored currently
  sub4_1t(test4_1t_pa);
}

void sub5_1(int *b)
{
  
#pragma HLS inline off
}
#pragma ACCEL kernel

void test5_1()
{
  int test5_1_a[46];
  
#pragma HLS array_partition variable=test5_1_a dim=1
  sub5_1(test5_1_a);
}

void sub6_1(int **sub6_1_a)
{
  
#pragma HLS inline
}

void sub6_1s(int sub6_1s_a)
{
}

void sub6_1t(int **sub6_1t_a)
{
  
#pragma HLS inline
}
#pragma ACCEL kernel

void test6_1()
{
  int test6_1_a[16];
  
#pragma HLS array_partition variable=test6_1_a dim=1
  sub6_1(test6_1_a);
  int test6_1s_a[16];
  
#pragma HLS array_partition variable=test6_1s_a dim=1
  sub6_1s(test6_1s_a);
  int test6_1t_a[16];
  int **test6_1t_b = (int **)test6_1t_a;
  
#pragma HLS array_partition variable=test6_1t_a dim=1
  sub6_1t(test6_1t_b);
}
#pragma ACCEL kernel

void test6_2()
{
  int test6_2_a[16];
  
#pragma HLS array_partition variable=test6_2_a dim=1
  sub6_2(test6_2_a);
}
void test6_3();

void sub6_3(int *sub6_3_a)
{
  test6_3();
}
#pragma ACCEL kernel

void test6_3()
{
  int test6_3_a[16];
  
#pragma HLS array_partition variable=test6_3_a dim=1
  sub6_3(test6_3_a);
}

struct s6_4 
{
  int s6_4_a[16];
}
;

void sub6_4(struct s6_4 *sub6_4_a)
{
  
#pragma HLS inline
}

void sub6_4s(int *sub6_4s_a)
{
  
#pragma HLS inline
  
#pragma HLS array_partition variable=sub6_4s_a dim=1
}
#pragma ACCEL kernel

void test6_4()
{
  struct s6_4 test6_4_a[16];
  
#pragma HLS array_partition variable=test6_4_a dim=1
  sub6_4(test6_4_a);
  struct s6_4 test6_4s_a[16];
  sub6_4s(test6_4s_a[0] . s6_4_a);
}
#pragma ACCEL kernel

void test6_5()
{
}
// moved to test_function_inline.cpp

void sub6_6(int *sub6_6_a)
{
  
#pragma HLS inline
}
#pragma ACCEL kernel

void test6_6(int *in_a)
{
  int test6_6_a[16];
  
#pragma HLS array_partition variable=test6_6_a dim=1
  sub6_6(test6_6_a);
  memcpy(in_a,test6_6_a,16 * sizeof(int ));
}
#else
//void sub6_3(int * sub6_3_a) { test6_3(); }
//#pragma ACCEL kernel
//void test6_3()
//{
//  int test6_3_a[16];
//#pragma HLS array_partition variable=test6_3_a dim=1
//  sub6_3(test6_3_a); 
//}
//
//void sub1_1(int * sub1_1_a) {}
//
//#pragma ACCEL kernel
//void test1_1()
//{
//  int test1_1_a[16];
//#pragma HLS array_partition variable=test1_1_a dim=1
//  sub1_1(test1_1_a); 
//}
#endif 
//#define FOCUS 0
#if !FOCUS

class c6_5 
{
  

  public: inline void sub6_5(int *sub6_5_a)
{
    
#pragma HLS inline
  }
  

  inline static void sub6_5s(int *sub6_5_a)
{
    
#pragma HLS inline
  }
  

  inline void sub6_5t()
{
    
#pragma HLS array_partition variable=c6_5_a dim=1
  }
  protected: int c6_5_a[16];
}
;
#pragma ACCEL kernel

void test6_5()
{
  class c6_5 test6_5_a[16];
  
#pragma HLS array_partition variable=test6_5_a dim=1
  test6_5_a[0] .  sub6_5 (0);
  int test6_5_b[16];
  
#pragma HLS array_partition variable=test6_5_b dim=1
  test6_5_a[0] .  sub6_5 (test6_5_b);
  test6_5_a[0] .  sub6_5s (test6_5_b);
  test6_5_a[0] .  sub6_5t ();
}
#else
//void sub1_1(int * sub1_1_a) {}
//
//#pragma ACCEL kernel
//void test1_1()
//{
//  int test1_1_a[16];
//#pragma HLS array_partition variable=test1_1_a dim=1
//  sub1_1(test1_1_a); 
//}
#endif
