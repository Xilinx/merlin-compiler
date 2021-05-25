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


#include <cxxabi.h>
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <tuple>
#include <boost/tokenizer.hpp>
using std::cout;
using std::endl;
using std::list;
using std::map;
using std::set;
using std::string;
using std::tuple;
using std::unordered_map;
using std::vector;
#include "common.h"
#define _DEBUG_ROSE_SANITY_CHECK 0
#define _DEBUG_DEFUSE 0
#define _DEBUG_copy_and_replace_ref                                            \
  0  //  may cause issue when printing floating node
#define _DEBUG_get_induct_in_scope 0
#define _DEBUG_get_induct_in_scope_detail 0
#define ENABLE_OR_OP_GEN 0
#if PROJDEBUG
#define my_assert_or_return(retval) assert(0);
#else
#define my_assert_or_return(retval) return retval;
#endif

#include "rose.h"

#include "RoseAst.h"

#include "Outliner.hh"
#include "ir_types.h"
#include "mars_ir_v2.h"
#include "mars_ir.h"
#include "mars_message.h"
#include "history_message.h"
#include "id_update.h"
#include "mars_opt.h"
#include "codegen.h"
#include "file_parser.h"

#include "PolyModel.h"
#include "tldm_annotate.h"

#include "program_analysis.h"

#include "astPostProcessing.h"

using MarsProgramAnalysis::ArrayRangeAnalysis;
using MarsProgramAnalysis::CMarsAccess;
using MarsProgramAnalysis::CMarsExpression;
using MarsProgramAnalysis::CMarsRangeExpr;
using MarsProgramAnalysis::CMarsVariable;
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/ostreamwrapper.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/rapidjson.h"
//  using rapidjson;
#undef DEBUG
#define DISABLE_DEBUG 1
#if DISABLE_DEBUG
#define DEBUG(stmts)
#else
#define DEBUG(stmts)                                                           \
  do {                                                                         \
    stmts;                                                                     \
  } while (false)
#endif

static const std::unordered_set<string> xil_ap{"ap_int", "ap_uint", "ap_fixed",
                                               "ap_ufixed"};
static const std::unordered_set<string> xil_ap_int{"ap_int", "ap_uint"};
static const std::unordered_set<string> xil_ap_fixed{"ap_fixed", "ap_ufixed"};
static const std::unordered_set<string> xil_hls_stream{"hls::stream", "stream"};
static const std::unordered_set<string> std_complex{"std::complex", "complex"};
static const std::unordered_set<char> numeric_prefix{
    '(', ')', '+', '-', '*', '/', '>', '<', '=', '%', ',', '.', '&'};
bool g_muteMERLINError = false;
struct hash_pair {
  template <typename T1, typename T2>
  size_t operator()(std::pair<T1, T2> one) const {
    return std::hash<T1>()(one.first) ^ std::hash<T2>()(one.second);
  }
};

int IsWithLength(const CSageCodeGen *ast, void *sg_type) {
  if (ast->IsType(sg_type) == 0) {
    return 0;
  }
  if (ast->IsTypedefType(sg_type) == 0) {
    return 0;
  }
  if (ast->IsModifierType(
          isSgTypedefType(static_cast<SgNode *>(sg_type))->get_base_type()) ==
      0) {
    return 0;
  }
  static set<string> database;
  static bool init = false;
  if (!init) {
    init = true;
    for (int i = 1; i <= 512; ++i) {
      database.insert("int" + std::to_string(i));
      database.insert("uint" + std::to_string(i));
    }
  }

  string wholeType = ast->_p(sg_type);
  int index = wholeType.rfind(':');
  string type = wholeType.substr(index + 1, wholeType.length());
  if (database.find(type) != database.end()) {
    return 1;
  }
  return 0;
}

void tryBindNodeOrgId(CSageCodeGen *ast, void *node_, void *bindNode_,
                      bool force = false) {
  if (ast->is_floating_node(bindNode_) != 0) {
    HIS_WARNING("not bind for floating node");
    return;
  }

  SgNode *node = static_cast<SgNode *>(node_);
  SgNode *bindNode = static_cast<SgNode *>(bindNode_);
  if (node->class_name() != bindNode->class_name()) {
    HIS_WARNING("not bind for different type nodes");
    return;
  }

  if (node->class_name() == "SgForStatement") {
    bindNodeOrgId(ast, node_, bindNode_, force);
  } else {
    bindNodeOrgId(ast, node_, bindNode_, force);
    HIS_WARNING("bind for other types");
  }

  //  update qor_merge_mode
  string qorMergeMode = getQoRMergeMode(ast, bindNode);
  setQoRMergeMode(ast, node_, qorMergeMode);
}

void replaceExpressionWithHistory(CSageCodeGen *ast, SgExpression *exp1,
                                  SgExpression *exp2,
                                  bool keep_old_exp = true) {
  tryBindNodeOrgId(ast, exp2, exp1, true);
  string oldExpStr = printNodeInfo(ast, exp1);
  DEFINE_START_END;
  STEMP(start);
  /////////////////////////////////////////////////
  // Handling the cases that Expression Replacement does not work:
  /////////////////////////////////////////////////
  // SageInterface::replaceExpression(exp1, exp2, keepOldExp);
  if (auto sg_array_type = isSgArrayType(exp1->get_parent())) {
    if (sg_array_type->get_index() == (exp1)) {
      sg_array_type->set_index(exp2);
    }
  } else if (auto sg_case_option_stmt =
                 isSgCaseOptionStmt(exp1->get_parent())) {
    if (sg_case_option_stmt->get_key() == (exp1)) {
      sg_case_option_stmt->set_key(exp2);
      exp2->set_parent(sg_case_option_stmt);
    }
  } else {
    // Fix MER1671 keep old expression to avoid access invalid expression
    SageInterface::replaceExpression(exp1, exp2, true);
  }
  /////////////////////////////////////////////////

  ACCTM(sageReplaceExpr, start, end);
  if (exp1->get_parent() == nullptr ||
      (static_cast<int>(exp1->get_parent()->get_childIndex(exp1))) < 0) {
    HIS_ERROR("replace a floating node.");
    HIS_ERROR("parent: " << exp1->get_parent());
    if (exp1->get_parent() != nullptr) {
      HIS_ERROR("index: " << (static_cast<int>(
                    exp1->get_parent()->get_childIndex(exp1))));
    }
  } else if (isSkipped(ast, exp1)) {
    HIS_WARNING("do not mark history for skipped node.");
  } else {
    markDelete(ast, exp1->get_parent(), getTopoIndex(ast, exp1),
               "Replace(1): " + oldExpStr, exp1);
    markInsert(ast, exp2->get_parent(), getTopoIndex(ast, exp2),
               "Replace(2): " + printNodeInfo(ast, exp2));
  }
}

extern map<SgNode *, string> orgIdMap;
void deleteHistory(void *sg_node, void *ast) {
  auto *history = (static_cast<CSageCodeGen *>(ast))->getHistories();
  history->erase(static_cast<SgNode *>(sg_node));
  orgIdMap.erase(static_cast<SgNode *>(sg_node));
}

#define FORCE_ASSERT 0

//  also defined in tldm_task_extract.cpp
#define USE_LOWERCASE_NAME 1

#define DEBUG_FORCE_ASSERT 1

#define _DEBUG_PARSE_PNTR_ 0
#define _DEBUG_RANGE_ANALYSIS_ 0  //  may cause deadlock on division

#define _USE_CACHE_IN_TRAVERSE_ 1

//  #define DEBUG_FUNC_TRACE_UP

//  even not initialize, the range analysis will be performed as well when you
//  call get_range();
#define _INITIAL_RANGE_ANALYSIS_ 0

#include <boost/filesystem.hpp>
extern void GetTLDMInfo_withPointer4(void *sg_node, void *pArg);
extern void all_pragma_parse_whole(
    const string &sPragma, string *sFilter, string *sCmd,
    map<string, pair<vector<string>, vector<string>>> *mapParam,
    vector<string> *p_mapKey);
int test_dir_existence(const string &sDirName) {
#if 0
    //  cannot work if the directory has no write permission
    FILE * fp;
    sDirName += "/test_dir_existence.log";
    if (!(fp=fopen(sDirName.c_str(), "w"))) {
        return 0;
    }
    fclose(fp);
    sDirName = "rm -rf "+sDirName;
    system(sDirName.c_str());
    return 1;
#else
  boost::filesystem::path p(sDirName);
  if (exists(p) && is_directory(p)) {
    return 1;
  }
  return 0;
#endif
}

static vector<string> split_flags(const string &cflags) {
  size_t pos = cflags.find('-');
  vector<string> ret;
  if (pos == string::npos) {
    ret.push_back(cflags);
    return ret;
  }
  ret.push_back(cflags.substr(0, pos));
  size_t start = pos;
  while (start != cflags.size()) {
    size_t curr_start = start;
    while (start != cflags.size() && cflags[start] == '-') {
      start++;
    }
    pos = cflags.find('-', start);
    if (pos == string::npos) {
      ret.push_back(cflags.substr(curr_start));
      break;
    }
    ret.push_back(cflags.substr(curr_start, pos - curr_start));
    start = pos;
  }
  return ret;
}

//  const char message_file[] = "message.xml";
static const char *cpp_suffix[] = {"cpp", "cc", "C", "cxx", "c++"};
static const char *header_suffix[] = {"h", "hh", "H", "hxx", "hpp", "h++"};
static bool is_cpp_file(const string &file_name) {
  string::size_type pos = file_name.find_last_of('.');
  if (pos != string::npos) {
    string suffix = file_name.substr(pos + 1);
    for (size_t i = 0; i != sizeof(cpp_suffix) / sizeof(string); ++i) {
      if (cpp_suffix[i] == suffix) {
        return true;
        break;
      }
    }
  }
  return false;
}
static bool is_c_file(const string &file_name) {
  string::size_type pos = file_name.find_last_of('.');
  if (pos != string::npos) {
    string suffix = file_name.substr(pos + 1);
    return suffix == "c";
  }
  return false;
}
namespace MarsProgramAnalysis {

auto mars_expr_hash = [](const CMarsExpression &g) {
  return std::hash<string>{}(g.print_coeff(-1));
};
auto mars_expr_comp = [](const CMarsExpression &l, const CMarsExpression &r) {
  return l == r;
};
unordered_map<void *, MarsProgramAnalysis::CMarsRangeExpr> m_variable_range;
unordered_map<CMarsExpression, MarsProgramAnalysis::CMarsRangeExpr,
              decltype(mars_expr_hash), decltype(mars_expr_comp)>
    m_expr_range(10, mars_expr_hash, mars_expr_comp);
int m_init_range = 0;
//  ZP :20150725 : to be extended later
static CMarsRangeExpr analyze_variable_range(void *sg_node, CMarsAST_IF *ast,
                                             void *pos, void *scope) {
  //  ZP: 20160303
  if (ast->IsForStatement(sg_node) != 0) {
    ast->init_range_analysis_for_loop(sg_node);
    if (m_variable_range.find(sg_node) != m_variable_range.end()) {
      return m_variable_range[sg_node];
    }
    return CMarsRangeExpr(ast);
  }
  if (ast->IsInitializer(sg_node) != 0) {
    sg_node = ast->GetInitializerOperand(sg_node);
  }
  if (ast->IsExpression(sg_node) || ast->IsInitName(sg_node)) {
    return CMarsRangeExpr(CMarsExpression(sg_node, ast, pos, scope),
                          CMarsExpression(sg_node, ast, pos, scope));
  } else {
    return CMarsRangeExpr(neg_INF, pos_INF, false);
  }
  //  return CMarsRangeExpr();
}

//  ZP: 20170314 to support range copy for assert variables
void copy_variable_range(void *sg_from, void *sg_to) {
  if (m_variable_range.end() != m_variable_range.find(sg_from)) {
    if (m_variable_range.end() == m_variable_range.find(sg_to)) {
      set_variable_range(sg_to, m_variable_range[sg_from]);
    }
  }
}

void set_variable_range(void *sg_node, CMarsRangeExpr mr) {
  if (m_init_range == 0) {
    mr.get_ast()->reset_range_analysis();
  }
  if (m_variable_range.find(sg_node) == m_variable_range.end()) {
    m_variable_range[sg_node] = mr;
  } else {
#ifdef PROJDEBUG
    printf("ERROR: can not reset range for CMarsVariable directly\n Please "
           "call clear_variable_range() first.\n");
    assert(0);
#endif
  }
}
#if 0
void clear_variable_range(void *sg_node) { m_variable_range.erase(sg_node)); }
#endif
unordered_map<void *, CMarsRangeExpr> get_range_analysis(CMarsAST_IF *ast) {
  if (m_init_range == 0) {
    ast->reset_range_analysis();
  }
  return m_variable_range;
}

#if 0
void *getSingleDef(CSageCodeGen *ast, void *sg_var, void *pos) {
  vector<void *> vec_defs = ast->GetVarDefbyPosition(sg_var, pos);
  if (vec_defs.size() != 1) {
    return nullptr;
  }

  return vec_defs[0];
}
#endif

vector<void *> getSuitableDef(CSageCodeGen *ast, void *sg_var, void *loc) {
  vector<void *> vec_defs = ast->GetVarDefbyPosition(sg_var, loc);
  vector<void *> ret;
  for (auto e : vec_defs) {
    if ((ast != nullptr) && ast->isDom(loc, e)) {
      ret.push_back(e);
    }
  }
  return ret;
}

bool getRangeFromCache(CSageCodeGen *ast, void *sg_var, void *pos, void *scope,
                       CMarsRangeExpr *range, bool induct_check) {
  if (m_variable_range.find(sg_var) != m_variable_range.end()) {
    *range = m_variable_range[sg_var];
    return true;
  }
  if ((ast == nullptr) || (ast->is_floating_node(sg_var) != 0) ||
      (ast->is_floating_node(pos) != 0)) {
    return false;
  }

  CMarsRangeExpr res(ast);
  if (!induct_check) {
    void *type = nullptr;
    if (ast->IsExpression(sg_var) != 0) {
      type = ast->GetTypeByExp(sg_var);
    } else if (ast->IsInitName(sg_var) != 0) {
      type = ast->GetTypebyVar(sg_var);
    }
    if (type != nullptr) {
      res = res.Intersect(CMarsRangeExpr(ast, type));
    }
  }
  bool find = false;
  void *single_def = nullptr;
  if (isSgVarRefExp(static_cast<SgNode *>(sg_var)) != nullptr) {
    if (ast->has_side_effect(sg_var) != 0) {
      return false;
    }
    void *var = ast->GetVariableInitializedName(sg_var);
    vector<void *> varDefs = ast->GetVarDefbyPosition(var, pos);
    if (varDefs.size() == 1) {
      single_def = varDefs[0];
    }
  } else {
    single_def = sg_var;
  }
  //  ALGOP("RAN", "get range for var " << printNodeInfo(ast, sg_var)
  //                   << " in line " << ast->GetFileInfo_line(pos));
  for (auto &ele : m_variable_range) {
    vector<void *> assertDefs;
    void *assert_node = ele.first;
    void *stmt = ast->TraceUpToStatement(assert_node);
    void *single_assert_def = nullptr;
    if (isSgVarRefExp(static_cast<SgNode *>(assert_node)) != nullptr) {
      void *var = ast->GetVariableInitializedName(assert_node);
      vector<void *> assertDefs = ast->GetVarDefbyPosition(var, assert_node);
      if (assertDefs.size() == 1) {
        single_assert_def = assertDefs[0];
      }

    } else {
      single_assert_def = assert_node;
    }
    //  ALGOP("RAN", "working on the assertion for "
    //                  << printNodeInfo(ast, ele.first) << " in line "
    //                  << ast->GetFileInfo_line(ele.first));
    //  ALGOP("RAN", "its def is in line " <<
    //  ast->GetFileInfo_line(assert_def));
    if ((single_def == single_assert_def) && (single_def != nullptr) &&
        (ast != nullptr) && ast->isDom(stmt, pos)) {
      res = res.Intersect(ele.second);
      find = true;
    }
  }

  if (find && (isSgVarRefExp(static_cast<SgNode *>(sg_var)) != nullptr)) {
    m_variable_range[sg_var] = res;
  }
  if (!find && isSgVarRefExp(static_cast<SgNode *>(sg_var)) != nullptr) {
    if (ast->has_side_effect(sg_var) != 0) {
      return false;
    }
    static std::unordered_set<pair<void *, void *>,
                              boost::hash<pair<void *, void *>>>
        visited;
    auto key = std::make_pair(sg_var, scope);
    if (visited.count(key) <= 0) {
      visited.insert(key);
      void *var = ast->GetVariableInitializedName(sg_var);
      void *sg_loop = ast->GetLoopFromIteratorByPos(var, pos, 0);
      if (nullptr == sg_loop) {
        vector<void *> varDefs = ast->GetVarDefbyPosition(var, pos);
        // Youxiang: 10202019 If it is single-def, CMarsExpression may have
        // handled and applied induction if moveable test is also statisfied.
        // Currently, we just apply aggressive analysis to get more accurate
        // range for multiple definitions. At the beginning, I tried to handle
        // both single and multiple definitions issue but affected our other
        // part of range analysis. It is very sensitive to do minor change for
        // our current range analysis.
        if (varDefs.size() > 1) {
          bool all_has_range = !varDefs.empty();
          vector<CMarsRangeExpr> vec_def_range;
          for (auto def : varDefs) {
            void *assign_val =
                ast->GetSingleAssignExprForSingleDef(var, def, scope);
            if (assign_val && ast->has_side_effect(assign_val) == 0) {
              CMarsExpression me_def(assign_val, ast, assign_val, scope);
              if (!me_def.has_range()) {
                all_has_range = false;
                break;
              }
              vec_def_range.push_back(me_def.get_range());
            } else {
              all_has_range = false;
              break;
            }
          }
          if (all_has_range && !vec_def_range.empty()) {
            CMarsRangeExpr def_union_range = vec_def_range[0];
            for (size_t i = 1; i < vec_def_range.size(); ++i)
              def_union_range = def_union_range.Union(vec_def_range[i]);
            res = def_union_range;
          }
        }
      }
      visited.erase(key);
    }
  }
  *range = res;
  return range->has_bound() != 0;
}

bool getRangeFromCache(CSageCodeGen *ast, const CMarsExpression &expr,
                       CMarsRangeExpr *range) {
  if (ast == nullptr) {
    return false;
  }
  CMarsRangeExpr res(ast);
  if (!m_expr_range.empty()) {
    if (m_expr_range.count(expr) > 0) {
      *range = m_expr_range[expr];
      return range->has_bound() != 0;
    }
  }
  for (auto &[curr_expr, curr_range] : m_expr_range) {
    CMarsExpression diff = expr - curr_expr;
    if (diff.IsConstant() != 0) {
      *range = curr_range + diff;
      if ((diff != 0) != 0) {
        m_expr_range[expr] = *range;
      }
      return range->has_bound() != 0;
    }
  }
  *range = res;
  return false;
}

CMarsRangeExpr get_variable_range(void *sg_node, CMarsAST_IF *ast, void *pos,
                                  void *scope) {
  DEFINE_START_END;

  if (m_init_range == 0) {
    ast->reset_range_analysis();
  }

  CMarsRangeExpr res;
  bool has_cache = getRangeFromCache(ast, sg_node, pos, scope, &res, false);
#if 1  //  ad-hoc process for division "var/const"
  //  only do it if the coefficients of var_bound are all divisible by const
  bool divide_exp = false;
  {
    for (int t = 0; t < 1; t++) {  //  only for exit the scope
      void *op1 = nullptr;
      void *op2 = nullptr;
      int op = 0;
      if (!ast->IsAPIntOp(sg_node, &op1, &op2, &op) &&
          ast->IsDivideOp(sg_node)) {
        ast->GetExpOperand(sg_node, &op1, &op2);
        op = ast->GetExpressionOp(sg_node);
      }

      if (V_SgDivideOp != op) {
        break;
      }

      STEMP(start);
      int64_t int_value = 0;
      if (!ast->IsConstantInt(op2, &int_value, true, nullptr)) {
        break;
      }
      if (int_value == 0) {
        break;
      }
      ACCTM(get_variable_range___is_constant_div, start, end);

      int sign = static_cast<int>(int_value >= 0);
      int64_t abs = sign != 0 ? int_value : -int_value;

      STEMP(start);
      CMarsExpression me1(op1, ast, pos, scope);
      CMarsRangeExpr mr1 = me1.get_range();
      ACCTM(get_variable_range___get_div_range, start, end);

      STEMP(start);
      CMarsExpression me_lb;
      CMarsExpression me_ub;
      if (mr1.get_simple_bound(&me_lb, &me_ub) == 0) {
        break;
      }
      ACCTM(get_variable_range___get_simple_bound, start, end);

      STEMP(start);
      int const_lb = me_lb.get_const();
      int const_ub = me_ub.get_const();
      ACCTM(get_variable_range___get_const, start, end);

      STEMP(start);
      CMarsExpression new_lb;
      {
        CMarsExpression tmp = me_lb - const_lb;
        if (divisible(tmp, abs, false) != 0) {
          //  new_lb = tmp / abs + ceil(const_lb * 1.0 / abs);
          new_lb = tmp / abs + const_lb / abs;
        } else {
          new_lb = CMarsExpression(0, 0, 0, 0, 0, 2);
          new_lb.set_ast(ast);
          //  CMarsExpression(sg_node, ast, pos); //  neg_INF;
        }
      }
      ACCTM(get_variable_range___cal_lower_bound, start, end);

      STEMP(start);
      CMarsExpression new_ub;
      {
        CMarsExpression tmp = me_ub - const_ub;
        if (divisible(tmp, abs, false) != 0) {
          //  new_ub = tmp / abs + floor(const_ub * 1.0 / abs);
          new_ub = tmp / abs + const_ub / abs;
        } else {
          new_ub = CMarsExpression(0, 0, 0, 0, 0, 1);
          new_ub.set_ast(ast);
          //  new_ub = CMarsExpression(sg_node, ast, pos); //  pos_INF;
        }
      }
      ACCTM(get_variable_range___cal_upper_bound, start, end);

      STEMP(start);
      CMarsRangeExpr divide_range;
      if (sign != 0) {
        divide_range = CMarsRangeExpr(new_lb, new_ub, mr1.is_exact());
      } else {
        divide_range = CMarsRangeExpr(-new_ub, -new_lb, mr1.is_exact());
      }
      if (has_cache && (res.is_exact() == divide_range.is_exact())) {
        res = res.Intersect(divide_range);
      } else {
        res = divide_range;
      }
      ACCTM(get_variable_range___result, start, end);
      divide_exp = true;
    }
  }
  if (!divide_exp && !has_cache) {
    STEMP(start);
    CMarsRangeExpr range(analyze_variable_range(sg_node, ast, pos, scope));
    ACCTM(get_variable_range___analyze_variable_range, start, end);
    //  if (range.has_lb() || range.has_ub())
    //   m_variable_range[sg_node] = range;
    res = range;
  }
#endif

#if 1  //  refine loop iterator range according to position
  bool is_exact = true;
  if ((ast->IsForStatement(sg_node) != 0) && nullptr != pos) {
    void *loop_iterator = ast->GetLoopIterator(sg_node);
    if (loop_iterator != nullptr) {
      //  refine loop iterator range according to position
      map<void *, bool> conds;
      //  handle the corner case if start position is if-statement
      void *curr_pos = ast->TraceUpToStatement(pos);
      void *prev_pos = nullptr;
      if (curr_pos != nullptr) {
        prev_pos = curr_pos;
        curr_pos = ast->GetParent(curr_pos);
      }
      //  1. get all conditions from current position to current loop
      while (curr_pos != nullptr) {
        if (ast->IsFunctionDeclaration(curr_pos) != 0) {
          break;
        }
        if (prev_pos == sg_node) {
          break;
        }
        if (ast->IsIfStatement(curr_pos) != 0) {
          void *true_body = ast->GetIfStmtTrueBody(curr_pos);
          void *false_body = ast->GetIfStmtFalseBody(curr_pos);
          void *cond = ast->GetIfStmtCondition(curr_pos);
          assert(prev_pos);
          if (prev_pos == true_body) {
            conds[cond] = true;
          } else if (prev_pos == false_body) {
            conds[cond] = false;
          }
        }
        prev_pos = curr_pos;
        curr_pos = ast->GetParent(curr_pos);
      }
      //  2. process each condition
      for (auto &cond : conds) {
        void *cond_stmt = cond.first;
        bool true_branch = cond.second;
        void *cond_exp = ast->GetExprFromStmt(cond_stmt);
        vector<void *> vec_conds;
        if (true_branch) {
          ast->GetExprListFromAndOp(cond_exp, &vec_conds);
        } else {
          ast->GetExprListFromOrOp(cond_exp, &vec_conds);
        }
        for (auto &one_cond : vec_conds) {
          void *op0 = nullptr;
          void *op1 = nullptr;
          int op = 0;
          if (!ast->IsAPIntOp(one_cond, &op0, &op1, &op) &&
              ast->IsCompareOp(one_cond)) {
            ast->GetExpOperand(one_cond, &op0, &op1);
            op = ast->GetCompareOp(one_cond);
          }
          if (ast->IsCompareOp(op) == 0) {
            is_exact = false;
            continue;
          }
          CMarsExpression ce0(op0, ast, nullptr, scope);
          CMarsExpression ce1(op1, ast, nullptr, scope);
          CMarsExpression ce_diff = ce0 - ce1;
          auto vars = ce_diff.get_vars();
          bool reverse = false;
          bool support = false;
          CMarsExpression me_value;
          bool contains_single_iterator =
              vars.size() == 1 && ast->IsForStatement(vars[0]);
          for (auto &var : vars) {
            if (var == sg_node) {
              auto coeff = ce_diff.get_coeff(var);
              if (coeff.IsConstant() != 0) {
                int64_t const_coeff = coeff.GetConstant();
                if (std::abs(const_coeff) != 1) {
                  bool divisible = true;
                  for (auto &other_var : vars) {
                    if (other_var == var) {
                      continue;
                    }
                    auto other_coeff = ce_diff.get_coeff(other_var);
                    if ((other_coeff.IsConstant() == 0) ||
                        (other_coeff.GetConstant() % const_coeff) != 0) {
                      divisible = false;
                      break;
                    }
                  }
                  if ((ce_diff.GetConstant() % const_coeff) != 0) {
                    divisible = false;
                  }
                  if (!divisible) {
                    break;
                  }
                  ce_diff = ce_diff / const_coeff;
                  const_coeff /= std::abs(const_coeff);
                }
                support = true;
                if (const_coeff > 0) {
                  me_value =
                      CMarsExpression(ast->CreateVariableRef(loop_iterator),
                                      ast, one_cond) -
                      ce_diff;
                } else {
                  me_value =
                      CMarsExpression(ast->CreateVariableRef(loop_iterator),
                                      ast, one_cond) +
                      ce_diff;
                  reverse = true;
                }
              }
            } else if (ast->IsForStatement(var) != 0) {
              //  if there was any other iterator(s),
              //  it would result in infinite recursion.
              support = false;
              break;
            }
          }

          if (!support) {
            if (!contains_single_iterator)
              is_exact = false;
            continue;
          }
          CMarsExpression me_lb;
          CMarsExpression me_ub;
          int ret = res.get_simple_bound(&me_lb, &me_ub);
          if (ret == 0) {
            is_exact = false;
            continue;
          }
          if (me_value.IsConstant() == 0 && !me_value.has_range() &&
              (me_value - me_lb).IsConstant() == 0 &&
              (me_value - me_ub).IsConstant() == 0) {
            is_exact = false;
            continue;
          }
          bool normal_dir = true_branch ^ reverse;
          if (op == V_SgLessOrEqualOp) {
            if (normal_dir) {
              res = res.Intersect(CMarsRangeExpr(me_lb, me_value, 1));
            } else {
              res = res.Intersect(CMarsRangeExpr(me_value + 1, me_ub, 1));
            }
          }
          if (V_SgLessThanOp == op) {
            if (normal_dir) {
              res = res.Intersect(CMarsRangeExpr(me_lb, me_value - 1, 1));
            } else {
              res = res.Intersect(CMarsRangeExpr(me_value, me_ub, 1));
            }
          }
          if (V_SgGreaterOrEqualOp == op) {
            if (normal_dir) {
              res = res.Intersect(CMarsRangeExpr(me_value, me_ub, 1));
            } else {
              res = res.Intersect(CMarsRangeExpr(me_lb, me_value - 1, 1));
            }
          }
          if (V_SgGreaterThanOp == op) {
            if (normal_dir) {
              res = res.Intersect(CMarsRangeExpr(me_value + 1, me_ub, 1));
            } else {
              res = res.Intersect(CMarsRangeExpr(me_lb, me_value, 1));
            }
          }
          if (V_SgEqualityOp == op) {
            if (normal_dir) {
              res = res.Intersect(CMarsRangeExpr(me_value, me_value));
            } else {
              if ((me_value == me_lb) != 0) {
                res = res.Intersect(CMarsRangeExpr(me_value + 1, me_ub, 1));
              } else if ((me_value == me_ub) != 0) {
                res = res.Intersect(CMarsRangeExpr(me_lb, me_value - 1, 1));
              }
            }
          }
          if (V_SgNotEqualOp == op) {
            if (normal_dir) {
              if ((me_value == me_lb) != 0) {
                res = res.Intersect(CMarsRangeExpr(me_value + 1, me_ub, 1));
              } else if ((me_value == me_ub) != 0) {
                res = res.Intersect(CMarsRangeExpr(me_lb, me_value - 1, 1));
              }
            } else {
              res = res.Intersect(CMarsRangeExpr(me_value, me_value));
            }
          }
        }
      }
    }
  }
  if (res.is_exact()) {
    res.set_exact(is_exact);
  }
#endif
  void *cond_exp;
  void *true_exp;
  void *false_exp;
  if (ast->IsConditionalExp(sg_node, &cond_exp, &true_exp, &false_exp)) {
    CMarsExpression cond_val(cond_exp, ast, pos, scope);
    if (cond_val.IsConstant() != 0) {
      if (cond_val.GetConstant() != 0) {
        return CMarsVariable(true_exp, ast, pos, scope).get_range();
      }
      { return CMarsVariable(false_exp, ast, pos, scope).get_range(); }
    } else {
      return CMarsVariable(true_exp, ast, pos, scope)
          .get_range()
          .Union(CMarsVariable(false_exp, ast, pos, scope).get_range());
    }
  }
  return res;
}

CMarsRangeExpr get_variable_range(CMarsAST_IF *ast,
                                  const CMarsExpression &expr) {
  DEFINE_START_END;

  if (m_init_range == 0) {
    ast->reset_range_analysis();
  }

  CMarsRangeExpr res;
  getRangeFromCache(ast, expr, &res);
  return res;
}

bool has_variable_range(CSageCodeGen *ast, const CMarsExpression &expr) {
  assert(ast);
  if (m_init_range == 0) {
    ast->reset_range_analysis();
  }

  CMarsRangeExpr range;
  bool succ = getRangeFromCache(ast, expr, &range);
  return succ && (range.has_bound() != 0) &&
         !(range == CMarsRangeExpr(expr, expr, true));
}

bool has_variable_range(CSageCodeGen *ast, void *sg_expr, void *pos,
                        void *scope, bool induct_check) {
  assert(ast);
  if (m_init_range == 0) {
    ast->reset_range_analysis();
  }

  if (ast->IsForStatement(sg_expr) != 0) {
    ast->init_range_analysis_for_loop(sg_expr);
  }

#if 1  //  ZP: 20171021: div used in loop bound
  bool supported_divide_exp = false;
  for (int t = 0; t < 1; t++) {  //  only for exit the scope
    void *sg_node = sg_expr;
    void *op1 = nullptr;
    void *op2 = nullptr;
    int op = 0;
    if (!ast->IsAPIntOp(sg_node, &op1, &op2, &op) && ast->IsDivideOp(sg_node)) {
      ast->GetExpOperand(sg_node, &op1, &op2);
      op = ast->GetExpressionOp(sg_node);
    }

    if (V_SgDivideOp != op) {
      break;
    }

    int64_t int_value = 0;
    if (!ast->IsConstantInt(op2, &int_value, true, nullptr)) {
      break;
    }
    if (int_value == 0) {
      break;
    }

    int sign = static_cast<int>(int_value >= 0);
    int64_t abs = sign != 0 ? int_value : -int_value;

    CMarsExpression me1(op1, ast, pos);
    CMarsRangeExpr mr1 = me1.get_range();

    CMarsExpression me_lb;
    CMarsExpression me_ub;
    if (mr1.get_simple_bound(&me_lb, &me_ub) == 0) {
      break;
    }

    int const_lb = me_lb.get_const();
    int const_ub = me_ub.get_const();

    CMarsExpression new_lb;
    {
      CMarsExpression tmp = me_lb - const_lb;
      if (divisible(tmp, abs, false) == 0) {
        break;
      }
    }

    CMarsExpression new_ub;
    {
      CMarsExpression tmp = me_ub - const_ub;
      if (divisible(tmp, abs, false) == 0) {
        break;
      }
    }

    supported_divide_exp = true;
  }
  if (supported_divide_exp) {
    return true;
  }

#endif

  CMarsRangeExpr range;
  bool succ = getRangeFromCache(ast, sg_expr, pos, scope, &range, induct_check);
  if (succ && (range.has_bound() != 0)) {
    return true;
  }

  void *cond_exp;
  void *true_exp;
  void *false_exp;
  if (ast->IsConditionalExp(sg_expr, &cond_exp, &true_exp, &false_exp)) {
    CMarsExpression cond_val(cond_exp, ast, pos);
    if (cond_val.IsConstant() != 0) {
      return true;
    }
    if (has_variable_range(ast, true_exp, pos, scope, induct_check) &&
        has_variable_range(ast, false_exp, pos, scope, induct_check)) {
      return true;
    }
  }
  return false;
}
}  //  namespace MarsProgramAnalysis

using MarsProgramAnalysis::cache_consts;
using MarsProgramAnalysis::cache_get_range_exp;
using MarsProgramAnalysis::m_expr_range;
using MarsProgramAnalysis::m_variable_range;
using MarsProgramAnalysis::neg_INF;
using MarsProgramAnalysis::pos_INF;

//  test if pos1 dominates pos2
bool CSageCodeGen::isDom(void *pos1, void *pos2) {
  if ((is_floating_node(pos1) != 0) || (is_floating_node(pos2) != 0)) {
    return false;
  }
  void *func_decl = TraceUpToFuncDecl(pos1);
  void *func_decl2 = TraceUpToFuncDecl(pos2);
  if ((func_decl == nullptr) || func_decl != func_decl2) {
    return false;
  }
  void *func_body = GetFuncBody(func_decl);
  void *curr_pos = pos1;
  void *prev_pos = pos1;
  while (curr_pos != nullptr) {
    if ((IsLoopStatement(curr_pos) != 0) || (IsIfStatement(curr_pos) != 0) ||
        (IsSwitchStatement(curr_pos) != 0)) {
      break;
    }
    prev_pos = curr_pos;
    if (curr_pos == func_body) {
      break;
    }
    curr_pos = GetParent(curr_pos);
  }

  void *scope = GetScope(prev_pos);
  if (nullptr == scope)
    return false;
  bool isInScope = IsInScope(pos2, scope) != 0;

  //  if the unconditional scope of pos1 is function body, it can dominates
  //  function parameter
  if (IsArgumentInitName(pos2) != 0) {
    if (scope == GetFuncBody(func_decl2)) {
      isInScope = true;
    }
  }

  vector<void *> gotoStmt;
  GetNodesByType(scope, "preorder", "SgGotoStatement", &gotoStmt);
  vector<void *> labelStmt;
  GetNodesByType(scope, "preorder", "SgLabelStatement", &labelStmt);
  bool isLabelDead = labelStmt.empty();
  if (!isLabelDead) {
    bool used_label = false;
    for (auto label : labelStmt) {
      if (is_floating_node(label) != 0) {
        used_label = true;
        break;
      }
    }
    if (!used_label) {
      vector<void *> gotoInFunc;
      GetNodesByType(func_decl, "preorder", "SgGotoStatement", &gotoInFunc);
      set<void *> set_label(labelStmt.begin(), labelStmt.end());

      for (auto gotoStmt : gotoInFunc) {
        void *targetLabel =
            (static_cast<SgGotoStatement *>(gotoStmt))->get_label();
        if (set_label.count(targetLabel) > 0 ||
            (is_floating_node(targetLabel) != 0)) {
          used_label = true;
          break;
        }
      }
    }
    if (!used_label) {
      isLabelDead = true;
    }
  }

  vector<void *> returnStmt;
  GetNodesByType(scope, "preorder", "SgReturnStatement", &returnStmt);

  bool isReturnOk;
  if (returnStmt.empty()) {
    isReturnOk = true;
  } else if (returnStmt.size() == 1) {  //  return stmt in last stmt is ok
    bool isInTopLevel = (GetParent(returnStmt[0]) == scope);
    bool isLastStmt =
        ((static_cast<SgNode *>(scope))
             ->get_childIndex(static_cast<SgNode *>(returnStmt[0])) ==
         (static_cast<SgNode *>(scope))->get_numberOfTraversalSuccessors() - 1);
    isReturnOk = isInTopLevel && isLastStmt;
  } else {  //  complex return is as hard as goto, consider as not dominated
    return false;
  }

  return isReturnOk && gotoStmt.empty() && isLabelDead && isInScope;
}

void *CSageCodeGen::get_sizeof_value(void *sizeof_exp) {
  SgSizeOfOp *sg_sizeof_op =
      isSgSizeOfOp(static_cast<SgSizeOfOp *>(sizeof_exp));

  if (sg_sizeof_op == nullptr) {
    return nullptr;
  }
  void *sg_type = sg_sizeof_op->get_operand_type();
  if (sg_type == nullptr) {
    void *sg_exp = sg_sizeof_op->get_operand_expr();
    if (sg_exp != nullptr) {
      if (IsVarRefExp(sg_exp) != 0) {
        void *sg_init = GetVariableInitializedName(sg_exp);
        void *sg_initializer = GetInitializer(sg_init);
        if ((sg_initializer != nullptr) &&
            (IsArrayType(GetTypeByExp(sg_initializer)) != 0)) {
          sg_exp = sg_initializer;
        }
      }
      sg_type = GetTypeByExp(sg_exp);
    }
  }
  if (sg_type == nullptr) {
    return nullptr;
  }
  return get_type_size_exp(sg_type, sizeof_exp, false);
}

void *CSageCodeGen::get_type_size_exp(void *sg_type, void *pos, bool report) {
  if (IsPointerType(sg_type) != 0) {
    int *p;
    return CreateConst(
        sizeof(p));  //  FIXME: the size of pointer is not definitely to be 8
  }

  void *sg_base_type = GetBaseType(sg_type, true);
  void *size_exp = nullptr;
  if (IsArrayType(sg_type) != 0) {
    size_exp = get_type_size_exp(sg_base_type, pos, report);
    if (nullptr == size_exp) {
      return nullptr;
    }
    void *base_type;
    vector<void *> nSizes;
    get_type_dimension_exp(sg_type, &base_type, &nSizes, pos, true);
    bool has_null_exp = false;
    for (auto dim : nSizes) {
      if (IsNullExp(dim) != 0) {
        has_null_exp = true;
        break;
      }
    }
    if (has_null_exp) {
      for (auto dim : nSizes) {
        if (nullptr != dim) {
          DeleteNode(dim);
        }
      }
      return nullptr;
    }
    for (auto dim : nSizes) {
      size_exp = CreateExp(V_SgMultiplyOp, size_exp, dim);
    }

  } else if (IsEnumType(sg_type) != 0) {
    enum dummy {
      DUMMY = 0,
    };
    return CreateConst(sizeof(enum dummy));
  } else if ((IsStructureType(sg_type) != 0) || (IsClassType(sg_type) != 0)) {
    //  youxiang 20161102 handle ap_uint/ap_int
    size_t size = get_sizeof_arbitrary_precision_integer(
        get_bitwidth_from_type(sg_type, report));
    if (0 != size) {
      return CreateConst(size);
    }
    int total_bitwidth = 0;
    int aligned_bitwidth = 0;
    if (IsStructureWithAlignedScalarData(sg_type, &total_bitwidth,
                                         &aligned_bitwidth)) {
      return CreateConst(total_bitwidth);
    }
    //  youxiang 20161116 we cannot handle complext struct type
    //  1. alignment issue
    //  2. pack issue
  } else if (IsUnionType(sg_type) != 0) {
    return nullptr;
  } else if ((IsTypedefType(sg_type) != 0) || (IsModifierType(sg_type) != 0)) {
    size_exp = get_type_size_exp(GetBaseTypeOneLayer(sg_type), pos, report);
  } else if (IsComplexType(sg_type) != 0) {
    size_t size = get_sizeof_arbitrary_precision_integer(
        get_bitwidth_from_type(GetBaseType(sg_type, true), report));
    if (size <= 0)
      return nullptr;
    size_exp = CreateConst(size * 2);
  } else {
    int size = get_sizeof_arbitrary_precision_integer(
        get_bitwidth_from_type(sg_type, false));
    if (size <= 0) {
      return nullptr;
    }
    size_exp = CreateConst(size);
  }
  return size_exp;
}

int CSageCodeGen::get_type_size(void *sg_type, void *pos, bool report) {
  void *size_exp = get_type_size_exp(sg_type, pos, report);
  if (size_exp == nullptr) {
    return 0;
  }
  CMarsExpression me(size_exp, this, pos);
  if (me.IsConstant() != 0) {
    return me.GetConstant();
  }
  return 0;
}

//  if pointer/array, the size of element data type is returned
int CSageCodeGen::get_type_unit_size(void *sg_type, void *pos) {
  assert(isSgType(static_cast<SgNode *>(sg_type)));
  return get_type_size(GetBaseType(sg_type, true), pos);
}

int CSageCodeGen::get_bitwidth_from_type(void *sg_type, bool report) {
  int res = 0;
  if (SgType *ty = isSgType(static_cast<SgNode *>(GetOrigTypeByTypedef(
          GetBaseType(GetOrigTypeByTypedef(sg_type, false), false), false)))) {
    if (auto rv = get_bitwidth(ty, report)) {
      //  Conversion from unsigned to signed.
      if (*rv <= std::numeric_limits<int>::max()) {
        res = *rv;
      }
    }
  }
  return res;
}

int CSageCodeGen::get_bitwidth_from_type_str(const string &str_type,
                                             bool report) {
  int res = 0;
  if (auto *ty = static_cast<SgType *>(GetTypeByString(str_type))) {
    if (auto rv = get_bitwidth(ty, report)) {
      //  Conversion from unsigned to signed.
      if (*rv <= std::numeric_limits<int>::max()) {
        res = *rv;
      }
    }
  }
  return res;
}

bool CSageCodeGen::IsXilinxStreamType(void *type) const {
  //  Xilinx hls::stream<> can only be used as reference type in pure
  //  kernel.
  if (auto *cls_type = isSgClassType(static_cast<SgNode *>(type))) {
    if (auto *cls_decl =
            isSgTemplateInstantiationDecl(cls_type->get_declaration())) {
      if (xil_hls_stream.count(cls_decl->get_templateName().getString()) > 0) {
        return true;
      }
    }
  }
  return false;
}

bool CSageCodeGen::IsStdComplexType(void *type) const {
  //  C++ std::complex<> can only be used as reference type in pure
  //  kernel.
  if (auto *cls_type = isSgClassType(static_cast<SgNode *>(type))) {
    if (auto *cls_decl =
            isSgTemplateInstantiationDecl(cls_type->get_declaration())) {
      if (std_complex.count(cls_decl->get_templateName().getString()) > 0) {
        return true;
      }
    }
  }
  return false;
}

bool CSageCodeGen::IsXilinxAPIntType(void *type) const {
  //  List of Xilinx ap_* classes whose width is the first template arg.
  if (auto *cls_type = isSgClassType(static_cast<SgNode *>(type))) {
    if (auto *cls_decl =
            isSgTemplateInstantiationDecl(cls_type->get_declaration())) {
      if (xil_ap_int.count(cls_decl->get_templateName().getString()) > 0) {
        return true;
      }
    }
  }
  return false;
}

bool CSageCodeGen::IsXilinxAPFixedType(void *type) const {
  //  List of Xilinx ap_* classes whose width is the first template arg.
  if (auto *cls_type = isSgClassType(static_cast<SgNode *>(type))) {
    if (auto *cls_decl =
            isSgTemplateInstantiationDecl(cls_type->get_declaration())) {
      if (xil_ap_fixed.count(cls_decl->get_templateName().getString()) > 0) {
        return true;
      }
    }
  }
  return false;
}

bool CSageCodeGen::IsXilinxIntrinsic(void *fn_decl) const {
  SgFunctionDeclaration *decl =
      isSgFunctionDeclaration(static_cast<SgNode *>(fn_decl));
  if (decl == nullptr)
    return false;
  auto fn_name = GetFuncName(decl, false);
  if (fn_name.find("_ssdm_op_") == 0)
    return true;
  return false;
}

boost::optional<unsigned>
CSageCodeGen::get_xilinx_bitwidth(SgTemplateInstantiationDecl *xil,
                                  bool report) {
  //  List of Xilinx ap_* classes whose width is the first template arg.
  if (xil_ap.count(xil->get_templateName().getString()) != 0U) {
    assert(!xil->get_templateArguments().empty());
    SgExpression *width = xil->get_templateArguments()[0]->get_expression();
    if (width == nullptr) {
      return boost::none;
    }
    MarsProgramAnalysis::CMarsExpression w(width,
                                           const_cast<CSageCodeGen *>(this));
    if (w.IsConstant() == 0) {
#ifdef PROJDEBUG
      if (report) {
        cout << "AST_IF: ERROR: Expected first param of Xilinx ap "
                "class to be integer: "
             << _p(xil) << " param " << _p(width) << "\n";
      }
#endif
      return boost::none;
    }
    return w.GetConstant();
  }
  return boost::none;
}

//  Hacky way to return bit-width of types whose name matches /^u?int[0-9]+$/.
// TODO(Ram): Fix this. It's really bad. We need clearly defined semantics.
static boost::optional<unsigned>
get_hacky_extended_bitwidth(const string &tyname, bool report) {
  //  atoi conversions copied from the previous impl of
  //  get_bitwidth_from_type_str (see commit 89e5ef8c9d4). Note that even this
  //  is kind of broken, because types like "uint-123" would be accepted.
  if (tyname.find("int") == 0U) {
    if (int rv = my_atoi(tyname.substr(sizeof("in")))) {
      return rv;
    }
  } else if (tyname.find("uint") == 0U) {
    if (int rv = my_atoi(tyname.substr(sizeof("uin")))) {
      return rv;
    }
  } else if (tyname.find("merlin_uint_") == 0U) {
    if (int rv = my_atoi(tyname.substr(sizeof("merlin_uint")))) {
      return rv;
    }
  }
  return boost::none;
}

boost::optional<unsigned> CSageCodeGen::get_bitwidth(SgType *ty, bool report) {
  // TODO(ram): This is horribly broken. We rely on syntax (the name of the
  // type)
  //  rather than semantics to define our extended-width integer types.
  if (auto rv = get_hacky_extended_bitwidth(GetTypeNameByType(ty), report)) {
    return *rv;
  }

  SgType *in = ty->stripTypedefsAndModifiers();
  if (in->isIntegerType()) {
    //  From SgType::isIntegerType:
    //  T_CHAR, T_SIGNED_CHAR, T_UNSIGNED_CHAR, T_SHORT, T_SIGNED_SHORT,
    //  T_UNSIGNED_SHORT, T_INT, T_SIGNED_INT, T_UNSIGNED_INT, T_LONG,
    //  T_SIGNED_LONG, T_UNSIGNED_LONG, T_WCHAR, T_LONG_LONG,
    //  T_SIGNED_LONG_LONG, T_UNSIGNED_LONG_LONG, T_SIGNED_128BIT_INTEGER,
    //  T_UNSIGNED_128BIT_INTEGER, T_BOOL
    if ((isSgTypeChar(in) != nullptr) ||
        (isSgTypeUnsignedChar(in) != nullptr) ||
        (isSgTypeSignedChar(in) != nullptr)) {
      return 8 * sizeof(char);
    }
    if ((isSgTypeShort(in) != nullptr) ||
        (isSgTypeSignedShort(in) != nullptr) ||
        (isSgTypeUnsignedShort(in) != nullptr)) {
      return 8 * sizeof(int16_t);
    }
    if ((isSgTypeInt(in) != nullptr) || (isSgTypeSignedInt(in) != nullptr) ||
        (isSgTypeUnsignedInt(in) != nullptr)) {
      return 8 * sizeof(int);
    }
    if ((isSgTypeLong(in) != nullptr) || (isSgTypeSignedLong(in) != nullptr) ||
        (isSgTypeUnsignedLong(in) != nullptr)) {
      return 8 * sizeof(int64_t);
    }
    if ((isSgTypeLong(in) != nullptr) || (isSgTypeSignedLong(in) != nullptr) ||
        (isSgTypeUnsignedLong(in) != nullptr)) {
      return 8 * sizeof(int64_t);
    }
    if (isSgTypeWchar(in) != nullptr) {
      return 8 * sizeof(wchar_t);
    }
    if ((isSgTypeLongLong(in) != nullptr) ||
        (isSgTypeSignedLongLong(in) != nullptr) ||
        (isSgTypeUnsignedLongLong(in) != nullptr)) {
      return 8 * sizeof(int64_t);
    }
    if ((isSgTypeSigned128bitInteger(in) != nullptr) ||
        (isSgTypeUnsigned128bitInteger(in) != nullptr)) {
      return 128;
    }
    if (isSgTypeBool(in) != nullptr) {
      return 8 * sizeof(bool);
    }
#ifdef PROJDEBUG
    if (report) {
      cout << "AST_IF: ERROR: Unexpected integer type: " << _p(in) << "\n";
    }
#endif
  } else if (in->isFloatType()) {
    //  From SgType::isFloatType: T_FLOAT, T_DOUBLE, T_LONG_DOUBLE,
    if (isSgTypeFloat(in) != nullptr) {
      return 8 * sizeof(float);
    }
    if (isSgTypeDouble(in) != nullptr) {
      return 8 * sizeof(double);
    }
    if (isSgTypeLongDouble(in) != nullptr) {
      return 8 * sizeof(long double);
    }
#ifdef PROJDEBUG
    if (report) {
      cout << "AST_IF: ERROR: Unexpected floating type: " << _p(in) << "\n";
    }
#endif
  } else if (auto *clsty = isSgClassType(in)) {
    if (auto *tcinst =
            isSgTemplateInstantiationDecl(clsty->get_declaration())) {
      //  ap_intclass
      if (auto rv = get_xilinx_bitwidth(tcinst, report)) {
        return *rv;
      }
    }
    int total_bitwidth = 0;
    int aligned_bitwidth = 0;
    if (IsStructureWithAlignedScalarData(clsty, &total_bitwidth,
                                         &aligned_bitwidth)) {
      return total_bitwidth;
    }
  } else if (isSgEnumType(in) != nullptr) {
    // TODO(Ram): This is not entirely correct. n1256 states that it's at the
    //  discretion of the implementation what the actual width of an enum is,
    //  provided that it's at least as wide as an int.
    enum dummy { DUMMY = 0 };
    return 8 * sizeof(enum dummy);
  } else if (isSgPointerType(in) != nullptr) {
    return 8 * sizeof(void *);
  } else if (isSgTypeComplex(in) != nullptr) {
    return 8 * 2 *
           get_bitwidth_from_type(isSgTypeComplex(in)->get_base_type(), report);
  }
#ifdef PROJDEBUG
  if (report) {
    cout << "AST_IF: ERROR: Unrecognized interface type: " << _up(ty) << "\n";
  }
#endif
  return boost::none;
}

//  Hui
vector<string> kernel_invar_param;
map<string, string> mapAccess2PortType;

void traverseSgNodeByType(void *_sg_node, void *_pArg) {
  pair<string, vector<void *> *> *arg =
      static_cast<pair<string, vector<void *> *> *>(_pArg);
  string str_type = arg->first;
  vector<void *> *vecNodes = arg->second;
  SgNode *sg_node = static_cast<SgNode *>(_sg_node);

  //     if (isSgFunctionDeclaration(sg_node)){
  //         std::cout << string(isSgFunctionDeclaration(sg_node)->get_name
  //  ())
  //  <<
  //  endl;
  //         sg_node = sg_node;
  //     }
  if (sg_node->class_name() == str_type) {
    vecNodes->push_back(sg_node);
  }
}

void traverseSgNodeByType_compatible(void *_sg_node, void *_pArg) {
  pair<string, vector<void *> *> *arg =
      static_cast<pair<string, vector<void *> *> *>(_pArg);
  string str_type = arg->first;
  vector<void *> &vec_nodes = *(arg->second);
  SgNode *sg_node = static_cast<SgNode *>(_sg_node);

  if (str_type == "SgExpression") {
    if (isSgExpression(sg_node) != nullptr) {
      vec_nodes.push_back(_sg_node);
    }
  } else if (str_type == "SgExprStatement") {
    if (isSgExprStatement(sg_node) != nullptr) {
      vec_nodes.push_back(_sg_node);
    }
  } else if (str_type == "SgStatement") {
    if (isSgStatement(sg_node) != nullptr) {
      vec_nodes.push_back(_sg_node);
    }
  } else if (str_type == "SgInitializer") {
    if (isSgInitializer(sg_node) != nullptr) {
      vec_nodes.push_back(_sg_node);
    }
  } else if (str_type == "SgFunctionCallExp") {
    if ((isSgFunctionCallExp(sg_node) != nullptr) ||
        (isSgConstructorInitializer(sg_node) != nullptr)) {
      vec_nodes.push_back(_sg_node);
    }
  } else if (str_type == "SgFunctionDeclaration") {
    if (isSgFunctionDeclaration(sg_node) != nullptr) {
      vec_nodes.push_back(_sg_node);
    }
  } else if (str_type == "SgInitializedName") {
    if (isSgInitializedName(sg_node) != nullptr) {
      vec_nodes.push_back(_sg_node);
    }
  } else if (str_type == "SgVarRef") {
    if (isSgVarRefExp(sg_node) != nullptr) {
      vec_nodes.push_back(_sg_node);
    }
  } else if (str_type == "SgCompoundAssignOp") {
    if (isSgCompoundAssignOp(sg_node) != nullptr) {
      vec_nodes.push_back(sg_node);
    }
  } else if (str_type == "SgDeclarationStatement") {
    if (isSgDeclarationStatement(sg_node) != nullptr) {
      vec_nodes.push_back(sg_node);
    }
  } else if (str_type == "SgLocatedNode") {
    if (isSgLocatedNode(sg_node) != nullptr) {
      vec_nodes.push_back(sg_node);
    }
  } else if (str_type == "SgPragmaDeclaration") {
    if (isSgPragmaDeclaration(sg_node) != nullptr) {
      vec_nodes.push_back(sg_node);
    }
  } else if (str_type == "SgPntrArrRefExp") {
    if (isSgPntrArrRefExp(sg_node) != nullptr) {
      vec_nodes.push_back(sg_node);
    }
  } else if (str_type == "SgScopeStatement") {
    if (isSgScopeStatement(sg_node) != nullptr) {
      vec_nodes.push_back(sg_node);
    }
  } else if (str_type == "SgVariableDeclaration") {
    if (isSgVariableDeclaration(sg_node) != nullptr) {
      vec_nodes.push_back(sg_node);
    }
  } else if (str_type == "SgBinaryOp") {
    if (isSgBinaryOp(sg_node) != nullptr) {
      vec_nodes.push_back(sg_node);
    }
  } else if (str_type == "SgUnaryOp") {
    if (isSgUnaryOp(sg_node) != nullptr) {
      vec_nodes.push_back(sg_node);
    }
  } else if (str_type == "SgLoopStatement") {
    if ((isSgForStatement(sg_node) != nullptr) ||
        (isSgDoWhileStmt(sg_node) != nullptr) ||
        (isSgWhileStmt(sg_node) != nullptr)) {
      vec_nodes.push_back(sg_node);
    }
  } else {
#ifdef PROJDEBUG
    printf("[GetNodeByType_compatible] Unsupported type: %s\n",
           str_type.c_str());
#endif
    assert(false);  //  add the classes when needed
  }
}

void traverseSgNodeByType_int(void *_sg_node, void *_pArg) {
  pair<int, vector<void *> *> *arg =
      static_cast<pair<int, vector<void *> *> *>(_pArg);
  int v_type = arg->first;
  vector<void *> *vecNodes = arg->second;
  SgNode *sg_node = static_cast<SgNode *>(_sg_node);

  //     if (isSgFunctionDeclaration(sg_node)){
  //         std::cout << string(isSgFunctionDeclaration(sg_node)->get_name
  //  ())
  //  <<
  //  endl;
  //         sg_node = sg_node;
  //     }
  if (sg_node->variantT() == v_type) {
    vecNodes->push_back(sg_node);
  }
}

void traverseSgNodeByType_int_compatible(void *_sg_node, void *_pArg) {
  pair<int, vector<void *> *> *arg =
      static_cast<pair<int, vector<void *> *> *>(_pArg);
  int v_type = arg->first;
  vector<void *> &vec_nodes = *(arg->second);
  SgNode *sg_node = static_cast<SgNode *>(_sg_node);
  switch (v_type) {
  case V_SgExpression:
    if (isSgExpression(sg_node) != nullptr) {
      vec_nodes.push_back(_sg_node);
    }
    break;
  case V_SgExprStatement:
    if (isSgExprStatement(sg_node) != nullptr) {
      vec_nodes.push_back(_sg_node);
    }
    break;
  case V_SgStatement:
    if (isSgStatement(sg_node) != nullptr) {
      vec_nodes.push_back(_sg_node);
    }
    break;
  case V_SgInitializer:
    if (isSgInitializer(sg_node) != nullptr) {
      vec_nodes.push_back(_sg_node);
    }
    break;
  case V_SgFunctionCallExp:
    if ((isSgFunctionCallExp(sg_node) != nullptr) ||
        (isSgConstructorInitializer(sg_node) != nullptr)) {
      vec_nodes.push_back(_sg_node);
    }
    break;
  case V_SgFunctionDeclaration:
    if (isSgFunctionDeclaration(sg_node) != nullptr) {
      vec_nodes.push_back(_sg_node);
    }
    break;
  case V_SgTemplateFunctionDeclaration:
    if (isSgTemplateFunctionDeclaration(sg_node) != nullptr) {
      vec_nodes.push_back(_sg_node);
    }
    break;
  case V_SgTemplateMemberFunctionDeclaration:
    if (isSgTemplateMemberFunctionDeclaration(sg_node) != nullptr) {
      vec_nodes.push_back(_sg_node);
    }
    break;
  case V_SgClassDeclaration:
    if (isSgClassDeclaration(sg_node) != nullptr) {
      vec_nodes.push_back(_sg_node);
    }
    break;
  case V_SgTemplateClassDeclaration:
    if (isSgTemplateClassDeclaration(sg_node) != nullptr) {
      vec_nodes.push_back(_sg_node);
    }
    break;
  case V_SgPragmaDeclaration:
    if (isSgPragmaDeclaration(sg_node) != nullptr) {
      vec_nodes.push_back(_sg_node);
    }
    break;
  case V_SgScopeStatement:
    if (isSgScopeStatement(sg_node) != nullptr) {
      vec_nodes.push_back(_sg_node);
    }
    break;
  case V_SgInitializedName:
    if (isSgInitializedName(sg_node) != nullptr) {
      vec_nodes.push_back(_sg_node);
    }
    break;
  case V_SgVarRefExp:
    if (isSgVarRefExp(sg_node) != nullptr) {
      vec_nodes.push_back(_sg_node);
    }
    break;
  case V_SgCompoundAssignOp:
    if (isSgCompoundAssignOp(sg_node) != nullptr) {
      vec_nodes.push_back(sg_node);
    }
    break;
  case V_SgDeclarationStatement:
    if (isSgDeclarationStatement(sg_node) != nullptr) {
      vec_nodes.push_back(sg_node);
    }
    break;
  case V_SgVariableDeclaration:
    if (isSgVariableDeclaration(sg_node) != nullptr) {
      vec_nodes.push_back(sg_node);
    }
    break;
  case V_SgLocatedNode:
    if (isSgLocatedNode(sg_node) != nullptr) {
      vec_nodes.push_back(sg_node);
    }
    break;
  case V_SgValueExp:
    if (isSgValueExp(sg_node) != nullptr) {
      vec_nodes.push_back(sg_node);
    }
    break;
  default: {
#ifdef PROJDEBUG
    printf("[GetNodeByType_compatible] Unsupported type: %d\n", v_type);
#endif
    assert(false);  //  add the classes when needed
  } break;
  }
}

void traverseSgNodeForReleaseMemory(void *sg_node, void *pArg) {
  delete static_cast<SgNode *>(sg_node);
  //  ((SgNode*)sg_node)->unparseToString();
  //  cout << endl;
  //  (static_cast<vector<void*> *>(pArg))->push_back(sg_node);
}

void CSageCodeGen::GetNodesByType_compatible(void *sg_scope,
                                             const string &str_type,
                                             vector<void *> *vec_nodes) {
  GetNodesByType(sg_scope, "preorder", str_type, vec_nodes, true);
}

void CSageCodeGen::GetNodesByType_int_compatible(void *sg_scope, int v_type,
                                                 vector<void *> *vec_nodes) {
  GetNodesByType_int(sg_scope, "preorder", v_type, vec_nodes, true);
}

int CSageCodeGen::GetNodesByType_recur(void *sg_scope, const string &order,
                                       const string &str_type,
                                       vector<void *> *vecNodes,
                                       bool compatible /*=false*/,
                                       set<void *> *visited_func) {
  assert(order == "preorder" || order == "postorder");
  if (sg_scope == nullptr) {
    return 0;
  }
  vector<void *> sub_func_calls;

  //  visit scope itself
  if ("preorder" == order) {
    GetNodesByType(sg_scope, order, str_type, vecNodes, compatible, false,
                   true);
  }

  GetNodesByType(sg_scope, "preorder", "SgFunctionCallExp", &sub_func_calls,
                 true);
  for (auto one_call : sub_func_calls) {
    void *one_decl = GetFuncDeclByCall(one_call);
    if ((one_decl == nullptr) || (GetFuncBody(one_decl) == nullptr)) {
      continue;
    }
    if (visited_func->find(one_decl) != visited_func->end()) {
      continue;
    }

    //  check recursiveness
    void *rec_func = nullptr;
    if (IsRecursiveFunction(one_decl, &rec_func) != 0) {
      vecNodes->clear();
      cout << "[GetNodesByType_recur] ERROR: unexpected recursive function: "
           << _p(one_decl) << endl;
      return 0;  //  Note: recursive function should be considered carefully
                 //  outside this call
    }

    //  visit sub-functions
    int ret = GetNodesByType_recur(one_decl, order, str_type, vecNodes,
                                   compatible, visited_func);
    if (ret == 0) {
      return 0;
    }
    visited_func->insert(one_decl);
  }

  //  visit scope itself
  if ("postorder" == order) {
    GetNodesByType(sg_scope, order, str_type, vecNodes, compatible, false,
                   true);
  }

  return 1;
}

void CSageCodeGen::GetNodesByType(void *sg_scope, const string &order,
                                  const string &str_type,
                                  vector<void *> *vecNodes,
                                  bool compatible /*=false*/,
                                  bool cross_func /*=false*/,
                                  bool keep_org /*=false*/) {
  assert(order == "preorder" || order == "postorder");
  //  if (str_type == "SgVarRefExp") {
  //  //  ZP: 20150825
  //  //  For the performance purpose, this function is very heavily loaded
  //  //  A cache mechanism is applied
  //  //  NOTE:
  //  //      1. The assumption is made that the function structure is not
  //  changed
  //  if (_USE_CACHE_IN_TRAVERSE_) {

  //    if (s_var_ref_cache.find(sg_scope) != s_var_ref_cache.end()) {
  //      *vecNodes = s_var_ref_cache[sg_scope];
  //      return;
  //    }
  //  }
  //  }
  //
  if (cross_func) {
    set<void *> visited_decl;  //  empty
    if (!keep_org) {
      vecNodes->clear();
    }
    int ret = GetNodesByType_recur(sg_scope, order, str_type, vecNodes,
                                   compatible, &visited_decl);
    if (ret == 0) {  //  recursive function inside
      vecNodes->clear();
    }
    return;

    return;
  }

  if (sg_scope == nullptr) {
    sg_scope = m_sg_project;
  }
  if (!keep_org) {
    vecNodes->clear();
  }
  pair<string, vector<void *> *> arg;
  arg.first = str_type;
  arg.second = vecNodes;

  TraverseSimple(sg_scope, order,
                 compatible ? traverseSgNodeByType_compatible
                            : traverseSgNodeByType,
                 &arg);

  if (str_type == "SgVarRefExp") {
    s_var_ref_cache[sg_scope] = *vecNodes;
  }
}

void CSageCodeGen::GetNodesByType_int(void *sg_scope, const string &order,
                                      int v_type, vector<void *> *vecNodes,
                                      bool compatible) {
  assert(order == "preorder" || order == "postorder");
  //  if (str_type == "SgVarRefExp") {
  //  //  ZP: 20150825
  //  //  For the performance purpose, this function is very heavily loaded
  //  //  A cache mechanism is applied
  //  //  NOTE:
  //  //      1. The assumption is made that the function structure is not
  //  changed
  //  if (_USE_CACHE_IN_TRAVERSE_) {

  //    if (s_var_ref_cache.find(sg_scope) != s_var_ref_cache.end()) {
  //      *vecNodes = s_var_ref_cache[sg_scope];
  //      return;
  //    }
  //  }
  //  }

  if (sg_scope == nullptr) {
    sg_scope = m_sg_project;
  }
  vecNodes->clear();
  pair<int, vector<void *> *> arg;
  arg.first = v_type;
  arg.second = vecNodes;

  TraverseSimple(sg_scope, order,
                 compatible ? traverseSgNodeByType_int_compatible
                            : traverseSgNodeByType_int,
                 &arg);

  if (v_type == V_SgVarRefExp) {
    s_var_ref_cache[sg_scope] = *vecNodes;
  }
}
void *CSageCodeGen::TraceUpByType(void *sg_pos, const string &str_type) {
  SgNode *p_cur = static_cast<SgNode *>(sg_pos);
  while (p_cur != nullptr) {
    if (p_cur->class_name() == str_type) {
      return p_cur;
    }

    p_cur = p_cur->get_parent();
  }
  return nullptr;
}

void *CSageCodeGen::TraceUpByTypeCompatible(void *sg_pos, int V_sg_type) const {
  SgNode *p_cur = static_cast<SgNode *>(sg_pos);
  while (p_cur != nullptr) {
    //  NOTE: be care of the containing relation of the types:
    if (V_SgForStatement == V_sg_type) {
      if (isSgForStatement(p_cur) != nullptr) {
        return p_cur;
      }
    } else if (V_SgScopeStatement == V_sg_type) {
      if (isSgScopeStatement(p_cur) != nullptr) {
        return p_cur;
      }
      //  youxiang 20161103 handle function paramater
      if ((isSgFunctionDeclaration(p_cur) != nullptr) &&
          (isSgFunctionDeclaration(static_cast<SgNode *>(sg_pos)) == nullptr)) {
        void *func_body = GetFuncBody(p_cur);
        if (func_body != nullptr) {
          return func_body;
        }
      }
    } else if (V_SgFunctionDeclaration == V_sg_type) {
      if (isSgFunctionDeclaration(p_cur) != nullptr) {
        return p_cur;
      }
    } else if (V_SgFunctionDefinition == V_sg_type) {
      if (isSgFunctionDefinition(p_cur) != nullptr) {
        return p_cur;
      }
    } else if (V_SgFunctionCallExp == V_sg_type) {
      if ((isSgFunctionCallExp(p_cur) != nullptr) ||
          (isSgConstructorInitializer(p_cur) != nullptr)) {
        return p_cur;
      }
    } else if (V_SgStatement == V_sg_type) {
      if (isSgStatement(p_cur) != nullptr) {
        return p_cur;
      }
    } else if (V_SgBasicBlock == V_sg_type) {
      if (isSgBasicBlock(p_cur) != nullptr) {
        return p_cur;
      }
    } else if (V_SgInitializedName == V_sg_type) {
      if (isSgInitializedName(p_cur) != nullptr) {
        return p_cur;
      }
    } else if (V_SgClassDeclaration == V_sg_type) {
      if (isSgClassDeclaration(p_cur) != nullptr) {
        return p_cur;
      }
    } else if (V_SgExpression == V_sg_type) {
      if (isSgExpression(p_cur) != nullptr) {
        return p_cur;
      }
    } else if (V_SgGlobal == V_sg_type) {
      if (isSgGlobal(p_cur) != nullptr) {
        return p_cur;
      }
    } else if (V_SgAssignOp == V_sg_type) {
      if (isSgAssignOp(p_cur) != nullptr) {
        return p_cur;
      }
    } else if (V_SgIfStmt == V_sg_type) {
      if (isSgIfStmt(p_cur) != nullptr) {
        return p_cur;
      }
    } else {
      assert(0);
    }
    p_cur = p_cur->get_parent();
  }
  return nullptr;
}

void *CSageCodeGen::GetFuncCallInStmt(void *stmt) {
  for (size_t i = 0; i < GetChildStmtNum(stmt); i++) {
    void *child_stmt = GetChildStmt(stmt, i);
    if (IsBasicBlock(child_stmt) != 0) {
      void *func_call = GetFuncCallInStmt(child_stmt);
      return func_call;
    }
    if ((IsExprStatement(child_stmt) != 0) &&
        (IsFunctionCall(GetExprFromStmt(child_stmt)) != 0)) {
      void *func_call = GetExprFromStmt(child_stmt);
      //  printf("return1 func_call = %s\n",
      //  UnparseToString(func_call).c_str());
      return func_call;
    }
    vector<void *> v_all_calls;
    GetNodesByType(stmt, "preorder", "SgFunctionCallExp", &v_all_calls);
    if (v_all_calls.size() == 1) {
      return v_all_calls[0];
    }
  }
  return nullptr;
}
//  Yuxin: 04/27/2018 add argument:
//  exclusive: the loop scope whether if can be sg_pos itself
void *CSageCodeGen::TraceUpToLoopScope(void *sg_pos, bool exclusive) {
  SgNode *p_cur = static_cast<SgNode *>(sg_pos);
  if (exclusive) {
    p_cur = p_cur->get_parent();
  }

  while (p_cur != nullptr) {
    if (isSgForStatement(p_cur) != nullptr) {
      return p_cur;
    }
    if (isSgWhileStmt(p_cur) != nullptr) {
      return p_cur;
    }
    if (isSgDoWhileStmt(p_cur) != nullptr) {
      return p_cur;
    }
    if (isSgFunctionDeclaration(p_cur) != nullptr) {
      return nullptr;
    }
    if (isSgFunctionDefinition(p_cur) != nullptr) {
      return nullptr;
    }

    p_cur = p_cur->get_parent();
  }
  return nullptr;
}

void *CSageCodeGen::TraceUpToAccess(void *sg_pos) {
  void *sg_start;
  void *sg_out = nullptr;

  if ((IsVarRefExp(sg_pos) != 0) && (IsPntrArrRefExp(GetParent(sg_pos)) != 0) &&
      sg_pos == GetExpLeftOperand(GetParent(sg_pos))) {
    //  case 1: pointer/array variable
    sg_start = sg_pos;
  } else {
    //  case 2: variable in the index
    sg_start = TraceUpToIndex(sg_pos);
    if (sg_start != nullptr) {
      sg_start = GetParent(sg_start);  //  get the left part
    }
  }

  if (sg_start == nullptr) {
    sg_out = sg_pos;  //  for scalar variables
  } else {
    void *sg_pre = sg_start;
    void *sg_curr = GetParent(sg_start);
    while (true) {
      int cont = 0;
      if ((IsPntrArrRefExp(sg_curr) != 0) &&
          sg_pre == GetExpLeftOperand(sg_curr)) {
        cont = 1;
      }
      if (IsPointerDerefExp(sg_curr) != 0) {
        cont = 1;
      }
      if (((IsAddOp(sg_curr) != 0) || ((IsSubtractOp(sg_curr) != 0) &&
                                       GetExpLeftOperand(sg_curr) == sg_pre)) &&
          (IsPointerDerefExp(GetParent(sg_curr)) != 0)) {
        cont = 1;
      }
      if (0 == cont) {
        break;
      }

      if (IsStatement(sg_curr) != 0) {
        sg_pre = nullptr;
        break;
      }
      sg_pre = sg_curr;
      sg_curr = GetParent(sg_curr);
    }
    sg_out = sg_pre;
  }

  //    string sout = (sg_out) ? UnparseToString(sg_out) : string("nullptr");
  //    cout << "[TraceUpToAccess] in="<< UnparseToString(sg_pos) <<
  //        " ref=" << sout  << endl;
  return sg_out;
}

void *CSageCodeGen::TraceUpToIndex(void *sg_pos) {
  void *sg_curr = GetParent(sg_pos);
  void *sg_pre = sg_pos;
  while (true) {
    if ((IsPntrArrRefExp(sg_curr) != 0) &&
        GetExpRightOperand(sg_curr) == sg_pre) {
      break;
    }
    if (IsPointerDerefExp(sg_curr) != 0) {
      break;
    }

    if (IsStatement(sg_curr) != 0) {
      sg_curr = sg_pre = nullptr;
      break;
    }
    sg_pre = sg_curr;
    sg_curr = GetParent(sg_curr);
  }

  //    string sout = (sg_pre) ? UnparseToString(sg_pre) : string("nullptr");
  //    cout << "[TraceUpToIndex] in="<< UnparseToString(sg_pos) <<
  //        " idx=" << sout  << endl;
  return sg_pre;
}

void *CSageCodeGen::TraceUpToRootExpression(void *sg_pos) {
  void *sg_curr = GetParent(sg_pos);
  void *sg_pre = sg_pos;
  while (true) {
    if (IsExpression(sg_curr) == 0) {
      break;
    }
    sg_pre = sg_curr;
    sg_curr = GetParent(sg_curr);
  }

  return sg_pre;
}

void *CSageCodeGen::TraceUpToInitializedName(void *sg_exp) {
  void *root_exp = TraceUpToRootExpression(sg_exp);
  if (IsInitializer(root_exp) != 0) {
    return isSgInitializedName(static_cast<SgNode *>(GetParent(root_exp)));
  }
  return nullptr;
}

string get_scope_prefix(void *node) {
  string out = "";

  SgNode *p = static_cast<SgNode *>(node);
  while (p) {
    auto ns_decl = isSgNamespaceDeclarationStatement(p);
    if (ns_decl) {
      out = ns_decl->get_name() + "::" + out;
    } else if (out != "" && !isSgNamespaceDeclarationStatement(p) &&
               !isSgFunctionDefinition(p)) {
      return out;
    }
    p = p->get_parent();
  }
  return out;
}

string CSageCodeGen::UnparseToString(void *sg_node_, int len /*=0*/) const {
  SgNode *sg_node = (static_cast<SgNode *>(sg_node_));

  SgFunctionParameterList *plist = isSgFunctionParameterList(sg_node);
  SgFunctionDeclaration *func_decl = isSgFunctionDeclaration(sg_node);
  SgFunctionCallExp *func_call = isSgFunctionCallExp(sg_node);
  SgFunctionDefinition *func_def = isSgFunctionDefinition(sg_node);
  SgForStatement *for_loop = isSgForStatement(sg_node);
  SgProject *project = isSgProject(sg_node);
  SgVarRefExp *var_ref = isSgVarRefExp(sg_node);
  SgInitializedName *init_ref = isSgInitializedName(sg_node);

  string str;

  if (plist != nullptr) {
    SgInitializedNamePtrList name_list = plist->get_args();
    SgInitializedNamePtrList::iterator it;

    str = "";
    for (it = name_list.begin(); it != name_list.end(); it++) {
      if (it != name_list.begin()) {
        str += ",";
      }
      SgInitializedName *sg_name = *it;
      str += UnparseToString(sg_name->get_type(), 20) + " " +
             GetVariableName(sg_name);
    }
  } else if (func_decl != nullptr) {
    if (auto *tf = isSgTemplateFunctionDeclaration(func_decl)) {
      str = tf->get_string();
    } else if (auto *tmf = isSgTemplateMemberFunctionDeclaration(func_decl)) {
      str = tmf->get_string();
    } else {
      string str_return = UnparseToString(GetFuncReturnType(func_decl));
      str = str_return + " " + get_scope_prefix(func_decl) +
            GetFuncName(func_decl) + "(" +
            UnparseToString(func_decl->get_parameterList()) + ") {...}";
    }
  } else if (func_call != nullptr) {
    str = get_scope_prefix(GetAssociatedFuncDeclByCall(func_call)) +
          func_call->unparseToString();
  } else if (func_def != nullptr) {
    str = "function definition: " + UnparseToString(GetParent(func_def), len);
  } else if (project != nullptr) {
    //  return "_project_";
    str = "_p_";
  } else if (init_ref != nullptr) {
    string scope_prefix = get_scope_prefix(init_ref);
    str = scope_prefix + GetVariableName(init_ref);
  } else if (var_ref != nullptr) {
    void *init_ref = GetVariableInitializedName(var_ref);
    str = GetVariableName(init_ref);
  } else if (for_loop != nullptr) {
    str = sg_node->unparseToString();

    int pos = str.find("{");

    if (pos >= 0) {
      str = str.substr(0, pos);
      str += "{...}";
    }

    //    return str;
  } else if (sg_node != nullptr) {
    if (auto *tc = isSgTemplateClassDeclaration(sg_node)) {
      str = tc->get_string();
    } else {
      // if 'sg_node' is in a header file which is under a sub directory, e.g.
      // include/x/t.h, unparseToCompleteString() will return empty string
      str = sg_node->unparseToString();
    }
  }

  if (len != 0 && str.length() > static_cast<size_t>(len)) {
    str = str.substr(0, len) + "...";
  }

  return str;
}

int CSageCodeGen::IsBinaryAssign(SgBinaryOp *exp) {
  return static_cast<int>(nullptr != isSgCompoundAssignOp(exp));

  if (isSgAndAssignOp(exp) != nullptr) {
    return 1;
  }
  if (isSgDivAssignOp(exp) != nullptr) {
    return 1;
  }
  //  if (isSgExponentiationAssignOp(exp)) return 1;
  //  if (isSgIntegerDivideAssignOp(exp)) return 1;
  if (isSgIorAssignOp(exp) != nullptr) {
    return 1;
  }
  //  if (isSgJavaUnsignedRshiftAssignOp(exp)) return 1;
  if (isSgLshiftAssignOp(exp) != nullptr) {
    return 1;
  }
  if (isSgMinusAssignOp(exp) != nullptr) {
    return 1;
  }
  if (isSgModAssignOp(exp) != nullptr) {
    return 1;
  }
  if (isSgMultAssignOp(exp) != nullptr) {
    return 1;
  }
  if (isSgRshiftAssignOp(exp) != nullptr) {
    return 1;
  }
  if (isSgPlusAssignOp(exp) != nullptr) {
    return 1;
  }
  if (isSgXorAssignOp(exp) != nullptr) {
    return 1;
  }

  return 0;
}

int CSageCodeGen::GetBinaryOperationFromCompoundAssignOp(void *orig_exp) {
  SgExpression *exp = isSgExpression(static_cast<SgNode *>(orig_exp));
  if (exp == nullptr) {
    return 0;
  }
  if (isSgCompoundAssignOp(exp) == nullptr) {
    return 0;
  }
  if (isSgAndAssignOp(exp) != nullptr) {
    return V_SgAndOp;
  }
  if (isSgXorAssignOp(exp) != nullptr) {
    return V_SgBitXorOp;
  }
  if (isSgIorAssignOp(exp) != nullptr) {
    return V_SgOrOp;
  }
  //  if (isSgExponentiationAssignOp(exp)) return 1;
  //  if (isSgIntegerDivideAssignOp(exp)) return 1;
  //  if (isSgIorAssignOp(exp))  return 1;
  //  if (isSgJavaUnsignedRshiftAssignOp(exp)) return 1;
  if (isSgMinusAssignOp(exp) != nullptr) {
    return V_SgSubtractOp;
  }
  if (isSgModAssignOp(exp) != nullptr) {
    return V_SgModOp;
  }
  if (isSgMultAssignOp(exp) != nullptr) {
    return V_SgMultiplyOp;
  }
  if (isSgDivAssignOp(exp) != nullptr) {
    return V_SgDivideOp;
  }
  if (isSgPlusAssignOp(exp) != nullptr) {
    return V_SgAddOp;
  }
  if (isSgRshiftAssignOp(exp) != nullptr) {
    return V_SgRshiftOp;
  }
  if (isSgLshiftAssignOp(exp) != nullptr) {
    return V_SgLshiftOp;
  }

  return 0;
}

//  return true for pntr(a) = b.
bool CSageCodeGen::parse_assign_to_pntr(void *parent, void **var_name,
                                        void **value) {
  SgNode *sg_parent = static_cast<SgNode *>(parent);
  void *sg_left = nullptr;
  void *sg_right = nullptr;
  GetExpOperand(sg_parent, &sg_left, &sg_right);

RESTART:
  if (isSgAssignOp(sg_parent) != nullptr) {
    void *arr_ref;
    if (parse_array_ref_from_pntr(sg_left, &arr_ref) != 0) {
      *var_name = GetVariableInitializedName(arr_ref);
      *value = sg_right;
      return true;
    }
    return false;

  } else if (isSgAssignInitializer(sg_parent) != nullptr) {
    if (isSgAssignOp(static_cast<SgNode *>(GetParent(sg_parent))) != nullptr) {
      //  some times, y in x=y is a SgAssignInitializer
      sg_parent = static_cast<SgNode *>(GetParent(sg_parent));
      goto RESTART;
    }
    *var_name = GetParent(sg_parent);
    assert(IsInitName(*var_name));
    *value = isSgAssignInitializer(sg_parent)->get_operand();
    return true;
  } else {
    //  FIXME: to be extended
    *var_name = nullptr;
    *value = nullptr;
    return false;
  }
}

int CSageCodeGen::parse_assign(void *parent, void **var_name, void **value) {
  SgNode *sg_parent = static_cast<SgNode *>(parent);
  void *sg_left = nullptr;
  void *sg_right = nullptr;
  *var_name = nullptr;
  *value = nullptr;
  GetExpOperand(sg_parent, &sg_left, &sg_right);

RESTART:
  if (isSgAssignOp(sg_parent) != nullptr) {
    if (IsVarRefExp(sg_left) != 0) {
      *var_name = GetVariableInitializedName(sg_left);
      *value = sg_right;
      return 1;
    }
    return 0;
  } else if (isSgAssignInitializer(sg_parent) != nullptr) {
    if (isSgAssignOp(static_cast<SgNode *>(GetParent(sg_parent))) != nullptr) {
      //  some times, y in x=y is a SgAssignInitializer
      sg_parent = static_cast<SgNode *>(GetParent(sg_parent));
      goto RESTART;
    }
    *var_name = GetParent(sg_parent);
    assert(IsInitName(*var_name));
    *value = isSgAssignInitializer(sg_parent)->get_operand();
    return 1;
  } else {
    //  FIXME: to be extended
    *var_name = nullptr;
    *value = nullptr;
    return 0;
  }
}

//  ref = exp; //  ref and exp are class object, and operator = is overided
//  The ast is like this:
//  SgFuncCallExp (ref=exp)
//  |-- SgDotExp  (ref=)
//  |   |-- SgVarRefExp (ref)
//  |   |-- SgMemberFunctionRefExp (=)
//  |-- SgExprListExp (exp)
int CSageCodeGen::IsOverideAssignOp(void *sg_ref) {
  SgDotExp *sg_dot = isSgDotExp(static_cast<SgNode *>(sg_ref));
  if (sg_dot == nullptr) {
    return 0;
  }

  void *sg_right = GetExpRightOperand(sg_ref);

  SgMemberFunctionRefExp *sg_func =
      isSgMemberFunctionRefExp(static_cast<SgNode *>(sg_right));
  if (sg_func == nullptr) {
    return 0;
  }

  if ("=" == UnparseToString(sg_func)) {
    return 1;
  }

  return 0;
}

//  return true if interface is written and not fully access
bool CSageCodeGen::is_interface_alias_write(void *ref,
                                            const set<void *> ports_alias,
                                            bool top) {
  void *arr = nullptr;
  void *pntr = nullptr;
  vector<void *> idx;
  int pointer_dim;
  parse_pntr_ref_by_array_ref(ref, &arr, &pntr, &idx, &pointer_dim);

  if (is_write_conservative(pntr, false) == 0) {
    return false;
  }
  SgNode *parent = static_cast<SgNode *>(GetParent(pntr));
  void *right = GetExpRightOperand(parent);

  void *right_arr;
  vector<void *> right_arr_idx;
  parse_pntr_ref_new(right, &right_arr, &right_arr_idx);
  //  assign a interface with another interface is ok
  //  this part will direct return false
  if (right_arr != nullptr && ports_alias.count(right_arr) > 0) {
    return false;
  }

  void *base_type;
  vector<size_t> sizes;
  void *init = GetVariableInitializedName(ref);
  get_type_dimension(GetTypebyVar(init), &base_type, &sizes, ref);
  int dim_size = sizes.size();
  if (top) {
    return dim_size - pointer_dim >= 1;
  }
  return (dim_size - pointer_dim >= 2) ||
         //  fn(**p) { *p = b; }
         //  this kind of assign will affect the value which p pointer to
         ((dim_size >= 2) && ((dim_size - pointer_dim) >= 1));
}

//  ZP: 20150818
//  only work for variable ref or pntr ref
int CSageCodeGen::is_write_conservative(void *ref, bool recursive) {
  void *new_def = nullptr;
  void *var_type = GetTypeByExp(ref);
  bool is_pointer_or_array = IsPointerType(var_type) != 0 ||
                             IsArrayType(var_type) != 0 ||
                             IsReferenceType(var_type) != 0;
  return is_write_conservative(ref, &new_def, is_pointer_or_array, recursive);
}

int CSageCodeGen::is_write_conservative(void *ref, void **new_def,
                                        bool is_address, bool recursive) {
  SgNode *sg_parent = static_cast<SgNode *>(GetParent(ref));
  void *sg_left = nullptr;
  void *sg_right = nullptr;
  GetExpOperand(sg_parent, &sg_left, &sg_right);
  if (isSgArrayType(sg_parent)) {
    return false;
  }
  if (isSgReturnStmt(sg_parent) != nullptr) {
    if (recursive && is_address) {
      return true;
    }
    void *func = TraceUpToFuncDecl(sg_parent);
    return IsNonConstReferenceType(GetFuncReturnType(func));
  }
  if ((isSgAssignOp(sg_parent) != nullptr) ||
      (IsOverideAssignOp(sg_parent) != 0)) {
    if (ref == sg_left) {
      *new_def = sg_right;
      return 1;
    }
    if (is_address && recursive) {
      return 1;
    }
  }
  if ((isSgCompoundAssignOp(sg_parent) != nullptr) && ref == sg_left) {
    *new_def = sg_parent;
    return 1;
  }
  if ((isSgPlusPlusOp(sg_parent) != nullptr)) {
    *new_def = sg_parent;
    return 1;
  }
  if ((isSgMinusMinusOp(sg_parent) != nullptr)) {
    *new_def = sg_parent;
    return 1;
  }
  if (isSgSizeOfOp(sg_parent) != nullptr) {
    return 0;
  }

  if (SgConditionalExp *cond_exp = isSgConditionalExp(sg_parent)) {
    if (cond_exp->get_conditional_exp() == ref) {
      return 0;
    }
    //  Youxiang 20180612 condition expression can be used as left value
    //  e.g.  (i > 0  ? a : b) = 0;
    return is_write_conservative(sg_parent, new_def, is_address, recursive);
  }
  if (isSgExprStatement(sg_parent) != nullptr) {
    return 0;
  }
  if (isSgAssignInitializer(sg_parent) != nullptr) {
    if (is_address && recursive) {
      return 1;
    }
    return 0;
  }
  if (isSgUnaryOp(sg_parent) != nullptr) {
    if (recursive) {
      if ((isSgAddressOfOp(sg_parent) != nullptr) ||
          (isSgPointerDerefExp(sg_parent) != nullptr) ||
          (isSgCastExp(sg_parent) != nullptr)) {
        return is_write_conservative(
            sg_parent, new_def,
            is_address || isSgAddressOfOp(sg_parent) != nullptr, recursive);
      }
    } else {
      //  youxiang : return converative result for lvalue check
      if (isSgAddressOfOp(sg_parent) != nullptr) {
        *new_def = sg_parent;
        return 1;
      }
    }
    return 0;
  }

  if (isSgBinaryOp(sg_parent) != nullptr) {
    void *lhs;
    void *rhs;
    GetExpOperand(sg_parent, &lhs, &rhs);
    //  youxiang 20170329
    //  index of pntr will be always readonly
    if (isSgPntrArrRefExp(sg_parent) != nullptr) {
      if (rhs == ref)
        return 0;
      if (recursive)
        return is_write_conservative(sg_parent, new_def, false, recursive);
      return 0;
    }
    if ((isSgDotExp(sg_parent) != nullptr) ||
        (isSgArrowExp(sg_parent) != nullptr)) {
      SgNode *sg_parent_parent = sg_parent->get_parent();
      if (auto *fcallExp = isSgFunctionCallExp(sg_parent_parent)) {
        SgFunctionDeclaration *funcDecl =
            fcallExp->getAssociatedFunctionDeclaration();
        if (funcDecl) {
          SgMemberFunctionType *memfunctype =
              isSgMemberFunctionType(funcDecl->get_type());
          if (memfunctype && !memfunctype->isConstFunc()) {
            *new_def = sg_parent_parent;
            return 1;
          }
        }
      }
      //  field access
      if (lhs == ref) {
        //  parent node by itself will be readonly
        if (recursive) {
          return is_write_conservative(sg_parent, new_def, is_address,
                                       recursive);
        }
        return 0;
      }
      //  field node is checked recursively
      return is_write_conservative(sg_parent, new_def, is_address, recursive);
    }
    void *type = GetTypeByExp(ref);
    while ((IsModifierType(type) != 0) || (IsTypedefType(type) != 0)) {
      type = GetBaseTypeOneLayer(type);
    }
    if ((IsPointerType(type) != 0) || (IsArrayType(type) != 0)) {
      if (recursive) {
        return is_write_conservative(sg_parent, new_def, is_address, recursive);
      }
      return 0;
    }
    return 0;
  }

  int param_index = GetFuncCallParamIndex(ref);
  if (-1 != param_index) {
    void *sg_call = TraceUpToFuncCall(sg_parent);
    assert(IsFunctionCall(sg_call));
    if (IsMerlinInternalIntrinsic(GetFuncNameByCall(sg_call))) {
      return 0;
    }
    if (GetFuncNameByCall(sg_call).find("merlin_stream_read") == 0 &&
        param_index == 0) {
      return 0;
    }
    void *sg_decl_with_def = GetFuncDeclByCall(sg_call);
    void *sg_decl = sg_decl_with_def;
    if (sg_decl == nullptr) {
      sg_decl = GetFuncDeclByCall(sg_call, 0);
    }
    if (sg_decl == nullptr) {
      *new_def = sg_call;
      return 1;
    }
    void *sg_param = GetFuncParam(sg_decl, param_index);
    //  FIXME: we need to do more check to get correct result for
    //  function with variable parameter lists
    if (sg_param == nullptr) {
      if (GetFuncNameByCall(sg_call).find("printf") == 0) {
        return 0;
      }
      *new_def = sg_call;
      return 1;
    }
    //  Yuxin: Sep 6 2018
    //  If it is Ellipse type "...", return null as well
    //  e.g., the parameter of printf
    if ((sg_param != nullptr) && (isSgTypeEllipse(static_cast<SgNode *>(
                                      GetTypebyVar(sg_param))) != nullptr)) {
      if (GetFuncNameByCall(sg_call).find("printf") == 0) {
        return 0;
      }
    }

    void *sg_type = GetTypebyVar(sg_param);
    if ((IsScalarType(sg_type) != 0 ||
         IsStructureWithAlignedScalarData(sg_type) != 0) &&
        (IsNonConstReferenceType(sg_type) == 0)) {
      return 0;
    }

    if (recursive) {
      if (sg_decl_with_def == nullptr) {
        if (IsMemCpy(sg_call)) {
          if (param_index == 0) {
            return 1;
          }
          return 0;
        }
        return 1;
      }
      vector<void *> vec_refs;
      get_ref_in_scope(sg_param, sg_decl_with_def, &vec_refs);
      for (auto sub_ref : vec_refs) {
        if (is_write_conservative(sub_ref, new_def, false, recursive) != 0) {
          return 1;
        }
      }
      return 0;
    }
    if ((IsNonConstReferenceType(sg_type) != 0) &&
        (IsArrayType(GetElementType(sg_type)) == 0)) {
      return 1;
    }
    return 0;
  }
  if ((IsDotExp(ref) != 0) || (IsArrowExp(ref) != 0)) {
    void *sg_parent = GetParent(ref);
    if (IsFunctionCall(sg_parent) != 0) {
      int operator_op = GetOperatorOp(GetFuncDeclByCall(sg_parent, 0));
      if (IsReadOnlyOperatorOp(operator_op) != 0) {
        return 0;
      }
    }
  }

  *new_def = sg_parent;
  return 1;
}

int CSageCodeGen::is_lvalue(void *ref) {
  if (is_write_conservative(ref, false) != 0) {
    return 1;
  }
  void *parent = GetParent(ref);
  if (IsAddressOfOp(parent) != 0) {
    return 1;
  }
  if (IsInitializer(parent) != 0) {
    void *var_init = GetParent(parent);
    if (IsInitName(var_init) != 0) {
      if (IsReferenceType(GetTypebyVar(var_init)) != 0) {
        return 1;
      }
    }
  }
  if ((IsCastExp(parent) != 0) &&
      (IsReferenceType(GetTypeByExp(parent)) != 0)) {
    return 1;
  }
  return 0;
}

// Yuxin: Dec/19/2019, created by Yuxin
// Check whether if there is outofbound access under the scope
int CSageCodeGen::check_access_out_of_bound(void *sg_init, void *scope,
                                            const vector<size_t> &depths,
                                            bool *is_exact, string *msg) {
  bool empty_access;
  string r_msg = "";
  string w_msg = "";
  int ret = check_access_write_out_of_bound(sg_init, scope, depths,
                                            &empty_access, is_exact, &w_msg);
  ret |= check_access_read_out_of_bound(sg_init, scope, depths, &empty_access,
                                        is_exact, &r_msg);
  if (r_msg != "")
    *msg += " read " + r_msg;
  if (w_msg != "")
    *msg += " write " + w_msg;
  return ret;
}

int CSageCodeGen::check_access_write_out_of_bound(void *sg_init, void *scope,
                                                  const vector<size_t> &depths,
                                                  bool *empty_access,
                                                  bool *is_exact, string *msg) {
  ArrayRangeAnalysis mar(sg_init, this, scope, scope, false, true);
  if (mar.has_write() != 0) {
    vector<CMarsRangeExpr> vec_mr = mar.GetRangeExprWrite();
    if (vec_mr.size() != depths.size())
      return false;
    for (size_t i = 0; i < vec_mr.size(); ++i) {
      CMarsRangeExpr mr = vec_mr[i];
      CMarsExpression lb;
      CMarsExpression ub;
      int simple_bound = mr.get_simple_bound(&lb, &ub);
      bool OutOfBound = false;
      DEBUG(cout << "write bound: " << _p(sg_init) << " == :" << mr.print_e()
                 << endl);
      if (simple_bound != 0) {
        if ((lb.IsConstant() != 0) && lb.GetConstant() < 0) {
          OutOfBound = true;
          DEBUG(cout << "lb: " << lb.GetConstant() << endl);
        }
        DEBUG(cout << "write bound simple: " << _p(sg_init)
                   << " == :" << mr.print_e() << endl);
        if ((ub.IsConstant() != 0) &&
            (ub.GetConstant() < 0 ||
             (i < depths.size() && 0 != depths[i] &&
              static_cast<size_t>(ub.GetConstant()) >= depths[i]))) {
          OutOfBound = true;
          DEBUG(cout << "ub " << ub.GetConstant() << ", " << depths[i] << endl);
        }
      }
      if (mr.is_empty() == 0) {
        *empty_access = false;
      }
      if (mr.is_exact() == 0) {
        *is_exact = false;
      }
      if (OutOfBound) {
        *msg += "access range=[" + mr.print() + "]";
        return OutOfBound;
      }
    }
  }
  return false;
}

int CSageCodeGen::check_access_read_out_of_bound(void *sg_init, void *scope,
                                                 const vector<size_t> &depths,
                                                 bool *empty_access,
                                                 bool *is_exact, string *msg) {
  ArrayRangeAnalysis mar(sg_init, this, scope, scope, false, true);
  if (mar.has_read() != 0) {
    vector<CMarsRangeExpr> vec_mr = mar.GetRangeExprRead();
    // Probably linearized access, such as memcpy
    if (vec_mr.size() != depths.size())
      return false;
    for (size_t i = 0; i < vec_mr.size(); ++i) {
      CMarsRangeExpr mr = vec_mr[i];
      CMarsExpression lb;
      CMarsExpression ub;
      int simple_bound = mr.get_simple_bound(&lb, &ub);
      bool OutOfBound = false;
      DEBUG(cout << "read bound: " << _p(sg_init) << " == :" << mr.print_e()
                 << endl);
      if (simple_bound != 0) {
        DEBUG(cout << "read bound simple: " << _p(sg_init)
                   << " == :" << mr.print_e() << endl);
        if ((lb.IsConstant() != 0) && lb.GetConstant() < 0) {
          OutOfBound = true;
          cout << "lb: " << lb.GetConstant() << endl;
        }
        if ((ub.IsConstant() != 0) &&
            (ub.GetConstant() < 0 ||
             (i < depths.size() && 0 != depths[i] &&
              static_cast<size_t>(ub.GetConstant()) >= depths[i]))) {
          OutOfBound = true;
        }
      }
      if (mr.is_empty() == 0) {
        *empty_access = false;
      }
      if (mr.is_exact() == 0) {
        *is_exact = false;
      }
      if (OutOfBound) {
        *msg += "access range=[" + mr.print() + "]";
        return OutOfBound;
      }
    }
  }
  return false;
}

bool CSageCodeGen::is_xilinx_channel_read(void *call, void **ref) {
  return is_xilinx_channel_access(call, "read", ref) ||
         is_xilinx_channel_access(call, "operator>>", ref);
}

bool CSageCodeGen::is_xilinx_channel_write(void *call, void **ref) {
  return is_xilinx_channel_access(call, "write", ref) ||
         is_xilinx_channel_access(call, "operator<<", ref);
}

bool CSageCodeGen::is_xilinx_channel_access(void *call, string member_name,
                                            void **ref) {
  auto sg_func_call = isSgFunctionCallExp(static_cast<SgNode *>(call));
  if (sg_func_call == nullptr)
    return false;
  SgExpression *call_expr = sg_func_call->get_function();
  void *decl = nullptr;
  if ((IsArrowExp(call_expr) != 0) || (IsDotExp(call_expr) != 0)) {
    decl = sg_func_call->getAssociatedFunctionDeclaration();
    if (IsMemberFunction(decl)) {
      auto *class_decl = GetTypeDeclByMemberFunction(decl);
      if (class_decl != nullptr &&
          IsXilinxStreamType(
              isSgClassDeclaration(static_cast<SgNode *>(class_decl))
                  ->get_type())) {
        if (GetFuncName(decl) == member_name) {
          if (ref != nullptr) {
            *ref = GetExpLeftOperand(call_expr);
          }
          return true;
        }
      }
    }
  }
  return false;
}

int CSageCodeGen::is_altera_channel_read(void *ref) {
  int param_index = GetFuncCallParamIndex(ref);
  if (-1 == param_index) {
    return 0;
  }
  void *sg_call = TraceUpToFuncCall(ref);
  if (sg_call == nullptr) {
    return 0;
  }
  if (0 == GetFuncNameByCall(sg_call).find("read_channel_altera")) {
    return 1;
  }
  return 0;
}

int CSageCodeGen::is_altera_channel_write(void *ref) {
  int param_index = GetFuncCallParamIndex(ref);
  if (-1 == param_index) {
    return 0;
  }
  void *sg_call = TraceUpToFuncCall(ref);
  if (sg_call == nullptr) {
    return 0;
  }
  if (0 == GetFuncNameByCall(sg_call).find("write_channel_altera") &&
      param_index == 0) {
    return 1;
  }
  return 0;
}

int CSageCodeGen::is_altera_channel_call(void *call) {
  if (IsFunctionCall(call) == 0) {
    return 0;
  }
  string func_name = GetFuncNameByCall(call);
  if (func_name.find("read_channel_altera") == 0 ||
      func_name.find("write_channel_altera") == 0) {
    return 1;
  }
  return 0;
}

int CSageCodeGen::is_merlin_channel_call(void *call) {
  if (IsFunctionCall(call) == 0) {
    return 0;
  }
  string func_name = GetFuncNameByCall(call);
  if (func_name.find("merlin_stream_read") == 0 ||
      func_name.find("merlin_stream_write") == 0) {
    return 1;
  }
  return 0;
}
//
//  ZP: 20150818
//  only work for variable ref or pntr ref
int CSageCodeGen::has_write_conservative(void *ref) {
  return is_write_conservative(ref);
}

//  ZP: 20150818
//  only work for variable ref or pntr ref
int CSageCodeGen::has_read_conservative(void *ref, bool recursive) {
  SgNode *sg_parent = static_cast<SgNode *>(GetParent(ref));
  void *sg_left = nullptr;
  void *sg_right = nullptr;
  GetExpOperand(sg_parent, &sg_left, &sg_right);

  if ((isSgAssignOp(sg_parent) != nullptr) && ref == sg_left) {
    return 0;
  }
  if ((IsOverideAssignOp(sg_parent) != 0) && ref == sg_left) {
    return 0;
  }
  int op = 0;
  if (IsAPIntOp(sg_parent, &sg_left, &sg_right, &op)) {
    if (V_SgAssignOp == op && ref == sg_left) {
      return 0;
    }
  }
  if (IsDotExp(sg_parent) || IsArrowExp(sg_parent) ||
      IsPntrArrRefExp(sg_parent)) {
    if (sg_right == ref && IsPntrArrRefExp(sg_parent))
      return 1;
    if (recursive) {
      return has_read_conservative(sg_parent);
    }
  }
  //  Yuxin: 2018.9.25 recursively check
  if (isSgUnaryOp(sg_parent) != nullptr) {
    if (recursive) {
      if ((isSgAddressOfOp(sg_parent) != nullptr) ||
          (isSgCastExp(sg_parent) != nullptr)) {
        return has_read_conservative(sg_parent, recursive);
      }
    } else {
      return 1;
    }
  }
  int param_index = GetFuncCallParamIndex(ref);

  if (-1 != param_index) {
    void *sg_call = TraceUpToFuncCall(sg_parent);
    assert((sg_call || IsConstructorInitializer(sg_call)));
    if (IsMerlinInternalIntrinsic(GetFuncNameByCall(sg_call))) {
      return 0;
    }
    if (GetFuncNameByCall(sg_call).find("merlin_stream_write") == 0 &&
        param_index == 0) {
      return 0;
    }
    void *sg_decl = GetFuncDeclByCall(sg_call);
    if (sg_decl == nullptr) {
      return 1;
    }
    void *sg_param = GetFuncParam(sg_decl, param_index);
    //  FIXME: we need to do more check to get correct result for
    //  function with variable parameter lists
    if (sg_param == nullptr) {
      return 1;
    }
    vector<void *> vec_refs;
    get_ref_in_scope(sg_param, sg_decl, &vec_refs);
    for (auto sub_ref : vec_refs) {
      if (has_read_conservative(sub_ref) != 0) {
        return 1;
      }
    }
    return 0;
  }
  return 1;
}

void CSageCodeGen::get_ref_map_from_iter_map(
    void *scope, const map<void *, void *> &map_iter2expr,
    map<void *, void *> *map_to_insert) {
  CMarsAST_IF *ast = this;
  for (auto one_pair : map_iter2expr) {
    void *sg_init = one_pair.first;
    void *new_exp = one_pair.second;
    vector<void *> vec_ref;
    ast->GetNodesByType_int(scope, "preorder", V_SgVarRefExp, &vec_ref);
    for (auto ref : vec_ref) {
      if (sg_init == ast->GetVariableInitializedName(ref)) {
        map_to_insert->insert(pair<void *, void *>(ref, ast->CopyExp(new_exp)));
      }
    }
  }
}

int CSageCodeGen::is_write_ref(void *ref) {
  return static_cast<int>(poly_access_pattern::READ !=
                          is_write_analysis(static_cast<SgNode *>(ref)));
}

//  YUXIN add May 8: to check read, write, or read_write
//  return 0 for read, 1 for write, 2 for read_write
int CSageCodeGen::is_write_type(void *ref) {
  return is_write_analysis(static_cast<SgNode *>(ref));
}

//  return 0 for read, 1 for write, 2 for read_write
int CSageCodeGen::is_write_analysis(SgNode *sgnode) {
  assert(isSgExpression(sgnode));

  SgNode *stmt = sgnode;
  SgNode *stmt1 = nullptr;
  while ((stmt->get_parent() != nullptr) &&
         ((isSgExpression(stmt->get_parent())) != nullptr)) {
    stmt1 = stmt;
    stmt = stmt->get_parent();
  }
  assert(stmt);

  int iswrite = ((stmt1) != nullptr) && (0 == stmt->get_childIndex(stmt1))
                    ? poly_access_pattern::WRITE
                    : poly_access_pattern::READ;

  switch (stmt->variantT()) {
  case V_SgAssignOp:
    break;
  case V_SgAndAssignOp:
  case V_SgDivAssignOp:
  case V_SgIorAssignOp:
  case V_SgLshiftAssignOp:
  case V_SgMinusAssignOp:
  case V_SgModAssignOp:
  case V_SgMultAssignOp:
  case V_SgPlusAssignOp:
  case V_SgRshiftAssignOp:
  case V_SgXorAssignOp:
  case V_SgPlusPlusOp:
  case V_SgMinusMinusOp:
    if (iswrite != 0) {
      iswrite = poly_access_pattern::READWRITE;
    }
    break;
  default:
    iswrite = poly_access_pattern::READ;
    break;
  }

  return iswrite;
}

void *CSageCodeGen::GetAssociatedFuncDeclByCall(void *sg_func_call_) const {
  if (isSgFunctionCallExp(static_cast<SgNode *>(sg_func_call_)) == nullptr) {
    return nullptr;
  }
  SgExpression *call_expr =
      isSgFunctionCallExp(static_cast<SgNode *>(sg_func_call_))->get_function();
  void *decl = nullptr;
  if ((IsFunctionRefExp(call_expr) != 0) || (IsArrowExp(call_expr) != 0) ||
      (IsDotExp(call_expr) != 0)) {
    decl = isSgFunctionCallExp(static_cast<SgNode *>(sg_func_call_))
               ->getAssociatedFunctionDeclaration();
  } else if (IsMemberFunctionRefExp(call_expr)) {
    decl = isSgMemberFunctionRefExp(static_cast<SgNode *>(call_expr))
               ->getAssociatedMemberFunctionDeclaration();
  }
  return decl;
}

void *CSageCodeGen::GetFuncDeclByName(const string &sFuncName,
                                      int require_body /* = 1*/) {
  vector<void *> vecFuncs = GetAllFuncDeclByName(sFuncName, require_body);

  if (!vecFuncs.empty()) {
    return vecFuncs[0];
  }
  { return nullptr; }
}

bool CSageCodeGen::IsMatchedFuncAndCall(void *decl, void *call, void *curr_decl,
                                        int ignore_static /* = 0 */) {
  //  we generate FUNCTION_NAME_UNIQUFIED_TAG after kernel_separate.
  //  After kernel_seprate, all the function names are unique so that
  //  we can avoid intensive function matching and speed up running time
  static bool unique_function_name =
      test_file_for_read(FUNCTION_NAME_UNIQUFIED_TAG) ? true : false;
  void *func_decl1 = decl;
  void *func_decl2 =
      curr_decl != nullptr
          ? curr_decl
          : ((call != nullptr) ? GetAssociatedFuncDeclByCall(call) : nullptr);
  if (!func_decl1 || !func_decl2) {
    return false;
  }
  if ((static_cast<SgNode *>(func_decl1))->variantT() !=
      (static_cast<SgNode *>(func_decl2))->variantT()) {
    return false;
  }
  if (unique_function_name) {
    return GetFuncName(decl, true) == GetFuncName(func_decl2, true);
  }

#if _USE_CACHE_IN_TRAVERSE_
  MarsProgramAnalysis::DUMMY dummy(this, decl, call, curr_decl);
  if (MarsProgramAnalysis::cache_IsMatchedFuncAndCall.find(dummy) !=
      MarsProgramAnalysis::cache_IsMatchedFuncAndCall.end()) {
    return MarsProgramAnalysis::cache_IsMatchedFuncAndCall[dummy];
  }
#endif

  if (GetFuncName(decl) != GetFuncName(func_decl2)) {
    return false;
  }

  if (func_decl1 == func_decl2) {
    return true;
  }

  if (!IsStatic(decl) || IsMemberFunction(decl)) {
    bool ret = isSameFunction(func_decl1, func_decl2, ignore_static);
#if _USE_CACHE_IN_TRAVERSE_
    MarsProgramAnalysis::cache_IsMatchedFuncAndCall[dummy] = ret;
#endif
    return ret;
  }

  if (ignore_static == 0) {
    if (call != nullptr) {
      if (GetGlobal(decl) != GetGlobal(call))
        return false;
    } else {
      if (GetGlobal(func_decl1) != GetGlobal(func_decl2))
        return false;
    }
  }

  bool ret = isSameFunction(func_decl1, func_decl2, 1);
#if _USE_CACHE_IN_TRAVERSE_
  MarsProgramAnalysis::cache_IsMatchedFuncAndCall[dummy] = ret;
#endif
  return ret;
}

//  NOTE: if required_body == 0, this function can not gurantee which function
//  declaration is returned, if there is multiple ones in the sg_project
void *CSageCodeGen::GetFuncDeclByCall(void *sg_func_call_,
                                      int require_body /* = 1*/,
                                      int ignore_static /* = 0*/) {
  DEFINE_START_END;
  STEMP(start);
  void *decl = nullptr;
  if (isSgFunctionCallExp(static_cast<SgNode *>(sg_func_call_)) != nullptr) {
    decl = GetAssociatedFuncDeclByCall(sg_func_call_);
  } else if (IsConstructorInitializer(sg_func_call_) != 0) {
    decl = isSgConstructorInitializer(static_cast<SgNode *>(sg_func_call_))
               ->get_declaration();
  } else {
    return nullptr;
  }

  //  youxiang 20161127 function pointer
  if (decl == nullptr) {
    return nullptr;
  }
  if (require_body == 0) {
    return decl;
  }
  return GetDefiningFuncDecl(decl, sg_func_call_, ignore_static);
}

void *CSageCodeGen::GetDefiningFuncDecl(void *func_decl, void *sg_func_call_,
                                        bool ignore_static) {
  //  we generate FUNCTION_NAME_UNIQUFIED_TAG after kernel_separate.
  //  After kernel_seprate, all the function names are unique so that
  //  we can avoid intensive function matching and speed up running time
  static bool unique_function_name =
      test_file_for_read(FUNCTION_NAME_UNIQUFIED_TAG) ? true : false;

  auto sg_func_decl = isSgFunctionDeclaration(static_cast<SgNode *>(func_decl));
  if (sg_func_decl == nullptr) {
    return nullptr;
  }
  if (sg_func_decl->get_definition() != nullptr) {
    return func_decl;
  }

  // for same static functions in multiple source files, ROSE library will
  // report incorrect defining function
  if (unique_function_name) {
    if (auto def_decl =
            isSgFunctionDeclaration(static_cast<SgNode *>(func_decl))
                ->get_definingDeclaration()) {
      if (GetFuncBody(def_decl) != nullptr) {
        return def_decl;
      }
    }
  }

  //  ZP: 20150825
  //  For the performance purpose, this function is very heavily loaded
  //  A cache mechanism is applied
  //  NOTE:
  //      1. The assumption is made that the function structure is not changed
  //      2. The scope information is ignored (FIXME)
  if (_USE_CACHE_IN_TRAVERSE_) {
    if (sg_func_call_ != nullptr) {
      if (s_func_decl_cache.find(sg_func_call_) != s_func_decl_cache.end()) {
        auto res = s_func_decl_cache[sg_func_call_];
        if (res == nullptr)
          return nullptr;
        bool valid = true;
        if (static_cast<SgFunctionDeclaration *>(res)->get_definition() ==
            nullptr) {
          valid = false;
        } else if (GetFuncName(res, true) != GetFuncName(func_decl, true)) {
          valid = false;
        } else if (!unique_function_name &&
                   !IsMatchedFuncAndCall(res, sg_func_call_, func_decl,
                                         ignore_static)) {
          valid = false;
        }
        if (valid) {
          return res;
        }
      }
    } else if (s_func_decl_cache.find(func_decl) != s_func_decl_cache.end()) {
      auto res = s_func_decl_cache[func_decl];
      if (res == nullptr)
        return nullptr;
      bool valid = true;
      if (static_cast<SgFunctionDeclaration *>(res)->get_definition() ==
          nullptr) {
        valid = false;
      } else if (GetFuncName(res, true) != GetFuncName(func_decl, true)) {
        valid = false;
      } else if (!unique_function_name &&
                 !IsMatchedFuncAndCall(res, sg_func_call_, func_decl,
                                       ignore_static)) {
        valid = false;
      }
      if (valid) {
        return res;
      }
    }
  }

  vector<void *> vecDecl;
#if _USE_CACHE_IN_TRAVERSE_
  static bool init_cache = false;
  if (!init_cache) {
    GetNodesByType_int(m_sg_project, "preorder", V_SgFunctionDeclaration,
                       &vecDecl, true);
    for (auto func : vecDecl) {
      if (static_cast<SgFunctionDeclaration *>(func)->get_definition() !=
          nullptr)
        s_g_func_name_cache[GetFuncName(func, true)].insert(func);
    }
    init_cache = true;
  }

#else
  GetNodesByType_int(m_sg_project, "preorder", V_SgFunctionDeclaration,
                     &vecDecl, true);
  for (auto func : vecDecl) {
    if (static_cast<SgFunctionDeclaration *>(func)->get_definition() != nullptr)
      s_g_func_name_cache[GetFuncName(func, true)].insert(func);
  }
#endif

  void *res = nullptr;
  string curr_func_name = GetFuncName(func_decl, true);
  //  //////////////////////  /
  for (auto func : s_g_func_name_cache[curr_func_name]) {
    // match the function first in the same global scope
    if (sg_func_call_ && GetGlobal(func) != GetGlobal(sg_func_call_))
      continue;
    if (unique_function_name ||
        IsMatchedFuncAndCall(func, sg_func_call_, func_decl, ignore_static)) {
      res = func;
      break;
    }
  }
  if (res == nullptr) {
    for (auto func : s_g_func_name_cache[curr_func_name]) {
      if (!sg_func_call_ || GetGlobal(func) == GetGlobal(sg_func_call_))
        continue;
      if (unique_function_name ||
          IsMatchedFuncAndCall(func, sg_func_call_, func_decl, ignore_static)) {
        res = func;
        break;
      }
    }
  }
  s_func_decl_cache[func_decl] = res;
  if (sg_func_call_ != nullptr) {
    s_func_decl_cache[sg_func_call_] = res;
  }
  ACCTM(GetFuncDeclByCall, start, end);
  return res;
}

void *CSageCodeGen::GetFuncDeclByRef(void *func_ref) {
  SgFunctionRefExp *sg_func_ref =
      isSgFunctionRefExp(static_cast<SgNode *>(func_ref));
  if (sg_func_ref != nullptr) {
    return sg_func_ref->getAssociatedFunctionDeclaration();
  }
  SgMemberFunctionRefExp *sg_mem_func_ref =
      isSgMemberFunctionRefExp(static_cast<SgNode *>(func_ref));
  if (sg_mem_func_ref != nullptr) {
    return sg_mem_func_ref->getAssociatedMemberFunctionDeclaration();
  }
  return nullptr;
}

void *CSageCodeGen::GetFuncDeclByDefinition(void *func_def) {
  SgFunctionDefinition *def =
      isSgFunctionDefinition(static_cast<SgNode *>(func_def));
  if (def == nullptr) {
    return nullptr;
  }
  return def->get_declaration();
}

void *CSageCodeGen::GetUniqueFuncDeclByBody(void *func_body) {
  SgBasicBlock *body = isSgBasicBlock(static_cast<SgNode *>(func_body));
  SgNode *func = body->get_parent();
  SgFunctionDefinition *func_def = isSgFunctionDefinition(func);
  assert(func_def);
  return GetUniqueFuncDecl(func_def->get_declaration());
}

void *CSageCodeGen::GetUniqueFuncDecl(void *func_decl) {
  SgFunctionDeclaration *unique_decl =
      isSgFunctionDeclaration(static_cast<SgNode *>(func_decl));
  if (unique_decl == nullptr) {
    return nullptr;
  }
  unique_decl =
      isSgFunctionDeclaration(unique_decl->get_firstNondefiningDeclaration());
  return unique_decl;
}

//  get all the function decls with body
vector<void *> CSageCodeGen::GetAllFuncDecl() {
  vector<void *> vecDeclOut;

  vector<void *> vecDecl;
  GetNodesByType_int(m_sg_project, "preorder", V_SgFunctionDeclaration,
                     &vecDecl);

  for (size_t j = 0; j < vecDecl.size(); j++) {
    if (GetFuncBody(vecDecl[j]) == nullptr) {
      continue;
    }
    vecDeclOut.push_back(vecDecl[j]);
  }
  return vecDeclOut;
}

bool CSageCodeGen::is_in_same_file(void *sg1, void *sg2) {
  if ((is_floating_node(sg1) != 0) || (is_floating_node(sg2) != 0)) {
    return false;
  }
  if (IsCompilerGenerated(sg1) || IsCompilerGenerated(sg2) ||
      IsTransformation(sg1) || IsTransformation(sg2)) {
    //  youxiang 20161215 cannot handle transformed AST
    return GetGlobal(sg1) == GetGlobal(sg2);
  }
  string file1 = GetFileInfo_filename(sg1, 1);
  string file2 = GetFileInfo_filename(sg2, 1);
  return file1 == file2;
}

bool CSageCodeGen::is_in_same_function(void *sg1, void *sg2) {
  if ((is_floating_node(sg1) != 0) || (is_floating_node(sg2) != 0)) {
    return false;
  }
  return TraceUpToFuncDecl(sg1) == TraceUpToFuncDecl(sg2);
}

bool CSageCodeGen::isSameFunction(void *func_decl1, void *func_decl2,
                                  int ignore_static) {
  //  we generate FUNCTION_NAME_UNIQUFIED_TAG after kernel_separate.
  //  After kernel_seprate, all the function names are unique so that
  //  we can avoid intensive function matching and speed up running time
  static bool unique_function_name =
      test_file_for_read(FUNCTION_NAME_UNIQUFIED_TAG) ? true : false;
  if (func_decl1 == func_decl2) {
    return true;
  }
  if ((func_decl1 == nullptr) || (func_decl2 == nullptr)) {
    return false;
  }
  //  2016-08-15: youxiang
  if ((static_cast<SgNode *>(func_decl1))->variantT() !=
      (static_cast<SgNode *>(func_decl2))->variantT()) {
    return false;
  }
  if (unique_function_name) {
    return GetFuncName(func_decl1, true) == GetFuncName(func_decl2, true);
  }
  SgFunctionDeclaration *sg_decl1 =
      isSgFunctionDeclaration(static_cast<SgNode *>(func_decl1));
  SgFunctionDeclaration *sg_decl2 =
      isSgFunctionDeclaration(static_cast<SgNode *>(func_decl2));
  if ((sg_decl1 == nullptr) || (sg_decl2 == nullptr)) {
    return false;
  }

  //  ZP: 20160922
  if ((ignore_static == 0) && !IsMemberFunction(func_decl1)) {
    //  Youxiang: 20161102
    //  if (IsStatic(func_decl1) || IsStatic(func_decl2)) return false;
    if ((IsStatic(func_decl1) || IsStatic(func_decl2)) &&
        !is_in_same_file(func_decl1, func_decl2)) {
      return false;
    }
  }
  if (GetFuncName(func_decl1) != GetFuncName(func_decl2)) {
    return false;
  }
  if (SageInterface::isSameFunction(sg_decl1, sg_decl2)) {
    return true;
  }
  if (GetFuncName(func_decl1, true) != GetFuncName(func_decl2, true)) {
    return false;
  }
  if (!isCppLinkage(func_decl1) && !isCppLinkage(func_decl2)) {
    return true;
  }
  unsigned arg_size1 = GetFuncParamNum(sg_decl1);
  unsigned arg_size2 = GetFuncParamNum(sg_decl2);
  if (arg_size1 != arg_size2) {
    return false;
  }
  for (size_t i = 0; i != arg_size1; ++i) {
    void *sg_type1 = GetTypebyVar(GetFuncParam(func_decl1, i));
    void *sg_type2 = GetTypebyVar(GetFuncParam(func_decl2, i));
    void *sg_new_type1 = nullptr;
    if (RemoveTypedef(sg_type1, &sg_new_type1, nullptr, true, "") != 0) {
      sg_type1 = sg_new_type1;
    }
    void *sg_new_type2 = nullptr;
    if (RemoveTypedef(sg_type2, &sg_new_type2, nullptr, true, "") != 0) {
      sg_type2 = sg_new_type2;
    }
    if (SageInterface::checkTypesAreEqual(static_cast<SgType *>(sg_type1),
                                          static_cast<SgType *>(sg_type2))) {
      continue;
    }
    if (IsArrayType(sg_type1) != 0) {
      sg_type1 = CreatePointerType(GetBaseTypeOneLayer(sg_type1));
    }
    if (IsArrayType(sg_type2) != 0) {
      sg_type2 = CreatePointerType(GetBaseTypeOneLayer(sg_type2));
    }
    if (SageInterface::checkTypesAreEqual(static_cast<SgType *>(sg_type1),
                                          static_cast<SgType *>(sg_type2))) {
      continue;
    }
    if (GetStringByType(sg_type1) == GetStringByType(sg_type2)) {
      continue;
    }
    return false;
  }
  return true;
}

bool CSageCodeGen::UnSupportedMemberFunction(void *func_decl) {
  SgMemberFunctionDeclaration *mem_func =
      isSgMemberFunctionDeclaration(static_cast<SgNode *>(func_decl));
  if (mem_func == nullptr) {
    return false;
  }
  SgClassDeclaration *class_decl = dynamic_cast<SgClassDeclaration *>(
      mem_func->get_associatedClassDeclaration());
  if (class_decl == nullptr) {
    return false;
  }
  if (IsAssignOperator(func_decl) || IsConstructor(func_decl)) {
    return false;
  }

  string class_name = class_decl->get_qualified_name();
  static const string supported_class_prefix[] = {
      "::hls::ap_int", "::hls::ap_uint",  "::hls::ap_fixed", "::hls::ap_ufixed",
      "::ap_int_base", "::ap_fixed_base", "::ap_int",        "::ap_uint",
      "::ap_fixed",    "::ap_ufixed",     "hls::stream",     "::hls::stream",
      "::ap_bit_ref",  "::af_bit_ref",    "hls::ap_bit_ref", "hls::af_bit_ref"};
  for (auto class_prefix : supported_class_prefix) {
    if (class_name.find(class_prefix) == 0) {
      return false;
    }
  }
  return true;
}

int CSageCodeGen::IsSupportedMemberFunctionCall(void *func_call) {
  void *decl = GetFuncDeclByCall(func_call, 0);
  if (nullptr == decl) {
    return 0;
  }
  SgMemberFunctionDeclaration *mem_func =
      isSgMemberFunctionDeclaration(static_cast<SgNode *>(decl));
  if (mem_func == nullptr) {
    return 0;
  }
  SgClassDeclaration *class_decl =
      isSgClassDeclaration(mem_func->get_associatedClassDeclaration());
  if (class_decl == nullptr) {
    return 0;
  }
  string class_name = class_decl->get_qualified_name();
  static const string supported_class_prefix[] = {
      "::hls::ap_int", "::hls::ap_uint",  "::hls::ap_fixed", "::hls::ap_ufixed",
      "::ap_int_base", "::ap_fixed_base", "::ap_int",        "::ap_uint",
      "::ap_fixed",    "::ap_ufixed",     "hls::stream",     "::hls::stream"};
  for (auto class_prefix : supported_class_prefix) {
    if (class_name.find(class_prefix) == 0) {
      return 1;
    }
  }
  return 0;
}

int CSageCodeGen::IsSupportedMerlinSystemFunctionCall(void *func_call) {
  if (IsSupportedMemberFunctionCall(func_call)) {
    return 1;
  }
  void *decl = GetFuncDeclByCall(func_call, 0);
  if (nullptr == decl) {
    return 0;
  }
  if (auto *tfc = GetTemplateFuncDecl(decl)) {
    decl = tfc;
  }
  if (auto *tmfc = GetTemplateMemFuncDecl(decl)) {
    decl = tmfc;
  }
  return IsFromMerlinSystemFile(decl);
}

int CSageCodeGen::IsSupportedFunction(void *func_decl) {
  if (IsFunctionDeclaration(func_decl) == 0) {
    return 0;
  }
  //  bug2232 we cannot handle variable reference if there is asm statement
  vector<void *> vec_asm;
  GetNodesByType_int(func_decl, "preorder", V_SgAsmStmt, &vec_asm);
  if (!vec_asm.empty()) {
    return 0;
  }
  // TODO(youxiang): add more checker later
  return 1;
}

bool CSageCodeGen::IsUnionMemberVariable(void *var_init) {
  if (IsMemberVariable(var_init) == 0)
    return false;
  void *sg_def = GetScope(var_init);  // SgClassDefinition
  void *sg_decl = GetTypeDeclByDefinition(sg_def);
  if (sg_decl == nullptr)
    return false;
  SgClassDeclaration *curr_decl =  // SgClassDeclaration
      isSgClassDeclaration(static_cast<SgNode *>(sg_decl));
  if (curr_decl == nullptr)
    return false;
  SgType *curr_type = curr_decl->get_type();

  return IsUnionType(curr_type);
}

bool CSageCodeGen::IsMemberVariable(void *var_init) {
  SgInitializedName *mem_var =
      isSgInitializedName(static_cast<SgNode *>(var_init));
  if (mem_var == nullptr) {
    return false;
  }
  SgClassDefinition *class_decl = isSgClassDefinition(mem_var->get_scope());
  return class_decl != nullptr;
}

bool CSageCodeGen::IsMemberFunction(void *func_decl) {
  return isSgMemberFunctionDeclaration(static_cast<SgNode *>(func_decl)) !=
         nullptr;
}

bool CSageCodeGen::IsConstMemberFunction(void *func_decl) {
  auto mem_func_decl =
      isSgMemberFunctionDeclaration(static_cast<SgNode *>(func_decl));
  if (mem_func_decl == nullptr) {
    return false;
  }
  auto memfunc_type = isSgMemberFunctionType(mem_func_decl->get_type());
  return (memfunc_type != nullptr) && memfunc_type->isConstFunc();
}

bool CSageCodeGen::IsAssignOperator(void *func_decl) {
  return IsMemberFunction(func_decl) &&
         (V_SgAssignOp == GetOperatorOp(func_decl));
}

bool CSageCodeGen::IsConstructor(void *func_decl) {
  if (auto mem_func =
          isSgMemberFunctionDeclaration(static_cast<SgNode *>(func_decl))) {
    SgClassDeclaration *cls =
        isSgClassDeclaration(mem_func->get_associatedClassDeclaration());
    string class_name;
    if (cls != nullptr) {
      class_name = cls->get_name();
      if (auto tinstcls = isSgTemplateInstantiationDecl(cls)) {
        class_name = tinstcls->get_templateName();
      }
    }
    return mem_func->get_name() == class_name;
  }
  return false;
}

bool CSageCodeGen::IsDefaultConstructor(void *func_decl) {
  return IsConstructor(func_decl) && (GetFuncParamNum(func_decl) == 0);
}

//  get all the function decls pointed to the same function
vector<void *> CSageCodeGen::GetAllFuncDecl(void *func_decl) {
  vector<void *> vecDeclOut;

  vector<void *> vecDecl;
  GetNodesByType_int_compatible(m_sg_project, V_SgFunctionDeclaration,
                                &vecDecl);

  for (size_t j = 0; j < vecDecl.size(); j++) {
    if (GetFuncName(func_decl) != GetFuncName(vecDecl[j])) {
      continue;
    }
    if (isSameFunction(func_decl, vecDecl[j])) {
      vecDeclOut.push_back(vecDecl[j]);
    }
  }
  return vecDeclOut;
}

vector<void *> CSageCodeGen::GetAllFuncDeclByName(const string &sFuncName,
                                                  int require_body /* = 0*/) {
  vector<void *> vecDeclOut;
  size_t j;
  vector<void *> vecDecl;
  GetNodesByType_int(m_sg_project, "preorder", V_SgFunctionDeclaration,
                     &vecDecl);
  for (j = 0; j < vecDecl.size(); j++) {
    if ((GetFuncBody(vecDecl[j]) == nullptr) && (require_body != 0)) {
      continue;
    }
    //    cout << "-- " << GetFuncName(vecDecl[j]) << endl;
    if (GetFuncName(vecDecl[j]) == sFuncName) {
      vecDeclOut.push_back(vecDecl[j]);
    }
  }
  return vecDeclOut;
}

string CSageCodeGen::GetFuncNameByCall(void *sg_func_call_, bool qualified) {
  void *decl = GetFuncDeclByCall(sg_func_call_, 0);
  if (decl == nullptr) {
    return "";
  }
  return GetFuncName(decl, qualified);
}

string CSageCodeGen::GetFuncNameByRef(void *sg_func_ref_, bool qualified) {
  SgFunctionRefExp *func_ref =
      isSgFunctionRefExp(static_cast<SgNode *>(sg_func_ref_));
  if (func_ref != nullptr) {
    SgFunctionDeclaration *decl = func_ref->getAssociatedFunctionDeclaration();
    if (decl != nullptr) {
      return GetFuncName(decl, qualified);
    }
  }
  SgMemberFunctionRefExp *func_mem_ref =
      isSgMemberFunctionRefExp(static_cast<SgNode *>(sg_func_ref_));
  if (func_mem_ref != nullptr) {
    auto decl = func_mem_ref->getAssociatedMemberFunctionDeclaration();
    if (decl != nullptr) {
      return GetFuncName(decl, qualified);
    }
  }
  return "";
}

void CSageCodeGen::insert_argument_to_call(void *call, void *exp) {
  SgExprListExp *args = nullptr;
  if (auto *sg_call = isSgFunctionCallExp(static_cast<SgNode *>(call))) {
    args = sg_call->get_args();
  } else if (auto *sg_ctor =
                 isSgConstructorInitializer(static_cast<SgNode *>(call))) {
    args = sg_ctor->get_args();
  } else {
    assert(0 && "unexpected here");
    return;
  }
  args->append_expression(isSgExpression(static_cast<SgNode *>(exp)));
}

//  Note: ZP: 20160921:
//  this function is not stable, the initializad name can not be unparesed
//  Not suggested if there is additional ast operations after the transform
void CSageCodeGen::insert_param_to_decl(void *decl, void *name,
                                        bool to_all_decl) {
  //  Append the argument in the declaration with function body
  SgFunctionDeclaration *sg_decl =
      isSgFunctionDeclaration(static_cast<SgNode *>(decl));
  assert(sg_decl);
  if (GetFuncParamNum(decl) == 0) {
    sg_decl->set_oldStyleDefinition(false);
  }
  vector<void *> v_decls = GetAllFuncDecl(decl);
  sg_decl->append_arg(isSgInitializedName(static_cast<SgNode *>(name)));
  SgFunctionDeclaration *first_non_defined_decl =
      static_cast<SgFunctionDeclaration *>(
          sg_decl->get_firstNondefiningDeclaration());
  if (first_non_defined_decl != nullptr && first_non_defined_decl != sg_decl &&
      is_floating_node(first_non_defined_decl)) {
    first_non_defined_decl->append_arg(static_cast<SgInitializedName *>(
        CreateVariable(GetTypebyVar(name), "")));
  }
  //  Note: Yuxin 20170614
  //  Append the argument in the declaration without function body
  if (to_all_decl) {
    for (auto nd_decl : v_decls) {
      if (nd_decl == decl)
        continue;
      if (GetFuncBody(nd_decl) != nullptr) {
        continue;
      }
      if (isWithInHeaderFile(nd_decl)) {
        continue;
      }
      void *new_nd_decl = CloneFuncDecl(decl, GetGlobal(nd_decl), false);
      ReplaceStmt(nd_decl, new_nd_decl);
    }
  }
  SgFunctionDefinition *sg_def = sg_decl->get_definition();
  if (sg_def != nullptr) {
    SgInitializedName *new_sg_var =
        isSgInitializedName(static_cast<SgNode *>(name));
    new_sg_var->set_scope(sg_def);
    if (new_sg_var->search_for_symbol_from_symbol_table() == nullptr) {
      sg_def->insert_symbol(new_sg_var->get_name(),
                            new SgVariableSymbol(new_sg_var));
    }
  }
}

//  replace the variable reference in a scope (not fully tested !!!)
void CSageCodeGen::replace_variable_references_in_scope(void *var_old,
                                                        void *var_new,
                                                        void *scope) {
  void *sg_init_old = var_old;
  if (IsVariableDecl(var_old))
    sg_init_old = GetVariableInitializedName(var_old);

  vector<void *> vec_ref;
  get_ref_in_scope(sg_init_old, scope, &vec_ref);
  for (auto ref : vec_ref) {
    void *new_ref = CreateVariableRef(var_new, nullptr, ref);
    ReplaceExp(ref, new_ref);
  }
}

//  make vec[idx] += exp
//  simplify_idx=true will use CMarsExpression
void CSageCodeGen::add_exp_to_vector(vector<void *> *vec, int idx, void *exp,
                                     void *pos, bool simplify_idx /*=true*/) {
  assert(idx >= 0);

  DEFINE_START_END;

  STEMP(start);
  int curr_size = vec->size();
  for (int i = curr_size; i <= idx; i++) {
    vec->push_back(CreateConst(0));
  }
  TIMEP("        CreateConst", start, end);

  if (exp == nullptr) {
    return;  //  nothing to do, add by zero except resizing
  }

  void *exist = (*vec)[idx];
  if (exist == nullptr) {
    (*vec)[idx] = exp;
  } else {
    if (simplify_idx) {
      STEMP(start);
      assert(!is_floating_node(pos) || !is_floating_node(exist));
      assert(!is_floating_node(pos) || !is_floating_node(exp));
      CMarsExpression me1(exist, this, pos);
      CMarsExpression me2(exp, this, pos);
      TIMEP("        CMarsExpression", start, end);
      if ((me2 == 0) != 0) {
        return;  //  nothing to be added
      }
      if ((me1 == 0) != 0) {
        (*vec)[idx] = exp;
        return;
      }
    } else {
      int64_t value;
      bool simplify_cast = false;
      if (IsConstantInt(exp, &value, simplify_cast, nullptr)) {
        if (0 == value) {
          return;
        }
      }
      if (IsConstantInt((*vec)[idx], &value, simplify_cast, nullptr)) {
        if (0 == value) {
          (*vec)[idx] = exp;
          return;
        }
      }
    }

    STEMP(start);
    void *sg_add = nullptr;
    if (is_floating_node(exist) != 0) {
      sg_add = CreateExp(V_SgAddOp, exist, CopyExp(exp));
    } else {
      sg_add = CreateExp(V_SgAddOp, CopyExp(exist), CopyExp(exp));
    }
    TIMEP("        CreateExp", start, end);
    (*vec)[idx] = sg_add;

    //        CMarsExpression me1(exist, this, pos);
    //        CMarsExpression me2(exp  , this, pos);
    //        if (me2==0) return;
    //
    //        CMarsExpression me3 = me1+me2;
    //        (*vec)[idx] = me3.get_pointer();
  }
}

int CSageCodeGen::get_pntr_init_dim(void *pntr) {
  vector<void *> vec;
  void *pntr_init;
  parse_pntr_ref_new(pntr, &pntr_init, &vec);
  if (IsInitName(pntr_init) != 0) {
    return get_dim_num_from_var(pntr_init);
  }
  { return -1; }
}

int CSageCodeGen::get_pntr_pntr_dim(void *pntr) {
  void *sg_array = nullptr;
  void *var_ref;
  void *sg_pntr;  //  the pntr which should be equal to sg_ref
  vector<void *> sg_idx;
  int pointer_dim;
  if (parse_array_ref_from_pntr(pntr, &var_ref) == 0) {
    sg_array = nullptr;
    return -1;
  }

  parse_pntr_ref_by_array_ref(var_ref, &sg_array, &sg_pntr, &sg_idx,
                              &pointer_dim);

  return pointer_dim;
}

void *CSageCodeGen::get_pntr_from_var_ref(void *var_ref,
                                          void *pos /*= nullptr*/) {
  void *sg_array;
  void *sg_pntr;
  vector<void *> sg_indexes;
  int pointer_dim;

  parse_pntr_ref_by_array_ref(var_ref, &sg_array, &sg_pntr, &sg_indexes,
                              &pointer_dim, pos);

  return sg_pntr;
}

void *CSageCodeGen::get_array_from_pntr(void *pntr) {
  vector<void *> vec;
  void *pntr_init = nullptr;
  parse_pntr_ref_new(pntr, &pntr_init, &vec);
  return pntr_init;
}

//  sg_ref: A[e1][e2]  =>  sg_array=A  sg_indeces=e1,e2
//  [BE CAUTIOUS] the right offset beyond the pointer_dim are missing actually,
//               if those dimension is needed, call parse_pntr_ref_by_array_ref
//               instead !!!!
void CSageCodeGen::parse_pntr_ref_new(void *sg_ref, void **sg_array,
                                      vector<void *> *sg_indexes) {
  int i;
  void *var_ref;
  void *sg_pntr;  //  the pntr which should be equal to sg_ref
  vector<void *> sg_idx;
  int pointer_dim;
  void *sg_pos = (is_floating_node(sg_ref) == 0) ? sg_ref : GetProject();
  if (parse_array_ref_from_pntr(sg_ref, &var_ref, sg_pos) == 0) {
    *sg_array = nullptr;
    return;
  }

  parse_pntr_ref_by_array_ref(var_ref, sg_array, &sg_pntr, &sg_idx,
                              &pointer_dim, sg_pos);

  for (i = 0; i < pointer_dim; i++) {
    sg_indexes->push_back(sg_idx[pointer_dim - 1 - i]);
  }
}

void CSageCodeGen::remove_cast(void **sg_node) {
  while (IsCastExp(*sg_node) != 0) {
    *sg_node = GetExpUniOperand(*sg_node);
  }
}

int CSageCodeGen::parse_array_ref_from_pntr(void *pntr, void **array_ref,
                                            void *pos,
                                            bool simplify_idx,     /*true*/
                                            int adjusted_array_dim /*=-1*/
) {
  if (pos == nullptr) {
    pos = pntr;
  }

  if (IsVarRefExp(pntr) != 0) {
    *array_ref = pntr;
    return 1;
  }

  vector<void *> vec_refs;
  vec_refs.clear();
  GetNodesByType_int(pntr, "preorder", V_SgVarRefExp, &vec_refs);
  for (size_t i = 0; i < vec_refs.size(); i++) {
    void *ref = vec_refs[i];
    void *sg_array = nullptr;
    void *sg_pntr;
    vector<void *> vec_idx;
    int pointer_dim;

    int array_dim_to_try = -1;
    if (adjusted_array_dim != -1) {
      void *t_array = GetVariableInitializedName(ref);
      if (t_array == nullptr) {
        continue;
      }
      array_dim_to_try = get_dim_num_from_var(t_array);
      if (adjusted_array_dim > array_dim_to_try) {
        array_dim_to_try = adjusted_array_dim;
      }
    }
    parse_pntr_ref_by_array_ref(ref, &sg_array, &sg_pntr, &vec_idx,
                                &pointer_dim, pos, simplify_idx,
                                array_dim_to_try);

    remove_cast(&sg_pntr);
    void *clear_pntr = pntr;
    remove_cast(&clear_pntr);

    if (sg_pntr == clear_pntr && GetVariableInitializedName(ref) == sg_array) {
      //  if (GetVariableInitializedName(ref) == sg_array) {
      *array_ref = ref;
      return 1;
    }
  }

  *array_ref = nullptr;
  return 0;
}

//  if the format is not support for the pntr reference, return 0
int CSageCodeGen::parse_memcpy(void *memcpy_call, void **length_exp,
                               void **sg_array1, void **sg_pntr1,
                               vector<void *> *sg_index1, int *pointer_dim1,
                               void **sg_array2, void **sg_pntr2,
                               vector<void *> *sg_index2, int *pointer_dim2,
                               void *pos) {
  if (pos == nullptr) {
    pos = memcpy_call;
  }

  if (IsFunctionCall(memcpy_call) == 0) {
    return 0;
  }
  //  if ("memcpy" != GetFuncNameByCall(memcpy_call)) return 0;
  if (!IsMemCpy(memcpy_call)) {
    return 0;
  }
  if (3 != GetFuncCallParamNum(memcpy_call)) {
    return 0;
  }

  void *arg1 = GetFuncCallParam(memcpy_call, 0);
  void *arg2 = GetFuncCallParam(memcpy_call, 1);

  remove_cast(&arg1);
  remove_cast(&arg2);
  int ret = 1;
  void *ref1 = nullptr;
  if (parse_array_ref_from_pntr(arg1, &ref1, pos) == 0) {
    printf("failed parse_memcpy\n");
    ret = 0;
  }
  *sg_array1 = *sg_pntr1 = nullptr;
  if (ref1 != nullptr) {
    parse_pntr_ref_by_array_ref(ref1, sg_array1, sg_pntr1, sg_index1,
                                pointer_dim1);
    if (*sg_array1 != nullptr) {
      void *type = GetTypebyVar(*sg_array1);
      if ((IsArrayType(type) != 0) && *pointer_dim1 == -1) {
        //  array address is equal to its value, e.g.
        //  int a[2];
        //  &a == a
        *pointer_dim1 = 0;
      }
    }
  }

  void *ref2 = nullptr;
  if (parse_array_ref_from_pntr(arg2, &ref2, pos) == 0) {
    printf("failed parse_memcpy\n");
    ret = 0;
  }

  *sg_array2 = *sg_pntr2 = nullptr;
  if (ref2 != nullptr) {
    parse_pntr_ref_by_array_ref(ref2, sg_array2, sg_pntr2, sg_index2,
                                pointer_dim2);
    if (*sg_array2 != nullptr) {
      void *type = GetTypebyVar(*sg_array2);
      if ((IsArrayType(type) != 0) && *pointer_dim2 == -1) {
        //  array address is equal to its value, e.g.
        //  int a[2];
        //  &a == a
        *pointer_dim2 = 0;
      }
    }
  }
  *length_exp = GetFuncCallParam(memcpy_call, 2);

  return ret;
}

int CSageCodeGen::is_memcpy_with_small_const_trip_count(void *memcpy_call,
                                                        int threshold) {
  void *length_exp = nullptr;
  void *sg_array1;
  void *sg_pntr1;
  vector<void *> sg_index1;
  int pointer_dim1;
  void *sg_array2;
  void *sg_pntr2;
  vector<void *> sg_index2;
  int pointer_dim2;
  int ret = parse_memcpy(memcpy_call, &length_exp, &sg_array1, &sg_pntr1,
                         &sg_index1, &pointer_dim1, &sg_array2, &sg_pntr2,
                         &sg_index2, &pointer_dim2);
  if (length_exp == nullptr) {
    return 0;
  }
  CMarsExpression me_len(length_exp, this, memcpy_call);
  if (me_len.IsConstant() == 0) {
    return 0;
  }
  int64_t c_len = me_len.GetConstant();
  if (c_len <= threshold || (ret == 0)) {
    return 1;
  }
  //  check whether the memcpy can be converted into a loop with smaller trip
  //  count if the type of dst and src are the same
  void *array1_type = GetTypebyVar(sg_array1);
  void *array2_type = GetTypebyVar(sg_array2);
  if (GetBaseType(array1_type, true) != GetBaseType(array2_type, true)) {
    return 0;
  }
  int64_t unit_size = get_type_unit_size(array1_type, sg_array1);
  if (unit_size <= 1) {
    return 0;
  }
  c_len /= unit_size;
  if (c_len <= threshold) {
    return 1;
  }
  return 0;
}

//  for array_ref, it means whether there is write/read for any elements
//  Cross function
//  return 1 if  not determined
bool CSageCodeGen::array_ref_has_write_conservative(void *array_ref) {
  //  Yuxin: fix to bug 1118, special analysis to memcpy
  if (GetEnclosingNode("FunctionCallExp", array_ref) != nullptr) {
    void *sg_call = GetEnclosingNode("FunctionCallExp", array_ref);
    int idx = -1;
    for (int j = 0; j < GetFuncCallParamNum(sg_call); j++) {
      void *arg_exp = GetFuncCallParam(sg_call, j);
      if (IsInScope(array_ref, arg_exp) != 0) {
        idx = j;
        break;
      }
    }
    string str_name = GetFuncNameByCall(sg_call);
    if (str_name.find("memcpy_wide_bus_read") == 0 ||
        str_name.find("memcpy_wide_bus_write") == 0) {
      int is_write = static_cast<int>(
          GetFuncNameByCall(sg_call).find("memcpy_wide_bus_write") == 0);
      void *buf_arg = nullptr;
      if (is_write != 0) {
        buf_arg = GetFuncCallParam(sg_call, 1);
      } else {
        buf_arg = GetFuncCallParam(sg_call, 0);
      }
      assert(buf_arg);
      void *basic_type;
      vector<size_t> dims;
      get_type_dimension(GetTypeByExp(buf_arg), &basic_type, &dims, buf_arg);

      int offset_r = 1;
      if (dims.size() > 1 && (is_write == 0)) {
        offset_r += dims.size();
      }

      //            cout << "Offset and index: " << offset_r << ","
      //  << idx <<endl;

      if (idx == 0) {
        return true;
      }
      if (idx == offset_r) {
        return false;
      }
    }
  }
  void *sg_array;
  void *sg_pntr;
  vector<void *> sg_indexes;
  int pointer_dim;
  if (is_array_ref_full_dimension(array_ref, &sg_array, &sg_pntr, &sg_indexes,
                                  &pointer_dim) == 0) {
    int param_index = GetFuncCallParamIndex(sg_pntr);
    if (-1 != param_index) {
      void *sg_call = TraceUpToFuncCall(sg_pntr);
      assert((sg_call || IsConstructorInitializer(sg_call)));
      void *sg_decl_1 = GetFuncDeclByCall(sg_call, 1);  //  with body
      if (sg_decl_1 != nullptr) {
        void *sg_param = GetFuncParam(sg_decl_1, param_index);
        vector<void *> vec_arg_ref;
        GetNodesByType_int(GetFuncBody(sg_decl_1), "preorder", V_SgVarRefExp,
                           &vec_arg_ref);

        for (size_t i = 0; i < vec_arg_ref.size(); i++) {
          void *one_ref = vec_arg_ref[i];
          if (sg_param == GetVariableInitializedName(one_ref)) {
            if (array_ref_has_write_conservative(vec_arg_ref[i])) {
              return true;
            }
          }
        }
        return false;
      }
      string str_name = GetFuncNameByCall(sg_call);
      if (IsMerlinInternalIntrinsic(str_name)) {
        return false;
      }
      if (str_name.find("memcpy") == 0 || str_name == "memset") {
        return param_index == 0;
      }
      return true;  //  unknown function without decl
    }
    return true;  //  pointer alias`
  }
  return has_write_conservative(sg_pntr) != 0;
}
//  return 1 if  not determined
bool CSageCodeGen::array_ref_has_read_conservative(void *array_ref) {
  //  Yuxin: fix to bug 1118, special analysis to memcpy
  if (GetEnclosingNode("FunctionCallExp", array_ref) != nullptr) {
    void *sg_call = GetEnclosingNode("FunctionCallExp", array_ref);
    int idx = -1;
    for (int j = 0; j < GetFuncCallParamNum(sg_call); j++) {
      void *arg_exp = GetFuncCallParam(sg_call, j);
      if (IsInScope(array_ref, arg_exp) != 0) {
        idx = j;
        break;
      }
    }

    string str_name = GetFuncNameByCall(sg_call);
    if (str_name.find("memcpy_wide_bus_read") == 0 ||
        str_name.find("memcpy_wide_bus_write") == 0) {
      int is_write = static_cast<int>(
          GetFuncNameByCall(sg_call).find("memcpy_wide_bus_write") == 0);
      void *buf_arg = nullptr;
      if (is_write != 0) {
        buf_arg = GetFuncCallParam(sg_call, 1);
      } else {
        buf_arg = GetFuncCallParam(sg_call, 0);
      }
      assert(buf_arg);
      void *basic_type;
      vector<size_t> dims;
      get_type_dimension(GetTypeByExp(buf_arg), &basic_type, &dims, buf_arg);

      int offset_r = 1;
      if (dims.size() > 1 && (is_write == 0)) {
        offset_r += dims.size();
      }

      cout << "Offset and index: " << offset_r << "," << idx << endl;

      if (idx == 0) {
        return false;
      }
      if (idx == offset_r) {
        return true;
      }
    }
  }

  void *sg_array;
  void *sg_pntr;
  vector<void *> sg_indexes;
  int pointer_dim;
  if (is_array_ref_full_dimension(array_ref, &sg_array, &sg_pntr, &sg_indexes,
                                  &pointer_dim) == 0) {
    int param_index = GetFuncCallParamIndex(sg_pntr);
    if (-1 != param_index) {
      void *sg_call = TraceUpToFuncCall(sg_pntr);
      assert((sg_call || IsConstructorInitializer(sg_call)));
      void *sg_decl_1 = GetFuncDeclByCall(sg_call, 1);  //  with body
      if (sg_decl_1 != nullptr) {
        void *sg_param = GetFuncParam(sg_decl_1, param_index);
        vector<void *> vec_arg_ref;
        GetNodesByType_int(GetFuncBody(sg_decl_1), "preorder", V_SgVarRefExp,
                           &vec_arg_ref);

        for (size_t i = 0; i < vec_arg_ref.size(); i++) {
          void *one_ref = vec_arg_ref[i];
          if (sg_param == GetVariableInitializedName(one_ref)) {
            if (array_ref_has_read_conservative(vec_arg_ref[i])) {
              return true;
            }
          }
        }
        return false;
      }
      string str_name = GetFuncNameByCall(sg_call);
      if (IsMerlinInternalIntrinsic(str_name)) {
        return false;
      }
      if (str_name.find("memcpy") == 0 || str_name == "memset") {
        return param_index != 0;
      }
      {
        return true;  //  unknown function without decl
      }

    } else {
      return true;  //  pointer alias`
    }
  } else {
    return has_read_conservative(sg_pntr) != 0;
  }
}

//  0 for read, 1 for write, 2 for read & write
int CSageCodeGen::array_ref_is_write_conservative(void *array_ref) {
  int has_read = static_cast<int>(array_ref_has_read_conservative(array_ref));
  int has_write = static_cast<int>(array_ref_has_write_conservative(array_ref));

  if ((has_read != 0) && (has_write != 0)) {
    return 2;
  }

  if (has_write != 0) {
    return 1;
  }

  return 0;
}

int CSageCodeGen::is_array_ref_full_dimension(void *array_ref, void **sg_array,
                                              void **sg_pntr,
                                              vector<void *> *sg_indexes,
                                              int *pointer_dim) {
  parse_pntr_ref_by_array_ref(array_ref, sg_array, sg_pntr, sg_indexes,
                              pointer_dim);

  if (*sg_array == nullptr) {
    return 0;
  }

  {
    void *sg_type = GetTypebyVar(*sg_array);
    void *base_type;
    vector<size_t> nsizes;
    get_type_dimension(sg_type, &base_type, &nsizes, *sg_array);

    return static_cast<int>(nsizes.size() == static_cast<size_t>(*pointer_dim));
  }
}

void *CSageCodeGen::TraceUpToNonFloating(void *n) {
  void *p = n;
  while (p) {
    if (!is_floating_node(p))
      return p;

    SgType *sg_type = isSgType(reinterpret_cast<SgNode *>(p));
    if (sg_type) {
      return GetProject();
    } else {
      p = GetParent(p);
    }
  }

  return p;
}

//  This function handles
//  (for a[100][100][100]: )
//  1. ... = a[][][] ...
//  2. a[][][] = ...
//  3. f(..., a[][][], ...)
//  4. a
//  5. a+1
//  6. a[i]+1
//  7. a[i][j]
//  8. &(a[i][j][k]) + ...
//  9. *(a[i][j]) + ...
//  int pointer_dim: count from left
//     e.g. a[0][1][0] -> 3
//     e.g. a -> 0
//     e.g. &(a[0][1][0]) -> 2
//     e.g. &(&(a[0][0][0])+1) -> 1
//     e.g. &a -> -1
void CSageCodeGen::parse_pntr_ref_by_array_ref(
    void *sg_ref, void **sg_array, void **sg_pntr, vector<void *> *sg_indexes,
    int *pointer_dim, void *pos, bool simplify_idx /*= true*/,
    int adjusted_array_dim /*=-1*/, int include_class /*=0*/) {
  if (pos == nullptr) {
    pos = sg_ref;
  }

  pos = TraceUpToNonFloating(pos);

  DEFINE_START_END;
  vector<void *> ret_idx;

  *sg_array = GetVariableInitializedName(sg_ref);
  *sg_pntr = nullptr;

  void *sg_base_type;
  vector<size_t> sg_sizes;
  STEMP(start);
  get_type_dimension(GetTypebyVar(*sg_array), &sg_base_type, &sg_sizes,
                     *sg_array);
  TIMEP("      get_type_dimension", start, end);
  int dim = sg_sizes.size();
  STEMP(start);
  if (dim == 0) {  //  scalar, not undetermined
                   //  //////////////////////  /
                   //  ZP 20151123
    //  *sg_array = nullptr
    *sg_array = GetVariableInitializedName(sg_ref);
    //  //////////////////////  /
    //
    *sg_pntr = sg_ref;
    *pointer_dim = 0;
    return;
  }
  TIMEP("      parse_pntr_ref_by_array_ref___scalar", start, end);

  //  initialize
  //  ret_idx.resize(dim);
  STEMP(start);
  add_exp_to_vector(&ret_idx, dim - 1, nullptr, pos, simplify_idx);
  TIMEP("      add_exp_to_vector", start, end);

  int curr_dim = 0;  //  the leftmost dim
  void *curr_exp = sg_ref;
  void *curr_idx = nullptr;

  while (true) {
    void *parent = GetParent(curr_exp);
    if (adjusted_array_dim != -1 && dim < adjusted_array_dim) {
      dim = adjusted_array_dim;
    }

    if ((IsPntrArrRefExp(parent) != 0) &&
        curr_exp == GetExpLeftOperand(parent) && curr_dim >= 0 &&
        curr_dim < dim) {
      curr_idx = GetExpRightOperand(parent);

      STEMP(start);
      add_exp_to_vector(&ret_idx, curr_dim, curr_idx, pos, simplify_idx);
      TIMEP("      parse_pntr_ref_by_array_ref___array_ref", start, end);
      curr_dim++;
    } else if (IsAddressOfOp(parent) != 0) {
      curr_dim--;
    } else if ((IsCastExp(parent) != 0) &&
               (IsPointerType(GetTypeByExp(parent)) != 0)) {
      //  else do nothing
    } else if ((IsPointerDerefExp(parent) != 0) && curr_dim >= 0 &&
               curr_dim < dim) {
      STEMP(start);
      add_exp_to_vector(&ret_idx, curr_dim, nullptr, pos, simplify_idx);
      TIMEP("      parse_pntr_ref_by_array_ref___deref", start, end);
      curr_dim++;
    } else if ((IsAddOp(parent) != 0) && curr_dim < dim && curr_dim >= 0) {
      void *left = GetExpLeftOperand(parent);
      void *right = GetExpRightOperand(parent);

      if (curr_exp == left) {
        curr_idx = right;
      }
      if (curr_exp == right) {
        curr_idx = left;
      }

      STEMP(start);
      add_exp_to_vector(&ret_idx, curr_dim, curr_idx, pos, simplify_idx);
      TIMEP("      parse_pntr_ref_by_array_ref___add", start, end);
    } else if ((IsSubtractOp(parent) != 0) &&
               curr_exp == GetExpLeftOperand(parent) && curr_dim < dim &&
               curr_dim >= 0) {
      void *right = GetExpRightOperand(parent);

      curr_idx = CreateExp(V_SgMinusOp, CopyExp(right));

      STEMP(start);
      add_exp_to_vector(&ret_idx, curr_dim, curr_idx, pos, simplify_idx);
      TIMEP("      parse_pntr_ref_by_array_ref___subtract", start, end);
    } else if ((IsPlusPlusOp(parent) != 0) && curr_dim < dim && curr_dim >= 0) {
      //  FIXME: need to check whether post-incr or post-incr
      add_exp_to_vector(&ret_idx, curr_dim, parent, pos, simplify_idx);
    } else if ((IsMinusMinusOp(parent) != 0) && curr_dim < dim &&
               curr_dim >= 0) {
      //  FIXME: need to check whether post-decr or post-decr
      add_exp_to_vector(&ret_idx, curr_dim, parent, pos, simplify_idx);
    } else if ((include_class != 0) &&
               ((IsDotExp(parent) != 0) || (IsArrowExp(parent) != 0))) {
      //  else do nothing
      //  Yuxin: 06/30/2019
      //  support the partitioning of array in clase/struct
    } else if (parent == nullptr) {
      break;
    } else {
      break;
    }

    curr_exp = parent;
  }

  *sg_pntr = curr_exp;
  *sg_indexes = ret_idx;
  *pointer_dim = curr_dim;

  if (_DEBUG_PARSE_PNTR_) {
    string str_index = "idx->";
    for (size_t i = 0; i < sg_indexes->size(); i++) {
      //  if ((*sg_indexes)[i])
      str_index += "[" + UnparseToString((*sg_indexes)[i]) + "]";
      //  else
      //    str_index += "[0]";
    }
#ifdef PROJDEBUG
    cout << "[parse_pntr] " << UnparseToString(sg_ref) << " in "
         << UnparseToString(GetParent(sg_ref)) << " -> "
         << UnparseToString(*sg_pntr) << " " << str_index << " pntr->"
         << *pointer_dim << endl;
#endif
  }
}

bool CSageCodeGen::is_access_array_element(void *sg_ref, void *scope) {
  void *sg_array = GetVariableInitializedName(sg_ref);

  void *sg_base_type;
  vector<size_t> sg_sizes;
  get_type_dimension(GetTypebyVar(sg_array), &sg_base_type, &sg_sizes,
                     sg_array);
  int dim = sg_sizes.size();
  if (dim == 0) {  //  scalar, not undetermined
                   //  //////////////////////  /
                   //  ZP 20151123
    return true;
  }

  int curr_dim = 0;  //  the leftmost dim
  void *curr_exp = sg_ref;

  while (true) {
    void *parent = GetParent(curr_exp);
    if (!parent || !IsInScope(parent, scope))
      break;
    if ((IsPntrArrRefExp(parent) != 0) &&
        curr_exp == GetExpLeftOperand(parent) && curr_dim >= 0 &&
        curr_dim < dim) {
      curr_dim++;
    } else if (IsAddressOfOp(parent) != 0) {
      curr_dim--;
    } else if ((IsCastExp(parent) != 0) &&
               (IsPointerType(GetTypeByExp(parent)) != 0)) {
      //  else do nothing
    } else if ((IsPointerDerefExp(parent) != 0) && curr_dim >= 0 &&
               curr_dim < dim) {
      curr_dim++;
    } else if ((IsAddOp(parent) != 0) && curr_dim < dim && curr_dim >= 0) {
    } else if ((IsSubtractOp(parent) != 0) &&
               curr_exp == GetExpLeftOperand(parent) && curr_dim < dim &&
               curr_dim >= 0) {
    } else if ((IsPlusPlusOp(parent) != 0) && curr_dim < dim && curr_dim >= 0) {
    } else if ((IsMinusMinusOp(parent) != 0) && curr_dim < dim &&
               curr_dim >= 0) {
    } else if ((IsDotExp(parent) != 0) || (IsArrowExp(parent) != 0)) {
      if (IsDotExp(parent) != 0 && curr_exp == GetExpLeftOperand(parent))
        curr_dim++;
      break;
    } else if (parent == nullptr) {
      break;
    } else {
      break;
    }
    curr_exp = parent;
  }
  return curr_dim > 0;
}

void CSageCodeGen::parse_pntr_ref_by_func_call(void *sg_ref, void **sg_func,
                                               void **sg_pntr,
                                               vector<void *> *sg_indexes,
                                               int *pointer_dim, void *pos) {
  if (pos == nullptr) {
    pos = sg_ref;
  }

  vector<void *> ret_idx;

  *sg_func = GetFuncDeclByCall(sg_ref);
  if (*sg_func == nullptr) {
    *sg_func = GetFuncDeclByCall(sg_ref, 0);
  }
  if (*sg_func == nullptr) {
    return;
  }
  *sg_pntr = nullptr;

  void *sg_base_type;
  vector<size_t> sg_sizes;
  get_type_dimension(GetFuncReturnType(*sg_func), &sg_base_type, &sg_sizes);
  int dim = sg_sizes.size();

  if (dim == 0) {  //  scalar, not undetermined
                   //  //////////////////////  /
                   //  ZP 20151123
    *sg_pntr = sg_ref;
    *pointer_dim = 0;
    return;
  }

  //  initialize
  //  ret_idx.resize(dim);
  add_exp_to_vector(&ret_idx, dim - 1, nullptr, pos);

  int curr_dim = 0;  //  the leftmost dim
  void *curr_exp = sg_ref;
  void *curr_idx = nullptr;

  while (true) {
    void *parent = GetParent(curr_exp);

    if ((IsPntrArrRefExp(parent) != 0) &&
        curr_exp == GetExpLeftOperand(parent) && curr_dim >= 0 &&
        curr_dim < dim) {
      curr_idx = GetExpRightOperand(parent);

      add_exp_to_vector(&ret_idx, curr_dim, curr_idx, pos);
      curr_dim++;
    } else if ((IsCastExp(parent) != 0) &&
               (IsPointerType(GetTypeByExp(parent)) != 0)) {
      //  do nothing
    } else if ((IsPointerDerefExp(parent) != 0) && curr_dim >= 0 &&
               curr_dim < dim) {
      add_exp_to_vector(&ret_idx, curr_dim, nullptr, pos);
      curr_dim++;
    } else if ((IsAddOp(parent) != 0) && curr_dim < dim && curr_dim >= 0) {
      void *left = GetExpLeftOperand(parent);
      void *right = GetExpRightOperand(parent);

      if (curr_exp == left) {
        curr_idx = right;
      }
      if (curr_exp == right) {
        curr_idx = left;
      }

      add_exp_to_vector(&ret_idx, curr_dim, curr_idx, pos);
    } else if ((IsSubtractOp(parent) != 0) &&
               curr_exp == GetExpLeftOperand(parent) && curr_dim < dim &&
               curr_dim >= 0) {
      void *right = GetExpRightOperand(parent);

      curr_idx = CreateExp(V_SgMinusOp, CopyExp(right));

      add_exp_to_vector(&ret_idx, curr_dim, curr_idx, pos);
    } else if (parent == nullptr) {
      break;
    } else {
      break;
    }

    curr_exp = parent;
  }

  *sg_pntr = curr_exp;
  *sg_indexes = ret_idx;
  *pointer_dim = curr_dim;

  if (_DEBUG_PARSE_PNTR_) {
    string str_index = "idx->";
    for (size_t i = 0; i < sg_indexes->size(); i++) {
      //  if ((*sg_indexes)[i])
      str_index += "[" + UnparseToString((*sg_indexes)[i]) + "]";
      //  else
      //    str_index += "[0]";
    }
#ifdef PROJDEBUG
    cout << "[parse_pntr] " << UnparseToString(sg_ref) << " in "
         << UnparseToString(GetParent(sg_ref)) << " -> "
         << UnparseToString(*sg_pntr) << " " << str_index << " pntr->"
         << *pointer_dim << endl;
#endif
  }
}

int CSageCodeGen::parse_standard_pntr_ref_from_array_ref(
    void *sg_ref, void **sg_array, void **sg_pntr, void **sg_base_type,
    vector<void *> *sg_indexes, int *pointer_dim, void *pos) {
  if (pos == nullptr) {
    pos = sg_ref;
  }

  vector<void *> ret_idx;

  *sg_array = GetVariableInitializedName(sg_ref);
  *sg_pntr = sg_ref;
  *pointer_dim = 0;
  vector<size_t> sg_sizes;
  get_type_dimension(GetTypebyVar(*sg_array), sg_base_type, &sg_sizes,
                     *sg_array);
  int dim = sg_sizes.size();

  if (dim == 0) {  //  scalar, not undetermined
                   //  //////////////////////  /
                   //  ZP 20151123
    return 1;
  }

  int curr_dim = 0;  //  the leftmost dim
  void *curr_exp = sg_ref;
  void *curr_idx = nullptr;
  bool has_cast = false;
  while (true) {
    void *parent = GetParent(curr_exp);

    if ((IsPntrArrRefExp(parent) != 0) &&
        curr_exp == GetExpLeftOperand(parent) && curr_dim >= 0 &&
        curr_dim < dim) {
      if (has_cast) {
        return 0;
      }
      curr_idx = CopyExp(GetExpRightOperand(parent));
      add_exp_to_vector(&ret_idx, curr_dim, curr_idx, pos);
      curr_dim++;
    } else if (IsAddressOfOp(parent) != 0) {
      curr_dim--;
    } else if ((IsCastExp(parent) != 0) &&
               (IsPointerType(GetTypeByExp(parent)) != 0)) {
      void *new_type = GetTypeByExp(parent);
      vector<size_t> new_sizes;
      void *sg_cast_type;
      int new_dim =
          get_type_dimension(new_type, &sg_cast_type, &new_sizes, parent);
      //  youxiang 20170127 only support cast from more dimensions to less
      //  dimensions
      if (new_dim + curr_dim > dim) {
        return 0;
      }
      { curr_dim = dim - new_dim; }
      if (!has_cast) {
        has_cast = true;
        *sg_base_type = sg_cast_type;
      } else if (*sg_base_type != sg_cast_type) {
        return 0;
      }

    } else if ((IsPointerDerefExp(parent) != 0) && curr_dim >= 0 &&
               curr_dim < dim) {
      curr_dim++;
    } else if ((IsAddOp(parent) != 0) && curr_dim < dim && curr_dim >= 0) {
      if (has_cast) {
        return 0;
      }
      void *left = GetExpLeftOperand(parent);
      void *right = GetExpRightOperand(parent);

      if (curr_exp == left) {
        curr_idx = right;
      }
      if (curr_exp == right) {
        curr_idx = left;
      }
      add_exp_to_vector(&ret_idx, curr_dim, curr_idx, pos);

    } else if ((IsSubtractOp(parent) != 0) &&
               curr_exp == GetExpLeftOperand(parent) && curr_dim < dim &&
               curr_dim >= 0) {
      if (has_cast) {
        return 0;
      }
      *sg_pntr = parent;
      *pointer_dim = curr_dim;
      void *right = GetExpRightOperand(parent);

      curr_idx = CreateExp(V_SgMinusOp, CopyExp(right));

      add_exp_to_vector(&ret_idx, curr_dim, curr_idx, pos);

    } else if (parent == nullptr) {
      break;
    } else {
      break;
    }

    curr_exp = parent;
  }

  *sg_pntr = curr_exp;
  *pointer_dim = curr_dim;
  *sg_indexes = ret_idx;
  return 1;
}

int CSageCodeGen::standardize_pntr_ref_from_array_ref(void *sg_ref, void *pos) {
#ifdef PROJDEBUG
  cout << "standardize pointer deferences " << _p(sg_ref) << endl;
#endif
  void *sg_array = nullptr;
  void *sg_pntr = nullptr;
  vector<void *> sg_indexes;
  int pointer_dim = 0;
  if (pos == nullptr) {
    pos = sg_ref;
  }

  vector<void *> ret_idx;

  sg_array = GetVariableInitializedName(sg_ref);

  void *sg_base_type;
  vector<size_t> sg_sizes;
  get_type_dimension(GetTypebyVar(sg_array), &sg_base_type, &sg_sizes,
                     sg_array);
  int dim = sg_sizes.size();

  if (dim == 0) {  //  scalar, not undetermined
                   //  //////////////////////  /
                   //  ZP 20151123
    return 1;
  }

  //  initialize
  //  ret_idx.resize(dim);
  add_exp_to_vector(&ret_idx, dim - 1, nullptr, pos);

  int curr_dim = 0;  //  the leftmost dim
  void *curr_exp = sg_ref;
  void *curr_idx = nullptr;
  int org_bitwidth = get_bitwidth_from_type(sg_base_type);
  void *new_base_type = nullptr;
  int new_bitwidth = org_bitwidth;
  bool standardized_pntr = true;
  bool has_cast = false;
  while (true) {
    void *parent = GetParent(curr_exp);

    if ((IsPntrArrRefExp(parent) != 0) &&
        curr_exp == GetExpLeftOperand(parent) && curr_dim >= 0 &&
        curr_dim < dim) {
      curr_idx = CopyExp(GetExpRightOperand(parent));
      if (curr_dim + 1 == dim && new_bitwidth != org_bitwidth) {
        curr_idx = CreateExp(V_SgMultiplyOp, CopyExp(curr_idx),
                             CreateConst(new_bitwidth / org_bitwidth));
      }
      if (has_cast) {
        standardized_pntr = false;
      }
      add_exp_to_vector(&ret_idx, curr_dim, curr_idx, pos);
      curr_dim++;
    } else if (IsAddressOfOp(parent) != 0) {
      curr_dim--;
    } else if ((IsCastExp(parent) != 0) &&
               (IsPointerType(GetTypeByExp(parent)) != 0)) {
      has_cast = true;
      void *new_type = GetTypeByExp(parent);
      vector<size_t> new_sizes;
      void *cast_base_type;
      int new_dim =
          get_type_dimension(new_type, &cast_base_type, &new_sizes, parent);
      //  youxiang 20170127 only support cast from more dimensions to less
      //  dimensions
      if (new_dim + curr_dim > dim) {
        return 0;
      }
      { curr_dim = dim - new_dim; }
      if (new_base_type == nullptr) {
        new_base_type = cast_base_type;
      } else if (new_base_type != cast_base_type) {
        //  cannot support multiple pointer cast
        return 0;
      }

      new_bitwidth = get_bitwidth_from_type(new_base_type);
      //  currently, we only support casting from small size type to big size
      //  type and the size of result type is divisible by the size of original
      //  type
      if (new_bitwidth == -1 || org_bitwidth == -1 || org_bitwidth == 0 ||
          (org_bitwidth != 0 && new_bitwidth % org_bitwidth != 0)) {
        return 0;
      }
    } else if ((IsPointerDerefExp(parent) != 0) && curr_dim >= 0 &&
               curr_dim < dim) {
      add_exp_to_vector(&ret_idx, curr_dim, nullptr, pos);
      curr_dim++;
    } else if ((IsAddOp(parent) != 0) && curr_dim < dim && curr_dim >= 0) {
      void *left = GetExpLeftOperand(parent);
      void *right = GetExpRightOperand(parent);

      if (curr_exp == left) {
        curr_idx = right;
      }
      if (curr_exp == right) {
        curr_idx = left;
      }

      if (curr_dim + 1 == dim && new_bitwidth != org_bitwidth) {
        curr_idx = CreateExp(V_SgMultiplyOp, CopyExp(curr_idx),
                             CreateConst(new_bitwidth / org_bitwidth));
      }
      if (has_cast) {
        standardized_pntr = false;
      }
      add_exp_to_vector(&ret_idx, curr_dim, curr_idx, pos);
    } else if ((IsSubtractOp(parent) != 0) &&
               curr_exp == GetExpLeftOperand(parent) && curr_dim < dim &&
               curr_dim >= 0) {
      void *right = GetExpRightOperand(parent);

      curr_idx = CreateExp(V_SgMinusOp, CopyExp(right));

      if (curr_dim + 1 == dim && new_bitwidth != org_bitwidth) {
        curr_idx = CreateExp(V_SgMultiplyOp, CopyExp(curr_idx),
                             CreateConst(new_bitwidth / org_bitwidth));
      }
      add_exp_to_vector(&ret_idx, curr_dim, curr_idx, pos);
      if (has_cast) {
        standardized_pntr = false;
      }
    } else if (parent == nullptr) {
      break;
    } else {
      break;
    }

    curr_exp = parent;
  }

  sg_pntr = curr_exp;
  sg_indexes = ret_idx;

  if (_DEBUG_PARSE_PNTR_) {
    pointer_dim = curr_dim;
    string str_index = "idx->";
    for (size_t i = 0; i < sg_indexes.size(); i++) {
      //  if (sg_indexes[i])
      str_index += "[" + UnparseToString(sg_indexes[i]) + "]";
      //  else
      //    str_index += "[0]";
    }
#ifdef PROJDEBUG
    cout << "[parse_pntr] " << UnparseToString(sg_ref) << " in "
         << UnparseToString(GetParent(sg_ref)) << " -> "
         << UnparseToString(sg_pntr) << " " << str_index << " pntr->"
         << pointer_dim << endl;
#endif
  }

  if (standardized_pntr) {
    return 1;
  }
  sg_indexes.clear();
  for (auto idx : ret_idx) {
    if (is_floating_node(idx) == 0) {
      idx = CopyExp(idx);
    }
    sg_indexes.push_back(idx);
  }

  void *new_pntr = CreateArrayRef(CreateVariableRef(sg_array), sg_indexes);
  if (curr_dim == dim) {
    new_pntr = CreateExp(V_SgAddressOfOp, new_pntr);
    new_pntr = CreateCastExp(new_pntr, CreatePointerType(new_base_type));
    new_pntr = CreateExp(V_SgPointerDerefExp, new_pntr);
  } else {
    assert(curr_dim < dim);
    int i = curr_dim;
    void *cast_type = new_base_type;
    while (i != curr_dim) {
      cast_type = CreatePointerType(cast_type);
      ++i;
    }
    new_pntr = CreateCastExp(new_pntr, cast_type);
  }
  ReplaceExp(sg_pntr, new_pntr);
  return 1;
}

void CSageCodeGen::parse_pntr_ref(void *sg_ref, void **sg_array,
                                  vector<void *> *sg_indexes) {
  void *sg_var = nullptr;
  *sg_array = nullptr;

  get_var_from_pntr_ref(sg_ref, &sg_var);

  if (sg_var == nullptr) {
    return;
  }

  *sg_array = GetVariableInitializedName(sg_var);

  get_ref_dimension(sg_var, sg_indexes);
}

//  ref is the innermost VarRefExp
int CSageCodeGen::get_ref_dimension(void *ref, vector<void *> *indexes) {
  if ((isSgPntrArrRefExp(static_cast<SgNode *>(GetParent(ref))) != nullptr) &&
      isSgPntrArrRefExp(static_cast<SgNode *>(GetParent(ref)))
              ->get_lhs_operand() == ref) {
    indexes->push_back(isSgPntrArrRefExp(static_cast<SgNode *>(GetParent(ref)))
                           ->get_rhs_operand());
    return get_ref_dimension(GetParent(ref), indexes) + 1;
  }
  { return 0; }
}

//  recur for the global variables
void CSageCodeGen::get_ref_in_scope_recursive(void *var_init, void *scope,
                                              vector<void *> *vec_refs,
                                              bool skip_access_intrinsic) {
  get_ref_in_scope(var_init, scope, vec_refs, skip_access_intrinsic);

  if (IsGlobalInitName(var_init) != 0) {
    vector<void *> vec_calls;
    GetNodesByType_int(scope, "preorder", V_SgFunctionCallExp, &vec_calls);

    for (size_t i = 0; i < vec_calls.size(); i++) {
      void *sg_call = vec_calls[i];
      void *sg_decl = GetFuncDeclByCall(sg_call);
      void *sg_body = GetFuncBody(sg_decl);

      if (IsFromInputFile(sg_decl) != 0) {
        get_ref_in_scope_recursive(var_init, sg_body, vec_refs,
                                   skip_access_intrinsic);
      }
    }
  }
}

//  only used in is_movable test
int CSageCodeGen::has_write_in_scope_fast(void *var, void *scope) {
  pair<void *, void *> var_in_scope(var, scope);
  if (s_write_in_scope_fast_cache.count(var_in_scope) > 0) {
    return static_cast<int>(s_write_in_scope_fast_cache[var_in_scope]);
  }
  vector<void *> vec_refs;
  get_ref_in_scope(var, scope, &vec_refs, true);
  bool res = false;
  for (auto ref : vec_refs) {
    if (is_write_conservative(ref) != 0) {
      res = true;
      break;
    }
  }
  s_write_in_scope_fast_cache[var_in_scope] = res;
  return static_cast<int>(res);
}

//  only used in is_movable test
int CSageCodeGen::has_read_in_scope_fast(void *var, void *scope) {
  pair<void *, void *> var_in_scope(var, scope);
  if (s_read_in_scope_fast_cache.count(var_in_scope) > 0) {
    return static_cast<int>(s_read_in_scope_fast_cache[var_in_scope]);
  }
  vector<void *> vec_refs;
  get_ref_in_scope(var, scope, &vec_refs, true);
  bool res = false;
  for (auto ref : vec_refs) {
    if (has_read_conservative(ref, false) != 0) {
      res = true;
      break;
    }
  }
  s_read_in_scope_fast_cache[var_in_scope] = res;
  return static_cast<int>(res);
}

int CSageCodeGen::has_write_in_scope(void *array, void *scope) {
  pair<void *, void *> var_in_scope(array, scope);
  if (s_write_in_scope_cache.count(var_in_scope) > 0) {
    return static_cast<int>(s_write_in_scope_cache[var_in_scope]);
  }
  ArrayRangeAnalysis mar(array, this, scope, scope, false, false);
  bool res = mar.has_write() != 0;
  s_write_in_scope_cache[var_in_scope] = res;
  return static_cast<int>(res);
}

int CSageCodeGen::has_read_in_scope(void *array, void *scope) {
  pair<void *, void *> var_in_scope(array, scope);
  if (s_read_in_scope_cache.count(var_in_scope) > 0) {
    return static_cast<int>(s_read_in_scope_cache[var_in_scope]);
  }
  ArrayRangeAnalysis mar(array, this, scope, scope, false, false);
  bool res = mar.has_read() != 0;
  s_read_in_scope_cache[var_in_scope] = res;
  return static_cast<int>(res);
}

void CSageCodeGen::get_ref_in_scope(void *var_init, void *scope,
                                    vector<void *> *vec_refs,
                                    bool skip_access_intrinsic /*=false*/,
                                    bool is_preorder /*=true*/) {
  vector<void *> vec_nodes;
  string str_order = (is_preorder) ? "preorder" : "postorder";
  GetNodesByType_int(scope, str_order, V_SgVarRefExp, &vec_nodes);
  for (size_t i = 0; i < vec_nodes.size(); i++) {
    void *ref = vec_nodes[i];
    void *curr_var_init = GetVariableInitializedName(ref);
    if (curr_var_init == var_init) {
      if (skip_access_intrinsic) {
        int arg_idx = GetFuncCallParamIndex(ref);
        if (-1 != arg_idx) {
          void *func_call = TraceUpToFuncCall(ref);
          if (IsMerlinInternalIntrinsic(GetFuncNameByCall(func_call, false))) {
            continue;
          }
        }
      }
      vec_refs->push_back(ref);
    }
  }
}

bool CSageCodeGen::has_ref_in_scope(void *var_init, void *scope) {
  vector<void *> vec_nodes;
  GetNodesByType_int(scope, "preorder", V_SgVarRefExp, &vec_nodes);
  for (auto ref : vec_nodes) {
    void *curr_var_init = GetVariableInitializedName(ref);
    if (curr_var_init == var_init) {
      return true;
    }
  }
  return false;
}

void CSageCodeGen::get_ref_in_scope(void *var_init, void *scope,
                                    const t_func_call_path &call_path,
                                    vector<void *> *vec_refs,
                                    bool skip_access_intrinsic,
                                    bool is_preorder) {
  vector<void *> vec_nodes;
  string str_order = (is_preorder) ? "preorder" : "postorder";
  GetNodesByType_int(scope, str_order, V_SgVarRefExp, &vec_nodes);
  auto func = TraceUpToFuncDecl(var_init);
  auto curr_func = TraceUpToFuncDecl(scope);
  for (size_t i = 0; i < vec_nodes.size(); i++) {
    void *ref = vec_nodes[i];
    void *curr_var_init = GetVariableInitializedName(ref);
    if (func == curr_func && curr_var_init != var_init)
      continue;
    if (skip_access_intrinsic) {
      int arg_idx = GetFuncCallParamIndex(ref);
      if (-1 != arg_idx) {
        void *func_call = TraceUpToFuncCall(ref);
        if (IsMerlinInternalIntrinsic(GetFuncNameByCall(func_call, false))) {
          continue;
        }
      }
    }
    if ((is_same_instance(curr_var_init, var_init, call_path) != 0) &&
        (is_located_in_path_v2(ref, call_path) != 0)) {
      vec_refs->push_back(ref);
    }
  }
}

int CSageCodeGen::is_same_instance(void *target_init, void *src_init,
                                   const t_func_call_path &call_path) {
  auto call_iter = call_path.begin();
  while (true) {
    if (target_init == src_init) {
      return 1;
    }
    if (IsArgumentInitName(target_init) == 0) {
      return 0;
    }
    void *func_decl = TraceUpToFuncDecl(target_init);
    if (func_decl == nullptr) {
      return 0;
    }
    int idx = GetFuncParamIndex(target_init);
    assert(-1 != idx);
    while (call_iter != call_path.end()) {
      void *call = call_iter->second;
      if (IsFunctionCall(call) == 0) {
        ++call_iter;
        continue;
      }
      void *callee = call_iter->first;
      if ((call != nullptr) && IsMatchedFuncAndCall(func_decl, call, callee)) {
        break;
      }
      ++call_iter;
    }
    if (call_iter == call_path.end()) {
      break;
    }
    void *actual_exp = GetFuncCallParam(call_iter->second, idx);
    remove_cast(&actual_exp);
    void *array = get_array_from_pntr(actual_exp);
    if (array == nullptr) {
      return 0;
    }
    target_init = array;
    ++call_iter;
  }
  return 0;
}

//  pntr is the whole PntrRefExp and var is the innermost VarRefExp
int CSageCodeGen::get_var_from_pntr_ref(void *pntr, void **var) {
  if (IsVarRefExp(pntr) != 0) {
    *var = pntr;
    return 1;
  }
  if (IsPntrArrRefExp(pntr) == 0) {
    *var = nullptr;
    return 0;
  }
  SgNode *left_val =
      isSgPntrArrRefExp(static_cast<SgNode *>(pntr))->get_lhs_operand();

  if (IsPntrArrRefExp(left_val) == 0) {
    *var = left_val;
    return 1;
  }
  return get_var_from_pntr_ref(left_val, var);
}

//  pntr is the whole PntrRefExp and indexes is from rightmost dimension to the
//  left
int CSageCodeGen::get_index_from_pntr_ref(void *pntr, vector<void *> *indexes) {
  if (IsPntrArrRefExp(pntr) == 0) {
    return 0;
  }
  SgNode *left_val =
      isSgPntrArrRefExp(static_cast<SgNode *>(pntr))->get_lhs_operand();
  SgNode *right_val =
      isSgPntrArrRefExp(static_cast<SgNode *>(pntr))->get_rhs_operand();

  indexes->push_back(right_val);

  if (IsPntrArrRefExp(left_val) == 0) {
    //  find the array variable reference
    return 1;
  }
  return get_index_from_pntr_ref(left_val, indexes);
}

#if 0
int CSageCodeGen::get_pointer_dimension(void *sg_type_, void *&basic_type,
                                        vector<size_t> &nSizes) {
  //  if(isSgPointerType((SgNode*)sg_type_))
  SgPointerType *sg_type = isSgPointerType(static_cast<SgNode *>(sg_type_));
  if (!sg_type) {
    basic_type = sg_type_;
    return 0;
  }

  int dim_out =
      get_pointer_dimension(sg_type->get_base_type(), basic_type, nSizes);
  return dim_out + 1;
}
#endif

int CSageCodeGen::get_type_dimension_by_ref(void *sg_varref_or_pntr,
                                            void **sg_array, void **basic_type,
                                            vector<size_t> *vecSizes) {
  set<void *> visited_funcs;
  return get_type_dimension_by_ref(sg_varref_or_pntr, sg_array, basic_type,
                                   vecSizes, &visited_funcs);
}
//  int A[100][150];  A[10] -> int [150]
//  This analysis will cross function boundary
//  Supported types:
//  a. cmost_malloc and cmost_malloc_1d
//  b. malloc/calloc
//  c. array with size
//  d. new with size
//  Note:
//  1. If multiple sources are founded, return 0. E.g. multiple calls, multiple
//  malloc, multiple alias
//  2. If the bound is not determined, return 0. E.g. non-constant malloc
//  3. for scalar variable, vecSizes.size() = 0
int CSageCodeGen::get_type_dimension_by_ref(void *sg_varref_or_pntr,
                                            void **sg_array, void **basic_type,
                                            vector<size_t> *vecSizes,
                                            set<void *> *visited_funcs) {
  *sg_array = nullptr;

  void *sg_array1 = nullptr;
  void *basic_type1 = nullptr;
  vector<size_t> vecSizes1;
  int pointer_dim1 = 0;

  int ret = 0;

  //  1. handling in-function declaration
  //    this function returns the func arg decl if it is the decl of the
  //    reference
  void *decl_arg = parse_pntr_type_in_function(
      sg_varref_or_pntr, &sg_array1, &basic_type1, &vecSizes1, &pointer_dim1);

  //  2. handling function argument
  if (decl_arg != nullptr) {
    void *sg_func_decl = TraceUpToFuncDecl(decl_arg);
    if (visited_funcs->count(sg_func_decl) > 0) {
      return 0;
    }
    int index = GetFuncParamIndex(decl_arg);
    assert(index != -1);

    vector<void *> all_calls;
    GetFuncCallsFromDecl(sg_func_decl, nullptr, &all_calls);

    int nSize = all_calls.size();
    if (nSize != 1) {
      return 0;
    }

    void *sg_array2 = nullptr;
    void *basic_type2 = nullptr;
    vector<size_t> vecSize2;
    void *arg = GetFuncCallParam(all_calls[0], index);
    if (arg == nullptr) {
      return 0;
    }
    visited_funcs->insert(sg_func_decl);
    ret = get_type_dimension_by_ref(arg, &sg_array2, &basic_type2, &vecSize2,
                                    visited_funcs);
    visited_funcs->erase(sg_func_decl);
    //    assert(ret);
    if (ret == 0) {
      return 0;
    }

    vecSizes1.clear();
    sg_array1 = sg_array2;
    basic_type1 = basic_type2;
    for (size_t i = pointer_dim1; i < vecSize2.size(); i++) {
      vecSizes1.push_back(vecSize2[i]);
    }
  }

  //  for (size_t i = 0; i < vecSizes1.size(); i++) {
  //    if (vecSizes1[i] == 0)
  //      return 0;
  //  }

  *sg_array = sg_array1;
  *basic_type = basic_type1;
  *vecSizes = vecSizes1;

  return 1;
}

int CSageCodeGen::get_dim_num_from_var(void *array) {
  if (IsInitName(array) == 0) {
    return -1;
    // assert(0);
  }

  vector<size_t> sg_sizes;
  void *sg_base_type;
  get_type_dimension(GetTypebyVar(array), &sg_base_type, &sg_sizes,
                     GetVariableDecl(array));
  return sg_sizes.size();
}

//  Note
//  The order of the return dims is from high_dim to low_dim (left to right)
//  E.g. int a[1][2][3] => ( 1,2,3 )
//      int (*a)[10]   => ( 0, 10 )
//      int *a[10]     => ( 10, 0 )
int CSageCodeGen::get_type_dimension_new(void *sg_type_in, void **basic_type,
                                         vector<size_t> *nSizes, void *pos,
                                         bool stop_pointer) {
  int ret =
      get_type_dimension(sg_type_in, basic_type, nSizes, pos, stop_pointer);
  std::reverse(nSizes->begin(), nSizes->end());

  return ret;
}

//  Note
//  The order of the return dims is from low_dim to high_dim (right to left)
//  E.g. int a[1][2][3] => ( 3,2,1 )
//      int (*a)[10]   => ( 10, 0 )
//      int *a[10]     => ( 0, 10 )
//  This analysis will not cross function boundary
//  > 0: for normal array with contant array size
//  0: pointer
//  -1 (EMPTY_DIM_SIZE): array with null expression as array size
//  -2 (UNKNOWN_DIM_SIZE): array with variable array size (WARNING: do not
//  compare like "<0", use
//  "==-1" instead, since it is unsigned type
int CSageCodeGen::get_type_dimension(void *sg_type_in, void **basic_type,
                                     vector<size_t> *nSizes, void *pos,
                                     bool stop_pointer) {
  vector<void *> nSizes_exp;
  int ret = get_type_dimension_exp(sg_type_in, basic_type, &nSizes_exp, pos,
                                   stop_pointer);
  int dimension_size = nSizes_exp.size();
  nSizes->resize(dimension_size);
  for (int i = 0; i < dimension_size; ++i) {
    void *dim_exp = nSizes_exp[i];
    size_t size = 0;
    if (IsNullExp(dim_exp) != 0) {
      size = EMPTY_SIZE;
    } else {
      CMarsExpression me(dim_exp, this, pos);
      if (me.IsConstant() != 0) {
        size = me.GetConstant();
      } else {
        size = UNKNOWN_SIZE;
      }
    }
    (*nSizes)[i] = size;
    DeleteNode(dim_exp);
  }
  return ret;
}

int CSageCodeGen::get_type_dimension_exp(void *sg_type_in, void **basic_type,
                                         vector<void *> *nSizes, void *pos,
                                         bool stop_pointer) {
  void *sg_orig_type = sg_type_in;
  sg_type_in = GetOrigTypeByTypedef(sg_type_in);
  while ((IsModifierType(sg_type_in) != 0) ||
         (IsReferenceType(sg_type_in) != 0)) {
    sg_type_in = GetElementType(sg_type_in);
  }
  SgArrayType *sg_type_array = isSgArrayType(static_cast<SgNode *>(sg_type_in));
  SgPointerType *sg_type_pointer =
      isSgPointerType(static_cast<SgNode *>(sg_type_in));
  if ((sg_type_array == nullptr) &&
      ((sg_type_pointer == nullptr) || stop_pointer)) {
    SgType *type_tmp = isSgType(static_cast<SgNode *>(sg_type_in));
    assert(type_tmp);
    *basic_type = sg_orig_type;
    return 0;
  }

  if (sg_type_array != nullptr) {
    SgExpression *size_exp = sg_type_array->get_index();

    void *size = nullptr;
    if (IsNullExp(size_exp) != 0) {
    } else {
      if (isSgValueExp(size_exp) != nullptr) {
        int64_t value = 0;
        if (IsConstantInt(size_exp, &value, true, nullptr)) {
          size = CreateConst(value);
        }
        //  assert(0);
      } else {
        CMarsExpression me(size_exp, this, pos);
        if (me.IsConstant() != 0) {
          size = CreateConst(me.GetConstant());
        }
      }
    }
    if (nullptr == size) {
      size = CopyExp(size_exp);
    }

    int dim_out = get_type_dimension_exp(sg_type_array->get_base_type(),
                                         basic_type, nSizes, pos, stop_pointer);

    nSizes->push_back(size);  //  the last dimension

    return dim_out + 1;
  }
  if (sg_type_pointer != nullptr) {
    int dim_out = get_type_dimension_exp(sg_type_pointer->get_base_type(),
                                         basic_type, nSizes, pos, stop_pointer);

    nSizes->push_back(CreateConst(POINTER_SIZE));  //  FIXME: do not detect the
                                                   // bound for pointer yet.

    return dim_out + 1;
  }

  return 0;
}

void *CSageCodeGen::GetScope(void *stmt_or_exp) {
  return TraceUpByTypeCompatible(stmt_or_exp, V_SgScopeStatement);
}

void *CSageCodeGen::GetFirstFuncDeclInGlobal(void *pos) {
  void *sg_global = GetGlobal(pos);
  auto str_file = get_file(sg_global);
  for (size_t i = 0; i < GetChildStmtNum(sg_global); i++) {
    void *child = GetChildStmt(sg_global, i);
    if (isWithInHeaderFile(child))
      continue;
    if (!IsTransformation(child) && get_file(child) != str_file)
      continue;
    if ((IsFunctionDeclaration(child) != 0) &&
        (GetFuncBody(child) != nullptr)) {
      return child;
    }
  }
  return nullptr;
}

void CSageCodeGen::SetTypetoVar(void *var_init_name, void *sg_type) {
  void *sg_name_ = var_init_name;
  SgInitializedName *sg_name =
      isSgInitializedName(static_cast<SgNode *>(sg_name_));
  assert(sg_name);
  SgType *sg_type_ = isSgType(static_cast<SgNode *>(sg_type));
  assert(sg_type_);
  sg_name->set_type(sg_type_);

  //  Yuxin: 20181126
  //  Update symbol table
  if (IsArgumentInitName(sg_name) != 0) {
    SgFunctionDeclaration *sg_func_decl =
        static_cast<SgFunctionDeclaration *>(TraceUpToFuncDecl(sg_name));
    SgFunctionDefinition *sg_def = sg_func_decl->get_definition();
    if (sg_def != nullptr) {
      sg_def->insert_symbol(sg_name->get_name(), new SgVariableSymbol(sg_name));
    }
    // update the type of parameter of first non defined declaration which is
    // related to function symbol and function call
    SgFunctionDeclaration *sg_first_non_defined_func_decl =
        static_cast<SgFunctionDeclaration *>(
            sg_func_decl->get_firstNondefiningDeclaration());
    if (sg_first_non_defined_func_decl != sg_func_decl) {
      int arg_index = GetFuncParamIndex(sg_name);
      if (arg_index != -1 &&
          arg_index < GetFuncParamNum(sg_first_non_defined_func_decl)) {
        void *sg_param =
            GetFuncParam(sg_first_non_defined_func_decl, arg_index);
        if (sg_param != nullptr) {
          static_cast<SgInitializedName *>(sg_param)->set_type(sg_type_);
        }
      }
    }
  }
}

void *CSageCodeGen::GetTypebyNewExp(void *new_exp_) {
  void *sg_new_exp = new_exp_;
  SgNewExp *sg_new = isSgNewExp(static_cast<SgNode *>(sg_new_exp));
  assert(sg_new);
  return sg_new->get_specified_type();
}

void *CSageCodeGen::GetTypebyVar(void *var_init_name) const {
  void *sg_name_ = var_init_name;
  SgInitializedName *sg_name =
      isSgInitializedName(static_cast<SgNode *>(sg_name_));
  // assert(sg_name);
  if (!sg_name)
    return nullptr;
  return sg_name->get_type();
}

void *CSageCodeGen::GetTypebyVarRef(void *var_ref_) {
  void *sg_name_ = GetVariableInitializedName(var_ref_);
  SgInitializedName *sg_name =
      isSgInitializedName(static_cast<SgNode *>(sg_name_));
  if (!sg_name)
    return nullptr;
  // assert(sg_name);
  return sg_name->get_type();
}

void *CSageCodeGen::GetTypeByExp(void *sg_exp_) {
  SgExpression *sg_exp = isSgExpression(static_cast<SgNode *>(sg_exp_));
  if (sg_exp == nullptr) {
    return nullptr;
  }
  return sg_exp->get_type();
}

void *CSageCodeGen::GetTypeByEnumVal(void *sg_enum_val_) {
  SgEnumVal *sg_enum_val = isSgEnumVal(static_cast<SgNode *>(sg_enum_val_));
  if (sg_enum_val == nullptr) {
    return nullptr;
  }
  SgEnumDeclaration *sg_enum_decl = sg_enum_val->get_declaration();
  if (sg_enum_decl == nullptr) {
    return nullptr;
  }
  return sg_enum_decl->get_type();
}

void *CSageCodeGen::GetBaseTypeOneLayer(void *sg_type) const {
  if (isSgTypedefType(static_cast<SgNode *>(sg_type)) != nullptr) {
    return isSgTypedefType(static_cast<SgNode *>(sg_type))->get_base_type();
  }
  if (isSgModifierType(static_cast<SgNode *>(sg_type)) != nullptr) {
    return isSgModifierType(static_cast<SgNode *>(sg_type))->get_base_type();
  }
  if (isSgPointerType(static_cast<SgNode *>(sg_type)) != nullptr) {
    return isSgPointerType(static_cast<SgNode *>(sg_type))->get_base_type();
  }
  if (isSgArrayType(static_cast<SgNode *>(sg_type)) != nullptr) {
    return isSgArrayType(static_cast<SgNode *>(sg_type))->get_base_type();
  }
  if (isSgReferenceType(static_cast<SgNode *>(sg_type)) != nullptr) {
    return isSgReferenceType(static_cast<SgNode *>(sg_type))->get_base_type();
  }
  return nullptr;
}
void *CSageCodeGen::trace_base_type(void *sg_type,
                                    bool skip_anonymous_type) const {
  if (IsWithLength(this, sg_type) != 0) {
    return sg_type;
  }
  if (isSgModifierType(static_cast<SgNode *>(sg_type)) != nullptr) {
    return trace_base_type(
        isSgModifierType(static_cast<SgNode *>(sg_type))->get_base_type(),
        skip_anonymous_type);
  }
  if (isSgPointerType(static_cast<SgNode *>(sg_type)) != nullptr) {
    return trace_base_type(
        isSgPointerType(static_cast<SgNode *>(sg_type))->get_base_type(),
        skip_anonymous_type);
  }
  if (isSgArrayType(static_cast<SgNode *>(sg_type)) != nullptr) {
    return trace_base_type(
        isSgArrayType(static_cast<SgNode *>(sg_type))->get_base_type(),
        skip_anonymous_type);
  }
  if (isSgReferenceType(static_cast<SgNode *>(sg_type)) != nullptr) {
    return trace_base_type(
        isSgReferenceType(static_cast<SgNode *>(sg_type))->get_base_type(),
        skip_anonymous_type);
  }
  if (isSgTypedefType(static_cast<SgNode *>(sg_type)) != nullptr) {
    void *base_type =
        isSgTypedefType(static_cast<SgNode *>(sg_type))->get_base_type();
    if (!skip_anonymous_type && (IsAnonymousType(base_type) != 0)) {
      return sg_type;
    }
    return trace_base_type(base_type, skip_anonymous_type);
  }
  return sg_type;
}

void *CSageCodeGen::GetElementType(void *sg_type_) const {
  SgType *sg_type = isSgType(static_cast<SgNode *>(sg_type_));
  if (sg_type == nullptr) {
    return nullptr;
  }
  if ((isSgPointerType(sg_type) != nullptr) ||
      (isSgArrayType(sg_type) != nullptr)) {
    return SageInterface::getElementType(sg_type);
  }
  if (isSgReferenceType(sg_type) != nullptr) {
    return isSgReferenceType(sg_type)->get_base_type();
  }
  if (isSgModifierType(static_cast<SgNode *>(sg_type)) != nullptr) {
    return isSgModifierType(sg_type)->get_base_type();
  }
  if (isSgTypedefType(sg_type) != nullptr) {
    return isSgTypedefType(sg_type)->get_base_type();
  }
  return sg_type;
}

void *CSageCodeGen::CreateClassDecl(
    const SgName &name, SgClassDeclaration::class_types kind,
    SgScopeStatement *scope, SgClassDeclaration *nonDefiningDecl,
    bool buildTemplateInstantiation,
    SgTemplateArgumentPtrList *templateArgumentsList,
    void *bindNode /*= nullptr*/) {
#ifdef PROJDEBUG
  assert(!nonDefiningDecl || is_floating_node(nonDefiningDecl));
  assert(!templateArgumentsList || is_floating_node(templateArgumentsList));
#endif
  auto new_class_decl = SageBuilder::buildClassDeclaration_nfi(
      name, kind, scope, nonDefiningDecl, buildTemplateInstantiation,
      templateArgumentsList);
  markBuild(this, new_class_decl,
            "Build in " + std::to_string(__LINE__) + " for " +
                std::to_string((int64_t)new_class_decl),
            bindNode);
  return new_class_decl;
}

void *CSageCodeGen::CreateModifierType(void *new_base_type,
                                       void *orig_modifier_type,
                                       void *bindNode /*= nullptr*/) {
  SgModifierType *sg_m_type =
      isSgModifierType(static_cast<SgNode *>(orig_modifier_type));
  SgTypeModifier orig_modifier = sg_m_type->get_typeModifier();
  SgModifierType *sg_m_base_type =
      isSgModifierType(static_cast<SgNode *>(new_base_type));
  if (sg_m_base_type == nullptr) {
    sg_m_base_type = SageBuilder::buildModifierType(
        isSgType(static_cast<SgNode *>(new_base_type)));
    //  markBuild(* this, new_sg_m_type, "Build in " + std::to_string(__LINE__)
    //  + " for " + std::to_string((int64_t) new_sg_m_type), bindNode);
    sg_m_base_type->get_typeModifier() = orig_modifier;
  } else {
    SgTypeModifier new_modifier = sg_m_base_type->get_typeModifier();
    SgConstVolatileModifier &new_cv = new_modifier.get_constVolatileModifier();
    SgConstVolatileModifier &old_cv = orig_modifier.get_constVolatileModifier();
    if (old_cv.isConst() && !new_cv.isConst()) {
      sg_m_base_type = SageBuilder::buildConstType(sg_m_base_type);
    }
    if (old_cv.isVolatile() && !new_cv.isVolatile()) {
      sg_m_base_type = SageBuilder::buildVolatileType(sg_m_base_type);
    }
  }
  return sg_m_base_type;
}

//  NOTE: 20170320: ZP:
//  @Youxiang, what does the return value mean? it seems a basic primitive type
//  will also return 0 ?? return 0: no need to replace or cannot replace return
//  1: need to replace
int CSageCodeGen::RemoveTypedef(void *sg_type_, void **new_type, void *pos,
                                bool skip_builtin, string msg) {
  if (nullptr == sg_type_ || (IsWithLength(this, sg_type_) != 0)) {
    return 0;
  }
  SgType *sg_type = isSgType(static_cast<SgNode *>(sg_type_));
  *new_type = sg_type;
  if ((IsBuiltinType(sg_type) != 0) && skip_builtin) {
    return 0;
  }
  if (isSgTypedefType(sg_type) != nullptr) {
    void *base_type = isSgTypedefType(sg_type)->get_base_type();
    if ((IsAnonymousType(base_type) != 0) || (IsDefaultType(base_type) != 0)) {
      return 0;
    }
    // some kind of typedef is different from its base type, such as
    // typedef struct st AlignedSt __attribute__((aligned(64))
    if ((IsUnionType(base_type) != 0) || (IsEnumType(base_type) != 0) ||
        (IsStructureType(base_type) != 0) || (IsClassType(base_type) != 0)) {
      return 0;
    }
    //  YX: 20171010 only handle typedef of modifier type
    if (skip_builtin && (IsModifierType(base_type) == 0)) {
      return 0;
    }
    RemoveTypedef(base_type, new_type, pos, skip_builtin, msg);
    return 1;
  }

  if (isSgPointerType(sg_type) != nullptr) {
    void *base_type = GetElementType(sg_type);
    void *new_base_type = base_type;
    int ret = RemoveTypedef(base_type, &new_base_type, pos, skip_builtin, msg);
    if (ret != 0) {
      *new_type = CreatePointerType(new_base_type);
    }
    return ret;
  }

  if (isSgArrayType(sg_type) != nullptr) {
    SgArrayType *sg_type_array = isSgArrayType(sg_type);
    SgExpression *size_exp = sg_type_array->get_index();
    void *base_type = sg_type_array->get_base_type();
    void *new_base_type = base_type;
    int ret = RemoveTypedef(base_type, &new_base_type, pos, skip_builtin, msg);
    if (ret != 0) {
      *new_type = SageBuilder::buildArrayType(
          isSgType(static_cast<SgNode *>(new_base_type)), size_exp);
    }
    return ret;
  }
  if (isSgReferenceType(sg_type) != nullptr) {
    void *base_type = GetElementType(sg_type);
    void *new_base_type = base_type;
    int ret = RemoveTypedef(base_type, &new_base_type, pos, skip_builtin, msg);
    if (ret != 0) {
      *new_type = CreateReferenceType(new_base_type);
    }
    return ret;
  }

  if (isSgModifierType(static_cast<SgNode *>(sg_type)) != nullptr) {
    SgModifierType *sg_m_type =
        isSgModifierType(static_cast<SgNode *>(sg_type));
    SgTypeModifier orig_modifier = sg_m_type->get_typeModifier();
    void *base_type = sg_m_type->get_base_type();
    void *new_base_type = base_type;
    int ret = RemoveTypedef(base_type, &new_base_type, pos, skip_builtin, msg);
    if (ret != 0) {
      SgModifierType *new_sg_m_type = SageBuilder::buildModifierType(
          isSgType(static_cast<SgNode *>(new_base_type)));
      //  markBuild(* this, new_sg_m_type, "Build in " +
      //  std::to_string(__LINE__) + " for " + std::to_string((int64_t)
      //  new_sg_m_type));
      new_sg_m_type->get_typeModifier() = orig_modifier;
      *new_type = new_sg_m_type;
    }
    return ret;
  }

  //  FIXME: handle struct type
  return 0;
  //  return 1;
}

void *CSageCodeGen::RemoveConstType(void *sg_type_, void *pos, bool *changed) {
  if (nullptr == sg_type_) {
    return nullptr;
  }
  SgType *sg_type = isSgType(static_cast<SgNode *>(sg_type_));
  void *new_type = nullptr;
  if (isSgTypedefType(sg_type) != nullptr) {
    void *base_type = isSgTypedefType(sg_type)->get_base_type();
    void *new_type = RemoveConstType(base_type, pos, changed);
    if (base_type == new_type) {
      return sg_type;
    }
    return new_type;
  }
  if (isSgModifierType(sg_type) != nullptr) {
    SgModifierType *sg_m_type =
        isSgModifierType(static_cast<SgNode *>(sg_type));
    SgTypeModifier new_modifier = sg_m_type->get_typeModifier();
    SgConstVolatileModifier &cv = new_modifier.get_constVolatileModifier();
    if (cv.isConst()) {
      *changed = true;
      cv.unsetConst();
    }
    void *base_type = sg_m_type->get_base_type();
    void *new_base_type = RemoveConstType(base_type, pos, changed);
    if (new_base_type != nullptr) {
      if (cv.isDefault()) {
        return new_base_type;
      }
      SgModifierType *new_sg_m_type = SageBuilder::buildModifierType(
          isSgType(static_cast<SgNode *>(new_base_type)));
      new_sg_m_type->get_typeModifier() = new_modifier;
      return new_sg_m_type;
    }
    return nullptr;
  }

  if (isSgArrayType(sg_type) != nullptr) {
    //  void *base_type = nullptr;
    SgArrayType *sg_type_array = isSgArrayType(sg_type);
    SgExpression *size_exp = sg_type_array->get_index();

    int64_t size;
    if (IsNullExp(size_exp) != 0) {
      size = EMPTY_SIZE;
    } else {
      if (isSgValueExp(size_exp) != nullptr) {
        int64_t value = 0;
        if (IsConstantInt(size_exp, &value, true, nullptr)) {
          size = value;
        } else {
          size = UNKNOWN_SIZE;
        }
      } else {
        {
          CMarsExpression me(size_exp, this, pos);
          if (me.IsConstant() != 0) {
            size = me.GetConstant();
          } else {
            size = UNKNOWN_SIZE;  //  FIXME: the behaviour when non-constant
          }
          //  size is not
          //  determined
        }
      }
    }

    void *base_type = sg_type_array->get_base_type();
    vector<size_t> dims;
    dims.push_back(size);
    void *new_base_type = RemoveConstType(base_type, pos, changed);
    if (new_base_type != nullptr) {
      std::reverse(dims.begin(), dims.end());
      new_type = CreateArrayType(new_base_type, dims);
      return new_type;
    }
    return nullptr;
  }

  if (isSgPointerType(sg_type) != nullptr ||
      isSgReferenceType(sg_type) != nullptr) {
    // youxiang 20191031 we cannot remove const for pointer base type
    // MER-1493
    return sg_type;
  }

  //  FIXME: handle struct type
  return sg_type;
}

int CSageCodeGen::ConvertBool2Char(void *sg_type_, void *pos, void **new_type) {
  if (nullptr == sg_type_) {
    return 0;
  }
  SgType *sg_type = isSgType(static_cast<SgNode *>(sg_type_));
  if (isSgTypedefType(sg_type) != nullptr) {
    void *base_type = isSgTypedefType(sg_type)->get_base_type();
    return ConvertBool2Char(base_type, pos, new_type);
  }
  if (isSgModifierType(sg_type) != nullptr) {
    SgModifierType *sg_m_type =
        isSgModifierType(static_cast<SgNode *>(sg_type));
    void *base_type = sg_m_type->get_base_type();
    void *new_base_type = nullptr;
    if (ConvertBool2Char(base_type, pos, &new_base_type) != 0) {
      *new_type = CreateModifierType(new_base_type, sg_type);
      return 1;
    }
    return 0;
  }

  if (isSgPointerType(sg_type) != nullptr) {
    void *base_type = GetElementType(sg_type);
    void *new_base_type = nullptr;
    if (ConvertBool2Char(base_type, pos, &new_base_type) != 0) {
      *new_type = CreatePointerType(new_base_type);
      return 1;
    }
    return 0;
  }

  if (isSgArrayType(sg_type) != nullptr) {
    //  void *base_type = nullptr;
    SgArrayType *sg_type_array = isSgArrayType(sg_type);
    SgExpression *size_exp = sg_type_array->get_index();

    int64_t size;
    if (IsNullExp(size_exp) != 0) {
      size = EMPTY_SIZE;
    } else {
      if (isSgValueExp(size_exp) != nullptr) {
        int64_t value = 0;
        if (IsConstantInt(size_exp, &value, true, nullptr)) {
          size = value;
        } else {
          size = UNKNOWN_SIZE;
        }
      } else {
        {
          CMarsExpression me(size_exp, this, pos);
          if (me.IsConstant() != 0) {
            size = me.GetConstant();
          } else {
            size = UNKNOWN_SIZE;  //  FIXME: the behaviour when non-constant
          }
          //  size is not
          //  determined
        }
      }
    }

    void *base_type = sg_type_array->get_base_type();
    vector<size_t> dims;
    dims.push_back(size);
    void *new_base_type = nullptr;
    if (ConvertBool2Char(base_type, pos, &new_base_type) != 0) {
      std::reverse(dims.begin(), dims.end());
      *new_type = CreateArrayType(new_base_type, dims);
      return 1;
    }
    return 0;
  }
  if (isSgReferenceType(sg_type) != nullptr) {
    void *base_type = GetElementType(sg_type);
    void *new_base_type = nullptr;
    if (ConvertBool2Char(base_type, pos, &new_base_type) != 0) {
      *new_type = CreateReferenceType(new_base_type);
      return 1;
    }
    return 0;
  }

  if (isSgTypeBool(sg_type) != nullptr) {
    *new_type = GetTypeByString("char");
    return 1;
  }
  auto char_type = SageBuilder::buildCharType();
  if (auto *clsty = isSgClassType(sg_type)) {
    bool res = false;
    if (auto *tc = isSgTemplateInstantiationDecl(clsty->get_declaration())) {
      for (SgTemplateArgument *targ : tc->get_templateArguments()) {
        if (auto *tt = targ->get_type()) {
          if (isSgTypeBool(tt)) {
            targ->set_type(char_type);
            res = true;
          } else if (auto *tydef = isSgTypedefType(tt)) {
            if (char_type == tydef->get_base_type()) {
              res = true;
              targ->set_type(char_type);
            }
          } else {
            void *new_base_type = nullptr;
            res |= ConvertBool2Char(tt, pos, &new_base_type);
          }
        }
      }
      if (auto def = tc->get_definingDeclaration()) {
        if (auto *tc_def = isSgTemplateInstantiationDecl(def)) {
          for (SgTemplateArgument *targ : tc_def->get_templateArguments()) {
            if (auto *tt = targ->get_type()) {
              if (isSgTypeBool(tt)) {
                targ->set_type(char_type);
                res = true;
              } else if (auto *tydef = isSgTypedefType(tt)) {
                if (char_type == tydef->get_base_type()) {
                  res = true;
                  targ->set_type(char_type);
                }
              } else {
                void *new_base_type = nullptr;
                res |= ConvertBool2Char(tt, pos, &new_base_type);
              }
            }
          }
        }
      }
    }
    *new_type = sg_type;
    return res;
  }
  *new_type = sg_type;
  return 0;
}

void CSageCodeGen::get_exprs_from_type(void *sg_type_,
                                       vector<void *> *vec_exprs) {
  if (nullptr == sg_type_) {
    return;
  }
  SgType *sg_type = isSgType(static_cast<SgNode *>(sg_type_));
  if (isSgTypedefType(sg_type) != nullptr) {
    return;
  }
  if (isSgModifierType(sg_type) != nullptr) {
    SgModifierType *sg_m_type =
        isSgModifierType(static_cast<SgNode *>(sg_type));
    void *base_type = sg_m_type->get_base_type();
    get_exprs_from_type(base_type, vec_exprs);
    return;
  }

  if (isSgPointerType(sg_type) != nullptr) {
    void *base_type = GetElementType(sg_type);
    get_exprs_from_type(base_type, vec_exprs);
    return;
  }

  if (isSgArrayType(sg_type) != nullptr) {
    //  void *base_type = nullptr;
    SgArrayType *sg_type_array = isSgArrayType(sg_type);
    SgExpression *size_exp = sg_type_array->get_index();
    vec_exprs->insert(vec_exprs->end(), size_exp);
    void *base_type = sg_type_array->get_base_type();
    get_exprs_from_type(base_type, vec_exprs);
    return;
  }
  if (isSgReferenceType(sg_type) != nullptr) {
    void *base_type = GetElementType(sg_type);
    get_exprs_from_type(base_type, vec_exprs);
    return;
  }
}

void CSageCodeGen::replace_var_refs_in_type(void *sg_type_, void *pos) {
  if (nullptr == sg_type_) {
    return;
  }
  SgType *sg_type = isSgType(static_cast<SgNode *>(sg_type_));
  if (isSgTypedefType(sg_type) != nullptr) {
    replace_var_refs_in_type(GetElementType(sg_type), pos);
    return;
  }
  if (isSgModifierType(sg_type) != nullptr) {
    replace_var_refs_in_type(GetElementType(sg_type), pos);
    return;
  }

  if (isSgPointerType(sg_type) != nullptr) {
    replace_var_refs_in_type(GetElementType(sg_type), pos);
    return;
  }

  if (isSgArrayType(sg_type) != nullptr) {
    //  void *base_type = nullptr;
    SgArrayType *sg_type_array = isSgArrayType(sg_type);
    SgExpression *size_exp = sg_type_array->get_index();
    vector<void *> vec_refs;
    GetNodesByType_int(size_exp, "preorder", V_SgVarRefExp, &vec_refs);
    vector<void *> vec_enum_vals;
    GetNodesByType_int(size_exp, "preorder", V_SgEnumVal, &vec_enum_vals);
    vec_refs.insert(vec_refs.end(), vec_enum_vals.begin(), vec_enum_vals.end());
    if (!vec_refs.empty()) {
      CMarsExpression me_size_exp(size_exp, this, pos);
      if (me_size_exp.IsConstant() && me_size_exp.GetConstant() < 0) {
        // MER1540 do nothing for negative size
      } else {
        for (auto ref : vec_refs) {
          CMarsExpression me_val(ref, this, pos);
          if (me_val.is_movable_to(pos) != 0) {
            void *parent = GetParent(ref);
            if (IsExpression(parent) != 0) {
              ReplaceExp(ref, me_val.get_expr_from_coeff());
            } else if (size_exp == ref) {
              auto new_size_exp = isSgExpression(
                  static_cast<SgNode *>(me_val.get_expr_from_coeff()));
              sg_type_array->set_index(new_size_exp);
            } else {
              assert(0);
            }
          }
        }
      }
    }
    replace_var_refs_in_type(sg_type_array->get_base_type(), pos);
    return;
  }
  if (isSgReferenceType(sg_type) != nullptr) {
    replace_var_refs_in_type(GetElementType(sg_type), pos);
  }
  return;
}

void *CSageCodeGen::RemoveConstVolatileType(void *sg_type_, void *pos) {
  if (nullptr == sg_type_) {
    return nullptr;
  }
  SgType *sg_type = isSgType(static_cast<SgNode *>(sg_type_));
  void *new_type = nullptr;
  if (isSgTypedefType(sg_type) != nullptr) {
    void *base_type = isSgTypedefType(sg_type)->get_base_type();
    void *new_type = RemoveConstVolatileType(base_type, pos);
    if (base_type == new_type) {
      return sg_type;
    }
    return new_type;
  }
  if (isSgModifierType(sg_type) != nullptr) {
    SgModifierType *sg_m_type =
        isSgModifierType(static_cast<SgNode *>(sg_type));
    SgTypeModifier new_modifier = sg_m_type->get_typeModifier();
    SgConstVolatileModifier &cv = new_modifier.get_constVolatileModifier();
    if (cv.isConst()) {
      cv.unsetConst();
    }
    if (cv.isVolatile()) {
      cv.unsetVolatile();
    }
    void *base_type = sg_m_type->get_base_type();
    void *new_base_type = RemoveConstVolatileType(base_type, pos);
    if (new_base_type != nullptr) {
      if (cv.isDefault()) {
        return new_base_type;
      }
      SgModifierType *new_sg_m_type = SageBuilder::buildModifierType(
          isSgType(static_cast<SgNode *>(new_base_type)));
      new_sg_m_type->get_typeModifier() = new_modifier;
      return new_sg_m_type;
    }
    return nullptr;
  }

  if (isSgPointerType(sg_type) != nullptr) {
    void *base_type = GetElementType(sg_type);
    void *new_base_type = RemoveConstVolatileType(base_type, pos);
    if (new_base_type != nullptr) {
      new_type = CreatePointerType(new_base_type);
      return new_type;
    }
    return nullptr;
  }

  if (isSgArrayType(sg_type) != nullptr) {
    //  void *base_type = nullptr;
    SgArrayType *sg_type_array = isSgArrayType(sg_type);
    SgExpression *size_exp = sg_type_array->get_index();

    int64_t size;
    if (IsNullExp(size_exp) != 0) {
      size = EMPTY_SIZE;
    } else {
      if (isSgValueExp(size_exp) != nullptr) {
        int64_t value = 0;
        if (IsConstantInt(size_exp, &value, true, nullptr)) {
          size = value;
        } else {
          size = UNKNOWN_SIZE;
        }
      } else {
        {
          CMarsExpression me(size_exp, this, pos);
          if (me.IsConstant() != 0) {
            size = me.GetConstant();
          } else {
            size = UNKNOWN_SIZE;  //  FIXME: the behaviour when non-constant
          }
          //  size is not
          //  determined
        }
      }
    }

    void *base_type = sg_type_array->get_base_type();
    vector<size_t> dims;
    dims.push_back(size);
    void *new_base_type = RemoveConstVolatileType(base_type, pos);
    if (new_base_type != nullptr) {
      std::reverse(dims.begin(), dims.end());
      new_type = CreateArrayType(new_base_type, dims);
      return new_type;
    }
    return nullptr;
  }
  if (isSgReferenceType(sg_type) != nullptr) {
    void *base_type = GetElementType(sg_type);
    void *new_base_type = RemoveConstVolatileType(base_type, pos);
    if (new_base_type != nullptr) {
      new_type = CreateReferenceType(new_base_type);
      return new_type;
    }
    return nullptr;
  }

  //  FIXME: handle struct type
  return sg_type;
}

//  ZP: add the support for crossing function
void *CSageCodeGen::GetLoopFromIteratorByPos(void *sg_name, void *sg_pos,
                                             int cross_func /* = 0*/,
                                             int trace_back /* =1 */) {
  //  FIXME: need to give in a position argument

  if (IsInitName(sg_name) == 0) {
    return nullptr;
  }
  if ((sg_pos) == nullptr) {
    return nullptr;
  }

  DEFINE_START_END;

  void *sg_curr = TraceUpByTypeCompatible(sg_pos, V_SgForStatement);
  void *sg_pre = sg_pos;

  while (sg_curr != nullptr) {
    if (sg_name == GetLoopIterator(sg_curr)) {
      break;
    }
    sg_pre = sg_curr;
    sg_curr = TraceUpByTypeCompatible(GetParent(sg_curr), V_SgForStatement);
  }

  void *sg_func_decl = TraceUpByTypeCompatible(sg_pre, V_SgFunctionDeclaration);

  STEMP(start);
  //  ZP: add the support for crossing function
  if ((cross_func != 0) && (sg_pre != nullptr) && (sg_curr == nullptr)) {
    //  void *sg_func_decl= TraceUpByTypeCompatible(sg_pre,
    //  V_SgFunctionDeclaration);

    int arg_idx = GetFuncParamIndex(sg_name, nullptr);
    if (-1 != arg_idx) {  //  the init_name is func argument
      vector<void *> vec_calls;
      GetFuncCallsFromDecl(sg_func_decl, nullptr, &vec_calls);

      if (vec_calls.size() == 1) {
        void *actual_arg = GetFuncCallParam(vec_calls[0], arg_idx);
        if (IsVarRefExp(actual_arg) != 0) {
          void *new_name = GetVariableInitializedName(actual_arg);

          void *loop =
              GetLoopFromIteratorByPos(new_name, actual_arg, cross_func);

          return loop;
        }
      }
    } else if ((cross_func != 0) &&
               (is_located_in_scope(sg_name, sg_func_decl) ==
                0)) {  //  the init_name
                       //  is not in the
                       //  function
                       //  declaration
      //  void *sg_func_decl= TraceUpByTypeCompatible(sg_pre,
      //  V_SgFunctionDeclaration);
      vector<void *> vec_calls;
      GetFuncCallsFromDecl(sg_func_decl, nullptr, &vec_calls);

      if (vec_calls.size() == 1) {
        return GetLoopFromIteratorByPos(sg_name, vec_calls[0], cross_func);
      }
    }
  }
  TIMEP("GetLoopFromIteratorByPos___cross_function", start, end);

  if ((trace_back != 0) && nullptr == sg_curr &&
      (is_located_in_scope(sg_name, sg_func_decl) != 0)) {
    STEMP(start);
    void *sg_expr =
        GetSingleAssignExpr(sg_name, sg_pos, 0, sg_func_decl, false);
    ACCTM(GetLoopFromIteratorByPos___GetSingleAssignExpr, start, end);
    if ((sg_expr != nullptr) && (IsVarRefExp(sg_expr) != 0)) {
      return GetLoopFromIteratorByPos(GetVariableInitializedName(sg_expr),
                                      sg_expr, cross_func);
    }
  }

  return sg_curr;
}

//  Note: make sure the ref sg_var has the position information
//       Some temporary created ref does not contains position
void *CSageCodeGen::GetLoopFromIteratorRef(void *sg_var) {
  if (IsVarRefExp(sg_var) == 0) {
    return nullptr;
  }

  return GetLoopFromIteratorByPos(GetVariableInitializedName(sg_var), sg_var);
}

void *CSageCodeGen::GetLoopIterator(void *for_loop) {
  void *ivar = nullptr;
  //  int is_canonical = ParseOneForLoop(for_loop, ivar, lb, ub, step, cond_op,
  //  loop_body);

  int is_canonical = IsCanonicalForLoop(for_loop, &ivar);
  if (is_canonical == 0) {
    return nullptr;
  }
  return ivar;
}

void *CSageCodeGen::GetLoopBody(void *for_loop) {
  if (isSgForStatement(static_cast<SgNode *>(for_loop)) != nullptr) {
    return (isSgForStatement(static_cast<SgNode *>(for_loop)))->get_loop_body();
  }
  if (isSgWhileStmt(static_cast<SgNode *>(for_loop)) != nullptr) {
    return (isSgWhileStmt(static_cast<SgNode *>(for_loop)))->get_body();
  }
  if (isSgDoWhileStmt(static_cast<SgNode *>(for_loop)) != nullptr) {
    return (isSgDoWhileStmt(static_cast<SgNode *>(for_loop)))->get_body();
  }

  return nullptr;

  //  if (!isSgForStatement(static_cast<SgNode *>(for_loop))) return nullptr;
  //  return (isSgForStatement(static_cast<SgNode
  //  *>(for_loop)))->get_loop_body();
}
void *CSageCodeGen::GetWhileCondition(void *for_loop) {
  if (isSgWhileStmt(static_cast<SgNode *>(for_loop)) != nullptr) {
    return (isSgWhileStmt(static_cast<SgNode *>(for_loop)))->get_condition();
  }
  if (isSgDoWhileStmt(static_cast<SgNode *>(for_loop)) != nullptr) {
    return (isSgDoWhileStmt(static_cast<SgNode *>(for_loop)))->get_condition();
  }

  return nullptr;

  //  if (!isSgForStatement(static_cast<SgNode *>(for_loop))) return nullptr;
  //  return (isSgForStatement(static_cast<SgNode
  //  *>(for_loop)))->get_loop_body();
}

//  Yuxin: Jun 14 2018
//  Check whether if a for loop has direct sub-loops
//  Not consider loops in function calls
int CSageCodeGen::is_innermost_for_loop(void *for_loop) {
  if (IsForStatement(for_loop) == 0) {
    return 0;
  }
  void *scope = (static_cast<SgForStatement *>(for_loop))->get_loop_body();

  vector<void *> vec_for_stmt;
  GetNodesByType_int(scope, "preorder", V_SgForStatement, &vec_for_stmt);
  if (!vec_for_stmt.empty()) {
    return 0;
  }

  vector<void *> vec_while_loops;
  GetNodesByType_int(scope, "preorder", V_SgWhileStmt, &vec_while_loops);
  if (!vec_while_loops.empty()) {
    return 0;
  }

  vector<void *> vec_doWhile_loops;
  GetNodesByType_int(scope, "preorder", V_SgDoWhileStmt, &vec_doWhile_loops);
  if (!vec_doWhile_loops.empty()) {
    return 0;
  }

  return 1;
}

void *CSageCodeGen::ArrayToPointerRecur(void *sg_type, bool skip_typedef) {
  if (isSgModifierType(static_cast<SgNode *>(sg_type)) != nullptr) {
    SgModifierType *sg_m_type =
        isSgModifierType(static_cast<SgNode *>(sg_type));
    SgTypeModifier orig_modifier = sg_m_type->get_typeModifier();
    SgModifierType *new_sg_m_type =
        SageBuilder::buildModifierType(isSgType(static_cast<SgNode *>(
            ArrayToPointerRecur(sg_m_type->get_base_type(), skip_typedef))));
    //  markBuild(* this, new_sg_m_type, "Build in " + std::to_string(__LINE__)
    //  + " for " + std::to_string((int64_t) new_sg_m_type));
    new_sg_m_type->get_typeModifier() = orig_modifier;
    return new_sg_m_type;
  }
  if ((isSgArrayType(static_cast<SgNode *>(sg_type)) != nullptr) ||
      (isSgPointerType(static_cast<SgNode *>(sg_type)) != nullptr)) {
    void *sg_base = GetBaseTypeOneLayer(sg_type);
    void *new_type = ArrayToPointerRecur(sg_base, skip_typedef);
    auto sg_new_type = SageBuilder::buildPointerType(
        isSgType(static_cast<SgNode *>(new_type)));
    //  markBuild(* this, sg_new_type, "Build in " + std::to_string(__LINE__) +
    //  " for " + std::to_string((int64_t) sg_new_type));
    return sg_new_type;
  }
  if (!skip_typedef &&
      (isSgTypedefType(static_cast<SgNode *>(sg_type)) != nullptr)) {
    SgTypedefType *sg_typedef = isSgTypedefType(static_cast<SgNode *>(sg_type));
    return ArrayToPointerRecur(sg_typedef->get_base_type(), skip_typedef);
  }
  return sg_type;
}

void *CSageCodeGen::GetOrigTypeByTypedef(void *sg_type_,
                                         bool skip_anonymous_type) const {
  if (IsType(sg_type_) == 0) {
    return nullptr;
  }
  if (IsWithLength(this, sg_type_) != 0) {
    return sg_type_;
  }
  if (SgType *sg_type = isSgTypedefType(static_cast<SgNode *>(sg_type_))) {
    void *base_type = isSgTypedefType(sg_type)->get_base_type();
    if (!skip_anonymous_type && (IsAnonymousType(base_type) != 0)) {
      return sg_type;
    }
    return GetOrigTypeByTypedef(base_type, skip_anonymous_type);
  }
  return sg_type_;
}

//  support cross-function, if multi-call, return 0
int CSageCodeGen::get_surrounding_control_stmt(void *sg_pos, void *sg_scope,
                                               vector<void *> *sg_loops_ifs) {
  vector<pair<void *, void *>> sg_loops_ifs_with_last;
  int ret =
      get_surrounding_control_stmt(sg_pos, sg_scope, &sg_loops_ifs_with_last);
  if (ret) {
    for (auto one : sg_loops_ifs_with_last) {
      sg_loops_ifs->push_back(one.first);
    }
  }
  return ret;
}

int CSageCodeGen::get_surrounding_control_stmt(
    void *sg_pos, void *sg_scope, vector<pair<void *, void *>> *sg_loops_ifs) {
  if (sg_scope == nullptr) {
    sg_scope = GetProject();
  }

  list<t_func_call_path> vec_paths;
  int ret = get_call_paths(sg_pos, sg_scope, &vec_paths);

  t_func_call_path path;
  if ((ret != 0) &&
      !vec_paths.empty()) {  //  by default, use the first path, use
                             //  the other function for specific aths
    path = *vec_paths.begin();
  }
  set<void *> visited;
  return get_surrounding_control_stmt(sg_pos, sg_scope, path, sg_loops_ifs,
                                      &visited, true);
}

void *get_second_in_fn_path(const CSageCodeGen &codegen,
                            const t_func_call_path &path, void *first) {
  t_func_call_path::const_iterator it;
  for (it = path.begin(); it != path.end(); it++) {
    if (it->first == first) {
      return it->second;
    }
  }
  return nullptr;
}

//  get the loop nest order for a specific func call path
//  the order is outer scope first - polyhedral model
int CSageCodeGen::get_surrounding_control_stmt(
    void *sg_pos, void *sg_scope, const t_func_call_path &func_path,
    vector<pair<void *, void *>> *sg_loops_ifs, set<void *> *visited,
    bool reverse) {
  if (sg_scope == nullptr) {
    sg_scope = GetProject();
  }

  //  ZP: 20170530
  //  if (IsFunctionDeclaration(sg_scope)) sg_scope = GetFuncBody(sg_scope);

  void *sg_curr = sg_pos;
  int ret = 0;
  void *sg_last = sg_curr;
  while (true) {
    //  ZP: 20160203
    if (IsControlStatement(sg_curr) != 0 &&
        (!IsIfStatement(sg_curr) || GetIfStmtCondition(sg_curr) != sg_last) &&
        (!IsForStatement(sg_curr) || GetLoopInit(sg_curr) != sg_last)) {
      sg_loops_ifs->push_back(std::make_pair(sg_curr, sg_last));
    }
    if (sg_curr == sg_scope) {
      ret = 1;
      break;
    }
    if (sg_curr == nullptr) {
      return 0;
    }
    if (GetFuncName(sg_curr) == "main" && GetProject() == sg_scope) {
      ret = 1;
      break;
    }
    if (GetFuncName(sg_curr) == "main") {
      return 1;
    }

    if (IsFunctionDeclaration(sg_curr) != 0) {
      ret = 2;
      break;
    }
    sg_last = sg_curr;
    sg_curr = GetParent(sg_curr);
  }

  void *sg_decl = sg_curr;
  string sFuncName = GetFuncName(sg_decl);

  if (ret == 2) {
    void *sg_decl = sg_curr;
    if (visited->count(sg_decl) > 0) {
      return 0;
    }
    visited->insert(sg_decl);
    void *sg_call = get_second_in_fn_path(*this, func_path, sg_decl);
    if (sg_call != nullptr) {
      assert(IsFunctionCall(sg_call));

      int ret = get_surrounding_control_stmt(sg_call, sg_scope, func_path,
                                             sg_loops_ifs, visited, false);

      if (ret == 0) {
        return 0;
      }
    }
  }

  if (reverse) {
    std::reverse(sg_loops_ifs->begin(), sg_loops_ifs->end());
  }
  return 1;
}

//  support cross-function, if multi-call, return 0
int CSageCodeGen::get_loop_nest_in_scope(void *sg_pos, void *sg_scope,
                                         vector<void *> *sg_loops) {
  vector<pair<void *, void *>> sg_loops_with_last;
  int ret = get_loop_nest_in_scope(sg_pos, sg_scope, &sg_loops_with_last);
  if (ret) {
    for (auto one : sg_loops_with_last) {
      sg_loops->push_back(one.first);
    }
  }
  return ret;
}

int CSageCodeGen::get_loop_nest_in_scope(
    void *sg_pos, void *sg_scope,
    vector<pair<void *, void *>> *sg_loops_with_last) {
  if (sg_scope == nullptr) {
    sg_scope = GetProject();
  }

  list<t_func_call_path> vec_paths;
  int ret = get_call_paths(sg_pos, sg_scope, &vec_paths);

  t_func_call_path path;
  if ((ret != 0) &&
      !vec_paths.empty()) {  //  by default, use the first path, use
                             //  the other function for specific aths
    path = *vec_paths.begin();
  }

  get_loop_nest_in_scope(sg_pos, sg_scope, path, sg_loops_with_last);
  return 1;
}

int CSageCodeGen::get_call_paths(void *sg_pos, void *sg_scope,
                                 list<t_func_call_path> *vec_paths) {
  set<void *> visited;
  int ret = get_call_paths_int(sg_pos, sg_scope, &visited, vec_paths);

  for (auto &path : *vec_paths) {
    path.emplace_front(nullptr, sg_pos);
    path.emplace_back(sg_scope, nullptr);
  }
  return ret;
}

string CSageCodeGen::print_func_path(const t_func_call_path &fn_path) {
  string str;

  for (auto it = fn_path.begin(); it != fn_path.end(); it++) {
    auto it1 = it;
    it1++;

    str += "'" + UnparseToString(it->second).substr(0, 80) + "'\n";
    if (it1 != fn_path.end()) {
      str += "..\n'" + UnparseToString(it1->first).substr(0, 80) + "'\n ";
    }
#if 1
    cout << UnparseToString(it->second).substr(0, 80) << endl;
    if (it1 != fn_path.end()) {
      cout << "->" << endl;
      cout << UnparseToString(it1->first).substr(0, 80) << endl;
    }
#endif
  }
#if 0
  cout << "current func path: " << endl;
  for (auto it = fn_path.begin(); it != fn_path.end(); it++) {
    cout << (it->first ? _up(it->first) : "null") << ", "
      << (it->second? _up(it->second) : "null") << endl;
  }
#endif
  return str;
}

void CSageCodeGen::get_all_func_path_in_scope(
    void *sg_scope, list<t_func_call_path> *vec_paths) {
  get_all_func_path_in_scope_int(sg_scope, vec_paths);

  for (auto &path : *vec_paths) {
    path.emplace_front(nullptr, nullptr);
    path.emplace_back(sg_scope, nullptr);
  }
}
void CSageCodeGen::get_all_func_path_in_scope_int(
    void *range_scope, list<t_func_call_path> *vec_paths) {
  vec_paths->clear();

  vector<void *> vec_calls;
  GetNodesByType_int_compatible(range_scope, V_SgFunctionCallExp, &vec_calls);

  for (size_t i = 0; i < vec_calls.size(); i++) {
    void *sg_call = vec_calls[i];
    if (is_altera_channel_call(sg_call) != 0) {
      continue;
    }
    void *sg_decl = GetFuncDeclByCall(sg_call);
    void *sg_body = GetFuncBody(sg_decl);
    if ((sg_decl != nullptr) && (sg_body != nullptr)) {
      pair<void *, void *> edge(sg_decl, sg_call);

      list<t_func_call_path> tmp_paths;

      get_all_func_path_in_scope_int(sg_body, &tmp_paths);

      for (auto path : tmp_paths) {
        path.push_back(edge);
        vec_paths->push_back(path);
      }
    }
  }

  if (vec_paths->empty()) {
    t_func_call_path empty_path;
    vec_paths->push_back(empty_path);
  }
}

void CSageCodeGen::get_all_func_path_in_scope(
    void *array, void *sg_scope, list<t_func_call_path> *vec_paths) {
  //  get_all_func_path_in_scope_int(array, sg_scope, vec_paths1);
  set<void *> visited;
  get_all_func_path_in_scope_int_v2(array, sg_scope, vec_paths, &visited, true);

  for (auto &path : *vec_paths) {
    path.emplace_front(nullptr, nullptr);
    path.emplace_back(sg_scope, nullptr);
  }
}

void CSageCodeGen::get_all_func_path_in_scope_int_v2(
    void *array, void *range_scope, list<t_func_call_path> *vec_paths,
    set<void *> *visited, bool top) {
  vec_paths->clear();
  if (visited->count(array) > 0) {
    return;
  }
  visited->insert(array);
  void *sg_base_type;
  vector<size_t> sg_sizes;
  get_type_dimension(GetTypebyVar(array), &sg_base_type, &sg_sizes, array);
  //  int dim_size = sg_sizes.size();
  DEFINE_START_END;

  STEMP(start);
  vector<void *> vec_refs;
  get_ref_in_scope(array, range_scope, &vec_refs);
  list<t_func_call_path> top_call_path;
  set<void *> top_visited;
  bool add_top_call_path = false;
  if (top) {
    get_call_paths_int(array, range_scope, &top_visited, &top_call_path);
  }
  TIMEP("    get_ref_in_scope", start, end);

  for (auto ref : vec_refs) {
    void *sg_array;
    void *sg_pntr;
    vector<void *> sg_indexes;

    int pointer_dim;
    STEMP(start);
    parse_pntr_ref_by_array_ref(ref, &sg_array, &sg_pntr, &sg_indexes,
                                &pointer_dim);
    TIMEP("    parse_pntr_ref_by_array_ref", start, end);

    if (pointer_dim < 0) {
      continue;
    }
    int arg_idx = GetFuncCallParamIndex(sg_pntr);

    STEMP(start);
    if (arg_idx != -1) {
      void *sg_call = TraceUpToFuncCall(sg_pntr);
      void *sg_decl = GetFuncDeclByCall(sg_call);
      void *sg_body = GetFuncBody(sg_decl);
      if ((sg_decl != nullptr) && (sg_body != nullptr)) {
        void *new_array = GetFuncParam(sg_decl, arg_idx);
        pair<void *, void *> edge(sg_decl, sg_call);
        //  Youxiang 20180627 do not analysis the implementation of wide bus
        if (GetFuncName(sg_decl).find("memcpy_wide_bus_") == 0) {
          t_func_call_path path;
          path.push_back(edge);
          vec_paths->push_back(path);
          continue;
        }
        list<t_func_call_path> tmp_paths;

        get_all_func_path_in_scope_int_v2(new_array, sg_body, &tmp_paths,
                                          visited, false);

        for (auto path : tmp_paths) {
          path.push_back(edge);
          if (top && !top_call_path.empty()) {
            for (auto top_path : top_call_path) {
              t_func_call_path full_path = path;
              full_path.insert(full_path.end(), top_path.begin(),
                               top_path.end());
              vec_paths->push_back(full_path);
            }
          } else {
            vec_paths->push_back(path);
          }
        }
      }
      continue;
    }
    TIMEP("    arg_index_1", start, end);

    //  if (pointer_dim < dim_size)
    {
      //  support simple pointer alias
      STEMP(start);
      void *var_name;
      void *value_exp;
      if (parse_assign(GetParent(sg_pntr), &var_name, &value_exp) != 0) {
        if (value_exp == sg_pntr && (IsInitName(var_name) != 0)) {
          void *new_array = var_name;
          list<t_func_call_path> tmp_paths;
          get_all_func_path_in_scope_int_v2(new_array, range_scope, &tmp_paths,
                                            visited, top);
          if (tmp_paths.size() != 1 || !(*tmp_paths.begin()).empty()) {
            vec_paths->splice(vec_paths->end(), tmp_paths, tmp_paths.begin(),
                              tmp_paths.end());
          }
        }
        continue;
      }
      TIMEP("    pointer_alias", start, end);
    }
    if (top && !add_top_call_path) {
      vec_paths->insert(vec_paths->end(), top_call_path.begin(),
                        top_call_path.end());
      add_top_call_path = true;
    }
  }

  if (vec_paths->empty()) {
    t_func_call_path empty_path;
    vec_paths->push_back(empty_path);
  }
  visited->erase(array);
}
//  each map is a possible path, there are multiple paths because there multiple
//  calls of a func
//  map<func_decl, func_call>
int CSageCodeGen::get_call_paths_int(void *sg_pos, void *sg_scope,
                                     set<void *> *visited,
                                     list<t_func_call_path> *vec_paths) {
  if (sg_scope == nullptr) {
    sg_scope = GetProject();
  }
  //  1. trace up to the function until main() or sg_scope
  void *sg_curr = sg_pos;

  vector<void *> vec_calls;
  void *sg_decl = nullptr;
  while (true) {
    if (sg_curr == sg_scope) {
      t_func_call_path empty_map;
      if (sg_scope == GetProject()) {
        empty_map.push_back(pair<void *, void *>(sg_decl, nullptr));
      }
      vec_paths->push_back(empty_map);
      return 1;
    }
    if (sg_curr == nullptr) {
      return 0;
    }
    if (IsFunctionDeclaration(sg_curr) != 0) {
      if (visited->count(sg_curr) > 0) {
        // found a recursive function
        return 0;
      }
      sg_decl = sg_curr;

      GetFuncCallsFromDecl(sg_decl, nullptr, &vec_calls);

      if (!vec_calls.empty()) {
        break;
      }
    }
    sg_curr = GetParent(sg_curr);
  }

  // insert function declaration to avoid recursive function declaration
  if (nullptr != sg_decl) {
    visited->insert(sg_decl);
  }
  int ret = 0;
  for (size_t i = 0; i < vec_calls.size(); i++) {
    void *sg_call = vec_calls[i];
    list<t_func_call_path> vec_org_paths;
    int sub_ret =
        get_call_paths_int(sg_call, sg_scope, visited, &vec_org_paths);

    if (sub_ret != 0) {
      ret = sub_ret;
      if (vec_org_paths.empty()) {
        assert(0);
      } else {
        for (auto path : vec_org_paths) {
          path.emplace_front(sg_decl, sg_call);
          vec_paths->push_back(path);
        }
      }
    }
  }
  if (nullptr != sg_decl) {
    visited->erase(sg_decl);
  }
  if (vec_paths->empty()) {
    t_func_call_path empty_map;
    vec_paths->push_back(empty_map);
  }

#if 0
    {
        printf("[Get Path] pos=%s scope=%s \n",
                UnparseToString(sg_pos).c_str(),
                UnparseToString(sg_scope).c_str());
        int i = 0;
        for (auto path : *vec_paths) {
            printf("  Path %d  \n", i++);
            for (auto it = path.begin(); it != path.end(); it++) {
                printf("    %s -> %s\n",
                        UnparseToString(it->first).c_str(),
                        UnparseToString(it->second).c_str());
            }
        }
    }

#endif
  return ret;
}

int CSageCodeGen::is_located_in_scope(void *sg_pos, void *sg_scope) {
  if (s_location_cache.count(std::make_pair(sg_pos, sg_scope)) > 0) {
    return static_cast<int>(s_location_cache[std::make_pair(sg_pos, sg_scope)]);
  }
  if (is_floating_node(sg_pos) != 0) {
    return 0;
  }
  if (sg_scope == nullptr || sg_pos == sg_scope) {
    return 1;
  }
  if (is_located_in_scope_simple(sg_pos, sg_scope) != 0) {
    return 1;
  }
  if (is_located_in_scope_simple(sg_scope, sg_pos) != 0) {
    return 0;
  }

  list<t_func_call_path> vec_paths;
  int ret = get_call_paths(sg_pos, sg_scope, &vec_paths);
  t_func_call_path path;

//  ///////////////////////  /
//  ZP: 20160225
#if 0
  if (ret && vec_paths.size() >= 1) {
    //  by default, use the first path, use the
    //  other function for specific aths
    path = vec_paths[0];
  }
  ret = is_located_in_scope(sg_pos, sg_scope, path);
#else
  int res = 0;
  if (ret == 0 || vec_paths.empty()) {
    res = is_located_in_scope(sg_pos, sg_scope, path);
  } else {
    res = is_located_in_scope(sg_pos, sg_scope, vec_paths);
  }
  s_location_cache[std::make_pair(sg_pos, sg_scope)] = (res != 0);
  return res;
#endif
}

int CSageCodeGen::is_located_in_scope(void *sg_pos, void *sg_scope,
                                      const list<t_func_call_path> &vec_paths) {
  for (auto path : vec_paths) {
    if (is_located_in_scope(sg_pos, sg_scope, path) != 0) {
      return 1;
    }
  }
  return 0;
}

int CSageCodeGen::is_located_in_path_v2(void *ref, t_func_call_path fn_path) {
  //  The structure of a path
  //  nullptr->org_pos, decl->call, decl_>call, org_scope->nullptr
  //  1. if it was in a call and the callee is not nullptr,
  //    determine if sg_pos is in any call
  //  2. otherwise, determine whether if sg_pos is in org_scope or decl_
  void *sg_pntr = nullptr;
  int pointer_dim = 0;
  if (IsVarRefExp(ref) != 0) {
    void *sg_array = nullptr;
    vector<void *> sg_indexes;
    parse_pntr_ref_by_array_ref(ref, &sg_array, &sg_pntr, &sg_indexes,
                                &pointer_dim);
  } else {
    sg_pntr = ref;
  }
  bool is_in_call = false;
  if (pointer_dim >= 0) {
    int arg_idx = GetFuncCallParamIndex(sg_pntr);
    void *call = (arg_idx != -1) ? TraceUpToFuncCall(sg_pntr) : nullptr;
    void *callee = call != nullptr ? GetFuncDeclByCall(call) : nullptr;
    is_in_call = callee != nullptr;
  }
  t_func_call_path::iterator it = fn_path.begin();
  t_func_call_path::iterator it1 = it;
  it1++;
  for (; it != fn_path.end(); it++, it1++) {
    if (it1 == fn_path.end()) {
      break;
    }

    if (it->second == nullptr) {
      continue;
    }
#ifdef PROJDEBUG
    assert(!it1->first || is_located_in_scope_simple(it->second, it1->first));
#endif
    if (is_located_in_scope_simple(ref, it->second) != 0) {
      return 1;
    }
  }
  if (!is_in_call) {
    it = fn_path.begin();
    for (; it != fn_path.end(); it++) {
      if (it->first == nullptr) {
        continue;
      }
      if (is_located_in_scope_simple(ref, it->first) != 0) {
        return 1;
      }
    }
  }

  return 0;
}

int CSageCodeGen::is_located_in_path(void *sg_pos, t_func_call_path fn_path) {
  if (fn_path.empty()) {
    return 1;
  }
  if (sg_pos == m_sg_project || sg_pos == nullptr) {
    return 1;
  }
  void *new_pos = sg_pos;
  if (-1 != GetFuncParamIndex(sg_pos)) {
    new_pos = TraceUpToFuncDecl(sg_pos);
  }
  if (IsFunctionDeclaration(new_pos) != 0) {
    for (auto func_call : fn_path) {
      if (func_call.first == sg_pos) {
        return 1;
      }
    }
    // call path may contain sg_project
  }
  //  The structure of a path
  //  nullptr->org_pos, decl->call, decl_>call, org_scope->nullptr
  //  1. if it was in a call and the callee is not nullptr,
  //    determine if sg_pos is in any call
  //  2. otherwise, determine whether if sg_pos is in org_scope or decl_

  //  //////////////////////////////////////////////////////////////////////  /

  void *call = TraceUpToFuncCall(new_pos);
  void *callee = nullptr;
  if (call != nullptr) {
    callee = GetFuncDeclByCall(call);
  }
  bool is_in_call = callee != nullptr;
  t_func_call_path::iterator it = fn_path.begin();
  t_func_call_path::iterator it1 = it;
  it1++;
  bool has_call = false;
  for (; it != fn_path.end(); it++, it1++) {
    if (it->second == nullptr) {
      continue;
    }
    has_call |= IsFunctionCall(it->second);
#ifdef PROJDEBUG
    if (it1 != fn_path.end()) {
      assert(!it1->first || is_located_in_scope_simple(it->second, it1->first));
    }
#endif
    if (is_located_in_scope_simple(new_pos, it->second) != 0) {
      return 1;
    }
  }
  if (!is_in_call || !has_call) {
    it = fn_path.begin();
    for (; it != fn_path.end(); it++) {
      if (it->first == nullptr) {
        continue;
      }
      if (is_located_in_scope_simple(new_pos, it->first) != 0) {
        return 1;
      }
    }
  }

  return 0;
}

int CSageCodeGen::is_included_in_path(const pair<void *, void *> &call_site,
                                      t_func_call_path fn_path) {
  //  The structure of a path
  //  nullptr->org_pos, decl->call, decl_>call, org_scope->nullptr
  //  This function determine if decl->call is any (it->first->it->second)

  t_func_call_path::iterator it = fn_path.begin();
  for (; it != fn_path.end(); it++) {
    if (*it == call_site) {
      return 1;
    }
  }
  return 0;
}

int CSageCodeGen::any_var_located_in_scope(void *term, void *scope) {
  if (IsForStatement(term) != 0) {
    if (is_located_in_scope_simple(term, scope) != 0) {
      return 1;
    }
  }
  if (!is_floating_node(term)) {
    if (is_located_in_scope(term, scope) == 0) {
      return 0;
    } else if (IsInitializer(term) || IsInitName(term)) {
      return 1;
    }
  }
  if (IsExpression(term) != 0) {
    if (has_side_effect(term) != 0) {
      return 1;
    }
    vector<void *> vec_refs;
    GetNodesByType_int(term, "preorder", V_SgVarRefExp, &vec_refs);
    for (auto ref : vec_refs) {
      void *var_init = GetVariableInitializedName(ref);
      if ((is_located_in_scope(var_init, scope) != 0) ||
          (has_write_in_scope_fast(var_init, scope) != 0)) {
        return 1;
      }
    }
    return 0;
  }
  return 0;
}

int CSageCodeGen::is_located_in_scope(void *sg_pos, void *sg_scope,
                                      const t_func_call_path &func_path) {
  if (is_floating_node(sg_pos) != 0) {
    return 0;
  }
  if (sg_scope == nullptr) {
    return 1;
  }
  void *sg_curr = sg_pos;

  int ret = 0;
  while (true) {
    if (sg_curr == sg_scope) {
      ret = 1;
      break;
    }
    if (sg_curr == nullptr) {
      //  ZP : 20160303
      if (GetProject() == sg_scope) {
        return 1;
      }
      { return 0; }
    }
    if (GetFuncName(sg_curr) == "main" && GetProject() == sg_scope) {
      ret = 1;
      break;
    }

    if (IsFunctionDeclaration(sg_curr) != 0) {
      ret = 2;
      break;
    }

    sg_curr = GetParent(sg_curr);
  }

  void *sg_decl = sg_curr;
  string sFuncName = GetFuncName(sg_decl);

  if (ret == 2) {
    void *sg_decl = sg_curr;
    void *sg_call = get_second_in_fn_path(*this, func_path,
                                          sg_decl);  //  func_path[sg_decl];
    if (sg_call != nullptr) {
      assert(IsFunctionCall(sg_call));

      int ret = is_located_in_scope(sg_call, sg_scope, func_path);

      return ret;
    }
    //  ZP : 20160303
    if (GetProject() == sg_scope) {
      return 1;
    }
    { return 0; }
  }

  return ret;
}

int CSageCodeGen::is_located_in_scope_simple(void *sg_pos, void *sg_scope) {
  void *sg_curr = sg_pos;
  int ret = 0;
  while (sg_curr != nullptr) {
    if (sg_curr == sg_scope) {
      ret = 1;
      break;
    }
    sg_curr = GetParent(sg_curr);
  }
  return ret;
}

//  get the loop nest order for a specific func call path
int CSageCodeGen::get_loop_nest_in_scope(void *sg_pos, void *sg_scope,
                                         const t_func_call_path &func_path,
                                         vector<void *> *sg_loops) {
  vector<pair<void *, void *>> sg_loops_with_last;
  int ret =
      get_loop_nest_in_scope(sg_pos, sg_scope, func_path, &sg_loops_with_last);
  if (ret) {
    for (auto one : sg_loops_with_last) {
      sg_loops->push_back(one.first);
    }
  }
  return ret;
}

int CSageCodeGen::get_loop_nest_in_scope(
    void *sg_pos, void *sg_scope, const t_func_call_path &func_path,
    vector<pair<void *, void *>> *sg_loops_with_last) {
  set<void *> visited;
  return get_loop_nest_in_scope(sg_pos, sg_scope, func_path, sg_loops_with_last,
                                &visited, true);
}

int CSageCodeGen::get_loop_nest_in_scope(
    void *sg_pos, void *sg_scope, const t_func_call_path &func_path,
    vector<pair<void *, void *>> *sg_loops_with_last, set<void *> *visited,
    bool reverse) {
  if (sg_scope == nullptr) {
    sg_scope = GetProject();
  }

  void *sg_curr = sg_pos;
  int ret = 0;
  while (true) {
    //  ZP: 20160203
    if (IsForStatement(sg_curr) != 0 || IsWhileStatement(sg_curr) != 0 ||
        IsDoWhileStatement(sg_curr) != 0) {
      sg_loops_with_last->push_back(std::make_pair(sg_curr, sg_pos));
    }
    if (sg_curr == sg_scope) {
      ret = 1;
      break;
    }
    if (sg_curr == nullptr) {
      return 0;
    }
    if (GetFuncName(sg_curr) == "main" && GetProject() == sg_scope) {
      ret = 1;
      break;
    }
    if (GetFuncName(sg_curr) == "main") {
      return 1;
    }

    if (IsFunctionDeclaration(sg_curr) != 0) {
      ret = 2;
      break;
    }
    sg_curr = GetParent(sg_curr);
  }

  void *sg_decl = sg_curr;
  string sFuncName = GetFuncName(sg_decl);

  if (ret == 2) {
    void *sg_decl = sg_curr;
    if (visited->count(sg_decl) > 0) {
      return 0;
    }
    visited->insert(sg_decl);
    void *sg_call = get_second_in_fn_path(*this, func_path, sg_decl);
    if (sg_call != nullptr) {
      assert(IsFunctionCall(sg_call));

      int ret = get_loop_nest_in_scope(sg_call, sg_scope, func_path,
                                       sg_loops_with_last, visited, false);

      if (ret == 0) {
        return 0;
      }
    }
    visited->erase(sg_decl);
  }

  if (reverse) {
    std::reverse(sg_loops_with_last->begin(), sg_loops_with_last->end());
  }
  return 1;
}

int CSageCodeGen::IsLoopInit(void *sg_stmt) {
  if (sg_stmt == nullptr) {
    return 0;
  }
  if (isSgForInitStatement(static_cast<SgNode *>(sg_stmt)) != nullptr) {
    return 1;
  }
  void *parent = GetParent(sg_stmt);
  if (isSgForInitStatement(static_cast<SgNode *>(parent)) != nullptr) {
    return 1;
  }
  return 0;
}

int CSageCodeGen::IsLoopTest(void *sg_stmt) {
  if (sg_stmt == nullptr) {
    return 0;
  }
  void *sg_loop = TraceUpToForStatement(sg_stmt);
  if ((sg_loop != nullptr) && GetLoopTest(sg_loop) == sg_stmt) {
    return 1;
  }
  return 0;
}

#if 0
int CSageCodeGen::IsLoopBody(void *sg_stmt) {
  if (!sg_stmt)
    return 0;
  void *sg_loop = TraceUpToLoopScope(sg_stmt);
  if (sg_loop && GetLoopBody(sg_loop) == sg_stmt)
    return 1;
  return 0;
}
#endif

void *CSageCodeGen::GetLoopInit(void *sg_for) {
  SgForStatement *target_loop = isSgForStatement(static_cast<SgNode *>(sg_for));
  if (target_loop == nullptr)
    return nullptr;
  SgForInitStatement *init_stmt = target_loop->get_for_init_stmt();

  return init_stmt;
}

void *CSageCodeGen::GetLoopTest(void *sg_loop) {
  if (SgForStatement *target_loop =
          isSgForStatement(static_cast<SgNode *>(sg_loop))) {
    return target_loop->get_test();
  }
  if (SgDoWhileStmt *target_loop =
          isSgDoWhileStmt(static_cast<SgNode *>(sg_loop))) {
    return target_loop->get_condition();
  }
  if (SgWhileStmt *target_loop =
          isSgWhileStmt(static_cast<SgNode *>(sg_loop))) {
    return target_loop->get_condition();
  }
  return nullptr;
}

void *CSageCodeGen::GetLoopIncrementExpr(void *sg_for) {
  SgForStatement *target_loop = isSgForStatement(static_cast<SgNode *>(sg_for));

  SgExpression *step_exp = target_loop->get_increment();

  return step_exp;
}

int CSageCodeGen::GetLoopStep(void *loop, int64_t *nStep) {
  void *iv;
  void *lb;
  void *ub;
  void *step;
  void *body;
  iv = lb = ub = step = body = nullptr;
  bool inc_dir;
  bool inclusive_bound;
  inc_dir = inclusive_bound = false;
  if (IsCanonicalForLoop(loop, &iv, &lb, &ub, &step, &body, &inc_dir,
                         &inclusive_bound) != 0) {
    int ret = GetLoopStepValueFromExpr(step, nStep);
    if (step != nullptr && GetParent(step) == nullptr) {
      DeleteNode(step);
    }
    return ret;
  }
  return 0;
}

int CSageCodeGen::ParseForLoop(SgForStatement *target_loop,
                               SgInitializedName **ivar, SgExpression **lb,
                               SgExpression **ub, SgExpression **step,
                               SgBinaryOp **cond_op, SgStatement **body,
                               int *ub_limit) {
  //  Check for loop Canonical
  // TODO(yuxing) ...

  //  Get IV
  SgForInitStatement *init_stmt = target_loop->get_for_init_stmt();
  SgExprStatement *init_exp_stmt =
      isSgExprStatement(init_stmt->get_init_stmt()[0]);
  SgVariableDeclaration *init_decl_stmt =
      isSgVariableDeclaration(init_stmt->get_init_stmt()[0]);
  if (init_decl_stmt != nullptr) {
    SgInitializedName *init_name =
        isSgInitializedName(init_decl_stmt->get_variables()[0]);
    //  ROSE_ASSERT(init_name);
    if (init_name == nullptr) {
      return 0;
    }
    SgAssignInitializer *init_initializer =
        isSgAssignInitializer(init_name->get_initializer());
    if (init_initializer == nullptr) {
      return 0;
    }
    SgExpression *ivar_exp = isSgExpression(init_initializer->get_operand());
    //  ROSE_ASSERT(ivar_exp);
    if (ivar_exp == nullptr) {
      return 0;
    }
    *ivar = init_name;

    //  Get Lower Bound (initial)
    *lb = ivar_exp;
  } else if (init_exp_stmt != nullptr) {
    //  ROSE_ASSERT(init_exp_stmt);
    SgAssignOp *init_assign_op = isSgAssignOp(init_exp_stmt->get_expression());
    //  ROSE_ASSERT(init_assign_op);
    if (init_assign_op == nullptr) {
      return 0;
    }
    SgVarRefExp *ivar_exp = isSgVarRefExp(init_assign_op->get_lhs_operand_i());
    //  ROSE_ASSERT(ivar_exp);
    if (ivar_exp == nullptr) {
      return 0;
    }
    *ivar = ivar_exp->get_symbol()->get_declaration();

    //  Get Lower Bound (initial)
    *lb = isSgExpression(init_assign_op->get_rhs_operand_i());
  }
  //  e.g. for(;;) {}
  //  ROSE_ASSERT(*ivar);
  //  ROSE_ASSERT(*lb);

  //  Get Upper Bound (limit)
  SgStatement *cond_stmt = target_loop->get_test();
  if (cond_stmt != nullptr) {
    //  ROSE_ASSERT(cond_exp_stmt);
    SgExprStatement *cond_exp_stmt = isSgExprStatement(cond_stmt);
    if (cond_exp_stmt == nullptr) {
      return 0;
    }
    SgExpression *cond_exp = cond_exp_stmt->get_expression();
    while (isSgCastExp(cond_exp) != nullptr) {
      cond_exp = isSgCastExp(cond_exp)->get_operand();
    }
    SgBinaryOp *bin_op = isSgBinaryOp(cond_exp);
    //  ROSE_ASSERT(bin_op);
    if (bin_op == nullptr) {
      return 0;
    }
    SgExpression *limit_exp = isSgExpression(bin_op->get_rhs_operand_i());
    //  ROSE_ASSERT(limit_exp);
    if (limit_exp == nullptr) {
      return 0;
    }
    *ub = limit_exp;

    //  FIXME: ZP: 2015-05-04: commented because bin_op is passed outside, the
    //  expression can be analyzed outside where may be easier to handle the
    //  "+1"
    //  //  Condition operator: < or <=
    //  if(isSgLessOrEqualOp(bin_op))
    //  {
    //    printf("\n[source code] ERROR: Do not support 'i <= ...' in loop bound
    //    -
    //  modify to 'i < ...+1'.\n\n");
    //    exit(1);
    //  }
    //  FIXME: Condition operator: < or <=. This constraint was used in
    //  codegen-> Yuxin April 15, 2015
    if (isSgLessOrEqualOp(bin_op) != nullptr) {
      //        printf("\n[source code] ERROR: Do not support 'i <= ...'
      //  in loop bound - modify to 'i < ...+1'.\n\n");
      //        exit(1);
      *ub_limit = 0;
    } else if (isSgLessThanOp(bin_op) != nullptr) {
      *ub_limit = -1;
    }

    //  ROSE_ASSERT(isSgLessThanOp(bin_op));
    //  ROSE_ASSERT(isSgLessThanOp(bin_op) || isSgLessOrEqualOp(bin_op));
    *cond_op = bin_op;
  }
  //  Step: 1 or -1
  SgExpression *step_exp = target_loop->get_increment();
  //  FIXME: ZP: 2015-05-04: can also be assign Op
  //  ROSE_ASSERT(isSgPlusPlusOp(step_exp) || isSgMinusMinusOp(step_exp) ||
  //        isSgPlusAssignOp(step_exp) || isSgMinusAssignOp(step_exp));
  *step = step_exp;

  //  Body:
  *body = target_loop->get_loop_body();

  return static_cast<int>((static_cast<int>(*ivar != nullptr) != 0) &&
                          (*lb != nullptr) && (*ub != nullptr) &&
                          (*step) != nullptr);
}

//  This function copies the sg node in the list
void *CSageCodeGen::CreateAndOpFromExprList(vector<void *> vec_expr,
                                            void *bindNode /*= nullptr*/) {
  if (vec_expr.empty()) {
    return nullptr;
  }
  void *curr_exp = vec_expr[0];

  if (isSgExpression(static_cast<SgNode *>(curr_exp)) == nullptr) {
    return nullptr;
  }

  void *out_exp = CopyExp(curr_exp);

  for (size_t i = 1; i < vec_expr.size(); i++) {
    if (isSgExpression(static_cast<SgNode *>(vec_expr[i])) != nullptr) {
      out_exp =
          CreateExp(V_SgAndOp, out_exp, CopyExp(vec_expr[i]), 0, bindNode);
    } else {
      return nullptr;
    }
  }

  return out_exp;
}

void CSageCodeGen::GetExprList(void *sg_exp, vector<void *> *vec_expr) {
  SgExprListExp *sg_expr_list = isSgExprListExp(static_cast<SgNode *>(sg_exp));
  if (sg_expr_list == nullptr) {
    vec_expr->push_back(sg_expr_list);
    return;
  }
  for (auto sub_exp : sg_expr_list->get_expressions()) {
    vec_expr->push_back(sub_exp);
  }
}

void CSageCodeGen::GetExprListFromAndOp(void *sg_comma,
                                        vector<void *> *vec_expr) {
  if (isSgAndOp(static_cast<SgNode *>(sg_comma)) == nullptr) {
    vec_expr->push_back(sg_comma);
    return;
  }
  void *left = GetExpLeftOperand(sg_comma);
  void *right = GetExpRightOperand(sg_comma);

  if (isSgAndOp(static_cast<SgNode *>(left)) != nullptr) {
    GetExprListFromAndOp(left, vec_expr);
  } else {
    vec_expr->push_back(left);
  }

  if (isSgAndOp(static_cast<SgNode *>(right)) != nullptr) {
    GetExprListFromAndOp(right, vec_expr);
  } else {
    vec_expr->push_back(right);
  }
}

void CSageCodeGen::GetExprListFromOrOp(void *sg_comma,
                                       vector<void *> *vec_expr) {
  if (isSgOrOp(static_cast<SgNode *>(sg_comma)) == nullptr) {
    vec_expr->push_back(sg_comma);
    return;
  }

  void *left = GetExpLeftOperand(sg_comma);
  void *right = GetExpRightOperand(sg_comma);

  if (isSgOrOp(static_cast<SgNode *>(left)) != nullptr) {
    GetExprListFromOrOp(left, vec_expr);
  } else {
    vec_expr->push_back(left);
  }

  if (isSgOrOp(static_cast<SgNode *>(right)) != nullptr) {
    GetExprListFromOrOp(right, vec_expr);
  } else {
    vec_expr->push_back(right);
  }
}

int CSageCodeGen::GetExprListFromCommaOp(void *sg_comma,
                                         vector<void *> *vec_expr) {
  void *left = nullptr;
  void *right = nullptr;
  int op = 0;
  if (isSgCommaOpExp(static_cast<SgNode *>(sg_comma)) != nullptr) {
    GetExpOperand(sg_comma, &left, &right);
  } else if (IsAPIntOp(sg_comma, &left, &right, &op) && V_SgCommaOpExp == op) {
  } else {
    return 0;
  }

  if (GetExprListFromCommaOp(left, vec_expr) == 0) {
    vec_expr->push_back(left);
  }

  if (GetExprListFromCommaOp(right, vec_expr) == 0) {
    vec_expr->push_back(right);
  }
  return 1;
}

//  Get the bounds of scalar variable from expression
//  Conservative
//  map: var -> expr (bound)
int CSageCodeGen::GetVarRangeFromCompareOp(void *expr, map<void *, void *> *lbs,
                                           map<void *, void *> *ubs,
                                           bool true_value) {
  void *sg_left;
  void *sg_right;
  sg_left = sg_right = nullptr;
  int op;
  if (IsBinaryOp(expr) != 0) {
    GetExpOperand(expr, &sg_left, &sg_right);
    op = (static_cast<SgNode *>(expr))->variantT();
    if (IsCompareOp(op) == 0 || IsIntegerType(GetTypeByExp(sg_left)) == 0) {
      return 0;
    }
  } else if (IsAPIntOp(expr, &sg_left, &sg_right, &op) &&
             (IsCompareOp(op) != 0)) {
  } else {
    return 0;
  }
  assert(sg_left && sg_right);

  //  ////////////////////////////////////////////////////////////  /
  //  Analyze the bound using linearization function
  //  1. convert to expr >= 0 form
  //  2. for each term, check if a var (VarRefExp and constant coefficient)
  //  3. if coeff>0, var >= var-expr/coeff, else var <= var-expr/coeff
  //  ////////////////////////////////////////////////////////////  /

  //  1. convert to expr >= 0 form
  {
    CMarsExpression me_left(sg_left, this);
    CMarsExpression me_right(sg_right, this);

    CMarsExpression me_norm = me_left - me_right;
    if (V_SgLessOrEqualOp == op) {
      if (true_value) {
        me_norm = -me_norm;
      } else {
        me_norm = me_norm + 1;
      }
    } else if (V_SgLessThanOp == op) {
      if (true_value) {
        me_norm = -me_norm - 1;
      }
    } else if (V_SgGreaterThanOp == op) {
      if (true_value) {
        me_norm = me_norm - 1;
      } else {
        me_norm = -me_norm;
      }
    } else if (V_SgGreaterOrEqualOp == op) {
      if (!true_value) {
        me_norm = -me_norm - 1;
      }
    } else if ((V_SgEqualityOp == op && true_value) ||
               (V_SgNotEqualOp == op && !true_value)) {
    } else {
      return 0;
    }

    me_norm.ParseVarBoundSimple(lbs, ubs);

    if ((V_SgEqualityOp == op && true_value) ||
        (V_SgNotEqualOp == op && !true_value)) {
      (-me_norm).ParseVarBoundSimple(lbs, ubs);
    }
  }

  return 1;
}

int CSageCodeGen::IsIncrementOp(void *sg_node) {
  SgNode *sg_expr = static_cast<SgNode *>(sg_node);
  return IsIncrementOp(sg_expr->variantT());
}

int CSageCodeGen::IsIncrementOp(int op_code) {
  return static_cast<int>(
      V_SgPlusAssignOp == op_code || V_SgMinusAssignOp == op_code ||
      V_SgPlusPlusOp == op_code || V_SgMinusMinusOp == op_code);
}

int CSageCodeGen::IsCompareOp(void *sg_node) {
  int opcode = (static_cast<SgNode *>(sg_node))->variantT();
  return IsCompareOp(opcode);
}

int CSageCodeGen::IsCompareOp(int opcode) {
  return static_cast<int>(
      (V_SgLessOrEqualOp == opcode) || (V_SgLessThanOp == opcode) ||
      (V_SgGreaterOrEqualOp == opcode) || (V_SgEqualityOp == opcode) ||
      (V_SgNotEqualOp == opcode) || (V_SgGreaterThanOp == opcode));
}

int CSageCodeGen::GetCompareOp(void *sg_exp) {
  if (IsLessOrEqualOp(sg_exp) != 0) {
    return V_SgLessOrEqualOp;
  }
  if (IsLessThanOp(sg_exp) != 0) {
    return V_SgLessThanOp;
  }
  if (IsGreaterOrEqualOp(sg_exp) != 0) {
    return V_SgGreaterOrEqualOp;
  }
  if (IsGreaterThanOp(sg_exp) != 0) {
    return V_SgGreaterThanOp;
  }
  if (IsEqualOp(sg_exp) != 0) {
    return V_SgEqualityOp;
  }
  if (IsNonEqualOp(sg_exp) != 0) {
    return V_SgNotEqualOp;
  }
  assert(0);
  return -1;
}

void *CSageCodeGen::GetInitializer(void *sg_name) {
  if (IsGlobalInitName(sg_name) != 0) {
    sg_name = GetVariableDefinitionByName(sg_name);
  }
  SgInitializedName *name = isSgInitializedName(static_cast<SgNode *>(sg_name));
  if (name == nullptr) {
    return nullptr;
  }
  return name->get_initializer();
}

void *CSageCodeGen::GetInitializerOperand(void *sg_input) {
  SgAssignInitializer *assign_init =
      isSgAssignInitializer(static_cast<SgNode *>(sg_input));
  if (assign_init != nullptr) {
    return assign_init->get_operand();
  }
  if (SgConstructorInitializer *contor_init =
          isSgConstructorInitializer(static_cast<SgNode *>(sg_input))) {
    return contor_init;
  }
  SgAggregateInitializer *aggr_init =
      isSgAggregateInitializer(static_cast<SgNode *>(sg_input));
  if (aggr_init != nullptr) {
    return aggr_init;  //  ZP: FIXME: to be changed into 'return value' instead
  }
  //  of return the initializer

  SgInitializedName *name =
      isSgInitializedName(static_cast<SgNode *>(sg_input));
  if (name != nullptr) {
    return GetInitializerOperand(GetInitializer(sg_input));
  }

  if (void *res = GetAggrInitializerFromCompoundLiteralExp(sg_input)) {
    return res;
  }

  return nullptr;
}

void *CSageCodeGen::GetAggrInitializerFromCompoundLiteralExp(void *sg_exp) {
  if (IsCompoundLiteralExp(sg_exp) == 0) {
    return nullptr;
  }
  //  Yuxin: 11/17/2017, get aggregate initializer from SgCompoundLiteralExp
  SgCompoundLiteralExp *sg_compound =
      isSgCompoundLiteralExp(static_cast<SgNode *>(sg_exp));
  if (sg_compound != nullptr) {
    SgVariableSymbol *sg_symbol =
        static_cast<SgVariableSymbol *>(sg_compound->get_symbol());
    SgInitializedName *init = sg_symbol->get_declaration();
    SgAggregateInitializer *aggr_init1 =
        isSgAggregateInitializer(init->get_initptr());
    if (aggr_init1 != nullptr) {
      return aggr_init1;
    }
  }
  return nullptr;
}

void CSageCodeGen::SetInitializer(void *sg_name, void *sg_init) {
  SgInitializedName *name = isSgInitializedName(static_cast<SgNode *>(sg_name));
  if (name == nullptr) {
    return;
  }
  if ((isSgInitializer(static_cast<SgNode *>(sg_init)) != nullptr) ||
      sg_init == nullptr) {
    name->set_initializer(isSgInitializer(static_cast<SgNode *>(sg_init)));
  }
}

int CSageCodeGen::IsSimpleAggregateInitializer(void *sg_initer_) {
  SgAggregateInitializer *sg_initer =
      isSgAggregateInitializer(static_cast<SgNode *>(sg_initer_));

  if (sg_initer == nullptr) {
    return 0;
  }
  if (sg_initer->get_initializers()->get_expressions().size() != 1) {
    return 0;
  }

  void *init_exp = sg_initer->get_initializers()->get_expressions()[0];

  if (isSgAssignInitializer(static_cast<SgNode *>(init_exp)) != nullptr) {
    string str = UnparseToString(init_exp);
    if (str != "(0)" && str != "(0.0)") {
      return 0;
    }
  } else if ((isSgConstructorInitializer(static_cast<SgNode *>(init_exp)) !=
              nullptr) ||
             (isSgDesignatedInitializer(static_cast<SgNode *>(init_exp)) !=
              nullptr)) {
    return 0;
  } else if (isSgAggregateInitializer(static_cast<SgNode *>(init_exp)) !=
             nullptr) {
    return IsSimpleAggregateInitializer(init_exp);
  } else {
//  FIXME: handle other cases later
#ifdef PROJDEBUG
    cout << "ERROR: Unknown Initializer Type:"
         << (static_cast<SgNode *>(init_exp))->class_name() << endl;
#endif
    return 0;
  }
  return 1;
}

void *CSageCodeGen::CreateAggregateInitializer(const vector<void *> &vec_exp,
                                               void *sg_type) {
  SgExprListExp *expr_list =
      isSgExprListExp(static_cast<SgNode *>(CreateExpList(vec_exp)));
  void *aggr_initializer = SageBuilder::buildAggregateInitializer(
      expr_list, isSgType(static_cast<SgNode *>(sg_type)));
  return aggr_initializer;
}

void *CSageCodeGen::reset_array_from_array(void *array, void *base_type,
                                           const vector<size_t> &nSizes,
                                           void *from_array, void *bindNode) {
  //  0 . Crate new Basicblock
  void *sg_bb = CreateBasicBlock(nullptr, false, bindNode);

  //  1 . create iterator declaration
  int dim = nSizes.size();
  vector<void *> vec_decl;
  for (int i = 0; i < dim; i++) {
    void *decl = CreateVariableDecl(GetTypeByString("int"), "_i" + my_itoa(i),
                                    nullptr, sg_bb, bindNode);
    AppendChild(sg_bb, decl);
    vec_decl.push_back(decl);
  }

  //  2. create assignement
  void *sg_assign_bb = CreateBasicBlock(nullptr, false, bindNode);
  {
    void *lhs = array;
    for (int i = 0; i < dim; i++) {
      lhs = CreateExp(V_SgPntrArrRefExp, lhs, CreateVariableRef(vec_decl[i]), 0,
                      bindNode);
    }
    void *rhs = from_array;
    for (int i = 0; i < dim; i++) {
      rhs = CreateExp(V_SgPntrArrRefExp, rhs, CreateVariableRef(vec_decl[i]), 0,
                      bindNode);
    }
    void *sg_assign = CreateExp(V_SgAssignOp, lhs, rhs, 0, bindNode);
    void *sg_stmt = CreateStmt(V_SgExprStatement, sg_assign, bindNode);
    AppendChild(sg_assign_bb, sg_stmt);
  }

  //  3. Create the loop levels
  void *for_loop = sg_assign_bb;
  for (int i = 0; i < dim; i++) {
    int64_t step = 1;
    void *sg_body = nullptr;
    if (for_loop == sg_assign_bb) {
      sg_body = sg_assign_bb;
    } else {
      sg_body = CreateBasicBlock(nullptr, false, bindNode);
      AppendChild(sg_body, for_loop);
    }
    for_loop = CreateStmt(V_SgForStatement,
                          GetVariableInitializedName(vec_decl[dim - 1 - i]),
                          CreateConst(0), CreateConst(nSizes[i] - 1), sg_body,
                          &step, nullptr, bindNode);
  }
  AppendChild(sg_bb, for_loop);
  return sg_bb;
}

void *
CSageCodeGen::create_for_loop_for_reset_array(void *lhs, void *base_type,
                                              const vector<size_t> &nSizes,
                                              void *tmp_var, void *bindNode) {
  SgExpression *lexp = isSgExpression(static_cast<SgNode *>(lhs));

  //  0 . Crate new Basicblock
  void *sg_bb = CreateBasicBlock(nullptr, false, bindNode);

  //  1 . create iterator declaration
  int dim = nSizes.size();
  vector<void *> vec_decl;
  for (int i = 0; i < dim; i++) {
    void *decl = CreateVariableDecl(GetTypeByString("int"), "_i" + my_itoa(i),
                                    nullptr, sg_bb, bindNode);
    AppendChild(sg_bb, decl);
    vec_decl.push_back(decl);
  }

  //  2. create assignement
  void *sg_assign_bb = CreateBasicBlock(nullptr, false, bindNode);
  {
    void *pntr = lexp ? lexp : CreateVariableRef(lhs);
    for (int i = 0; i < dim; i++) {
      pntr = CreateExp(V_SgPntrArrRefExp, pntr, CreateVariableRef(vec_decl[i]),
                       0, bindNode);
    }
    if (tmp_var == nullptr) {
      string var_name = lexp ? "" : GetVariableName(lhs);
      string tmp_var_name = var_name + "_tmp";
      void *initializer = nullptr;
      if (IsCompoundType(base_type) == 0) {
        initializer = CreateConst(0);
      } else {
        vector<void *> vec_exp;
        vec_exp.push_back(CreateConst(0));
        SgExprListExp *zero_list =
            isSgExprListExp(static_cast<SgNode *>(CreateExpList(vec_exp)));
        initializer = SageBuilder::buildAggregateInitializer(
            zero_list, isSgType(static_cast<SgNode *>(base_type)));
      }
      tmp_var = CreateVariableDecl(base_type, tmp_var_name, initializer,
                                   sg_assign_bb, bindNode);

      AppendChild(sg_assign_bb, tmp_var);
    }

    void *sg_assign =
        CreateExp(V_SgAssignOp, pntr, CreateVariableRef(tmp_var), 0, bindNode);
    void *sg_stmt = CreateStmt(V_SgExprStatement, sg_assign, bindNode);
    AppendChild(sg_assign_bb, sg_stmt);
  }

  //  3. Create the loop levels
  void *for_loop = sg_assign_bb;

  for (int i = 0; i < dim; i++) {
    int64_t step = 1;
    void *sg_body = nullptr;
    if (for_loop == sg_assign_bb) {
      sg_body = sg_assign_bb;
    } else {
      sg_body = CreateBasicBlock(nullptr, false, bindNode);
      AppendChild(sg_body, for_loop);
    }
    for_loop = CreateStmt(V_SgForStatement,
                          GetVariableInitializedName(vec_decl[dim - 1 - i]),
                          CreateConst(0), CreateConst(nSizes[i] - 1), sg_body,
                          &step, nullptr, bindNode);
  }
  AppendChild(sg_bb, for_loop);

  return sg_bb;
}

//  Transform array aggregate initializer into for loop
void CSageCodeGen::parse_aggregate(void *scope) {
  if (scope == nullptr) {
    scope = m_sg_project;
  }

  //  1. Array aggregate initializer
  //  Fixing: Split all the aggregate initialier into memset and for loop
  //  assignment
  {
    vector<void *> vec_initializer;
    //  GetNodesByType_int_compatible(scope, V_SgInitializer, &vec_initializer);
    GetNodesByType_int(scope, "preorder", V_SgInitializedName,
                       &vec_initializer);

    //  cout << vec_initializer.size() << endl;
    for (size_t i = 0; i < vec_initializer.size(); i++) {
      void *one_init = vec_initializer[i];
      if (UnparseToString(one_init).empty()) {
        continue;
      }
      if (UnparseToString(one_init).find("__builtin__") == 0) {
        continue;
      }

      void *initer = GetInitializer(one_init);
      if (initer == nullptr) {
        continue;
      }

      //  FIXME: ZP: 20151116: do not know if aggregate initializer for global
      //  variable matters
      //  BUt do not have good solution yet
      if (IsGlobalInitName(one_init) != 0) {
        continue;
      }

      SgAggregateInitializer *sg_ag_init =
          isSgAggregateInitializer(static_cast<SgNode *>(initer));

      if (sg_ag_init != nullptr) {
        //  cout << _p((_pa(one_init))) << endl;

        //  FIXME: ZP: 20151116
        //  FIXME: do not have a good way for complex aggregate
        {
          if (IsSimpleAggregateInitializer(sg_ag_init) == 0) {
            continue;
          }
        }

        void *sg_type = GetOrigTypeByTypedef(GetTypebyVar(one_init));
        if (IsArrayType(sg_type) == 0) {
          continue;
        }

        void *base_type;
        vector<size_t> nSizes;
        get_type_dimension(sg_type, &base_type, &nSizes, one_init);

        if (IsConstType(base_type) != 0) {
          continue;
        }
        size_t size = 1;
        size_t t;
        for (t = 0; t < nSizes.size(); t++) {
          assert((int64_t)nSizes[t] > 0);
          size *= nSizes[t];
        }
        void *pntr = CreateVariableRef(one_init);
        for (t = 1; t < nSizes.size(); t++) {
          pntr = CreateExp(V_SgPntrArrRefExp, pntr, CreateConst(0));
        }

        void *new_stmt = nullptr;
#if 0
        vector<void *> arg_list;
        arg_list.push_back(pntr);
        arg_list.push_back(CreateConst(0));
        arg_list.push_back(CreateExp(V_SgMultiplyOp,
                                     CreateExp(V_SgSizeOfOp, base_type),
                                     CreateConst(size)));
        void *sg_func_call = CreateFuncCall(
            "memset", GetTypeByString("void*"), arg_list,
            TraceUpByTypeCompatible(one_init, V_SgScopeStatement));
        new_stmt = CreateStmt(V_SgExprStatement, sg_func_call),
#else
        new_stmt = create_for_loop_for_reset_array(one_init, base_type, nSizes,
                                                   nullptr, one_init);
#endif
        InsertAfterStmt(new_stmt,
                        TraceUpByTypeCompatible(one_init, V_SgStatement));

        SetInitializer(one_init, nullptr);
      }
    }
  }

  //  2. reset defuse
  init_defuse();
}

//  There is two cases that defuse will not suppported well
//  1. Array aggregate initializer
//  2. Pass-by-reference argument in function call
void CSageCodeGen::fix_defuse_issues(void *scope, bool need_parse_aggregate) {
  if (scope == nullptr) {
    scope = m_sg_project;
  }

  //  1. Array aggregate initializer
  //  Fixing: Split all the aggregate initialier into memset and for loop
  //  assignment
  if (need_parse_aggregate) {
    parse_aggregate(scope);
  }
#if 0
  //  2. Pass-by-reference argument in function call
  //  Fixing: insert an redundant assignment: if (a) a = a
  {
    vector<void *> vec_calls;
    GetNodesByType_int_compatible(scope, V_SgFunctionCallExp, &vec_calls);
    for (size_t i = 0; i < vec_calls.size(); i++) {
      void *one_call = vec_calls[i];
      void *func_decl = GetFuncDeclByCall(one_call, 0);

      string sFuncName = GetFuncNameByCall(one_call);
      if (sFuncName == "printf")
        continue;
      if (sFuncName == "memcpy")
        continue;
      if (sFuncName == "memset")
        continue;
      if (!func_decl)
        continue;
      if (sFuncName.find("__") == 0)
        continue;

      string sFileName = GetFileInfo_filename(one_call);
      int line = GetFileInfo_line(one_call);

      if (sFileName.find("g++_HEADERS") != string::npos)
        continue;

      if (GetFuncCallParamNum(one_call) != GetFuncParamNum(func_decl)) {
        string sFileName = GetFileInfo_filename(one_call);
        int line = GetFileInfo_line(one_call);
        printf("\n");
        printf("  Warning: function declaration is missing or mismatch: \n");
        printf("      Function name: %s \n", GetFuncName(func_decl).c_str());
        printf("      Call at %s:%d \n\n", sFileName.c_str(), line);
        continue;
        //  exit(-1);
      }

      for (int j = 0; j < GetFuncCallParamNum(one_call); j++) {
        void *actual_arg = GetFuncCallParam(one_call, j);
        int is_ref = 0;
        int is_int_type = 0;
        if (IsVarRefExp(actual_arg)) {
          void *formal_param = GetFuncParam(func_decl, j);
          if (!formal_param)
            continue;
          void *sg_type = GetTypebyVar(formal_param);
          is_ref = IsReferenceType(sg_type);

          string str_type = UnparseToString(sg_type);
          //  FIXME: ZP: 20151116
          is_int_type =
              //                        UnparseToString(sg_type) == "float" ||
              //                        UnparseToString(sg_type) == "double" ||
              str_type == "int &" || str_type == "unsigned int &" ||
              str_type == "short &" || str_type == "unsigned short &" ||
              str_type == "char &" || str_type == "unsigned char &" ||

              str_type == "int *" || str_type == "unsigned int *" ||
              str_type == "short *" || str_type == "unsigned short *" ||
              str_type == "char *" || str_type == "unsigned char *";
        }
        if (is_ref && is_int_type) {
        //  if (is_int_type) //  ZP: 20151116: plan to do the transform for
        //  pointer as well,
        //  but it is not safe, and has big impact on generated code
          void *fake_exp = (is_ref)
                               ? actual_arg
                               : CreateExp(V_SgPntrArrRefExp,
                                           CopyExp(actual_arg), CreateConst(0));
          void *v_l = CopyExp(fake_exp);
          void *v_r = CopyExp(fake_exp);
          void *v_cond = CopyExp(fake_exp);
          void *sg_assign = CreateExp(V_SgAssignOp, v_l, v_r);
          void *sg_if = CreateIfStmt(
              v_cond, CreateStmt(V_SgExprStatement, sg_assign), nullptr);
          InsertStmt(sg_if, TraceUpByTypeCompatible(one_call, V_SgStatement));
        }
      }
    }
  }
#endif
  //  3. reset defuse
  init_defuse();
}

//  return initialized names
vector<void *> CSageCodeGen::get_var_decl_in_for_loop(void *sg_for_loop_) {
  SgForStatement *sg_for_loop =
      isSgForStatement(static_cast<SgNode *>(sg_for_loop_));
  vector<void *> ret;
  SgForInitStatement *init_stmt = sg_for_loop->get_for_init_stmt();

  SgStatementPtrList init_list = init_stmt->get_init_stmt();
  SgStatementPtrList::iterator it;
  for (it = init_list.begin(); it != init_list.end(); it++) {
    SgNode *sg_init_stmt = *it;
    SgVariableDeclaration *sg_decl_stmt = isSgVariableDeclaration(sg_init_stmt);
    ret.push_back(GetVariableInitializedName(sg_decl_stmt));
  }

  return ret;
}

int CSageCodeGen::hasLabelWithGoto(void *for_loop) {
  if (is_floating_node(for_loop) != 0) {
    return 0;
  }
  if (s_loop_label_cache.count(for_loop) <= 0) {
    bool res = false;
    vector<void *> labelStmts;
    GetNodesByType(for_loop, "preorder", "SgLabelStatement", &labelStmts);
    if (!labelStmts.empty()) {
      vector<void *> gotoInFunc;
      GetNodesByType(TraceUpToFuncDecl(for_loop), "preorder", "SgGotoStatement",
                     &gotoInFunc);
      for (auto labelStmt : labelStmts) {
        for (auto gotoStmt : gotoInFunc) {
          void *targetLabel =
              (static_cast<SgGotoStatement *>(gotoStmt))->get_label();
          if (targetLabel == labelStmt ||
              (is_floating_node(targetLabel) != 0) ||
              (is_floating_node(labelStmt) != 0)) {
            res = true;
            break;
          }
        }
      }
    }
    s_loop_label_cache[for_loop] = res;
  }
  return static_cast<int>(s_loop_label_cache[for_loop]);
}

//  Some point may not work because the sg_node are temporary (no parent)
//  #define _DEBUG_CANONICALIZE_LOOP_
void CSageCodeGen::get_all_candidates_from_loop_init(
    void *sg_for_loop_, vector<void *> *iv_cand, vector<void *> *iv_cand_init,
    vector<void *> *iv_cand_start, void **init_pos, void **iterator_ptr) {
  auto *sg_for_loop = static_cast<SgForStatement *>(sg_for_loop_);
  SgForInitStatement *init_stmt = sg_for_loop->get_for_init_stmt();

  SgStatementPtrList init_list = init_stmt->get_init_stmt();
  SgStatementPtrList::iterator it;
  for (it = init_list.begin(); it != init_list.end(); it++) {
    SgNode *sg_init_stmt = *it;
    *init_pos = sg_init_stmt;
#ifdef _DEBUG_CANONICALIZE_LOOP_
//  cout << UnparseToString(sg_init_stmt) << " " <<
//  sg_init_stmt->class_name() << endl;
#endif
    //  Note: the condition for iv candidate:
    //  a) it is an ExprStatement with an assignment expr
    //  b) it is an Variable declaration with an initializer
    SgExprStatement *sg_expr_stmt = isSgExprStatement(sg_init_stmt);
    SgVariableDeclaration *sg_decl_stmt = isSgVariableDeclaration(sg_init_stmt);
    if (sg_expr_stmt != nullptr) {
      SgExpression *sg_expr = sg_expr_stmt->get_expression();

#ifdef _DEBUG_CANONICALIZE_LOOP_
//  cout << "  " << UnparseToString(sg_expr) << " " <<
//  sg_expr->class_name() << endl;
#endif
      void *left_exp;
      void *right_exp;
      int ap_op = 0;
      IsAPIntOp(sg_expr, &left_exp, &right_exp, &ap_op);
      if (IsGeneralAssignOp(sg_expr, &left_exp, &right_exp)) {
        if (IsVarRefExp(left_exp) != 0) {
          void *sg_name = GetVariableInitializedName(left_exp);
          void *sg_init_value = right_exp;

          iv_cand->push_back(sg_name);
          iv_cand_init->push_back(sg_expr);
          iv_cand_start->push_back(sg_init_value);
        } else if (IsPointerDerefExp(left_exp) != 0) {
          *iterator_ptr = left_exp;
        }
      } else if ((isSgCommaOpExp(sg_expr) != nullptr) ||
                 V_SgCommaOpExp == ap_op) {
        vector<void *> vec_expr;
        GetExprListFromCommaOp(sg_expr, &vec_expr);

        for (size_t j = 0; j < vec_expr.size(); j++) {
          void *curr_expr = vec_expr[j];
          //  cout << "++" << UnparseToString(curr_expr) << endl;

          void *one_left_exp;
          void *one_right_exp;
          if (!IsGeneralAssignOp(curr_expr, &one_left_exp, &one_right_exp) ||
              (IsVarRefExp(one_left_exp) == 0)) {
            continue;
          }
          void *sg_name = GetVariableInitializedName(one_left_exp);
          void *sg_init_value = one_right_exp;

          iv_cand->push_back(sg_name);
          iv_cand_init->push_back(curr_expr);
          iv_cand_start->push_back(sg_init_value);
        }
      }
    } else if (sg_decl_stmt != nullptr) {
      //  skip if no initializer
      assert(sg_decl_stmt->get_variables().size() == 1);

      SgInitializedName *init_name =
          isSgInitializedName(sg_decl_stmt->get_variables()[0]);
      if (auto *ivar_exp = GetInitializerOperand(init_name)) {
        iv_cand->push_back(init_name);
        iv_cand_init->push_back(sg_decl_stmt);
        iv_cand_start->push_back(ivar_exp);
      }
      //  } else {
      //  //  FIXME-Yuxin: null initializer
      //  cout << "[CanonicalizeLoop] Unsupported type: "
      //  << GetASTTypeString(sg_init_stmt) << endl;
      //  assert(0);
    }
  }
#ifdef _DEBUG_CANONICALIZE_LOOP_
  for (size_t i = 0; i < iv_cand->size(); i++) {
    cout << "--iv_cand " << UnparseToString((*iv_cand)[i]) << "="
         << UnparseToString((*iv_cand_start)[i]) << " "
         << UnparseToString((*iv_cand_init)[i]) << endl;
  }
#endif
}

bool CSageCodeGen::get_all_candidate_from_loop_condition(
    void *sg_for_loop_, vector<void *> *iv_cond, vector<void *> *iv_cond_end,
    vector<int> *iv_cond_dir) {
  size_t i;
  auto *sg_for_loop = static_cast<SgForStatement *>(sg_for_loop_);
  SgStatement *cond_stmt = sg_for_loop->get_test();
  SgExprStatement *cond_exp_stmt = isSgExprStatement(cond_stmt);
  map<string, string> msg_map;
  if (cond_exp_stmt == nullptr) {
    msg_map[SRC_TRIP_COUNT] = "?";
    insertMessage(this, sg_for_loop, msg_map);
    return false;
  }
  SgExpression *sg_expr = cond_exp_stmt->get_expression();
  while (IsCommaExp(sg_expr)) {
    sg_expr = static_cast<SgExpression *>(GetExpRightOperand(sg_expr));
  }
  //  FIXME: Yuxin, Aug 24 2016
  //  If the testing part is not a compare operation, stop canonicalization.
  void *left_exp;
  void *right_exp;
  left_exp = right_exp = nullptr;
  int op = 0;

  if ((IsCompareOp(sg_expr) == 0) &&
      !(IsAPIntOp(sg_expr, &left_exp, &right_exp, &op) &&
        (IsCompareOp(op) != 0))) {
    msg_map[SRC_TRIP_COUNT] = "?";
    insertMessage(this, sg_for_loop, msg_map);
    return false;
  }

  if ((isSgEqualityOp(sg_expr) != nullptr) ||
      (isSgNotEqualOp(sg_expr) != nullptr) || V_SgEqualityOp == op ||
      V_SgNotEqualOp == op) {
    msg_map[SRC_TRIP_COUNT] = "?";
    insertMessage(this, sg_for_loop, msg_map);
    return false;
  }

#ifdef _DEBUG_CANONICALIZE_LOOP_
  cout << "  " << UnparseToString(sg_expr) << " " << sg_expr->class_name()
       << endl;
#endif
  //  FIXME: Yuxin-only consider SgAndOp and SgCompareOp
  if (isSgAndOp(sg_expr) != nullptr) {
    //  dead branch ???
    vector<void *> vec_test_temp;
    GetExprListFromAndOp(sg_expr, &vec_test_temp);
    for (i = 0; i < vec_test_temp.size(); i++) {
      void *cmp_op = vec_test_temp[i];
      void *left_exp;
      void *right_exp;
      left_exp = right_exp = nullptr;
      int op = 0;
      IsAPIntOp(cmp_op, &left_exp, &right_exp, &op);
      if ((IsCompareOp(cmp_op) != 0) || (IsCompareOp(op) != 0)) {
        map<void *, void *> lbs;
        map<void *, void *> ubs;
        GetVarRangeFromCompareOp(cmp_op, &lbs, &ubs);

        for (auto itt = lbs.begin(); itt != lbs.end(); itt++) {
          void *sg_var = itt->first;
          remove_cast(&sg_var);
          if (IsInitName(sg_var) == 0) {
            if (IsForStatement(sg_var) != 0) {
              sg_var = GetLoopIterator(sg_var);
            } else if (IsVarRefExp(sg_var) != 0) {
              sg_var = GetVariableInitializedName(sg_var);
            } else {
              sg_var = nullptr;  //  not considered as valid iterator variable
            }
          }
          if (sg_var != nullptr) {
            iv_cond->push_back(sg_var);
            iv_cond_end->push_back(itt->second);
            iv_cond_dir->push_back(1);
          }
        }
        for (auto itt = ubs.begin(); itt != ubs.end(); itt++) {
          void *sg_var = itt->first;
          remove_cast(&sg_var);
          if (IsInitName(sg_var) == 0) {
            if (IsForStatement(sg_var) != 0) {
              sg_var = GetLoopIterator(sg_var);
            } else if (IsVarRefExp(sg_var) != 0) {
              sg_var = GetVariableInitializedName(sg_var);
            } else {
              sg_var = nullptr;  //  not considered as valid iterator variable
            }
          }
          if (sg_var != nullptr) {
            iv_cond->push_back(sg_var);
            iv_cond_end->push_back(itt->second);
            iv_cond_dir->push_back(-1);
          }
        }
      }
    }
  } else if ((IsCompareOp(sg_expr) != 0) || (IsCompareOp(op) != 0)) {
    void *cmp_op = sg_expr;
    map<void *, void *> lbs;
    map<void *, void *> ubs;
    map<void *, void *>::iterator itt;
    GetVarRangeFromCompareOp(cmp_op, &lbs, &ubs);

    for (itt = lbs.begin(); itt != lbs.end(); itt++) {
      void *sg_var = itt->first;
      remove_cast(&sg_var);
      sg_var = RemoveAPIntFixedCast(sg_var);
      if (IsInitName(sg_var) == 0) {
        if (IsForStatement(sg_var) != 0) {
          sg_var = GetLoopIterator(sg_var);
        } else if (IsVarRefExp(sg_var) != 0) {
          sg_var = GetVariableInitializedName(sg_var);
        } else {
          sg_var = nullptr;  //  not considered as valid iterator variable
        }
      }
      if (sg_var != nullptr) {
        iv_cond->push_back(sg_var);
        iv_cond_end->push_back(itt->second);
        iv_cond_dir->push_back(1);
      }
    }
    for (itt = ubs.begin(); itt != ubs.end(); itt++) {
      void *sg_var = itt->first;
      remove_cast(&sg_var);
      sg_var = RemoveAPIntFixedCast(sg_var);
      if (IsInitName(sg_var) == 0) {
        if (IsForStatement(sg_var) != 0) {
          sg_var = GetLoopIterator(sg_var);
        } else if (IsVarRefExp(sg_var) != 0) {
          sg_var = GetVariableInitializedName(sg_var);
        } else {
          sg_var = nullptr;  //  not considered as valid iterator variable
        }
      }
      if (sg_var != nullptr) {
        iv_cond->push_back(sg_var);
        iv_cond_end->push_back(itt->second);
        iv_cond_dir->push_back(-1);
      }
    }
  }

#ifdef _DEBUG_CANONICALIZE_LOOP_
  for (i = 0; i < iv_cond->size(); i++) {
    void *sg_iv = (*iv_cond)[i];
    void *sg_bound = (*iv_cond_end)[i];
    string str_dir = (1 == (*iv_cond_dir)[i]) ? "lb" : "ub";
    cout << "--test " << str_dir << "["
         << (static_cast<SgNode *>(sg_iv))->class_name() << " "
         << UnparseToString(sg_iv)
         << "]= " << (static_cast<SgNode *>(sg_bound))->class_name() << " "
         << UnparseToString(sg_bound) << endl;
  }
#endif
  return true;
}

void CSageCodeGen::get_all_candidate_from_loop_increment(
    void *sg_for_loop_, vector<void *> *iv_incr,
    vector<int64_t> *iv_incr_step) {
  size_t i;
  auto *sg_for_loop = static_cast<SgForStatement *>(sg_for_loop_);
  SgExpression *step_exp = sg_for_loop->get_increment();

#ifdef _DEBUG_CANONICALIZE_LOOP_
  cout << "  " << UnparseToString(step_exp) << " " << step_exp->class_name()
       << endl;
#endif

  vector<void *> sub_exps;
  int op = 0;
  void *left_exp;
  void *right_exp;
  left_exp = right_exp = nullptr;
  if ((IsIncrementOp(step_exp) != 0) ||
      (IsAPIntOp(step_exp, &left_exp, &right_exp, &op) &&
       (IsIncrementOp(op) != 0))) {
    sub_exps.push_back(step_exp);
  } else if ((isSgCommaOpExp(step_exp) != nullptr) || V_SgCommaOpExp == op) {
    GetExprListFromCommaOp(step_exp, &sub_exps);
  }

  for (i = 0; i < sub_exps.size(); i++) {
    SgNode *sg_step = static_cast<SgNode *>(sub_exps[i]);
    void *left = nullptr;
    void *right = nullptr;
    GetExpOperand(sg_step, &left, &right);
    int op = (static_cast<SgNode *>(sg_step))->variantT();

    if ((IsBinaryOp(sg_step) != 0) ||
        (IsAPIntOp(sg_step, &left, &right, &op) &&
         (V_SgPlusAssignOp == op || V_SgAssignOp == op))) {
      CMarsExpression me_right(right, this);

      if (V_SgPlusAssignOp == op && (IsVarRefExp(left) != 0) &&
          (me_right.IsConstant() != 0)) {
        iv_incr->push_back(GetVariableInitializedName(left));
        iv_incr_step->push_back(me_right.GetConstant());
      } else if (V_SgMinusAssignOp == op && (IsVarRefExp(left) != 0) &&
                 (me_right.IsConstant() != 0)) {
        iv_incr->push_back(GetVariableInitializedName(left));
        iv_incr_step->push_back(-(me_right.GetConstant()));
      }
    } else if (V_SgPlusPlusOp == op) {
      if (IsAPIntOp(sg_step, &left, &right, &op) || (IsVarRefExp(left) != 0)) {
        iv_incr->push_back(GetVariableInitializedName(left));
        iv_incr_step->push_back(1);
      }
    } else if (V_SgMinusMinusOp == op) {
      if (IsAPIntOp(sg_step, &left, &right, &op) || (IsVarRefExp(left) != 0)) {
        iv_incr->push_back(GetVariableInitializedName(left));
        iv_incr_step->push_back(-1);
      }
    } else {
      //  FIXME: doesnt support the other increment
      //  unsupported type, no canonical
      cout << "increment op not support\n" << endl;
    }
  }

  for (i = 0; i < iv_incr->size(); i++) {
    void *sg_iv = (*iv_incr)[i];
    int64_t sg_step = (*iv_incr_step)[i];
    ConvertBigIntegerIntoSignedInteger(&sg_step, GetTypebyVar(sg_iv));
    (*iv_incr_step)[i] = sg_step;
  }
#ifdef _DEBUG_CANONICALIZE_LOOP_
  for (i = 0; i < iv_incr->size(); i++) {
    void *sg_iv = (*iv_incr)[i];
    int64_t sg_step = (*iv_incr_step)[i];
    cout << "--incr " << UnparseToString(sg_iv) << " " << sg_step << endl;
  }
#endif
}

void CSageCodeGen::convert_loop_iterator_with_char_type_into_int(
    void *sg_for_loop_, void **iterator_variable) {
  auto *sg_for_loop = static_cast<SgForStatement *>(sg_for_loop_);
  SgForInitStatement *init_stmt = sg_for_loop->get_for_init_stmt();
  SgStatementPtrList init_list = init_stmt->get_init_stmt();
  SgStatementPtrList::iterator it;
  void *func_decl = TraceUpToFuncDecl(sg_for_loop);
  for (it = init_list.begin(); it != init_list.end(); it++) {
    SgNode *sg_init_stmt = *it;

    SgExprStatement *sg_expr_stmt = isSgExprStatement(sg_init_stmt);
    SgVariableDeclaration *sg_decl_stmt = isSgVariableDeclaration(sg_init_stmt);
    if (sg_decl_stmt != nullptr) {
      SgInitializedNamePtrList &var_list = sg_decl_stmt->get_variables();
      for (size_t i = 0; i != var_list.size(); ++i) {
        SgInitializedName *init_name = isSgInitializedName(var_list[i]);
        SgAssignInitializer *sg_initializer =
            isSgAssignInitializer(init_name->get_initializer());
        void *ivar_exp = nullptr;
        if (sg_initializer != nullptr) {
          SgExpression *ivar_exp_orig =
              isSgExpression(sg_initializer->get_operand());
          if (ivar_exp_orig != nullptr) {
            CMarsExpression me(ivar_exp_orig, this);
            if (me.IsConstant() != 0) {
              ivar_exp = CreateConst(static_cast<int>(me.GetConstant()));
            } else {
              ivar_exp = CopyExp(ivar_exp_orig);
            }
          }
        }
        string new_var_name = "_s_" + init_name->unparseToString();
        get_valid_local_name(func_decl, &new_var_name);
        SgVariableDeclaration *new_decl = static_cast<SgVariableDeclaration *>(
            CreateVariableDecl("int", new_var_name, ivar_exp,
                               TraceUpToBasicBlock(sg_for_loop)));
        InsertStmt(new_decl, sg_for_loop, true);

        SgInitializedName *new_init = static_cast<SgInitializedName *>(
            GetVariableInitializedName(new_decl));
        replace_variable_references_in_scope(init_name, new_init, sg_for_loop);

        if (*iterator_variable == init_name) {
          *iterator_variable = new_init;
        }
      }
    } else if (sg_expr_stmt != nullptr) {
      SgInitializedName *init_name =
          static_cast<SgInitializedName *>(*iterator_variable);
      if (nullptr != init_name) {
#if 0
        SgInitializer *initializer = nullptr;
        if (init_name != nullptr)
          initializer = init_name->get_initializer();
        SgAssignInitializer *sg_initializer = nullptr;
        if (nullptr != initializer)
          sg_initializer = isSgAssignInitializer(initializer);
        void *ivar_exp = nullptr;
        if (sg_initializer != nullptr) {
          SgExpression *ivar_exp_orig =
            isSgExpression(sg_initializer->get_operand());
          if (ivar_exp_orig != nullptr) {
            CMarsExpression me(ivar_exp_orig, this);
            if (me.IsConstant() != 0) {
              ivar_exp = CreateConst(static_cast<int>(me.GetConstant()));
            } else {
              ivar_exp = static_cast<SgExpression *>(CopyExp(ivar_exp_orig));
            }
          }
        }
#endif
        string new_var_name = "_s_" + init_name->unparseToString();
        get_valid_local_name(func_decl, &new_var_name);
        void *new_decl = CreateVariableDecl("int", new_var_name, nullptr,
                                            TraceUpToBasicBlock(sg_for_loop));
        InsertStmt(new_decl, sg_for_loop, true);

        void *new_init = GetVariableInitializedName(new_decl);
        replace_variable_references_in_scope(init_name, new_init, sg_for_loop);
        //  Yuxin: Apr 20 2018
        void *new_stmt = CreateStmt(V_SgAssignStatement,
                                    CreateVariableRef(*iterator_variable),
                                    CreateVariableRef(new_init));
        InsertAfterStmt(new_stmt, sg_for_loop);

        *iterator_variable = new_init;
      }
    }
  }
#ifdef PROJDEBUG
  cout << _p(sg_for_loop) << endl;
#endif
}

bool CSageCodeGen::convert_loop_iterator_with_pointer_type(void *sg_for_loop_,
                                                           void *iterator_ptr) {
  auto *sg_for_loop = static_cast<SgForStatement *>(sg_for_loop_);
  void *func_decl = TraceUpToFuncDecl(sg_for_loop);
  cout << "The non-canonicalize loop has pointer dereferencing iterator\n";

  void *var_exp = GetExpUniOperand(iterator_ptr);
  void *sg_name = GetVariableInitializedName(var_exp);
  void *sg_type = GetTypebyVar(sg_name);
  void *sg_base_type;
  if (IsPointerType(sg_type) != 0) {
    sg_base_type = GetBaseTypeOneLayer(sg_type);
  } else {
    map<string, string> msg_map;
    msg_map[SRC_TRIP_COUNT] = "?";
    insertMessage(this, sg_for_loop, msg_map);
    return false;
  }

  string new_var_name = "_d_" + UnparseToString(sg_name);
  get_valid_local_name(func_decl, &new_var_name);
  SgVariableDeclaration *new_decl = static_cast<SgVariableDeclaration *>(
      CreateVariableDecl(sg_base_type, new_var_name, CopyExp(iterator_ptr),
                         TraceUpToBasicBlock(sg_for_loop)));
  InsertStmt(new_decl, sg_for_loop, true);

  SgInitializedName *new_init =
      static_cast<SgInitializedName *>(GetVariableInitializedName(new_decl));
  void *new_ref = CreateVariableRef(new_init);
  void *assign_expr =
      CreateExp(V_SgAssignOp, CopyExp(iterator_ptr), new_ref, 0);
  void *stmt = CreateStmt(V_SgExprStatement, assign_expr);
  InsertAfterStmt(stmt, sg_for_loop);

  Rose_STL_Container<SgNode *> varList =
      NodeQuery::querySubTree(sg_for_loop, V_SgPointerDerefExp);
  Rose_STL_Container<SgNode *>::iterator var;
  for (var = varList.begin(); var != varList.end(); var++) {
    void *ref = *var;
    void *exp = GetExpUniOperand(ref);
    void *arr_var = GetVariableInitializedName(exp);
    if (arr_var == sg_name) {
      SgExpression *new_ref =
          static_cast<SgExpression *>(CreateVariableRef(new_init));
      ReplaceExp(ref, new_ref);
    }
  }
  return true;
}

//  ////////////////////////////////////////////////////////////  /
//  Peng and Yuxin edit 2016 Aug
//  1. If convertable to a canonical form, do it by determining the iv, init,
//  cond and incr
//  2. if not convertable, transfrom it into a while loop
//
int CSageCodeGen::CanonicalizeForLoop(void **sg_for_loop_, bool transform) {
  cout << "\n====== Loop Canonicalize ====> \n";
  size_t i;
  size_t j;
  SgForStatement *sg_for_loop =
      isSgForStatement(static_cast<SgNode *>(*sg_for_loop_));
  if (sg_for_loop == nullptr) {
    return 0;
  }

  if (hasLabelWithGoto(*sg_for_loop_) != 0) {
    return 0;
  }

  void *sg_for_prev_stmt = GetPreviousStmt(sg_for_loop);
  void *iv;
  void *lb;
  void *ub;
  void *step;
  void *body;
  iv = lb = ub = step = body = nullptr;
  bool inc_dir;
  bool inclusive_bound;
  bool ret = IsCanonicalForLoop(sg_for_loop, &iv, &lb, &ub, &step, &body,
                                &inc_dir, &inclusive_bound) != 0;
  map<string, string> msg_map;
  if (ret) {
    cout << "[isCanonical] " << UnparseToString(sg_for_loop) << endl;
    int64_t range = 0;
    int64_t range_ub = 0;
    int ret = get_loop_trip_count(sg_for_loop, &range, &range_ub);
    if ((ret != 0) || (range_ub != 0)) {
      std::ostringstream oss;
      oss << range_ub;
      msg_map[SRC_TRIP_COUNT] = oss.str();
      insertMessage(this, sg_for_loop, msg_map);
    }
    return 0;
  }
  cout << endl << "[Canonicalize] " << UnparseToString(sg_for_loop) << endl;

  void *func_decl =
      GetEnclosingNode("Function", *sg_for_loop_);  //  get enclosing statement

  //  1. determine iterator variable
  void *iterator_variable = nullptr;
  void *iterator_start = nullptr;
  void *iterator_end = nullptr;
  void *iterator_ptr =
      nullptr;  //  To store the iterator which might be a pointer dereferencing
  int64_t iterator_step;
  void *init_pos = nullptr;
  {
    //  1.1 get all iv candidates from init_list
    vector<void *> iv_cand;
    vector<void *> iv_cand_init;
    vector<void *> iv_cand_start;
    get_all_candidates_from_loop_init(sg_for_loop, &iv_cand, &iv_cand_init,
                                      &iv_cand_start, &init_pos, &iterator_ptr);

    //  1.2 find the iv candidates from loop condition (test statement)
    vector<void *> iv_cond;
    vector<void *> iv_cond_end;
    vector<int> iv_cond_dir;
    if (!get_all_candidate_from_loop_condition(sg_for_loop, &iv_cond,
                                               &iv_cond_end, &iv_cond_dir))
      return 1;

    //  1.3 find the iv candidates from loop increment (test statement)
    vector<void *> iv_incr;
    vector<int64_t> iv_incr_step;
    get_all_candidate_from_loop_increment(sg_for_loop, &iv_incr, &iv_incr_step);

    //  1.4 match iv_incr and iv_cond
    vector<void *> iv_incr_cond;
    vector<void *> iv_incr_cond_bound;
    vector<int> iv_incr_cond_step;
    {
      for (i = 0; i < iv_cond.size(); i++) {
        for (j = 0; j < iv_incr.size(); j++) {
#ifdef _DEBUG_CANONICALIZE_LOOP_
//  cout << "  ++ " << GetASTTypeString(iv_cond[i]) << " "
//       << UnparseToString(iv_cond[i]) << " <-> "
//       << GetASTTypeString(iv_incr[j]) << " "
//       << UnparseToString(iv_incr[j]) << endl;
#endif

          //  iterator match, and direction match
          if (iv_cond[i] == iv_incr[j]) {
            if (iv_cond_dir[i] * iv_incr_step[j] < 0) {
              iv_incr_cond.push_back(iv_cond[i]);
              iv_incr_cond_bound.push_back(iv_cond_end[i]);
              iv_incr_cond_step.push_back(iv_incr_step[j]);

#ifdef _DEBUG_CANONICALIZE_LOOP_
//  cout << "  incr_cond push " << UnparseToString(iv_cond[i])
//       << endl;
#endif
            }
          }
        }
      }
    }

    //  1.5 match iv_cand and iv_incr_cond
    //  FIXME: only find one, then break
    int find = 0;
    {
      for (i = 0; i < iv_cand.size(); i++) {
        for (j = 0; j < iv_incr_cond.size(); j++) {
          //  iterator match, and direction match
          if (iv_cand[i] == iv_incr_cond[j]) {
            iterator_variable = iv_cand[i];
            iterator_start = CopyExp(iv_cand_start[i]);
            iterator_end = CopyExp(iv_incr_cond_bound[j]);
            iterator_step = iv_incr_cond_step[j];
            find = 1;
            break;
          }
        }
        if (find != 0) {
          break;
        }
      }
      if (iv_cand.empty() && (init_pos != nullptr)) {
        for (size_t i = 0; i < iv_incr_cond.size(); ++i) {
          void *candidate_variable = iv_incr_cond[i];
          CMarsExpression me_start(CreateVariableRef(candidate_variable), this,
                                   init_pos, TraceUpToFuncDecl(sg_for_loop));
          if ((me_start.is_movable_to(init_pos) != 0) &&
              me_start.print_s() != UnparseToString(candidate_variable)) {
            iterator_start = me_start.get_expr_from_coeff();
            iterator_end = CopyExp(iv_incr_cond_bound[i]);
            iterator_step = iv_incr_cond_step[i];
            iterator_variable = candidate_variable;
            // find = 1;
            break;
          }
        }
      }
    }

    //  ZP: 20150727: if no expr reduce in CMarsExpression::reduce, the lb and
    //  ub can not be printed.
    //  if (find)
    //  {
    //    cout << "--IV: " <<
    //        UnparseToString(iterator_variable) <<
    //        " [" <<
    //        UnparseToString(iterator_start) << ".." <<
    //        UnparseToString(iterator_end) << "]" <<
    //        " step=" <<
    //        iterator_step << endl;
    //  }
  }

  //  2.  Convert to while loop if not canonicalizable
  //  FIXME-Yuxin: what is the principle? Find at least one iterator can
  //  convert? Current fixing: if a loop is uncanonicalizable, stop and report
  //  to the user.
  if (iterator_variable == nullptr) {
    if (iterator_ptr != nullptr) {
      if (!convert_loop_iterator_with_pointer_type(sg_for_loop, iterator_ptr))
        return 1;
    } else {
      cout << "Non-canonicalizable \n";
      msg_map[SRC_TRIP_COUNT] = "?";
      insertMessage(this, sg_for_loop, msg_map);
      return 1;
    }
  }

  //  3. Convert the for loop to canonical formata if necessary
  //  FIXME-Yuxin: only convert to one loop?
  if (iterator_variable != nullptr) {
    void *sg_name_1;
    if (IsCanonicalForLoop(sg_for_loop, &sg_name_1) != 0) {
      //  Do nothing
      assert(sg_name_1 == iterator_variable);
    } else {
      void *sg_type = nullptr;
      sg_type = GetOrigTypeByTypedef(GetTypebyVar(iterator_variable), true);
      if ((IsIntegerType(sg_type) == 0) && (IsGeneralCharType(sg_type) == 0) &&
          (IsXilinxAPIntType(sg_type) == 0)) {
#ifdef _DEBUG_CANONICALIZE_LOOP_
        cout << "[Canonicalize] Unsupported iterator type " << _p(sg_type)
             << endl;
#endif
        msg_map[SRC_TRIP_COUNT] = "?";
        insertMessage(this, sg_for_loop, msg_map);
        return 1;
      }
      if (has_write_in_scope_fast(iterator_variable,
                                  GetLoopBody(sg_for_loop)) != 0) {
        cout << "Cannot be canonicalized: iterator variable is updated in loop "
                "body"
             << endl;
        msg_map[SRC_TRIP_COUNT] = "?";
        insertMessage(this, sg_for_loop, msg_map);
        return 1;
      }
      if (!transform) {
        return 2;
      }
      //  3.0.0 Yuxin: replace the "char" iterators with "int"
      if (IsGeneralCharType(sg_type) != 0) {
        convert_loop_iterator_with_char_type_into_int(sg_for_loop,
                                                      &iterator_variable);
      }

      //  3.0 Generate the Comment for the original code
      string str_org_loop = UnparseToString(sg_for_loop);

      //  3.1 get all the additional init statements to move out
      vector<void *> new_init_stmts;
      {
        SgForInitStatement *init_stmt = sg_for_loop->get_for_init_stmt();
        SgStatementPtrList init_list = init_stmt->get_init_stmt();
        SgStatementPtrList::iterator it;

        for (it = init_list.begin(); it != init_list.end(); it++) {
          SgNode *sg_init_stmt = *it;

          SgExprStatement *sg_expr_stmt = isSgExprStatement(sg_init_stmt);
          SgVariableDeclaration *sg_decl_stmt =
              isSgVariableDeclaration(sg_init_stmt);
          if (sg_expr_stmt != nullptr) {
            SgExpression *sg_expr = sg_expr_stmt->get_expression();

            vector<void *> vec_assign;
            void *left_exp;
            void *right_exp;
            left_exp = right_exp = nullptr;
            int op = 0;
            GetExpOperand(sg_expr, &left_exp, &right_exp);
            if ((IsAssignOp(sg_expr) != 0) ||
                (IsAPIntOp(sg_expr, &left_exp, &right_exp, &op) &&
                 V_SgAssignOp == op)) {
              if (IsVarRefExp(left_exp) != 0) {
                void *init_name = GetVariableInitializedName(left_exp);
                if (init_name != iterator_variable) {
                  vec_assign.push_back(sg_expr);
                }
              } else {
                vec_assign.push_back(sg_expr);
              }
            } else if ((isSgCommaOpExp(sg_expr) != nullptr) ||
                       V_SgCommaOpExp == op) {
              vector<void *> vec_expr;
              GetExprListFromCommaOp(sg_expr, &vec_expr);

              for (j = 0; j < vec_expr.size(); j++) {
                void *curr_expr = vec_expr[j];
                //  cout << "++" << UnparseToString(curr_expr) << endl;
                void *left_exp;
                void *right_exp;
                left_exp = right_exp = nullptr;
                int op = 0;
                GetExpOperand(curr_expr, &left_exp, &right_exp);
                if (((IsAssignOp(static_cast<SgNode *>(curr_expr)) != 0) ||
                     (IsAPIntOp(curr_expr, &left_exp, &right_exp, &op) &&
                      V_SgAssignOp == op)) &&
                    (IsVarRefExp(left_exp) != 0)) {
                  void *init_name = GetVariableInitializedName(left_exp);
                  if (init_name != iterator_variable) {
                    vec_assign.push_back(curr_expr);
                  }
                } else {
                  vec_assign.push_back(curr_expr);
                }
              }
            }

            void *sg_curr_init = nullptr;
            for (i = 0; i < vec_assign.size(); i++) {
              if (sg_curr_init != nullptr) {
                sg_curr_init = CreateExp(V_SgCommaOpExp, sg_curr_init,
                                         CopyExp(vec_assign[i]));
              } else {
                sg_curr_init = CopyExp(vec_assign[i]);
              }
            }
            if (sg_curr_init != nullptr) {
              new_init_stmts.push_back(
                  CreateStmt(V_SgExprStatement, sg_curr_init));
            }
          } else if (sg_decl_stmt != nullptr) {
            SgInitializedName *init_name =
                isSgInitializedName(sg_decl_stmt->get_variables()[0]);
            void *ivar_exp = nullptr;
            if (auto *ivar_exp_orig = GetInitializerOperand(init_name)) {
              ivar_exp_orig = RemoveAPIntFixedCast(ivar_exp_orig);
              CMarsExpression me(ivar_exp_orig, this);
              if (me.IsConstant() != 0) {
                ivar_exp = CreateConst(static_cast<int>(me.GetConstant()));
              } else {
                ivar_exp = static_cast<SgExpression *>(CopyExp(ivar_exp_orig));
              }
            }
            string new_var_name = "_l_" + init_name->unparseToString();
            get_valid_local_name(func_decl, &new_var_name);
            SgVariableDeclaration *new_decl =
                static_cast<SgVariableDeclaration *>(
                    CreateVariableDecl(sg_type, new_var_name, ivar_exp,
                                       TraceUpToBasicBlock(sg_for_loop)));
            InsertStmt(new_decl, sg_for_loop, true);

            void *new_init = GetVariableInitializedName(new_decl);
            replace_variable_references_in_scope(init_name, new_init,
                                                 sg_for_loop);

            if (iterator_variable == init_name) {
              iterator_variable = new_init;
            }
          }
        }
      }

      //  3.2 Get all the additional incr statements
      void *new_body;
      {
        SgExpression *step_exp = sg_for_loop->get_increment();

        //  3.2.1 remove the incr of iv from step_exp
        {
          vector<void *> vec_addi_incr;
          vector<void *> sub_exps;
          void *left_exp;
          void *right_exp;
          left_exp = right_exp = nullptr;
          int op = 0;
          if ((IsIncrementOp(step_exp) != 0) ||
              (IsAPIntOp(step_exp, &left_exp, &right_exp, &op) &&
               (IsIncrementOp(op) != 0))) {
            sub_exps.push_back(step_exp);
          } else if ((isSgCommaOpExp(step_exp) != nullptr) ||
                     V_SgCommaOpExp == op) {
            GetExprListFromCommaOp(step_exp, &sub_exps);
          }
          for (i = 0; i < sub_exps.size(); i++) {
            SgNode *sg_step = static_cast<SgNode *>(sub_exps[i]);
            void *left = nullptr;
            void *right = nullptr;
            int op = 0;
            GetExpOperand(sg_step, &left, &right);
            IsAPIntOp(sg_step, &left, &right, &op);

            //  FIXME: only test ++,==,+=,-=
            if (((isSgPlusAssignOp(sg_step) != nullptr) ||
                 V_SgPlusAssignOp == op) &&
                GetVariableInitializedName(left) == iterator_variable) {
            } else if (((isSgMinusAssignOp(sg_step) != nullptr) ||
                        V_SgMinusAssignOp == op) &&
                       GetVariableInitializedName(left) == iterator_variable) {
            } else if (((isSgPlusPlusOp(sg_step) != nullptr) ||
                        V_SgPlusPlusOp == op) &&
                       GetVariableInitializedName(left) == iterator_variable) {
            } else if (((isSgMinusMinusOp(sg_step) != nullptr) ||
                        V_SgMinusMinusOp == op) &&
                       GetVariableInitializedName(left) == iterator_variable) {
            } else {
              vec_addi_incr.push_back(sg_step);
            }
          }
          void *sg_curr_incr = nullptr;
          for (i = 0; i < vec_addi_incr.size(); i++) {
            if (sg_curr_incr != nullptr) {
              sg_curr_incr =
                  CreateExp(V_SgAndOp, sg_curr_incr, CopyExp(vec_addi_incr[i]));
            } else {
              sg_curr_incr = CopyExp(vec_addi_incr[i]);
            }
          }
          step_exp = isSgExpression(static_cast<SgNode *>(sg_curr_incr));
        }
        void *sg_body = GetLoopBody(sg_for_loop);
        new_body = CopyStmt(sg_body);
        if (IsBasicBlock(new_body) != 0) {
          if (step_exp != nullptr) {
            AppendChild(new_body,
                        CreateStmt(V_SgExprStatement, CopyExp(step_exp)));
          }
        } else {
          void *new_bb = CreateBasicBlock();
          AppendChild(new_bb, new_body);
          if (step_exp != nullptr) {
            AppendChild(new_bb,
                        CreateStmt(V_SgExprStatement, CopyExp(step_exp)));
          }
          new_body = new_bb;
        }
      }
      //  3.3 Get all the additional cond statements
      //  void * new_body;
      {
        SgStatement *cond_stmt = sg_for_loop->get_test();
        SgExprStatement *cond_exp_stmt = isSgExprStatement(cond_stmt);
        ROSE_ASSERT(cond_exp_stmt);
        SgExpression *cond_exp = cond_exp_stmt->get_expression();
        //  3.3.1 remove the cond of iv from cond_exp
        {
          void *sg_expr = cond_exp;
          void *left_exp;
          void *right_exp;
          left_exp = right_exp = nullptr;
          int op = 0;
          IsAPIntOp(sg_expr, &left_exp, &right_exp, &op);
          if ((IsCompareOp(sg_expr) != 0) || (IsCompareOp(op) != 0)) {
            cond_exp = nullptr;
          } else if (isSgAndOp(static_cast<SgNode *>(sg_expr)) != nullptr) {
            vector<void *> vec_addi_cond;
            vector<void *> vec_test_temp;
            GetExprListFromAndOp(sg_expr, &vec_test_temp);
            for (i = 0; i < vec_test_temp.size(); i++) {
              void *cmp_op = vec_test_temp[i];
              void *left_exp;
              void *right_exp;
              left_exp = right_exp = nullptr;
              int op = 0;
              GetExpOperand(cmp_op, &left_exp, &right_exp);
              IsAPIntOp(cmp_op, &left_exp, &right_exp, &op);
              if ((IsCompareOp(cmp_op) != 0) || (IsCompareOp(op) != 0)) {
                void *var_ref = left_exp;
                if (GetVariableInitializedName(var_ref) != iterator_variable) {
                  vec_addi_cond.push_back(cmp_op);
                }
              } else {
                vec_addi_cond.push_back(cmp_op);
              }
            }
            void *sg_curr_cond = nullptr;
            for (i = 0; i < vec_addi_cond.size(); i++) {
              if (sg_curr_cond != nullptr) {
                sg_curr_cond = CreateExp(V_SgCommaOpExp, sg_curr_cond,
                                         CopyExp(vec_addi_cond[i]));
              } else {
                sg_curr_cond = CopyExp(vec_addi_cond[i]);
              }
            }
            cond_exp = isSgExpression(static_cast<SgNode *>(sg_curr_cond));
          } else {
            //  do nothing for cond, not sub term to eliminate
          }
        }
        if (cond_exp != nullptr) {
          void *sg_if_stmt = CreateIfStmt(CopyExp(cond_exp), new_body, nullptr);
          new_body = CreateBasicBlock();
          PrependChild(new_body, sg_if_stmt);
        }
      }
      //  3.4 Create and replace loop
      void *new_for_loop =
          CreateStmt(V_SgForStatement, iterator_variable,
                     RemoveAPIntFixedCast(iterator_start), iterator_end,
                     new_body, &iterator_step, nullptr, sg_for_loop);
      //            cout << " => " <<  _p(sg_for_loop) << endl;
      cout << "  -- Rewrite the for loop into canonical form" << endl;
      cout << "[canonicalize] Move and rename multiple init declaration.\n";
      for (i = 0; i < new_init_stmts.size();
           i++) {  //  maintain the original order
        void *init_stmt = new_init_stmts[i];
        InsertStmt(init_stmt, sg_for_loop, true);
      }
      ReplaceStmt(sg_for_loop, new_for_loop);
      int64_t range = 0;
      int64_t range_ub = 0;
      int ret = get_loop_trip_count(new_for_loop, &range, &range_ub);
      if ((ret != 0) || (range_ub != 0)) {
        std::ostringstream oss;
        oss << range_ub;
        msg_map[SRC_TRIP_COUNT] = oss.str();
        insertMessage(this, new_for_loop, msg_map);
      }
      if ((sg_for_prev_stmt != nullptr) &&
          (IsLabelStatement(sg_for_prev_stmt) != 0)) {
        (static_cast<SgLabelStatement *>(sg_for_prev_stmt))
            ->set_scope(static_cast<SgForStatement *>(new_for_loop));
      }
      //        cout << " => " <<  _p(new_for_loop) << endl;
      AddComment("Canonicalized from: " + str_org_loop, new_for_loop);
      *sg_for_loop_ = new_for_loop;
    }
    return 2;
  }
  return 2;
}

//  Yuxin edit 2016 Aug
int CSageCodeGen::StandardizeForLoop(void **sg_for_loop_) {
  cout << "====== Loop Standardize ====> " << endl;

  SgForStatement *sg_for_loop =
      isSgForStatement(static_cast<SgNode *>(*sg_for_loop_));
  string str_org_loop = UnparseToString(sg_for_loop);
  if (sg_for_loop == nullptr) {
    return 0;
  }

  void *iv;
  void *lb;
  void *ub;
  void *step;
  void *body;
  bool inc_dir;
  bool inclusive_bound;
  bool ret = IsCanonicalForLoop(sg_for_loop, &iv, &lb, &ub, &step, &body,
                                &inc_dir, &inclusive_bound) != 0;
  if (!ret) {
#ifdef _DEBUG_CANONICALIZE_LOOP_
    cout << "[isNonCanonical] " << UnparseToString(sg_for_loop) << endl;
#endif
    return 0;
  }

  void *v_step = nullptr;
  int64_t nStep;
  ret = (GetLoopStepValueFromExpr(step, &nStep, &v_step) != 0);
  if (!ret || nStep == 0) {
#ifdef _DEBUG_CANONICALIZE_LOOP_
    cout << "[isNonCanonical] " << UnparseToString(sg_for_loop) << endl;
#endif
    return 0;
  }

#ifdef _DEBUG_CANONICALIZE_LOOP_
  cout << endl;
  cout << "[Standardize] " << UnparseToString(sg_for_loop) << endl
       << "==> step: " << nStep << endl;
#endif

  //  Yuxin: Apr 2018
  //  Donnot support loops with break/goto inside
  vector<void *> vec_breaks;
  GetNodesByType_int(sg_for_loop, "preorder", V_SgBreakStmt, &vec_breaks);
  for (auto stmt : vec_breaks) {
    if (IsScopeBreak(stmt, sg_for_loop)) {
      cout << "Quit standardize because of break statement.\n";
      return 0;
    }
  }

  vector<void *> vec_gotos;
  GetNodesByType_int(sg_for_loop, "preorder", V_SgGotoStatement, &vec_gotos);
  if (static_cast<unsigned int>(!vec_gotos.empty()) != 0U) {
    cout << "Quit standardize because of goto statement.\n";
    return 0;
  }

  void *func_decl =
      GetEnclosingNode("Function", *sg_for_loop_);  //  get enclosing statement
  SgExpression *sg_step = sg_for_loop->get_increment();

  //  support checker
  // TODO(yuxing): enhance for i=i+coeff
  int op = (static_cast<SgNode *>(sg_step))->variantT();
  void *left_exp = nullptr;
  void *right_exp = nullptr;
  IsAPIntOp(sg_step, &left_exp, &right_exp, &op);
  if (V_SgPlusAssignOp != op && V_SgMinusAssignOp != op &&
      V_SgMinusMinusOp != op) {
#ifdef _DEBUG_CANONICALIZE_LOOP_
    cout << "No need to standardize: we only support +=, -=, -- " << endl;
#endif
    return 0;
  }

  SgStatement *cond_stmt = sg_for_loop->get_test();
  SgExprStatement *cond_exp_stmt = isSgExprStatement(cond_stmt);
  SgExpression *sg_cond_expr = cond_exp_stmt->get_expression();
  void *sg_body = GetLoopBody(sg_for_loop);
  SgInitializedName *sg_var =
      static_cast<SgInitializedName *>(GetLoopIterator(sg_for_loop));
  void *sg_type = GetTypebyVar(sg_var);
  if (!inc_dir && nStep > 0) {
    ConvertBigIntegerIntoSignedInteger(&nStep, sg_type);
  }
  if (has_write_in_scope(sg_var, sg_body) != 0) {
#ifdef _DEBUG_CANONICALIZE_LOOP_
    cout << "Cannot standardize: has write to loop iterator in loop body"
         << endl;
#endif
    return 0;
  }

#ifdef _DEBUG_CANONICALIZE_LOOP_
//            cout << "  " << UnparseToString(step_exp) << " " <<
//            step_exp->class_name() << endl;
#endif

  CMarsRangeExpr mr = CMarsVariable(sg_for_loop, this).get_range();
  CMarsExpression me_lb;
  CMarsExpression me_ub;
  if (mr.get_simple_bound(&me_lb, &me_ub) == 0) {
#ifdef _DEBUG_CANONICALIZE_LOOP_
    cout << "Cannot get a loop range" << endl;
#endif
    return 0;
  }
  void *ub_var = me_ub.get_expr_from_coeff();
  void *lb_var = me_lb.get_expr_from_coeff();

  CMarsExpression me_b = me_ub - me_lb;
#ifdef _DEBUG_CANONICALIZE_LOOP_
  cout << "range: [" << me_lb.print() << "," << me_ub.print() << "]" << endl;
  cout << _p(lb_var) << ":" << _p(ub_var) << endl;
#endif
  if (me_b.IsConstant() == 0) {
#ifdef _DEBUG_CANONICALIZE_LOOP_
    cout << "[Standardize] Non-constant range" << endl;
#endif
    return 0;
  }

  CMarsExpression me_step(v_step, this, step);
  CMarsExpression me_end = me_ub;
  CMarsExpression me_start = me_lb;
  CMarsExpression me_range = me_end - me_start;
  int64_t v_ub = me_ub.GetConstant();
  int64_t v_lb = me_lb.GetConstant();
  if (nStep > 0) {
    me_range = (me_end - me_start) / nStep;
  } else if (nStep < 0) {
    me_range = (me_end - me_start) / -nStep;
  }
  int64_t range = me_range.GetConstant();
  void *r_var = me_range.get_expr_from_coeff();
  void *iterator_end = nullptr;
  void *iterator_lb = nullptr;
  void *iterator_ub = nullptr;

  if ((me_lb.IsConstant() == 0) || (me_ub.IsConstant() == 0)) {
    iterator_lb = lb_var;
    iterator_ub = ub_var;
    iterator_end = r_var;
  } else {
    int v_type = (static_cast<SgType *>(sg_type))->variantT();
    if (IsIntegerType(sg_type) == 0) {
      v_type = V_SgTypeLong;
    }
    iterator_lb = CreateConst(&v_lb, v_type);
    iterator_ub = CreateConst(&v_ub, v_type);
    iterator_end = CreateConst(&range, v_type);
  }

  if ((iterator_end == nullptr) || (iterator_lb == nullptr) ||
      (iterator_ub == nullptr)) {
    return 0;
  }
  SgExpression *new_step = static_cast<SgExpression *>(
      CreateExp(V_SgPlusPlusOp, CreateVariableRef(sg_var), nullptr, 1));
  SgExpression *new_init_expr = static_cast<SgExpression *>(
      CreateExp(V_SgAssignOp, CreateVariableRef(sg_var), CreateConst(0), 0));
  SgExpression *new_test = static_cast<SgExpression *>(
      CreateExp(V_SgLessOrEqualOp, CreateVariableRef(sg_var), iterator_end, 0));

  SgInitializer *new_decl_init = SageBuilder::buildAssignInitializer(
      static_cast<SgExpression *>(CreateConst(0)));
  markBuild(this, new_decl_init,
            "Build in " + std::to_string(__LINE__) + " for " +
                std::to_string((int64_t)new_decl_init));

  void *sg_coeff = nullptr;
  void *ivar_exp = nullptr;
  void *end_exp = nullptr;

  int64_t coeff = nStep;
#ifdef _DEBUG_CANONICALIZE_LOOP_
  cout << "coeff: " << nStep << endl;
  cout << _p(step) << endl;
  cout << _p(sg_step) << endl;
  cout << "direction: " << inc_dir << endl;
#endif

  if (V_SgPlusAssignOp == op) {
    sg_coeff = CreateConst(coeff);
    //  if (!IsLongLongType(sg_type))
    //  sg_coeff = CreateCastExp(sg_coeff, sg_type);

    ivar_exp = CreateExp(V_SgMultiplyOp, sg_coeff, CreateVariableRef(sg_var));
    if (nStep > 0) {
      ivar_exp = CreateExp(V_SgAddOp, iterator_lb, ivar_exp);
      end_exp = CreateExp(V_SgAddOp, iterator_ub, CopyExp(sg_coeff));
    } else {
      ivar_exp = CreateExp(V_SgAddOp, iterator_ub, ivar_exp);
      end_exp = CreateExp(V_SgAddOp, iterator_lb, CopyExp(sg_coeff));
    }
  } else if (V_SgMinusAssignOp == op || V_SgMinusMinusOp == op) {
    if (V_SgMinusMinusOp == op) {
      sg_coeff = CreateConst(-1);
    } else {
      sg_coeff = CreateConst(-coeff);
    }

    //  if (!IsLongLongType(sg_type))
    //  sg_coeff = CreateCastExp(sg_coeff, sg_type);

    ivar_exp = CreateExp(V_SgMultiplyOp, sg_coeff, CreateVariableRef(sg_var));

    if (nStep < 0) {
      ivar_exp = CreateExp(V_SgAddOp, iterator_lb, ivar_exp);
      end_exp = CreateExp(V_SgAddOp, iterator_ub, CopyExp(sg_coeff));
    } else {
      ivar_exp = CreateExp(V_SgAddOp, iterator_ub, ivar_exp);
      end_exp = CreateExp(V_SgAddOp, iterator_lb, CopyExp(sg_coeff));
    }
  }
  if (ivar_exp == nullptr) {
    return 0;
  }
  SgForInitStatement *init_stmt = sg_for_loop->get_for_init_stmt();
  SgExprStatement *sg_expr_stmt =
      isSgExprStatement(init_stmt->get_init_stmt()[0]);
  SgVariableDeclaration *sg_decl_stmt =
      isSgVariableDeclaration(init_stmt->get_init_stmt()[0]);

  string new_var_name = "_in_" + UnparseToString(sg_var);
  get_valid_local_name(func_decl, &new_var_name);
  SgVariableDeclaration *new_decl = static_cast<SgVariableDeclaration *>(
      CreateVariableDecl(sg_type, new_var_name, ivar_exp, sg_body));

  if (IsBasicBlock(sg_body) == 0) {  //  If loop body is a stmt
    InsertStmt(new_decl, sg_body);
  } else {
    PrependChild(sg_body, new_decl);
  }

//  #ifdef PROJDEBUG
#ifdef _DEBUG_CANONICALIZE_LOOP_
  cout << _p(new_decl) << endl;
#endif
  SgInitializedName *new_init =
      static_cast<SgInitializedName *>(GetVariableInitializedName(new_decl));

//  Replace the variable references to the original iterator with the new one
#if 1
  Rose_STL_Container<SgNode *> varList =
      NodeQuery::querySubTree(static_cast<SgNode *>(sg_body), V_SgVarRefExp);
  Rose_STL_Container<SgNode *>::iterator var;
  for (var = varList.begin(); var != varList.end(); var++) {
    SgExpression *ref = isSgExpression(*var);
    SgInitializedName *arr_var =
        static_cast<SgInitializedName *>(GetVariableInitializedName(ref));

    if (arr_var == static_cast<SgInitializedName *>(sg_var)) {
      if (IsInScope(ref, new_decl) != 0) {
        continue;
      }
      SgExpression *new_ref =
          static_cast<SgExpression *>(CreateVariableRef(new_init));
      replaceExpressionWithHistory(this, ref, new_ref);
    }
  }
  ReplaceExp(sg_step, new_step);
  ReplaceExp(sg_cond_expr, new_test);
  if (sg_expr_stmt != nullptr) {
    SgExpression *sg_expr = sg_expr_stmt->get_expression();
    ReplaceExp(sg_expr, new_init_expr);

    //  If it is an iterator declared outside,
    //  the final value needs to be assigned back to the iterator.
    void *new_stmt =
        CreateStmt(V_SgAssignStatement, CreateVariableRef(sg_var), end_exp);
    InsertAfterStmt(new_stmt, sg_for_loop);

  } else {
    DeleteNode(end_exp);
    if (sg_decl_stmt != nullptr && sg_var != nullptr) {
      SgInitializer *old_init = sg_var->get_initializer();
      ReplaceExp(old_init, new_decl_init);
    }
  }
#endif

  AddComment("Standardize from: " + str_org_loop, sg_for_loop);

  return 1;
}

int CSageCodeGen::IsCanonicalForLoop(void *for_loop, void **ivar, void **lb,
                                     void **ub, void **step, void **body,
                                     bool *hasIncrementalIterationSpace,
                                     bool *isInclusiveUpperBound) {
  if (nullptr == lb && nullptr == ub && nullptr == step && nullptr == body &&
      nullptr == hasIncrementalIterationSpace &&
      nullptr == isInclusiveUpperBound) {
    if (s_loop_iterator_cache.count(for_loop) > 0) {
      void *iter = s_loop_iterator_cache[for_loop];
      if (ivar != nullptr) {
        *ivar = iter;
      }
      return static_cast<int>(iter != nullptr);
    }
  }
  SgNode *i_loop = static_cast<SgNode *>(for_loop);
  void *i_ivar = nullptr;
  void *i_lb = nullptr;
  void *i_ub = nullptr;
  void *i_step = nullptr;
  void *i_body = nullptr;
  if (ivar != nullptr) {
    *ivar = nullptr;
  }
  if (lb != nullptr) {
    *lb = nullptr;
  }
  if (ub != nullptr) {
    *ub = nullptr;
  }
  if (step != nullptr) {
    *step = nullptr;
  }
  if (body != nullptr) {
    *body = nullptr;
  }
  //  youxiang 2017-03-06 we can support for-loop currently
  if (IsForStatement(for_loop) == 0) {
    s_loop_iterator_cache[for_loop] = nullptr;
    return 0;
  }

  if (hasLabelWithGoto(for_loop) != 0) {
    s_loop_iterator_cache[for_loop] = nullptr;
    return 0;
  }

  bool i_hasIncrementalIterationSpace = false;
  bool i_isInclusiveUpperBound = false;
  if ((IsExprStatement(GetLoopTest(for_loop)) == 0) ||
      (GetLoopInit(for_loop) == nullptr) ||
      (GetLoopIncrementExpr(for_loop) == nullptr)) {
    s_loop_iterator_cache[for_loop] = nullptr;
    return 0;
  }
  int ret = static_cast<int>(isCanonicalForLoop(
      i_loop, &i_ivar, &i_lb, &i_ub, &i_step, &i_body,
      &i_hasIncrementalIterationSpace, &i_isInclusiveUpperBound));
  if ((ret != 0) && (has_write_in_scope_fast(i_ivar, i_body) != 0)) {
    s_loop_iterator_cache[for_loop] = nullptr;
    return 0;
  }

  if (ivar != nullptr) {
    *ivar = i_ivar;
  }
  if (lb != nullptr) {
    *lb = i_lb;
  }
  if (ub != nullptr) {
    *ub = i_ub;
  }
  if (step != nullptr) {
    *step = i_step;
  } else if ((i_step != nullptr) && (GetParent(i_step) == nullptr)) {
    DeleteNode(i_step);
  }
  if (body != nullptr) {
    *body = i_body;
  }
  if (isInclusiveUpperBound != nullptr) {
    *isInclusiveUpperBound = i_isInclusiveUpperBound;
  }
  if (hasIncrementalIterationSpace != nullptr) {
    *hasIncrementalIterationSpace = i_hasIncrementalIterationSpace;
  }

  s_loop_iterator_cache[for_loop] = i_ivar;
  return ret;
}

//  this function only save on Canonical for loop
int CSageCodeGen::GetLoopStepValueFromExpr(void *step_expr, int64_t *val,
                                           void **sg_value) {
  if (isSgIntVal(static_cast<SgNode *>(step_expr)) != nullptr) {
    *val = isSgIntVal(static_cast<SgNode *>(step_expr))->get_value();
    if (sg_value != nullptr) {
      *sg_value = step_expr;
    }
    return 1;
  }
  if (isSgPlusPlusOp(static_cast<SgNode *>(step_expr)) != nullptr) {
    *val = 1;
    if (sg_value != nullptr) {
      void *uni = GetExpUniOperand(step_expr);
      int64_t step = 1;
      void *type = GetTypeByExp(uni);
      int int_type = (static_cast<SgType *>(type))->variantT();
      if (IsPointerType(type) != 0) {
        int_type = V_SgTypeUnsignedLong;
      }
      *sg_value = CreateConst(&step, int_type);
    }
    return 1;
  }
  if (isSgMinusMinusOp(static_cast<SgNode *>(step_expr)) != nullptr) {
    *val = -1;
    if (sg_value != nullptr) {
      void *uni = GetExpUniOperand(step_expr);
      int64_t step = -1;
      void *type = GetTypeByExp(uni);
      int int_type = (static_cast<SgType *>(type))->variantT();
      if (IsPointerType(type) != 0) {
        int_type = V_SgTypeUnsignedLong;
      }
      *sg_value = CreateConst(&step, int_type);
    }
    return 1;
  }
  if (isSgPlusAssignOp(static_cast<SgNode *>(step_expr)) != nullptr) {
    void *val_exp = GetExpRightOperand(step_expr);
    if (sg_value != nullptr) {
      *sg_value = val_exp;
    }
    CMarsExpression me_val(val_exp, this);
    if (me_val.IsConstant() != 0) {
      *val = me_val.GetConstant();
      return 1;
    }
  } else if (isSgMinusAssignOp(static_cast<SgNode *>(step_expr)) != nullptr) {
    void *val_exp = GetExpRightOperand(step_expr);
    if (sg_value != nullptr) {
      *sg_value = CreateExp(V_SgMinusOp, CopyExp(val_exp));
    }
    CMarsExpression me_val(val_exp, this);
    if (me_val.IsConstant() != 0) {
      *val = -me_val.GetConstant();
      return 1;
    }
  } else {
    if (sg_value != nullptr) {
      *sg_value = step_expr;
    }
    CMarsExpression me_val(step_expr, this);
    if (me_val.IsConstant() != 0) {
      *val = me_val.GetConstant();
      return 1;
    }
  }
  return 0;
}

int CSageCodeGen::ParseOneForLoop(void *sg_for_stmt, void **var, void **start,
                                  void **end, void **step, void **cond_op,
                                  void **body, int *ulimit) {
  SgForStatement *sg_for = isSgForStatement(static_cast<SgNode *>(sg_for_stmt));
  assert(sg_for);
  SgInitializedName *ivar = nullptr;
  SgExpression *lb = nullptr;
  SgExpression *ub = nullptr;
  SgExpression *step_ = nullptr;
  SgStatement *loop_body = nullptr;
  SgBinaryOp *cond_op_ = nullptr;
  int is_canonical = ParseForLoop(sg_for, &ivar, &lb, &ub, &step_, &cond_op_,
                                  &loop_body, ulimit);

  *var = ivar;
  *start = lb;
  *end = ub;

  /*{
    int a;
    if (!get_const_value(lb, a))
    {
  //  printf("loop lower bound %s\n", UnparseToString(lb).c_str());
  start = lb;
  return 0;
  }
  else
  {
  ;//  printf("loop lower bound %s\n", UnparseToString(lb).c_str());
  }
  start = CreateConst(&a);
  }

  //  end = ub; //  to support constant expression
  {
  int a;
  if (!get_const_value(ub, a))
  {
  //  printf("loop upper bound %s\n", UnparseToString(ub).c_str());
  end = ub;
  return 0;
  }
  else
  {
  ;//  printf("loop upper bound %s\n", UnparseToString(ub).c_str());
  }
  end = CreateConst(&a);
  }*/

  *step = step_;
  *cond_op = cond_op_;
  *body = loop_body;

  return is_canonical;
}

void *CSageCodeGen::CreateWhileLoop(void *sg_cond_exp_, void *sg_body_,
                                    void *bindNode /*= nullptr*/) {
#ifdef PROJDEBUG
  assert(sg_cond_exp_ != nullptr && is_floating_node(sg_cond_exp_));
  assert(sg_body_ != nullptr && is_floating_node(sg_body_));
#endif
  SgExpression *sg_cond_exp =
      isSgExpression(static_cast<SgNode *>(sg_cond_exp_));
  SgStatement *sg_body = isSgStatement(static_cast<SgNode *>(sg_body_));

  auto sg_new_node = SageBuilder::buildWhileStmt(sg_cond_exp, sg_body);
  markBuild(this, sg_new_node,
            "Build in " + std::to_string(__LINE__) + " for " +
                std::to_string((int64_t)sg_new_node),
            bindNode);
  return sg_new_node;
}

void *CSageCodeGen::CreateForLoop(void *sg_init_, void *sg_test_,
                                  void *sg_incr_exp_, void *sg_body_,
                                  void *bindNode /*= nullptr*/) {
#ifdef PROJDEBUG
  assert(sg_init_ != nullptr && is_floating_node(sg_init_));
  assert(sg_test_ != nullptr && is_floating_node(sg_test_));
  assert(sg_incr_exp_ != nullptr && is_floating_node(sg_incr_exp_));
  assert(sg_body_ != nullptr && is_floating_node(sg_body_));
#endif
  SgStatement *sg_init = isSgStatement(static_cast<SgNode *>(sg_init_));
  SgStatement *sg_test = isSgStatement(static_cast<SgNode *>(sg_test_));
  SgExpression *sg_incr_exp =
      isSgExpression(static_cast<SgNode *>(sg_incr_exp_));
  SgStatement *sg_body = isSgStatement(static_cast<SgNode *>(sg_body_));
  auto sg_new_node =
      SageBuilder::buildForStatement(sg_init, sg_test, sg_incr_exp, sg_body);
  markBuild(this, sg_new_node,
            "Build in " + std::to_string(__LINE__) + " for " +
                std::to_string((int64_t)sg_new_node),
            bindNode);
  return sg_new_node;
}

void *CSageCodeGen::CreateIfStmt(void *sg_cond_exp_, void *sg_if_,
                                 void *sg_else_, void *bindNode /*= nullptr*/) {
#ifdef PROJDEBUG
  assert(sg_else_ == nullptr || is_floating_node(sg_else_));
  assert(sg_if_ != nullptr && is_floating_node(sg_if_));
  assert(sg_cond_exp_ != nullptr && is_floating_node(sg_cond_exp_));
#endif
  SgStatement *sg_if = isSgStatement(static_cast<SgNode *>(sg_if_));
  SgStatement *sg_else = isSgStatement(static_cast<SgNode *>(sg_else_));

  SgNode *sg_new_node;
  if (isSgExpression(static_cast<SgNode *>(sg_cond_exp_)) != nullptr) {
    SgExpression *sg_cond_exp =
        isSgExpression(static_cast<SgNode *>(sg_cond_exp_));
    sg_new_node = SageBuilder::buildIfStmt(sg_cond_exp, sg_if, sg_else);

  } else {
    SgStatement *sg_cond_stmt =
        isSgStatement(static_cast<SgNode *>(sg_cond_exp_));
    sg_new_node = SageBuilder::buildIfStmt(sg_cond_stmt, sg_if, sg_else);
  }
  markBuild(this, sg_new_node,
            "Build in " + std::to_string(__LINE__) + " for " +
                std::to_string((int64_t)sg_new_node),
            bindNode);
  return sg_new_node;
}

void *CSageCodeGen::GetIfStmtTrueBody(void *sg_if) {
  SgIfStmt *sg_stmt = isSgIfStmt(static_cast<SgNode *>(sg_if));
  if (sg_stmt == nullptr) {
    return nullptr;
  }
  { return sg_stmt->get_true_body(); }
}
#if 0
void CSageCodeGen::SetIfStmtTrueBody(void *sg_if, void *new_body) {
  SgIfStmt *sg_stmt = isSgIfStmt(static_cast<SgNode *>(sg_if));
  if (!sg_stmt)
    return;
  sg_stmt->set_true_body(isSgStatement(static_cast<SgNode *>(new_body)));
  return;
}
#endif
void *CSageCodeGen::GetIfStmtCondition(void *sg_if) {
  SgIfStmt *sg_stmt = isSgIfStmt(static_cast<SgNode *>(sg_if));
  if (sg_stmt == nullptr) {
    return nullptr;
  }
  { return sg_stmt->get_conditional(); }
}
void *CSageCodeGen::GetIfStmtFalseBody(void *sg_if) {
  SgIfStmt *sg_stmt = isSgIfStmt(static_cast<SgNode *>(sg_if));
  if (sg_stmt == nullptr) {
    return nullptr;
  }
  { return sg_stmt->get_false_body(); }
}

void CSageCodeGen::SetIfStmtFalseBody(void *sg_if, void *new_body) {
  SgIfStmt *sg_stmt = isSgIfStmt(static_cast<SgNode *>(sg_if));
  if (sg_stmt == nullptr) {
    return;
  }
  { sg_stmt->set_false_body(isSgStatement(static_cast<SgNode *>(new_body))); }
}
void *CSageCodeGen::CreatePragma(const string &sPragma, void *scope_,
                                 void *bindNode /*= nullptr*/) {
  SgScopeStatement *scope = isSgScopeStatement(static_cast<SgNode *>(scope_));
  SgPragmaDeclaration *sg_pragma =
      SageBuilder::buildPragmaDeclaration(sPragma, scope);
  markBuild(this, sg_pragma,
            "Build in " + std::to_string(__LINE__) + " for " +
                std::to_string((int64_t)sg_pragma),
            bindNode);
  return sg_pragma;
}

void *CSageCodeGen::GetEnclosingNode(const string &type_, void *sg_node_) {
  SgNode *sg_node = static_cast<SgNode *>(sg_node_);
  if ("BasicBlock" == type_) {
    return SageInterface::getEnclosingNode<SgBasicBlock>(sg_node);
  }
  if ("Statement" == type_) {
    //  if (0) printf("debug: %s", UnparseToString(sg_node).c_str());
    return SageInterface::getEnclosingNode<SgStatement>(sg_node);
  }
  if ("ForLoop" == type_) {
    return SageInterface::getEnclosingNode<SgForStatement>(sg_node);
  }
  if ("While" == type_) {
    return SageInterface::getEnclosingNode<SgWhileStmt>(sg_node);
  }
  if ("DoWhile" == type_) {
    return SageInterface::getEnclosingNode<SgDoWhileStmt>(sg_node);
  }
  if ("Function" == type_) {
    return SageInterface::getEnclosingNode<SgFunctionDeclaration>(sg_node);
  }
  if ("FunctionCallExp" == type_) {
    return SageInterface::getEnclosingNode<SgFunctionCallExp>(sg_node);
  } else if ("Switch" == type_) {
    return SageInterface::getEnclosingNode<SgSwitchStatement>(sg_node);
  } else if ("CompoundAssignOp" == type_) {
    SgBinaryOp *exp = SageInterface::getEnclosingNode<SgBinaryOp>(sg_node);
    if (IsBinaryAssign(exp) != 0) {
      return exp;
    } else {
      return nullptr;
    }
  } else if ("UnaryOp" == type_) {
    return SageInterface::getEnclosingNode<SgUnaryOp>(sg_node);
  } else if ("PntrArrRefExp" == type_) {
    return SageInterface::getEnclosingNode<SgPntrArrRefExp>(sg_node);
  }

#ifdef PROJDEBUG
  assert(0);
#endif
  //  Yuxin: Sep 3, should not assertout if no branch for the input type
  return nullptr;
}

void *CSageCodeGen::CreateSourceFile(const string &sFileName,
                                     void *bindNode /*= nullptr*/) {
  SgSourceFile *new_file = isSgSourceFile(SageBuilder::buildFile(
      sFileName, sFileName, static_cast<SgProject *>(GetProject())));

  markBuild(this, new_file,
            "Build in " + std::to_string(__LINE__) + " for " +
                std::to_string((int64_t)new_file),
            bindNode);
  markInsert(this, new_file->get_parent(), getTopoIndex(this, new_file),
             "Insert file: " + sFileName);
  return new_file->get_globalScope();
}

#define MAX_INPUT_FILES 100
char *g_rose_argv[MAX_INPUT_FILES];
char g_rose_argvv[MAX_INPUT_FILES][1024];

//  20180705 shanh some special process with filename
//  need to clean
void *CSageCodeGen::OpenSourceFile(vector<string> sFileName,
                                   string cflags /*=""*/) {
  //  cout << "cflags = \'" << cflags << "\'" << endl;
  DEFINE_START_END;
  STEMP(start);

  //  /////////////////  /
  //  ZP: 20171004: A workaround of issue that  multiple -I are considered as
  //  one file
  {
    int org_size = sFileName.size();

    int curr_idx = 0;
    for (auto i = 0; i < org_size; i++) {
      string one_file = sFileName[curr_idx];

      if (one_file.find("-I") != 0 || one_file.find(" -I") == string::npos) {
        curr_idx++;  //  nothing changed
      } else {
        //  like '-I xxx -I yyy -I zzz'
        vector<string> sub = str_split(one_file, " ");
        sFileName[curr_idx] = "";
        curr_idx++;
        for (auto one_inc : sub) {
          sFileName.insert(sFileName.begin() + curr_idx, one_inc);
          curr_idx++;
        }
      }
    }
  }

  //  /////////////////  /

  size_t i;
  m_inputFiles.clear();
  m_inputIncDirs.clear();
  m_inputMacros.clear();
  char *curr_dir_c_str = get_current_dir_name();
  assert(curr_dir_c_str);
  string curr_dir_str(curr_dir_c_str);
  for (i = 0; i < sFileName.size(); i++) {
    string file_name = sFileName[i];
    if (file_name == "-I" && i + 1 < sFileName.size()) {
      string dir_name = sFileName[i + 1];
      if (boost::filesystem::exists(dir_name)) {
        m_inputIncDirs.insert(boost::filesystem::canonical(dir_name).string());
      }
      ++i;
      continue;
    }
    if (file_name.find("-I") == 0) {
      string dir_name = file_name.substr(2);
      if (boost::filesystem::exists(dir_name)) {
        m_inputIncDirs.insert(boost::filesystem::canonical(dir_name).string());
      }
      continue;
    }

    if (file_name == "-D" && i + 1 < sFileName.size()) {
      string macro_def = sFileName[i + 1];
      vector<string> res;
      splitString(macro_def, "=", &res);
      if (res.size() == 1) {
        m_inputMacros[res[0]] = "";
      } else if (res.size() == 2) {
        m_inputMacros[res[0]] = res[1];
      }
      ++i;
      continue;
    }
    if (file_name.find("-D") == 0) {
      string macro_def = file_name.substr(2);
      vector<string> res;
      splitString(macro_def, "=", &res);
      if (res.size() == 1) {
        m_inputMacros[res[0]] = "";
      } else if (res.size() == 2) {
        m_inputMacros[res[0]] = res[1];
      }
      continue;
    }
    if (file_name.find("-") == 0) {
      continue;
    }

    string one_dir = curr_dir_str;
    string::size_type pos = file_name.find_last_of("/");
    if (pos != string::npos) {
      string relative_dir = file_name.substr(0, pos);
      if (relative_dir[0] == '/') {
        one_dir = relative_dir;
      } else {
        one_dir += "/" + relative_dir;
      }
      file_name = file_name.substr(pos + 1);
    }
    if (is_c_file(file_name) || is_cpp_file(file_name)) {
      if (boost::filesystem::exists(one_dir)) {
        m_inputIncDirs.insert(boost::filesystem::canonical(one_dir).string());
      }
      m_inputFiles.insert(file_name);
    }
  }
  free(curr_dir_c_str);

  string s_empty = " ";
  for (i = 0; i < MAX_INPUT_FILES; ++i) {
    g_rose_argvv[i][0] = '\0';
    g_rose_argv[i] = g_rose_argvv[i];
  }

  int argc = 0;
  auto add_arg = [&](const std::string &s) {
    snprintf(g_rose_argvv[argc], s.size() + 1, "%s", s.c_str());
    g_rose_argv[argc] = g_rose_argvv[argc];
    argc++;
  };
  add_arg("dummy");

  auto add_xilinx_lib = [&]() {
    ////////////////////////////////////////////////////////////////////////////////////
    // ZP: 2019-11-21 Move this part above to cover user include path (refer to
    // JIRA MER-1456)
    string xilinx_path = getenv("MERLIN_COMPILER_HOME") +
                         string("/source-opt/include/apint_include");
    if (test_dir_existence(xilinx_path) != 0) {
      xilinx_path = "-I" + xilinx_path;
      add_arg(xilinx_path);
      DEBUG(cout << xilinx_path << endl);
    } else {
      DEBUG(cout << xilinx_path << " not found " << endl);
    }
    ////////////////////////////////////////////////////////////////////////////////////
  };

  bool dash = false;
  assert(sFileName.size() < MAX_INPUT_FILES);
  for (i = 0; i < sFileName.size(); i++) {
    string input = sFileName[i];
    if (!dash && input.find('-') == 0) {
      add_arg("--");
      add_xilinx_lib();
      dash = true;
    }
    if (input == "--") {
      continue;
    }
    add_arg(input);
  }
  if (!dash) {
    add_arg("--");
    add_xilinx_lib();
  }
  //  add_arg("-rose:log none");
  add_arg("-I./");
  add_arg("-D_MARS_COMPILER_FLOW_");
  add_arg("-D__SYNTHESIS__");
  vector<string> vec_flags = split_flags(cflags);
  for (auto flag : vec_flags) {
    if (flag.empty()) {
      continue;
    }
    if ("--" == flag) {
      continue;
    }
    if (flag.find("cstd=") == 0) {
      string cstd = flag.substr(string("cstd=").size());
      if (!cstd.empty())
        add_arg("-std=" + cstd);
      continue;
    }
    if (flag.find("cxxstd=") == 0) {
      string cxxstd = flag.substr(string("cxxstd=").size());
      if (!cxxstd.empty())
        add_arg("-std=" + cxxstd);
      continue;
    }
    add_arg(flag);
  }
  add_arg("-w");
  //  //////////////////////  /
#if 1
  //  add gcc4.9.4 system header files path
  string gcc_path =
      getenv("MERLIN_COMPILER_HOME") + string("/source-opt/lib/gcc4.9.4");
  string gcc_include_path = "-I" + gcc_path + "/include/c++/4.9.4";
  add_arg(gcc_include_path);

  gcc_include_path =
      "-I" + gcc_path + "/include/c++/4.9.4/x86_64-unknown-linux-gnu";
  add_arg(gcc_include_path);

  gcc_include_path = "-I" + gcc_path + "/include/c++/4.9.4/backward";
  add_arg(gcc_include_path);

  gcc_include_path =
      "-I" + gcc_path + "/lib/gcc/x86_64-unknown-linux-gnu/4.9.4/include";
  add_arg(gcc_include_path);

  gcc_include_path =
      "-I" + gcc_path + "/lib/gcc/x86_64-unknown-linux-gnu/4.9.4/include-fixed";
  add_arg(gcc_include_path);

  gcc_include_path = "-I" + gcc_path + "/include";
  add_arg(gcc_include_path);
#endif
  //  set rose option? needreview
  vector<string> vecOutlineArgs;
  vecOutlineArgs.push_back("-rose:outline:new_file");
  Outliner::commandLineProcessing(vecOutlineArgs);
  Outliner::enable_classic = true;
  //  Outliner::temp_variable  = true;  //  there is a functional bug when on
  Outliner::enable_liveness = true;
  //  Outliner::enable_debug= true;

  if (IsMixedCAndCPlusPlusDesign() != 0) {
    dump_critical_message(SYNCHK_ERROR_3);
    return NULL;
  }
  //  for (int i = 0; i < argc; ++i)
  //  cout << g_rose_argv[i] << endl;
  ACCTM(before_frontend, start, end);
  STEMP(start);
  SgProject *project = frontend(argc, g_rose_argv);
  ACCTM(frontend, start, end);
  m_sg_project = project;
  STEMP(start);
  sanity_check();
  ACCTM(sanity_check, start, end);
  STEMP(start);
#if _DEBUG_DEFUSE
  if (!m_defuse || !m_liveness) {
    init_defuse();
  }
#endif
//  int num_file = m_sg_project->numberOfFiles();
//  for (int i = 0; i != num_file ; ++i) {
//  SgFile *sg_file = (*m_sg_project)[i];
//  sg_file->get_file_info()->setOutputInCodeGeneration();
//  sg_file->set_unparse_includes(true);
//  sg_file->set_unparseHeaderFiles(true);
//  sg_file->set_unparse_line_directives(true);
//  }
//  ZP: 20160812: call graph is not stable, so we moved to here so that it can
//  be turned off
//  if (!m_callgraph) {
//  init_callgraph();
//  }
#if _INITIAL_RANGE_ANALYSIS_
  reset_range_analysis();
#else
  MarsProgramAnalysis::m_init_range = 0;
#endif
  //  YX: 20171003 resetting incorrect scope to correct parent will result in
  //  assertion failure in backend
  //  fix_scope_issue(nullptr);
  // remove builtin global declaration
  int num_file = m_sg_project->numberOfFiles();
  for (int i = 0; i != num_file; ++i) {
    SgSourceFile *sg_file = isSgSourceFile((*m_sg_project)[i]);
    if (!sg_file)
      continue;
    SgGlobal *sg_scope = sg_file->get_globalScope();
    if (!sg_scope)
      continue;
    SgDeclarationStatementPtrList &decl_lst = sg_scope->get_declarations();
    auto it = decl_lst.begin();
    while (it != decl_lst.end()) {
      if (get_file(*it) != "rose_edg_required_macros_and_functions.h")
        break;
      it++;
    }
    decl_lst.erase(decl_lst.begin(), it);
  }

  ACCTM(after_frontend, start, end);
  return project;
}

void *CSageCodeGen::GetGlobal(int i) {
  if (i >= GetGlobalNum() || i < 0) {
    return nullptr;
  }
  SgSourceFile *sg_file = isSgSourceFile((*m_sg_project)[i]);
  assert(sg_file);
  SgGlobal *sg_global = sg_file->get_globalScope();
  return sg_global;
}

void *CSageCodeGen::GetGlobal(void *sg_pos) { return TraceUpToGlobal(sg_pos); }

vector<void *> CSageCodeGen::GetGlobals() {
  vector<void *> ret;
  for (int i = 0; i < GetGlobalNum(); i++) {
    ret.push_back(GetGlobal(i));
  }
  return ret;
}

void CSageCodeGen::InitBuiltinTypes() {
  m_builtin_types.clear();
  SgType *sg_type;

  sg_type = SageBuilder::buildBoolType();
  m_builtin_types["bool"] = sg_type;

  sg_type = SageBuilder::buildCharType();
  m_builtin_types["char"] = sg_type;
  m_builtin_types["byte"] = sg_type;
  sg_type = SageBuilder::buildSignedCharType();
  m_builtin_types["signed char"] = sg_type;
  sg_type = SageBuilder::buildUnsignedCharType();
  m_builtin_types["unsigned char"] = sg_type;
  sg_type = SageBuilder::buildWcharType();
  m_builtin_types["wchar_t"] = sg_type;
  sg_type = SageBuilder::buildShortType();
  m_builtin_types["short"] = sg_type;
  sg_type = SageBuilder::buildSignedShortType();
  m_builtin_types["signed short"] = sg_type;
  sg_type = SageBuilder::buildUnsignedShortType();
  m_builtin_types["unsigned short"] = sg_type;
  sg_type = SageBuilder::buildIntType();
  m_builtin_types["int"] = sg_type;
  sg_type = SageBuilder::buildSignedIntType();
  m_builtin_types["signed int"] = sg_type;
  sg_type = SageBuilder::buildUnsignedIntType();
  m_builtin_types["unsigned int"] = sg_type;
  sg_type = SageBuilder::buildLongType();
  m_builtin_types["long"] = sg_type;
  sg_type = SageBuilder::buildSignedLongType();
  m_builtin_types["signed long"] = sg_type;
  sg_type = SageBuilder::buildUnsignedLongType();
  m_builtin_types["unsigned long"] = sg_type;
  sg_type = SageBuilder::buildLongLongType();
  m_builtin_types["long long"] = sg_type;
  sg_type = SageBuilder::buildSignedLongLongType();
  m_builtin_types["signed long long"] = sg_type;
  sg_type = SageBuilder::buildUnsignedLongLongType();
  m_builtin_types["unsigned long long"] = sg_type;
  sg_type = SageBuilder::buildSigned128bitIntegerType();
  m_builtin_types["__int128"] = sg_type;
  m_builtin_types["signed __int128"] = sg_type;
  sg_type = SageBuilder::buildUnsigned128bitIntegerType();
  m_builtin_types["unsigned __int128"] = sg_type;
  sg_type = SageBuilder::buildFloatType();
  m_builtin_types["float"] = sg_type;
  sg_type = SageBuilder::buildDoubleType();
  m_builtin_types["double"] = sg_type;
  sg_type = SageBuilder::buildLongDoubleType();
  m_builtin_types["long double"] = sg_type;
  sg_type = SageBuilder::buildVoidType();
  m_builtin_types["void"] = sg_type;
  //  Ellipse type ...
  sg_type = SgTypeEllipse::createType();
  m_builtin_types["..."] = sg_type;

  //  C bool type
  RegisterType("_Bool");
#if 0
  //  uint1 - uint100
  for (i = 1; i <= 512; i++) {
    string sType = "uint" + my_itoa(i);
    RegisterType(sType);
  }

  //  int1 - int100
  for (i = 1; i <= 512; i++) {
    string sType = "int" + my_itoa(i);
    RegisterType(sType);
  }

  //  ap_uint
  for (i = 1; i <= 512; i++) {
    string sType = "ap_uint<" + my_itoa(i) + ">";
    RegisterType(sType);
  }
#endif
}

int CSageCodeGen::IsLocalInitName(void *sg_node) {
  int ret = static_cast<int>(
      (static_cast<int>(IsInitName(sg_node) != 0) != 0) &&
      (IsGlobalInitName(sg_node) == 0) && (IsArgumentInitName(sg_node) == 0) &&
      !IsMemberVariable(sg_node));
  return ret;
}

int CSageCodeGen::IsGlobalInitName(void *sg_node) {
  if (IsArgumentInitName(sg_node) != 0) {
    return 0;
  }

  SgInitializedName *sg_name =
      isSgInitializedName(static_cast<SgNode *>(sg_node));
  if (sg_name == nullptr) {
    return 0;
  }
  SgDeclarationStatement *sg_decl = sg_name->get_declaration();
  if (sg_decl == nullptr) {
    return 0;
  }
  if (IsStatic(sg_decl) && IsMemberVariable(sg_name)) {
    return 1;
  }
  if (IsExtern(sg_decl)) {
    return 1;
  }
  void *sg_global = GetGlobal(sg_node);
  void *sg_scope = GetScope(sg_node);
  //  return sg_scope == sg_global && !sg_decl->isForward();
  return static_cast<int>(sg_scope == sg_global);
}
int CSageCodeGen::IsArgumentInitName(void *sg_node) {
  SgInitializedName *sg_name =
      isSgInitializedName(static_cast<SgNode *>(sg_node));
  if (sg_name == nullptr) {
    return 0;
  }
  SgDeclarationStatement *sg_decl = sg_name->get_declaration();
  if (sg_decl == nullptr) {
    return 0;
  }
  SgFunctionParameterList *fpl = isSgFunctionParameterList(sg_decl);
  if (fpl == nullptr) {
    return 0;
  }

  return 1;
}

void *CSageCodeGen::CreateCastExp(void *sg_exp, const string &s_type,
                                  void *bindNode /*= nullptr*/) {
  SgType *sg_type = isSgType(static_cast<SgNode *>(GetTypeByString(s_type)));
  if (sg_type == nullptr) {
    return nullptr;
  }
  return CreateExp(V_SgCastExp, sg_exp, sg_type, 0, bindNode);
}

void *CSageCodeGen::CreateCastExp(void *sg_exp, void *sg_type,
                                  void *bindNode /*= nullptr*/) {
  return CreateExp(V_SgCastExp, sg_exp, sg_type, 0, bindNode);
}

int CSageCodeGen::AddCastToExp(void *sg_exp, void *sg_type) {
  void *sg_new_exp = CreateExp(V_SgCastExp, CopyExp(sg_exp), sg_type);

  ReplaceExp(sg_exp, sg_new_exp);

  return 1;
}

void *CSageCodeGen::RemoveCast(void *sg_exp) {
  if (IsCastExp(sg_exp) == 0) {
    return sg_exp;
  }
  return RemoveCast(GetExpUniOperand(sg_exp));
}

//  detect static_cast<int *>()(0)
int CSageCodeGen::IsNullPointerCast(void *sg_exp) {
  if (IsCastExp(sg_exp) == 0) {
    return 0;
  }
  void *orig_exp = GetExpUniOperand(sg_exp);
  while (IsCastExp(orig_exp) != 0) {
    void *base_type = GetBaseType(GetTypeByExp(orig_exp), true);
    if (IsVoidType(base_type) == 0) {
      return 0;
    }
    orig_exp = GetExpUniOperand(orig_exp);
  }
  int64_t value = -1;
  if (IsConstantInt(orig_exp, &value, true, nullptr)) {
    if (0 == value) {
      return 1;
    }
  }
  return 0;
}

void *CSageCodeGen::GetTypeByString(const string &sType_) {
  string sType = sType_;
  if (m_builtin_types.end() != m_builtin_types.find(sType)) {
    return m_builtin_types[sType];
  }
  if (sType == "signed char" || sType == "bool" || sType == "unsigned char" ||
      sType == "char" || sType == "wchar_t" || sType == "short" ||
      sType == "signed short" || sType == "unsigned short" ||
      sType == "signed int" || sType == "unsigned int" || sType == "int" ||
      sType == "double" || sType == "long double" || sType == "float" ||
      sType == "signed long" || sType == "long" || sType == "unsigned long" ||
      sType == "void" || sType == "..." || sType == "unsigned long long" ||
      sType == "long long" || sType == "signed long long" ||
      sType == "__int128" || sType == "signed __int128" ||
      sType == "unsigned __int128") {
    printf("[CSageCodeGen] unregistered native type: %s. "
           "Please add native type into m_builtin_types first\n",
           sType.c_str());
    return nullptr;
  }

  //  strip global namespace from Rose
  if (sType.find("::") == 0) {
    sType = sType_.substr(2);
  }
  size_t j = 0;
  string sPrefix = get_sub_delimited(sType, j, ' ');
  j += sPrefix.size() + 1;

  //  OpenCL qualifier
  if (sPrefix == "__global" || sPrefix == "__kernel" ||
      sPrefix == "__constant") {
#ifdef PROJDEBUG
    printf("[CSageCodeGen] unregistered OpenCL type: %s. "
           "Please use RegisterType(sType) first.\n",
           sType.c_str());
#endif
    return nullptr;
  }

  size_t j0 = j;
  size_t j1 = j;

  string sSuffix;
  while (j < sType.size()) {
    j1 = j;
    sSuffix = get_sub_delimited(sType, j, ' ');
    j += sSuffix.size() + 1;
  }

  if (!sSuffix.empty() && sSuffix[0] == '[') {  //  array
    string sSubType = sType.substr(0, j1 - 1);

    string sSize = get_sub_delimited(sSuffix, 1, ']');
    auto sg_index = SageBuilder::buildUnsignedLongVal(atoi(sSize.c_str()));
    //  markBuild(* this, sg_index, "Build in " + std::to_string(__LINE__) + "
    //  for "
    //  + std::to_string((int64_t) sg_index));
    void *base_type = GetTypeByString(sSubType);
    if (base_type != nullptr) {
#if 0
      auto sg_type = SageBuilder::buildArrayType(
          isSgType(static_cast<SgNode *>(base_type)), sg_index);
#else

      auto sg_type =
          new SgArrayType(isSgType(static_cast<SgNode *>(base_type)), sg_index);
      m_new_types.insert(sg_type);
#endif
      //  markBuild(* this, sg_type, "Build in " + std::to_string(__LINE__) + "
      //  for "
      //  + std::to_string((int64_t) sg_type));
      return sg_type;
    }
  } else if (sSuffix == "*") {
    string sSubType = sType.substr(0, j1 - 1);
    void *base_type = GetTypeByString(sSubType);
    if (base_type != nullptr) {
#if 0
      auto sg_type = SageBuilder::buildPointerType(
          isSgType(static_cast<SgNode *>(base_type)));
#else
      auto sg_type =
          new SgPointerType(isSgType(static_cast<SgNode *>(base_type)));
      m_new_types.insert(sg_type);
#endif
      //  markBuild(* this, sg_type, "Build in " + std::to_string(__LINE__) + "
      //  for "
      //  + std::to_string((int64_t) sg_type));
      return sg_type;
    }
  } else if (*sType.rbegin() == '*') {
    string sSubType = sType.substr(0, sType.size() - 1);
    void *base_type = GetTypeByString(sSubType);
    if (base_type != nullptr) {
#if 0
      auto sg_type = SageBuilder::buildPointerType(
          isSgType(static_cast<SgNode *>(GetTypeByString)(sSubType)));
#else
      auto sg_type =
          new SgPointerType(isSgType(static_cast<SgNode *>(base_type)));
      m_new_types.insert(sg_type);
#endif
      //  markBuild(* this, sg_type, "Build in " + std::to_string(__LINE__) + "
      //  for "
      //  + std::to_string((int64_t) sg_type));
      return sg_type;
    }
  } else if (sPrefix == "volatile") {
    string sSubType = sType.substr(j0);
    void *base_type = GetTypeByString(sSubType);
    if (base_type != nullptr) {
#if 1
      auto sg_type = SageBuilder::buildVolatileType(
          isSgType(static_cast<SgNode *>(base_type)));
#else
      auto sg_type =
          new SgModifierType(isSgType(static_cast<SgNode *>(base_type)));
      sg_type->get_typeModifier().get_constVolatileModifier().setVolatile();
      m_new_types.insert(sg_type);
#endif
      //  markBuild(* this, sg_type, "Build in " + std::to_string(__LINE__) + "
      //  for "
      //  + std::to_string((int64_t) sg_type));
      return sg_type;
    }
  } else if (sPrefix == "static") {
    printf("type should not be static, but declaration. Use SetStatic for "
           "declaration instead.\n");
    return nullptr;
  } else if (sPrefix == "const") {
    string sSubType = sType.substr(j0);
    void *base_type = GetTypeByString(sSubType);
    if (base_type != nullptr) {
#if 1
      auto sg_type = SageBuilder::buildConstType(
          isSgType(static_cast<SgNode *>(base_type)));
#else
      auto sg_type =
          new SgModifierType(isSgType(static_cast<SgNode *>(base_type)));
      sg_type->get_typeModifier().get_constVolatileModifier().setConst();
      m_new_types.insert(sg_type);
#endif
      //  markBuild(* this, sg_type, "Build in " + std::to_string(__LINE__) + "
      //  for "
      //  + std::to_string((int64_t) sg_type));
      return sg_type;
    }
  }
  if (m_builtin_types.end() == m_builtin_types.find(sType)) {
    if (void *sg_compund_type = GetTypeByName(sType)) {
      m_builtin_types[sType] = sg_compund_type;
    } else if (sType.find("ap_int") != string::npos ||
               sType.find("ap_uint") != string::npos ||
               sType.find("ap_fixed") != string::npos ||
               sType.find("ap_ufixed") != string::npos) {
      RegisterType(sType);
    } else {
#ifdef PROJDEBUG
      cout << "WARNING: cannot get type for " << sType << endl;
// assert(0);
#endif
      return nullptr;
    }
  }
  return m_builtin_types[sType];
}

void CSageCodeGen::RegisterType(const string &sType, void *pos /*=nullptr*/) {
  assert(m_builtin_types["int"]);

  //  ZP: buildTypedefDeclaration has a 100 limit
  if (m_builtin_types.find(sType) != m_builtin_types.end()) {
    return;
  }

  void *scope = (pos) != nullptr ? GetGlobal(pos) : GetGlobal(0);

  SgTypedefDeclaration *sg_typedef = SageBuilder::buildTypedefDeclaration(
      sType, isSgType(static_cast<SgNode *>(m_builtin_types["int"])),
      isSgScopeStatement(static_cast<SgNode *>(scope)));
  //  markBuild(* this, sg_typedef, "Build in " + std::to_string(__LINE__) + "
  //  for "
  //  + std::to_string((int64_t) sg_typedef));

  //  printf("Register type: %s\n", sType.c_str());
  m_builtin_types[sType] = sg_typedef->get_type();
  m_builtin_type_decls.push_back(sg_typedef);
}

void *CSageCodeGen::CreateVariable(const string &sType, const string &sVar,
                                   void *bindNode /*= nullptr*/) {
  SgType *sg_type = isSgType(static_cast<SgNode *>(GetTypeByString(sType)));
  assert(sg_type);
  SgInitializedName *sg_param =
      SageBuilder::buildInitializedName(sVar, sg_type);
  markBuild(this, sg_param,
            "Build in " + std::to_string(__LINE__) + " for " +
                std::to_string((int64_t)sg_param),
            bindNode);
  return sg_param;
}
void *CSageCodeGen::CreateVariable(void *sg_type_, const string &sVar,
                                   void *bindNode /*= nullptr*/) {
  SgType *sg_type = isSgType(static_cast<SgNode *>(sg_type_));
  assert(sg_type);
  SgInitializedName *sg_param =
      SageBuilder::buildInitializedName(sVar, sg_type);
  markBuild(this, sg_param,
            "Build in " + std::to_string(__LINE__) + " for " +
                std::to_string((int64_t)sg_param),
            bindNode);
  return sg_param;
}

SgInitializedName *
CSageCodeGen::CreateVariable_v1(SgName sVar, void *sg_type_,
                                void *bindNode /*= nullptr*/) {
  SgType *sg_type = isSgType(static_cast<SgNode *>(sg_type_));
  assert(sg_type);
  SgInitializedName *sg_param =
      SageBuilder::buildInitializedName(sVar, sg_type);
  markBuild(this, sg_param,
            "Build in " + std::to_string(__LINE__) + " for " +
                std::to_string((int64_t)sg_param),
            bindNode);
  return sg_param;
}

//  return a SgInitializedName
void *CSageCodeGen::GetFuncParam(void *sg_func_decl_, int i) const {
  SgFunctionDeclaration *sg_func_decl =
      isSgFunctionDeclaration(static_cast<SgNode *>(sg_func_decl_));
  if (GetFuncParamNum(sg_func_decl_) <= i) {
    return nullptr;
  }
  void *sg_param = nullptr;
  sg_param = sg_func_decl->get_parameterList()->get_args()[i];
  return sg_param;
}

int CSageCodeGen::GetFuncParamNum(void *sg_func_decl_) const {
  SgFunctionDeclaration *sg_func_decl =
      isSgFunctionDeclaration(static_cast<SgNode *>(sg_func_decl_));
  if (sg_func_decl == nullptr) {
    return 0;
  }
  return sg_func_decl->get_parameterList()->get_args().size();
}

vector<void *> CSageCodeGen::GetFuncParams(void *sg_func_decl_) const {
  vector<void *> ret;
  for (int i = 0; i < GetFuncParamNum(sg_func_decl_); i++) {
    ret.push_back(GetFuncParam(sg_func_decl_, i));
  }
  return ret;
}

void *CSageCodeGen::GetFuncReturnType(void *sg_func_decl_) const {
  SgFunctionDeclaration *sg_func_decl =
      isSgFunctionDeclaration(static_cast<SgNode *>(sg_func_decl_));
  SgFunctionType *sg_func_type = sg_func_decl->get_type();
  return sg_func_type->get_return_type();
  //  return sg_func_decl->get_orig_return_type();
}

void CSageCodeGen::SetFuncReturnType(void *sg_func_decl_, void *new_type) {
  //
  SgFunctionDeclaration *sg_func_decl =
      isSgFunctionDeclaration(static_cast<SgNode *>(sg_func_decl_));
  bool is_static = false;
  bool is_extern = false;
  if (IsStatic(sg_func_decl)) {
    is_static = true;
  }
  if (IsExtern(sg_func_decl)) {
    is_extern = true;
  }
  SgFunctionType *new_return_type = SageBuilder::buildFunctionType(
      static_cast<SgType *>(new_type), sg_func_decl->get_parameterList());

  sg_func_decl->set_type(new_return_type);
  if (is_static) {
    SetStatic(sg_func_decl);
  }
  if (is_extern) {
    SetExtern(sg_func_decl);
  }
}

void *CSageCodeGen::CreateFuncParamList(const vector<void *> &param_list,
                                        void *bindNode /*= nullptr*/) {
  size_t i;

  SgFunctionParameterList *sg_fplist =
      SageBuilder::buildFunctionParameterList_nfi();
  markBuild(this, sg_fplist,
            "Build in " + std::to_string(__LINE__) + " for " +
                std::to_string((int64_t)sg_fplist),
            bindNode);
  for (i = 0; i < param_list.size(); i++) {
#ifdef PROJDEBUG
    assert(param_list[i] != nullptr && is_floating_node(param_list[i]));
#endif
    SgInitializedName *pParamDecl =
        isSgInitializedName(static_cast<SgNode *>(param_list[i]));
    assert(pParamDecl);

    sg_fplist->append_arg(pParamDecl);
  }
  return sg_fplist;
}

int CSageCodeGen::GetFuncCallParamNum(void *sg_func_call_) {
  if (SgFunctionCallExp *sg_func_call =
          isSgFunctionCallExp(static_cast<SgNode *>(sg_func_call_))) {
    return sg_func_call->get_args()->get_expressions().size();
  }
  if (SgConstructorInitializer *sg_ctor =
          isSgConstructorInitializer(static_cast<SgNode *>(sg_func_call_))) {
    return sg_ctor->get_args()->get_expressions().size();
  }
  assert(0 && "unexpected here");
  return -1;
}
void *CSageCodeGen::GetFuncCallParam(void *sg_func_call_,
                                     int i) {  //  the SgInitializedName object
  if (SgFunctionCallExp *sg_func_call =
          isSgFunctionCallExp(static_cast<SgNode *>(sg_func_call_))) {
    if (static_cast<size_t>(i) <
        sg_func_call->get_args()->get_expressions().size()) {
      return sg_func_call->get_args()->get_expressions()[i];
    }
    return nullptr;
  }
  if (auto *sg_ctor =
          isSgConstructorInitializer(static_cast<SgNode *>(sg_func_call_))) {
    if (static_cast<size_t>(i) <
        sg_ctor->get_args()->get_expressions().size()) {
      return sg_ctor->get_args()->get_expressions()[i];
    }
    return nullptr;
  }
  assert(0 && "unexpected here");
  return nullptr;
}
vector<void *> CSageCodeGen::GetFuncCallParamList(void *sg_func_call_) {
  vector<void *> ret;
  for (auto i = 0; i < GetFuncCallParamNum(sg_func_call_); i++) {
    ret.push_back(GetFuncCallParam(sg_func_call_, i));
  }
  return ret;
}
vector<void *> CSageCodeGen::CopyExprList(const vector<void *> &org_exp_list) {
  vector<void *> ret;
  for (size_t i = 0; i < org_exp_list.size(); i++) {
    assert(IsExpression(org_exp_list[i]));
    ret.push_back(CopyExp(org_exp_list[i]));
  }
  return ret;
}

void *CSageCodeGen::CreateExpList(const vector<void *> &exp_list_,
                                  void *bindNode /*= nullptr*/) {
  size_t i;
  vector<SgExpression *> exp_list;
  for (i = 0; i < exp_list_.size(); i++) {
#ifdef PROJDEBUG
    assert(exp_list_[i] != nullptr && is_floating_node(exp_list_[i]));
#endif
    SgExpression *sg_exp = isSgExpression(static_cast<SgNode *>(exp_list_[i]));
    if (sg_exp != nullptr)
      exp_list.push_back(sg_exp);
  }
  SgExprListExp *sg_list = SageBuilder::buildExprListExp(exp_list);
  markBuild(this, sg_list,
            "Build in " + std::to_string(__LINE__) + " for " +
                std::to_string((int64_t)sg_list),
            bindNode);
  return sg_list;
}

//  Add by Yuxin, May 26 2015, get the statement list in sg_scope_
//  Modified by Yuxin, Jun 28 2018, return if-stmt itself, if if-stmt is input
void CSageCodeGen::GetChildStmtList(void *sg_scope_,
                                    SgStatementPtrList *stmt_lst) const {
  //  ZP: 20160324
  if (isSgGlobal(static_cast<SgNode *>(sg_scope_)) != nullptr) {
    SgGlobal *sg_scope = isSgGlobal(static_cast<SgNode *>(sg_scope_));
    SgDeclarationStatementPtrList decl_lst = sg_scope->get_declarations();
    stmt_lst->clear();
    stmt_lst->insert(stmt_lst->end(), decl_lst.begin(), decl_lst.end());
    return;
  }
  if (isSgNamespaceDefinitionStatement(static_cast<SgNode *>(sg_scope_)) !=
      nullptr) {
    SgNamespaceDefinitionStatement *sg_scope =
        isSgNamespaceDefinitionStatement(static_cast<SgNode *>(sg_scope_));
    SgDeclarationStatementPtrList decl_lst = sg_scope->get_declarations();
    stmt_lst->clear();
    stmt_lst->insert(stmt_lst->end(), decl_lst.begin(), decl_lst.end());
    return;
  }
  if (isSgClassDefinition(static_cast<SgNode *>(sg_scope_)) != nullptr) {
    SgClassDefinition *sg_scope =
        isSgClassDefinition(static_cast<SgNode *>(sg_scope_));
    SgDeclarationStatementPtrList decl_lst = sg_scope->getDeclarationList();
    stmt_lst->clear();
    stmt_lst->insert(stmt_lst->end(), decl_lst.begin(), decl_lst.end());
    return;
  }

  if (isSgIfStmt(static_cast<SgNode *>(sg_scope_)) != nullptr) {
    //    GetChildStmtList(isSgIfStmt(static_cast<SgNode
    //    *>(sg_scope_))->get_true_body(),
    //                     stmt_lst);
    stmt_lst->push_back(isSgStatement(static_cast<SgNode *>(sg_scope_)));
  } else if (isSgScopeStatement(static_cast<SgNode *>(sg_scope_)) != nullptr) {
    SgScopeStatement *sg_scope =
        isSgScopeStatement(static_cast<SgNode *>(sg_scope_));
    *stmt_lst = sg_scope->getStatementList();
  } else if (isSgStatement(static_cast<SgNode *>(sg_scope_)) != nullptr) {
    stmt_lst->push_back(isSgStatement(static_cast<SgNode *>(sg_scope_)));
  }
}

vector<void *> CSageCodeGen::GetChildStmts(void *sg_scope_) {
  vector<void *> v_stmt;
  for (size_t i = 0; i < GetChildStmtNum(sg_scope_); i++) {
    v_stmt.push_back(GetChildStmt(sg_scope_, i));
  }
  return v_stmt;
}
void *CSageCodeGen::GetChildStmt(void *sg_scope_, size_t i) {
  //  ZP: 20160324
  if (isSgGlobal(static_cast<SgNode *>(sg_scope_)) != nullptr) {
    SgGlobal *sg_scope = isSgGlobal(static_cast<SgNode *>(sg_scope_));
    SgDeclarationStatementPtrList stmt_lst = sg_scope->get_declarations();
    assert(i < stmt_lst.size());
    return stmt_lst[i];
  }

  //  Modify by Yuxin, May 25, when only one statement in sg_scope_
  SgStatementPtrList stmt_lst;
  GetChildStmtList(sg_scope_, &stmt_lst);
  /*
     if(isSgScopeStatement((SgNode*)sg_scope_))
     {
     SgScopeStatement * sg_scope = isSgScopeStatement((SgNode*)sg_scope_);
     stmt_lst = sg_scope->getStatementList();
     }
     else if(isSgStatement((SgNode*)sg_scope_))
     {
     stmt_lst.push_back(isSgStatement((SgNode*)sg_scope_));
     }
     */
  //    SgScopeStatement * sg_scope = isSgScopeStatement((SgNode*)sg_scope_);
  //    assert(sg_scope);
  //    SgStatementPtrList & stmt_lst = sg_scope->getStatementList();
  assert(i < stmt_lst.size());
  return stmt_lst[i];
}

size_t CSageCodeGen::GetChildStmtIdx(void *sg_scope_, void *sg_child_) {
  size_t i;
  //  Modify by Yuxin, May 25, when only one statement in sg_scope_
  SgStatementPtrList stmt_lst;
  GetChildStmtList(sg_scope_, &stmt_lst);
  SgStatement *sg_child = isSgStatement(static_cast<SgNode *>(sg_child_));
  for (i = 0; i < stmt_lst.size(); i++) {
    if (sg_child == stmt_lst[i]) {
      return i;
    }
  }
  return std::numeric_limits<size_t>::max();
}

size_t CSageCodeGen::GetChildStmtNum(void *sg_scope_) const {
  //  ZP: 20160324
  if (isSgGlobal(static_cast<SgNode *>(sg_scope_)) != nullptr) {
    SgGlobal *sg_scope = isSgGlobal(static_cast<SgNode *>(sg_scope_));
    SgDeclarationStatementPtrList stmt_lst = sg_scope->get_declarations();
    return stmt_lst.size();
  }

  //  Modify by Yuxin, May 25, when only one statement in sg_scope_
  SgStatementPtrList stmt_lst;
  GetChildStmtList(sg_scope_, &stmt_lst);
  return stmt_lst.size();
}

void *CSageCodeGen::CloneFuncDecl(void *func_decl, void *sg_scope,
                                  bool definition, bool bind) {
  void *ret_type = GetFuncReturnType(func_decl);
  string sFuncName = GetFuncName(func_decl);
  int i;
  vector<void *> fp_list;
  int num_args = GetFuncParamNum(func_decl);
  for (i = 0; i < num_args; i++) {
    void *arg = GetFuncParam(func_decl, i);
    void *sg_param = nullptr;
    if (bind) {
      sg_param = CreateVariable(GetTypebyVar(arg), GetVariableName(arg), arg);
    } else {
      sg_param = CreateVariable(GetTypebyVar(arg), GetVariableName(arg));
    }
    if (auto init = GetInitializer(arg))
      SetInitializer(sg_param, init);
    fp_list.push_back(sg_param);
  }
  void *sg_func_decl = CreateFuncDecl(ret_type, sFuncName, fp_list, sg_scope,
                                      definition, func_decl);
  if (IsStatic(func_decl)) {
    SetStatic(sg_func_decl);
  }
  if (IsExtern(func_decl)) {
    SetExtern(sg_func_decl);
  }

  return sg_func_decl;
}

void *CSageCodeGen::CloneFuncDecl(void *func_decl, void *sg_scope,
                                  const string &new_func_name,
                                  bool definition) {
  void *ret_type = GetFuncReturnType(func_decl);
  string sFuncName = new_func_name;
  int i;
  vector<void *> fp_list;
  int num_args = GetFuncParamNum(func_decl);
  for (i = 0; i < num_args; i++) {
    void *arg = GetFuncParam(func_decl, i);
    void *sg_param =
        CreateVariable(GetTypebyVar(arg), GetVariableName(arg), arg);
    if (auto init = GetInitializer(arg))
      SetInitializer(sg_param, init);
    fp_list.push_back(sg_param);
  }
  void *sg_func_decl = CreateFuncDecl(ret_type, sFuncName, fp_list, sg_scope,
                                      definition, func_decl);
  if (IsStatic(func_decl)) {
    SetStatic(sg_func_decl);
  }
  if (IsExtern(func_decl)) {
    SetExtern(sg_func_decl);
  }
  if (!definition) {
    return sg_func_decl;
  }
  //  2.2 Copy function body into new function declaration
  //  2.2.1 Create new function body
  void *func_body = GetFuncBody(func_decl);
  if (func_body == nullptr) {
    return sg_func_decl;
  }
  //  20170629 Remove function declaration before copying to avoid failure in
  //  ast->CopyStmt
  vector<void *> v_func_decl;
  GetNodesByType_int(func_body, "preorder", V_SgFunctionDeclaration,
                     &v_func_decl);
  for (auto callee_decl : v_func_decl) {
    if (IsFunctionDeclaration(callee_decl) != 0) {
      RemoveStmt(callee_decl);
    }
  }
  void *new_body = CopyStmt(func_body);
  //  2.2.2 Update variable references which are function arguments
  //  i) map oringal argument list to the new argument list
  //  ii) for all the variable references:
  //      if var initialized name == argument, then update variable reference
  for (int i = 0; i < GetFuncParamNum(func_decl); ++i) {
    replace_variable_references_in_scope(
        GetFuncParam(func_decl, i), GetFuncParam(sg_func_decl, i), new_body);
  }
  //  2.2.3 Insert new function body into new function declaration
  SetFuncBody(sg_func_decl, new_body);

  return sg_func_decl;
}

pair<vector<SgInitializedName *>, map<SgInitializedName *, SgInitializedName *>>
CSageCodeGen::cloneFuncParams(const SgFunctionDeclaration &fn) {
  vector<SgInitializedName *> rv;
  map<SgInitializedName *, SgInitializedName *> remap;
  for (SgInitializedName *arg : fn.get_args()) {
    auto *newarg = static_cast<SgInitializedName *>(
        CreateVariable(arg->get_type(), arg->get_name().getString()));
    if (auto init = arg->get_initializer())
      SetInitializer(newarg, CopyExp(init));
    rv.push_back(newarg);
    remap.insert({arg, newarg});
  }
  return {std::move(rv), std::move(remap)};
}

static void
remapVarRefs(SgScopeStatement *scope,
             const map<SgInitializedName *, SgInitializedName *> &var_map,
             CSageCodeGen *cg) {
  for (const auto &pair : var_map) {
    SgInitializedName *old = pair.first;
    SgInitializedName *new_ = pair.second;
    cg->replace_variable_references_in_scope(old, new_, scope);
  }
}

SgFunctionDeclaration *CSageCodeGen::cloneFuncWith(
    SgFunctionDeclaration *fn, SgScopeStatement *newscope, SgType *rettype,
    const string &newname, const vector<SgInitializedName *> &args,
    const map<SgInitializedName *, SgInitializedName *> &arg_remap) {
  assert(fn->get_definition() &&
         "cloneFuncWith does not clone non-defining fn decls.");

  auto *newdecl = static_cast<SgFunctionDeclaration *>(
      CreateFuncDecl(rettype, std::move(newname), {args.begin(), args.end()},
                     newscope, true, fn));
  assert(newdecl && "Expected fn decl to have been created.");

  if (IsStatic(fn)) {
    SetStatic(newdecl);
  }
  if (IsExtern(fn)) {
    SetExtern(newdecl);
  }

  //  Also clone fn's body, if it exists, and remap references to fn's params.
  auto *olddef = static_cast<SgBasicBlock *>(GetFuncBody(fn));

  //  First kill off any nested fn decls in the body. Otherwise, the fn body
  //  created by CopyStmt would contain a nested fn that causes ROSE's unparser
  //  to crash (EROSE).
  vector<SgFunctionDeclaration *> nested_fns;
  TraversePre(olddef, [&](SgNode *n) {
    if (auto *decl = isSgFunctionDeclaration(n)) {
      nested_fns.push_back(decl);
    }
  });
  for (SgFunctionDeclaration *nested : nested_fns) {
    RemoveStmt(nested);
  }

  //  Copy and remap param references.
  auto *newdef = static_cast<SgBasicBlock *>(CopyStmt(olddef));
  remapVarRefs(newdef, arg_remap, this);
  SetFuncBody(newdecl, newdef);

  return newdecl;
}

void *CSageCodeGen::CreateMemberFuncDecl(const string &sType,
                                         const string &sFuncName,
                                         vector<void *> fp_list,
                                         void *sg_scope_, bool definition,
                                         void *bindNode /*= nullptr*/) {
  void *sg_fp_list_ = CreateFuncParamList(fp_list, bindNode);
  SgFunctionParameterList *sg_fp_list =
      isSgFunctionParameterList(static_cast<SgNode *>(sg_fp_list_));
  SgScopeStatement *sg_scope =
      isSgScopeStatement(static_cast<SgNode *>(sg_scope_));
  assert(sg_fp_list && sg_scope);
#ifdef PROJDEBUG
  for (auto fp : fp_list) {
    assert(fp != nullptr && is_floating_node(fp));
  }
#endif
  SgType *sg_type = isSgType(static_cast<SgNode *>(GetTypeByString(sType)));
  SgFunctionDeclaration *sg_func_decl = nullptr;
  if (definition) {
    sg_func_decl = SageBuilder::buildDefiningMemberFunctionDeclaration(
        sFuncName, sg_type, sg_fp_list, sg_scope);
    markBuild(this, sg_func_decl,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)sg_func_decl),
              bindNode);
    sg_func_decl->set_definingDeclaration(sg_func_decl);
  } else {
    sg_func_decl = SageBuilder::buildNondefiningMemberFunctionDeclaration(
        sFuncName, sg_type, sg_fp_list, sg_scope);
    markBuild(this, sg_func_decl,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)sg_func_decl),
              bindNode);
  }
  if (sg_scope->lookup_function_symbol(sFuncName) == nullptr) {
    sg_scope->insert_symbol(sFuncName,
                            new SgMemberFunctionSymbol(sg_func_decl));
  }
  return sg_func_decl;
}

void *CSageCodeGen::CreateMemberFuncDecl(void *ret_type,
                                         const string &sFuncName,
                                         vector<void *> fp_list,
                                         void *sg_scope_, bool definition,
                                         void *bindNode /*= nullptr*/) {
  void *sg_fp_list_ = CreateFuncParamList(fp_list, bindNode);
  SgFunctionParameterList *sg_fp_list =
      isSgFunctionParameterList(static_cast<SgNode *>(sg_fp_list_));
#ifdef PROJDEBUG
  for (auto fp : fp_list) {
    assert(fp != nullptr && is_floating_node(fp));
  }
#endif
  assert(sg_fp_list);
  SgScopeStatement *sg_scope =
      isSgScopeStatement(static_cast<SgNode *>(sg_scope_));
  assert(sg_scope);

  SgType *sg_type = isSgType(static_cast<SgNode *>(ret_type));
  SgFunctionDeclaration *sg_func_decl = nullptr;
  if (definition) {
    sg_func_decl = SageBuilder::buildDefiningMemberFunctionDeclaration(
        sFuncName, sg_type, sg_fp_list, sg_scope);
    markBuild(this, sg_func_decl,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)sg_func_decl),
              bindNode);
    sg_func_decl->set_definingDeclaration(sg_func_decl);
  } else {
    sg_func_decl = SageBuilder::buildNondefiningMemberFunctionDeclaration(
        sFuncName, sg_type, sg_fp_list, sg_scope);
    markBuild(this, sg_func_decl,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)sg_func_decl),
              bindNode);
  }
  if (sg_scope->lookup_function_symbol(sFuncName) == nullptr) {
    sg_scope->insert_symbol(sFuncName,
                            new SgMemberFunctionSymbol(sg_func_decl));
  }
  return sg_func_decl;
}

void *CSageCodeGen::CreateFuncDecl(const string &ret_type,
                                   const string &sFuncName,
                                   vector<string> vec_name,
                                   vector<string> vec_type, void *sg_scope,
                                   bool definition,
                                   void *bindNode /*= nullptr*/) {
  size_t i;
  vector<void *> fp_list;
  for (i = 0; i < vec_name.size(); i++) {
    void *sg_param = CreateVariable(vec_type[i], vec_name[i]);
    fp_list.push_back(sg_param);
  }
  void *sg_func_decl = CreateFuncDecl(ret_type, sFuncName, fp_list, sg_scope,
                                      definition, bindNode);

  return sg_func_decl;
}

void *CSageCodeGen::CreateFuncDecl(const string &sType, const string &sFuncName,
                                   vector<void *> fp_list, void *sg_scope_,
                                   bool definition,
                                   void *bindNode /*= nullptr*/) {
  void *sg_fp_list_ = CreateFuncParamList(fp_list, bindNode);
  SgFunctionParameterList *sg_fp_list =
      isSgFunctionParameterList(static_cast<SgNode *>(sg_fp_list_));
  SgScopeStatement *sg_scope =
      isSgScopeStatement(static_cast<SgNode *>(sg_scope_));
  assert(sg_fp_list && sg_scope);
#ifdef PROJDEBUG
  for (auto fp : fp_list) {
    assert(fp != nullptr && is_floating_node(fp));
  }
#endif
  SgType *sg_type = isSgType(static_cast<SgNode *>(GetTypeByString(sType)));
  SgFunctionDeclaration *sg_func_decl = nullptr;
  if (definition) {
    sg_func_decl = SageBuilder::buildDefiningFunctionDeclaration(
        sFuncName, sg_type, sg_fp_list, sg_scope);
    markBuild(this, sg_func_decl,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)sg_func_decl),
              bindNode);
    sg_func_decl->set_definingDeclaration(sg_func_decl);
  } else {
    sg_func_decl = SageBuilder::buildNondefiningFunctionDeclaration(
        sFuncName, sg_type, sg_fp_list, sg_scope);
    markBuild(this, sg_func_decl,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)sg_func_decl),
              bindNode);
  }
  if (sg_scope->lookup_function_symbol(sFuncName) == nullptr) {
    sg_scope->insert_symbol(sFuncName, new SgFunctionSymbol(sg_func_decl));
  }
  return sg_func_decl;
}

void *CSageCodeGen::CreateFuncDecl(void *ret_type, const string &sFuncName,
                                   vector<void *> fp_list, void *sg_scope_,
                                   bool definition,
                                   void *bindNode /*= nullptr*/) {
  void *sg_fp_list_ = CreateFuncParamList(fp_list, bindNode);
  SgFunctionParameterList *sg_fp_list =
      isSgFunctionParameterList(static_cast<SgNode *>(sg_fp_list_));
#ifdef PROJDEBUG
  for (auto fp : fp_list) {
    assert(fp != nullptr && is_floating_node(fp));
  }
#endif
  assert(sg_fp_list);
  SgScopeStatement *sg_scope =
      isSgScopeStatement(static_cast<SgNode *>(sg_scope_));
  assert(sg_scope);

  SgType *sg_type = isSgType(static_cast<SgNode *>(ret_type));
  SgFunctionDeclaration *sg_func_decl = nullptr;
  if (definition) {
    sg_func_decl = SageBuilder::buildDefiningFunctionDeclaration(
        sFuncName, sg_type, sg_fp_list, sg_scope);
    markBuild(this, sg_func_decl,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)sg_func_decl),
              bindNode);
    sg_func_decl->set_definingDeclaration(sg_func_decl);
  } else {
    sg_func_decl = SageBuilder::buildNondefiningFunctionDeclaration(
        sFuncName, sg_type, sg_fp_list, sg_scope);
    markBuild(this, sg_func_decl,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)sg_func_decl),
              bindNode);
  }
  if (sg_scope->lookup_function_symbol(sFuncName) == nullptr) {
    sg_scope->insert_symbol(sFuncName, new SgFunctionSymbol(sg_func_decl));
  }
  return sg_func_decl;
}

void *CSageCodeGen::CreateFuncDecl(const string &sType, const string &sFuncName,
                                   void *sg_fp_list_, void *sg_scope_,
                                   bool definition,
                                   void *bindNode /*= nullptr*/) {
  SgFunctionParameterList *sg_fp_list =
      isSgFunctionParameterList(static_cast<SgNode *>(sg_fp_list_));
  SgScopeStatement *sg_scope =
      isSgScopeStatement(static_cast<SgNode *>(sg_scope_));
  assert(sg_fp_list && sg_scope);

  SgType *sg_type = isSgType(static_cast<SgNode *>(GetTypeByString(sType)));
  SgFunctionDeclaration *sg_func_decl = nullptr;
  if (definition) {
    sg_func_decl = SageBuilder::buildDefiningFunctionDeclaration(
        sFuncName, sg_type, sg_fp_list, sg_scope);
    sg_func_decl->set_definingDeclaration(sg_func_decl);
  } else {
    sg_func_decl = SageBuilder::buildNondefiningFunctionDeclaration(
        sFuncName, sg_type, sg_fp_list, sg_scope);
  }
  if (sg_scope->lookup_function_symbol(sFuncName) == nullptr) {
    sg_scope->insert_symbol(sFuncName, new SgFunctionSymbol(sg_func_decl));
  }
  markBuild(this, sg_func_decl,
            "Build in " + std::to_string(__LINE__) + " for " +
                std::to_string((int64_t)sg_func_decl),
            bindNode);
  return sg_func_decl;
}

void *CSageCodeGen::CreateFunctionRef(void *func_decl) {
  auto sg_func_decl = isSgFunctionDeclaration(static_cast<SgNode *>(func_decl));
  if (sg_func_decl == nullptr)
    return nullptr;
  auto sg_mem_func_decl = isSgMemberFunctionDeclaration(sg_func_decl);
  if (sg_mem_func_decl != nullptr) {
    if (sg_func_decl->get_declaration_associated_with_symbol() != nullptr) {
      SgMemberFunctionSymbol *sg_func_symbol = isSgMemberFunctionSymbol(
          sg_func_decl->search_for_symbol_from_symbol_table());
      if (sg_func_symbol != nullptr) {
        return SageBuilder::buildMemberFunctionRefExp(sg_func_symbol, false,
                                                      false);
      }
    }
  }
  return SageBuilder::buildFunctionRefExp(sg_func_decl);
}

string CSageCodeGen::GetFuncName(void *sg_decl_, bool qualified) const {
  SgNode *sg_decl = static_cast<SgNode *>(sg_decl_);
  SgFunctionDeclaration *sg_func = isSgFunctionDeclaration(sg_decl);
  if ((sg_func == nullptr) && (isSgFunctionDefinition(sg_decl) != nullptr)) {
    sg_func = isSgFunctionDefinition(sg_decl)->get_declaration();
  }
  if (sg_func == nullptr) {
    return "";
  }

  if (qualified) {
    return sg_func->get_qualified_name();
  }
  string name = sg_func->get_name();
  return name;
}

string CSageCodeGen::DemangleName(string name) const {
  if (name.find("p_Z") == 0) {
    name = name.substr(1);
  }
  if (name.find("_Z") != 0) {
    return name;
  }
  int status;
  char *demangle_res = abi::__cxa_demangle(name.c_str(), 0, 0, &status);
  if (0 == status) {
    // The demangling operation succeeded
    assert(demangle_res != nullptr);
    string ret(demangle_res);
    free(demangle_res);
    return ret;
  }
  return name;
}

int CSageCodeGen::SetFuncName(void *sg_decl_, const string &new_name) {
  SgNode *sg_decl = static_cast<SgNode *>(sg_decl_);
  SgFunctionDeclaration *sg_func = isSgFunctionDeclaration(sg_decl);
  if (sg_func == nullptr) {
    return 0;
  }
  if (sg_func->get_definition() != nullptr)
    s_g_func_name_cache[GetFuncName(sg_func, true)].erase(sg_func);
  vector<void *> vec_calls;
  GetFuncCallsFromDecl(sg_func, nullptr, &vec_calls);
  SgSymbol *sg_func_sym = nullptr;
  if (sg_func->get_declaration_associated_with_symbol() != nullptr) {
    sg_func_sym = sg_func->search_for_symbol_from_symbol_table();
  }
  sg_func->set_name(SgName(new_name));
  if (sg_func->get_definition() != nullptr)
    s_g_func_name_cache[GetFuncName(sg_func, true)].insert(sg_func);
  if (sg_func_sym != nullptr) {
    SgScopeStatement *scope = sg_func->get_scope();
    if (scope != nullptr) {
      if (sg_func_sym != nullptr) {
        scope->remove_symbol(sg_func_sym);
      }
      SgFunctionSymbol *newFunctionSymbol = new SgFunctionSymbol(sg_func);
      //  Insert the function symbol into the global scope.
      scope->insert_symbol(new_name, newFunctionSymbol);
    }
  }
  for (auto call : vec_calls) {
    vector<void *> vec_args;
    for (auto arg : GetFuncCallParamList(call)) {
      vec_args.push_back(CopyExp(arg));
    }
    void *new_call = CreateFuncCall(new_name, GetFuncReturnType(sg_func),
                                    vec_args, GetScope(call), call);
    ReplaceExp(call, new_call);
  }

  return 1;
}

void *CSageCodeGen::GetFuncBody(void *sg_func_decl_) const {
  SgFunctionDeclaration *sg_func_decl =
      isSgFunctionDeclaration(static_cast<SgNode *>(sg_func_decl_));
  SgFunctionDefinition *sg_func_def =
      isSgFunctionDefinition(static_cast<SgNode *>(sg_func_decl_));
  if (sg_func_decl != nullptr) {
    sg_func_def = sg_func_decl->get_definition();
  }
  if (sg_func_def == nullptr) {
    return nullptr;
  }
  SgBasicBlock *body = sg_func_def->get_body();
  return body;
}

int CSageCodeGen::SetFuncBody(void *sg_func_decl_, void *sg_body_) {
  SgFunctionDeclaration *sg_func_decl =
      isSgFunctionDeclaration(static_cast<SgNode *>(sg_func_decl_));
  SgBasicBlock *sg_body = isSgBasicBlock(static_cast<SgNode *>(sg_body_));
  if (sg_func_decl == nullptr) {
    return 0;
  }
  if (sg_func_decl->get_definition() == nullptr) {
    return 0;
  }
  SgFunctionDefinition *sg_func_def = sg_func_decl->get_definition();
  void *orig_body = GetFuncBody(sg_func_decl_);
  string oldFuncBodyStr = printNodeInfo(this, orig_body);
  sg_func_def->set_body(sg_body);
  sg_body->set_parent(sg_func_def);
  if (orig_body != nullptr) {
    RemoveChild(orig_body);
  }
  markDelete(this, sg_body->get_parent(), getTopoIndex(this, sg_body),
             "Replace(1): " + oldFuncBodyStr, static_cast<SgNode *>(orig_body));
  markInsert(this, sg_body->get_parent(), getTopoIndex(this, sg_body),
             "Replace(2): " + printNodeInfo(this, sg_body));
  return 1;
}

void *CSageCodeGen::CreateFuncCall(const string &sFunc, void *sg_ret_type_,
                                   vector<void *> param_list_, void *sg_scope_,
                                   void *bindNode /*= nullptr*/) {
  SgName sgname(sFunc);
  SgType *sg_ret_type = isSgType(static_cast<SgNode *>(sg_ret_type_));
  SgExprListExp *param_list =
      isSgExprListExp(static_cast<SgNode *>(CreateExpList(param_list_)));
  SgScopeStatement *sg_scope =
      isSgScopeStatement(static_cast<SgNode *>(sg_scope_));

  SgFunctionCallExp *ret_exp = SageBuilder::buildFunctionCallExp(
      sgname, sg_ret_type, param_list, sg_scope);
  markBuild(this, ret_exp,
            "Build in " + std::to_string(__LINE__) + " for " +
                std::to_string((int64_t)ret_exp),
            bindNode);

  return ret_exp;
}

void *CSageCodeGen::CreateFuncCall(void *sg_exp_or_func_decl,
                                   vector<void *> param_list_,
                                   void *bindNode /*= nullptr*/) {
  SgFunctionRefExp *sg_exp =
      isSgFunctionRefExp(static_cast<SgNode *>(sg_exp_or_func_decl));
  SgDotExp *sg_dot_exp = isSgDotExp(static_cast<SgNode *>(sg_exp_or_func_decl));
  SgExprListExp *param_list =
      isSgExprListExp(static_cast<SgNode *>(CreateExpList(param_list_)));
  if (sg_exp != nullptr) {
    SgFunctionCallExp *ret_exp =
        SageBuilder::buildFunctionCallExp(sg_exp, param_list);
    markBuild(this, ret_exp,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_exp),
              bindNode);

    return ret_exp;
  }
  if (sg_dot_exp != nullptr) {
    if (isSgMemberFunctionRefExp(
            static_cast<SgNode *>(GetExpRightOperand(sg_dot_exp)))) {
      SgFunctionCallExp *ret_exp =
          SageBuilder::buildFunctionCallExp(sg_dot_exp, param_list);
      markBuild(this, ret_exp,
                "Build in " + std::to_string(__LINE__) + " for " +
                    std::to_string((int64_t)ret_exp),
                bindNode);

      return ret_exp;
    }
  }
  if (IsFunctionDeclaration(sg_exp_or_func_decl) != 0) {
    SgFunctionDeclaration *sg_func =
        isSgFunctionDeclaration(static_cast<SgNode *>(sg_exp_or_func_decl));
    if (sg_func->get_declaration_associated_with_symbol() != nullptr) {
      SgFunctionSymbol *sg_func_symbol =
          isSgFunctionSymbol(sg_func->search_for_symbol_from_symbol_table());
      if (sg_func_symbol != nullptr) {
        auto sg_type =
            SageBuilder::buildFunctionCallExp(sg_func_symbol, param_list);
        markBuild(this, sg_type,
                  "Build in " + std::to_string(__LINE__) + " for " +
                      std::to_string((int64_t)sg_type),
                  bindNode);
        return sg_type;
      }
    }
  }
  assert(0);
  return nullptr;
}

void *CSageCodeGen::CreateFuncCallStmt(void *sg_decl_,
                                       vector<void *> param_list_,
                                       void *bindNode) {
  void *call = CreateFuncCall(sg_decl_, param_list_, bindNode);
  return CreateStmt(V_SgExprStatement, call, bindNode);
}

void *CSageCodeGen::CreateFuncCallStmt(void *sg_decl_, void *param_list_,
                                       void *bindNode /*= nullptr*/) {
#ifdef PROJDEBUG
  assert(param_list_ != nullptr && is_floating_node(param_list_));
#endif
  SgFunctionCallExp *call_exp = SageBuilder::buildFunctionCallExp(
      SageBuilder::buildFunctionRefExp(
          isSgFunctionDeclaration(static_cast<SgNode *>(sg_decl_))),
      isSgExprListExp(static_cast<SgNode *>(param_list_)));
  markBuild(this, call_exp,
            "Build in " + std::to_string(__LINE__) + " for " +
                std::to_string((int64_t)call_exp),
            bindNode);
  SgExprStatement *ret_stmt = SageBuilder::buildExprStatement(call_exp);
  markBuild(this, ret_stmt,
            "Build in " + std::to_string(__LINE__) + " for " +
                std::to_string((int64_t)ret_stmt),
            bindNode);

  return ret_stmt;
}

void *CSageCodeGen::GetPragmaAbove(void *pos) {
  void *sg_pragma = GetPreviousStmt(pos);
  if ((sg_pragma != nullptr) && (IsPragmaDecl(sg_pragma) != 0)) {
    return sg_pragma;
  }
  { return nullptr; }
}

string CSageCodeGen::GetPragmaString(void *sg_node_) {
  SgPragmaDeclaration *pPragma =
      isSgPragmaDeclaration(static_cast<SgNode *>(sg_node_));
  if (pPragma == nullptr) {
    return "";
  }
  if (pPragma->get_pragma() == nullptr) {
    return "";
  }
  //  ZP: 2014-01-29: ROSE will add " " for ong pragmas (FIXME: THIS IS ONLY A
  //  WORKAROUND!!!)
  string str_temp = pPragma->get_pragma()->get_pragma();
  return str_replace(str_temp, "\" \"", "");
}

void *CSageCodeGen::SetPragmaString(void *sg_node_, const string &str_value) {
  auto sg_pragma_decl = isSgPragmaDeclaration(static_cast<SgNode *>(sg_node_));
  if (sg_pragma_decl == nullptr)
    return nullptr;
  sg_pragma_decl->set_pragma(SageBuilder::buildPragma(str_value));
  return sg_pragma_decl;
}

void *CSageCodeGen::CreateVariableDecl(const string &sType, const string &sVar,
                                       void *sg_init_exp_, void *sg_scope_,
                                       void *bindNode /*= nullptr*/) {
#ifdef PROJDEBUG
  assert(sg_init_exp_ == nullptr || is_floating_node(sg_init_exp_));
#endif
  SgInitializer *sg_init;
  SgExpression *sg_exp = isSgExpression(static_cast<SgNode *>(sg_init_exp_));
  if (sg_exp != nullptr) {
    if (isSgInitializer(sg_exp) != nullptr) {
      sg_init = isSgInitializer(sg_exp);
    } else {
      sg_init = SageBuilder::buildAssignInitializer(sg_exp);
      markBuild(this, sg_init,
                "Build in " + std::to_string(__LINE__) + " for " +
                    std::to_string((int64_t)sg_init));
    }
  } else {
    sg_init = nullptr;
  }

  SgScopeStatement *sg_scope =
      isSgScopeStatement(static_cast<SgNode *>(sg_scope_));
  SgType *sg_type = isSgType(static_cast<SgNode *>(GetTypeByString(sType)));
  assert(sg_type);
  SgVariableDeclaration *sg_decl =
      SageBuilder::buildVariableDeclaration(sVar, sg_type, sg_init, sg_scope);
  markBuild(this, sg_decl,
            "Build in " + std::to_string(__LINE__) + " for " +
                std::to_string((int64_t)sg_decl),
            bindNode);
  return sg_decl;
}

void *CSageCodeGen::CreateVariableDecl(void *sg_type_, const string &sVar,
                                       void *sg_init_exp_, void *sg_scope_,
                                       void *bindNode /*= nullptr*/) {
#ifdef PROJDEBUG
  assert(sg_init_exp_ == nullptr || is_floating_node(sg_init_exp_));
#endif
  assert(sg_type_);
  SgInitializer *sg_init;
  SgExpression *sg_exp = isSgExpression(static_cast<SgNode *>(sg_init_exp_));
  if (sg_exp != nullptr) {
    if (isSgInitializer(sg_exp) != nullptr) {
      sg_init = isSgInitializer(sg_exp);
    } else {
      sg_init = SageBuilder::buildAssignInitializer(sg_exp);
      markBuild(this, sg_init,
                "Build in " + std::to_string(__LINE__) + " for " +
                    std::to_string((int64_t)sg_init));
    }
  } else {
    sg_init = nullptr;
  }

  SgScopeStatement *sg_scope =
      isSgScopeStatement(static_cast<SgNode *>(sg_scope_));
  SgType *sg_type = isSgType(static_cast<SgNode *>(sg_type_));
  SgVariableDeclaration *sg_decl =
      SageBuilder::buildVariableDeclaration(sVar, sg_type, sg_init, sg_scope);
  markBuild(this, sg_decl,
            "Build in " + std::to_string(__LINE__) + " for " +
                std::to_string((int64_t)sg_decl),
            bindNode);
  return sg_decl;
}

void *CSageCodeGen::CreateInitializer(void *sg_init_exp_,
                                      void *bindNode /*= nullptr*/) {
#ifdef PROJDEBUG
  assert(sg_init_exp_ != nullptr && is_floating_node(sg_init_exp_));
#endif
  SgInitializer *sg_init;
  SgExpression *sg_exp = isSgExpression(static_cast<SgNode *>(sg_init_exp_));
  if (sg_exp != nullptr) {
    if (isSgInitializer(sg_exp) != nullptr) {
      sg_init = isSgInitializer(sg_exp);
    } else {
      sg_init = SageBuilder::buildAssignInitializer(sg_exp);
      markBuild(this, sg_init,
                "Build in " + std::to_string(__LINE__) + " for " +
                    std::to_string((int64_t)sg_init));
    }
  } else {
    sg_init = nullptr;
  }
  return sg_init;
}

void CSageCodeGen::SetStatic(void *sg_decl_) {
  SgDeclarationStatement *sg_decl =
      isSgDeclarationStatement(static_cast<SgNode *>(sg_decl_));
  if (sg_decl == nullptr) {
    return;
  }
  SageInterface::setStatic(sg_decl);
}

void CSageCodeGen::UnSetStatic(void *sg_decl_) {
  if (!IsStatic(sg_decl_)) {
    return;
  }
  SgDeclarationStatement *sg_decl =
      isSgDeclarationStatement(static_cast<SgNode *>(sg_decl_));
  ((sg_decl->get_declarationModifier()).get_storageModifier()).setDefault();
}

bool CSageCodeGen::IsStatic(void *sg_decl_) {
  SgDeclarationStatement *sg_decl =
      isSgDeclarationStatement(static_cast<SgNode *>(sg_decl_));
  if (sg_decl == nullptr) {
    return false;
  }
  if (SageInterface::isStatic(sg_decl)) {
    return true;
  }
  if ((IsTemplateFunctionDecl(sg_decl) != 0) ||
      (IsTemplateMemberFunctionDecl(sg_decl) != 0)) {
    string tp_str = _up(sg_decl);
    if (tp_str.find(" static ") != string::npos) {
      return true;
    }
  }

  if (IsTemplateInstantiationFunctionDecl(sg_decl) != 0) {
    void *tf = GetTemplateFuncDecl(sg_decl);
    return IsStatic(tf);
  }

  if (IsTemplateInstantiationMemberFunctionDecl(sg_decl) != 0) {
    void *tmf = GetTemplateMemFuncDecl(sg_decl);
    return IsStatic(tmf);
  }

  return false;
}

bool CSageCodeGen::IsMutable(void *sg_decl_) {
  SgDeclarationStatement *sg_decl =
      isSgDeclarationStatement(static_cast<SgNode *>(sg_decl_));
  if (sg_decl == nullptr) {
    return false;
  }

  return ((sg_decl->get_declarationModifier()).get_storageModifier())
      .isMutable();
}

bool CSageCodeGen::IsExtern(void *sg_decl) {
  return SageInterface::isExtern(
      static_cast<SgDeclarationStatement *>(sg_decl));
}

bool CSageCodeGen::IsCompilerGenerated(void *sg_expr_or_stat) {
  SgLocatedNode *sg_loc =
      isSgLocatedNode(static_cast<SgNode *>(sg_expr_or_stat));
  if (sg_loc == nullptr) {
    return false;
  }
  //  return sg_loc->isCompilerGenerated();
  return get_file(sg_loc) == "compilerGenerated";
}

bool CSageCodeGen::IsTransformation(void *sg_expr_or_stat) {
  SgLocatedNode *sg_loc =
      isSgLocatedNode(static_cast<SgNode *>(sg_expr_or_stat));
  if (sg_loc == nullptr) {
    return false;
  }
  return sg_loc->isTransformation();
}

bool CSageCodeGen::IsNULLFile(void *sg_expr_or_stat) {
  SgLocatedNode *sg_loc =
      isSgLocatedNode(static_cast<SgNode *>(sg_expr_or_stat));
  if (sg_loc == nullptr) {
    return false;
  }
  return get_file(sg_loc) == "nullptr_FILE";
}

void CSageCodeGen::SetExtern(void *sg_decl) {
  SageInterface::setExtern(static_cast<SgDeclarationStatement *>(sg_decl));
}

void *CSageCodeGen::GetVariableInitializedName(void *sg_var_ref_decl) const {
  vector<void *> vec_var;
  GetVariableInitializedName(sg_var_ref_decl, &vec_var);
  if (!vec_var.empty()) {
    return vec_var[0];
  }
  return nullptr;
}
void *CSageCodeGen::GetVariableDefinition(void *decl_, void *name_) {
  SgVariableDeclaration *decl =
      isSgVariableDeclaration(static_cast<SgNode *>(decl_));
  //  SgInitializedName* name = isSgInitializedName((SgNode*)name_);

  if (decl == nullptr) {
    return nullptr;
  }

  //  return decl->get_definition(name); //  always return 0 ??
  return decl->get_definition(nullptr);
}

void CSageCodeGen::GetVariableInitializedName(void *sg_var_ref_decl,
                                              vector<void *> *vec_var) const {
  SgVarRefExp *sg_var_ref =
      isSgVarRefExp(static_cast<SgNode *>(sg_var_ref_decl));
  SgVariableDeclaration *sg_var_decl =
      isSgVariableDeclaration(static_cast<SgNode *>(sg_var_ref_decl));
  if ((sg_var_ref == nullptr) && (sg_var_decl == nullptr)) {
    return;
  }
  if (sg_var_ref != nullptr) {
    vec_var->push_back(sg_var_ref->get_symbol()->get_declaration());
  } else {
    SgInitializedNamePtrList &var_list = sg_var_decl->get_variables();
    for (size_t i = 0; i != var_list.size(); ++i) {
      vec_var->push_back(var_list[i]);
    }
  }
}

void *CSageCodeGen::GetVariableDecl(void *sg_var_ref_) {
  SgVarRefExp *sg_var_ref = isSgVarRefExp(static_cast<SgNode *>(sg_var_ref_));
  if (sg_var_ref != nullptr) {
    return sg_var_ref->get_symbol()->get_declaration()->get_declaration();
  }
  SgInitializedName *sg_name =
      isSgInitializedName(static_cast<SgNode *>(sg_var_ref_));
  if (sg_name != nullptr) {
#if 0
    SgVariableSymbol *varsym = isSgVariableSymbol(
        sg_name->search_for_symbol_from_symbol_table());
    if (!m_var_syms_valid)
      reset_var_syms();
    auto it = m_var_syms.find(varsym);
    SgInitializedName *real_var = isSgInitializedName(static_cast<SgNode*>((it
          != m_var_syms.end() ? it->second : nullptr)));
    if (real_var)
      sg_name = real_var;
#endif
    return sg_name->get_declaration();
  }

  return nullptr;
}

//  SgExpression * CreateVariableRef(SgNode * var, int type = V_SgVarRefExp)
void *CSageCodeGen::CreateVariableRef(void *var, void *scope_,
                                      void *bindNode /*= nullptr*/) {
  SgExpression *ret_exp;
  SgVariableDeclaration *var_decl =
      isSgVariableDeclaration(static_cast<SgNode *>(var));
  SgInitializedName *var_init = isSgInitializedName(static_cast<SgNode *>(var));
  if (var_decl != nullptr) {  //  (V_SgVarRefExp == type)
    ret_exp = SageBuilder::buildVarRefExp(var_decl);
    markBuild(this, ret_exp,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_exp),
              bindNode);
  } else if (var_init != nullptr) {  //  (V_SgInitializedName == type)
    SgScopeStatement *scope = isSgScopeStatement(static_cast<SgNode *>(scope_));
    if (var_init->search_for_symbol_from_symbol_table() == nullptr) {
      SgScopeStatement *sg_var_scope = var_init->get_scope();
      if (sg_var_scope != nullptr) {
        sg_var_scope->insert_symbol(var_init->get_name(),
                                    new SgVariableSymbol(var_init));
      }
    }
    ret_exp = SageBuilder::buildVarRefExp(var_init, scope);
    markBuild(this, ret_exp,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_exp),
              bindNode);
  } else {
    assert(false);
  }

  return ret_exp;
}

void *CSageCodeGen::CreateVariableRef(const string &var_name, void *scope,
                                      void *bindNode /*= nullptr*/) {
#ifdef PROJDEBUG
  assert(IsScopeStatement(scope) &&
         getVariableSymbolFromName(scope, var_name) &&
         "backend may crash, such as bug2144");
#endif
  auto sg_type = SageBuilder::buildVarRefExp(
      var_name, isSgScopeStatement(static_cast<SgNode *>(scope)));
  markBuild(this, sg_type,
            "Build in " + std::to_string(__LINE__) + " for " +
                std::to_string((int64_t)sg_type),
            bindNode);
  return sg_type;
}

string CSageCodeGen::GetVariableName(void *sg_var_, bool qualified) const {
  SgNode *sg_var = static_cast<SgNode *>(sg_var_);
  SgInitializedName *sg_init = isSgInitializedName(sg_var);
  SgVariableDeclaration *sg_decl = isSgVariableDeclaration(sg_var);
  SgVarRefExp *sg_var_exp = isSgVarRefExp(sg_var);
  if (sg_var_exp != nullptr) {
    sg_init = sg_var_exp->get_symbol()->get_declaration();
  }
  if (sg_init != nullptr) {
    return qualified ? sg_init->get_qualified_name().getString()
                     : sg_init->get_name().getString();
  }
  if (sg_decl != nullptr) {
    return qualified
               ? sg_decl->get_variables()[0]->get_qualified_name().getString()
               : sg_decl->get_variables()[0]->get_name().getString();
  }
  return "";
}

void CSageCodeGen::SetVariableName(void *sg_var_, const string &new_name) {
  SgNode *sg_var = static_cast<SgNode *>(sg_var_);
  SgInitializedName *sg_init = isSgInitializedName(sg_var);
  SgVariableDeclaration *sg_decl = isSgVariableDeclaration(sg_var);

  if (sg_init != nullptr) {
    SageInterface::set_name(sg_init, new_name);
  }
  if (sg_decl != nullptr) {
    sg_init = isSgInitializedName(sg_decl->get_variables()[0]);
    if (sg_init != nullptr) {
      SageInterface::set_name(sg_init, new_name);
    }
  }
}

string CSageCodeGen::GetVariableTypeName(void *sg_var_) {
  SgNode *sg_var = static_cast<SgNode *>(sg_var_);
  SgInitializedName *sg_init = isSgInitializedName(sg_var);
  SgVariableDeclaration *sg_decl = isSgVariableDeclaration(sg_var);

  if (sg_init != nullptr) {
    return UnparseToString(sg_init->get_type());
  }
  if (sg_decl != nullptr) {
    return UnparseToString(sg_decl->get_variables()[0]->get_type());
  }
  assert(0);
  return "";
}

void *CSageCodeGen::CreateArrayRef(void *array_var_ref,
                                   vector<void *> vec_index,
                                   void *bindNode /*= nullptr*/) {
#ifdef PROJDEBUG
  assert(array_var_ref != nullptr && is_floating_node(array_var_ref));
#endif
  SgExpression *arr_ref = isSgExpression(static_cast<SgNode *>(array_var_ref));
  for (size_t i = 0; i < vec_index.size(); i++) {
    //  SgExpression * arr_ref = isSgExpression((SgNode*)
    arr_ref = isSgExpression(static_cast<SgNode *>(
        CreateExp(V_SgPntrArrRefExp, arr_ref, vec_index[i], 0, bindNode)));
  }
  return arr_ref;
}

void *CSageCodeGen::CreatePointerType(void *base_type,
                                      void *bindNode /*= nullptr*/) {
  if (IsReferenceType(base_type))
    base_type = GetElementType(base_type);
  SgType *curr_type = static_cast<SgType *>(base_type);
  auto sg_type = SageBuilder::buildPointerType(curr_type);
  //  markBuild(* this, sg_type, "Build in " + std::to_string(__LINE__) + " for
  //  " + std::to_string((int64_t) sg_type), bindNode);
  return sg_type;
}

void *CSageCodeGen::CreateReferenceType(void *base_type,
                                        void *bindNode /*= nullptr*/) {
  SgType *curr_type = static_cast<SgType *>(base_type);
  auto sg_type = SageBuilder::buildReferenceType(curr_type);
  //  markBuild(* this, sg_type, "Build in " + std::to_string(__LINE__) + " for
  //  " + std::to_string((int64_t) sg_type), bindNode);
  return sg_type;
}

//  Order of vec_dim: Left to right dimensions
void *CSageCodeGen::CreateArrayType(void *base_type, vector<size_t> vec_dim,
                                    void *bindNode /*= nullptr*/) {
  SgType *curr_type = static_cast<SgType *>(base_type);
  for (size_t i = 0; i < vec_dim.size(); i++) {
    size_t curr_dim = vec_dim[vec_dim.size() - 1 - i];
    if ((static_cast<size_t>(EMPTY_SIZE)) == curr_dim) {
      curr_type = SageBuilder::buildArrayType(
          curr_type, SageBuilder::buildNullExpression());
    } else if ((static_cast<size_t>(POINTER_SIZE)) == curr_dim ||
               (static_cast<size_t>(UNKNOWN_SIZE)) == curr_dim) {
      curr_type = SageBuilder::buildPointerType(curr_type);
      //  markBuild(* this, curr_type, "Build in " + std::to_string(__LINE__) +
      //  " for " + std::to_string((int64_t) curr_type), bindNode);
    } else {
      auto dim_type = SageBuilder::buildIntVal(curr_dim);
      curr_type = SageBuilder::buildArrayType(curr_type, dim_type);
      //  markBuild(* this, dim_type, "Build in " + std::to_string(__LINE__) + "
      //  for
      //  "
      //  + std::to_string((int64_t) dim_type)); markBuild(* this, curr_type,
      //  "Build in "
      //  + std::to_string(__LINE__) + " for " + std::to_string((int64_t)
      //  curr_type), bindNode);
    }
  }
  return curr_type;
}
//  Order of vec_dim: Left to right dimensions
void *CSageCodeGen::CreateCombinedPointerArrayType(
    void *base_type, vector<size_t> vec_dim, void *bindNode /*= nullptr*/) {
  SgType *curr_type = static_cast<SgType *>(base_type);
  for (size_t i = 0; i < vec_dim.size(); i++) {
    int dim_size = vec_dim[vec_dim.size() - 1 - i];

    if (dim_size == (static_cast<size_t>(POINTER_SIZE)) ||
        dim_size == (static_cast<size_t>(UNKNOWN_SIZE))) {
      curr_type = SageBuilder::buildPointerType(curr_type);
      //  markBuild(* this, curr_type, "Build in " + std::to_string(__LINE__) +
      //  " for " + std::to_string((int64_t) curr_type));
    } else if (dim_size == (static_cast<size_t>(EMPTY_SIZE))) {
      curr_type = SageBuilder::buildArrayType(
          curr_type, SageBuilder::buildNullExpression());
      //  markBuild(* this, curr_type, "Build in " + std::to_string(__LINE__) +
      //  " for " + std::to_string((int64_t) curr_type));
    } else {
      auto dim_type = SageBuilder::buildIntVal(dim_size);
      curr_type = SageBuilder::buildArrayType(curr_type, dim_type);
      //  markBuild(* this, dim_type, "Build in " + std::to_string(__LINE__) + "
      //  for
      //  "
      //  + std::to_string((int64_t) dim_type)); markBuild(* this, curr_type,
      //  "Build in "
      //  + std::to_string(__LINE__) + " for " + std::to_string((int64_t)
      //  curr_type));
    }
  }
  // TODO(youxiang): to be bind
  return curr_type;
}

void *CSageCodeGen::CreateConst(int value_, void *bindNode /*= nullptr*/) {
  auto sg_type = SageBuilder::buildIntVal(value_);
#if 0
  markBuild(this, sg_type,
            "Build in " + std::to_string(__LINE__) + " for " +
                std::to_string((int64_t)sg_type),
            bindNode);
#endif
  return sg_type;
}

void *CSageCodeGen::CreateConst(int64_t value_, void *bindNode /*= nullptr*/) {
  auto sg_type = SageBuilder::buildLongIntVal(value_);
#if 0
  markBuild(this, sg_type,
            "Build in " + std::to_string(__LINE__) + " for " +
                std::to_string((int64_t)sg_type),
            bindNode);
#endif
  return sg_type;
}

void *CSageCodeGen::CreateConst(size_t value_, void *bindNode /*= nullptr*/) {
  auto sg_type = SageBuilder::buildUnsignedLongVal(value_);
#if 0
  markBuild(this, sg_type,
            "Build in " + std::to_string(__LINE__) + " for " +
                std::to_string((int64_t)sg_type),
            bindNode);
#endif
  return sg_type;
}

void *CSageCodeGen::CreateConst(void *val_, int const_type /*= V_SgIntVal*/,
                                void *bindNode /*= nullptr*/) {
  SgExpression *ret_exp;
  if (V_SgIntVal == const_type || V_SgTypeInt == const_type ||
      V_SgTypeSignedInt == const_type) {
    int val = *(static_cast<int64_t *>(val_));
    ret_exp = SageBuilder::buildIntVal(val);
  } else if (V_SgFloatVal == const_type || V_SgTypeFloat == const_type) {
    float val = *(static_cast<float *>(val_));
    ret_exp = SageBuilder::buildFloatVal(val);
  } else if (V_SgDoubleVal == const_type || V_SgTypeDouble == const_type) {
    double val = *(static_cast<double *>(val_));
    ret_exp = SageBuilder::buildDoubleVal(val);
  } else if (V_SgStringVal == const_type || V_SgTypeString == const_type) {
    string val = *(static_cast<string *>(val_));
    ret_exp = SageBuilder::buildStringVal(val);
  } else if (V_SgShortVal == const_type ||  //  Yuxin Aug 2016
             V_SgTypeShort == const_type || V_SgTypeSignedShort == const_type) {
    int16_t val = *(static_cast<int64_t *>(val_));
    ret_exp = SageBuilder::buildShortVal(val);
  } else if (V_SgCharVal == const_type ||  //  Yuxin Aug 2016
             V_SgTypeChar == const_type || V_SgTypeSignedChar == const_type) {
    char val = *(static_cast<int64_t *>(val_));
    ret_exp = SageBuilder::buildCharVal(val);
  } else if (V_SgLongIntVal == const_type || V_SgTypeLong == const_type ||
             V_SgTypeSignedLong == const_type) {
    int64_t val = *(static_cast<int64_t *>(val_));
    ret_exp = SageBuilder::buildLongIntVal(val);
  } else if (V_SgLongLongIntVal == const_type ||
             V_SgTypeLongLong == const_type ||
             V_SgTypeSignedLongLong == const_type) {
    int64_t val = *(static_cast<int64_t *>(val_));
    ret_exp = SageBuilder::buildLongLongIntVal(val);
  } else if (V_SgUnsignedIntVal == const_type ||  // Yuxin Oct 2018
             V_SgTypeUnsignedInt == const_type) {
    unsigned int val = *(static_cast<int64_t *>(val_));
    ret_exp = SageBuilder::buildUnsignedIntVal(val);
  } else if (V_SgUnsignedShortVal == const_type ||  //  Yuxin Oct 2018
             V_SgTypeUnsignedShort == const_type) {
    uint16_t val = *(static_cast<int64_t *>(val_));
    ret_exp = SageBuilder::buildUnsignedShortVal(val);
  } else if (V_SgUnsignedCharVal == const_type ||  //  Yuxin Oct 2018
             V_SgTypeUnsignedChar == const_type) {
    unsigned char val = *(static_cast<int64_t *>(val_));
    ret_exp = SageBuilder::buildUnsignedCharVal(val);
  } else if (V_SgUnsignedLongVal == const_type ||  //  Yuxin Oct 2018
             V_SgTypeUnsignedLong == const_type) {
    uint64_t val = *(static_cast<int64_t *>(val_));
    ret_exp = SageBuilder::buildUnsignedLongVal(val);
  } else if (V_SgUnsignedLongLongIntVal == const_type ||  // Yuxin Oct 2018
             V_SgTypeUnsignedLongLong == const_type) {
    uint64_t val = *(static_cast<uint64_t *>(val_));
    ret_exp = SageBuilder::buildUnsignedLongLongIntVal(val);
  } else if (V_SgBoolValExp == const_type || V_SgTypeBool == const_type) {
    bool val = *(static_cast<int64_t *>(val_)) != 0;
    ret_exp = SageBuilder::buildBoolValExp(val);
  } else {
#ifdef PROJDEBUG
    assert(0 && "Unsupported type build\n");
#endif
    return nullptr;
  }
#if 0
    markBuild(this, ret_exp,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_exp),
              bindNode);
#endif
  return ret_exp;
}

void *CSageCodeGen::CreateStringExp(const string &val,
                                    void *bindNode /*= nullptr*/) {
  auto sg_type = SageBuilder::buildStringVal(val);
#if 0
  markBuild(this, sg_type,
            "Build in " + std::to_string(__LINE__) + " for " +
                std::to_string((int64_t)sg_type),
            bindNode);
#endif
  return sg_type;
}

void *CSageCodeGen::GetExpUniOperand(void *sg_exp) {
  void *left = nullptr;
  void *right = nullptr;
  GetExpOperand(sg_exp, &left, &right);
  if (right == nullptr) {
    return left;
  }
  { return nullptr; }
}
void *CSageCodeGen::GetExpLeftOperand(void *sg_exp) {
  void *left = nullptr;
  void *right = nullptr;
  GetExpOperand(sg_exp, &left, &right);
  return left;
}
void *CSageCodeGen::GetExpRightOperand(void *sg_exp) {
  void *left = nullptr;
  void *right = nullptr;
  GetExpOperand(sg_exp, &left, &right);
  return right;
}

void CSageCodeGen::GetExpOperand(void *sg_exp_, void **op0, void **op1) {
  SgNode *sg_exp = static_cast<SgNode *>(sg_exp_);

  if (isSgUnaryOp(sg_exp) != nullptr) {
    *op0 = isSgUnaryOp(sg_exp)->get_operand();
    *op1 = nullptr;
  } else if (isSgBinaryOp(sg_exp) != nullptr) {
    *op0 = isSgBinaryOp(sg_exp)->get_lhs_operand();
    *op1 = isSgBinaryOp(sg_exp)->get_rhs_operand();
  } else {
    *op0 = *op1 = nullptr;
  }
}

void *CSageCodeGen::CreateConditionExp(void *op0, void *op1, void *op2,
                                       void *bindNode /*= nullptr*/) {
#ifdef PROJDEBUG
  assert(op0 != nullptr && is_floating_node(op0));
  assert(op1 != nullptr && is_floating_node(op1));
  assert(op2 != nullptr && is_floating_node(op2));
#endif
  SgExpression *op0_exp = isSgExpression(static_cast<SgNode *>(op0));
  SgExpression *op1_exp = isSgExpression(static_cast<SgNode *>(op1));
  SgExpression *op2_exp = isSgExpression(static_cast<SgNode *>(op2));
  SgExpression *ret_exp;
  ret_exp = SageBuilder::buildConditionalExp(op0_exp, op1_exp, op2_exp);
  markBuild(this, ret_exp,
            "Build in " + std::to_string(__LINE__) + " for " +
                std::to_string((int64_t)ret_exp),
            bindNode);
  return ret_exp;
}

//  if operand is a var_decl, it will generate a ref_exp automatically.
void *CSageCodeGen::CreateExp(int opcode, void *op0, void *op1 /*= nullptr*/,
                              int unary_mode_ /*= 0*/,
                              void *bindNode /*= nullptr*/,
                              bool check_floating_node /* true */) {
  SgUnaryOp::Sgop_mode unary_mode =
      (unary_mode_ == 0) ? SgUnaryOp::prefix : SgUnaryOp::postfix;
  SgVariableDeclaration *op0_decl =
      isSgVariableDeclaration(static_cast<SgNode *>(op0));
  SgVariableDeclaration *op1_decl =
      isSgVariableDeclaration(static_cast<SgNode *>(op1));
  SgInitializedName *op0_name = isSgInitializedName(static_cast<SgNode *>(op0));
  SgInitializedName *op1_name = isSgInitializedName(static_cast<SgNode *>(op1));

  SgExpression *op0_exp = isSgExpression(static_cast<SgNode *>(op0));
  SgType *op0_type = isSgType(static_cast<SgNode *>(op0));
  SgType *op1_type = isSgType(static_cast<SgNode *>(op1));
  SgExpression *op1_exp = isSgExpression(static_cast<SgNode *>(op1));

  op0_exp = ((op0_decl != nullptr) || (op0_name != nullptr))
                ? isSgExpression(static_cast<SgNode *>(CreateVariableRef(op0)))
                : op0_exp;
  op1_exp = ((op1_decl != nullptr) || (op1_name != nullptr))
                ? isSgExpression(static_cast<SgNode *>(CreateVariableRef(op1)))
                : op1_exp;

  SgExpression *ret_exp;
  switch (opcode) {
  case V_SgStatementExpression:
#ifdef PROJDEBUG
    assert(op0 != nullptr && (!check_floating_node || is_floating_node(op0)));
#endif
    ret_exp = SageBuilder::buildStatementExpression(
        isSgStatement(static_cast<SgNode *>(op0)));
    markBuild(this, ret_exp,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_exp),
              bindNode);
    break;
  case V_SgNullExpression:
    ret_exp = SageBuilder::buildNullExpression();
    markBuild(this, ret_exp,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_exp),
              bindNode);
    break;

  //  ///////////////////////////////////////////////////////////////////////
  //  / Unary
  case V_SgNotOp:
#ifdef PROJDEBUG
    assert(op0 != nullptr &&
           (!check_floating_node || is_floating_node(op0_exp)));
#endif
    ret_exp = SageBuilder::buildNotOp(op0_exp);
    markBuild(this, ret_exp,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_exp),
              bindNode);
    break;

  case V_SgPlusPlusOp:
#ifdef PROJDEBUG
    assert(op0 != nullptr && (!check_floating_node || is_floating_node(op0)));
#endif
    ret_exp = SageBuilder::buildPlusPlusOp(op0_exp, unary_mode);
    markBuild(this, ret_exp,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_exp),
              bindNode);
    break;

  case V_SgMinusMinusOp:
#ifdef PROJDEBUG
    assert(op0 != nullptr && (!check_floating_node || is_floating_node(op0)));
#endif
    ret_exp = SageBuilder::buildMinusMinusOp(op0_exp, unary_mode);
    markBuild(this, ret_exp,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_exp),
              bindNode);
    break;

  case V_SgPointerDerefExp:
#ifdef PROJDEBUG
    assert(op0 != nullptr && (!check_floating_node || is_floating_node(op0)));
#endif
    ret_exp = SageBuilder::buildPointerDerefExp(op0_exp);
    markBuild(this, ret_exp,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_exp),
              bindNode);
    break;

  case V_SgAddressOfOp:
#ifdef PROJDEBUG
    assert(op0 != nullptr && (!check_floating_node || is_floating_node(op0)));
#endif
    ret_exp = SageBuilder::buildAddressOfOp(op0_exp);
    markBuild(this, ret_exp,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_exp),
              bindNode);
    break;

  case V_SgMinusOp:
#ifdef PROJDEBUG
    assert(op0 != nullptr && (!check_floating_node || is_floating_node(op0)));
#endif
    ret_exp = SageBuilder::buildMinusOp(op0_exp);
    markBuild(this, ret_exp,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_exp),
              bindNode);
    break;

  case V_SgUnaryAddOp:
#ifdef PROJDEBUG
    assert(op0 != nullptr && (!check_floating_node || is_floating_node(op0)));
#endif
    ret_exp = SageBuilder::buildUnaryAddOp(op0_exp);
    markBuild(this, ret_exp,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_exp),
              bindNode);
    break;

  case V_SgSizeOfOp:
#ifdef PROJDEBUG
    assert(op0 != nullptr);
#endif
    ret_exp = SageBuilder::buildSizeOfOp(op0_type);
    markBuild(this, ret_exp,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_exp),
              bindNode);
    break;
  case V_SgCastExp:
#ifdef PROJDEBUG
    assert(op0 != nullptr && (!check_floating_node || is_floating_node(op0)));
#endif
    ret_exp = SageBuilder::buildCastExp(op0_exp, op1_type);
    markBuild(this, ret_exp,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_exp),
              bindNode);
    break;

  //  ///////////////////////////////////////////////////////////////////////
  //  / Binary
  case V_SgAssignOp:
#ifdef PROJDEBUG
    assert(op0_exp != nullptr &&
           (!check_floating_node || is_floating_node(op0_exp)));
    assert(op1_exp != nullptr &&
           (!check_floating_node || is_floating_node(op1_exp)));
    assert(!IsInitializer(op1_exp) || IsConstructorInitializer(op1_exp));
#endif
    ret_exp = SageBuilder::buildAssignOp(op0_exp, op1_exp);
    markBuild(this, ret_exp,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_exp),
              bindNode);
    break;
  case V_SgCommaOpExp:
#ifdef PROJDEBUG
    assert(op0_exp != nullptr &&
           (!check_floating_node || is_floating_node(op0_exp)));
    assert(op1_exp != nullptr &&
           (!check_floating_node || is_floating_node(op1_exp)));
#endif
    ret_exp = SageBuilder::buildCommaOpExp(op0_exp, op1_exp);
    markBuild(this, ret_exp,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_exp),
              bindNode);
    break;
  case V_SgPlusAssignOp:
#ifdef PROJDEBUG
    assert(op0_exp != nullptr &&
           (!check_floating_node || is_floating_node(op0_exp)));
    assert(op1_exp != nullptr &&
           (!check_floating_node || is_floating_node(op1_exp)));
#endif
    ret_exp = SageBuilder::buildPlusAssignOp(op0_exp, op1_exp);
    markBuild(this, ret_exp,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_exp),
              bindNode);
    break;
  case V_SgMinusAssignOp:
#ifdef PROJDEBUG
    assert(op0_exp != nullptr &&
           (!check_floating_node || is_floating_node(op0_exp)));
    assert(op1_exp != nullptr &&
           (!check_floating_node || is_floating_node(op1_exp)));
#endif
    ret_exp = SageBuilder::buildMinusAssignOp(op0_exp, op1_exp);
    markBuild(this, ret_exp,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_exp),
              bindNode);
    break;
  case V_SgMultAssignOp:
#ifdef PROJDEBUG
    assert(op0_exp != nullptr &&
           (!check_floating_node || is_floating_node(op0_exp)));
    assert(op1_exp != nullptr &&
           (!check_floating_node || is_floating_node(op1_exp)));
#endif
    ret_exp = SageBuilder::buildMultAssignOp(op0_exp, op1_exp);
    markBuild(this, ret_exp,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_exp),
              bindNode);
    break;
  case V_SgModOp:
#ifdef PROJDEBUG
    assert(op0_exp != nullptr &&
           (!check_floating_node || is_floating_node(op0_exp)));
    assert(op1_exp != nullptr &&
           (!check_floating_node || is_floating_node(op1_exp)));
#endif
    ret_exp = SageBuilder::buildModOp(op0_exp, op1_exp);
    markBuild(this, ret_exp,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_exp),
              bindNode);
    break;
  case V_SgPntrArrRefExp:
#ifdef PROJDEBUG
    assert(op0_exp != nullptr &&
           (!check_floating_node || is_floating_node(op0_exp)));
    assert(op1_exp != nullptr &&
           (!check_floating_node || is_floating_node(op1_exp)));
#endif
    ret_exp = SageBuilder::buildPntrArrRefExp(op0_exp, op1_exp);
    markBuild(this, ret_exp,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_exp),
              bindNode);
    break;
  case V_SgAddOp:
#ifdef PROJDEBUG
    assert(op0_exp != nullptr &&
           (!check_floating_node || is_floating_node(op0_exp)));
    assert(op1_exp != nullptr &&
           (!check_floating_node || is_floating_node(op1_exp)));
#endif
    ret_exp = SageBuilder::buildAddOp(op0_exp, op1_exp);
    markBuild(this, ret_exp,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_exp),
              bindNode);
    break;
  case V_SgSubtractOp:
#ifdef PROJDEBUG
    assert(op0_exp != nullptr &&
           (!check_floating_node || is_floating_node(op0_exp)));
    assert(op1_exp != nullptr &&
           (!check_floating_node || is_floating_node(op1_exp)));
#endif
    ret_exp = SageBuilder::buildSubtractOp(op0_exp, op1_exp);
    markBuild(this, ret_exp,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_exp),
              bindNode);
    break;
  case V_SgMultiplyOp:
#ifdef PROJDEBUG
    assert(op0_exp != nullptr &&
           (!check_floating_node || is_floating_node(op0_exp)));
    assert(op1_exp != nullptr &&
           (!check_floating_node || is_floating_node(op1_exp)));
#endif
    ret_exp = SageBuilder::buildMultiplyOp(op0_exp, op1_exp);
    markBuild(this, ret_exp,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_exp),
              bindNode);
    break;
  case V_SgDivideOp:
#ifdef PROJDEBUG
    assert(op0_exp != nullptr &&
           (!check_floating_node || is_floating_node(op0_exp)));
    assert(op1_exp != nullptr &&
           (!check_floating_node || is_floating_node(op1_exp)));
#endif
    ret_exp = SageBuilder::buildDivideOp(op0_exp, op1_exp);
    markBuild(this, ret_exp,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_exp),
              bindNode);
    break;
  case V_SgAndOp:
#ifdef PROJDEBUG
    assert(op0_exp != nullptr &&
           (!check_floating_node || is_floating_node(op0_exp)));
    assert(op1_exp != nullptr &&
           (!check_floating_node || is_floating_node(op1_exp)));
#endif
    ret_exp = SageBuilder::buildAndOp(op0_exp, op1_exp);
    markBuild(this, ret_exp,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_exp),
              bindNode);
    break;
  case V_SgOrOp:
#ifdef PROJDEBUG
    assert(op0_exp != nullptr &&
           (!check_floating_node || is_floating_node(op0_exp)));
    assert(op1_exp != nullptr &&
           (!check_floating_node || is_floating_node(op1_exp)));
#endif
    ret_exp = SageBuilder::buildOrOp(op0_exp, op1_exp);
    markBuild(this, ret_exp,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_exp),
              bindNode);
    break;
  case V_SgBitAndOp:
#ifdef PROJDEBUG
    assert(op0_exp != nullptr &&
           (!check_floating_node || is_floating_node(op0_exp)));
    assert(op1_exp != nullptr &&
           (!check_floating_node || is_floating_node(op1_exp)));
#endif
    ret_exp = SageBuilder::buildBitAndOp(op0_exp, op1_exp);
    markBuild(this, ret_exp,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_exp),
              bindNode);
    break;
  case V_SgBitOrOp:
#ifdef PROJDEBUG
    assert(op0_exp != nullptr &&
           (!check_floating_node || is_floating_node(op0_exp)));
    assert(op1_exp != nullptr &&
           (!check_floating_node || is_floating_node(op1_exp)));
#endif
    ret_exp = SageBuilder::buildBitOrOp(op0_exp, op1_exp);
    markBuild(this, ret_exp,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_exp),
              bindNode);
    break;
  case V_SgBitXorOp:
#ifdef PROJDEBUG
    assert(op0_exp != nullptr &&
           (!check_floating_node || is_floating_node(op0_exp)));
    assert(op1_exp != nullptr &&
           (!check_floating_node || is_floating_node(op1_exp)));
#endif
    ret_exp = SageBuilder::buildBitXorOp(op0_exp, op1_exp);
    markBuild(this, ret_exp,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_exp),
              bindNode);
    break;
  case V_SgEqualityOp:
#ifdef PROJDEBUG
    assert(op0_exp != nullptr &&
           (!check_floating_node || is_floating_node(op0_exp)));
    assert(op1_exp != nullptr &&
           (!check_floating_node || is_floating_node(op1_exp)));
#endif
    ret_exp = SageBuilder::buildEqualityOp(op0_exp, op1_exp);
    markBuild(this, ret_exp,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_exp),
              bindNode);
    break;
  case V_SgNotEqualOp:
#ifdef PROJDEBUG
    assert(op0_exp != nullptr &&
           (!check_floating_node || is_floating_node(op0_exp)));
    assert(op1_exp != nullptr &&
           (!check_floating_node || is_floating_node(op1_exp)));
#endif
    ret_exp = SageBuilder::buildNotEqualOp(op0_exp, op1_exp);
    markBuild(this, ret_exp,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_exp),
              bindNode);
    break;
  case V_SgLessThanOp:
#ifdef PROJDEBUG
    assert(op0_exp != nullptr &&
           (!check_floating_node || is_floating_node(op0_exp)));
    assert(op1_exp != nullptr &&
           (!check_floating_node || is_floating_node(op1_exp)));
#endif
    ret_exp = SageBuilder::buildLessThanOp(op0_exp, op1_exp);
    markBuild(this, ret_exp,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_exp),
              bindNode);
    break;
  case V_SgLessOrEqualOp:
#ifdef PROJDEBUG
    assert(op0_exp != nullptr &&
           (!check_floating_node || is_floating_node(op0_exp)));
    assert(op1_exp != nullptr &&
           (!check_floating_node || is_floating_node(op1_exp)));
#endif
    ret_exp = SageBuilder::buildLessOrEqualOp(op0_exp, op1_exp);
    markBuild(this, ret_exp,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_exp),
              bindNode);
    break;
  case V_SgGreaterThanOp:
#ifdef PROJDEBUG
    assert(op0_exp != nullptr &&
           (!check_floating_node || is_floating_node(op0_exp)));
    assert(op1_exp != nullptr &&
           (!check_floating_node || is_floating_node(op1_exp)));
#endif
    ret_exp = SageBuilder::buildGreaterThanOp(op0_exp, op1_exp);
    markBuild(this, ret_exp,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_exp),
              bindNode);
    break;
  case V_SgGreaterOrEqualOp:
#ifdef PROJDEBUG
    assert(op0_exp != nullptr &&
           (!check_floating_node || is_floating_node(op0_exp)));
    assert(op1_exp != nullptr &&
           (!check_floating_node || is_floating_node(op1_exp)));
#endif
    ret_exp = SageBuilder::buildGreaterOrEqualOp(op0_exp, op1_exp);
    markBuild(this, ret_exp,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_exp),
              bindNode);
    break;
  case V_SgDotExp:
#ifdef PROJDEBUG
    assert(op0_exp != nullptr &&
           (!check_floating_node || is_floating_node(op0_exp)));
    assert(op1_exp != nullptr &&
           (!check_floating_node || is_floating_node(op1_exp)));
#endif
    ret_exp = SageBuilder::buildDotExp(op0_exp, op1_exp);
    markBuild(this, ret_exp,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_exp),
              bindNode);
    break;
  case V_SgArrowExp:
#ifdef PROJDEBUG
    assert(op0_exp != nullptr &&
           (!check_floating_node || is_floating_node(op0_exp)));
    assert(op1_exp != nullptr &&
           (!check_floating_node || is_floating_node(op1_exp)));
#endif
    ret_exp = SageBuilder::buildArrowExp(op0_exp, op1_exp);
    markBuild(this, ret_exp,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_exp),
              bindNode);
    break;

  case V_SgLshiftOp:
#ifdef PROJDEBUG
    assert(op0_exp != nullptr &&
           (!check_floating_node || is_floating_node(op0_exp)));
    assert(op1_exp != nullptr &&
           (!check_floating_node || is_floating_node(op1_exp)));
#endif
    ret_exp = SageBuilder::buildLshiftOp(op0_exp, op1_exp);
    markBuild(this, ret_exp,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_exp),
              bindNode);
    break;
  case V_SgRshiftOp:
#ifdef PROJDEBUG
    assert(op0_exp != nullptr &&
           (!check_floating_node || is_floating_node(op0_exp)));
    assert(op1_exp != nullptr &&
           (!check_floating_node || is_floating_node(op1_exp)));
#endif
    ret_exp = SageBuilder::buildRshiftOp(op0_exp, op1_exp);
    markBuild(this, ret_exp,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_exp),
              bindNode);
    break;

  default:
    assert(false);
    break;
  }

  return ret_exp;
}

//  if operand is a var_decl, it will generate a ref_exp automatically.
//  op2 and op3 are only used in SgForStatement
void *
CSageCodeGen::CreateStmt(int stmt_type, void *op0_, void *op1_ /*= nullptr*/,
                         void *op2_ /*= nullptr*/, void *op3_ /*= nullptr*/,
                         void *op4_ /*= nullptr*/, void *op5_ /*= nullptr */,
                         void *op6_ /*= nullptr*/) {
  SgStatement *ret_stmt;
  if (stmt_type == V_SgBasicBlock) {
    if (op0_ == nullptr) {
      ret_stmt = SageBuilder::buildBasicBlock();
    } else {
      bool *group = static_cast<bool *>(op1_);
      if (*group) {
        vector<SgStatement *> *stmt_list =
            static_cast<vector<SgStatement *> *>(op0_);
#ifdef PROJDEBUG
        for (auto stmt : *stmt_list) {
          assert(stmt != nullptr && is_floating_node(stmt));
        }
#endif
        ret_stmt = SageBuilder::buildBasicBlock_nfi(*stmt_list);
      } else {
        SgStatement *stmt = static_cast<SgStatement *>(op0_);
#ifdef PROJDEBUG
        assert(stmt != nullptr && is_floating_node(stmt));
#endif
        ret_stmt = SageBuilder::buildBasicBlock(stmt);
      }
    }
    markBuild(this, ret_stmt,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_stmt),
              op2_);
    return ret_stmt;
  }

  SgNode *op0 = static_cast<SgNode *>(op0_);
  SgNode *op1 = static_cast<SgNode *>(op1_);
  SgNode *op2 = static_cast<SgNode *>(op2_);
  SgNode *op3 = static_cast<SgNode *>(op3_);
  SgNode *op6 = static_cast<SgNode *>(op6_);

  SgVariableDeclaration *op0_decl = isSgVariableDeclaration(op0);
  SgVariableDeclaration *op1_decl = isSgVariableDeclaration(op1);

  SgExpression *op0_exp = isSgExpression(op0);
  SgExpression *op1_exp = isSgExpression(op1);

  if (op0_decl != nullptr) {
    op0_exp = SageBuilder::buildVarRefExp(op0_decl);
    markBuild(this, op0_exp,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)op0_exp));
  }

  if (op1_decl != nullptr) {
    op1_exp = SageBuilder::buildVarRefExp(op1_decl);
    markBuild(this, op1_exp,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)op1_exp));
  }

  switch (stmt_type) {
  case V_SgAssignStatement:
#ifdef PROJDEBUG
    assert(op0_exp != nullptr && is_floating_node(op0_exp));
    assert(op1_exp != nullptr && is_floating_node(op1_exp));
    assert(!isSgAssignInitializer(static_cast<SgNode *>(op1_exp)));
#endif
    ret_stmt = SageBuilder::buildAssignStatement(op0_exp, op1_exp);
    markBuild(this, ret_stmt,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_stmt),
              op2);
    break;
  case V_SgExprStatement:
#ifdef PROJDEBUG
    assert(op0_exp != nullptr && is_floating_node(op0_exp));
#endif
    ret_stmt = SageBuilder::buildExprStatement(op0_exp);
    markBuild(this, ret_stmt,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_stmt),
              op1);
    break;
  case V_SgWhileStmt:
#ifdef PROJDEBUG
    assert(op0_exp != nullptr && is_floating_node(op0_exp));
    assert(op1 != nullptr && is_floating_node(op1));
#endif
    ret_stmt = SageBuilder::buildWhileStmt(op0_exp, isSgStatement(op1));
    markBuild(this, ret_stmt,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_stmt),
              op2);
    break;
  case V_SgBreakStmt:
    ret_stmt = SageBuilder::buildBreakStmt();
    markBuild(this, ret_stmt,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_stmt),
              op0);
    break;
  case V_SgContinueStmt:
    ret_stmt = SageBuilder::buildContinueStmt();
    markBuild(this, ret_stmt,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_stmt),
              op0);
    break;
  case V_SgNullStatement:
    ret_stmt = SageBuilder::buildNullStatement();
    markBuild(this, ret_stmt,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_stmt),
              op0);
    break;
  case V_SgForStatement: {
#ifdef PROJDEBUG
    assert(!op1 || is_floating_node(op1));
    assert(!op2 || is_floating_node(op2));
    assert(!op3 || is_floating_node(op3));
#endif
    //  for (op0 = op1; op0 < op2; op0++) op3;
    SgVariableDeclaration *ivar_decl = isSgVariableDeclaration(op0);
    SgInitializedName *ivar = isSgInitializedName(op0);
    assert(ivar_decl || ivar);
    SgStatement *sgInit = nullptr;
    if (ivar != nullptr) {
      SgExpression *sgLB = isSgExpression(op1);
      assert(sgLB);
      sgInit = static_cast<SgStatement *>(
          CreateStmt(V_SgAssignStatement, CreateVariableRef(ivar), sgLB));
    } else {
      sgInit = ivar_decl;
      ivar = isSgInitializedName(
          static_cast<SgNode *>(GetVariableInitializedName(ivar_decl)));
    }
    void *iterator_type = GetTypebyVar(ivar);
    SgExpression *sgUB = isSgExpression(op2);
    assert(sgUB);
    SgExpression *sgTestExp;
    bool inclusive_bound = false;
    bool has_inclusive_flag = false;
    if (op5_ != nullptr) {
      has_inclusive_flag = true;
      inclusive_bound = *static_cast<bool *>(op5_);
    }
    if (op4_ == nullptr) {
      sgTestExp = static_cast<SgExpression *>(
          CreateExp(has_inclusive_flag && inclusive_bound ? V_SgLessOrEqualOp
                                                          : V_SgLessThanOp,
                    CreateVariableRef(ivar), sgUB));
    } else {
      int64_t nInc = *(static_cast<int64_t *>(op4_));

      if (nInc > 0) {
        sgTestExp = static_cast<SgExpression *>(CreateExp(
            has_inclusive_flag && !inclusive_bound ? V_SgLessThanOp
                                                   : V_SgLessOrEqualOp,
            CreateVariableRef(ivar), sgUB));
      } else {
        sgTestExp = static_cast<SgExpression *>(CreateExp(
            has_inclusive_flag && !inclusive_bound ? V_SgGreaterThanOp
                                                   : V_SgGreaterOrEqualOp,
            CreateVariableRef(ivar), sgUB));
      }
    }

    SgStatement *sgTest =
        static_cast<SgStatement *>(CreateStmt(V_SgExprStatement, sgTestExp));

    SgExpression *sgIncr;
    if (op4_ == nullptr) {
      sgIncr = static_cast<SgExpression *>(CreateExp(
          V_SgPlusPlusOp, CreateVariableRef(ivar), nullptr, SgUnaryOp::prefix));
    } else {
      int64_t nInc = *(static_cast<int64_t *>(op4_));
      int int_type = V_SgTypeLong;
      if (IsIntegerType(iterator_type) != 0) {
        int_type = (static_cast<SgType *>(iterator_type))->variantT();
      } else if (IsXilinxAPIntType(iterator_type) != 0) {
        int_type = GetNearestIntegerType(iterator_type);
      }
      if (nInc == 1) {
        sgIncr = static_cast<SgExpression *>(
            CreateExp(V_SgPlusPlusOp, CreateVariableRef(ivar), nullptr,
                      SgUnaryOp::prefix));
      } else if (nInc == -1) {
        sgIncr = static_cast<SgExpression *>(
            CreateExp(V_SgMinusMinusOp, CreateVariableRef(ivar), nullptr,
                      SgUnaryOp::prefix));
      } else if (nInc > 1) {
        sgIncr = static_cast<SgExpression *>(
            CreateExp(V_SgPlusAssignOp, CreateVariableRef(ivar),
                      CreateConst(&nInc, int_type)));
      } else if (nInc < -1) {
        nInc = -nInc;
        sgIncr = static_cast<SgExpression *>(
            CreateExp(V_SgMinusAssignOp, CreateVariableRef(ivar),
                      CreateConst(&nInc, int_type)));
      } else {
        assert(0);
      }
    }

    SgStatement *sgBody = isSgStatement(op3);
    assert(sgBody);

    ret_stmt = SageBuilder::buildForStatement(sgInit, sgTest, sgIncr, sgBody);
    markBuild(this, ret_stmt,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_stmt),
              op6);
  } break;
  //  case V_SgBasicBlock:
  //    if (stmt_list == nullptr)
  //      ret_stmt = SageBuilder::buildBasicBlock();
  //    else
  //      ret_stmt = SageBuilder::buildBasicBlock_nfi(*stmt_list);
  //    markBuild(* this, ret_stmt, "Build in " + std::to_string(__LINE__) + "
  //    for " + std::to_string((int64_t) ret_stmt)); break;
  case V_SgReturnStmt:
#ifdef PROJDEBUG
    assert(!op0 || is_floating_node(op0));
#endif
    ret_stmt = SageBuilder::buildReturnStmt(op0_exp);
    markBuild(this, ret_stmt,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ret_stmt),
              op1);
    break;
  default:
    assert(false);
    break;
  }

  return ret_stmt;
}

void CSageCodeGen::ReplaceChild(void *sg_old_, void *sg_new_,
                                bool replace_prep_info) {
  tryBindNodeOrgId(this, sg_new_, sg_old_, true);

  //  ZP: 20170527
  if (IsExpression(sg_old_) != 0) {
    assert(IsExpression(sg_new_));
    ReplaceExp(sg_old_, sg_new_);
    return;
  }

  SgStatement *stmt_old = isSgStatement(static_cast<SgNode *>(sg_old_));
  void *scope = stmt_old;
  if (IsFunctionDeclaration(stmt_old) != 0) {
    s_func_call_cache.erase(stmt_old);
    s_func_decl_cache.erase(stmt_old);
    s_g_funcs_cache.clear();
    s_g_func_name_cache[GetFuncName(stmt_old, true)].erase(stmt_old);
    s_def_use_cache.erase(stmt_old);
    scope = GetFuncBody(stmt_old);
  }
  if (scope != nullptr) {
    vector<void *> vec_calls;
    GetNodesByType_int(scope, "preorder", V_SgFunctionCallExp, &vec_calls);
    for (auto call : vec_calls) {
      void *func_decl = GetFuncDeclByCall(call, 1);
      if (func_decl != nullptr) {
        s_func_call_cache.erase(func_decl);
      }
      s_func_decl_cache.erase(call);
    }
  }
  if (scope != nullptr) {
    vector<void *> vec_for_loop;
    GetNodesByType_int(scope, "preorder", V_SgForStatement, &vec_for_loop);
    for (auto loop : vec_for_loop) {
      s_loop_iterator_cache.erase(loop);
      s_loop_label_cache.erase(loop);
    }
  }
  if (IsPragmaDecl(stmt_old) != 0) {
    s_pragma_to_variable_cache.erase(stmt_old);
    for (auto &pair : s_variable_to_pragma_cache) {
      pair.second.erase(stmt_old);
    }
  }
  SgStatement *stmt_new = isSgStatement(static_cast<SgNode *>(sg_new_));
  if (auto new_func = isSgFunctionDeclaration(stmt_new)) {
    if (new_func->get_definition() != nullptr) {
      s_g_func_name_cache[GetFuncName(new_func, true)].insert(new_func);
    }
  }
  string oldStmtStr = printNodeInfo(this, stmt_old);
  SageInterface::replaceStatement(
      stmt_old, stmt_new,
      replace_prep_info);  //  true: move preprocess info like #include
  //  Yuxin: 05/19/18 add by YX
  //  Due to ROSE liveness analysis crash
  if ((stmt_old != nullptr) && (is_floating_node(stmt_old) == 0) &&
      (isSgDeclarationStatement(stmt_old) == nullptr)) {
    //  stmt_old->set_parent(nullptr);
    RemoveStmt(stmt_old);
  }
  if (stmt_new->get_parent() == nullptr ||
      (static_cast<int>(stmt_new->get_parent()->get_childIndex(stmt_new))) <
          0) {
    HIS_ERROR("replace a floating node.");
    HIS_ERROR("parent: " << stmt_new->get_parent());
    if (stmt_new->get_parent() != nullptr) {
      HIS_ERROR("index: " << (static_cast<int>(
                    stmt_new->get_parent()->get_childIndex(stmt_new))));
    }
  } else if (isSkipped(this, stmt_new)) {
    HIS_WARNING("do not mark history for skipped node.");
  } else {
    markDelete(this, stmt_new->get_parent(), getTopoIndex(this, stmt_new),
               "Replace(1): " + oldStmtStr, stmt_old);
    markInsert(this, stmt_new->get_parent(), getTopoIndex(this, stmt_new),
               "Replace(2): " + printNodeInfo(this, stmt_new));
  }
}

bool CSageCodeGen::IsSupportedReplaceExpressionParent(void *exp_parent) {
  return IsExpression(exp_parent) || IsExprStatement(exp_parent) ||
         IsForStatement(exp_parent) || IsReturnStmt(exp_parent) ||
         IsInitName(exp_parent) || IsArrayType(exp_parent) ||
         IsCaseOptionStmt(exp_parent);
}

void CSageCodeGen::ReplaceExp(void *sg_old_, void *sg_new_, bool keepoldExp) {
  assert(IsSupportedReplaceExpressionParent(GetParent(sg_old_)));
  SgExpression *expr_old = isSgExpression(static_cast<SgNode *>(sg_old_));
  vector<void *> vec_calls;
  GetNodesByType_int(expr_old, "preorder", V_SgFunctionCallExp, &vec_calls);
  for (auto call : vec_calls) {
    void *func_decl = GetFuncDeclByCall(call, 1);
    if (func_decl != nullptr) {
      s_func_call_cache.erase(func_decl);
    }
    s_func_decl_cache.erase(call);
  }
  if (isSgExpression(static_cast<SgNode *>(sg_new_)) != nullptr) {
    SgExpression *expr_new = isSgExpression(static_cast<SgNode *>(sg_new_));
    replaceExpressionWithHistory(this, expr_old, expr_new,
                                 keepoldExp);  //  true: keep old expression
  } else {
    assert(0);
  }
}

void CSageCodeGen::DeleteNode(void *sg_node) {
  if (sg_node == nullptr) {
    return;
  }
  assert(GetParent(sg_node) == nullptr);
  vector<void *> res;
  TraverseSimple(sg_node, "postorder", deleteHistory, this);
  TraverseSimple(sg_node, "postorder", traverseSgNodeForReleaseMemory, &res);
}

void CSageCodeGen::RemoveChild(void *sg_child_, bool remove_prep_info) {
  SgStatement *stmt_child = isSgStatement(static_cast<SgNode *>(sg_child_));

  SgStatement *parentStatement = isSgStatement(stmt_child->get_parent());
  void *scope = stmt_child;
  if (IsFunctionDeclaration(stmt_child) != 0) {
    s_func_call_cache.erase(stmt_child);
    s_func_decl_cache.erase(stmt_child);
    s_g_funcs_cache.clear();
    s_g_func_name_cache[GetFuncName(stmt_child, true)].erase(stmt_child);
    s_def_use_cache.erase(stmt_child);
    scope = GetFuncBody(stmt_child);
  }
  if (scope != nullptr) {
    vector<void *> vec_calls;
    GetNodesByType_int(scope, "preorder", V_SgFunctionCallExp, &vec_calls);
    for (auto call : vec_calls) {
      void *func_decl = GetFuncDeclByCall(call, 1);
      if (func_decl != nullptr) {
        s_func_call_cache.erase(func_decl);
      }
      s_func_decl_cache.erase(call);
    }
  }
  if (scope != nullptr) {
    vector<void *> vec_for_loop;
    GetNodesByType_int(scope, "preorder", V_SgForStatement, &vec_for_loop);
    for (auto loop : vec_for_loop) {
      s_loop_iterator_cache.erase(loop);
      s_loop_label_cache.erase(loop);
    }
  }
  if (IsPragmaDecl(stmt_child) != 0) {
    s_pragma_to_variable_cache.erase(stmt_child);
    for (auto &pair : s_variable_to_pragma_cache) {
      pair.second.erase(stmt_child);
    }
  }
  bool isRemovable = (parentStatement != nullptr)
                         ? LowLevelRewrite::isRemovableStatement(stmt_child)
                         : false;
  if (!isRemovable) {
    if (is_floating_node(stmt_child) == 0) {
      ReplaceChild(stmt_child, CreateStmt(V_SgNullStatement));
    }
    return;
  }

  int index = -1;
  if (stmt_child->get_parent() != nullptr) {
    index = stmt_child->get_parent()->get_childIndex(stmt_child);
  }
  if (stmt_child->get_parent() == nullptr || index < 0) {
    HIS_WARNING("remove a floating node.");
    HIS_WARNING("parent: " << stmt_child->get_parent());
    if (stmt_child->get_parent() != nullptr) {
      HIS_WARNING("index: " << index);
      HIS_WARNING("index: " << (static_cast<int>(
                      stmt_child->get_parent()->get_childIndex(stmt_child))));
    }
  } else if (isSkipped(this, stmt_child)) {
    HIS_WARNING("do not mark history for skipped node.");
  } else {
    // index = stmt_child->get_parent()->get_childIndex(stmt_child);
    markDelete(this, stmt_child->get_parent(), getTopoIndex(this, stmt_child),
               "Delete: " + printNodeInfo(this, stmt_child) +
                   "\nfrom: " + printNodeInfo(this, stmt_child->get_parent()),
               stmt_child);
    // index = stmt_child->get_parent()->get_childIndex(stmt_child);
  }
  SageInterface::removeStatement(stmt_child, remove_prep_info);  //  true: move
                                                                 // preprocess
                                                                 // info like
                                                                 // #include
}

void *CSageCodeGen::CopyStmt(void *sg_stmt_, bool passMajor /*= false*/) {
  assert(IsStatement(sg_stmt_) && "Expected a statement to copy.");
  DEBUG(vector<void *> vec_decl; GetNodesByType_int_compatible(
            sg_stmt_, V_SgFunctionDeclaration, &vec_decl);
        assert(vec_decl.empty() &&
               "ROSE backend may crash. Please remove function "
               "declaration before copying");
        vector<void *> vec_floating_refs;
        get_floating_var_refs(sg_stmt_, &vec_floating_refs);
        for (auto ref
             : vec_floating_refs)
            assert(!IsLocalInitName(GetVariableInitializedName(ref)) &&
                   "Cannot update floating references");)

  SgStatement *sg_stmt = isSgStatement(static_cast<SgNode *>(sg_stmt_));
  assert(sg_stmt);
  SgStatement *sg_new_stmt = SageInterface::copyStatement(
      sg_stmt);  //  true: move preprocess info like #include
  markCopy(this, sg_stmt, sg_new_stmt, "Copy: " + printNodeInfo(this, sg_stmt),
           passMajor);
  //  update local references
  //  //////////////////////////////////////////  /
  vector<void *> org_decls;
  vector<void *> new_decls;
  GetNodesByType_int(sg_stmt, "preorder", V_SgVariableDeclaration, &org_decls);
  GetNodesByType_int(sg_new_stmt, "preorder", V_SgVariableDeclaration,
                     &new_decls);
  map<void *, void *> orgVar2newVar;
  for (size_t i = 0; i != org_decls.size(); ++i) {
    SgVariableDeclaration *org_sg_decl =
        isSgVariableDeclaration(static_cast<SgNode *>(org_decls[i]));
    SgVariableDeclaration *new_sg_decl =
        isSgVariableDeclaration(static_cast<SgNode *>(new_decls[i]));
    for (size_t j = 0; j != org_sg_decl->get_variables().size(); ++j) {
      SgInitializedName *org_sg_var = org_sg_decl->get_variables()[j];
      SgInitializedName *new_sg_var = new_sg_decl->get_variables()[j];
      orgVar2newVar[org_sg_var] = new_sg_var;
      if (new_sg_var->search_for_symbol_from_symbol_table() != nullptr) {
        continue;
      }
      //  In order to create a new variable reference which can trace to new var
      //  init, we need to craete a variable symbol for this variable
      new_sg_var->get_scope()->insert_symbol(new_sg_var->get_name(),
                                             new SgVariableSymbol(new_sg_var));
    }
  }
  vector<void *> vec_refs;
  GetNodesByType_int(sg_new_stmt, "preorder", V_SgVarRefExp, &vec_refs);
  for (auto ref : vec_refs) {
    if (GetParent(ref) == nullptr)
      continue;
    void *var_init = GetVariableInitializedName(ref);
    if (orgVar2newVar.count(var_init) > 0) {
      ReplaceExp(ref, CreateVariableRef(orgVar2newVar[var_init], nullptr, ref));
    }
  }

  //  clean_empty_aggregate_initializer(sg_new_stmt);

  GetNodesByType_int(sg_new_stmt, "preorder", V_SgPragmaDeclaration,
                     &new_decls);
  //  YX: work around bug of sgpramgdecl copy to pass sanity check
  for (auto new_pragma : new_decls) {
    SgPragmaDeclaration *new_pragma_decl =
        isSgPragmaDeclaration(static_cast<SgNode *>(new_pragma));
    new_pragma_decl->set_definingDeclaration(new_pragma_decl);
    new_pragma_decl->set_firstNondefiningDeclaration(new_pragma_decl);
  }

  return sg_new_stmt;
}

void *CSageCodeGen::CopyExp(void *sg_exp_, bool passMajor /*= false*/) {
#ifdef PROJDEBUG
  assert(IsExpression(sg_exp_) && "Expect an expression");
#endif
#if 0
  SgAggregateInitializer *sg_aggr_init =
      isSgAggregateInitializer(static_cast<SgNode *>(sg_exp_));
  if (sg_aggr_init != nullptr) {
    vector<void *> new_vec_exp;
    for (auto exp : sg_aggr_init->get_initializers()->get_expressions()) {
      new_vec_exp.push_back(CopyExp(exp, passMajor));
    }
    return CreateAggregateInitializer(new_vec_exp, GetTypeByExp(sg_aggr_init));
  }
#endif
  SgExpression *sg_exp = isSgExpression(static_cast<SgNode *>(sg_exp_));
  assert(sg_exp);
  auto sg_new_exp = SageInterface::copyExpression(
      sg_exp);  //  true: move preprocess info like #include
  if (!IsValueExp(sg_exp_))
    markCopy(this, sg_exp, sg_new_exp, "Copy: " + printNodeInfo(this, sg_exp),
             passMajor);
  return sg_new_exp;
}

void CSageCodeGen::DiscardPreprocInfo(void *sg_stmt_or_exp) {
  SgLocatedNode *sg_node =
      isSgLocatedNode(static_cast<SgNode *>(sg_stmt_or_exp));
  if (sg_node == nullptr) {
    return;
  }
  sg_node->set_attachedPreprocessingInfoPtr(nullptr);
}

void CSageCodeGen::DiscardIncludePreprocInfo(void *sg_stmt_or_exp) {
  SgLocatedNode *sg_node =
      isSgLocatedNode(static_cast<SgNode *>(sg_stmt_or_exp));
  if (sg_node == nullptr) {
    return;
  }
  AttachedPreprocessingInfoType *old_info_list =
      sg_node->get_attachedPreprocessingInfoPtr();
  if (old_info_list == nullptr) {
    return;
  }
  bool contain_include_directive = false;
  for (auto info : *old_info_list) {
    if (info->getTypeOfDirective() ==
        PreprocessingInfo::CpreprocessorIncludeDeclaration) {
      contain_include_directive = true;
    }
  }
  if (!contain_include_directive) {
    return;
  }
  AttachedPreprocessingInfoType *new_info = new AttachedPreprocessingInfoType;
  for (auto info : *old_info_list) {
    if (info->getTypeOfDirective() !=
        PreprocessingInfo::CpreprocessorIncludeDeclaration) {
      new_info->push_back(info);
    }
  }

  sg_node->set_attachedPreprocessingInfoPtr(new_info);
}

void *CSageCodeGen::GetNextStmt_v1(void *sg_stmt_) {
  if (sg_stmt_ == nullptr) {
    return nullptr;
  }
  void *sg_parent = GetParent(sg_stmt_);
  if (sg_parent == nullptr) {
    return nullptr;
  }

  size_t idx = GetChildStmtIdx(sg_parent, sg_stmt_);
  if (idx + 1 == GetChildStmtNum(sg_parent) ||
      idx >= GetChildStmtNum(sg_parent)) {
    return nullptr;
  }

  return GetChildStmt(sg_parent, idx + 1);
}

void *CSageCodeGen::GetNextStmt(void *sg_stmt,
                                bool includingCompilerGenerated) {
  void *sg_next_stmt = GetNextStmt_v1(sg_stmt);
  while (!includingCompilerGenerated && (sg_next_stmt != nullptr) &&
         IsCompilerGenerated(sg_next_stmt)) {
    sg_stmt = sg_next_stmt;
    sg_next_stmt = GetNextStmt_v1(sg_stmt);
  }
  return sg_next_stmt;
}

void *CSageCodeGen::GetPreviousStmt_v1(void *sg_stmt_) {
  if (sg_stmt_ == nullptr) {
    return nullptr;
  }
  void *sg_parent = GetParent(sg_stmt_);
  if (sg_parent == nullptr) {
    return nullptr;
  }

  size_t idx = GetChildStmtIdx(sg_parent, sg_stmt_);
  if (idx < 1 || idx >= GetChildStmtNum(sg_parent)) {
    return nullptr;
  }

  return GetChildStmt(sg_parent, idx - 1);
}

void *CSageCodeGen::GetPreviousStmt(void *sg_stmt,
                                    bool includingCompilerGenerated) {
  void *sg_prev_stmt = GetPreviousStmt_v1(sg_stmt);
  while (!includingCompilerGenerated && (sg_prev_stmt != nullptr) &&
         IsCompilerGenerated(sg_prev_stmt)) {
    sg_stmt = sg_prev_stmt;
    sg_prev_stmt = GetPreviousStmt_v1(sg_stmt);
  }
  return sg_prev_stmt;
}

//  avoid: avoid the label/pragma before a statement
//       insert the stmt before the label/pragma if there is
void CSageCodeGen::InsertStmt(void *sg_stmt_, void *sg_before_,
                              bool avoid_pragma, bool before_label) {
  SgStatement *sg_stmt = isSgStatement(static_cast<SgNode *>(sg_stmt_));
  if (IsFunctionDeclaration(sg_stmt_) != 0) {
    s_g_funcs_cache.clear();
    if (static_cast<SgFunctionDeclaration *>(sg_stmt)->get_definition() !=
        nullptr)
      s_g_func_name_cache[GetFuncName(sg_stmt, true)].insert(sg_stmt);
  }
  void *parent = GetParent(sg_before_);
  while (IsLabelStatement(parent) != 0) {
    sg_before_ = parent;
    parent = GetParent(sg_before_);
  }
  if (before_label && (IsBasicBlock(parent) != 0) &&
      (GetChildStmtIdx(parent, sg_before_) == 0) &&
      (IsLabelStatement(GetParent(parent)) != 0)) {
    sg_before_ = GetParent(parent);
  }

  if (avoid_pragma) {
    void *prev_stmt = GetPreviousStmt(sg_before_);
    while ((prev_stmt != nullptr) && (IsPragmaDecl(prev_stmt) != 0)) {
      sg_before_ = prev_stmt;
      prev_stmt = GetPreviousStmt(sg_before_);
    }
  }
  SgStatement *sg_parent = isSgStatement(static_cast<SgNode *>(parent));
  if ((isSgIfStmt(sg_parent) != nullptr) &&
      isSgIfStmt(sg_parent)->get_conditional() == sg_before_) {
    sg_before_ = parent;
  }

  if ((isSgForInitStatement(sg_parent) != nullptr)) {
    sg_before_ = GetParent(parent);
  }

  if ((isSgSwitchStatement(sg_parent) != nullptr) &&
      sg_before_ == isSgSwitchStatement(sg_parent)->get_item_selector()) {
    sg_before_ = parent;
  }
  if ((IsCaseOptionStmt(parent) != 0) || (IsDefaultOptionStmt(parent) != 0)) {
    if (IsVariableDecl(sg_stmt_) != 0) {
      sg_before_ = parent;
    } else {
      bindNodeOrgId(this, sg_before_, sg_before_);
      SgStatement *sg_before_stmt =
          isSgStatement(static_cast<SgNode *>(sg_before_));
      sg_before_stmt->set_parent(nullptr);
      SgCaseOptionStmt *case_stmt =
          isSgCaseOptionStmt(static_cast<SgNode *>(parent));
      SgDefaultOptionStmt *default_stmt =
          isSgDefaultOptionStmt(static_cast<SgNode *>(parent));
      if (case_stmt != nullptr) {
        case_stmt->set_body(sg_stmt);
        sg_stmt->set_parent(case_stmt);
      }
      if (default_stmt != nullptr) {
        default_stmt->set_body(sg_stmt);
        sg_stmt->set_parent(default_stmt);
      }
      InsertAfterStmt(sg_before_stmt, parent);
      return;
    }
  }
  SgStatement *sg_before = isSgStatement(static_cast<SgNode *>(sg_before_));
  SgScopeStatement *orig_scope = sg_before->get_scope();
  bool reset = false;
  if ((IsFunctionDeclaration(sg_stmt) != 0) &&
      (GetFuncBody(sg_stmt) == nullptr)) {
    SgScopeStatement *orig_parent =
        isSgScopeStatement(static_cast<SgNode *>(sg_before->get_parent()));
    if (orig_parent != orig_scope && (orig_parent != nullptr)) {
      sg_before->set_scope(orig_parent);
      reset = true;
    }
  }
  SageInterface::insertStatement(sg_before, sg_stmt,
                                 true);  //  true: insert before
  if (reset) {
    sg_before->set_scope(orig_scope);
  }
  if (isSkipped(this, sg_stmt)) {
    HIS_ERROR("do not mark history for skipped node.");
  } else {
    markInsert(this, sg_before->get_parent(), getTopoIndex(this, sg_stmt),
               "Insert: " + printNodeInfo(this, sg_stmt) +
                   "\nfrom: " + printNodeInfo(this, sg_before_));
  }
}
void CSageCodeGen::InsertAfterStmt(void *sg_stmt_, void *sg_after_) {
  void *parent = GetParent(sg_after_);
  while (IsLabelStatement(parent) != 0) {
    sg_after_ = parent;
    parent = GetParent(sg_after_);
  }
  SgStatement *sg_parent = isSgStatement(static_cast<SgNode *>(parent));
  if ((isSgIfStmt(sg_parent) != nullptr) &&
      isSgIfStmt(sg_parent)->get_conditional() == sg_after_) {
    sg_after_ = parent;
  }

  if ((isSgForInitStatement(sg_parent) != nullptr)) {
    sg_after_ = GetParent(parent);
  }

  if ((isSgSwitchStatement(sg_parent) != nullptr) &&
      sg_after_ == isSgSwitchStatement(sg_parent)->get_item_selector()) {
    sg_after_ = parent;
  }
  if ((IsCaseOptionStmt(parent) != 0) || (IsDefaultOptionStmt(parent) != 0)) {
    sg_after_ = parent;
  }
  SgStatement *sg_stmt = isSgStatement(static_cast<SgNode *>(sg_stmt_));
  SgStatement *sg_after = isSgStatement(static_cast<SgNode *>(sg_after_));
  SgScopeStatement *orig_scope = sg_after->get_scope();
  bool reset = false;
  if (IsFunctionDeclaration(sg_stmt) != 0) {
    if (GetFuncBody(sg_stmt) == nullptr) {
      SgScopeStatement *orig_parent =
          isSgScopeStatement(static_cast<SgNode *>(sg_after->get_parent()));
      if (orig_parent != orig_scope && (orig_parent != nullptr)) {
        sg_after->set_scope(orig_parent);
        reset = true;
      }
    } else {
      if (static_cast<SgFunctionDeclaration *>(sg_stmt)->get_definition() !=
          nullptr)
        s_g_func_name_cache[GetFuncName(sg_stmt, true)].insert(sg_stmt);
    }
    s_g_funcs_cache.clear();
  }
  SageInterface::insertStatement(sg_after, sg_stmt,
                                 false);  //  true: insert before
  if (reset) {
    sg_after->set_scope(orig_scope);
  }
  if (isSkipped(this, sg_stmt)) {
    HIS_WARNING("do not mark history for skipped node.");
  } else {
    markInsert(this, sg_after->get_parent(), getTopoIndex(this, sg_stmt),
               "Insert: " + printNodeInfo(this, sg_stmt) +
                   "\nfrom: " + printNodeInfo(this, sg_after_));
  }
}

void CSageCodeGen::sanity_check() {
#if _DEBUG_ROSE_SANITY_CHECK
  AstTests::runAllTests(m_sg_project);
#endif
}

void CSageCodeGen::GenerateCode() {
  freopen("rose.log", "w", stdout);
  freopen("rose.log", "w", stderr);
  sanity_check();
  DEFINE_START_END;
  STEMP(start);
  backend(m_sg_project);
  ACCTM(backend, start, end);

  freopen("/dev/tty", "w", stdout);
  freopen("/dev/tty", "w", stderr);
}

void CSageCodeGen::AddDirectives(const string &sDirective, void *sg_node_,
                                 int before /*=1*/) {
  SgLocatedNode *sg_node = isSgLocatedNode(static_cast<SgNode *>(sg_node_));
  assert(sg_node);
  if (before != 0) {
    SageInterface::addTextForUnparser(sg_node, sDirective,
                                      AstUnparseAttribute::e_before);
  } else {
    SageInterface::addTextForUnparser(sg_node, sDirective,
                                      AstUnparseAttribute::e_after);
  }
}

void CSageCodeGen::AddComment(const string &sComment, void *sg_node_before) {
  SageInterface::attachComment(
      isSgLocatedNode(static_cast<SgNode *>(sg_node_before)), sComment);
}

void CSageCodeGen::AddHeader(const string &sDirective, void *sg_file_pos) {
  SgGlobal *sg_global =
      isSgGlobal(static_cast<SgNode *>(TraceUpByType(sg_file_pos, "SgGlobal")));
  assert(sg_global);
#if 0
    //  hidden declarations make the following invalid
    for (i = 0; i < sg_global->get_declarations().size(); i++) {
        SgDeclarationStatement * sg_first_decl =
          sg_global->get_declarations()[i];

        //  cout << UnparseToString(sg_first_decl) << endl;

        if ("" == UnparseToString(sg_first_decl)) continue;
        AddDirectives(sDirective, sg_first_decl);
        break;
    }
#else
  AddDirectives(sDirective, sg_global);
  markIncludeMacro(this, sg_global, sDirective, 1);
#endif
}

void CSageCodeGen::InsertHeader(const string &header_file_name, void *sg_stmt_,
                                int before, int isSystemHeader) {
  SgStatement *sg_stmt = isSgStatement(static_cast<SgNode *>(sg_stmt_));
  if (sg_stmt == nullptr) {
    return;
  }

  string content;
  if (isSystemHeader != 0) {
    content = "#include <" + header_file_name + ">\n";
  } else {
    content = "#include \"" + header_file_name + "\"\n";
  }

  PreprocessingInfo *result = new PreprocessingInfo(
      PreprocessingInfo::CpreprocessorIncludeDeclaration, content,
      "Transformation generated", 0, 0, 0,
      before != 0 ? PreprocessingInfo::before : PreprocessingInfo::after);
  ROSE_ASSERT(result);
  sg_stmt->addToAttachedPreprocessingInfo(
      result,
      before != 0 ? PreprocessingInfo::after : PreprocessingInfo::before);
  markIncludeFile(this, sg_stmt, header_file_name, before);
}

void *CSageCodeGen::CopyChild(void *sg_node) {
  if (IsStatement(sg_node) != 0) {
    return CopyStmt(sg_node);
  }
  if (IsExpression(sg_node) != 0) {
    return CopyExp(sg_node);
  }
  return nullptr;
}

void CSageCodeGen::AppendChild(void *sg_scope_, void *sg_child_) {
  SgGlobal *sg_global = isSgGlobal(static_cast<SgNode *>(sg_scope_));
  SgScopeStatement *sg_scope =
      isSgScopeStatement(static_cast<SgNode *>(sg_scope_));
  //  (isSgGlobal((SgNode*)sg_global))->append_declaration(isSgFunctionDeclaration((SgNode*)sg_func_decl));

  if (IsFunctionDeclaration(sg_child_) != 0) {
    s_g_funcs_cache.clear();
    if (static_cast<SgFunctionDeclaration *>(sg_child_)->get_definition() !=
        nullptr)
      s_g_func_name_cache[GetFuncName(sg_child_, true)].insert(sg_child_);
  }
  if (sg_global != nullptr) {
    //  including SgClassDeclaration, SgFunctionDeclaration,
    //  SgVariableDeclaration, ...
    SgDeclarationStatement *sg_decl =
        isSgDeclarationStatement(static_cast<SgNode *>(sg_child_));
    assert(sg_decl);
    int index = getChildrenNumWithoutInclude(this, sg_global);
    sg_global->append_declaration(sg_decl);
    if (isSkipped(this, sg_decl)) {
      HIS_ERROR("do not mark history for skipped node.");
    } else {
      markInsert(this, sg_global, index,
                 "Insert: " + printNodeInfo(this, sg_child_));
    }
  } else if (sg_scope != nullptr) {
    if (IsLoopStatement(sg_scope)) {
      // in order to work around reference induct issue, use loop body instead
      // of loop statement itself
      sg_scope =
          isSgScopeStatement(static_cast<SgNode *>(GetLoopBody(sg_scope)));
      assert(sg_scope != nullptr);
    }
    SgStatement *sg_stmt = isSgStatement(static_cast<SgNode *>(sg_child_));
    assert(sg_stmt);
    int index = getChildrenNumWithoutInclude(this, sg_scope);
    sg_scope->append_statement(sg_stmt);
    if (isSkipped(this, sg_stmt)) {
      HIS_ERROR("do not mark history for skipped node.");
    } else {
      markInsert(this, sg_scope, index,
                 "Insert: " + printNodeInfo(this, sg_child_));
    }
  } else {
    assert(0);
  }
}

void CSageCodeGen::PrependChild(void *sg_scope_, void *sg_child_,
                                bool insert_after_pragma) {
  // MER-2325 insert new stmt after pragma
  if (insert_after_pragma) {
    void *prev_stmt = nullptr;
    bool found_pragma = false;
    for (auto child : GetChildStmts(sg_scope_)) {
      if (IsPragmaDecl(child)) {
        found_pragma = true;
        prev_stmt = child;
      } else {
        break;
      }
    }
    if (found_pragma) {
      InsertAfterStmt(sg_child_, prev_stmt);
      return;
    }
  }

  SgGlobal *sg_global = isSgGlobal(static_cast<SgNode *>(sg_scope_));
  SgScopeStatement *sg_scope =
      isSgScopeStatement(static_cast<SgNode *>(sg_scope_));

  if (IsFunctionDeclaration(sg_child_) != 0) {
    s_g_funcs_cache.clear();
    if (static_cast<SgFunctionDeclaration *>(sg_child_)->get_definition() !=
        nullptr)
      s_g_func_name_cache[GetFuncName(sg_child_, true)].insert(sg_child_);
  }
  if (sg_global != nullptr) {
    //  including SgClassDeclaration, SgFunctionDeclaration,
    //  SgVariableDeclaration, ...
    SgDeclarationStatement *sg_decl =
        isSgDeclarationStatement(static_cast<SgNode *>(sg_child_));
    assert(sg_decl);
    sg_global->prepend_declaration(sg_decl);
    if (isSkipped(this, sg_decl)) {
      HIS_ERROR("do not mark history for skipped node.");
    } else {
      markInsert(this, sg_global, 0,
                 "Insert: " + printNodeInfo(this, sg_child_));
    }
  } else if (sg_scope != nullptr) {
    if (IsLoopStatement(sg_scope)) {
      // in order to work around reference induct issue, use loop body instead
      // of loop statement itself
      sg_scope =
          isSgScopeStatement(static_cast<SgNode *>(GetLoopBody(sg_scope)));
      assert(sg_scope != nullptr);
    }
    SgStatement *sg_stmt = isSgStatement(static_cast<SgNode *>(sg_child_));
    assert(sg_stmt);
    sg_scope->prepend_statement(sg_stmt);
    if (isSkipped(this, sg_stmt)) {
      HIS_ERROR("do not mark history for skipped node.");
    } else {
      markInsert(this, sg_scope, 0,
                 "Insert: " + printNodeInfo(this, sg_child_));
    }
  } else {
    assert(0);
  }
}

void *CSageCodeGen::CreateBasicBlock(void *stmt_list, bool group,
                                     void *bindNode /*= nullptr*/) {
  return CreateStmt(V_SgBasicBlock, stmt_list, &group, bindNode);
}

void *CSageCodeGen::GetParent(void *sg_node_) const {
  //  if (!sg_node_) return nullptr;
  assert(sg_node_);
  const SgNode *sg_node = static_cast<SgNode *>(sg_node_);
  return sg_node->get_parent();
}

typedef struct {
  CSageCodeGen *codegen;
  void *sg_scope;
  vector<void *> *vec_nodes;
} func_trace_up_ARG_TYPE;

typedef struct {
  CSageCodeGen *codegen;
  vector<void *> *vecSize;
  void **base_type;
  void **sg_array;
} func_trace_up_type_ARG_TYPE;

int CSageCodeGen::GetFuncParamIndex(void *sg_arg, void *sg_scope /*=nullptr*/) {
  SgInitializedName *sg_name =
      isSgInitializedName(static_cast<SgNode *>(sg_arg));
  if (sg_name == nullptr) {
    return -1;
  }
  SgDeclarationStatement *sg_decl = sg_name->get_declaration();
  if (sg_decl == nullptr) {
    return -1;
  }
  SgFunctionParameterList *fpl = isSgFunctionParameterList(sg_decl);
  if (fpl == nullptr) {
    return -1;
  }
  //  ZP: 2016-03-03
  if ((sg_scope != nullptr) && (is_located_in_scope(sg_decl, sg_scope) == 0)) {
    return -1;
  }
  return fpl->get_childIndex(sg_name);
}

int CSageCodeGen::GetFuncCallParamIndex(void *sg_arg) {
  //  void *sg_call = TraceUpByTypeCompatible(sg_arg, V_SgFunctionCallExp);
  //
  //  if (!sg_call)
  //    return -1;
  //
  //  int index = -1;
  //  int i;
  //  for (i = 0; i < GetFuncCallParamNum(sg_call); i++) {
  //    if (GetFuncCallParam(sg_call, i) == sg_arg)
  //      index = i;
  //  }
  //
  //  return index;
  //  YX: 20180308: treat the rightmost of comma expression
  //
  void *curr_exp = sg_arg;
  void *sg_parent = GetParent(curr_exp);
  while ((IsCommaExp(sg_parent) != 0) &&
         GetExpRightOperand(sg_parent) == curr_exp) {
    curr_exp = sg_parent;
    sg_parent = GetParent(curr_exp);
  }
  if (isSgExprListExp(static_cast<SgNode *>(sg_parent)) == nullptr) {
    return -1;
  }
  void *func_call = GetParent(sg_parent);
  if (IsFunctionCall(func_call) == 0) {
    return -1;
  }
  int index = -1;
  size_t i;
  SgExpressionPtrList &exps =
      isSgExprListExp(static_cast<SgNode *>(sg_parent))->get_expressions();
  for (i = 0; i < exps.size(); i++) {
    if (exps[i] == curr_exp) {
      index = i;
    }
  }

  return index;
}

int CSageCodeGen::GetFuncCallParamIndexRecur(void *sg_arg) {
  if (GetEnclosingNode("FunctionCallExp", sg_arg) == nullptr) {
    return -1;
  }
  void *sg_param_list = GetParent(sg_arg);
  while (isSgExprListExp(static_cast<SgNode *>(sg_param_list)) == nullptr) {
    // Yuxin: Oct 09 2019
    // Considering sg_arg may not be an argument in the param_list
    // e.g. a->write();
    if (IsFunctionCall(sg_param_list)) {
      return -1;
    }
    sg_param_list = GetParent(sg_param_list);
  }
  void *func_call = GetParent(sg_param_list);
  if (IsFunctionCall(func_call) == 0) {
    return -1;
  }
  int index = -1;
  size_t i;
  SgExpressionPtrList &exps =
      isSgExprListExp(static_cast<SgNode *>(sg_param_list))->get_expressions();
  for (i = 0; i < exps.size(); i++) {
    if (IsInScope(sg_arg, exps[i]) != 0) {
      index = i;
    }
  }

  return index;
}
void CSageCodeGen::GetFuncCallsByName(const string &s_name, void *sg_scope,
                                      vector<void *> *vec_calls) {
  vector<void *> all_calls;
  GetNodesByType_int(sg_scope, "preorder", V_SgFunctionCallExp, &all_calls);

  for (size_t i = 0; i < all_calls.size(); i++) {
    if (GetFuncNameByCall(all_calls[i]) == s_name) {
      vec_calls->push_back(all_calls[i]);
    }
  }
}

void CSageCodeGen::GetSubFuncDeclsRecursively(void *sg_func_decl,
                                              SetVector<void *> *sub_decls,
                                              SetVector<void *> *sub_calls,
                                              bool required_body) {
  if (IsFunctionDeclaration(sg_func_decl) == 0) {
    return;
  }
  if (GetFuncBody(sg_func_decl) == nullptr) {
    return;
  }

  vector<void *> all_calls;
  GetNodesByType_int(GetFuncBody(sg_func_decl), "preorder", V_SgFunctionCallExp,
                     &all_calls);
  for (auto call : all_calls) {
    void *sg_curr_decl = GetFuncDeclByCall(call, 1);
    if (sg_curr_decl == nullptr) {
      if (!required_body) {
        sg_curr_decl = GetFuncDeclByCall(call, 0);
      }
      if (sg_curr_decl == nullptr) {
        continue;
      }
    }
    sub_calls->insert(call);
    if (sub_decls->find(sg_curr_decl) == sub_decls->end()) {
      sub_decls->insert(sg_curr_decl);
      GetSubFuncDeclsRecursively(sg_curr_decl, sub_decls, sub_calls,
                                 required_body);
    }
  }
}

void CSageCodeGen::GetSubFuncDeclsRecursively(
    void *sg_func_decl, SetVector<void *> *sub_decls,
    map<void *, SetVector<void *>> *callee2Callers, bool required_body) {
  if (IsFunctionDeclaration(sg_func_decl) == 0) {
    return;
  }
  if (GetFuncBody(sg_func_decl) == nullptr) {
    return;
  }

  vector<void *> all_calls;
  GetNodesByType_int(GetFuncBody(sg_func_decl), "preorder", V_SgFunctionCallExp,
                     &all_calls);
  for (auto call : all_calls) {
    void *sg_curr_decl = GetFuncDeclByCall(call, 1);
    if (sg_curr_decl == nullptr) {
      if (!required_body) {
        sg_curr_decl = GetFuncDeclByCall(call, 0);
      }
      if (sg_curr_decl == nullptr) {
        continue;
      }
    }
    (*callee2Callers)[sg_curr_decl].insert(sg_func_decl);
    if (sub_decls->find(sg_curr_decl) == sub_decls->end()) {
      sub_decls->insert(sg_curr_decl);
      GetSubFuncDeclsRecursively(sg_curr_decl, sub_decls, callee2Callers,
                                 required_body);
    }
  }
}

void CSageCodeGen::GetFuncCallsFromDecl(void *sg_func_decl, void *sg_scope,
                                        vector<void *> *vec_calls) {
  //    cout << "[GetCallFromDecl] " << GetFuncName(sg_func_decl) << endl;
  assert(IsFunctionDeclaration(sg_func_decl));
  //  assert(GetFuncBody(sg_func_decl));

  //  ZP: 20150825
  //  For the performance purpose, this function is very heavily loaded
  //  A cache mechanism is applied
  //  NOTE:
  //      1. The assumption is made that the function structure is not changed
  //      2. The scope information is ignored (FIXME)
  if (_USE_CACHE_IN_TRAVERSE_) {
    if (sg_scope == nullptr || sg_scope == m_sg_project) {
      if (s_func_call_cache.find(sg_func_decl) != s_func_call_cache.end()) {
        auto res = s_func_call_cache[sg_func_decl];
        bool valid = true;
        for (auto call : res) {
          if (!IsMatchedFuncAndCall(sg_func_decl, call)) {
            valid = false;
            break;
          }
        }
        if (valid) {
          *vec_calls = res;
          return;
        }
      }
    }
  }

  //  /////////  /
  //  ZP: 20160923: to suppport static function
  //  Note: if static, only compare name
  vector<void *> all_calls;
  GetNodesByType_int(sg_scope, "postorder", V_SgFunctionCallExp, &all_calls);
  for (auto call : all_calls) {
    if (IsMatchedFuncAndCall(sg_func_decl, call)) {
      vec_calls->push_back(call);
    }
  }

  if (sg_scope == nullptr || sg_scope == m_sg_project) {
    s_func_call_cache[sg_func_decl] = *vec_calls;
  }
}

//  only handle 1-D array
//  return 0 if not 1-D, or the size is not determined
int CSageCodeGen::analyze_type_with_new_and_malloc(void *sg_array,
                                                   void *sg_scope,
                                                   vector<size_t> *vecSizes) {
  size_t i;
  void *basic_type1;
  vector<size_t> vecSizes1;
  get_type_dimension(GetTypebyVar(sg_array), &basic_type1, &vecSizes1,
                     sg_array);

  if (vecSizes1.size() != 1) {
    return 0;
  }
  if (static_cast<int64_t>(vecSizes1[0]) > 0) {
    *vecSizes = vecSizes1;
    return 1;
  }

  int found = 0;
  int unit_size = get_sizeof_arbitrary_precision_integer(
      get_bitwidth_from_type(basic_type1));
  if (unit_size == 0) {
    return 0;
  }
  //  handling malloc
  {
    vector<void *> vec_mallocs;
    GetFuncCallsByName("malloc", sg_scope, &vec_mallocs);

    for (i = 0; i < vec_mallocs.size(); i++) {
      void *one_call = vec_mallocs[i];

      void *parent = GetParent(one_call);

      while (IsCastExp(parent) != 0) {
        parent = GetParent(parent);
      }

      void *var_name;
      void *sg_value;
      int ret = parse_assign(parent, &var_name, &sg_value);
      if (ret == 0) {
        continue;
      }
      if (var_name == sg_array) {
        if (found != 0) {
          return 0;
        }
        found = 1;

        void *exp_size = GetFuncCallParam(one_call, 0);
        if (exp_size == nullptr) {
          return 0;
        }
        CMarsExpression me(exp_size, this, one_call);
        if (me.IsConstant() != 0) {
          vecSizes1[0] = me.GetConstant() / unit_size;
        } else {
          CMarsRangeExpr mr = me.get_range();

          mr.refine_range_in_scope(nullptr, one_call);

          if (mr.is_const_ub() != 0) {
            vecSizes1[0] = mr.get_const_ub() / unit_size;
          }
        }
      }
    }
  }
  //  handling cmost_malloc_1d
  {
    vector<void *> vec_mallocs;
    GetFuncCallsByName("cmost_malloc_1d", sg_scope, &vec_mallocs);

    for (i = 0; i < vec_mallocs.size(); i++) {
      void *one_call = vec_mallocs[i];

      void *var_exp = GetFuncCallParam(one_call, 0);
      if (var_exp == nullptr) {
        continue;
      }
      remove_cast(&var_exp);
      if (IsAddressOfOp(var_exp) != 0) {
        var_exp = GetExpUniOperand(var_exp);
      }

      void *var_name = GetVariableInitializedName(var_exp);

      if (var_name == sg_array) {
        if (found != 0) {
          return 0;
        }
        found = 1;

        CMarsExpression me1(GetFuncCallParam(one_call, 2), this, one_call);
        CMarsExpression me2(GetFuncCallParam(one_call, 3), this, one_call);
        if ((me1.IsConstant() != 0) && (me2.IsConstant() != 0)) {
          vecSizes1[0] = me1.GetConstant() * me2.GetConstant() / unit_size;
        } else {
          CMarsExpression me = me1 * me2;
          CMarsRangeExpr mr = me.get_range();
          mr.refine_range_in_scope(nullptr, one_call);
          if (mr.is_const_ub() != 0) {
            vecSizes1[0] = mr.get_const_ub() / unit_size;
          }
        }
      }
    }
  }
  //  handling cmost_malloc
  {
    vector<void *> vec_mallocs;
    GetFuncCallsByName("cmost_malloc", sg_scope, &vec_mallocs);

    for (i = 0; i < vec_mallocs.size(); i++) {
      void *one_call = vec_mallocs[i];

      void *var_exp = GetFuncCallParam(one_call, 0);

      remove_cast(&var_exp);
      //  if (IsCastExp(var_exp))
      //  var_exp = GetExpUniOperand(var_exp);
      if (IsAddressOfOp(var_exp) != 0) {
        var_exp = GetExpUniOperand(var_exp);
      }

      void *var_name = GetVariableInitializedName(var_exp);

      if (var_name == sg_array) {
        if (found != 0) {
          return 0;
        }
        found = 1;

        void *exp_size = GetFuncCallParam(one_call, 1);
        CMarsExpression me(exp_size, this, one_call);
        if (me.IsConstant() != 0) {
          vecSizes1[0] = me.GetConstant() / unit_size;
        } else {
          CMarsRangeExpr mr = me.get_range();
          mr.refine_range_in_scope(nullptr, one_call);
          if (mr.is_const_ub() != 0) {
            vecSizes1[0] = mr.get_const_ub() / unit_size;
          }
        }
      }
    }
  }
  //  handling posix_memalign
  {
    vector<void *> vec_mallocs;
    GetFuncCallsByName("posix_memalign", sg_scope, &vec_mallocs);

    for (i = 0; i < vec_mallocs.size(); i++) {
      void *one_call = vec_mallocs[i];

      void *var_exp = GetFuncCallParam(one_call, 0);

      //      if (IsCastExp(var_exp))
      //        var_exp = GetExpUniOperand(var_exp);
      remove_cast(&var_exp);
      if (IsAddressOfOp(var_exp) != 0) {
        var_exp = GetExpUniOperand(var_exp);
      }

      void *var_name = GetVariableInitializedName(var_exp);

      if (var_name == sg_array) {
        if (found != 0) {
          return 0;
        }
        found = 1;

        void *exp_size = GetFuncCallParam(one_call, 2);
        CMarsExpression me(exp_size, this, one_call);
        if (me.IsConstant() != 0) {
          vecSizes1[0] = me.GetConstant() / unit_size;
        } else {
          CMarsRangeExpr mr = me.get_range();
          mr.refine_range_in_scope(nullptr, one_call);
          if (mr.is_const_ub() != 0) {
            vecSizes1[0] = mr.get_const_ub() / unit_size;
          }
        }
      }
    }
  }
  //  handling calloc
  {
    vector<void *> vec_mallocs;
    GetFuncCallsByName("calloc", sg_scope, &vec_mallocs);

    for (i = 0; i < vec_mallocs.size(); i++) {
      void *one_call = vec_mallocs[i];

      void *parent = GetParent(one_call);

      while (IsCastExp(parent) != 0) {
        parent = GetParent(parent);
      }
      //      if (IsCastExp(parent))
      //        parent = GetParent(parent);

      void *var_name;
      void *sg_value;
      int ret = parse_assign(parent, &var_name, &sg_value);
      if (ret == 0) {
        continue;
      }
      if (var_name == sg_array) {
        if (found != 0) {
          return 0;
        }
        found = 1;

        CMarsExpression me1(GetFuncCallParam(one_call, 0), this, one_call);
        CMarsExpression me2(GetFuncCallParam(one_call, 1), this, one_call);
        if ((me1.IsConstant() != 0) && (me2.IsConstant() != 0)) {
          vecSizes1[0] = me1.GetConstant() * me2.GetConstant() / unit_size;
        } else {
          CMarsExpression me = me1 * me2;
          CMarsRangeExpr mr = me.get_range();
          mr.refine_range_in_scope(nullptr, one_call);
          if (mr.is_const_ub() != 0) {
            vecSizes1[0] = mr.get_const_ub() / unit_size;
          }
        }
      }
    }
  }
  //  handling new
  {
    vector<void *> vec_news;
    GetNodesByType_int(sg_scope, "preorder", V_SgNewExp, &vec_news);

    for (i = 0; i < vec_news.size(); i++) {
      void *one_new = vec_news[i];

      void *parent = GetParent(one_new);

      //  if (IsCastExp(parent))
      //  parent = GetParent(parent);
      while (IsCastExp(parent) != 0) {
        parent = GetParent(parent);
      }

      void *var_name;
      void *sg_value;
      int ret = parse_assign(parent, &var_name, &sg_value);
      if (ret == 0) {
        continue;
      }
      if (var_name == sg_array) {
        if (found != 0) {
          return 0;
        }
        found = 1;
        void *sg_type = GetTypebyNewExp(one_new);

        void *basic_type2;
        vector<size_t> vecSize2;
        get_type_dimension(sg_type, &basic_type2, &vecSize2, one_new);

        int64_t nsize = vecSize2[0];

        if (nsize <= 0) {
          SgArrayType *arr_type = isSgArrayType(static_cast<SgNode *>(sg_type));
          if (arr_type != nullptr) {
            void *dim_exp = arr_type->get_index();
            if (IsNullExp(dim_exp) != 0) {
              nsize = EMPTY_SIZE;
            } else {
              CMarsExpression me(dim_exp, this, one_new);
              CMarsRangeExpr mr = me.get_range();
              mr.refine_range_in_scope(nullptr, one_new);
              if (mr.is_const_ub() != 0) {
                nsize = mr.get_const_ub();
#if 0
              //  ZP: fix the dim problem: the position of dim_exp is fake
              //    that make def-use fails
              } else {
                void *new_dim_exp = CopyExp(dim_exp);
                CTempASTStmt tmp(new_dim_exp, this, one_new);
                tmp.reset_defuse();

                CMarsExpression me(new_dim_exp, this, one_new);
                CMarsRangeExpr mr = me.get_range();
                mr.refine_range_in_scope(nullptr, one_new);
                if (mr.is_const_ub()) {
                  nsize = mr.get_const_ub();
                } else {
                  //  FIXME: ZP
                  //  20151115: ROSE has bug in def-use analysis
                  //  for the expressions in "new" expression
                  //  So the range propagation will fails in this case
                  //  assert(0);
                }
#endif
              }
            }
          }
        }

        vecSizes1[0] = nsize;
      }
    }
  }

  if (found != 0) {
    *vecSizes = vecSizes1;
  }

  return found;
}

//  return the array init name if the array is in the argument list,
//  otherwise return 0
//  if sg_pntr is a scalar reference, return 0;
void *CSageCodeGen::parse_pntr_type_in_function(void *sg_pntr, void **sg_array,
                                                void **base_type,
                                                vector<size_t> *vecSizes,
                                                int *pointer_dim) {
  *sg_array = nullptr;
  vector<void *> sg_indexes;
  parse_pntr_ref_new(sg_pntr, sg_array, &sg_indexes);
  *pointer_dim = sg_indexes.size();
  //  int dim = sg_indexes.size();
  //  vecSizes.resize(dim);
  //  for (auto &tt:vecSizes) tt = 0;

  if (*sg_array == nullptr) {
    return nullptr;  //  sg_pntr is not a pntr
  }

  void *sg_type = GetTypebyVar(*sg_array);
  assert(sg_type);

  vector<size_t> vecAllSizes;
  vector<size_t> vecSizes1;
  get_type_dimension(sg_type, base_type, &vecAllSizes, sg_pntr);

  if (vecAllSizes.empty()) {  //  scalar
    //  sg_array, base_type and pointer_dim are all set already
    return nullptr;
  }

  //  handling 1-D new or malloc (only handle the lowest dimension currently)
  if (static_cast<int64_t>(vecAllSizes[0]) <= 0) {
    void *sg_scope = GetScope(*sg_array);
    if (-1 != GetFuncParamIndex(*sg_array)) {
      sg_scope = GetFuncBody(TraceUpToFuncDecl(*sg_array));
    }
    assert(IsScopeStatement(sg_scope));

    vector<size_t> vecSize2;
    int ret = analyze_type_with_new_and_malloc(*sg_array, sg_scope, &vecSize2);
    //    assert(ret);
    if ((ret != 0) && !vecSize2.empty()) {
      vecAllSizes[0] = vecSize2[0];
    }
  }

  //  only return the lower (right) dimensions, because the upper dimensions are
  //  determined
  size_t n_size = vecAllSizes.size() - sg_indexes.size();
  //  for (i = sg_indexes.size(); i < vecAllSizes.size(); i++)
  for (size_t i = 0; i < n_size; i++) {
    //  the order in vecSizes is from left to right
    vecSizes1.push_back(vecAllSizes[n_size - 1 - i]);
  }

  //  if the type is determined not further tracing up is required
  int determined = 1;
  for (size_t i = 0; i < vecSizes1.size(); i++) {
    if (static_cast<int64_t>(vecSizes1[i]) <= 0) {
      determined = 0;
    }
  }

  //  3. output vecSizes (the dimension in the funcion), and init_name
  if (determined != 0) {
    *vecSizes = vecSizes1;
  }

  if (-1 != GetFuncParamIndex(*sg_array) && (determined == 0)) {
    return *sg_array;
  }
  return nullptr;
}

//  This function trace the source of the function call parameters
//  The results can be
//  1. local/global variable ref: push_back
//  2. function argument ref: recurse
//  3. general expression: push_back
//
//  return: the func arg name for the next trace step
//  sg_curr_param is the parameter in the function call
void *func_trace_up_source(void *sg_curr_param, void *pArg) {
  func_trace_up_ARG_TYPE *arg = static_cast<func_trace_up_ARG_TYPE *>(pArg);
  CSageCodeGen *codegen = arg->codegen;
  vector<void *> *vec_nodes = arg->vec_nodes;
  void *sg_scope = arg->sg_scope;

  //    cout << "[func_print] " << codegen->UnparseToString(sg_curr_param) <<
  //            " : " << codegen->GetASTTypeString(sg_curr_param) << endl;
  if (codegen->IsVarRefExp(sg_curr_param) != 0) {
    void *sg_name = codegen->GetVariableInitializedName(sg_curr_param);
    assert(sg_name);

    if (-1 == codegen->GetFuncParamIndex(sg_name, sg_scope)) {
#ifdef DEBUG_FUNC_TRACE_UP
      cout << "[func_print]   : push " << my_itoa_hex((int64_t)sg_name) << " "
           << codegen->UnparseToString(codegen->TraceUpToStatement(sg_name))
           << endl;
#endif
      //  vec_nodes->push_back(sg_name);
      if (codegen->is_located_in_scope(sg_curr_param, sg_scope) != 0) {
        vec_nodes->push_back(sg_curr_param);  //  VarRef of the argument
      }
      return nullptr;
    }

    //  ZP: 20160303
    if (MarsProgramAnalysis::has_variable_range(codegen, sg_name, sg_curr_param,
                                                sg_scope, true)) {
      if (codegen->is_located_in_scope(sg_name, sg_scope) != 0) {
        vec_nodes->push_back(sg_name);  //  init_name with range
      }
      return nullptr;
    }

#ifdef DEBUG_FUNC_TRACE_UP
    void *func_decl =
        codegen->TraceUpByTypeCompatible(sg_name, V_SgFunctionDeclaration);
    cout << "[func_print]   : recurse " << my_itoa_hex((int64_t)sg_name) << " "
         << codegen->GetFuncName(func_decl) << " -> "
         << codegen->UnparseToString(sg_name) << endl;
#endif
    return sg_name;  //  find in the function declaration list, return for
                     //  recursion
  }
  if (codegen->IsCastExp(sg_curr_param) != 0) {
    return func_trace_up_source(codegen->GetExpUniOperand(sg_curr_param), pArg);
  }
#ifdef DEBUG_FUNC_TRACE_UP
  cout << "[func_print]   : push " << my_itoa_hex((int64_t)sg_curr_param) << " "
       << codegen->GetASTTypeString(sg_curr_param) << " "
       << codegen->UnparseToString(codegen->TraceUpToStatement(sg_curr_param))
       << endl;
#endif

  if (codegen->is_located_in_scope(sg_curr_param, sg_scope) != 0) {
    vec_nodes->push_back(sg_curr_param);  //  general expression
  }
  return nullptr;

  return nullptr;
}

//  Source type:
//  1. General Expression
//  2. VarRefExpr of local/global variables
//  3. Function argument (recursed, so not in final output)
void CSageCodeGen::TraceUpFuncArgSource(void *sg_scope, void *sg_func_decl_arg,
                                        vector<void *> *vec_source) {
  sg_scope = (sg_scope) != nullptr ? sg_scope : m_sg_project;
  func_trace_up_ARG_TYPE arg;
  arg.codegen = this;
  arg.vec_nodes = vec_source;
  arg.sg_scope = sg_scope;

  TraceUpFuncCallsGeneric(sg_scope, sg_func_decl_arg, func_trace_up_source,
                          &arg);
}

//  call function pFunc for each calls traversed for an variable
//  For every traced call, function pFunc is called
//  pFunc returns another source for recursive tracing
void CSageCodeGen::TraceUpFuncCallsGeneric(void *sg_scope,
                                           void *sg_func_decl_arg,
                                           const t_func_trace_up &pFunc,
                                           void *pFuncArg) {
  set<void *> visited;
  TraceUpFuncCallsGeneric(sg_scope, sg_func_decl_arg, pFunc, pFuncArg,
                          &visited);
}

void CSageCodeGen::TraceUpFuncCallsGeneric(void *sg_scope,
                                           void *sg_func_decl_arg,
                                           const t_func_trace_up &pFunc,
                                           void *pFuncArg,
                                           set<void *> *visited) {
  //  0. Get function information
  if (sg_func_decl_arg == nullptr) {
    return;
  }
  void *sg_func_decl =
      TraceUpByTypeCompatible(sg_func_decl_arg, V_SgFunctionDeclaration);
  if (sg_func_decl == nullptr || visited->count(sg_func_decl) > 0) {
    return;
  }
  visited->insert(sg_func_decl);
#ifdef DEBUG_FUNC_TRACE_UP
  cout << "[TraceFuncCall] " << (GetFuncName(sg_func_decl)) << " : "
       << UnparseToString(sg_func_decl_arg) << endl;
#endif

  //  0.1 Get Argument index
  int index = GetFuncParamIndex(sg_func_decl_arg, sg_scope);
  assert(index >= 0);

  //  1. GetFuncCalls, determine whether to recur according to the return value
  vector<void *> all_calls;

  //  ZP: 20160303
  //  GetFuncCallsFromDecl(sg_func_decl, sg_scope, &all_calls);
  GetFuncCallsFromDecl(sg_func_decl, nullptr, &all_calls);

  for (auto call : all_calls) {
    if (is_located_in_scope(call, sg_scope) == 0) {
      continue;
    }
#ifdef DEBUG_FUNC_TRACE_UP
    cout << "  => " << UnparseToString(call) << endl;
#endif
    void *sg_curr_param = GetFuncCallParam(call, index);
    void *new_func_arg = (*pFunc)(sg_curr_param, pFuncArg);
    if (new_func_arg != nullptr) {
      TraceUpFuncCallsGeneric(sg_scope, new_func_arg, pFunc, pFuncArg, visited);
    }
  }
  visited->erase(sg_func_decl);
}

void traverse_all_nodes(void *n, const CInputOptions &option,
                        t_func_rose_simple_traverse node_func,
                        void *node_func_arg);
void traverse_all_nodes(void *n, const CInputOptions &option,
                        vector<SgNode *> *vec);
void CSageCodeGen::TraverseSimple(void *sg_scope, const string &sOrder,
                                  t_func_rose_simple_traverse pFunc,
                                  void *pFuncArg) {
// Please keep this macro selection here for debugging
#if 0  // ZP: DEC 2019: update to full tree search
  CSageCodeGen_TraverseSimple *visitor = new CSageCodeGen_TraverseSimple();
  visitor->setFuncArg(pFunc, pFuncArg);
  t_traverseOrder order = (sOrder == "preorder") ? preorder : postorder;
  visitor->traverse(static_cast<SgNode *>(sg_scope), order);
  delete visitor;
#else

  if (sOrder == "preorder") {
#if 0
    vector<SgNode *> vec;

    CInputOptions option;
    option.set_flag("-a");
    option.add_option("-a", "order=" + sOrder);
    traverse_all_nodes(sg_scope, option, &vec);

    std::set<SgNode *> dup;
    for (auto node : vec) {
      if (dup.find(node) == dup.end()) {
        (*pFunc)(node, pFuncArg);
        dup.insert(node);
      }
    }
#else
    CInputOptions option;
    option.set_flag("-a");
    option.add_option("-a", "order=" + sOrder);
    traverse_all_nodes(sg_scope, option, pFunc, pFuncArg);
#endif

  } else {
    CSageCodeGen_TraverseSimple *visitor = new CSageCodeGen_TraverseSimple();
    visitor->setFuncArg(pFunc, pFuncArg);
    t_traverseOrder order = (sOrder == "preorder") ? preorder : postorder;
    visitor->traverse(static_cast<SgNode *>(sg_scope), order);
    delete visitor;
  }

#endif
}

void *CSageCodeGen::find_interface_by_name(const string &var, void *kernel) {
  if (IsFunctionDeclaration(kernel) != 0) {
    void *kernel_decl = kernel;
    for (int i = 0; i < GetFuncParamNum(kernel_decl); i++) {
      void *arg_init = GetFuncParam(kernel_decl, i);
      if ((arg_init != nullptr) && GetVariableName(arg_init) == var) {
        return arg_init;
      }
    }
  }
  return nullptr;
}

void *CSageCodeGen::find_variable_by_name(const string &var, void *pos,
                                          bool inclusive_scope) {
  if (is_floating_node(pos))
    return nullptr;
  void *curr_pos = pos;
  while (curr_pos != nullptr) {
    void *sg_scope = curr_pos;
    if (IsFunctionDeclaration(curr_pos) != 0) {
      void *kernel_decl = curr_pos;
      for (int i = 0; i < GetFuncParamNum(kernel_decl); i++) {
        void *arg_init = GetFuncParam(kernel_decl, i);
        if ((arg_init != nullptr) && GetVariableName(arg_init) == var) {
          return arg_init;
        }
      }
      sg_scope = GetFuncDefinitionByDecl(curr_pos);
    }

    SgScopeStatement *outer_scope =
        isSgScopeStatement(static_cast<SgNode *>(sg_scope));
    if (outer_scope != nullptr) {
      if (auto *res = getInitializedNameFromName(outer_scope, var, false)) {
        return res;
      }
    }
    curr_pos = GetParent(curr_pos);
  }

  void *curr_scope = GetEnclosingNode("BasicBlock", pos);
  if (inclusive_scope) {
    vector<void *> vec_decls;
    GetNodesByType_int(curr_scope, "preorder", V_SgVariableDeclaration,
                       &vec_decls);
    for (auto decl : vec_decls) {
      void *var_init = GetVariableInitializedName(decl);
      string str_var = _up(var_init);
      if (str_var == var) {
        return var_init;
      }
    }
  }

  return nullptr;
}

bool CSageCodeGen::check_valid_field_name(const string &field_name, void *pos) {
  vector<string> vs_expr = str_split(field_name, "->");
  int first = 1;

  void *curr_struct_type = nullptr;
  void *sg_type = nullptr;
  for (auto s_var : vs_expr) {
    void *sg_init = nullptr;
    if (first != 0) {
      sg_init = find_variable_by_name(s_var, pos);
      first = 0;
    } else {
      if (curr_struct_type != nullptr) {
        sg_init = GetClassMemberByName(s_var, curr_struct_type);
      }
    }
    if (sg_init == nullptr) {
      return false;
    }
    sg_type = GetTypebyVar(sg_init);
    void *sg_base = GetBaseType(sg_type, true);

    if ((IsStructureType(sg_base) != 0) || (IsClassType(sg_base) != 0)) {
      curr_struct_type = sg_base;
    }
  }
  return true;
}

void *CSageCodeGen::find_field_by_name(const string &field_name, void *pos) {
  vector<string> vs_expr = str_split(field_name, "->");
  int first = 1;

  void *curr_struct_type = nullptr;
  void *sg_type = nullptr;
  void *sg_init = nullptr;
  for (auto s_var : vs_expr) {
    if (first != 0) {
      sg_init = find_variable_by_name(s_var, pos);
      first = 0;
    } else {
      if (curr_struct_type != nullptr) {
        sg_init = GetClassMemberByName(s_var, curr_struct_type);
      }
    }
    if (sg_init == nullptr) {
      return nullptr;
    }
    sg_type = GetTypebyVar(sg_init);
    void *sg_base = GetBaseType(sg_type, true);

    if ((IsStructureType(sg_base) != 0) || (IsClassType(sg_base) != 0)) {
      curr_struct_type = sg_base;
    }
  }
  return sg_init;
}
//  Add by Yuxin
//  Yuxin: Nov/12/2019, no more scan inner scopes
SgVariableSymbol *
CSageCodeGen::getVariableSymbolFromName(void *scope, const string &varStr) {
  assert(IsScopeStatement(scope));
  auto *sg_scope = isSgScopeStatement(static_cast<SgNode *>(scope));
  SgVariableSymbol *sym = sg_scope->lookup_variable_symbol(varStr);
  return sym;
}

//  Add by Yuxin Nov.1.2015
//  Add by Yuxin Jan 2019, enhanced from the api above
//  Yuxin: Nov/12/2019, considering SgGlobal
//  Youxiang: Nov/28/2019  Look up variable declaration from the innermost
//  scope to global scope according to variable name
SgInitializedName *CSageCodeGen::getInitializedNameFromName(
    void *innermost_scope, const string &varStr, bool recursive_field) {
  if (is_floating_node(innermost_scope))
    return nullptr;
  void *scope = innermost_scope;
  if (IsFunctionDeclaration(scope) != 0) {
    scope = GetFuncDefinitionByDecl(scope);
  } else {
    scope = GetScope(scope);
  }
  void *global_scope = GetGlobal(scope);
  SgVariableSymbol *array_sym = nullptr;

  // Yuxin: Feb/14/2020
  // Support namespace
  if (varStr.find("::") != string::npos) {
    vector<string> v_sub = str_split(varStr, "::");
    void *name_scope = global_scope;
    for (size_t i = 0; i < v_sub.size(); i++) {
      array_sym = getVariableSymbolFromName(name_scope, v_sub[i]);
      if (array_sym != nullptr) {
        return array_sym->get_declaration();
      }
      if (CheckSymbolExits(name_scope, v_sub[i])) {
        for (size_t j = 0; j < GetChildStmtNum(name_scope); j++) {
          void *child_stmt = GetChildStmt(name_scope, j);
          auto ns_decl = isSgNamespaceDeclarationStatement(
              static_cast<SgNode *>(child_stmt));
          if (ns_decl && ns_decl->get_name() == v_sub[i]) {
            name_scope = ns_decl->get_definition();
            break;
          }
          auto cls_decl =
              isSgClassDeclaration(static_cast<SgNode *>(child_stmt));
          if (cls_decl && cls_decl->get_name() == v_sub[i]) {
            name_scope = cls_decl->get_definition();
            break;
          }
        }
      }
    }
  }

  if (scope) {
    do {
      // Yuxin: Dec/19/2019
      // Support class member
      if (recursive_field) {
        if (varStr.find("->") != string::npos) {
          void *sg_member = find_field_by_name(varStr, scope);
          if (sg_member != nullptr)
            return static_cast<SgInitializedName *>(sg_member);
        }
      }
      array_sym = getVariableSymbolFromName(scope, varStr);
      if (array_sym != nullptr) {
        return array_sym->get_declaration();
      }
      SgEnumFieldSymbol *enumerator_sym =
          static_cast<SgScopeStatement *>(scope)->lookup_enum_field_symbol(
              varStr);
      if (enumerator_sym != nullptr) {
        return enumerator_sym->get_declaration();
      }
      if (scope == global_scope)
        break;
      scope = GetScope(GetParent(scope));
    } while (true);
  }
  return nullptr;
}

bool CSageCodeGen::CheckSymbolExits(void *scope, const string &varStr) {
  assert(IsScopeStatement(scope));
  auto *sg_scope = isSgScopeStatement(static_cast<SgNode *>(scope));
  return sg_scope->symbol_exists(varStr);
}

//  Add by Yuxin July 23
string CSageCodeGen::GetSgPntrArrName(void *expr) {
  SgExpression *ref = isSgExpression(static_cast<SgNode *>(expr));
  string arrayName;
  if (ref != nullptr && ref->variantT() == V_SgPntrArrRefExp) {
    SgExpression *n = ref;

    while (true) {
      SgPntrArrRefExp *arr = isSgPntrArrRefExp(n);
      if (arr == nullptr) {
        break;
      }

      n = arr->get_lhs_operand();
    }

    DEBUG(std::cout << n->unparseToString() << std::endl);
    arrayName = n->unparseToString();
  }
  return arrayName;
}

int CSageCodeGen::IsInScope(void *sg_node, void *sg_scope) {
  return is_located_in_scope_simple(sg_node, sg_scope);
}

#if 0
//  to check if any side effect on sg_ref if move sg_from to sg_to (before)
//  This function is conservative
int CSageCodeGen::CheckMovable(void *sg_from, void *sg_to, void *sg_ref) {
  return 0;
}
#endif

int CSageCodeGen::IsScalarType(void *sg_name_or_type) {
  void *sg_type;

  if (IsInitName(sg_name_or_type) != 0) {
    assert(IsInitName(sg_name_or_type));
    sg_type = GetTypebyVar(sg_name_or_type);
  } else {
    sg_type = sg_name_or_type;
  }
  if (isSgType(static_cast<SgNode *>(sg_type)) == nullptr) {
    return 0;
  }

  while ((isSgModifierType(static_cast<SgType *>(sg_type)) != nullptr) ||
         (isSgReferenceType(static_cast<SgType *>(sg_type)) != nullptr) ||
         (isSgTypedefType(static_cast<SgType *>(sg_type)) != nullptr)) {
    if (isSgReferenceType(static_cast<SgType *>(sg_type)) != nullptr) {
      if (IsNonConstReferenceType(sg_type) != 0) {
        break;
      }
      //  strip constant reference type
      sg_type = GetBaseTypeOneLayer(sg_type);
    }
    sg_type = GetBaseTypeOneLayer(sg_type);
  }

  return static_cast<int>(
      SageInterface::isScalarType(static_cast<SgType *>(sg_type)) ||
      (IsEnumType(sg_type) != 0) || (IsXilinxAPIntType(sg_type) != 0) ||
      (IsXilinxAPFixedType(sg_type)) != 0);
}

vector<void *> CSageCodeGen::GetAllRefInScope(void *sg_name, void *scope) {
  vector<void *> vec_ref;
  get_ref_in_scope(sg_name, scope, &vec_ref);
  return vec_ref;
}

std::vector<std::pair<void *, void *>>  //  a multi-map an init_name to its
                                        //  defs, at a certain position
CSageCodeGen::GetAllDefbyPosition(void *pos) {
  if (m_defuse == nullptr) {
    init_defuse();
  }
  void *func_decl = TraceUpToFuncDecl(pos);
  if ((func_decl != nullptr) && s_def_use_cache.count(func_decl) <= 0) {
    s_def_use_cache.insert(func_decl);
    string func_name = GetFuncName(func_decl, false);
    if (func_name.find("memcpy_wide_bus_read") != 0 &&
        func_name.find("memcpy_wide_bus_write") != 0) {
      SgFunctionDefinition *sg_func_def = isSgFunctionDefinition(
          static_cast<SgNode *>(GetFuncDefinitionByDecl(func_decl)));
      m_defuse->start_traversal_of_one_function(sg_func_def);
    }
  }
  std::vector<std::pair<SgInitializedName *, SgNode *>> defs =
      //  std::map<SgInitializedName *, std::set<SgNode *> >  defs =
      m_defuse->getDefMultiMapFor(static_cast<SgNode *>(pos));

  std::vector<std::pair<void *, void *>> ret;

  for (auto p : defs) {
#if 0
    const std::set<SgNode*> &defnode_set = p.second;
    for (auto defnode : defnode_set)
      ret.push_back(std::pair<void *, void *>(p.first, defnode));
#else
    ret.push_back(std::pair<void *, void *>(p.first, p.second));
#endif
  }

  return ret;
}

std::vector<std::pair<void *, void *>>  //  a multi-map an init_name to its
                                        //  defs, at a certain position
CSageCodeGen::GetAllUsebyPosition(void *pos) {
  if (m_defuse == nullptr) {
    init_defuse();
  }
  void *func_decl = TraceUpToFuncDecl(pos);
  if ((func_decl != nullptr) && s_def_use_cache.count(func_decl) <= 0) {
    s_def_use_cache.insert(func_decl);

    string func_name = GetFuncName(func_decl, false);
    if (func_name.find("memcpy_wide_bus_read") != 0 &&
        func_name.find("memcpy_wide_bus_write") != 0) {
      SgFunctionDefinition *sg_func_def = isSgFunctionDefinition(
          static_cast<SgNode *>(GetFuncDefinitionByDecl(func_decl)));
      m_defuse->start_traversal_of_one_function(sg_func_def);
    }
  }
  std::vector<std::pair<SgInitializedName *, SgNode *>> uses =
      //  std::map<SgInitializedName *, set<SgNode *> > uses =
      m_defuse->getUseMultiMapFor(static_cast<SgNode *>(pos));

  std::vector<std::pair<void *, void *>> ret;

  for (auto p : uses) {
#if 0
    const std::set<SgNode*> &usenode_set = p.second;
    for (auto usenode : usenode_set)
      ret.push_back(std::pair<void *, void *>(p.first, usenode));
#else
    ret.push_back(std::pair<void *, void *>(p.first, p.second));
#endif
  }

  return ret;
}

std::vector<void *>  //  defs of an init_name , at a certain position
CSageCodeGen::GetVarDefbyPosition(void *init_name, void *pos,
                                  int simple_iterator) {
  DEFINE_START_END;

  vector<void *> ret;

//  ZP: 20180307: try to save the calling to def_use
//  when the variable is detected as a loop iterator
#define NEW_SCHEME 2

#if NEW_SCHEME == 1  //  20180317
  if (simple_iterator) {
    void *sg_pos = pos;
    while (1) {
      void *loop = TraceUpToForStatement(sg_pos);
      if (nullptr == loop)
        break;  //  normal process

      int is_write_body = has_write_in_scope_fast(init_name, GetLoopBody(loop));
      if (is_write_body)
        break;  //  normal process

      int is_write_init = has_write_in_scope_fast(init_name, GetLoopInit(loop));
      int is_write_test = has_write_in_scope_fast(init_name, GetLoopTest(loop));
      int is_write_incr =
          has_write_in_scope_fast(init_name, GetLoopIncrementExpr(loop));

      if (!is_write_init && !is_write_test && !is_write_incr) {
        sg_pos = GetParent(loop);
        continue;
      }

      if (is_write_test)
        return ret;  //  complex case that we do not support
                     //  (conservative process outside)

      // TODO(pengzh): we need to handle the impact if init is not exact write,
      //  where there are more outside defs
      //  if (not_exact_write (init)) return ret; //  in this case, previous def
      //  is necessary.

      //  if (is_write_init)
      ret.push_back(GetLoopInit(loop));
      if (is_write_incr)
        ret.push_back(GetLoopIncrementExpr(loop));

      return ret;
    }
  }

#elif NEW_SCHEME == 2  //  20180317
  if (simple_iterator != 0) {
    void *sg_pos = pos;
    std::vector<void *> v_defs =
        CSageCodeGen::GetVarDefbyPosition(init_name, pos, 0);

    while (true) {
      void *loop = TraceUpToForStatement(sg_pos);
      if (nullptr == loop) {
        break;  //  normal process
      }
      void *loop_init_stmt = GetLoopInit(loop);
      if (is_located_in_scope_simple(sg_pos, loop_init_stmt) != 0) {
        break;
      }
      if (IsScalarType(GetTypebyVar(init_name)) == 0) {
        break;
      }

      //  ZP: 20180419: selectively exclude the case that has write in
      //  test/incr/body
      //
      //  The idea is to exclude the obviously error case (by ROSE) that
      //  iterator has write in body, but the defuse outputs have no any def in
      //  either test/incr/body

      int is_write_test = has_write_in_scope_fast(init_name, GetLoopTest(loop));
      int is_write_incr =
          has_write_in_scope_fast(init_name, GetLoopIncrementExpr(loop));
      int is_write_body = has_write_in_scope_fast(init_name, GetLoopBody(loop));

      //  1. test assumption: there is write in the loop body/test/incr
      if ((is_write_test == 0) && (is_write_incr == 0) &&
          (is_write_body == 0)) {
        sg_pos = GetParent(loop);
        continue;
      }

      //  2. check if missing in defuse results
      if (!v_defs.empty()) {
        int has_any_in_loop = 0;
        for (auto one_def : v_defs) {
          if ((is_located_in_scope(one_def, loop) != 0) &&
              (is_located_in_scope(one_def, GetLoopInit(loop)) == 0)) {
            has_any_in_loop = 1;
            break;
          }
        }
        if (has_any_in_loop == 0) {
          return ret;
        }
      }

      sg_pos = GetParent(loop);
    }
  }

#else
  if (simple_iterator) {
    int is_cross_func = 0;
    int is_trace_back = 0;
    void *loop =
        GetLoopFromIteratorByPos(init_name, pos, is_cross_func, is_trace_back);
    if (init_name && loop) {
      if (!IsCanonicalForLoop(loop))
        return ret;  //  empty
      if (has_write_in_scope_fast(init_name, GetLoopBody(loop))) {
        //  return ret;  //  TODO: can not pass unit_test (program_analysis) if
        //  uncommeting here
      } else {
        ret.push_back(GetLoopInit(loop));
        ret.push_back(GetLoopIncrementExpr(loop));
        return ret;
      }
    }
  }
#endif

  if (m_defuse == nullptr) {
    init_defuse();
  }
  void *func_decl = TraceUpToFuncDecl(pos);
  if ((func_decl != nullptr) && s_def_use_cache.count(func_decl) <= 0) {
    s_def_use_cache.insert(func_decl);
    string func_name = GetFuncName(func_decl, false);
    if (func_name.find("memcpy_wide_bus_read") != 0 &&
        func_name.find("memcpy_wide_bus_write") != 0) {
      SgFunctionDefinition *sg_func_def = isSgFunctionDefinition(
          static_cast<SgNode *>(GetFuncDefinitionByDecl(func_decl)));
      STEMP(start);
      m_defuse->start_traversal_of_one_function(sg_func_def);
      ACCTM(GetVarDefbyPosition___start_traversal_of_one_function, start, end);
    }
  }
  //  use the first statement as postiion
  //  because basic block has no defuse
  if ((IsBasicBlock(pos) != 0) && GetChildStmtNum(pos) > 0) {
    pos = GetChildStmt(pos, 0);
  }
  vector<SgNode *> vec_defs = m_defuse->getDefFor(
      static_cast<SgNode *>(pos),
      isSgInitializedName(static_cast<SgNode *>(init_name)));
  STEMP(start);
  for (size_t i = 0; i < vec_defs.size(); i++) {
    //  YX: fix defuse a bug 20170607
    //  for pointer and array variable, defuse treats write on one element is
    //  one def of the variable itself
    void *def = vec_defs[i];
    //  FIXME: YX: 20170630 the following assertion should be always true
    //  assert(!is_floating_node(def));
    if (is_floating_node(def) != 0) {
      continue;
    }
    if ((IsInitializer(def) != 0) || (IsInitName(def) != 0)) {
      ret.push_back(def);
      continue;
    }
    vector<void *> vec_var_refs;
    get_ref_in_scope(init_name, def, &vec_var_refs);
    if (!vec_var_refs.empty()) {
      for (auto ref : vec_var_refs) {
        if (is_write_conservative(ref, false) != 0) {
          ret.push_back(def);
          break;
        }
      }
    }
  }
  TIMEP("GetVarDefbyPosition___loop", start, end);

  return ret;
}

//  FilteredCFGNode<IsDFAFilter> s_liveness_cfg;

void CSageCodeGen::set_function_for_liveness(void *func_decl) {
  if (func_decl == nullptr) {
    s_liveness_previous_func = nullptr;
    return;
  }  //  reset the cache

  if (s_liveness_previous_func == func_decl) {
    return;  //  do nothing
  }

  if (IsFunctionDeclaration(func_decl) == 0) {
    assert(0);
  }

  if (m_liveness == nullptr) {
    init_defuse();
  }

  bool abortme = false;
  s_liveness_previous_func = func_decl;
#if 1
  if (s_def_use_cache.count(func_decl) <= 0) {
    s_def_use_cache.insert(func_decl);
    string func_name = GetFuncName(func_decl, false);
    if (func_name.find("memcpy_wide_bus_read") != 0 &&
        func_name.find("memcpy_wide_bus_write") != 0) {
      SgFunctionDefinition *sg_func_def = isSgFunctionDefinition(
          static_cast<SgNode *>(GetFuncDefinitionByDecl(func_decl)));
      m_defuse->start_traversal_of_one_function(sg_func_def);
    }
  }

  m_liveness->run((isSgFunctionDeclaration(static_cast<SgNode *>(func_decl)))
                      ->get_definition(),
                  abortme);
#endif
}

void CSageCodeGen::get_liveness_in(
    void *sg_pos_expr,
    vector<void *> *ret) {  //  get the init_name of the alive variables
  //  Please make sure the function scope is the correct one
  assert(TraceUpByTypeCompatible(sg_pos_expr, V_SgFunctionDeclaration) ==
         s_liveness_previous_func);

#if 0
  void *func_decl =
      TraceUpByTypeCompatible(sg_pos_expr, V_SgFunctionDeclaration);
  bool abortme = false;
  FilteredCFGNode<IsDFAFilter> s_liveness_cfg1 = m_liveness->run(
      (isSgFunctionDeclaration(static_cast<SgNode *>(func_decl)))
      ->get_definition(), abortme);
#endif
  if (m_liveness == nullptr) {
    init_defuse();
  }
  vector<SgInitializedName *> liveIn;
  vector<SgInitializedName *> liveOut;
  vector<SgInitializedName *>::iterator it;
  liveIn = m_liveness->getIn(static_cast<SgNode *>(sg_pos_expr));

  for (it = liveIn.begin(); it != liveIn.end(); it++) {
    ret->push_back(*it);
  }
}

void CSageCodeGen::get_liveness_out(
    void *sg_pos_expr,
    vector<void *> *ret) {  //  get the init_name of the alive variables
  //  Please make sure the function scope is the correct one
  assert(TraceUpByTypeCompatible(sg_pos_expr, V_SgFunctionDeclaration) ==
         s_liveness_previous_func);
#if 0
  void *func_decl =
      TraceUpByTypeCompatible(sg_pos_expr, V_SgFunctionDeclaration);
  bool abortme = false;
  FilteredCFGNode<IsDFAFilter> s_liveness_cfg1 = m_liveness->run(
      (isSgFunctionDeclaration(
      static_cast<SgNode *>(func_decl)))->get_definition(), abortme);
#endif
  if (m_liveness == nullptr) {
    init_defuse();
  }
  vector<SgInitializedName *> liveIn;
  vector<SgInitializedName *> liveOut;
  vector<SgInitializedName *>::iterator it;
  liveOut = m_liveness->getOut(static_cast<SgNode *>(sg_pos_expr));
  for (it = liveOut.begin(); it != liveOut.end(); it++) {
    ret->push_back(*it);
  }
}

void CSageCodeGen::get_liveness_for_loop(
    void *sg_loop, vector<void *> *in,
    vector<void *> *out) {  //  get the init_name of the alive variables
  //  Please make sure the function scope is the correct one
  assert(TraceUpByTypeCompatible(sg_loop, V_SgFunctionDeclaration) ==
         s_liveness_previous_func);

#if 0
  void *func_decl =
      TraceUpByTypeCompatible(sg_pos_expr, V_SgFunctionDeclaration);
  bool abortme = false;
  FilteredCFGNode<IsDFAFilter> s_liveness_cfg1 = m_liveness->run(
      (isSgFunctionDeclaration(
       static_cast<SgNode *>(func_decl)))->get_definition(), abortme);
#endif
  set<SgInitializedName *> liveIn;
  set<SgInitializedName *> liveOut;
  set<SgInitializedName *>::iterator it;
  SgForStatement *for_loop = isSgForStatement(static_cast<SgNode *>(sg_loop));
  if (for_loop == nullptr) {
    return;
  }
  if (m_liveness == nullptr) {
    init_defuse();
  }
  SageInterface::getLiveVariables(m_liveness, for_loop, liveIn, liveOut);

  for (it = liveIn.begin(); it != liveIn.end(); it++) {
    in->push_back(*it);
  }
  for (it = liveOut.begin(); it != liveOut.end(); it++) {
    out->push_back(*it);
  }
}

int CSageCodeGen::IsStructElement(string str, void *kernel) {
  vector<string> v_str = str_split(str, ".");

  void *curr_base;
  for (size_t j = 0; j < v_str.size() - 1; j++) {
    void *var_init;
    int is_interface = 0;
    //  the outmost level must interface related, else not interface related
    if (j == 0) {
      for (int i = 0; i < GetFuncParamNum(kernel); i++) {
        void *arg_init = GetFuncParam(kernel, i);
        if (UnparseToString(arg_init) == v_str[0]) {
          var_init = arg_init;

          void *sg_type = GetTypebyVar(var_init);
          curr_base = GetBaseType(sg_type, true);
          assert(IsStructureType(curr_base));
          is_interface = 1;
        }
      }
      if (is_interface == 0) {
        return 0;
      }
    }

    void *member_init = GetClassMemberByName(v_str[j + 1], curr_base);
    void *member_type = GetTypebyVar(member_init);
    if (j == v_str.size() - 2) {
      if (IsIntegerType(member_type) != 0) {
        return 1;
      }
    } else {
      if (IsStructureType(GetBaseType(member_type, true)) != 0) {
        curr_base = GetBaseType(member_type, true);
      } else {
        return 0;
      }
    }
  }
  return 0;
}

vector<void *> CSageCodeGen::GetUniqueInterfacePragma(void *sg_init,
                                                      CMarsIrV2 *mars_ir) {
  void *kernel = TraceUpToFuncDecl(sg_init);

  vector<void *> res;
  if (!mars_ir->is_kernel(kernel)) {
    return res;
  }

  void *body = GetFuncBody(kernel);
  if (IsBasicBlock(body) == 0) {
    return res;
  }

  vector<void *> vec_pragmas;
  GetNodesByType(kernel, "preorder", "SgPragmaDeclaration", &vec_pragmas);

  for (auto &pragma : vec_pragmas) {
    string s_var = mars_ir->get_pragma_attribute(pragma, CMOST_variable);
    string s_intf = mars_ir->get_pragma_attribute(pragma, CMOST_interface);
    boost::algorithm::to_upper(s_intf);
    if (s_var == _up(sg_init) && s_intf == CMOST_interface) {
      res.push_back(pragma);
    }
  }

  return res;
}

vector<string> CSageCodeGen::GetUniqueInterfaceAttribute(void *sg_init,
                                                         string attribute,
                                                         CMarsIrV2 *mars_ir) {
  void *kernel = TraceUpToFuncDecl(sg_init);

  vector<void *> vec_pragmas;
  GetNodesByType(kernel, "preorder", "SgPragmaDeclaration", &vec_pragmas);

  void *sg_pragma = nullptr;
  for (auto &pragma : vec_pragmas) {
    string s_var = mars_ir->get_pragma_attribute(pragma, CMOST_variable);
    string s_intf = mars_ir->get_pragma_attribute(pragma, CMOST_interface);
    boost::algorithm::to_upper(s_intf);
    if (s_var == _up(sg_init) && s_intf == CMOST_interface) {
      sg_pragma = pragma;
    }
  }

  string attribute_all = mars_ir->get_pragma_attribute(sg_pragma, attribute);
  vector<string> attribute_set = str_split(attribute_all, ",");
  if (attribute == "depth") {
    replace_depth(attribute_all, sg_pragma, kernel, mars_ir);
  }
  return attribute_set;
}

//  Add by Han, if depth is constant variable, directly replace the constant
//  variable
void CSageCodeGen::replace_depth(string depth_all, void *pragma, void *kernel,
                                 CMarsIrV2 *mars_ir) {
  DEBUG(printf("\n\nenter replace depth\n"));
  vector<string> depths = str_split(depth_all, ",");
  string depth_replace;
  string depth_new;
  int flag = 0;
  for (size_t j = 0; j < depths.size(); j++) {
    if (IsNumberString(depths[j]) != 0) {
      depth_new = depths[j];
    } else {
      void *var_name = find_variable_by_name(depths[j], GetGlobal(kernel));
      if (var_name != nullptr) {
        void *var_type = GetTypebyVar(var_name);
        if (IsConstType(var_type) != 0) {
          flag = 1;
          void *init_value = GetInitializerOperand(var_name);
          depth_new = UnparseToString(init_value);
        } else {
          depth_new = depths[j];
        }
      } else {
        depth_new = depths[j];
      }
    }
    if (j == 0) {
      depth_replace = depth_new;
    } else {
      depth_replace += "," + depth_new;
    }
  }
  if (flag == 1) {
    mars_ir->set_pragma_attribute(&pragma, "depth", depth_replace);
  }
}

bool CSageCodeGen::is_floating_node(void *sg_node) const {
  void *curr = sg_node;
  while (curr != nullptr) {
    if (curr == m_sg_project) {
      return false;
    }
    void *parent = GetParent(curr);
    if (parent != nullptr) {
      int index = (static_cast<SgNode *>(parent))
                      ->get_childIndex(static_cast<SgNode *>(curr));
      if (index < 0) {
        return true;
      }
    }
    curr = parent;
  }
  return true;
}

void *CSageCodeGen::GetSingleAssignExpr_path(void *sg_var,
                                             const t_func_call_path &fn_path,
                                             void **new_pos, void *pos,
                                             int reset_defuse, void *scope) {
  assert(!is_floating_node(sg_var) && !is_floating_node(pos));
  //  FIXME: ZP 20150822
  //       for the created nodes, the def-use does not exist
  //       which will impact the analysis results
  //       As a workaround, we reset the defuse, but the efficiency
  //       is a problem
  if ((m_defuse == nullptr) || (reset_defuse != 0)) {
    init_defuse();
  }

  vector<void *> vec_defs = GetVarDefbyPosition(sg_var, pos, 1);

  //  path only associated with function crossing
  //  filter_vec_node_in_path(fn_path, vec_defs);

  SgNode *sg_expr = nullptr;

  {
    if (vec_defs.empty() || (IsInitName(pos) != 0)) {
      sg_expr = static_cast<SgNode *>(sg_var);
    } else if (vec_defs.size() >= 2) {
      return nullptr;
    } else {
      sg_expr = static_cast<SgNode *>(vec_defs[0]);
    }
  }

  if (MarsProgramAnalysis::has_variable_range(this, sg_expr, pos, scope,
                                              true)) {
    return sg_expr;
  }

  if (isSgInitializedName(sg_expr) != nullptr) {
    //  can be global, local and arguments
    SgAssignInitializer *sg_init =
        isSgAssignInitializer(isSgInitializedName(sg_expr)->get_initializer());
    if (sg_init != nullptr) {
      return sg_init->get_operand();
    }

    //  if arguments, trace up to the callers
    int arg_idx = GetFuncParamIndex(sg_expr);
    if (-1 != arg_idx) {
      void *func_decl = TraceUpToFuncDecl(sg_expr);
      if (is_located_in_path(func_decl, fn_path) == 0) {
        return sg_expr;
      }
      void *func_call = get_call_in_path(func_decl, fn_path);
      if (func_call == nullptr) {
        return sg_expr;
      }
      *new_pos = func_call;
      return GetFuncCallParam(func_call, arg_idx);
    }
  } else if (isSgAssignInitializer(sg_expr) != nullptr) {
    return isSgAssignInitializer(sg_expr)->get_operand();
  } else if (isSgAssignOp(sg_expr) != nullptr) {
    if (IsVarRefExp(GetExpLeftOperand(sg_expr)) == 0) {
      return nullptr;
    }
    return GetExpRightOperand(sg_expr);
  } else if (isSgFunctionCallExp(sg_expr) != nullptr) {
  } else if (isSgConstructorInitializer(sg_expr) != nullptr) {
  } else if (isSgCompoundAssignOp(sg_expr) != nullptr) {
  } else if (isSgAddressOfOp(sg_expr) != nullptr) {
  } else if ((isSgPlusPlusOp(sg_expr) != nullptr) ||
             (isSgMinusMinusOp(sg_expr) != nullptr)) {
  } else {
#ifdef PROJDEBUG
    cout << "[CMarsExpression::substitude] Unknown def type "
         << GetASTTypeString(sg_expr) << endl;
#endif
  }

  //  if (sg_expr == sg_var) return nullptr;
  return sg_expr;
}

bool CSageCodeGen::get_map_ref_to_exp(void *expr, void *boundary_scope,
                                      map<void *, void *> *map_ref2exp) {
  vector<void *> all_refs;
  GetNodesByType_int(expr, "preorder", V_SgVarRefExp, &all_refs);
  void *pos = expr;
  for (auto one_ref : all_refs) {
    void *sg_var = GetVariableInitializedName(one_ref);
    void *sg_loop = GetLoopFromIteratorByPos(sg_var, one_ref, 0, 0);
    if (sg_loop != nullptr) {
      if (has_write_in_scope(sg_var, GetLoopBody(sg_loop)) != 0) {
#if _DEBUG_get_induct_in_scope_detail
        cout << "cannot induct variable reference '" << _up(one_ref, 0) << "' ("
             << get_location(one_ref, true, true)
             << ") because the correspond loop iterator is written in loop body"
             << endl;
#endif
        return false;
      }
      continue;
    }
    //  skip local or global array variables which are outside the boundary
    //  scope
    if ((IsArgumentInitName(sg_var) == 0) &&
        (IsArrayType(GetTypebyVar(sg_var)) != 0) &&
        (is_located_in_scope(GetVariableDecl(sg_var), boundary_scope) == 0)) {
      continue;
    }

    DEFINE_START_END;
    STEMP(start);
    void *def = GetSingleAssignExpr(sg_var, pos, 0, boundary_scope, true,
                                    true /*skip range */);
    ACCTM(get_induct_expr_in_scope___GetSingleAssignExpr, start, end);

    if ((def != nullptr) && (IsInitName(def) == 0)) {
      //  ZP: 20170605: the SSA of "x" in "x++" is "x++", then recursively
      //  replacing it endlessly.
      vector<void *> vec_self_ref;
      get_ref_in_scope(sg_var, def, &vec_self_ref);
      if (!vec_self_ref.empty()) {
        //  if (has_side_effect_v2(def, sg_var)) {
        if (is_located_in_scope(one_ref, boundary_scope) != 0) {
#if _DEBUG_get_induct_in_scope_detail
          cout << "cannot induct variable reference '" << _up(one_ref, 0)
               << "' (" << get_location(one_ref, true, true)
               << ") because its single definition has side effect" << endl;
#endif
          return false;
        }
        continue;
      }
      if (!get_map_ref_to_exp(def, boundary_scope, map_ref2exp)) {
        return false;
      }
      map_ref2exp->insert(pair<void *, void *>(one_ref, def));
    } else {
      if (is_located_in_scope(def != nullptr ? def : one_ref, boundary_scope) !=
          0) {
#if _DEBUG_get_induct_in_scope_detail
        cout << "cannot induct variable reference '" << _up(one_ref, 0) << "' ("
             << get_location(one_ref, true, true)
             << ") because it has no single definition" << endl;
#endif
        return false;
      }
    }
  }
  return true;
}

void *CSageCodeGen::get_induct_expr_in_scope(
    void *var_ref, void *pos, void *boundary_scope,
    vector<void *> *inner_terms /* = nullptr*/) {
  if (nullptr == pos) {
    pos = var_ref;
  }
  assert(!is_floating_node(pos));

  DEFINE_START_END;
  void *ret = nullptr;

  STEMP(start);
  void *sg_name = GetVariableInitializedName(var_ref);
  void *sg_loop = GetLoopFromIteratorByPos(sg_name, pos, 0, 0);
  TIMEP("get_induct_expr_in_scope___get_loop", start, end);
  if (sg_loop != nullptr) {
    if (has_write_in_scope(sg_name, GetLoopBody(sg_loop)) != 0) {
      return nullptr;  //  ZP: 20170603
    }
    {
      return sg_loop;  // TODO(youxiang): ZP 20170607 replace into copy of
                       // original
    }
    //  var_ref
  }

  STEMP(start);
  TIMEP("get_induct_expr_in_scope___get_expr_from_coeff", start, end);

  map<void *, void *> map_ref2exp;
  bool succeed = get_map_ref_to_exp(var_ref, boundary_scope, &map_ref2exp);

  if (succeed) {
    STEMP(start);
    ret = copy_and_replace_var_ref(var_ref, map_ref2exp);
    ACCTM(get_induct_expr_in_scope___copy_and_replace_var_ref, start, end);
  }
#if _DEBUG_get_induct_in_scope_detail
  cout << "--   step: " << _up(var_ref) << "(" << _up(_pa(var_ref)) << ")->"
       << _p(ret) << endl;
#endif

  //  #if _DEBUG_get_induct_in_scope
  //  cout << " induct0: " << _up(var_ref) << "(" << _up(_pa(var_ref)) << ")->"
  //  << _p(ret) << endl;
  //  #endif

  //  if (is_identical_expr(var_ref, new_expr, pos, pos))
  //  if (!has_change)
  //  {
  //  return nullptr;
  //  }

#if _DEBUG_get_induct_in_scope
  cout << "-- induct: " << _up(var_ref) << "(" << _up(_pa(var_ref)) << ")->"
       << _p(ret) << endl;
#endif

  return ret;
}

//  input: [Notice] the original AST node, because def/use result is used in
//  analysis
void *CSageCodeGen::copy_and_replace_var_ref(
    void *sg_node, const map<void *, void *> &map_var2exp,
    map<void *, void *> *map_replaced /* = nullptr*/,
    int no_copy /* = false*/) {
  CMarsAST_IF *ast = this;
  void *old_node = sg_node;

  //  Note: we assume the reference order in the original and copied
  //  nodes are in the same order

  //  1. get the ordered list of all reference in original node
  vector<pair<void *, void *>> ordered_list;
  ordered_list.clear();
  {
#if _DEBUG_copy_and_replace_ref
    cout << " old - ";
#endif
    vector<void *> v_ref;
    v_ref.clear();
    ast->GetNodesByType_int(old_node, "preorder", V_SgVarRefExp, &v_ref);
    for (auto ref : v_ref) {
      auto it = map_var2exp.find(ref);
      if (it == map_var2exp.end()) {
        ordered_list.push_back(pair<void *, void *>(nullptr, nullptr));
      } else {
        ordered_list.push_back(pair<void *, void *>(ref, it->second));

#if _DEBUG_copy_and_replace_ref  //  requires ref_scope is on AST
        cout << ast->_up(ast->_pa(ref)) << "@"
             << ast->GetVariableInitializedName(ref) << ",";
#endif
      }
    }
#if _DEBUG_copy_and_replace_ref
    cout << endl;
#endif
  }

  //  2. Apply the replace on the copied node
  void *new_node = no_copy != 0 ? old_node : ast->CopyChild(old_node);
  {
#if _DEBUG_copy_and_replace_ref
    cout << " new - ";
#endif
    vector<void *> v_ref;
    v_ref.clear();
    ast->GetNodesByType_int(new_node, "preorder", V_SgVarRefExp, &v_ref);
    int i = 0;
    for (auto ref : v_ref) {
      if (ordered_list[i].first != nullptr) {
        void *replace = ast->copy_and_replace_var_ref(ordered_list[i].second,
                                                      map_var2exp, nullptr, 0);

        //  ZP: 20170527
        {
          if (nullptr == GetParent(ref)) {
            assert(ref == new_node);
            if (no_copy == 0) {
              DeleteNode(new_node);
            }
            new_node = replace;
#if _DEBUG_copy_and_replace_ref
            cout << ast->_up(new_node) << ",";
#endif
            break;
          }
          if (map_replaced != nullptr) {
            map_replaced->insert(pair<void *, void *>(ref, replace));
          }
          ast->ReplaceChild(ref, replace);
#if _DEBUG_copy_and_replace_ref
          cout << ast->_up(ast->_pa(replace)) << "@"
               << ast->GetVariableInitializedName(replace) << ",";
#endif
        }
      }

      i++;
    }
#if _DEBUG_copy_and_replace_ref
    cout << endl;
#endif
  }

  return new_node;
}

//  return nullptr, if not single assignment.
//  sg_var is an init_name: global, argument or local
//  This function is conservative
void *CSageCodeGen::GetSingleAssignExpr(void *sg_var, void *pos,
                                        int reset_defuse, void *scope,
                                        bool cross_function, bool skip_range) {
  pos = TraceUpToNonFloating(pos);
  if (is_floating_node(pos)) {
    return nullptr;
  }
  DEFINE_START_END;
  //  FIXME: ZP 20150822
  //       for the created nodes, the def-use does not exist
  //       which will impact the analysis results
  //       As a workaround, we reset the defuse, but the efficiency
  //       is a problem
  if ((m_defuse == nullptr) || (reset_defuse != 0)) {
    init_defuse();
  }
  if (scope == nullptr) {
    scope = m_sg_project;
  }

  STEMP(start);
  vector<void *> vec_defs = GetVarDefbyPosition(sg_var, pos, 1);
  TIMEP("GetSingleAssignExpr___GetVarDefbyPosition", start, end);

  //  revert back: ZP: 20170314
  //  if (vec_defs.size() == 0) {
  //      init_defuse();
  //      vec_defs = GetVarDefbyPosition(sg_var, pos);
  //  }
  //  // ZP: 20170314 //  A trial fix for the uninitialized variable (e.g.
  //  function argument)
  //  //  sometime it can not find the defs for the transformed (generated)
  //  code, even after init_defuse if (0 == vec_defs.size() &&
  //  MarsProgramAnalysis::has_variable_range(this, sg_var))
  //    return sg_var;   //  not sure if it will generate the recursion ??

  SgNode *sg_expr = nullptr;

  //  if (IsInitName(sg_var)) {
  //    sg_expr = (SgNode*)sg_var;
  //  }
  //  else
  {
    if (vec_defs.empty()) {
      return nullptr;
      //  sg_expr = static_cast<SgNode *>(sg_var);
    }
    if (vec_defs.size() >= 2) {
      return nullptr;
      //  sg_expr = static_cast<SgNode *>(sg_var);
    }
    sg_expr = static_cast<SgNode *>(vec_defs[0]);
  }

  if (!skip_range && MarsProgramAnalysis::has_variable_range(this, sg_expr, pos,
                                                             scope, true)) {
    return sg_expr;
  }
  return GetSingleAssignExprForSingleDef(sg_var, sg_expr, scope,
                                         cross_function);
}

void *CSageCodeGen::GetSingleAssignExprForSingleDef(void *sg_var,
                                                    void *single_def_,
                                                    void *scope,
                                                    bool cross_func) {
  SgNode *single_def = static_cast<SgNode *>(single_def_);
  if (isSgInitializedName(single_def) != nullptr) {
    //  can be global, local and arguments
    STEMP(start);
    //  if arguments, trace up to the callers
    if (-1 != GetFuncParamIndex(single_def)) {
      //  ZP: 20170605
      // TODO(youxiang): ??: do not propagate if it is pass-by-value
      {}

      if (!cross_func) {
        return single_def;
      }
      //  ZP: 2016-02-24, consider the scope of propagation
      void *func_decl = TraceUpToFuncDecl(single_def);
      if (IsFunctionDeclaration(scope) != 0) {
        scope = GetFuncBody(scope);
      }
      if (is_located_in_scope(func_decl, scope) == 0) {
        return single_def;
      }

      vector<void *> vec_source;  //  either local/global init_name or general
                                  // expr
      //  TraceUpFuncArgSource(nullptr, single_def, vec_source);
      TraceUpFuncArgSource(scope, single_def, &vec_source);  //  ZP: 20160225

      void *ret;
      //  1. if not tracable (not single assignement), return the init_name of
      //  the argument
      if (vec_source.empty() || vec_source.size() >= 2) {
        ret = single_def;
      } else {
        //  2. if tracable, return the expr directly
        ret = vec_source[0];
      }

      if (ret == sg_var) {
        return nullptr;
      }
      return ret;
    }
  } else if (isSgAssignInitializer(single_def) != nullptr) {
    return isSgAssignInitializer(single_def)->get_operand();
  } else if (isSgAssignOp(single_def) != nullptr) {
    if (IsVarRefExp(GetExpLeftOperand(single_def)) == 0) {
      return single_def;
    }
    if (GetVariableInitializedName(GetExpLeftOperand(single_def)) != sg_var) {
      return single_def;
    }
    return GetExpRightOperand(single_def);
  } else if (isSgFunctionCallExp(single_def) != nullptr) {
    void *left_expr;
    void *right_expr;
    left_expr = right_expr = nullptr;
    int op = 0;
    if (IsAPIntOp(single_def, &left_expr, &right_expr, &op) &&
        V_SgAssignOp == op) {
      left_expr = RemoveAPIntFixedCast(left_expr);
      if ((IsVarRefExp(left_expr) != 0) &&
          GetVariableInitializedName(left_expr) == sg_var) {
        return RemoveAPIntFixedCast(right_expr);
      }
    }
  } else if (isSgConstructorInitializer(single_def) != nullptr) {
    return RemoveAPIntFixedCast(single_def);
  } else if (isSgCompoundAssignOp(single_def) != nullptr) {
  } else if ((isSgPlusPlusOp(single_def) != nullptr) ||
             (isSgMinusMinusOp(single_def) != nullptr)) {
  } else {
    DEBUG(cout << "[CMarsExpression::substitude] Unknown single_def type "
               << GetASTTypeString(single_def) << endl);
    return nullptr;
  }
  return single_def;
}

//  map<void*, CMarsRangeExpr> CSageCodeGen::m_variable_range;

void CSageCodeGen::reset_range_analysis() {
  MarsProgramAnalysis::m_init_range = 1;
  m_variable_range.clear();
  m_expr_range.clear();
  init_range_analysis();
}

void CSageCodeGen::reset_expr_range_analysis() { m_expr_range.clear(); }

bool CSageCodeGen::isSimpleOp(void *op, void *var_name) {
  void *sg_left_op = GetExpLeftOperand(op);
  void *sg_right_op = GetExpRightOperand(op);
  return isSimpleOp(sg_left_op, sg_right_op, var_name);
}

bool CSageCodeGen::isSimpleOp(void *sg_left_op, void *sg_right_op,
                              void *var_name) {
  int var_count = 0;
  int const_count = 0;

  int64_t value;
  if (IsConstantInt(sg_left_op, &value, true, nullptr)) {
    ++const_count;
  }
  if (IsConstantInt(sg_right_op, &value, true, nullptr)) {
    ++const_count;
  }
  remove_cast(&sg_left_op);
  remove_cast(&sg_right_op);
  if (IsVarRefExp(sg_left_op) != 0) {
    ++var_count;
  }
  if (IsVarRefExp(sg_right_op) != 0) {
    ++var_count;
  }

  return var_count == 1 && const_count == 1;
}

void CSageCodeGen::init_range_analysis_for_loop(void *for_loop) {
  if (m_variable_range.count(for_loop) > 0) {
    return;
  }
  if (m_variable_range.find(for_loop) == m_variable_range.end()) {
    CMarsRangeExpr mr(this);
    m_variable_range.insert(std::make_pair(for_loop, mr));
  }
  void *iv;
  void *lb;
  void *ub;
  void *step;
  void *body;
  iv = lb = ub = step = body = nullptr;
  bool inc_dir;
  bool inclusive_bound;
  if (IsCanonicalForLoop(for_loop, &iv, &lb, &ub, &step, &body, &inc_dir,
                         &inclusive_bound) != 0) {
    ALGOP("RAN", endl << "loop: " << printNodeInfo(this, for_loop) << endl
                      << "iv: " << printNodeInfo(this, iv) << endl
                      << "lb: " << printNodeInfo(this, lb) << endl
                      << "ub: " << printNodeInfo(this, ub) << endl
                      << "step: " << printNodeInfo(this, step) << endl
                      << "inc_dir: " << inc_dir << endl
                      << "inc_ub: " << inclusive_bound);

    int64_t nStep = 0;
    int ret = GetLoopStepValueFromExpr(step, &nStep);
    if (!inc_dir && nStep > 0 && (iv != nullptr)) {
      ConvertBigIntegerIntoSignedInteger(&nStep, GetTypebyVar(iv));
    }
    SgForStatement *sg_for_loop =
        isSgForStatement(static_cast<SgNode *>(for_loop));
    assert(sg_for_loop && "should be a for loop");
    SgExpression *sg_step_exp = sg_for_loop->get_increment();
    bool isIncrease;
    int op = 0;
    void *left_exp;
    void *right_exp;
    left_exp = right_exp = nullptr;
    if (IsAPIntOp(sg_step_exp, &left_exp, &right_exp, &op)) {
      if (V_SgMinusMinusOp == op) {
        isIncrease = false;
      } else if (V_SgPlusPlusOp == op) {
        isIncrease = true;
      } else if (V_SgMinusAssignOp == op) {
        isIncrease = (nStep < 0);
      } else if (V_SgPlusAssignOp == op) {
        isIncrease = (nStep > 0);
      } else if (V_SgAssignOp == op) {
        if ((IsVarRefExp(left_exp) != 0) &&
            GetVariableInitializedName(left_exp) == iv) {
          int bin_op = 0;
          void *bin_op_0;
          void *bin_op_1;
          bin_op_0 = bin_op_1 = nullptr;
          void *val_exp = RemoveAPIntFixedCast(RemoveCast(right_exp));
          if (!IsAPIntOp(val_exp, &bin_op_0, &bin_op_1, &bin_op)) {
            if (IsBinaryOp(val_exp) == 0) {
              return;
            }
            bin_op = (static_cast<SgNode *>(val_exp))->variantT();
            GetExpOperand(val_exp, &bin_op_0, &bin_op_1);
          }
          bin_op_0 = RemoveAPIntFixedCast(RemoveCast(bin_op_0));
          bin_op_1 = RemoveAPIntFixedCast(RemoveCast(bin_op_1));
          if (bin_op == V_SgSubtractOp && isSimpleOp(bin_op_0, bin_op_1, iv)) {
            isIncrease = (nStep < 0);
          } else if (bin_op == V_SgAddOp &&
                     isSimpleOp(bin_op_0, bin_op_1, iv)) {
            isIncrease = (nStep > 0);
          } else {
            return;
          }
        } else {
          return;
        }
      } else {
        return;
      }

    } else {
      if (isSgMinusMinusOp(sg_step_exp) != nullptr) {
        isIncrease = false;
      } else if (isSgPlusPlusOp(sg_step_exp) != nullptr) {
        isIncrease = true;
      } else if (isSgMinusAssignOp(sg_step_exp) != nullptr) {
        isIncrease = (nStep < 0);
      } else if (isSgPlusAssignOp(sg_step_exp) != nullptr) {
        isIncrease = (nStep > 0);
      } else {
        void *var_name;
        void *value_exp;
        if ((parse_assign(sg_step_exp, &var_name, &value_exp) != 0) &&
            var_name == iv) {
          remove_cast(&value_exp);
          SgNode *sg_value_exp = static_cast<SgNode *>(value_exp);

          if ((isSgSubtractOp(sg_value_exp) != nullptr) &&
              isSimpleOp(value_exp, iv)) {
            isIncrease = (nStep < 0);
          } else if ((IsAddOp(sg_value_exp) != 0) &&
                     isSimpleOp(value_exp, iv)) {
            isIncrease = (nStep > 0);
          } else {
            return;
          }
        } else {
          return;
        }
      }
    }

    ALGOP("RAN", "isIncrease: " << isIncrease);
    if (inc_dir ^ isIncrease) {
      ALGOP("RAN", "increase and inc_dir should be the same");
      return;
    }
    //  Yuxin (20181106)
    //  when loop init is like for(k = i = 0; ...)
    if (IsAssignOp(lb) != 0) {
      lb = GetExpRightOperand(lb);
    }
    //  Get constant bounds
    CMarsExpression me_lb(lb, this, for_loop, for_loop);
    CMarsExpression me_ub(ub, this, for_loop, for_loop);

    //  liangz (20180203)
    //  bug 2052: if me_lb or me_ub is not a loop invariant, get_range function
    //  will not terminate
    if (!me_lb.IsLoopInvariant(for_loop) || !me_ub.IsLoopInvariant(for_loop)) {
      return;
    }

    if ((ret == 0) || nStep == 0) {
      return;
    }
    if (nStep < 0) {
      nStep = -nStep;
    }

    if (!inclusive_bound) {
      int delta = (isIncrease) ? -1 : 1;
      me_ub = me_ub + delta;
    }

    //  ZP: 20151118: fix the actual bound if step is not 1
    {
      if ((me_lb.IsConstant() != 0) && (me_ub.IsConstant() != 0) &&
          std::abs(nStep) > 1) {
        int64_t tt;
        int64_t tt_last;

        int64_t n_lb = me_lb.get_const();
        int64_t n_ub = me_ub.get_const();

        if ((isIncrease && n_lb > n_ub) || (!isIncrease && n_lb < n_ub)) {
          // empty range
          m_variable_range[for_loop] = CMarsRangeExpr(pos_INF, neg_INF, true);
          return;
        }

        if (isIncrease) {
          tt_last = n_lb;
          for (tt = n_lb; tt <= n_ub; tt += nStep) {
            tt_last = tt;
          }
          me_ub.SetConstant(tt_last);
        } else {
          tt_last = n_lb;
          for (tt = n_lb; tt >= n_ub; tt -= nStep) {
            tt_last = tt;
          }
          me_ub.SetConstant(tt_last);
        }
      }
    }

    if (isIncrease) {  //  indicating actual step is positive
      m_variable_range[for_loop].AppendLB(me_lb);
      m_variable_range[for_loop].AppendUB(me_ub);
    } else {
      m_variable_range[for_loop].AppendLB(me_ub);
      m_variable_range[for_loop].AppendUB(me_lb);
    }
    m_variable_range[for_loop].set_exact(static_cast<int>(nStep == 1));
  }
  if ((step != nullptr) && (GetParent(step) == nullptr)) {
    DeleteNode(step);
  }
}

//  Initialize the range list
//  1 for loops
//  2 assert
//  3 range pragmas
//  -> map<void*, CMarsRangeExpr> m_variable_range;
void CSageCodeGen::init_range_analysis() {
  //  1 find all the assert
  vector<void *> vec_assert_call;
  GetNodesByType_int(nullptr, "preorder", V_SgFunctionCallExp,
                     &vec_assert_call);
  for (size_t i = 0; i != vec_assert_call.size(); ++i) {
    void *assert_call = vec_assert_call[i];
    if ((IsFromInputFile(assert_call) == 0) && !IsTransformation(assert_call)) {
      continue;
    }
    if (IsAssertFailCall(assert_call)) {
      void *conditional_exp = GetParent(assert_call);
      void *cond_exp;
      void *true_exp;
      void *false_exp;
      if (IsConditionalExp(conditional_exp, &cond_exp, &true_exp, &false_exp)) {
        HandleCondition(cond_exp, assert_call == false_exp);
      }
    }
  }

  //  2 find all the range pragmas

  /*
  //  3 find all the for loops
  {
    vector<void *> vec_for;
    GetNodesByType_int(nullptr, "preorder", V_SgForStatement, &vec_for);
    for (size_t i = 0; i < vec_for.size(); i++) {
      void *for_loop = vec_for[i];
      if (!IsFromInputFile(for_loop) &&
          !IsTransformation(for_loop)) continue;
      init_range_analysis_for_loop(for_loop);
    }
  }*/

  if (_DEBUG_RANGE_ANALYSIS_) {
    printf("\n[Variable Range Analysis]\n");
    map<void *, CMarsRangeExpr>::iterator it;

    size_t total_cnt = m_variable_range.size();
    auto m_variable_range_t = m_variable_range;

    for (auto it : m_variable_range_t) {
      void *var = it.first;
      CMarsRangeExpr range = it.second;

      void *var_s = var;
      if ((IsForStatement(var_s) != 0) && (GetLoopIterator(var_s) != nullptr)) {
        var_s = GetLoopIterator(var_s);
      }

#ifdef PROJDEBUG
      printf("--Range %s %s -> %s \n", GetASTTypeString(var).c_str(),
             UnparseToString(var_s).c_str(), range.print().c_str());
#endif
      assert(total_cnt == m_variable_range_t.size());
    }
    printf("\n");
  }
}

string CSageCodeGen::get_file(void *sgnode) const {
  return GetFileInfo_filename(sgnode, 1);
}
int CSageCodeGen::get_line(void *sgnode) const {
  return GetFileInfo_line(sgnode);
}
int CSageCodeGen::get_col(void *sgnode) const {
  return GetFileInfo_col(sgnode);
}

//  int CSageCodeGen::IsFromInputFile(const string &file_name) {
int CSageCodeGen::IsFromInputFile(void *sgnode) const {
  if (sgnode == nullptr) {
    return 0;
  }

  string file_name = get_file(sgnode);
  if (m_inputFiles.count(file_name) > 0) {
    return 1;
  }
  if (test_file_for_read(file_name) != 0) {
    return 1;
  }
  string absolute_path = GetFileInfo_filename(sgnode, 0);
  if (absolute_path.length() > 0 && absolute_path[0] != '/' &&
      absolute_path != "compilerGenerated" &&
      absolute_path != "transformation" &&
      boost::filesystem::exists(absolute_path)) {
    boost::filesystem::path p(absolute_path);
    absolute_path = boost::filesystem::absolute(p).string();
  }
  for (auto dir : m_inputIncDirs) {
    //  skip system directory
    if (dir.find("mars-gen/lib/merlin") != string::npos ||
        dir.find("source-opt/include/apint_include") != string::npos) {
      continue;
    }
    string::size_type pos = absolute_path.find(dir);
    if (pos == 0 && absolute_path[dir.size()] == '/') {
      return 1;
    }
    //  string full_name = dir + "/" + file_name;
    //  if (test_file_for_read(full_name))
    //  return 1;
  }
  return 0;
}

int CSageCodeGen::IsFromMerlinSystemFile(void *sgnode) {
  if (sgnode == nullptr) {
    return 0;
  }
  string file_name = get_file(sgnode);
  if (m_inputFiles.count(file_name) > 0) {
    return 0;
  }
  if (test_file_for_read(file_name) != 0) {
    return 0;
  }
  string absolute_path = GetFileInfo_filename(sgnode, 0);
  string str_complex_file = "/complex";
  //  system directory
  if (absolute_path.find("mars-gen/lib/merlin") != string::npos ||
      absolute_path.find("source-opt/include/apint_include") != string::npos ||
      (absolute_path.rfind(str_complex_file) + str_complex_file.size() ==
       absolute_path.size())) {
    return 1;
  }
  return 0;
}

int CSageCodeGen::IsSystemFunction(void *sg_func) {
  if (IsFunctionDeclaration(sg_func) == 0) {
    return 0;
  }
  if (auto *tfc = GetTemplateFuncDecl(sg_func)) {
    sg_func = tfc;
  }
  if (auto *tmfc = GetTemplateMemFuncDecl(sg_func)) {
    sg_func = tmfc;
  }
  if ((IsFromInputFile(sg_func) == 0) && !IsTransformation(sg_func) &&
      !IsCompilerGenerated(sg_func) && isWithInHeaderFile(sg_func)) {
    return 1;
  }
  vector<void *> vec_decl = GetAllFuncDecl(sg_func);
  //  check a decl only in header file
  for (auto decl : vec_decl) {
    if (!isWithInHeaderFile(decl)) {
      continue;
    }
    if ((IsFromInputFile(decl) == 0) && !IsTransformation(decl) &&
        !IsCompilerGenerated(decl)) {
      return 1;
    }
  }
  return 0;
}

string CSageCodeGen::_p(void *sgnode, int len /*=0*/) const {
  if (sgnode == nullptr) {
    return "nullptr";
  }
  if (GetParent(sgnode) == nullptr) {
    //  return "P_nullptr:" + UnparseToString(sgnode, len);
    return "P_nullptr:" + GetASTTypeString(sgnode) + ":" +
           UnparseToString(sgnode, len);
  }
  if (is_floating_node(sgnode) != 0) {
    return "Floating Node:" + UnparseToString(sgnode, len);
  }
  return GetASTTypeString(sgnode) + ":" + UnparseToString(sgnode, len);
}
void *CSageCodeGen::_pa(void *sgnode) const { return GetParent(sgnode); }

bool CSageCodeGen::GetTopoOrderFunctions(void *top_func,
                                         vector<void *> *functions) {
  SetVector<void *> sub_funcs;
  map<void *, SetVector<void *>> callee2callers;
  GetSubFuncDeclsRecursively(top_func, &sub_funcs, &callee2callers, true);
  functions->push_back(top_func);
  set<void *> processed;
  processed.insert(top_func);
  bool found;
  do {
    found = false;
    for (auto sub_func : sub_funcs) {
      if (processed.count(sub_func) > 0) {
        continue;
      }
      bool all_processed = true;
      if (callee2callers.count(sub_func) > 0) {
        for (auto caller : callee2callers[sub_func]) {
          if (processed.count(caller) <= 0) {
            all_processed = false;
            break;
          }
        }
      }
      if (all_processed) {
        functions->push_back(sub_func);
        processed.insert(sub_func);
        found = true;
      }
    }
  } while (found);
  return processed.size() == sub_funcs.size() + 1;
}

void *CSageCodeGen::GetFuncDefinitionByDecl(void *sg_func_decl) {
  SgFunctionDeclaration *func_decl =
      isSgFunctionDeclaration(static_cast<SgNode *>(sg_func_decl));
  if (func_decl == nullptr) {
    return nullptr;
  }
#if 0
  // for same static functions in multiple source files, ROSE library will
  // report incorrect defining function
  if (auto defining_decl =
          isSgFunctionDeclaration(func_decl->get_definingDeclaration())) {
    return defining_decl->get_definition();
  }
#endif
  if (auto defining_decl = GetDefiningFuncDecl(sg_func_decl, nullptr, true)) {
    return isSgFunctionDeclaration(static_cast<SgNode *>(defining_decl))
        ->get_definition();
  }
  return func_decl->get_definition();
}

//  needreview
bool CSageCodeGen::isCppLinkage(void *sgnode) {
  SgDeclarationStatement *decl =
      isSgDeclarationStatement(static_cast<SgNode *>(sgnode));
  if (decl == nullptr) {
    return false;
  }
  void *sg_global = GetGlobal(sgnode);
  if (!isWithInCppFile(sg_global) && !isWithInHeaderFile(sg_global)) {
    return false;
  }
  string linkage = decl->get_linkage();
  if (linkage == "C++") {
    return true;
  }
  if (!linkage.empty()) {
    return false;
  }
  vector<void *> vec_decl;
#if _USE_CACHE_IN_TRAVERSE_
  auto key = std::make_pair(sg_global, V_SgFunctionDeclaration);
  if (s_g_funcs_cache.find(key) == s_g_funcs_cache.end()) {
    GetNodesByType_int(sg_global, "preorder", V_SgFunctionDeclaration,
                       &vec_decl);
    s_g_funcs_cache[key] = vec_decl;
  } else {
    vec_decl = s_g_funcs_cache[key];
  }
#else
  GetNodesByType_int(sg_global, "preorder", V_SgFunctionDeclaration, &vec_decl);
#endif
  for (auto one_decl : vec_decl) {
    if (decl == one_decl || GetFuncName(one_decl) != GetFuncName(decl)) {
      continue;
    }
    string linkage = isSgDeclarationStatement(static_cast<SgNode *>(one_decl))
                         ->get_linkage();
    if (linkage == "C++") {
      return true;
    }
    if (!linkage.empty()) {
      return false;
    }
  }
  //  FIXME
  return true;
}

bool CSageCodeGen::isWithInCppFile(void *sgnode) {
  if (sgnode == GetProject()) {
    for (auto file_name : m_inputFiles) {
      if (!is_cpp_file(file_name)) {
        return false;
      }
    }
    return true;
  }
  void *sg_global = GetGlobal(sgnode);
  if (IsTransformation(sg_global) || IsCompilerGenerated(sg_global)) {
    return false;
  }
  string file_name = GetFileInfo_filename(sg_global, 1);
  return is_cpp_file(file_name);
}

bool CSageCodeGen::isWithInHeaderFile(void *sgnode) {
  if (IsTransformation(sgnode) || IsCompilerGenerated(sgnode) ||
      IsNULLFile(sgnode)) {
    return false;
  }
  string file_name = GetFileInfo_filename(sgnode, 1);
  string::size_type pos = file_name.find_last_of('.');
  if (pos == string::npos) {
    return true;
  }
  string suffix = file_name.substr(pos + 1);
  for (size_t i = 0; i != sizeof(header_suffix) / sizeof(string); ++i) {
    if (header_suffix[i] == suffix) {
      return true;
    }
  }
  return false;
}

bool CSageCodeGen::IsAssertFailCall(void *assert_call) {
  string func_name = GetFuncNameByCall(assert_call);
  return "__assert_fail" == func_name;
}

bool CSageCodeGen::IsMemCpy(void *sg_call) {
  string func_name = GetFuncNameByCall(sg_call);
  return "memcpy" == func_name;
}

bool CSageCodeGen::IsConditionalExp(void *exp, void **cond_exp, void **true_exp,
                                    void **false_exp) {
  if (isSgConditionalExp(static_cast<SgNode *>(exp)) != nullptr) {
    SgConditionalExp *conditional =
        isSgConditionalExp(static_cast<SgNode *>(exp));
    *cond_exp = conditional->get_conditional_exp();
    *true_exp = conditional->get_true_exp();
    *false_exp = conditional->get_false_exp();
    return true;
  }
  return false;
}

void CSageCodeGen::HandleCondition(void *cond_exp, bool true_value) {
  set<void *> vars;
  map<void *, int64_t> var2lb;
  map<void *, int64_t> var2ub;
  set<void *> exprs;
  map<void *, int64_t> expr2lb;
  map<void *, int64_t> expr2ub;
  vector<void *> vec_test_temp;
  if ((isSgAndOp(static_cast<SgNode *>(cond_exp)) != nullptr) && true_value) {
    GetExprListFromAndOp(cond_exp, &vec_test_temp);
  } else if (IsCompareOp(cond_exp) != 0) {
    vec_test_temp.push_back(cond_exp);
  } else if ((isSgOrOp(static_cast<SgNode *>(cond_exp)) != nullptr) &&
             !true_value) {
    GetExprListFromOrOp(cond_exp, &vec_test_temp);
  } else {
    return;
  }
  for (size_t i = 0; i < vec_test_temp.size(); i++) {
    void *cmp_op = vec_test_temp[i];
    if (IsCompareOp(cmp_op) != 0) {
      map<void *, int64_t> lbs;
      map<void *, int64_t> ubs;
      map<void *, int64_t>::iterator itt;
      int ret = GetVarDefRangeFromCompareOp(cmp_op, &lbs, &ubs, true_value);
      if (ret != 0) {
        for (itt = lbs.begin(); itt != lbs.end(); itt++) {
          void *sg_var = itt->first;
          assert(var2lb.count(sg_var) <= 0);
          var2lb[sg_var] = itt->second;
          vars.insert(sg_var);
        }
        for (itt = ubs.begin(); itt != ubs.end(); itt++) {
          void *sg_var = itt->first;
          assert(var2ub.count(sg_var) <= 0);
          var2ub[sg_var] = itt->second;
          vars.insert(sg_var);
        }
      } else {
        lbs.clear();
        ubs.clear();
        GetExprRangeFromCompareOp(cmp_op, &lbs, &ubs, true_value);
        for (itt = lbs.begin(); itt != lbs.end(); itt++) {
          void *sg_expr = itt->first;
          assert(expr2lb.count(sg_expr) <= 0);
          expr2lb[sg_expr] = itt->second;
          exprs.insert(sg_expr);
        }
        for (itt = ubs.begin(); itt != ubs.end(); itt++) {
          void *sg_expr = itt->first;
          assert(expr2ub.count(sg_expr) <= 0);
          expr2ub[sg_expr] = itt->second;
          exprs.insert(sg_expr);
        }
      }
    }
  }
  //  void *assert_stmt = TraceUpToStatement(cond_exp);
  for (set<void *>::iterator var_it = vars.begin(); var_it != vars.end();
       ++var_it) {
    void *var = *var_it;
    CMarsExpression me(var, this, cond_exp, TraceUpToFuncDecl(cond_exp));
    if (me.IsConstant() != 0) {
      continue;
    }
    if (m_variable_range.count(var) <= 0) {
      void *type = nullptr;
      if (IsInitName(var) != 0) {
        type = GetTypebyVar(var);
      } else if (IsExpression(var) != 0) {
        type = GetTypeByExp(var);
      }
      m_variable_range[var] = CMarsRangeExpr(this, type);
    }
    if (var2lb.count(var) > 0) {
      CMarsRangeExpr range = m_variable_range[var];
      m_variable_range[var] = range.Intersect(
          CMarsRangeExpr(CMarsExpression(this, var2lb[var]), pos_INF));
    }
    if (var2ub.count(var) > 0) {
      CMarsRangeExpr range = m_variable_range[var];
      m_variable_range[var] = range.Intersect(
          CMarsRangeExpr(neg_INF, CMarsExpression(this, var2ub[var])));
    }
    m_variable_range[var].set_exact(0);
  }

  for (auto expr : exprs) {
    CMarsExpression me(expr, this, cond_exp);
    if (me.IsInteger() == 0) {
      continue;
    }
    bool found = false;
    for (auto &[curr_expr, range] : m_expr_range) {
      if ((curr_expr == me) != 0) {
        found = true;
        me = curr_expr;
        break;
      }
    }
    if (!found) {
      void *type = GetTypeByExp(expr);
      m_expr_range[me] = CMarsRangeExpr(this, type);
    }
    if (expr2lb.count(expr) > 0) {
      CMarsRangeExpr range = m_expr_range[me];
      m_expr_range[me] = range.Intersect(
          CMarsRangeExpr(CMarsExpression(this, expr2lb[expr]), pos_INF));
    }
    if (expr2ub.count(expr) > 0) {
      CMarsRangeExpr range = m_expr_range[me];
      m_expr_range[me] = range.Intersect(
          CMarsRangeExpr(neg_INF, CMarsExpression(this, expr2ub[expr])));
    }
    m_expr_range[me].set_exact(0);
  }
}

bool CSageCodeGen::IsConstantInt(void *sg_exp, int64_t *value,
                                 bool simplify_cast, void *scope) {
  MarsProgramAnalysis::DUMMY dummy(
      this, sg_exp, reinterpret_cast<void *>(simplify_cast), scope);
  if (cache_consts.find(dummy) != cache_consts.end()) {
    *value = std::get<1>(cache_consts[dummy]);
    return std::get<0>(cache_consts[dummy]);
  }

  if (IsCastExp(sg_exp) != 0) {
    void *type = GetTypeByExp(sg_exp);
    void *sg_sub_exp = GetExpUniOperand(sg_exp);
    if (IsConstantInt(sg_sub_exp, value, simplify_cast, scope)) {
      if (GetCastValue(value, type)) {
        if (is_floating_node(sg_exp) == 0) {
          cache_consts[dummy] = tuple<bool, int64_t>(true, *value);
        }
      }
      return true;
    }
    if (simplify_cast && (is_floating_node(sg_exp) == 0)) {
      DEFINE_START_END;
      STEMP(start);
      CMarsExpression me_op(sg_sub_exp, this, sg_exp, scope);
      ACCTM(CSageCodeGen___IsConstantInt, start, end);
      if (me_op.IsConstant() != 0) {
        *value = me_op.get_const();
        if (GetCastValue(value, type)) {
          if (is_floating_node(sg_exp) == 0) {
            cache_consts[dummy] = tuple<bool, int64_t>(true, *value);
          }
          return true;
        }
      }
    }
  }
  SgMinusOp *sg_minus_op = isSgMinusOp(static_cast<SgNode *>(sg_exp));
  if (sg_minus_op != nullptr) {
    void *constVal = GetExpUniOperand(sg_exp);
    if (IsConstantInt(constVal, value, simplify_cast, scope)) {
      *value = -*value;
      if (is_floating_node(sg_exp) == 0) {
        cache_consts[dummy] = tuple<bool, int64_t>(true, *value);
      }
      return true;
    }
  }
  SgValueExp *sg_val_exp = isSgValueExp(static_cast<SgNode *>(sg_exp));
  if (sg_val_exp != nullptr) {
    if (isSgIntVal(sg_val_exp) != nullptr) {
      *value = isSgIntVal(sg_val_exp)->get_value();
    } else if (isSgLongIntVal(sg_val_exp) != nullptr) {
      *value = (isSgLongIntVal(sg_val_exp)->get_value());
    } else if (isSgLongLongIntVal(sg_val_exp) != nullptr) {
      *value = (isSgLongLongIntVal(sg_val_exp)->get_value());
    } else if (isSgShortVal(sg_val_exp) != nullptr) {
      *value = (isSgShortVal(sg_val_exp)->get_value());
    } else if (isSgCharVal(sg_val_exp) != nullptr) {
      *value = (isSgCharVal(sg_val_exp)->get_value());
    } else if (isSgBoolValExp(sg_val_exp) != nullptr) {
      *value = (isSgBoolValExp(sg_val_exp)->get_value());
    } else if (isSgUnsignedCharVal(sg_val_exp) != nullptr) {
      *value = (isSgUnsignedCharVal(sg_val_exp)->get_value());
    } else if (isSgUnsignedShortVal(sg_val_exp) != nullptr) {
      *value = (isSgUnsignedShortVal(sg_val_exp)->get_value());
    } else if (isSgUnsignedIntVal(sg_val_exp) != nullptr) {
      *value = (isSgUnsignedIntVal(sg_val_exp)->get_value());
    } else if (isSgUnsignedLongLongIntVal(sg_val_exp) != nullptr) {
      //  FIXME:
      *value = (isSgUnsignedLongLongIntVal(sg_val_exp)->get_value());
    } else if (isSgUnsignedLongVal(sg_val_exp) != nullptr) {
      *value = (isSgUnsignedLongVal(sg_val_exp)->get_value());
    } else if (SgEnumVal *en = isSgEnumVal(sg_val_exp)) {
      *value = en->get_value();
    } else {
      if (is_floating_node(sg_exp) == 0) {
        cache_consts[dummy] = tuple<bool, int64_t>(false, *value);
      }
      return false;
    }
    if (is_floating_node(sg_exp) == 0) {
      cache_consts[dummy] = tuple<bool, int64_t>(true, *value);
    }
    return true;
  }
  SgExpression *sgExp = isSgExpression(static_cast<SgNode *>(sg_exp));
  if (sgExp != nullptr) {
    if (isSgAssignInitializer(sgExp) != nullptr) {
      return IsConstantInt(isSgAssignInitializer(sgExp)->get_operand(), value,
                           simplify_cast, scope);
    }
    if (isSgBinaryOp(sgExp)) {
      int64_t c_l, c_r;
      void *lhs, *rhs;
      lhs = rhs = nullptr;
      GetExpOperand(sgExp, &lhs, &rhs);
      if (IsConstantInt(lhs, &c_l, simplify_cast, scope) &&
          IsConstantInt(rhs, &c_r, simplify_cast, scope)) {
        CMarsExpression me(sgExp, this, sgExp, scope);
        if (me.IsConstant()) {
          *value = me.GetConstant();
          return true;
        }
      }
    }
    //  ZP: 2014-08-21
    if (is_floating_node(sg_exp) == 0) {
      cache_consts[dummy] = tuple<bool, int64_t>(false, *value);
    }
    return false;
    //  assert(false);
  }
  if (is_floating_node(sg_exp) == 0) {
    cache_consts[dummy] = tuple<bool, int64_t>(false, *value);
  }
  return false;
}

//  Get the bounds of scalar variable from expression
//  Conservative
//  map: var -> expr (bound)
int CSageCodeGen::GetVarDefRangeFromCompareOp(void *expr,
                                              map<void *, int64_t> *lbs,
                                              map<void *, int64_t> *ubs,
                                              bool true_value) {
  //  void *assert_stmt = TraceUpToStatement(expr);
  void *scope = TraceUpToScope(expr);
  if (IsBinaryOp(expr) == 0) {
    return 0;
  }
  void *sg_left = GetExpLeftOperand(expr);
  remove_cast(&sg_left);
  void *sg_right = GetExpRightOperand(expr);
  remove_cast(&sg_right);
  CMarsExpression expr1(sg_left, this, expr);
  CMarsExpression expr2(sg_right, this, expr);
  assert(sg_left && sg_right);
  int64_t value;
  if ((IsVarRefExp(sg_left) != 0) && (expr2.IsConstant() != 0)) {
    value = expr2.GetConstant();
  } else if ((IsVarRefExp(sg_right) != 0) && (expr1.IsConstant() != 0)) {
    value = expr1.GetConstant();
  } else {
    return 0;
  }
  //  convert sg_left >= sg_right + offset
  int is_equal = 0;
  int offset = 0;
  {
    if (IsLessOrEqualOp(expr) != 0) {
      if (true_value) {
        std::swap(sg_left, sg_right);
      } else {
        offset = 1;
      }
    } else if (IsLessThanOp(expr) != 0) {
      if (true_value) {
        std::swap(sg_left, sg_right);
        offset = 1;
      }
    } else if (IsGreaterThanOp(expr) != 0) {
      if (true_value) {
        offset = 1;
      } else {
        std::swap(sg_left, sg_right);
      }
    } else if (IsGreaterOrEqualOp(expr) != 0) {
      if (!true_value) {
        std::swap(sg_left, sg_right);
        offset = 1;
      }
    } else if (((IsEqualOp(expr) != 0) && true_value) ||
               ((IsNonEqualOp(expr) != 0) && !true_value)) {
      is_equal = 1;
    } else {
      return 0;
    }
    if (IsVarRefExp(sg_left) != 0) {
      void *sg_var = GetVariableInitializedName(sg_left);
      if (sg_var == nullptr) {
        return 0;
      }
      //  vector<void *> vec_defs = GetVarDefbyPosition(sg_var, expr);
      vector<void *> vec_defs =
          MarsProgramAnalysis::getSuitableDef(this, sg_var, expr);
      if (vec_defs.empty()) {
        vec_defs.push_back(sg_left);
      }
      if (vec_defs.size() == 1) {
        (*lbs)[vec_defs[0]] = value + offset;
        if (is_equal != 0) {
          (*ubs)[vec_defs[0]] = value + offset;
        }
      } else {
        vector<void *> vec_refs;
        get_ref_in_scope(sg_var, scope, &vec_refs);
        for (auto ref : vec_refs) {
          if (is_movable_test(expr, ref) != 0) {
            (*lbs)[ref] = value + offset;
            if (is_equal != 0) {
              (*ubs)[ref] = value + offset;
            }
          }
        }
      }
    } else {
      assert(IsVarRefExp(sg_right));
      void *sg_var = GetVariableInitializedName(sg_right);
      if (sg_var == nullptr) {
        return 0;
      }
      //  vector<void *> vec_defs = GetVarDefbyPosition(sg_var, expr);
      vector<void *> vec_defs =
          MarsProgramAnalysis::getSuitableDef(this, sg_var, expr);
      if (vec_defs.empty()) {
        vec_defs.push_back(sg_right);
      }
      if (vec_defs.size() == 1) {
        (*ubs)[vec_defs[0]] = value - offset;
        if (is_equal != 0) {
          (*lbs)[vec_defs[0]] = value - offset;
        }
      } else {
        vector<void *> vec_refs;
        get_ref_in_scope(sg_var, scope, &vec_refs);
        for (auto ref : vec_refs) {
          if (is_movable_test(expr, ref) != 0) {
            (*ubs)[ref] = value - offset;
            if (is_equal != 0) {
              (*lbs)[ref] = value - offset;
            }
          }
        }
      }
    }
  }

  return 1;
}

//  Get the bounds of expr from expression
//  Conservative
//  map: expr -> expr (bound)
int CSageCodeGen::GetExprRangeFromCompareOp(void *expr,
                                            map<void *, int64_t> *lbs,
                                            map<void *, int64_t> *ubs,
                                            bool true_value) {
  //  void *assert_stmt = TraceUpToStatement(expr);
  if (IsBinaryOp(expr) == 0) {
    return 0;
  }
  void *sg_left = GetExpLeftOperand(expr);
  remove_cast(&sg_left);
  void *sg_right = GetExpRightOperand(expr);
  remove_cast(&sg_right);
  CMarsExpression me_left(sg_left, this, expr);
  CMarsExpression me_right(sg_right, this, expr);
  assert(sg_left && sg_right);
  CMarsExpression diff = me_left - me_right;
  //  default diff >= offset
  int is_equal = 0;
  int offset = 0;
  bool lower_bound = true;
  {
    if (IsLessOrEqualOp(expr) != 0) {
      if (true_value) {
        lower_bound = false;
      } else {
        offset = 1;
      }
    } else if (IsLessThanOp(expr) != 0) {
      if (true_value) {
        lower_bound = false;
        offset = 1;
      }
    } else if (IsGreaterThanOp(expr) != 0) {
      if (true_value) {
        offset = 1;
      } else {
        lower_bound = false;
      }
    } else if (IsGreaterOrEqualOp(expr) != 0) {
      if (!true_value) {
        lower_bound = false;
        offset = 1;
      }
    } else if (((IsEqualOp(expr) != 0) && true_value) ||
               ((IsNonEqualOp(expr) != 0) && !true_value)) {
      is_equal = 1;
    } else {
      return 0;
    }
  }
  //  expr (>= or <=) 0
  //  =>
  //  expr + value (>= or <=) value
  int64_t value = -diff.GetConstant();
  CMarsExpression diff_minus_const = diff + value;
  void *copy_expr = diff_minus_const.get_expr_from_coeff();
  if (lower_bound) {
    (*lbs)[copy_expr] = value + offset;
    if (is_equal != 0) {
      (*ubs)[copy_expr] = value + offset;
    }
  } else {
    (*ubs)[copy_expr] = value - offset;
    if (is_equal != 0) {
      (*lbs)[copy_expr] = value - offset;
    }
  }

  return 1;
}

//  compare two expression to see whether they are identical
//  in symbolic sementics via DefUseAnalysis
bool CSageCodeGen::is_identical_expr(void *sg_node1, void *sg_node2,
                                     void *sg_pos1, void *sg_pos2) {
  if (sg_node1 == sg_node2) {
    return true;
  }
  /*
   if (IsInitName(sg_node1) && IsInitName(sg_node2)) {
         CMarsRangeExpr mr1 = CMarsVariable(sg_node1, this,
   sg_pos1).get_range(); CMarsRangeExpr mr2 = CMarsVariable(sg_node2, this,
   sg_pos2).get_range(); if(mr1.is_cover(mr1,mr2) && mr1.is_cover(mr2,mr1) ) {
             return true;
         }
    }
 */
  if ((IsExpression(sg_node1) == 0) || (IsExpression(sg_node2) == 0)) {
    return false;
  }
  void *left_op1;
  void *left_op2;
  void *right_op1;
  void *right_op2;
  left_op1 = left_op2 = right_op1 = right_op2 = nullptr;
  int compare_op1 = 0;
  int compare_op2 = 0;
  if (IsAPIntOp(sg_node1, &left_op1, &right_op1, &compare_op1) &&
      IsAPIntOp(sg_node2, &left_op2, &right_op2, &compare_op2)) {
    if (compare_op1 != compare_op2) {
      return false;
    }
    if (!is_identical_expr(left_op1, left_op2, sg_pos1, sg_pos2) ||
        !is_identical_expr(right_op1, right_op2, sg_pos1, sg_pos2)) {
      return false;
    }
    return true;
  }

  SgExpression *sg_expr1 = isSgExpression(static_cast<SgNode *>(sg_node1));
  SgExpression *sg_expr2 = isSgExpression(static_cast<SgNode *>(sg_node2));
  //  1. check whether they are the same type of expression
  if (sg_expr1->class_name() != sg_expr2->class_name()) {
    return false;
  }
  //  2. check whether they are constant int
  if (isSgIntVal(sg_expr1) != nullptr) {
    int64_t val1;
    int64_t val2;
    return IsConstantInt(sg_expr1, &val1, true, nullptr) &&
           IsConstantInt(sg_expr2, &val2, true, nullptr) && val1 == val2;
  }
  if (IsBinaryOp(sg_expr1) != 0) {
    return !(
        !is_identical_expr(GetExpLeftOperand(sg_expr1),
                           GetExpLeftOperand(sg_expr2), sg_pos1, sg_pos2) ||
        !is_identical_expr(GetExpRightOperand(sg_expr1),
                           GetExpRightOperand(sg_expr2), sg_pos1, sg_pos2));
  }
  if (IsUnaryOp(sg_expr1) != 0) {
    return is_identical_expr(GetExpUniOperand(sg_expr1),
                             GetExpUniOperand(sg_expr2), sg_pos1, sg_pos2);
  }
  if (IsInitializer(sg_expr1) != 0 &&
      GetInitializerOperand(sg_expr1) != sg_expr1) {
    return is_identical_expr(GetInitializerOperand(sg_expr1),
                             GetInitializerOperand(sg_expr2), sg_pos1, sg_pos2);
  }
#if 0
  if (IsPntrArrRefExp(sg_expr1) || IsPointerDerefExp(sg_expr1)) {
    void *sg_array1 = nullptr;
    vector<void *> index1;
    parse_pntr_ref_new(sg_expr1, &sg_array1, &index1);
    void *sg_array2 = nullptr;
    vector<void *> index2;
    parse_pntr_ref_new(sg_expr2, &sg_array2, &index2);
    if (index1.size() != index2.size())
      return false;
    if (!sg_array1 || !sg_array2)
      return false;
    if (!is_identical_expr(sg_array1, sg_array2, sg_pos1, sg_pos2))
      return false;
    for (size_t i = 0; i != index1.size(); ++i)
      if (!is_identical_expr(index1[i], index2[i], sg_pos1, sg_pos2))
        return false;
    return true;
  }
#endif

  assert((!is_floating_node(sg_expr1) || !is_floating_node(sg_pos1)) &&
         "sg_pos1  should not be floating node");
  assert((!is_floating_node(sg_expr2) || !is_floating_node(sg_pos2)) &&
         "sg_pos2 should not be floating node");

  if (IsVarRefExp(sg_expr1) != 0) {
    void *sg_var1 = GetVariableInitializedName(sg_expr1);
    if (sg_var1 == nullptr) {
      return false;
    }
    void *sg_var2 = GetVariableInitializedName(sg_expr2);
    if (sg_var2 == nullptr) {
      return false;
    }
    if (sg_var1 != sg_var2) {
      return false;
    }
    void *pos1 = sg_expr1;
    if (is_floating_node(sg_expr1) != 0) {
      pos1 = sg_pos1;
    }
    void *pos2 = sg_expr2;
    if (is_floating_node(sg_expr2) != 0) {
      pos2 = sg_pos2;
    }

    vector<void *> vec_def1 = GetVarDefbyPosition(sg_var1, pos1, 1);
    vector<void *> vec_def2 = GetVarDefbyPosition(sg_var2, pos2, 1);
    if (vec_def1.size() != vec_def2.size()) {
      return false;
    }
#if 1
    if (vec_def1.empty()) {
      return false;
    }
    sort(vec_def1.begin(), vec_def1.end());
    sort(vec_def2.begin(), vec_def2.end());
    for (size_t i = 0; i != vec_def1.size(); ++i) {
      if (vec_def1[i] != vec_def2[i]) {
        return false;
      }
    }
#else
    if (vec_def1.size() != 1 || vec_def1[0] != vec_def2[0])
      return false;
#endif
    return true;
  }
  //  FIXME: can we handle other cases?
  return false;
}

//  Add by Yuxin 06-Nov-15
bool CSageCodeGen::IsIncludeVar(void *expr, void *var) {
  if (IsVarRefExp(expr) == 0) {
    return false;
  }
  if (IsInitName(var) == 0) {
    return false;
  }

  Rose_STL_Container<SgNode *> varList =
      NodeQuery::querySubTree(static_cast<SgNode *>(expr), V_SgVarRefExp);
  Rose_STL_Container<SgNode *>::iterator it;

  for (it = varList.begin(); it != varList.end(); it++) {
    SgExpression *ref = isSgExpression(*it);
    SgInitializedName *arr_var;

    void *var_ref;
    if (get_var_from_pntr_ref(ref, &var_ref) == 0) {
      arr_var = isSgInitializedName(
          static_cast<SgNode *>(GetVariableInitializedName(ref)));
    } else {
      arr_var = isSgInitializedName(
          static_cast<SgNode *>(GetVariableInitializedName(var_ref)));
    }
    if (arr_var == isSgInitializedName(static_cast<SgNode *>(var))) {
      return true;
    }
  }
  return false;
}

bool CSageCodeGen::IsLoopInvariant(void *for_loop, void *expr, void *pos) {
  if (IsInScope(pos, for_loop) == 0) {
    return true;
  }
  if ((is_floating_node(expr) == 0) && (IsInScope(expr, for_loop) == 0)) {
    return true;
  }
  vector<void *> vec_refs;
  GetNodesByType_int(expr, "preorder", V_SgVarRefExp, &vec_refs);
  for (auto ref : vec_refs) {
    void *var = GetVariableInitializedName(ref);
    void *type = GetTypebyVar(var);
#if 0
    void *loop = GetLoopFromIteratorByPos(var, pos);
    if (loop) {
      if (loop == for_loop || IsInScope(loop, for_loop)) {
        return false;
      }
      continue;
    }
#endif
    vector<void *> def_vec = GetVarDefbyPosition(var, pos);
    if (def_vec.empty()) {
      return false;
    }
#if 1
    //  do not propagate further.
    //  It is recommended to call CMarsExpression IsLoopInVariant()
    //  directly
    for (size_t i = 0; i != def_vec.size(); ++i) {
      void *def = def_vec[i];
      if (IsInScope(def, for_loop) != 0) {
        return false;
      }
    }
#else
    bool has_def_in_loop = false;
    bool has_def_out_loop = false;
    //  1. all defs are outside the loop, return true
    //  2. only one def is in the loop and the init is invariant
    //   return true
    //  3. for the rest cases, return false;
    for (size_t i = 0; i != def_vec.size(); ++i) {
      void *def = def_vec[i];
      if (IsInScope(def, for_loop)) {
        if (has_def_in_loop || has_def_out_loop) {
          return false;
        }
        has_def_in_loop = true;
        void *init = nullptr;
        SgNode *sg_expr = static_cast<SgNode *>(def);
        if (isSgInitializedName(sg_expr)) {
#if 0
          //  can be global, local and arguments
          SgAssignInitializer *sg_init = isSgAssignInitializer(
              isSgInitializedName(sg_expr)->get_initializer());
          if (sg_init) {
            init = sg_init->get_operand();
          }
#endif
        } else if (isSgAssignInitializer(sg_expr)) {
#if 0
          init = isSgAssignInitializer(sg_expr)->get_operand();
#endif
        } else if (isSgAssignOp(sg_expr)) {
          init = GetExpRightOperand(sg_expr);
        }
        if (!init) {
          return false;
        }
        CMarsExpression cexp(init, this, pos);
        if (!cexp.IsLoopInvariant(for_loop)) {
          return false;
        }
      } else {
        has_def_out_loop = true;
      }
    }
    assert((has_def_in_loop ^ has_def_out_loop) || def_vec.size() == 0);
#endif
    if ((IsPointerType(type) != 0) || (IsArrayType(type) != 0)) {
      if (has_write_in_scope_fast(var, for_loop) != 0) {
        return false;
      }
    }
  }
  return true;
}

//  check whether an expression cover a continuous integer field
//  i.e. i + 2j + start (i->1...10, j ->1...5, start ->n...n) -> n + 3 ...n + 20
bool CSageCodeGen::IsContinuousAccess(void *expr, void *for_loop, void *pos) {
  //  cannot support floating node
  if (((pos == nullptr) || (is_floating_node(pos) != 0)) &&
      (is_floating_node(expr) != 0)) {
    return false;
  }
  CMarsExpression cexpr(expr, this, pos);
  map<void *, int64_t> iterators;
  map<void *, int64_t> invariants;
  if (!cexpr.IsStandardForm(for_loop, &iterators, &invariants)) {
    return false;
  }
  if (iterators.empty()) {
    return true;
  }
  vector<pair<void *, int64_t>> iterator_vec;
  for (map<void *, int64_t>::iterator it = iterators.begin();
       it != iterators.end(); ++it) {
    iterator_vec.push_back(*it);
  }
  std::sort(iterator_vec.begin(), iterator_vec.end(), [](auto one, auto other) {
    return std::abs(one.second) < std::abs(other.second);
  });

  if (std::abs(iterator_vec[0].second) != 1) {
    return false;
  }
  CMarsExpression var(iterator_vec[0].first, this, pos);
  CMarsRangeExpr range = var.get_range();
  range = range * iterator_vec[0].second;
  for (size_t i = 1; i < iterator_vec.size(); ++i) {
    int next_coef = iterator_vec[i].second;
    //  check whether ub - lb + 1 >= next_coef
    if (!range.length_is_greater_or_equal(std::abs(next_coef))) {
      return false;
    }
    CMarsExpression next_var(iterator_vec[i].first, this, pos);
    CMarsRangeExpr next_range = next_var.get_range();
    range = range + (next_range * next_coef);
  }
  return true;
}

//  Should support both floating and non-floating nodes
int CSageCodeGen::is_movable_test(void *exp, void *move_to, void *move_from) {
  if (move_from == nullptr) {
    move_from = exp;
  }

  if (has_side_effect(exp) != 0) {
    return 0;
  }

  if (move_to == move_from) {
    return 1;
  }

  if (IsExpression(move_from) != 0) {
    move_from = TraceUpToStatement(move_from);
  }
  if (IsExpression(move_to) != 0) {
    move_to = TraceUpToStatement(move_to);
  }

  //  /////////////////  /
  //  ZP: 2016-03-03
  if (IsValueExp(exp) != 0) {
    return 1;
  }
  if (TraceUpToFuncDecl(move_to) != TraceUpToFuncDecl(move_from)) {
    return 0;
  }
  //  /////////////////  /
  //

  //  ZP: 20161204: test the movability to a range
  //  if (IsForStatement(move_to)) move_to = GetLoopBody(move_to);
  if (IsBasicBlock(move_to) != 0) {
    void *first_non_trival_stmt = GetFirstNonTrivalStmt(move_to);
    void *last_non_trival_stmt = GetLastNonTrivalStmt(move_to);
    if ((first_non_trival_stmt != nullptr) &&
        (last_non_trival_stmt != nullptr)) {
      return static_cast<int>(
          (static_cast<int>(is_movable_test(exp, first_non_trival_stmt,
                                            move_from) != 0) != 0) &&
          (is_movable_test(exp, last_non_trival_stmt, move_from)) != 0);
    }
    { return 1; }
  }

  int64_t value;
  if (IsConstantInt(exp, &value, true, nullptr)) {
    return 1;
  }
  assert(!is_floating_node(move_from));
  assert(!is_floating_node(move_to));

  map<void *, int> vec_names;
  vector<void *> vec_refs;
  GetNodesByType_int(exp, "preorder", V_SgVarRefExp, &vec_refs);

  for (auto ref : vec_refs) {
    void *name = GetVariableInitializedName(ref);

    vec_names[name] = is_access_array_element(ref, exp);
  }

  void *new_scope = TraceUpByTypeCompatible(move_to, V_SgScopeStatement);
  void *common_scope = GetCommonScope(move_to, move_from);
  map<void *, int>::iterator it;
  for (it = vec_names.begin(); it != vec_names.end(); it++) {
    void *sg_name = it->first;
    bool access_element = it->second;
    void *sg_type = GetTypebyVar(sg_name);
    //  Bug1970 20180118 DefUse cannot support memory alias well
    if (access_element &&
        ((IsPointerType(sg_type) != 0) || (IsArrayType(sg_type) != 0))) {
      if (has_write_in_scope_fast(sg_name, common_scope) != 0) {
        return 0;
      }
    }
    void *var_scope = GetProject();
    if (IsArgumentInitName(sg_name) != 0) {
      void *func_decl = TraceUpToFuncDecl(sg_name);
      var_scope = GetFuncBody(func_decl);
    } else if (IsLocalInitName(sg_name) != 0) {
      var_scope = TraceUpByTypeCompatible(sg_name, V_SgScopeStatement);
    }
    if (IsInScope(new_scope, var_scope) == 0) {
      return 0;
    }
    //  Movable condition:
    //  1. only one def is found both move_to and move_from
    //  2. the same defs can be found at both move_to and move_from
    //  3. the scope of 'move_to' is in the scope of variable declarations
    vector<void *> vec_def1 = GetVarDefbyPosition(sg_name, move_from);
    vector<void *> vec_def2 = GetVarDefbyPosition(sg_name, move_to);
    if (vec_def1.size() != vec_def2.size()) {
      return 0;
    }
#if 1
    if (vec_def1.empty()) {
      return 0;
    }
    sort(vec_def1.begin(), vec_def1.end());
    sort(vec_def2.begin(), vec_def2.end());
    for (size_t i = 0; i != vec_def1.size(); ++i) {
      if (vec_def1[i] != vec_def2[i]) {
        return 0;
      }

      //  ZP: 20161204
      if ((is_located_in_scope_simple(vec_def1[i], move_from) != 0) ||
          (is_located_in_scope_simple(vec_def1[i], move_to) != 0)) {
        return 0;
      }
    }
#else
    if (vec_def1.size() != 1 || vec_def1[0] != vec_def2[0])
      return 0;
#endif
  }

  return 1;
}

int CSageCodeGen::has_lvalue_ref_in_scope(void *scope, void *name,
                                          vector<void *> *v_ret_ref) {
  int ret = 0;

  vector<void *> v_ref;
  get_ref_in_scope(name, scope, &v_ref);

  for (auto ref : v_ref) {
    //  SgExpression * sg_exp = isSgExpression((SgNode*) ref);

    //  if (sg_exp->isUsedAsLValue()) {
    if (is_write_conservative(ref, false) != 0) {
      if (v_ret_ref != nullptr) {
        v_ret_ref->push_back(ref);
      }
      ret = 1;
    }
  }

  return ret;
}

//  ZP: 20170606
//  if init_name is specified, only check for the array
//  otherwise check all the variabled defined outside the scope
int CSageCodeGen::has_side_effect_v2(void *exp,
                                     void *init_name /* = nullptr */) {
  assert(!is_floating_node(exp));  //  does not support floating node now

  //  1. get the interface variables
  set<void *> s_var;
  if (init_name != nullptr) {
    s_var.insert(init_name);
  } else {
    vector<void *> v_ref;
    GetNodesByType_int(exp, "preorder", V_SgVarRefExp, &v_ref);
    for (auto ref : v_ref) {
      void *init = GetVariableInitializedName(ref);
      if (is_located_in_scope(init, exp) == 0) {
        s_var.insert(init);
      }
    }
  }

  //  2. check write in scope
  for (auto var : s_var) {
    if (has_write_in_scope(var, exp) != 0) {
      return 1;
    }
  }

  return 0;
}
//  #endif

//  Shold support both floating and non-floating nodes
int CSageCodeGen::has_side_effect(void *exp) {
  //  ZP: TODO: check if ROSE has the similiar functions

  //  refer to is_write_conservative
  vector<void *> vec_set;

  vec_set.clear();
  GetNodesByType_int(exp, "preorder", V_SgAssignOp, &vec_set);
  if (!vec_set.empty()) {
    return 1;
  }

  vec_set.clear();
  GetNodesByType_int(exp, "preorder", V_SgCompoundAssignOp, &vec_set,
                     true /*compatible*/);
  if (!vec_set.empty()) {
    return 1;
  }

  vec_set.clear();
  GetNodesByType_int(exp, "preorder", V_SgPlusPlusOp, &vec_set);
  if (!vec_set.empty()) {
    return 1;
  }

  vec_set.clear();
  GetNodesByType_int(exp, "preorder", V_SgMinusMinusOp, &vec_set);
  if (!vec_set.empty()) {
    return 1;
  }

  vec_set.clear();
  GetNodesByType_int_compatible(exp, V_SgFunctionCallExp, &vec_set);
  if (!vec_set.empty()) {
    for (auto call : vec_set) {
      int op = GetOperatorOp(GetFuncDeclByCall(call, 0));
      if (op == 0 || IsCompoundAssignOp(op) || IsIncrementOp(op) ||
          IsAssignOp(op))
        return 1;
      // if it is the rest binary, unary or operator conversion op, there is no
      // side effect
      if (IsBinaryOp(op) || IsUnaryOp(op) || IsImplicitConversionOp(op))
        continue;
      return 1;
    }
    return 0;
  }

  return 0;
}
//
//  This function returns the real expression node in AST, which can be later
//  replaced by
//  Cross-function analysis
//  Handling of special cases:
//  0. function call argument: propagate for VarRef
//  1. Partial dim reference: do not propagate currently
//  2. Alias:                 propagate currently
//  3. if a function call has another calls of the same function return empty
int CSageCodeGen::GetPntrReferenceOfArray(void *array, void *sg_scope,
                                          vector<void *> *vec_access,
                                          int allow_artifical_ref) {
  set<void *> visited;
  return GetPntrReferenceOfArray(array, sg_scope, vec_access, &visited,
                                 allow_artifical_ref);
}

int CSageCodeGen::GetPntrReferenceOfArray(void *array, void *sg_scope,
                                          vector<void *> *vec_access,
                                          set<void *> *visited,
                                          int allow_artifical_ref) {
  if (visited->count(array) > 0) {
#if PROJDEBUG
    cout << "  ---- [GetPntr] Multiple visit of the same array: " << _p(array)
         << endl;
#endif
    return 0;
  }
  visited->insert(array);

  size_t i;
  //  1. get all the references
  vector<void *> vec_refs;
  get_ref_in_scope(array, sg_scope, &vec_refs);
  int dim;
  //  int data_unit_size = 0;
  {
    void *sg_base_type;
    vector<size_t> sg_sizes;
    get_type_dimension(GetTypebyVar(array), &sg_base_type, &sg_sizes, array);
    dim = sg_sizes.size();
    //  data_unit_size = get_type_unit_size(sg_base_type, array);
  }

  //  2. handling the accesses
  //  2.1 element reference
  //  2.2 argument (with offset)
  //  2.3 full assignment (with offset)
  //  2.4 partial assignment (with offset) [ZP: supported, 20150821]
  //  2.5 memcpy (with offset)             [ZP: supported, 20150821]
  //  2.6 complex than assignment with offset => full range
  int res = 1;
  for (i = 0; i < vec_refs.size(); i++) {
    void *curr_ref = vec_refs[i];

    void *sg_array = nullptr;
    void *sg_pntr = nullptr;
    vector<void *> sg_indexes;
    int pointer_dim;
    parse_pntr_ref_by_array_ref(curr_ref, &sg_array, &sg_pntr, &sg_indexes,
                                &pointer_dim);

    if ((sg_array == nullptr) || (sg_pntr == nullptr)) {
#if PROJDEBUG
      cout << "  ---- [GetPntr] Array or pntr not recognized in ref: "
           << _up(curr_ref) << endl;
#endif
      res = 0;
      continue;
    }

    if (pointer_dim == dim) {
      //  1. full pntr
      vec_access->push_back(sg_pntr);
      continue;
    }
    //  2. pointer assignement or function argument
    void *new_array = nullptr;
    void *new_scope = nullptr;

    //  2.0 point assignment lhs, e.g. int * array = ...;
    {
      //  FIXME: typically the array is current the alias of another array
      //       this reference is ignored in range analysis

      void *var_name;
      void *value_exp;
      if (parse_assign(GetParent(sg_pntr), &var_name, &value_exp) != 0) {
        if (var_name == array) {
#if PROJDEBUG
          cout << "  ---- [GetPntr] Array ref is being assigned: "
               << _up(_pa(sg_pntr)) << endl;
#endif
          res = 0;
          continue;
        }
      }
    }

    //  2.1 point assignment rhs, e.g. int * alias = array;
    {
      void *var_name;
      void *value_exp;
      if (parse_assign(GetParent(sg_pntr), &var_name, &value_exp) != 0) {
        vec_access->push_back(sg_pntr);
        if (value_exp == sg_pntr && (IsInitName(var_name) != 0)) {
          new_array = var_name;
          new_scope = sg_scope;
        }
      }
    }

    //  2.2 function argument, e.g. sub_func(a); //  sub_func(int *b)
    {
      if (false) {
#ifdef PROJDEBUG
        void *func_call1 = TraceUpToFuncCall(sg_pntr);
        static int count = 0;
        cout << "FUNC:: " << count << " " << UnparseToString(func_call1)
             << endl;
        count++;
#endif
      }

      int arg_idx = GetFuncCallParamIndex(sg_pntr);
      if (-1 != arg_idx) {
        void *func_call = TraceUpToFuncCall(sg_pntr);
        assert(func_call);
        void *func_decl = GetFuncDeclByCall(func_call);

        if (IsMemCpy(func_call)) {
          vec_access->push_back(sg_pntr);
          continue;
        }
        if (GetFuncNameByCall(func_call).find("memcpy_wide_bus_read") == 0 ||
            GetFuncNameByCall(func_call).find("memcpy_wide_bus_write") == 0) {
          vec_access->push_back(sg_pntr);
          continue;
        }
        if (GetFuncNameByCall(func_call).find("__merlin_") == 0) {
          //  skip internal intrinsics
          continue;
        }
        if (IsFunctionDeclaration(func_decl) != 0) {
          //  ZP: 20151119
          {
            vector<void *> all_calls;
            GetFuncCallsFromDecl(func_decl, nullptr, &all_calls);
            if (all_calls.size() > 1) {
              new_array = GetFuncParam(func_decl, arg_idx);
              new_scope = GetFuncBody(func_decl);

              //  ZP: 20170318: if multi-calls, try to use the intrinsic bound
              //  for the refernece
              //  FIXME: this is a quick fix here, it would be better if the
              //  check is moved to analysis_delinear()
              //        because this multi-call checking is too specific to
              //        delinearization pass
              //  NOTE: function hierarchy path is the suggested way to deal
              //  with multiple call issue
              //
              //  This applies when the following conditions:
              //  1. new_array is 1-d
              //  2. new_array has constant bound in new_scope
              //  3. new_array has intrinsic call in new_scope
              if (allow_artifical_ref != 0) {
                int has_constant_bound = 1;
                int has_intrinsic_call = 0;

                //  FIXME there is a big assumption here to say the array
                //  dimension is 1
                if (get_dim_num_from_var(new_array) == 1) {
                  vector<void *> v_calls;
                  GetNodesByType_int_compatible(new_scope, V_SgFunctionCallExp,
                                                &v_calls);
                  for (auto call : v_calls) {
                    if (GetFuncNameByCall(call) == "__merlin_access_range") {
                      void *exp0 = GetFuncCallParam(call, 0);
                      if ((exp0 != nullptr) && (IsVarRefExp(exp0) != 0) &&
                          GetVariableInitializedName(exp0) == new_array) {
                        has_intrinsic_call = 1;
                        break;
                      }
                    }
                  }
                  if (has_intrinsic_call != 0) {
                    ArrayRangeAnalysis mar(
                        new_array, this,
                        //  new_scope, TraceUpToFuncDecl(sg_pntr));
                        new_scope, GetProject(), false, false);

                    CMarsRangeExpr mr_w = mar.GetRangeExprWrite()[0];
                    CMarsRangeExpr mr_r = mar.GetRangeExprRead()[0];

                    if (((mr_w.is_empty() == 0) &&
                         (mr_w.is_const_bound() == 0)) ||
                        ((mr_r.is_empty() == 0) &&
                         (mr_r.is_const_bound() == 0))) {
                      has_constant_bound = 0;
                    }

                    //  ZP: 20170401
                    //  Since the propagation is disabled, the range need to
                    //  be recorded separately
                    int64_t lb_w =
                        mr_w.is_empty() != 0 ? 0 : mr_w.get_const_lb();
                    int64_t ub_w =
                        mr_w.is_empty() != 0 ? -1 : mr_w.get_const_ub();
                    int64_t lb_r =
                        mr_r.is_empty() != 0 ? 0 : mr_r.get_const_lb();
                    int64_t ub_r =
                        mr_r.is_empty() != 0 ? -1 : mr_r.get_const_ub();
                    int64_t n_ub = max(ub_w, ub_r);
                    int64_t n_lb = max(lb_w, lb_r);

                    if (has_constant_bound != 0) {
                      vec_access->push_back(sg_pntr);

                      //  ZP: 20170401
                      //  Since the propagation is disabled, the range need to
                      //  be recorded separately
                      CMarsExpression me_var(CMarsVariable(
                          this, CMarsRangeExpr(n_lb, n_ub, this)));

                      vector<void *> v_idx;
                      v_idx.push_back(me_var.get_expr_from_coeff());
                      void *sg_float_pntr =
                          CreateArrayRef(CreateVariableRef(new_array), v_idx);
                      vec_access->push_back(sg_float_pntr);

                      continue;
                    }
                  }
                }
              }

#if PROJDEBUG
              cout << "  ---- [GetPntr] Multiple calls of the same function: "
                   << _up(func_call) << endl;
#endif
              res = 0;
            }
          }

          vec_access->push_back(sg_pntr);
          new_array = GetFuncParam(func_decl, arg_idx);
          new_scope = GetFuncBody(func_decl);
          assert(new_scope);
        } else {
          //  undefined system call
        }
      }
    }
    if ((new_array != nullptr) && (new_scope != nullptr)) {
      int ret = GetPntrReferenceOfArray(new_array, new_scope, vec_access,
                                        visited, allow_artifical_ref);
      if (ret == 0) {
        res = 0;
      }
      continue;
    }

    //  ZP: 20170311 return 0 if the ref has write to the array
    if (is_write_conservative(sg_pntr) != 0) {
#if PROJDEBUG
      cout << "  ---- [GetPntr] Array ref is possibly written: "
           << _up(_pa(sg_pntr)) << endl;
#endif
      res = 0;
    } else {
      vec_access->push_back(sg_pntr);
    }
  }

  return res;
}

void *CSageCodeGen::get_alias_array_from_assignment_expression(void *sg_pntr) {
  int is_memcpy = 0;
  return get_alias_array_from_assignment_expression(sg_pntr, &is_memcpy);
}

//  Return the initialized name of the pointer, if
//  1. the sg_pntr is assigned into the pointer
//  2. the sg_pntr is used as the actual argument for the pointer as a formal
//  argument
void *CSageCodeGen::get_alias_array_from_assignment_expression(void *sg_pntr,
                                                               int *is_memcpy) {
  if (is_floating_node(sg_pntr) != 0) {
    return nullptr;
  }

  int pntr_pntr_dim = get_pntr_pntr_dim(sg_pntr);
  int pntr_init_dim = get_pntr_init_dim(sg_pntr);
  if (pntr_init_dim == pntr_pntr_dim) {
    return nullptr;  //  full access
  }

  *is_memcpy = 0;
  void *sg_alias = nullptr;
  //  2.1 point assignment rhs, e.g. int * alias = array;
  {
    void *var_name;
    void *value_exp;
    if (parse_assign(GetParent(sg_pntr), &var_name, &value_exp) != 0) {
      if (value_exp == sg_pntr && (IsInitName(var_name) != 0)) {
        sg_alias = var_name;
      }
    }
  }
  //  2.2 function argument, e.g. sub_func(a); //  sub_func(int *b)
  {
    //  void *sg_array_ref;
    int arg_idx = GetFuncCallParamIndex(sg_pntr);
    if (-1 != arg_idx) {
      void *func_call = TraceUpToFuncCall(sg_pntr);
      assert(func_call);
      void *func_decl = GetFuncDeclByCall(func_call);

      if (IsMemCpy(func_call)) {
        *is_memcpy = 1;
        return nullptr;
      }
      if (GetFuncNameByCall(func_call).find("memcpy_wide_bus_read") == 0 ||
          GetFuncNameByCall(func_call).find("memcpy_wide_bus_write") == 0) {
        *is_memcpy = 1;
        return nullptr;
      }
      if (IsFunctionDeclaration(func_decl) != 0) {
        sg_alias = GetFuncParam(func_decl, arg_idx);
      }
    }
  }
  return sg_alias;
}

int CSageCodeGen::analysis_lift_scalar(void *scalar_init) {
  if (IsLocalInitName(scalar_init) == 0) {
    return 0;
  }

  void *tt_type = GetTypebyVar(scalar_init);
  if (IsScalarType(tt_type) == 0) {
    return 0;
  }

  if (IsConstType(tt_type) != 0) {
    return 0;
  }

  return 1;
}

int CSageCodeGen::apply_lift_scalar(void *scalar_init) {
  void *sg_scope = m_sg_project;
  void *sg_base_type = GetTypebyVar(scalar_init);
  vector<size_t> dims(1, 1);
  void *sg_array_type = CreateArrayType(sg_base_type, dims);
  string var_name = GetVariableName(scalar_init);
  string new_var_name = var_name + "_arr";
  void *func_decl = TraceUpToFuncDecl(scalar_init);
  if (func_decl == nullptr) {
    return 0;
  }
  void *func_body = GetFuncBody(func_decl);
  if (func_body == nullptr) {
    return 0;
  }
  void *orig_var_decl = GetVariableDecl(scalar_init);
  if (orig_var_decl == nullptr) {
    return 0;
  }
  if (GetInitializer(scalar_init) != nullptr) {
    return 0;
  }

  vector<void *> vec_refs;
  get_ref_in_scope(scalar_init, sg_scope, &vec_refs);
  if (vec_refs.empty()) {
    return 0;
  }
  get_valid_local_name(func_decl, &new_var_name);
  void *new_var_decl = CreateVariableDecl(sg_array_type, new_var_name, nullptr,
                                          TraceUpToScope(orig_var_decl));
  InsertStmt(new_var_decl, orig_var_decl);
  for (auto &ref : vec_refs) {
    void *var_ref = CreateVariableRef(new_var_decl);
    void *pntr_ref = CreateExp(V_SgPntrArrRefExp, var_ref, CreateConst(0));
    ReplaceExp(ref, pntr_ref);
  }
  return 1;
}

int CSageCodeGen::is_perfectly_nested(void *sg_loop) {
  void *sg_parent_loop = TraceUpToForStatement(GetParent(sg_loop));

  if (sg_parent_loop == nullptr) {
    return 0;
  }

  void *curr_loop = sg_loop;
  while (curr_loop != nullptr) {
    curr_loop = GetParent(curr_loop);

    if (GetChildStmtNum(curr_loop) != 1) {
      return 0;
    }

    if (curr_loop == sg_parent_loop) {
      return 1;
    }

    if (IsBasicBlock(curr_loop) == 0) {
      return 0;
    }
  }
  return 0;
}

string CSageCodeGen::get_internal_loop_label(void *sg_loop) {
  string ret;
  vector<int> order = get_loop_order_vector_in_function(sg_loop);

  ret = "L";
  for (size_t i = 0; i < order.size(); i++) {
    ret += "_" + my_itoa(order[i]);
  }
  //  ret += "_" + UnparseToString(GetLoopIterator(sg_loop));

  return ret;
}

//  ZP: Note:
//  1. The for-loop in if-branch and else-branch will be the different order at
//  the same level
//  2. FIXME: while loop is not considered as a loop layer
vector<int> CSageCodeGen::get_loop_order_vector_in_function(void *sg_pos) {
  vector<int> ret;

  assert(!is_floating_node(sg_pos));
  void *sg_loop = TraceUpToLoopScope(sg_pos);
  void *sg_func_decl = TraceUpToFuncDecl(sg_loop);

  //  1. get the order vector of upper loop
  {
    void *sg_upper_loop = TraceUpToLoopScope(GetParent(sg_loop));
    if (sg_upper_loop != nullptr) {
      ret = get_loop_order_vector_in_function(sg_upper_loop);
    }
  }

  //  2. append the loop position of the current loop
  int n_loop_pos = 0;
  {
    void *sg_upper_loop = TraceUpToLoopScope(GetParent(sg_loop));

    if ((sg_upper_loop != nullptr) &&
        (IsBasicBlock(GetLoopBody(sg_upper_loop)) == 0)) {
      n_loop_pos = 0;  //  the upper loop has no body
    } else {
      void *sg_bb = GetFuncBody(sg_func_decl);
      void *sg_upper_scope = nullptr;
      if (sg_upper_loop != nullptr) {
        sg_bb = GetLoopBody(sg_upper_loop);
        sg_upper_scope = sg_upper_loop;
      }

      vector<void *> vec_sub_loops;
      GetNodesByType_compatible(sg_bb, "SgLoopStatement", &vec_sub_loops);
      for (size_t j = 0; j < vec_sub_loops.size(); j++) {
        if (sg_loop == vec_sub_loops[j]) {
          break;
        }

        if (TraceUpToLoopScope(GetParent(vec_sub_loops[j])) == sg_upper_scope) {
          n_loop_pos++;
        }
      }
    }
  }
  ret.push_back(n_loop_pos);

  return ret;
}

void CSageCodeGen::clean_empty_aggregate_initializer(void *sg_scope) {
  //  FIXME: ZP: 20151204: ROSE BUG, it will automatically add aggregate
  //  initializer for object arrays
  vector<void *> ap_int_decl;
  GetNodesByType_int(sg_scope, "preorder", V_SgVariableDeclaration,
                     &ap_int_decl);
  for (size_t i = 0; i < ap_int_decl.size(); i++) {
    string typeStr = UnparseToString(GetOrigTypeByTypedef(GetBaseType(
        GetTypebyVar(GetVariableInitializedName(ap_int_decl[i])), true)));
    if (typeStr.find("class") == 0 || typeStr.find("struct") == 0) {
#ifdef PROJDEBUG
//  cout << _p(ap_int_decl[i]) << endl;
#endif
      void *initer = GetInitializer(GetVariableInitializedName(ap_int_decl[i]));
      //  cout << _p(initer) << endl;

      int is_empty_aggregate =
          static_cast<int>(UnparseToString(initer).empty());
      if (is_empty_aggregate != 0) {
        SetInitializer(GetVariableInitializedName(ap_int_decl[i]), nullptr);
      }

#ifdef PROJDEBUG
//  initer = GetInitializer(GetVariableInitializedName(ap_int_decl[i]));
//  cout << "->" << endl;
//  cout << _p(initer) << endl;
//  cout << _p(ap_int_decl[i]) << endl;
#endif
    }
  }
}

bool CSageCodeGen::clean_implict_conversion_operator(void *sg_scope) {
  //  FIXME: Youxiang: 20190628 ROSE BUG, it will automatically add implict
  //  operator conversion
  vector<void *> vec_call_exp;
  GetNodesByType_int(sg_scope, "postorder", V_SgFunctionCallExp, &vec_call_exp);
  bool Changed = false;
  for (auto call : vec_call_exp) {
    void *func_decl = GetFuncDeclByCall(call, 0);
    int arg_num = GetFuncCallParamNum(call);
    if (arg_num != 0) {
      continue;
    }
    if (!IsMemberFunction(func_decl)) {
      continue;
    }
    if (GetFuncName(func_decl).find("operator ") != 0) {
      continue;
    }
    SgExpression *call_expr =
        isSgFunctionCallExp(static_cast<SgNode *>(call))->get_function();
    if (IsDotExp(call_expr) != 0) {
      void *obj_exp = GetExpLeftOperand(call_expr);
      ReplaceExp(call, CopyExp(obj_exp));
      Changed = true;
    } else if (IsArrowExp(call_expr) != 0) {
      void *obj_exp = GetExpLeftOperand(call_expr);
      ReplaceExp(call, CreateExp(V_SgPointerDerefExp, CopyExp(obj_exp)));
      Changed = true;
    }
  }
  return Changed;
}

// This function detect the missing hls:: namespace prefix and add into the code
// explicitly Refer to MER-1697 for details Example:
//    log(...) => hls::log(...)
void CSageCodeGen::recover_hls_namespace_prefix(void *scope) {
  vector<void *> vec;
  GetNodesByType(scope, "preorder", "SgFunctionRefExp", &vec);

  for (auto ref : vec) {
    SgFunctionRefExp *sg_ref =
        isSgFunctionRefExp(reinterpret_cast<SgNode *>(ref));
    SgSymbol *symbol = sg_ref->get_symbol();
    SgScopeStatement *scope =
        reinterpret_cast<SgScopeStatement *>(TraceUpToScope(symbol));
    auto ns_def = isSgNamespaceDefinitionStatement(scope);
    if (ns_def) {
      auto ns_decl = ns_def->get_namespaceDeclaration();
      if (ns_decl && "hls" == ns_decl->get_name().getString()) {
        SageInterface::addTextForUnparser(
            sg_ref, "hls::", AstUnparseAttribute::e_before);
      }
    }
  }
}

void CSageCodeGen::get_perfectly_nested_loops_from_bottom(
    void *sg_loop, vector<void *> *loops) {
  if (IsForStatement(sg_loop) == 0) {
    return;
  }
  void *curr_stmt = sg_loop;
  while (true) {
    if (IsForStatement(curr_stmt) != 0) {
      loops->push_back(curr_stmt);
      curr_stmt = GetParent(curr_stmt);
    } else if (IsBasicBlock(curr_stmt) != 0) {
      size_t num_childs = GetChildStmtNum(curr_stmt);
      if (num_childs == 1) {
        curr_stmt = GetParent(curr_stmt);
        continue;
      }
      void *next_stmt = nullptr;
      for (size_t i = 0; i < num_childs; ++i) {
        void *one_child = GetChildStmt(curr_stmt, i);
        if (IsPragmaDecl(one_child) != 0) {
          continue;
        }
        if (IsLabelStatement(one_child) != 0) {
          void *stmt = GetStmtFromLabel(one_child);
          if ((IsBasicBlock(stmt) != 0) && GetChildStmtNum(stmt) == 0) {
            continue;
          }
        }
        if (next_stmt != nullptr) {
          next_stmt = nullptr;
          break;
        }
        next_stmt = one_child;
      }
      if (next_stmt == nullptr) {
        break;
      }
      curr_stmt = GetParent(curr_stmt);
    } else if (IsLabelStatement(curr_stmt) != 0) {
      curr_stmt = GetParent(curr_stmt);
    } else {
      break;
    }
  }
}

void *CSageCodeGen::rebuild_pntr_ref(
    void *pntr_ref, void *sg_pos) {  //  rebuild the pntr by value propagation
  if (sg_pos == nullptr) {
    sg_pos = pntr_ref;
  }
  assert(!is_floating_node(sg_pos));

  void *ret;
  void *curr_array;  //  = GetVariableInitializedName(sg_pntr);
  //  void *array_ref;
  vector<void *> tt_vec;
  parse_pntr_ref_new(pntr_ref, &curr_array, &tt_vec);

  vector<void *> new_idx;
  //  FIXME, ZP: do not know why parse_pntr_ref_new is generating in the
  //  reversed order
  size_t n = tt_vec.size() - 1;
  for (size_t i = 0; i < tt_vec.size(); i++) {
    CMarsExpression me(tt_vec[n - i], this, sg_pos);
    new_idx.push_back(me.get_expr_from_coeff());
  }

  ret = CreateArrayRef(CreateVariableRef(curr_array), new_idx);

  return ret;
}

void *CSageCodeGen::GetLocation(void *start_stmt, bool forward) {
  void *loc_node = start_stmt;
  while ((loc_node != nullptr) &&
         ((IsPragmaDecl(loc_node) != 0) || IsTransformation(loc_node)) &&
         (IsLabelStatement(loc_node) == 0)) {
    loc_node = forward ? GetNextStmt(loc_node) : GetPreviousStmt(loc_node);
  }
  if ((loc_node == nullptr) || IsTransformation(loc_node)) {
    loc_node = GetScope(start_stmt);
  }
  return loc_node;
}

bool CSageCodeGen::ContainsUnsupportedLoop(void *scope, bool recursive,
                                           bool while_support) {
  //  Yuxin: April 2018
  //  Supported by Mars IR now
  if (!while_support) {
    vector<void *> vec_while_loops;
    GetNodesByType_int(scope, "preorder", V_SgWhileStmt, &vec_while_loops);
    if (!vec_while_loops.empty()) {
      return true;
    }
    vector<void *> vec_doWhile_loops;
    GetNodesByType_int(scope, "preorder", V_SgDoWhileStmt, &vec_doWhile_loops);
    if (!vec_doWhile_loops.empty()) {
      return true;
    }
  }
  vector<void *> vec_calls;
  GetNodesByType_int(scope, "preorder", V_SgFunctionCallExp, &vec_calls);
  for (auto call : vec_calls) {
    if (IsMemCpy(call) && (is_memcpy_with_small_const_trip_count(
                               call, MERLIN_SMALL_TRIP_COUNT_THRESHOLD) == 0)) {
      return true;
    }
    void *callee_decl = GetFuncDeclByCall(call, 1);
    if ((callee_decl == nullptr) || (GetFuncBody(callee_decl) == nullptr)) {
      continue;
    }
    if (recursive) {
      if (ContainsUnsupportedLoop(GetFuncBody(callee_decl), recursive,
                                  while_support)) {
        return true;
      }
    }
  }
  return false;
}

void *CSageCodeGen::GetSpecifiedLoopByPragma(void *pragma_decl) {
  void *scope = GetParent(pragma_decl);
  size_t idx = GetChildStmtIdx(scope, pragma_decl);
  size_t num = GetChildStmtNum(scope);
  while (++idx < num) {
    void *next_stmt = GetChildStmt(scope, idx);
    if (IsLoopStatement(next_stmt) != 0) {
      return next_stmt;
    }
    if (IsLabelStatement(next_stmt) != 0) {
      void *label = next_stmt;
      void *stmt = GetStmtFromLabel(label, true /* skip brace */);
      if (IsLoopStatement(stmt) != 0) {
        return stmt;
      }
    }
  }
  return nullptr;
}

bool CSageCodeGen::check_name_visible_in_global_scope(void *scope,
                                                      const string &name,
                                                      bool check_sub_scope) {
  assert(IsScopeStatement(scope));
  if (check_sub_scope) {
    vector<void *> vec_scope;
    GetNodesByType_int_compatible(scope, V_SgScopeStatement, &vec_scope);
    for (auto sub_scope : vec_scope) {
      if (CheckSymbolExits(sub_scope, name))
        return true;
    }
  } else {
    if (CheckSymbolExits(scope, name))
      return true;
  }
  return false;
}

bool CSageCodeGen::check_name_visible_in_function(void *kernel_decl,
                                                  const string &name,
                                                  bool check_sub_scope) {
  void *func_def = GetFuncDefinitionByDecl(kernel_decl);
  if (!func_def)
    return false;
  if (check_sub_scope) {
    vector<void *> vec_scope;
    GetNodesByType_int_compatible(func_def, V_SgScopeStatement, &vec_scope);
    for (auto scope : vec_scope) {
      if (CheckSymbolExits(scope, name))
        return true;
    }
  } else {
    if (CheckSymbolExits(func_def, name))
      return true;
  }
  void *global_scope = GetGlobal(kernel_decl);
  void *curr_scope = GetScope(kernel_decl);
  do {
    if (CheckSymbolExits(curr_scope, name))
      return true;
    if (curr_scope == global_scope)
      break;
    curr_scope = GetScope(GetParent(curr_scope));
  } while (true);
  return false;
}

bool CSageCodeGen::check_name_visible_in_position(void *sg_pos,
                                                  const string &name) {
  void *curr_scope = GetScope(sg_pos);
  vector<void *> vec_sub_scope;
  GetNodesByType_int_compatible(curr_scope, V_SgScopeStatement, &vec_sub_scope);
  for (auto sub_scope : vec_sub_scope) {
    if (CheckSymbolExits(sub_scope, name))
      return true;
  }

  void *global_scope = GetGlobal(sg_pos);
  do {
    if (CheckSymbolExits(curr_scope, name))
      return true;
    if (curr_scope == global_scope)
      break;
    curr_scope = GetScope(GetParent(curr_scope));
  } while (true);
  return false;
}

bool CSageCodeGen::check_name_declared_within_function(void *kernel_decl,
                                                       const string &name) {
  void *func_def = GetFuncDefinitionByDecl(kernel_decl);
  if (!func_def)
    return false;
  vector<void *> vec_scope;
  GetNodesByType_int_compatible(func_def, V_SgScopeStatement, &vec_scope);
  for (auto scope : vec_scope) {
    if (CheckSymbolExits(scope, name))
      return true;
  }
  return false;
}

void CSageCodeGen::get_valid_local_name(void *func_decl, string *name) {
  *name = legalizeName(*name);
  bool add_underscore = true;
  if (name->back() == '_') {
    add_underscore = false;
  }
  if (check_name_visible_in_function(func_decl, *name, true)) {
    int suffix = 0;
    while (check_name_visible_in_function(
        func_decl, *name + (add_underscore ? "_" : "") + my_itoa(suffix),
        true)) {
      suffix++;
    }
    *name += (add_underscore ? "_" : "") + my_itoa(suffix);
  }
}

void CSageCodeGen::get_valid_name(void *sg_pos, string *name) {
  *name = legalizeName(*name);
  bool add_underscore = true;
  if (name->back() == '_') {
    add_underscore = false;
  }
  if (check_name_visible_in_position(sg_pos, *name)) {
    int suffix = 0;
    while (check_name_visible_in_position(
        sg_pos, *name + (add_underscore ? "_" : "") + my_itoa(suffix))) {
      suffix++;
    }
    *name += (add_underscore ? "_" : "") + my_itoa(suffix);
  }
}

void CSageCodeGen::get_valid_global_name(void *global_scope, string *name) {
  static bool unique_function_name =
      test_file_for_read(FUNCTION_NAME_UNIQUFIED_TAG) ? true : false;
  if (unique_function_name) {
    global_scope =
        static_cast<SgProject *>(GetProject())->get_globalScopeAcrossFiles();
  } else if (isSgGlobal(static_cast<SgNode *>(global_scope)) == nullptr) {
    return;
  }
  *name = legalizeName(*name);
  if (check_name_visible_in_global_scope(global_scope, *name, true)) {
    int suffix = 0;
    while (check_name_visible_in_global_scope(
        global_scope, *name + "_" + my_itoa(suffix), true)) {
      suffix++;
    }
    *name += "_" + my_itoa(suffix);
  }
}

string CSageCodeGen::legalizeName(const string &var_name) {
  string ret;
  for (auto c : var_name) {
    if ((isalnum(c) != 0) || c == '_') {
      ret += c;
    } else {
      ret += '_';
    }
  }
  std::string::iterator new_end =
      std::unique(ret.begin(), ret.end(), [](char lhs, char rhs) {
        return (lhs == rhs) && lhs == '_';
      });
  ret.erase(new_end, ret.end());
#if 0
    //  keep double underscore prefix
    if (var_name.find("__") == 0)
      ret = "_" + ret;
#endif
  return ret;
}

int CSageCodeGen::IsomorphicLoops(void *one_loop, void *other_loop) {
  if (one_loop == other_loop) {
    return 1;
  }
  void *iv;
  void *lb;
  void *ub;
  void *step;
  void *body;
  bool inc_dir;
  bool inclusive_bound;
  int64_t step_val_0;
  int64_t step_val_1;
  int ret = IsCanonicalForLoop(one_loop, &iv, &lb, &ub, &step, &body, &inc_dir,
                               &inclusive_bound);
  ret &= GetLoopStepValueFromExpr(step, &step_val_0);

  if ((step != nullptr) && (GetParent(step) == nullptr)) {
    DeleteNode(step);
  }
  if (ret == 0) {
    return 0;
  }

  ret = IsCanonicalForLoop(other_loop, &iv, &lb, &ub, &step, &body, &inc_dir,
                           &inclusive_bound);
  ret &= GetLoopStepValueFromExpr(step, &step_val_1);
  if ((step != nullptr) && (GetParent(step) == nullptr)) {
    DeleteNode(step);
  }
  if (ret == 0) {
    return 0;
  }
  if (step_val_0 != step_val_1) {
    return 0;
  }
  CMarsVariable one_mv(one_loop, this);
  CMarsRangeExpr one_mr = one_mv.get_range();
  CMarsVariable other_mv(other_loop, this);
  CMarsRangeExpr other_mr = other_mv.get_range();
  vector<CMarsExpression> one_lb = one_mr.get_lb_set();
  vector<CMarsExpression> other_lb = other_mr.get_lb_set();
  if (one_lb.size() != 1 || other_lb.size() != 1 ||
      ((one_lb[0] == other_lb[0]) == 0)) {
    return 0;
  }
  vector<CMarsExpression> one_ub = one_mr.get_ub_set();
  vector<CMarsExpression> other_ub = other_mr.get_ub_set();
  if (one_ub.size() != 1 || other_ub.size() != 1 ||
      ((one_ub[0] == other_ub[0]) == 0)) {
    return 0;
  }
  return 1;
}

void *CSageCodeGen::GetCommonPosition(void *sg_pos1, void *sg_pos2) {
  if (sg_pos1 == sg_pos2) {
    return sg_pos1;
  }
  if ((sg_pos1 == nullptr) || (sg_pos2 == nullptr) ||
      (is_floating_node(sg_pos1) != 0) || (is_floating_node(sg_pos2) != 0)) {
    return nullptr;
  }
  void *sg_decl_1 = TraceUpToFuncDecl(sg_pos1);
  void *sg_decl_2 = TraceUpToFuncDecl(sg_pos2);
  if (sg_decl_1 != sg_decl_2) {
    return nullptr;
  }
  void *sg_parent = sg_pos1;
  list<void *> stack_trace;
  while (IsInScope(sg_pos2, sg_parent) == 0) {
    stack_trace.push_back(sg_parent);
    sg_parent = GetParent(sg_parent);
  }
  while ((IsBasicBlock(sg_parent) != 0) && !stack_trace.empty()) {
    sg_parent = stack_trace.back();
    stack_trace.pop_back();
  }
  return sg_parent;
}

// if both nodes are under the same if branch
bool CSageCodeGen::InSameUnconditionalScope(void *pos1, void *pos2) {
  if (pos1 == pos2) {
    return true;
  }
  if ((pos1 == nullptr) || (pos2 == nullptr) || (is_floating_node(pos1) != 0) ||
      (is_floating_node(pos2) != 0)) {
    return false;
  }
  void *decl_1 = TraceUpToFuncDecl(pos1);
  void *decl_2 = TraceUpToFuncDecl(pos2);
  if (decl_1 != decl_2) {
    return false;
  }
  void *common_parent = pos1;
  while (IsInScope(pos2, common_parent) == 0) {
    common_parent = GetParent(common_parent);
  }
  void *curr_pos = pos1;
  while (curr_pos != common_parent) {
    void *parent = GetParent(curr_pos);
    if (IsIfStatement(parent))
      return false;
    curr_pos = parent;
  }
  curr_pos = pos2;
  while (curr_pos != common_parent) {
    void *parent = GetParent(curr_pos);
    if (IsIfStatement(parent))
      return false;
    curr_pos = parent;
  }
  return true;
}

void *CSageCodeGen::GetCommonPosition(void *sg_pos1,
                                      const t_func_call_path &path1,
                                      void *sg_pos2,
                                      const t_func_call_path &path2) {
  if ((sg_pos1 == nullptr) || (sg_pos2 == nullptr) ||
      (is_floating_node(sg_pos1) != 0) || (is_floating_node(sg_pos2) != 0)) {
    return nullptr;
  }

  if (path1 == path2) {
    if (sg_pos1 == sg_pos2) {
      return sg_pos1;
    }

    void *curr_sg_pos1 = sg_pos1;
    void *curr_sg_pos2 = sg_pos2;
    while ((curr_sg_pos1 != nullptr) && (curr_sg_pos2 != nullptr)) {
      void *sg_decl_1 = TraceUpToFuncDecl(curr_sg_pos1);
      void *sg_decl_2 = TraceUpToFuncDecl(curr_sg_pos2);
      if (sg_decl_1 == sg_decl_2) {
        return GetCommonPosition(curr_sg_pos1, curr_sg_pos2);
      }
      t_func_call_path::const_iterator it = path1.begin();
      bool update = false;
      while (it != path1.end()) {
        void *curr_scope = it->first;
        if ((curr_scope != nullptr) && (is_floating_node(curr_scope) == 0)) {
          curr_scope = TraceUpToFuncDecl(curr_scope);
        }
        if (curr_scope == sg_decl_1) {
          curr_sg_pos1 = it->second;
          update = true;
          break;
        }
        if (curr_scope == sg_decl_2) {
          curr_sg_pos2 = it->second;
          update = true;
          break;
        }
        ++it;
      }
      if (!update) {
        break;
      }
    }
    return nullptr;
  }

  t_func_call_path::const_reverse_iterator iter1 = path1.rbegin();
  t_func_call_path::const_reverse_iterator iter2 = path2.rbegin();
  void *comm_pos = nullptr;
  while (iter1 != path1.rend() && iter2 != path2.rend()) {
    if (*iter1 != *iter2) {
      if (iter1->first == iter2->first && (iter1->second != nullptr) &&
          (iter2->second != nullptr)) {
        void *curr_pos1 = iter1->second;
        void *curr_pos2 = iter2->second;
        return GetCommonPosition(curr_pos1, curr_pos2);
      }
      break;
    }
    comm_pos = iter1->second != nullptr ? iter1->second : iter1->first;
    iter1++;
    iter2++;
  }
  return comm_pos;
}

t_func_call_path CSageCodeGen::GetCommonPath(const t_func_call_path &path1,
                                             const t_func_call_path &path2) {
  t_func_call_path::const_reverse_iterator iter1 = path1.rbegin();
  t_func_call_path::const_reverse_iterator iter2 = path2.rbegin();
  t_func_call_path ret;
  while (iter1 != path1.rend() && iter2 != path2.rend()) {
    if (*iter1 != *iter2) {
      break;
    }
    ret.push_front(*iter1);
    iter1++;
    iter2++;
  }
  return ret;
}

//  ZP: A insert position is defined as a function decl with body
void *CSageCodeGen::GetFirstInsertPosInGlobal(void *sg_node) {
  void *global = GetGlobal(sg_node);
  return GetChildStmt(global, 0);

  //  for (int i = 0; i < GetChildStmtNum(global); i++)
  //  {
  //    void * decl = GetChildStmt(global, i);
  //    cout << "["<<my_itoa(i)<<"]---- " << _p(decl) << endl;
  //    if (IsFunctionDeclaration(decl) && GetFuncBody(decl))
  //    {
  //        return decl;
  //    }
  //  }

  //  return 0;
}

void *CSageCodeGen::GetCommonScope(void *sg_pos1, void *sg_pos2) {
  if (sg_pos1 == sg_pos2) {
    return sg_pos1;
  }
  if ((sg_pos1 == nullptr) || (sg_pos2 == nullptr) ||
      (is_floating_node(sg_pos1) != 0) || (is_floating_node(sg_pos2) != 0)) {
    return nullptr;
  }
  void *sg_decl_1 = TraceUpToFuncDecl(sg_pos1);
  void *sg_decl_2 = TraceUpToFuncDecl(sg_pos2);
  if (sg_decl_1 != sg_decl_2) {
    return nullptr;
  }
  void *sg_parent = sg_pos1;
  while (IsInScope(sg_pos2, sg_parent) == 0) {
    sg_parent = GetParent(sg_parent);
  }
  return sg_parent;
}

void *CSageCodeGen::GetCommonScope(const vector<void *> &vec_pos) {
  if (vec_pos.empty()) {
    return nullptr;
  }
  void *comm_scope = vec_pos[0];
  size_t i = 1;
  while (i < vec_pos.size()) {
    comm_scope = GetCommonScope(comm_scope, vec_pos[i]);
    ++i;
  }
  return comm_scope;
}

int CSageCodeGen::is_register(void *var_decl) {
  SgVariableDeclaration *sg_var_decl =
      isSgVariableDeclaration(static_cast<SgNode *>(var_decl));
  if (sg_var_decl == nullptr) {
    return 0;
  }
  return static_cast<int>(sg_var_decl->get_declarationModifier()
                              .get_storageModifier()
                              .isRegister());
}

void CSageCodeGen::set_register(void *var_decl) {
  SgVariableDeclaration *sg_var_decl =
      isSgVariableDeclaration(static_cast<SgNode *>(var_decl));
  if (sg_var_decl == nullptr) {
    return;
  }
  sg_var_decl->get_declarationModifier().get_storageModifier().setRegister();
}

void CSageCodeGen::unset_register(void *var_decl) {
  SgVariableDeclaration *sg_var_decl =
      isSgVariableDeclaration(static_cast<SgNode *>(var_decl));
  if (sg_var_decl == nullptr) {
    return;
  }
  sg_var_decl->get_declarationModifier().get_storageModifier().setDefault();
}

bool CSageCodeGen::IsSafeIntConversion(void *exp, void *pos, void *scope,
                                       void *type_a, void *type_b) {
  if ((IsIntegerType(type_a) == 0) || (IsIntegerType(type_b) == 0)) {
    return false;
  }

  const SgType *sg_type_a = isSgType(static_cast<SgNode *>(type_a));
  if (sg_type_a == nullptr) {
    return false;
  }
  const SgType *sg_type_b = isSgType(static_cast<SgNode *>(type_b));
  if (sg_type_b == nullptr) {
    return false;
  }
#if 0
    cout << _p(type_a) << " : " << sg_type_a->isUnsignedType() << endl;
    cout << _p(type_b) << " : " << sg_type_b->isUnsignedType() << endl;
#endif

  bool is_unsigned_a = sg_type_a->isUnsignedType();
  bool is_unsigned_b = sg_type_b->isUnsignedType();

  MarsProgramAnalysis::DUMMY dummy(this, exp, pos, scope);
  CMarsRangeExpr me_op_range(this);
  if (cache_get_range_exp.find(dummy) != cache_get_range_exp.end()) {
    me_op_range = cache_get_range_exp[dummy];
  } else {
    DEFINE_START_END;
    STEMP(start);
    // drop scope to get global range
    CMarsExpression me_op(exp, this, pos, scope);
    if (me_op.has_range())
      me_op_range = me_op.get_range();
    cache_get_range_exp.insert(
        pair<MarsProgramAnalysis::DUMMY, CMarsRangeExpr>(dummy, me_op_range));
    ACCTM(CSageCodeGen___IsSafeIntConversion_construct, start, end);
  }

  if (isSgTypeBool(static_cast<const SgNode *>(sg_type_b)) != nullptr) {
    return ((me_op_range.is_const_lb() != 0) &&
            me_op_range.get_const_lb() > 0) ||
           ((me_op_range.is_const_ub() != 0) &&
            me_op_range.get_const_ub() < 0) ||
           ((me_op_range.is_const_lb() != 0) &&
            (me_op_range.is_const_ub() != 0) &&
            me_op_range.get_const_lb() == me_op_range.get_const_ub() &&
            me_op_range.get_const_lb() == 0);
  }

  int a_bitwidth = get_bitwidth_from_type(type_a);
  int b_bitwidth = get_bitwidth_from_type(type_b);
  if (is_unsigned_b) {
    uint64_t max = std::numeric_limits<uint64_t>::max();
    if (b_bitwidth >= 1 && b_bitwidth <= sizeof(uint64_t) * 8)
      max = (1UL << (b_bitwidth - 1)) - 1 + (1UL << (b_bitwidth - 1));

    if (is_unsigned_a) {
      return (a_bitwidth <= b_bitwidth) || ((me_op_range.is_const_ub() != 0) &&
                                            me_op_range.get_const_ub() <= max);
    }
    return ((me_op_range.is_const_lb() != 0) &&
            me_op_range.get_const_lb() >= 0) &&
           ((a_bitwidth <= b_bitwidth) || ((me_op_range.is_const_ub() != 0) &&
                                           me_op_range.get_const_ub() <= max));
  }
  //  Youxiang 20190708 the following expression will result in overflow when
  //  the bitwdith is 64 which will result in incorrect optimization result
  //  int64_t max = (1UL << (b_bitwidth - 1)) - 1;
  int64_t max = std::numeric_limits<int64_t>::max();
  if (b_bitwidth >= 1 && b_bitwidth <= sizeof(uint64_t) * 8)
    max = (1UL << (b_bitwidth - 1)) - 1;
  int64_t min = -max - 1;

  if (is_unsigned_a) {
    return (a_bitwidth < b_bitwidth) || ((me_op_range.is_const_ub() != 0) &&
                                         me_op_range.get_const_ub() <= max);
  }
  return (a_bitwidth <= b_bitwidth) || ((me_op_range.is_const_lb() != 0) &&
                                        (me_op_range.is_const_ub() != 0) &&
                                        me_op_range.get_const_lb() >= min &&
                                        me_op_range.get_const_ub() <= max);
}

bool CSageCodeGen::GetCastValue(int64_t *value, void *cast_type) {
  SgType *sg_type = isSgType(static_cast<SgNode *>(cast_type));
  const SgType *t = sg_type;
#if 0
    while (t->variant() == T_TYPEDEF) {
      t = (static_cast<SgTypedefType *>(t))->get_base_type();
      ROSE_ASSERT(t != nullptr);
    }
#endif
  switch (t->variant()) {
  case T_CHAR:
  case T_SIGNED_CHAR:

    *value = ((*value & 0xff) << 56) >> 56;
    break;
  case T_UNSIGNED_CHAR:
    *value = *value & 0xff;
    break;
  case T_SHORT:
  case T_SIGNED_SHORT:
    *value = ((*value & 0xffff) << 48) >> 48;
    break;
  case T_UNSIGNED_SHORT:
    *value = *value & 0xffff;
    break;
  case T_INT:
  case T_SIGNED_INT:
    *value = ((*value & 0xffffffff) << 32) >> 32;
    break;
  case T_UNSIGNED_INT:
    *value = (*value & 0xffffffff);
    break;
  case T_BOOL:
    *value = static_cast<int64_t>(*value != 0);
    break;
  case T_LONG:
  case T_SIGNED_LONG:
  case T_UNSIGNED_LONG:
  case T_LONG_LONG:
  case T_SIGNED_LONG_LONG:
  case T_UNSIGNED_LONG_LONG:
    break;
  default:
    return false;  //  0
  }
  return true;
}

string CSageCodeGen::get_location(void *sg_node, bool simple,
                                  bool line_number) const {
  string sFileName = GetFileInfo_filename(sg_node, static_cast<int>(simple));
  if (!line_number) {
    return sFileName;
  }
  string sLineName = my_itoa(GetFileInfo_line(sg_node));
  return sFileName + ":" + sLineName;
}

#if 0
  string CSageCodeGen::get_location_from_metadata(void *sg_node) {
    string id = get_identifier_from_ast(sg_node);
    return GetLocationFromIdentifier(id);
  }
#endif

int CSageCodeGen::IsRecursiveFunction(void *func_decl, void **rec_func) {
  set<void *> visited;
  if (IsRecursiveFunction(func_decl, rec_func, &visited) != 0) {
    return 1;
  }
  return 0;
}

int CSageCodeGen::IsRecursiveFunction(void *func_decl, void **rec_func,
                                      set<void *> *visited) {
  static unordered_map<void *, void *> s_recursived_func;
  if (s_recursived_func.count(func_decl) > 0) {
    auto *res = s_recursived_func[func_decl];
    if (nullptr == res)
      return 0;
    *rec_func = res;
    return 1;
  }

  for (auto &func : *visited) {
    if (isSameFunction(func_decl, func)) {
      *rec_func = func_decl;
      s_recursived_func[func_decl] = func_decl;
      return 1;
    }
  }
  visited->insert(func_decl);
  void *func_body = GetFuncBody(func_decl);
  if (func_body == nullptr) {
    s_recursived_func[func_decl] = nullptr;
    return 0;
  }
  vector<void *> vec_call;
  GetNodesByType_int(func_body, "preorder", V_SgFunctionCallExp, &vec_call);
  for (auto &call : vec_call) {
    void *sub_func = GetFuncDeclByCall(call);
    if (sub_func == nullptr) {
      continue;
    }
    if (IsRecursiveFunction(sub_func, rec_func, visited) != 0) {
      return 1;
    }
  }
  visited->erase(func_decl);
  s_recursived_func[func_decl] = nullptr;
  return 0;
}

//  If there is alias pntr that can not be analyzed, 'confidence' will be
//  'false', in this case, the result explains like this: return 1 means
//  recursive definitely, and return 0 does not mean no recursion
int CSageCodeGen::IsRecursivePointerAlias(void *var_init,
                                          bool *confidence_of_negative) {
  set<void *> visited;
  *confidence_of_negative = true;
  if (IsRecursivePointerAlias(var_init, confidence_of_negative, &visited) !=
      0) {
    return 1;
  }
  return 0;
}
set<void *> CSageCodeGen::GetAssociatedAlias(void *array, bool *confidence) {
  set<void *> alias_set;
  *confidence = true;

  vector<void *> v_ref;
  get_ref_in_scope(array, nullptr, &v_ref);
  for (auto one_ref : v_ref) {
    {
      void *call = TraceUpToFuncCall(one_ref);

      if ((call != nullptr) &&
          IsMerlinInternalIntrinsic(GetFuncNameByCall(call))) {
        continue;
      }
    }

    void *one_pntr;
    void *sg_array;
    vector<void *> indexes;
    int pointer_dim;
    parse_pntr_ref_by_array_ref(one_ref, &sg_array, &one_pntr, &indexes,
                                &pointer_dim, one_ref);

    if ((one_pntr == nullptr) || sg_array != array) {
      *confidence = false;
      continue;
    }

    if (get_pntr_init_dim(one_pntr) == get_pntr_pntr_dim(one_pntr)) {
      continue;
    }

    void *parent = GetParent(one_pntr);
    if ((parent != nullptr) && (IsAssignOp(parent) != 0) &&
        one_pntr == GetExpLeftOperand(parent)) {
      if (IsExprStatement(GetParent(parent)) == 0) {
        *confidence = false;
      }
      continue;
    }

    //  Lvalue: Detect alias_var in four cases
    //  a. Function arg
    //  b. assignOp
    //  c. initial assign
    void *alias_var = get_alias_array_from_assignment_expression(one_pntr);

    //  d. exp stmt (useful in test)
    if ((IsExprStatement(GetParent(one_pntr)) != 0) &&
        ((IsPntrArrRefExp(one_pntr) != 0) || (IsAddOp(one_pntr) != 0))) {
      continue;
    }

    if (alias_var == nullptr) {
      *confidence = false;
      continue;
    }

    if (alias_set.find(alias_var) == alias_set.end()) {
      alias_set.insert(alias_var);
    }
  }

  return alias_set;
}

int CSageCodeGen::IsRecursivePointerAlias(void *var_init,
                                          bool *confidence_of_negative,
                                          set<void *> *visited) {
  if (visited->find(var_init) != visited->end()) {
    return 1;
  }
  visited->insert(var_init);
  bool conf = true;
  set<void *> v_alias = GetAssociatedAlias(var_init, &conf);
  if (!conf) {
    *confidence_of_negative = false;
  }
  for (auto one_alias : v_alias) {
    if (IsRecursivePointerAlias(one_alias, confidence_of_negative, visited) !=
        0) {
      return 1;
    }
  }
  visited->erase(var_init);
  return 0;
}

int CSageCodeGen::IsRecursiveType(void *sg_type) {
  set<void *> visited;
  void *rec_type = nullptr;
  if (IsRecursiveType(sg_type, &rec_type, &visited) != 0) {
    return 1;
  }
  return 0;
}

int CSageCodeGen::IsRecursiveType(void *sg_type, void **rec_type) {
  set<void *> visited;
  if (IsRecursiveType(sg_type, rec_type, &visited) != 0) {
    return 1;
  }
  return 0;
}

int CSageCodeGen::IsRecursiveType(void *sg_type, void **rec_type,
                                  set<void *> *visited) {
  void *base_type = GetOrigTypeByTypedef(sg_type, true);
  base_type = GetBaseType(base_type, true);
  base_type = GetOrigTypeByTypedef(base_type, true);
  static unordered_map<void *, void *> s_recursived_type;
  if (s_recursived_type.count(base_type) > 0) {
    auto *res = s_recursived_type[base_type];
    if (nullptr == res)
      return 0;
    *rec_type = res;

    return 1;
  }

  if ((IsStructureType(base_type) != 0) || (IsClassType(base_type) != 0) ||
      (IsUnionType(base_type) != 0)) {
    if (visited->count(base_type) > 0) {
      *rec_type = base_type;
      s_recursived_type[base_type] = base_type;
      return 1;
    }
    for (auto type : *visited) {
      if (SageInterface::checkTypesAreEqual(
              isSgType(static_cast<SgNode *>(base_type)),
              isSgType(static_cast<SgNode *>(type)))) {
        *rec_type = base_type;
        s_recursived_type[base_type] = base_type;

        return 1;
      }
    }
    visited->insert(base_type);
    vector<void *> vec_data_members;
    GetClassDataMembers(base_type, &vec_data_members);
    for (auto member : vec_data_members) {
      void *var_init = GetVariableInitializedName(member);
      void *var_type = GetTypebyVar(var_init);
      if (IsRecursiveType(var_type, rec_type, visited) != 0) {
        *rec_type = member;
        s_recursived_type[base_type] = member;
        return 1;
      }
    }
    visited->erase(base_type);
  }

  if ((IsTypedefType(base_type) != 0) || (IsModifierType(base_type) != 0)) {
    void *orig_type = GetElementType(base_type);
    if (IsRecursiveType(orig_type, rec_type, visited) != 0) {
      s_recursived_type[base_type] = rec_type;
      return 1;
    }
  }
  s_recursived_type[base_type] = nullptr;
  return 0;
}

int CSageCodeGen::IsMixedCAndCPlusPlusDesign() {
  bool has_c = false;
  bool has_cpp = false;
  for (auto &file_name : m_inputFiles) {
    string::size_type pos = file_name.find_last_of('.');
    if (pos == string::npos) {
      continue;
    }
    string suffix = file_name.substr(pos + 1);
    for (size_t i = 0; i != sizeof(cpp_suffix) / sizeof(string); ++i) {
      if (cpp_suffix[i] == suffix) {
        has_cpp = true;
        break;
      }
    }
    if (suffix == "c") {
      has_c = true;
    }
  }
  return static_cast<int>(has_c && has_cpp);
}

int CSageCodeGen::IsPureCDesign() {
  bool has_c = false;
  bool has_cpp = false;
  for (auto &file_name : m_inputFiles) {
    string::size_type pos = file_name.find_last_of('.');
    if (pos == string::npos) {
      continue;
    }
    string suffix = file_name.substr(pos + 1);
    for (size_t i = 0; i != sizeof(cpp_suffix) / sizeof(string); ++i) {
      if (cpp_suffix[i] == suffix) {
        has_cpp = true;
        break;
      }
    }
    if (suffix == "c") {
      has_c = true;
    }
  }
  return has_c && !has_cpp;
}

void CSageCodeGen::GetClassMembers(void *sg_class_type_or_decl,
                                   vector<void *> *members) {
  void *sg_class_decl = nullptr;
  if (isSgType(static_cast<SgNode *>(sg_class_type_or_decl))) {
    void *sg_class_type = GetOrigTypeByTypedef(sg_class_type_or_decl, true);
    sg_class_decl = GetTypeDeclByType(sg_class_type);
  } else {
    sg_class_decl =
        isSgClassDeclaration(static_cast<SgNode *>(sg_class_type_or_decl));
  }
  if (sg_class_decl == nullptr) {
    return;
  }
  SgClassDefinition *sg_class_def = isSgClassDefinition(
      static_cast<SgNode *>(GetClassDefinition(sg_class_decl)));
  if (sg_class_def == nullptr) {
    return;
  }
  for (auto m : sg_class_def->get_members()) {
    members->push_back(m);
  }
}

void *CSageCodeGen::GetClassMemberByName(const string &s_var_in,
                                         void *sg_class_type) {
  void *sg_class_decl = GetTypeDeclByType(sg_class_type);
  if (sg_class_decl == nullptr) {
    return nullptr;
  }
  SgClassDefinition *sg_class_def = isSgClassDefinition(
      static_cast<SgNode *>(GetClassDefinition(sg_class_decl)));
  if (sg_class_def == nullptr) {
    return nullptr;
  }
  for (auto m : sg_class_def->get_members()) {
    void *sg_ret = nullptr;
    string s_var;
    if (IsVariableDecl(m) != 0) {
      s_var = _up(GetVariableInitializedName(m));
      sg_ret = GetVariableInitializedName(m);
    } else if (IsFunctionDeclaration(m) != 0) {
      s_var = GetFuncName(m);
      sg_ret = m;
    } else {
      continue;
    }

    //  remove "::"
    vector<string> v_sub = str_split(s_var, "::");

    if (v_sub.size() > 0 && v_sub[v_sub.size() - 1] == s_var_in) {
      return sg_ret;
    }
  }
  return nullptr;
}

void *CSageCodeGen::GetDefaultConstructor(void *type_or_decl) {
  void *org_cls = type_or_decl;
  if (IsType(type_or_decl))
    org_cls = GetTypeDeclByType(type_or_decl);
  auto cls = isSgClassDeclaration(static_cast<SgNode *>(org_cls));
  if (cls == nullptr) {
    return nullptr;
  }
  SgClassDefinition *sg_class_def =
      isSgClassDefinition(static_cast<SgNode *>(GetClassDefinition(cls)));
  if (sg_class_def == nullptr) {
    return nullptr;
  }
  string class_name = cls->get_name();
  if (auto tinstcls = isSgTemplateInstantiationDecl(cls)) {
    class_name = tinstcls->get_templateName();
  }

  for (auto m : sg_class_def->get_members()) {
    if (IsFunctionDeclaration(m) != 0) {
      if (GetFuncName(m, false) == class_name) {
        // constructor
        bool contain_non_default_param = false;
        for (auto param : GetFuncParams(m)) {
          if (GetInitializer(param) == nullptr) {
            contain_non_default_param = true;
            break;
          }
        }
        if (!contain_non_default_param) {
          return m;
        }
      }
    }
  }
  return nullptr;
}

void CSageCodeGen::GetClassDataMembers(void *sg_class_type_or_decl,
                                       vector<void *> *data_members) {
  vector<void *> members;
  GetClassMembers(sg_class_type_or_decl, &members);
  for (auto m : members) {
    if (IsVariableDecl(m)) {
      data_members->push_back(m);
    } else if (IsClassDecl(m)) {
      if (IsAnonymousName(GetClassName(m))) {
        GetClassDataMembers(m, data_members);
      }
    }
  }
}

void CSageCodeGen::GetClassNonStaticDataMembers(void *sg_class_type_or_decl,
                                                vector<void *> *data_members) {
  vector<void *> members;
  GetClassMembers(sg_class_type_or_decl, &members);
  for (auto m : members) {
    if (IsVariableDecl(m) && !IsStatic(m)) {
      data_members->push_back(m);
    }
  }
}

void *CSageCodeGen::GetClassDefinition(void *sg_class_decl_) {
  SgClassDeclaration *sg_class_decl =
      isSgClassDeclaration(static_cast<SgNode *>(sg_class_decl_));

  if (sg_class_decl == nullptr) {
    return nullptr;
  }
  if (sg_class_decl->get_definition() != nullptr) {
    return sg_class_decl->get_definition();
  }
  if (sg_class_decl->get_declaration_associated_with_symbol() == nullptr) {
    return nullptr;
  }
  SgClassSymbol *clssym =
      isSgClassSymbol(sg_class_decl->search_for_symbol_from_symbol_table());
  if (!m_cls_syms_valid) {
    reset_cls_syms();
  }
  auto it = m_cls_syms.find(clssym);
  SgClassDeclaration *real_cls = isSgClassDeclaration(
      static_cast<SgNode *>(it != m_cls_syms.end() ? it->second : nullptr));
  if (real_cls != nullptr) {
    sg_class_decl = real_cls;
  }
  return sg_class_decl->get_definition();
}

void *CSageCodeGen::GetTypeDeclByDefinition(void *sg_class_def_) {
  SgClassDefinition *sg_class_def =
      isSgClassDefinition(static_cast<SgNode *>(sg_class_def_));
  if (sg_class_def == nullptr) {
    return nullptr;
  }
  return sg_class_def->get_declaration();
}

void *CSageCodeGen::GetTypeDeclByMemberFunction(void *sg_mem_func_) {
  SgMemberFunctionDeclaration *sg_mem_func =
      isSgMemberFunctionDeclaration(static_cast<SgNode *>(sg_mem_func_));
  if (sg_mem_func == nullptr) {
    return nullptr;
  }
  return sg_mem_func->get_associatedClassDeclaration();
}

void *CSageCodeGen::GetTypeDefDecl(void *sg_type_) {
  SgTypedefType *sg_type = isSgTypedefType(static_cast<SgNode *>(sg_type_));
  if (sg_type == nullptr) {
    return nullptr;
  }
  return sg_type->get_declaration();
}

void *CSageCodeGen::GetTypeByName(const string &type_name) {
  if (s_name_type_cache.count(type_name) > 0) {
    return s_name_type_cache[type_name];
  }
  vector<void *> vecDecl;
  void *res = nullptr;
  GetNodesByType_int_compatible(m_sg_project, V_SgClassDeclaration, &vecDecl);
  for (auto decl : vecDecl) {
    SgClassDeclaration *curr_decl =
        isSgClassDeclaration(static_cast<SgNode *>(decl));
    SgType *curr_type = curr_decl->get_type();
    string curr_type_name = GetTypeName(decl, true);
    if (!curr_type_name.empty())
      s_name_type_cache[curr_type_name] = curr_type;
    if (curr_type_name == type_name ||
        GetStringByType(curr_type) == type_name) {
      res = curr_type;
      break;
    }
  }
  if (nullptr == res) {
    vecDecl.clear();
    GetNodesByType_int(m_sg_project, "preorder", V_SgEnumDeclaration, &vecDecl);
    for (auto decl : vecDecl) {
      SgEnumDeclaration *curr_decl =
          isSgEnumDeclaration(static_cast<SgNode *>(decl));
      SgType *curr_type = curr_decl->get_type();
      string curr_type_name = GetTypeName(decl, true);
      if (!curr_type_name.empty())
        s_name_type_cache[curr_type_name] = curr_type;
      if (curr_type_name == type_name ||
          GetStringByType(curr_type) == type_name) {
        res = curr_type;
        break;
      }
    }
  }
  if (nullptr == res) {
    vecDecl.clear();
    GetNodesByType_int(m_sg_project, "preorder", V_SgTypedefDeclaration,
                       &vecDecl);
    for (auto decl : vecDecl) {
      SgTypedefDeclaration *curr_decl =
          isSgTypedefDeclaration(static_cast<SgNode *>(decl));
      SgType *curr_type = curr_decl->get_type();
      string curr_type_name = GetTypeName(decl, true);
      if (!curr_type_name.empty())
        s_name_type_cache[curr_type_name] = curr_type;
      if (curr_type_name == type_name ||
          GetStringByType(curr_type) == type_name) {
        res = curr_type;
        break;
      }
    }
  }
  s_name_type_cache[type_name] = res;
  return res;
}

void *CSageCodeGen::GetDirectTypeDeclByType(void *sg_type_) {
  SgClassType *sg_class_type = isSgClassType(static_cast<SgNode *>(sg_type_));
  if (sg_class_type != nullptr) {
    return sg_class_type->get_declaration();
  }
  SgEnumType *sg_enum_type = isSgEnumType(static_cast<SgNode *>(sg_type_));
  if (sg_enum_type != nullptr) {
    return sg_enum_type->get_declaration();
  }
  SgTypedefType *sg_typedef = isSgTypedefType(static_cast<SgNode *>(sg_type_));
  if (sg_typedef != nullptr) {
    return sg_typedef->get_declaration();
  }
  return nullptr;
}

void *CSageCodeGen::GetTypeDeclByType(void *sg_type_, int require_def) {
  SgClassType *sg_class_type = isSgClassType(static_cast<SgNode *>(sg_type_));
  if (sg_class_type != nullptr) {
    SgClassDeclaration *sg_class_decl =
        isSgClassDeclaration(sg_class_type->get_declaration());
    SgClassSymbol *clsssym = nullptr;
    if (sg_class_decl->get_declaration_associated_with_symbol() != nullptr) {
      clsssym =
          isSgClassSymbol(sg_class_decl->search_for_symbol_from_symbol_table());
    }
    if (!m_cls_syms_valid) {
      reset_cls_syms();
    }
    auto it = clsssym != nullptr ? m_cls_syms.find(clsssym) : m_cls_syms.end();
    SgClassDeclaration *real_cls = isSgClassDeclaration(
        static_cast<SgNode *>(it != m_cls_syms.end() ? it->second : nullptr));
    if (real_cls != nullptr) {
      sg_class_decl = real_cls;
    }
    if (GetClassDefinition(sg_class_decl) != nullptr) {
      return sg_class_decl;
    }
    if (IsTemplateInstClassDecl(sg_class_decl) != 0) {
      return GetTemplateClassDecl(sg_class_decl);
    }
    if (require_def == 0) {
      void *ret = sg_class_decl;
#if 0
      void *parent = sg_class_decl->get_parent();
      if (sg_class_decl->get_isUnNamed() && IsTypedefDecl(parent))
        ret = parent;
#endif
      return ret;
    }

    ////////////////////////////////////////////////////////////
    // ZP: dec-2019
    // Fix the issue in the following case where new travserse lead to wrong
    // ERROR message unit_test/kernel_separate/test_floating_decl
    if (sg_class_decl && sg_class_decl->get_definingDeclaration())
      return sg_class_decl->get_definingDeclaration();
    ////////////////////////////////////////////////////////////

    void *res = nullptr;
    if (s_type_decl_cache.count(sg_class_type) > 0) {
      res = s_type_decl_cache[sg_class_type];
      if (is_floating_node(res) != 0) {
        s_type_decl_cache.erase(sg_class_type);
      } else {
        return res;
      }
    }
    res = nullptr;
    vector<void *> vecDecl;
    GetNodesByType_int(m_sg_project, "preorder", V_SgClassDeclaration,
                       &vecDecl);
    for (auto decl : vecDecl) {
      SgClassDeclaration *curr_decl =
          isSgClassDeclaration(static_cast<SgNode *>(decl));
      SgType *curr_type = curr_decl->get_type();
      if ((GetClassDefinition(curr_decl) != nullptr) &&
          (GetTypeName(decl, true) == GetTypeName(sg_class_decl, true) ||
           SageInterface::checkTypesAreEqual(curr_type, sg_class_type) ||
           UnparseToString(curr_type) == UnparseToString(sg_class_type))) {
        res = decl;
        break;
      }
    }
    s_type_decl_cache[sg_class_type] = res;
    return res;
  }

  SgEnumType *sg_enum_type = isSgEnumType(static_cast<SgNode *>(sg_type_));
  if (sg_enum_type != nullptr) {
    SgEnumDeclaration *sg_enum_decl =
        isSgEnumDeclaration(sg_enum_type->get_declaration());
    if ((require_def == 0) || !sg_enum_decl->isForward()) {
      void *ret = sg_enum_decl;
#if 0
        void *parent = sg_enum_decl->get_parent();
        if (sg_enum_decl->get_isUnNamed() && IsTypedefDecl(parent))
          ret = parent;
#endif
      return ret;
    }
    void *res = nullptr;
    if (s_type_decl_cache.count(sg_enum_type) > 0) {
      res = s_type_decl_cache[sg_enum_type];
      if (is_floating_node(res) != 0) {
        s_type_decl_cache.erase(sg_enum_type);
      } else {
        return res;
      }
    }
    res = nullptr;
    vector<void *> vecDecl;
    GetNodesByType_int(m_sg_project, "preorder", V_SgEnumDeclaration, &vecDecl);
    for (auto decl : vecDecl) {
      SgEnumDeclaration *curr_decl =
          isSgEnumDeclaration(static_cast<SgNode *>(decl));
      SgType *curr_type = curr_decl->get_type();
      if (!curr_decl->isForward() &&
          (GetTypeName(decl, true) == GetTypeName(sg_enum_decl, true) ||
           SageInterface::checkTypesAreEqual(curr_type, sg_enum_type) ||
           UnparseToString(curr_type) == UnparseToString(sg_enum_type))) {
        res = decl;
        break;
      }
    }
    s_type_decl_cache[sg_enum_type] = res;
    return res;
  }

  SgTypedefType *sg_typedef = isSgTypedefType(static_cast<SgNode *>(sg_type_));
  if (sg_typedef != nullptr) {
    return sg_typedef->get_declaration();
  }
  return nullptr;
}

int CSageCodeGen::IsAnonymousName(const string &name) const {
  return static_cast<int>(name.find("__anonymous_") != string::npos);
}

string CSageCodeGen::GetClassName(void *class_decl) const {
  SgClassDeclaration *sg_class_decl =
      isSgClassDeclaration(static_cast<SgNode *>(class_decl));
  if (sg_class_decl == nullptr) {
    return "";
  }
  if (auto *tcinst = isSgTemplateInstantiationDecl(sg_class_decl)) {
    return tcinst->get_templateName();
  }
  return sg_class_decl->get_name();
}

string CSageCodeGen::GetTypeName(void *sg_type_decl_, bool qualified) const {
  void *curr_decl = sg_type_decl_;
#if 0
    while (IsAnonymousTypeDecl(curr_decl))
      curr_decl = GetParent(curr_decl);
#endif
  string name;
  SgClassDeclaration *sg_class_decl =
      isSgClassDeclaration(static_cast<SgNode *>(curr_decl));
  if (sg_class_decl != nullptr) {
    name = qualified ? sg_class_decl->get_qualified_name()
                     : sg_class_decl->get_name();
  }
  SgEnumDeclaration *sg_enum_decl =
      isSgEnumDeclaration(static_cast<SgNode *>(curr_decl));
  if (sg_enum_decl != nullptr) {
    name = qualified ? sg_enum_decl->get_qualified_name()
                     : sg_enum_decl->get_name();
  }
  SgTypedefDeclaration *sg_typedef_decl =
      isSgTypedefDeclaration(static_cast<SgNode *>(curr_decl));
  if (sg_typedef_decl != nullptr) {
    name = qualified ? sg_typedef_decl->get_qualified_name()
                     : sg_typedef_decl->get_name();
  }
  if (name.find("::") == 0) {
    return name.substr(2);
  }
  return name;
}

string CSageCodeGen::GetTypeNameByType(void *sg_type, bool qualified) const {
  SgClassType *sg_class_type = isSgClassType(static_cast<SgNode *>(sg_type));
  SgEnumType *sg_enum_type = isSgEnumType(static_cast<SgNode *>(sg_type));
  SgTypedefType *sg_typedef_type =
      isSgTypedefType(static_cast<SgNode *>(sg_type));
  void *type_decl = nullptr;
  if (sg_class_type != nullptr) {
    type_decl = isSgClassDeclaration(sg_class_type->get_declaration());
  } else if (sg_enum_type != nullptr) {
    type_decl = isSgEnumDeclaration(sg_enum_type->get_declaration());
  } else if (sg_typedef_type != nullptr) {
    type_decl = isSgTypedefDeclaration(sg_typedef_type->get_declaration());
  } else {
    return GetStringByType(sg_type);
  }
  while (IsAnonymousTypeDecl(type_decl) != 0) {
    type_decl = GetParent(type_decl);
  }
  return GetTypeName(type_decl, qualified);
}

void *CSageCodeGen::GetVariableDefinitionByName(void *sg_var) {
  if (IsGlobalInitName(sg_var) == 0) {
    return sg_var;
  }
  void *sg_var_decl = GetVariableDecl(sg_var);
  if (sg_var_decl == nullptr) {
    return sg_var;
  }
  if (!IsExtern(sg_var_decl) && !IsMemberVariable(sg_var)) {
    return sg_var;
  }
  if (_USE_CACHE_IN_TRAVERSE_) {
    if (s_global_decl_cache.count(sg_var) > 0) {
      return s_global_decl_cache[sg_var];
    }
  }
  void *res = sg_var;
  vector<void *> vec_vars;
  GetNodesByType_int(nullptr, "preorder", V_SgInitializedName, &vec_vars);
  for (auto var : vec_vars) {
    if (IsGlobalInitName(var) == 0) {
      continue;
    }
    void *sg_var_decl = GetVariableDecl(var);
    if ((sg_var_decl == nullptr) || IsStatic(sg_var_decl) ||
        IsExtern(sg_var_decl)) {
      continue;
    }
    if (GetVariableName(var, true) == GetVariableName(sg_var, true)) {
      //  youxiang 20161019  cannot handle anonymous structure well
      //  assert(SageInterface::checkTypesAreEqual(isSgType((SgNode*)GetTypebyVar(var)),
      //                                         isSgType((SgNode*)GetTypebyVar(sg_var))));
      res = var;
      break;
    }
  }
  s_global_decl_cache[sg_var] = res;
  return res;
}

void *CSageCodeGen::GetSwitchStmtSelector(void *sg_switch) {
  SgSwitchStatement *sg_switch_stmt =
      isSgSwitchStatement(static_cast<SgNode *>(sg_switch));
  if (sg_switch_stmt != nullptr) {
    return sg_switch_stmt->get_item_selector();
  }
  return nullptr;
}

int CSageCodeGen::IsCompatibleType(void *sg_type1_, void *sg_type2_, void *pos,
                                   bool strict) {
  return IsCompatibleType(sg_type1_, sg_type2_, pos, true /*top*/, strict);
}

int CSageCodeGen::IsCompatibleType(void *sg_type1_, void *sg_type2_, void *pos,
                                   bool top, bool strict) {
  SgType *sg_type1 =
      isSgType(static_cast<SgNode *>(GetOrigTypeByTypedef(sg_type1_)));
  SgType *sg_type2 =
      isSgType(static_cast<SgNode *>(GetOrigTypeByTypedef(sg_type2_)));
  if ((sg_type2 == nullptr) || (sg_type2 == nullptr)) {
    return 0;
  }
  //  1. exactly the same
  if (sg_type1 == sg_type2) {
    return 1;
  }
  void *sg_new_type1 = nullptr;
  if (RemoveTypedef(sg_type1_, &sg_new_type1, pos) != 0) {
    sg_type1 = isSgType(static_cast<SgNode *>(sg_new_type1));
  }
  void *sg_new_type2 = nullptr;
  if (RemoveTypedef(sg_type2_, &sg_new_type2, pos) != 0) {
    sg_type2 = isSgType(static_cast<SgNode *>(sg_new_type2));
  }
  if (SageInterface::checkTypesAreEqual(sg_type1, sg_type2)) {
    return 1;
  }
  if (UnparseToString(sg_type1) == UnparseToString(sg_type2)) {
    return 1;
  }
  if (top || !strict) {
    // we can only strip const/volatile for scalar but not for pointer/array
    //  2. strip const/volatile
    if (SageInterface::isConstType(sg_type1) ||
        SageInterface::isVolatileType(sg_type1) ||
        SageInterface::isConstType(sg_type2) ||
        SageInterface::isVolatileType(sg_type2)) {
      void *new_sg_type1 = RemoveConstVolatileType(sg_type1, pos);
      void *new_sg_type2 = RemoveConstVolatileType(sg_type2, pos);
      if ((new_sg_type1 != nullptr) || (new_sg_type2 != nullptr)) {
        if (new_sg_type1 != nullptr) {
          sg_type1 = isSgType(static_cast<SgNode *>(new_sg_type1));
        }
        if (new_sg_type2 != nullptr) {
          sg_type2 = isSgType(static_cast<SgNode *>(new_sg_type2));
        }
        if (IsCompatibleType(sg_type1, sg_type2, pos, top, strict) != 0) {
          return 1;
        }
      }
    }
    if (SageInterface::isVolatileType(sg_type1) ||
        SageInterface::isVolatileType(sg_type2)) {
      void *new_sg_type1 = RemoveConstVolatileType(sg_type1, pos);
      void *new_sg_type2 = RemoveConstVolatileType(sg_type2, pos);
      sg_type1 = isSgType(static_cast<SgNode *>(new_sg_type1));
      sg_type2 = isSgType(static_cast<SgNode *>(new_sg_type2));
      if (IsCompatibleType(sg_type1, sg_type2, pos, top, strict) != 0) {
        return 1;
      }
    }
  }

  if (top || !strict) {
    //  3. pointer and array promotion
    if (((IsPointerType(sg_type1) != 0) || (IsArrayType(sg_type1) != 0)) &&
        ((IsPointerType(sg_type2) != 0) || (IsArrayType(sg_type2) != 0))) {
      if (IsCompatibleType(GetElementType(sg_type1), GetElementType(sg_type2),
                           pos, false, strict) != 0) {
        return 1;
      }
    }
  }

  if (!strict) {
    //  4. scalar promotion
    if ((sg_type1->isIntegerType() || IsXilinxAPFixedType(sg_type1) ||
         IsXilinxAPIntType(sg_type1) || sg_type1->isFloatType()) &&
        (sg_type2->isIntegerType() || sg_type2->isFloatType() ||
         IsXilinxAPIntType(sg_type2) || IsXilinxAPFixedType(sg_type2))) {
      return 1;
    }
  }

  //  5. handle SgTypeString
  //  Need refactor as I'm using function parameter directly
  if ((IsSgTypeString(sg_type1_) != 0) || (IsSgTypeString(sg_type2_) != 0)) {
    sg_type1_ = ConvertTypeStrToPtrChar(sg_type1_);
    sg_type2_ = ConvertTypeStrToPtrChar(sg_type2_);
    if (IsCompatibleType(sg_type1_, sg_type2_, pos, top, strict) != 0) {
      return 1;
    }
  }
  return 0;
}

void *CSageCodeGen::find_kernel_call(void *pragma_decl) {
  //  skip compiler generated statement
  void *next_stmt = GetNextStmt(pragma_decl, false);
  if ((next_stmt == nullptr) || (IsFunctionDeclaration(next_stmt) != 0)) {
    return nullptr;
  }
  vector<void *> vec_expr;
  GetNodesByType_int(next_stmt, "preorder", V_SgFunctionCallExp, &vec_expr);
  void *expr = nullptr;
  if (vec_expr.size() == 1) {
    expr = vec_expr[0];
  }
  return expr;
}

string CSageCodeGen::get_task_name_by_kernel(void *kernel) {
  string func_name = GetFuncName(kernel);
  vector<void *> func_calls;
  GetFuncCallsFromDecl(kernel, nullptr, &func_calls);
  //    for (size_t j = 0; j < func_calls.size(); j++) {
  //        void * wrapper_func = TraceUpToFuncDecl(func_calls[j]);
  void *scope = GetProject();
  vector<pair<void *, string>> vecTldmPragmas;
  //        TraverseSimple(wrapper_func, "preorder", GetTLDMInfo_withPointer4,
  //        &vecTldmPragmas);
  TraverseSimple(scope, "preorder", GetTLDMInfo_withPointer4, &vecTldmPragmas);
  for (size_t i = 0; i < vecTldmPragmas.size(); i++) {
    void *kernel_pragma = vecTldmPragmas[i].first;
    string pragma_str = vecTldmPragmas[i].second;
    string sFilter;
    string sCmd;
    map<string, pair<vector<string>, vector<string>>> mapParams;
    tldm_pragma_parse_whole(pragma_str, &sFilter, &sCmd, &mapParams);
    boost::algorithm::to_upper(sFilter);
    boost::algorithm::to_upper(sCmd);
    if (CMOST_PRAGMA != sFilter && ACCEL_PRAGMA != sFilter &&
        TLDM_PRAGMA != sFilter) {
      continue;
    }
    if ("TASK" != sCmd && "KERNEL" != sCmd) {
      continue;
    }
    string user_kernel_name;
    if (!mapParams["name"].first.empty()) {
      user_kernel_name = mapParams["name"].first[0];
    }
    void *next_stmt = GetNextStmt(kernel_pragma);
    string kernel_name;
    if (IsFunctionDeclaration(next_stmt) != 0) {
      kernel_name = GetFuncName(next_stmt);
    } else {
      void *next_expr = GetExprFromStmt(next_stmt);
      if (IsFunctionCall(next_expr) != 0) {
        kernel_name = GetFuncNameByCall(next_expr);
        //  printf("Han1 Find a kernel : %s, func_name = %s\n",
        //  kernel_name.c_str(), func_name.c_str());
      }
    }
    if (user_kernel_name.empty()) {
      user_kernel_name = kernel_name;
    }
    if (func_name == kernel_name) {
      //  printf("Han1 Find a task name : %s\n", user_kernel_name.c_str());
      return user_kernel_name;
    }
  }
  //    }
  return "";
}

string CSageCodeGen::get_wrapper_name_by_kernel(void *kernel) {
  string func_name = GetFuncName(kernel);
  string task_name;
  //  task_name = get_task_name_by_kernel(kernel);
  //  if(task_name == "") {
  //    func_name = MERLIN_WRAPPER_PREFIX + func_name;
  //  } else {
  //    func_name = MERLIN_WRAPPER_PREFIX + task_name;
  //  }
  func_name = MERLIN_WRAPPER_PREFIX + func_name;
  return func_name;
}

void CSageCodeGen::remove_double_brace(void *sg_scope) {
  //  youxiang 20161222 remove double brace
  //  {{ ..}}
  vector<void *> vec_bbs;
  GetNodesByType_int(sg_scope, "preorder", V_SgBasicBlock, &vec_bbs);

  for (auto bb : vec_bbs) {
    void *parent = GetParent(bb);
    if (IsBasicBlock(parent) == 0) {
      continue;
    }
    if ((IsFromInputFile(parent) == 0) && !IsCompilerGenerated(parent) &&
        !IsTransformation(parent)) {
      continue;
    }
    if (GetChildStmtNum(parent) == 1 && GetChildStmt(parent, 0) == bb) {
      //  Copy the whole basicblock instead
      void *new_bb = CopyStmt(bb);
      ReplaceStmt(parent, new_bb);
    }
  }
}

void CSageCodeGen::remove_brace_after_label(void *sg_scope) {
  // L1: {st0; ...}
  // => L1: st0; {...}
  vector<void *> vec_labels;
  GetNodesByType_int(sg_scope, "postorder", V_SgLabelStatement, &vec_labels);

  for (auto label : vec_labels) {
    SgLabelStatement *sg_label =
        isSgLabelStatement(static_cast<SgNode *>(label));
    if ((IsFromInputFile(label) == 0) && !IsCompilerGenerated(label) &&
        !IsTransformation(label)) {
      continue;
    }
    if (sg_label == nullptr) {
      continue;
    }
    SgStatement *stmt = sg_label->get_statement();
    if ((stmt == nullptr) || (IsBasicBlock(stmt) == 0)) {
      continue;
    }
    size_t child_num = GetChildStmtNum(stmt);
    if (child_num == 0) {
      continue;
    }
    void *first_child = GetChildStmt(stmt, 0);
    void *first_child_copy = CopyStmt(first_child);
    if (child_num == 1) {
      ReplaceStmt(stmt, first_child_copy);
      continue;
    }
    RemoveStmt(first_child);
    void *rest_stmt_copy = CopyStmt(stmt);
    ReplaceStmt(stmt, first_child_copy);
    InsertAfterStmt(rest_stmt_copy, label);
  }
}

vector<void *> CSageCodeGen::get_alias_source(void *target_init, void *pos) {
  void *type = GetTypebyVar(target_init);
  vector<void *> res;
  if (IsPointerType(type) == 0) {
    return res;
  }
  vector<void *> vec_defs = GetVarDefbyPosition(target_init, pos);
  for (auto def : vec_defs) {
    void *init_val = def;
    if (IsInitializer(def) != 0) {
      init_val = GetInitializerOperand(def);
    } else if (IsAssignOp(def) != 0) {
      init_val = GetExpRightOperand(def);
    }
    res.push_back(init_val);
  }
  return res;
}

int CSageCodeGen::get_loop_trip_count_from_pragma(void *loop,
                                                  int64_t *trip_count) {
  if (IsLoopStatement(loop) == 0)
    return 0;
  void *loop_body = GetLoopBody(loop);

  ALGOP("CDG", "try to get trip count from pragma");
  vector<SgNode *> children =
      (static_cast<SgNode *>(loop_body))->get_traversalSuccessorContainer();
  for (auto child : children) {
    auto pragma = GetPragmaString(child);
    if (pragma.empty()) {
      continue;
    }
    ALGOP("CDG", "pragma: " << pragma);
    if ((pragma.find("LOOP_TRIPCOUNT") != string::npos ||
         pragma.find("loop_tripcount") != string::npos) &&
        (pragma.find("max") != string::npos ||
         pragma.find("MAX") != string::npos)) {
      auto begin_pos = pragma.find("max");
      if (begin_pos == string::npos) {
        begin_pos = pragma.find("MAX");
      }
      auto start_pos = pragma.find("=", begin_pos);
      if (start_pos != string::npos) {
        start_pos += 1;
      }
      auto end_pos = pragma.find(" ", begin_pos);
      if (end_pos == string::npos) {
        end_pos = pragma.size();
      }
      ALGOP("CDG", "begin index: " << begin_pos);
      ALGOP("CDG", "start index: " << start_pos);
      ALGOP("CDG", "end index: " << end_pos);

      if (start_pos == string::npos || end_pos == string::npos ||
          start_pos >= pragma.size() || end_pos > pragma.size()) {
        continue;
      }

      auto num = pragma.substr(start_pos, end_pos - start_pos);
      // Yuxin: Aug 31 2019
      // stoi will error out if the tripcount is variable
      *trip_count = my_atoi(num);
      ALGOP("CDG", "trip count: " << trip_count);
      return 1;
    }
  }

  return 0;
}

int CSageCodeGen::get_block_pragma_val(void *block, string key, int64_t *val) {
  boost::algorithm::to_upper(key);
  ALGOP("CDG", "key (upper case): " << key);

  ALGOP("CDG", "try to get value from pragma");
  vector<SgNode *> children =
      (static_cast<SgNode *>(block))->get_traversalSuccessorContainer();

  for (auto child : children) {
    auto pragma = GetPragmaString(child);
    if (pragma.empty()) {
      continue;
    }
    boost::algorithm::to_upper(pragma);
    ALGOP("CDG", "pragma (upper case): " << pragma);

    auto begin_pos = pragma.find(key);
    if (begin_pos != string::npos) {
      auto start_pos = pragma.find("=", begin_pos);
      if (start_pos != string::npos) {
        start_pos += 1;
      }
      auto end_pos = pragma.find(" ", start_pos);
      if (end_pos == string::npos) {
        end_pos = pragma.size();
      }

      ALGOP("CDG", "begin index: " << begin_pos);
      ALGOP("CDG", "start index: " << start_pos);
      ALGOP("CDG", "end index: " << end_pos);

      if (start_pos == string::npos || end_pos == string::npos ||
          start_pos >= pragma.size() || end_pos > pragma.size()) {
        continue;
      }

      auto num = pragma.substr(start_pos, end_pos - start_pos);
      try {
        int64_t new_val = stoi(num);
        if (new_val > 0) {
          *val = new_val;
          ALGOP("CDG", "val: " << *val);
          return 1;
        }
      } catch (std::exception &e) {
        ALGOP("CDG", "invalid pragma: " << pragma);
      }
    }
  }
  return 0;
}

int CSageCodeGen::get_loop_trip_count(void *for_loop, int64_t *trip_count,
                                      int64_t *trip_count_ub /*= nullptr*/) {
  if (trip_count_ub != nullptr) {
    get_loop_trip_count_from_pragma(for_loop, trip_count_ub);
    if (IsDoWhileStatement(for_loop) != 0 || IsWhileStatement(for_loop) != 0) {
      void *cond = GetLoopTest(for_loop);
      if (IsExprStatement(cond)) {
        void *cond_exp = GetExprFromStmt(cond);
        CMarsExpression me_cond(cond_exp, this, cond_exp, for_loop);
        if (me_cond.IsConstant() && me_cond.GetConstant() == 0) {
          *trip_count_ub = IsDoWhileStatement(for_loop) != 0;
        }
      }
    }
  }
  void *iv;
  void *lb;
  void *ub;
  void *step;
  void *body;
  bool inc_dir;
  bool inclusive_bound;
  int64_t step_val;
  int ret = IsCanonicalForLoop(for_loop, &iv, &lb, &ub, &step, &body, &inc_dir,
                               &inclusive_bound);
  ret &= GetLoopStepValueFromExpr(step, &step_val);
  if ((ret == 0) || (std::abs(step_val) == 0)) {
    return 0;
  }
  ALGOP("CDG", "start calculate TC for " << printNodeInfo(this, for_loop));
  CMarsRangeExpr mr = CMarsVariable(for_loop, this).get_range();
  if (mr.is_empty()) {
    *trip_count = 0;
    if (trip_count_ub != nullptr) {
      *trip_count_ub = 0;
    }
    return 1;
  }

  if (mr.get_ast() != nullptr &&
      ((mr.is_const_ub() == 0) || (mr.is_const_lb() == 0))) {
    mr.refine_range_in_scope(mr.get_ast()->GetProject(), for_loop);
  }
  CMarsExpression me_lb;
  CMarsExpression me_ub;
  if (mr.get_simple_bound(&me_lb, &me_ub) != 0) {
    CMarsExpression len = me_ub - me_lb + 1;
    if (len.IsConstant() != 0) {
      *trip_count =
          (len.GetConstant() + std::abs(step_val) - 1) / std::abs(step_val);
      if (trip_count_ub != nullptr) {
        *trip_count_ub = *trip_count;
      }
      ALGOP("CDG", "finish calculate TC for " << printNodeInfo(this, for_loop));
      return 1;
    }
    if (len.get_range().is_const_ub() != 0) {
      if (trip_count_ub != nullptr) {
        *trip_count_ub = len.get_range().get_const_ub() / std::abs(step_val);
      }
      ALGOP("CDG",
            "finish calculate TC_ub for " << printNodeInfo(this, for_loop));
    }
  }

  return 0;
}

int64_t CSageCodeGen::get_average_loop_trip_count(void *for_loop,
                                                  const t_func_call_path &path,
                                                  void *scope) {
  void *iv;
  void *lb;
  void *ub;
  void *step;
  void *body;
  bool inc_dir;
  bool inclusive_bound;
  int64_t step_val;
  int ret = IsCanonicalForLoop(for_loop, &iv, &lb, &ub, &step, &body, &inc_dir,
                               &inclusive_bound);
  ret &= GetLoopStepValueFromExpr(step, &step_val);
  if (ret == 0) {
    return std::numeric_limits<int64_t>::max();
  }
  CMarsRangeExpr mr = CMarsVariable(for_loop, this).get_range();
  CMarsExpression me_lb;
  CMarsExpression me_ub;
  if (mr.get_simple_bound(&me_lb, &me_ub) != 0) {
    CMarsExpression len = me_ub - me_lb + 1;
    CMarsRangeExpr len_r = len.get_range();
    len_r.refine_range_in_scope_v2(scope, for_loop, path);
    int64_t trip_count = 0;
    if (len_r.is_const_lb() != 0) {
      trip_count = len_r.get_const_lb();
    }
    if (len_r.is_const_ub() == 0) {
      return std::numeric_limits<int64_t>::max();
    }
    trip_count += len_r.get_const_ub();
    //  FIXME:
    trip_count >>= 1;

    trip_count = (trip_count + step_val - 1) / step_val;
    return trip_count;
  }
  return std::numeric_limits<int64_t>::max();
}

int64_t
CSageCodeGen::get_average_loop_trip_count(void *pos, void *scope,
                                          const t_func_call_path &path) {
  vector<void *> sg_loops;
  get_loop_nest_in_scope(pos, scope, path, &sg_loops);
  int64_t trip_count = 1;
  for (auto loop : sg_loops) {
    int64_t ave_trip_count = get_average_loop_trip_count(loop, path, scope);
    if (std::numeric_limits<int64_t>::max() == ave_trip_count) {
      return std::numeric_limits<int64_t>::max();
    }
    trip_count *= ave_trip_count;
  }
  return trip_count;
}

bool CSageCodeGen::get_loop_trip_count_simple(void *sg_loop,
                                              int64_t *trip_count) {
  void *ivar = nullptr;
  int ret = IsCanonicalForLoop(sg_loop, &ivar);
  if (ret == 0) {
    return false;
  }

  void *func = GetEnclosingNode("Function", sg_loop);
  CMarsRangeExpr mr = CMarsVariable(sg_loop, this, func).get_range();
  if (mr.is_empty() != 0) {
    *trip_count = 0;
    return true;
  }
  CMarsExpression me_lb;
  CMarsExpression me_ub;
  int ret1 = mr.get_simple_bound(&me_lb, &me_ub);
  if (ret1 == 0) {
    return false;
  }

  CMarsExpression me_range = me_ub - me_lb + 1;
  if (me_range.IsConstant() == 0) {
    return false;
  }
  *trip_count = me_range.get_const();
  return true;
}

void markInsertBracket(CSageCodeGen *ast, SgNode *parent, SgNode *body,
                       SgNode *content) {
  markInsert(ast, parent, getTopoIndex(ast, body),
             "Insert: " + printNodeInfo(ast, body, 80) +
                 "\nfrom: " + printNodeInfo(ast, parent, 80));
  markInsert(ast, body, getTopoIndex(ast, content),
             "Insert: " + printNodeInfo(ast, content, 80) +
                 "\nfrom: " + printNodeInfo(ast, body, 80));
}

void CSageCodeGen::detachFromParent(SgNode *content) {
  SgNode *parent = content->get_parent();
  if (!parent)
    return;
  markDelete(this, parent, getTopoIndex(this, content),
             "Delete: " + printNodeInfo(this, content) +
                 "\nfrom: " + printNodeInfo(this, parent));
  content->set_parent(nullptr);
}

int CSageCodeGen::add_label(const string &label_name, void *loop) {
  auto *parent = static_cast<SgNode *>(GetParent(loop));
  if (IsLabelStatement(parent) == 0) {
#if 1
    void *label_stmt =
        CreateLabelStmt(label_name, CopyStmt(loop), TraceUpToScope(parent));
    ReplaceStmt(loop, label_stmt);
#else
    auto *sg_loop = static_cast<SgNode *>(loop);
    markDelete(this, parent, getTopoIndex(this, sg_loop),
               "Delete: " + printNodeInfo(this, sg_loop) +
                   "\nfrom: " + printNodeInfo(this, parent));
    void *nullstmt = CreateStmt(V_SgNullStatement);
    InsertStmt(nullstmt, loop);
    SageInterface::removeStatement(
        static_cast<SgStatement *>(loop),
        true);  // true: move preprocess info like #include
    SgNode *ls = static_cast<SgNode *>(
        CreateLabelStmt(label_name, loop, TraceUpToScope(parent)));
    InsertStmt(ls, nullstmt);
    markInsert(this, ls, getTopoIndex(this, sg_loop),
               "Insert: " + printNodeInfo(this, sg_loop, 80) +
                   "\nfrom: " + printNodeInfo(this, ls, 80));
    RemoveStmt(nullstmt);
#endif
    return 1;
  }
  return 0;
}

void CSageCodeGen::SetLabelName(void *label, const string &new_name) {
  SgLabelStatement *sg_label = static_cast<SgLabelStatement *>(label);
  sg_label->set_label(new_name);
}

int CSageCodeGen::add_missing_brace(void *scope) {
  bool Changed = false;
  //  1. add missing brace of for-statement
  vector<void *> vec_for_stmt;
  GetNodesByType_int(scope, "preorder", V_SgForStatement, &vec_for_stmt);
  for (auto for_loop : vec_for_stmt) {
    SgForStatement *loop = isSgForStatement(static_cast<SgNode *>(for_loop));
    SgStatement *loop_body = loop->get_loop_body();
    if (isSgBasicBlock(loop_body) == nullptr) {
      SgNode *content = loop_body;
      detachFromParent(content);
      loop_body = static_cast<SgStatement *>(
          CreateBasicBlock(loop_body, false, for_loop));
      loop->set_loop_body(loop_body);
      loop_body->set_parent(loop);
      Changed = true;
      markInsertBracket(this, loop, loop_body, content);
    }
  }

  //  2 add missing brace of while-statement

  vector<void *> vec_while_stmt;
  GetNodesByType_int(scope, "preorder", V_SgWhileStmt, &vec_while_stmt);
  for (auto while_loop : vec_while_stmt) {
    SgWhileStmt *loop = isSgWhileStmt(static_cast<SgNode *>(while_loop));
    SgStatement *loop_body = loop->get_body();
    if (isSgBasicBlock(loop_body) == nullptr) {
      SgNode *content = loop_body;
      detachFromParent(content);
      loop_body = static_cast<SgStatement *>(
          CreateBasicBlock(loop_body, false, while_loop));
      loop->set_body(loop_body);
      loop_body->set_parent(loop);
      Changed = true;
      markInsertBracket(this, loop, loop_body, content);
    }
  }

  //  3 add missing brace of do-while-statement

  vector<void *> vec_dowhile_stmt;
  GetNodesByType_int(scope, "preorder", V_SgDoWhileStmt, &vec_dowhile_stmt);
  for (auto dowhile : vec_dowhile_stmt) {
    SgDoWhileStmt *loop = isSgDoWhileStmt(static_cast<SgNode *>(dowhile));
    SgStatement *loop_body = loop->get_body();
    if (isSgBasicBlock(loop_body) == nullptr) {
      SgNode *content = loop_body;
      detachFromParent(content);
      loop_body = static_cast<SgStatement *>(
          CreateBasicBlock(loop_body, false, dowhile));
      loop->set_body(loop_body);
      loop_body->set_parent(loop);
      Changed = true;
      markInsertBracket(this, loop, loop_body, content);
    }
  }

  //  4. add missing brace of if-statement
  vector<void *> vec_if_stmt;
  GetNodesByType_int(scope, "preorder", V_SgIfStmt, &vec_if_stmt);

  for (auto if_stmt : vec_if_stmt) {
    SgIfStmt *ifstmt = isSgIfStmt(static_cast<SgNode *>(if_stmt));
    SgStatement *true_body = ifstmt->get_true_body();
    if ((true_body != nullptr) && (isSgBasicBlock(true_body) == nullptr)) {
      SgNode *content = true_body;
      detachFromParent(content);
      true_body = static_cast<SgStatement *>(
          CreateBasicBlock(true_body, false, if_stmt));
      ifstmt->set_true_body(true_body);
      true_body->set_parent(ifstmt);
      Changed = true;
      markInsertBracket(this, ifstmt, true_body, content);
    }
    SgStatement *false_body = ifstmt->get_false_body();
    if ((false_body != nullptr) && (isSgBasicBlock(false_body) == nullptr)) {
      SgNode *content = false_body;
      detachFromParent(content);
      false_body = static_cast<SgStatement *>(
          CreateBasicBlock(false_body, false, if_stmt));
      ifstmt->set_false_body(false_body);
      false_body->set_parent(ifstmt);
      Changed = true;
      markInsertBracket(this, ifstmt, false_body, content);
    }
  }
#if 0
    //  20191119: Yuxin, till now it works without adding brackets
    //  20180502: YX new ROSE library treats case/default as a label which only
    //  contains the following the single statement. Adding brace will break
    //  semantics
    //  5 add missing brace of case-statement

    vector<void *> vec_case_stmt;
    GetNodesByType_int(scope, "preorder", V_SgCaseOptionStmt, &vec_case_stmt);
    for (auto it_case : vec_case_stmt) {
      SgCaseOptionStmt *sg_case = isSgCaseOptionStmt(
          static_cast<SgNode *>(it_case));
      SgStatement *case_body = sg_case->get_body();
      if (!isSgBasicBlock(case_body)) {
        SgNode *content = case_body;
        detachFromParent(content);
        case_body = static_cast<SgStatement *>(CreateBasicBlock)(case_body,
            false, sg_case);
        sg_case->set_body(case_body);
        case_body->set_parent(sg_case);
        Changed = true;
        markInsertBracket(this, sg_case, case_body, content);
      }
    }

    //  6 add missing brace of default-statement

    vector<void *> vec_default_stmt;
    GetNodesByType_int(scope, "preorder", V_SgDefaultOptionStmt,
        &vec_default_stmt);
    for (auto it_default : vec_default_stmt) {
      SgDefaultOptionStmt *sg_default =
        isSgDefaultOptionStmt(static_cast<SgNode *>(it_default));
      SgStatement *default_body = sg_default->get_body();
      if (!isSgBasicBlock(default_body)) {
        SgNode *content = default_body;
        detachFromParent(content);
        default_body =
          static_cast<SgStatement *>(CreateBasicBlock)(default_body,
              false, sg_default);
        sg_default->set_body(default_body);
        default_body->set_parent(sg_default);
        Changed = true;
        markInsertBracket(this, sg_default, default_body, content);
      }
    }
#endif
  return static_cast<int>(Changed);
}

//  Yuxin: Feb/12/2019
//  Currently only add for for-loops with labels
int CSageCodeGen::add_label_scope(void *sg_loop) {
  void *parent = GetParent(sg_loop);
  if ((parent != nullptr) && (IsLabelStatement(parent) != 0)) {
    SgLabelStatement *label = isSgLabelStatement(static_cast<SgNode *>(parent));
    void *bb = CreateBasicBlock();
    void *copy_stmt = CopyStmt(sg_loop);
    vector<void *> old_decls;
    vector<void *> new_decls;
    GetNodesByType_int(sg_loop, "preorder", V_SgVariableDeclaration,
                       &old_decls);
    GetNodesByType_int(copy_stmt, "preorder", V_SgVariableDeclaration,
                       &new_decls);
    for (size_t j = 0; j < old_decls.size(); j++) {
      void *new_init = GetVariableInitializedName(new_decls[j]);
      void *old_init = GetVariableInitializedName(old_decls[j]);
      replace_variable_references_in_scope(old_init, new_init, copy_stmt);
    }
    AppendChild(bb, copy_stmt);
    string label_str = GetLabelName(label);
    SgLabelStatement *ls = static_cast<SgLabelStatement *>(
        CreateLabelStmt(label_str, static_cast<SgStatement *>(bb),
                        SageInterface::getEnclosingProcedure(label), label));
    ReplaceStmt(label, ls);
    return 1;
  }
  return 0;
}

//  if add_number_suffix = tree (default value), the function name will has
//  suffix of _#, and automatically numbering will make sure no name conflict
//  else if add_number_suffix = false, the caller will ensure no name conflict.
//  when conflict happens, it return null for call and decl
void CSageCodeGen::function_outline(void *node_stmt, void **func_call,
                                    void **func_decl,
                                    const string &func_name_prefix,
                                    bool add_number_suffix,
                                    bool remove_dead_code, bool full,
                                    bool keep_orig_stmt, bool keep_global_var) {
  if (remove_dead_code) {
    dead_stmts_removal(node_stmt);
  }

  vector<void *> vec_refs;
  //  YX: 20170711 Bug1437 apply value propagation to remove unused ports
  GetNodesByType_int(node_stmt, "preorder", V_SgVarRefExp, &vec_refs);
  if (!full) {
    for (auto ref : vec_refs) {
      printf("ref = %s\n", UnparseToString(ref).c_str());
      void *var_init = GetVariableInitializedName(ref);
      if (var_init == nullptr) {
        continue;
      }
      if ((IsArgumentInitName(var_init) != 0) ||
          (IsGlobalInitName(var_init) != 0)) {
        continue;
      }
      void *var_decl = GetVariableDecl(var_init);
      if (is_located_in_scope_simple(var_decl, node_stmt) != 0) {
        continue;
      }
      void *var_type = GetTypebyVar(var_init);
      if (IsArrayType(var_type) != 0) {
        continue;
      }
      if (is_lvalue(ref) != 0) {
        continue;
      }
      vector<void *> defs = GetVarDefbyPosition(var_init, ref);
      if (defs.size() != 1) {
        continue;
      }
      void *def_exp = defs[0];
      void *val_exp = nullptr;
      if (GetInitializer(var_init) == def_exp) {
        val_exp = GetInitializerOperand(var_init);
      } else if (IsAssignOp(def_exp) != 0) {
        val_exp = GetExpRightOperand(def_exp);
      }
      if (nullptr == val_exp) {
        continue;
      }
      CMarsExpression me(val_exp, this);
      if (me.IsConstant() != 0) {
        ReplaceExp(ref, me.get_expr_from_coeff());
        continue;
      }
      if (IsVarRefExp(val_exp) != 0) {
        if (GetVariableInitializedName(val_exp) == var_init) {
          continue;
        }
      }
      if ((is_movable_test(val_exp, ref) != 0) &&
          (is_located_in_scope_simple(val_exp, node_stmt) == 0)) {
        ReplaceExp(ref, CopyExp(val_exp));
        continue;
      }
    }
  }

  GetNodesByType_int(node_stmt, "preorder", V_SgVarRefExp, &vec_refs);

  map<void *, set<void *>> var2refs;
  for (auto ref : vec_refs) {
    void *var_init = GetVariableInitializedName(ref);
    DEBUG(printf("ref = %s, init = %s\n", UnparseToString(ref).c_str(),
                 UnparseToString(var_init).c_str()));
    var2refs[var_init].insert(ref);
  }

  set<void *> ports;
  map<void *, enum access_type> port2access_type;
  map<void *, vector<void *>> port2offset_indices;
  set<string> var_names;

  vector<void *> vec_decls;
  GetNodesByType_int(node_stmt, "preorder", V_SgVariableDeclaration,
                     &vec_decls);
  for (auto var : vec_decls) {
    var_names.insert(GetVariableName(var));
  }
  //  collect port information
  for (auto var_ref_pair : var2refs) {
    auto var = var_ref_pair.first;
    auto refs = var_ref_pair.second;
    void *var_decl = GetVariableDecl(var);
    if ((is_located_in_scope(var_decl, node_stmt) != 0) ||
        IsMemberVariable(var)) {
      continue;
    }

    if (keep_global_var && (IsGlobalInitName(var) != 0)) {
      continue;
    }

    ports.insert(var);

    //  analyze read/write access

    int write = 0;
    for (auto ref : refs) {
      write |= is_write_conservative(ref, false);
    }
    port2access_type[var] = static_cast<enum access_type>(write << 1);
  }

  vector<void *> vec_parameters;
  map<void *, void *> origport2newport;
  set<void *> pointer_ports;
  vector<void *> vec_args;
  for (auto port : ports) {
    void *type = GetTypebyVar(port);
    string var_name = GetVariableName(port);
    string new_name = var_name;
    int suffix = 0;
    while (var_names.count(new_name) > 0) {
      new_name = var_name + "_" + my_itoa(suffix);
      suffix++;
    }
    void *arg = CreateVariableRef(port);
    void *new_type = type;
    if ((port2access_type[port] & WRITE) != 0) {
      new_type = CreatePointerType(new_type);
      pointer_ports.insert(port);
      arg = CreateExp(V_SgAddressOfOp, arg);
    }
    void *parameter = CreateVariable(new_type, new_name);
    if (IsScalarType(type) != 0) {
      vec_args.insert(vec_args.end(), arg);
      vec_parameters.insert(vec_parameters.end(), parameter);
    } else {
      vec_args.insert(vec_args.begin(), arg);
      vec_parameters.insert(vec_parameters.begin(), parameter);
    }
    origport2newport[port] = parameter;
  }

  string new_func_name = "merlin_outline_";

  //  ZP: 20170530
  if (!func_name_prefix.empty()) {
    new_func_name = func_name_prefix;
  }

  //  YX: 20170608
  new_func_name = legalizeName(new_func_name);

  if (add_number_suffix) {
    int i = 0;
    do {
      if (GetFuncDeclByName(new_func_name + my_itoa(i), 1) == nullptr) {
        break;
      }
      ++i;
    } while (true);

    new_func_name += my_itoa(i);
  } else {
    if (GetFuncDeclByName(new_func_name, 1) != nullptr) {
      cout << "ERROR: function name conflict in outline." << endl;
      *func_decl = nullptr;
      *func_call = nullptr;
      return;
    }
  }

  void *new_func = CreateFuncDecl("void", new_func_name, vec_parameters,
                                  GetGlobal(node_stmt), true, nullptr);
  void *caller = TraceUpToFuncDecl(node_stmt);

  void *prev_stmt = caller;
  while (IsPragmaDecl(GetPreviousStmt(prev_stmt)) != 0) {
    prev_stmt = GetPreviousStmt(prev_stmt);
  }
  InsertStmt(new_func, prev_stmt);

  *func_decl = new_func;
  void *copy_stmt = CopyStmt(node_stmt);
  if (IsBasicBlock(copy_stmt) != 0) {
    SetFuncBody(new_func, copy_stmt);
  } else {
    AppendChild(GetFuncBody(new_func), copy_stmt);
  }

  //  create new function call
  {
    void *scope = TraceUpToScope(GetParent(node_stmt));
    *func_call =
        CreateFuncCall(new_func_name, GetTypeByString("void"), vec_args, scope);
    void *func_call_stmt = CreateStmt(V_SgExprStatement, *func_call);
    InsertStmt(func_call_stmt, node_stmt);
  }

  //  replace variable reference
  for (auto port : ports) {
    vector<void *> vec_refs;
    get_ref_in_scope(port, copy_stmt, &vec_refs);
    for (auto ref : vec_refs) {
      void *new_port = origport2newport[port];
      void *new_ref = CreateVariableRef(new_port);
      if (pointer_ports.count(port) > 0) {
        new_ref = CreateExp(V_SgPointerDerefExp, new_ref);
      }
      ReplaceExp(ref, new_ref);
    }
  }

  //  remove original statement
  if (!keep_orig_stmt) {
    RemoveStmt(node_stmt);
  }
}

CNameConflictSolver::CNameConflictSolver(CMarsAST_IF *ast /* = nullptr*/,
                                         void *scope /* = nullptr*/) {
  m_record.clear();
  if (ast != nullptr) {
    if (scope == nullptr) {
      scope = ast->GetProject();
    }

    //  1. get all the variable names in the scope
    vector<void *> vec_ref;
    ast->GetNodesByType_int(scope, "preorder", V_SgVarRefExp, &vec_ref);
    for (auto ref : vec_ref) {
      m_record.insert(ast->_up(ref));
    }
    {
      vector<void *> vec_ref;
      ast->GetNodesByType_int(scope, "preorder", V_SgInitializedName, &vec_ref);
      for (auto ref : vec_ref) {
        m_record.insert(ast->_up(ref));
      }
    }

    //  2. get all the function declaration names
    vector<void *> vec_func;
    ast->GetNodesByType_int(scope, "preorder", V_SgFunctionDeclaration,
                            &vec_func);
    for (auto decl : vec_func) {
      m_record.insert(ast->GetFuncName(decl));
    }
  }
}

string CNameConflictSolver::get_and_assign(const string &prefix) {
  if (m_record.count(prefix) == 0U) {
    m_record.insert(prefix);
    return prefix;
  }
  int suffix = 1;
  while (suffix < 1e28) {
    string str_cand = prefix + my_itoa(suffix);
    if (m_record.count(str_cand) == 0U) {
      m_record.insert(str_cand);
      return str_cand;
    }

    suffix++;
  }

  return prefix + "__too_many_name_conflict";
}

string CSageCodeGen::GetSafeNewVarName(const string &prefix, void *scope) {
  CNameConflictSolver solver(this, scope);
  return solver.get_and_assign(prefix);
}

void CSageCodeGen::set_kernel_attribute(void *func_decl, const string &attr,
                                        const string &val) {
  if (val.empty()) {
    AddDirectives("\n __attribute__ (( " + attr + "))\n", func_decl);
  } else {
    AddDirectives("\n __attribute__ (( " + attr + "(" + val + ")))\n",
                  func_decl);
  }
}

bool CSageCodeGen::dead_stmts_removal(void *scope) {
  if (IsFunctionDeclaration(scope) != 0) {
    scope = GetFuncBody(scope);
  }

  if ((scope == nullptr) || (IsScopeStatement(scope) == 0) ||
      GetChildStmtNum(scope) <= 0 || nullptr != TraceUpToExpression(scope)) {
    return false;
  }

  void *func = TraceUpToFuncDecl(scope);
  if (func == nullptr) {
    return false;
  }

  //  split SgExprListExp into multiple SgExprStatement
  vector<void *> vec_stmts;
  GetNodesByType_int(scope, "preorder", V_SgStatement, &vec_stmts, true);
  bool Changed = true;
  for (auto stmt : vec_stmts) {
    if (IsExprStatement(stmt) == 0) {
      continue;
    }
    void *expr = GetExprFromStmt(stmt);
    if (IsExprList(expr) == 0) {
      continue;
    }
    vector<void *> vec_expr;
    GetExprList(expr, &vec_expr);
    if (vec_expr.size() <= 1) {
      continue;
    }
    for (auto sub_expr : vec_expr) {
      void *new_sub_stmt = CreateStmt(V_SgExprStatement, CopyExp(sub_expr));
      InsertStmt(new_sub_stmt, stmt);
    }
    RemoveStmt(stmt);
  }

  bool LocalChanged = false;
  auto vars_used_in_pragma = GetVariablesUsedByPragmaInScope(scope, true);
  BuildPragmaCache();
  do {
    LocalChanged = false;

    //  collect all variables with read references
    vector<void *> vec_refs;
    GetNodesByType_int(scope, "preorder", V_SgVarRefExp, &vec_refs);
    vector<void *> vec_floating_refs;
    get_floating_var_refs(scope, &vec_floating_refs);
    vec_refs.insert(vec_refs.end(), vec_floating_refs.begin(),
                    vec_floating_refs.end());
    set<void *> vars_with_read_refs;
    // FIXME: shall we treat variables used in pragma into read references set?
    auto used_vars = vars_used_in_pragma;
    for (auto ref : vec_refs) {
      void *var_init = GetVariableInitializedName(ref);
      if (has_read_conservative(ref, false) != 0) {
        vars_with_read_refs.insert(var_init);
      }
      used_vars.insert(var_init);
    }
    vector<void *> vec_stmts;
    set<void *> unused_pragmas;
    GetNodesByType_int(scope, "preorder", V_SgStatement, &vec_stmts, true);

    for (auto stmt : vec_stmts) {
      if ((is_floating_node(stmt) != 0) || (IsBasicBlock(stmt) != 0) ||
          (IsPragmaDecl(stmt) != 0) || (IsLabelStatement(stmt) != 0) ||
          nullptr != TraceUpToExpression(stmt)) {
        continue;
      }
      //  0 check whether current statement is a part of other statement, such
      //  as condition for if-stmt
      void *blk = GetParent(stmt);
      if (IsBasicBlock(blk) == 0) {
        continue;
      }
      if (IsUnderConditionStmt(blk) &&
          GetChildStmtIdx(blk, stmt) == GetChildStmtNum(blk) - 1) {
        continue;
      }
      if ((IsVariableDecl(stmt) != 0)) {
        void *var_init = GetVariableInitializedName(stmt);
        if (used_vars.count(var_init) > 0)
          continue;
        void *initializer = GetInitializer(GetVariableInitializedName(stmt));
        if (!initializer || !has_side_effect(initializer)) {
#ifdef PROJDEBUG
          cout << "Remove dead stmt " << _up(stmt) << endl;
#endif
          RemoveStmt(stmt);
          auto related_pragma = get_pragma_related_to_variable(var_init);
          unused_pragmas.insert(related_pragma.begin(), related_pragma.end());
          LocalChanged = true;
          Changed = true;
          continue;
        }
      }

      //  1.1 if there is function call or function declaration, we cannot
      //  remove stmt;
      vector<void *> vec_calls;
      GetNodesByType_int(stmt, "preorder", V_SgFunctionCallExp, &vec_calls,
                         true);
      if (!vec_calls.empty()) {
        continue;
      }

      vector<void *> vec_decls;
      GetNodesByType_int(stmt, "preorder", V_SgFunctionDeclaration, &vec_decls,
                         true);
      if (!vec_decls.empty()) {
        continue;
      }

      //  1.2 if there is break statement
      vector<void *> vec_breaks;
      GetNodesByType_int(stmt, "preorder", V_SgBreakStmt, &vec_breaks);
      if (!vec_breaks.empty()) {
        continue;
      }

      //  1.3 if there is continue statement
      vector<void *> vec_continues;
      GetNodesByType_int(stmt, "preorder", V_SgContinueStmt, &vec_continues);
      if (!vec_continues.empty()) {
        continue;
      }

      vector<void *> vec_gotos;
      GetNodesByType_int(stmt, "preorder", V_SgGotoStatement, &vec_gotos);
      if (!vec_gotos.empty()) {
        continue;
      }

      vector<void *> vec_rets;
      GetNodesByType_int(stmt, "preorder", V_SgReturnStmt, &vec_rets);
      if (!vec_rets.empty()) {
        continue;
      }

      vector<void *> vec_cases;
      GetNodesByType_int(stmt, "preorder", V_SgCaseOptionStmt, &vec_cases);
      if (!vec_cases.empty()) {
        continue;
      }

      vector<void *> vec_defs;
      GetNodesByType_int(stmt, "preorder", V_SgDefaultOptionStmt, &vec_defs);
      if (!vec_defs.empty()) {
        continue;
      }

      vector<void *> vec_labels;
      GetNodesByType_int(stmt, "preorder", V_SgLabelStatement, &vec_labels);
      if (!vec_labels.empty()) {
        continue;
      }

      vector<void *> vec_throws;
      GetNodesByType_int(stmt, "preorder", V_SgThrowOp, &vec_throws);
      if (!vec_throws.empty()) {
        continue;
      }

      //  2. check whether there is read-write on the same variables
      vector<void *> vec_refs;
      GetNodesByType_int(stmt, "preorder", V_SgVarRefExp, &vec_refs);
      map<void *, access_type> var2access_type;
      for (auto ref : vec_refs) {
        void *var_init = GetVariableInitializedName(ref);
        int res = (has_write_conservative(ref) << 1) |
                  has_read_conservative(ref, false);
        if (var2access_type.count(var_init) <= 0) {
          var2access_type[var_init] = static_cast<enum access_type>(res);
        } else {
          var2access_type[var_init] =
              static_cast<enum access_type>(res | var2access_type[var_init]);
        }
      }

      bool has_read_write = false;
      for (auto var_info : var2access_type) {
        if (var_info.second == READWRITE) {
          has_read_write = true;
          break;
        }
      }
      if (has_read_write) {
        continue;
      }

      //  3. for read-only, check whether its type is volatile
      //   for write-only, check whether it is traced to interface or
      //                                 it has other read accesses
      bool is_dead = true;
      for (auto var_info : var2access_type) {
        void *var_init = var_info.first;
        void *type = GetTypebyVar(var_init);
        if (var_info.second == READ) {
          if (IsVolatileType(type) != 0) {
            is_dead = false;
            break;
          }
        }

        if (var_info.second == WRITE) {
          if (vars_with_read_refs.count(var_init) > 0) {
            is_dead = false;
            break;
          }

          //  check whether it is defined outside the scope
          if ((IsGlobalInitName(var_init) != 0) ||
              (IsArgumentInitName(var_init) != 0) ||
              ((IsLocalInitName(var_init) != 0) &&
               (has_read_in_scope_fast(var_init, TraceUpToScope(GetVariableDecl(
                                                     var_init))) != 0))) {
            is_dead = false;
            break;
          }
        }
      }
      if (!is_dead) {
        continue;
      }
#ifdef PROJDEBUG
      cout << "Remove dead stmt " << _up(stmt) << endl;
#endif
      RemoveStmt(stmt);
      LocalChanged = true;
      Changed = true;
    }
    vector<void *> vec_unused_pragmas(unused_pragmas.begin(),
                                      unused_pragmas.end());
    for (auto pragma : vec_unused_pragmas) {
      if (is_floating_node(pragma))
        continue;
      RemoveStmt(pragma);
    }
  } while (LocalChanged);
  return Changed;
}

bool CSageCodeGen::remove_dead_parameters(void **func_decl) {
  if (GetFuncParamNum(*func_decl) <= 0) {
    return false;
  }
  void *func_body = GetFuncBody(*func_decl);
  if (func_body == nullptr) {
    return false;
  }
  dead_stmts_removal(func_body);

  auto used_vars_by_pragma = GetVariablesUsedByPragmaInScope(func_body, true);
  vector<void *> sub_calls;
  GetNodesByType_int(func_body, "preorder", V_SgFunctionCallExp, &sub_calls);

  bool Changed = false;
  for (auto one_call : sub_calls) {
    void *sub_decl = GetFuncDeclByCall(one_call, 1);
    if (sub_decl == nullptr) {
      continue;
    }
    Changed |= remove_dead_parameters(&sub_decl);
  }

  vector<void *> new_vec_params;
  map<void *, void *> oldParam2NewParam;
  vector<int> bit_map(GetFuncParamNum(*func_decl), 0);
  int arg_index = 0;
  vector<void *> all_calls;
  GetFuncCallsFromDecl(*func_decl, nullptr, &all_calls);
  for (auto arg : GetFuncParams(*func_decl)) {
    bool not_dead_argument = false;
    for (auto call : all_calls) {
      if (has_side_effect(GetFuncCallParam(call, arg_index)) != 0) {
        not_dead_argument = true;
        break;
      }
    }
    vector<void *> vec_refs;
    get_ref_in_scope(arg, func_body, &vec_refs);
    if (!vec_refs.empty() || not_dead_argument ||
        used_vars_by_pragma.count(arg) > 0) {
      new_vec_params.push_back(arg);
      bit_map[arg_index] = 1;
    }
    ++arg_index;
  }
  if (new_vec_params.size() ==
      static_cast<size_t>(GetFuncParamNum(*func_decl))) {
    return Changed;
  }

  for (size_t i = 0; i < new_vec_params.size(); ++i) {
    void *old_param = new_vec_params[i];
    void *new_param =
        CreateVariable(GetTypebyVar(old_param), GetVariableName(old_param));
    oldParam2NewParam[old_param] = new_param;
    new_vec_params[i] = new_param;
  }

  void *new_func_decl =
      CreateFuncDecl(GetFuncReturnType(*func_decl), GetFuncName(*func_decl),
                     new_vec_params, GetGlobal(*func_decl), true, nullptr);
  if (IsStatic(*func_decl))
    SetStatic(new_func_decl);
  void *copy_func_body = CopyStmt(func_body);
  SetFuncBody(new_func_decl, copy_func_body);

  for (auto pair : oldParam2NewParam) {
    replace_variable_references_in_scope(pair.first, pair.second,
                                         copy_func_body);
  }

  //  update function calls
  for (auto one_call : all_calls) {
    vector<void *> vec_args;
    for (int i = 0; i < GetFuncCallParamNum(one_call); ++i) {
      if (bit_map[i] != 0) {
        vec_args.push_back(CopyExp(GetFuncCallParam(one_call, i)));
      }
    }
    void *new_call =
        CreateFuncCall(new_func_decl, vec_args, GetScope(one_call));
    ReplaceExp(one_call, new_call);
  }

  //  update function declarations
  vector<void *> vec_func_decls;
  GetNodesByType_int(nullptr, "preorder", V_SgFunctionDeclaration,
                     &vec_func_decls);
  for (auto one_decl : vec_func_decls) {
    if (GetFuncBody(one_decl) != nullptr) {
      continue;
    }
    if (!isSameFunction(one_decl, *func_decl)) {
      continue;
    }
    //  youxiang: 20160923 bug639: do not touch header file
    if (isWithInHeaderFile(one_decl)) {
      continue;
    }
    void *new_decl = CloneFuncDecl(new_func_decl, GetGlobal(one_decl), false);
    ReplaceStmt(one_decl, new_decl);
  }
  ReplaceStmt(*func_decl, new_func_decl);
  *func_decl = new_func_decl;
  return true;
}

void CSageCodeGen::replace_with_dummy_decl(void *stmt) {
  vector<string> empty;
  string s_name = _up(stmt, 30);
  s_name = legalizeName(s_name);
  void *new_decl = CreateFuncDecl("void", "__merlin_dummy_" + s_name, empty,
                                  empty, GetGlobal(stmt), false, nullptr);
  SetStatic(new_decl);
  ReplaceStmt(stmt, new_decl, true);
}

void *CSageCodeGen::copy_loop_for_c_syntax(void *sg_for_loop, void *sg_scope) {
  SgForInitStatement *init_stmt =
      static_cast<SgForInitStatement *>(GetLoopInit(sg_for_loop));
  SgVariableDeclaration *sg_decl_stmt =
      isSgVariableDeclaration(init_stmt->get_init_stmt()[0]);
  if (isWithInCppFile(sg_for_loop) && (sg_decl_stmt != nullptr)) {
    void *new_loop = CopyStmt(sg_for_loop);
    DiscardPreprocInfo(new_loop);

    void *loop_body = GetLoopBody(new_loop);
    ReplaceStmt(loop_body, CreateBasicBlock());
    return new_loop;
  }

  void *iv;
  void *lb;
  void *ub;
  void *step;
  void *body;
  bool inc_dir;
  bool inclusive_bound;
  int64_t nStep = 0;
  bool ret = IsCanonicalForLoop(sg_for_loop, &iv, &lb, &ub, &step, &body,
                                &inc_dir, &inclusive_bound) != 0;
  ret &= GetLoopStepValueFromExpr(step, &nStep);
  if (!ret || (std::abs(nStep) == 0)) {
    void *new_loop = CopyStmt(sg_for_loop);
    DiscardPreprocInfo(new_loop);

    cout << "[isNonCanonical] " << UnparseToString(sg_for_loop) << endl;
    return new_loop;
  }

  void *func = GetEnclosingNode("Function", sg_for_loop);
  void *func_body = GetFuncBody(func);
  void *old_iter = GetLoopIterator(sg_for_loop);
  void *sg_type = GetTypebyVar(old_iter);

  string new_var_name = _up(old_iter);
  get_valid_local_name(func, &new_var_name);
  void *new_iter_decl =
      CreateVariableDecl(sg_type, new_var_name, nullptr,
                         sg_scope != nullptr ? sg_scope : func_body);
  if (sg_scope != nullptr) {
    PrependChild(sg_scope, new_iter_decl);
  } else {
    PrependChild(func_body, new_iter_decl);
  }
  void *new_iter = GetVariableInitializedName(new_iter_decl);

  CMarsRangeExpr mr = CMarsVariable(sg_for_loop, this, func).get_range();
  CMarsExpression me_lb;
  CMarsExpression me_ub;
  int ret1 = mr.get_simple_bound(&me_lb, &me_ub);
  assert(ret1);
  void *start_exp = CopyExp(me_lb.get_expr_from_coeff());
  void *end_exp = CopyExp(me_ub.get_expr_from_coeff());
  void *new_loop = CreateStmt(V_SgForStatement, new_iter, start_exp, end_exp,
                              CreateBasicBlock(), &nStep, nullptr, sg_for_loop);

  return new_loop;
}

bool CSageCodeGen::replace_sizeof(void *scope) {
  vector<void *> vec_sizeof;
  GetNodesByType_int(scope, "preorder", V_SgSizeOfOp, &vec_sizeof);
  std::reverse(vec_sizeof.begin(), vec_sizeof.end());
  bool Changed = false;
  for (auto &sizeof_op : vec_sizeof) {
    if (GetParent(sizeof_op) == nullptr) {
      continue;
    }
    if ((IsFromInputFile(sizeof_op) == 0)) {
      continue;
    }
    void *value = get_sizeof_value(sizeof_op);
    if (value != nullptr) {
      ReplaceExp(sizeof_op, CopyExp(value));
      Changed = true;
    }
  }
  return Changed;
}

void *CSageCodeGen::GetFirstNonTrivalStmt(void *blk) {
  int num_child = GetChildStmtNum(blk);
  if (num_child <= 0) {
    return nullptr;
  }
  for (int i = 0; i < num_child; ++i) {
    void *stmt = GetChildStmt(blk, i);
    //    if (IsPragmaDecl(stmt) || IsLabelStatement(stmt))
    if (IsPragmaDecl(stmt) != 0) {
      continue;
    }
    return stmt;
  }
  return nullptr;
}

void *CSageCodeGen::GetLastNonTrivalStmt(void *blk) {
  int num_child = GetChildStmtNum(blk);
  if (num_child <= 0) {
    return nullptr;
  }
  for (int i = num_child - 1; i >= 0; --i) {
    void *stmt = GetChildStmt(blk, i);
    if ((IsPragmaDecl(stmt) != 0) || (IsLabelStatement(stmt) != 0)) {
      continue;
    }
    return stmt;
  }
  return nullptr;
}

void *CSageCodeGen::get_call_in_path(void *func_decl,
                                     const t_func_call_path &call_path) {
  for (auto func_call : call_path) {
    if (func_call.first == func_decl) {
      return func_call.second;
    }
  }
  return nullptr;
}

void CSageCodeGen::splitString(string str, const string &delim,
                               vector<string> *results) {
  size_t cutAt;
  while ((cutAt = str.find_first_of(delim)) != std::string::npos) {
    if (cutAt > 0) {
      results->push_back(str.substr(0, cutAt));
    }
    str = str.substr(cutAt + 1);
  }

  if (str.length() > 0) {
    results->push_back(str);
  }
}

//  Replace a given expression with a list of statements produced by a
//  generator.  The generator, when given a variable as input, must produce
//  some code which leaves its result in the given variable.  The output
//  from the generator is then inserted into the original program in such a
//  way that whenever the expression had previously been evaluated, the
//  statements produced by the generator are run instead and their result is
//  used in place of the expression.
//  Assumptions: not currently traversing from or the statement it is in
void CSageCodeGen::replaceExpressionWithStatement(
    SgExpression *from, SageInterface::StatementGenerator *to) {
  pushExpressionIntoBody(from);
  replaceSubexpressionWithStatement(from, to);

  //  printf("Leaving replaceExpressionWithStatement(from,to) \n");
}

void CSageCodeGen::pushExpressionIntoBody(SgExpression *from) {
  //  DQ (3/11/2006): The problem here is that the test expression for a "for
  //  loop" (SgForStmt) is assumed to be a SgExpression.  This was changed in
  //  Sage III as part of a bugfix and so the original assumptions upon which
  //  this function was based are not incorrect, hence the bug! Note that a
  //  number of cases were changed when this fix was made to SageIII (see
  //  documentation for SgScopeStatement).

  SgStatement *enclosingStatement = getStatementOfExpression(from);
  SgExprStatement *exprStatement = isSgExprStatement(enclosingStatement);

  SgForStatement *forStatement = isSgForStatement(enclosingStatement);
  SgReturnStmt *returnStatement = isSgReturnStmt(enclosingStatement);
  SgVariableDeclaration *varDeclarationStatement =
      isSgVariableDeclaration(enclosingStatement);

  //  DQ (3/11/2006): Bugfix for special cases of conditional that are either
  //  SgStatement or SgExpression IR nodes.

  ROSE_ASSERT(exprStatement || forStatement || returnStatement ||
              varDeclarationStatement);

  if (varDeclarationStatement != nullptr) {
  } else {
    SgExpression *root = getRootOfExpression(from);
    ROSE_ASSERT(root);
    //  printf("root = %p \n",root);
    {
      if ((forStatement != nullptr) && forStatement->get_increment() == root) {
        //  printf("Convert step of for statement \n");
        //  Convert step of for statement
        //  for (init; test; e) body; (where e contains from) becomes
        //  for (init; test; ) {
        //   body (with "continue" changed to "goto label");
        //   label: e;
        //  }
        //  std::cout << "Converting for step" << std::endl;
        detachFromParent(root);
        SgExprStatement *incrStmt =
            isSgExprStatement(static_cast<SgNode *>(CreateStmt(
                V_SgExprStatement, forStatement->get_increment(), from)));
        forStatement->get_increment()->set_parent(incrStmt);

        SageInterface::addStepToLoopBody(forStatement, incrStmt);
        SgNullExpression *ne = isSgNullExpression(
            static_cast<SgNode *>(CreateExp(V_SgNullExpression, from)));
        forStatement->set_increment(ne);
        ne->set_parent(forStatement);
      } else {
        SgStatement *enclosingStmtParent =
            isSgStatement(enclosingStatement->get_parent());
        assert(enclosingStmtParent);
        SgWhileStmt *whileStatement = isSgWhileStmt(enclosingStmtParent);
        SgDoWhileStmt *doWhileStatement = isSgDoWhileStmt(enclosingStmtParent);
        SgIfStmt *ifStatement = isSgIfStmt(enclosingStmtParent);
        SgSwitchStatement *switchStatement =
            isSgSwitchStatement(enclosingStmtParent);
        SgForStatement *enclosingForStatement =
            isSgForStatement(enclosingStmtParent);
        if ((enclosingForStatement != nullptr) &&
            enclosingForStatement->get_test() == exprStatement) {
          //  printf("Found the test in the for loop \n");
          //  ROSE_ASSERT(false);

          //  Convert test of for statement:
          //  for (init; e; step) body; (where e contains from) becomes
          //  for (init; true; step) {
          //   bool temp;
          //   temp = e;
          //   if (!temp) break;
          //   body;
          //  }
          //  in which "temp = e;" is rewritten further
          //  std::cout << "Converting for test" << std::endl;
          pushTestIntoBody(enclosingForStatement);
        } else if ((whileStatement != nullptr) &&
                   whileStatement->get_condition() == exprStatement) {
          //  printf("Convert while statements \n");
          //  Convert while statement:
          //  while (e) body; (where e contains from) becomes
          //  while (true) {
          //   bool temp;
          //   temp = e;
          //   if (!temp) break;
          //   body;
          //  }
          //  in which "temp = e;" is rewritten further
          //  std::cout << "Converting while test" << std::endl;
          pushTestIntoBody(whileStatement);
          //  FixSgTree(whileStatement);
        } else if ((doWhileStatement != nullptr) &&
                   doWhileStatement->get_condition() == exprStatement) {
          //  printf("Convert do-while statements \n");
          //  Convert do-while statement:
          //  do body; while (e); (where e contains from) becomes
          //  {bool temp = true;
          //  do {
          //    body (with "continue" changed to "goto label";
          //    label:
          //    temp = e;} while (temp);}
          //  in which "temp = e;" is rewritten further
          //  std::cout << "Converting do-while test" << std::endl;
          detachFromParent(root);
          SgBasicBlock *new_statement = nullptr;
          auto parent = doWhileStatement->get_parent();
          assert(parent);
          bool single_child = IsSingleChild(parent);
          if (single_child) {
            new_statement = isSgBasicBlock(parent);
          } else {
            new_statement = isSgBasicBlock(
                static_cast<SgNode *>(CreateBasicBlock(nullptr, false, from)));
            //  \n",new_statement);
            new_statement->set_parent(doWhileStatement->get_parent());
            myStatementInsert(doWhileStatement, new_statement, false);
            myRemoveStatement(doWhileStatement);
          }
          SgName varname =
              "rose__temp";  //  Does not need to be unique, but must
          //  not be used in user code anywhere
          SgAssignInitializer *assignInitializer =
              SageBuilder::buildAssignInitializer(
                  SageBuilder::buildBoolValExp(true));
          SgVariableDeclaration *new_decl =
              isSgVariableDeclaration(static_cast<SgNode *>(
                  CreateVariableDecl(SageBuilder::buildBoolType(), varname,
                                     assignInitializer, new_statement, from)));

          SgInitializedName *initname = new_decl->get_variables().back();
          initname->set_scope(new_statement);

          //  DQ (12/14/2006): set the parent of the SgAssignInitializer to the
          //  variable (SgInitializedName).
          assignInitializer->set_parent(initname);

          SgVariableSymbol *varsym = new SgVariableSymbol(initname);
          new_statement->insert_symbol(varname, varsym);
          varsym->set_parent(new_statement->get_symbol_table());
          if (!single_child) {
            AppendChild(new_statement, new_decl);
            AppendChild(new_statement, doWhileStatement);
          } else {
            InsertStmt(new_decl, doWhileStatement);
          }
          assert(varsym);
          SgCastExp *castExp1 =
              SageBuilder::buildCastExp(root, SageBuilder::buildBoolType());
          SgVarRefExp *vr =
              isSgVarRefExp(static_cast<SgNode *>(CreateVariableRef(initname)));
          vr->set_lvalue(true);

          SgExprStatement *temp_setup = isSgExprStatement(static_cast<SgNode *>(
              CreateStmt(V_SgAssignStatement, vr, castExp1)));

          SageInterface::addStepToLoopBody(doWhileStatement, temp_setup);
          SgVarRefExp *varsymVr =
              isSgVarRefExp(static_cast<SgNode *>(CreateVariableRef(initname)));

          SgExprStatement *condStmt = isSgExprStatement(
              static_cast<SgNode *>(CreateStmt(V_SgExprStatement, varsymVr)));
          varsymVr->set_parent(condStmt);
          doWhileStatement->set_condition(condStmt);
          condStmt->set_parent(doWhileStatement);
        } else if ((ifStatement != nullptr) &&
                   ifStatement->get_conditional() == exprStatement) {
          auto parent = ifStatement->get_parent();
          assert(parent);
          detachFromParent(root);
          bool is_single_child = IsSingleChild(ifStatement);
          SgBasicBlock *new_statement;
          if (is_single_child) {
            new_statement = isSgBasicBlock(parent);
          } else {
            new_statement = isSgBasicBlock(
                static_cast<SgNode *>(CreateBasicBlock(nullptr, false, from)));
            //  printf("Building IR node #15: new SgBasicBlock = %p
            //  \n",new_statement);
            new_statement->set_parent(ifStatement->get_parent());
            myStatementInsert(ifStatement, new_statement, false);
            myRemoveStatement(ifStatement);
          }
          SgName varname =
              "rose__temp";  //  Does not need to be unique, but must
          //  not be used in user code anywhere
          SgBoolValExp *trueVal = SageBuilder::buildBoolValExp(true);

          SgAssignInitializer *ai =
              SageBuilder::buildAssignInitializer(trueVal);

          SgVariableDeclaration *new_decl =
              isSgVariableDeclaration(static_cast<SgNode *>(
                  CreateVariableDecl(SageBuilder::buildBoolType(), varname, ai,
                                     new_statement, from)));
          SgInitializedName *initname = new_decl->get_variables().back();
          ai->set_parent(initname);
          initname->set_scope(new_statement);
          SgVariableSymbol *varsym = new SgVariableSymbol(initname);
          new_statement->insert_symbol(varname, varsym);
          varsym->set_parent(new_statement->get_symbol_table());
          if (!is_single_child)
            AppendChild(new_statement, new_decl);
          else
            InsertStmt(new_decl, ifStatement);
          SgCastExp *castExp2 = static_cast<SgCastExp *>(
              CreateCastExp(root, SageBuilder::buildBoolType(), exprStatement));
          SgVarRefExp *vr =
              isSgVarRefExp(static_cast<SgNode *>(CreateVariableRef(initname)));
          vr->set_lvalue(true);
          void *temp_setup = CreateStmt(V_SgAssignStatement, vr, castExp2);
          if (!is_single_child)
            AppendChild(new_statement, temp_setup);
          else
            InsertStmt(temp_setup, ifStatement);
          if (!is_single_child)
            AppendChild(new_statement, ifStatement);
          SgVarRefExp *vr2 =
              isSgVarRefExp(static_cast<SgNode *>(CreateVariableRef(initname)));
          SgExprStatement *es = isSgExprStatement(
              static_cast<SgNode *>(CreateStmt(V_SgExprStatement, vr2)));
          ifStatement->set_conditional(es);
          es->set_parent(ifStatement);
        } else if ((switchStatement != nullptr) &&
                   switchStatement->get_item_selector() == exprStatement) {
          SgExpression *switchCond = exprStatement->get_expression();
          ROSE_ASSERT(switchCond);
          detachFromParent(switchCond);
          auto parent = switchStatement->get_parent();
          assert(parent);
          bool is_single_child = IsSingleChild(switchStatement);
          SgBasicBlock *new_statement;
          if (is_single_child) {
            new_statement = isSgBasicBlock(parent);
          } else {
            new_statement = isSgBasicBlock(
                static_cast<SgNode *>(CreateBasicBlock(nullptr, false, from)));
            //  printf("Building IR node #15: new SgBasicBlock = %p
            //  \n",new_statement);
            new_statement->set_parent(switchStatement->get_parent());
            myStatementInsert(switchStatement, new_statement, false);
            myRemoveStatement(switchStatement);
          }
          SgName varname =
              "rose__temp";  //  Does not need to be unique, but must
          //  not be used in user code anywhere
          SgVariableDeclaration *new_decl =
              isSgVariableDeclaration(static_cast<SgNode *>(CreateVariableDecl(
                  switchCond->get_type(), varname,
                  SageBuilder::buildAssignInitializer(switchCond),
                  new_statement, from)));
          if (!is_single_child)
            AppendChild(new_statement, new_decl);
          else
            InsertStmt(new_decl, switchStatement);
          if (!is_single_child)
            AppendChild(new_statement, switchStatement);
          SgVarRefExp *vr2 =
              isSgVarRefExp(static_cast<SgNode *>(CreateVariableRef(new_decl)));
          SgExprStatement *es = isSgExprStatement(
              static_cast<SgNode *>(CreateStmt(V_SgExprStatement, vr2, from)));
          switchStatement->set_item_selector(es);
          es->set_parent(switchStatement);
        } else {
          //  printf("Handles expression and return statements \n");
          //  Handles expression and return statements
          //  std::cout << "Converting other statement" << std::endl;
        }
      }
    }
  }
}
//  ! Replace a given expression with a list of statements produced by a
//  ! generator.  The generator, when given a variable as input, must produce
//  ! some code which leaves its result in the given variable.  The output
//  ! from the generator is then inserted into the original program in such a
//  ! way that whenever the expression had previously been evaluated, the
//  ! statements produced by the generator are run instead and their result is
//  ! used in place of the expression.
//  ! Assumptions: not currently traversing from or the statement it is in

//  Similar to replaceExpressionWithStatement, but with more restrictions.
//  Assumptions: from is not within the test of a loop or if
//              not currently traversing from or the statement it is in
void CSageCodeGen::replaceSubexpressionWithStatement(
    SgExpression *from, SageInterface::StatementGenerator *to) {
  SgStatement *stmt = getStatementOfExpression(from);

  if (isSgExprStatement(stmt) != nullptr) {
    SgExpression *top = getRootOfExpression(from);
    if ((isSgAssignOp(top) != nullptr) &&
        isSgAssignOp(top)->get_rhs_operand() == from) {
      SgAssignOp *t = isSgAssignOp(top);
      SgStatement *generated = to->generate(t->get_lhs_operand());
      ReplaceStmt(stmt, generated);
      return;
    }
  }

  SgAssignInitializer *init =
      isSgAssignInitializer(static_cast<SgNode *>(splitExpression(from)));
  convertInitializerIntoAssignment(init);
  SgExprStatement *new_stmt = isSgExprStatement(getStatementOfExpression(from));
  assert(new_stmt !=
         nullptr);  //  Should now have this form because of conversion
  replaceAssignmentStmtWithStatement(new_stmt, to);
}

void CSageCodeGen::replaceAssignmentStmtWithStatement(
    SgExprStatement *from, SageInterface::StatementGenerator *to) {
  assert(from);
  assert(to);
  //  SgAssignOp* assignment =
  //  isSgAssignOp(from->get_expression_root()->get_operand_i());
  SgAssignOp *assignment = isSgAssignOp(from->get_expression());
  assert(assignment);
  SgVarRefExp *var = isSgVarRefExp(assignment->get_lhs_operand());
  assert(var);
  SgStatement *replacement = to->generate(var);
  myStatementInsert(from, replacement, false);
  myRemoveStatement(from);
}

//  Rewrites a while or for loop so that the official test is changed to
//  "true" and what had previously been the test is now an if-break
//  combination (with an inverted condition) at the beginning of the loop
//  body
//
//  For example, "while (a < 5) ++a;" becomes:
//  "while (true) {bool temp = (a < 5); if (!temp) break; ++a;}"
//  "for (init; test; step) body;" becomes:
//  "for (init; true; step) {bool temp = test; if (!temp) break; body;}"
//  "do body; while (test);" becomes:
//  "do {body; bool temp = test; if (!temp) break;} while (true);"
void CSageCodeGen::pushTestIntoBody(SgScopeStatement *loopStmt) {
  AstPostProcessing(loopStmt);
  SgBasicBlock *new_body = isSgBasicBlock(
      static_cast<SgNode *>(CreateBasicBlock(nullptr, false, loopStmt)));
  SgStatement *old_body = SageInterface::getLoopBody(loopStmt);
  SageInterface::setLoopBody(loopStmt, new_body);
  new_body->set_parent(loopStmt);
  AstPostProcessing(loopStmt);
  SgStatement *cond = SageInterface::getLoopCondition(loopStmt);
  ROSE_ASSERT(isSgExprStatement(cond));
  SgExpression *root = isSgExprStatement(cond)->get_expression();
  detachFromParent(root);
  SgCastExp *cast = static_cast<SgCastExp *>(
      CreateCastExp(root, SageBuilder::buildBoolType(), root));
  //  Name does not need to be unique, but must not be used in user code
  //  anywhere
  AstPostProcessing(loopStmt);
  SgVariableDeclaration *new_decl =
      isSgVariableDeclaration(static_cast<SgNode *>(CreateVariableDecl(
          SageBuilder::buildBoolType(), "rose__temp",
          SageBuilder::buildAssignInitializer(cast), new_body, loopStmt)));

  AppendChild(new_body, new_decl);
  AstPostProcessing(loopStmt);
  SgIfStmt *loop_break = isSgIfStmt(static_cast<SgNode *>(CreateIfStmt(
      CreateExp(V_SgNotOp, CreateVariableRef(new_decl), loopStmt),
      CreateBasicBlock(SageBuilder::buildBreakStmt(), false, loopStmt), nullptr,
      loopStmt)));
  AppendChild(new_body, loop_break);
  AstPostProcessing(loopStmt);
  if (isSgDoWhileStmt(loopStmt) != nullptr) {
    SgName label = "rose_test_label__";
    static int gensym_counter = 0;
    label << ++gensym_counter;
    SgLabelStatement *ls = static_cast<SgLabelStatement *>(CreateLabelStmt(
        label, loopStmt, SageInterface::getEnclosingProcedure(loopStmt),
        loopStmt));
    SageInterface::changeContinuesToGotos(old_body, ls);
    //  Note that these two insertions are backwards of the resulting statement
    //  order
    PrependChild(new_body, ls);
    PrependChild(new_body, old_body);
  } else {
    AppendChild(new_body, old_body);
  }
  AstPostProcessing(loopStmt);
  SageInterface::setLoopCondition(
      loopStmt,
      isSgStatement(static_cast<SgNode *>(CreateStmt(
          V_SgExprStatement, SageBuilder::buildBoolValExp(true), loopStmt))));
  AstPostProcessing(loopStmt);
}

//  Get the expression root of an expression.  Needs to be moved.
SgExpression *CSageCodeGen::getRootOfExpression(SgExpression *n) {
  ROSE_ASSERT(n);

  SgNode *parent = n->get_parent();
  while (isSgExpression(parent) != nullptr) {
    n = isSgExpression(parent);
    parent = n->get_parent();
    ROSE_ASSERT(n != nullptr);
  }

  return n;
}

void CSageCodeGen::myStatementInsert(SgStatement *target, SgStatement *newstmt,
                                     bool before, bool allowForInit) {
  ROSE_ASSERT(target != nullptr);
  ROSE_ASSERT(newstmt != nullptr);
  SgStatement *parent = isSgStatement(target->get_parent());
  if ((isSgIfStmt(parent) != nullptr) &&
      isSgIfStmt(parent)->get_conditional() == target) {
    target = parent;
    parent = isSgScopeStatement(target->get_parent());
  }

  //  printf("allowForInit = %s \n",allowForInit ? "true" : "false");
  if ((isSgForInitStatement(target->get_parent()) != nullptr) &&
      !allowForInit) {
    target = isSgScopeStatement(target->get_parent()->get_parent());
    parent = isSgScopeStatement(target->get_parent());
    assert(target);
  }

  if ((isSgSwitchStatement(target->get_parent()) != nullptr) &&
      target ==
          isSgSwitchStatement(target->get_parent())->get_item_selector()) {
    target = isSgScopeStatement(target->get_parent()->get_parent());
    parent = isSgScopeStatement(target->get_parent());
    assert(target);
  }

  ROSE_ASSERT(target != nullptr);
  ROSE_ASSERT(parent);
  SgStatementPtrList *siblings_ptr;
  if (isSgForInitStatement(target->get_parent()) != nullptr) {
    siblings_ptr = &isSgForInitStatement(target->get_parent())->get_init_stmt();
  } else {
    assert(parent);
    if (isSgScopeStatement(parent) != nullptr) {
      ROSE_ASSERT(parent != nullptr);
      siblings_ptr = &isSgScopeStatement(parent)->getStatementList();
      parent = isSgStatement(
          target
              ->get_parent());  //  getStatementList might have changed it when
      //  parent was a loop or something similar
      ROSE_ASSERT(parent);
    } else {
      assert(!"Bad parent type");
    }
  }

  ROSE_ASSERT(siblings_ptr != nullptr);
  ROSE_ASSERT(target != nullptr);

  SgStatementPtrList &siblings = *siblings_ptr;
  SgStatementPtrList::iterator stmt_iter =
      std::find(siblings.begin(), siblings.end(), target);
  ROSE_ASSERT(stmt_iter != siblings.end());

  if (before) {
    InsertStmt(newstmt, *stmt_iter);
  } else {
    InsertAfterStmt(newstmt, *stmt_iter);
  }
}

class AndOpGenerator : public SageInterface::StatementGenerator {
  CSageCodeGen *ast;
  SgAndOp *op;
  SgNode *op_lhs;
  SgNode *op_rhs;

 public:
  explicit AndOpGenerator(CSageCodeGen *ast, SgAndOp *op) : ast(ast), op(op) {
    op_lhs = op->get_lhs_operand();
    op_rhs = op->get_rhs_operand();
  }
  virtual SgStatement *generate(SgExpression *lhs) {
    ast->detachFromParent(op_lhs);
    ast->detachFromParent(op_rhs);
    SgIfStmt *tree = static_cast<SgIfStmt *>(ast->CreateIfStmt(
        ast->CreateStmt(V_SgExprStatement, op_lhs, op),
        ast->CreateBasicBlock(
            ast->CreateStmt(V_SgAssignStatement, ast->CopyExp(lhs), op_rhs, op),
            false, op),
        ast->CreateBasicBlock(
            ast->CreateStmt(V_SgAssignStatement, ast->CopyExp(lhs),
                            SageBuilder::buildBoolValExp(false), op),
            false, op)));
    return tree;
  }
  ~AndOpGenerator() {}
};
#if ENABLE_OR_OP_GEN
class OrOpGenerator : public SageInterface::StatementGenerator {
  SgOrOp *op;
  CSageCodeGen *ast;
  SgNode *op_lhs;
  SgNode *op_rhs;

 public:
  explicit OrOpGenerator(CSageCodeGen *ast, SgOrOp *op) : ast(ast), op(op) {
    op_lhs = op->get_lhs_operand();
    op_rhs = op->get_rhs_operand();
  }

  virtual SgStatement *generate(SgExpression *lhs) {
    ast->detachFromParent(op_lhs);
    ast->detachFromParent(op_rhs);
    SgIfStmt *tree = static_cast<SgIfStmt *>(ast->CreateIfStmt(
        ast->CreateStmt(V_SgExprStatement, op_lhs, op),
        ast->CreateBasicBlock(
            ast->CreateStmt(V_SgAssignStatement, ast->CopyExp(lhs),
                            SageBuilder::buildBoolValExp(true), op),
            false, op),
        ast->CreateBasicBlock(ast->CreateStmt(
            V_SgAssignStatement(ast->CopyExp(hls), op_rhs, op), false, op))));
    return tree;
  }
  ~OrOpGenerator() {}
};
#endif
class ConditionalExpGenerator : public SageInterface::StatementGenerator {
  CSageCodeGen *ast;
  SgConditionalExp *op;
  SgNode *cond_exp;
  SgNode *true_exp;
  SgNode *false_exp;

 public:
  ConditionalExpGenerator(CSageCodeGen *ast, SgConditionalExp *op)
      : ast(ast), op(op) {
    cond_exp = op->get_conditional_exp();
    true_exp = op->get_true_exp();
    false_exp = op->get_false_exp();
  }

  virtual SgStatement *generate(SgExpression *lhs) {
    ast->detachFromParent(cond_exp);
    ast->detachFromParent(true_exp);
    ast->detachFromParent(false_exp);
    if (lhs != nullptr) {
      SgIfStmt *tree = isSgIfStmt(static_cast<SgNode *>(ast->CreateIfStmt(
          ast->CreateStmt(V_SgExprStatement,
                          ast->RemoveAPIntFixedCast(cond_exp), op),
          ast->CreateBasicBlock(
              ast->CreateStmt(V_SgAssignStatement,
                              ast->CopyExp(ast->RemoveAPIntFixedCast(lhs)),
                              ast->RemoveAPIntFixedCast(true_exp), op),
              false, op),
          ast->CreateBasicBlock(
              ast->CreateStmt(V_SgAssignStatement,
                              ast->CopyExp(ast->RemoveAPIntFixedCast(lhs)),
                              ast->RemoveAPIntFixedCast(false_exp), op),
              false, op),
          op)));
      return tree;
    }
    SgIfStmt *tree = isSgIfStmt(static_cast<SgNode *>(ast->CreateIfStmt(
        ast->CreateStmt(V_SgExprStatement, ast->RemoveAPIntFixedCast(cond_exp),
                        op),
        ast->CreateBasicBlock(
            ast->CreateStmt(V_SgExprStatement,
                            ast->RemoveAPIntFixedCast(true_exp), op),
            false, op),
        ast->CreateBasicBlock(
            ast->CreateStmt(V_SgExprStatement,
                            ast->RemoveAPIntFixedCast(false_exp), op),
            false, op),
        op)));
    return tree;
  }
  ~ConditionalExpGenerator() {}
};
void CSageCodeGen::ReplaceExpWithStmt(void *sg_old_) {
  if (isSgConditionalExp(static_cast<SgNode *>(sg_old_)) == nullptr) {
    return;
  }
  ConditionalExpGenerator *gen = new ConditionalExpGenerator(
      this, static_cast<SgConditionalExp *>(sg_old_));
  replaceExpressionWithStatement(
      static_cast<SgExpression *>(sg_old_),
      gen);  //  true: move preprocess info like #include
  delete gen;
}

void *CSageCodeGen::splitExpression(void *from_, string newName /* ="" */) {
  SgExpression *from = isSgExpression(static_cast<SgNode *>(from_));
  ROSE_ASSERT(from != nullptr);
  pushExpressionIntoBody(from);
  SgStatement *stmt = getStatementOfExpression(from);
  assert(stmt);
  if (isSgForInitStatement(stmt->get_parent()) == nullptr) {
    //  SageInterface::ensureBasicBlockAsParent(stmt);
    //  no return value is accepted. Only the optional transformation matters
    if (SageInterface::isBodyStatement(stmt) &&
        (isSgBasicBlock(stmt) == nullptr)) {
      SageInterface::makeSingleStatementBodyToBlock(stmt);
    }
  }

  SgScopeStatement *parent = isSgScopeStatement(stmt->get_parent());
  //  cout << "parent is a " << (parent ? parent->sage_class_name() : "nullptr")
  //  << endl;
  if ((parent == nullptr) &&
      (isSgForInitStatement(stmt->get_parent()) != nullptr)) {
    parent = isSgScopeStatement(stmt->get_parent()->get_parent()->get_parent());
  }
  assert(parent);
  //  cout << "parent is a " << parent->sage_class_name() << endl;
  //  cout << "parent is " << parent->unparseToString() << endl;
  //  cout << "stmt is " << stmt->unparseToString() << endl;
  string varname = "rose_temp";
  void *func_decl = TraceUpToFuncDecl(from);
  get_valid_local_name(func_decl, &varname);

  SgType *vartype = from->get_type();
  SgNode *fromparent = from->get_parent();
  //  Fix bug2108
  if (IsInitializer(fromparent) != 0) {
    vartype =
        isSgType(static_cast<SgNode *>(GetTypebyVar(fromparent->get_parent())));
  }
  vector<SgExpression *> ancestors;
  for (SgExpression *expr = from, *anc = isSgExpression(fromparent); anc != 0;
       expr = anc, anc = isSgExpression(anc->get_parent())) {
    if (((isSgAndOp(anc) != nullptr) &&
         expr != isSgAndOp(anc)->get_lhs_operand()) ||
#if ENABLE_OR_OP_GEN
        (isSgOrOp(anc) && expr != isSgOrOp(anc)->get_lhs_operand()) ||
#endif
        ((isSgConditionalExp(anc) != nullptr) &&
         expr != isSgConditionalExp(anc)->get_conditional_exp())) {
      ancestors.push_back(anc);  //  Closest first
    }
  }
  //  cout << "This expression to split has " << ancestors.size() << "
  //  ancestor(s)" << endl;
  for (vector<SgExpression *>::reverse_iterator ai = ancestors.rbegin();
       ai != ancestors.rend(); ++ai) {
    SageInterface::StatementGenerator *gen;
    switch ((*ai)->variantT()) {
    case V_SgAndOp:
      gen = new AndOpGenerator(this, isSgAndOp(*ai));
      break;
#if ENABLE_OR_OP_GEN
    case V_SgOrOp:
      gen = new OrOpGenerator(this, isSgOrOp(*ai));
      break;
#endif
    case V_SgConditionalExp:
      gen = new ConditionalExpGenerator(this, isSgConditionalExp(*ai));
      break;
    default:
      assert(!"Should not happen");
    }
    replaceExpressionWithStatement(*ai, gen);
    delete gen;
  }  //  for
  if (!ancestors.empty()) {
    return splitExpression(from);
    //  Need to recompute everything if there were ancestors
  }
  bool changed = false;
  void *vardecl = CreateVariableDecl(RemoveConstType(vartype, from, &changed),
                                     varname, nullptr, parent, from);
  SgInitializedName *initname = isSgInitializedName(
      static_cast<SgNode *>(GetVariableInitializedName(vardecl)));
  ROSE_ASSERT(initname);
  void *varref = CreateVariableRef(vardecl);
  replaceExpressionWithExpression(from,
                                  isSgVarRefExp(static_cast<SgNode *>(varref)));
  SgAssignInitializer *ai = SageBuilder::buildAssignInitializer(from);
  initname->set_initializer(ai);
  ai->set_parent(initname);
  myStatementInsert(
      stmt, isSgVariableDeclaration(static_cast<SgNode *>(vardecl)), true);
  return ai;
}

//  Replace the expression "from" with another expression "to", wherever it
//  appears in the AST.  The expression "from" is not deleted, and may be
//  reused elsewhere in the AST.
void CSageCodeGen::replaceExpressionWithExpression(SgExpression *from,
                                                   SgExpression *to) {
  SgNode *fromparent = from->get_parent();

  to->set_parent(fromparent);
  if (isSgExprStatement(fromparent) != nullptr) {
    isSgExprStatement(fromparent)->set_expression(to);
  } else if (isSgReturnStmt(fromparent) != nullptr) {
    isSgReturnStmt(fromparent)->set_expression(to);
  } else if (isSgDoWhileStmt(fromparent) != nullptr) {
    ROSE_ASSERT(
        !"FIXME -- this case is present for when the test of a do-while "
         "statement is changed to an expression rather than a statement");
  } else if (isSgForStatement(fromparent) != nullptr) {
    ROSE_ASSERT(isSgForStatement(fromparent)->get_increment() == from);
    isSgForStatement(fromparent)->set_increment(to);
  } else if (isSgExpression(fromparent) != nullptr) {
    //  std::cout << "Unparsed: " << fromparent->sage_class_name() << " --- " <<
    //  from->unparseToString() << std::endl; std::cout << "Unparsed 2: " <<
    //  varref->sage_class_name() << " --- " << varref->unparseToString() <<
    //  std::endl;
    int worked = isSgExpression(fromparent)->replace_expression(from, to);
    ROSE_ASSERT(worked);
  } else {
    ROSE_ASSERT(!"Parent of expression is an unhandled case");
  }
}

//  special purpose remove for AST transformation/optimization from astInliner,
//  don't use it otherwise.
void CSageCodeGen::myRemoveStatement(SgStatement *stmt) {
  //  assert (LowLevelRewrite::isRemovableStatement(*i));
  SgStatement *parent = isSgStatement(stmt->get_parent());
  ROSE_ASSERT(parent);
  SgBasicBlock *bb = isSgBasicBlock(parent);
  SgForInitStatement *fis = isSgForInitStatement(parent);
  if ((bb != nullptr) || (fis != nullptr)) {
    ROSE_ASSERT(bb || fis);
    RemoveStmt(stmt);
  } else {
    ReplaceStmt(stmt, new SgNullStatement(TRANS_FILE));
  }
}

//  Convert something like "int a = foo();" into "int a; a = foo();"
SgAssignOp *
CSageCodeGen::convertInitializerIntoAssignment(SgAssignInitializer *init) {
  //  assert
  //  (SageInterface::isDefaultConstructible(init->get_operand_i()->get_type()));
  SgStatement *stmt = getStatementOfExpression(init);
  assert(stmt);
  SgScopeStatement *parent = isSgScopeStatement(stmt->get_parent());
  if ((parent == nullptr) &&
      (isSgForInitStatement(stmt->get_parent()) != nullptr)) {
    parent = isSgScopeStatement(stmt->get_parent()->get_parent()->get_parent());
  }
  assert(parent);
  SgNode *initparent = init->get_parent();
  assert(initparent);

  SgInitializedName *initname = nullptr;
  if (isSgInitializedName(initparent) != nullptr) {
    initname = isSgInitializedName(initparent);
  } else if (isSgVariableDefinition(initparent) != nullptr) {
    initname = isSgVariableDefinition(initparent)->get_vardefn();
  } else if (isSgVariableDeclaration(initparent) != nullptr) {
    SgInitializedNamePtrList &vars =
        isSgVariableDeclaration(initparent)->get_variables();
    for (SgInitializedNamePtrList::iterator i = vars.begin(); i != vars.end();
         ++i) {
      if ((*i)->get_initializer() == init) {
        initname = *i;
        break;
      }
    }
  } else {
    std::cout << "initparent is a " << initparent->sage_class_name()
              << std::endl;
    assert(!"Should not happen");
  }

  assert(initname);
  assert(initname->get_initializer() == init);
  assert(parent);
  SgVarRefExp *vr =
      isSgVarRefExp(static_cast<SgNode *>(CreateVariableRef(initname)));
  vr->set_lvalue(true);
  initname->set_initializer(nullptr);
  SgExprStatement *assign_stmt = isSgExprStatement(static_cast<SgNode *>(
      CreateStmt(V_SgAssignStatement, vr, init->get_operand(), stmt)));

  myStatementInsert(stmt, assign_stmt, false);
  assign_stmt->set_parent(parent);

  return isSgAssignOp(assign_stmt->get_expression());
}

#if USED_CODE_IN_COVERAGE_TEST
void CSageCodeGen::SetToJson(const string &filename, const string &key,
                             const vector<string> &values,
                             const string &level_1, const string &level_2) {
  assert(!level_1.empty());

  ifstream ifs(filename);
  Document d;
  if (ifs.good()) {
    IStreamWrapper isw(ifs);
    d.ParseStream(isw);
  } else {
    d.SetObject();
  }
  ifs.close();

  Value::MemberIterator it = d.FindMember(level_1.c_str());
  if (it == d.MemberEnd()) {
    Value v(kObjectType);
    Value key;
    key.SetString(level_1.c_str(), level_1.length(), d.GetAllocator());
    d.AddMember(key, v, d.GetAllocator());
    it = d.FindMember(level_1.c_str());
  } else {
    assert(it->value.IsObject());
  }

  if (!level_2.empty()) {
    Value::MemberIterator it2 = it->value.FindMember(level_2.c_str());
    if (it2 == it->value.MemberEnd()) {
      Value v(kObjectType);
      Value key;
      key.SetString(level_2.c_str(), level_2.length(), d.GetAllocator());
      it->value.AddMember(key, v, d.GetAllocator());
    } else {
      assert(it2->value.IsObject());
    }
  }

  if (!key.empty() && !values.empty()) {
    Value::MemberIterator dst;
    if (level_2.empty()) {
      dst = d.FindMember(level_1.c_str());
    } else {
      dst = d[level_1.c_str()].FindMember(level_2.c_str());
    }

    Value v;
    if (values.size() == 1) {
      v.SetString(values[0].c_str(), values[0].length(), d.GetAllocator());
    } else {
      v.SetArray();
      for (size_t i = 0; i < values.size(); ++i) {
        v.PushBack(Value().SetString(values[i].c_str(), values[i].length(),
                                     d.GetAllocator()),
                   d.GetAllocator());
      }
    }
    dst->value.AddMember(
        Value().SetString(key.c_str(), key.length(), d.GetAllocator()), v,
        d.GetAllocator());
  }

  ofstream ofs(filename);
  OStreamWrapper osw(ofs);
  PrettyWriter<OStreamWrapper> writer(osw);
  d.Accept(writer);
  ofs.close();
}
#endif

void CSageCodeGen::dump_all_func() {
  vector<void *> vec_funcs;
  // GetNodesByType_int(nullptr, "preorder", V_SgFunctionDeclaration,
  // &vec_funcs);
  GetNodesByType_int(nullptr, "preorder", V_SgFunctionCallExp, &vec_funcs);
  for (auto func : vec_funcs) {
    if (UnparseToString(func).find("mylib") == -1) {
      // continue;
    }
    cout << " **** DUMP = " << _p(func, 0) << endl;
  }
}

//  Yuxin: 05/02/2018
//  By default, not processing pragmas from third party
void CSageCodeGen::PropagatePragma(void *old_var, void *scope,
                                   const string &new_var, bool third_party,
                                   vector<void *> *new_pragmas) {
  vector<void *> vec_pragma;
  GetNodesByType_int(scope, "preorder", V_SgPragmaDeclaration, &vec_pragma);
  CMarsIrV2 mars_ir(this);
  for (size_t i = 0; i < vec_pragma.size(); ++i) {
    void *pragma = vec_pragma[i];
    if (!mars_ir.get_pragma_attribute(pragma, CMOST_interface).empty() ||
        !mars_ir.get_pragma_attribute(pragma, CMOST_LINE_BUFFER).empty() ||
        !mars_ir.get_pragma_attribute(pragma, CMOST_cache).empty()) {
      continue;
    }
    bool is_cmost = true;
    string var_name;
    if (third_party) {
      var_name =
          mars_ir.get_all_pragma_attribute(pragma, CMOST_variable, &is_cmost);
    } else {
      var_name = mars_ir.get_pragma_attribute(pragma, CMOST_variable);
    }

    if (var_name.empty()) {
      continue;
    }
    if (find_variable_by_name(var_name, pragma) != old_var) {
      continue;
    }
    void *new_pragma = CopyStmt(pragma);
    InsertStmt(new_pragma, pragma);
    mars_ir.set_pragma_attribute(&new_pragma, CMOST_variable, new_var,
                                 third_party);
    if (new_pragmas != nullptr) {
      new_pragmas->push_back(new_pragma);
    }
    //  Yuxin: 05/02/2018
    //  Remove the old pragmas if they are not ACCEL pragmas
    if (!is_cmost) {
      RemoveStmt(pragma);
    }
  }
}

bool CSageCodeGen::is_identical_base_type(void *sg_array1, void *sg_array2) {
  void *array1_type_var = GetTypebyVar(sg_array1);
  void *array1_basic_type = nullptr;
  vector<size_t> array1_basic_size;
  get_type_dimension(array1_type_var, &array1_basic_type, &array1_basic_size,
                     sg_array1);
  void *array2_type_var = GetTypebyVar(sg_array2);

  void *array2_basic_type = nullptr;
  vector<size_t> array2_basic_size;
  get_type_dimension(array2_type_var, &array2_basic_type, &array2_basic_size,
                     sg_array2);
  return (array1_basic_type != nullptr) &&
         (GetBaseType(array1_basic_type, true) ==
          GetBaseType(array2_basic_type, true));
}

bool CSageCodeGen::IsUnderConditionStmt(void *stmt) {
  void *curr_stmt = stmt;
  while (curr_stmt != nullptr) {
    void *parent = GetParent(curr_stmt);
    if (IsBasicBlock(parent) != 0) {
    } else if (IsLoopStatement(parent) != 0) {
      return GetLoopTest(parent) == curr_stmt;
    } else if (IsSwitchStatement(parent) != 0) {
      return GetSwitchStmtSelector(parent) == curr_stmt;
    } else if (IsIfStatement(parent) != 0) {
      return GetIfStmtCondition(parent) == curr_stmt;
    } else if ((IsExpression(parent) == 0) && (IsExprStatement(parent) == 0)) {
      return false;
    }
    curr_stmt = parent;
  }
  return false;
}

bool CSageCodeGen::IsScopeBreak(void *sg_break, void *sg_scope) {
  if (GetChildStmtIdx(sg_scope, sg_break) < GetChildStmtNum(sg_scope)) {
    return true;
  }

  void *sg_loop = GetEnclosingNode("ForLoop", sg_break);
  void *sg_while = GetEnclosingNode("While", sg_break);
  void *sg_dowhile = GetEnclosingNode("DoWhile", sg_break);
  void *sg_switch = GetEnclosingNode("Switch", sg_break);

  if (sg_loop != nullptr) {
    if (sg_loop != sg_scope && (IsInScope(sg_loop, sg_scope) != 0)) {
      return false;
    }
  }

  if (sg_while != nullptr) {
    if (sg_while != sg_scope && (IsInScope(sg_while, sg_scope) != 0)) {
      return false;
    }
  }

  if (sg_dowhile != nullptr) {
    if (sg_dowhile != sg_scope && (IsInScope(sg_dowhile, sg_scope) != 0)) {
      return false;
    }
  }

  if (sg_switch != nullptr) {
    if (sg_switch != sg_scope && (IsInScope(sg_switch, sg_scope) != 0)) {
      return false;
    }
  }
  return true;
}

bool CSageCodeGen::IsScopeContinue(void *sg_continue, void *sg_scope) {
  if (GetChildStmtIdx(sg_scope, sg_continue) < GetChildStmtNum(sg_scope)) {
    return true;
  }

  void *sg_loop = GetEnclosingNode("ForLoop", sg_continue);
  void *sg_while = GetEnclosingNode("While", sg_continue);
  void *sg_dowhile = GetEnclosingNode("DoWhile", sg_continue);

  if (sg_loop != nullptr) {
    if (sg_loop != sg_scope && (IsInScope(sg_loop, sg_scope) != 0)) {
      return false;
    }
  }

  if (sg_while != nullptr) {
    if (sg_while != sg_scope && (IsInScope(sg_while, sg_scope) != 0)) {
      return false;
    }
  }

  if (sg_dowhile != nullptr) {
    if (sg_dowhile != sg_scope && (IsInScope(sg_dowhile, sg_scope) != 0)) {
      return false;
    }
  }
  return true;
}

void CSageCodeGen::get_floating_var_refs(void *scope,
                                         vector<void *> *vec_refs) {
  vector<void *> vec_exprs;
  get_floating_exprs(scope, &vec_exprs);
  for (auto expr : vec_exprs) {
    vector<void *> vec_sub_refs;
    GetNodesByType_int(expr, "preorder", V_SgVarRefExp, &vec_sub_refs);
    vec_refs->insert(vec_refs->end(), vec_sub_refs.begin(), vec_sub_refs.end());
  }
}

void CSageCodeGen::get_floating_exprs(void *scope, vector<void *> *vec_res) {
  //  variable refs occur in type definition
  vector<void *> vec_inits;
  GetNodesByType_int(scope, "preorder", V_SgInitializedName, &vec_inits);
  for (auto init : vec_inits) {
    void *var_type = GetTypebyVar(init);
    get_exprs_from_type(var_type, vec_res);
  }

  vector<void *> vec_exps;
  GetNodesByType_int_compatible(scope, V_SgExpression, &vec_exps);

  for (auto exp : vec_exps) {
    void *exp_type = nullptr;
    if (IsCastExp(exp) != 0) {
      exp_type = GetTypeByExp(exp);
    } else if (SgSizeOfOp *sg_sizeof_op =
                   isSgSizeOfOp(static_cast<SgNode *>(exp))) {
      void *sg_type = sg_sizeof_op->get_operand_type();
      if (sg_type != nullptr) {
        exp_type = sg_type;
      }
    }
    if (exp_type == nullptr) {
      continue;
    }
    get_exprs_from_type(exp_type, vec_res);
  }
}

void CSageCodeGen::replace_floating_var_refs(void *scope) {
  //  variable refs occur in type definition
  vector<void *> vec_inits;
  GetNodesByType_int(scope, "preorder", V_SgInitializedName, &vec_inits);
  for (auto init : vec_inits) {
    void *var_type = GetTypebyVar(init);
    replace_var_refs_in_type(var_type, init);
  }

  vector<void *> vec_exps;
  GetNodesByType_int_compatible(scope, V_SgExpression, &vec_exps);

  for (auto exp : vec_exps) {
    void *exp_type = nullptr;
    if (SgSizeOfOp *sg_sizeof_op = isSgSizeOfOp(static_cast<SgNode *>(exp))) {
      void *sg_type = sg_sizeof_op->get_operand_type();
      if (sg_type != nullptr) {
        exp_type = sg_type;
      }
    } else {
      exp_type = GetTypeByExp(exp);
    }
    if (exp_type == nullptr) {
      continue;
    }
    replace_var_refs_in_type(exp_type, exp);
  }
}

static int get_multi_assign_count(void *node) {
  SgNode *n = static_cast<SgNode *>(node);
  if (isSgAssignOp(n) != nullptr) {
    SgAssignOp *assign_op = isSgAssignOp(n);
    return 1 + get_multi_assign_count(assign_op->get_rhs_operand());
  }
  return 0;
}

static bool is_multi_assign(void *n) {
  if (n == nullptr) {
    return false;
  }
  return get_multi_assign_count(n) > 1;
}

void *CSageCodeGen::convertAssign(void *node) {
  SgExpression *n = isSgExpression(static_cast<SgNode *>(node));
  SgAssignOp *curr = isSgAssignOp(n);
  assert(curr != nullptr);
  vector<void *> exps;
  for (; curr != nullptr;) {
    void *exp = CopyExp(curr->get_lhs_operand());
    exps.push_back(exp);
    n = curr->get_rhs_operand();
    curr = isSgAssignOp(n);
  }
  assert(n != nullptr);
  exps.push_back(CopyExp(n));
  assert(exps.size() >= 3);

  void *rhs = CreateExp(V_SgAssignOp, exps[0], exps[1]);
  void *lhs = CreateExp(V_SgAssignOp, CopyExp(exps[1]), exps[2]);
  rhs = CreateExp(V_SgCommaOpExp, lhs, rhs);
  for (size_t i = 3; i < exps.size(); ++i) {
    lhs = CreateExp(V_SgAssignOp, CopyExp(exps[i - 1]), exps[i]);
    rhs = CreateExp(V_SgCommaOpExp, lhs, rhs);
  }
  return rhs;
}

//  first node and last node can have side effect
bool CSageCodeGen::isMultiAssignHaveSideEffect(void *node) {
  SgNode *n = static_cast<SgNode *>(node);
  SgAssignOp *prev = isSgAssignOp(n);
  if (prev == nullptr) {
    return false;
  }
  SgExpression *curr_exp = prev->get_rhs_operand();
  SgAssignOp *curr = isSgAssignOp(curr_exp);
  if (curr == nullptr) {
    return false;
  }
  SgAssignOp *next = isSgAssignOp(curr->get_rhs_operand());
  bool ret = has_side_effect(curr->get_lhs_operand()) != 0;
  for (; next != nullptr;
       curr = next, next = isSgAssignOp(next->get_rhs_operand())) {
    SgExpression *lhs = curr->get_lhs_operand();
    ret = (ret && (has_side_effect(lhs) != 0));
  }
  return ret;
}

bool CSageCodeGen::SplitMultiAssign(void *scope) {
  vector<void *> stmts;
  set<void *> changed_nodes;
  GetNodesByType(scope, "postorder", "SgAssignOp", &stmts);
  for (auto n : stmts) {
    SgNode *curr = static_cast<SgNode *>(n);
    if (curr == nullptr || (is_floating_node(curr) != 0)) {
      continue;
    }
    SgNode *parent = static_cast<SgNode *>(GetParent(curr));
    while (isSgAssignOp(parent) != nullptr) {
      curr = parent;
      parent = static_cast<SgNode *>(GetParent(curr));
    }
    if (curr == nullptr || !is_multi_assign(curr) ||
        changed_nodes.count(parent) > 0) {
      continue;
    }
    if (isMultiAssignHaveSideEffect(curr)) {
      continue;
    }
    changed_nodes.insert(parent);
    void *ret = convertAssign(curr);
    ReplaceExp(curr, ret);
  }
  return !changed_nodes.empty();
}

//  Yuxin: April 3 2018:
//    collect the sub loops from scope to the ref
//    Considering the loops in the function calls
//  Yuxin: Nov/13/2019
//    Enhance code structure
//    get call paths from scope not project
void CSageCodeGen::get_loop_nest_from_ref(void *scope, void *ref,
                                          set<void *> *loops,
                                          bool include_while, bool skip_scope) {
  if (IsInScope(ref, scope) != 0 || skip_scope != 0) {
    void *sub_loop;
    if (include_while) {
      sub_loop = TraceUpToLoopScope(ref, true);
    } else {
      sub_loop = GetEnclosingNode("ForLoop", ref);
    }
    while ((sub_loop != nullptr) &&
           (IsInScope(sub_loop, scope) != 0 || skip_scope != 0)) {
      if (loops->find(sub_loop) == loops->end()) {
        loops->insert(sub_loop);
      }
      if (include_while) {
        sub_loop = TraceUpToLoopScope(sub_loop, true);
      } else {
        sub_loop = GetEnclosingNode("ForLoop", sub_loop);
      }
    }
    return;
  }

  list<t_func_call_path> vec_paths;
  int ret = get_call_paths(ref, scope, &vec_paths);
  if (ret == 0) {
    return;
  }
  if (vec_paths.empty()) {
    return;
  }
  // FIXME: Yuxin, Nov/11/2019
  // Handle multiple call paths
  t_func_call_path call_path = *vec_paths.begin();

#if 0
    for (fp = call_path.begin(); fp != call_path.end(); fp++) {
      cout  << "callee: " << _p(fp->first) <<endl;
      cout  << "call: " << _p(fp->second) <<endl;
    }
#endif

  void *ref_exp = ref;
  t_func_call_path::iterator fp;
  for (fp = call_path.begin(); fp != call_path.end(); fp++) {
    if (IsInScope(ref_exp, scope) != 0)
      get_loop_nest_from_ref(scope, ref_exp, loops, include_while, false);
    else
      get_loop_nest_from_ref(scope, ref_exp, loops, include_while, true);
    ref_exp = fp->second;
  }
  return;
}

void CSageCodeGen::RemoveLabel(void *sg_node) {
  SgLabelStatement *label = isSgLabelStatement(static_cast<SgNode *>(sg_node));
  if (label == nullptr) {
    return;
  }
  SgStatement *stmt = label->get_statement();
  if (stmt == nullptr) {
    return;
  }
  bindNodeOrgId(this, stmt, stmt);
  label->set_statement(nullptr);
  stmt->set_parent(nullptr);
  ReplaceStmt(label, stmt);
}

string CSageCodeGen::GetLabelName(void *sg_node) {
  SgLabelStatement *label = isSgLabelStatement(static_cast<SgNode *>(sg_node));
  if (label == nullptr) {
    return "";
  }
  return label->get_label();
}

CSageCodeGen::CSageCodeGen(SgProject *sg) : m_sg_project(sg) {
  //  m_nFileIdx = 0;
  m_defuse = nullptr;
  m_liveness = nullptr;
  m_callgraph = nullptr;
  s_liveness_previous_func = nullptr;
  m_tfn_syms_valid = false;
  m_tmfn_syms_valid = false;
  m_tc_syms_valid = false;
  m_cls_syms_valid = false;
  m_var_syms_valid = false;
  m_func_syms_valid = false;
}

CSageCodeGen::~CSageCodeGen() {
  delete m_defuse;
  delete m_liveness;
  delete m_callgraph;
  delete m_sg_project;
  for (auto typedef_decl : m_builtin_type_decls) {
    delete typedef_decl;
  }
  m_builtin_type_decls.clear();
  for (auto new_type : m_new_types) {
    delete new_type;
  }
  m_new_types.clear();
}

string CSageCodeGen::GetFileInfo_filename(void *sgnode, int simple) const {
  if (sgnode == nullptr) {
    return "";
  }
  string sname =
      (static_cast<SgNode *>(sgnode))->get_file_info()->get_filenameString();
  if (simple != 0) {
    if (sname.rfind("/") != string::npos) {
      sname = sname.substr(sname.rfind("/") + 1);
    }
  }
  return sname;
}

void *CSageCodeGen::GetFuncDeclByInstDirectiveStmt(void *sg_node) {
  SgTemplateInstantiationDirectiveStatement *tp_inst_dir_stmt =
      isSgTemplateInstantiationDirectiveStatement(
          static_cast<SgNode *>(sg_node));
  if (tp_inst_dir_stmt != nullptr) {
    return tp_inst_dir_stmt->get_declaration();
  }
  return nullptr;
}

void *CSageCodeGen::GetTemplateFuncDecl(void *sg_node) {
  SgTemplateFunctionDeclaration *tf = nullptr;
  if (auto *sg_tf =
          isSgTemplateFunctionDeclaration(static_cast<SgNode *>(sg_node))) {
    tf = sg_tf;
  }
  if (auto *sg_inst = isSgTemplateInstantiationFunctionDecl(
          static_cast<SgNode *>(sg_node))) {
    tf = sg_inst->get_templateDeclaration();
  }
  if ((tf == nullptr) ||
      (tf->get_declaration_associated_with_symbol() == nullptr)) {
    return nullptr;
  }
  SgTemplateFunctionSymbol *tfnsym =
      isSgTemplateFunctionSymbol(tf->search_for_symbol_from_symbol_table());
  if (!m_tfn_syms_valid) {
    reset_tfn_syms();
  }
  auto it = m_tfn_syms.find(tfnsym);
  return it != m_tfn_syms.end() ? it->second : nullptr;
}

void *CSageCodeGen::GetTemplateMemFuncDecl(void *sg_node) {
  SgTemplateMemberFunctionDeclaration *tmf = nullptr;
  if (auto *sg_tmf = isSgTemplateMemberFunctionDeclaration(
          static_cast<SgNode *>(sg_node))) {
    tmf = sg_tmf;
  }
  if (auto *sg_inst_mem = isSgTemplateInstantiationMemberFunctionDecl(
          static_cast<SgNode *>(sg_node))) {
    tmf = sg_inst_mem->get_templateDeclaration();
  }
  if ((tmf == nullptr) ||
      (tmf->get_declaration_associated_with_symbol() == nullptr)) {
    return nullptr;
  }
  SgTemplateMemberFunctionSymbol *tmfnsym = isSgTemplateMemberFunctionSymbol(
      tmf->search_for_symbol_from_symbol_table());
  if (!m_tmfn_syms_valid) {
    reset_tmfn_syms();
  }
  auto it = m_tmfn_syms.find(tmfnsym);
  return it != m_tmfn_syms.end() ? it->second : nullptr;
}

void CSageCodeGen::reset_tfn_syms() {
  m_tfn_syms.clear();
  vector<void *> vec_tfn;
  GetNodesByType_int_compatible(nullptr, V_SgTemplateFunctionDeclaration,
                                &vec_tfn);
  for (auto tfn : vec_tfn) {
    SgTemplateFunctionSymbol *tfnsym = nullptr;
    auto *tfn_decl =
        isSgTemplateFunctionDeclaration(static_cast<SgNode *>(tfn));
    if ((tfn_decl != nullptr) &&
        (tfn_decl->get_declaration_associated_with_symbol() != nullptr)) {
      tfnsym = isSgTemplateFunctionSymbol(
          tfn_decl->search_for_symbol_from_symbol_table());
    }
    if (m_tfn_syms.count(tfnsym) > 0) {
      auto orig_tfn = m_tfn_syms[tfnsym];
      if (get_line(orig_tfn) != 0) {
        continue;
      }
    }
    m_tfn_syms[tfnsym] = tfn;
  }
  m_tfn_syms_valid = true;
}
void CSageCodeGen::reset_tmfn_syms() {
  m_tmfn_syms.clear();
  vector<void *> vec_tmfn;
  GetNodesByType_int_compatible(nullptr, V_SgTemplateMemberFunctionDeclaration,
                                &vec_tmfn);
  for (auto tmfn : vec_tmfn) {
    auto *tmfn_decl =
        isSgTemplateMemberFunctionDeclaration(static_cast<SgNode *>(tmfn));

    SgTemplateMemberFunctionSymbol *tmfnsym = nullptr;
    if ((tmfn_decl != nullptr) &&
        (tmfn_decl->get_declaration_associated_with_symbol() != nullptr)) {
      tmfnsym = isSgTemplateMemberFunctionSymbol(
          tmfn_decl->search_for_symbol_from_symbol_table());
    }
    if (m_tmfn_syms.count(tmfnsym) > 0) {
      auto orig_tmfn = m_tmfn_syms[tmfnsym];
      if (get_line(orig_tmfn) != 0) {
        continue;
      }
    }
    m_tmfn_syms[tmfnsym] = tmfn;
  }
  m_tmfn_syms_valid = true;
}

void *CSageCodeGen::GetTemplateClassDecl(void *sg_node) {
  SgTemplateClassDeclaration *tc = nullptr;
  if (auto *sg_tc =
          isSgTemplateClassDeclaration(static_cast<SgNode *>(sg_node))) {
    tc = sg_tc;
  } else if (auto *sg_inst_tc = isSgTemplateInstantiationDecl(
                 static_cast<SgNode *>(sg_node))) {
    tc = sg_inst_tc->get_templateDeclaration();
  }
  if ((tc == nullptr) ||
      (tc->get_declaration_associated_with_symbol() == nullptr)) {
    return nullptr;
  }
  SgTemplateClassSymbol *tcsym =
      isSgTemplateClassSymbol(tc->search_for_symbol_from_symbol_table());
  if (!m_tc_syms_valid) {
    reset_tc_syms();
  }
  auto it = m_tc_syms.find(tcsym);
  return it != m_tc_syms.end() ? it->second : nullptr;
}

void CSageCodeGen::reset_tc_syms() {
  m_tc_syms.clear();
  vector<void *> vec_tc;
  GetNodesByType_int_compatible(nullptr, V_SgTemplateClassDeclaration, &vec_tc);
  for (auto tc : vec_tc) {
    SgTemplateClassSymbol *tcsym = nullptr;
    auto *tc_decl = isSgTemplateClassDeclaration(static_cast<SgNode *>(tc));
    if (tc_decl->get_declaration_associated_with_symbol() != nullptr) {
      tcsym = isSgTemplateClassSymbol(
          tc_decl->search_for_symbol_from_symbol_table());
    }
    if (m_tc_syms.count(tcsym) > 0) {
      auto orig_tc = m_tc_syms[tcsym];
      if (get_line(orig_tc) != 0) {
        continue;
      }
    }
    m_tc_syms[tcsym] = tc;
  }
  m_tc_syms_valid = true;
}

void CSageCodeGen::reset_cls_syms() {
  m_cls_syms.clear();
  vector<void *> vec_cls;
  GetNodesByType_int_compatible(nullptr, V_SgClassDeclaration, &vec_cls);
  for (auto cls : vec_cls) {
    auto *cls_decl = isSgClassDeclaration(static_cast<SgNode *>(cls));
    SgClassSymbol *clssym = nullptr;
    if ((cls_decl != nullptr) &&
        (cls_decl->get_declaration_associated_with_symbol() != nullptr)) {
      clssym = isSgClassSymbol(cls_decl->search_for_symbol_from_symbol_table());
    }
    if (m_cls_syms.count(clssym) > 0 &&
        (isSgClassDeclaration(static_cast<SgNode *>(m_cls_syms[clssym]))
             ->get_definition() != nullptr)) {
      continue;
    }
    m_cls_syms[clssym] = cls;
  }
  m_cls_syms_valid = true;
}

void CSageCodeGen::reset_var_syms() {
  m_var_syms.clear();
  vector<void *> vec_var;
  GetNodesByType_int_compatible(nullptr, V_SgInitializedName, &vec_var);
  for (auto var : vec_var) {
    auto *varsym =
        isSgVariableSymbol(isSgInitializedName(static_cast<SgNode *>(var))
                               ->search_for_symbol_from_symbol_table());
    if (m_var_syms.count(varsym) > 0 &&
        (isSgInitializedName(static_cast<SgNode *>(m_var_syms[varsym]))
             ->get_initializer() != nullptr)) {
      continue;
    }
    m_var_syms[varsym] = var;
  }
  m_var_syms_valid = true;
}

void CSageCodeGen::reset_func_syms() {
  m_func_syms.clear();
  vector<void *> vec_fn;
  GetNodesByType_int_compatible(nullptr, V_SgFunctionDeclaration, &vec_fn);
  for (auto fn : vec_fn) {
    SgFunctionSymbol *fnsym = nullptr;
    auto *sg_fn = isSgFunctionDeclaration(static_cast<SgNode *>(fn));
    if ((sg_fn != nullptr) &&
        (sg_fn->get_declaration_associated_with_symbol() != nullptr)) {
      fnsym = isSgFunctionSymbol(sg_fn->search_for_symbol_from_symbol_table());
    }
    m_func_syms[fnsym] = fn;
  }
  m_func_syms_valid = true;
}

string CSageCodeGen::get_template_string(void *sg_node) {
  void *sg_template = GetTemplateClassDecl(sg_node);
  if (sg_template != nullptr) {
    SgTemplateClassDeclaration *sg_class_decl =
        isSgTemplateClassDeclaration(static_cast<SgNode *>(sg_template));
    return sg_class_decl->get_string();
  }
  sg_template = GetTemplateFuncDecl(sg_node);
  if (sg_template != nullptr) {
    SgTemplateFunctionDeclaration *sg_func_decl =
        isSgTemplateFunctionDeclaration(static_cast<SgNode *>(sg_template));
    return sg_func_decl->get_string();
  }
  sg_template = GetTemplateMemFuncDecl(sg_node);
  if (sg_template != nullptr) {
    SgTemplateMemberFunctionDeclaration *sg_mem_func_decl =
        isSgTemplateMemberFunctionDeclaration(
            static_cast<SgNode *>(sg_template));
    return sg_mem_func_decl->get_string();
  }
  return "";
}

int CSageCodeGen::IsValueExp(void *sg_node) {
  if (isSgMinusOp(static_cast<SgNode *>(sg_node)) != nullptr) {
    return IsValueExp(
        isSgMinusOp(static_cast<SgNode *>(sg_node))->get_operand());
  }
  return static_cast<int>(isSgValueExp(static_cast<SgNode *>(sg_node)) != 0);
}

int CSageCodeGen::is_all_dim_array(void *sg_type_in) {
  SgInitializedName *sg_name =
      isSgInitializedName(static_cast<SgNode *>(sg_type_in));
  if (sg_name != nullptr) {
    sg_type_in = GetTypebyVar(sg_name);
  }

  SgArrayType *sg_type_array = isSgArrayType(static_cast<SgNode *>(sg_type_in));
  SgPointerType *sg_type_pointer =
      isSgPointerType(static_cast<SgNode *>(sg_type_in));

  if (sg_type_pointer != nullptr) {
    return 0;
  }
  if (sg_type_array != nullptr) {
    return is_all_dim_array(sg_type_array->get_base_type());
  }
  return 1;
}

int CSageCodeGen::IsConstType(void *sg_type) const {
  if (IsType(sg_type) == 0) {
    return 0;
  }
  if (SageInterface::isConstType(isSgType(static_cast<SgNode *>(sg_type)))) {
    return 1;
  }
  if ((IsPointerType(sg_type) != 0) || (IsArrayType(sg_type) != 0)) {
    return IsConstType(GetBaseTypeOneLayer(sg_type));
  }
  return 0;
}

int CSageCodeGen::IsNonConstReferenceType(void *sg_type) const {
  if (auto *ref_type = isSgReferenceType(static_cast<SgNode *>(sg_type))) {
    return static_cast<int>(
        !SageInterface::isConstType(ref_type->get_base_type()));
  }
  return 0;
}

void *CSageCodeGen::CreateConstType(void *sg_type, void *pos) {
  if (IsConstType(sg_type) != 0) {
    return sg_type;
  }
  vector<size_t> dims;
  void *base_type = nullptr;
  get_type_dimension_new(sg_type, &base_type, &dims, pos, false);
  if (!dims.empty()) {
    return CreateArrayType(CreateConstType(base_type, pos), dims);
  }

  auto sg_m_type =
      SageBuilder::buildConstType(isSgType(static_cast<SgNode *>(sg_type)));
  return sg_m_type;
}

int CSageCodeGen::IsVolatileType(void *sg_type) const {
  if (IsType(sg_type) == 0) {
    return 0;
  }
  if (SageInterface::isVolatileType(isSgType(static_cast<SgNode *>(sg_type)))) {
    return 1;
  }
  if ((IsPointerType(sg_type) != 0) || (IsArrayType(sg_type) != 0)) {
    return IsVolatileType(GetBaseTypeOneLayer(sg_type));
  }
  return 0;
}

int CSageCodeGen::IsGeneralCharType(void *sg_type) const {
  if (isSgTypeUnsignedChar(static_cast<SgNode *>(sg_type)) != nullptr) {
    return 1;
  }
  if (isSgTypeChar(static_cast<SgNode *>(sg_type)) != nullptr) {
    return 1;
  }
  if (isSgTypeSignedChar(static_cast<SgNode *>(sg_type)) != nullptr) {
    return 1;
  }
  return 0;
}

int CSageCodeGen::IsStructureType(void *sg_type_) const {
  //  void * sg_type = GetOrigTypeByTypedef(sg_type_, true);
  //  SgClassType *sg_class_type = isSgClassType(static_cast<SgType *>(
  //  sg_type));
  SgClassType *sg_class_type = isSgClassType(static_cast<SgType *>(sg_type_));
  if (sg_class_type == nullptr) {
    return 0;
  }
  SgDeclarationStatement *sg_decl = sg_class_type->get_declaration();
  SgClassDeclaration *sg_class_decl = isSgClassDeclaration(sg_decl);
  if (sg_class_decl == nullptr) {
    return 0;
  }
  return static_cast<int>(sg_class_decl->get_class_type() ==
                          SgClassDeclaration::e_struct);
}

int CSageCodeGen::IsUnionType(void *sg_type_) const {
  if (IsType(sg_type_) == 0) {
    return 0;
  }
  //  void *t = GetOrigTypeByTypedef(sg_type_, true);
  SgClassType *class_type = isSgClassType(static_cast<SgNode *>(sg_type_));
  int result = 0;
  if (class_type != nullptr) {
    SgDeclarationStatement *decl = class_type->get_declaration();
    result = static_cast<int>(SageInterface::isUnionDeclaration(decl));
  }
  return result;
}

int CSageCodeGen::IsEnumType(void *sg_type_) const {
  if ((IsType(sg_type_) == 0) || (IsModifierType(sg_type_) != 0)) {
    return 0;
  }
  //  void *sg_type = GetOrigTypeByTypedef(sg_type_, true);
  return static_cast<int>(isSgEnumType(static_cast<SgNode *>(sg_type_)) !=
                          nullptr);
}

int CSageCodeGen::IsAnonymousType(void *sg_type_) const {
  SgClassType *sg_class_type = isSgClassType(static_cast<SgType *>(sg_type_));
  if (sg_class_type != nullptr) {
    SgDeclarationStatement *sg_decl = sg_class_type->get_declaration();
    SgClassDeclaration *sg_class_decl = isSgClassDeclaration(sg_decl);
    if (sg_class_decl != nullptr) {
      return static_cast<int>(sg_class_decl->get_isUnNamed());
    }
  }
  SgEnumType *sg_enum_type = isSgEnumType(static_cast<SgType *>(sg_type_));
  if (sg_enum_type != nullptr) {
    SgDeclarationStatement *sg_decl = sg_enum_type->get_declaration();
    SgEnumDeclaration *sg_enum_decl = isSgEnumDeclaration(sg_decl);
    if (sg_enum_decl != nullptr) {
      return static_cast<int>(sg_enum_decl->get_isUnNamed());
    }
  }
  return 0;
}

int CSageCodeGen::IsAnonymousTypeDecl(void *sg_decl) const {
  SgClassDeclaration *sg_class_decl =
      isSgClassDeclaration(static_cast<SgNode *>(sg_decl));
  if (sg_class_decl != nullptr) {
    return static_cast<int>(sg_class_decl->get_isUnNamed());
  }
  SgEnumDeclaration *sg_enum_decl =
      isSgEnumDeclaration(static_cast<SgNode *>(sg_decl));
  if (sg_enum_decl != nullptr) {
    return static_cast<int>(sg_enum_decl->get_isUnNamed());
  }
  return 0;
}

int CSageCodeGen::IsTypeDecl(void *sg_decl) const {
  SgClassDeclaration *sg_class_decl =
      isSgClassDeclaration(static_cast<SgNode *>(sg_decl));
  SgEnumDeclaration *sg_enum_decl =
      isSgEnumDeclaration(static_cast<SgNode *>(sg_decl));
  SgTypedefDeclaration *sg_type_def =
      isSgTypedefDeclaration(static_cast<SgNode *>(sg_decl));
  return static_cast<int>((static_cast<int>(sg_class_decl != nullptr) != 0) ||
                          (sg_enum_decl != nullptr) ||
                          (sg_type_def) != nullptr);
}

int CSageCodeGen::IsClassType(void *sg_type_) const {
  //  void * sg_type = GetOrigTypeByTypedef(sg_type_, true);
  //  SgClassType *sg_class_type = isSgClassType(static_cast<SgType *>(
  //  sg_type));
  SgClassType *sg_class_type = isSgClassType(static_cast<SgType *>(sg_type_));
  if (sg_class_type == nullptr) {
    return 0;
  }
  SgDeclarationStatement *sg_decl = sg_class_type->get_declaration();
  SgClassDeclaration *sg_class_decl = isSgClassDeclaration(sg_decl);
  if (sg_class_decl == nullptr) {
    return 0;
  }
  return static_cast<int>(sg_class_decl->get_class_type() ==
                          SgClassDeclaration::e_class);
}

void CSageCodeGen::GetPointerInStruct(void *struct_type,
                                      vector<void *> *vec_pointer) {
  vector<void *> decls;
  GetClassDataMembers(struct_type, &decls);
  for (auto decl : decls) {
    void *var_init = GetVariableInitializedName(decl);
    void *type = GetTypebyVar(var_init);
    void *sg_type = GetOrigTypeByTypedef(type, true);
    void *base_type = GetBaseType(sg_type, true);
    if (IsPointerType(sg_type) != 0) {
      (*vec_pointer).push_back(decl);
    }
    if ((IsStructureType(base_type) != 0) || (IsClassType(base_type) != 0)) {
      GetPointerInStruct(base_type, vec_pointer);
    }
  }
}

int CSageCodeGen::ContainsUnSupportedType(void *sg_type_,
                                          void **unsupported_type,
                                          string *reason, bool intel_flow,
                                          bool is_return, string *string_type,
                                          bool has_array_pointer_parent) {
  void *sg_type = GetOrigTypeByTypedef(sg_type_, true);
  void *base_type = GetBaseType(sg_type, true);
  DEBUG(cout << "type = " << UnparseToString(sg_type, 100) << endl);
  DEBUG(cout << "base_type = " << UnparseToString(base_type, 100) << endl);
  if (IsXilinxAPFixedType(base_type) != 0) {
    return 0;
  }
  if (IsRecursiveType(base_type, unsupported_type) != 0) {
    *reason = "recursive type";
    return 1;
  }
  // if array or pointer type, pass type true to sub functions
  // so if member function have enum type, error out
  if (IsEnumType(base_type) != 0 && has_array_pointer_parent) {
    *unsupported_type = base_type;
    *reason = "an array or pointer of enum type";
    return 1;
  }

  if (IsArrayType(sg_type) != 0 || IsPointerType(sg_type) != 0) {
    has_array_pointer_parent = true;
  }

  if ((IsStructureType(base_type) != 0) || (IsClassType(base_type) != 0)) {
    void *type_decl = GetTypeDeclByType(base_type, 1);
    if (is_return) {
      vector<void *> vec_pointer;
      GetPointerInStruct(base_type, &vec_pointer);
      for (auto one_pointer : vec_pointer) {
        *unsupported_type = one_pointer;
        if (string_type != nullptr)
          *string_type += UnparseToString(one_pointer);
        *reason = "pointer in return struct";
      }
      if (vec_pointer.size() > 0) {
        return 1;
      }
    }

    if (type_decl == nullptr) {
      *unsupported_type = base_type;
      *reason = "non-defined class type";
      return 1;
    }
    if ((IsAnonymousTypeDecl(type_decl) != 0) &&
        (IsTypedefDecl(GetParent(type_decl)) == 0) &&
        (TraceUpToClassDecl(GetParent(type_decl)) == nullptr)) {
      *unsupported_type = base_type;
      *reason = "anonymous class decl";
      return 1;
    }
    vector<void *> vec_members;
    GetClassMembers(base_type, &vec_members);
    for (auto member : vec_members) {
      if (IsAnonymousTypeDecl(member) != 0) {
        *unsupported_type = member;
        *reason = "anonymous class decl";
        return 1;
      }
      void *var_init = nullptr;
      void *var_type = nullptr;
      if (IsVariableDecl(member) != 0) {
        if (static_cast<SgVariableDeclaration *>(member)->get_bitfield() !=
            nullptr) {
          *unsupported_type = member;
          *reason = "bitfield decl";
          return 1;
        }
        var_init = GetVariableInitializedName(member);
        var_type = GetTypebyVar(var_init);
      }

      if (var_type != nullptr) {
        if (ContainsUnSupportedType(var_type, unsupported_type, reason,
                                    intel_flow, is_return, string_type,
                                    has_array_pointer_parent) != 0) {
          *unsupported_type = member;
          return 1;
        }
      }
      if (IsStatic(member)) {
        *unsupported_type = member;
        *reason = "static class member";
        return 1;
      }
      if ((var_init != nullptr) &&
          (GetVariableName(var_init).empty() ||
           (IsAnonymousName(GetVariableName(var_init)) != 0))) {
        *unsupported_type = member;
        *reason = "unnamed class member";
        return 1;
      }
    }
    return 0;
  }
  if (((IsIntegerType(base_type) != 0) &&
       (!intel_flow || (IsGeneralLongLongType(base_type) == 0))) ||
      (IsFloatType(base_type) != 0) || (IsXilinxAPFixedType(base_type) != 0) ||
      (IsXilinxAPIntType(base_type) != 0) || IsComplexType(base_type) != 0) {
    return 0;
  }
  // if not return value, we will error out when argument is enum of array
  // if return value, we will error out when enum type
  // because merlin will put it to argument as array
  DEBUG(cout << "sg_type" << UnparseToString(sg_type) << endl;
        cout << "base_type" << UnparseToString(base_type) << endl);

  if (IsEnumType(base_type) != 0 &&
      (IsArrayType(sg_type) == 0 && IsPointerType(sg_type) == 0) &&
      !is_return) {
    return 0;
  }

  *unsupported_type = base_type;
  if (IsFunctionType(base_type) != 0) {
    *reason = "function pointer type";
  } else if (IsUnionType(base_type) != 0) {
    *reason = "union type";
  } else if (IsVoidType(base_type) != 0) {
    *reason = "void type";
  } else if (IsGeneralLongLongType(base_type) != 0 && intel_flow) {
    *reason = "long long type not supported by OpenCL standard";
  } else if (IsEnumType(base_type) != 0 && is_return) {
    *reason = "enum type";
  } else if (IsEnumType(base_type) != 0 &&
             (IsArrayType(sg_type) != 0 || IsPointerType(sg_type) != 0)) {
    *reason = "an array or pointer of enum type";
  } else {
    *reason = "unknown type";
  }
  return 1;
}

void CSageCodeGen::GetAllUnSupportedType(void *sg_type_,
                                         set<pair<void *, string>> *res) {
  void *sg_type = GetOrigTypeByTypedef(sg_type_, true);
  void *base_type = GetBaseType(sg_type, true);
  void *unsupported_type = nullptr;
  string reason;
  if (IsRecursiveType(base_type, &unsupported_type) != 0) {
    res->insert(std::make_pair(unsupported_type, "recursive type"));
    return;
  }
  if ((IsStructureType(base_type) != 0) || (IsClassType(base_type) != 0)) {
    vector<void *> vec_members;
    GetClassDataMembers(base_type, &vec_members);
    for (auto member : vec_members) {
      void *var_init = GetVariableInitializedName(member);
      void *var_type = GetTypebyVar(var_init);
      GetAllUnSupportedType(var_type, res);
      if (IsStatic(member)) {
        res->insert(std::make_pair(member, "static class member"));
      }
    }
    return;
  }
  if ((IsEnumType(base_type) != 0) ||
      ((IsIntegerType(base_type) != 0) &&
       (IsGeneralLongLongType(base_type) == 0)) ||
      (IsFloatType(base_type) != 0)) {
    return;
  }

  unsupported_type = base_type;
  if (IsFunctionType(base_type) != 0) {
    reason = "function pointer type";
  } else if (IsVoidType(base_type) != 0) {
    reason = "void type";
  } else if (IsUnionType(base_type) != 0) {
    reason = "union type";
  } else if (IsGeneralLongLongType(base_type) != 0) {
    reason = "int64_t type not supported by OpenCL standard";
  } else {
    reason = "unknown type";
  }
  res->insert(std::make_pair(unsupported_type, reason));
}

int CSageCodeGen::IsCompoundType(void *sg_type) {
  if ((IsUnionType(sg_type) != 0) || (IsEnumType(sg_type) != 0) ||
      (IsStructureType(sg_type) != 0) || (IsClassType(sg_type) != 0)) {
    return 1;
  }
  if ((IsTypedefType(sg_type) != 0) || (IsModifierType(sg_type) != 0)) {
    void *base_type = GetBaseType(sg_type, true);
    if (base_type == sg_type) {
      return 0;
    }
    return IsCompoundType(base_type);
  }

  return 0;
}

void *CSageCodeGen::GetExprFromStmt(void *sg_node) {
  if (IsExprStatement(sg_node) != 0) {
    return isSgExprStatement(static_cast<SgNode *>(sg_node))->get_expression();
  }
  if (IsReturnStmt(sg_node) != 0) {
    return isSgReturnStmt(static_cast<SgNode *>(sg_node))->get_expression();
  }  // TODO(youxiang): add more here
  return nullptr;
}

void *CSageCodeGen::get_array_base_type(void *sg_array) {
  void *base_type;
  vector<size_t> nSizes;
  get_type_dimension(GetTypebyVar(sg_array), &base_type, &nSizes, sg_array);

  return base_type;
}

string CSageCodeGen::GetStringByType(void *sg_type) const {
  string res;
  if ((IsClassType(sg_type) != 0) || (IsStructureType(sg_type) != 0) ||
      (IsUnionType(sg_type) != 0) || (IsEnumType(sg_type) != 0)) {
    res = GetTypeNameByType(sg_type, true);
    if (!res.empty() && res.find("_anonymous_0x") == string::npos) {
      if (res.find("::") == 0) {
        res = res.substr(2);
      }
    }
    if (IsClassType(sg_type) != 0) {
      res = "class " + res;
    } else if (IsStructureType(sg_type) != 0) {
      res = "struct " + res;
    } else if (IsUnionType(sg_type) != 0) {
      res = "union " + res;
    } else if (IsEnumType(sg_type) != 0) {
      res = "enum " + res;
    }
    return res;
  }
  if (IsPointerType(sg_type) != 0) {
    void *base_type = GetElementType(sg_type);
    return GetStringByType(base_type) +
           (((IsPointerType(base_type) != 0) || (IsArrayType(base_type) != 0))
                ? "*"
                : " *");
  }
  if (IsArrayType(sg_type) != 0) {
    void *array_dim =
        isSgArrayType(static_cast<SgNode *>(sg_type))->get_index();
    void *base_type = GetElementType(sg_type);
    return GetStringByType(base_type) +
           (((IsArrayType(base_type) != 0) || (IsPointerType(base_type) != 0))
                ? ""
                : " ") +
           "[" + _up(array_dim) + "]";
  }
  if (IsReferenceType(sg_type) != 0) {
    string res = GetStringByType(GetElementType(sg_type)) + " &";
  }
  if (IsConstType(sg_type) != 0) {
    return "const " + GetStringByType(GetElementType(sg_type));
  }
  if (IsVolatileType(sg_type) != 0) {
    return "volatile " + GetStringByType(GetElementType(sg_type));
  }
  string type_name = _up(sg_type);
  if (type_name.find("::") == 0) {
    type_name = type_name.substr(2);
  }
  return type_name;
}

void CSageCodeGen::init_defuse() {
  int debug = 0;

  if (m_defuse != nullptr) {
    delete m_defuse;
    m_defuse = nullptr;
  }
  if (m_liveness != nullptr) {
    delete m_liveness;
    m_liveness = nullptr;
  }

  m_defuse = new DefUseAnalysis(m_sg_project);
  m_liveness = new LivenessAnalysis(debug != 0, m_defuse);
#if _DEBUG_DEFUSE
  m_defuse->run(true);
  m_defuse->printDefMap();
//  m_defuse->printUseMap();
#endif
  s_def_use_cache.clear();
  s_liveness_previous_func = nullptr;
}

//  Induction Algorithm:
//  1. while there is more var_ref to refine:
//  2.   for each var_ref in the scope
//        if var_ref is a loop iterator, and it is with in the loop, skip the
//        var_ref else if var_ref does not have a single assignment def (SAD),
//        return nullptr else if the SAD is in the boundary scope, replace the
//        ref with the SAD value else if the SAD is outside the boundary scope,
//        skip the var_ref
int CSageCodeGen::analysis_induct_map_in_scope(
    void *ref_scope, void *boundary_scope, map<void *, void *> *map_var2exp,
    vector<void *> *inner_terms /* = nullptr*/) {
  CMarsAST_IF *ast = this;
  DEFINE_START_END;

  //  assert(!is_floating_node(ref_scope));
  //  assert(!is_floating_node(boundary_scope));

  // TODO(youxiang): how to judge a node is original code, not trasnformed code?

  //  1. full dim access
  {
    vector<void *> v_ref;
    v_ref.clear();
    ast->GetNodesByType_int(ref_scope, "preorder", V_SgVarRefExp, &v_ref);

    for (auto ref : v_ref) {
      void *expr;
      STEMP(start);
      if (cache_analysis_induct_map_in_scope.find(ref) !=
          cache_analysis_induct_map_in_scope.end()) {
        expr = cache_analysis_induct_map_in_scope[ref];
      } else {
        expr = ast->get_induct_expr_in_scope(ref, ref, boundary_scope, nullptr);
        cache_analysis_induct_map_in_scope[ref] = expr;
      }
      ACCTM(analysis_induct_map_in_scope___main, start, end);

      if ((expr != nullptr) && (ast->IsForStatement(expr) != 0)) {
        continue;
      }
      if (expr != nullptr) {
        auto one_pair = pair<void *, void *>(ref, expr);
        map_var2exp->insert(one_pair);
      } else if (inner_terms != nullptr) {
        inner_terms->push_back(ref);
      }
    }
  }

  //  2. pointer alias
  //  handled by step 1 when processing var_ref

  //  3. function calls
  //  Just simply recurive call to collect the map inside sub function
  {
    vector<void *> v_ref;
    v_ref.clear();
    ast->GetNodesByType_int(ref_scope, "preorder", V_SgFunctionCallExp, &v_ref);

    for (auto ref : v_ref) {
      void *call = ref;
      void *func = ast->GetFuncDeclByCall(call);

      // TODO(youxiang): here is just a workaround: to handle the non-define
      // function in
      //  general
      if (func == nullptr) {
        continue;
      }

      assert(func);
      void *body = ast->GetFuncBody(func);
      assert(body);
      int ret = analysis_induct_map_in_scope(body, boundary_scope, map_var2exp,
                                             inner_terms);

      if (ret == 0) {
        return 0;
      }
    }
  }

  return 1;
}

void *CSageCodeGen::ConvertTypeStrToPtrChar(void *sg_type) {
  return IsSgTypeString(sg_type) != 0
             ? isSgType(static_cast<SgNode *>(
                   CreatePointerType(GetTypeByString("char"))))
             : sg_type;
}

int CSageCodeGen::IsFunctionDefinition(void *sg_node) {
  return static_cast<int>(
      isSgFunctionDefinition(static_cast<SgNode *>(sg_node)) != 0);
}

vector<string> CSageCodeGen::GetOpenCLName(bool is_intel) {
  vector<string> OpenCLNameConflict;
  if (is_intel) {
    OpenCLNameConflict = {"char2",
                          "uchar2",
                          "short2",
                          "ushort2",
                          "int2",
                          "uint2",
                          "long2",
                          "ulong2",
                          "float2",
                          "char4",
                          "uchar4",
                          "short4",
                          "ushort4",
                          "int4",
                          "uint4",
                          "long4",
                          "ulong4",
                          "float4",
                          "char8",
                          "uchar8",
                          "short8",
                          "ushort8",
                          "int8",
                          "uint8",
                          "long8",
                          "ulong8",
                          "float8",
                          "char16",
                          "uchar16",
                          "short16",
                          "ushort16",
                          "int16",
                          "uint16",
                          "long16",
                          "ulong16",
                          "float16",
                          "cl_char2",
                          "cl_uchar2",
                          "cl_short2",
                          "cl_ushort2",
                          "cl_int2",
                          "cl_uint2",
                          "cl_long2",
                          "cl_ulong2",
                          "cl_float2",
                          "cl_char4",
                          "cl_uchar4",
                          "cl_short4",
                          "cl_ushort4",
                          "cl_int4",
                          "cl_uint4",
                          "cl_long4",
                          "cl_ulong4",
                          "cl_float4",
                          "cl_char8",
                          "cl_uchar8",
                          "cl_short8",
                          "cl_ushort8",
                          "cl_int8",
                          "cl_uint8",
                          "cl_long8",
                          "cl_ulong8",
                          "cl_float8",
                          "cl_char16",
                          "cl_uchar16",
                          "cl_short16",
                          "cl_ushort16",
                          "cl_int16",
                          "cl_uint16",
                          "cl_long16",
                          "cl_ulong16",
                          "cl_float16",
                          "image2d_t",
                          "image3d_t",
                          "sampler_t",
                          "event_t",
                          "bool2",
                          "bool4",
                          "bool8",
                          "bool16",
                          "double2",
                          "double4",
                          "double8",
                          "double16",
                          "half",
                          "half2",
                          "half4",
                          "half8",
                          "half16",
                          "quad",
                          "quad2",
                          "quad4",
                          "quad8",
                          "quad16",
                          "complex half",
                          "complex half2",
                          "complex half4",
                          "complex half8",
                          "complex half16",
                          "complex float",
                          "complex float2",
                          "complex float4",
                          "complex float8",
                          "complex float16",
                          "complex double",
                          "complex double2",
                          "complex double4",
                          "complex double8",
                          "complex double16",
                          "complex quad",
                          "complex quad2",
                          "complex quad4",
                          "complex quad8",
                          "complex quad16",
                          "imaginary half",
                          "imaginary half2",
                          "imaginary half4",
                          "imaginary half8",
                          "imaginary half16",
                          "imaginary float",
                          "imaginary float2",
                          "imaginary float4",
                          "imaginary float8",
                          "imaginary float16",
                          "imaginary double",
                          "imaginary double2",
                          "imaginary double4",
                          "imaginary double8",
                          "imaginary double16",
                          "imaginary quad",
                          "imaginary quad2",
                          "imaginary quad4",
                          "imaginary quad8",
                          "imaginary quad16",
                          "float2x2",
                          "float2x4",
                          "float2x8",
                          "float2x16",
                          "float4x2",
                          "float4x4",
                          "float4x8",
                          "float4x16",
                          "float8x2",
                          "float8x4",
                          "float8x8",
                          "float8x16",
                          "float16x2",
                          "float16x4",
                          "float16x8",
                          "float16x16",
                          "double2x2",
                          "double2x4",
                          "double2x8",
                          "double2x16",
                          "double4x2",
                          "double4x4",
                          "double4x8",
                          "double4x16",
                          "double8x2",
                          "double8x4",
                          "double8x8",
                          "double8x16",
                          "double16x2",
                          "double16x4",
                          "double16x8",
                          "double16x16",
                          "uint64_t",
                          "long long",
                          "long long2",
                          "long long4",
                          "long long8",
                          "long long16",
                          "unsigned long long2",
                          "unsigned long long4",
                          "unsigned long long8",
                          "unsigned long long16",
                          "long double2",
                          "long double4",
                          "long double8",
                          "long double16"};
  } else {
    OpenCLNameConflict = {"long double"};
  }
  return OpenCLNameConflict;
}

void CSageCodeGen::ConvertBigIntegerIntoSignedInteger(int64_t *nStep,
                                                      void *sg_type) {
  if ((IsUnsignedCharType(sg_type) != 0) || (IsCharType(sg_type) != 0)) {
    *nStep = static_cast<char>(*nStep);
  } else if ((IsUnsignedShortType(sg_type) != 0) ||
             (IsShortType(sg_type) != 0)) {
    *nStep = static_cast<int16_t>(*nStep);
  } else if ((IsUnsignedIntType(sg_type) != 0) || (IsIntType(sg_type) != 0)) {
    *nStep = static_cast<int>(*nStep);
  } else if ((IsUnsignedLongType(sg_type) != 0) || (IsLongType(sg_type) != 0)) {
    *nStep = static_cast<int64_t>(*nStep);
  }
}

int CSageCodeGen::GetIntTypeBitWidth(int v_type) {
  switch (v_type) {
  case V_SgTypeBool:
    return 1;
    break;
  case V_SgTypeChar:
  case V_SgTypeSignedChar:
  case V_SgTypeUnsignedChar:
    return 8;
    break;
  case V_SgTypeShort:
  case V_SgTypeSignedShort:
  case V_SgTypeUnsignedShort:
    return 16;
    break;
  case V_SgTypeInt:
  case V_SgTypeSignedInt:
  case V_SgTypeUnsignedInt:
    return 32;
    break;
  case V_SgTypeLong:
  case V_SgTypeSignedLong:
  case V_SgTypeUnsignedLong:
    return 64;
    break;
  case V_SgTypeLongLong:
  case V_SgTypeSignedLongLong:
  case V_SgTypeUnsignedLongLong:
    return 64;
    break;
  default:
    break;
  }
  // assert(0 && "should not happen here");
  return 0;
}

int CSageCodeGen::GetBinaryResultType(int op0_type, int op1_type,
                                      int binary_op) {
  if (op0_type == op1_type) {
    return op0_type;
  }
  bool op0_is_unsigned = IsUnsignedType(op0_type) != 0;
  bool op1_is_unsigned = IsUnsignedType(op1_type) != 0;
  int op0_bw = GetIntTypeBitWidth(op0_type);
  int op1_bw = GetIntTypeBitWidth(op1_type);
  if (op0_bw == 0 || op1_bw == 0) {
    return op0_bw ? op0_bw : op1_bw;
  }
  assert(op0_bw && op1_bw);
  //  https://  en.cppreference.com/w/c/language/conversion
  //  the same signness, return type with greater conversion rank
  if (op0_is_unsigned == op1_is_unsigned) {
    return op0_bw >= op1_bw ? op0_type : op1_type;
  }

  if (op0_bw > op1_bw) {
    return op0_type;
  }
  if (op0_bw < op1_bw) {
    return op1_type;
  }
  return op0_is_unsigned ? op0_type : op1_type;
}

void *CSageCodeGen::CreateSignnesCast(void *exp) {
  if (IsExpression(exp) == 0) {
    return exp;
  }
  void *type = GetTypeByExp(exp);
  if (IsIntegerType(type) == 0) {
    return exp;
  }
  int int_type = (static_cast<SgType *>(type))->variantT();
  if (IsUnsignedType(int_type) == 0) {
    return exp;
  }
  switch (int_type) {
  case V_SgTypeSignedChar:
  case V_SgTypeUnsignedChar:
  case V_SgTypeChar:
    return CreateCastExp(exp, "char");
  case V_SgTypeUnsignedShort:
  case V_SgTypeSignedShort:
  case V_SgTypeShort:
    return CreateCastExp(exp, "short");
  case V_SgTypeSignedInt:
  case V_SgTypeUnsignedInt:
  case V_SgTypeInt:
    return CreateCastExp(exp, "int");
  case V_SgTypeSignedLong:
  case V_SgTypeUnsignedLong:
  case V_SgTypeLong:
    return CreateCastExp(exp, "long");
  default:
    break;
  }
  return exp;
}

int CSageCodeGen::IsIntegerType(int v_type) {
  switch (v_type) {
  case V_SgTypeBool:
  case V_SgTypeChar:
  case V_SgTypeSignedChar:
  case V_SgTypeUnsignedChar:
  case V_SgTypeShort:
  case V_SgTypeSignedShort:
  case V_SgTypeUnsignedShort:
  case V_SgTypeInt:
  case V_SgTypeSignedInt:
  case V_SgTypeUnsignedInt:
  case V_SgTypeLong:
  case V_SgTypeSignedLong:
  case V_SgTypeUnsignedLong:
  case V_SgTypeLongLong:
  case V_SgTypeSignedLongLong:
  case V_SgTypeUnsignedLongLong:
    return 1;
    break;
  default:
    break;
  }
  return 0;
}

int CSageCodeGen::GetNearestIntegerType(void *xilinx_ap_type) {
  //  List of Xilinx ap_* classes whose width is the first template arg.
  if (auto *cls_type = isSgClassType(static_cast<SgNode *>(xilinx_ap_type))) {
    if (auto *cls_decl =
            isSgTemplateInstantiationDecl(cls_type->get_declaration())) {
      string cls_name = cls_decl->get_templateName().getString();
      if (xil_ap_int.count(cls_name) > 0) {
        bool is_signed = "ap_int" == cls_name;
        unsigned int bitwidth = 0;
        if (auto rv = get_xilinx_bitwidth(cls_decl, false)) {
          bitwidth = *rv;
        }
        if (0 == bitwidth) {
          return V_SgTypeVoid;
        }
        if (1 == bitwidth) {
          return V_SgTypeBool;
        }
        switch ((bitwidth + 7) >> 3) {
        case 1:
          return is_signed ? V_SgTypeChar : V_SgTypeUnsignedChar;
        case 2:
          return is_signed ? V_SgTypeShort : V_SgTypeUnsignedShort;
        case 3:
        case 4:
          return is_signed ? V_SgTypeInt : V_SgTypeUnsignedInt;
        default:
          return is_signed ? V_SgTypeLong : V_SgTypeUnsignedLong;
        }
      }
    }
  }
  return V_SgTypeVoid;
}

int CSageCodeGen::get_sizeof_arbitrary_precision_integer(int bitwidth) const {
  if (bitwidth == 0)
    return 0;
  if (bitwidth <= 8)
    return 1;
  else if (bitwidth <= 16)
    return 2;
  else if (bitwidth <= 32)
    return 4;
  else if (bitwidth <= 64)
    return 8;
  else
    return (bitwidth + 63) / 64 * 8;
}

int CSageCodeGen::RemoveRedundantContinueStmt(void *body) {
  if (body == nullptr) {
    return 0;
  }
  size_t numStmts = GetChildStmtNum(body);
  if (numStmts > 0) {
    void *lastStmt = GetChildStmt(body, numStmts - 1);
    if (IsContinueStatement(lastStmt) != 0) {
      RemoveStmt(lastStmt);
      RemoveRedundantContinueStmt(body);
      return 1;
    }
    if (IsBasicBlock(lastStmt) != 0) {
      return RemoveRedundantContinueStmt(lastStmt);
    }
    if (IsIfStatement(lastStmt) != 0) {
      void *true_body = GetIfStmtTrueBody(lastStmt);
      void *false_body = GetIfStmtFalseBody(lastStmt);
      int res = 0;
      res |= RemoveRedundantContinueStmt(true_body);
      res |= RemoveRedundantContinueStmt(false_body);
      return res;
    }
  }
  return 0;
}

void CSageCodeGen::GetTokens(vector<string> *tokens, const string &str,
                             const char *sep_list) {
  tokens->clear();
  typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
  boost::char_separator<char> sep{"", sep_list != nullptr ? sep_list
                                                          : " []()+-*/>"
                                                            ";,.={}&"};
  tokenizer tok{str, sep};
  for (const auto &t : tok) {
    tokens->push_back(t);
  }
}

void CSageCodeGen::MergeTokens(const vector<string> &tokens, string *ret) {
  string str;
  for (auto &token : tokens) {
    str = str + token;
  }
  *ret = str;
}

bool CSageCodeGen::is_numeric_option(string attr, bool numeric_only) {
  static const std::unordered_set<string> non_numeric_option{
      "core",     "bundle",    "clock",  "offset",
      "instance", "instances", "domain", "name"};
  boost::algorithm::to_lower(attr);
  static const std::unordered_set<string> string_option{"variable"};
  return non_numeric_option.count(attr) <= 0 &&
         (!numeric_only || string_option.count(attr) <= 0);
}

void CSageCodeGen::ReplacePragmaTokens(
    void *scope, const unordered_map<string, string> &def_directive_map_,
    const char *sep_list) {
  vector<void *> pragmas;
  GetNodesByType_int(scope, "preorder", V_SgPragmaDeclaration, &pragmas);
  for (auto &pragma : pragmas) {
    RewritePragmaTokens(pragma, def_directive_map_, sep_list);
  }
}

void CSageCodeGen::RewritePragmaTokens(
    void *pragma, const unordered_map<string, string> &def_directive_map_,
    const char *sep_list) {
  bool need_rewrite = false;
  string str_pragma = GetPragmaString(pragma);
  vector<string> tokens;
  GetTokens(&tokens, str_pragma, sep_list);
  bool is_value = false;
  bool numeric_option = false;
  string last_token;
  for (auto &token : tokens) {
    if (token == " ") {
      continue;
    }
    if (is_value && numeric_option && def_directive_map_.count(token) != 0U) {
      token = def_directive_map_.find(token)->second;
      need_rewrite = true;
    }
    is_value = token.size() == 1 && numeric_prefix.count(token[0]) > 0;
    if (token == "=") {
      string option = last_token;
      boost::algorithm::to_lower(option);
      numeric_option = is_numeric_option(option, false);
    }
    last_token = token;
  }
  MergeTokens(tokens, &str_pragma);
  // TODO(youxiang): Consider changing codegen API IsFromInputFile such that
  //  strings with only space difference are treated the same
  //  For example:
  //  foo1 foo2 foo3            /*modified file string*/
  //  foo1    foo2  foo3        /*original file string*/
  //  should be both treated as the same string.
  //  Temp fix is to skip formatting no-macro pragmas,
  //  but we can do better in design
  if (need_rewrite) {
    SetPragmaString(pragma, str_pragma);
  }
}
// Input string is in capital
string CSageCodeGen::FindTokenAttribute(const vector<string> &tokens,
                                        const string &str) {
  bool find_token = false;
  bool start_check = false;
  bool fin_check = false;
  string return_str = "";
  for (auto t : tokens) {
    if (!find_token) {
      boost::algorithm::to_upper(t);
      if (t == str)
        find_token = true;
    } else if (find_token && !start_check && t != "=") {
      find_token = false;
    } else if (find_token && !start_check && t == "=") {
      start_check = true;
    } else if (start_check && !fin_check && t != " ") {
      return_str += t;
      fin_check = true;
    } else if (start_check && fin_check && t == ",") {
      return_str += t;
      fin_check = false;
    } else if (start_check && fin_check && t == " ") {
      return return_str;
    }
  }
  return return_str;
}

// Yuxin: Dec/09/2019, build pragma cache when necessary
void CSageCodeGen::BuildPragmaCache() {
  reset_pragma_cache();

  void *sg_scope = m_sg_project;
  vector<void *> vec_pragmas;
  GetNodesByType_int(sg_scope, "preorder", V_SgPragmaDeclaration, &vec_pragmas);

  for (auto pragma : vec_pragmas) {
    set<void *> related_variables;
    string str_pragma = GetPragmaString(pragma);
    vector<string> tokens;
    GetTokens(&tokens, str_pragma);
    bool is_value = false;
    bool numeric_option = false;
    string last_token;
    for (auto &token : tokens) {
      if (token == " ") {
        continue;
      }
      if (is_value && numeric_option) {
        void *sg_var =
            getInitializedNameFromName(GetScope(pragma), token, false);
        if (sg_var != nullptr) {
          related_variables.insert(sg_var);
          if (s_variable_to_pragma_cache.count(sg_var) > 0) {
            s_variable_to_pragma_cache[sg_var].insert(pragma);
          } else {
            set<void *> related_pragmas;
            related_pragmas.insert(pragma);
            s_variable_to_pragma_cache[sg_var] = related_pragmas;
          }
        }
      }
      is_value = token.size() == 1 && numeric_prefix.count(token[0]) > 0;
      if (token == "=") {
        string option = last_token;
        boost::algorithm::to_lower(option);
        numeric_option = is_numeric_option(option, false);
      }
      last_token = token;
    }
    s_pragma_to_variable_cache[pragma] = related_variables;
  }
}

set<void *> CSageCodeGen::get_variable_used_in_pragma(void *pragma) {
  set<void *> ret;
  if (s_pragma_to_variable_cache.count(pragma) > 0)
    ret = s_pragma_to_variable_cache[pragma];
  return ret;
}

set<void *> CSageCodeGen::get_pragma_related_to_variable(void *sg_var) {
  set<void *> ret;
  if (s_variable_to_pragma_cache.count(sg_var) > 0)
    ret = s_variable_to_pragma_cache[sg_var];
  return ret;
}

bool CSageCodeGen::isCanonicalForLoop(
    void *loop, void **ivar /*=NULL*/, void **lb /*=NULL*/, void **ub /*=NULL*/,
    void **step /*=NULL*/, void **body /*=NULL*/,
    bool *hasIncrementalIterationSpace /*= NULL*/,
    bool *isInclusiveUpperBound /*=NULL*/) {
  ROSE_ASSERT(loop != NULL);
  SgForStatement *fs = isSgForStatement(static_cast<SgNode *>(loop));
  if (fs == NULL) {
    return false;
  }
  //  1. Check initialization statement is something like i=xx;
  SgStatementPtrList &init = fs->get_init_stmt();
  if (init.size() != 1) {
    return false;
  }
  SgStatement *init1 = init.front();
  void *ivarast = NULL;
  void *lbast = NULL;
  void *ubast = NULL;
  void *stepast = NULL;
  SgInitializedName *ivarname = NULL;

  bool isCase1 = false;
  bool isCase2 = false;
  //  consider C99 style: for (int i=0;...)
  if (isSgVariableDeclaration(init1) != nullptr) {
    SgVariableDeclaration *decl = isSgVariableDeclaration(init1);
    ivarname = decl->get_variables().front();
    ROSE_ASSERT(ivarname != NULL);
    SgInitializer *initor = ivarname->get_initializer();
    if (auto *assign_tor = isSgAssignInitializer(initor)) {
      lbast = assign_tor->get_operand();
      isCase1 = true;
    } else if (auto *ctor = isSgConstructorInitializer(initor)) {
      lbast = ctor;
      isCase1 = true;
    }
    //  other regular case: for (i=0;..)
  } else if (IsAssignStatement(init1, &ivarast, &lbast)) {
    SgVarRefExp *var =
        isSgVarRefExp(static_cast<SgNode *>(RemoveCast(ivarast)));
    if (var != nullptr) {
      ivarname = var->get_symbol()->get_declaration();
      isCase2 = true;
    }
  }
  //  Cannot be both true
  ROSE_ASSERT(!(isCase1 && isCase2));
  //  if not either case is true
  if (!(isCase1 || isCase2)) {
    return false;
  }

  //  Check loop index's type
  void *index_ty = GetOrigTypeByTypedef(ivarname->get_type(), true);
  if (!SageInterface::isStrictIntegerType(static_cast<SgType *>(index_ty)) &&
      (IsXilinxAPIntType(index_ty) == 0)) {
    return false;
  }

  //  2. Check test expression i [<=, >=, <, > ,!=] bound
  SgExpression *test_expr = fs->get_test_expr();
  SgBinaryOp *test = isSgBinaryOp(test_expr);
  int compare_op = 0;
  SgExpression *left_expr = nullptr;
  if (test != nullptr) {
    compare_op = test->variantT();
    left_expr = test->get_lhs_operand();
    //  grab the upper bound
    ubast = test->get_rhs_operand();
  } else {
    void *op0;
    void *op1;
    op0 = op1 = nullptr;
    if (!IsAPIntOp(test_expr, &op0, &op1, &compare_op)) {
      return false;
    }
    if (IsCompareOp(compare_op) == 0) {
      return false;
    }
    left_expr = isSgExpression(static_cast<SgNode *>(op0));
    ubast = op1;
    if ((left_expr == nullptr) || (ubast == nullptr)) {
      return false;
    }
  }
  //  check the tested variable is the same as the loop index
  SgVarRefExp *testvar = isSgVarRefExp(
      static_cast<SgNode *>(RemoveAPIntFixedCast(RemoveCast(left_expr))));
  if (testvar == NULL) {
    return false;
  }
  if (testvar->get_symbol() != ivarname->get_symbol_from_symbol_table()) {
    return false;
  }

  switch (compare_op) {
  case V_SgLessOrEqualOp:
    if (isInclusiveUpperBound != NULL) {
      *isInclusiveUpperBound = true;
    }
    if (hasIncrementalIterationSpace != NULL) {
      *hasIncrementalIterationSpace = true;
    }
    break;
  case V_SgLessThanOp:
    if (isInclusiveUpperBound != NULL) {
      *isInclusiveUpperBound = false;
    }
    if (hasIncrementalIterationSpace != NULL) {
      *hasIncrementalIterationSpace = true;
    }
    break;
  case V_SgGreaterOrEqualOp:
    if (isInclusiveUpperBound != NULL) {
      *isInclusiveUpperBound = true;
    }
    if (hasIncrementalIterationSpace != NULL) {
      *hasIncrementalIterationSpace = false;
    }
    break;
  case V_SgGreaterThanOp:
    if (isInclusiveUpperBound != NULL) {
      *isInclusiveUpperBound = false;
    }
    if (hasIncrementalIterationSpace != NULL) {
      *hasIncrementalIterationSpace = false;
    }
    break;
    //    case V_SgNotEqualOp: //  Do we really want to allow this != operator ?
    break;
  default:
    return false;
  }

  //  3. Check the increment expression
  /* Allowed forms
     ++var
     var++
     --var
     var--

     var += incr
     var -= incr

     var = var + incr
     var = incr + var
     var = var - incr
     */
  SgExpression *incr = fs->get_increment();
  SgVarRefExp *incr_var = NULL;
  int incr_op = 0;
  void *incr_op_0;
  void *incr_op_1;
  incr_op_0 = incr_op_1 = nullptr;
  if (IsAPIntOp(incr, &incr_op_0, &incr_op_1, &incr_op)) {
    switch (incr_op) {
    case V_SgPlusAssignOp:   //  +=
    case V_SgMinusAssignOp:  //  -=
      incr_var = isSgVarRefExp(static_cast<SgNode *>(RemoveCast(incr_op_0)));
      stepast = incr_op_1;
      break;
    case V_SgPlusPlusOp:    //  ++
    case V_SgMinusMinusOp:  //  --
      incr_var = isSgVarRefExp(static_cast<SgNode *>(RemoveCast(incr_op_0)));
      stepast =
          CreateConst(1);  //  will this dangling SgNode cause any problem?
      break;
    case V_SgAssignOp: {  //  cases : var + incr, var - incr, incr + var (not
                          //  allowed: incr-var)
      incr_var = isSgVarRefExp(static_cast<SgNode *>(RemoveCast(incr_op_0)));
      if (incr_var == NULL) {
        return false;
      }
      int binary_op = 0;
      void *binary_op0;
      void *binary_op1;
      binary_op0 = binary_op1 = nullptr;
      bool matched = false;
      void *assign_val = RemoveAPIntFixedCast(RemoveCast(incr_op_1));
      if (!IsAPIntOp(static_cast<SgNode *>(assign_val), &binary_op0,
                     &binary_op1, &binary_op)) {
        if (IsBinaryOp(assign_val) == 0) {
          return false;
        }
        binary_op = isSgBinaryOp(static_cast<SgNode *>(assign_val))->variantT();
        GetExpOperand(assign_val, &binary_op0, &binary_op1);
      }
      if (V_SgAddOp != binary_op && V_SgSubtractOp != binary_op) {
        return false;
      }
      if (SgVarRefExp *varRefExp = isSgVarRefExp(static_cast<SgNode *>(
              RemoveAPIntFixedCast(RemoveCast(binary_op0))))) {
        if (GetVariableInitializedName(incr_var) ==
            GetVariableInitializedName(varRefExp)) {
          //  cases : var + incr, var - incr
          stepast = binary_op1;
          matched = true;
        }
      } else if (SgVarRefExp *varRefExp = isSgVarRefExp(static_cast<SgNode *>(
                     RemoveAPIntFixedCast(RemoveCast(binary_op1))))) {
        if ((V_SgAddOp == binary_op) &&
            (GetVariableInitializedName(varRefExp) ==
             GetVariableInitializedName(incr_var))) {
          //  case : incr + var (not allowed: incr-var)
          stepast = binary_op0;
          matched = true;
        }
      }
      if (!matched) {
        return false;
      }
      break;
    }  //  end of V_AssignOp
    default:
      return false;
    }
  } else {
    switch (incr->variantT()) {
    case V_SgPlusAssignOp:   //  +=
    case V_SgMinusAssignOp:  //  -=
      incr_var = isSgVarRefExp(static_cast<SgNode *>(
          RemoveCast(isSgBinaryOp(incr)->get_lhs_operand())));
      stepast = isSgBinaryOp(incr)->get_rhs_operand();
      break;
    case V_SgPlusPlusOp:    //  ++
    case V_SgMinusMinusOp:  //  --
      incr_var = isSgVarRefExp(
          static_cast<SgNode *>(RemoveCast(isSgUnaryOp(incr)->get_operand())));
      stepast =
          CreateConst(1);  //  will this dangling SgNode cause any problem?
      break;
    case V_SgAssignOp: {  //  cases : var + incr, var - incr, incr + var (not
                          //  allowed: incr-var)
      incr_var = isSgVarRefExp(static_cast<SgNode *>(
          RemoveCast(isSgBinaryOp(incr)->get_lhs_operand())));
      if (incr_var == NULL) {
        return false;
      }
      SgExpression *assign_rhs = isSgBinaryOp(incr)->get_rhs_operand();
      SgAddOp *addOp = isSgAddOp(static_cast<SgNode *>(RemoveCast(assign_rhs)));
      SgSubtractOp *subtractOp =
          isSgSubtractOp(static_cast<SgNode *>(RemoveCast(assign_rhs)));
      SgBinaryOp *arithOp = 0;
      if (addOp != nullptr) {
        arithOp = addOp;
      } else if (subtractOp != nullptr) {
        arithOp = subtractOp;
      } else {
        return false;
      }
      ROSE_ASSERT(arithOp != 0);
      bool matched_pattern = false;
      if (SgVarRefExp *varRefExp = isSgVarRefExp(static_cast<SgNode *>(
              RemoveCast(isSgBinaryOp(arithOp)->get_lhs_operand())))) {
        if (GetVariableInitializedName(incr_var) ==
            GetVariableInitializedName(varRefExp)) {
          //  cases : var + incr, var - incr
          stepast = arithOp->get_rhs_operand();
          matched_pattern = true;
        }
      } else if (SgVarRefExp *varRefExp = isSgVarRefExp(static_cast<SgNode *>(
                     RemoveCast(isSgBinaryOp(arithOp)->get_rhs_operand())))) {
        if ((isSgAddOp(arithOp) != nullptr) &&
            (GetVariableInitializedName(incr_var) ==
             GetVariableInitializedName(varRefExp))) {
          //  case : incr + var (not allowed: incr-var)
          matched_pattern = true;
          stepast = arithOp->get_lhs_operand();
        }
      }
      if (!matched_pattern) {
        return false;
      }
      break;
    }  //  end of V_AssignOp
    default:
      return false;
    }
  }

  if (incr_var == NULL) {
    return false;
  }
  if (incr_var->get_symbol() != ivarname->get_symbol_from_symbol_table()) {
    return false;
  }

  if (IsConstructorInitializer(lbast) != 0) {
    lbast = GetFuncCallParam(lbast, 0);
  }
  if (IsConstructorInitializer(ubast) != 0) {
    ubast = GetFuncCallParam(ubast, 0);
  }
  if (IsConstructorInitializer(stepast) != 0) {
    stepast = GetFuncCallParam(stepast, 0);
  }
  //  return loop information if requested
  if (ivar != NULL) {
    *ivar = ivarname;
  }
  if (lb != NULL) {
    *lb = lbast;
  }
  if (ub != NULL) {
    *ub = ubast;
  }
  if (step != NULL) {
    *step = stepast;
  }
  if (body != NULL) {
    *body = fs->get_loop_body();
  }
  return true;
}

bool CSageCodeGen::IsAssignStatement(void *stmt, void **left_exp,
                                     void **right_exp) {
  void *expr = GetExprFromStmt(stmt);
  if (IsAssignOp(expr) != 0) {
    GetExpOperand(expr, left_exp, right_exp);
    return true;
  }
  int op = 0;
  return IsAPIntOp(expr, left_exp, right_exp, &op) && V_SgAssignOp == op;
}
int CSageCodeGen::IsWriteOperatorOp(int opcode) {
  return static_cast<int>(
      (V_SgPlusPlusOp == opcode) || (V_SgMinusMinusOp == opcode) ||
      (V_SgLshiftAssignOp == opcode) || (V_SgRshiftAssignOp == opcode) ||
      (V_SgPlusAssignOp == opcode) || (V_SgMinusAssignOp == opcode) ||
      (V_SgMultAssignOp == opcode) || (V_SgDivAssignOp == opcode) ||
      (V_SgModAssignOp == opcode) || (V_SgIorAssignOp == opcode) ||
      (V_SgXorAssignOp == opcode) || (V_SgAndAssignOp == opcode) ||
      (V_SgAssignOp == opcode));
}

int CSageCodeGen::IsReadOnlyOperatorOp(int opcode) {
  return static_cast<int>(
      (V_SgLessThanOp == opcode) || (V_SgGreaterThanOp == opcode) ||
      (V_SgAddOp == opcode) || (V_SgSubtractOp == opcode) ||
      (V_SgMultiplyOp == opcode) || (V_SgDivideOp == opcode) ||
      (V_SgModOp == opcode) || (V_SgBitAndOp == opcode) ||
      (V_SgBitOrOp == opcode) || (V_SgBitXorOp == opcode) ||
      (V_SgGreaterOrEqualOp == opcode) || (V_SgEqualityOp == opcode) ||
      (V_SgLessOrEqualOp == opcode) || (V_SgNotEqualOp == opcode) ||
      (IsImplicitConversionOp(opcode)) != 0);
}

int CSageCodeGen::IsImplicitConversionOp(int opcode) {
  return static_cast<int>((V_SgOperatorLong == opcode) ||
                          (V_SgOperatorULong == opcode) ||
                          (V_SgOperatorFloat == opcode));
}

int CSageCodeGen::GetOperatorOp(void *decl) {
  if (IsFunctionDeclaration(decl) == 0) {
    return 0;
  }
  string func_name = GetFuncName(decl);
  if (func_name.find("operator ++") != string::npos) {
    return V_SgPlusPlusOp;
  }
  if (func_name.find("operator --") != string::npos) {
    return V_SgMinusMinusOp;
  }
  if (func_name.find("operator <<=") != string::npos) {
    return V_SgLshiftAssignOp;
  }
  if (func_name.find("operator >>=") != string::npos) {
    return V_SgRshiftAssignOp;
  }
  if (func_name.find("operator >=") != string::npos) {
    return V_SgGreaterOrEqualOp;
  }
  if (func_name.find("operator ==") != string::npos) {
    return V_SgEqualityOp;
  }
  if (func_name.find("operator <=") != string::npos) {
    return V_SgLessOrEqualOp;
  }
  if (func_name.find("operator !=") != string::npos) {
    return V_SgNotEqualOp;
  }
  if (func_name.find("operator +=") != string::npos) {
    return V_SgPlusAssignOp;
  }
  if (func_name.find("operator -=") != string::npos) {
    return V_SgMinusAssignOp;
  }
  if (func_name.find("operator *=") != string::npos) {
    return V_SgMultAssignOp;
  }
  if (func_name.find("operator /=") != string::npos) {
    return V_SgDivAssignOp;
  }
  if (func_name.find("operator %=") != string::npos) {
    return V_SgModAssignOp;
  }
  if (func_name.find("operator |=") != string::npos) {
    return V_SgIorAssignOp;
  }
  if (func_name.find("operator ^=") != string::npos) {
    return V_SgXorAssignOp;
  }
  if (func_name.find("operator &=") != string::npos) {
    return V_SgAndAssignOp;
  }
  if (func_name.find("operator <") != string::npos) {
    return V_SgLessThanOp;
  }
  if (func_name.find("operator >") != string::npos) {
    return V_SgGreaterThanOp;
  }
  if (func_name.find("operator +") != string::npos) {
    return V_SgAddOp;
  }
  if (func_name.find("operator -") != string::npos) {
    return V_SgSubtractOp;
  }
  if (func_name.find("operator *") != string::npos) {
    return V_SgMultiplyOp;
  }
  if (func_name.find("operator /") != string::npos) {
    return V_SgDivideOp;
  }
  if (func_name.find("operator %") != string::npos) {
    return V_SgModOp;
  }
  if (func_name.find("operator &") != string::npos) {
    return V_SgBitAndOp;
  }
  if (func_name.find("operator |") != string::npos) {
    return V_SgBitOrOp;
  }
  if (func_name.find("operator ^") != string::npos) {
    return V_SgBitXorOp;
  }
  if (func_name.find("operator =") != string::npos) {
    return V_SgAssignOp;
  }
  if (func_name.find("operator long") != string::npos) {
    return V_SgOperatorLong;
  }
  if (func_name.find("operator unsigned long") != string::npos) {
    return V_SgOperatorULong;
  }
  if (func_name.find("operator float") != string::npos) {
    return V_SgOperatorFloat;
  }

  if (func_name.find("operator++") != string::npos) {
    return V_SgPlusPlusOp;
  }
  if (func_name.find("operator--") != string::npos) {
    return V_SgMinusMinusOp;
  }
  if (func_name.find("operator<<=") != string::npos) {
    return V_SgLshiftAssignOp;
  }
  if (func_name.find("operator>>=") != string::npos) {
    return V_SgRshiftAssignOp;
  }
  if (func_name.find("operator>=") != string::npos) {
    return V_SgGreaterOrEqualOp;
  }
  if (func_name.find("operator==") != string::npos) {
    return V_SgEqualityOp;
  }
  if (func_name.find("operator<=") != string::npos) {
    return V_SgLessOrEqualOp;
  }
  if (func_name.find("operator!=") != string::npos) {
    return V_SgNotEqualOp;
  }
  if (func_name.find("operator+=") != string::npos) {
    return V_SgPlusAssignOp;
  }
  if (func_name.find("operator-=") != string::npos) {
    return V_SgMinusAssignOp;
  }
  if (func_name.find("operator*=") != string::npos) {
    return V_SgMultAssignOp;
  }
  if (func_name.find("operator/=") != string::npos) {
    return V_SgDivAssignOp;
  }
  if (func_name.find("operator%=") != string::npos) {
    return V_SgModAssignOp;
  }
  if (func_name.find("operator|=") != string::npos) {
    return V_SgIorAssignOp;
  }
  if (func_name.find("operator^=") != string::npos) {
    return V_SgXorAssignOp;
  }
  if (func_name.find("operator&=") != string::npos) {
    return V_SgAndAssignOp;
  }
  if (func_name.find("operator<") != string::npos) {
    return V_SgLessThanOp;
  }
  if (func_name.find("operator>") != string::npos) {
    return V_SgGreaterThanOp;
  }
  if (func_name.find("operator+") != string::npos) {
    return V_SgAddOp;
  }
  if (func_name.find("operator-") != string::npos) {
    return V_SgSubtractOp;
  }
  if (func_name.find("operator*") != string::npos) {
    return V_SgMultiplyOp;
  }
  if (func_name.find("operator/") != string::npos) {
    return V_SgDivideOp;
  }
  if (func_name.find("operator%") != string::npos) {
    return V_SgModOp;
  }
  if (func_name.find("operator&") != string::npos) {
    return V_SgBitAndOp;
  }
  if (func_name.find("operator|") != string::npos) {
    return V_SgBitOrOp;
  }
  if (func_name.find("operator^") != string::npos) {
    return V_SgBitXorOp;
  }
  if (func_name.find("operator=") != string::npos) {
    return V_SgAssignOp;
  }
  if (func_name.find("operator,") != string::npos) {
    return V_SgCommaOpExp;
  }

  if (func_name.find("operatorlong") != string::npos) {
    return V_SgOperatorLong;
  }
  if (func_name.find("operatorunsigned long") != string::npos) {
    return V_SgOperatorULong;
  }
  if (func_name.find("operatorfloat") != string::npos) {
    return V_SgOperatorFloat;
  }
  return 0;
}

//  remove ap_int/ap_fixed constructor/implicit conversion
void *CSageCodeGen::RemoveAPIntFixedCast(void *exp) {
  SgConstructorInitializer *c_tor =
      isSgConstructorInitializer(static_cast<SgNode *>(exp));
  if (c_tor != nullptr) {
    if (GetFuncCallParamNum(c_tor) != 1) {
      return exp;
    }
    if (auto *cls_decl = c_tor->get_class_decl()) {
      if (xil_ap.count(GetClassName(cls_decl)) > 0) {
        return RemoveAPIntFixedCast(GetFuncCallParam(c_tor, 0));
      }
    }
  }
  SgFunctionCallExp *call_exp = isSgFunctionCallExp(static_cast<SgNode *>(exp));
  if (call_exp != nullptr) {
    void *decl = GetFuncDeclByCall(exp, 0);
    if (decl == nullptr) {
      return exp;
    }
    if (IsImplicitConversionOp(GetOperatorOp(decl)) == 0) {
      return exp;
    }
    SgExpression *func_exp = call_exp->get_function();
    if (IsDotExp(func_exp) != 0) {
      if (auto *imf = isSgTemplateInstantiationMemberFunctionDecl(
              static_cast<SgNode *>(decl))) {
        auto *sg_class_decl = isSgTemplateInstantiationDecl(
            imf->get_associatedClassDeclaration());
        if ((sg_class_decl != nullptr) &&
            (xil_ap.count(sg_class_decl->get_templateName().getString()) !=
             0U)) {
          return GetExpLeftOperand(func_exp);
        }
      }
    }
  }
  return exp;
}

bool CSageCodeGen::IsAPIntOp(void *expr, void **left_exp, void **right_exp,
                             int *op) {
  void *local_expr = expr;
  auto func_call_expr = isSgFunctionCallExp(static_cast<SgNode *>(local_expr));
  if (func_call_expr == nullptr) {
    return false;
  }
  void *decl = GetFuncDeclByCall(local_expr, 0);
  if (decl == nullptr) {
    return false;
  }
  *op = GetOperatorOp(decl);
  if (*op == 0) {
    return false;
  }
  SgExpression *call_expr =
      isSgFunctionCallExp(static_cast<SgNode *>(func_call_expr))
          ->get_function();
  if (IsDotExp(call_expr) != 0) {
    if (auto *imf = isSgTemplateInstantiationMemberFunctionDecl(
            static_cast<SgNode *>(decl))) {
      auto *sg_class_decl =
          isSgTemplateInstantiationDecl(imf->get_associatedClassDeclaration());
      if ((sg_class_decl != nullptr) &&
          (xil_ap.count(sg_class_decl->get_templateName().getString()) != 0U)) {
        if (GetFuncCallParamNum(func_call_expr) > 0) {
          *right_exp = GetFuncCallParam(func_call_expr, 0);
        }
        *left_exp = GetExpLeftOperand(call_expr);
        return true;
      }
    }
  }
  if (IsFunctionRefExp(call_expr) != 0) {
    unsigned arg_size = GetFuncCallParamNum(func_call_expr);
    if (auto *id = isSgTemplateInstantiationFunctionDecl(
            static_cast<SgNode *>(decl))) {
      void *arg_0 =
          arg_size > 0 ? GetFuncCallParam(func_call_expr, 0) : nullptr;
      void *arg_1 =
          arg_size > 1 ? GetFuncCallParam(func_call_expr, 1) : nullptr;
      bool res = false;
      if (arg_0 != nullptr) {
        arg_0 = RemoveAPIntFixedCast(arg_0);
        void *exp_type = GetTypeByExp(arg_0);
        if ((IsPointerType(exp_type) == 0) && (IsArrayType(exp_type) == 0)) {
          void *base_type = GetBaseType(exp_type, true);
          if ((IsXilinxAPIntType(base_type) != 0) ||
              (IsXilinxAPFixedType(base_type) != 0)) {
            res = true;
          }
          *left_exp = arg_0;
        }
      }
      if (arg_1 != nullptr) {
        arg_1 = RemoveAPIntFixedCast(arg_1);
        void *exp_type = GetTypeByExp(arg_1);
        if ((IsPointerType(exp_type) == 0) && (IsArrayType(exp_type) == 0)) {
          void *base_type = GetBaseType(exp_type, true);
          if ((IsXilinxAPIntType(base_type) != 0) ||
              (IsXilinxAPFixedType(base_type) != 0)) {
            res = true;
          }
          *right_exp = arg_1;
        }
      }

      return res;
    }
  }
  return false;
}

int CSageCodeGen::IsCompoundAssignOp(int opcode) {
  return static_cast<int>(
      (V_SgAndAssignOp == opcode) || (V_SgDivAssignOp == opcode) ||
      (V_SgIorAssignOp == opcode) || (V_SgLshiftAssignOp == opcode) ||
      (V_SgRshiftAssignOp == opcode) || (V_SgMinusAssignOp == opcode) ||
      (V_SgModAssignOp == opcode) || (V_SgMultAssignOp == opcode) ||
      (V_SgPlusAssignOp == opcode) || (V_SgRshiftAssignOp == opcode) ||
      (V_SgXorAssignOp == opcode));
}

int CSageCodeGen::IsBinaryOp(int opcode) {
  return static_cast<int>(
      (V_SgAddOp == opcode) || (V_SgSubtractOp == opcode) ||
      (V_SgMultiplyOp == opcode) || (V_SgDivideOp == opcode) ||
      (V_SgAndOp == opcode) || (V_SgArrowExp == opcode) ||
      (V_SgAssignOp == opcode) || (V_SgBitAndOp == opcode) ||
      (V_SgBitOrOp == opcode) || (V_SgBitXorOp == opcode) ||
      (V_SgDotExp == opcode) || (V_SgLshiftOp == opcode) ||
      (V_SgRshiftOp == opcode) || (V_SgModOp == opcode) ||
      (V_SgOrOp == opcode) || (V_SgPntrArrRefExp == opcode) ||
      (IsCompareOp(opcode) != 0) || (IsCompoundAssignOp(opcode)) != 0);
}

void *CSageCodeGen::GetOriginalNode(void *sg_node) {
  void *sg_func = TraceUpToFuncDecl(sg_node);
  if ((sg_func == nullptr) ||
      ((IsTemplateInstantiationFunctionDecl(sg_func) == 0) &&
       (IsTemplateInstantiationMemberFunctionDecl(sg_func) == 0)) ||
      !IsCompilerGenerated(sg_func)) {
    return sg_node;
  }
  void *sg_orig_func = nullptr;
  if (IsTemplateInstantiationFunctionDecl(sg_func) != 0) {
    sg_orig_func = GetTemplateFuncDecl(sg_func);
  }
  if (IsTemplateInstantiationMemberFunctionDecl(sg_func) != 0) {
    sg_orig_func = GetTemplateMemFuncDecl(sg_func);
  }
  if (sg_orig_func == nullptr) {
    return sg_node;
  }
  if (IsTemplateInstantiationFunctionDecl(sg_node) != 0) {
    return sg_orig_func;
  }
  if (IsTemplateInstantiationMemberFunctionDecl(sg_node) != 0) {
    return sg_orig_func;
  }
  string topo_id = getTopoLocation(this, static_cast<SgNode *>(sg_node),
                                   static_cast<SgNode *>(GetParent(sg_func)));
  if (topo_id.empty()) {
    return sg_node;
  }
  void *orig_node =
      getSgNode(this, topo_id, static_cast<SgNode *>(sg_orig_func));
  if (orig_node != nullptr) {
    return orig_node;
  }
  return sg_node;
}

void *CSageCodeGen::CreateLabelStmt(const string &label, void *stmt,
                                    void *scope, void *bindNode) {
  SgLabelStatement *ls =
      SageBuilder::buildLabelStatement(label, static_cast<SgStatement *>(stmt),
                                       static_cast<SgScopeStatement *>(scope));
  if (bindNode != nullptr) {
    markBuild(this, ls,
              "Build in " + std::to_string(__LINE__) + " for " +
                  std::to_string((int64_t)ls),
              bindNode);
  }
  return ls;
}

bool CSageCodeGen::IsGeneralAssignOp(void *expr, void **left_exp,
                                     void **right_exp) {
  if (IsAssignOp(expr) != 0) {
    GetExpOperand(expr, left_exp, right_exp);
    return true;
  }
  int ap_op = 0;
  if (IsAPIntOp(expr, left_exp, right_exp, &ap_op) && V_SgAssignOp == ap_op) {
    return true;
  }
  if (IsFunctionCall(expr)) {
    void *func_call = expr;
    void *func_decl = GetFuncDeclByCall(func_call, 0);
    if (IsAssignOperator(func_decl)) {
      SgExpression *call_expr =
          isSgFunctionCallExp(static_cast<SgNode *>(func_call))->get_function();
      if (IsDotExp(call_expr) != 0 || IsArrowExp(call_expr) != 0) {
        if (GetFuncCallParamNum(expr) > 0) {
          *right_exp = GetFuncCallParam(func_call, 0);
          *left_exp = GetExpLeftOperand(call_expr);
          return true;
        }
      }
    }
  }
  return false;
}

// we can get the struct variables but cannot get the struct fields
// e.g.a->b
// 'a' can be collected but 'b' can not
map<void *, string> CSageCodeGen::GetVariablesUsedByPragma(void *pragma,
                                                           bool *report_err,
                                                           string *msg,
                                                           bool numeric_only) {
  map<void *, string> res;
  void *scope = GetScope(pragma);
  if (nullptr == scope) {
    return res;
  }
  string str_pragma = GetPragmaString(pragma);
  vector<string> tokens;
  GetTokens(&tokens, str_pragma);
  bool numeric_option = false;
  string last_token;
  bool is_value = false;
  for (auto &token : tokens) {
    if (token == " ") {
      continue;
    }
    // Maybe several tokens are value, e.g.
    // factor=-(2*3)
    // FIXME: what if "#pragma ACCEL parallel factor=(a + b) complete"
    if (token.size() == 1 && numeric_prefix.count(token[0]) > 0) {
      is_value = true;
      if (token == "=") {
        string option = last_token;
        boost::algorithm::to_lower(option);
        numeric_option = is_numeric_option(option, numeric_only);
      }
      continue;
    }
    // Yuxin: Feb 14 2020
    // Handle cases which contain class/namespace
    // e.g. variable = a::b::var
    // is_value is used to skip the check of attribute keywords
    if (is_value && numeric_option) {
      void *sg_var = getInitializedNameFromName(GetScope(pragma), token, false);
      if (sg_var != nullptr) {
        res[sg_var] = token;
      } else {
        // DSE mode
        if (FindTokenAttribute(tokens, "FACTOR") == token &&
            (token == "auto" || token == "AUTO"))
          continue;
        if (FindTokenAttribute(tokens, "FACTOR") == token ||
            FindTokenAttribute(tokens, "VARIABLE") == token ||
            FindTokenAttribute(tokens, "DEPTH") == token ||
            FindTokenAttribute(tokens, "DIM") == token ||
            FindTokenAttribute(tokens, "II") == token ||
            FindTokenAttribute(tokens, "MAX") == token) {
          if (atol(token.c_str()) != 0 ||
              (atol(token.c_str()) == 0 && token == "0"))
            continue;
          if (report_err != nullptr)
            *report_err = true;
          if (msg != nullptr)
            *msg = token;
        }
      }
      is_value = false;
    }
    last_token = token;
  }
  return res;
}

bool CSageCodeGen::IsPragmaWithDeadVariable(void *pragma) {
  void *scope = GetScope(pragma);
  if (nullptr == scope) {
    return false;
  }
  string str_pragma = GetPragmaString(pragma);
  vector<string> tokens;
  GetTokens(&tokens, str_pragma);
  bool is_value = false;
  bool variable_option = false;
  string last_token;
  for (auto &token : tokens) {
    if (token == " ") {
      continue;
    }
    if (is_value && variable_option) {
      void *sg_var = getInitializedNameFromName(GetScope(pragma), token, false);
      if (sg_var == nullptr) {
        return true;
      }
    }
    is_value = token.size() == 1 && token[0] == ',';
    if (token == "=") {
      string option = last_token;
      boost::algorithm::to_lower(option);
      variable_option = option == "variable";
    }
    last_token = token;
  }
  return false;
}

SetVector<void *>
CSageCodeGen::GetVariablesUsedByPragmaInScope(void *scope, bool numeric_only) {
  vector<void *> vec_pragma;
  GetNodesByType(scope, "preorder", "SgPragmaDeclaration", &vec_pragma);
  SetVector<void *> res;
  for (auto pragma : vec_pragma) {
    bool err = false;
    string token_str = "";
    auto sub_res =
        GetVariablesUsedByPragma(pragma, &err, &token_str, numeric_only);
    for (auto var_pair : sub_res)
      res.insert(var_pair.first);
  }
  return res;
}

void CSageCodeGen::splitInlinedAggregatedDefinition(void *scope) {
  vector<void *> vec_decls;
  GetNodesByType_int(scope, "preorder", V_SgClassDeclaration, &vec_decls);
  for (auto decl : vec_decls) {
    if (!IsFromInputFile(decl) && !IsTransformation(decl))
      continue;
    void *decl_parent = GetParent(decl);
    if (auto var_decl =
            isSgVariableDeclaration(static_cast<SgNode *>(decl_parent))) {
      SgClassDeclaration *sg_class_decl =
          isSgClassDeclaration(static_cast<SgNode *>(decl));
      if (sg_class_decl->get_isUnNamed()) {
        string new_type_name = GetVariableName(decl_parent, false);
        SgScopeStatement *sg_scope = sg_class_decl->get_scope();
        int suffix = 0;
        while (sg_scope != nullptr &&
               sg_scope->lookup_class_symbol(new_type_name) != nullptr) {
          new_type_name =
              GetVariableName(decl_parent, false) + "_" + my_itoa(++suffix);
        }
        sg_class_decl->set_name(new_type_name);
        sg_class_decl->set_isUnNamed(false);
        auto sg_first_decl = isSgClassDeclaration(
            sg_class_decl->get_firstNondefiningDeclaration());
        if (sg_first_decl != nullptr && sg_first_decl != sg_class_decl) {
          sg_first_decl->set_name(new_type_name);
          sg_first_decl->set_isUnNamed(false);
          if (sg_scope != nullptr) {
            sg_scope->insert_symbol(new_type_name,
                                    new SgClassSymbol(sg_first_decl));
          }
        }
      }
      sg_class_decl->set_parent(nullptr);
      var_decl->set_baseTypeDefiningDeclaration(nullptr);
      InsertStmt(sg_class_decl, decl_parent);
    }
  }
}

void CSageCodeGen::removeSpaces(string *stringIn) {
  string::size_type pos = 0;
  bool spacesLeft = true;

  while (spacesLeft) {
    pos = stringIn->find(" ");

    if (pos != string::npos) {
      stringIn->erase(pos, 1);
    } else {
      spacesLeft = false;
    }
  }
}

bool CSageCodeGen::IsStructureWithAlignedScalarData(void *type) {
  int total_bitwidth, aligned_bitwidth;
  total_bitwidth = aligned_bitwidth = 0;
  return IsStructureWithAlignedScalarData(type, &total_bitwidth,
                                          &aligned_bitwidth);
}

bool CSageCodeGen::IsStructureWithAlignedScalarData(void *type,
                                                    int *total_bitwidth,
                                                    int *aligned_bitwidth) {
  while ((isSgModifierType(static_cast<SgType *>(type)) != nullptr) ||
         (isSgReferenceType(static_cast<SgType *>(type)) != nullptr) ||
         (isSgTypedefType(static_cast<SgType *>(type)) != nullptr)) {
    if (isSgReferenceType(static_cast<SgType *>(type)) != nullptr) {
      if (IsNonConstReferenceType(type) != 0) {
        break;
      }
      //  strip constant reference type
      type = GetBaseTypeOneLayer(type);
    }
    type = GetBaseTypeOneLayer(type);
  }

  if (!IsStructureType(type) && !IsClassType(type)) {
    return false;
  }
  vector<void *> vec_members;
  GetClassDataMembers(type, &vec_members);
  if (vec_members.empty()) {
    return true;
  }
  int last_aligned_bitwidth = 0;
  int res_total_bitwidth = 0;
  for (auto member : vec_members) {
    auto var_decl = static_cast<SgVariableDeclaration *>(member);
    if (var_decl->get_bitfield() != nullptr) {
      return false;
    }
    void *var_init = GetVariableInitializedName(member);
    if (IsAnonymousName(GetVariableName(var_init))) {
      return false;
    }
    void *var_type = GetOrigTypeByTypedef(GetTypebyVar(var_init), true);
    int curr_aligned_bitwidth = 0;
    int curr_total_bitwidth = 0;
    if (!IsStructureType(var_type) && !IsClassType(var_type)) {
      if (IsScalarType(var_type)) {
        curr_total_bitwidth = curr_aligned_bitwidth =
            get_bitwidth_from_type(var_type, false);
      } else if (IsPointerType(var_type) || IsArrayType(var_type) ||
                 IsReferenceType(var_type)) {
        return false;
      }
    } else {
      if (IsStructureWithAlignedScalarData(var_type, &curr_total_bitwidth,
                                           &curr_total_bitwidth)) {
        if (curr_total_bitwidth == 0)
          continue;
      } else {
        return false;
      }
    }
    if (last_aligned_bitwidth == 0 ||
        last_aligned_bitwidth == curr_aligned_bitwidth) {
      last_aligned_bitwidth = curr_aligned_bitwidth;
      res_total_bitwidth += curr_total_bitwidth;
    } else {
      return false;
    }
  }
  *total_bitwidth = res_total_bitwidth;
  *aligned_bitwidth = last_aligned_bitwidth;
  return true;
}

bool CSageCodeGen::IsDataflowScope(void *scope) {
  vector<void *> pragmas;
  int is_compatible = 1;
  GetNodesByType(scope, "preorder", "SgPragmaDeclaration", &pragmas,
                 is_compatible != 0);
  CMarsIrV2 mars_ir(this);
  for (auto one_pragma : pragmas) {
    bool allow_hls = true;
    string pragma_name =
        mars_ir.get_pragma_attribute(one_pragma, "dataflow", allow_hls);
    boost::algorithm::to_lower(pragma_name);
    if (pragma_name == "dataflow") {
      return true;
    }
  }
  return false;
}

bool CSageCodeGen::IsParallelLoop(void *loop) {
  vector<void *> pragmas;
  int is_compatible = 1;
  GetNodesByType(loop, "preorder", "SgPragmaDeclaration", &pragmas,
                 is_compatible != 0);
  CMarsIrV2 mars_ir(this);
  for (auto one_pragma : pragmas) {
    if (TraceUpToLoopScope(one_pragma) != loop)
      continue;
    bool allow_hls = true;
    string pragma_name =
        mars_ir.get_pragma_attribute(one_pragma, "unroll", allow_hls);
    boost::algorithm::to_lower(pragma_name);
    if (pragma_name == "unroll") {
      return true;
    }
    pragma_name = mars_ir.get_pragma_attribute(one_pragma, "parallel", false);
    boost::algorithm::to_lower(pragma_name);
    if (pragma_name == "parallel") {
      return true;
    }
  }
  return false;
}

bool CSageCodeGen::func_has_dataflow(void *func_decl) {
  if ((func_decl == nullptr) || (GetFuncBody(func_decl) == nullptr)) {
    return false;
  }
  return IsDataflowScope(GetFuncBody(func_decl));
}

bool CSageCodeGen::IsDataflowProcess(void *func_decl) {
  vector<void *> vec_calls;
  GetFuncCallsFromDecl(func_decl, nullptr, &vec_calls);
  for (auto call : vec_calls) {
    void *loop = TraceUpToLoopScope(call);
    if (nullptr != loop) {
      if (IsDataflowScope(loop))
        return true;
    }
    if (IsDataflowScope(TraceUpToFuncDecl(call)))
      return true;
  }
  return false;
}

bool CSageCodeGen::IsMerlinInternalIntrinsic(string func_name) {
  if (func_name.find("__merlin_access_range") == 0 ||
      func_name.find("__merlin_streaming_range") == 0) {
    return true;
  }
  return false;
}

bool CSageCodeGen::simplify_pointer_access(void *scope) {
  //  5. replace *(&a), &(*a) with a
  bool Changed = false;
  vector<void *> vec_addressof;
  GetNodesByType_int(scope, "postorder", V_SgAddressOfOp, &vec_addressof);
  for (auto &addressof_op : vec_addressof) {
    void *parent = GetParent(addressof_op);
    if ((parent != nullptr) && (IsPointerDerefExp(parent) != 0)) {
      void *orig_exp = GetExpUniOperand(addressof_op);
      ReplaceExp(parent, CopyExp(orig_exp));
      Changed = true;
    } else {
      void *orig_exp = GetExpUniOperand(addressof_op);
      if (IsPointerDerefExp(orig_exp) != 0) {
        ReplaceExp(addressof_op, CopyExp(GetExpUniOperand(orig_exp)));
        Changed = true;
      }
    }
  }
  return Changed;
}

vector<void *> CSageCodeGen::GetInheritances(void *class_decl) {
  void *class_def = GetClassDefinition(class_decl);
  if (class_def == nullptr) {
    return vector<void *>{};
  }
  auto sg_class_def = isSgClassDefinition(static_cast<SgNode *>(class_def));
  if (sg_class_def == nullptr) {
    return vector<void *>{};
  }
  vector<void *> res;
  for (SgBaseClass *ba : sg_class_def->get_inheritances()) {
    SgClassDeclaration *cls = ba->get_base_class();
    res.push_back(cls);
  }
  return res;
}

vector<void *> CSageCodeGen::GetDataMemberExp(void *var_decl, void *mem_init) {
  vector<void *> res;
  void *var_init = GetVariableInitializedName(var_decl);
  if (!var_init)
    return res;
  void *var_type = GetTypebyVar(var_init);
  void *base_type = GetBaseType(var_type, true);
  if (IsStructureType(base_type) == 0 && IsClassType(base_type) == 0) {
    return res;
  }
  vector<void *> vec_members;
  GetClassDataMembers(base_type, &vec_members);
  for (auto member : vec_members) {
    void *curr_mem_init = GetVariableInitializedName(member);
    if (curr_mem_init == mem_init) {
      auto pntr_vec = CreatePntrExp(var_init);
      for (auto pntr : pntr_vec) {
        void *member_exp = CreateVariableRef(mem_init);
        void *dot_exp = CreateExp(V_SgDotExp, pntr, member_exp);
        res.push_back(dot_exp);
      }
      continue;
    }
    vector<void *> sub_res = GetDataMemberExp(member, mem_init);
    for (auto sub_exp : sub_res) {
      auto pntr_vec = CreatePntrExp(var_init);
      int i = 0;
      for (auto pntr : pntr_vec) {
        void *dot_exp =
            CreateExp(V_SgDotExp, pntr, i == 0 ? sub_exp : CopyExp(sub_exp));
        res.push_back(dot_exp);
        i++;
      }
    }
  }
  return res;
}

vector<void *> CSageCodeGen::CreatePntrExp(void *var_init) {
  vector<void *> res;
  void *sg_type = GetTypebyVar(var_init);
  vector<size_t> sg_sizes;
  void *sg_base_type;
  get_type_dimension(sg_type, &sg_base_type, &sg_sizes, var_init);
  if (sg_sizes.empty()) {
    res.push_back(CreateVariableRef(var_init));
    return res;
  }
  for (auto &dim_size : sg_sizes) {
    if (!IsDeterminedSize(dim_size)) {
      dim_size = 1;
    }
  }
  std::reverse(sg_sizes.begin(), sg_sizes.end());
  int dim = 0;
  for (auto dim_size : sg_sizes) {
    vector<void *> tmp_res;
    for (size_t i = 0; i < dim_size; i++) {
      if (dim == 0) {
        res.push_back(CreateExp(V_SgPntrArrRefExp, CreateVariableRef(var_init),
                                CreateConst(static_cast<int>(i))));
      } else {
        for (auto pntr : res) {
          tmp_res.push_back(CreateExp(V_SgPntrArrRefExp,
                                      i == 0 ? pntr : CopyExp(pntr),
                                      CreateConst(static_cast<int>(i))));
        }
      }
    }
    if (dim > 0)
      res.swap(tmp_res);
    dim++;
  }
  return res;
}

bool CSageCodeGen::detect_user_for_loop_burst(void *ref, void *pntr,
                                              CMarsIr *mars_ir) {
  //  1. check whether the immediate loop of 'pntr' is a for-loop
  void *loop = TraceUpToLoopScope(pntr);
  if ((loop == nullptr) || (IsForStatement(loop) == 0) ||
      IsParallelLoop(loop)) {
    return false;
  }
  void *iv;
  void *lb;
  void *ub;
  void *step;
  void *body;
  iv = lb = ub = step = body = nullptr;
  bool inc_dir;
  bool inclusive_bound;
  inc_dir = inclusive_bound = false;
  if (IsCanonicalForLoop(loop, &iv, &lb, &ub, &step, &body, &inc_dir,
                         &inclusive_bound) != 0) {
    int64_t nStep = 0;
    int ret = GetLoopStepValueFromExpr(step, &nStep);
    if ((step != nullptr) && (GetParent(step) == nullptr)) {
      DeleteNode(step);
    }
    if ((ret == 0) || nStep != 1) {
      return false;
    }
  } else {
    return false;
  }

  //  2. check whether the index of external memory is linear to for-loop
  {
    CMarsAccess access(ref, this, loop);
    CMarsExpression index = access.GetIndexes()[0];
    map<void *, int64_t> iterators;
    map<void *, int64_t> invariants;
    if (!index.IsStandardForm(loop, &iterators, &invariants)) {
      return false;
    }
    if (iterators.count(loop) <= 0 || iterators[loop] != 1) {
      return false;
    }
  }
  //  3. check whether it is conditional assignment
  {
    void *loop_body = GetLoopBody(loop);
    void *parent = GetParent(ref);
    while (parent != loop_body) {
      if (IsIfStatement(parent) || IsSwitchStatement(parent)) {
        return false;
      }
      parent = GetParent(parent);
    }
  }
  bool burst_only = false;
  bool burst_with_coalescing = false;
  bool burst_with_streaming = false;
  //  4.1 check whether the index of on-chip buffer is linear to for-loop
  {
    //  check whether 'pntr' is assigned to or from a local buffer
    void *assign_op = GetParent(pntr);
    if (IsDotExp(assign_op) && IsFunctionCall(GetParent(assign_op))) {
      assign_op = GetParent(assign_op);
    }
    void *rhs;
    void *lhs;
    rhs = lhs = nullptr;
    if (IsGeneralAssignOp(assign_op, &rhs, &lhs)) {
      void *other = rhs == pntr ? lhs : rhs;
      if (other != nullptr) {
        void *local_buf_ref = nullptr;
        parse_array_ref_from_pntr(other, &local_buf_ref, other, false);
        void *local_buf = nullptr;
        if ((local_buf_ref != nullptr) && (IsVarRefExp(local_buf_ref) != 0)) {
          local_buf = GetVariableInitializedName(local_buf_ref);
        }
        map<void *, bool> res;
        if ((local_buf != nullptr) &&
            !mars_ir->any_trace_is_bus(this, TraceUpToFuncDecl(pntr), local_buf,
                                       &res, assign_op)) {
          bool buf_index_linear_to_loop = false;
          CMarsAccess access(local_buf_ref, this, loop);
          for (auto index : access.GetIndexes()) {
            map<void *, int64_t> iterators;
            map<void *, int64_t> invariants;
            if (!index.IsStandardForm(loop, &iterators, &invariants)) {
              continue;
            }
            if (iterators.count(loop) <= 0 || iterators[loop] == 0) {
              continue;
            }
            buf_index_linear_to_loop = true;
            break;
          }
          if (buf_index_linear_to_loop) {
            burst_only = true;
          }
        }
      }
    }
  }
  // 4.2 check whether it is burst combined with coalescing
  // e.g.
  // for (i=0...100) {
  //   tmp = a[i];
  //   for (j=0...16) {
  //     a_buf[i * 16 + j] = tmp.range(j * 32 + 31, j * 32);
  //   }
  // }
  {
    void *parent = GetParent(pntr);
    if (IsDotExp(parent) && IsFunctionCall(GetParent(parent))) {
      parent = GetParent(parent);
    }
    void *rhs;
    void *lhs;
    rhs = lhs = nullptr;
    void *tmp_buf = nullptr;
    void *tmp_ref = nullptr;
    if (IsGeneralAssignOp(parent, &rhs, &lhs)) {
      void *other = rhs == pntr ? lhs : rhs;
      if (other != nullptr) {
        if (IsVarRefExp(other) != 0) {
          tmp_buf = GetVariableInitializedName(other);
          tmp_ref = other;
        }
      }
    } else if (IsInitializer(parent)) {
      tmp_buf = GetParent(parent);
    }
    if (tmp_buf != nullptr) {
      vector<void *> vec_refs;
      get_ref_in_scope(tmp_buf, loop, &vec_refs);
      void *other_tmp_ref = nullptr;
      for (auto one_ref : vec_refs) {
        if (one_ref == tmp_ref)
          continue;
        if (other_tmp_ref == nullptr) {
          other_tmp_ref = one_ref;
        } else {
          other_tmp_ref = nullptr;
          break;
        }
      }
      if (other_tmp_ref != nullptr) {
        void *tmp_ref_parent = GetParent(other_tmp_ref);
        if (IsDotExp(tmp_ref_parent)) {
          void *rhs = GetExpRightOperand(tmp_ref_parent);
          if (IsMemberFunctionRefExp(rhs)) {
            void *mem_func = GetFuncDeclByRef(rhs);
            string func_name = GetFuncName(mem_func);
            if (func_name == "range" || func_name == "operator()" ||
                func_name == "operator ()") {
              burst_with_coalescing = true;
            }
          }
        }
      }
    }
  }
  {
    // check whether burst with streaming
    // e.g. for (i = 0...100) {
    //        a[i] = stream_b.read();
    //        or
    //          stream_b >> a[i];
    //        or
    //          stream_b.write(a[i]);
    //        or
    //          stream_b << a[i];
    //        }
    void *assign_exp = GetParent(pntr);
    void *left_exp = nullptr;
    void *assign_val = nullptr;
    if (IsGeneralAssignOp(assign_exp, &left_exp, &assign_val)) {
      void *ref = nullptr;
      if (left_exp == pntr && is_xilinx_channel_write(assign_val, &ref)) {
        burst_with_streaming = true;
      }
    } else {
      if (GetFuncCallParamIndex(pntr) != -1) {
        void *func_call = TraceUpToFuncCall(pntr);
        if (is_xilinx_channel_write(func_call, &ref) ||
            is_xilinx_channel_read(func_call, &ref)) {
          burst_with_streaming = true;
        }
      }
    }
  }
  return burst_only || burst_with_coalescing || burst_with_streaming;
}

bool CSageCodeGen::IsInputOutputGlobalVariable(void *var_init) {
  static const std::unordered_set<string> input_output_global_variable{
      "stderr", "stdout", "stdin", "std::cout", "std::cin", "std::cerr"};
  if (IsGlobalInitName(var_init)) {
    string var_name = GetVariableName(var_init, true);
    if (var_name.find("::") == 0)
      var_name = var_name.substr(2);
    return input_output_global_variable.count(var_name) > 0;
  }
  return false;
}

// //[Han] kick task out of kernels
// vector<string> CSageCodeGen::GetFirstDim(string file_name) {
//  vector<string> exist_task;
//  if (FILE *file = fopen(file_name.c_str(), "r")) {
//    fclose(file);
//    map<string, map<string, string>> task_kernel;
//    readInterReport(file_name, &task_kernel);
//    map<string, map<string, string>>::iterator iter0;
//    for (iter0 = task_kernel.begin(); iter0 != task_kernel.end(); iter0++) {
//      string task_name = iter0->first;
//      exist_task.push_back(task_name);
//    }
//  }
//  return exist_task;
//  }

#if USED_CODE_IN_COVERAGE_TEST
#include "codegen->cpp.dead"
#endif
