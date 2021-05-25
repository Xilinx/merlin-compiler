// (C) Copyright 2016-2021 Xilinx, Inc.
// All Rights Reserved.
//
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.


#pragma once

// #define POLYMODEL_SUPPORT_ISL
#ifdef POLYMODEL_SUPPORT_ISL
#include <isl/ctx.h>
#include <isl/dim.h>
#include <isl/set.h>
#include <isl/constraint.h>
#endif  // POLYMODEL_SUPPORT_ISL

#include <stdio.h>

#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include "file_parser.h"

using std::cerr;
using std::cout;
using std::endl;
using std::map;
using std::max;
using std::min;
using std::string;
using std::vector;

/////////////////////////////////////////////////////////////////////////
// BEGIN: This range is for Muhuan to edit

struct tldm_variable_range {
  explicit tldm_variable_range(string v = "", string l = "", string u = "") {
    variable = v;
    lb = l;
    ub = u;
  }
  string variable;
  string lb;
  string ub;
  //    int lb;
  //    int ub;
};

class tldm_polynomial {
 public:
  explicit tldm_polynomial(string sExpression) {
    m_is_valid = 1;
    parse_polynomial(sExpression);
  }
  tldm_polynomial(string sExpression, vector<tldm_variable_range> range) {
    m_is_valid = 1;
    parse_polynomial(sExpression);
    set_range(range);
  }
  tldm_polynomial(const tldm_polynomial &polynomial)
      : m_is_valid(polynomial.m_is_valid), m_variables(polynomial.m_variables),
        m_var_ranges(polynomial.m_var_ranges),
        m_term_and_radix(polynomial.m_term_and_radix) {}

  tldm_polynomial operator=(const tldm_polynomial &polynomial) {
    m_is_valid = (polynomial.m_is_valid);
    m_variables = (polynomial.m_variables);
    m_var_ranges = (polynomial.m_var_ranges);
    m_term_and_radix = (polynomial.m_term_and_radix);
    return *this;
  }

  //=======================================================================//
  // These basic functions can be used directly as
  // tldm_polynomial::parse_element("2*i"); They do not need to use recursive
  // function, but directly textual processing

  // 0: not/unknown; 1: yes
  static int is_polynomial(string exp);  // has body in PolyModel.cpp

  // 0: not/unknown; 1: integer(0x[0-9]L|UL); 2: variable([a-z|A-Z|_]);
  static int is_element(string exp);

  // 0: not/unknown; 1: add; 2: sub; 3: mul; 4: div* (can be partially
  // supported)
  static int is_binary_op(string exp, string *left_exp, string *right_exp,
                          string *op);

  // 0: not/unknown; 1: minus; 2: plus
  static int is_unary_op(string exp, string *sub_exp, string *op);

  // 0: not a brace; 1: is a brace
  static int is_brace(string exp);
  //=======================================================================//
  // These functions deal with variable list and their range

  // m_variables is generated from parsing, not from setting outside
  // so we will check if the generated variables are defined in some given set
  // if any generate variable is not in the set, we should give error msg
  // outside
  int match_variable(vector<string> defined_variables,
                     vector<string> *undefined_variables);
  void merge_term();
  tldm_polynomial normalize_polynomial(vector<string> variables);

  // set m_var_ranges
  // return 0 if there are undefined variables; 1 for success
  int set_range(vector<tldm_variable_range> vec_range);
  vector<tldm_variable_range> get_range();

  // make change on m_variables, m_var_ranges, and m_term_and_radix;
  // return 0 if var_old does not exist
  int rename_variable(string var_old, string var_new);

  vector<string> get_nonzero_var();

  //=======================================================================//
  // These functions deal with term_and_radix

  // 0: not/unknown; 1: linear for all the variables, the variable may not exist
  // in which the coefficient is zero
  int check_linear(vector<string> variables);

  // return the same as check_linear, if success, coeffs are in the order of
  // variables (variables can be iterators)
  int get_linear_coefficients(vector<string> variables,
                              vector<tldm_polynomial> *coeffs);
  tldm_polynomial get_linear_coefficients(
      string var);  // if nonlinear, return an invalid polynomial.

  // get the coefficient in the order of radix of var
  // E.g. m*m*i+m*j+k+m*2+5 --(var=m)--> coeff=("k+5", "j+2", "i")
  int get_polynomial_coefficients(string var, vector<tldm_polynomial> *coeffs);

  //=======================================================================//
  // These functions calculate bounds, reduction and substitute
  // return 0 if unbounded, 1 for success
  int get_lower_bound_const(int *bound);
  int get_upper_bound_const(int *bound);
  int get_bound_const(int *lower_bound, int *upper_bound);
  tldm_polynomial get_lower_bound_by_reduction(string var_to_reduce);
  tldm_polynomial get_upper_bound_by_reduction(string var_to_reduce);
  tldm_polynomial get_polynomial_by_substitute(string var_to_replace,
                                               tldm_polynomial new_value);

  //=======================================================================//
  // These functions are for dumping and debugging
  string exp_to_string();
  string vars_to_string();
  string ranges_to_string();

  //=======================================================================//
  // These functions are operators
  // NOTE: the range of the two operands should not conflict: the common
  // variable should have the same range
  tldm_polynomial operator+(const tldm_polynomial &oprand2) const;
  tldm_polynomial operator*(const tldm_polynomial &oprand2) const;
  tldm_polynomial operator/(const tldm_polynomial &operand2) const;
  tldm_polynomial operator-() const;
  tldm_polynomial operator-(const tldm_polynomial &oprand2) const;
  bool operator==(const tldm_polynomial &oprand2) const;

  //=======================================================================//
  // These functions deal with invalidation
  // valid flag is used to mark the cases that the expression can not be
  // expressed as a polynomial, e.g. during division
  int is_valid() const { return m_is_valid; }
  void set_invalid() { m_is_valid = 0; }

 protected:
  // use set_invalid to stop parsing complex cases, and propagate using "if
  // (!a.is_valid()) b.set_invalid();"
  int parse_polynomial(string exp);

  // ZP: replace the constant coefficient in a term with the new constant
  void set_constant_coefficient(int term, int val);
  int get_constant_coefficient(int term);

  // construct a polynomial from a single term
  tldm_polynomial construct_polynomial_from_term(
      map<string, int> *term, map<string, tldm_variable_range> *record) const;

 protected:
  // these structures are not supposed to be exposed outside this class
  // valid flag is used to mark the cases that the expression can not be
  // expressed as a polynomial, e.g. during division
  int m_is_valid;

  // we do not need to separate iterators and parameters here.
  vector<string> m_variables;
  map<string, tldm_variable_range> m_var_ranges;

  // exp  = (term0, term1, term2, ...)
  // term = (var0,var1,.. -> radix0,radix1,...)
  // e.g. M*M*N*i       + 3*M*N*j           + N*k         + l     + M*N*M     +
  // 2*M       + 256 (    (M,N,i->2,1,1), (M,N,j,3->1,1,1,1), (N,k->1,1),
  // (l->1), (M,N->2,1), (2,M->1,1), (256->1))
  vector<map<string, int>> m_term_and_radix;

  // For those terms that we do not supported, we introduce an inducted term
  // which represents the non-polynomial expression, so that the remaining term
  // are polynomials NOTE:
  // 1. the induction only happens in parsing and division
  // 2. the range calculation will also be done during the induction
  map<string, string>
      map_var2exp;  // map the new variable to the non-polynomial expression
};

// int test_string_is_int(string str, int &value);
int test_string_is_int(string str);

// END: This range is for Muhuan to edit
/////////////////////////////////////////////////////////////////////////

//! Polyhedral model in vector
/** A PolyVector is used to describe a vector.
 *
 *    PolyVector encapsulates the operations of integer vectors.
 *
 *    PolyVectors are used to express
 *
 *    (1) dependence distance
 *
 *    (2) reuse distance
 *
 *    ...
 */
class PolyVector {
 public:
  enum { OP_EQ = 0, OP_GE, OP_GT, OP_NE };
  static const int BOUND_MAX = 2 ^ 30;

  static const char *CONST_POINTER;      // 0
  static const char *OP_FLAG_POINTER;    // ((long*)(0));
  static const char *POSITIVE_INFINITE;  //
  static const char *LEXIGRAPHIC_DELTA;
  static const char *FIXED_CONDITION;

  string print();
  string print_var();
  string print_expression();  // print the expression of sVar
  string print_condition();   // print the equality or inequality

  explicit PolyVector(int val = 1);
  explicit PolyVector(string var, int val = 1);
  PolyVector(const PolyVector &polyvec);

  PolyVector operator+(PolyVector oprand2);
  PolyVector operator*(PolyVector oprand2);
  PolyVector operator/(int oprand2);
  PolyVector operator-();
  PolyVector operator-(PolyVector oprand2);

  bool operator==(PolyVector oprand2);

  PolyVector operator!();

  int level();
  int nonzero_level();
  int get_const();
  int get_coeff(string var);
  int get_cond();
  int get_coeff(int i) { return m_coeffs[i]; }
  int is_const() {
    vector<string> vec;
    get_nonzero_vars(&vec);
    return vec.size() == 0;
  }

  void set_coeff(string var, int coeff);

  void normalize_coeff(string var);

  void align(PolyVector *oprand3);

  void sort(vector<string> *var_order);
  void sort(PolyVector *vec_order);

  void get_vars(vector<string> *vec_vars);
  void get_nonzero_vars(vector<string> *vec_vars);
  vector<string> get_nonzero_vars();
  vector<string> get_vars_no_const();
  int get_vars_num();
  void rename_var(string old_var, string new_var);

  int get_iterator_num() { return m_iterator_num; }
  int get_param_num() { return get_vars_num() - m_iterator_num; }
  void set_iterators(vector<string> *iterators);
  vector<string> get_iterators();
  vector<string> get_parameters();

  void clear() {
    m_vars.clear();
    m_coeffs.clear();
  }

  int isTrue();

  void reduce_coefficient();
  int substitude(string var, PolyVector exp);

#ifdef POLYMODEL_SUPPORT_ISL
  struct isl_constraint *get_isl_set_constraint(isl_ctx *ctx, int n_dim);
  struct isl_constraint *get_isl_map_constraint(isl_ctx *ctx, int n_in,
                                                int n_out);
#endif

 protected:
  void append(string var, int value) {
    int idx = m_coeffs.size();
    m_vars[var] = idx;
    m_coeffs.push_back(value);
  }

 protected:
  int m_iterator_num;
  map<string, int> m_vars; /*!< \brief string -> its position (idx) */
  vector<int> m_coeffs;
  // !< \brief coefficient values (m_coeffs[m_vars["var"]]),
  // present lexicographic order after sorting by sort()
};

class PolyMatrixSet;

//! Polyhedral model in matrix
/** A PolyMatrix describes a polyhedron with or without boundary. Each row of
 *the matrix is a constraint or a face of the polyhedron. The first column
 *indicates the relative operator, such as eq, gt, ge, lt, le. From the second
 *column to the last but one column corresponds to variables, such as iteration
 *variables and parameters (iv precedes parameters). And the last column is
 *constant, which is also at the left side of the equation.
 *
 *    PolyMatrix and PolyVector encapsulates the operations of affine
 *constraints.
 *
 *    PolyMatrices are used to express
 *
 *    (1) iteration domain: (iv, parameter)
 *
 *    (2) array domain: (idx)
 *
 *    (3) array reference function: (iv, idx)
 *
 *    (4) dependence function: (iv, iv)
 *
 *    ...
 */
class PolyMatrix {
 public:
  void append_vector(PolyVector row);
  void append_matrix(PolyMatrix mat);
  string print();
  string print_expression(string sVar);  // find the expression by detecting the
                                         // first equation with the variable
  void sort(vector<string> *var_order);
  void set_iterators(vector<string> *iterators);
  vector<string> get_iterators();
  vector<string> get_parameters();
  void rename_var(string old_var, string new_var);

  void get_vars(vector<string> *vec_vars);
  vector<string> get_vars() {
    vector<string> vec_vars;
    get_vars(&vec_vars);
    return vec_vars;
  }
  int get_vars_num();

  PolyMatrixSet operator!();
  PolyMatrix operator&(PolyMatrix mat2);

  PolyVector &GetVector(int i) { return m_mat[i]; }
  unsigned int size() { return m_mat.size(); }

  // Return the low/upper bounds of a variable
  PolyMatrix GetLowerBoundVector(string sVar);
  PolyMatrix GetUpperBoundVector(string sVar);

  int GetUpperBoundSimple(string sVar);
  int GetLowerBoundSimple(string sVar);

 public:
#ifdef POLYMODEL_SUPPORT_ISL
  struct isl_basic_set *get_isl_basic_set(isl_ctx *ctx, int n_dim);
  struct isl_basic_map *get_isl_basic_map(isl_ctx *ctx, int n_in, int n_out);

  // Operations
  int ProjectOut(string anode);
  int Simplify();
#endif  // POLYMODEL_SUPPORT_ISL

  int get_iter_bitwidth(string sVar) { return 32; }
  int get_iter_total_bitwidth();

  PolyMatrix remove_redundant_variables();
  void remove_redundant_vector();

  // for back-annotation, added by zhangpeng 2013-06-20
 public:
  vector<string> &get_inner_iterators() { return m_inner_iterators; }

 protected:
  vector<string>
      m_inner_iterators;  // can be considered as a special kind of parameters

 protected:
  vector<PolyVector> m_mat;
};

//! Union of polytopes
/** ...
 *
 */
class PolyMatrixSet {
 public:
  void append_matrix(PolyMatrix mat);
  void clear();
  string print();
  void sort(vector<string> *var_order);
  void set_iterators(vector<string> *iterators);
  vector<string> get_iterators();
  vector<string> get_parameters();

  void get_vars(vector<string> *vec_vars, int idx);
  int get_vars_num();

  PolyMatrixSet operator|(PolyMatrixSet matset2);
  PolyMatrixSet operator&(PolyMatrixSet matset2);
  PolyMatrixSet operator!();

  vector<PolyMatrix> &get_matrix_set() { return m_matset; }
#ifdef POLYMODEL_SUPPORT_ISL
  struct isl_set *get_isl_set(isl_ctx *ctx, int n_dim);
  struct isl_map *get_isl_map(isl_ctx *ctx, int n_in, int n_out);
#endif  // POLYMODEL_SUPPORT_ISL
 protected:
  vector<PolyMatrix> m_matset;
};

class tldm_polyhedral_info {
 public:
  tldm_polyhedral_info() {
    outer_level = -1;
    gid_level = -1;
  }

  string name;  // for both task and port
  // string ref_name;
  string type;

  string iterator_vector;   // "i,j,k"
  string parameter_vector;  // "M,n,P"
  string iterator_range;    // "0..M-1,0..N-1,0..P-1"
  string parameter_range;   // "32:512,1:1:1"
  string order_vector;      // "0,tp,0,tn,0,tm,0,pp,0,nn,0,mm,0"
  string condition_vector;  // "1,exit_cond,exe_cond,1,1,1,1,1,1,1,1,1,1"
  string access_pattern;    // "r:expression" or "w:expression"

  map<string, string> properties;  // additional information, they are:
  // 1. ref_name; used to indicate the references in the source code

  int get_loop_level() {
    vector<string> vecIter = str_split(iterator_vector, ',');
    return vecIter.size();
  }

  vector<tldm_variable_range> get_parameter_range();
  vector<tldm_variable_range> get_iterator_range();
  vector<tldm_variable_range> get_variable_range();

  string to_string() {
    string str;
    //         if ("" != name            ) str += "name=\""             + name +
    // "\" ";         if ("" != type            ) str += "type=\""             +
    // type
    // +
    // "\" ";         if ("" != iterator_vector ) str += "iterator_vector=\""  +
    // iterator_vector  + "\" ";         if ("" != parameter_vector) str +=
    // "parameter_vector=\"" + parameter_vector + "\" ";         if ("" !=
    // iterator_range  ) str += "iterator_range=\""   + iterator_range   + "\"
    // ";         if ("" != parameter_range ) str += "paramter_range=\""   +
    // parameter_range  + "\" ";         if ("" != order_vector    ) str +=
    // "order_vector=\""     + order_vector     + "\" ";         if ("" !=
    // condition_vector) str += "condition_vector=\"" + condition_vector + "\"
    // ";         if ("" != access_pattern  ) str += "access_pattern=\""   +
    // access_pattern   + "\" ";

    map<string, string>::iterator it;
    for (it = properties.begin(); it != properties.end(); it++) {
      if ("additional_tunning_string" == it->first) {
        str += it->second;
      } else if ("" != it->second) {
        string value_exp = it->second;
        str += it->first + "=\"" + value_exp + "\" ";
      }
    }

    return str;
  }

 public:
  int outer_level;  // -1: to be determined later; 0, 1, 2, ... : number of
                    // "outer" loops outside tldm graph
  int gid_level;  // -1: to be determined later; 0, 1, 2, ... : number of "tldm"
                  // loops between tldm graph and tldm task
};

tldm_polyhedral_info tldm_polyinfo_merge(tldm_polyhedral_info task,
                                         tldm_polyhedral_info access);

struct poly_access_pattern {
  enum { READ, WRITE, READWRITE };
  void *sRef;
  string sArray;
  int nDim;  // 0 for scalar, 1 for 1-D pointer, 2 for 2-D, ...
  vector<int> dim_size;
  int nDir;
  string sIterators;
  string iter_list;
  string up_list;
  string sPortType;
  PolyMatrix matrix;
  string sDataType;
  string sOrderVec;
  string access_pattern;
  // added for back-annotation
  vector<string> inner_iterators;
};

PolyMatrix generate_constraints_from_boundary(string var, PolyVector *vec_lb,
                                              PolyVector *vec_ub,
                                              int reach_ub = 1);

PolyMatrix CombineAccessConstraints(PolyMatrix domain, PolyMatrix order,
                                    PolyMatrix access, string sArray,
                                    string sSuffix);
PolyMatrix GetDependence(string sArray, PolyMatrix domain0, PolyMatrix order0,
                         PolyMatrix access0, PolyMatrix domain1,
                         PolyMatrix order1, PolyMatrix access1,
                         int skip_reduce = 0);
PolyMatrix GetLoopBounds(PolyMatrix domain, PolyMatrix order);

vector<string> merge_iterator(vector<string> vec0, vector<string> vec1);

void ConstraintReduce(PolyMatrix *constraints, string var);

void GetDeepBounds(const PolyMatrix &_constraints, string sVar,
                   int *upper_bound, int *lower_bound);
// int GetDeepBounds_LP(PolyMatrix &constrains, string sVar, int &upper_bound,
//                      int &lower_bound);
void ConstraintReduceDeep(PolyMatrix *constraints, string var);

void ConstraintReduceEquality(PolyMatrix *constraints, string var);

// used for new TLDM format (2013-09-30)
string DependenceAnalysisTLDMFormat(tldm_polyhedral_info ref0,
                                    tldm_polyhedral_info ref1, int outer_level,
                                    int gid_level);
