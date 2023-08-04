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
/*
 * normal-c.hpp
 *
 *      @date: May 22, 2015
 *      @author: Yong-Dian Jian
 *		@brief:
 *
 */
#include "normal-c.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
//std::numeric_limits<Scalar>::epsilon();
const double epsilon = 2.220446e-16;
////namespace gicp {
/*********************************************************************************************************/

void cross_v(const Scalar *x,const Scalar *y,Scalar *x_cross_y)
{
  x_cross_y[0] = x[1] * y[2] - x[2] * y[1];
  x_cross_y[1] = x[2] * y[0] - x[0] * y[2];
  x_cross_y[2] = x[0] * y[1] - x[1] * y[0];
}
/*********************************************************************************************************/

Scalar squared_norm_v(Scalar *x)
{
//  Scalar a = x[0]*x[0];
//  Scalar b = x[1]*x[1];
//  Scalar c = x[2]*x[2];
//  Scalar d = a+b;
//  Scalar e = d+c;
//
//  return e; //reduce critical path
  return x[0] * x[0] + x[1] * x[1] + x[2] * x[2];
}
/*********************************************************************************************************/

void div_v(Scalar *x,Scalar a,Scalar *y)
{
  y[0] = x[0] / a;
  y[1] = x[1] / a;
  y[2] = x[2] / a;
}
/* Input:
 * data is a buffer of length n*step (like a n by step matrix)
 * n is the number of total points
 * step is the number of elements per row
 * indices is of length m where each number is the row index of interest
 * origin is of length 3: indicating the location of sensor, the resulting normal has to face to the origin
 *
 * Output:
 * covariance is of length 9, denoting a 3 by 3 covariance matrix
 *
 */

void covariance_v(const Scalar *data,const int step,const int *indices,const int m,Scalar *cov)
{
// create the buffer on the stack which is much faster than using mat[indices[i]] and centroid as a buffer
  Scalar accu[9] = {(0.0), (0.0), (0.0), (0.0), (0.0), (0.0), (0.0), (0.0), (0.0)};
  int i;
  for (i = 0; i < m; ++i) {
    int idx = indices[i];
    const Scalar *ptr = data + idx * step;
    Scalar x =  *ptr;
    Scalar y =  *(ptr + 1);
    Scalar z =  *(ptr + 2);
    accu[0] += x * x;
    accu[1] += x * y;
    accu[2] += x * z;
    accu[3] += y * y;
    accu[4] += y * z;
    accu[5] += z * z;
    accu[6] += x;
    accu[7] += y;
    accu[8] += z;
  }
  for (i = 0; i < 9; ++i) {
    accu[i] /= m;
  }
  cov[0] = accu[0] - accu[6] * accu[6];
  cov[1] = accu[1] - accu[6] * accu[7];
  cov[2] = accu[2] - accu[6] * accu[8];
  cov[4] = accu[3] - accu[7] * accu[7];
  cov[5] = accu[4] - accu[7] * accu[8];
  cov[8] = accu[5] - accu[8] * accu[8];
  cov[3] = cov[1];
  cov[6] = cov[2];
  cov[7] = cov[5];
}
/*********************************************************************************************************/
/** \brief Compute the roots of a quadratic polynom x^2 + b*x + c = 0
 * \param[in] b linear parameter
 * \param[in] c constant parameter
 * \param[out] roots solutions of x^2 + b*x + c = 0
 */

void computeRoots2_v(const Scalar b,const Scalar c,Scalar *roots)
{
  roots[0] = 0.0;
  Scalar d = b * b * 1.0 - 4.0 * c;
// no real roots!!!! THIS SHOULD NOT HAPPEN!
  if (d < 0.0) {
    d = 0.0;
  }
  Scalar sd = sqrt(d);
  roots[2] = ((Scalar )0.5) * (b + sd);
  roots[1] = ((Scalar )0.5) * (b - sd);
}
/*********************************************************************************************************/
/** \brief computes the roots of the characteristic polynomial of the input matrix m, which are the eigenvalues
 * \param[in] m is of length 9, denoting a 3 by 3 matrix
 * \param[out] roots is of length 3, denotes the eigenvalues of m
 */

static void swap(double *a,double *b)
{
  double tmp;
  tmp =  *a;
   *a =  *b;
   *b = tmp;
}

void computeRoots_v(const Scalar *m,Scalar *roots)
{
// The characteristic equation is x^3 - c2*x^2 + c1*x - c0 = 0.  The
// eigenvalues are the roots to this equation, all guaranteed to be
// real-valued, because the matrix is symmetric.
// m is a 3 by 3 matrix in the following ordering
// m(0) m(1) m(2)
// m(3) m(4) m(5)
// m(6) m(7) m(8)
  Scalar c0 = m[0] * m[4] * m[8] + ((Scalar )2) * m[1] * m[2] * m[5] - m[0] * m[5] * m[5] - m[4] * m[2] * m[2] - m[8] * m[1] * m[1];
  Scalar c1 = m[0] * m[4] - m[1] * m[1] + m[0] * m[8] - m[2] * m[2] + m[4] * m[8] - m[5] * m[5];
  Scalar c2 = m[0] + m[4] + m[8];
//if (fabs (c0) < std::numeric_limits<Scalar>::epsilon() )// one root is 0 -> quadratic equation
// one root is 0 -> quadratic equation
  if (fabs(c0) < epsilon) {
    computeRoots2_v(c2,c1,roots);
  }
   else {
    const Scalar s_inv3 = (Scalar )(1.0 / 3.0);
    const Scalar s_sqrt3 = sqrt(((Scalar )3.0));
// Construct the parameters used in classifying the roots of the equation
// and in solving the equation for the roots in closed form.
    Scalar c2_over_3 = c2 * s_inv3;
    Scalar a_over_3 = (c1 - c2 * c2_over_3) * s_inv3;
    if (a_over_3 > ((Scalar )0)) {
      a_over_3 = ((Scalar )0);
    }
    Scalar half_b = ((Scalar )0.5) * (c0 + c2_over_3 * (((Scalar )2) * c2_over_3 * c2_over_3 - c1));
    Scalar q = half_b * half_b + a_over_3 * a_over_3 * a_over_3;
    if (q > ((Scalar )0)) {
      q = ((Scalar )0);
    }
// Compute the eigenvalues by solving for the roots of the polynomial.
    Scalar rho = sqrt(-a_over_3);
    Scalar theta = atan2((sqrt(-q)),half_b) * s_inv3;
    Scalar cos_theta = cos(theta);
    Scalar sin_theta = sin(theta);
    roots[0] = c2_over_3 + ((Scalar )2) * rho * cos_theta;
    roots[1] = c2_over_3 - rho * (cos_theta + s_sqrt3 * sin_theta);
    roots[2] = c2_over_3 - rho * (cos_theta - s_sqrt3 * sin_theta);
// Sort in increasing order.
    if (roots[0] >= roots[1]) {
      swap(&roots[0],&roots[1]);
    }
    if (roots[1] >= roots[2]) {
      swap(&roots[1],&roots[2]);
      if (roots[0] >= roots[1]) {
        swap(&roots[0],&roots[1]);
      }
    }
// eigenval for symetric positive semi-definite matrix can not be negative! Set it to 0
    if (roots[0] <= 0) {
      computeRoots2_v(c2,c1,roots);
    }
  }
}
/*********************************************************************************************************/
/*
 * Input:
 * covariance is of length 9, denoting a 3 by 3 covariance matrix
 *
 * Output:
 * v3 is of length 3, denoting the smallest eigenvector
 * lambda is of length 1, denoting the smallest eigenvalue
 */

void eigen33_v(const Scalar *cov,Scalar *v3,Scalar *lambda3)
{
/* solve for plane equation */
// Scale the matrix so its entries are in [-1,1].  The scaling is applied
// only when at least one matrix entry has magnitude larger than 1.
  int i;
  Scalar scale = 0.0;
  for (i = 0; i < 9; ++i) {
    Scalar abs_value = fabs(cov[i]);
    if (scale < abs_value) {
      scale = abs_value;
    }
  }
  if (scale < 1.0) {
    scale = 1.0;
  }
  Scalar scaled_cov[9];
  for (i = 0; i < 9; ++i) {
    scaled_cov[i] = cov[i] / scale;
  }
  Scalar eigenvalues[3];
  computeRoots_v(scaled_cov,eigenvalues);
   *lambda3 = eigenvalues[0] * scale;
/* subtract the diagonal by eigenvalue[0] */
  scaled_cov[0] -= eigenvalues[0];
  scaled_cov[4] -= eigenvalues[0];
  scaled_cov[8] -= eigenvalues[0];
  Scalar vec1[3];
  Scalar vec2[3];
  Scalar vec3[3];
/* cross vector */
  cross_v(scaled_cov,(scaled_cov + 3),vec1);
  cross_v(scaled_cov,(scaled_cov + 6),vec2);
  cross_v((scaled_cov + 3),(scaled_cov + 6),vec3);
  Scalar len1 = squared_norm_v(vec1);
  Scalar len2 = squared_norm_v(vec2);
  Scalar len3 = squared_norm_v(vec3);
  if (len1 >= len2 && len1 >= len3) {
    div_v(vec1,(sqrt(len1)),v3);
  }
   else {
    if (len2 >= len1 && len2 >= len3) {
      div_v(vec2,(sqrt(len2)),v3);
    }
     else {
      div_v(vec3,(sqrt(len3)),v3);
    }
  }
}
/*********************************************************************************************************/
/* Input:
 * data is a buffer of length n*step (like a n by step matrix)
 * n is the number of total points
 * step is the number of elements per row
 * indices is of length m where each number is the row index of interest
 * origin is of length 3: indicating the location of sensor, the resulting normal has to face to the origin
 *
 * Output:
 * normal is of length 3: will store the result normal
 */

void compute_normal_vector_v_core(bus_data_type data[(90000 + 1 - 1) / 1],bus_data_type data1[(90000 + 1 - 1) / 1],int indices[540000],double origin_t[(540000 + 18 - 1) / 18 * 3],double normal_t[(540000 + 18 - 1) / 18 * 3])
{
  int i;
  int j;
  int step;
  int next_idx_0;
  Scalar ptr1_0;
  Scalar ptr2_0;
  Scalar ptr3_0;
  int start_0;
  int sub_start_0;
  int j0_0;
  int n_0;
  Scalar accu_0[9][1000];
  Scalar accu1_0[9][1000];
  int sub_start0_0;
  int sub_start1_0;
  int new_start0_0;
  int new_start1_0;
  int idx_step_0;
  int idx_step1_0;
  Scalar x_0;
  Scalar y_0;
  Scalar z_0;
  Scalar x1_0;
  Scalar y1_0;
  Scalar z1_0;
  int new_start_0;
  double accu_local_0[9];
  double cov_0[9];
  double v3_0[3];
  double lambda3_0;
  Scalar scale_0;
  Scalar abs_value_0;
  Scalar scaled_cov_0[9];
  Scalar eigenvalues_0[3];
  Scalar c0_0;
  Scalar c1_0;
  Scalar c2_0;
  Scalar d_0;
  Scalar sd_0;
  const Scalar s_inv3_0 = 1.0 / 3.0;
  const Scalar s_sqrt3_0 = sqrt(3.0);
  Scalar c2_over_3_0;
  Scalar a_over_3_0;
  Scalar half_b_0;
  Scalar q_0;
  Scalar rho_0;
  Scalar theta_0;
  Scalar cos_theta_0;
  Scalar sin_theta_0;
  Scalar d_1;
  Scalar sd_1;
  Scalar vec1_0[3];
  Scalar vec2_0[3];
  Scalar vec3_0[3];
  double len1_0;
  double len2_0;
  double len3_0;
  int len12_0;
  int len13_0;
  int len21_0;
  int len23_0;
  double d_2[3];
  int result_index_0;
  double dot_product_0;
  step = 3;
  ;
//printf("%d ", 0);
//bus_data_type data1[(BUFFER_LENGTH+bus_data_width-1)/bus_data_width];
//    double data1[BUFFER_LENGTH];
//#pragma HLS array_partition variable=data1 cyclic factor=24 dim=1
//    memcpy(data1, data, sizeof(double) * BUFFER_LENGTH); 
  next_idx_0 = indices[0];
  ptr1_0 = data[next_idx_0 * step];
  ptr2_0 = data[next_idx_0 * step + 1];
  ptr3_0 = data[next_idx_0 * step + 2];
#define TS    (10000)     // total 64KB
#define TS_10 (1000)     // total 64KB
#define TS_10_2 (500)     // total 64KB
  540000 % 18 == 0?((void )0) : __assert_fail("540000 % 18 == 0","normal_kernel.cpp",295,__PRETTY_FUNCTION__);
  540000 / 18 % 10000 == 0?((void )0) : __assert_fail("540000 / 18 % (10000) == 0","normal_kernel.cpp",295,__PRETTY_FUNCTION__);
  n_0 = 9;
  ;
//for (start=0; start<INDEX_LENGTH / TS ; start+=COMPUTE_LENGTH) 
//for (start=0; start<INDEX_LENGTH / TS /COMPUTE_LENGTH + 1; start++) 
//if ( new_start*COMPUTE_LENGTH >= INDEX_LENGTH) continue;
  for (start_0 = 0; start_0 < 540000 / 10000 / 18; start_0++) {
    for (j0_0 = 0; j0_0 < 10; j0_0++) 
// Original: #pragma ACCEL pipeline
{
      
#pragma ACCEL PIPELINE II=1
//HEAVY_LOOP:  
      for (i = 0; i < 18; ++i) {
//#pragma ACCEL pipeline
        for (sub_start_0 = 0; sub_start_0 < 500; sub_start_0++) {
          sub_start0_0 = sub_start_0 * 2 + 0;
          sub_start1_0 = sub_start_0 * 2 + 1;
          new_start0_0 = start_0 * 10000 + sub_start0_0 + j0_0 * 1000;
          new_start1_0 = start_0 * 10000 + sub_start1_0 + j0_0 * 1000;
          idx_step_0 = indices[new_start0_0 * 18 + i] * step;
          idx_step1_0 = indices[new_start1_0 * 18 + i] * step;
          idx_step_0 < 90000?((void )0) : __assert_fail("idx_step < 90000","normal_kernel.cpp",327,__PRETTY_FUNCTION__);
          idx_step1_0 < 90000?((void )0) : __assert_fail("idx_step1 < 90000","normal_kernel.cpp",328,__PRETTY_FUNCTION__);
          x_0 = data[idx_step_0 + 0];
          y_0 = data[idx_step_0 + 1];
          z_0 = data[idx_step_0 + 2];
          x1_0 = data1[idx_step1_0 + 0];
          y1_0 = data1[idx_step1_0 + 1];
          z1_0 = data1[idx_step1_0 + 2];
//if (1) // possible transformation
//{
//    int idx_step_div = idx_step/4;
//    int idx_step_rem = idx_step%4;
//    if (idx_step_rem == 0) {
//        x = data_2_0_buf[4*idx_step_div+0];
//        y = data_2_0_buf[4*idx_step_div+1];
//        z = data_2_0_buf[4*idx_step_div+2];
//    } else if (idx_step_rem == 1) {
//        x = data_2_0_buf[4*idx_step_div+1];
//        y = data_2_0_buf[4*idx_step_div+2];
//        z = data_2_0_buf[4*idx_step_div+3];
//    } else if (idx_step_rem == 2) {
//        x = data_2_0_buf[4*idx_step_div+2];
//        y = data_2_0_buf[4*idx_step_div+3];
//        z = data_2_0_buf[4*idx_step_div+4];
//    } else {
//        x = data_2_0_buf[4*idx_step_div+3];
//        y = data_2_0_buf[4*idx_step_div+4];
//        z = data_2_0_buf[4*idx_step_div+5];
//    }
//}
          if (i == 0) {
//8cycles
            accu_0[0][sub_start0_0] = x_0 * x_0;
            accu_0[1][sub_start0_0] = x_0 * y_0;
            accu_0[2][sub_start0_0] = x_0 * z_0;
            accu_0[3][sub_start0_0] = y_0 * y_0;
            accu_0[4][sub_start0_0] = y_0 * z_0;
            accu_0[5][sub_start0_0] = z_0 * z_0;
            accu_0[6][sub_start0_0] = x_0;
            accu_0[7][sub_start0_0] = y_0;
            accu_0[8][sub_start0_0] = z_0;
          }
           else {
//8cycles
            accu_0[0][sub_start0_0] += x_0 * x_0;
            accu_0[1][sub_start0_0] += x_0 * y_0;
            accu_0[2][sub_start0_0] += x_0 * z_0;
            accu_0[3][sub_start0_0] += y_0 * y_0;
            accu_0[4][sub_start0_0] += y_0 * z_0;
            accu_0[5][sub_start0_0] += z_0 * z_0;
            accu_0[6][sub_start0_0] += x_0;
            accu_0[7][sub_start0_0] += y_0;
            accu_0[8][sub_start0_0] += z_0;
          }
          if (i == 0) {
//8cycles
            accu_0[0][sub_start1_0] = x1_0 * x1_0;
            accu_0[1][sub_start1_0] = x1_0 * y1_0;
            accu_0[2][sub_start1_0] = x1_0 * z1_0;
            accu_0[3][sub_start1_0] = y1_0 * y1_0;
            accu_0[4][sub_start1_0] = y1_0 * z1_0;
            accu_0[5][sub_start1_0] = z1_0 * z1_0;
            accu_0[6][sub_start1_0] = x1_0;
            accu_0[7][sub_start1_0] = y1_0;
            accu_0[8][sub_start1_0] = z1_0;
          }
           else {
//8cycles
            accu_0[0][sub_start1_0] += x1_0 * x1_0;
            accu_0[1][sub_start1_0] += x1_0 * y1_0;
            accu_0[2][sub_start1_0] += x1_0 * z1_0;
            accu_0[3][sub_start1_0] += y1_0 * y1_0;
            accu_0[4][sub_start1_0] += y1_0 * z1_0;
            accu_0[5][sub_start1_0] += z1_0 * z1_0;
            accu_0[6][sub_start1_0] += x1_0;
            accu_0[7][sub_start1_0] += y1_0;
            accu_0[8][sub_start1_0] += z1_0;
          }
        }
      }
//SLOW_LOOP:            
      for (sub_start_0 = 0; sub_start_0 < 1000; sub_start_0++) 
// Original: #pragma ACCEL pipeline flatten II=9
{
        
#pragma ACCEL PIPELINE II=9
        new_start_0 = start_0 * 10000 + sub_start_0 + j0_0 * 1000;
        for (i = 0; i < n_0; ++i) {
          
#pragma ACCEL PARALLEL COMPLETE
          accu_local_0[i] = accu_0[i][sub_start_0] / 18;
        }
//update_normal_t(accu, origin_t, normal_t);
//ORIGINAL end
{
          cov_0[0] = accu_local_0[0] - accu_local_0[6] * accu_local_0[6];
          cov_0[1] = accu_local_0[1] - accu_local_0[6] * accu_local_0[7];
          cov_0[2] = accu_local_0[2] - accu_local_0[6] * accu_local_0[8];
          cov_0[4] = accu_local_0[3] - accu_local_0[7] * accu_local_0[7];
          cov_0[5] = accu_local_0[4] - accu_local_0[7] * accu_local_0[8];
          cov_0[8] = accu_local_0[5] - accu_local_0[8] * accu_local_0[8];
          cov_0[3] = cov_0[1];
          cov_0[6] = cov_0[2];
          cov_0[7] = cov_0[5];
//for (int j = 0; j < n; j++) {
//  printf("HW======new_start = %d, cov[%d] = %e\n", new_start, j, cov[j]);
//}
////end
// find the eigenvector of the smallest eigenvalue
////   eigen33_v<Scalar>(cov, v3, &lambda3);
////new_start:
// solve for plane equation 
// Scale the matrix so its entries are in [-1,1].  The scaling is applied
// only when at least one matrix entry has magnitude larger than 1.
          scale_0 = 0.0;
          for (i = 0; i < n_0; ++i) {
            
#pragma ACCEL PARALLEL COMPLETE
            abs_value_0 = fabs(cov_0[i]);
            if (scale_0 < abs_value_0) {
              scale_0 = abs_value_0;
            }
          }
/*should scale * scale*/
          if (scale_0 < 1.0) {
/*should scale * scale*/
            scale_0 = 1.0;
          }
          for (i = 0; i < n_0; ++i) {
            
#pragma ACCEL PARALLEL COMPLETE
            scaled_cov_0[i] = cov_0[i] / scale_0;
          }
          c0_0 = scaled_cov_0[0] * scaled_cov_0[4] * scaled_cov_0[8] + ((Scalar )2) * scaled_cov_0[1] * scaled_cov_0[2] * scaled_cov_0[5] - scaled_cov_0[0] * scaled_cov_0[5] * scaled_cov_0[5] - scaled_cov_0[4] * scaled_cov_0[2] * scaled_cov_0[2] - scaled_cov_0[8] * scaled_cov_0[1] * scaled_cov_0[1];
          c1_0 = scaled_cov_0[0] * scaled_cov_0[4] - scaled_cov_0[1] * scaled_cov_0[1] + scaled_cov_0[0] * scaled_cov_0[8] - scaled_cov_0[2] * scaled_cov_0[2] + scaled_cov_0[4] * scaled_cov_0[8] - scaled_cov_0[5] * scaled_cov_0[5];
          c2_0 = scaled_cov_0[0] + scaled_cov_0[4] + scaled_cov_0[8];
// one root is 0 -> quadratic equation
          if (fabs(c0_0) < epsilon) {
////computeRoots2_v (c2, c1, eigenvalues);
////new_start:
            eigenvalues_0[0] = ((Scalar )0);
            d_0 = ((Scalar )(c2_0 * c2_0 - 4.0 * c1_0));
// no real eigenvalues!!!! THIS SHOULD NOT HAPPEN!
            if (d_0 < 0.0) {
              d_0 = 0.0;
            }
            sd_0 = sqrt(d_0);
            eigenvalues_0[2] = ((Scalar )0.5) * (c2_0 + sd_0);
            eigenvalues_0[1] = ((Scalar )0.5) * (c2_0 - sd_0);
////end
          }
           else {
            c2_over_3_0 = c2_0 * s_inv3_0;
            a_over_3_0 = (c1_0 - c2_0 * c2_over_3_0) * s_inv3_0;
            if (a_over_3_0 > 0.0) {
              a_over_3_0 = ((Scalar )0.0);
            }
            half_b_0 = ((Scalar )0.5) * (c0_0 + c2_over_3_0 * (((Scalar )2) * c2_over_3_0 * c2_over_3_0 - c1_0));
            q_0 = half_b_0 * half_b_0 + a_over_3_0 * a_over_3_0 * a_over_3_0;
            if (q_0 > ((Scalar )0)) {
              q_0 = ((Scalar )0);
            }
// Compute the eigenvalues by solving for the eigenvalues of the polynomial.
            rho_0 = sqrt(-a_over_3_0);
            theta_0 = atan2((sqrt(-q_0)),half_b_0) * s_inv3_0;
            cos_theta_0 = cos(theta_0);
            sin_theta_0 = sin(theta_0);
            eigenvalues_0[0] = c2_over_3_0 + 2.0 * rho_0 * cos_theta_0;
            eigenvalues_0[1] = c2_over_3_0 - rho_0 * (cos_theta_0 + s_sqrt3_0 * sin_theta_0);
            eigenvalues_0[2] = c2_over_3_0 - rho_0 * (cos_theta_0 - s_sqrt3_0 * sin_theta_0);
// Sort in increasing order.
            if (eigenvalues_0[0] >= eigenvalues_0[1]) {
              swap(&eigenvalues_0[0],&eigenvalues_0[1]);
            }
            if (eigenvalues_0[1] >= eigenvalues_0[2]) {
              swap(&eigenvalues_0[1],&eigenvalues_0[2]);
              if (eigenvalues_0[0] >= eigenvalues_0[1]) {
                swap(&eigenvalues_0[0],&eigenvalues_0[1]);
              }
            }
// eigenval for symetric positive semi-definite matrix can not be negative! Set it to 0
            if (eigenvalues_0[0] <= 0) {
////computeRoots2_v (c2, c1, eigenvalues);
////new_start:
              eigenvalues_0[0] = 0.0;
              d_1 = ((Scalar )(c2_0 * c2_0 - 4.0 * c1_0));
// no real eigenvalues!!!! THIS SHOULD NOT HAPPEN!
              if (d_1 < 0.0) {
                d_1 = 0.0;
              }
              sd_1 = sqrt(d_1);
              eigenvalues_0[2] = ((Scalar )0.5) * (c2_0 + sd_1);
              eigenvalues_0[1] = ((Scalar )0.5) * (c2_0 - sd_1);
////end
            }
          }
// Construct the parameters used in classifying the eigenvalues of the equation
// and in solving the equation for the eigenvalues in closed form.
////end: computeRoots_c
          lambda3_0 = eigenvalues_0[0] * scale_0;
//subtract the diagonal by eigenvalue[0] 
          scaled_cov_0[0] -= eigenvalues_0[0];
          scaled_cov_0[4] -= eigenvalues_0[0];
          scaled_cov_0[8] -= eigenvalues_0[0];
// cross vector 
          cross_v(scaled_cov_0,(scaled_cov_0 + 3),vec1_0);
          cross_v(scaled_cov_0,(scaled_cov_0 + 6),vec2_0);
          cross_v((scaled_cov_0 + 3),(scaled_cov_0 + 6),vec3_0);
          len1_0 = squared_norm_v(vec1_0);
          len2_0 = squared_norm_v(vec2_0);
          len3_0 = squared_norm_v(vec3_0);
          len12_0 = ((int )(len1_0 >= len2_0));
          len13_0 = ((int )(len1_0 >= len3_0));
          len21_0 = ((int )(len2_0 >= len1_0));
          len23_0 = ((int )(len2_0 >= len3_0));
          if (len12_0 && len13_0) {
            div_v(vec1_0,(sqrt(len1_0)),v3_0);
          }
           else {
            if (len21_0 && len23_0) {
              div_v(vec2_0,(sqrt(len2_0)),v3_0);
            }
             else {
              div_v(vec3_0,(sqrt(len3_0)),v3_0);
            }
          }
////end: eigen33_c
//int result_index = new_start;
//fixed bug155
//start * TS + sub_start;
          result_index_0 = new_start_0;
//int result_index = start * TS + sub_start + j0*TS_10;
////   const Scalar *ptr1 = data + indices[0]*step;
          d_2[0] = origin_t[result_index_0 * 3 + 0] - ptr1_0;
          d_2[1] = origin_t[result_index_0 * 3 + 1] - ptr2_0;
          d_2[2] = origin_t[result_index_0 * 3 + 2] - ptr3_0;
          dot_product_0 = v3_0[0] * d_2[0] + v3_0[1] * d_2[1] + v3_0[2] * d_2[2];
          if (dot_product_0 >= 0.0) {
            normal_t[result_index_0 * 3 + 0] = v3_0[0];
            normal_t[result_index_0 * 3 + 1] = v3_0[1];
            normal_t[result_index_0 * 3 + 2] = v3_0[2];
          }
           else {
            normal_t[result_index_0 * 3 + 0] = -v3_0[0];
            normal_t[result_index_0 * 3 + 1] = -v3_0[1];
            normal_t[result_index_0 * 3 + 2] = -v3_0[2];
          }
        }
      }
    }
  }
//    memcpy(normal_t,  normal,  sizeof(double)*SAFE_LENGTH *NORMAL_LENGTH);
}
//void compute_normal_vector_v_kernel(
//        bus_data_type data[(BUFFER_LENGTH+bus_data_width-1)/bus_data_width],
//        int indices_t[INDEX_LENGTH],
//        double origin_t[SAFE_LENGTH *NORMAL_LENGTH],
//        double normal_t[SAFE_LENGTH *NORMAL_LENGTH])
//void compute_normal_vector_v_kernel(

#pragma ACCEL kernel
void normal_kernel(bus_data_type *data,bus_data_type *data1,int *indices_t,double *origin_t,double *normal_t)
{
  int _memcpy_i2_3;
  int _memcpy_i2_2;
  int _memcpy_i2_1;
  int _memcpy_i2_0;
  int _memcpy_i1_2;
  int _memcpy_i1_1;
  int _memcpy_i1_0;
  int _memcpy_i0_3;
  int _memcpy_i0_2;
  int _memcpy_i0_1;
  int _memcpy_i0_0;
  double normal_t_buf_0[3][10][1000][3];
  double origin_t_buf_0[3][10][1000][3];
  int i_0;
  int j_0;
  int i__compute_normal_vector_v_core_0;
  int j__compute_normal_vector_v_core_0;
  int step__compute_normal_vector_v_core_0;
  int next_idx_0__compute_normal_vector_v_core_0;
  Scalar ptr1_0__compute_normal_vector_v_core_0;
  Scalar ptr2_0__compute_normal_vector_v_core_0;
  Scalar ptr3_0__compute_normal_vector_v_core_0;
  int start_0__compute_normal_vector_v_core_0;
  int sub_start_0__compute_normal_vector_v_core_0;
  int j0_0__compute_normal_vector_v_core_0;
  int n_0__compute_normal_vector_v_core_0;
  Scalar accu_0__compute_normal_vector_v_core_0[9][1000];
  Scalar accu1_0__compute_normal_vector_v_core_0[9][1000];
  int sub_start0_0__compute_normal_vector_v_core_0;
  int sub_start1_0__compute_normal_vector_v_core_0;
  int new_start0_0__compute_normal_vector_v_core_0;
  int new_start1_0__compute_normal_vector_v_core_0;
  int idx_step_0__compute_normal_vector_v_core_0;
  int idx_step1_0__compute_normal_vector_v_core_0;
  Scalar x_0__compute_normal_vector_v_core_0;
  Scalar y_0__compute_normal_vector_v_core_0;
  Scalar z_0__compute_normal_vector_v_core_0;
  Scalar x1_0__compute_normal_vector_v_core_0;
  Scalar y1_0__compute_normal_vector_v_core_0;
  Scalar z1_0__compute_normal_vector_v_core_0;
  int new_start_0__compute_normal_vector_v_core_0;
  double accu_local_0__compute_normal_vector_v_core_0[9];
  double cov_0__compute_normal_vector_v_core_0[9];
  double v3_0__compute_normal_vector_v_core_0[3];
  double lambda3_0__compute_normal_vector_v_core_0;
  Scalar scale_0__compute_normal_vector_v_core_0;
  Scalar abs_value_0__compute_normal_vector_v_core_0;
  Scalar scaled_cov_0__compute_normal_vector_v_core_0[9];
  Scalar eigenvalues_0__compute_normal_vector_v_core_0[3];
  Scalar c0_0__compute_normal_vector_v_core_0;
  Scalar c1_0__compute_normal_vector_v_core_0;
  Scalar c2_0__compute_normal_vector_v_core_0;
  Scalar d_0__compute_normal_vector_v_core_0;
  Scalar sd_0__compute_normal_vector_v_core_0;
  const Scalar s_inv3_0_0 = 1.0 / 3.0;
  const Scalar s_sqrt3_0_0 = sqrt(3.0);
  Scalar c2_over_3_0__compute_normal_vector_v_core_0;
  Scalar a_over_3_0__compute_normal_vector_v_core_0;
  Scalar half_b_0__compute_normal_vector_v_core_0;
  Scalar q_0__compute_normal_vector_v_core_0;
  Scalar rho_0__compute_normal_vector_v_core_0;
  Scalar theta_0__compute_normal_vector_v_core_0;
  Scalar cos_theta_0__compute_normal_vector_v_core_0;
  Scalar sin_theta_0__compute_normal_vector_v_core_0;
  Scalar d_1__compute_normal_vector_v_core_0;
  Scalar sd_1__compute_normal_vector_v_core_0;
  Scalar vec1_0__compute_normal_vector_v_core_0[3];
  Scalar vec2_0__compute_normal_vector_v_core_0[3];
  Scalar vec3_0__compute_normal_vector_v_core_0[3];
  double len1_0__compute_normal_vector_v_core_0;
  double len2_0__compute_normal_vector_v_core_0;
  double len3_0__compute_normal_vector_v_core_0;
  int len12_0__compute_normal_vector_v_core_0;
  int len13_0__compute_normal_vector_v_core_0;
  int len21_0__compute_normal_vector_v_core_0;
  int len23_0__compute_normal_vector_v_core_0;
  double d_2__compute_normal_vector_v_core_0[3];
  int result_index_0__compute_normal_vector_v_core_0;
  double dot_product_0__compute_normal_vector_v_core_0;
  int indices_t_buf_0_0[10][500][36];
  double tmp__swap_2;
  double tmp__swap_0_0;
  double tmp__swap_1_0;
  Scalar return_0_squared_norm_v_0;
  Scalar return_1_squared_norm_v_0;
  Scalar return_2_squared_norm_v_0;
  Scalar a__div_v_2;
  Scalar a__div_v_0_0;
  Scalar a__div_v_1_0;
  for (_memcpy_i0_3 = 0; _memcpy_i0_3 < 3; ++_memcpy_i0_3) {
    for (_memcpy_i0_2 = 0; _memcpy_i0_2 < 10; ++_memcpy_i0_2) {
      for (_memcpy_i0_1 = 0; _memcpy_i0_1 < 1000; ++_memcpy_i0_1) {
        for (_memcpy_i0_0 = 0; _memcpy_i0_0 < 3; ++_memcpy_i0_0) {
          origin_t_buf_0[_memcpy_i0_3][_memcpy_i0_2][_memcpy_i0_1][_memcpy_i0_0] = origin_t[0 + ((((0 * 3 + _memcpy_i0_3) * 1000 + _memcpy_i0_2) * 10 + _memcpy_i0_1) * 3 + _memcpy_i0_0)];
        }
      }
    }
  }
{
    step__compute_normal_vector_v_core_0 = 3;
    ;
//printf("%d ", 0);
//bus_data_type data1[(BUFFER_LENGTH+bus_data_width-1)/bus_data_width];
//    double data1[BUFFER_LENGTH];
//#pragma HLS array_partition variable=data1 cyclic factor=24 dim=1
//    memcpy(data1, data, sizeof(double) * BUFFER_LENGTH); 
    next_idx_0__compute_normal_vector_v_core_0 = indices_t[0];
    ptr1_0__compute_normal_vector_v_core_0 = data[next_idx_0__compute_normal_vector_v_core_0 * step__compute_normal_vector_v_core_0];
    ptr2_0__compute_normal_vector_v_core_0 = data[next_idx_0__compute_normal_vector_v_core_0 * step__compute_normal_vector_v_core_0 + 1];
    ptr3_0__compute_normal_vector_v_core_0 = data[next_idx_0__compute_normal_vector_v_core_0 * step__compute_normal_vector_v_core_0 + 2];
#define TS    (10000)     // total 64KB
#define TS_10 (1000)     // total 64KB
#define TS_10_2 (500)     // total 64KB
    540000 % 18 == 0?((void )0) : __assert_fail("540000 % 18 == 0","normal_kernel.cpp",((unsigned int )295),__PRETTY_FUNCTION__);
    540000 / 18 % 10000 == 0?((void )0) : __assert_fail("540000 / 18 % (10000) == 0","normal_kernel.cpp",((unsigned int )295),__PRETTY_FUNCTION__);
    n_0__compute_normal_vector_v_core_0 = 9;
    ;
//for (start=0; start<INDEX_LENGTH / TS ; start+=COMPUTE_LENGTH) 
//for (start=0; start<INDEX_LENGTH / TS /COMPUTE_LENGTH + 1; start++) 
//if ( new_start*COMPUTE_LENGTH >= INDEX_LENGTH) continue;
    for (start_0__compute_normal_vector_v_core_0 = 0; start_0__compute_normal_vector_v_core_0 < 540000 / 10000 / 18; start_0__compute_normal_vector_v_core_0++) {
      for (_memcpy_i1_2 = 0; _memcpy_i1_2 < 10; ++_memcpy_i1_2) {
        for (_memcpy_i1_1 = 0; _memcpy_i1_1 < 500; ++_memcpy_i1_1) {
          for (_memcpy_i1_0 = 0; _memcpy_i1_0 < 36; ++_memcpy_i1_0) {
            indices_t_buf_0_0[_memcpy_i1_2][_memcpy_i1_1][_memcpy_i1_0] = indices_t[start_0__compute_normal_vector_v_core_0 * 180000 + (((0 * 36 + _memcpy_i1_2) * 500 + _memcpy_i1_1) * 10 + _memcpy_i1_0)];
          }
        }
      }
      for (j0_0__compute_normal_vector_v_core_0 = 0; j0_0__compute_normal_vector_v_core_0 < 10; j0_0__compute_normal_vector_v_core_0++) 
// Original: #pragma ACCEL pipeline
{
        
#pragma ACCEL PIPELINE II=1
//HEAVY_LOOP:  
        for (i__compute_normal_vector_v_core_0 = 0; i__compute_normal_vector_v_core_0 < 18; ++i__compute_normal_vector_v_core_0) {
//#pragma ACCEL pipeline
          for (sub_start_0__compute_normal_vector_v_core_0 = 0; sub_start_0__compute_normal_vector_v_core_0 < 500; sub_start_0__compute_normal_vector_v_core_0++) {
            sub_start0_0__compute_normal_vector_v_core_0 = sub_start_0__compute_normal_vector_v_core_0 * 2 + 0;
            sub_start1_0__compute_normal_vector_v_core_0 = sub_start_0__compute_normal_vector_v_core_0 * 2 + 1;
            new_start0_0__compute_normal_vector_v_core_0 = start_0__compute_normal_vector_v_core_0 * 10000 + sub_start0_0__compute_normal_vector_v_core_0 + j0_0__compute_normal_vector_v_core_0 * 1000;
            new_start1_0__compute_normal_vector_v_core_0 = start_0__compute_normal_vector_v_core_0 * 10000 + sub_start1_0__compute_normal_vector_v_core_0 + j0_0__compute_normal_vector_v_core_0 * 1000;
            idx_step_0__compute_normal_vector_v_core_0 = indices_t_buf_0_0[j0_0__compute_normal_vector_v_core_0][sub_start_0__compute_normal_vector_v_core_0][i__compute_normal_vector_v_core_0] * 3;
            idx_step1_0__compute_normal_vector_v_core_0 = indices_t_buf_0_0[j0_0__compute_normal_vector_v_core_0][sub_start_0__compute_normal_vector_v_core_0][18 + i__compute_normal_vector_v_core_0] * 3;
            idx_step_0__compute_normal_vector_v_core_0 < 90000?((void )0) : __assert_fail("idx_step < 90000","normal_kernel.cpp",((unsigned int )327),__PRETTY_FUNCTION__);
            idx_step1_0__compute_normal_vector_v_core_0 < 90000?((void )0) : __assert_fail("idx_step1 < 90000","normal_kernel.cpp",((unsigned int )328),__PRETTY_FUNCTION__);
            x_0__compute_normal_vector_v_core_0 = data[idx_step_0__compute_normal_vector_v_core_0 + 0];
            y_0__compute_normal_vector_v_core_0 = data[idx_step_0__compute_normal_vector_v_core_0 + 1];
            z_0__compute_normal_vector_v_core_0 = data[idx_step_0__compute_normal_vector_v_core_0 + 2];
            x1_0__compute_normal_vector_v_core_0 = data1[idx_step1_0__compute_normal_vector_v_core_0 + 0];
            y1_0__compute_normal_vector_v_core_0 = data1[idx_step1_0__compute_normal_vector_v_core_0 + 1];
            z1_0__compute_normal_vector_v_core_0 = data1[idx_step1_0__compute_normal_vector_v_core_0 + 2];
//if (1) // possible transformation
//{
//    int idx_step_div = idx_step/4;
//    int idx_step_rem = idx_step%4;
//    if (idx_step_rem == 0) {
//        x = data_2_0_buf[4*idx_step_div+0];
//        y = data_2_0_buf[4*idx_step_div+1];
//        z = data_2_0_buf[4*idx_step_div+2];
//    } else if (idx_step_rem == 1) {
//        x = data_2_0_buf[4*idx_step_div+1];
//        y = data_2_0_buf[4*idx_step_div+2];
//        z = data_2_0_buf[4*idx_step_div+3];
//    } else if (idx_step_rem == 2) {
//        x = data_2_0_buf[4*idx_step_div+2];
//        y = data_2_0_buf[4*idx_step_div+3];
//        z = data_2_0_buf[4*idx_step_div+4];
//    } else {
//        x = data_2_0_buf[4*idx_step_div+3];
//        y = data_2_0_buf[4*idx_step_div+4];
//        z = data_2_0_buf[4*idx_step_div+5];
//    }
//}
            if (i__compute_normal_vector_v_core_0 == 0) {
//8cycles
              accu_0__compute_normal_vector_v_core_0[0][sub_start0_0__compute_normal_vector_v_core_0] = x_0__compute_normal_vector_v_core_0 * x_0__compute_normal_vector_v_core_0;
              accu_0__compute_normal_vector_v_core_0[1][sub_start0_0__compute_normal_vector_v_core_0] = x_0__compute_normal_vector_v_core_0 * y_0__compute_normal_vector_v_core_0;
              accu_0__compute_normal_vector_v_core_0[2][sub_start0_0__compute_normal_vector_v_core_0] = x_0__compute_normal_vector_v_core_0 * z_0__compute_normal_vector_v_core_0;
              accu_0__compute_normal_vector_v_core_0[3][sub_start0_0__compute_normal_vector_v_core_0] = y_0__compute_normal_vector_v_core_0 * y_0__compute_normal_vector_v_core_0;
              accu_0__compute_normal_vector_v_core_0[4][sub_start0_0__compute_normal_vector_v_core_0] = y_0__compute_normal_vector_v_core_0 * z_0__compute_normal_vector_v_core_0;
              accu_0__compute_normal_vector_v_core_0[5][sub_start0_0__compute_normal_vector_v_core_0] = z_0__compute_normal_vector_v_core_0 * z_0__compute_normal_vector_v_core_0;
              accu_0__compute_normal_vector_v_core_0[6][sub_start0_0__compute_normal_vector_v_core_0] = x_0__compute_normal_vector_v_core_0;
              accu_0__compute_normal_vector_v_core_0[7][sub_start0_0__compute_normal_vector_v_core_0] = y_0__compute_normal_vector_v_core_0;
              accu_0__compute_normal_vector_v_core_0[8][sub_start0_0__compute_normal_vector_v_core_0] = z_0__compute_normal_vector_v_core_0;
            }
             else {
//8cycles
              accu_0__compute_normal_vector_v_core_0[0][sub_start0_0__compute_normal_vector_v_core_0] += x_0__compute_normal_vector_v_core_0 * x_0__compute_normal_vector_v_core_0;
              accu_0__compute_normal_vector_v_core_0[1][sub_start0_0__compute_normal_vector_v_core_0] += x_0__compute_normal_vector_v_core_0 * y_0__compute_normal_vector_v_core_0;
              accu_0__compute_normal_vector_v_core_0[2][sub_start0_0__compute_normal_vector_v_core_0] += x_0__compute_normal_vector_v_core_0 * z_0__compute_normal_vector_v_core_0;
              accu_0__compute_normal_vector_v_core_0[3][sub_start0_0__compute_normal_vector_v_core_0] += y_0__compute_normal_vector_v_core_0 * y_0__compute_normal_vector_v_core_0;
              accu_0__compute_normal_vector_v_core_0[4][sub_start0_0__compute_normal_vector_v_core_0] += y_0__compute_normal_vector_v_core_0 * z_0__compute_normal_vector_v_core_0;
              accu_0__compute_normal_vector_v_core_0[5][sub_start0_0__compute_normal_vector_v_core_0] += z_0__compute_normal_vector_v_core_0 * z_0__compute_normal_vector_v_core_0;
              accu_0__compute_normal_vector_v_core_0[6][sub_start0_0__compute_normal_vector_v_core_0] += x_0__compute_normal_vector_v_core_0;
              accu_0__compute_normal_vector_v_core_0[7][sub_start0_0__compute_normal_vector_v_core_0] += y_0__compute_normal_vector_v_core_0;
              accu_0__compute_normal_vector_v_core_0[8][sub_start0_0__compute_normal_vector_v_core_0] += z_0__compute_normal_vector_v_core_0;
            }
            if (i__compute_normal_vector_v_core_0 == 0) {
//8cycles
              accu_0__compute_normal_vector_v_core_0[0][sub_start1_0__compute_normal_vector_v_core_0] = x1_0__compute_normal_vector_v_core_0 * x1_0__compute_normal_vector_v_core_0;
              accu_0__compute_normal_vector_v_core_0[1][sub_start1_0__compute_normal_vector_v_core_0] = x1_0__compute_normal_vector_v_core_0 * y1_0__compute_normal_vector_v_core_0;
              accu_0__compute_normal_vector_v_core_0[2][sub_start1_0__compute_normal_vector_v_core_0] = x1_0__compute_normal_vector_v_core_0 * z1_0__compute_normal_vector_v_core_0;
              accu_0__compute_normal_vector_v_core_0[3][sub_start1_0__compute_normal_vector_v_core_0] = y1_0__compute_normal_vector_v_core_0 * y1_0__compute_normal_vector_v_core_0;
              accu_0__compute_normal_vector_v_core_0[4][sub_start1_0__compute_normal_vector_v_core_0] = y1_0__compute_normal_vector_v_core_0 * z1_0__compute_normal_vector_v_core_0;
              accu_0__compute_normal_vector_v_core_0[5][sub_start1_0__compute_normal_vector_v_core_0] = z1_0__compute_normal_vector_v_core_0 * z1_0__compute_normal_vector_v_core_0;
              accu_0__compute_normal_vector_v_core_0[6][sub_start1_0__compute_normal_vector_v_core_0] = x1_0__compute_normal_vector_v_core_0;
              accu_0__compute_normal_vector_v_core_0[7][sub_start1_0__compute_normal_vector_v_core_0] = y1_0__compute_normal_vector_v_core_0;
              accu_0__compute_normal_vector_v_core_0[8][sub_start1_0__compute_normal_vector_v_core_0] = z1_0__compute_normal_vector_v_core_0;
            }
             else {
//8cycles
              accu_0__compute_normal_vector_v_core_0[0][sub_start1_0__compute_normal_vector_v_core_0] += x1_0__compute_normal_vector_v_core_0 * x1_0__compute_normal_vector_v_core_0;
              accu_0__compute_normal_vector_v_core_0[1][sub_start1_0__compute_normal_vector_v_core_0] += x1_0__compute_normal_vector_v_core_0 * y1_0__compute_normal_vector_v_core_0;
              accu_0__compute_normal_vector_v_core_0[2][sub_start1_0__compute_normal_vector_v_core_0] += x1_0__compute_normal_vector_v_core_0 * z1_0__compute_normal_vector_v_core_0;
              accu_0__compute_normal_vector_v_core_0[3][sub_start1_0__compute_normal_vector_v_core_0] += y1_0__compute_normal_vector_v_core_0 * y1_0__compute_normal_vector_v_core_0;
              accu_0__compute_normal_vector_v_core_0[4][sub_start1_0__compute_normal_vector_v_core_0] += y1_0__compute_normal_vector_v_core_0 * z1_0__compute_normal_vector_v_core_0;
              accu_0__compute_normal_vector_v_core_0[5][sub_start1_0__compute_normal_vector_v_core_0] += z1_0__compute_normal_vector_v_core_0 * z1_0__compute_normal_vector_v_core_0;
              accu_0__compute_normal_vector_v_core_0[6][sub_start1_0__compute_normal_vector_v_core_0] += x1_0__compute_normal_vector_v_core_0;
              accu_0__compute_normal_vector_v_core_0[7][sub_start1_0__compute_normal_vector_v_core_0] += y1_0__compute_normal_vector_v_core_0;
              accu_0__compute_normal_vector_v_core_0[8][sub_start1_0__compute_normal_vector_v_core_0] += z1_0__compute_normal_vector_v_core_0;
            }
          }
        }
//SLOW_LOOP:            
        for (sub_start_0__compute_normal_vector_v_core_0 = 0; sub_start_0__compute_normal_vector_v_core_0 < 1000; sub_start_0__compute_normal_vector_v_core_0++) 
// Original: #pragma ACCEL pipeline flatten II=9
{
          
#pragma ACCEL PIPELINE II=9
          new_start_0__compute_normal_vector_v_core_0 = start_0__compute_normal_vector_v_core_0 * 10000 + sub_start_0__compute_normal_vector_v_core_0 + j0_0__compute_normal_vector_v_core_0 * 1000;
          for (i__compute_normal_vector_v_core_0 = 0; i__compute_normal_vector_v_core_0 < 9; ++i__compute_normal_vector_v_core_0) {
            
#pragma ACCEL PARALLEL COMPLETE
            accu_local_0__compute_normal_vector_v_core_0[i__compute_normal_vector_v_core_0] = accu_0__compute_normal_vector_v_core_0[i__compute_normal_vector_v_core_0][sub_start_0__compute_normal_vector_v_core_0] / ((double )18);
          }
//update_normal_t(accu, origin_t, normal_t);
//ORIGINAL end
{
            cov_0__compute_normal_vector_v_core_0[0] = accu_local_0__compute_normal_vector_v_core_0[0] - accu_local_0__compute_normal_vector_v_core_0[6] * accu_local_0__compute_normal_vector_v_core_0[6];
            cov_0__compute_normal_vector_v_core_0[1] = accu_local_0__compute_normal_vector_v_core_0[1] - accu_local_0__compute_normal_vector_v_core_0[6] * accu_local_0__compute_normal_vector_v_core_0[7];
            cov_0__compute_normal_vector_v_core_0[2] = accu_local_0__compute_normal_vector_v_core_0[2] - accu_local_0__compute_normal_vector_v_core_0[6] * accu_local_0__compute_normal_vector_v_core_0[8];
            cov_0__compute_normal_vector_v_core_0[4] = accu_local_0__compute_normal_vector_v_core_0[3] - accu_local_0__compute_normal_vector_v_core_0[7] * accu_local_0__compute_normal_vector_v_core_0[7];
            cov_0__compute_normal_vector_v_core_0[5] = accu_local_0__compute_normal_vector_v_core_0[4] - accu_local_0__compute_normal_vector_v_core_0[7] * accu_local_0__compute_normal_vector_v_core_0[8];
            cov_0__compute_normal_vector_v_core_0[8] = accu_local_0__compute_normal_vector_v_core_0[5] - accu_local_0__compute_normal_vector_v_core_0[8] * accu_local_0__compute_normal_vector_v_core_0[8];
            cov_0__compute_normal_vector_v_core_0[3] = cov_0__compute_normal_vector_v_core_0[1];
            cov_0__compute_normal_vector_v_core_0[6] = cov_0__compute_normal_vector_v_core_0[2];
            cov_0__compute_normal_vector_v_core_0[7] = cov_0__compute_normal_vector_v_core_0[5];
//for (int j = 0; j < n; j++) {
//  printf("HW======new_start = %d, cov[%d] = %e\n", new_start, j, cov[j]);
//}
////end
// find the eigenvector of the smallest eigenvalue
////   eigen33_v<Scalar>(cov, v3, &lambda3);
////new_start:
// solve for plane equation 
// Scale the matrix so its entries are in [-1,1].  The scaling is applied
// only when at least one matrix entry has magnitude larger than 1.
            scale_0__compute_normal_vector_v_core_0 = 0.0;
            for (i__compute_normal_vector_v_core_0 = 0; i__compute_normal_vector_v_core_0 < 9; ++i__compute_normal_vector_v_core_0) {
              
#pragma ACCEL PARALLEL COMPLETE
              abs_value_0__compute_normal_vector_v_core_0 = fabs(cov_0__compute_normal_vector_v_core_0[i__compute_normal_vector_v_core_0]);
              if (scale_0__compute_normal_vector_v_core_0 < abs_value_0__compute_normal_vector_v_core_0) {
                scale_0__compute_normal_vector_v_core_0 = abs_value_0__compute_normal_vector_v_core_0;
              }
            }
/*should scale * scale*/
            if (scale_0__compute_normal_vector_v_core_0 < 1.0) {
/*should scale * scale*/
              scale_0__compute_normal_vector_v_core_0 = 1.0;
            }
            for (i__compute_normal_vector_v_core_0 = 0; i__compute_normal_vector_v_core_0 < 9; ++i__compute_normal_vector_v_core_0) {
              
#pragma ACCEL PARALLEL COMPLETE
              scaled_cov_0__compute_normal_vector_v_core_0[i__compute_normal_vector_v_core_0] = cov_0__compute_normal_vector_v_core_0[i__compute_normal_vector_v_core_0] / scale_0__compute_normal_vector_v_core_0;
            }
            c0_0__compute_normal_vector_v_core_0 = scaled_cov_0__compute_normal_vector_v_core_0[0] * scaled_cov_0__compute_normal_vector_v_core_0[4] * scaled_cov_0__compute_normal_vector_v_core_0[8] + ((Scalar )2) * scaled_cov_0__compute_normal_vector_v_core_0[1] * scaled_cov_0__compute_normal_vector_v_core_0[2] * scaled_cov_0__compute_normal_vector_v_core_0[5] - scaled_cov_0__compute_normal_vector_v_core_0[0] * scaled_cov_0__compute_normal_vector_v_core_0[5] * scaled_cov_0__compute_normal_vector_v_core_0[5] - scaled_cov_0__compute_normal_vector_v_core_0[4] * scaled_cov_0__compute_normal_vector_v_core_0[2] * scaled_cov_0__compute_normal_vector_v_core_0[2] - scaled_cov_0__compute_normal_vector_v_core_0[8] * scaled_cov_0__compute_normal_vector_v_core_0[1] * scaled_cov_0__compute_normal_vector_v_core_0[1];
            c1_0__compute_normal_vector_v_core_0 = scaled_cov_0__compute_normal_vector_v_core_0[0] * scaled_cov_0__compute_normal_vector_v_core_0[4] - scaled_cov_0__compute_normal_vector_v_core_0[1] * scaled_cov_0__compute_normal_vector_v_core_0[1] + scaled_cov_0__compute_normal_vector_v_core_0[0] * scaled_cov_0__compute_normal_vector_v_core_0[8] - scaled_cov_0__compute_normal_vector_v_core_0[2] * scaled_cov_0__compute_normal_vector_v_core_0[2] + scaled_cov_0__compute_normal_vector_v_core_0[4] * scaled_cov_0__compute_normal_vector_v_core_0[8] - scaled_cov_0__compute_normal_vector_v_core_0[5] * scaled_cov_0__compute_normal_vector_v_core_0[5];
            c2_0__compute_normal_vector_v_core_0 = scaled_cov_0__compute_normal_vector_v_core_0[0] + scaled_cov_0__compute_normal_vector_v_core_0[4] + scaled_cov_0__compute_normal_vector_v_core_0[8];
// one root is 0 -> quadratic equation
            if (fabs(c0_0__compute_normal_vector_v_core_0) < epsilon) {
////computeRoots2_v (c2, c1, eigenvalues);
////new_start:
              eigenvalues_0__compute_normal_vector_v_core_0[0] = ((Scalar )0);
              d_0__compute_normal_vector_v_core_0 = ((Scalar )(c2_0__compute_normal_vector_v_core_0 * c2_0__compute_normal_vector_v_core_0 - 4.0 * c1_0__compute_normal_vector_v_core_0));
// no real eigenvalues!!!! THIS SHOULD NOT HAPPEN!
              if (d_0__compute_normal_vector_v_core_0 < 0.0) {
                d_0__compute_normal_vector_v_core_0 = 0.0;
              }
              sd_0__compute_normal_vector_v_core_0 = sqrt(d_0__compute_normal_vector_v_core_0);
              eigenvalues_0__compute_normal_vector_v_core_0[2] = ((Scalar )0.5) * (c2_0__compute_normal_vector_v_core_0 + sd_0__compute_normal_vector_v_core_0);
              eigenvalues_0__compute_normal_vector_v_core_0[1] = ((Scalar )0.5) * (c2_0__compute_normal_vector_v_core_0 - sd_0__compute_normal_vector_v_core_0);
////end
            }
             else {
              c2_over_3_0__compute_normal_vector_v_core_0 = c2_0__compute_normal_vector_v_core_0 * s_inv3_0_0;
              a_over_3_0__compute_normal_vector_v_core_0 = (c1_0__compute_normal_vector_v_core_0 - c2_0__compute_normal_vector_v_core_0 * c2_over_3_0__compute_normal_vector_v_core_0) * s_inv3_0_0;
              if (a_over_3_0__compute_normal_vector_v_core_0 > 0.0) {
                a_over_3_0__compute_normal_vector_v_core_0 = ((Scalar )0.0);
              }
              half_b_0__compute_normal_vector_v_core_0 = ((Scalar )0.5) * (c0_0__compute_normal_vector_v_core_0 + c2_over_3_0__compute_normal_vector_v_core_0 * (((Scalar )2) * c2_over_3_0__compute_normal_vector_v_core_0 * c2_over_3_0__compute_normal_vector_v_core_0 - c1_0__compute_normal_vector_v_core_0));
              q_0__compute_normal_vector_v_core_0 = half_b_0__compute_normal_vector_v_core_0 * half_b_0__compute_normal_vector_v_core_0 + a_over_3_0__compute_normal_vector_v_core_0 * a_over_3_0__compute_normal_vector_v_core_0 * a_over_3_0__compute_normal_vector_v_core_0;
              if (q_0__compute_normal_vector_v_core_0 > ((Scalar )0)) {
                q_0__compute_normal_vector_v_core_0 = ((Scalar )0);
              }
// Compute the eigenvalues by solving for the eigenvalues of the polynomial.
              rho_0__compute_normal_vector_v_core_0 = sqrt(-a_over_3_0__compute_normal_vector_v_core_0);
              theta_0__compute_normal_vector_v_core_0 = atan2((sqrt(-q_0__compute_normal_vector_v_core_0)),half_b_0__compute_normal_vector_v_core_0) * s_inv3_0_0;
              cos_theta_0__compute_normal_vector_v_core_0 = cos(theta_0__compute_normal_vector_v_core_0);
              sin_theta_0__compute_normal_vector_v_core_0 = sin(theta_0__compute_normal_vector_v_core_0);
              eigenvalues_0__compute_normal_vector_v_core_0[0] = c2_over_3_0__compute_normal_vector_v_core_0 + 2.0 * rho_0__compute_normal_vector_v_core_0 * cos_theta_0__compute_normal_vector_v_core_0;
              eigenvalues_0__compute_normal_vector_v_core_0[1] = c2_over_3_0__compute_normal_vector_v_core_0 - rho_0__compute_normal_vector_v_core_0 * (cos_theta_0__compute_normal_vector_v_core_0 + s_sqrt3_0_0 * sin_theta_0__compute_normal_vector_v_core_0);
              eigenvalues_0__compute_normal_vector_v_core_0[2] = c2_over_3_0__compute_normal_vector_v_core_0 - rho_0__compute_normal_vector_v_core_0 * (cos_theta_0__compute_normal_vector_v_core_0 - s_sqrt3_0_0 * sin_theta_0__compute_normal_vector_v_core_0);
// Sort in increasing order.
              if (eigenvalues_0__compute_normal_vector_v_core_0[0] >= eigenvalues_0__compute_normal_vector_v_core_0[1]) {
                tmp__swap_2 =  *(&eigenvalues_0__compute_normal_vector_v_core_0[0]);
                 *(&eigenvalues_0__compute_normal_vector_v_core_0[0]) =  *(&eigenvalues_0__compute_normal_vector_v_core_0[1]);
                 *(&eigenvalues_0__compute_normal_vector_v_core_0[1]) = tmp__swap_2;
              }
              if (eigenvalues_0__compute_normal_vector_v_core_0[1] >= eigenvalues_0__compute_normal_vector_v_core_0[2]) {{
                  tmp__swap_0_0 =  *(&eigenvalues_0__compute_normal_vector_v_core_0[1]);
                   *(&eigenvalues_0__compute_normal_vector_v_core_0[1]) =  *(&eigenvalues_0__compute_normal_vector_v_core_0[2]);
                   *(&eigenvalues_0__compute_normal_vector_v_core_0[2]) = tmp__swap_0_0;
                }
                if (eigenvalues_0__compute_normal_vector_v_core_0[0] >= eigenvalues_0__compute_normal_vector_v_core_0[1]) {
                  tmp__swap_1_0 =  *(&eigenvalues_0__compute_normal_vector_v_core_0[0]);
                   *(&eigenvalues_0__compute_normal_vector_v_core_0[0]) =  *(&eigenvalues_0__compute_normal_vector_v_core_0[1]);
                   *(&eigenvalues_0__compute_normal_vector_v_core_0[1]) = tmp__swap_1_0;
                }
              }
// eigenval for symetric positive semi-definite matrix can not be negative! Set it to 0
              if (eigenvalues_0__compute_normal_vector_v_core_0[0] <= ((double )0)) {
////computeRoots2_v (c2, c1, eigenvalues);
////new_start:
                eigenvalues_0__compute_normal_vector_v_core_0[0] = 0.0;
                d_1__compute_normal_vector_v_core_0 = ((Scalar )(c2_0__compute_normal_vector_v_core_0 * c2_0__compute_normal_vector_v_core_0 - 4.0 * c1_0__compute_normal_vector_v_core_0));
// no real eigenvalues!!!! THIS SHOULD NOT HAPPEN!
                if (d_1__compute_normal_vector_v_core_0 < 0.0) {
                  d_1__compute_normal_vector_v_core_0 = 0.0;
                }
                sd_1__compute_normal_vector_v_core_0 = sqrt(d_1__compute_normal_vector_v_core_0);
                eigenvalues_0__compute_normal_vector_v_core_0[2] = ((Scalar )0.5) * (c2_0__compute_normal_vector_v_core_0 + sd_1__compute_normal_vector_v_core_0);
                eigenvalues_0__compute_normal_vector_v_core_0[1] = ((Scalar )0.5) * (c2_0__compute_normal_vector_v_core_0 - sd_1__compute_normal_vector_v_core_0);
////end
              }
            }
// Construct the parameters used in classifying the eigenvalues of the equation
// and in solving the equation for the eigenvalues in closed form.
////end: computeRoots_c
            lambda3_0__compute_normal_vector_v_core_0 = eigenvalues_0__compute_normal_vector_v_core_0[0] * scale_0__compute_normal_vector_v_core_0;
//subtract the diagonal by eigenvalue[0] 
            scaled_cov_0__compute_normal_vector_v_core_0[0] -= eigenvalues_0__compute_normal_vector_v_core_0[0];
            scaled_cov_0__compute_normal_vector_v_core_0[4] -= eigenvalues_0__compute_normal_vector_v_core_0[0];
            scaled_cov_0__compute_normal_vector_v_core_0[8] -= eigenvalues_0__compute_normal_vector_v_core_0[0];
{
              vec1_0__compute_normal_vector_v_core_0[0] = ((const Scalar *)scaled_cov_0__compute_normal_vector_v_core_0)[1] * ((const Scalar *)(scaled_cov_0__compute_normal_vector_v_core_0 + 3))[2] - ((const Scalar *)scaled_cov_0__compute_normal_vector_v_core_0)[2] * ((const Scalar *)(scaled_cov_0__compute_normal_vector_v_core_0 + 3))[1];
              vec1_0__compute_normal_vector_v_core_0[1] = ((const Scalar *)scaled_cov_0__compute_normal_vector_v_core_0)[2] * ((const Scalar *)(scaled_cov_0__compute_normal_vector_v_core_0 + 3))[0] - ((const Scalar *)scaled_cov_0__compute_normal_vector_v_core_0)[0] * ((const Scalar *)(scaled_cov_0__compute_normal_vector_v_core_0 + 3))[2];
              vec1_0__compute_normal_vector_v_core_0[2] = ((const Scalar *)scaled_cov_0__compute_normal_vector_v_core_0)[0] * ((const Scalar *)(scaled_cov_0__compute_normal_vector_v_core_0 + 3))[1] - ((const Scalar *)scaled_cov_0__compute_normal_vector_v_core_0)[1] * ((const Scalar *)(scaled_cov_0__compute_normal_vector_v_core_0 + 3))[0];
            }
{
              vec2_0__compute_normal_vector_v_core_0[0] = ((const Scalar *)scaled_cov_0__compute_normal_vector_v_core_0)[1] * ((const Scalar *)(scaled_cov_0__compute_normal_vector_v_core_0 + 6))[2] - ((const Scalar *)scaled_cov_0__compute_normal_vector_v_core_0)[2] * ((const Scalar *)(scaled_cov_0__compute_normal_vector_v_core_0 + 6))[1];
              vec2_0__compute_normal_vector_v_core_0[1] = ((const Scalar *)scaled_cov_0__compute_normal_vector_v_core_0)[2] * ((const Scalar *)(scaled_cov_0__compute_normal_vector_v_core_0 + 6))[0] - ((const Scalar *)scaled_cov_0__compute_normal_vector_v_core_0)[0] * ((const Scalar *)(scaled_cov_0__compute_normal_vector_v_core_0 + 6))[2];
              vec2_0__compute_normal_vector_v_core_0[2] = ((const Scalar *)scaled_cov_0__compute_normal_vector_v_core_0)[0] * ((const Scalar *)(scaled_cov_0__compute_normal_vector_v_core_0 + 6))[1] - ((const Scalar *)scaled_cov_0__compute_normal_vector_v_core_0)[1] * ((const Scalar *)(scaled_cov_0__compute_normal_vector_v_core_0 + 6))[0];
            }
{
              vec3_0__compute_normal_vector_v_core_0[0] = ((const Scalar *)(scaled_cov_0__compute_normal_vector_v_core_0 + 3))[1] * ((const Scalar *)(scaled_cov_0__compute_normal_vector_v_core_0 + 6))[2] - ((const Scalar *)(scaled_cov_0__compute_normal_vector_v_core_0 + 3))[2] * ((const Scalar *)(scaled_cov_0__compute_normal_vector_v_core_0 + 6))[1];
              vec3_0__compute_normal_vector_v_core_0[1] = ((const Scalar *)(scaled_cov_0__compute_normal_vector_v_core_0 + 3))[2] * ((const Scalar *)(scaled_cov_0__compute_normal_vector_v_core_0 + 6))[0] - ((const Scalar *)(scaled_cov_0__compute_normal_vector_v_core_0 + 3))[0] * ((const Scalar *)(scaled_cov_0__compute_normal_vector_v_core_0 + 6))[2];
              vec3_0__compute_normal_vector_v_core_0[2] = ((const Scalar *)(scaled_cov_0__compute_normal_vector_v_core_0 + 3))[0] * ((const Scalar *)(scaled_cov_0__compute_normal_vector_v_core_0 + 6))[1] - ((const Scalar *)(scaled_cov_0__compute_normal_vector_v_core_0 + 3))[1] * ((const Scalar *)(scaled_cov_0__compute_normal_vector_v_core_0 + 6))[0];
            }
{
              return_0_squared_norm_v_0 = vec1_0__compute_normal_vector_v_core_0[0] * vec1_0__compute_normal_vector_v_core_0[0] + vec1_0__compute_normal_vector_v_core_0[1] * vec1_0__compute_normal_vector_v_core_0[1] + vec1_0__compute_normal_vector_v_core_0[2] * vec1_0__compute_normal_vector_v_core_0[2];
            }
            len1_0__compute_normal_vector_v_core_0 = return_0_squared_norm_v_0;
{
              return_1_squared_norm_v_0 = vec2_0__compute_normal_vector_v_core_0[0] * vec2_0__compute_normal_vector_v_core_0[0] + vec2_0__compute_normal_vector_v_core_0[1] * vec2_0__compute_normal_vector_v_core_0[1] + vec2_0__compute_normal_vector_v_core_0[2] * vec2_0__compute_normal_vector_v_core_0[2];
            }
            len2_0__compute_normal_vector_v_core_0 = return_1_squared_norm_v_0;
{
              return_2_squared_norm_v_0 = vec3_0__compute_normal_vector_v_core_0[0] * vec3_0__compute_normal_vector_v_core_0[0] + vec3_0__compute_normal_vector_v_core_0[1] * vec3_0__compute_normal_vector_v_core_0[1] + vec3_0__compute_normal_vector_v_core_0[2] * vec3_0__compute_normal_vector_v_core_0[2];
            }
            len3_0__compute_normal_vector_v_core_0 = return_2_squared_norm_v_0;
            len12_0__compute_normal_vector_v_core_0 = ((int )(len1_0__compute_normal_vector_v_core_0 >= len2_0__compute_normal_vector_v_core_0));
            len13_0__compute_normal_vector_v_core_0 = ((int )(len1_0__compute_normal_vector_v_core_0 >= len3_0__compute_normal_vector_v_core_0));
            len21_0__compute_normal_vector_v_core_0 = ((int )(len2_0__compute_normal_vector_v_core_0 >= len1_0__compute_normal_vector_v_core_0));
            len23_0__compute_normal_vector_v_core_0 = ((int )(len2_0__compute_normal_vector_v_core_0 >= len3_0__compute_normal_vector_v_core_0));
            if (((bool )len12_0__compute_normal_vector_v_core_0) && ((bool )len13_0__compute_normal_vector_v_core_0)) {
              a__div_v_2 = sqrt(len1_0__compute_normal_vector_v_core_0);
              v3_0__compute_normal_vector_v_core_0[0] = vec1_0__compute_normal_vector_v_core_0[0] / a__div_v_2;
              v3_0__compute_normal_vector_v_core_0[1] = vec1_0__compute_normal_vector_v_core_0[1] / a__div_v_2;
              v3_0__compute_normal_vector_v_core_0[2] = vec1_0__compute_normal_vector_v_core_0[2] / a__div_v_2;
            }
             else {
              if (((bool )len21_0__compute_normal_vector_v_core_0) && ((bool )len23_0__compute_normal_vector_v_core_0)) {
                a__div_v_0_0 = sqrt(len2_0__compute_normal_vector_v_core_0);
                v3_0__compute_normal_vector_v_core_0[0] = vec2_0__compute_normal_vector_v_core_0[0] / a__div_v_0_0;
                v3_0__compute_normal_vector_v_core_0[1] = vec2_0__compute_normal_vector_v_core_0[1] / a__div_v_0_0;
                v3_0__compute_normal_vector_v_core_0[2] = vec2_0__compute_normal_vector_v_core_0[2] / a__div_v_0_0;
              }
               else {
                a__div_v_1_0 = sqrt(len3_0__compute_normal_vector_v_core_0);
                v3_0__compute_normal_vector_v_core_0[0] = vec3_0__compute_normal_vector_v_core_0[0] / a__div_v_1_0;
                v3_0__compute_normal_vector_v_core_0[1] = vec3_0__compute_normal_vector_v_core_0[1] / a__div_v_1_0;
                v3_0__compute_normal_vector_v_core_0[2] = vec3_0__compute_normal_vector_v_core_0[2] / a__div_v_1_0;
              }
            }
////end: eigen33_c
//int result_index = new_start;
//fixed bug155
//start * TS + sub_start;
            result_index_0__compute_normal_vector_v_core_0 = new_start_0__compute_normal_vector_v_core_0;
//int result_index = start * TS + sub_start + j0*TS_10;
////   const Scalar *ptr1 = data + indices[0]*step;
            d_2__compute_normal_vector_v_core_0[0] = origin_t_buf_0[start_0__compute_normal_vector_v_core_0][j0_0__compute_normal_vector_v_core_0][sub_start_0__compute_normal_vector_v_core_0][0] - data[next_idx_0__compute_normal_vector_v_core_0 * 3];
            d_2__compute_normal_vector_v_core_0[1] = origin_t_buf_0[start_0__compute_normal_vector_v_core_0][j0_0__compute_normal_vector_v_core_0][sub_start_0__compute_normal_vector_v_core_0][1] - data[next_idx_0__compute_normal_vector_v_core_0 * 3 + 1];
            d_2__compute_normal_vector_v_core_0[2] = origin_t_buf_0[start_0__compute_normal_vector_v_core_0][j0_0__compute_normal_vector_v_core_0][sub_start_0__compute_normal_vector_v_core_0][2] - data[next_idx_0__compute_normal_vector_v_core_0 * 3 + 2];
            dot_product_0__compute_normal_vector_v_core_0 = v3_0__compute_normal_vector_v_core_0[0] * d_2__compute_normal_vector_v_core_0[0] + v3_0__compute_normal_vector_v_core_0[1] * d_2__compute_normal_vector_v_core_0[1] + v3_0__compute_normal_vector_v_core_0[2] * d_2__compute_normal_vector_v_core_0[2];
            if (dot_product_0__compute_normal_vector_v_core_0 >= 0.0) {
              normal_t_buf_0[start_0__compute_normal_vector_v_core_0][j0_0__compute_normal_vector_v_core_0][sub_start_0__compute_normal_vector_v_core_0][0] = v3_0__compute_normal_vector_v_core_0[0];
              normal_t_buf_0[start_0__compute_normal_vector_v_core_0][j0_0__compute_normal_vector_v_core_0][sub_start_0__compute_normal_vector_v_core_0][1] = v3_0__compute_normal_vector_v_core_0[1];
              normal_t_buf_0[start_0__compute_normal_vector_v_core_0][j0_0__compute_normal_vector_v_core_0][sub_start_0__compute_normal_vector_v_core_0][2] = v3_0__compute_normal_vector_v_core_0[2];
            }
             else {
              normal_t_buf_0[start_0__compute_normal_vector_v_core_0][j0_0__compute_normal_vector_v_core_0][sub_start_0__compute_normal_vector_v_core_0][0] = -v3_0__compute_normal_vector_v_core_0[0];
              normal_t_buf_0[start_0__compute_normal_vector_v_core_0][j0_0__compute_normal_vector_v_core_0][sub_start_0__compute_normal_vector_v_core_0][1] = -v3_0__compute_normal_vector_v_core_0[1];
              normal_t_buf_0[start_0__compute_normal_vector_v_core_0][j0_0__compute_normal_vector_v_core_0][sub_start_0__compute_normal_vector_v_core_0][2] = -v3_0__compute_normal_vector_v_core_0[2];
            }
          }
        }
      }
    }
//    memcpy(normal_t,  normal,  sizeof(double)*SAFE_LENGTH *NORMAL_LENGTH);
  }
  for (_memcpy_i2_3 = 0; _memcpy_i2_3 < 3; ++_memcpy_i2_3) {
    for (_memcpy_i2_2 = 0; _memcpy_i2_2 < 10; ++_memcpy_i2_2) {
      for (_memcpy_i2_1 = 0; _memcpy_i2_1 < 1000; ++_memcpy_i2_1) {
        for (_memcpy_i2_0 = 0; _memcpy_i2_0 < 3; ++_memcpy_i2_0) {
          normal_t[0 + ((((0 * 3 + _memcpy_i2_3) * 1000 + _memcpy_i2_2) * 10 + _memcpy_i2_1) * 3 + _memcpy_i2_0)] = normal_t_buf_0[_memcpy_i2_3][_memcpy_i2_2][_memcpy_i2_1][_memcpy_i2_0];
        }
      }
    }
  }
}
