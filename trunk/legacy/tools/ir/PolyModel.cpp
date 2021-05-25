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


#include "PolyModel.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <set>
#include <iostream>
#include <algorithm>
#include "file_parser.h"

// #include "glpk.h"
#include "math.h"

using std::istringstream;
using std::set;
using std::stringstream;

// #define DEBUG_GETDEPENDENCE 1
#define DEBUG_GETDEPENDENCE 0
#define DEBUG_GETDEPENDENCE_DETAIL 0

// #define PRINT_DEBUG_POLY_DIV
// #define PRINT_DEBUG_POLY_DIV_RECUR

#define ASSERT_AT_INVALID 1

// check if a input term is constant; if positive const, return 1;
// if negative, return -1; if not constant, return 0
static int constant_term_check(string input);

int sign(int a) { return (a >= 0) ? 1 : -1; }

int sign1(int a) { return (a > 0) ? 1 : (a < 0) ? -1 : 0; }

//////////////////////////////////////////////////////////////////////////
// BEGIN: This range is for Muhuan to edit

int tldm_polynomial::is_polynomial(string exp) {
  string type;
  string left_exp;
  string right_exp;
  string op;
  string sub_exp;
  int ret = tldm_polynomial::is_element(exp);
  if (ret != 0) {
    return 1;
  }

  ret = tldm_polynomial::is_unary_op(exp, &sub_exp, &op);
  if (ret != 0) {
    return tldm_polynomial::is_polynomial(sub_exp);
  }

  ret = tldm_polynomial::is_binary_op(exp, &left_exp, &right_exp, &op);
  if (ret != 0) {
    return static_cast<int>((static_cast<int>(tldm_polynomial::is_polynomial(
                                                  left_exp) != 0) != 0) &&
                            (tldm_polynomial::is_polynomial(left_exp)) != 0);
  }

  return 0;
}

int tldm_polynomial::parse_polynomial(string exp) {
  if (exp.empty()) {
    exp = "0";
  }

  string type;
  string left_exp;
  string right_exp;
  string op;
  string sub_exp;
  int ret = is_brace(exp);
  if (ret != 0) {
    tldm_polynomial brace_poly(exp.substr(1, exp.size() - 2));
    *this = brace_poly;
    return 1;
  }

  ret = is_element(exp);
  if (ret != 0) {
    // do things for element ...
    // m_variables ...

    // Fixed by Muhuan 11/14/13
    // if (ret == 1) // is integer
    //    m_variables.push_back("");
    // else
    //    m_variables.push_back(exp);

    // m_term_and_radix ...

    if (ret != 1) {  // is not integer
      m_variables.push_back(exp);
    }

    // ZP: 2013-11-14
    if (exp != "0") {
      map<string, int> mst;
      mst[exp] = 1;
      // Muhuan 11/20/13, add term "1" for string
      if (ret != 1) {
        mst["1"] = 1;
      }
      m_term_and_radix.push_back(mst);
    }
    return 1;
  }

  ret = is_unary_op(exp, &sub_exp, &op);
  if (ret != 0) {
    tldm_polynomial sub_poly(sub_exp);
    // do things for unary ...
    if (op == "+") {
      *this = sub_poly;
    } else if (op == "-") {
      tldm_polynomial empty_poly("0");
      *this = empty_poly - sub_poly;
    }
    return 1;
  }

  ret = is_binary_op(exp, &left_exp, &right_exp, &op);
  if (ret != 0) {
    // do things for binary ...
    tldm_polynomial left_poly(left_exp);
    tldm_polynomial right_poly(right_exp);

    if (op == "+") {
      *this = left_poly + right_poly;
    } else if (op == "-") {
      *this = left_poly - right_poly;
    } else if (op == "*") {
      *this = left_poly * right_poly;
    }
    //        DO NOT support op "/" in parsing
    //        set invalid??
    //        else if (op == "/")
    //            *this = left_poly / right_poly;

    return 1;
  }
  return 0;
}

//=======================================================================//
// These basic functions can be used directly as
// tldm_polynomial::parse_element("2*i"); They do not need to use recursive
// function, but directly textual processing

// 0: not/unknown; 1: integer(0x[0-9]L|UL); 2: variable([a-z|A-Z|_]);
int tldm_polynomial::is_element(string exp) {
  //    regex reg_number("(0x)?[\d]+(L|(UL))?");
  //    regex reg_string("[a-zA-Z_][a-zA-Z_0-9]*", icase);
  //
  //    if (regex_match(exp, reg_number) || regex_match(exp, reg_string))
  //        return 1;
  //    else
  //        return 0;

  if (exp.empty()) {
    return 0;
  }

  // try to parse integer
  string exp_int = exp;
  if (exp_int[0] == '+' || exp_int[0] == '-') {
    exp_int = exp_int.substr(1);
  }

  if (exp_int.size() >= 2) {
    string lead2 = exp_int.substr(0, 2);
    if (lead2 == "0x") {
      exp_int = exp_int.substr(2);
    }
  }
  bool fsm_int_digit = true;
  size_t i;
  for (i = 0; i < exp_int.size(); i++) {
    if (exp_int[i] > '9' || exp_int[i] < '0') {
      fsm_int_digit = false;
      break;
    }
  }
  if ((fsm_int_digit) || (i == (exp_int.size() - 1) && exp_int[i] == 'L') ||
      (i == (exp_int.size() - 2) && exp_int.substr(i) == "UL")) {
    return 1;
  }

  // try to parse variable
  char exp0 = exp[0];
  if ((exp0 <= 'z' && exp0 >= 'a') || (exp0 <= 'Z' && exp0 >= 'A') ||
      (exp0 == '_')) {
    if (exp.size() == 1) {
      return 2;
    }
    bool is_var = true;
    for (size_t i = 1; i < exp.size(); i++) {
      if ((exp0 <= 'z' && exp0 >= 'a') || (exp0 <= 'Z' && exp0 >= 'A') ||
          (exp0 == '_') || (exp0 <= '9' && exp0 >= '0')) {
      } else {
        is_var = false;
        break;
      }
    }
    if (is_var) {
      return 2;
    }
  }
  return 0;
}

// 0: not/unknown; 1: add; 2: sub; 3: mul; 4: div* (can be partially supported)
int tldm_polynomial::is_binary_op(string exp, string *left_exp,
                                  string *right_exp, string *op) {
  int left_brace_counter = 0;
  if (exp.size() < 7) {  // at least (.)*(.) -- 7 chars
    return 0;
  }

  if (exp[0] != '(') {
    return 0;
  }

  left_brace_counter = 1;
  size_t i;
  for (i = 1; i < exp.size(); i++) {
    if (exp[i] == '(') {
      left_brace_counter++;
    } else if (exp[i] == ')') {
      left_brace_counter--;
    } else {
      continue;
    }

    if (left_brace_counter == 0) {
      break;
    }
  }
  if (i == 1) {
    return 0;  // empty left_exp
  }
  *left_exp = exp.substr(1, i - 1);

  exp = exp.substr(i + 1);
  if (exp.size() < 4) {  // at least *(.) -- 7 chars
    return 0;
  }

  char op_tmp = exp[0];
  int ret_op;
  switch (op_tmp) {
  case '+':
    *op = "+";
    ret_op = 1;
    break;
  case '-':
    *op = "-";
    ret_op = 2;
    break;
  case '*':
    *op = "*";
    ret_op = 3;
    break;
  case '/':
    *op = "/";
    ret_op = 4;
    break;
  default:
    return 0;
  }

  exp = exp.substr(1);
  if (exp[0] != '(' || exp[exp.size() - 1] != ')') {
    return 0;
  }

  left_brace_counter = 1;
  for (i = 1; i < exp.size(); i++) {
    if (exp[i] == '(') {
      left_brace_counter++;
    } else if (exp[i] == ')') {
      left_brace_counter--;
    } else {
      continue;
    }

    if (left_brace_counter == 0) {
      break;
    }
  }
  if (left_brace_counter != 0 || i != exp.size() - 1) {
    return 0;
  }

  *right_exp = exp.substr(1, exp.size() - 2);
  return ret_op;
}

// 0: not/unknown; 1: minus; 2: plus
int tldm_polynomial::is_unary_op(string exp, string *sub_exp, string *op) {
  if (exp.size() < 4) {  // -(.) -- at least 4 characters
    return 0;
  }

  if (exp[1] != '(' || exp[exp.size() - 1] != ')') {
    return 0;
  }

  int ret_op;
  if (exp[0] == '-') {
    *op = "-";
    ret_op = 1;
  } else if (exp[0] == '+') {
    *op = "+";
    ret_op = 2;
  } else {
    return 0;
  }

  *sub_exp = exp.substr(2, exp.size() - 3);

  return ret_op;
}

int tldm_polynomial::is_brace(string exp) {
  if (exp.size() <= 2) {
    return 0;
  }

  if ((exp[0] != '(') || (exp[exp.size() - 1] != ')')) {
    return 0;
  }
  int unmatched_left_counter = 1;
  size_t i;
  for (i = 1; i < exp.size(); i++) {
    if (exp[i] == '(') {
      unmatched_left_counter++;
    } else if (exp[i] == ')') {
      unmatched_left_counter--;
    }

    if (unmatched_left_counter == 0) {
      break;
    }
  }
  if (unmatched_left_counter == 0 && i == exp.size() - 1) {
    return 1;
  }
  { return 0; }
}
//=======================================================================//
// These functions deal with variable list and their range

// m_variables is generated from parsing, not from setting outside
// so we will check if the generated variables are defined in some given set
// if any generate variable is not in the set, we should give error msg outside
int tldm_polynomial::match_variable(vector<string> defined_variables,
                                    vector<string> *undefined_variables) {
  // remove duplicas in defined_variables
  vector<string> no_dup_defined_var;
  for (size_t i = 0; i < defined_variables.size(); i++) {
    string dv = defined_variables[i];
    // if (std::find(no_dup_defined_var.begin(), no_dup_defined_var.end(), dv)
    // == no_dup_defined_var.end())
    if (-1 == str_vector_find(no_dup_defined_var, dv)) {
      no_dup_defined_var.push_back(dv);
    }
  }
  defined_variables = no_dup_defined_var;

  // has to be in m_variables
  undefined_variables->clear();
  for (vector<string>::iterator iter = defined_variables.begin();
       iter != defined_variables.end(); iter++) {
    string dv = (*iter);
    // if( find(m_variables.begin(), m_variables.end(), dv) ==
    // m_variables.end()) // did not find
    if (-1 == str_vector_find(m_variables, dv)) {
      undefined_variables->push_back(dv);
      // 11/20/13 Muhuan
      // defined_variables.erase(mv_iter); // better efficiency in next for()
      // loop
    }
  }

  // 11/20/13 Muhuan
  for (vector<string>::iterator iter = undefined_variables->begin();
       iter != undefined_variables->end(); iter++) {
    string dv = (*iter);
    vector<string>::iterator removed_iter =
        find(defined_variables.begin(), defined_variables.end(), dv);
    if (removed_iter != defined_variables.end()) {
      defined_variables.erase(removed_iter);
    }
  }

  // coeff in m_term_and_radix not 0
  for (vector<string>::iterator iter = defined_variables.begin();
       iter != defined_variables.end(); iter++) {
    string dv = (*iter);
    bool none_zero_coeff = false;
    for (vector<map<string, int>>::iterator vmsi_iter =
             m_term_and_radix.begin();
         vmsi_iter != m_term_and_radix.end(); vmsi_iter++) {
      map<string, int>::iterator msi_iter;
      msi_iter = (*vmsi_iter).find(dv);
      if (msi_iter != (*vmsi_iter).end() && msi_iter->second != 0) {
        none_zero_coeff = true;
        break;
      }
    }
    if (!none_zero_coeff) {
      undefined_variables->push_back(dv);
    }
  }

  if (undefined_variables->empty()) {
    return 1;
  }
  { return 0; }
}

int tldm_polynomial::set_range(vector<tldm_variable_range> vec_range) {
  // ZP: 2013-10-10: note that we need all the parameters, even if they only
  // exist in the range TOFIX: HERE IS A MISUNDERSTANDING
  //      all-define means the variables in the expressions are all in
  //      vec_range, which means we should scan m_variables instead.

  for (vector<tldm_variable_range>::iterator iter = vec_range.begin();
       iter != vec_range.end(); iter++) {
    tldm_variable_range tvr = (*iter);

    vector<string>::iterator mv_iter;
    mv_iter = find(m_variables.begin(), m_variables.end(), tvr.variable);
    if (mv_iter == m_variables.end()) {
      m_variables.push_back(tvr.variable);
    }
    m_var_ranges[tvr.variable] = tvr;
  }

  bool alldefined = true;
  for (vector<string>::iterator iter1 = m_variables.begin();
       iter1 != m_variables.end(); iter1++) {
    int found = 0;
    for (vector<tldm_variable_range>::iterator iter2 = vec_range.begin();
         iter2 != vec_range.end(); iter2++) {
      if (*iter1 == iter2->variable) {
        found = 1;
        break;
      }
    }

    if (found == 0) {
      alldefined = false;
    }
  }

  return static_cast<int>(alldefined);
}

vector<tldm_variable_range> tldm_polynomial::get_range() {
  vector<tldm_variable_range> vtvr;
  for (map<string, tldm_variable_range>::iterator it = m_var_ranges.begin();
       it != m_var_ranges.end(); it++) {
    vtvr.push_back(it->second);
  }

  return vtvr;
}
int tldm_polynomial::rename_variable(string var_old, string var_new) {
  // update m_variables
  vector<string>::iterator mv_iter;
  mv_iter = find(m_variables.begin(), m_variables.end(), var_old);
  if (mv_iter == m_variables.end()) {
    return 0;
  }
  m_variables.erase(mv_iter);
  m_variables.push_back(var_new);

  // update m_var_range
  map<string, tldm_variable_range>::iterator tvr_iter;
  tvr_iter = m_var_ranges.find(var_old);
  if (tvr_iter == m_var_ranges.end()) {
    // return 0;
  } else {
    tldm_variable_range new_tvr;
    new_tvr.variable = var_new;
    new_tvr.lb = tvr_iter->second.lb;
    new_tvr.ub = tvr_iter->second.ub;

    m_var_ranges.erase(tvr_iter);
    m_var_ranges[var_new] = new_tvr;
  }

  // TODO(youxiang): update on m_term_and_radix
  vector<map<string, int>> bak_term_and_radix(m_term_and_radix);
  m_term_and_radix.clear();

  for (vector<map<string, int>>::iterator vmsi_iter =
           bak_term_and_radix.begin();
       vmsi_iter != bak_term_and_radix.end(); vmsi_iter++) {
    map<string, int> term = *vmsi_iter;

    map<string, int>::iterator msi_iter = term.find(var_old);
    if (msi_iter != term.end()) {
      int pow = term[var_old];
      term.erase(msi_iter);
      term[var_new] = pow;
    }
    m_term_and_radix.push_back(term);
  }

  return 1;
}

int test_string_is_int(string str, int *value) {
  // TODO(youxiang): "L" && "UL" is not parsed!!
  // TODO(youxiang): return 0 if a string is not int!!
  bool neg_sign = false;
  if (str[0] == '-') {
    neg_sign = true;
    str = str.substr(1);
  } else if (str[0] == '+') {
    str = str.substr(1);
  }

  bool hex = false;
  if (str.substr(0, 2) == "0x") {
    hex = true;
  }

  for (size_t i = (hex ? 2 : 0); i < str.size(); i++) {
    if (str[i] > '9' || str[i] < '0') {
      return 0;
    }
  }

  if (hex) {
    *value = my_atoi_hex(str);
  } else {
    *value = my_atoi(str);
  }

  if (neg_sign) {
    *value = 0 - *value;
  }

  return 1;
}
int test_string_is_int(string str) {
  // TODO(youxiang): "L" && "UL" is not parsed!!
  // TODO(youxiang): return 0 if a string is not int!!
  if (str[0] == '-') {
    str = str.substr(1);
  } else if (str[0] == '+') {
    str = str.substr(1);
  }

  if (str.substr(0, 2) == "0x") {
    str = str.substr(2);
  }

  for (size_t i = 0; i < str.size(); i++) {
    if (str[i] > '9' || str[i] < '0') {
      return 0;
    }
  }

  return 1;
}
//=======================================================================//
// These functions calculate bounds, reduction and substitute
// return 0 if unbounded, 1 for success
int tldm_polynomial::get_lower_bound_const(int *bound) {
  int lb;
  int ub;
  int ret = get_bound_const(&lb, &ub);
  *bound = lb;
  return ret;
}
int tldm_polynomial::get_upper_bound_const(int *bound) {
  int lb;
  int ub;
  int ret = get_bound_const(&lb, &ub);
  *bound = ub;
  return ret;
}
int tldm_polynomial::get_bound_const(int *lower_bound, int *upper_bound) {
  int sum_lb = 0;
  int sum_ub = 0;

  // FIXED BY ZP: 2013-10-10
  // if (m_term_and_radix.size() == 0)
  //    return 0;
  if (m_term_and_radix.empty()) {
    *lower_bound = *upper_bound = 0;
    return 1;
  }
  //////////////////////////////////////////////////////////////////////////

  for (vector<map<string, int>>::iterator vmsi_iter = m_term_and_radix.begin();
       vmsi_iter != m_term_and_radix.end(); vmsi_iter++) {
    map<string, int> msi = *vmsi_iter;
    int term_ub;
    int term_lb;
    term_ub = term_lb = 1;
    for (map<string, int>::iterator msi_iter = msi.begin();
         msi_iter != msi.end(); msi_iter++) {
      string str = msi_iter->first;
      int power = msi_iter->second;
      int str_ub;
      int str_lb;
      int single_term_ub;
      int single_term_lb;

      // get range of variables
      map<string, tldm_variable_range>::iterator mst_find =
          m_var_ranges.find(str);
      if (mst_find != m_var_ranges.end()) {
        int ub_ub;
        int ub_lb;
        int lb_ub;
        int lb_lb;
        vector<tldm_variable_range> current_range = this->get_range();

        tldm_polynomial ub_poly(mst_find->second.ub, current_range);
        // tldm_polynomial ub_poly(mst_find->second.ub);
        ub_poly.get_bound_const(&ub_lb, &ub_ub);

        tldm_polynomial lb_poly(mst_find->second.lb, current_range);
        // tldm_polynomial lb_poly(mst_find->second.lb);
        lb_poly.get_bound_const(&lb_lb, &lb_ub);

        str_ub = ub_ub;
        str_lb = lb_lb;
      } else {
        int value = 0;

        // ZP: fixed 2013-10-30
        // assert(test_string_is_int(str));
        if (test_string_is_int(str) == 0) {
          *lower_bound = *upper_bound = 0;
          return 0;
        }

        test_string_is_int(str, &value);  // constant term
        str_ub = value;
        str_lb = value;
      }

      assert(!(str_ub > 0 &&
               str_lb < 0));  // do not support range [-x, y], x>=0, y>=0

      // get range of single_term
      single_term_lb = static_cast<int>(pow(str_lb, power));
      single_term_ub = static_cast<int>(pow(str_ub, power));
      if (str_ub <= 0 && str_lb <= 0) {
        if (power % 2 == 0) {
          single_term_lb = static_cast<int>(pow(str_ub, power));
          single_term_ub = static_cast<int>(pow(str_lb, power));
        }
      }

      // ZP: FIXED 2013-10-11
      int pre_term_ub = term_ub;
      int pre_term_lb = term_lb;

      // get range of product of single_term
      if (term_ub >= 0 && term_lb >= 0 && single_term_ub >= 0 &&
          single_term_lb >= 0) {
        // ZP: FIXED 2013-10-11
        // term_ub = term_ub * single_term_ub;
        // term_lb = term_lb * single_term_lb;
        term_ub = pre_term_ub * single_term_ub;
        term_lb = pre_term_lb * single_term_lb;
      } else if (term_ub >= 0 && term_lb >= 0 && single_term_ub <= 0 &&
                 single_term_lb <= 0) {
        term_ub = pre_term_lb * single_term_ub;
        term_lb = pre_term_ub * single_term_lb;
      } else if (term_ub <= 0 && term_lb <= 0 && single_term_ub >= 0 &&
                 single_term_lb >= 0) {
        term_ub = pre_term_ub * single_term_lb;
        term_lb = pre_term_lb * single_term_ub;
      } else if (term_ub <= 0 && term_lb <= 0 && single_term_ub <= 0 &&
                 single_term_lb <= 0) {
        term_ub = pre_term_lb * single_term_lb;
        term_lb = pre_term_ub * single_term_ub;
      } else {
        assert(0);
      }

      assert(sign1(term_ub) * sign1(term_lb) >= 0);
    }
    sum_lb += term_lb;
    sum_ub += term_ub;
  }
  *lower_bound = sum_lb;
  *upper_bound = sum_ub;
  return 1;
}

// ZP: 2013-10-10
tldm_polynomial
tldm_polynomial::get_lower_bound_by_reduction(string var_to_reduce) {
  tldm_polynomial minus_1("-1", get_range());
  tldm_polynomial neg = (*this) * minus_1;

  tldm_polynomial poly("1", get_range());
  poly = neg.get_upper_bound_by_reduction(var_to_reduce) * minus_1;
  return poly;
}

// ZP: 2013-10-10
tldm_polynomial
tldm_polynomial::get_upper_bound_by_reduction(string var_to_reduce) {
  tldm_polynomial bad_result("");
  bad_result.set_invalid();

  if (is_valid() == 0) {
    if (ASSERT_AT_INVALID) {
      assert(0);
    }
    return bad_result;
  }

  tldm_polynomial var_exp(var_to_reduce, get_range());
  int lb_var;
  int ub_var;
  var_exp.get_bound_const(&lb_var, &ub_var);
  if (lb_var < 0 && ub_var > 0) {
    if (ASSERT_AT_INVALID) {
      assert(0);
    }
    return bad_result;
  }

  if (m_var_ranges.find(var_to_reduce) == m_var_ranges.end()) {
    if (ASSERT_AT_INVALID) {
      assert(0);
    }
    return bad_result;
  }

  tldm_polynomial lb_var_poly(m_var_ranges[var_to_reduce].lb);
  tldm_polynomial ub_var_poly(m_var_ranges[var_to_reduce].ub);

  tldm_polynomial lb_var_poly_power("1");
  tldm_polynomial ub_var_poly_power("1");

  tldm_polynomial poly_bound("0", get_range());
  vector<tldm_polynomial> radix_coeff;
  int ret = get_polynomial_coefficients(var_to_reduce, &radix_coeff);
  if (ret == 0) {
    if (ASSERT_AT_INVALID) {
      assert(0);
    }
    return bad_result;
  }

  for (size_t i = 0; i < radix_coeff.size(); i++) {
    tldm_polynomial poly_term = radix_coeff[i];

    if (i == 0) {
      poly_bound = poly_bound + poly_term;
    } else {
      int lb_coeff;
      int ub_coeff;
      poly_term.get_bound_const(&lb_coeff, &ub_coeff);
      if (lb_coeff < 0 && ub_coeff > 0) {
        if (ASSERT_AT_INVALID) {
          assert(0);
        }
        return bad_result;
      }

      int coeff_pos = static_cast<int>(lb_coeff >= 0);
      int radix_odd = static_cast<int>(i % 2 == 1);

      if (lb_var >= 0 || ((coeff_pos ^ radix_odd) != 0)) {
        if (ub_coeff <= 0) {
          poly_bound = poly_bound + poly_term * lb_var_poly_power;
        } else if (lb_coeff >= 0) {
          poly_bound = poly_bound + poly_term * ub_var_poly_power;
        }
      } else {
        if (ub_coeff <= 0) {
          poly_bound = poly_bound + poly_term * ub_var_poly_power;
        } else if (lb_coeff >= 0) {
          poly_bound = poly_bound + poly_term * lb_var_poly_power;
        }
      }
    }

    lb_var_poly_power = lb_var_poly_power * lb_var_poly;
    ub_var_poly_power = ub_var_poly_power * ub_var_poly;
  }

  // TOFIX: range should not be reduced;
  poly_bound.set_range(get_range());

  return poly_bound;
}

tldm_polynomial
tldm_polynomial::get_polynomial_by_substitute(string var_to_replace,
                                              tldm_polynomial new_value) {
  vector<tldm_polynomial> coeff;
  tldm_polynomial old_value = tldm_polynomial(var_to_replace);
  if (get_polynomial_coefficients(var_to_replace, &coeff) != 0) {
    tldm_polynomial new_poly = tldm_polynomial("0");

    // for(size_t i = 0; i <= coeff.size(); i++)
    /******************
     * Fixed by Muhuan Oct 28, 2013
     */
    for (int i = static_cast<int>(coeff.size()) - 1; i >= 0; i--) {
      new_poly = new_poly * new_value;
      // cout << "in polymodel.cpp  " << coeff[i].exp_to_string() << endl;
      new_poly = new_poly + coeff[i];
    }
    new_poly.merge_term();
    return new_poly;
  }
  { assert(0); }
}

/* assign each variable a value, so that each term (w/o constant) will get a
 * unique value */
void get_tar_hash_table(const vector<map<string, int>> term_and_radix,
                        const vector<string> variables,
                        map<string, int> *hash_table) {
  hash_table->clear();
  int max_radix = 0;
  for (size_t i = 0; i < term_and_radix.size(); i++) {
    map<string, int> msi = term_and_radix[i];
    for (map<string, int>::iterator msi_iter = msi.begin();
         msi_iter != msi.end(); msi_iter++) {
      if (msi_iter->second > max_radix) {
        max_radix = msi_iter->second;
      }
    }
  }
  max_radix++;
  int hash_value = 1;
  for (int i = static_cast<int>(variables.size()) - 1; i >= 0; i--) {
    (*hash_table)[variables[i]] = hash_value;
    hash_value *= max_radix;
  }
}
int get_hash_value(map<string, int> msi, map<string, int> radix_hash_table) {
  int ret_value = 0;
  for (map<string, int>::iterator msi_iter = msi.begin(); msi_iter != msi.end();
       msi_iter++) {
    string var = msi_iter->first;
    int radix = msi_iter->second;

    if (test_string_is_int(var) == 0) {
      assert(radix_hash_table.find(var) != radix_hash_table.end());
      ret_value += radix_hash_table[var] * radix;
    }
  }
  return ret_value;
}
//  merge -xyz + xyz = 0
void tldm_polynomial::merge_term() {
  /*************************************
   * checking m_term_and_radix format  *
   *************************************/
  // FIX 11/15/13 Muhuan
  // (1) No empty m_term_and_radix
  for (vector<map<string, int>>::iterator btar_iter = m_term_and_radix.begin();
       btar_iter != m_term_and_radix.end(); btar_iter++) {
    if ((*btar_iter).empty()) {
      cout << "ERROR: empty term in the radix" << endl;
      assert(0);
    }
  }

  // 11/20/13 Muhuan
  // (2) each term has one and only one constant term
  for (vector<map<string, int>>::iterator btar_iter = m_term_and_radix.begin();
       btar_iter != m_term_and_radix.end(); btar_iter++) {
    map<string, int> tmp_msi = *btar_iter;
    int count = 0;
    for (map<string, int>::iterator msi_iter = tmp_msi.begin();
         msi_iter != tmp_msi.end(); msi_iter++) {
      if (test_string_is_int(msi_iter->first) != 0) {
        count++;
      }
    }
    if (count != 1) {
      cout << "ERROR: term in polynomial has more than one constant" << endl;
      assert(0);
    }
  }

  // 11/20/13 Muhuan
  // (3) radix should not be zero. if (radix is zero), change it to 1
  // TODO(youxiang): should not assert
  // for(vector<map<string, int> >::iterator btar_iter =
  // m_term_and_radix.begin();         btar_iter != m_term_and_radix.end();
  // btar_iter++)
  //{
  //    map<string, int> tmp_msi = *btar_iter;
  //    for(map<string, int>::iterator msi_iter = tmp_msi.begin(); msi_iter !=
  // tmp_msi.end(); msi_iter++)
  //    {
  //        if (msi_iter->second == 0)
  //        {
  //            cout << "ERROR: radix should not be zero" << endl;
  //            cout << this->exp_to_string() << "\t";
  //            cout << "term: " << msi_iter->first<< endl;
  //            assert(0);
  //        }
  //    }
  //}
  /*************************************
   * EOF checking *
   *************************************/

  map<string, int> radix_hash_table;
  get_tar_hash_table(m_term_and_radix, m_variables, &radix_hash_table);

  // for(map<string, int>::iterator rht_iter = radix_hash_table.begin();
  // rht_iter != radix_hash_table.end(); rht_iter++)     cout << "hash table: "
  // <<
  // rht_iter -> first << " --> " << rht_iter -> second << endl;

  // reset original m_term_and_radix
  vector<map<string, int>> bak_term_and_radix(m_term_and_radix);
  m_term_and_radix.clear();

  set<int> all_values;
  for (size_t i = 0; i < bak_term_and_radix.size(); i++) {
    all_values.insert(get_hash_value(bak_term_and_radix[i], radix_hash_table));
  }

  for (set<int>::iterator sii = all_values.begin(); sii != all_values.end();
       sii++) {
    int hashed_value = *sii;

    // constant value will get hashed_value = zero
    // if (hashed_value == 0) continue; // skip empty terms; only empty term
    // return 0

    int coeff = 0;
    map<string, int> cur_msi;
    // find all map that has the same hashed_value
    for (size_t i = 0; i < bak_term_and_radix.size(); i++) {
      if (get_hash_value(bak_term_and_radix[i], radix_hash_table) !=
          hashed_value) {
        continue;
      }
      cur_msi = bak_term_and_radix[i];

      // get coeff
      bool found_coeff = false;
      for (map<string, int>::iterator it = cur_msi.begin(); it != cur_msi.end();
           it++) {
        if (test_string_is_int(it->first) != 0) {
          int local_coeff = 0;
          test_string_is_int(it->first, &local_coeff);
          coeff += local_coeff;
          found_coeff = true;
          break;  // should have checked that a term has only one const coeff
        }
      }
      if (!found_coeff) {
        coeff = 1;
      }
    }
    // set coeff of cur_msi
    if (coeff != 0) {  // term gets removed if coeff = 0
      // remove original coeff
      for (map<string, int>::iterator it = cur_msi.begin(); it != cur_msi.end();
           it++) {
        if (test_string_is_int(it->first) != 0) {
          cur_msi.erase(it);
          break;  // should have checked that a term has only one const coeff
        }
      }
      // add new coeff
      cur_msi[my_itoa(coeff)] = 1;
      // add into m_term_and_radix
      m_term_and_radix.push_back(cur_msi);
    }
  }

  //////////////////////////////////////////////////////////////////////////
  // ZP: FIXME, here is only a workaround
  if (false) {
    // reset original m_term_and_radix
    map<string, int> cur_msi;
    size_t i;
    for (i = 0; i < bak_term_and_radix.size(); i++) {
      if (get_hash_value(bak_term_and_radix[i], radix_hash_table) == 0) {
        break;
      }
    }
    if (i < bak_term_and_radix.size()) {
      int coeff = 1;

      for (map<string, int>::iterator it = bak_term_and_radix[i].begin();
           it != bak_term_and_radix[i].end(); it++) {
        if (constant_term_check(it->first) != 0) {
          coeff *= my_atoi(it->first);
        }
      }

      cur_msi[my_itoa(coeff)] = 1;
      m_term_and_radix.push_back(cur_msi);
    }
  }
}
tldm_polynomial
tldm_polynomial::normalize_polynomial(vector<string> variables) {
  vector<string> undefined_var;
  assert(match_variable(variables, &undefined_var));

  map<string, int> radix_hash_table;
  get_tar_hash_table(m_term_and_radix, variables, &radix_hash_table);

  merge_term();

  map<int, int> mii;  // first: hash value, second: index in m_term_and_radix

  for (size_t i = 0; i < m_term_and_radix.size(); i++) {
    mii[get_hash_value(m_term_and_radix[i], radix_hash_table)] = i;
  }

  vector<map<string, int>> new_vmsi;
  for (map<int, int>::reverse_iterator mii_iter = mii.rbegin();
       mii_iter != mii.rend();
       mii_iter++) {  // map are sorted from small to large
    map<string, int> new_msi(m_term_and_radix[mii_iter->second]);
    new_vmsi.push_back(new_msi);
  }

  m_term_and_radix.clear();
  for (size_t i = 0; i < new_vmsi.size(); i++) {
    m_term_and_radix.push_back(new_vmsi[i]);
  }

  return (*this);
}

// END: This range is for Muhuan to edit
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// BEGIN: This range is for Libo to edit

//=======================================================================//
// These functions are for dumping and debugging

// Recursively add ( ) to form a string representation of a multiplication term
static string mul_recurse(string prefix, string var, int pow) {
  if (prefix.empty()) {
    if (pow <= 0) {
      return prefix;
    }
    if (pow == 1) {
      return var;
    }
    { return mul_recurse(var, var, pow - 1); }
  } else {
    if (pow <= 0) {
      return prefix;
    }
    { return mul_recurse("(" + prefix + ")*(" + var + ")", var, pow - 1); }
  }
}

static string mul_term_to_string(map<string, int> *term_ptr) {
  string result;
  map<string, int>::iterator it;
  for (it = term_ptr->begin(); it != term_ptr->end(); it++) {
    result = mul_recurse(result, it->first, it->second);
  }

  // ZP: 2013-11-14
  if (result.empty()) {
    result = "0";
  }
  //////////////////////////////////////////////////////////////////////////

  return result;
}

string tldm_polynomial::exp_to_string() {
  if (is_valid() == 0) {
    return "invalid polynomial";
  }
  string result;
  int term_count = m_term_and_radix.size();
  if (term_count == 0) {
    return "0";
  }

  // polynomial has at least one term.
  for (int i = 0; i < term_count; i++) {
    if (i == 0) {
      result = mul_term_to_string(&m_term_and_radix[i]);
    } else {
      result =
          "(" + result + ")+(" + mul_term_to_string(&m_term_and_radix[i]) + ")";
    }
  }
  return result;
}

string tldm_polynomial::vars_to_string() {
  if (is_valid() == 0) {
    return "invalid polynomial\n";
  }
  string result;
  size_t var_count = m_variables.size();
  for (size_t i = 0; i < var_count; i++) {
    if (!m_variables[i].empty()) {
      result += "\"" + m_variables[i] + "\" ";
    }
  }
  return result + "\n";
}

string tldm_polynomial::ranges_to_string() {
// ZP: 2013-10-10
#if 0
    if (!is_valid()) return "invalid polynomial\n";
    string result = "-- Start printing range --\n";
    int var_count = m_variables.size();
    for (int i = 0; i < var_count; i++) {
        if (m_variables[i] != ""
            && m_var_ranges.find(m_variables[i]) != m_var_ranges.end()) {
            tldm_variable_range *var_range_ptr = &m_var_ranges[m_variables[i]];
            result += var_range_ptr->variable + ": [" +
                      var_range_ptr->lb + ", " +
                  var_range_ptr->ub + "]\n";
        }
    }
    return result + "-- End printing range --\n";
#endif
  string result;

  for (map<string, tldm_variable_range>::iterator it = m_var_ranges.begin();
       it != m_var_ranges.end(); it++) {
    result += it->first + ":" + it->second.lb + ".." + it->second.ub + ",";
  }

  if (!result.empty()) {
    result = result.substr(0, result.size() - 1);  // remove the last ','
  }

  return result;
}

//=======================================================================//
// These functions deal with term_and_radix

// 0: not/unknown; 1: linear for all the variables, the variable may not exist
// in which the coefficient is zero
int tldm_polynomial::check_linear(vector<string> variables) {
  size_t check_var_count = variables.size();
  map<string, tldm_variable_range>::iterator it;
  // FIXME: do we want to perform merge_term() on caller? Currently no
  tldm_polynomial p(*this);
  p.merge_term();
  size_t term_count = p.m_term_and_radix.size();

  for (size_t i = 0; i < term_count; i++) {
    for (size_t j = 0; j < check_var_count; j++) {
      // found a term with power >= 1
      if (p.m_term_and_radix[i].find(variables[j]) !=
              p.m_term_and_radix[i].end() &&
          p.m_term_and_radix[i][variables[j]] != 1) {
        return 0;
      }
    }
  }
  return 1;
}

// return the same as check_linear, if success, coeffs are in the order of
// variables (variables can be iterators)
int tldm_polynomial::get_linear_coefficients(vector<string> variables,
                                             vector<tldm_polynomial> *coeffs) {
  size_t check_var_count = variables.size();
  vector<tldm_polynomial> new_coeff;
  for (size_t i = 0; i < check_var_count; i++) {
    tldm_polynomial current_coeff = get_linear_coefficients(variables[i]);
    if (current_coeff.is_valid() != 0) {
      new_coeff.push_back(current_coeff);
    } else {
      return 0;
    }
  }
  *coeffs = new_coeff;
  return 1;
}

tldm_polynomial tldm_polynomial::construct_polynomial_from_term(
    map<string, int> *term, map<string, tldm_variable_range> *record) const {
  tldm_polynomial retval("0");

  vector<string> variables_temp;
  map<string, tldm_variable_range> var_ranges_temp;
  vector<map<string, int>> term_temp;

  for (map<string, int>::iterator it = term->begin(); it != term->end(); it++) {
    string val = it->first;
    // is a variable
    if (constant_term_check(val) == 0) {
      variables_temp.push_back(val);
    }

    // can find range record
    if (record->find(val) != record->end()) {
      var_ranges_temp[val] = (*record)[val];
    }
  }

  term_temp.push_back(*term);

  retval.m_variables = variables_temp;

  // ZP: 2013-10-10
  // retval.m_var_ranges = var_ranges_temp;
  retval.m_var_ranges = *record;
  for (map<string, tldm_variable_range>::iterator itt = record->begin();
       itt != record->end(); itt++) {
    vector<string>::iterator mv_iter;
    mv_iter =
        find(retval.m_variables.begin(), retval.m_variables.end(), itt->first);
    if (mv_iter == retval.m_variables.end()) {
      retval.m_variables.push_back(itt->first);
    }
  }
  //////////////////////////////////////////////////////////////////////////

  retval.m_term_and_radix = term_temp;
  return retval;
}

struct VarInfo {
  map<string, int> coeff;
  int pow;
};

static void get_single_coefficient(map<string, int> *term, string var,
                                   VarInfo *retval) {
  if (term->find(var) == term->end()) {
    retval->coeff = *term;
    retval->pow = 0;
  } else {
    retval->pow = (*term)[var];

    // Muhuan: fixed 2013-11-20
    retval->coeff = *term;
    retval->coeff.erase(retval->coeff.find(var));

    if (retval->coeff.empty()) {
      retval->coeff["1"] = 1;
    }

    //    // ZP: fixed 2013-11-14
    //    retval.coeff = term;
    //    retval.coeff.erase(retval.coeff.find(var));
    //    //term.erase(term.find(var));
    //    //retval.coeff = term;
  }
  // return retval;
}

map<string, int> ccc;
tldm_polynomial bbb("");

tldm_polynomial tldm_polynomial::get_linear_coefficients(
    string var) {  // if nonlinear, return an invalid polynomial.
  tldm_polynomial retval("0");
  size_t term_count = m_term_and_radix.size();
  for (size_t i = 0; i < term_count; i++) {
    VarInfo coeff_info;
    get_single_coefficient(&m_term_and_radix[i], var, &coeff_info);

    // printf("pow: %d\n", coeff_info.pow);

    ccc = coeff_info.coeff;
    // cout << ccc << endl;

    // non-linear term
    if (coeff_info.pow > 1) {
      tldm_polynomial bad_result("");
      bad_result.set_invalid();
      return bad_result;
    }
    // linear term
    if (coeff_info.pow == 1) {
      tldm_polynomial current_term_p =
          construct_polynomial_from_term(&coeff_info.coeff, &m_var_ranges);

      // retval = retval.operator+(current_term_p);
      retval = retval + current_term_p;

      bbb = current_term_p;
    } else {
    }
  }

  return retval;
}

struct CompObj {
  bool operator()(const VarInfo &t1, const VarInfo &t2) {
    return t1.pow < t2.pow;
  }
};

// get the coefficient in the order of radix of var
// E.g. m*m*i+m*j+k+m*2+5 --(var=m)--> coeff=("k+5", "j+2", "i")
int tldm_polynomial::get_polynomial_coefficients(
    string var, vector<tldm_polynomial> *coeffs) {
  CompObj co;
  vector<VarInfo> temp_coeffs;
  size_t term_count = m_term_and_radix.size();
  for (size_t i = 0; i < term_count; i++) {
    VarInfo temp;
    get_single_coefficient(&m_term_and_radix[i], var, &temp);
    temp_coeffs.push_back(temp);
  }
  sort(temp_coeffs.begin(), temp_coeffs.end(), co);

  // sort temp single coefficients in ascending power
  vector<tldm_polynomial> retval;

  // iteratively add all terms with the same power of var
  int current_power = 0;
  tldm_polynomial current_p("0");
  for (size_t i = 0; i < term_count; i++) {
    // moved to a new power: push old one, move to higher power
    if (temp_coeffs[i].pow > current_power) {
      current_power++;
      retval.push_back(current_p);
      tldm_polynomial new_power_polynomial("0");
      current_p = new_power_polynomial;
      i--;  // stay at the same term
    } else {
      tldm_polynomial current_term_p =
          construct_polynomial_from_term(&temp_coeffs[i].coeff, &m_var_ranges);
      current_p = current_p.operator+(current_term_p);
    }
  }
  // after checking the last term, current_p contains coefficient for
  // highest power
  retval.push_back(current_p);

  *coeffs = retval;
  return 1;
}

//=======================================================================//

// Check if an input term is constant.
// Return 0 if not const, 1 for positive, -1 for negative
static int constant_term_check(string input) {
  size_t l = input.length();
  int retval = 1;
  for (size_t i = 0; i != l; i++) {
    // allow '-' at the beginning
    if (i == 0 && input[i] == '-') {
      retval = -1;
    } else if (input[i] - '0' > 9 || input[i] - '0' < 0) {
      retval = 0;
      break;
    } else {
    }
  }
  return retval;
}

// These functions are operators
// NOTE: the range of the two operands should not conflict: the common variable
// should have the same range
tldm_polynomial tldm_polynomial::
operator+(const tldm_polynomial &oprand2) const {
  tldm_polynomial result(*this);
  tldm_polynomial operand2(oprand2);
  if (!((result.is_valid() != 0) && (operand2.is_valid() != 0))) {
    tldm_polynomial bad_result("");
    bad_result.set_invalid();
    return bad_result;
  }

  // merge variable records
  size_t op2_var_count = operand2.m_variables.size();
  for (size_t i = 0; i < op2_var_count; i++) {
    string current_var = operand2.m_variables[i];
    if (!current_var.empty()) {
      if (find(result.m_variables.begin(), result.m_variables.end(),
               current_var) == result.m_variables.end()) {
        result.m_variables.push_back(current_var);
      }

      if (result.m_var_ranges.find(current_var) == result.m_var_ranges.end() &&
          operand2.m_var_ranges.find(current_var) !=
              operand2.m_var_ranges.end()) {
        result.m_var_ranges[current_var] = operand2.m_var_ranges[current_var];
      }
    }
  }

  // append multiplication terms from operand 2
  result.m_term_and_radix.insert(result.m_term_and_radix.end(),
                                 operand2.m_term_and_radix.begin(),
                                 operand2.m_term_and_radix.end());

  /*cerr << "    ** terms: "<< result.m_term_and_radix.size() << endl;
  cerr << "    ** "<< result.vars_to_string();
  cerr << "    ** "<< result.ranges_to_string();
  cerr << "    ** "<< result.exp_to_string() << endl;
  */
  // collect terms
  result.merge_term();
  /*cerr << "    ** terms: "<< result.m_term_and_radix.size() << endl;
  cerr << "    ** "<< result.vars_to_string();
  cerr << "    ** "<< result.ranges_to_string();
  cerr << "    ** "<< result.exp_to_string()<<endl;
  */
  return result;
}

#include <sstream>

string string_mul(string mag1, int sign1, string mag2, int sign2) {
  int m1;
  int m2;
  istringstream(mag1) >> m1;
  istringstream(mag2) >> m2;
  stringstream ss;
  ss << (m1 * m2 * sign1 * sign2);
  return ss.str();
}

map<string, int> multiply_term(const map<string, int> &input_t1,
                               const map<string, int> &t2) {
  map<string, int> t1 = input_t1;
  string const1;
  string mag1;
  int sign1;
  bool const1_found = false;
  for (map<string, int>::iterator it1 = t1.begin(); it1 != t1.end(); it1++) {
    string var1 = it1->first;
    int temp_sign1 = constant_term_check(var1);
    // const term
    if (temp_sign1 > 0) {
      sign1 = temp_sign1;
      mag1 = var1;
      const1 = var1;
      const1_found = true;
    } else if (temp_sign1 < 0) {
      sign1 = temp_sign1;
      mag1 = var1.substr(1);
      const1 = var1;
      const1_found = true;
    } else {
    }
  }
  if (!const1_found) {
    sign1 = 1;
    mag1 = "1";
  }

  string mag2;
  string const2;
  bool const2_found = false;
  int sign2;
  for (map<string, int>::const_iterator it2 = t2.begin(); it2 != t2.end();
       it2++) {
    string var2 = it2->first;
    // not a constant
    int temp_sign2 = constant_term_check(var2);
    if (temp_sign2 > 0) {
      sign2 = temp_sign2;
      mag2 = var2;
      const2_found = true;
      const2 = var2;
    } else if (temp_sign2 < 0) {
      sign2 = temp_sign2;
      mag2 = var2.substr(1);
      const2_found = true;
      const2 = var2;
    } else {
      int power2 = it2->second;
      // found variable in term 1
      if (t1.find(var2) != t1.end()) {
        int new_power = t1[var2] + power2;
        t1.erase(t1.find(var2));
        t1[var2] = new_power;
      } else {
        t1[var2] = power2;
      }
    }
  }
  if (!const2_found) {
    mag2 = "1";
    sign2 = 1;
  }

  // update power of constant term
  if (!const1_found) {
    if (const2_found) {
      t1[const2] = 1;
    }
  } else {
    if (const2_found) {
      t1.erase(t1.find(const1));
      string new_const = string_mul(mag1, sign1, mag2, sign2);
      t1[new_const] = 1;
    }
  }
  return t1;
}

tldm_polynomial tldm_polynomial::
operator*(const tldm_polynomial &oprand2) const {
  tldm_polynomial operand2(oprand2);
  if (!((is_valid() != 0) && (operand2.is_valid() != 0))) {
    tldm_polynomial bad_result("");
    bad_result.set_invalid();
    return bad_result;
  }

  tldm_polynomial zero("0");
  if (this->operator==(zero) || operand2.operator==(zero)) {
    return zero;
  }

  // setup return value
  tldm_polynomial retval("0");
  retval.m_variables = m_variables;
  retval.m_var_ranges = m_var_ranges;
  vector<map<string, int>> new_terms;

  // merge record
  size_t var_count_2 = operand2.m_variables.size();
  for (size_t i = 0; i < var_count_2; i++) {
    string current_var = operand2.m_variables[i];
    if (!current_var.empty()) {
      if (find(retval.m_variables.begin(), retval.m_variables.end(),
               current_var) == retval.m_variables.end()) {
        retval.m_variables.push_back(current_var);
      }

      if (retval.m_var_ranges.find(current_var) == retval.m_var_ranges.end() &&
          operand2.m_var_ranges.find(current_var) !=
              operand2.m_var_ranges.end()) {
        retval.m_var_ranges[current_var] = operand2.m_var_ranges[current_var];
      }
    }
  }
  // multiply term by term
  size_t len1 = m_term_and_radix.size();
  size_t len2 = operand2.m_term_and_radix.size();
  for (size_t i = 0; i < len1; i++) {
    for (size_t j = 0; j < len2; j++) {
      new_terms.push_back(
          multiply_term(m_term_and_radix[i], operand2.m_term_and_radix[j]));
    }
  }
  retval.m_term_and_radix = new_terms;
  retval.merge_term();
  return retval;
}

void tldm_polynomial::set_constant_coefficient(int term, int val) {
  for (map<string, int>::iterator it = m_term_and_radix[term].begin();
       it != m_term_and_radix[term].end(); it++) {
    if (constant_term_check(it->first) != 0) {
      m_term_and_radix[term].erase(it);
      break;
    }
  }
  if (val != 0) {
    (m_term_and_radix[term])[my_itoa(val)] = 1;
  }
}

int tldm_polynomial::get_constant_coefficient(int term) {
  for (map<string, int>::iterator it = m_term_and_radix[term].begin();
       it != m_term_and_radix[term].end(); it++) {
    if (constant_term_check(it->first) != 0) {
      return my_atoi(it->first);
    }
  }
  return 1;
}

vector<string> tldm_polynomial::get_nonzero_var() {
  vector<string> nonzero;
  tldm_polynomial zero("");

  //////////////////////////////////////////////////////////////////////////
  // FIXME !!! ZP: I do a copy because otherwise the value will be change in
  // merge_term
  tldm_polynomial copy(*this);

  copy.merge_term();
  size_t i;
  size_t j;
  for (i = 0; i < copy.m_variables.size(); i++) {
    string sVar = copy.m_variables[i];
    vector<tldm_polynomial> vec_poly;
    copy.get_polynomial_coefficients(sVar, &vec_poly);

    for (j = 1; j < vec_poly.size(); j++) {
      // ZP: 2013-11-13: It shoule be a typo and I fixed it.
      if (!(zero == vec_poly[j])) {
        nonzero.push_back(sVar);
      }
      // if (!(zero == vec_poly[i])) nonzero.push_back(sVar);
    }
  }
  return nonzero;
}

tldm_polynomial tldm_polynomial::
operator/(const tldm_polynomial &operand2) const {
  size_t i;
  size_t j;
  int ret;
  tldm_polynomial oprand1(*this);
  tldm_polynomial oprand2(operand2);
  tldm_polynomial bad_result("");
  bad_result.set_invalid();

#ifdef PRINT_DEBUG_POLY_DIV_RECUR
  {
    printf("\n Recursion: %s / %s.\n", oprand1.exp_to_string().c_str(),
           oprand2.exp_to_string().c_str());
  }
#endif

  if (!((is_valid() != 0) && (operand2.is_valid() != 0))) {
    return bad_result;
  }

  // ZP: 2013-11-14: Should not have this assumption
  // # ASSUMPTION 1: oprand2 has at most one variable
  // if (oprand2.get_nonzero_var().size() >= 2) {
  //    if (ASSERT_AT_INVALID) assert(0);
  //    return bad_result;
  //}

  // ASSUMPTION 2: oprand2 has at most one term
  vector<map<string, int>> &op2_term = oprand2.m_term_and_radix;
  if (op2_term.size() != 1) {
    if (ASSERT_AT_INVALID) {
      assert(0);
    }
    return bad_result;
  }

  // ASSUMPTION 3: the maximum radix of oprand 2 is one
  // string sVar="__UNKNOWN_VARIABLE_";
  vector<string> vec_sVars;
  for (i = 0; i < op2_term.size(); i++) {
    for (map<string, int>::iterator it = op2_term[i].begin();
         it != op2_term[i].end(); it++) {
      if (it->second >= 2) {
        if (ASSERT_AT_INVALID) {
          assert(0);
        }
        return bad_result;
      }
      // if (!constant_term_check(it->first)) sVar   = it->first;
      if (constant_term_check(it->first) == 0) {
        vec_sVars.push_back(it->first);
      }
    }
  }
  int nConst2 = oprand2.get_constant_coefficient(0);

  vector<map<string, int>> &op1_term = oprand1.m_term_and_radix;
  vector<tldm_variable_range> range0 = oprand1.get_range();

  int lb2;
  int ub2;
  oprand2.get_bound_const(&lb2, &ub2);

  // assert(lb2 * ub2 > 0);
  if ((lb2 >= 0 && ub2 <= 0) || (lb2 <= 0 && ub2 >= 0)) {
    // assert(lb2 * ub2 > 0);

    printf("\n\n[Dependence Calculation] Fail to calculate polynomial DIV: "
           "\n\t%s / %s.\n",
           oprand1.exp_to_string().c_str(), oprand2.exp_to_string().c_str());
    printf("\nVariable Ranges: \n\t%s\n", oprand2.ranges_to_string().c_str());
    printf(
        "\nInstruction: \n\tPlease set proper ranges of the parameters to make "
        "sure the sign of the divisor is determined and not zero.\n\n\n");

    if (ASSERT_AT_INVALID) {
      assert(0);
    }
    return bad_result;
  }

  int has_remaining = 0;  // see explanation in item 4
  int sign_remaining;     // see explanation in item 4

  for (i = 0; i < op1_term.size(); i++) {
    // 1. we process the terms in oprand1 one by one, and divided by oprand2
    //    the results are stored in oprand1 directly

#ifdef PRINT_DEBUG_POLY_DIV_RECUR
    {
      tldm_polynomial term =
          construct_polynomial_from_term(&op1_term[i], &oprand1.m_var_ranges);
      printf("Term %d: %s\n", i, term.exp_to_string().c_str());
    }
#endif

    int coeff = oprand1.get_constant_coefficient(i);
    int abs_coeff1 = coeff > 0 ? coeff : -coeff;
    int abs_coeff2 = nConst2 > 0 ? nConst2 : -nConst2;
    int const_dividable =
        static_cast<int>((abs_coeff1 / abs_coeff2 * abs_coeff2) == abs_coeff1);
    //        int var_dividable = ( op1_term[i].find(sVar) != op1_term[i].end()
    //        )
    //&&                             (op1_term[i])[sVar] >= 1;
    int var_dividable = 1;

    for (j = 0; j < vec_sVars.size(); j++) {
      if (op1_term[i].find(vec_sVars[j]) == op1_term[i].end()) {
        var_dividable = 0;
      }
      if ((op1_term[i])[vec_sVars[j]] < 1) {
        var_dividable = 0;
      }
    }

    if ((const_dividable != 0) && (var_dividable != 0)) {
      // 2 If the term is fully dividable (conservatively), just divided it
      for (j = 0; j < vec_sVars.size(); j++) {
        string sVar = vec_sVars[j];
        (op1_term[i])[sVar]--;
      }
      oprand1.set_constant_coefficient(i, coeff / nConst2);
    } else {
      // 3 If the term is not dividable, try to find whether the range of the
      // dividend falls into a fixed constant

      // 3.1 we first try to compare the absolute bound, which totally does not
      // consider the relation between terms
      if (lb2 <= 0 && ub2 >= 0) {
        if (ASSERT_AT_INVALID) {
          assert(0);
        }
        return bad_result;
      }
      int abs_lb2 = (lb2 > 0) ? lb2 : ub2;
      int abs_ub2 = (lb2 < 0) ? lb2 : ub2;

      int lb1;
      int ub1;
      tldm_polynomial term =
          construct_polynomial_from_term(&op1_term[i], &oprand1.m_var_ranges);
      term.get_bound_const(&lb1, &ub1);

      int abs_lb1 = ((lb1 >= 0 && ub1 <= 0) || (lb1 <= 0 && ub1 >= 0))
                        ? 0
                        : min(abs(lb1), abs(ub1));
      int abs_ub1 = max(abs(lb1), abs(ub1));

      int abs_lb = abs_lb1 / abs_ub2;
      int abs_ub = abs_ub1 / abs_lb2;

      vector<string> vec_curr_var;
      // vec_curr_var.push_back(sVar);
      vec_curr_var = vec_sVars;

      // ZP : 2013-11-14: fixed the bug to cause infinite recursion
      // vector<string> to_reduce = str_vector_sub(oprand1.m_variables,
      // vec_curr_var);
      vector<string> to_reduce =
          str_vector_sub(oprand1.get_nonzero_var(), vec_curr_var);

      if (abs_ub != abs_lb && !to_reduce.empty()) {
        // 3.2 If we fail to bounded with absolute bound, more aggressive
        // approach is tried to keep the relations
        //     of the variable existing in oprand2, it's done by two steps:
        //     1) Reduce the other variables, and obtain the symbolic
        //     lower/upper bounds of oprand1 2) Divide the bounds with oprand2
        //     recursively, and try to bound the division results
        // Note that by checking to_reduce set's emptiness, the convergence of
        // recursion is ensured.

        tldm_polynomial reduce_lb(term);
        tldm_polynomial reduce_ub(term);
        int ub_detemined = 0;
        int lb_detemined = 0;
        for (j = 0; j < to_reduce.size(); j++) {
          // printf("reduce[j=%d]: %s : %s\n", j,
          // reduce_lb.vars_to_string().c_str(), to_reduce[j].c_str());
          tldm_polynomial pre_reduce_ub(reduce_ub);
          tldm_polynomial pre_reduce_lb(reduce_lb);
          if (ub_detemined == 0) {
            reduce_ub = reduce_ub.get_upper_bound_by_reduction(to_reduce[j]);
          }
          if (lb_detemined == 0) {
            reduce_lb = reduce_lb.get_lower_bound_by_reduction(to_reduce[j]);
          }

          // Save reduction time if a constant bound has already been achieved.
          int curr_ub;
          int curr_lb;
          if (ub_detemined == 0) {
            reduce_ub.get_bound_const(&curr_lb, &curr_ub);
            if (curr_lb == curr_ub) {
              ub_detemined = 1;
            }
          }
          if (lb_detemined == 0) {
            reduce_lb.get_bound_const(&curr_lb, &curr_ub);
            if (curr_lb == curr_ub) {
              lb_detemined = 1;
            }
          }

          assert(reduce_lb.is_valid());
          assert(reduce_ub.is_valid());
          if (!((reduce_lb.is_valid() != 0) && (reduce_lb.is_valid() != 0))) {
            if (ASSERT_AT_INVALID) {
              assert(0);
            }
            return bad_result;
          }
        }

        int rdc_lb1;
        int rdc_ub1;
        reduce_lb.get_lower_bound_const(&rdc_lb1);
        reduce_ub.get_upper_bound_const(&rdc_ub1);

        // Use absolute value to calculate the result range of division
        tldm_polynomial poly_abs_lb1("0", oprand1.get_range());
        tldm_polynomial poly_abs_ub1("0", oprand1.get_range());
        tldm_polynomial poly_abs_lb2("0", oprand1.get_range());
        tldm_polynomial poly_abs_ub2("0", oprand1.get_range());

        if (lb2 > 0 && ub2 > 0) {
          poly_abs_lb2 = poly_abs_ub2 = operand2;
        } else {
          poly_abs_lb2 = poly_abs_ub2 = -operand2;
        }

        if (rdc_lb1 >= 0 && rdc_ub1 >= 0) {
          poly_abs_lb1 = reduce_lb;
          poly_abs_ub1 = reduce_ub;
        } else {
          poly_abs_lb1 = -reduce_ub;
          poly_abs_ub1 = -reduce_lb;
        }

        tldm_polynomial poly_abs_lb("0", oprand1.get_range());
        tldm_polynomial poly_abs_ub("0", oprand1.get_range());
        poly_abs_lb = poly_abs_lb1 / poly_abs_ub2;
        if (poly_abs_lb.is_valid() == 0) {
          if (ASSERT_AT_INVALID) {
            assert(0);
          }
          return bad_result;
        }
        poly_abs_ub = poly_abs_ub1 / poly_abs_lb2;
        if (poly_abs_ub.is_valid() == 0) {
          if (ASSERT_AT_INVALID) {
            assert(0);
          }
          return bad_result;
        }

        int rdc_abs_lb;
        int rdc_abs_ub;
        ret = poly_abs_lb.get_lower_bound_const(&rdc_abs_lb);
        assert(ret);
        ret = poly_abs_ub.get_upper_bound_const(&rdc_abs_ub);
        assert(ret);

        if (rdc_abs_lb != rdc_abs_ub) {
          printf("\n\n[Dependence Calculation] Fail to calculate polynomial "
                 "DIV: \n\t%s / %s.\n",
                 oprand1.exp_to_string().c_str(),
                 oprand2.exp_to_string().c_str());
          printf("\nVariable Ranges: \n\t%s\n",
                 poly_abs_ub.ranges_to_string().c_str());
          printf("\nInstruction: \n\tPlease set proper ranges of the "
                 "parameters to make sure the DIV result is a constant.\n\n\n");
          if (ASSERT_AT_INVALID) {
            assert(0);
          }
          return bad_result;
        }
        int const_val = rdc_abs_lb * sign(rdc_lb1 * lb2);
        has_remaining = 1;
        sign_remaining = sign(rdc_lb1 * lb2);

        map<string, int> new_term;
        new_term[my_itoa(const_val)] = 1;
        op1_term[i] = new_term;

      } else {
        int const_val = ub1 / lb2;
        has_remaining = 1;
        sign_remaining = sign(ub1) * sign(lb2);

        map<string, int> new_term;
        new_term[my_itoa(const_val)] = 1;
        op1_term[i] = new_term;
      }
    }
  }

  // 4. [IMPORTANT]
  // NOTE THAT: (x+y)/z is not equal to x/z + y/z, "/" means integer division
  // with rounding towards zero here 1) The term which is not fully dividable
  // will cause a +/- one difference in the result 2) The delta difference is
  // ignorable if it has the same direction with result 3) If their signs are
  // different, addi is applied to the division result to pull it towards zero
  tldm_polynomial zero("");
  if ((has_remaining != 0) && !(oprand1 == zero)) {
    int curr_lb;
    int curr_ub;
    ret = oprand1.get_bound_const(&curr_lb, &curr_ub);
    assert(ret);
    assert(sign(curr_lb) * sign(curr_ub) >= 0);
    int delta = sign_remaining;

    string addi;
    if (curr_lb * delta >= 0) {
      addi = "0";
    } else {
      addi = (delta > 0) ? "1" : "-1";
    }

    oprand1 = oprand1 + tldm_polynomial(addi);
  }
  oprand1.merge_term();
  return oprand1;
}

tldm_polynomial tldm_polynomial::operator-() const {
  if (is_valid() == 0) {
    tldm_polynomial bad_result("");
    bad_result.set_invalid();
    return bad_result;
  }
  tldm_polynomial result(*this);
  size_t term_count = result.m_term_and_radix.size();
  if (term_count != 0) {
    for (size_t i = 0; i < term_count; i++) {
      map<string, int> *current_term_ptr = &result.m_term_and_radix[i];
      string current_const_term;
      string negated_const_term;

      // get const term and compute negated const term
      for (map<string, int>::iterator it = current_term_ptr->begin();
           it != current_term_ptr->end(); it++) {
        int const_flag = constant_term_check(it->first);
        if (const_flag == 1) {
          current_const_term = it->first;
          negated_const_term = "-" + current_const_term;
          break;
        }
        if (const_flag == -1) {
          current_const_term = it->first;
          negated_const_term = current_const_term.substr(1);
          break;
        }
      }

      // const term != 1: remove old const, add new const
      if (!negated_const_term.empty()) {
        current_term_ptr->erase(current_term_ptr->find(current_const_term));
      } else {  // no old const to remove
        negated_const_term = "-1";
      }

      // cerr << "curr: "<< current_const_term << "; neg: " <<
      // negated_const_term << endl;

      // leave out const term if new result is 1
      // if (negated_const_term != "1")
      (*current_term_ptr)[negated_const_term] = 1;
    }
  }  // otherwise: polynomial == 0
  // cerr << result.exp_to_string();
  return result;
}

tldm_polynomial tldm_polynomial::
operator-(const tldm_polynomial &oprand2) const {
  tldm_polynomial n2 = -oprand2;
  return *this + n2;
}

static bool term_equal(map<string, int> *t1_ptr, map<string, int> *t2_ptr) {
  size_t t1_size = t1_ptr->size();
  size_t t2_size = t2_ptr->size();
  // term has the same number of variables/constants?
  if (t1_size != t2_size) {
    return false;
  }
  for (map<string, int>::iterator it = t1_ptr->begin(); it != t1_ptr->end();
       it++) {
    string var = it->first;
    // unmatched variable/constant
    if (t2_ptr->find(var) == t2_ptr->end()) {
      return false;
    }
    // different power
    if ((*t2_ptr)[var] != (*t1_ptr)[var]) {
      return false;
    }
  }

  return true;
}

bool tldm_polynomial::operator==(const tldm_polynomial &oprand2) const {
  // operator == shouldn't change original arguments
  tldm_polynomial p1(*this);
  tldm_polynomial p2(oprand2);

  // FIXME: ZP: 2013-11-14, this is just a workaround
  // return p1.exp_to_string() == p2.exp_to_string();

  p1.merge_term();
  p2.merge_term();

  int term_count_p1 = p1.m_term_and_radix.size();
  int term_count_p2 = p2.m_term_and_radix.size();

  // after term merging, should have the same number of distinct terms
  if (term_count_p1 != term_count_p2) {
    return false;
  }

  // zero
  if (term_count_p1 == 0) {
    return true;
  }

  // If every term of polynomial 1 can be found in polynomial 2, it's sufficient
  // to declare them equal.
  for (int i = 0; i < term_count_p1; i++) {
    map<string, int> *t1_ptr = &p1.m_term_and_radix[i];
    for (int j = 0; j < term_count_p2; j++) {
      // 11/20/13 Muhuan ...
      map<string, int> *t2_ptr = &p2.m_term_and_radix[j];
      //    map<string, int> *t2_ptr = &p2.m_term_and_radix[i];
      if (term_equal(t1_ptr, t2_ptr)) {
        break;
      }
      if (j == term_count_p2 - 1) {
        return false;
      }
    }
  }
  return true;
}

// END: This range is for Libo to edit
//////////////////////////////////////////////////////////////////////////

const char *PolyVector::OP_FLAG_POINTER = "$";  // ((astNode*)(-1));
const char *PolyVector::CONST_POINTER = "#";    // ((astNode*)(0));
const char *PolyVector::POSITIVE_INFINITE = "__INF__";
const char *PolyVector::LEXIGRAPHIC_DELTA = "__LEX_DELT_d";
const char *PolyVector::FIXED_CONDITION = "__CONST_COND_d";

string op_flag_to_string(string sValue) {
  if (sValue == "0") {
    return "==";
  }
  if (sValue == "1") {
    return ">=";
  }
  if (sValue == "2") {
    return ">";
  }
  if (sValue == "3") {
    return "na";
  }
  assert(0);
  return "";
}

int PolyMatrix::get_iter_total_bitwidth() {
  size_t i;
  int total = 0;
  vector<string> vecIters = get_iterators();
  for (i = 0; i < vecIters.size(); i++) {
    total += get_iter_bitwidth(vecIters[i]);
  }
  return total;
}

#define OLD_FORMAT 0

string PolyVector::print() {
#if OLD_FORMAT
  string sDump = string("(");
  for (size_t i = 0; i < m_coeffs.size(); i++) {
    char szValue[1024];
    snprintf(szValue, sizeof(szValue), "%d", m_coeffs[i]);
    string sValue = string(szValue);

    map<string, int>::iterator it;
    for (it = m_vars.begin(); it != m_vars.end(); it++) {
      if (it->second == static_cast<int>(i))
        break;
    }
    if (OP_FLAG_POINTER == it->first)
      sValue = op_flag_to_string(sValue);

    sDump += sValue;
    if (i < m_coeffs.size() - 1)
      sDump += string(" ");
  }
  sDump += string(")");
  return sDump;
#else
  string sDump = string("(");
  for (size_t i = 0; i < m_coeffs.size(); i++) {
    char szValue[1024];
    snprintf(szValue, sizeof(szValue), "%d", m_coeffs[i]);
    string sValue = string(szValue);

    map<string, int>::iterator it;
    for (it = m_vars.begin(); it != m_vars.end(); it++) {
      if (it->second == static_cast<int>(i)) {
        break;
      }
    }

    if (OP_FLAG_POINTER == it->first) {
      sValue = op_flag_to_string(sValue);
    }

    string sVariable;
    int bZero = 0;
    if (CONST_POINTER == it->first) {
      sVariable = string("1");
    } else if (OP_FLAG_POINTER == it->first) {
      sVariable = string("$");
    } else {
      sVariable = it->first;
      if (m_coeffs[i] == 0) {
        bZero = 1;
      }
    }

    if (bZero == 0) {
      sDump += sValue + "*" + sVariable;
      if (i < m_coeffs.size() - 1) {
        sDump += string(" ");
      }
    }
  }
  sDump += string(")");
  return sDump;
#endif
}

string PolyVector::print_expression() {
  string sDump;
  for (size_t i = 0; i < m_coeffs.size(); i++) {
    string sValue = my_itoa(m_coeffs[i]);

    map<string, int>::iterator it;
    for (it = m_vars.begin(); it != m_vars.end(); it++) {
      if (it->second == static_cast<int>(i)) {
        break;
      }
    }

    if (OP_FLAG_POINTER == it->first) {
      continue;
    }

    string sVariable;
    int bZero = 0;
    if (CONST_POINTER == it->first) {
      sVariable = string("1");
    } else if (OP_FLAG_POINTER == it->first) {
      sVariable = string("$");
    } else {
      sVariable = it->first;
      if (m_coeffs[i] == 0) {
        bZero = 1;
      }
    }

    if (bZero == 0) {
      sDump += sValue;
      if (CONST_POINTER != it->first) {
        sDump += "*" + sVariable + "+";
      }
      // if (i < m_coeffs.size() - 1) sDump += string("+");
    }
  }
  return sDump;
}
string PolyVector::print_condition() {
  string sDump;
  for (size_t i = 0; i < m_coeffs.size(); i++) {
    string sValue = my_itoa(m_coeffs[i]);

    map<string, int>::iterator it;
    for (it = m_vars.begin(); it != m_vars.end(); it++) {
      if (it->second == static_cast<int>(i)) {
        break;
      }
    }

    if (OP_FLAG_POINTER == it->first) {
      sValue = op_flag_to_string(sValue);
    }

    string sVariable;
    int bZero = 0;
    if (CONST_POINTER == it->first) {
      sVariable = string("1");
    } else if (OP_FLAG_POINTER == it->first) {
      sVariable = string("$");
    } else {
      sVariable = it->first;
      if (m_coeffs[i] == 0) {
        bZero = 1;
      }
    }

    if (bZero == 0) {
      if (sVariable == "$") {
        sDump += sValue + "0";
      } else {
        // sDump += sValue + "*" + sVariable;
        sDump += sValue;
        if (CONST_POINTER != it->first) {
          sDump += "*" + sVariable + "+";
        }
      }
      // if (i < m_coeffs.size() - 1) sDump += string(" ");
    }
  }
  return sDump;
}

string PolyVector::print_var() {
#if OLD_FORMAT
  string sDump = string("(");
  for (size_t i = 0; i < m_coeffs.size(); i++) {
    map<string, int>::iterator it;
    for (it = m_vars.begin(); it != m_vars.end(); it++) {
      if (it->second == static_cast<int>(i))
        break;
    }
    if (CONST_POINTER == it->first)
      sDump += string("1");
    else if (OP_FLAG_POINTER == it->first)
      sDump += string("$");
    else
      sDump += it->first;
    if (i < m_coeffs.size() - 1)
      sDump += string(" ");
  }
  sDump += string(")");
  return sDump;
#else
  return "";
#endif
}

PolyVector::PolyVector(int val) {
  m_iterator_num = -1;
  m_vars.clear();
  m_coeffs.clear();
  append(PolyVector::CONST_POINTER, val);
}

PolyVector::PolyVector(string var, int val) {
  m_iterator_num = -1;
  m_vars.clear();
  m_coeffs.clear();
  append(var, val);
}

PolyVector PolyVector::operator+(PolyVector oprand2) {
  PolyVector result(*this);

  map<string, int>::iterator it;

  for (it = oprand2.m_vars.begin(); it != oprand2.m_vars.end(); it++) {
    string var = it->first;
    if (m_vars.end() != m_vars.find(var)) {
      int idx1 = m_vars[var];

      if (var == OP_FLAG_POINTER && result.m_coeffs[idx1] == OP_GE &&
          oprand2.get_coeff(var) == OP_GE) {
        result.m_coeffs[idx1] = OP_GE;
      } else {
        result.m_coeffs[idx1] += oprand2.get_coeff(var);
      }
    } else {
      result.append(var, oprand2.get_coeff(var));
    }
  }
  return result;
}

// return -1 if all zero, 0 if only constant term, >=1 if have variables
int PolyVector::level() {
  int level = 0;
  map<string, int>::iterator it;
  for (it = m_vars.begin(); it != m_vars.end(); it++) {
    if (CONST_POINTER != it->first && OP_FLAG_POINTER != it->first) {
      level++;
    }
  }
  return (m_vars.empty()) ? -1 : level;
}
int PolyVector::nonzero_level() {
  int level = 0;
  map<string, int>::iterator it;
  for (it = m_vars.begin(); it != m_vars.end(); it++) {
    if (CONST_POINTER != it->first && OP_FLAG_POINTER != it->first &&
        (m_coeffs[it->second] != 0)) {
      level++;
    }
  }
  return (m_vars.empty()) ? -1 : level;
}

int PolyVector::get_const() { return get_coeff(CONST_POINTER); }

int PolyVector::get_coeff(string var) {
  if (m_vars.find(var) == m_vars.end()) {
    return 0;
  }
  int idx = m_vars[var];
  return m_coeffs[idx];
}
int PolyVector::get_cond() { return get_coeff(OP_FLAG_POINTER); }

void PolyVector::normalize_coeff(string var) {
  if (get_cond() != OP_EQ) {
    // TODO(youxiang): do not change sign because we have no LE and LT
    // conditions

    if (get_cond() == OP_GT) {
      m_coeffs[m_vars[CONST_POINTER]]--;
      m_coeffs[m_vars[OP_FLAG_POINTER]] = OP_GE;
    }
  } else if (get_cond() == OP_EQ) {
    if (get_coeff(var) < 0) {
      (*this) = -(*this);
    }

    // TODO(youxiang): divide the common dividers
  }
}

PolyVector PolyVector::operator*(PolyVector oprand2) {
  int level1 = this->level();
  int level2 = oprand2.level();

  assert(0 == level1 || 0 == level2);
  // assert(-1 != level1 && -1 != level2);

  PolyVector result(0);
  int factor;

  if (0 == level1) {
    result = oprand2;
    factor = this->get_const();
  } else {  // 0 == level2
    result = *this;
    factor = oprand2.get_const();
  }

  // for (unsigned int idx = 0; idx < result.m_coeffs.size(); idx++)
  //{
  //    result.m_coeffs[idx] *= factor;
  //}

  map<string, int>::iterator it;
  for (it = result.m_vars.begin(); it != result.m_vars.end(); it++) {
    if (it->first == OP_FLAG_POINTER) {
      continue;
    }
    if (result.m_coeffs[it->second] != 0) {
      result.m_coeffs[it->second] *= factor;
    }
  }

  assert(result.m_vars.size() == result.m_coeffs.size());
  return result;
}

PolyVector PolyVector::operator/(int oprand2) {
  PolyVector result(*this);
  map<string, int>::iterator it;
  for (it = m_vars.begin(); it != m_vars.end(); it++) {
    if (it->first == OP_FLAG_POINTER) {
      continue;
    }
    string var = it->first;
    int idx1 = m_vars[var];
    double val = (result.m_coeffs[idx1] * 1.0) / oprand2;
    if (val > 0) {
      val += 0.49;
    }
    if (val < 0) {
      val -= 0.49;
    }

    result.m_coeffs[idx1] = static_cast<int>(val);
  }
  return result;
}

PolyVector PolyVector::operator-() {
  PolyVector minus_one(-1);
  return minus_one * (*this);
}

PolyVector PolyVector::operator-(PolyVector oprand2) {
  PolyVector minus_op2 = -(oprand2);
  return (*this) + minus_op2;
}

bool PolyVector::operator==(PolyVector oprand2) {
  if (m_vars.size() != oprand2.m_vars.size()) {
    return false;
  }

  map<string, int>::iterator it;
  for (it = oprand2.m_vars.begin(); it != oprand2.m_vars.end(); it++) {
    string var = it->first;
    if (m_vars.end() == m_vars.find(var)) {
      return false;
    }

    if (m_coeffs[m_vars[var]] != oprand2.m_coeffs[it->second]) {
      return false;
    }
  }
  return true;
}

PolyVector::PolyVector(const PolyVector &polyvec) {
  m_iterator_num = polyvec.m_iterator_num;

  m_vars.clear();
  m_coeffs.clear();

  m_vars.insert(polyvec.m_vars.begin(), polyvec.m_vars.end());
  m_coeffs.insert(m_coeffs.begin(), polyvec.m_coeffs.begin(),
                  polyvec.m_coeffs.end());
}

PolyVector PolyVector::operator!() {
  PolyVector result(*this);

  // Must have OP_FLAG in the PolyVector
  assert(m_vars.end() != m_vars.find(OP_FLAG_POINTER));
  int op_flag = m_coeffs[m_vars[OP_FLAG_POINTER]];

  switch (op_flag) {
  case OP_EQ:
    result.m_coeffs[m_vars[OP_FLAG_POINTER]] = OP_NE;
    break;
  case OP_NE:
    result.m_coeffs[m_vars[OP_FLAG_POINTER]] = OP_EQ;
    break;
  case OP_GE:
    result = -result;
    result.m_coeffs[m_vars[OP_FLAG_POINTER]] = OP_GT;
    break;
  case OP_GT:
    result = -result;
    result.m_coeffs[m_vars[OP_FLAG_POINTER]] = OP_GE;
    break;
  default:
    assert(false);
    break;
  }

  return result;
}

void PolyVector::align(PolyVector *oprand2) {
  map<string, int>::iterator it;
  for (it = oprand2->m_vars.begin(); it != oprand2->m_vars.end(); it++) {
    string var = it->first;
    if (m_vars.end() == m_vars.find(var)) {
      append(var, 0);
    }
  }
}

vector<string> PolyVector::get_iterators() {
  // assert(m_iterator_num != -1);  // set_iterators() is not called

  vector<string> vec;

  if (m_iterator_num == -1) {
    return vec;
  }

  for (int i = 0; i < m_iterator_num; i++) {
    map<string, int>::iterator it;
    for (it = m_vars.begin(); it != m_vars.end(); it++) {
      if (it->second == i) {
        break;
      }
    }
    vec.push_back(it->first);
  }
  return vec;
}

vector<string> PolyVector::get_parameters() {
  assert(m_iterator_num != -1);  // set_iterators() is not called

  vector<string> vec;

  for (size_t i = m_iterator_num; i < m_coeffs.size() - 2; i++) {
    map<string, int>::iterator it;
    for (it = m_vars.begin(); it != m_vars.end(); it++) {
      if (it->second == static_cast<int>(i)) {
        break;
      }
    }
    vec.push_back(it->first);
  }
  return vec;
}

void PolyVector::set_iterators(vector<string> *iterators) {
  if (m_vars.end() == m_vars.find(PolyVector::CONST_POINTER)) {
    PolyVector vec(0);
    *this = *this + vec;
  }

  if (m_vars.end() == m_vars.find(PolyVector::OP_FLAG_POINTER)) {
    PolyVector vec(PolyVector::OP_FLAG_POINTER, PolyVector::OP_EQ);
    *this = *this + vec;
  }

  size_t i;
  m_iterator_num = iterators->size();

  vector<string> vec_order;
  // reserve position for all the iterators
  for (i = 0; i < iterators->size(); i++) {
    string var = (*iterators)[i];
    if (m_vars.end() == m_vars.find(var)) {
      PolyVector vec(var, 0);
      *this = *this + vec;
    }
    vec_order.push_back(var);
  }

  if (m_vars.size() > iterators->size()) {
    // m_vars has been updated at this time
    // keep the original order of parameters

    map<string, int>::iterator it;
    for (it = m_vars.begin(); it != m_vars.end(); it++) {
      if (it->first == CONST_POINTER || it->first == OP_FLAG_POINTER) {
        continue;
      }
      for (i = 0; i < iterators->size(); i++) {
        if ((*iterators)[i] == it->first) {
          break;
        }
      }
      if (i == (*iterators).size()) {
        vec_order.push_back(it->first);
      }
    }
  }

  vec_order.push_back(CONST_POINTER);
  vec_order.push_back(OP_FLAG_POINTER);

  sort(&vec_order);
}

void PolyVector::sort(vector<string> *var_order) {
  PolyVector new_vec(*this);
  new_vec.clear();

  if (m_coeffs.size() > var_order->size()) {
    new_vec.clear();
  }
  assert(m_coeffs.size() <= var_order->size());

  int value;

  for (size_t i = 0; i < var_order->size(); i++) {
    string var = (*var_order)[i];
    if (m_vars.end() == m_vars.find(var)) {
      assert(0);  // The empty position should have been reserved by
    }
    // set_iterators()
    value = m_coeffs[m_vars[var]];
    PolyVector item(var, value);
    new_vec = new_vec + item;
  }

  assert(new_vec.m_coeffs.size() == m_coeffs.size());

  (*this) = new_vec;
}

void PolyVector::sort(PolyVector *vec_order) {
  vector<string> var_order;
  map<string, int>::iterator it;
  size_t idx;
  for (idx = 0; idx < vec_order->m_vars.size(); idx++) {
    for (it = vec_order->m_vars.begin(); it != vec_order->m_vars.end(); it++) {
      if (it->second == static_cast<int>(idx)) {
        break;
      }
    }
    assert(it != vec_order->m_vars.end());
    var_order.push_back(it->first);
  }
  sort(&var_order);
}

void PolyVector::get_vars(vector<string> *vec_vars) {
  map<string, int>::iterator it;
  for (it = m_vars.begin(); it != m_vars.end(); it++) {
    vec_vars->push_back(it->first);
  }
}
vector<string> PolyVector::get_vars_no_const() {
  vector<string> vec_vars;
  map<string, int>::iterator it;
  for (it = m_vars.begin(); it != m_vars.end(); it++) {
    if (it->first != PolyVector::CONST_POINTER &&
        it->first != PolyVector::OP_FLAG_POINTER) {
      vec_vars.push_back(it->first);
    }
  }
  return vec_vars;
}
int PolyVector::get_vars_num() {
  vector<string> vec_vars;
  get_vars(&vec_vars);
  return static_cast<int>(vec_vars.size() - 2);
}
int PolyMatrix::get_vars_num() {
  vector<string> vec_vars;
  get_vars(&vec_vars);
  return static_cast<int>(vec_vars.size() - 2);
}
int PolyMatrixSet::get_vars_num() {
  vector<string> vec_vars;
  get_vars(&vec_vars, 0);
  return static_cast<int>(vec_vars.size() - 2);
}

void PolyVector::get_nonzero_vars(vector<string> *vec_vars) {
  map<string, int>::iterator it;
  for (it = m_vars.begin(); it != m_vars.end(); it++) {
    if (it->first == CONST_POINTER) {
      continue;
    }
    if (it->first == OP_FLAG_POINTER) {
      continue;
    }
    if (m_coeffs[it->second] != 0) {
      vec_vars->push_back(it->first);
    }
  }
}

vector<string> PolyVector::get_nonzero_vars() {
  vector<string> vec_vars;
  map<string, int>::iterator it;
  for (it = m_vars.begin(); it != m_vars.end(); it++) {
    if (it->first == CONST_POINTER) {
      continue;
    }
    if (it->first == OP_FLAG_POINTER) {
      continue;
    }
    if (m_coeffs[it->second] != 0) {
      vec_vars.push_back(it->first);
    }
  }
  return vec_vars;
}

int PolyVector::isTrue() {
  vector<string> vecVars;
  get_nonzero_vars(&vecVars);
  if (vecVars.empty()) {
    if (get_const() >= 0 && get_cond() == OP_GE) {
      return 1;
    }
    if (get_const() == 0 && get_cond() == OP_EQ) {
      return 1;
    }
    if (get_const() >= 0 && get_cond() == OP_GT) {
      return 1;
    } else {
      return 0;
    }
  }

  return 0;

  //     unsigned int i;
  //     for (i = 0; i < m_coeffs.size(); i++)
  //     {
  //         if (m_coeffs[i]) return false;
  //     }
  //     return true;
}

//////////////////////////////////////////////////////////////////////////

void PolyMatrix::sort(vector<string> *var_order) {
  for (size_t i = 0; i < m_mat.size(); i++) {
    m_mat[i].sort(var_order);
  }
}

void PolyMatrix::set_iterators(vector<string> *iterators) {
  for (size_t i = 0; i < m_mat.size(); i++) {
    m_mat[i].set_iterators(iterators);
  }
}
vector<string> PolyMatrix::get_iterators() {
  vector<string> empty;
  if (!m_mat.empty()) {
    return m_mat[0].get_iterators();
  }
  { return empty; }
}
vector<string> PolyMatrix::get_parameters() {
  vector<string> empty;
  if (!m_mat.empty()) {
    return m_mat[0].get_parameters();
  }
  { return empty; }
}

void PolyMatrix::append_vector(PolyVector row) {
  // add new columns (align columns)
  for (size_t i = 0; i < m_mat.size(); i++) {
    m_mat[i].align(&row);
  }

  // add new row
  if (!m_mat.empty()) {
    row.align(&m_mat[0]);
  }
  m_mat.push_back(row);

  // remove reduntant vectors
  remove_redundant_vector();
}

void PolyMatrix::append_matrix(PolyMatrix mat) {
  size_t i;
  for (i = 0; i < mat.size(); i++) {
    append_vector(mat.GetVector(i));
  }
}

void PolyMatrix::remove_redundant_vector() {
  int no_more = 0;
  while (no_more == 0) {
    no_more = 1;
    for (size_t i = 0; i < m_mat.size(); i++) {
      // remove all true constraints
      if ((m_mat[i].isTrue() != 0) && static_cast<int>(m_mat.size()) > 1) {
        no_more = 0;
        m_mat.erase(m_mat.begin() + i);
        break;
      }
      // remove idential constraints
      for (size_t j = i + 1; j < m_mat.size(); j++) {
        if (m_mat[i] == m_mat[j]) {
          no_more = 0;
          m_mat.erase(m_mat.begin() + j);
          break;
        }

        if (m_mat[i].get_cond() == PolyVector::OP_GE &&
            m_mat[j].get_cond() == PolyVector::OP_GE) {
          PolyVector vecSub = m_mat[i] - m_mat[j];
          vector<string> vecNZ;
          vecSub.get_nonzero_vars(&vecNZ);
          if (vecNZ.empty()) {
            if (vecSub.get_const() >= 0) {
              no_more = 0;
              m_mat.erase(m_mat.begin() + i);
              break;
            }
            if (vecSub.get_const() < 0) {
              no_more = 0;
              m_mat.erase(m_mat.begin() + j);
              break;
            }
          }
        }
      }
      if (no_more == 0) {
        break;
      }
    }
  }
}

void PolyMatrix::get_vars(vector<string> *vec_vars) {
  if (m_mat.empty()) {
    return;
  }
  m_mat[0].get_vars(vec_vars);
}

string PolyMatrix::print() {
  size_t i;
  string sDump;
  if (m_mat.empty()) {
    return string("");
  }

  vector<string> vecIter = get_iterators();
  vector<string> vecParam = get_parameters();

  sDump += "(iterators: ";
  for (i = 0; i < vecIter.size(); i++) {
    sDump += vecIter[i] + " ";
  }
  sDump += ") (";
  sDump += "parameters: ";
  for (i = 0; i < vecParam.size(); i++) {
    sDump += vecParam[i] + " ";
  }
  sDump += ")\n";

  for (i = 0; i < m_mat.size(); i++) {
    sDump += m_mat[i].print_var();  // + string("\n");
    sDump += m_mat[i].print() + string("\n");
  }
  return sDump;
}

string PolyMatrix::print_expression(
    string sVar) {  // find the expression by detecting the first equality with
                    // the variable
  size_t i;
  string sConditions;
  string sExpression;
  for (i = 0; i < size(); i++) {
    PolyVector row = GetVector(i);
    if (row.get_cond() == PolyVector::OP_EQ) {
      if (row.get_coeff(sVar) != 0 && sExpression.empty()) {
        int coeff = row.get_coeff(sVar);

        PolyVector vec = -row / coeff;
        PolyVector one(sVar);
        vec = vec + one;

        sExpression = vec.print_expression();
      }
    } else {
      sConditions += row.print_condition() + ";";
    }
  }
  return sExpression + ":" + sConditions;
}

void PolyMatrix::rename_var(string old_var, string new_var) {
  size_t i;
  for (i = 0; i < size(); i++) {
    PolyVector &row = GetVector(i);
    row.rename_var(old_var, new_var);
  }
}

PolyMatrixSet PolyMatrix::operator!() {
  PolyMatrixSet result;

  for (size_t i = 0; i < m_mat.size(); i++) {
    PolyMatrix mat;
    PolyVector vec = !m_mat[i];
    mat.append_vector(vec);
    result.append_matrix(mat);
  }

  return result;
}

PolyMatrix PolyMatrix::operator&(PolyMatrix mat2) {
  PolyMatrix result;
  size_t i;

  for (i = 0; i < m_mat.size(); i++) {
    result.append_vector(m_mat[i]);
  }
  for (i = 0; i < mat2.m_mat.size(); i++) {
    result.append_vector(mat2.m_mat[i]);
  }

  return result;
}

PolyMatrix PolyMatrix::remove_redundant_variables() {
  size_t i;
  size_t j;
  vector<string> vecVar = GetVector(0).get_vars_no_const();

  // 1. detect zero variables
  vector<string> vecNonzero;
  for (i = 0; i < vecVar.size(); i++) {
    for (j = 0; j < size(); j++) {
      if (GetVector(j).get_coeff(vecVar[i]) != 0) {
        break;
      }
    }
    if (j != size()) {  // has
      vecNonzero.push_back(vecVar[i]);
    }
  }

  // 2. construct the matrix with only non-zero variables
  PolyMatrix new_mat;
  for (j = 0; j < size(); j++) {
    PolyVector vec = GetVector(j);
    PolyVector tmp(0);

    for (i = 0; i < vecNonzero.size(); i++) {
      PolyVector item(vecNonzero[i], vec.get_coeff(vecNonzero[i]));
      tmp = tmp + item;
    }
    PolyVector item1(PolyVector::OP_FLAG_POINTER, vec.get_cond());
    tmp = tmp + item1;
    PolyVector item2(PolyVector::CONST_POINTER, vec.get_const());
    tmp = tmp + item2;

    new_mat.append_vector(tmp);
  }

  new_mat.set_iterators(&vecNonzero);

  return new_mat;
}

#ifdef POLYMODEL_SUPPORT_ISL
int PolyMatrix::ProjectOut(string anode) {
  vector<string> vars;
  get_vars(vars);
  int var_num = vars.size() - 2;

  struct isl_ctx *ctx = isl_ctx_alloc();
  isl_space *space = isl_space_set_alloc(ctx, 0, var_num);
  struct isl_basic_set *bset = isl_basic_set_universe(isl_space_copy(space));

  isl_local_space *ls = isl_local_space_from_space(space);

  // init with var_num
  // struct isl_ctx *ctx = isl_ctx_alloc();
  // struct isl_dim * dim = isl_dim_set_alloc(ctx, 0, var_num);
  // struct isl_basic_set * bset = isl_basic_set_universe(isl_dim_copy(dim));

  // add contraints
  isl_int v;

  // a = 2b
  struct isl_constraint *c = isl_equality_alloc(isl_local_space_copy(ls));
  isl_int_set_si(v, -1);
  isl_constraint_set_coefficient(c, isl_dim_set, 0, v);  // first var a
  isl_int_set_si(v, 2);
  isl_constraint_set_coefficient(c, isl_dim_set, 1, v);  // second var b
  bset = isl_basic_set_add_constraint(bset, c);

  // a >= 10
  c = isl_inequality_alloc(isl_local_space_copy(ls));
  isl_int_set_si(v, -10);
  isl_constraint_set_constant(c, v);
  isl_int_set_si(v, 1);
  isl_constraint_set_coefficient(c, isl_dim_set, 0, v);
  bset = isl_basic_set_add_constraint(bset, c);

  // project a out => b >= 5
  bset = isl_basic_set_project_out(bset, isl_dim_set, 0, 1);

  return 1;
}

int PolyMatrix::Simplify() { return 1; }
#endif

PolyMatrix PolyMatrix::GetLowerBoundVector(string sVar) {
  PolyMatrix matRet;
  size_t i;

  for (i = 0; i < size(); i++) {
    PolyVector vec = GetVector(i);

    int coeff = vec.get_coeff(sVar);

    if ((coeff != 0 && PolyVector::OP_EQ == vec.get_cond()) ||
        (coeff > 0 && PolyVector::OP_GE == vec.get_cond())) {
      PolyVector sub(sVar, coeff);
      vec = vec - sub;
      vec = vec / (-coeff);
      matRet.append_vector(vec);
    }
  }
  return matRet;
}

PolyMatrix PolyMatrix::GetUpperBoundVector(string sVar) {
  PolyMatrix matRet;
  size_t i;

  for (i = 0; i < size(); i++) {
    PolyVector vec = GetVector(i);

    int coeff = vec.get_coeff(sVar);

    if ((coeff != 0 && PolyVector::OP_EQ == vec.get_cond()) ||
        (coeff < 0 && PolyVector::OP_GE == vec.get_cond())) {
      PolyVector sub(sVar, coeff);
      vec = vec - sub;
      vec = vec / (-coeff);
      matRet.append_vector(vec);
    }
  }
  return matRet;
}

int PolyMatrix::GetUpperBoundSimple(string sVar) {
  size_t i;
  for (i = 0; i < size(); i++) {
    PolyVector vec = GetVector(i);
    int coeff = vec.get_coeff(sVar);

    if ((coeff != 0 && PolyVector::OP_EQ == vec.get_cond()) ||
        (coeff < 0 && PolyVector::OP_GE == vec.get_cond())) {
      vector<string> vecvar;
      vec.get_nonzero_vars(&vecvar);

      if (vecvar.size() != 1) {
        printf("\n\n[PolyMatrix] ERROR: GetUpperBoundSimple() is called when "
               "there is not in the simple form :\n");
        printf("%s\n", print().c_str());
        assert(0);
      }
      // assert(vecvar.size() == 1);

      return vec.get_const() / (-coeff);
    }
  }
  return static_cast<int>(1e20);
}

int PolyMatrix::GetLowerBoundSimple(string sVar) {
  size_t i;
  for (i = 0; i < size(); i++) {
    PolyVector vec = GetVector(i);
    int coeff = vec.get_coeff(sVar);

    if ((coeff != 0 && PolyVector::OP_EQ == vec.get_cond()) ||
        (coeff > 0 && PolyVector::OP_GE == vec.get_cond())) {
      vector<string> vecvar;
      vec.get_nonzero_vars(&vecvar);
      assert(vecvar.size() == 1);
      return vec.get_const() / (-coeff);
    }
  }
  return -static_cast<int>(1e20);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void PolyMatrixSet::append_matrix(PolyMatrix mat) {
  // align for variables in each matrix
  // for (int i = 0; i < (int)m_matset.size(); i++)
  //{
  //    m_matset[i].align(mat);
  //}
  // if (m_matset.size() > 0) mat.align(m_mat[0]);

  m_matset.push_back(mat);
}

PolyMatrixSet PolyMatrixSet::operator|(PolyMatrixSet matset2) {
  PolyMatrixSet result;
  size_t i;

  for (i = 0; i < m_matset.size(); i++) {
    result.append_matrix(m_matset[i]);
  }
  for (i = 0; i < matset2.m_matset.size(); i++) {
    result.append_matrix(matset2.m_matset[i]);
  }

  return result;
}

PolyMatrixSet PolyMatrixSet::operator&(PolyMatrixSet matset2) {
  PolyMatrixSet result;
  size_t i;
  size_t j;

  for (i = 0; i < m_matset.size(); i++) {
    for (j = 0; j < matset2.m_matset.size(); j++) {
      PolyMatrix item = m_matset[i] & matset2.m_matset[i];
      result.append_matrix(item);
    }
  }

  return result;
}

PolyMatrixSet PolyMatrixSet::operator!() {
  size_t i;
  PolyMatrixSet result;

  for (i = 0; i < m_matset.size(); i++) {
    PolyMatrixSet pset = !m_matset[i];

    if (i == 0) {
      result = pset;
    } else {
      result = result & pset;
    }
  }
  return result;
}

void PolyMatrixSet::clear() { m_matset.clear(); }

void PolyMatrixSet::sort(vector<string> *var_order) {
  size_t i;
  for (i = 0; i < m_matset.size(); i++) {
    m_matset[i].sort(var_order);
  }
}

void PolyMatrixSet::set_iterators(vector<string> *iterators) {
  size_t i;
  for (i = 0; i < m_matset.size(); i++) {
    m_matset[i].set_iterators(iterators);
  }
}
vector<string> PolyMatrixSet::get_iterators() {
  return m_matset[0].get_iterators();
}
vector<string> PolyMatrixSet::get_parameters() {
  return m_matset[0].get_parameters();
}

void PolyMatrixSet::get_vars(vector<string> *vec_vars, int idx) {
  if (idx >= static_cast<int>(m_matset.size())) {
    return;
  }

  m_matset[idx].get_vars(vec_vars);
}

string PolyMatrixSet::print() {
  string sDump;
  size_t i;

  for (i = 0; i < m_matset.size(); i++) {
    sDump += string("{\n");
    sDump += m_matset[i].print();
    sDump += string("}");
  }

  return sDump;
}

//////////////////////////////////////////////////////////////////////////
PolyMatrix generate_constraints_from_boundary(string var, PolyVector *vec_lb,
                                              PolyVector *vec_ub,
                                              int reach_ub) {
  PolyMatrix mat;
  PolyVector vec_iv(var, 1);
  PolyVector vec_1(PolyVector::CONST_POINTER, 1);
  PolyVector vec_op(PolyVector::OP_FLAG_POINTER, PolyVector::OP_GE);
  PolyVector vec0 = vec_iv - *vec_lb + vec_op;
  PolyVector vec1 = (reach_ub) != 0 ? *vec_ub - vec_iv + vec_op
                                    : *vec_ub - vec_1 - vec_iv + vec_op;
  mat.append_vector(vec0);
  mat.append_vector(vec1);
  return mat;
}

#ifdef POLYMODEL_SUPPORT_ISL
//////////////////////////////////////////////////////////////////////////
struct isl_constraint *PolyVector::get_isl_set_constraint(isl_ctx *ctx,
                                                          int n_dim) {
  size_t i;
  int n_param = get_vars_num() - n_dim;
  // struct isl_dim *dim;
  // dim = isl_dim_set_alloc(ctx, n_param, n_dim);

  isl_space *space;
  space = isl_space_set_alloc(ctx, n_param, n_dim);

  isl_local_space *ls;
  ls = isl_local_space_from_space(space);

  isl_int v;
  isl_int_init(v);

  struct isl_constraint *c;

  if (PolyVector::OP_EQ == get_cond()) {
    c = isl_equality_alloc((ls));
  } else if (PolyVector::OP_GE == get_cond() ||
             PolyVector::OP_GT == get_cond()) {  // OP_GE or OP_GT
    c = isl_inequality_alloc((ls));
  } else {
    assert(0);
  }

  for (i = 0; i < m_coeffs.size() - 2; i++) {  //
    isl_int_set_si(v, m_coeffs[i]);
    isl_constraint_set_coefficient(c, isl_dim_set, i, v);
  }

  int const_val =
      (PolyVector::OP_GT == get_cond()) ? m_coeffs[i] - 1 : m_coeffs[i];
  isl_int_set_si(v, const_val);
  isl_constraint_set_constant(c, v);

  isl_int_clear(v);

  return c;
}

struct isl_basic_set *PolyMatrix::get_isl_basic_set(isl_ctx *ctx, int n_dim) {
  size_t i;
  int n_param = get_vars_num() - n_dim;

  struct isl_dim *dim;
  struct isl_basic_set *bset;

  dim = isl_dim_set_alloc(ctx, n_param, n_dim);
  bset = isl_basic_set_universe(dim);

  for (i = 0; i < size(); i++) {
    struct isl_constraint *c = GetVector(i).get_isl_set_constraint(ctx, n_dim);
    bset = isl_basic_set_add_constraint(bset, c);
  }

  return bset;
}

struct isl_set *PolyMatrixSet::get_isl_set(isl_ctx *ctx, int n_dim) {
  size_t i;
  int n_param = get_vars_num() - n_dim;

  struct isl_dim *dim;
  struct isl_set *isl_set0;

  dim = isl_dim_set_alloc(ctx, n_param, n_dim);
  isl_set0 = isl_set_empty(dim);

  for (i = 0; i < m_matset.size(); i++) {
    struct isl_basic_set *cur_bset = m_matset[i].get_isl_basic_set(ctx, n_dim);
    struct isl_set *cur_set = isl_set_from_basic_set(cur_bset);
    isl_set0 = isl_set_union(isl_set0, cur_set);
  }

  return isl_set0;
}

#endif  // POLYMODEL_SUPPORT_ISL

void PolyVector::rename_var(string old_var, string new_var) {
  //     map<string, int>::iterator it = m_vars.find(old_name);
  //     assert(m_vars.end() != it);
  //     pair<string, int> item(new_name, it->second);
  //     m_vars.erase(it);
  //     m_vars.insert(item);

  if (old_var == new_var) {
    return;
  }
  map<string, int>::iterator it = m_vars.find(old_var);
  assert(it != m_vars.end());
  map<string, int>::iterator it1 = m_vars.find(new_var);
  assert(it1 == m_vars.end());
  m_vars[new_var] = m_vars[old_var];
  m_vars.erase(it);
}

vector<string> merge_iterator(vector<string> vec0, vector<string> vec1) {
  size_t i;
  size_t j;
  vector<string> vec_out = vec0;
  for (i = 0; i < vec1.size(); i++) {
    for (j = 0; j < vec_out.size(); j++) {
      if (vec1[i] == vec_out[j]) {
        break;
      }
    }
    if (j == vec_out.size()) {
      vec_out.push_back(vec1[i]);
    }
  }
  return vec_out;
}

PolyMatrix CombineAccessConstraints(PolyMatrix domain, PolyMatrix order,
                                    PolyMatrix access, string sArray,
                                    string sSuffix) {
  size_t i;
  size_t j;
  PolyMatrix combined;

  combined = domain;

  vector<string> merged_iter;
  if (0 < domain.size()) {
    merged_iter = domain.GetVector(0).get_vars_no_const();
  }

  for (i = 0; i < access.size(); i++) {
    PolyVector &vec = access.GetVector(i);
    vector<string> vecParams = vec.get_parameters();
    for (j = 0; j < vecParams.size(); j++) {
      vec.rename_var(vecParams[j], "__" + vecParams[j]);
    }
  }
  combined.append_matrix(access);
  if (0 < access.size()) {
    merged_iter =
        merge_iterator(merged_iter, access.GetVector(0).get_vars_no_const());
  }

  for (i = 0; i < order.size(); i++) {
    PolyVector &vec = order.GetVector(i);
    PolyVector vec1("_order" + my_itoa(i), -1);
    vec = vec + vec1;
  }
  vector<string> vecIter0;
  for (i = 0; i < order.size(); i++) {
    vecIter0.push_back("_order" + my_itoa(i));
  }
  order.set_iterators(&vecIter0);
  combined.append_matrix(order);
  if (0 < order.size()) {
    merged_iter =
        merge_iterator(merged_iter, order.GetVector(0).get_vars_no_const());
  }

  combined.set_iterators(&merged_iter);

  // rename to add the task suffix
  for (i = 0; i < combined.size(); i++) {
    PolyVector &vec = combined.GetVector(i);
    for (j = 0; j < merged_iter.size(); j++) {
      string matched = sArray + "__d";
      if (0 != merged_iter[j].find(matched)) {
        vec.rename_var(merged_iter[j], merged_iter[j] + sSuffix);
      }
    }
  }

  return combined;
}

int ConstraintReduce(PolyVector *constraint, PolyVector *vec_ref,
                     const string &var) {
  int coeff_ref = vec_ref->get_coeff(var);
  int coeff_cst = constraint->get_coeff(var);

  if (0 == coeff_cst || 0 == coeff_ref) {
    return 0;
  }
  if (0 != abs(coeff_cst) % abs(coeff_ref)) {
    return 0;
  }

  PolyVector factor(coeff_cst / coeff_ref);
  PolyVector vec = *vec_ref * factor;
  *constraint = *constraint - vec;
  return 1;
}

void ConstraintReduce(PolyMatrix *constraints, string var) {
  size_t i;
  PolyMatrix matExp;
  int min_coeff_idx = -1;
  int min_coeff = static_cast<int>(1e20);
  for (i = 0; i < constraints->size(); i++) {
    PolyVector row = constraints->GetVector(i);
    if (row.get_cond() == PolyVector::OP_EQ) {
      int coeff = row.get_coeff(var);
      if (coeff != 0) {
        int abs_coeff = abs(coeff);
        if (abs_coeff < min_coeff) {
          min_coeff_idx = i;
          min_coeff = abs_coeff;
        }
      }
    }
  }

  if (-1 != min_coeff_idx) {
    PolyVector &vecEquality = constraints->GetVector(min_coeff_idx);
    for (i = 0; i < constraints->size(); i++) {
      if (i == static_cast<size_t>(min_coeff_idx)) {
        continue;
      }
      PolyVector &row = constraints->GetVector(i);
      ConstraintReduce(&row, &vecEquality, var);
    }

    vector<string> vecVars = vecEquality.get_vars_no_const();
    vecEquality = PolyVector(0);
    vecEquality.set_iterators(&vecVars);
  }
}

int PolyVector::substitude(string var, PolyVector exp) {
  PolyVector &temp = *this;
  PolyVector var_1(var, -1);
  exp = exp + var_1;
  return ConstraintReduce(&temp, &exp, var);
}

PolyMatrix GetExpressionSimple(PolyMatrix *constraints,
                               const vector<string> &var,
                               const vector<string> &param) {
  size_t i;
  size_t j;
  PolyMatrix reduced_mat;

  if (0 == constraints->size()) {
    return reduced_mat;
  }
  vector<string> vecVars = constraints->GetVector(0).get_vars_no_const();
  vector<string> vecVars_int;
  for (i = 0; i < vecVars.size(); i++) {
    for (j = 0; j < var.size(); j++) {
      if (vecVars[i] == var[j]) {
        break;
      }
    }
    if (j != var.size()) {
      continue;
    }
    for (j = 0; j < param.size(); j++) {
      if (vecVars[i] == param[j]) {
        break;
      }
    }
    if (j != param.size()) {
      continue;
    }

    vecVars_int.push_back(vecVars[i]);
  }

  for (i = 0; i < vecVars_int.size(); i++) {
    ConstraintReduce(constraints, vecVars_int[i]);
    constraints->remove_redundant_vector();
#if DEBUG_GETDEPENDENCE
    {
      printf("[Dependence simplify] Step %d: variable=%s\n", i,
             vecVars_int[i].c_str());
      printf("%s\n", constraints->print().c_str());
    }
#endif
  }

  for (i = 0; i < constraints->size(); i++) {
    PolyVector row = constraints->GetVector(i);

    // remove inequality
    if (row.get_cond() != PolyVector::OP_EQ) {
      continue;
    }

    // remove the equalities which has intermediate values;
    for (j = 0; j < vecVars_int.size(); j++) {
      if (row.get_coeff(vecVars_int[j]) != 0) {
        break;
      }
    }
    if (j != vecVars_int.size()) {
      continue;
    }

    reduced_mat.append_vector(row);
  }

  {
    vector<string> vecIters = merge_iterator(var, param);
    reduced_mat.set_iterators(&vecIters);
  }

  // printf("HERE 0:\n");
  // printf("%s\n", reduced_mat.print().c_str());

  // split the equality to multiple ones for variables
  PolyMatrix new_rows;
  if (true) {
    for (i = 0; i < reduced_mat.size(); i++) {
      PolyVector row = reduced_mat.GetVector(i);
      vector<string> vecVar = row.get_vars_no_const();

      int bOut = 0;
      while (bOut == 0) {
        // 1. get the var with maximal coefficient
        string varMax;
        {
          int max_coeff = 0;
          for (j = 0; j < var.size(); j++) {
            int coeff = abs(row.get_coeff(var[j]));
            if (coeff > max_coeff) {
              varMax = var[j];
              max_coeff = coeff;
            }
          }
          bOut = static_cast<int>(varMax.empty());
          if (bOut != 0) {
            break;
          }
        }

        // 2. get the expression of the varMax
        PolyVector new_row;
        {
          // FIXME: direct divide may have problems!!!
          new_row = row / (-row.get_coeff(varMax));
          new_rows.append_vector(new_row);
        }

        // 3. update row to eliminate varMax
        ConstraintReduce(&row, &new_row, varMax);
      }
    }
    new_rows = new_rows.remove_redundant_variables();
  } else {
    new_rows = reduced_mat;
  }

  // printf("HERE 1:\n");
  // printf("%s\n", new_rows.print().c_str());

  return new_rows;
  // return reduced_mat;
}

void ConstraintReduceEquality(PolyMatrix *constraints, string var) {
  PolyMatrix mat;
  for (size_t i = 0; i < constraints->size(); i++) {
    if (constraints->GetVector(i).get_cond() != PolyVector::OP_EQ) {
      continue;
    }
    mat.append_vector(constraints->GetVector(i));
  }

  ConstraintReduce(&mat, var);
  mat.remove_redundant_vector();

  *constraints = mat;
}

void InequalityReduce(PolyVector *constraint, PolyVector *vec_ref,
                      const string &var, int is_upper) {
  int coeff_ref = vec_ref->get_coeff(var);
  int coeff_cst = constraint->get_coeff(var);

  if (0 == coeff_cst || 0 == coeff_ref) {
    return;
  }
  if (0 != abs(coeff_cst) % abs(coeff_ref)) {
    return;
  }
  if (is_upper == static_cast<int>(coeff_cst < 0)) {
    return;
  }

  PolyVector factor(coeff_cst / coeff_ref);
  PolyVector vec = *vec_ref * factor;
  *constraint = *constraint - vec;
}

void PolyVector::reduce_coefficient() {
  vector<string> vecVars;
  get_nonzero_vars(&vecVars);

  int min_abs = static_cast<int>(1e20);
  size_t i;
  for (i = 0; i < vecVars.size(); i++) {
    int coeff = abs(get_coeff(vecVars[i]));
    if (coeff < min_abs) {
      min_abs = coeff;
    }
  }

  int dividable = 1;
  for (i = 0; i < vecVars.size(); i++) {
    int coeff = abs(get_coeff(vecVars[i]));
    if (coeff % min_abs != 0) {
      dividable = 0;
    }
  }
  if (dividable != 0) {
    *this = *this / min_abs;
  }
}

#define DEBUG_INEQUALITY_REDUCE 0

void InequalityReduce(PolyMatrix *constraints, string var) {
  size_t i;
  size_t j;
  PolyMatrix matExp;

  // FM elimination
  PolyMatrix vecUpperBounds;
  PolyMatrix vecLowerBounds;
  for (i = 0; i < constraints->size(); i++) {
    PolyVector row = constraints->GetVector(i);
    int coeff = row.get_coeff(var);
    if (row.get_cond() == PolyVector::OP_GE && coeff != 0) {
      if (coeff < 0) {
        vecUpperBounds.append_vector(row);
      } else {
        vecLowerBounds.append_vector(row);
      }
    } else if (row.get_cond() == PolyVector::OP_EQ && coeff != 0) {
      if (coeff < 0) {
        vecLowerBounds.append_vector(-row);
        vecUpperBounds.append_vector(row);
      } else {
        vecLowerBounds.append_vector(row);
        vecUpperBounds.append_vector(-row);
      }
    }
  }

#if DEBUG_INEQUALITY_REDUCE
  {
    std::cout << "LOWER BOUNDS: " << endl;
    std::cout << vecLowerBounds.print() << endl;
    std::cout << "UPPER BOUNDS: " << endl;
    std::cout << vecUpperBounds.print() << endl;
  }
#endif

  matExp = *constraints;
  for (i = 0; i < vecUpperBounds.size(); i++) {
    PolyVector row1 = vecUpperBounds.GetVector(i);
    int coeff1 = row1.get_coeff(var);
    for (j = 0; j < vecLowerBounds.size(); j++) {
      PolyVector row2 = vecLowerBounds.GetVector(j);
      int coeff2 = row2.get_coeff(var);

      PolyVector new_row(0);

      PolyVector vcoeff1(-coeff1);
      PolyVector vcoeff2(coeff2);

      PolyVector term1 = row1 * vcoeff2;
      PolyVector term2 = row2 * vcoeff1;

      new_row = term1 + term2;
      if (row1.get_cond() == PolyVector::OP_GE ||
          row2.get_cond() == PolyVector::OP_GE) {
        new_row.set_coeff(PolyVector::OP_FLAG_POINTER, PolyVector::OP_GE);
      } else if (row1.get_cond() == PolyVector::OP_GT ||
                 row2.get_cond() == PolyVector::OP_GT) {
        assert(0);
      }
      new_row.reduce_coefficient();

      //             if (abs(coeff1) >= abs(coeff2) && 0 == abs(coeff1) %
      // abs(coeff2))
      //             {
      //                 PolyVector ratio(-coeff1/coeff2);
      //                 PolyVector tmp = row2 * ratio;
      //                 new_row = row1 + tmp;
      //             }
      //             else if (abs(coeff1) <= abs(coeff2) && 0 == abs(coeff2) %
      // abs(coeff1))
      //             {
      //                 PolyVector ratio(-coeff2/coeff1);
      //                 new_row = row1 * ratio + row2;
      //             }
      //             else
      //             {
      //                 printf("[PolyModel] InequalityReduce: coefficients to
      //                 be
      // reduced are not dividable: %d%s %d%s\n", coeff1, var.c_str(), coeff2,
      // var.c_str());                 assert(0);  //FIXME:
      //             }

#if DEBUG_INEQUALITY_REDUCE
      {
        std::cout << "Add new row: ";
        std::cout << new_row.print() << endl;
      }
#endif
      matExp.append_vector(new_row);
    }
    vector<string> vecVar = constraints->get_iterators();
    matExp.set_iterators(&vecVar);
  }

#if DEBUG_INEQUALITY_REDUCE
  {
    std::cout << "MERGED: " << endl;
    std::cout << matExp.print() << endl;
  }
#endif
  matExp.remove_redundant_vector();
#if DEBUG_INEQUALITY_REDUCE
  {
    std::cout << "MERGED+reduce_coeff: " << endl;
    std::cout << matExp.print() << endl;
  }
#endif

  for (i = 0; i < constraints->size(); i++) {
    PolyVector row = constraints->GetVector(i);
    int coeff = row.get_coeff(var);
    if (row.get_cond() == PolyVector::OP_GE && coeff != 0) {
      PolyVector &vecEquality = matExp.GetVector(i);
      vector<string> vecVars = vecEquality.get_vars_no_const();
      vecEquality = PolyVector(0);
      vecEquality.set_iterators(&vecVars);
    } else if (row.get_cond() == PolyVector::OP_EQ && coeff != 0) {
      PolyVector &vecEquality = matExp.GetVector(i);
      vector<string> vecVars = vecEquality.get_vars_no_const();
      vecEquality = PolyVector(0);
      vecEquality.set_iterators(&vecVars);
    }
  }
#if DEBUB_PRINT
  printf("hehe0 %s", matExp.print().c_str());
  matExp.remove_redundant_vector();
  printf("hehe1 %s\n\n\n", matExp.print().c_str());
#else
  matExp.remove_redundant_vector();
#endif

  *constraints = matExp;
}

PolyMatrix InequalityReducetoBound(PolyMatrix *constraints,
                                   const string &sVar) {
  size_t i;
  vector<string> sVars = constraints->GetVector(0).get_vars_no_const();
  for (i = 0; i < sVars.size(); i++) {
    if (sVars[i] != sVar) {
      InequalityReduce(constraints, sVars[i]);
    }
  }

  constraints->remove_redundant_vector();

  return *constraints;
}

PolyMatrix GetLoopBounds(PolyMatrix domain, PolyMatrix order) {
  size_t i;
  PolyMatrix access_dummy;
  PolyMatrix constraints0 =
      CombineAccessConstraints(domain, order, access_dummy, "", "");
  PolyMatrix reduced_mat;

  if (0 == constraints0.size()) {
    return reduced_mat;
  }
  vector<string> vecVars = constraints0.GetVector(0).get_vars_no_const();
  // for (i = 0; i < order.size(); i++)
  for (i = 0; i < vecVars.size(); i++) {
    string matched = "_order";
    if (0 != vecVars[i].find(matched)) {
      ConstraintReduce(&constraints0, vecVars[i]);
    }
  }

  constraints0 = constraints0.remove_redundant_variables();

  // further reduce inequality
  {
    vector<string> vecVars = constraints0.GetVector(0).get_vars_no_const();
    for (i = 0; i < vecVars.size(); i++) {
      PolyMatrix copy = constraints0;
      PolyMatrix matBounds = InequalityReducetoBound(&copy, vecVars[i]);
      reduced_mat.append_matrix(matBounds);
    }
    reduced_mat.set_iterators(&vecVars);
  }

  return reduced_mat;
}

PolyMatrix GetDependence(string sArray, PolyMatrix domain0, PolyMatrix order0,
                         PolyMatrix access0, PolyMatrix domain1,
                         PolyMatrix order1, PolyMatrix access1,
                         int skip_reduce /*=0*/) {
  size_t i;
  PolyMatrix constraints0 =
      CombineAccessConstraints(domain0, order0, access0, sArray, "_0");
  PolyMatrix constraints1 =
      CombineAccessConstraints(domain1, order1, access1, sArray, "_1");
  vector<string> vecVar0 = constraints0.GetVector(0).get_vars_no_const();
  vector<string> vecVar1 = constraints1.GetVector(0).get_vars_no_const();
  vecVar0 = merge_iterator(vecVar0, vecVar1);
  constraints0.append_matrix(constraints1);
  constraints0.set_iterators(&vecVar0);

#if DEBUG_GETDEPENDENCE
  printf("[Dependence simplify] Input: \n");
  printf("%s\n", constraints0.print().c_str());
#endif

  vector<string> vecIter0;
  for (i = 0; i < order0.size(); i++) {
    vecIter0.push_back("_order" + my_itoa(i) + "_0");
  }
  for (i = 0; i < access0.get_parameters().size(); i++) {
    vecIter0.push_back("__" + access0.get_parameters()[i] + "_0");
  }

  vector<string> vecIter1;
  for (i = 0; i < order1.size(); i++) {
    vecIter1.push_back("_order" + my_itoa(i) + "_1");
  }
  for (i = 0; i < access1.get_parameters().size(); i++) {
    vecIter1.push_back("__" + access1.get_parameters()[i] + "_1");
  }

  if (skip_reduce != 0) {
    return constraints0;
  }
  PolyMatrix reduced = GetExpressionSimple(&constraints0, vecIter0, vecIter1);
  return reduced;
}

#define DEBUG_GETDEEPBOUNDS 0

void GetDeepBounds(const PolyMatrix &_constraints, string sVar,
                   int *upper_bound, int *lower_bound) {
  PolyMatrix constraints = _constraints;
  *upper_bound = PolyVector::BOUND_MAX;
  *lower_bound = -PolyVector::BOUND_MAX;

  if (constraints.size() == 0) {
    return;
  }

#if DEBUG_GETDEEPBOUNDS
  std::cout << "[PolyModel] GETDEEPBOUNDS - start: var_to_evaluate =" << sVar
            << endl;
  std::cout << constraints.print() << endl;
#endif

  vector<string> vecVars = constraints.GetVector(0).get_vars_no_const();
  vector<string> vecVarsRemain;
  size_t j;
  for (size_t i = 0; i < vecVars.size(); i++) {
    if (vecVars[i] == sVar) {
      continue;
    }

    int hit = 0;
    for (j = 0; j < constraints.size(); j++) {
      if (0 != constraints.GetVector(j).get_coeff(vecVars[i])) {
        hit++;
      }
    }
    if (hit > 1) {
      vecVarsRemain.push_back(vecVars[i]);
      continue;
    }

    if (vecVars[i] != sVar) {
#if DEBUG_GETDEEPBOUNDS
      std::cout << "[PolyModel] GETDEEPBOUNDS - simplify 0 var_to_reduce ="
                << vecVars[i] << endl;
      std::cout << constraints.print() << endl;
#endif
      //             ConstraintReduce(constraints, vecVars[i]);
      // #if DEBUG_GETDEEPBOUNDS
      //             std::cout << "[PolyModel] GETDEEPBOUNDS - simplify 1
      // var_to_reduce =" << vecVars[i] << endl;             std::cout <<
      // constraints.print() << endl; #endif
      InequalityReduce(&constraints, vecVars[i]);
#if DEBUG_GETDEEPBOUNDS
      std::cout << "[PolyModel] GETDEEPBOUNDS - simplify 2 var_to_reduce ="
                << vecVars[i] << endl;
      std::cout << constraints.print() << endl;
#endif
      constraints.remove_redundant_vector();
    }
  }

  for (size_t i = 0; i < vecVarsRemain.size(); i++) {
#if DEBUG_GETDEEPBOUNDS
    std::cout << "[PolyModel] GETDEEPBOUNDS - simplify 0 var_to_reduce ="
              << vecVarsRemain[i] << endl;
    std::cout << constraints.print() << endl;
#endif
    //             ConstraintReduce(constraints, vecVars[i]);
    // #if DEBUG_GETDEEPBOUNDS
    //             std::cout << "[PolyModel] GETDEEPBOUNDS - simplify 1
    // var_to_reduce
    // ="
    // << vecVars[i] << endl;             std::cout << constraints.print() <<
    // endl; #endif
    InequalityReduce(&constraints, vecVarsRemain[i]);
#if DEBUG_GETDEEPBOUNDS
    std::cout << "[PolyModel] GETDEEPBOUNDS - simplify 2 var_to_reduce ="
              << vecVarsRemain[i] << endl;
    std::cout << constraints.print() << endl;
#endif
    constraints.remove_redundant_vector();
  }

#if DEBUG_GETDEEPBOUNDS
  std::cout << "[PolyModel] GETDEEPBOUNDS - all simplified" << endl;
  std::cout << constraints.print() << endl;
#endif

  *upper_bound = constraints.GetUpperBoundSimple(sVar);
  *lower_bound = constraints.GetLowerBoundSimple(sVar);
}

void ConstraintReduceDeep(PolyMatrix *constraints, string var) {
  size_t i;

  int found_to_reduce = 1;
  string var_to_reduce = var;
  while (found_to_reduce != 0) {
    found_to_reduce = 0;
    ConstraintReduce(constraints, var_to_reduce);
    constraints->remove_redundant_vector();

    for (i = 0; i < constraints->size(); i++) {
      PolyVector vec = constraints->GetVector(i);
      vector<string> vecVars;
      vec.get_nonzero_vars(&vecVars);
      if (vecVars.size() == 1 && vec.get_cond() == PolyVector::OP_EQ) {
        found_to_reduce = 1;
        var_to_reduce = vecVars[0];
        break;
      }
    }
  }
}

// int GetDeepBounds_LP_int(PolyMatrix & constrains, string sVar, int & bound,
// int isUpper)
//{
//    int i, j;
//    if (constrains.size() == 0) return 1;
//
//    vector<string> vecVars = constrains.GetVector(0).get_vars_no_const();
//    int numOfVar = vecVars.size();
//    //cout << "num of Var " << numOfVar << endl;
//
//    int numOfRow = constrains.size();
//    //cout << "num of Row " << numOfFor << endl;
//
//    int numOfCoeffs = numOfVar * (numOfRow);
//
//    glp_prob *lp;
//    int *row_idx;
//    row_idx = new int [numOfCoeffs+1]; // index start from 1...
//    int *col_idx;
//    col_idx = new int [numOfCoeffs+1];
//    double *weight;
//    weight = new double[numOfCoeffs+1];
//    int coeff_idx = 0;
//
//    lp = glp_create_prob();
//    glp_set_prob_name(lp, "polymatrix_get_var_bound");
//    if (isUpper)
//        glp_set_obj_dir(lp, GLP_MAX);
//    else
//        glp_set_obj_dir(lp, GLP_MIN);
//
//    glp_add_rows(lp, constrains.size());
//    for(i = 0; i < constrains.size(); i++)
//    {
//        PolyVector row = constrains.GetVector(i);
//        int bound = -row.get_const();
//        string row_name = "row" + my_itoa(i);
//        glp_set_row_name(lp, i+1, row_name.c_str());
//        if (PolyVector::OP_EQ == row.get_cond())
//            glp_set_row_bnds(lp, i+1, GLP_FX, bound, bound);        // 0 for
// objective         else if (PolyVector::OP_GE == row.get_cond())
// glp_set_row_bnds(lp,
// i+1, GLP_LO, bound, bound);         else if (PolyVector::OP_GT ==
// row.get_cond())             glp_set_row_bnds(lp, i+1, GLP_LO, bound+1,
// bound+1); else
//        {
//            assert(0);
//        }
//
//        for (j = 0; j < vecVars.size(); j++)
//        {
//            // fill one coeff
//            row_idx[coeff_idx+1] = i+1;
//            col_idx[coeff_idx+1] = j+1;
//            weight[coeff_idx+1] = (double)row.get_coeff(vecVars[j]);
//
//            coeff_idx++;
//        }
//    }
//
//    glp_add_cols(lp, numOfVar);
//
//    for(i = 0; i < vecVars.size(); i++)
//    {
//        glp_set_col_name(lp, i+1, vecVars[i].c_str());
//        glp_set_col_bnds(lp, i+1, GLP_FR, 0.0, 0.0);
//        //glp_set_col_kind(lp, i+1, GLP_IV);
//        int coeff = (sVar == vecVars[i]);
//        glp_set_obj_coef(lp, i+1, (double)coeff);
//    }
//
//    //    cout << "row_count " << row_count << endl;
//    //    cout << "count " << count << endl;
//
//    glp_set_obj_coef(lp, 0, 0);
//    glp_load_matrix(lp, coeff_idx, row_idx, col_idx, weight);
//    glp_term_out(GLP_OFF);  // disable terminal output
//
//    glp_write_lp(lp, 0, "asdf.txt");
//
//    glp_simplex(lp, NULL);
//    glp_intopt(lp, NULL);
//
//    int flag = glp_get_status(lp);
//
//    //glp_print_sol(lp, "/dev/tty");
//    //glp_print_mip(lp, "/dev/tty");
//
// #if 0
//     switch(flag)
//     {
//     case GLP_OPT:
//         cout << "Solution is optimal" << endl; break;
//     case GLP_FEAS:
//         cout << "Solution is feasible" << endl; break;
//     case GLP_INFEAS:
//         cout << "Solution is infeasible" << endl; break;
//     case GLP_NOFEAS:
//         cout << "Problem has no feasible solution" << endl; break;
//     case GLP_UNBND:
//         cout << "Problem has unbounded solution" << endl; break;
//     case GLP_UNDEF:
//         cout << "Solution is undefined" << endl; break;
//     }
// #endif
//
//    int ret;
//    if(flag == GLP_OPT)
//    {
//        //for(int i = 0; i < numOfVar; i++)
//        //    getNodeFromId(i)->setBeginTime((int)glp_get_col_prim(lp, i+1));
//        bound = (int)glp_get_obj_val(lp);  //FIXME if the value is not an
// integer         ret = 1;
//    }
//    else
//    {
//        ret = 0;
//    }
//
//    glp_delete_prob(lp);
//    delete [] row_idx;
//    delete [] col_idx;
//    delete [] weight;
//
//    return ret;
//}
//
// int GetDeepBounds_LP(PolyMatrix & constrains, string sVar, int & upper_bound,
// int & lower_bound)
//{
//    int ret = 1;
//    ret &= GetDeepBounds_LP_int(constrains, sVar, upper_bound, 1);
//    ret &= GetDeepBounds_LP_int(constrains, sVar, lower_bound, 0);
//    return ret;
//}

// added by mhhuang
void PolyVector::set_coeff(string var, int coeff) {
  assert(m_vars.find(var) != m_vars.end());
  m_coeffs[m_vars[var]] = coeff;
}

vector<tldm_variable_range> tldm_polyhedral_info::get_parameter_range() {
  size_t i;
  vector<tldm_variable_range> ret;
  vector<string> vars = str_split(parameter_vector, ',');
  vector<vector<string>> ranges = str_split_2d(parameter_range, ",", "..");
  for (i = 0; i < vars.size(); i++) {
    tldm_variable_range var_range;
    var_range.variable = vars[i];
    var_range.lb = ranges[i][0];
    var_range.ub = ranges[i][1];
    ret.push_back(var_range);
  }
  return ret;
}

vector<tldm_variable_range> tldm_polyhedral_info::get_iterator_range() {
  size_t i;
  vector<tldm_variable_range> ret;
  vector<string> vars = str_split(iterator_vector, ',');
  vector<vector<string>> ranges = str_split_2d(iterator_range, ",", "..");
  for (i = 0; i < vars.size(); i++) {
    tldm_variable_range var_range;
    var_range.variable = vars[i];
    var_range.lb = ranges[i][0];
    var_range.ub = ranges[i][1];
    ret.push_back(var_range);
  }
  return ret;
}
vector<tldm_variable_range> tldm_polyhedral_info::get_variable_range() {
  vector<tldm_variable_range> ret0 = get_iterator_range();
  vector<tldm_variable_range> ret1 = get_parameter_range();
  ret0.insert(ret0.end(), ret1.begin(), ret1.end());
  return ret0;
}

tldm_polyhedral_info tldm_polyinfo_merge(tldm_polyhedral_info task,
                                         tldm_polyhedral_info access) {
  size_t i;
  tldm_polyhedral_info merged;

  // 1. direct append:
  //    iterator, iterator range, order, condition
  string comma =
      (!task.iterator_vector.empty() && !access.iterator_vector.empty()) ? ","
                                                                         : "";
  merged.iterator_vector =
      task.iterator_vector + comma + access.iterator_vector;
  comma = (!task.iterator_range.empty() && !access.iterator_range.empty()) ? ","
                                                                           : "";
  merged.iterator_range = task.iterator_range + comma + access.iterator_range;

  // fixed by zhangpeng: 2013-10-08
  {
    size_t ii;
    vector<string> vec_order0 = str_split(task.order_vector, ',');
    vector<string> vec_order1 = str_split(access.order_vector, ',');
    vector<string> merged_order;
    for (ii = 0; ii < vec_order0.size(); ii++) {
      merged_order.push_back(vec_order0[ii]);
    }
    for (ii = 1; ii < vec_order1.size(); ii++) {
      merged_order.push_back(vec_order1[ii]);
    }
    merged.order_vector = str_merge(',', merged_order);

    merged_order.clear();
    vec_order0 = str_split(task.condition_vector, ',');
    vec_order1 = str_split(access.condition_vector, ',');
    for (ii = 0; ii < vec_order0.size() - 1; ii++) {
      merged_order.push_back(vec_order0[ii]);
    }
    merged_order.push_back(vec_order0[ii] + "&&" + vec_order1[0]);
    for (ii = 1; ii < vec_order1.size(); ii++) {
      merged_order.push_back(vec_order1[ii]);
    }
    merged.condition_vector = str_merge(',', merged_order);

    // merged.order_vector    = task.order_vector + "," + access.order_vector;
    // merged.condition_vector= task.condition_vector + "," +
    // access.condition_vector;
  }

  // 2. merge without duplication
  //    parameter and parameter range
  vector<string> vecParamTask = str_split(task.parameter_vector, ',');
  vector<string> vecParamAccess = str_split(access.parameter_vector, ',');

  vector<vector<string>> vecPRangeTask =
      str_split_2d(task.parameter_range, ",", "..");
  vector<vector<string>> vecPRangeAccess =
      str_split_2d(access.parameter_range, ",", "..");

  map<string, string> map_LB;
  map<string, string> map_UB;

  for (i = 0; i < vecParamTask.size(); i++) {
    vector<string> vecBound = vecPRangeTask[i];
    string var = vecParamTask[i];
    if (map_LB.find(var) != map_LB.end()) {
      assert(map_LB[var] == vecBound[0] && map_UB[var] == vecBound[1]);
    } else {
      map_LB[var] = vecBound[0];
      map_UB[var] = vecBound[1];
    }
  }

  map<string, string>::iterator it;
  vector<string> sParam;
  vector<string> sParamRange;
  for (it = map_LB.begin(); it != map_LB.end(); it++) {
    sParam.push_back(it->first);
    sParamRange.push_back(map_LB[it->first] + ".." + map_UB[it->first]);
  }

  merged.parameter_vector = str_merge(',', sParam);
  merged.parameter_range = str_merge(',', sParamRange);

  merged.access_pattern = access.access_pattern;

  return merged;
}

//////////////////////////////////////////////////////////////////////////
// 2013-09-30 zhangpeng
// This function performs dependence analysis for the new TLDM format which
// contains parameters and conditions in the ID and accesses. Note: 1. according
// to the execution model,
//       (1) if the dependence is carried on the outer loops or the inner loops,
//       the distance with be performed in the standard way without changing the
//       execution order (2) if the dependence is carried on the tldm loops, we
//       will try to merge the loops to determine the minimal synchronization
//       level
// 2. The dependence is calculated in conservative way:
//    (1) if the loop bounds are not equal for the loop levels, the distance
//    will carry-propagate to the upper level. (2) if condition vector is not 1
//    (always true),           the distance will carry-propagate to the upper
//    level. (3) if the iterator names are not matched,                the
//    distance will carry-propagate to the upper level. (can be fixed later) (4)
//    if one iterator exist in more dimensions which are not equal, the distance
//    will carry-propagate to the upper level. (5) if multiple iterators exist
//    in one dimension, and fails to be separated, carry-propagation (6) if
//    coefficient is not equal, carry_propogation (7) if non-linear term,
//    carry_propogation
// 3. ref0 and ref1 are the combined access pattern contains the outer loops of
// interests and inner loops
string DependenceAnalysisTLDMFormat(tldm_polyhedral_info ref0,
                                    tldm_polyhedral_info ref1, int outer_level,
                                    int gid_level) {
  tldm_polynomial aa("(1)*(mm_0)");
  tldm_polynomial bb = aa.get_linear_coefficients("mm_0");
  tldm_polynomial cc("1");
  cc.merge_term();

  size_t level;
  size_t i;
  size_t j;
  // 1 parse string into vectors
  vector<string> iterator_vector0 = str_split(ref0.iterator_vector, ",");
  vector<string> iterator_vector1 = str_split(ref1.iterator_vector, ",");

  vector<string> parameter_vector0 = str_split(ref0.parameter_vector, ",");
  vector<string> parameter_vector1 = str_split(ref1.parameter_vector, ",");

  vector<vector<string>> iterator_range0 =
      str_split_2d(ref0.iterator_range, ",", "..");
  vector<vector<string>> iterator_range1 =
      str_split_2d(ref1.iterator_range, ",", "..");

  vector<vector<string>> parameter_range0 =
      str_split_2d(ref0.parameter_range, ",", "..");
  vector<vector<string>> parameter_range1 =
      str_split_2d(ref1.parameter_range, ",", "..");

  vector<string> order_vector0 = str_split(ref0.order_vector, ",");
  vector<string> order_vector1 = str_split(ref1.order_vector, ",");

  vector<string> condition_vector0 = str_split(ref0.condition_vector, ",");
  vector<string> condition_vector1 = str_split(ref1.condition_vector, ",");

  vector<string> access_vector0 = str_split(ref0.access_pattern, ":");
  vector<string> access_vector1 = str_split(ref1.access_pattern, ":");

  //////////////////////////////////////////////////////////////////////////
  // ZP: 2014-01-04: for nonlinear form like memcpy, return conservative
  // dependence
  // assert(access_vector0.size() == 2);
  // assert(access_vector1.size() == 2);
  if (access_vector0.size() != 2 || access_vector1.size() != 2) {
    string s_dist;
    for (i = 0; i < min(order_vector0.size(), order_vector1.size()); i++) {
      s_dist += "*";
      if (i != min(order_vector0.size(), order_vector1.size()) - 1) {
        s_dist += ",";
      }
    }

    if (DEBUG_GETDEPENDENCE) {
      printf("\tdist=%s\n\n", s_dist.c_str());
    }

    return s_dist;
  }
  //////////////////////////////////////////////////////////////////////////

  vector<string> access_pattern_vector0 = str_split(access_vector0[1], ",");
  vector<string> access_pattern_vector1 = str_split(access_vector1[1], ",");

  // int access_direction0 = (access_vector0[0] == "w");
  // int access_direction1 = (access_vector1[0] == "w");

  if (access_pattern_vector0.size() != access_pattern_vector1.size()) {
    assert(0);  // array dim is not equal
  }

  // 2 prepare polynomial
  vector<tldm_variable_range> tldm_range0;
  vector<tldm_variable_range> tldm_range1;
  {
    tldm_variable_range range;
    for (i = 0; i < iterator_range0.size(); i++) {
      range.variable = iterator_vector0[i];
      range.lb = iterator_range0[i][0];
      range.ub = iterator_range0[i][1];
      tldm_range0.push_back(range);
    }
    for (i = 0; i < parameter_range0.size(); i++) {
      range.variable = parameter_vector0[i];
      range.lb = parameter_range0[i][0];
      range.ub = parameter_range0[i][1];
      tldm_range0.push_back(range);
    }

    for (i = 0; i < iterator_range1.size(); i++) {
      range.variable = iterator_vector1[i];
      range.lb = iterator_range1[i][0];
      range.ub = iterator_range1[i][1];
      tldm_range1.push_back(range);
    }
    for (i = 0; i < parameter_range1.size(); i++) {
      range.variable = parameter_vector1[i];
      range.lb = parameter_range1[i][0];
      range.ub = parameter_range1[i][1];
      tldm_range1.push_back(range);
    }
  }

  vector<tldm_polynomial> access_pattern_poly0;
  vector<tldm_polynomial> access_pattern_poly1;
  for (i = 0; i < access_pattern_vector0.size(); i++) {
    // tldm_polynomial a(access_pattern_vector0[i], tldm_range0);
    // tldm_polynomial b("((((tp)*(TS_P))+(pp)))+(1))");

    access_pattern_poly0.push_back(
        tldm_polynomial(access_pattern_vector0[i], tldm_range0));
    access_pattern_poly1.push_back(
        tldm_polynomial(access_pattern_vector1[i], tldm_range1));
  }

  vector<tldm_polynomial> access_pattern_poly0_org = access_pattern_poly0;
  vector<tldm_polynomial> access_pattern_poly1_org = access_pattern_poly1;

  // change iterator name to xxx_0 and xxx_1, they will be renamed back when
  // they are matched.
  for (i = 0; i < access_pattern_poly0.size(); i++) {
    tldm_polynomial &poly = access_pattern_poly0[i];
    for (j = 0; j < iterator_vector0.size(); j++) {
      poly.rename_variable(iterator_vector0[j], iterator_vector0[j] + "_0");
    }

    tldm_polynomial &poly1 = access_pattern_poly1[i];
    for (j = 0; j < iterator_vector1.size(); j++) {
      poly1.rename_variable(iterator_vector1[j], iterator_vector1[j] + "_1");
    }
  }

  // 3 calculate dependence level by level
  string sDistance;
  vector<string> vec_dist;
  for (level = 0; level < min(order_vector0.size(), order_vector1.size());
       level++) {
    string dist;
    if ((level % 2) == 0) {  // position level
      int order0 = my_atoi(order_vector0[level]);
      int order1 = my_atoi(order_vector1[level]);
      if ((level <= static_cast<size_t>(2) * outer_level) ||
          (level > static_cast<size_t>(2) * (outer_level + gid_level))) {
        dist = my_itoa((order0 < order1) ? 1 : (order0 > order1) ? -1 : 0);
      } else {
        // try to merge the loop, so the dependence can only carry on the loop
        // that can not be merged.
        dist = "0";
      }

      if (DEBUG_GETDEPENDENCE_DETAIL) {
        printf("[level %d] position level:\n", static_cast<int>(level));
      }
    } else {  // loop level
      //// TO BE FIXED: 2013-11-15

      string sIter0 = order_vector0[level];
      string sIter1 = order_vector1[level];
      size_t loop_level = level / 2;

      if (DEBUG_GETDEPENDENCE_DETAIL) {
        printf("[level %d] loop level:\n", static_cast<int>(level));
      }

      // (1). if loop bound is not the same, return *

      // (2). if condition vector is not 1 (always true), return *
      {
        string cond0 = condition_vector0[level];
        if (cond0 != "1" && cond0 != "1&&1") {
          dist = "*";
          goto DISTANCE_OF_ONE_LEVEL_IS_DETERMINED;
        }

        string cond1 = condition_vector1[level];
        if (cond1 != "1" && cond0 != "1&&1") {
          dist = "*";
          goto DISTANCE_OF_ONE_LEVEL_IS_DETERMINED;
        }
      }

      // (3). if the iterator names are not matched, return *
      {
        string iter0 = iterator_vector0[loop_level];
        string iter1 = iterator_vector1[loop_level];
        if (iter0 != iter1) {
          dist = "*";
          goto DISTANCE_OF_ONE_LEVEL_IS_DETERMINED;
        }

        if (DEBUG_GETDEPENDENCE_DETAIL) {
          printf("iterator: %s\n", iter0.c_str());
        }
      }

      // (4), (5)
      {
        // a) get expressions
        // b) go through each dimension

        assert(access_pattern_poly0.size() == access_pattern_poly1.size());

        if (DEBUG_GETDEPENDENCE_DETAIL) {
          printf("ref0: \n");
          for (i = 0; i < access_pattern_poly0.size(); i++) {
            printf("\t%s\n", access_pattern_poly0[i].exp_to_string().c_str());
          }
          printf("ref1: \n");
          for (i = 0; i < access_pattern_poly1.size(); i++) {
            printf("\t%s\n", access_pattern_poly1[i].exp_to_string().c_str());
          }
        }

        dist = "__not_found__";
        string curr_iter = iterator_vector0[loop_level];
        for (i = 0; i < access_pattern_poly0.size(); i++) {
          tldm_polynomial coeff0 =
              access_pattern_poly0[i].get_linear_coefficients(curr_iter + "_0");
          tldm_polynomial coeff1 =
              access_pattern_poly1[i].get_linear_coefficients(curr_iter + "_1");

          tldm_polynomial zero("0");

          // (6), (7)
          if (!((coeff0.is_valid() != 0) && (coeff0.is_valid() != 0))) {
            dist = "*";
            goto DISTANCE_OF_ONE_LEVEL_IS_DETERMINED;
          }
          if (!((coeff0 - coeff1) == zero)) {
            dist = "*";
            goto DISTANCE_OF_ONE_LEVEL_IS_DETERMINED;
          }
          if (coeff0 == zero) {
            continue;
          }

          tldm_polynomial access0 = access_pattern_poly0[i];
          tldm_polynomial access1 = access_pattern_poly1[i];
          access0.rename_variable(curr_iter + "_0", curr_iter);
          access1.rename_variable(curr_iter + "_1", curr_iter);

          tldm_polynomial diff = access0 - access1;

#ifdef PRINT_DEBUG_POLY_DIV
          printf("Polynomial DIV: \"%s\" / \"%s\" = \"\n",
                 diff.exp_to_string().c_str(), coeff0.exp_to_string().c_str());
#endif
          tldm_polynomial dist_poly = diff / coeff0;

#ifdef PRINT_DEBUG_POLY_DIV
          printf("\t%s\" \n", dist_poly.exp_to_string().c_str());
#endif
          if (dist_poly.is_valid() == 0) {
            assert(0);
          }

          if (dist_poly.is_valid() == 0) {
            dist = "*";
            goto DISTANCE_OF_ONE_LEVEL_IS_DETERMINED;
          }
          int nDist_lb;
          int nDist_ub;
          string curr_dist;
          if ((dist_poly.get_lower_bound_const(&nDist_lb) != 0) &&
              (dist_poly.get_upper_bound_const(&nDist_ub) != 0) &&
              nDist_ub == nDist_lb) {
            curr_dist = my_itoa(nDist_lb);
          } else {
            curr_dist = "*";
          }

          if (dist != "__not_found__" && dist != curr_dist) {
            // distance in multiple dimensions does not match => return *
            dist = "*";
            goto DISTANCE_OF_ONE_LEVEL_IS_DETERMINED;
          }

          dist = curr_dist;

          if (dist == "*") {
            goto DISTANCE_OF_ONE_LEVEL_IS_DETERMINED;
          }
        }

        if (dist == "__not_found__") {
          dist = "x";
          goto DISTANCE_OF_ONE_LEVEL_IS_DETERMINED;
        }

        // By setting the matched iterator back to the original name,
        // the corresponding terms will be reduced, because they always have the
        // same coefficient
        for (i = 0; i < access_pattern_poly0.size(); i++) {
          assert(dist != "*");

          access_pattern_poly0[i].rename_variable(curr_iter + "_0", curr_iter);
          access_pattern_poly1[i].rename_variable(curr_iter + "_1", curr_iter);

          if (dist != "0") {
            // replace variable to make it reducable: i -> i - dist
            // tldm_polynomial new_iter_0("("+curr_iter+")-("+dist+")",
            // tldm_range0); access_pattern_poly0[i] =
            // access_pattern_poly0[i].get_polynomial_by_substitute(
            // curr_iter,
            // new_iter_0);

            vector<tldm_polynomial> poly_coeff;
            access_pattern_poly0[i].get_polynomial_coefficients(curr_iter,
                                                                &poly_coeff);
            assert(poly_coeff.size() <= 2);
            if (poly_coeff.size() == 2) {
              tldm_polynomial sub_term = poly_coeff[1] * tldm_polynomial(dist);
              access_pattern_poly0[i] = access_pattern_poly0[i] - sub_term;
            }
          }
        }
      }
    }

  DISTANCE_OF_ONE_LEVEL_IS_DETERMINED:

    if (DEBUG_GETDEPENDENCE) {
      printf("\tdist=%s\n\n", dist.c_str());
    }

    vec_dist.push_back(dist);
    sDistance += dist;
    // if (dist != "0") { return sDistance; }
    if (level + 1 < min(order_vector0.size(), order_vector1.size())) {
      sDistance += ",";
    }
  }

  // ZP: fixed 2013-11-14
  // if all zero, that means the access has nothing to do with the iterators.
  // It only contains one single location in all the iterators.
  // In this cases, if the access has any variable, then data reuse distance is
  // * Otherwise (no variable), if the constant is the same, then distance is
  // also *, Otherwise distance is MAX, which means no depedence
  {
    for (i = 0; i < vec_dist.size(); i++) {
      if (vec_dist[i] != "0" && vec_dist[i] != "x") {
        break;
      }
    }
    if (i == vec_dist.size()) {
      int has_variable = 0;
      int is_equal = 1;
      for (size_t j = 0; j < access_pattern_poly0.size(); j++) {
        tldm_polynomial index0 = access_pattern_poly0_org[j];
        tldm_polynomial index1 = access_pattern_poly1_org[j];
        has_variable |= static_cast<int>(!index0.get_nonzero_var().empty());
        has_variable |= static_cast<int>(!index1.get_nonzero_var().empty());

        is_equal &= static_cast<int>(index0 == index1);

        if (has_variable != 0) {
          break;
        }
      }

      if ((has_variable != 0) || (is_equal != 0)) {
        // sDistance = sDistance;  // str_replace(sDistance, "x", "*");
      } else {
        sDistance = "na";
      }
    }
  }

  return sDistance;
}
