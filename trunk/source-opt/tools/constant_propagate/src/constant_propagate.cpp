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


#include "interface_transform.h"

using MarsProgramAnalysis::CMarsExpression;
using MarsProgramAnalysis::CMarsRangeExpr;

using std::map;
using std::set;
using std::unordered_map;
using std::vector;

int constant_propagation(CSageCodeGen *codegen, void *func,
                         set<void *> *visited_funcs,
                         set<void *> *assert_header_file,
                         map<void *, int64_t> *visited_vars, bool is_top,
                         bool assert_generation) {
  if (visited_funcs->count(func) > 0) {
    return 0;
  }
  visited_funcs->insert(func);
  int ret = 0;
  int cross_ret = 0;
  if (!is_top) {
    //  propagate constant argument
    vector<void *> calls;
    codegen->GetFuncCallsFromDecl(func, nullptr, &calls);
    void *func_body = codegen->GetFuncBody(func);
    for (int i = 0; i < codegen->GetFuncParamNum(func); ++i) {
      void *param = codegen->GetFuncParam(func, i);
      void *param_type = codegen->GetTypebyVar(param);
      string param_name = codegen->GetVariableName(param);
      if (codegen->IsIntegerType(param_type) == 0) {
        continue;
      }
      vector<void *> vec_refs;
      codegen->get_ref_in_scope(param, func_body, &vec_refs);
      if (vec_refs.empty()) {
        continue;
      }
      bool all_is_const = true;
      int64_t min_val, max_val;
      min_val = std::numeric_limits<int64_t>::max();
      max_val = std::numeric_limits<int64_t>::min();
      for (auto call : calls) {
        void *actual_arg = codegen->GetFuncCallParam(call, i);
        CMarsExpression me(actual_arg, codegen, actual_arg,
                           codegen->TraceUpToFuncDecl(call));
        if (me.IsConstant() != 0) {
          min_val = std::min(me.GetConstant(), min_val);
          max_val = std::max(me.GetConstant(), max_val);
        } else {
          all_is_const = false;
          break;
        }
      }
      if (!all_is_const) {
        continue;
      }
      if (min_val == max_val) {
        int64_t val = min_val;
        void *new_init = nullptr;
        if (val >= std::numeric_limits<int>::min() &&
            val < std::numeric_limits<int>::max()) {
          new_init = codegen->CreateConst(static_cast<int>(val));
        } else {
          new_init = codegen->CreateConst(val);
        }
        string new_name = param_name + "_copy";
        codegen->get_valid_local_name(func, &new_name);
        void *param_copy = codegen->CreateVariableDecl(param_type, new_name,
                                                       new_init, func_body);
        codegen->PrependChild(func_body, param_copy);
        codegen->replace_variable_references_in_scope(param, param_copy,
                                                      func_body);
        // Yuxin: propagate values in the pragma
        set<void *> pragmas = codegen->get_pragma_related_to_variable(param);
        unordered_map<string, string> replace_defs;
        replace_defs[param_name] = my_itoa(val);
        for (auto pragma : pragmas) {
          codegen->RewritePragmaTokens(pragma, replace_defs);
        }
      } else if (assert_generation) {
        void *cond_exp = codegen->CreateExp(V_SgGreaterOrEqualOp,
                                            codegen->CreateVariableRef(param),
                                            codegen->CreateConst(min_val));
        cond_exp = codegen->CreateExp(
            V_SgAndOp,
            codegen->CreateExp(V_SgLessOrEqualOp,
                               codegen->CreateVariableRef(param),
                               codegen->CreateConst(max_val)),
            cond_exp);
        void *global = codegen->GetGlobal(func);
        if (assert_header_file->count(global) <= 0) {
          codegen->AddHeader("\n#include<assert.h>\n", global);
          assert_header_file->insert(global);
        }

        void *assert_call =
            codegen->CreateFuncCall("assert", codegen->GetTypeByString("void"),
                                    vector<void *>{cond_exp}, func_body);
        codegen->PrependChild(
            func_body, codegen->CreateStmt(V_SgExprStatement, assert_call));
      }
      cross_ret = 1;
      ret = 1;
    }
  }
  if (cross_ret != 0) {
    codegen->init_defuse();
  }

  // Yuxin: Dec/10/2019
  // If the pragmas are implemented as function calls,
  // the use of the variables in the pragma will appear as VarRefs here.
  map<void *, bool> var_with_mutiple_def;
  vector<void *> vec_refs;
  codegen->GetNodesByType(func, "postorder", "SgVarRefExp", &vec_refs);
  for (auto ref : vec_refs) {
    void *var_init = codegen->GetVariableInitializedName(ref);
    if (var_init == nullptr) {
      continue;
    }
    if (codegen->IsArgumentInitName(var_init) != 0) {
      continue;
    }
    void *var_type = codegen->GetTypebyVar(var_init);
    if (codegen->IsScalarType(var_type) != 0) {
      var_type = codegen->GetBaseType(var_type, true);
    }
    if (codegen->IsIntegerType(var_type) == 0) {
      continue;
    }
    if (codegen->is_lvalue(ref) != 0) {
      continue;
    }
    //  FIXME: because of bug of defuse, bug2496,
    //  we only apply simple constant propagation with only one def
    if (var_with_mutiple_def.count(var_init) <= 0) {
      int def_count =
          static_cast<int>(codegen->GetInitializer(var_init) != nullptr);
      vector<void *> vec_refs;
      codegen->get_ref_in_scope(
          var_init, codegen->TraceUpToScope(codegen->GetVariableDecl(var_init)),
          &vec_refs, true);
      for (auto ref : vec_refs) {
        if (codegen->is_write_conservative(ref, false) != 0) {
          def_count++;
        }
      }
      var_with_mutiple_def[var_init] = def_count != 1;
    }
    if (var_with_mutiple_def[var_init]) {
      continue;
    }
    CMarsExpression me(ref, codegen, ref, func);
    if (me.IsConstant() != 0) {
      int64_t val = me.GetConstant();
      if (val >= std::numeric_limits<int>::min() &&
          val < std::numeric_limits<int>::max()) {
        codegen->ReplaceExp(ref, codegen->CreateConst(static_cast<int>(val)));
      } else {
        codegen->ReplaceExp(ref, codegen->CreateConst(val));
      }
      (*visited_vars)[var_init] = val;
      ret = 1;
    }
  }
  vector<void *> vec_calls;
  codegen->GetNodesByType(func, "preorder", "SgFunctionCallExp", &vec_calls);
  for (auto call : vec_calls) {
    void *func_decl = codegen->GetFuncDeclByCall(call);
    if (func_decl == nullptr) {
      continue;
    }
    ret |= constant_propagation(codegen, func_decl, visited_funcs,
                                assert_header_file, visited_vars, false,
                                assert_generation);
  }
  return ret;
}

// Yuxin: Dec/10/2019, propagate values in the pragmas
void constant_propagation_in_pragma(CSageCodeGen *codegen, void *sg_global,
                                    map<void *, int64_t> *visited_vars) {
  map<void *, bool> var_with_mutiple_def;
  vector<void *> vec_inits;
  codegen->GetNodesByType_int(sg_global, "preorder", V_SgInitializedName,
                              &vec_inits);
  for (auto var_init : vec_inits) {
    if (visited_vars->count(var_init) > 0) {
      set<void *> pragmas = codegen->get_pragma_related_to_variable(var_init);
      unordered_map<string, string> replace_defs;
      replace_defs[codegen->GetVariableName(var_init)] =
          my_itoa((*visited_vars)[var_init]);
      for (auto pragma : pragmas) {
        cout << "Propagating: " << codegen->_up(var_init) << "=> "
             << codegen->_up(pragma) << endl;
        codegen->RewritePragmaTokens(pragma, replace_defs);
      }
      continue;
    }
    // Yuxin: Feb/17/2020
    // No related reference in functions
    if (codegen->IsArgumentInitName(var_init) != 0) {
      continue;
    }
    void *var_type = codegen->GetTypebyVar(var_init);
    if (codegen->IsScalarType(var_type) != 0) {
      var_type = codegen->GetBaseType(var_type, true);
    }
    if (codegen->IsIntegerType(var_type) == 0) {
      continue;
    }
    void *def_ref = nullptr;
    if (var_with_mutiple_def.count(var_init) <= 0) {
      int def_count =
          static_cast<int>(codegen->GetInitializer(var_init) != nullptr);
      vector<void *> vec_refs;
      codegen->get_ref_in_scope(
          var_init, codegen->TraceUpToScope(codegen->GetVariableDecl(var_init)),
          &vec_refs, true);
      for (auto ref : vec_refs) {
        if (codegen->is_write_conservative(ref, false) != 0) {
          def_count++;
        }
      }
      var_with_mutiple_def[var_init] = def_count != 1;
      def_ref = codegen->GetInitializer(var_init);
    }
    // Yuxin: Only has one def, which is initializer
    if (var_with_mutiple_def[var_init]) {
      continue;
    }
    CMarsExpression me(def_ref, codegen, def_ref, sg_global);
    if (me.IsConstant() != 0) {
      int64_t val = me.GetConstant();
      (*visited_vars)[var_init] = val;
      set<void *> pragmas = codegen->get_pragma_related_to_variable(var_init);
      unordered_map<string, string> replace_defs;
      replace_defs[codegen->GetVariableName(var_init)] = my_itoa(val);
      for (auto pragma : pragmas) {
        cout << "Propagating: " << codegen->_up(var_init) << "=> "
             << codegen->_up(pragma) << endl;
        codegen->RewritePragmaTokens(pragma, replace_defs);
      }
    }
  }
}

int constant_propagation_top(CSageCodeGen *codegen, void *pTopFunc,
                             const CInputOptions &option,
                             bool assert_generation) {
  printf("    Enter const propagation...\n");
  CMarsIrV2 mars_ir;
  mars_ir.build_mars_ir(codegen, pTopFunc);
  vector<void *> vecTopKernels = mars_ir.get_top_kernels();
  bool Changed = true;
  set<void *> visited_funcs;
  set<void *> assert_header_file;
  map<void *, int64_t> visited_vars;
  codegen->init_defuse();
  codegen->BuildPragmaCache();
  vector<void *> vec_inits;
  // initialize enumerator value
  codegen->GetNodesByType_int(pTopFunc, "preorder", V_SgInitializedName,
                              &vec_inits);
  for (auto init : vec_inits) {
    if (codegen->IsEnumDecl(codegen->GetVariableDecl(init))) {
      auto initializer = codegen->GetInitializer(init);
      if (initializer != nullptr) {
        auto initializer_exp = codegen->GetInitializerOperand(initializer);
        auto em_val = isSgEnumVal(static_cast<SgNode *>(initializer_exp));
        if (em_val != nullptr) {
          visited_vars[init] = em_val->get_value();
        }
      }
    }
  }
  for (auto func : vecTopKernels) {
    Changed |=
        constant_propagation(codegen, func, &visited_funcs, &assert_header_file,
                             &visited_vars, true, assert_generation);
  }
  if (vecTopKernels.size() > 0) {
    void *sg_global = codegen->GetGlobal(vecTopKernels[0]);
    constant_propagation_in_pragma(codegen, sg_global, &visited_vars);
  }
  if (Changed) {
    codegen->init_defuse();
  }
  return static_cast<int>(Changed);
}

int constant_propagation_top(CSageCodeGen *codegen, void *pTopFunc,
                             const CInputOptions &option) {
  return constant_propagation_top(codegen, pTopFunc, option, true);
}
