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
// RUN: %clang_cc1 -std=c++11 -fsyntax-only -verify %s
// expected-no-diagnostics
// Metafunction to extract the Nth type from a set of types.
template < unsigned N, typename ... Types > struct get_nth_type;
template < unsigned N, typename Head, typename ... Tail >
struct get_nth_type < N, Head, Tail ... > : get_nth_type < N - 1, Tail ... > { };
template < typename Head, typename ... Tail >
struct get_nth_type < 0, Head, Tail ... > {
  typedef Head type;
};
// Placeholder type  when get_nth_type fails.

struct no_type 
{
}
;
template < unsigned N >
struct get_nth_type < N > {
  typedef no_type type;
};
template < typename ... Args >
typename get_nth_type < 0, Args ... > :: type first_arg ( Args ... );
template < typename ... Args >
typename get_nth_type < 1, Args ... > :: type second_arg ( Args ... );
// Test explicit specification of function template arguments.

void test_explicit_spec_simple()
{
  int *ip1 = ::first_arg< int * > (0);
  int *ip2 = ::first_arg< int * , float * > (0,0);
  float *fp1 = ::first_arg< float * , double * , int * > (0,0,0);
}
// Template argument deduction can extend the sequence of template
// arguments corresponding to a template parameter pack, even when the
// sequence contains explicitly specified template arguments.
#pragma ACCEL kernel name="test_explicit_spec_extension"

void test_explicit_spec_extension(double *dp)
{
  int *ip1 = ::first_arg< int * , int  > (0,0);
  int *ip2 = ::first_arg< int * , float * , int  , int  > (0,0,0,0);
  float *fp1 = ::first_arg< float * , double * , int * > (0,0,0);
  int *i1 = ::second_arg< float * , int * , int  > (0,((int *)0),0);
  double *dp1 = ::first_arg< double * > (dp);
}
template < typename ... Types >
struct tuple { };
template < typename ... Types >
void accept_tuple ( tuple < Types ... > );

void test_explicit_spec_extension_targs(struct tuple< int  , float  , double  > t3)
{
  ::accept_tuple< int  , float  , double  > (t3);
  ::accept_tuple< int  , float  , double  > (t3);
  ::accept_tuple< int  , float  , double  > (t3);
  ::accept_tuple< int  , float  , double  > (t3);
}
template < typename R, typename ... ParmTypes >
void accept_function_ptr ( R ( * ) ( ParmTypes ... ) );

void test_explicit_spec_extension_funcparms(int (*f3)(int , float , double ))
{
  ::accept_function_ptr< int  , int  , float  , double  > (f3);
  ::accept_function_ptr< int  , int  , float  , double  > (f3);
  ::accept_function_ptr< int  , int  , float  , double  > (f3);
  ::accept_function_ptr< int  , int  , float  , double  > (f3);
  ::accept_function_ptr< int  , int  , float  , double  > (f3);
}
