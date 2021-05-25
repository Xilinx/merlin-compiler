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
#include "input_checker.h"
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

void *GetExpTypeFromString(string s_expr, void *pos, CSageCodeGen *ast) {
  vector<string> vs_expr = str_split(s_expr, "->");

  int first = 1;

  void *curr_struct_type = nullptr;
  int cnt = 0;
  void *sg_type = nullptr;
  for (auto s_var : vs_expr) {
    void *sg_init;
    if (first != 0) {
      sg_init = ast->find_variable_by_name(s_var, pos);
      first = 0;
    } else {
      if (curr_struct_type == nullptr) {
        return nullptr;
      }
      sg_init = ast->GetClassMemberByName(s_var, curr_struct_type);
    }
    assert(sg_init);

    sg_type = ast->GetTypebyVar(sg_init);
    void *sg_base = ast->GetBaseType(sg_type, true);

    if (ast->IsStructureType(sg_base) != 0) {
      curr_struct_type = sg_base;
    } else if (static_cast<size_t>(cnt) != vs_expr.size() - 1) {
      return nullptr;
    } else {
      return sg_type;
    }

    cnt++;
  }

  return sg_type;
}

void *CreateStructElementExp(string s_depth, void *kernel, CSageCodeGen *ast) {
  string kernel_name = ast->GetFuncName(kernel);
  string wrapper_name = "__merlinwrapper_" + kernel_name;
  void *func_decl = ast->GetFuncDeclByName(wrapper_name);
  vector<string> v_str = str_split(s_depth, "->");
  void *curr_base = nullptr;
  void *curr_ref = nullptr;
  for (size_t j = 0; j < v_str.size() - 1; j++) {
    void *var_init;
    if (j == 0) {
      for (int i = 0; i < ast->GetFuncParamNum(func_decl); i++) {
        void *arg_init = ast->GetFuncParam(func_decl, i);
        if (ast->UnparseToString(arg_init) == v_str[0]) {
          var_init = arg_init;
          void *sg_type = ast->GetTypebyVar(var_init);
          curr_base = ast->GetBaseType(sg_type, true);
          assert(ast->IsStructureType(curr_base));
          curr_ref = ast->CreateVariableRef(var_init);
        }
      }
    }

    void *member_init = ast->GetClassMemberByName(v_str[j + 1], curr_base);
    void *member_type = ast->GetTypebyVar(member_init);
    curr_base = ast->GetBaseType(member_type, true);
    void *second_ref = ast->CreateVariableRef(member_init);
    curr_ref = ast->CreateExp(
        ast->IsPointerType(ast->GetTypeByExp(curr_ref)) != 0 ? V_SgArrowExp
                                                             : V_SgDotExp,
        curr_ref, second_ref);
  }
  return curr_ref;
}

void *CreateExpFromString(string s_expr, vector<void *> indexes,
                          vector<int> dims, void *pos, CSageCodeGen *ast,
                          void **cond, bool include_cond = false) {
  vector<string> vs_expr = str_split(s_expr, "->");
  vector<string> tmp = vs_expr;
  vs_expr.clear();
  for (auto expr : tmp) {
    vector<string> sub_vs_expr = str_split(expr, ".");
    vs_expr.insert(vs_expr.end(), sub_vs_expr.begin(), sub_vs_expr.end());
  }
  *cond = nullptr;
  int i = 0;
  int first = 1;
  void *curr_ref = nullptr;
  void *curr_struct_type = nullptr;
  for (auto s_var : vs_expr) {
    void *sg_init;
    if (first != 0) {
      sg_init = ast->find_variable_by_name(s_var, pos);
      first = 0;
    } else {
      if (curr_struct_type == nullptr) {
        return nullptr;
      }
      sg_init = ast->GetClassMemberByName(s_var, curr_struct_type);
      assert(sg_init);
    }

    void *sg_type = ast->GetTypebyVar(sg_init);
    void *sg_base = ast->GetBaseType(sg_type, true);

    if (ast->IsStructureType(sg_base) != 0) {
      curr_struct_type = sg_base;
    }

    int curr_dim = ast->get_dim_num_from_var(sg_init);

    vector<void *> tt_index;
    for (int t = 0; t < curr_dim; t++) {
      if (static_cast<size_t>(i) < indexes.size()) {
        tt_index.push_back(ast->CopyExp(indexes[i++]));
      }
    }
    if (include_cond && curr_dim > 0) {
      void *curr_pointer = ast->CreateVariableRef(sg_init);
      if (curr_ref != nullptr) {
        curr_pointer = ast->CreateExp(
            ast->IsPointerType(ast->GetTypeByExp(curr_ref)) != 0 ? V_SgArrowExp
                                                                 : V_SgDotExp,
            curr_ref, curr_pointer);
      }
      void *test_null_pointer =
          ast->CreateExp(V_SgNotEqualOp, curr_pointer, ast->CreateConst(0));
      if (*cond == nullptr) {
        *cond = test_null_pointer;
      } else {
        *cond = ast->CreateExp(V_SgAndOp, *cond, test_null_pointer);
      }
    }
    void *new_ref =
        ast->CreateArrayRef(ast->CreateVariableRef(sg_init), tt_index);

    if (curr_ref == nullptr) {
      curr_ref = new_ref;
    } else {
      curr_ref = ast->CreateExp(
          ast->IsPointerType(ast->GetTypeByExp(curr_ref)) != 0 ? V_SgArrowExp
                                                               : V_SgDotExp,
          curr_ref, new_ref);
    }
  }

  //  FIXME: ZP: 20160913
  //  if (i != indexes.size()) return nullptr;

  return curr_ref;
}

void *CreateOneDimOffset(vector<void *> v_index, vector<int> v_dim,
                         CSageCodeGen *ast) {
  int nsize = 1;
  int dim = v_dim.size();
  vector<int> v_dim_fsize;
  v_dim_fsize.resize(dim);
  for (int i = dim - 1; i >= 0; i--) {
    v_dim_fsize[i] = nsize;
    nsize *= v_dim[i];
  }

  int i = 0;
  void *addr = nullptr;
  for (auto idx : v_index) {
    void *term = ast->CreateExp(V_SgMultiplyOp, ast->CopyExp(idx),
                                ast->CreateConst(v_dim_fsize[i]));
    if (addr == nullptr) {
      addr = term;
    } else {
      addr = ast->CreateExp(V_SgAddOp, addr, term);
    }
    i++;
  }

  if (addr == nullptr) {
    addr = ast->CreateConst(0);
  }

  return addr;
}

void generate_copy_in_or_out(
    vector<pair<void *, void *>> v_port_pragma, int is_copy_in,
    string task_name, void *scope, void *kernel, CSageCodeGen *ast,
    CMarsIrV2 *mars_ir,
    const map<string, string> &port_name_to_wrapper_variable,
    string pcie_transfer_opt) {
  //   void * read_write_buffer;
  //   void * new_bb_body;
  //   if(is_copy_in == 1) {
  //        read_write_buffer= ast->GetFuncDeclByName(MERLIN_WRITE_BUFFER_PREFIX
  //        + task_name); string name = MERLIN_WRITE_BUFFER_PREFIX + task_name;
  //   } else {
  //        read_write_buffer= ast->GetFuncDeclByName(MERLIN_READ_BUFFER_PREFIX
  //        + task_name); string name = MERLIN_READ_BUFFER_PREFIX + task_name;
  //   }
  //   void * read_write_buffer_body = ast->GetFuncBody(read_write_buffer);

  for (auto it : v_port_pragma) {
    void *sg_init = it.first;
    void *sg_pragma = it.second;
    string s_io = mars_ir->get_pragma_attribute(sg_pragma, "io");
    boost::algorithm::to_upper(s_io);
    if (s_io == "NO") {
      continue;
    }
    if (s_io == "WO" && (is_copy_in != 0)) {
      continue;
    }
    if (s_io == "RO" && (is_copy_in == 0)) {
      continue;
    }

    string s_port = mars_ir->get_pragma_attribute(sg_pragma, "port");
    assert(s_port == ast->_up(sg_init));
    string s_max_depth = mars_ir->get_pragma_attribute(sg_pragma, "max_depth");
    string s_depth = mars_ir->get_pragma_attribute(sg_pragma, "depth");
    string s_var = mars_ir->get_pragma_attribute(sg_pragma, CMOST_variable);
    string s_type = mars_ir->get_pragma_attribute(sg_pragma, "data_type");
    int b_cont_flag =
        my_atoi(mars_ir->get_pragma_attribute(sg_pragma, "continue"));
    vector<string> vs_depth = str_split(s_depth, ',');
    //  Han Jun 8th 2017, if depth variable related, not continuous
    InputChecker instance(ast, kernel);
    if (instance.IsInterfaceRelated(vs_depth, kernel) != 0) {
      b_cont_flag = 0;
    }
    if (vs_depth.empty()) {
      continue;
    }

    if (s_max_depth.empty()) {
      continue;
    }

    void *bb_body = ast->CreateBasicBlock();

    //  a.0 preprocess vs_depth: for the cases that struct_a[100].scalar_member
    //  vs_depth: (100) -> (100,1) //  and set the is_scalar_last_dim flag
    //
    //  Overall scheme for scalar_member
    //  1. The output code is like:
    //   for (i=0..depth) __t_var[i] = &var[i].var;
    //   memcpy(__m_var, __t_var, depth);
    //  2. processing:
    //    a.0.a. detect the type of the var from string
    //    a.0.b. create a __t_ buffer with the depth (1-D buffer with total dim)
    //    a.0.c. assign each element to __t_ buffer with for-loops
    //    a.0.d. create a final memcpy from __t_ buffer to __m_ opencl buffer

    //  a.0.a fixing for efficiency on multiple scalars
    int is_scalar_last_dim = 0;
    {
      void *var_type = GetExpTypeFromString(s_var, scope, ast);
      //  if (ast->IsScalarType(var_type))
      if ((ast->IsPointerType(var_type) == 0) &&
          (ast->IsArrayType(var_type) == 0)) {
        vs_depth.push_back("1");
        is_scalar_last_dim = 1;

        //  FIXME:
        b_cont_flag = 0;
      }
    }

    //  a.0.b fixing for efficiency on multiple scalars
    void *scalar_buf_decl = nullptr;

    int total_size_int;
    if (is_scalar_last_dim != 0) {
      int size = get_total_size_from_string(s_max_depth);
      void *sg_type = ast->GetTypeByString(s_type);
      assert(sg_type);
      vector<size_t> v_size;
      v_size.push_back(size);
      total_size_int = size;
      //  sg_type = ast->CreateArrayType(sg_type, v_size);
      sg_type = ast->CreatePointerType(sg_type);
      string s_buf_name;
      if (is_copy_in != 0) {
        s_buf_name = "__ti_" + s_port;
      } else {
        s_buf_name = "__to_" + s_port;
      }
      void *new_decl =
          ast->CreateVariableDecl(sg_type, s_buf_name, nullptr, scope);
      ast->SetStatic(new_decl);
      scalar_buf_decl = new_decl;
    }

    //  a. generate the for-loops
    vector<void *> loop_ub;
    vector<void *> v_index;
    void *outmost_loop = nullptr;
    void *last_dim_size = nullptr;
    {
      for (auto s_depth : vs_depth) {
        int n_depth = my_atoi(s_depth);
        void *sg_ub;
        if (n_depth != 0) {
          sg_ub = ast->CreateConst(n_depth);
        } else {
          if (port_name_to_wrapper_variable.count(s_depth) > 0) {
            s_depth = port_name_to_wrapper_variable.find(s_depth)->second;
          }

          vector<void *> empty;
          vector<int> empty_i;
          void *cond = nullptr;
          vector<string> depth_split = str_split(s_depth, "->");
          if (depth_split.size() > 1) {
            sg_ub = CreateStructElementExp(s_depth, kernel, ast);
          } else {
            sg_ub =
                CreateExpFromString(s_depth, empty, empty_i, scope, ast, &cond);
          }
        }
        loop_ub.push_back(sg_ub);
      }

      void *curr_loop = bb_body;
      if (b_cont_flag == 0) {
        for (size_t j = loop_ub.size() - 1; j > 0; j--) {
          void *ub = loop_ub[j - 1];
          string s_it = "_i" + my_itoa(j - 1);

          void *sg_it = ast->find_variable_by_name(s_it, scope);
          void *decl;
          if (sg_it == nullptr) {
            decl = ast->CreateVariableDecl(ast->GetTypeByString("int"), s_it,
                                           nullptr, scope);
            ast->PrependChild(scope, decl, true);
            sg_it = ast->GetVariableInitializedName(decl);
          }
          //                void * sg_it_w = ast->find_variable_by_name(s_it,
          //                read_write_buffer_body); if (sg_it_w == nullptr) {
          //                    decl = ast->CreateVariableDecl(
          //                            ast->GetTypeByString("int"),
          //                            s_it,
          //                            nullptr,
          //                            read_write_buffer_body);
          //                    ast->PrependChild(read_write_buffer_body, decl,
          //                    true);
          //                    //  Han, write to new OpenCL API
          //                }

          void *sg_lb = ast->CreateConst(0);
          void *sg_ub = ast->CopyExp(ub);
          curr_loop =
              ast->CreateStmt(V_SgForStatement, sg_it, sg_lb, sg_ub, curr_loop);
          v_index.push_back(ast->CreateVariableRef(sg_it));
        }
      } else {
        for (size_t j = loop_ub.size() - 1; j > 0; j--) {
          v_index.push_back(ast->CreateConst(0));
        }
      }

      std::reverse(v_index.begin(), v_index.end());
      ast->AppendChild(scope, curr_loop);
      outmost_loop = curr_loop;

      //  if (my_atoi(vs_depth[dim-1]) == 1) last_dim_size = nullptr;
      //  else last_dim_size = loop_ub[dim-1];
    }

    //  a.0.b fixing for efficiency on multiple scalars
    if (scalar_buf_decl != nullptr) {
      ast->InsertStmt(scalar_buf_decl, outmost_loop);
      vector<void *> args;
      void *sg_type = ast->GetTypeByString(s_type);
      void *new_ref = ast->CreateVariableRef(scalar_buf_decl);
      void *ref_cast =
          ast->CreateCastExp(ast->CreateExp(V_SgAddressOfOp, new_ref),
                             ast->GetTypeByString("void **"));
      void *alignment = ast->CreateConst(64);
      void *sg_size =
          ast->CreateExp(V_SgMultiplyOp, ast->CreateExp(V_SgSizeOfOp, sg_type),
                         ast->CreateConst(total_size_int));
      args.push_back(ref_cast);
      args.push_back(alignment);
      args.push_back(sg_size);
      void *call = ast->CreateFuncCall(
          "posix_memalign", ast->GetTypeByString("void"), args, scope);
      void *stmt = ast->CreateStmt(V_SgExprStatement, call);
      ast->InsertStmt(stmt, outmost_loop);
    }

    void *total_size = ast->CopyExp(loop_ub[0]);
    for (size_t t = 1; t < loop_ub.size(); t++) {
      total_size =
          ast->CreateExp(V_SgMultiplyOp, total_size, ast->CopyExp(loop_ub[t]));
    }

    //  b. generate the data-copy
    //  if (last_dim_size)
    {  //  memcpy
      //  if (!last_dim_size) last_dim_size = ast->CreateConst(1);

      int dim = vs_depth.size();
      last_dim_size = loop_ub[dim - 1];

      if (b_cont_flag != 0) {
        last_dim_size = total_size;
      }

      string s_var = mars_ir->get_pragma_attribute(sg_pragma, CMOST_variable);
      string s_port = mars_ir->get_pragma_attribute(sg_pragma, "port");
      assert(s_port == ast->_up(sg_init));
      string s_dim = mars_ir->get_pragma_attribute(sg_pragma, "max_depth");
      vector<string> vs_dim = str_split(s_dim, ',');
      vector<int> v_dim;
      for (auto s_one_dim : vs_dim) {
        v_dim.push_back(my_atoi(s_one_dim));
      }

      string s_type = mars_ir->get_pragma_attribute(sg_pragma, "data_type");
      void *data_type = ast->GetTypeByString(s_type);
      assert(data_type);
      void *cond = nullptr;
      void *ref_var =
          CreateExpFromString(s_var, v_index, v_dim, bb_body, ast, &cond, true);
      assert(ref_var);  //  FIXME

      void *sg_offset = CreateOneDimOffset(v_index, v_dim, ast);
      void *sg_port = ast->find_variable_by_name("__m_" + s_port, scope);
      DEBUG(printf("memcpy buffer scope = %s\n, sport = %s, sg_port = %s\n",
                   ast->UnparseToString(scope).c_str(), s_port.c_str(),
                   ast->UnparseToString(sg_port).c_str()));
      assert(sg_port);  //  FIXME
      void *ref_port = ast->CreateExp(
          V_SgAddOp, ast->CreateVariableRef(sg_port), ast->CopyExp(sg_offset));

      //  /////////  /
      //  zp: 20160921: fixing the bug due to global variable

      void *ref_var_copy = ref_var;
      {
        //  string scope_type = mars_ir->get_pragma_attribute(sg_pragma,
        //  "scope_type"); if (scope_type == "global")

        if (is_scalar_last_dim != 0) {
          //  ref_var = ast->CreateExp(V_SgAddressOfOp, ref_var);
          ref_var = ast->CreateVariableRef(scalar_buf_decl);
          ref_port = ast->CreateVariableRef(sg_port);

          last_dim_size = total_size;
        }
      }
      //  /////////  /

      void *ref_from = (is_copy_in) != 0 ? ref_var : ref_port;
      void *ref_to = (is_copy_in) != 0 ? ref_port : ref_var;

      void *sg_size =
          ast->CreateExp(V_SgMultiplyOp, ast->CopyExp(last_dim_size),
                         ast->CreateExp(V_SgSizeOfOp, data_type));
      //  Han add debug information for opencl transfer
      string ref_var_s = s_port;
      string direction_s = (is_copy_in) != 0 ? "copy in" : "copy out";
      string sg_size_s = ast->UnparseToString(sg_size);
      //  string size_s = "ACCEL debug char size_s[256] = \"" + sg_size_s +
      //  "\";";
      string fflush = "ACCEL debug fflush(stdout);";
      string start_print = "ACCEL debug printf(\"[Merlin Info] Start %s data "
                           "for %s, data size = %s...\\n\", \"" +
                           direction_s + "\", \"" + ref_var_s + "\", \"" +
                           sg_size_s + "\");";

      vector<void *> args;
      ref_to = ast->CreateCastExp(ref_to, ast->GetTypeByString("void *"));
      ref_from =
          ast->CreateCastExp(ref_from, ast->GetTypeByString("const void *"));
      args.push_back(ref_to);
      args.push_back(ref_from);
      args.push_back(sg_size);

      void *call = ast->CreateFuncCall("memcpy", ast->GetTypeByString("void"),
                                       args, bb_body);

      void *stmt = ast->CreateStmt(V_SgExprStatement, call);

      //    a.0.c. assign each element to __t_ buffer with for-loops
      //    a.0.d. create a final memcpy from __t_ buffer to __m_ opencl buffer
      if (is_scalar_last_dim != 0) {
        void *scalar_ref_var = ast->CopyExp(ref_var_copy);
        vector<void *> v_idx_tmp;
        v_idx_tmp.push_back(ast->CopyExp(sg_offset));
        void *scalar_ref_buf = ast->CreateArrayRef(
            ast->CreateVariableRef(scalar_buf_decl), v_idx_tmp);

        void *scalar_ref_from =
            (is_copy_in) != 0 ? scalar_ref_var : scalar_ref_buf;
        void *scalar_ref_to =
            (is_copy_in) != 0 ? scalar_ref_buf : scalar_ref_var;

        void *assign_stmt = ast->CreateStmt(
            V_SgExprStatement,
            ast->CreateExp(V_SgAssignOp, scalar_ref_to, scalar_ref_from));
        if (cond != nullptr) {
          void *true_body = ast->CreateBasicBlock();
          ast->AppendChild(true_body, assign_stmt);
          void *false_body = ast->CreateBasicBlock();
          void *error_msg =
              ast->CreateStringExp("Error! Detected null pointer \'" + s_var +
                                   "\' for external memory.\\n");
          void *print_call =
              ast->CreateFuncCall("printf", ast->GetTypeByString("void"),
                                  vector<void *>(1, error_msg), bb_body);
          void *print_stmt = ast->CreateStmt(V_SgExprStatement, print_call);
          ast->AppendChild(false_body, print_stmt);
          void *exit_call = ast->CreateFuncCall(
              "exit", ast->GetTypeByString("void"),
              vector<void *>(1, ast->CreateConst(1)), bb_body);
          void *exit_stmt = ast->CreateStmt(V_SgExprStatement, exit_call);
          ast->AppendChild(false_body, exit_stmt);
          assign_stmt = ast->CreateIfStmt(cond, true_body, false_body);
        }

        ast->AppendChild(bb_body, assign_stmt);

        if (is_copy_in != 0) {
          ast->InsertAfterStmt(stmt, outmost_loop);
        } else {
          ast->InsertStmt(stmt, outmost_loop);
        }

        //  Hanhu 20170901, to write opencl new API, handle global variable
        //                new_bb_body = ast->CreateBasicBlock();
        //                if (scalar_buf_decl)
        //                {
        //                    ast->AppendChild(new_bb_body,
        //                    ast->CopyStmt(scalar_buf_decl));
        //                }
        //                void * new_outmost_loop = ast->CopyStmt(outmost_loop);
        //                void * new_stmt = ast->CopyStmt(stmt);
        //                ast->AppendChild(new_bb_body, new_outmost_loop);
        //                if (is_copy_in)
        //                    ast->InsertAfterStmt(new_stmt, new_outmost_loop);
        //                else
        //                    ast->InsertStmt(new_stmt, new_outmost_loop);
      } else {
        if (cond != nullptr) {
          void *true_body = ast->CreateBasicBlock();
          ast->AppendChild(true_body, stmt);
          void *false_body = ast->CreateBasicBlock();
          void *error_msg =
              ast->CreateStringExp("Error! Detected null pointer \'" + s_var +
                                   "\' for external memory.\\n");
          void *print_call =
              ast->CreateFuncCall("printf", ast->GetTypeByString("void"),
                                  vector<void *>(1, error_msg), bb_body);
          void *print_stmt = ast->CreateStmt(V_SgExprStatement, print_call);
          ast->AppendChild(false_body, print_stmt);
          void *exit_call = ast->CreateFuncCall(
              "exit", ast->GetTypeByString("void"),
              vector<void *>(1, ast->CreateConst(1)), bb_body);
          void *exit_stmt = ast->CreateStmt(V_SgExprStatement, exit_call);
          ast->AppendChild(false_body, exit_stmt);
          stmt = ast->CreateIfStmt(cond, true_body, false_body);
        }

        ast->AppendChild(bb_body, stmt);
        //                new_bb_body =  ast->CopyStmt(bb_body);
      }
      //  Han add debug information for opencl transfer
      string finish_print =
          "ACCEL debug printf(\"[Merlin Info] Finish %s data for %s.\\n\", \"" +
          direction_s + "\", \"" + ref_var_s + "\");";
      //  ast->InsertStmt(ast->CreatePragma(size_s, scope), stmt);
      if (pcie_transfer_opt != "on") {
        ast->InsertStmt(ast->CreatePragma(start_print, scope), stmt);
        ast->InsertStmt(ast->CreatePragma(fflush, scope), stmt);
        ast->InsertAfterStmt(ast->CreatePragma(fflush, scope), stmt);
        ast->InsertAfterStmt(ast->CreatePragma(finish_print, scope), stmt);
      }
      //  ast->PrependChild(bb_body, ast->CreatePragma(fflush, scope));
      //  ast->PrependChild(bb_body, ast->CreatePragma(start_print, scope));
      //  ast->PrependChild(bb_body, ast->CreatePragma(size_s, scope));
      //  ast->AppendChild (bb_body, ast->CreatePragma(finish_print, scope));
      //  ast->AppendChild (bb_body, ast->CreatePragma(fflush, scope));
    }
    //        ast->AppendChild(read_write_buffer_body, new_bb_body);
  }
}

//  2.5 move the struct def into wrapper if any
void move_struct_into_wrapper(void *wrapper, void *kernel, CSageCodeGen *ast,
                              CMarsIrV2 *mars_ir) {
#if 1
#else
  void *global_kernel = ast->GetGlobal(kernel);
  {
    vector<void *> v_decl;
    ast->GetNodesByType(global_kernel, "preorder", "SgTypedefDeclaration",
                        &v_decl);

    void *insert_pos = ast->GetFirstInsertPosInGlobal(wrapper);
    for (auto decl : v_decl) {
      if (ast->GetClassDefinition(decl) && ast->IsFromInputFile(decl)) {
        void *new_decl = ast->CopyStmt(decl);
        ast->InsertStmt(new_decl, insert_pos);
        ast->RemoveStmt(decl);
      }
    }
  }
  {
    vector<void *> v_decl;
    ast->GetNodesByType(global_kernel, "preorder", "SgClassDeclaration",
                        &v_decl);

    void *insert_pos = ast->GetFirstInsertPosInGlobal(wrapper);
    for (auto decl : v_decl) {
      if (ast->GetClassDefinition(decl) && ast->IsFromInputFile(decl)) {
        void *new_decl = ast->CopyStmt(decl);
        ast->InsertStmt(new_decl, insert_pos);
        ast->RemoveStmt(decl);
      }
    }
  }
#endif
}

void remove_comment(CSageCodeGen *ast, void *scope) {
  string comment1 = "__MERLIN_COMMENT_BEGIN__";
  string comment2 = "__MERLIN_COMMENT_END__";
  void *variable1 = ast->find_variable_by_name(comment1, scope);
  void *stmt1 = ast->TraceUpToStatement(variable1);
  ast->RemoveChild(stmt1);
  void *variable2 = ast->find_variable_by_name(comment2, scope);
  void *stmt2 = ast->TraceUpToStatement(variable2);
  ast->RemoveChild(stmt2);
}
void data_transfer_gen_one_kernel(void *wrapper, void *kernel,
                                  CSageCodeGen *ast, CMarsIrV2 *mars_ir,
                                  string task_name, string pcie_transfer_opt) {
  vector<void *> v_to_be_appened_finally;
  vector<void *> v_to_be_appened_finally_r;
  void *body = ast->GetFuncBody(wrapper);
  assert(body);
  vector<void *> v_calls;
  //  get originalkernel pragma
  void *copy_kernel_pragma = nullptr;
  ast->GetNodesByType(wrapper, "preorder", "SgFunctionCallExp", &v_calls);
  for (auto one_call : v_calls) {
    void *one_stmt = ast->TraceUpToStatement(one_call);
    void *prev_stmt = ast->GetPreviousStmt(one_stmt);
    if (ast->IsPragmaDecl(prev_stmt) != 0) {
      copy_kernel_pragma = ast->CopyStmt(prev_stmt);
    }
  }
  void *new_body = ast->CreateBasicBlock();
  ast->AddHeader("#include <string.h> \n", ast->GetGlobal(wrapper));
  ast->AddHeader("#include <stdio.h> \n", ast->GetGlobal(wrapper));
  ast->AddHeader("#include <stdlib.h> \n", ast->GetGlobal(wrapper));

  void *pragma_bb = ast->CreateBasicBlock();
  ast->AppendChild(new_body, pragma_bb);

  void *write_buffer_bb = ast->CreateBasicBlock();
  void *write_buffer =
      ast->GetFuncDeclByName(MERLIN_WRITE_BUFFER_PREFIX + task_name);
  if (write_buffer != nullptr) {
    void *write_buffer_body = ast->GetFuncBody(write_buffer);
    if (ast->GetChildStmtNum(write_buffer_body) == 0) {
      ast->DeleteNode(write_buffer_bb);
      write_buffer_bb = write_buffer_body;
    } else {
      ast->AppendChild(write_buffer_body, write_buffer_bb);
    }
  }

  void *read_buffer_bb = ast->CreateBasicBlock();
  void *read_buffer =
      ast->GetFuncDeclByName(MERLIN_READ_BUFFER_PREFIX + task_name);
  if (read_buffer != nullptr) {
    void *read_buffer_body = ast->GetFuncBody(read_buffer);
    if (ast->GetChildStmtNum(read_buffer_body) == 0) {
      ast->DeleteNode(read_buffer_bb);
      read_buffer_bb = read_buffer_body;
    } else {
      ast->AppendChild(read_buffer_body, read_buffer_bb);
    }
  }
  //  /////////////////////////////  /
  //  ZP: 20161025
  //  fixing the problem of structure return type
  //  here: create the type for the return structure
  void *return_type = nullptr;
  {
    vector<void *> v_decls;
    ast->GetNodesByType(body, "preorder", "SgVariableDeclaration", &v_decls);
    for (auto decl : v_decls) {
      if (RETURN_VAR ==
          ast->GetVariableName(ast->GetVariableInitializedName(decl))) {
        return_type = ast->GetTypebyVar(ast->GetVariableInitializedName(decl));
      }
    }
  }

  //  2.0. get all the wrapper pragmas
  vector<pair<void *, void *>> v_port_pragma;
  map<string, string> port_name_to_wrapper_variable;
  {
    for (int i = 0; i < ast->GetFuncParamNum(kernel); i++) {
      void *sg_init = ast->GetFuncParam(kernel, i);
      void *sg_pragma = get_unique_wrapper_pragma(sg_init, ast, mars_ir);
      if (sg_pragma == nullptr) {
        continue;
      }
      //            string s_var = mars_ir->get_pragma_attribute(pragma,
      //            CMOST_variable); string s_port =
      //            mars_ir->get_pragma_attribute(pragma, "port"); if (s_var ==
      //            s_port) continue; string s_scope_type =
      //            mars_ir->get_pragma_attribute(sg_pragma, "scope_type"); if
      //            (s_scope_type == "return") continue;

      //  Yuxin 05/20/18: edit by Yuxin
      //  Otherwise the pragmas processed later will be floating nodes.
      void *new_pragma = ast->CopyStmt(sg_pragma);
      v_port_pragma.push_back(pair<void *, void *>(sg_init, new_pragma));
      ast->AppendChild(pragma_bb, new_pragma);
#if 0
      v_port_pragma.push_back(pair<void *, void *>(sg_init, sg_pragma));

      ast->AppendChild(pragma_bb, ast->CopyStmt(sg_pragma));
#endif
      string port_name = mars_ir->get_pragma_attribute(sg_pragma, "port");
      string var_name =
          mars_ir->get_pragma_attribute(sg_pragma, CMOST_variable);
      port_name_to_wrapper_variable[port_name] = var_name;
    }
  }
  ast->ReplaceStmt(body, new_body);

  //  2.1 declare buffer variables
  int count_port = 0;
  if (write_buffer != nullptr) {
    void *dummy_decl_0 = ast->CreateVariableDecl(
        "int", "__MERLIN_COMMENT_BEGIN__", nullptr, write_buffer_bb);
    ast->AppendChild(write_buffer_bb, dummy_decl_0);
  }
  if (read_buffer != nullptr) {
    void *dummy_decl_1 = ast->CreateVariableDecl(
        "int", "__MERLIN_COMMENT_BEGIN__", nullptr, read_buffer_bb);
    ast->AppendChild(read_buffer_bb, dummy_decl_1);
  }
  vector<void *> v_new_decl;
  for (auto it : v_port_pragma) {
    void *sg_init = it.first;
    void *sg_pragma = it.second;  //  wrapper pragma
    string s_var_wrapper =
        mars_ir->get_pragma_attribute(sg_pragma, CMOST_variable);
    void *sg_init_in_wrapper =
        ast->find_variable_by_name(s_var_wrapper, sg_pragma);
    string s_type = mars_ir->get_pragma_attribute(sg_pragma, "data_type");
    if (s_type.empty() && (sg_init_in_wrapper != nullptr)) {
      v_new_decl.push_back(sg_init_in_wrapper);
      continue;
    }  //  scalar
    void *sg_type = ast->GetTypeByString(s_type);
    string s_dim = mars_ir->get_pragma_attribute(sg_pragma, "max_depth");
    if (s_dim.empty() && (sg_init_in_wrapper != nullptr)) {
      v_new_decl.push_back(sg_init_in_wrapper);
      continue;
    }  //  scalar
    string s_depth = mars_ir->get_pragma_attribute(sg_pragma, "depth");
    if (s_depth.empty() && (sg_init_in_wrapper != nullptr)) {
      v_new_decl.push_back(sg_init_in_wrapper);
      continue;
    }  //  scalar
    int size = get_total_size_from_string(s_dim);

    if (size > 0) {
      if (sg_type == nullptr) {
        assert(0);
      }
      vector<size_t> v_size;
      v_size.push_back(size);
      sg_type = ast->CreateArrayType(sg_type, v_size);
    }

    if (sg_type == nullptr) {
      assert(0);
    }
    string s_var = ast->_up(sg_init);

    void *new_decl =
        ast->CreateVariableDecl(sg_type, "__m_" + s_var, nullptr, new_body);
    //  youxiang 20161012 avoid stack overflow
    ast->SetStatic(new_decl);
    v_new_decl.push_back(new_decl);
    ast->AppendChild(new_body, new_decl);

    if (write_buffer != nullptr) {
      void *new_decl_w = ast->CreateVariableDecl(sg_type, "__m_" + s_var,
                                                 nullptr, write_buffer_bb);
      ast->SetStatic(new_decl_w);
      ast->AppendChild(write_buffer_bb, new_decl_w);
    }
    if (read_buffer != nullptr) {
      void *new_decl_r = ast->CreateVariableDecl(sg_type, "__m_" + s_var,
                                                 nullptr, read_buffer_bb);
      ast->SetStatic(new_decl_r);
      ast->AppendChild(read_buffer_bb, new_decl_r);
    }
    count_port++;
  }
  if (write_buffer != nullptr) {
    void *dummy_decl_2 = ast->CreateVariableDecl(
        "int", "__MERLIN_COMMENT_END__", nullptr, write_buffer_bb);
    ast->AppendChild(write_buffer_bb, dummy_decl_2);
  }
  if (read_buffer != nullptr) {
    void *dummy_decl_3 = ast->CreateVariableDecl(
        "int", "__MERLIN_COMMENT_END__", nullptr, read_buffer_bb);
    ast->AppendChild(read_buffer_bb, dummy_decl_3);
  }
  //  2.1.1 create return variable
  {
    int hit_return = 0;
    for (auto it : v_port_pragma) {
      //  void * sg_init = it.first;
      void *sg_pragma = it.second;  //  wrapper pragma
      string s_var_wrapper =
          mars_ir->get_pragma_attribute(sg_pragma, CMOST_variable);
      string s_scope_type =
          mars_ir->get_pragma_attribute(sg_pragma, "scope_type");
      boost::algorithm::to_lower(s_scope_type);
      if (s_scope_type == "return" && hit_return == 0 &&
          (return_type != nullptr)) {
        hit_return = 1;
        vector<size_t> v1;
        v1.push_back(1);
        void *type = ast->CreateArrayType(return_type,
                                          v1);  //  ast->GetTypebyVar(sg_init);
        string s_var = RETURN_VAR;              //  ast->_up(sg_init);

        void *decl = ast->CreateVariableDecl(type, s_var, nullptr, new_body);
        ast->PrependChild(new_body, decl);
        void *ret_stmt = ast->CreateStmt(
            V_SgReturnStmt,
            ast->CreateExp(V_SgPointerDerefExp, ast->CreateVariableRef(decl)));
        v_to_be_appened_finally.push_back(ret_stmt);

        if (read_buffer != nullptr) {
          void *decl_r =
              ast->CreateVariableDecl(type, s_var, nullptr, read_buffer_bb);
          ast->PrependChild(read_buffer_bb, decl_r);
          void *ret_stmt_r = ast->CreateStmt(
              V_SgReturnStmt, ast->CreateExp(V_SgPointerDerefExp,
                                             ast->CreateVariableRef(decl_r)));
          v_to_be_appened_finally_r.push_back(ret_stmt_r);
        }
      }
    }
  }

  //  2.2 generating copy-in transfer for RO and RW ports
  if (write_buffer != nullptr) {
    generate_copy_in_or_out(v_port_pragma, 1, task_name, write_buffer_bb,
                            kernel, ast, mars_ir, port_name_to_wrapper_variable,
                            pcie_transfer_opt);
    ast->AppendChild(new_body, ast->CopyStmt(write_buffer_bb));
    remove_comment(ast, write_buffer_bb);
  } else {
    generate_copy_in_or_out(v_port_pragma, 1, task_name, new_body, kernel, ast,
                            mars_ir, port_name_to_wrapper_variable,
                            pcie_transfer_opt);
  }

  //  2.3 generating kernel function call
  {
    vector<void *> v_args;
    int tt = 0;
    for (auto new_decl : v_new_decl) {
      void *sg_init = v_port_pragma[tt].first;
      void *sg_pragma = v_port_pragma[tt].second;
      if (ast->IsInitName(sg_init) != 0) {
        //  if (ast->IsScalarType(ast->GetTypebyVar(sg_init)))
        if ((ast->IsPointerType(ast->GetTypebyVar(sg_init)) == 0) &&
            (ast->IsArrayType(ast->GetTypebyVar(sg_init)) == 0)) {
          string s_var =
              mars_ir->get_pragma_attribute(sg_pragma, CMOST_variable);
          string s_port = mars_ir->get_pragma_attribute(sg_pragma, "port");
          assert(s_port == ast->_up(sg_init));
          vector<int> v_dim;
          vector<void *> v_index;
          void *cond = nullptr;
          void *ref_var =
              CreateExpFromString(s_var, v_index, v_dim, new_body, ast, &cond);

          assert(ref_var);  //  FIXME

          v_args.push_back(ref_var);
        } else {
          v_args.push_back(ast->CreateVariableRef(new_decl));
        }
      }

      tt++;
    }
    void *call = ast->CreateFuncCall(kernel, v_args, new_body);
    void *stmt = ast->CreateStmt(V_SgExprStatement, call);
    //  string task_pragm_s = "ACCEL task";
    //  if(task_name == "") {
    //  } else {
    //    task_pragm_s = task_pragm_s + " name=" + task_name;
    //  }
    //  void * task_pragma = ast->CreatePragma(task_pragm_s, new_body);
    //  ast->AppendChild(new_body, task_pragma);
    ast->AppendChild(new_body, copy_kernel_pragma);
    ast->AppendChild(new_body, stmt);
  }

  //  2.4 generating copy-out transfer for WO and RW ports
  if (read_buffer != nullptr) {
    generate_copy_in_or_out(v_port_pragma, 0, task_name, read_buffer_bb, kernel,
                            ast, mars_ir, port_name_to_wrapper_variable,
                            pcie_transfer_opt);
    void *copy_body = ast->CopyStmt(read_buffer_bb);
    ast->AppendChild(new_body, copy_body);
    //  remove return declaration from wrapper read buffer function
    void *return_ref = ast->find_variable_by_name(RETURN_VAR, copy_body);
    void *return_decl = ast->GetVariableDecl(return_ref);
    if (ast->IsVariableDecl(return_decl) != 0) {
      ast->RemoveStmt(return_decl);
    }
    remove_comment(ast, read_buffer_bb);
  } else {
    generate_copy_in_or_out(v_port_pragma, 0, task_name, new_body, kernel, ast,
                            mars_ir, port_name_to_wrapper_variable,
                            pcie_transfer_opt);
  }

  //  2.5 move the struct def into wrapper if any
  move_struct_into_wrapper(wrapper, kernel, ast, mars_ir);
  if (write_buffer != nullptr) {
    move_struct_into_wrapper(write_buffer, kernel, ast, mars_ir);
  }
  if (read_buffer != nullptr) {
    move_struct_into_wrapper(read_buffer, kernel, ast, mars_ir);
  }

  //  2.6 post processing
  {
    for (auto stmt : v_to_be_appened_finally) {
      ast->AppendChild(new_body, stmt);
    }
    if (read_buffer != nullptr) {
      for (auto stmt : v_to_be_appened_finally_r) {
        ast->AppendChild(read_buffer_bb, stmt);
      }
    }
  }
}

//  process:
//  1. for each kernel, find its wrapper function if exists
//  2. for each, create a new body and replace the original one
//  2.1 declare buffer variables
//  2.2 generating copy-in transfer for RO and RW ports
//  2.3 generating kernel function call
//  2.4 generating copy-out transfer for WO and RW ports

int data_transfer_gen_top(CSageCodeGen *codegen, void *pTopFunc,
                          const CInputOptions &options) {
  string tool_version = options.get_option_key_value("-a", "tool_version");
  bool hls_flow = false;
  if ("vitis_hls" == tool_version || "vivado_hls" == tool_version) {
    hls_flow = true;
  }
  bool has_opencl_host_generation_disable_tag =
      test_file_for_read(OPENCL_HOST_GENERATION_DISABLED_TAG) != 0;
  if (hls_flow || has_opencl_host_generation_disable_tag) {
    cout << "Skip data transfer generation";
    return 0;
  }
  cout << "Data transfer generation" << endl;
  string pcie_transfer_opt = "off";
  if ("on" == options.get_option_key_value("-a", "pcie_transfer_opt")) {
    pcie_transfer_opt = "on";
  }

  CMarsIrV2 mars_ir;
  mars_ir.build_mars_ir(codegen, pTopFunc);

  auto kernels = mars_ir.get_top_kernels();

  for (auto kernel : kernels) {
    //  string func_name = codegen->GetFuncName(kernel);
    string task_name;
    //  task_name = get_task_name_by_kernel(codegen, kernel);
    task_name = codegen->get_task_name_by_kernel(kernel);
    //  string func_name = get_wrapper_name_by_kernel(codegen, kernel);
    string func_name = codegen->get_wrapper_name_by_kernel(kernel);
    void *wrapper = codegen->GetFuncDeclByName(func_name);
    if (wrapper == nullptr) {
      continue;
    }
    if (codegen->GetFuncBody(wrapper) == nullptr) {
      continue;
    }

    data_transfer_gen_one_kernel(wrapper, kernel, codegen, &mars_ir, task_name,
                                 pcie_transfer_opt);
  }

  return 0;
}
