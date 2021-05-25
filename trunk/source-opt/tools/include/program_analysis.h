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


//  ///////////////////////////////////  /
//  Program Analysis Functionalities
//  E.g. Linear expression analysis,
//  range analysis and dependence analysis
//
//  Create date: 2015-07-16 (pengzhang@falcon-computing.com)
//  ///////////////////////////////////  /

#pragma once

#include <assert.h>
#include <algorithm>
#include <utility>
#include <map>
#include <unordered_map>
#include <string>
#include <tuple>
#include <vector>
#include <list>
#include <memory>
#include "file_parser.h"
#include "rose.h"
#include "codegen.h"
#include "common.h"

using std::shared_ptr;
using std::unique_ptr;
using std::unordered_map;

class CSageCodeGen;
typedef CSageCodeGen CMarsAST_IF;

enum access_type {
  NO_ACCESS = 0,
  READ = 1,
  WRITE = 2,
  READWRITE = 3,
};

namespace MarsProgramAnalysis {

class CMarsExpression;
class CMarsRangeExpr;

CMarsRangeExpr get_variable_range(void *sg_node, CMarsAST_IF *ast, void *pos,
                                  void *scope);
CMarsRangeExpr get_variable_range(CMarsAST_IF *ast,
                                  const CMarsExpression &expr);
void clear_variable_range(void *sg_node);
void set_variable_range(void *sg_node, CMarsRangeExpr mr);

//  ZP: 20170314 to support range copy for assert variables
void copy_variable_range(void *sg_from, void *sg_to);

bool has_variable_range(CMarsAST_IF *ast, void *sg_expr, void *pos, void *scope,
                        bool induct_check);

bool has_variable_range(CMarsAST_IF *ast, const CMarsExpression &);

std::unordered_map<void *, CMarsRangeExpr> get_range_analysis(CMarsAST_IF *ast);
//  ZP: 20150822: for temporay insersion of the statements
class CTempASTStmt {
 public:
  CTempASTStmt(void *temp_expr, CMarsAST_IF *ast, void *pos);
  ~CTempASTStmt();
  void reset_defuse();

 protected:
  void *m_temp_stmt;
  CMarsAST_IF *m_ast;
  int m_reset_defuse;
};

class CMarsVariable {
 public:
  CMarsVariable(void *var, CMarsAST_IF *ast, void *pos = nullptr,
                void *scope = nullptr);
  CMarsVariable(CMarsAST_IF *ast,
                CMarsRangeExpr mr);  //  a dummy variable with a given range

  //  Can not clear the range because the range is shared by othre objects
  //  ~CMarsVariable() { if (m_ast->IsBasicBlock(m_var)) clear_range(); }

  friend class CMarsExpression;
  friend class DUMMY;

 public:
  std::string unparse() const;
  //  0 iterator, 1 local, 2 expression, 3 argument
  std::string GetTypeString();

  CMarsRangeExpr get_range() const;

  //  Utility function: used for temporary CMarsExpression/CMarsRangeExpr
  //  objects
  void set_range(CMarsRangeExpr mr);

  //  Variable ranges
  int GetBoundAbs(int64_t *lb, int64_t *ub);

  void *get_pointer() const { return m_var; }
  CMarsAST_IF *get_ast() const { return m_ast; }
  void *get_pos() { return m_pos; }

  std::string print_var() const;
  std::string print() const;

  static bool is_tmp_var(void *var) {
    return s_map_tmpvar_name.find(var) != s_map_tmpvar_name.end();
  }
  static std::string get_tmp_var_name(void *var) {
    if (s_map_tmpvar_name.find(var) == s_map_tmpvar_name.end())
      return "";
    else
      return s_map_tmpvar_name[var];
  }

  bool operator<(const CMarsVariable &var) const;

#if KEEP_UNUSED
  int GetType();
  void clear_range();
  int IsConstantRange();
  int64_t GetLBAbs();
  int64_t GetUBAbs();
  int GetBoundExpr(CMarsExpression *lb, CMarsExpression *ub);
#endif

 protected:
  void *m_var;
  CMarsAST_IF *m_ast;
  void *m_pos;
  void *m_scope;
  static map<void *, std::string> s_map_tmpvar_name;
};

//  Initialized_name ->
extern map<void *, CMarsRangeExpr> g_variable_range_cache;
void analyze_all_variable_ranges(CMarsAST_IF *ast);

//  map iv vector to order vector
class CMarsInterleaveOrderMap {
 public:
  //  lexigraphic order comparison

 protected:
  std::vector<int> m_vec_even;
  std::vector<void *> m_vec_odd;
};

class DUMMY;

class CMarsExpression {
  friend class DUMMY;

 public:
  CMarsExpression(void *expr, CMarsAST_IF *ast, void *pos = nullptr,
                  void *scope = nullptr);
  CMarsExpression(CMarsAST_IF *ast, int64_t val);
  CMarsExpression(CMarsAST_IF *ast, int val);
  CMarsExpression(const CMarsExpression &op);

  CMarsExpression();

  explicit CMarsExpression(CMarsVariable mv);

  CMarsExpression(int64_t dummy0, int64_t dummy1, int64_t dummy2,
                  int64_t dummy3, int64_t dummy4, int finite_flag = 0) {
    //  dont care for other fields
    m_finite = finite_flag;
    m_int_type = V_SgTypeLong;
  }

 protected:
  CMarsExpression(void *expr, map<void *, CMarsExpression> coeff,
                  CMarsAST_IF *ast, int64_t const_i, void *pos, void *scope,
                  int int_type, int finite_flag);

  void print_debug();

 public:
  void *get_expr_from_coeff(bool final_expr = false) const;

 public:
  int is_INF() const { return m_finite != 0; }
  int is_pINF() const { return (m_finite & 1) == 1; }
  int is_nINF() const { return (m_finite & 2) == 2; }
  int is_fINF() const { return is_nINF() && is_pINF(); }

 public:
  std::string unparse() const;
  std::string print(int level = 0) const;
  std::string print_s() const { return unparse(); }
  std::string print_coeff(int level = 0) const;
  void *get_pointer() const { return m_expr; }
  CMarsAST_IF *get_ast() const { return m_ast; }
  void set_ast(CMarsAST_IF *ast) { m_ast = ast; }
  void *get_pos() const { return m_pos; }
  void set_pos(void *new_pos);
  void set_scope(void *new_scope) { m_scope = new_scope; }
  void get_vars(std::vector<void *> *vars) const;
  std::vector<void *> get_vars() const;
  int has_var(void *var) const;

  //  zero-order
  int IsSignned() const { return !m_ast->IsUnsignedType(m_int_type); }
  int IsConstant() const;
  int IsInteger() const { return m_ast && m_ast->IsIntegerType(m_int_type); }
  int64_t GetConstant() const {
    switch (m_int_type) {
    case V_SgTypeBool:
      return static_cast<bool>(m_const);
    case V_SgTypeChar:
    case V_SgTypeSignedChar:
      return static_cast<char>(m_const);
    case V_SgTypeUnsignedChar:
      return static_cast<unsigned char>(m_const);
    case V_SgTypeShort:
    case V_SgTypeSignedShort:
      return static_cast<int16_t>(m_const);
    case V_SgTypeUnsignedShort:
      return static_cast<uint16_t>(m_const);
    case V_SgTypeInt:
    case V_SgTypeSignedInt:
      return static_cast<int>(m_const);
    case V_SgTypeUnsignedInt:
      return static_cast<unsigned int>(m_const);
    case V_SgTypeUnsignedLong:
      return static_cast<uint64_t>(m_const);
    default:
      return m_const;
    }
  }
  void SetConstant(int64_t value) { m_const = value; }
  int64_t get_const() const { return GetConstant(); }
  void set_const(int64_t value) { SetConstant(value); }
  int is_zero() const {
    return !is_INF() && IsConstant() && 0 == GetConstant();
  }

  int IsAtomic() const;

  int IsAtomicWithNonDeterminedRange() const;

  int IsNormal() const;  //  all the coefficient are constant

  //  first-order
  int IsLinearToIterator() const;  //
  int GetConstantCoeff(
      void *var,
      int64_t *value_out) const;  //  return 0 if not a constant
  int64_t GetConstantCoeff(void *var) const;

  bool IsLoopInvariant(void *for_loop) const;

  bool IsStandardForm(void *for_loop, map<void *, int64_t> *iterators,
                      map<void *, int64_t> *invariants) const;

  //  high-order
  map<void *, CMarsExpression> *get_coeff() { return &m_coeff; }
  CMarsExpression get_coeff(void *var) const;
  int64_t get_const_coeff(void *var);
  void set_coeff(void *var, const CMarsExpression &coeff) {
    m_coeff[var] = coeff;
    reduce();
  }
  std::vector<std::vector<int64_t>> get_coeff_range_simple();
  int has_coeff_range_simple() const;

  CMarsRangeExpr get_range() const;
  bool has_range() const;
  int any_var_located_in_scope(void *scope);
  int any_var_located_in_scope_except_loop_iterator(void *scope);
  int is_movable_to(void *pos);

  void operator=(const CMarsExpression &op);

  void ParseVarBoundSimple(map<void *, void *> *lb, map<void *, void *> *ub);

  friend CMarsExpression operator-(const CMarsExpression &op);
  friend CMarsExpression operator+(const CMarsExpression &op1,
                                   const CMarsExpression &op2);
  friend CMarsExpression operator-(const CMarsExpression &op1,
                                   const CMarsExpression &op2);
  friend CMarsExpression operator+(const CMarsExpression &op1, int64_t op2);
  friend CMarsExpression operator-(const CMarsExpression &op1, int64_t op2);
  friend CMarsExpression operator*(const CMarsExpression &op1,
                                   const CMarsExpression &op2);
  friend CMarsExpression operator*(const CMarsExpression &op1, int64_t op2);
  friend CMarsExpression operator/(const CMarsExpression &op1,
                                   const CMarsExpression &op2);
  friend CMarsExpression operator/(const CMarsExpression &op1, int64_t op2);

  friend CMarsExpression operator%(const CMarsExpression &op1,
                                   const CMarsExpression &op2);
  friend CMarsExpression operator%(const CMarsExpression &op1, int64_t op2);
  friend CMarsExpression operator<<(const CMarsExpression &op1,
                                    const CMarsExpression &op2);
  friend CMarsExpression operator>>(const CMarsExpression &op1,
                                    const CMarsExpression &op2);
  friend int divisible(const CMarsExpression &op1, int64_t op2,
                       bool check_range);
  friend int operator==(const CMarsExpression &op1, const CMarsExpression &op2);
  friend int operator!=(const CMarsExpression &op1, const CMarsExpression &op2);
  friend int operator==(const CMarsExpression &op1, int64_t op2);
  friend int operator!=(const CMarsExpression &op1, int64_t op2);
  friend int operator>(const CMarsExpression &op1, const CMarsExpression &op2);
  friend int operator<(const CMarsExpression &op1, const CMarsExpression &op2);
  friend int operator>=(const CMarsExpression &op1, int64_t op2);

  int IsNonNegative() const;
  int IsNonPositive() const;

  int substitute(void *sg_var, const CMarsExpression &sg_expr);
  int substitute(void *sg_var, int64_t value);

//   bool hasOnlyIteratorsInScope(void *scope);
//
#if KEEP_UNUSED
  CMarsRangeExpr get_range_in_scope(void *range_scope);
#endif
  friend int operator>=(const CMarsExpression &op1, const CMarsExpression &op2);
  friend int operator<=(const CMarsExpression &op1, const CMarsExpression &op2);
  friend int operator>(const CMarsExpression &op1, int64_t op2);
  friend int operator<(const CMarsExpression &op1, int64_t op2);
  friend int operator<=(const CMarsExpression &op1, int64_t op2);

 protected:
  //  m_expr, m_ast -> m_coeff
  void analysis();
  void reduce();  //  remove the zero terms
  void induct();  //  find substitutions for variables

  //  youxiang: 2016-09-22, the api can be replaced by operator +
  //  which can handle infinite expression
  //  void AddValueconst (const CMarsExpression &&exp);

 protected:
  CMarsAST_IF *m_ast;

  void *m_expr;  //  The corresponding AST node, nullptr if no correspondence
  void *m_pos;   //  used for scope or position purpose
  void *m_scope;
  int m_float_pos;

  map<void *, CMarsExpression> m_coeff;
  int64_t m_const;

  int m_int_type;
  int m_finite;  //  the flag for m_finite: 0 for normal, 1 for pos_infinite, 2
                 //  for neg_infinite, 3 for infinite
};

extern const CMarsExpression pos_INF;
extern const CMarsExpression neg_INF;
extern const CMarsExpression full_INF;

class CMarsRangeExpr {
 public:
  static const int64_t MARS_INT64_MAX = 0x7fffffffffffffffl;
  static const int64_t MARS_INT64_MIN = 0x8000000000000000l;
  CMarsRangeExpr(const CMarsExpression &lb, const CMarsExpression &ub,
                 bool is_exact = false);
  CMarsRangeExpr(const std::vector<CMarsExpression> &lb,
                 const std::vector<CMarsExpression> &ub, bool is_exact = false);
  explicit CMarsRangeExpr(CMarsAST_IF *ast = nullptr) {
    m_is_exact = false;
    m_ast = ast;
  }  //  full range
  CMarsRangeExpr(CMarsAST_IF *ast,
                 void *type);  //  full range according to type
  CMarsRangeExpr(int64_t lb, int64_t ub, CMarsAST_IF *ast,
                 bool is_exact = false);
  CMarsRangeExpr(int64_t ublb, CMarsAST_IF *ast, bool is_exact = true);

  //  union operation
  void AppendLB(const CMarsExpression &lb);
  void AppendUB(const CMarsExpression &ub);
  void set_lb(const CMarsExpression &lb);
  void set_ub(const CMarsExpression &ub);
  void append_lb(const CMarsExpression &lb) { AppendLB(lb); }
  void append_ub(const CMarsExpression &ub) { AppendUB(ub); }

  int has_lb() const { return m_lb.size() > 0; }
  int has_ub() const { return m_ub.size() > 0; }
  int has_bound() const { return has_lb() || has_ub(); }

  void update_position(const t_func_call_path &fn_path, void *pos);
  void set_pos(void *pos);
  //  FIXME: to be deprecated, using the set based one
  CMarsExpression get_lb(int idx = 0) const;
  CMarsExpression get_ub(int idx = 0) const;

  std::vector<CMarsExpression> get_lb_set() const { return m_lb; }
  std::vector<CMarsExpression> get_ub_set() const { return m_ub; }

  int get_simple_bound(CMarsExpression *lb, CMarsExpression *ub) const;

  int is_simple_bound() const {
    CMarsExpression lb, ub;
    return get_simple_bound(&lb, &ub);
  }

  void get_vars(std::vector<void *> *reduced_all_vars) const;

  //  conservative
  int is_empty() const;

  int is_infinite() const { return m_lb.size() <= 0 || m_ub.size() <= 0; }

  void set_empty(CMarsAST_IF *ast = nullptr);

  int any_var_located_in_scope_except_loop_iterator(void *scope);
  int any_var_located_in_scope(void *scope);
  int is_movable_to(void *pos);
  int has_var(void *var);

  int substitute(void *var, void *pos);
  int substitute(void *var, const CMarsExpression &lb,
                 const CMarsExpression &ub, bool is_exact = false);

  std::string print_const_bound() const;
  std::string print() const;
  std::string print_e() const;

  bool length_is_greater_or_equal(int64_t len) const;

  bool IsConstantLength(int64_t *len) const;

  bool is_single_access() const;

  bool is_full_access(int64_t size) const;

 protected:
  std::string print(const std::vector<CMarsExpression> &bound, int is_lb) const;

  void reduce();

 public:
  CMarsAST_IF *get_ast() const;
  void set_ast(CMarsAST_IF *ast) { m_ast = ast; }

 public:
  friend CMarsRangeExpr operator-(const CMarsRangeExpr &op);
  friend bool operator==(const CMarsRangeExpr &op1, const CMarsRangeExpr &op2);
  friend CMarsRangeExpr operator+(const CMarsRangeExpr &op1,
                                  const CMarsRangeExpr &op2);
  friend CMarsRangeExpr operator-(const CMarsRangeExpr &op1,
                                  const CMarsRangeExpr &op2);
  friend CMarsRangeExpr operator*(const CMarsRangeExpr &op1, int64_t op2);
  friend CMarsRangeExpr operator/(const CMarsRangeExpr &op1,
                                  const CMarsExpression &op2);
  friend CMarsRangeExpr min(const CMarsRangeExpr &op1,
                            const CMarsRangeExpr &op2);
  friend CMarsRangeExpr max(const CMarsRangeExpr &op1,
                            const CMarsRangeExpr &op2);
  friend CMarsRangeExpr min(const CMarsExpression &op1,
                            const CMarsExpression &op2);
  friend CMarsRangeExpr max(const CMarsExpression &op1,
                            const CMarsExpression &op2);
  friend CMarsRangeExpr operator+(const CMarsRangeExpr &op1,
                                  const CMarsExpression &op2);
#if KEEP_UNUSED
  friend CMarsRangeExpr operator*(const CMarsRangeExpr &op1,
                                  const CMarsExpression &op2);
  friend CMarsRangeExpr operator*(const CMarsRangeExpr &op1,
                                  const CMarsRangeExpr &op2);
#endif

  int IsNonNegative() const;
  int IsNonPositive() const;

  int IsConstantBound() const { return is_const_bound(); }
  int is_const_bound() const { return is_const_lb() && is_const_ub(); }
  int is_const_lb() const;
  int is_const_ub() const;
  int64_t get_const_lb() const;
  int64_t get_const_ub() const;

  int IsConstant() const {
    return is_const_bound() && get_const_lb() == get_const_ub();
  }

  CMarsExpression GetConstant();

  //  These operations are done in a conservative way
  CMarsRangeExpr Union(const CMarsRangeExpr &range) const;
  CMarsRangeExpr Intersect(const CMarsRangeExpr &range) const;
  //  return true if *this cover mr
  //  update the position to create new merged range
  int is_cover(const CMarsRangeExpr &mr, CMarsRangeExpr *merged_range) const;

  bool is_exact() const { return m_is_exact; }
  void set_exact(bool is_exact) { m_is_exact = is_exact; }

  void refine_range_in_scope(void *range_scope, void *pos);
  void refine_range_in_scope_v2(void *range_scope, void *pos, void *array,
                                bool is_write, const t_func_call_path &path);
  void refine_range_in_scope_v2(void *range_scope, void *pos,
                                const t_func_call_path &path);
  void update_control_flag(void *range_scope, void *pos, void *array,
                           bool is_write, const t_func_call_path &path);
  void
  refine_range_get_remain_substitutes(void *range_scope, void *pos,
                                      const t_func_call_path &fn_path,
                                      std::vector<void *> *vec_sub_vars,
                                      std::vector<CMarsExpression> *vec_sub_lbs,
                                      std::vector<CMarsExpression> *vec_sub_ubs,
                                      std::vector<bool> *vec_sub_is_exact);
  int refine_range_in_scope_v2_exact(void *range_scope, void *pos,
                                     const t_func_call_path &fn_path);

 protected:
#if KEEP_UNUSED
  int refine_range_in_scope_one_variable(void *range_scope,
                                         std::vector<void *> *loops_in_scope);
#endif

 protected:
  //  Note: vector is used to represent multiple bounds
  //  each bound is a requirement to satisfy
  //  and these conditinos are combined together by "or" operator
  std::vector<CMarsExpression> m_lb;
  std::vector<CMarsExpression> m_ub;
  bool m_is_exact;  //  used to indicate full access of the range, ZP: 20160205
  CMarsAST_IF *m_ast;
};

typedef pair<int, int> CMarsRangeInt;

class CMarsDepDistSet {
 public:
  CMarsDepDistSet() { set_empty(); }
  CMarsDepDistSet(int64_t lb, int64_t ub) {
    m_lb = lb;
    m_ub = ub;
  }

  int has_lb() { return m_lb != CMarsRangeExpr::MARS_INT64_MIN; }
  int has_ub() { return m_ub != CMarsRangeExpr::MARS_INT64_MAX; }

  int64_t get_lb() { return m_lb; }
  int64_t get_ub() { return m_ub; }
  std::vector<int64_t> get_set();
  int is_empty();
  void set_empty() {
    m_lb = CMarsRangeExpr::MARS_INT64_MAX;
    m_ub = CMarsRangeExpr::MARS_INT64_MIN;
  }
  int64_t size();

  std::string print();

 protected:
  int64_t m_lb;
  int64_t m_ub;
};

class CMarsScope {
 public:
  explicit CMarsScope(void *scope) { m_scope = scope; }

  int IsLoop();
  int IsFuncbody();
  void *get_pointer() { return m_scope; }

 protected:
  void *m_scope;
};

class CMarsResultFlags {
 public:
  void set_flag(std::string flag_name, int64_t value) {
    m_flags[flag_name] = value;
  }
  int64_t get_flag(std::string flag_name) { return m_flags[flag_name]; }

 protected:
  map<std::string, int64_t> m_flags;
};

class CMarsAccess {
 public:
  CMarsAccess(void *ref, CMarsAST_IF *ast, void *scope, void *pos = nullptr);

  void set_scope(void *scope);

  void set_pos(void *pos);

  //  each CMarsRangeExpr for a dimension
  std::vector<CMarsRangeExpr> GetRangeExpr();
  CMarsRangeExpr GetRangeExpr(int idx);

  void *GetRef() const { return m_ref; }
  void *GetArray() const { return m_array; }
  void *get_ref() const { return m_ref; }
  CMarsAST_IF *get_ast() const { return m_ast; }
  void *get_array() const { return m_array; }
  void *get_scope() const { return m_scope; }
  std::vector<CMarsExpression> GetIndexes() const { return m_expr_list; }
  CMarsExpression GetIndex(size_t i) const {
    assert(i < m_expr_list.size());
    return m_expr_list[i];
  }
  int get_dim() const { return m_expr_list.size(); }

#if KEEP_UNUSED
  CMarsRangeExpr GetRangeExpr_old(int idx);
#endif

 public:
  //  Simple Access Type: Single Variable Continous access
  //  For each dimension: the index is like "i+1" or "i-2"
  //  1. each dimension has at most one non-zero variable
  //  2. the coefficient of the variable is constant 1 or -1
  //  3. each variable exist in at most one dimension
  bool is_simple_type_v1();
  static bool is_simple_type_v1(const std::vector<CMarsExpression> &indexes);
  //  for each array dim: std::tuple<var, coeff, const>
  std::vector<std::tuple<void *, int64_t, int64_t>>
  simple_type_v1_get_dim_to_var();
  //  for each variable: array_dim(-1 if not found), coeff, const
  //  is compact: if true, only return the tuples that matches the array index
  std::vector<std::tuple<int, int64_t, int64_t>>
  simple_type_v1_get_var_to_dim(std::vector<void *> vec_vars,
                                int is_compact = 0);

 public:
  std::string print();
  std::string print_s();
  friend int GetDependence(CMarsAccess access1, CMarsAccess access2,
                           CMarsDepDistSet *dist_set,
                           CMarsResultFlags *results_flags);

 protected:
  void analysis_exp();
  void analysis_var();

 protected:
  void *m_ref;
  void *m_array;
  CMarsAST_IF *m_ast;
  std::vector<CMarsExpression> m_expr_list;

  //  CMarsScope m_scope;
  void *m_scope;
  std::vector<void *> m_iv;
  std::vector<void *> m_pv;

  // TODO(pengzh): not implemented
  CMarsInterleaveOrderMap m_order_vector_map;

  //  for fake access nodes;
  void *m_pos;
};

struct RangeInfo {
  const void *sgnode;
  CMarsRangeExpr range;
  size_t acc_size;
  t_func_call_path func_path;
  RangeInfo(const void *sgnode, const CMarsRangeExpr &range, size_t acc_size)
      : sgnode(sgnode), range(range), acc_size(acc_size) {}
  explicit RangeInfo(const void *sgnode, const CMarsRangeExpr &range)
      : RangeInfo(sgnode, range, 1) {}
};

using range = CMarsRangeExpr;
using list_range = std::list<CMarsRangeExpr>;
using list_rinfo = std::list<RangeInfo>;
using vec_range = std::vector<CMarsRangeExpr>;
using vec_rinfo = std::vector<RangeInfo>;
using vec_list_range = std::vector<list_range>;
using vec_list_rinfo = std::vector<list_rinfo>;
using map2vec_list_range = linked_map<void *, shared_ptr<vec_list_range>>;
using map2vec_list_rinfo = linked_map<void *, shared_ptr<vec_list_rinfo>>;

struct Result {
  shared_ptr<vec_list_rinfo> vl_rinfo;

  explicit Result(shared_ptr<vec_list_rinfo> vl_rinfo) : vl_rinfo(vl_rinfo) {}
  explicit Result(size_t num_dims)
      : Result(std::make_shared<vec_list_rinfo>(num_dims)) {}

  void extend_1d(shared_ptr<Result> r2, size_t d) {
    vl_rinfo->at(d).insert(vl_rinfo->at(d).end(), r2->vl_rinfo->at(d).begin(),
                           r2->vl_rinfo->at(d).end());
  }

  void merge(shared_ptr<Result> r2) {
    for (size_t d = 0; d < r2->vl_rinfo->size(); ++d) {
      extend_1d(r2, d);
    }
  }
};

struct MyAST {
  string type;
  const void *sgnode;
  // -1:unitialized, 0:non_exact_condition, 1: exact_condition
  int true_branch_is_exact_condition;
  int false_branch_is_exact_condition;
  size_t trip_count;
  shared_ptr<Result> result;
  vector<shared_ptr<MyAST>> body;
  vector<shared_ptr<MyAST>> orelse;
  const void *true_body;
  const void *false_body;
  MyAST(const string &type, void *sgnode, shared_ptr<Result> result)
      : type(type), sgnode(sgnode), true_branch_is_exact_condition(-1),
        false_branch_is_exact_condition(-1), trip_count(1), result(result),
        true_body(nullptr), false_body(nullptr) {}
  MyAST(const string &type, void *sgnode) : MyAST(type, sgnode, nullptr) {}
};

class ArrayRangeAnalysis {
 private:
  using DFS_cache =
      std::map<std::pair<void *,                // ptr to SgFunctionDeclaration
                         void *>,               // ptr to SgInitializedName
               std::pair<shared_ptr<Result>,    // for read access
                         shared_ptr<Result>>>;  // for write access
  DFS_cache m_cache;
  CMarsAST_IF *m_ast;

  // must keep
  range m_flatten_range_w;
  range m_flatten_range_r;
  vec_range m_range_w;
  vec_range m_range_r;
  int64_t m_access_size;
  bool m_eval_access_size;
  bool m_check_access_bound;

  void *get_interesting_upper(void *node, void *root);
  bool child_of(void *subnode, void *node);
  bool child_of_if_true_body(void *node, void *ifstmt);
  shared_ptr<MyAST> create_tree(void *range_scope,
                                const map2vec_list_rinfo &range_for);
  shared_ptr<Result> postorder(shared_ptr<MyAST> tree, const void *range_scope,
                               int dim_size);
  void try_merge(shared_ptr<vec_list_rinfo> ranges, int dim);
  range try_merge(shared_ptr<list_range> ranges);
  void union_group_range_exact(shared_ptr<vec_list_rinfo> rinfos,
                               const void *range_scope, const void *upper,
                               int dim, size_t upper_trip_count = 1);
  range union_group_range(shared_ptr<vec_list_rinfo> rinfos,
                          const void *range_scope, int dim);
  shared_ptr<Result> Union(const map2vec_list_rinfo &range_for,
                           void *range_scope, int dim_size);
  std::pair<std::shared_ptr<Result>, std::shared_ptr<Result>>
  DFS_analyze(void *range_scope, void *array, void *ref_scope);
  void analyze(void *array, void *range_scope, void *ref_scope);

  // base range analysis
  bool equal_range(const list_rinfo &one_rinfos,
                   const list_rinfo &other_rinfos);
  void analyze_ref(void *curr_ref, void *range_scope, void *ref_scope, int dim,
                   bool all_dimension_size_known, size_t total_flatten_size,
                   int data_unit_size, const vector<size_t> &sg_sizes,
                   shared_ptr<vec_list_rinfo> r_range,
                   shared_ptr<vec_list_rinfo> w_range, void **new_scope,
                   void **new_range_scope, void **new_array,
                   void **new_func_call, int *new_pointer_dim);

  void propagate_range(void *curr_ref, void *range_scope,
                       const vec_list_rinfo &r1, const vec_list_rinfo &w1,
                       int dim, int new_pointer_dim,
                       shared_ptr<vec_list_rinfo> w_range,
                       shared_ptr<vec_list_rinfo> r_range);
  void get_range_with_offset(const list_rinfo &old_range,
                             const CMarsRangeExpr &offset,
                             list_rinfo *new_range);
  void substitute_parameter_atom_in_range(
      void *array, shared_ptr<vec_list_rinfo> read_range,
      shared_ptr<vec_list_rinfo> write_range, void *func_decl, void *func_call,
      void *range_scope);
  void check_exact_flag_for_loop(const void *loop, const void *range_scope,
                                 shared_ptr<vec_list_rinfo> range_group);
  void set_non_exact_flag(shared_ptr<vec_list_rinfo> range_group, int dim);

  void get_array_dimension_size(void *array, size_t *dim, int *data_unit_size,
                                int *dim_fake, bool *all_dimension_size_known,
                                size_t *total_flatten_size,
                                vector<size_t> *sg_sizes);

  bool is_empty_list_range(const list_rinfo &ranges);

  void intersect_with_access_range_info(void *array, void *ref_scope,
                                        void *range_scope);

  void use_access_range_info(void *array, void *ref_scope, void *range_scope,
                             shared_ptr<vec_list_rinfo> read_ranges,
                             shared_ptr<vec_list_rinfo> write_ranges);

  bool check_local_or_infinite_range(shared_ptr<vec_list_rinfo> ranges, int dim,
                                     void *range_scope);

  size_t merge_access_size(size_t one_access_size, size_t other_access_size);

 public:
  ArrayRangeAnalysis(void *array, CMarsAST_IF *ast, void *ref_scope,
                     void *range_scope, bool eval_access_size,
                     bool check_access_bound);

  vec_range GetRangeExprWrite() { return m_range_w; }
  vec_range GetRangeExprRead() { return m_range_r; }

  range GetFlattenRangeExprWrite() { return m_flatten_range_w; }
  range GetFlattenRangeExprRead() { return m_flatten_range_r; }
  int has_read();
  int has_write();

  std::string print();
  std::string print_s();

  int64_t get_access_size() { return m_access_size; }
};

class CMarsArrayRangeInScope {
 public:
  CMarsArrayRangeInScope(void *array, CMarsAST_IF *ast,
                         const std::list<t_func_call_path> &vec_call_path,
                         void *ref_scope, void *range_scope,
                         bool eval_access_size, bool check_access_bound);
  std::vector<CMarsRangeExpr> GetRangeExprWrite() { return m_range_w; }
  std::vector<CMarsRangeExpr> GetRangeExprRead() { return m_range_r; }

  CMarsRangeExpr GetFlattenRangeExprWrite() { return m_flatten_range_w; }
  CMarsRangeExpr GetFlattenRangeExprRead() { return m_flatten_range_r; }
  int has_read();
  int has_write();

  std::string print();
  std::string print_s();

  int64_t get_access_size() {
    assert(m_eval_access_size);
    return m_access_size;
  }

 protected:
  void *m_array;
  CMarsAST_IF *m_ast;
  void *m_scope;
  CMarsRangeExpr m_flatten_range_w;
  CMarsRangeExpr m_flatten_range_r;
  std::vector<CMarsRangeExpr> m_range_w;
  std::vector<CMarsRangeExpr> m_range_r;
  int64_t m_access_size;
  bool m_eval_access_size;
  bool m_check_access_bound;
};

void GetAccessRangeInScope(void *array, void *ref_scope, void *range_scope,
                           CMarsAST_IF *ast,
                           std::vector<CMarsRangeExpr> *r_range,
                           std::vector<CMarsRangeExpr> *w_range,
                           CMarsRangeExpr *r_flatten_range,
                           CMarsRangeExpr *w_flatten_range);

//  /////////////////////////////////////  /
int CheckAccessSeparabilityInScope(CMarsAST_IF *ast, void *array,
                                   void *ref_scope, void *boundary_scope,
                                   CMerlinMessage *msg);

void *CopyAccessInScope(CMarsAST_IF *ast, void *array, void *ref_scope,
                        void *boundary_scope, int *is_write,
                        map<void *, void *> *map_spref2snref = nullptr,
                        std::vector<void *> *vec_params = nullptr);
//  /////////////////////////////////////  /

int GetDependence(CMarsAccess access1, CMarsAccess access2,
                  CMarsDepDistSet *dist_set, CMarsResultFlags *results_flags);

std::vector<CMarsRangeExpr>
RangeUnioninVector(const std::vector<CMarsRangeExpr> &vec1,
                   const std::vector<CMarsRangeExpr> &vec2);

int is_exact_condition(CMarsAST_IF *ast, void *cond_stmt, void *scope,
                       bool true_branch);

void *GetAccessRangeFromIntrinsic(void *array, void *ref_scope,
                                  void *range_scope, CMarsAST_IF *ast,
                                  std::vector<CMarsRangeExpr> *range,
                                  bool *valid);

class DUMMY {
  std::vector<int64_t> v;

 public:
  explicit DUMMY(const CMarsExpression *exp);

  explicit DUMMY(const CMarsVariable *var);

  DUMMY(CMarsAST_IF *ast, void *expr, void *pos, void *scope);

  bool operator<(const DUMMY &var) const;

  std::string toString();
};

extern map<DUMMY, CMarsRangeExpr> cache_get_range_exp;
extern map<DUMMY, std::tuple<bool, int64_t>> cache_consts;
extern map<DUMMY, bool> cache_IsMatchedFuncAndCall;
}  //  namespace MarsProgramAnalysis
