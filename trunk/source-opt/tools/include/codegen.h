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

#include <unordered_map>
#include <set>
#include <map>
#include <vector>
#include <string>
#include <list>
#include <utility>
#include "rose.h"
#include "CallGraph.h"

#include "PolyModel.h"
#include "file_parser.h"

#include "history.h"
#include "id_update.h"
#include "report.h"

#include <boost/optional.hpp>
#if PROJDEBUG
#define XDEBUG(stmts)                                                          \
  do {                                                                         \
    stmts;                                                                     \
  } while (false)
#else
#define XDEBUG(stmts)
#endif
#if 0  //  control log
#define MERLIN_ALGO(x)                                                         \
  cout << "[MERLIN ALGO][" << __func__ << ", " << __LINE__ << "]" << x << endl;
#else
#define MERLIN_ALGO(x)
#endif

extern bool g_muteMERLINError;
#if 0
#define MERLIN_ERROR(x)                                                        \
  if (!g_muteMERLINError) {                                                    \
    cerr << "[MERLIN ERROR][" << __func__ << ", " << __LINE__ << "]" << x      \
         << endl;                                                              \
  } else {                                                                     \
    cerr << "[MERLIN WARNING][" << __func__ << ", " << __LINE__ << "]" << x    \
         << endl;                                                              \
  }

#define MERLIN_ERROR_COND(cond, x)                                             \
  if (!g_muteMERLINError && cond) {                                            \
    cerr << "[MERLIN ERROR][" << __func__ << ", " << __LINE__ << "]" << x      \
         << endl;                                                              \
  } else {                                                                     \
    cerr << "[MERLIN WARNING][" << __func__ << ", " << __LINE__ << "]" << x    \
         << endl;                                                              \
  }
#else
#define MERLIN_ERROR(x)
#define MERLIN_ERROR_COND(cond, x)
#endif

typedef void (*t_func_rose_simple_traverse)(void *sg_node, void *pArg);
typedef void *(*t_func_trace_up)(void *sg_node, void *pArg);

//  typedef std::map<void *, void *> t_func_call_path;
//  std::pair<func_decl, func_call>
typedef std::list<std::pair<void *, void *>> t_func_call_path;

class CMarsIrV2;
class CMarsIr;

template <typename K, typename V> class linked_map {
  using Entry = std::pair<K, V>;
  using iterator = typename std::list<Entry>::iterator;
  using const_iterator = typename std::list<Entry>::const_iterator;

  std::list<Entry> _list;
  std::unordered_map<K, iterator> _map;

 public:
  V &operator[](const K &key) {
    auto it = _map.find(key);
    if (it == _map.end()) {
      _list.push_back({key, V()});
      _map[key] = --_list.end();
      return _map[key]->second;
    } else {
      return it->second->second;
    }
  }

  iterator begin() { return _list.begin(); }
  iterator end() { return _list.end(); }
  const_iterator begin() const { return _list.begin(); }
  const_iterator end() const { return _list.end(); }
  bool empty() const { return _map.empty(); }
};

template <typename T> class SetVector {
  std::set<T> m_set;
  std::vector<T> m_vec;

 public:
  SetVector() {}
  void clear() {
    m_set.clear();
    m_vec.clear();
  }
  void insert(T t) {
    if (m_set.count(t) > 0)
      return;
    m_set.insert(t);
    m_vec.push_back(t);
  }
  auto find(const T &key) {
    if (m_set.count(key) <= 0)
      return end();
    for (auto it = begin(); it != end(); it++)
      if (*it == key)
        return it;
    return end();
  }
  auto find(const T &key) const {
    if (m_set.count(key) <= 0)
      return end();
    for (auto it = begin(); it != end(); it++)
      if (*it == key)
        return it;
    return end();
  }
  size_t count(T t) const { return m_set.count(t); }
  void erase(T t) {
    for (auto I = m_vec.begin(); I != m_vec.end(); ++I)
      if (*I == t) {
        m_vec.erase(I);
        break;
      }
    m_set.erase(t);
  }
  //  Determine if the SetVector is empty or not.
  bool empty() const { return m_vec.empty(); }

  //  / Determine the number of elements in the SetVector.
  size_t size() const { return m_vec.size(); }

  //  / Get an iterator to the beginning of the SetVector.
  auto begin() { return m_vec.begin(); }

  //  / Get a const_iterator to the beginning of the     SetVector.
  auto begin() const { return m_vec.begin(); }

  //  / Get an iterator to the end of the SetVector.
  auto end() { return m_vec.end(); }

  //  / Get a const_iterator to the end of the SetVector.
  auto end() const { return m_vec.end(); }
  template <class InputIterator> void insert(InputIterator S, InputIterator E) {
    InputIterator I = S;
    while (I != E) {
      insert(*I++);
    }
  }
};
class CMerlinMessage {
 protected:
  int m_code;
  std::string m_msg;

 public:
  CMerlinMessage() {}
  CMerlinMessage(int code, const std::string &msg) {
    m_code = code;
    m_msg = msg;
  }
  int get_code() { return m_code; }
  std::string get_message() { return m_msg; }
};

//  An AST visiter that accepts C++11 lambdas.
template <typename Fn> class CSageWalker : public AstSimpleProcessing {
  Fn lambda;

 public:
  explicit CSageWalker(Fn &&lambda) : lambda(lambda) {}
  void visit(SgNode *node) override { lambda(node); }
};

enum ext_opcode {
  V_SgOperatorLong = -3,
  V_SgOperatorULong = -2,
  V_SgOperatorFloat = -1,
};
class CSageCodeGen_TraverseSimple : public AstSimpleProcessing {
 public:
  CSageCodeGen_TraverseSimple() {
    m_func = 0;
    m_pArg = 0;
  }
  virtual void visit(SgNode *sgnode) {
    assert(m_func);
    assert(m_pArg);
    (*m_func)(sgnode, m_pArg);
    return;
  }
  void setFuncArg(t_func_rose_simple_traverse pFunc, void *pArg) {
    m_func = pFunc;
    m_pArg = pArg;
  }

 protected:
  t_func_rose_simple_traverse m_func;
  void *m_pArg;
};

class CSageCodeGen {
  enum DIM_SIZE {
    EMPTY_SIZE = -1,
    UNKNOWN_SIZE = -2,
    POINTER_SIZE = 0,
  };

 public:
  bool IsUnknownSize(size_t dim) const { return dim == ((size_t)UNKNOWN_SIZE); }
  bool IsDeterminedSize(size_t dim) const {
    return dim != ((size_t)UNKNOWN_SIZE) && dim != ((size_t)POINTER_SIZE) &&
           dim != ((size_t)EMPTY_SIZE);
  }
  bool IsEmptySize(size_t dim) { return dim == ((size_t)EMPTY_SIZE); }

 public:
  explicit CSageCodeGen(SgProject *sg = nullptr);
  explicit CSageCodeGen(const std::string &sFileName);
  ~CSageCodeGen();
  /*********** Analyze json file ************/
  //  vector<std::string> GetFirstDim(const std::string &file_name);
  /*********** COMPILER OPTION ******/
  std::map<std::string, std::string> get_input_macro() const {
    return m_inputMacros;
  }
  /*********** FILE ************/
  void *CreateSourceFile(const std::string &sFileName,
                         void *bindNode = nullptr);
  void *GetProject() const { return m_sg_project; }
  void *OpenSourceFile(std::vector<std::string> sFileName,
                       std::string cflags = "");
  int GetGlobalNum() const { return m_sg_project->numberOfFiles(); }
  void *GetGlobal(int idx);
  //  global scope
  void *GetGlobal(void *sg_pos);
  std::vector<void *> GetGlobals();
  //  File Info
  std::string GetFileInfo_filename(void *sgnode, int simple = 0) const;

  int GetFileInfo_line(void *sgnode) const {
    if (!sgnode)
      return 0;
    return (static_cast<SgNode *>(sgnode)->get_file_info()->get_line());
  }
  int GetFileInfo_col(void *sgnode) const {
    if (!sgnode)
      return 0;
    return (static_cast<SgNode *>(sgnode)->get_file_info()->get_col());
  }
  std::string get_file(void *sgnode) const;
  int get_line(void *sgnode) const;
  int get_col(void *sgnode) const;
  std::string get_location(void *sg_node, bool simple, bool line_num) const;
  std::string get_location_from_metadata(void *sgNode);
  int IsMixedCAndCPlusPlusDesign();
  int IsPureCDesign();
  int IsFromInputFile(void *sgnode) const;
  int IsFromMerlinSystemFile(void *sgnode);
  int IsSystemFunction(void *sg_func);
  bool IsMerlinInternalIntrinsic(string func_name);
  bool isWithInCppFile(void *sgnode);
  bool isCppLinkage(void *sgnode);
  bool isWithInHeaderFile(void *sgnode);

  bool IsNULLFile(void *sg_expr_or_stat);
  bool IsCompilerGenerated(void *sg_expr_or_stat);
  bool IsTransformation(void *sg_expr_or_stat);
  //  for 'sg_node' in template instantiation, get its corresponding node
  //  in original template function
  void *GetOriginalNode(void *sg_node);
  /********* VARIABLE *****************/
  /**** Get variable info *****/
  std::vector<std::string> GetOpenCLName();
  void *GetVariableSymbol(void *sg_var_ref_);  //  NOT SUGGESTED, needreview
  void *GetVariableInitializedName(void *sg_var_ref_decl) const;
  void *GetVariableDefinition(void *decl_, void *name_ = nullptr);
  void GetVariableInitializedName(void *sg_var_ref_decl,
                                  std::vector<void *> *vec_var) const;
  void *find_interface_by_name(const std::string &var, void *kernel);
  void *
  find_variable_by_name(const std::string &var, void *pos,
                        bool inclusive_scope = false);  //  ZP: 20160901
                                                        // new function for
                                                        // pragma analysis
  bool check_valid_field_name(const std::string &field_name, void *pos);
  void *find_field_by_name(const string &field_name, void *pos);
  void *GetInitializer(void *sg_name);
  void *GetInitializerOperand(void *sg_input);
  void *GetVariableDecl(void *sg_var_ref_or_name);
  void *GetVariableDefinitionByName(void *sg_var);
  void *get_actual_var_decl_by_ref(void *var_ref);
  void *
  TraceVariableInitializedName(void *sg_name);  //  trace up cross function
                                                // calls
  std::string GetVariableTypeName(void *sg_var_);
  std::string GetVariableName(void *sg_var_, bool qualified = false) const;
  bool IsStatic(void *sg_decl_);
  bool IsMutable(void *sg_decl_);
  bool IsExtern(void *sg_decl_);
  //  storage modifier
  int is_register(void *var_decl);
  void *GetTypebyVar(void *var_init_name) const;
  int IsLocalInitName(void *sg_node);
  int IsForwardInitName(void *sg_node);
  int IsGlobalInitName(void *sg_node);
  int IsArgumentInitName(void *sg_node);
  bool IsMemberVariable(void *var_initname);
  bool IsUnionMemberVariable(void *var_init);
  bool IsInputOutputGlobalVariable(void *var_init);
  /***** updated variable info ******/
  void SetTypetoVar(void *var_init_name, void *sg_type);
  void SetInitializer(void *sg_name, void *sg_init);
  void SetVariableName(void *sg_var_, const std::string &new_name);
  void SetStatic(void *sg_decl_);
  void UnSetStatic(void *sg_decl_);
  void SetExtern(void *sg_decl_);
  void set_register(void *var_decl);
  void unset_register(void *var_decl);
#if USED_CODE_IN_COVERAGE_TEST
  void SetMutable(void *sg_decl_);
#endif

  /**** ceated variable *******/
  void *CreateVariable(const std::string &sType, const std::string &sVar,
                       void *bindNode = nullptr);
  void *CreateVariable(void *sg_type, const std::string &sVar,
                       void *bindNode = nullptr);
  void *CreateVariableDecl(const std::string &sType, const std::string &sVar,
                           void *sg_init_exp_, void *sg_scope_,
                           void *bindNode = nullptr);
  void *CreateVariableDecl(void *sg_type_, const std::string &sVar,
                           void *sg_init_exp_, void *sg_scope_,
                           void *bindNode = nullptr);
  void *CreateInitializer(void *sg_init_exp_, void *bindNode = nullptr);

  /******** FUNCTION *******************/
  /***** get function info ***********/
  bool UnSupportedMemberFunction(void *func);
  bool IsMemberFunction(void *func_decl);
  bool IsConstMemberFunction(void *func_decl);
  bool IsAssignOperator(void *func_decl);
  bool IsConstructor(void *func_decl);
  bool IsDefaultConstructor(void *func_decl);
  int IsSupportedFunction(void *func_decl);
  int IsVarArgFunction(void *func_decl);

  std::string GetFuncName(void *sg_decl_, bool qualified = false) const;
  std::string DemangleName(std::string name) const;
  std::string GetMangledFuncName(void *sg_decl_) const;
  int GetFuncParamNum(void *sg_func_decl_) const;
  void *GetFuncParam(void *sg_func_decl_,
                     int i) const;  //  the SgInitializedName object
  std::vector<void *> GetFuncParams(void *sg_func_decl_) const;
  int GetFuncParamIndex(void *sg_arg, void *sg_scope = nullptr);
  void *GetFuncParamFromCallArg(void *sg_arg);
  void *GetFuncReturnType(void *sg_func_decl_) const;
  std::vector<void *> GetFuncReturnStmt(void *sg_scope);
  void *GetFuncBody(void *sg_func_decl_) const;
  void *GetFuncDeclByCall(void *sg_func_call_, int require_body = 1,
                          int ignore_static = 0);
  void *GetFuncDeclByRef(void *sg_func_ref);
  void *GetFuncDeclByName(const std::string &sFuncName, int require_body = 1);
  void *GetFuncDeclByDefinition(void *func_def);
  void *GetDefiningFuncDecl(void *func_decl, void *sg_func_call_,
                            bool ignore_static);
  void GetAllFuncDeclToCall(const std::set<void *> &funcs,
                            std::map<void *, std::vector<void *>> *func2calls);
  void *GetAssociatedFuncDeclByCall(void *sg_func_call_) const;  //  not
                                                                 // suggested
  std::vector<void *> GetAllFuncDeclByCall(void *sg_func_call_,
                                           int require_body = 0);
  std::vector<void *> GetAllFuncDeclByName(const std::string &sFuncName,
                                           int require_body = 0);
  std::vector<void *> GetAllFuncCallByName(const std::string &sFuncName);
  std::vector<void *>
  GetAllFuncDecl();  //  get all the function decls with body
  std::vector<void *> GetAllFuncDecl(void *func_decl);  //  get all the function
                                                        // decls
  //  pointed to the same
  //  functions

  void GetSubFuncDeclsRecursively(void *sg_func_decl,
                                  SetVector<void *> *sub_decls,
                                  SetVector<void *> *sub_calls,
                                  bool required_body = true);

  void GetSubFuncDeclsRecursively(
      void *sg_func_decl, SetVector<void *> *sub_decls,
      std::map<void *, SetVector<void *>> *callee2callers,
      bool required_body = true);

  std::string GetFuncNameByCall(void *sg_func_call_, bool qualified = false);
  std::string GetFuncNameByRef(void *sg_func_ref_, bool qualified = false);
  void *GetFuncReturnTypeByCall(void *sg_func_call_);
  void *GetUniqueFuncDeclByBody(void *sg_func_body);
#if USED_CODE_IN_COVERAGE_TEST
  void *GetUniqueFuncDeclByCall(void *sg_call);
#endif
  void *GetFuncDefinitionByDecl(void *sg_func_decl);
  void *GetUniqueFuncDecl(void *sg_func_decl);

  /****** update function info **********/
  int SetFuncName(void *sg_decl_, const std::string &new_name);
  void SetFuncReturnType(void *sg_func_decl_, void *new_type);
  int SetFuncBody(void *sg_func_decl_, void *sg_body);
  void insert_param_to_decl(void *decl, void *name, bool to_all_decl = true);

  /***** create function info **********/
  void *CreateFuncParamList(const std::vector<void *> &param_list,
                            void *bindNode = nullptr);  //  not suggested
  void *CreateFuncDecl(const std::string &sType, const std::string &sFuncName,
                       std::vector<void *> fp_list, void *sg_scope_,
                       bool definition, void *bindNode);
  void *CreateFuncDecl(void *ret_type, const std::string &sFuncName,
                       std::vector<void *> fp_list, void *sg_scope_,
                       bool definition, void *bindNode);
  void *CreateFuncDecl(const std::string &sType, const std::string &sFuncName,
                       void *sg_fp_list_, void *sg_scope_, bool definition,
                       void *bindNode);
  void *CreateFuncDecl(const std::string &ret_type,
                       const std::string &sFuncName,
                       std::vector<std::string> vec_name,
                       std::vector<std::string> vec_type, void *sg_scope,
                       bool definition, void *bindNode);
  void *CreateFuncDecl(void *ret_type, const std::string &sFuncName,
                       std::vector<std::string> vec_name,
                       std::vector<void *> vec_type, void *sg_scope,
                       bool definitione, void *bindNode);
  void *CreateMemberFuncDecl(const std::string &sType,
                             const std::string &sFuncName,
                             std::vector<void *> fp_list, void *sg_scope_,
                             bool definition, void *bindNode);
  void *CreateMemberFuncDecl(void *ret_type, const std::string &sFuncName,
                             std::vector<void *> fp_list, void *sg_scope_,
                             bool definition, void *bindNode);
  void *CreateFunctionRef(void *func_decl);
  /******** clone function info ***********/
  void *CloneFuncDecl(void *func_decl, void *sg_scope, bool definition = true,
                      bool bind = true);
  void *CloneFuncDecl(void *func_decl, void *sg_scope,
                      const std::string &new_func_name, bool definition = true);

  //  Given a function declaration, clone each of its SgInitializedName
  //  parameters. Returns a vector of the cloned parameters and a mapping from
  //  old to new parameters.
  // TODO(youxiang): New parameters may need to bound, similar to what
  //  CloneFuncDecl does.
  std::pair<std::vector<SgInitializedName *>,
            std::map<SgInitializedName *, SgInitializedName *>>
  cloneFuncParams(const SgFunctionDeclaration &);

  //  Clones a func decl like CloneFuncDecl, except that the clone will be given
  //  the parameter list specified by the vector argument, and all references to
  //  old params in the func body will be remapped according to the map
  //  argument.
  SgFunctionDeclaration *
  cloneFuncWith(SgFunctionDeclaration *, SgScopeStatement *, SgType *,
                const std::string &, const std::vector<SgInitializedName *> &,
                const std::map<SgInitializedName *, SgInitializedName *> &);
  void *CopyFuncDecl(void *sg_decl_,
                     const std::string &sFuncName);  //  only support function
                                                     // with no argument

  /******* FUNCTION CALL ********************/
  /****** get function call info ****************/
  void GetFuncCallsFromDecl(void *sg_func_decl, void *sg_scope,
                            std::vector<void *> *vec_calls);
  void GetFuncCallsByName(const std::string &s_name, void *sg_scope,
                          std::vector<void *> *vec_calls);
  int GetFuncCallParamNum(void *sg_func_call_);
  void *GetFuncCallParam(void *sg_func_call_,
                         int i);  //  the SgInitializedName object
  std::vector<void *> GetFuncCallParamList(void *sg_func_call_);
  int GetFuncCallParamIndex(void *sg_arg);
  int GetFuncCallParamIndexRecur(void *sg_arg);
  bool is_xilinx_channel_read(void *call, void **ref);
  bool is_xilinx_channel_write(void *call, void **ref);
  bool is_xilinx_channel_access(void *call, string member_name, void **ref);
  int is_merlin_channel_read(void *ref);
  int is_merlin_channel_write(void *ref);
  int is_merlin_channel_call(void *call);
  int IsFunctionPointerCall(void *sg_node) const {
    SgFunctionCallExp *sg_call =
        isSgFunctionCallExp(static_cast<SgNode *>(sg_node));
    if (!sg_call)
      return 0;
    void *sg_decl = GetAssociatedFuncDeclByCall(sg_call);
    return nullptr == sg_decl;
  }
  // check whether the call is xilinx ap_int/ap_fixed/stream member function
  // call
  int IsSupportedMemberFunctionCall(void *func_call);
  // check whether the callee is xilinx ap_int/ap_fixed/stream member
  // function or extended friend function
  int IsSupportedMerlinSystemFunctionCall(void *func_call);

  bool IsAssertFailCall(void *assert_call);
  bool IsMemCpy(void *sg_call);
  //  parse the memcpy function
  int parse_memcpy(void *memcpy_call, void **length_exp, void **sg_array1,
                   void **sg_pntr1, std::vector<void *> *sg_index1,
                   int *pointer_dim1, void **sg_array2, void **sg_pntr2,
                   std::vector<void *> *sg_index2, int *pointer_dim2,
                   void *pos = nullptr);

  int is_memcpy_with_small_const_trip_count(void *memcpy_call, int threshold);

  /******** update function call ***********************/
  void insert_argument_to_call(void *call, void *exp);
  /***** create function call *******************/
  void *CreateFuncCall(const std::string &sFunc, void *sg_ret_type_,
                       std::vector<void *> param_list_, void *sg_scope_,
                       void *bindNode = nullptr);
  void *CreateFuncCall(void *sg_exp_, std::vector<void *> param_list_,
                       void *bindNode = nullptr);
  void *CreateFuncCallStmt(void *sg_decl_, void *param_list_,
                           void *bindNode = nullptr);
  void *CreateFuncCallStmt(void *sg_decl_, std::vector<void *> param_list_,
                           void *bindNode = nullptr);

  /*********  COMPARE AST NODE *************/
  bool isSameFunction(void *func_decl1, void *func_decl2,
                      int ignore_static = 0);
  bool isSameType(void *type1, void *type2);
  bool is_in_same_file(void *sg1, void *sg2);
  bool is_in_same_function(void *sg1, void *sg2);
  bool IsMatchedFuncAndCall(void *decl, void *call, void *curr_decl = nullptr,
                            int ignore_static = 0);  //  light weighted function
  int IsCompatibleType(void *sg_type1, void *sg_type2, void *pos, bool strict);
  bool is_identical_expr(void *sg_node1, void *sg_node2, void *sg_pos1,
                         void *sg_pos2);
  bool is_identical_base_type(void *sg_array1, void *sg_array2);
  //  youxiang: check whether 'target_init' is alias of 'src_init' by traversing
  //  along call path
  int is_same_instance(void *target_init, void *src_init,
                       const t_func_call_path &call_path);
  int IsomorphicLoops(void *one_loop, void *other_loop);
  bool isDom(void *pos1, void *pos2);
  bool InSameUnconditionalScope(void *pos1, void *pos2);
  /*******   PRAGMA *************************/
  /********** get pragma info **************/
  std::string GetPragmaString(void *sg_node_);
#if USED_CODE_IN_COVERAGE_TEST
  std::string GetPragmaStringAbove(void *pos);
#endif
  map<void *, string> GetVariablesUsedByPragma(void *pragma, bool *report_err,
                                               string *msg, bool numeric_only);
  SetVector<void *> GetVariablesUsedByPragmaInScope(void *scope,
                                                    bool numeric_only);
  bool IsPragmaWithDeadVariable(void *pragma);
  void *GetPragmaAbove(void *pos);
  bool func_has_dataflow(void *func_decl);
  bool IsDataflowProcess(void *func_decl);
  bool IsDataflowScope(void *scope);
  bool IsParallelLoop(void *loop);
  /********* update pragma info ************/
  void *SetPragmaString(void *sg_node_,
                        const std::string &str);  //  return the new pointer
  bool is_numeric_option(std::string attr, bool numeric_only);
  /********  create pragma info ***********/
  void *CreatePragma(const std::string &sPragma, void *sg_scope_,
                     void *bindNode = nullptr);

  /********** EXPRESSION & STATEMENT *************/
  /********* get expression info *************************/
  void detachFromParent(SgNode *node);
  int IsSimpleAggregateInitializer(void *sg_initer_);
  void *GetAggrInitializerFromCompoundLiteralExp(void *sg_exp);
  void parse_aggregate(void *scope = nullptr);
  void GetExpOperand(void *sg_exp, void **op0, void **op1);
  void *GetExpUniOperand(void *sg_exp);
  void *GetExpLeftOperand(void *sg_exp);
  void *GetExpRightOperand(void *sg_exp);
  void *GetFirstFuncDeclInGlobal(void *pos);
  void *GetFirstNonTrivalStmt(void *blk);
  void *GetLastNonTrivalStmt(void *blk);
  bool IsUnderConditionStmt(void *stmt);
  void *GetSwitchStmtSelector(void *sg_switch);
  void GetExprList(void *sg_exp_list, std::vector<void *> *vec_expr);
  int GetExprListFromCommaOp(void *sg_comma, std::vector<void *> *vec_expr);
  void GetExprListFromAndOp(void *sg_comma, std::vector<void *> *vec_expr);
  void GetExprListFromOrOp(void *sg_comma, std::vector<void *> *vec_expr);
  bool IsConditionalExp(void *exp, void **cond_exp, void **true_exp,
                        void **false_exp);
  void *GetNextStmt(void *sg_stmt_, bool includingCompilerGenerated = true);
  void *GetNextStmt_v1(void *sg_stmt_);
  void *GetPreviousStmt(void *sg_stmt_, bool includingCompilerGenerated = true);
  void *GetPreviousStmt_v1(void *sg_stmt_);
  void *GetParent(void *sg_node_) const;
  void *GetFirstInsertPosInGlobal(void *sg_node);

  void *GetChildStmt(void *sg_scope_, size_t i);
  std::vector<void *> GetChildStmts(void *sg_scope_);
  size_t GetChildStmtNum(void *sg_scope_) const;
  // idx == numeric_limits<size_t>::max() for not found
  size_t GetChildStmtIdx(void *sg_scope_, void *sg_child_);
  void GetChildStmtList(
      void *sg_scope_,
      SgStatementPtrList *stmt_lst) const;  //  add by Yuxin, May 26
                                            // 2015
  //  Mo, not suggested
  bool GetChildTaskIdx(void *sg_scope_, void *sg_child_, int *index, int HGT);
  //  void GetForStmt(void * sg_scope, std::vector<void*>* vecLoops) ;

  void *GetLocation(void *start_stmt, bool forward = true);
  int IsLoopStatement(void *sg_node) const {
    return IsForStatement(sg_node) || IsWhileStatement(sg_node) ||
           IsDoWhileStatement(sg_node);
  }
  void *GetGotoLabel(void *sg_node) const {
    SgGotoStatement *goto_stmt =
        isSgGotoStatement(static_cast<SgNode *>(sg_node));
    if (!goto_stmt)
      return nullptr;
    return goto_stmt->get_label();
  }

  void *GetStmtFromLabel(void *sg_node, bool skip_brace = false) {
    SgLabelStatement *label =
        isSgLabelStatement(static_cast<SgNode *>(sg_node));
    if (!label)
      return nullptr;
    void *stmt = label->get_statement();
    if (skip_brace) {
      if (IsBasicBlock(stmt) && (GetChildStmtNum(stmt) >= 1)) {
        return GetChildStmt(stmt, 0);
      }
    }
    return stmt;
  }

  std::string GetLabelName(void *sg_node);
  int IsControlStatement(void *sg_node) const {
    return IsIfStatement(sg_node) || IsLoopStatement(sg_node) ||
           IsSwitchStatement(sg_node);
  }
  void *GetReturnExp(void *sg_node) const {
    SgReturnStmt *ret_stmt = isSgReturnStmt(static_cast<SgNode *>(sg_node));
    if (!ret_stmt)
      return nullptr;
    return ret_stmt->get_expression();
  }

  int IsDummyReturnStmt(void *sg_node) const {
    SgReturnStmt *ret_stmt = isSgReturnStmt(static_cast<SgNode *>(sg_node));
    if (!ret_stmt)
      return false;
    SgExpression *ret_val = ret_stmt->get_expression();
    if (isSgNullExpression(ret_val))
      return true;
    return false;
  }
  int GetBinaryResultType(int op0_type, int op1_type, int binary_op);

  int IsOverideAssignOp(void *sg_ref);
  //  0 for prefix, 1 for suffix
  int GetUnaryOpMode(void *sg_exp) const {
    return isSgUnaryOp(static_cast<SgNode *>(sg_exp))->get_mode();
  }
  int IsCompareOp(void *sg_node);
  int IsCompareOp(int opcode);
  int GetCompareOp(void *sg_exp);
  int GetExpressionOp(void *sg_exp_) const {
    auto sg_exp = isSgExpression(static_cast<SgNode *>(sg_exp_));
    if (nullptr == sg_exp)
      return 0;
    return sg_exp->variant();
  }
  int IsIncrementOp(void *sg_node);
  int IsIncrementOp(int op_code);
  int GetBinaryOperationFromCompoundAssignOp(void *exp);
  bool isSimpleOp(void *op, void *var_name);
  bool isSimpleOp(void *sg_left_op, void *sg_right_op, void *var_name);
  int IsValueExp(void *sg_node);
  bool IsConstantInt(void *sg_exp, int64_t *value, bool simplify_cast,
                     void *scope);
  bool IsConstantTrue(void *sgExp);
  bool IsConstantFalse(void *sgExp);
  bool GetCastValue(int64_t *val, void *cast_type);
  int IsNullPointerCast(void *sg_exp);
  void *get_sizeof_value(void *sizeof_exp);
  void ConvertBigIntegerIntoSignedInteger(int64_t *nStep, void *sg_type);
  /************* update expression ********************/
  int AddCastToExp(void *sg_exp, const std::string &s_type);
  int AddCastToExp(void *sg_exp, void *sg_type);
  void remove_cast(void **sg_node);
  void *RemoveCast(void *sg_exp);
  void *RemoveAPIntFixedCast(void *exp);
  int standardize_pntr_ref_from_array_ref(void *sg_ref, void *pos = nullptr);

  void *splitExpression(void *from, std::string newName = "");
  void splitInlinedAggregatedDefinition(void *scope);
  /************* create const expression **************/
  void *CreateConst(void *val_, int const_type = V_SgIntVal,
                    void *bindNode = nullptr);
  void *CreateConst(int value_, void *bindNode = nullptr);
  void *CreateConst(int64_t value_, void *bindNode = nullptr);
  void *CreateConst(int16_t value_, void *bindNode = nullptr);
  void *CreateConst(size_t value_, void *bindNode = nullptr);
  void *CreateStringExp(const std::string &val, void *bindNode = nullptr);
  void *CreateAndOpFromExprList(std::vector<void *> vec_expr,
                                void *bindNode = nullptr);

  /************ create reference ************************/
  void *CreateVariableRef(void *var_initname_or_decl, void *scope = nullptr,
                          void *bindNode = nullptr);
  void *CreateVariableRef(const std::string &var_name, void *scope = nullptr,
                          void *bindNode = nullptr);
  void *CreateArrayRef(void *array_var_ref, std::vector<void *> vec_index,
                       void *bindNode = nullptr);

  /************ create expression & statement  ***********/
  void *CreateCastExp(void *sg_exp, const std::string &s_type,
                      void *bindNode = nullptr);
  void *CreateCastExp(void *sg_exp, void *sg_type, void *bindNode = nullptr);
  //  convert unsigned integer expression into corresponding signned integer
  //  expression such as, unsigned int -> int, unsigned char -> char
  void *CreateSignnesCast(void *exp);
  void *CreateConditionExp(void *op0, void *op1, void *op2,
                           void *bindNode = nullptr);
  void *CreateExp(int opcode, void *op0 = nullptr, void *op1 = nullptr,
                  int unary_mode_ = 0, void *bindNode = nullptr,
                  bool check_floating = true);
  vector<void *> CreatePntrExp(void *var_init);
  void *CreateStmt(int stmt_type, void *op0_ = nullptr, void *op1_ = nullptr,
                   void *op2_ = nullptr, void *op3_ = nullptr,
                   void *op4_ = nullptr, void *op5_ = nullptr,
                   void *op6_ = nullptr);

  void *CreateLabelStmt(const std::string &label, void *stmt, void *scope,
                        void *bindnode = nullptr);
#if USED_CODE_IN_COVERAGE_TEST
  void *CreateLabel(const std::string &sName, void *bindNode = nullptr);
#endif
  void *CreateBasicBlock(void *stmt_list = nullptr, bool group = false,
                         void *bindNode = nullptr);
  void *CreateExpList(const std::vector<void *> &exp_list_,
                      void *bindNode = nullptr);
  void *CreateExpList(const std::vector<size_t> &dims, void *sg_var,
                      void *bindNode = nullptr);
  void *CreateAggregateInitializer(const std::vector<void *> &vec_exp,
                                   void *sg_type);
  void *CreateAggregateInitializer(void *expr_list, void *sg_type);

  /********* TYPE  ****************************************************/

  /*************** get type or class info  *****************************/
  int IsScalarType(void *sg_name);
  int IsStructureType(void *sg_type_) const;
  int IsUnionType(void *sg_type_) const;
  int IsEnumType(void *sg_type_) const;
  int IsAnonymousType(void *sg_type_) const;
  int IsAnonymousName(const std::string &) const;
  int IsClassType(void *sg_type_) const;
  int IsCompoundType(void *sg_type);
  int IsRecursiveType(void *sg_type_, void **rec_type);
  int IsRecursiveType(void *sg_type_);
  //  primitive type, class/struct without function member, non-recusive type
  int IsCStyleSimpleType(void *sg_type_, void **unsupported_type,
                         std::string *reason);
  int IsConstType(void *sg_type) const;
  int IsVolatileType(void *sg_type) const;
  int IsGeneralCharType(void *sg_type) const;
  bool IsStructureWithAlignedScalarData(void *sg_type);
  bool IsStructureWithAlignedScalarData(void *sg_type, int *total_bitwidth,
                                        int *aligned_bitwidth);
  int get_type_unit_size(void *sg_type, void *pos);
  int get_type_size(void *sg_type, void *pos, bool report = true);
  void *get_type_size_exp(void *sg_type, void *pos, bool report);
  int get_pointer_dimension(void *sg_type_, void **basic_type,
                            std::vector<size_t> *nSizes);
  int get_arrays_dim(void *sg_bb_or_func_,
                     std::map<std::string, std::vector<int>> *arr_name2dim);

  //  not across function (stable)
  //  Note
  //  The order of the return dims is from low_dim to high_dim (right to left)
  //  E.g. int a[1][2][3] => ( 3,2,1 )
  //      int (*a)[10]   => ( 10, 0 )
  //      int *a[10]     => ( 0, 10 )
  int get_type_dimension(void *sg_type_, void **basic_type,
                         std::vector<size_t> *nSizes, void *pos = nullptr,
                         bool stop_pointer = false);
  int get_type_dimension_exp(void *sg_type_, void **basic_type,
                             std::vector<void *> *nSizes, void *pos,
                             bool stop_pointer);
  int get_type_dimension_new(void *sg_type_, void **basic_type,
                             std::vector<size_t> *nSizes, void *pos = nullptr,
                             bool stop_pointer = false);

  void *get_array_base_type(void *sg_array);

  //  not across function
  int get_dim_num_from_var(void *sg_array);

  //  the flagship for cross function analysis
  int get_type_dimension_by_ref(void *sg_varref_or_pntr, void **sg_array,
                                void **basic_type,
                                std::vector<size_t> *vecSizes);

  int get_bitwidth_from_type(void *sg_type, bool report = true);

  int IsAnonymousTypeDecl(void *sg_decl) const;
  int IsTypeDecl(void *sg_decl) const;

  void GetPointerInStruct(void *struct_type, vector<void *> *vec_pointer);
  int ContainsUnSupportedType(void *sg_type_, void **unsupported_type,
                              std::string *reason,
                              bool is_return = false,
                              std::string *string_type = nullptr,
                              bool has_array_pointer_parent = false);
  void GetAllUnSupportedType(void *sg_type_,
                             std::set<std::pair<void *, std::string>> *res);
  void *ContainsClassType(void *sg_type_);

  std::string get_var_ref_base_type(void *var_ref);
  void *GetTypebyVarRef(void *var_ref_);
  void *GetTypebyNewExp(void *new_exp_);
  void *GetTypeByExp(void *sg_exp);
  void *GetTypeByEnumVal(void *sg_enum_val_);
#if USED_CODE_IN_COVERAGE_TEST
  void *GetTypeByDecl(void *sg_class_decl);
#endif
  void *GetTypeDeclByType(void *sg_type, int require_def = 1);
  void *GetDirectTypeDeclByType(void *sg_type);
  void *GetTypeDefDecl(void *sg_type);
  void *GetClassDefinition(void *sg_class_decl);
  void *GetTypeDeclByDefinition(void *sg_class_def_);
  void *GetTypeDeclByMemberFunction(void *sg_mem_func);
  void *GetTypeByName(const std::string &type_name);
  std::string GetTypeName(void *sg_type_decl_, bool qualified = false) const;
  std::string GetClassName(void *sg_class_decl) const;
  std::string GetTypeNameByType(void *sg_class_type,
                                bool qualified = false) const;
  std::string GetStringByType(void *sg_type) const;
  void *GetOrigTypeByTypedef(void *sg_type,
                             bool skip_anonymous_struct = false) const;

  void *GetBaseTypeByModifierType(void *sg_type);

  //  Type related
  void InitBuiltinTypes();
  int IsBuiltinType(void *type) const {
    std::string type_name = GetStringByType(type);
    return IsBuiltinType(type_name);
  }
  int IsBuiltinType(const std::string &sType) const {
    return m_builtin_types.count(sType) > 0;
  }
  void *GetBaseType(void *sg_type, bool skip_anonymous_type) const {
    return trace_base_type(sg_type, skip_anonymous_type);
  }
  void *GetBaseTypeOneLayer(void *sg_type) const;

  void *GetElementType(void *sg_type) const;

  void GetClassMembers(void *sg_class_type_or_decl,
                       std::vector<void *> *members);
  void *GetClassMemberByName(const std::string &s_var, void *sg_class_type);
  void GetClassDataMembers(void *sg_class_type_or_decl,
                           std::vector<void *> *data_members);
  void GetClassNonStaticDataMembers(void *sg_class_type_or_decl,
                                    std::vector<void *> *data_members);
  void GetClassFunctionMembers(void *sg_class_type_or_decl,
                               std::vector<void *> *func_members);
  vector<void *> GetDataMemberExp(void *var_decl, void *mem_init);
  vector<void *> GetInheritances(void *sg_class_decl);
  void *GetDefaultConstructor(void *sg_class_type);
  /***** update type *******************************/
  void *RemoveConstType(void *sg_type, void *pos, bool *changed);
  void *RemoveConstVolatileType(void *sg_type_, void *pos);
  //  return 1 if typedef replacement happened
  //  return 0 otherwise
  int RemoveTypedef(void *sg_type, void **new_type, void *pos,
                    bool skip_builtin = false, std::string msg = "");
  int ConvertBool2Char(void *sg_type_, void *pos, void **new_type);
  /**************************** create type or class ************/
  void *GetTypeByString(const std::string &sType);
  void *ArrayToPointerRecur(void *sg_type, bool skip_typedef = false);
  void RegisterType(const std::string &sType, void *pos = nullptr);
  //  Order of vec_dim: Left to right dimensions
  void *CreateArrayType(void *base_type, std::vector<size_t> vec_dim,
                        void *bindNode = nullptr);
  void *CreatePointerType(void *base_type, void *bindNode = nullptr);
  void *CreateCombinedPointerArrayType(void *base_type,
                                       std::vector<size_t> vec_dim,
                                       void *bindNode = nullptr);
  void *CreatePointerType(void *base_type, int levels,
                          void *bindNode = nullptr);
  void *CreateReferenceType(void *base_type, void *bindNode = nullptr);
  void *CreateModifierType(void *new_base_type, void *orig_modifier,
                           void *bindNode = nullptr);
  void *CreateConstType(void *sg_type, void *pos);

  //  create a class declation by rose API
  //  SageBuilder::buildClassDeclaration_nfi also, mark the change in history
  void *CreateClassDecl(const SgName &name,
                        SgClassDeclaration::class_types kind,
                        SgScopeStatement *scope,
                        SgClassDeclaration *nonDefiningDecl,
                        bool buildTemplateInstantiation,
                        SgTemplateArgumentPtrList *templateArgumentsList,
                        void *bindNode = nullptr);

  //  Min: convert SgTypeString to Pointer of char
  void *ConvertTypeStrToPtrChar(void *sg_type);

  /******************************* LOOP ************************/
  /********** get loop info **********************************/
  void *GetLoopBody(void *for_loop);
  void *GetLoopTest(void *sg_loop);  //  return the statement
  void *GetLoopInit(void *sg_for);
  int IsLoopInit(void *sg_stmt);
  int IsLoopTest(void *sg_stmt);
  int IsLoopBody(void *sg_stmt);
  void *GetLoopIncrementExpr(void *sg_for);  //  return the expression
  void *GetLoopIterator(void *for_loop);
  void *GetLoopFromIteratorByPos(void *sg_name, void *sg_pos,
                                 int cross_func = 0, int trace_back = 1);
  void *GetLoopFromIteratorRef(void *sg_var_ref);
  void *GetWhileCondition(void *for_loop);
  int64_t get_average_loop_trip_count(void *for_loop,
                                      const t_func_call_path &fn_path,
                                      void *scope);
  int64_t get_average_loop_trip_count(void *pos, void *scope,
                                      const t_func_call_path &fn_path);
  bool get_loop_trip_count_simple(void *sg_loop, int64_t *trip_count);
  int get_loop_trip_count(void *for_loop, int64_t *trip_count,
                          int64_t *trip_count_ub = nullptr);
  int get_loop_trip_count(void *for_loop, void **trip_count);
  int get_loop_trip_count_from_pragma(void *loop, int64_t *trip_count);
  //  Across-Function Analysis
  int get_loop_nest_in_scope(void *sg_pos, void *sg_scope,
                             std::vector<void *> *sg_loops);
  int get_loop_nest_in_scope(
      void *sg_pos, void *sg_scope,
      std::vector<std::pair<void *, void *>> *sg_loops_with_last);
  int get_loop_nest_in_scope(void *sg_pos, void *sg_scope,
                             const t_func_call_path &func_path,
                             std::vector<void *> *sg_loops);
  int get_loop_nest_in_scope(
      void *sg_pos, void *sg_scope, const t_func_call_path &func_path,
      std::vector<std::pair<void *, void *>> *sg_loops_with_last);
  int get_loop_nest_in_scope(void *sg_pos, void *sg_scope,
                             const t_func_call_path &func_path,
                             std::vector<std::pair<void *, void *>> *sg_loops,
                             std::set<void *> *visited, bool reverse);

  int hasLabelWithGoto(void *for_loop);

  //  /////////////////////////////////////////////////////////////////////  /
  //  A canonical form is defined as : one initialization statement,
  //  a test expression, and an increment expression , loop index variable
  //  should be of an integer type. IsInclusiveUpperBound is true when <= or
  //  >= is used for loop condition.
  //  E.g. the usage in CSageCodeGen::init_range_analysis()
  //  /////////////////////////////////////////////////////////////////////  /
  int IsCanonicalForLoop(void *for_loop, void **ivar = nullptr,
                         void **lb = nullptr, void **ub = nullptr,
                         void **step = nullptr, void **body = nullptr,
                         bool *hasIncrementalIterationSpace = nullptr,
                         bool *isInclusiveUpperBound = nullptr);

  //  ZP: 20150725: this func is deprecated, recommend to use IsCanonicalForLoop
  //  instead
  int ParseOneForLoop(void *sg_for_stmt, void **var, void **start, void **end,
                      void **step, void **cond_op, void **body, int *ulimit);

  int GetLoopStepValueFromExpr(void *step_expr, int64_t *value,
                               void **sg_value = nullptr);  //  return 0 if not
                                                            // available (e.g.
                                                            // not constant)
  int is_innermost_for_loop(void *for_loop);
  void *GetSpecifiedLoopByPragma(void *pragma_decl);
  int is_perfectly_nested(void *sg_loop);
  void get_perfectly_nested_loops(void *sg_top_loop,
                                  std::vector<void *> *nested_loops);
  void
  get_perfectly_nested_loops_from_bottom(void *sg_loop,
                                         std::vector<void *> *nested_loops);
  std::vector<int> get_loop_order_vector_in_function(void *sg_pos);
  std::string get_internal_loop_label(void *sg_loop);
  /************** update loop info ***************************************/
  //  start is the real lower bound
  //  end is the real upper bound + 1
  int CanonicalizeForLoop(void **sg_for_loop, bool transform);
  int StandardizeForLoop(void **sg_for_loop_);

  /****************************** Create loop  ********************/
  void *CreateWhileLoop(void *sg_cond_exp, void *sg_body,
                        void *bindNode = nullptr);
  //  sg_init SgStatement, sg_test SgStatement, incr_exp SgExpression,
  //  sg_body SgBasicBlock
  void *CreateForLoop(void *sg_init, void *sg_test, void *sg_incr_exp_,
                      void *sg_body, void *bindNode = nullptr);

  /************************IF STATEMENT *************************************/
  /************************* If statement info ****************************/
  void *GetIfStmtTrueBody(void *sg_if);
  void *GetIfStmtCondition(void *sg_if);
  void *GetIfStmtFalseBody(void *sg_if);

  /****************** update If statement *********************************/
  void SetIfStmtTrueBody(void *sg_if, void *new_body);
  void SetIfStmtFalseBody(void *sg_if, void *new_body);

  /***************** create If statement **********************************/
  void *CreateIfStmt(void *sg_cond_exp_, void *sg_if_, void *sg_else_,
                     void *bindNode = nullptr);

  /********************* TRACE TREE  ***************************************/
  void TraverseSimple(void *sg_scope, const std::string &sOrder,
                      t_func_rose_simple_traverse pFunc, void *pFuncArg);

  template <typename Fn> void TraversePre(SgNode *n, Fn &&func) const {
    CSageWalker<Fn> *walker = new CSageWalker<Fn>(std::forward<Fn>(func));
    walker->traverse(n, preorder);
    delete walker;
  }

  template <typename Fn> void TraversePost(SgNode *n, Fn &&func) const {
    CSageWalker<Fn> *walker = new CSageWalker<Fn>(std::forward<Fn>(func));
    walker->traverse(n, postorder);
    delete walker;
  }

  /**************** Get all the type and derived type *******************/
  void GetNodesByType_compatible(void *sg_scope, const std::string &str_type,
                                 std::vector<void *> *vec_nodes);
  //  'str_type' is ROSE AST class name, such as 'SgVarRefExp'
  void GetNodesByType(void *sg_scope, const std::string &sOrder,
                      const std::string &str_type,
                      std::vector<void *> *vecNodes, bool compatible = false,
                      bool cross_func = false, bool keep_org = false);
  //  'v_type' is ROSE AST class enum number, such as 'V_SgVarRefExp'
  void GetNodesByType_int_compatible(void *sg_scope, int v_type,
                                     std::vector<void *> *vec_nodes);
  //  internal func to implement GetNodeByType(..., cross_func = 1, ...);
  int GetNodesByType_recur(void *sg_scope, const std::string &order,
                           const std::string &str_type,
                           std::vector<void *> *vecNodes,
                           bool compatible /*=false*/,
                           std::set<void *> *visited_func);

  void GetNodesByType_int(void *sg_scope, const std::string &sOrder, int v_type,
                          std::vector<void *> *vecNodes,
                          bool compatible = false);

  /************************* Trace up from bottom ***************************/
  //  from the parent of sg_node_
  void *GetEnclosingNode(const std::string &type_, void *sg_node_);
  void *TraceUpByType(void *sg_pos, const std::string &str_type);
  void *TraceUpByTypeCompatible(void *sg_pos,
                                int V_sg_type = V_SgScopeStatement) const;
  void *TraceUpToStatement(void *sg_pos) const {
    void *ret = TraceUpByTypeCompatible(sg_pos, V_SgStatement);
    if (!ret)
      return sg_pos;
    return ret;
  }
  void *GetScope(void *stmt_or_exp);
  void *TraceUpToGlobal(void *sg_pos) const {
    return TraceUpByTypeCompatible(sg_pos, V_SgGlobal);
  }
  void *TraceUpToScope(void *sg_pos) const {
    return TraceUpByTypeCompatible(sg_pos, V_SgScopeStatement);
  }
  void *TraceUpToFuncDecl(void *sg_pos) const {
    return TraceUpByTypeCompatible(sg_pos, V_SgFunctionDeclaration);
  }
  void *TraceUpToClassDecl(void *sg_pos) const {
    return TraceUpByTypeCompatible(sg_pos, V_SgClassDeclaration);
  }
  void *TraceUpToFuncCall(void *sg_pos) const {
    return TraceUpByTypeCompatible(sg_pos, V_SgFunctionCallExp);
  }
  void *TraceUpToForStatement(void *sg_pos) const {
    return TraceUpByTypeCompatible(sg_pos, V_SgForStatement);
  }
  void *TraceUpToBasicBlock(void *sg_pos) const {
    return TraceUpByTypeCompatible(sg_pos, V_SgBasicBlock);
  }
  //  trace up to either for- while- or dowhile- loop statement
  void *TraceUpToLoopScope(void *sg_pos, bool exclusive = false);

  void *TraceUpToAccess(void *sg_pos);
  void *TraceUpToIndex(void *sg_pos);
  void *TraceUpToExpression(void *sg_pos) const {
    return TraceUpByTypeCompatible(sg_pos, V_SgExpression);
  }
  void *TraceUpToRootExpression(void *sg_pos);
  void *TraceUpToInitializedName(void *sg_exp);

  void TraceUpFuncCallsGeneric(void *sg_scope, void *sg_func_decl_arg,
                               const t_func_trace_up &pFunc, void *pFuncArg);
  void TraceUpFuncArgSource(void *sg_scope, void *sg_func_decl_arg,
                            std::vector<void *> *vec_source);
  void *TraceUpToNonFloating(void *n);

  /******************* get specific nodes ********************/
  bool has_ref_in_scope(void *var_init, void *scope);
  void get_ref_in_scope(void *var_init, void *scope, std::vector<void *> *refs,
                        bool skip_access_intrinsic = false,
                        bool is_preorder = true);
  void get_ref_in_scope_recursive(void *var_init, void *scope,
                                  std::vector<void *> *vec_refs,
                                  bool skip_access_intrinsic = false);

  void get_ref_in_scope(void *var_init, void *scope,
                        const t_func_call_path &call_path,
                        std::vector<void *> *refs,
                        bool skip_access_intrinsic = false,
                        bool is_preorder = true);

  std::vector<void *> GetAllRefInScope(void *sg_name, void *scope);
  //  YX: 20180212 get variable references from floating node, such as array
  //  index
  void get_floating_var_refs(void *scope, std::vector<void *> *vec_refs);
  void get_floating_exprs(void *scope, std::vector<void *> *vec_res);
  void get_exprs_from_type(void *sg_type_, std::vector<void *> *vec_exprs);

  /************************** ANALYSIS ****************************/
  /************** def/use *********************************/
  //  return nullptr, if not single assignment.
  //  sg_var is an init_name: global, argument or local
  //  This function is conservative
  void *GetSingleAssignExpr(void *sg_var, void *pos = nullptr,
                            int reset_defuse = 0, void *scope = nullptr,
                            bool cross_function = true,
                            bool skip_range = false);
  void *GetSingleAssignExprForSingleDef(void *sg_var, void *single_def,
                                        void *scope = nullptr,
                                        bool cross_func = true);
  void *GetSingleAssignExpr_path(void *sg_var, const t_func_call_path &fn_path,
                                 void **new_pos, void *pos = nullptr,
                                 int reset_defuse = 0, void *scope = nullptr);

  std::vector<std::pair<void *, void *>>  //  a multi-map an init_name to its
                                          //  defs, at a certain position
  GetAllDefbyPosition(void *pos);

  std::vector<std::pair<void *, void *>>  //  a multi-map an init_name to its
                                          //  defs, at a certain position
  GetAllUsebyPosition(void *pos);

  std::vector<void *>  //  defs of an init_name , at a certain position
  GetVarDefbyPosition(void *init_name, void *pos, int simple_iterator = 0);

//  youxiang support pointer converative and accurate analysis
#if USED_CODE_IN_COVERAGE_TEST
  std::vector<void *>  //  defs of an init_name , at a certain position
  GetVarDefbyPosition_v2(void *init_name, void *pos);
#endif

#if USED_CODE_IN_COVERAGE_TEST
  std::vector<void *>  //  defs of an init_name , at a certain position
  GetVarUsebyPosition(void *init_name, void *pos);
#endif

  void set_function_for_liveness(void *func_decl);
  void get_liveness_in(void *sg_pos, std::vector<void *> *ret);  //  get the
                                                                 // init_name of
                                                                 // the alive
                                                                 // variables
  void get_liveness_out(void *sg_pos,
                        std::vector<void *> *ret);  //  get the
                                                    // init_name of
                                                    // the alive
                                                    // variables

  void get_liveness_for_loop(void *sg_loop, std::vector<void *> *in,
                             std::vector<void *> *out);

  /******************* read/write analysis ************************/
  bool is_interface_alias_write(void *ref, const std::set<void *> ports_alias,
                                bool top);
  int is_write_ref(void *ref);
  int is_write_type(void *ref);  //  add by Yuxin May 8 2015
  //  ZP: 20150818
  //  only work for variable ref or pntr ref
  int is_write_conservative(void *ref, bool recursive = true);
  int is_write_conservative(void *ref, void **new_def, bool is_address,
                            bool recursive);
  //  int is_read_conservative(void *ref) ;
  int has_read_conservative(void *ref,
                            bool recursive = true);  //  no matter if
                                                     // there is write
  int has_write_conservative(void *ref);  //  no matter if there is read
  //  array_ref: A (in A[e1][e2])
  //  cross-function
  bool array_ref_has_write_conservative(void *array_ref);
  bool array_ref_has_read_conservative(void *array_ref);
  int array_ref_is_write_conservative(void *array_ref);  //  0 for read, 1 for
                                                         // write, 2 for read &
                                                         // write
  //  ZP: standard function for both scalar and pointer, conservative,
  //  cross-function
  int has_write_in_scope(void *array, void *scope);
  int has_read_in_scope(void *array, void *scope);

  //  YX: only used in movable test for only pointer, conservative
  //  non-cross-function
  int has_write_in_scope_fast(void *var, void *scope);
  int has_read_in_scope_fast(void *var, void *scope);

  int has_side_effect(void *exp);
  //  #if  USED_CODE_IN_COVERAGE_TEST
  int has_side_effect_v2(void *exp, void *init_name = nullptr);
  //  #endif
  int has_lvalue_ref_in_scope(void *scope, void *name,
                              std::vector<void *> *v_ret_ref = nullptr);
  int is_lvalue(void *ref);
  int check_access_out_of_bound(void *sg_init, void *scope,
                                const vector<size_t> &depths,
                                bool *is_exact = nullptr,
                                string *msg = nullptr);
  int check_access_write_out_of_bound(void *sg_init, void *scope,
                                      const vector<size_t> &depths,
                                      bool *empty_access = nullptr,
                                      bool *is_exact = nullptr,
                                      string *msg = nullptr);
  int check_access_read_out_of_bound(void *sg_init, void *scope,
                                     const vector<size_t> &depths,
                                     bool *empty_access = nullptr,
                                     bool *is_exact = nullptr,
                                     string *msg = nullptr);
  /******************* movable test *****************************************/
  int is_movable_test(void *exp, void *move_to, void *move_from = nullptr);

  //  to check if any side effect on sg_ref if move sg_from to sg_to (before)
  //  This function is conservative
  int CheckMovable(void *sg_from, void *sg_to, void *sg_ref);

  /******************** Loop Invariant *********************************/

  bool IsLoopInvariant(void *for_loop, void *expr, void *pos);
  bool IsContinuousAccess(void *expr, void *for_loop, void *pos = nullptr);

  /************* Pointer and Array Analysis ***************************/
  //  This is the most advanced function
  //  sg_ref: A[e1][e2]  =>  sg_array=A  sg_indeces=e1,e2
  void parse_pntr_ref_new(void *sg_ref, void **sg_array,
                          std::vector<void *> *sg_indexes);

  //  pntr : A[e1][e2]  =>  array_ref=A
  int parse_array_ref_from_pntr(void *pntr, void **array_ref,
                                void *pos = nullptr, bool simplify_idx = true,
                                int adjusted_array_dim = -1);

  //  sg_ref: A (in A[e1][e2])  =>  sg_array=A  sg_indeces=e1,e2 //  stable and
  //  flagship simplify_idx: if true, CMarsExpress==0 will used in express
  //  simplification ignore_dim_limit: if true, pntr will be extracted without
  //  consider the array dim number
  void parse_pntr_ref_by_array_ref(void *sg_ref, void **sg_array,
                                   void **sg_pntr,
                                   std::vector<void *> *sg_indexes,
                                   int *pointer_dim, void *pos = nullptr,
                                   bool simplify_idx = true,
                                   int adjusted_array_dim = -1,
                                   int include_class = 0);
  // check whether the array element is accessed from variable reference
  // 'sg_ref' within scope 'scope'
  bool is_access_array_element(void *sg_ref, void *scope);
  void parse_pntr_ref_by_func_call(void *sg_ref, void **sg_func, void **sg_pntr,
                                   std::vector<void *> *sg_indexes,
                                   int *pointer_dim, void *pos = nullptr);

  int parse_standard_pntr_ref_from_array_ref(
      void *sg_ref, void **sg_array, void **sg_pntr, void **sg_base_type,
      std::vector<void *> *sg_indexes, int *pointer_dim, void *pos = nullptr);
  //  array_ref: A (in A[e1][e2])
  int is_array_ref_full_dimension(void *array_ref, void **sg_array,
                                  void **sg_pntr, vector<void *> *sg_indexes,
                                  int *pointer_dim);

  //  stable, call parse_pntr_ref_new
  void *get_array_from_pntr(void *pntr);

  //  stable, call parse_pntr_ref_by_array_ref
  void *get_pntr_from_var_ref(void *var_ref, void *pos = nullptr);

  //  stable
  int get_pntr_pntr_dim(void *pntr);
  int get_pntr_init_dim(void *pntr);
//

//  The simple version of parse_pntr_ref_by_array_ref
#if USED_CODE_IN_COVERAGE_TEST
  void get_pntr_ref_by_array_ref(void *sg_ref, void **sg_array, void **sg_pntr,
                                 std::vector<void *> *sg_indexes);
#endif

  //  ZP:20170606: Not stable!!! use get_pntr_from_var_ref() instead
  //  ref: A (in A[e1][e2])  =>  sg_pntr = A[e1][e2]
  void get_pntr_from_var(void *ref, void **sg_pntr);
  void *parse_pntr_type_in_function(void *sg_pntr, void **sg_array,
                                    void **base_type,
                                    std::vector<size_t> *vecSizes,
                                    int *pointer_dim);

  //  sg_ref: A[e1][e2]  =>  sg_array=A  sg_indeces=e1,e2
  void parse_pntr_ref(void *sg_ref, void **sg_array,
                      std::vector<void *> *sg_indexes);
  //  pntr is the whole PntrRefExp and var is the innermost VarRefExp
  int get_var_from_pntr_ref(void *pntr, void **var);
  //  pntr is the whole PntrRefExp and indexes is from rightmost dimension to
  //  the left
  int get_index_from_pntr_ref(void *pntr, std::vector<void *> *indexes);
  //  sub functions for get_dimension_by_ref
  int analyze_type_with_new_and_malloc(void *sg_array, void *sg_scope,
                                       std::vector<size_t> *vecSizes);

  bool parse_assign_to_pntr(void *parent, void **var_name, void **value);
  int parse_assign(void *parent, void **var_name, void **value);

  int get_ref_dimension(void *ref, std::vector<void *> *indexes);

  /*********************** scope analysis ******************************/

  int IsInScope(void *sg_node, void *sg_scope);
  bool IsScopeBreak(void *sg_break, void *sg_scope);
  bool IsScopeContinue(void *sg_break, void *sg_scope);
  //  The major function for induction analysis, call GetSingleAssignExpr
  //  [Notice] The input should be original in the AST node, because def/use
  //  result is used in analysis
  int analysis_induct_map_in_scope(void *ref_scope, void *boundary_scope,
                                   std::map<void *, void *> *map_var2exp,
                                   std::vector<void *> *inner_terms = nullptr);
  void *GetCommonScope(void *sg_pos1, void *sg_pos2);
  void *GetCommonScope(const std::vector<void *> &vec_pos);
  //  this function is similar with is_located_in_scope, but it
  //  will not do cross function analysis
  int is_located_in_scope_simple(void *sg_pos, void *sg_scope);
  //  Across funciton
  int is_located_in_scope(void *sg_pos, void *sg_scope);
  int is_located_in_scope(void *sg_pos, void *sg_scope,
                          const t_func_call_path &func_path);
  int is_located_in_scope(void *sg_pos, void *sg_scope,
                          const std::list<t_func_call_path> &vec_paths);

/*********************  call path analysis *****************************/
#if USED_CODE_IN_COVERAGE_TEST
  void filter_vec_node_in_path(const t_func_call_path &fn_path,
                               const std::vector<void *> &vec_in);
#endif
  //  each map is a possible path, there are multiple paths because there
  //  multiple calls of a func
  //  std::map<func_decl, func_call>
  int get_call_paths(void *sg_pos, void *sg_scope,
                     std::list<t_func_call_path> *vec_paths);
  int get_call_paths_int(void *sg_pos, void *sg_scope,
                         std::set<void *> *visited,
                         std::list<t_func_call_path> *vec_paths);
  std::string print_func_path(const t_func_call_path &fn_path);

  void get_all_func_path_in_scope(void *sg_scope,
                                  std::list<t_func_call_path> *vec_paths);
  void get_all_func_path_in_scope_int(void *range_scope,
                                      std::list<t_func_call_path> *vec_paths);

  void get_all_func_path_in_scope(void *array, void *sg_scope,
                                  std::list<t_func_call_path> *vec_paths);
  void get_all_func_path_in_scope_int(void *array, void *range_scope,
                                      std::list<t_func_call_path> *vec_paths);
  //  YX: support simple pointer alias
  void get_all_func_path_in_scope_int_v2(void *array, void *range_scope,
                                         std::list<t_func_call_path> *vec_paths,
                                         std::set<void *> *visited, bool top);
  int is_located_in_path(void *sg_pos, t_func_call_path fn_path);

  //  for array variable reference
  int is_located_in_path_v2(void *ref, t_func_call_path fn_path);
  int is_included_in_path(const std::pair<void *, void *> &call_site,
                          t_func_call_path fn_path);

  int any_var_located_in_scope(void *term, void *scope);

  void *get_call_in_path(void *func_decl, const t_func_call_path &call_path);

  void *GetCommonPosition(void *sg_pos1, void *sg_pos2);
  void *GetCommonPosition(void *sg_pos1, const t_func_call_path &path1,
                          void *sg_pos2, const t_func_call_path &path2);
  void *GetCommonPosition(const std::vector<void *> &vec_pos);
  t_func_call_path GetCommonPath(const t_func_call_path &path1,
                                 const t_func_call_path &path2);

  /*********************** Range analysis *************************************/
  int GetVarRangeFromCompareOp(void *expr, std::map<void *, void *> *lbs,
                               std::map<void *, void *> *ubs,
                               bool true_value = true);

  int GetVarDefRangeFromCompareOp(void *expr, std::map<void *, int64_t> *lbs,
                                  std::map<void *, int64_t> *ubs,
                                  bool true_value = true);

  int GetExprRangeFromCompareOp(void *expr, std::map<void *, int64_t> *lbs,
                                std::map<void *, int64_t> *ubs,
                                bool true_value = true);

  void reset_range_analysis();
  void reset_expr_range_analysis();
  void init_defuse_range_analysis() {
    init_defuse();
    reset_range_analysis();
  }
  void init_range_analysis_for_loop(void *for_loop);
  /**********************  Pointer alias analysis *****************************/
  void *get_alias_array_from_assignment_expression(void *sg_pntr);
  void *get_alias_array_from_assignment_expression(void *sg_pntr,
                                                   int *is_memcpy);
  int IsRecursivePointerAlias(void *var_init, bool *confidence_of_negative);
  std::set<void *> GetAssociatedAlias(void *array, bool *confidence);
  int IsRecursivePointerAlias(void *var_init, bool *confidence_of_negative,
                              std::set<void *> *visited);
  std::vector<void *> get_alias_source(void *target_init, void *pos);

  /*********************** TREE OPERATION
   * **************************************/
  /****************** Insert *****************************/
  void InsertStmt(void *sg_stmt_, void *sg_before_, bool avoid_pragma = false,
                  bool before_label = true);
  void InsertAfterStmt(void *sg_stmt_, void *sg_before_);
  void InsertStmtAvoidLabel(void *sg_stmt_, void **sg_before_);
  void PrependChild(void *sg_scope_, void *sg_child_,
                    bool insert_after_pragma = false);
  void AppendChild(void *sg_scope_, void *sg_child_);
  void myStatementInsert(SgStatement *target, SgStatement *newstmt, bool before,
                         bool allowForInit = false);

  /*****************  Copy ******************************/
  void *CopyChild(void *sg_node);
  void *CopyStmt(void *sg_stmt_, bool passMajor = false);
  void *CopyVarDeclStmt(void *sg_stmt_, void *scope);
  void *CopyInitName(void *sg_init);
  void *CopyScope(void *sg_scope_);
  void *CopyExp(void *sg_exp_, bool passMajor = false);
#if USED_CODE_IN_COVERAGE_TEST
  void CopyPreprocInfo(void *sg_to_, void *sg_from_);
#endif

  /**************  Replace ******************************/
  void SetLabelName(void *sg_label, const std::string &new_name);
  void ReplaceChild(void *sg_old_, void *sg_new_,
                    bool replace_prep_info = true);

  bool IsSupportedReplaceExpressionParent(void *exp_parent);
  void ReplaceExp(void *sg_old_, void *sg_new_, bool keepOldExp = true);
  void ReplaceExpWithStmt(void *sg_old_);
  void ReplaceStmt(void *sg_old, void *sg_new, bool replace_prep_info = true) {
    ReplaceChild(sg_old, sg_new, replace_prep_info);
  }

  void replace_var_refs_in_type(void *sg_type_, void *pos);
  void replace_floating_var_refs(void *scope);
  //  replace the variable reference in a scope
  void replace_variable_references_in_scope(void *sg_init_old,
                                            void *sg_init_new, void *scope);
  void replace_with_dummy_decl(void *stmt);

  /*************  Remove or Delete ************************/
  void DeleteNode(void *sg_node);
  void RemoveChild(void *sg_child_,
                   bool remove_prep_info = true);  //  for a statement
  void DiscardPreprocInfo(void *sg_stmt_or_exp);
  void DiscardIncludePreprocInfo(void *sg_stmt_or_exp);
  void RemoveStmt(void *sg_stmt, bool remove_prep_info = true) {
    RemoveChild(sg_stmt, remove_prep_info);
  }
  void RemoveLabel(void *sg_node);
  int RemoveRedundantContinueStmt(void *body);
  void myRemoveStatement(SgStatement *stmt);
  /***********   Complex operation **********************/
  bool dead_stmts_removal(void *scope);
  bool remove_dead_parameters(void **func_decl);
  void fix_defuse_issues(void *scope = nullptr, bool parse_aggregate = true);
  int add_missing_brace(void *scope);
  int add_label(const std::string &label_name, void *loop);
  void remove_double_brace(void *sg_scope = nullptr);
  bool simplify_pointer_access(void *scope);
  void remove_brace_after_label(void *sg_scope = nullptr);
  int add_label_scope(void *sg_loop);
  bool replace_sizeof(void *scope);
  void clean_empty_aggregate_initializer(void *sg_scope);
  bool clean_implict_conversion_operator(void *sg_scope);
  void recover_hls_namespace_prefix(void *scope);
  void function_outline(void *node_stmt, void **func_call, void **func_decl,
                        const std::string &func_name_prefix,
                        bool add_number_suffix, bool remove_dead_code,
                        bool full, bool keep_orig_stmt, bool keep_global_var);
  /*********** PREPROCESS DIRECTIVE *******************************/

  //  processing pragma with token
  void GetTokens(std::vector<std::string> *tokens, const std::string &str,
                 const char *sep = nullptr);
  void MergeTokens(const std::vector<std::string> &tokens, std::string *ret);
  void ReplacePragmaTokens(
      void *scope,
      const std::unordered_map<std::string, std::string> &def_directive_map_,
      const char *sep = nullptr);
  void RewritePragmaTokens(
      void *pragma,
      const std::unordered_map<std::string, std::string> &def_directive_map_,
      const char *sep = nullptr);
  string FindTokenAttribute(const std::vector<string> &tokens,
                            const std::string &str);
  void BuildPragmaCache();
  set<void *> get_variable_used_in_pragma(void *pragma);
  set<void *> get_pragma_related_to_variable(void *sg_var);

  //  insert macros, can be used any text, but no syntax meaning (not recommend)
  void AddDirectives(const std::string &sDirective, void *sg_node_before_,
                     int before = 1);
#if USED_CODE_IN_COVERAGE_TEST
  void PrependDirectives(const std::string &sDirective, void *sg_scope);
#endif
  void AddHeader(const std::string &sDirective,
                 void *sg_file_pos);  //  sg_pos specify the file to
  //  add the header, sg_pos
  //  should be in the file
  void InsertHeader(const std::string &header_file_name, void *sg_stmt,
                    int before = 1, int isSystemHeader = 0);
  void AddComment(const std::string &sComment, void *sg_node_before);

  /********************message infastructure
   * *************************************/
  std::string get_identifier_from_ast(void *sg_node);
  std::string GetLocationFromIdentifier(const std::string &id);
  void InsertIdentifierInformation(const std::string &id,
                                   const std::string &location);
  void InsertIdentifierInformation(
      const std::map<std::string, std::string> &id2location);
#if USED_CODE_IN_COVERAGE_TEST
  void UpdateLocationFromIdentifier(const std::string &id,
                                    const std::string &location);
#endif

  /*************  ROSE AST CLASS WRAPPER***********************************/
  bool IsMemberFunctionRefExp(void *ref_exp) const {
    return isSgMemberFunctionRefExp(static_cast<SgNode *>(ref_exp)) != nullptr;
  }
  int IsCompoundLiteralExp(void *sg_exp) const {
    return isSgCompoundLiteralExp(static_cast<SgNode *>(sg_exp)) != nullptr;
  }
  int IsBasicBlock(void *sg_node) const {
    return isSgBasicBlock(static_cast<SgNode *>(sg_node)) != 0;
  }
  bool IsSingleChild(void *sg_node) const {
    void *parent = GetParent(sg_node);
    if (IsBasicBlock(parent) != 0 && GetChildStmtNum(parent) == 1) {
      return true;
    }
    return false;
  }
  int IsScopeStatement(void *sg_node) const {
    return isSgScopeStatement(static_cast<SgNode *>(sg_node)) != 0;
  }
  bool IsPragma(void *sg_node) const {
    return isSgPragma(static_cast<SgNode *>(sg_node)) != 0;
  }

  bool IsPragmaDecl(void *sg_node) const {
    return isSgPragmaDeclaration(static_cast<SgNode *>(sg_node)) != 0;
  }

  int IsStatement(void *sg_node) const {
    return isSgStatement(static_cast<SgNode *>(sg_node)) != 0;
  }
  int IsExprStatement(void *sg_node) const {
    return isSgExprStatement(static_cast<SgNode *>(sg_node)) != 0;
  }
  int IsExprList(void *sg_node) const {
    return isSgExprListExp(static_cast<SgNode *>(sg_node)) != 0;
  }
  int IsCommaExp(void *sg_node) const {
    return isSgCommaOpExp(static_cast<SgNode *>(sg_node)) != 0;
  }
  int IsInitName(void *sg_node) const {
    return isSgInitializedName(static_cast<SgNode *>(sg_node)) != 0;
  }

  int IsInitializer(void *sg_node) const {
    return isSgInitializer(static_cast<SgNode *>(sg_node)) != 0;
  }
  int IsAggregateInitializer(void *sg_node) const {
    return isSgAggregateInitializer(static_cast<SgNode *>(sg_node)) != 0;
  }
  int IsAssignInitializer(void *sg_node) const {
    return isSgAssignInitializer(static_cast<SgNode *>(sg_node)) != 0;
  }
  //  Not compilable in the current ROSE, but it is specified in the latest ROSE
  //  doc
  //  int IsBracedInitializer(void *sg_node) const {
  //    return isSgBracedInitializer(static_cast<SgNode *>(sg_node)) != 0;
  //  }
  int IsCompoundInitializer(void *sg_node) const {
    return isSgCompoundInitializer(static_cast<SgNode *>(sg_node)) != 0;
  }
  bool IsConstructorInitializer(void *sg_node) const {
    return isSgConstructorInitializer(static_cast<SgNode *>(sg_node)) != 0;
  }
  int IsDesignatedInitializer(void *sg_node) const {
    return isSgDesignatedInitializer(static_cast<SgNode *>(sg_node)) != 0;
  }
  int IsForStatement(void *sg_node) const {
    return isSgForStatement(static_cast<SgNode *>(sg_node)) != 0;
  }
  int IsSwitchStatement(void *sg_node) const {
    return isSgSwitchStatement(static_cast<SgNode *>(sg_node)) != 0;
  }
  int IsCaseOptionStmt(void *sg_node) const {
    return isSgCaseOptionStmt(static_cast<SgNode *>(sg_node)) != 0;
  }
  int IsDefaultOptionStmt(void *sg_node) const {
    return isSgDefaultOptionStmt(static_cast<SgNode *>(sg_node)) != 0;
  }
  int IsForInitStatement(void *sg_node) const {
    return isSgForInitStatement(static_cast<SgNode *>(sg_node)) != 0;
  }
  int IsWhileStatement(void *sg_node) const {
    return isSgWhileStmt(static_cast<SgNode *>(sg_node)) != 0;
  }
  int IsDoWhileStatement(void *sg_node) const {
    return isSgDoWhileStmt(static_cast<SgNode *>(sg_node)) != 0;
  }
  int IsBreakStatement(void *sg_node) const {
    return isSgBreakStmt(static_cast<SgNode *>(sg_node)) != 0;
  }
  int IsContinueStatement(void *sg_node) const {
    return isSgContinueStmt(static_cast<SgNode *>(sg_node)) != 0;
  }
  int IsLabelStatement(void *sg_node) const {
    return isSgLabelStatement(static_cast<SgNode *>(sg_node)) != 0;
  }
  int IsGotoStatement(void *sg_node) const {
    return isSgGotoStatement(static_cast<SgNode *>(sg_node)) != 0;
  }

  int IsIfStatement(void *sg_node) const {
    return isSgIfStmt(static_cast<SgNode *>(sg_node)) != 0;
  }

  int IsReturnStatement(void *sg_node) const {
    return isSgReturnStmt(static_cast<SgNode *>(sg_node)) != 0;
  }

  int IsNullExp(void *sg_node) const {
    return isSgNullExpression(static_cast<SgNode *>(sg_node)) != nullptr;
  }

  int IsFunctionDefinition(void *sg_node);

  int IsFunctionCall(void *sg_node) const {
    return isSgFunctionCallExp(static_cast<SgNode *>(sg_node)) != 0 ||
           isSgConstructorInitializer(static_cast<SgNode *>(sg_node)) != 0;
  }
  //  int IsConstructorInitializer(void *sg_node) const {
  //    return isSgConstructorInitializer(static_cast<SgNode *>(sg_node)) != 0;
  //  }
  int IsFunctionDeclaration(void *sg_node) const {
    return isSgFunctionDeclaration(static_cast<SgNode *>(sg_node)) != 0;
  }
  int IsFunctionRefExp(void *sg_node) const {
    return isSgFunctionRefExp(static_cast<SgNode *>(sg_node)) != nullptr;
  }
  int IsRecursiveFunction(void *func_decl, void **rec_func);

  int IsTemplateInstantiationFunctionDecl(void *sg_node) const {
    return isSgTemplateInstantiationFunctionDecl(
               static_cast<SgNode *>(sg_node)) != 0;
  }

  int IsTemplateInstantiationMemberFunctionDecl(void *sg_node) const {
    return isSgTemplateInstantiationMemberFunctionDecl(
               static_cast<SgNode *>(sg_node)) != 0;
  }

  int IsTemplateFunctionDecl(void *sg_node) const {
    return isSgTemplateFunctionDeclaration(static_cast<SgNode *>(sg_node)) != 0;
  }

  int IsTemplateMemberFunctionDecl(void *sg_node) const {
    return isSgTemplateMemberFunctionDeclaration(
               static_cast<SgNode *>(sg_node)) != 0;
  }

  int IsTemplateInstantiationDirectiveStatement(void *sg_node) const {
    return isSgTemplateInstantiationDirectiveStatement(
               static_cast<SgNode *>(sg_node)) != 0;
  }

  int IsUsingDirectiveStmt(void *sg_node) const {
    return isSgUsingDirectiveStatement(static_cast<SgNode *>(sg_node)) !=
           nullptr;
  }

  int IsNameSpaceDeclStmt(void *sg_node) const {
    return isSgNamespaceDeclarationStatement(static_cast<SgNode *>(sg_node)) !=
           nullptr;
  }

  int IsNameSpaceDefStmt(void *sg_node) const {
    return isSgNamespaceDefinitionStatement(static_cast<SgNode *>(sg_node)) !=
           nullptr;
  }

  int IsTemplateInstClassDecl(void *sg_node) const {
    return isSgTemplateInstantiationDecl(static_cast<SgNode *>(sg_node)) != 0;
  }

  int IsReturnStmt(void *sg_node) const {
    return isSgReturnStmt(static_cast<SgNode *>(sg_node)) != 0;
  }

  int IsTemplateClassDecl(void *sg_node) const {
    return isSgTemplateClassDeclaration(static_cast<SgNode *>(sg_node)) != 0;
  }

  bool IsVariableDecl(void *sg_node) const {
    return isSgVariableDeclaration(static_cast<SgNode *>(sg_node)) != 0;
  }

  int IsVarRefExp(void *sg_node) const {
    return isSgVarRefExp(static_cast<SgNode *>(sg_node)) != 0;
  }
  int IsPntrArrRefExp(void *sg_node) const {
    return isSgPntrArrRefExp(static_cast<SgNode *>(sg_node)) != 0;
  }
  int IsDotExp(void *sg_node) const {
    return isSgDotExp(static_cast<SgNode *>(sg_node)) != 0;
  }
  int IsArrowExp(void *sg_node) const {
    return isSgArrowExp(static_cast<SgNode *>(sg_node)) != 0;
  }
  int IsAddOp(void *sg_node) const {
    return isSgAddOp(static_cast<SgNode *>(sg_node)) != 0;
  }
  int IsMultiplyOp(void *sg_node) const {
    return isSgMultiplyOp(static_cast<SgNode *>(sg_node)) != 0;
  }
  int IsDivideOp(void *sg_node) const {
    return isSgDivideOp(static_cast<SgNode *>(sg_node)) != 0;
  }
  int IsSubtractOp(void *sg_node) const {
    return isSgSubtractOp(static_cast<SgNode *>(sg_node)) != 0;
  }

  int IsFunctionType(void *sg_type) const {
    return isSgFunctionType(static_cast<SgNode *>(sg_type)) != nullptr;
  }
  int IsReferenceType(void *sg_type) const {
    return isSgReferenceType(static_cast<SgNode *>(sg_type)) != nullptr;
  }
  int IsNonConstReferenceType(void *sg_type) const;

  int IsPointerType(void *sg_type) const {
    return isSgPointerType(static_cast<SgNode *>(sg_type)) != nullptr;
  }
  int IsRestrictType(void *sg_type);

  int IsArrayType(void *sg_type) const {
    return isSgArrayType(static_cast<SgNode *>(sg_type)) != nullptr;
  }
  int IsTypedefDecl(void *sg_stmt) const {
    return isSgTypedefDeclaration(static_cast<SgNode *>(sg_stmt)) != nullptr;
  }
  int IsTypedefType(void *sg_type) const {
    return (isSgTypedefType(static_cast<SgNode *>(sg_type)) != 0);
  }
  int IsModifierType(void *sg_type) const {
    return (isSgModifierType(static_cast<SgNode *>(sg_type)) != 0);
  }

  int IsBoolType(void *sg_type) const {
    return isSgTypeBool(static_cast<SgNode *>(sg_type)) != nullptr;
  }
  //  if the type is char, short, int, long, int64_t
  int IsIntegerType(void *sg_type) const {
    SgType *type = isSgType(static_cast<SgNode *>(sg_type));
    return type && type->isIntegerType();
  }
  int IsIntegerType(int v_type);
  //  for xilinx ap_int type, get its nearest native integer type
  int GetNearestIntegerType(void *xilinx_ap_type);
  //  if the type is (signed) int
  int IsIntType(void *sg_type) const {
    return isSgTypeSignedInt(static_cast<SgNode *>(sg_type)) != 0 ||
           isSgTypeInt(static_cast<SgNode *>(sg_type)) != nullptr;
  }
  int IsUnsignedIntType(void *sg_type) const {
    return isSgTypeUnsignedInt(static_cast<SgNode *>(sg_type)) != nullptr;
  }
  bool IsComplexType(void *type) const {
    return isSgTypeComplex(static_cast<SgNode *>(type)) != nullptr;
  }
  int IsVoidType(void *sg_type) const {
    return isSgTypeVoid(static_cast<SgNode *>(sg_type)) != nullptr;
  }

  int IsFloatType(void *sg_type) const {
    SgType *type = isSgType(static_cast<SgNode *>(sg_type));
    return type && type->isFloatType();
  }
  int IsShortType(void *sg_type) const {
    return isSgTypeShort(static_cast<SgNode *>(sg_type)) != 0 ||
           isSgTypeSignedShort(static_cast<SgNode *>(sg_type)) != nullptr;
  }
  int IsUnsignedShortType(void *sg_type) const {
    return isSgTypeUnsignedShort(static_cast<SgNode *>(sg_type)) != nullptr;
  }
  int IsCharType(void *sg_type) const {
    return isSgTypeChar(static_cast<SgNode *>(sg_type)) != 0 ||
           isSgTypeSignedChar(static_cast<SgNode *>(sg_type)) != nullptr;
  }

  int IsUnsignedCharType(void *sg_type) const {
    return isSgTypeUnsignedChar(static_cast<SgNode *>(sg_type)) != nullptr;
  }

  int IsUnsignedType(int v_type) const {
    return v_type == V_SgTypeUnsignedChar || v_type == V_SgTypeUnsignedShort ||
           v_type == V_SgTypeUnsignedInt || v_type == V_SgTypeUnsignedLong ||
           v_type == V_SgTypeUnsignedLongLong;
  }

  int GetIntTypeBitWidth(int v_type);

  int IsSgTypeString(void *sg_type) const {
    return isSgTypeString(static_cast<SgNode *>(sg_type)) != nullptr;
  }

  int IsType(void *sg_type) const {
    return isSgType(static_cast<SgNode *>(sg_type)) != nullptr;
  }

  int IsLongLongType(void *sg_type) const {
    return isSgTypeLongLong(static_cast<SgNode *>(sg_type)) != nullptr;
  }
  int IsUnsignedLongLongType(void *sg_type) const {
    return isSgTypeUnsignedLongLong(static_cast<SgNode *>(sg_type)) != nullptr;
  }
  int IsLongType(void *sg_type) const {
    return isSgTypeLong(static_cast<SgNode *>(sg_type)) != nullptr;
  }
  int IsUnsignedLongType(void *sg_type) const {
    return isSgTypeUnsignedLong(static_cast<SgNode *>(sg_type)) != nullptr;
  }
  int IsGeneralLongLongType(void *sg_type_) const {
    SgType *sg_type = isSgType(static_cast<SgNode *>(sg_type_));
    return isSgTypeLongLong(sg_type) || isSgTypeUnsignedLongLong(sg_type) ||
           isSgTypeSignedLongLong(sg_type);
  }
  int IsIntegerTypeRecur(void *sg_type) const {
    return IsIntegerType(GetBaseType(sg_type, true));
  }

  int IsEnumVal(void *sg_exp) const {
    return isSgEnumVal(static_cast<SgNode *>(sg_exp)) != nullptr;
  }

  int IsEnumDecl(void *sg_stmt) const {
    return isSgEnumDeclaration(static_cast<SgNode *>(sg_stmt)) != nullptr;
  }

  int IsDefaultType(void *sg_type) const {
    return isSgTypeDefault(static_cast<SgNode *>(sg_type)) != nullptr;
  }

  int IsClassDecl(void *sg_stmt) const {
    return isSgClassDeclaration(static_cast<SgNode *>(sg_stmt)) != nullptr;
  }
  int IsClassDefinition(void *sg_stmt) const {
    return isSgClassDefinition(static_cast<SgNode *>(sg_stmt)) != nullptr;
  }
  int IsPlusAssignOp(void *sg_exp) const {
    return isSgPlusAssignOp(static_cast<SgNode *>(sg_exp)) != nullptr;
  }
  int IsAssignOp(void *sg_exp) const {
    return isSgAssignOp(static_cast<SgNode *>(sg_exp)) != nullptr;
  }
  int IsAssignOp(int opcode) const { return V_SgAssignOp == opcode; }
  bool IsGeneralAssignOp(void *sg_exp, void **left_exp, void **right_exp);

  int IsCompoundAssignOp(void *sg_exp) const {
    return isSgCompoundAssignOp(static_cast<SgNode *>(sg_exp)) != nullptr;
  }
  int IsPlusPlusOp(void *sg_exp) const {
    return isSgPlusPlusOp(static_cast<SgNode *>(sg_exp)) != nullptr;
  }
  int IsMinusMinusOp(void *sg_exp) const {
    return isSgMinusMinusOp(static_cast<SgNode *>(sg_exp)) != nullptr;
  }
  int IsMinusAssignOp(void *sg_exp) const {
    return isSgMinusAssignOp(static_cast<SgNode *>(sg_exp)) != nullptr;
  }
  int IsMultAssignOp(void *sg_exp) const {
    return isSgMultAssignOp(static_cast<SgNode *>(sg_exp)) != nullptr;
  }
  int IsDivAssignOp(void *sg_exp) const {
    return isSgDivAssignOp(static_cast<SgNode *>(sg_exp)) != nullptr;
  }
  int IsUnaryOp(void *sg_exp) const {
    return isSgUnaryOp(static_cast<SgNode *>(sg_exp)) != nullptr;
  }
  int IsUnaryOp(int opcode) const { return opcode == V_SgAssignOp; }
  int IsBinaryOp(void *sg_exp) const {
    return isSgBinaryOp(static_cast<SgNode *>(sg_exp)) != nullptr;
  }
  int IsBinaryOp(int opcode);
  int IsCompoundAssignOp(int opcode);
  int IsBinaryAssign(SgBinaryOp *exp);
  int IsLessThanOp(void *sg_exp) const {
    return isSgLessThanOp(static_cast<SgNode *>(sg_exp)) != nullptr;
  }
  int IsLessOrEqualOp(void *sg_exp) const {
    return isSgLessOrEqualOp(static_cast<SgNode *>(sg_exp)) != nullptr;
  }
  int IsGreaterThanOp(void *sg_exp) const {
    return isSgGreaterThanOp(static_cast<SgNode *>(sg_exp)) != nullptr;
  }
  int IsGreaterOrEqualOp(void *sg_exp) const {
    return isSgGreaterOrEqualOp(static_cast<SgNode *>(sg_exp)) != nullptr;
  }
  int IsEqualOp(void *sg_exp) const {
    return isSgEqualityOp(static_cast<SgNode *>(sg_exp)) != nullptr;
  }
  int IsNonEqualOp(void *sg_exp) const {
    return isSgNotEqualOp(static_cast<SgNode *>(sg_exp)) != nullptr;
  }
  int IsNotOp(void *sg_exp) const {
    return isSgNotOp(static_cast<SgNode *>(sg_exp)) != nullptr;
  }
  int IsAndOp(void *sg_exp) const {
    return isSgAndOp(static_cast<SgNode *>(sg_exp)) != nullptr;
  }
  int IsOrOp(void *sg_exp) const {
    return isSgOrOp(static_cast<SgNode *>(sg_exp)) != nullptr;
  }
  int IsCastExp(void *sg_exp) const {
    return isSgCastExp(static_cast<SgNode *>(sg_exp)) != nullptr;
  }
  int IsAddressOfOp(void *sg_exp) const {
    return isSgAddressOfOp(static_cast<SgNode *>(sg_exp)) != nullptr;
  }
  int IsPointerDerefExp(void *sg_exp) const {
    return isSgPointerDerefExp(static_cast<SgNode *>(sg_exp)) != nullptr;
  }
  int IsGlobal(void *sg_node) const {
    return isSgGlobal(static_cast<SgNode *>(sg_node)) != nullptr;
  }
  int IsFile(void *sg_node) const {
    return isSgFile(static_cast<SgNode *>(sg_node)) != nullptr;
  }
  int IsProject(void *sg_node) const {
    return isSgProject(static_cast<SgNode *>(sg_node)) != nullptr;
  }
  int IsExpression(void *sg_node) const {
    return isSgExpression(static_cast<SgNode *>(sg_node)) != 0;
  }

  /************************** Misc
   * **********************************************/
  inline int IsNumberString(string str) {
    int flag = 0;
    for (size_t i = 0; i < str.length(); i++) {
      if (str[i] >= '0' && str[i] <= '9') {
      } else {
        flag = 1;
      }
    }
    if (flag == 1) {
      return 0;
    }
    return 1;
  }
  int IsStructElement(string str, void *kernel);
  vector<void *> GetUniqueInterfacePragma(void *sg_init, CMarsIrV2 *mars_ir);
  vector<string> GetUniqueInterfaceAttribute(void *sg_init, string attribute,
                                             CMarsIrV2 *mars_ir);
  void replace_depth(string depth_all, void *pragma, void *kernel,
                     CMarsIrV2 *mars_ir);
  /********************** reverse arguments ********************************/
  void InsertStmt_v1(void *sg_ref_, void *sg_stmt_, bool avoid_pragma = false) {
    InsertStmt(sg_stmt_, sg_ref_, avoid_pragma);
  }
  void InsertAfterStmt_v1(void *sg_ref_, void *sg_stmt_) {
    InsertAfterStmt(sg_stmt_, sg_ref_);
  }
  void AddComment_v1(void *sg_node_before, const std::string &sDirective) {
    AddComment(sDirective, sg_node_before);
  }
  void AppendChild_v1(void *sg_child_, void *sg_scope_) {
    AppendChild(sg_scope_, sg_child_);
  }
  void PrependChild_v1(void *sg_child_, void *sg_scope_,
                       bool insert_after_pragma = false) {
    PrependChild(sg_scope_, sg_child_, insert_after_pragma);
  }
  //  Output
  void GeneratePDF();
  void GenerateCode();
  void sanity_check();
  //  floating node means the temporarily generated code that has no been insert
  //  to AST
  bool is_floating_node(void *sg_node) const;

  //  check name is visible in the current position 'sg_pos', including symbol
  //  defined in sub scope(s) and its parent scope(s)
  bool check_name_visible_in_position(void *sg_pos, const std::string &name);
  void get_valid_name(void *sg_pos, std::string *name);

  //  check name is visible in the current function 'kernel_decl', including
  //  symbol defined in sub scope(s) if 'check_sub_scope' is true and its parent
  //  scope(s)
  bool check_name_visible_in_function(void *kernel_decl,
                                      const std::string &name,
                                      bool check_sub_scope);
  void get_valid_local_name(void *func_decl, std::string *name);

  //  check name is visible in the global scope 'global_scope', including symbol
  //  defined in sub scope(s) if 'check_sub_scope' is true
  bool check_name_visible_in_global_scope(void *global_scope,
                                          const std::string &name,
                                          bool check_sub_scope);
  void get_valid_global_name(void *global_scope, std::string *name);
  // check name is visible within the funciton 'kernel_decl', including symbol
  // defined in sub scope(s) but not its parent scope
  bool check_name_declared_within_function(void *kernel_decl,
                                           const std::string &name);
  void get_loop_nest_from_ref(void *curr_loop, void *ref,
                              std::set<void *> *loops,
                              bool include_while = false,
                              bool skip_scope = false);
  std::string legalizeName(const std::string &name);

  void PropagatePragma(void *var, void *scope, const std::string &new_var,
                       bool third_party = false,
                       std::vector<void *> *new_pragmas = nullptr);

  //  Across-Function Analysis
  int get_surrounding_control_stmt(void *sg_pos, void *sg_scope,
                                   std::vector<void *> *sg_loops_ifs);
  int get_surrounding_control_stmt(
      void *sg_pos, void *sg_scope,
      std::vector<std::pair<void *, void *>> *sg_loops_if_with_lasts);
  int get_surrounding_control_stmt(
      void *sg_pos, void *sg_scope, const t_func_call_path &func_path,
      std::vector<std::pair<void *, void *>> *sg_loops_if_with_lasts,
      std::set<void *> *visited, bool reverse);

  void set_kernel_attribute(void *func_decl, const std::string &attr,
                            const std::string &val);
  //  Add by Yuxin
  SgVariableSymbol *getVariableSymbolFromName(void *scope,
                                              const std::string &varStr);

  //  Look up variable declaration from the innermost scope to global scope
  //  according to variable name
  SgInitializedName *getInitializedNameFromName(void *scope,
                                                const std::string &varStr,
                                                bool recursive_field);
  // check whether there is any kind of symbol in the scope, such as variable,
  // function, namespace, class, enum, label, etc
  bool CheckSymbolExits(void *scope, const std::string &varStr);

  std::string GetSgPntrArrName(void *expr);
  bool IsIncludeVar(void *expr, void *var);
  //  //////////////////////////////  /
  std::vector<void *> get_var_decl_in_for_loop(void *sg_for_loop);
  void *reset_array_from_array(void *array, void *base_type,
                               const vector<size_t> &nSizes, void *from_array,
                               void *bindNode);
  void *create_for_loop_for_reset_array(void *var_int, void *base_type,
                                        const std::vector<size_t> &nSizes,
                                        void *tmp_var, void *bindNode);
#if USED_CODE_IN_COVERAGE_TEST
  int fix_scope_issue(void *scope = nullptr);
#endif
  void init_defuse();

  void init_callgraph();

#if USED_CODE_IN_COVERAGE_TEST
  int is_single_def_expression(void *exp, void *pos);
#endif
  void InitRenameList(const std::map<std::string, std::vector<std::string>>
                          &mapRefRenameList);  //  std::map<std::string port,
  //  std::vector<string ref_name_list> >
  void ResetRefRenameList();
  std::string GetRefRenameAndInc(const std::string &port);

#if USED_CODE_IN_COVERAGE_TEST
  void clean_floating_expr_nodes(void *scope,
                                 std::string str_sg_type = "SgExpression");
#endif

  bool GetTopoOrderFunctions(void *top_func, std::vector<void *> *functions);

  int GetPntrReferenceOfArray(void *array, void *sg_scope,
                              std::vector<void *> *vec_access,
                              int allow_artifical_ref = 0);
  int analysis_array_flatten(void *array_init,
                             const std::vector<size_t> &old_dims);
  int apply_array_flatten(void *array_init,
                          const std::vector<size_t> &new_dims);
  int analysis_lift_scalar(void *scalar_init);
  int apply_lift_scalar(void *scalar_init);

  void *rebuild_pntr_ref(
      void *pntr_ref,
      void *sg_pos = nullptr);  //  rebuild the pntr by value propagation

  void *find_kernel_call(void *pragma_decl);
  std::string get_task_name_by_kernel(void *kernel);
  std::string get_wrapper_name_by_kernel(void *kernel);

  void splitString(std::string str, const std::string &delim,
                   std::vector<std::string> *results);

#if USED_CODE_IN_COVERAGE_TEST
  int ConvertFuncParamTypePointer2Array(void *sg_func_arg);
#endif

  std::vector<void *> CopyExprList(const std::vector<void *> &org_exp_list);

  //  ////////////////////////  /
  //  ZP: 20170817: temporary functions to be removed
  //  Replace by CreateFuncDecl and CreateFuncCall
  void appendExp(void *arg_list, void *exp) {
    SageInterface::appendExpression(static_cast<SgExprListExp *>(arg_list),
                                    static_cast<SgExpression *>(exp));
  }

  void appendArg(void *arg_list, void *init) {
    SageInterface::appendArg(static_cast<SgFunctionParameterList *>(arg_list),
                             static_cast<SgInitializedName *>(init));
  }
  void *buildFunctionParameterList() {
    return SageBuilder::buildFunctionParameterList();
  }
  void *buildExprListExp() { return SageBuilder::buildExprListExp(); }
  SgInitializedName *CreateVariable_v1(SgName sVar, void *sg_type,
                                       void *bindNode = nullptr);

//  ////////////////////////  /
//

//  arr_name[ivar0+offset0][ivar1+offset1][ivar2+offset2]...
#if USED_CODE_IN_COVERAGE_TEST
  void *CreateSimpleArrayRef(void *array_var_ref,
                             const std::vector<void *> &vec_var,
                             const std::vector<int> &offsets,
                             void *bindNode = nullptr);
#endif
#if USED_CODE_IN_COVERAGE_TEST
  void *BuildExpressFromPolyVector(const PolyVector &vecDim,
                                   const std::vector<void *> &vecVarDecl,
                                   int *bit_out);  //  not suggested
#endif
  int SetScope(void *stmt_, void *scope_);
#if USED_CODE_IN_COVERAGE_TEST
  void *ParseLoopNest(void *sg_stmt, int loop_level,
                      std::vector<void *> *vec_iter);
#endif

  int get_block_pragma_val(void *block, std::string key, int64_t *val);

  //  int IsCanonicalForLoop(
  //         SgNode * for_loop,
  //         SgInitializedName **  ivar = nullptr,
  //         SgExpression **  lb = nullptr,
  //         SgExpression **  ub = nullptr,
  //         SgExpression **  step = nullptr,
  //         SgStatement **  body = nullptr,
  //         bool *  hasIncrementalIterationSpace = nullptr,
  //         bool *  isInclusiveUpperBound = nullptr
  //        ) ;

  void *copy_loop_for_c_syntax(void *sg_for_loop, void *sg_scope = nullptr);

  int has_access_in_scope(void *for_loop,
                          const std::vector<std::string> &vec_names,
                          std::string *found);  //  not suggested

  bool ContainsUnsupportedLoop(void *sg_scope, bool recursive = false,
                               bool while_support = false);

  void *GetFuncCallInStmt(void *stmt);
  std::vector<void *> TraceDownToFuncCall(void *sg_pos);

  //  support cross function induct
  bool get_map_ref_to_exp(void *expr, void *boundary_scope,
                          std::map<void *, void *> *map_ref2exp);
  //  The major function for copy and replace var reference
  //  [Notice] The input will be better in the AST node, because of print
  //  debugging
  void *copy_and_replace_var_ref(
      void *sg_node, const std::map<void *, void *> &map_var2exp,
      std::map<void *, void *> *map_replaced = nullptr, int no_copy = false);

#if USED_CODE_IN_COVERAGE_TEST
  void *copy_node_and_replace_reference(void *sg_node, void *sg_init,
                                        void *new_exp);
#endif
  bool detect_user_for_loop_burst(void *ref, void *pntr, CMarsIr *mars_ir);
  void reset_cls_syms();

  void reset_var_syms();

  void reset_func_syms();

  void *GetFuncDeclByInstDirectiveStmt(void *sg_node);

  void *GetTemplateFuncDecl(void *sg_node);

  void reset_tfn_syms();

  void *GetTemplateMemFuncDecl(void *sg_node);

  void reset_tmfn_syms();

  void *GetTemplateClassDecl(void *sg_node);

  void reset_tc_syms();
  std::string get_template_string(void *sg_node);

  int is_all_dim_array(void *sg_type_in);

  void *GetExprFromStmt(void *sg_node);

  //  //////////////////////////////////////////////////////////////////////////////////
  //  / ZP: 2015_04-30: The formal function to get linear coefficients from an
  //  expression.
  //  Note:
  //  1. sg_exp is the input expression
  //  2. key of the map item is the name of the variable, if that term is a
  //  linear term
  //    otherwise, key of the item is "nullptr", and the value is "1" (nonzero)
  //    a special variable has a key of "1", and the coefficient is the constant
  //    term;
  //  3. the terms of the same variables will be merged, and the
  //  4. Return value: 1 if the expression is linear of loop iters and
  //  loop-invariant variables, and 0 for nonlinear
  int analyze_linear_expression(void *sg_exp,
                                std::map<std::string, int> *mapVar2Coeff);
#if USED_CODE_IN_COVERAGE_TEST
  int analyze_linear_expression_new(void *sg_exp,
                                    std::map<void *, int> *mapVar2Coeff);
#endif
  //  //////////////////////////////////////////////////////////////////////////////////
  //  /
  int get_expression_bound_integer(void *sg_exp, int *upper, int *lower);

  //  lower is the real lower bound
  //  upper is the real upper bound + 1
  int analyze_iteration_domain_order(void *sg_scope_, void *sg_stmt_,
                                     const std::vector<std::string> &iterators,
                                     const std::vector<int> &loop_indices,
                                     const std::vector<std::string> &condition,
                                     const std::vector<std::string> &lower,
                                     const std::vector<std::string> &upper,
                                     int HGT);
  int analyze_iteration_domain_order(void *sg_scope_, void *sg_stmt_,
                                     const std::vector<std::string> &iterators,
                                     const std::vector<int> &loop_indices,
                                     const std::vector<std::string> &condition,
                                     const std::vector<void *> &lower,
                                     const std::vector<void *> &upper, int HGT);
  int analyze_iteration_domain(void *sg_scope_, void *sg_stmt_,
                               const std::vector<std::string> &iterators,
                               const std::vector<std::string> &lower,
                               const std::vector<std::string> &upper);
  int analyze_iteration_domain(void *sg_scope_, void *sg_stmt_,
                               const std::vector<std::string> &iterators,
                               const std::vector<void *> &lower,
                               const std::vector<void *> &upper);
  int analyze_iteration_domain(void *sg_scope_, void *sg_stmt_,
                               const PolyMatrix &matrix);
  int get_io_matrix_of_function(
      void *sg_func_decl,
      const std::map<std::string, std::vector<poly_access_pattern>> &mapMatrix,
      const std::vector<std::string> &iterators,
      const std::vector<std::string> &params,
      const std::vector<std::string> &orderVar2Matrix);
  void get_ref_map_from_iter_map(void *scope,
                                 const std::map<void *, void *> &map_iter2expr,
                                 std::map<void *, void *> *map_to_insert);

  //  not suggested, use CMarsExpression instead
  int simplify_intermediate_variables(
      void *sg_exp_, void *sg_scope_, const PolyVector &poly_in,
      const std::vector<std::string> &parameters);
  int simplify_intermediate_variables(
      void *sg_exp_, void *sg_scope_, const std::string &poly_in,
      const std::vector<std::string> &vec_vars,
      const std::vector<std::string> &parameters);

  //  not suggested
  int linearize_arrays(void *sg_bb_or_func_);
  int linearize_arrays_detect(void *sg_bb_or_func_, const std::string &var,
                              std::vector<void *> *bb_collected,
                              std::vector<void *> *func_collected);
  int linearize_arrays_apply(void *sg_bb_or_func_, const std::string &var,
                             void *sg_type_,
                             const std::vector<void *> &bb_collected,
                             const std::vector<void *> &func_collected);

  //  not suggested
  int access_wrapper(
      void *sg_func,
      const std::map<std::string, std::vector<std::string>> &mapRefRenameList,
      const std::string &func_prefix,
      const std::map<std::string, std::vector<std::string>>
          &mapArray2Dim);  //  wrap all the
  //  access of
  //  the pointer
  //  arguments
  //  with
  //  function
  //  calls
  int one_access_wrapper(
      void *sg_func_, const std::string &sVar, const std::string &sPort,
      const std::string &func_prefix,
      const std::map<std::string, std::vector<std::string>> &mapArray2Dim,
      const std::map<std::string, bool> &mapFuncVisited);

//  Outline
#if USED_CODE_IN_COVERAGE_TEST
  void *outline(void *sg_bb_, const std::string &func_name,
                void **sg_new_decl = nullptr);
#endif

  void InsertPipelinePragma(void *scope,
                            const std::vector<std::string> &vec_bus_e1);

  std::string GetMallocVarName(void *exp);  //  , int& elem_size, int&
                                            // total_size);

  //  vendor library analysis
  bool IsStdComplexType(void *type) const;
  bool IsXilinxStreamType(void *type) const;
  bool IsXilinxAPIntType(void *type) const;
  bool IsXilinxAPFixedType(void *type) const;
  bool IsXilinxIntrinsic(void *fn_decl) const;
  bool isCanonicalForLoop(void *loop, void **ivar = nullptr,
                          void **lb = nullptr, void **ub = nullptr,
                          void **step = nullptr, void **body = nullptr,
                          bool *hasIncrementalIterationSpace = nullptr,
                          bool *isInclusiveUpperBound = nullptr);
  int GetLoopStep(void *loop, int64_t *nStep);
  bool IsAssignStatement(void *stmt, void **left_exp, void **right_exp);

  int GetOperatorOp(void *decl);

  int IsWriteOperatorOp(int opcode);
  int IsReadOnlyOperatorOp(int opcode);
  int IsImplicitConversionOp(int opcode);
  bool IsAPIntOp(void *expr, void **left_exp, void **right_exp, int *op);

 public:  //  for debug
  string _p(void *sgnode,
            int len = 0) const;  //  brief form of UnparseToString,
                                 // with type info
  std::string _up(void *sgnode, int len = 0) const {
    return UnparseToString(sgnode, len);
  }
  void *_pa(void *sgnode) const;  //  brief form of GetParent()
  void dump_all_func();
  string UnparseToString(void *sg_node, int len = 0) const;
  std::string GetASTTypeString(void *sg_node) const {
    return (static_cast<SgNode *>(sg_node)->class_name());
  }
  void reset_func_call_cache() { s_func_call_cache.clear(); }
  void reset_func_decl_cache() { s_func_decl_cache.clear(); }
  void reset_var_ref_cache() { s_var_ref_cache.clear(); }
  void reset_global_decl_cache() { s_global_decl_cache.clear(); }
  void reset_type_decl_cache() { s_type_decl_cache.clear(); }
  void reset_name_type_cache() { s_name_type_cache.clear(); }
  void reset_loop_info_cache() {
    s_loop_iterator_cache.clear();
    s_loop_label_cache.clear();
  }
  void reset_message_metadata_cache() { s_message_metadata_cache.clear(); }
  void reset_access_in_scope_cache() {
    s_write_in_scope_cache.clear();
    s_write_in_scope_fast_cache.clear();
    s_read_in_scope_cache.clear();
  }
  void reset_pragma_cache() {
    s_variable_to_pragma_cache.clear();
    s_pragma_to_variable_cache.clear();
  }
  std::map<SgNode *, HistoryMarker> *getHistories() { return &histories; }
  const std::map<SgNode *, HistoryMarker> &getHistories() const {
    return histories;
  }

  void *
  get_induct_expr_in_scope(void *var_ref, void *pos, void *boundary_scope,
                           std::vector<void *> *inner_terms /* = nullptr*/);
#if USED_CODE_IN_COVERAGE_TEST
  void SetToJson(const std::string &filename, const std::string &key,
                 const std::vector<std::string> &values,
                 const std::string &level_1, const std::string &level_2);

  std::vector<std::string> GetFromJson(const std::string &filename,
                                       const std::string &key,
                                       const std::string &level_1,
                                       const std::string &level_2);
#endif

  //  If we have a type that could have bitwidth, return it. Otherwise, return
  //  boost::none. The second bool param controls whether debug output is
  //  emitted upon failure.
  boost::optional<unsigned> get_bitwidth(SgType *, bool report = true);

  boost::optional<unsigned> get_xilinx_bitwidth(SgTemplateInstantiationDecl *,
                                                bool report = true);

  // TODO(youxiang): This should return an unsigned.
  int get_bitwidth_from_type_str(const std::string &str_type,
                                 bool report = true);
  int get_sizeof_arbitrary_precision_integer(int bitwidth) const;
  bool isMultiAssignHaveSideEffect(void *node);
  bool SplitMultiAssign(void *scope);
  void *convertAssign(void *node);
  bool IsSafeIntConversion(void *exp, void *pos, void *scope, void *type_a,
                           void *type_b);

  std::string GetSafeNewVarName(const std::string &prefix, void *scope);

  void removeSpaces(string *stringIn);

 protected:
#if USED_CODE_IN_COVERAGE_TEST
  std::vector<std::string>
  outline_simplify_get_deref_vars(SgFunctionDeclaration *sg_func_decl);
#endif
  //  void outline_simplify_one_var(const std::string &sFuncName, const
  //  std::string &sVar);
  void outline_simplify_one_var(SgFunctionDeclaration *sg_func_decl, int i);
  int outline_check_false_deref(SgFunctionDeclaration *sg_func_decl,
                                SgInitializedName *arg);

  int get_io_matrix_of_function_recur(
      void *sg_func_decl, const std::string &sPort, int nDim,
      std::map<std::string, std::vector<poly_access_pattern>> *mapMatrix,
      const std::vector<std::string> &iterators);

#if USED_CODE_IN_COVERAGE_TEST
  int function_argument_name_replace(
      void *sg_call, void *sg_decl, const std::vector<std::string> &parameters);
#endif
  void function_argument_volatile_type(void *func_decl, int arg_idx);

  void *pointer_to_array_recur(void *sg_type);

  void *match_var_in_expression(const std::string &sVar, void *sg_exp);

  void *trace_base_type(void *sg_type, bool skip_anonymous_struct = true) const;

  void *is_pointer_ref_in_memcpy(void *var_ref);

  void add_exp_to_vector(std::vector<void *> *vec, int idx, void *exp,
                         void *pos = nullptr, bool simplify_idx = true);

  int ParseForLoop(SgForStatement *target_loop, SgInitializedName **ivar,
                   SgExpression **lb, SgExpression **ub, SgExpression **step,
                   SgBinaryOp **cond_op, SgStatement **orig_body,
                   int *ub_limit);

  //  int Expr2Vector(SgExpression * sgExp, PolyVector * poly_vec, int
  //  force_linear=0);
  int Expr2Vector(void *sgExp, PolyVector *poly_vec, int force_linear = 0);
  int Expr2Vector(SgExpression *sgExp, std::string *poly_vec,
                  std::vector<std::string> *vec_vars, int force_linear = 0);

  //  int Expr2Vector(void * sgExp, std::vector<void *, int> * m_coeff) ;

  int get_const_value(void *ub, int *a);

  int is_write_analysis(SgNode *sgnode);

  std::map<std::string, std::vector<std::string>>
      m_mapRefRenameList;  //  std::map<std::string port,
                           //  std::vector<std::string ref_name> >
  std::map<std::string, int> m_mapRefRenameIdx;  //  std::map<std::string port,
                                                 // int
  //  current_ref_idx_of_the_port>;

  std::map<void *, std::vector<void *>> s_func_call_cache;
  std::map<void *, std::vector<void *>> s_var_ref_cache;
  std::map<void *, std::set<void *>> s_variable_to_pragma_cache;
  std::map<void *, std::set<void *>> s_pragma_to_variable_cache;
  std::map<void *, void *> s_func_decl_cache;
  std::map<void *, void *> s_global_decl_cache;
  std::map<void *, void *> s_type_decl_cache;
  std::map<void *, void *> s_loop_iterator_cache;
  std::map<void *, bool> s_loop_label_cache;
  std::map<std::string, void *> s_name_type_cache;
  std::map<std::pair<void *, void *>, bool> s_write_in_scope_cache;
  std::map<std::pair<void *, void *>, bool> s_write_in_scope_fast_cache;
  std::map<std::pair<void *, void *>, bool> s_read_in_scope_cache;
  std::map<std::pair<void *, void *>, bool> s_read_in_scope_fast_cache;
  std::set<void *> s_def_use_cache;
  std::map<std::pair<void *, int>, std::vector<void *>> s_g_funcs_cache;
  std::map<std::string, std::set<void *>> s_g_func_name_cache;
  std::map<std::pair<void *, void *>, bool> s_location_cache;
  std::map<std::string, std::string> s_message_metadata_cache;
  std::map<SgNode *, HistoryMarker> histories;
  std::set<std::string> m_inputFiles;
  std::set<std::string> m_inputIncDirs;
  std::map<std::string, std::string> m_inputMacros;
  std::map<void *, void *> m_tfn_syms;
  std::map<void *, void *> m_tmfn_syms;
  std::map<void *, void *> m_tc_syms;
  std::map<void *, void *> m_func_syms;
  std::map<void *, void *> m_cls_syms;
  std::map<void *, void *> m_var_syms;
  bool m_tfn_syms_valid;
  bool m_tmfn_syms_valid;
  bool m_tc_syms_valid;
  bool m_cls_syms_valid;
  bool m_func_syms_valid;
  bool m_var_syms_valid;

  SgProject *m_sg_project;
  std::map<std::string, void *> m_builtin_types;
  std::set<SgType *> m_new_types;
  std::vector<SgTypedefDeclaration *> m_builtin_type_decls;

  DefUseAnalysis *m_defuse;
  LivenessAnalysis *m_liveness;
  void *s_liveness_previous_func;  //  used as cache

  CallGraphBuilder *m_callgraph;

  //  for range anlaysis
  void init_range_analysis();

 private:
  void TraceUpFuncCallsGeneric(void *sg_scope, void *sg_func_decl_arg,
                               const t_func_trace_up &pFunc, void *pFuncArg,
                               set<void *> *visited);
  int get_type_dimension_by_ref(void *sg_varref_or_pntr, void **sg_array,
                                void **basic_type,
                                std::vector<size_t> *vecSizes,
                                std::set<void *> *visited_funcs);

  int IsCompatibleType(void *sg_type1, void *sg_type2, void *pos, bool top,
                       bool strict);
  void get_all_candidates_from_loop_init(void *sg_for_loop_,
                                         std::vector<void *> *iv_cand,
                                         std::vector<void *> *iv_cand_init,
                                         std::vector<void *> *iv_cand_start,
                                         void **init_pos, void **iterator_ptr);

  bool get_all_candidate_from_loop_condition(void *sg_for_loop,
                                             std::vector<void *> *iv_cond,
                                             std::vector<void *> *iv_cond_end,
                                             std::vector<int> *iv_cond_dir);

  void
  get_all_candidate_from_loop_increment(void *sg_for_loop_,
                                        std::vector<void *> *iv_incr,
                                        std::vector<int64_t> *iv_incr_step);

  void convert_loop_iterator_with_char_type_into_int(void *sg_for_loop_,
                                                     void **iterator_variable);

  bool convert_loop_iterator_with_pointer_type(void *sg_for_loop_,
                                               void *iterator_ptr);

  void HandleCondition(void *cond_exp, bool true_value);
  int IsRecursiveFunction(void *func_decl, void **rec_func,
                          std::set<void *> *visited);

  int IsRecursiveType(void *sg_type, void **rec_type,
                      std::set<void *> *visited);

  int GetPntrReferenceOfArray(void *array, void *sg_scope,
                              std::vector<void *> *vec_access,
                              std::set<void *> *visited,
                              int allow_artifical_ref = 0);

  void replaceExpressionWithStatement(SgExpression *from,
                                      SageInterface::StatementGenerator *to);

  void replaceSubexpressionWithStatement(SgExpression *from,
                                         SageInterface::StatementGenerator *to);

  void
  replaceAssignmentStmtWithStatement(SgExprStatement *from,
                                     SageInterface::StatementGenerator *to);

  void pushTestIntoBody(SgScopeStatement *loopStmt);

  void pushExpressionIntoBody(SgExpression *from);

  SgExpression *getRootOfExpression(SgExpression *n);

  void replaceExpressionWithExpression(SgExpression *from, SgExpression *to);

  SgAssignOp *convertInitializerIntoAssignment(SgAssignInitializer *init);
};
typedef CSageCodeGen CMarsAST_IF;

int is_cmost_pragma(std::string sFilter, bool allow_hls = 0);
int tldm_pragma_parse_whole(
    std::string sPragma, std::string *p_sFilter, std::string *p_sCmd,
    std::map<std::string,
             std::pair<std::vector<std::string>, std::vector<std::string>>>
        *p_mapParam,
    bool allow_hls = false);
void all_pragma_parse_whole(
    std::string sPragma, std::string *p_sFilter, std::string *p_sCmd,
    std::map<std::string,
             std::pair<std::vector<std::string>, std::vector<std::string>>>
        *p_mapParam,
    vector<string> *p_mapKey);

class CNameConflictSolver {
 public:
  explicit CNameConflictSolver(CSageCodeGen *ast = nullptr,
                               void *scope = nullptr);
  std::string get_and_assign(const std::string &prefix);

 protected:
  std::set<std::string> m_record;
};
