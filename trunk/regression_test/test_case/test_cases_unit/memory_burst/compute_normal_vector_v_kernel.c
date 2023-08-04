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

static  double epsilon = 2.220446e-16; //std::numeric_limits<Scalar>::epsilon();

////namespace gicp {
/*********************************************************************************************************/
void cross_v(const Scalar *x, const Scalar *y, Scalar *x_cross_y) {
#pragma HLS inline
  x_cross_y[0] = x[1]*y[2] - x[2]*y[1];
  x_cross_y[1] = x[2]*y[0] - x[0]*y[2];
  x_cross_y[2] = x[0]*y[1] - x[1]*y[0];
}

/*********************************************************************************************************/
Scalar squared_norm_v(Scalar *x) {
#pragma HLS inline
 return x[0]*x[0] + x[1]*x[1] + x[2]*x[2];
}

/*********************************************************************************************************/
void div_v(Scalar *x, Scalar a, Scalar *y) {
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
static void swap(double * a, double * b)
{
    double tmp = *a;
    *a = *b;
    *b = tmp;
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
#pragma ACCEL kernel
void compute_normal_vector_v_kernel(
        bus_data_type data1[(BUFFER_LENGTH+bus_data_width-1)/bus_data_width], 
//        int indices_t[INDEX_LENGTH], 
//        double origin_t[SAFE_LENGTH *NORMAL_LENGTH], 
        double normal[SAFE_LENGTH *NORMAL_LENGTH]) 
{
#ifdef ADD_IF
#pragma HLS INTERFACE m_axi port=data1 offset=slave bundle=gmem
//#pragma HLS INTERFACE m_axi port=indices offset=slave bundle=gmem1
#pragma HLS INTERFACE m_axi port=normal offset=slave bundle=gmem2
///#pragma HLS INTERFACE m_axi port=origin offset=slave bundle=gmem3
#pragma HLS interface s_axilite port=return
#endif
    int i, j;
  int step = STEP;
  const Scalar ptr1 = data1[0];
  const Scalar ptr2 = data1[1];
  const Scalar ptr3 = data1[2];

  int start;
  for (start=0; start<INDEX_LENGTH; start+=COMPUTE_LENGTH) {
   int n = ACCU_INDEX;
   double cov[9];
#pragma HLS pipeline II=18
#pragma HLS array partition variable=cov complete dim=1

  Scalar accu[9];
  Scalar accu1[9];
//#pragma HLS array partition variable=accu complete dim=1
//#pragma HLS array partition variable=accu1 complete dim=1
LOOP_SLOW:  for ( i = 0 ; i < COMPUTE_LENGTH; ++i )
  {
    #pragma ACCEL pipeline
    Scalar x;
    Scalar y;
    Scalar z;

    {
            x = data1[3*(start+i)+0];
            y = data1[3*(start+i)+1];
            z = data1[3*(start+i)+2];
    }

    if (i == 0) {
      accu [0] = x * x; //8cycles
      accu [1] = x * y;
      accu [2] = x * z;
      accu [3] = y * y;
      accu [4] = y * z;
      accu [5] = z * z;
      accu [6] = x;
      accu [7] = y;
      accu [8] = z;
    } else {
      accu [0] += x * x; //8cycles
      accu [1] += x * y;
      accu [2] += x * z;
      accu [3] += y * y;
      accu [4] += y * z;
      accu [5] += z * z;
      accu [6] += x;
      accu [7] += y;
      accu [8] += z;
    }

  }
//*/
  for ( i = 0 ; i < n ; ++i ) {
    accu[i] /= COMPUTE_LENGTH ;
  }

//ORIGINAL end

  cov[0] =  (accu [0] - accu [6] * accu [6]);
  cov[1] =  (accu [1] - accu [6] * accu [7]);
  cov[2] =  (accu [2] - accu [6] * accu [8]);
  cov[4] =  (accu [3] - accu [7] * accu [7]);
  cov[5] =  (accu [4] - accu [7] * accu [8]);
  cov[8] =  (accu [5] - accu [8] * accu [8]);
  cov[3] = cov[1];
  cov[6] = cov[2];
  cov[7] = cov[5];
  //for (int j = 0; j < n; j++) {
  //  printf("HW======start = %d, cov[%d] = %e\n", start, j, cov[j]);
  //}

////end

   // find the eigenvector of the smallest eigenvalue
   double v3[3];
#pragma HLS ARRAY_RESHAPE variable=v3 complete dim=1
   double lambda3;

////   eigen33_v<Scalar>(cov, v3, &lambda3);
////start:
   // solve for plane equation 
   // Scale the matrix so its entries are in [-1,1].  The scaling is applied
   // only when at least one matrix entry has magnitude larger than 1.

   Scalar scale = 0.0;
   for ( i = 0 ; i < n ; ++i ) {
     Scalar abs_value = fabs(cov[i]);
     if ( scale < abs_value ) {
       scale = abs_value;
     }
   }

   if ( scale < 1.0 /*should scale * scale*/)
     scale = 1.0 /*should scale * scale*/;

   Scalar scaled_cov[9];
#pragma HLS array partition variable=scaled_cov complete dim=1

   for ( i = 0 ; i < n ; ++i ) {
     scaled_cov[i] = cov[i] / scale;
   }

   Scalar eigenvalues[3];
////#pragma HLS ARRAY_RESHAPE variable=eigenvalues complete dim=1

   Scalar *roots = eigenvalues;
////   computeRoots_v (scaled_cov, eigenvalues);
////start:
  // The characteristic equation is x^3 - c2*x^2 + c1*x - c0 = 0.  The
  // eigenvalues are the roots to this equation, all guaranteed to be
  // real-valued, because the matrix is symmetric.

  // m is a 3 by 3 matrix in the following ordering
  // m(0) m(1) m(2)
  // m(3) m(4) m(5)
  // m(6) m(7) m(8)


  Scalar c0 =   scaled_cov[0] * scaled_cov[4] * scaled_cov[8]
              + (Scalar) (2) * scaled_cov[1] * scaled_cov[2] * scaled_cov[5]
              - scaled_cov[0] * scaled_cov[5] * scaled_cov[5]
              - scaled_cov[4] * scaled_cov[2] * scaled_cov[2]
              - scaled_cov[8] * scaled_cov[1] * scaled_cov[1];

  Scalar c1 = scaled_cov[0] * scaled_cov[4] -
              scaled_cov[1] * scaled_cov[1] +
              scaled_cov[0] * scaled_cov[8] -
              scaled_cov[2] * scaled_cov[2] +
              scaled_cov[4] * scaled_cov[8] -
              scaled_cov[5] * scaled_cov[5];

  Scalar c2 = scaled_cov[0] + scaled_cov[4] + scaled_cov[8];


  //printf ("epsilon = %le\n", eps);
  if (fabs (c0) < epsilon ) {// one root is 0 -> quadratic equation
    ////computeRoots2_v (c2, c1, roots);
    ////start:
     roots [0] = (Scalar) (0);
     Scalar d = (Scalar) (c2 * c2 - 4.0 * c1);
     if (d < 0.0) // no real roots!!!! THIS SHOULD NOT HAPPEN!
       d = 0.0;
   
     Scalar sd = sqrt (d);
   
     roots[2] = (Scalar)0.5 * (c2 + sd);
     roots[1] = (Scalar)0.5 * (c2 - sd);
    ////end
  }
  else
  {
    const Scalar s_inv3 = (1.0 / 3.0);
    const Scalar s_sqrt3 = sqrt ((3.0));
    // Construct the parameters used in classifying the roots of the equation
    // and in solving the equation for the roots in closed form.
    Scalar c2_over_3 = c2*s_inv3;
    Scalar a_over_3 = (c1 - c2 * c2_over_3) * s_inv3;
    if (a_over_3 > (0.0))
      a_over_3 = (Scalar) (0.0);

    Scalar half_b = (Scalar) (0.5) * (c0 + c2_over_3 * ((Scalar) (2) * c2_over_3 * c2_over_3 - c1));

    Scalar q = half_b * half_b + a_over_3 * a_over_3*a_over_3;
    if (q > (Scalar) (0))
      q = (Scalar) (0);

    // Compute the eigenvalues by solving for the roots of the polynomial.
    Scalar rho = sqrt (-a_over_3);
    Scalar theta = atan2 (sqrt (-q), half_b) * s_inv3;
    Scalar cos_theta = cos (theta);
    Scalar sin_theta = sin (theta);
    roots[0] = c2_over_3 + 2.0 * rho * cos_theta;
    roots[1] = c2_over_3 - rho * (cos_theta + s_sqrt3 * sin_theta);
    roots[2] = c2_over_3 - rho * (cos_theta - s_sqrt3 * sin_theta);

    // Sort in increasing order.
    if (roots[0] >= roots[1])
      swap (&roots[0], &roots[1]);
    if (roots[1] >= roots[2])
    {
      swap (&roots[1], &roots[2]);
      if (roots[0] >= roots[1])
        swap (&roots[0], &roots[1]);
    }

    if (roots[0] <= 0) {// eigenval for symetric positive semi-definite matrix can not be negative! Set it to 0
      ////computeRoots2_v (c2, c1, roots);
       ////start:
      roots [0] = (0.0);
      Scalar d = (Scalar) (c2 * c2 - 4.0 * c1);
      if (d < 0.0) // no real roots!!!! THIS SHOULD NOT HAPPEN!
        d = 0.0;

      Scalar sd = sqrt (d); 
      roots[2] = (Scalar)0.5 * (c2 + sd);
      roots[1] = (Scalar)0.5 * (c2 - sd);
    ////end
    }
  }
////end: computeRoots_c

   lambda3 = eigenvalues[0] * scale;

   //subtract the diagonal by eigenvalue[0] 
   scaled_cov[0] -= eigenvalues[0];
   scaled_cov[4] -= eigenvalues[0];
   scaled_cov[8] -= eigenvalues[0];


   Scalar vec1[3], vec2[3], vec3[3];

   // cross vector 
   cross_v(scaled_cov, scaled_cov+3, vec1);
   cross_v(scaled_cov, scaled_cov+6, vec2);
   cross_v(scaled_cov+3, scaled_cov+6, vec3);

   double len1 = squared_norm_v(vec1);
   double len2 = squared_norm_v(vec2);
   double len3 = squared_norm_v(vec3);

   int len12 = (len1 >= len2);
   int len13 = (len1 >= len3);
   int len21 = (len2 >= len1);
   int len23 = (len2 >= len3);

   if (len12 && len13)
     div_v(vec1, sqrt(len1), v3);
   else if (len21 && len23)
     div_v(vec2, sqrt(len2), v3);
   else
     div_v(vec3, sqrt(len3), v3);

////end: eigen33_c
   double d[3];
#pragma HLS array partition variable=d complete dim=1

  int result_index = start/COMPUTE_LENGTH; 

   normal[result_index*3+0] = v3[0];
   normal[result_index*3+1] = v3[1];
   normal[result_index*3+2] = v3[2];


    } 

}

int main() {
  bus_data_type *data1;
  double *normal;
  compute_normal_vector_v_kernel(
        data1, normal);
  return 0;
}
