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
 * normal-c.h
 *
 *      @date: May 22, 2015
 *      @author: Yong-Dian Jian
 *		@brief:
 *
 */


#pragma once

#define bus_data_width 1
typedef double bus_data_type;
//#define bus_data_type double

typedef double Scalar;

#define BUFFER_LENGTH 90000 //240030 //16 kernels: 90828/16 //8->19683, 9->24843, 10->30603
#define INDEX_LENGTH  540000 //80010*2 //16 kernels: 30276/16 
#define NORMAL_LENGTH 3
#define INDEX 80000 //1->121, 5->2601, 6->3721, 7->5041 ok, 8->6561 ->0, 9->8281, 10->10201
#define STEP  3;
#define COMPUTE_LENGTH  18 //216
#define COMPUTE_START   0 
#define ACCU_INDEX 9;

#define SAFE_LENGTH ((INDEX_LENGTH+COMPUTE_LENGTH-1)/COMPUTE_LENGTH)


//namespace gicp {
/*********************************************************************************************************/
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
//void covariance_v(const Scalar *data, const int n, const int step, const int *indices, const int m, Scalar *cov);
void covariance_v(const Scalar *data,                    const int step, const int *indices, const int m, Scalar *cov) ;

//*********************************************************************************************************/
/** \brief Compute the roots of a quadratic polynom x^2 + b*x + c = 0
 * \param[in] b linear parameter
 * \param[in] c constant parameter
 * \param[out] roots solutions of x^2 + b*x + c = 0
 */
void computeRoots2_v(const Scalar b, const Scalar c, Scalar *roots);

/*********************************************************************************************************/
/** \brief computes the roots of the characteristic polynomial of the input matrix m, which are the eigenvalues
 *
 * \param[in] m is of length 9, denoting a 3 by 3 matrix
 *
 * \param[out] roots is of length 3, denotes the eigenvalues of m
 */
void computeRoots_v(const Scalar *matrix, Scalar *roots);

/*********************************************************************************************************/
/*
 * Input:
 * covariance is of length 9, denoting a 3 by 3 covariance matrix
 *
 * Output:
 * v3 is of length 3, denoting the smallest eigenvector
 * lambda is of length 1, denoting the smallest eigenvalue
 */
void eigen33_v(const Scalar *cov, Scalar *v3, Scalar *lambda3) ;

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
//void compute_normal_vector_v(const bus_data_type data[BUFFER_LENGTH/bus_data_width+1], const int indices[INDEX_LENGTH], const double origin[INDEX_LENGTH/COMPUTE_LENGTH][NORMAL_LENGTH], double normal[INDEX_LENGTH/COMPUTE_LENGTH][NORMAL_LENGTH]) ;

#ifdef _MARS_COMPILER_FLOW_
void compute_normal_vector_v_kernel(
#else
void compute_normal_vector_v(
#endif
        bus_data_type data[(BUFFER_LENGTH+bus_data_width-1)/bus_data_width], 
        bus_data_type data1[(BUFFER_LENGTH+bus_data_width-1)/bus_data_width], 
        int indices_t[INDEX_LENGTH], 
        double origin_t[SAFE_LENGTH *NORMAL_LENGTH], 
        double normal_t[SAFE_LENGTH *NORMAL_LENGTH]) ;


////}
//
//
//



//

