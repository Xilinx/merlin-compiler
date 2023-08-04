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
#include "cmost.h"
/*
 * testNormalC.cpp
 *
 *      @date: May 22, 2015
 *      @author: Yong-Dian Jian
 *		@brief:
 *
 */
//#include <catch.hpp>
#include "normal-c.h"
#include "math.h"
//#include <iostream>
//#include <iomanip>
//
//#include <numeric>
//#include <limits>
//#include <cmath>
//#include <iostream>
#include <stdio.h>
#include <string.h>
////using namespace gicp;
//numeric_limits<Scalar>::epsilon();
static double epsilon = 2.220446e-16;
/*********************************************************************************************************/

void cross_c(const Scalar *x,const Scalar *y,Scalar *x_cross_y)
{
  x_cross_y[0] = x[1] * y[2] - x[2] * y[1];
  x_cross_y[1] = x[2] * y[0] - x[0] * y[2];
  x_cross_y[2] = x[0] * y[1] - x[1] * y[0];
}
/*********************************************************************************************************/

Scalar squared_norm_c(Scalar *x)
{
  return x[0] * x[0] + x[1] * x[1] + x[2] * x[2];
}
/*********************************************************************************************************/

void div_c(Scalar *x,Scalar a,Scalar *y)
{
  y[0] = x[0] / a;
  y[1] = x[1] / a;
  y[2] = x[2] / a;
}
/* Input:
 * data is a buffer of length n*step (like a n by step matrix)
 * n is the number of total points
 * step is the number of elements per row
 * indices_t is of length m where each number is the row index of interest
 * origin_t is of length 3: indicating the location of sensor, the resulting normal_t has to face to the origin_t
 *
 * Output:
 * covariance is of length 9, denoting a 3 by 3 covariance matrix
 *
 */

void covariance_c(const double *data,const int start,const size_t step,const int *indices_t,double *cov)
{
// create the buffer on the stack which is much faster than using mat[indices_t[i]] and centroid as a buffer
  double accu[9] = {(0.0), (0.0), (0.0), (0.0), (0.0), (0.0), (0.0), (0.0), (0.0)};
  int i;
  int j;
  for (i = start; i < start + 18; ++i) {
    int idx = indices_t[i];
    const double *ptr = data + idx * step;
    double x =  *ptr;
    double y =  *(ptr + 1);
    double z =  *(ptr + 2);
    accu[0] += x * x;
    accu[1] += x * y;
    accu[2] += x * z;
    accu[3] += y * y;
    accu[4] += y * z;
    accu[5] += z * z;
    accu[6] += x;
    accu[7] += y;
    accu[8] += z;
//    printf("SW: idx = %d, x = %e, y = %e, z = %e\n", idx, x, y, z);
  }
  for (j = 0; j < 9; j++) {
//    printf("SW1======accu[%d] = %e\n", j, accu[j]);
  }
  for (i = 0; i < 9; ++i) {
    accu[i] /= 18;
  }
  for (j = 0; j < 9; j++) {
//    printf("SW2======accu[%d] = %e\n", j, accu[j]);
  }
//  printf("SW======4-7*8 = %e, accu7*8 = %e\n", accu[4]-accu[7]*accu[8], accu [7] * accu [8]);
//  printf("SW======accu6*7 = %e, accu1-6*7 = %e\n", accu[6]*accu[7], accu [1] - accu [6] * accu [7]);
  cov[0] = accu[0] - accu[6] * accu[6];
  cov[1] = accu[1] - accu[6] * accu[7];
  cov[2] = accu[2] - accu[6] * accu[8];
  cov[4] = accu[3] - accu[7] * accu[7];
  cov[5] = accu[4] - accu[7] * accu[8];
  cov[8] = accu[5] - accu[8] * accu[8];
  cov[3] = cov[1];
  cov[6] = cov[2];
  cov[7] = cov[5];
  for (j = 0; j < 9; j++) {
//    printf("SW======start = %d, cov[%d] = %e\n", start, j, cov[j]);
  }
}
/*********************************************************************************************************/
/** \brief Compute the roots of a quadratic polynom x^2 + b*x + c = 0
 * \param[in] b linear parameter
 * \param[in] c constant parameter
 * \param[out] roots solutions of x^2 + b*x + c = 0
 */

void computeRoots2_c(const Scalar b,const Scalar c,Scalar *roots)
{
  roots[0] = ((Scalar )0);
  Scalar d = (Scalar )(b * b - 4.0 * c);
// no real roots!!!! THIS SHOULD NOT HAPPEN!
  if (d < 0.0) {
    d = 0.0;
  }
  Scalar sd = sqrt(d);
  roots[2] = ((Scalar )0.5) * (b + sd);
  roots[1] = ((Scalar )0.5) * (b - sd);
}

static void swap(double *a,double *b)
{
  double tmp =  *a;
   *a =  *b;
   *b = tmp;
}
/*********************************************************************************************************/
/** \brief computes the roots of the characteristic polynomial of the input matrix m, which are the eigenvalues
 * \param[in] m is of length 9, denoting a 3 by 3 matrix
 * \param[out] roots is of length 3, denotes the eigenvalues of m
 */

void computeRoots_c(const Scalar *m,Scalar *roots)
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
//if (fabs (c0) < numeric_limits<Scalar>::epsilon() )// one root is 0 -> quadratic equation
// one root is 0 -> quadratic equation
  if (fabs(c0) < epsilon) {
    computeRoots2_c(c2,c1,roots);
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
      computeRoots2_c(c2,c1,roots);
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

void eigen33_c(const Scalar *cov,Scalar *v3,Scalar *lambda3)
{
/* solve for plane equation */
// Scale the matrix so its entries are in [-1,1].  The scaling is applied
// only when at least one matrix entry has magnitude larger than 1.
  Scalar scale = 0.0;
  int i;
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
  computeRoots_c(scaled_cov,eigenvalues);
   *lambda3 = eigenvalues[0] * scale;
/* subtract the diagonal by eigenvalue[0] */
  scaled_cov[0] -= eigenvalues[0];
  scaled_cov[4] -= eigenvalues[0];
  scaled_cov[8] -= eigenvalues[0];
  Scalar vec1[3];
  Scalar vec2[3];
  Scalar vec3[3];
/* cross vector */
  cross_c(scaled_cov,(scaled_cov + 3),vec1);
  cross_c(scaled_cov,(scaled_cov + 6),vec2);
  cross_c((scaled_cov + 3),(scaled_cov + 6),vec3);
  Scalar len1 = squared_norm_c(vec1);
  Scalar len2 = squared_norm_c(vec2);
  Scalar len3 = squared_norm_c(vec3);
  if (len1 >= len2 && len1 >= len3) {
    div_c(vec1,(sqrt(len1)),v3);
  }
   else {
    if (len2 >= len1 && len2 >= len3) {
      div_c(vec2,(sqrt(len2)),v3);
    }
     else {
      div_c(vec3,(sqrt(len3)),v3);
    }
  }
}
/*********************************************************************************************************/
/* Input:
 * data is a buffer of length n*step (like a n by step matrix)
 * n is the number of total points
 * step is the number of elements per row
 * indices_t is of length m where each number is the row index of interest
 * origin_t is of length 3: indicating the location of sensor, the resulting normal_t has to face to the origin_t
 *
 * Output:
 * normal_t is of length 3: will store the result normal_t
 */

void compute_normal_vector_c(const double *data,const int start,const size_t step,const int *indices_t,double origin_t[540000][3],double normal_t[540000][3])
{
  Scalar cov[9];
  covariance_c(data,start,step,indices_t,cov);
/* find the eigenvector of the smallest eigenvalue*/
  Scalar v3[3];
  Scalar lambda3;
  eigen33_c(cov,v3,&lambda3);
  Scalar d[3];
  const Scalar *ptr = data + indices_t[0] * step;
  int result_index = start / 18;
  d[0] = origin_t[result_index][0] -  *ptr;
  d[1] = origin_t[result_index][1] -  *(ptr + 1);
  d[2] = origin_t[result_index][2] -  *(ptr + 2);
  Scalar dot_product = v3[0] * d[0] + v3[1] * d[1] + v3[2] * d[2];
  if (dot_product >= 0.0) {
    normal_t[result_index][0] = v3[0];
    normal_t[result_index][1] = v3[1];
    normal_t[result_index][2] = v3[2];
  }
   else {
    normal_t[result_index][0] = -v3[0];
    normal_t[result_index][1] = -v3[1];
    normal_t[result_index][2] = -v3[2];
  }
// printf("SW==========normal_t[%d][0] = %e, normal_t[%d][1] = %e, normal_t[%d][2] = %e\n", result_index, normal_t[result_index][0], result_index, normal_t[result_index][1], result_index, normal_t[result_index][2]);
}
void normal_kernel(bus_data_type *data,bus_data_type *data1,int *indices_t,double *origin_t,double *normal_t);
/*********************************************************************************************************/

int main()
{
//HW:
  typedef double Scalar;
/*17.3*/
  double start = 0;
  double end = 3.00;
  double r = 0.01;
  size_t n = 0;
  double buffer[90000];
  int bufIdx = 0;
  int indIdx = 0;
  double x;
  double y;
  int i;
  int j;
#ifdef _MARS_COMPILER_FLOW_
  double *normal_t;
  cmost_malloc(((void **)(&normal_t)),(540000 + 18 - 1) / 18 * 3 * 8);
  double *origin_t;
  cmost_malloc(((void **)(&origin_t)),(540000 + 18 - 1) / 18 * 3 * 8);
  memset(((void **)origin_t),0,sizeof(double ) * ((540000 + 18 - 1) / 18) * 3);
  bus_data_type *data;
  cmost_malloc(((void **)(&data)),90000 / 1 * 8);
  int *indices_t;
  cmost_malloc(((void **)(&indices_t)),540000 * 4);
#else
#endif
  double origin_t_c[540017 / 18][3];
{
    int _i0;
    int _i1;
    for (_i0 = 0; _i0 <= 29999; ++_i0) 
      for (_i1 = 0; _i1 <= 2; ++_i1) 
        origin_t_c[_i0][_i1] = 0;
  }
  for (x = start; x < end; x += r) {
    for (y = start; y < end; y += r) {
      if (bufIdx < 90000 - 3) {
        buffer[bufIdx++] = x;
        buffer[bufIdx++] = y;
        buffer[bufIdx++] = 10.0;
      }
//      indices_t[indIdx++] = n++;
//      printf("SW: bufIdx = %d, indIdx = %d, x = %e, y = %e, buffer size = %d, indices_t size = %d\n", bufIdx, indIdx, x, y, sizeof(buffer)/8, sizeof(indices_t)/4);
    }
  }
  for (i = bufIdx; i < 90000; i++) {
    buffer[i] = 0;
  }
{
    int j = 0;
    for (j = 0; j < 540000; j++) {
      indices_t[j] = j % (90000 / 3);
      indIdx++;
    }
  }
  printf("final SW: bufIdx = %d, indIdx = %d\n",bufIdx,indIdx);
  for (i = 0; i < 90000 / 1 + 1; i++) {
    bus_data_type tmp;
    for (j = 0; j < 1; j++) {
      if (i * 1 + j < 90000) {
//tmp.range((j+1)*64-1, j*64) = *(long long*) (&buffer[i*bus_data_width+j]);
#ifdef SUPPORT_WIDE_BUS
#else
        tmp = buffer[i * 1 + j];
      }
#endif
    }
    data[i] = tmp;
  }
  
//compute_normal_vector_v_kernel(data, indices_t, origin_t, normal_t);
  normal_kernel(data,data,indices_t,origin_t,normal_t);
//SW: 
  double expNormal[540000 / 18][3];
{
    int _i0;
    int _i1;
    for (_i0 = 0; _i0 <= 29999; ++_i0) 
      for (_i1 = 0; _i1 <= 2; ++_i1) 
        expNormal[_i0][_i1] = 0;
  }
//  start_timer();
//for (i=0; (i+COMPUTE_LENGTH)<=indIdx; i+=COMPUTE_LENGTH) {
  for (i = 0; i + 18 <= 540000; i += 18) {
/*start*/
    compute_normal_vector_c((&buffer[0]),i,3,(&indices_t[0]),origin_t_c,expNormal);
  }
//  acc_comp_timer();
//  report_timer();
//HW & SW compare:
  int err_num = 0;
  double req_error = 1.0e-15;
  for (i = 0; i < 540000 / 18; i++) {
    for (j = 0; j < 3; j++) {
/*1.0e-30*/
      if (normal_t[i * 3 + j] - expNormal[i][j] > req_error) {
//      else
        if (err_num < 10) {
          printf("Test FAIL: SW expNormal[%d][%d] = %e, normal_t[%d][%d] = %e, diff = %e\n",i,j,expNormal[i][j],i,j,normal_t[i * 3 + j],normal_t[i * 3 + j] - expNormal[i][j]);
        }
        err_num++;
      }
//        printf("Test PASS: SW expNormal[%d][%d] = %e, normal_t[%d][%d] = %e, diff = %e\n", i, j, expNormal[i][j], i, j, normal_t[i*3+j], (normal_t[i*3+j] - expNormal[i][j]));
    }
  }
#ifdef _MARS_COMPILER_FLOW_
  cmost_dump(normal_t,"normal_t_out.dat");
  cmost_write_file((&normal_t[0]),"normal_t_out.bin",540000 / 18 * 3 * 8);
  cmost_write_file((&expNormal[0][0]),"normal_t_exp.bin",540000 / 18 * 3 * 8);
#endif
  if (err_num == 0) {
    printf("Test PASSED: no difference is larger than %e\n",req_error);
  }
//  REQUIRE(normal_t(0) == Approx(0.0).epsilon(1e-5));
//  REQUIRE(normal_t(1) == Approx(0.0).epsilon(1e-5));
//  REQUIRE(normal_t(2) == Approx(-1.0).epsilon(1e-5));
  return 0;
}
