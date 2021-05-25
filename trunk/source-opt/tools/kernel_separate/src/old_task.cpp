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


//  int check_global_and_static(CSageCodeGen &ast, void *func) {
//  int ret = 0;
//  vector<void *> func_calls;
//  ast.GetNodesByType_int(func, "preorder", V_SgFunctionCallExp, &func_calls);
//  for (auto call : func_calls) {
//    void *decl = ast.GetFuncDeclByCall(call, 1);
//    if (!decl) {
//      continue;
//    }
//    check_global_and_static(ast, decl);
//  }
//  vector<void *> ref_vars;
//  ast.GetNodesByType_int(func, "preorder", V_SgVarRefExp, &ref_vars);
//  for (auto var : ref_vars) {
//    void *name = ast.GetVariableInitializedName(var);
//    if (ast.IsGlobalInitName(name) && ast.IsFromInputFile(name)) {
//        string info = ast.UnparseToString(name) + "" +
//                           getUserCodeFileLocation(&ast, name, true);
//        string msg = "Cannot support global variable in task scope: \n " +
//        info; dump_critical_message(SEPAR_ERROR_18(msg), 0); ret = 1;
//    }
//    if (ast.IsLocalInitName(name)) {
//      void *decl = ast.GetVariableDecl(var);
//      if (ast.IsStatic(decl)) {
//        string info = ast.UnparseToString(name) + "" +
//                           getUserCodeFileLocation(&ast, name, true);
//        string msg = "Cannot support static variable in task scope: \n " +
//        info; dump_critical_message(SEPAR_ERROR_18(msg), 0); ret = 1;
//      }
//    }
//  }
//  if(ret) throw std::exception();
//  return ret;
//  }

//  Replace the task specification with L1 API calls
//  Note: consider the outer pragma together with the scope
//  0. each sub-task is a loop or basicblock
//  a. parallel sub-task //  ZP: 20170901: only support parallel of atom kernel
//  call first b. sequential sub-task c. pipeline sub-task //  ZP: 20170901: not
//  support, consider as sequential
//  int replace_task_with_l2_api(CMarsAST_IF *ast, CMarsIrV2 *mars_ir,
//                             void *sg_scope, string task_name) {
//  if (!ast->IsScopeStatement(sg_scope))
//    return 1;
//  if (0 >= ast->GetChildStmtNum(sg_scope))
//    return 1;
//
//  void *pre_pragma = nullptr;
//  void *task_decl = ast->GetFuncDeclByName(task_name);
//  if (task_decl) {
//    pre_pragma = ast->GetPreviousStmt_v1(task_decl);
//  } else {
//    //  HAN: FIXME the first statement of function call is pragma
//    //  now, the input sgcope include the parallel pragma and basic block
//    //  so the sg_scop should be inner basic block, then can find
//    //  the parallel pragma as previsous stmt
//    for (auto stmt : ast->GetChildStmts(sg_scope)) {
//      if (ast->IsBasicBlock(stmt)) {
//        sg_scope = stmt;
//        break;
//      }
//    }
//
//    pre_pragma = ast->GetPreviousStmt_v1(sg_scope);
//  }
//  cout << "task pragma = " << ast->UnparseToString(pre_pragma) << endl;
//  if (!ast->IsPragmaDecl(pre_pragma))
//    pre_pragma = nullptr;
//
//  string cmd = "";
//  if (pre_pragma)
//    cmd = mars_ir->get_pragma_attribute(pre_pragma, "parallel");
//  if (pre_pragma && cmd == "")
//    cmd = mars_ir->get_pragma_attribute(pre_pragma, "pipeline");
//
//  if (ast->IsBasicBlock(sg_scope)) {
//    //  Algorithm:
//    //  a. Partition the program into three parts:
//    //  b. Part 1: all the L1_write_buffer
//    //  c. Part 2: original statements, with kernel call replaced with
//    //  L1_execute d. Part 3: wait for all the L1_execute and read all
//    //  the L1_read_buffer
//
//    //  3.1 Get all the kernels in the scope of parallel
//    vector<void *> v_kernel;
//    vector<void *> v_call;
//    vector<vector<void *>> v_arg_list; //  each list per kernel call
//
//    for (auto stmt : ast->GetChildStmts(sg_scope)) {
//      void *expr = ast->GetExprFromStmt(stmt);
//      if (!ast->IsFunctionCall(expr))
//        continue;
//      void *decl = ast->GetFuncDeclByCall(expr);
//      if (!mars_ir->is_kernel(decl))
//        continue;
//
//      v_kernel.push_back(decl);
//      v_call.push_back(expr);
//      v_arg_list.push_back(
//          ast->CopyExprList(ast->GetFuncCallParamList(expr)));
//      cout << "fine one kernel call = " << ast->UnparseToString(expr) << endl;
//      cout << "fine one kernel decl = " << ast->UnparseToString(decl) << endl;
//    }
//
//    vector<void *> write_buffer_stmt;
//    vector<void *> wait_write_stmt;
//    vector<void *> read_buffer_stmt;
//    vector<void *> wait_read_stmt;
//    vector<void *> exec_kernel_call;
//    vector<void *> wait_exec_stmt;
//
//    //  3.2 Generate Write buffer
//    //  __merlin_write_buffer_kernelname
//    //  __merlin_wait_write_kernelname
//    {
//      for (size_t i = 0; i < v_kernel.size(); i++) {
//        void *kernel = v_kernel[i];
//        //  void * call   = v_call[i];
//        string kernel_name = ast->GetFuncName(kernel);
//
//        //  __merlin_write_buffer_kernelname
//        {
//          vector<void *> arg_list = ast->CopyExprList(v_arg_list[i]);
//          void *new_call = ast->CreateFuncCall(
//              "__merlin_write_buffer_" + kernel_name,
//              ast->GetTypeByString("void"), arg_list, sg_scope);
//          void *new_stmt = ast->CreateStmt(V_SgExprStatement, new_call);
//          write_buffer_stmt.push_back(new_stmt);
//        }
//        //  __merlin_wait_write_kernelname
//        {
//          vector<void *> arg_list = ast->CopyExprList(v_arg_list[i]);
//          vector<void *> empty_list;
//          void *new_call = ast->CreateFuncCall(
//              "__merlin_wait_write_" + kernel_name,
//              ast->GetTypeByString("void"), empty_list, sg_scope);
//          void *new_stmt = ast->CreateStmt(V_SgExprStatement, new_call);
//          wait_write_stmt.push_back(new_stmt);
//        }
//      }
//    }
//
//    //  3.3 Generate Read buffer
//    //  __merlin_read_buffer_kernelname
//    //  __merlin_wait_read_kernelname
//    {
//      for (size_t i = 0; i < v_kernel.size(); i++) {
//        void *kernel = v_kernel[i];
//        //  void * call   = v_call[i];
//        string kernel_name = ast->GetFuncName(kernel);
//
//        //  __merlin_read_buffer_kernelname
//        {
//          vector<void *> arg_list = ast->CopyExprList(v_arg_list[i]);
//          void *new_call = ast->CreateFuncCall(
//              "__merlin_read_buffer_" + kernel_name,
//              ast->GetTypeByString("void"), arg_list, sg_scope);
//          void *new_stmt = ast->CreateStmt(V_SgExprStatement, new_call);
//          read_buffer_stmt.push_back(new_stmt);
//        }
//        //  __merlin_wait_read_kernelname
//        {
//          vector<void *> arg_list = ast->CopyExprList(v_arg_list[i]);
//          vector<void *> empty_list;
//          void *new_call = ast->CreateFuncCall(
//              "__merlin_wait_read_" + kernel_name,
//              ast->GetTypeByString("void"), empty_list, sg_scope);
//          void *new_stmt = ast->CreateStmt(V_SgExprStatement, new_call);
//          wait_read_stmt.push_back(new_stmt);
//        }
//      }
//    }
//    //  3.4 Generate kernel execution
//    //  __merlin_execute_kernelname
//    for (size_t i = 0; i < v_kernel.size(); i++) {
//      void *kernel = v_kernel[i];
//      string kernel_name = ast->GetFuncName(kernel);
//
//      //  __merlin_wait_kernel_kernelname
//      {
//        vector<void *> arg_list = ast->CopyExprList(v_arg_list[i]);
//        vector<void *> empty_list;
//        void *new_call = ast->CreateFuncCall(
//            "__merlin_wait_kernel_" + kernel_name,
//            ast->GetTypeByString("void"), empty_list, sg_scope);
//        void *new_stmt = ast->CreateStmt(V_SgExprStatement, new_call);
//        wait_exec_stmt.push_back(new_stmt);
//      }
//
//      //  __merlin_execute_kernelname
//      {
//        vector<void *> arg_list = ast->CopyExprList(v_arg_list[i]);
//        void *new_call = ast->CreateFuncCall(
//            "__merlin_execute_" + kernel_name, ast->GetTypeByString("void"),
//            arg_list, sg_scope);
//        exec_kernel_call.push_back(new_call);
//      }
//    }
//
//    int number = ast->GetChildStmtNum(sg_scope);
//    void *insert_pos_head = ast->GetChildStmt(sg_scope, 0);
//    assert(insert_pos_head);
//    void *insert_pos_end = ast->GetChildStmt(sg_scope, number-1);
//    assert(insert_pos_end);
//    if (cmd == "__merlin_parallel") {
//        //  insert write buffer API
//        for (size_t i = 0; i < v_kernel.size(); i++) {
//            ast->InsertStmt(write_buffer_stmt[i], insert_pos_head);
//            ast->InsertStmt(wait_write_stmt[i], insert_pos_head);
//        }
//        //  insert read buffer API
//        for (size_t i = 0; i < v_kernel.size(); i++) {
//            ast->InsertAfterStmt(wait_read_stmt[i], insert_pos_end);
//            ast->InsertAfterStmt(read_buffer_stmt[i], insert_pos_end);
//        }
//        //  insert kernel execute API
//        for (size_t i = 0; i < v_kernel.size(); i++) {
//            void *call = v_call[i];
//            ast->ReplaceExp(call, exec_kernel_call[i]);
//            ast->InsertAfterStmt(wait_exec_stmt[i], insert_pos_end);
//        }
//    } else if (cmd == "__merlin_pipeline") {
//    } else {
//        //  insert write buffer API
//        for (size_t i = 0; i < v_kernel.size(); i++) {
//            void *call = v_call[i];
//            void * insert_pos = ast->TraceUpToStatement(call);
//            ast->InsertStmt(write_buffer_stmt[i], insert_pos);
//            ast->InsertStmt(wait_write_stmt[i], insert_pos);
//            ast->InsertAfterStmt(wait_read_stmt[i], insert_pos);
//            ast->InsertAfterStmt(read_buffer_stmt[i], insert_pos);
//            ast->InsertAfterStmt(wait_exec_stmt[i], insert_pos);
//            ast->ReplaceExp(call, exec_kernel_call[i]);
//        }
//    }
//
//    //  FIXME Han, init and release task kernel
//    { //  insert init function call
//      vector<void *> empty_list;
//      void *call = ast->CreateFuncCall("__merlin_init_" + task_name,
//                                       ast->GetTypeByString("int"),
//                                       empty_list, sg_scope);
//      void *stmt = ast->CreateStmt(V_SgExprStatement, call);
//      ast->PrependChild(sg_scope, stmt);
//    }
//
//    { //  insert release function call
//      vector<void *> empty_list;
//      void *call = ast->CreateFuncCall("__merlin_release_" + task_name,
//                                       ast->GetTypeByString("int"),
//                                       empty_list, sg_scope);
//      void *stmt = ast->CreateStmt(V_SgExprStatement, call);
//      ast->AppendChild(sg_scope, stmt);
//    }
//  }

/*
  boost::algorithm::to_lower(cmd);
  //  1. parallel sub-task
  if (cmd == "__merlin_parallel") {
    if (ast->IsBasicBlock(sg_scope)) {
      //  Algorithm:
      //  a. Partition the program into three parts:
      //  b. Part 1: all the L1_write_buffer
      //  c. Part 2: original statements, with kernel call replaced with
      //  L1_execute d. Part 3: wait for all the L1_execute and read all
      //  the L1_read_buffer

      //  3.1 Get all the kernels in the scope of parallel
      vector<void *> v_kernel;
      vector<void *> v_call;
      vector<vector<void *>> v_arg_list; //  each list per kernel call

      for (auto stmt : ast->GetChildStmts(sg_scope)) {
        void *expr = ast->GetExprFromStmt(stmt);
        if (!ast->IsFunctionCall(expr))
          continue;
        void *decl = ast->GetFuncDeclByCall(expr);
        if (!mars_ir->is_kernel(decl))
          continue;

        v_kernel.push_back(decl);
        v_call.push_back(expr);
        v_arg_list.push_back(
            ast->CopyExprList(ast->GetFuncCallParamList(expr)));
        cout << "fine one kernel call = " << ast->UnparseToString(expr) << endl;
        cout << "fine one kernel decl = " << ast->UnparseToString(decl) << endl;
      }

      //  3.2 Generate Write buffer
      //  __merlin_write_buffer_kernelname
      //  __merlin_wait_write_kernelname
      {
        void *insert_pos = ast->GetChildStmt(sg_scope, 0);
        assert(insert_pos);

        for (size_t i = 0; i < v_kernel.size(); i++) {
          void *kernel = v_kernel[i];
          //  void * call   = v_call[i];
          string kernel_name = ast->GetFuncName(kernel);

          //  __merlin_write_buffer_kernelname
          {
            vector<void *> arg_list = ast->CopyExprList(v_arg_list[i]);
            void *new_call = ast->CreateFuncCall(
                "__merlin_write_buffer_" + kernel_name,
                ast->GetTypeByString("void"), arg_list, sg_scope);
            void *new_stmt = ast->CreateStmt(V_SgExprStatement, new_call);
            ast->InsertStmt(new_stmt, insert_pos);
          }
          //  __merlin_wait_write_kernelname
          {
            vector<void *> arg_list = ast->CopyExprList(v_arg_list[i]);
            vector<void *> empty_list;
            void *new_call = ast->CreateFuncCall(
                "__merlin_wait_write_" + kernel_name,
                ast->GetTypeByString("void"), empty_list, sg_scope);
            void *new_stmt = ast->CreateStmt(V_SgExprStatement, new_call);
            ast->InsertStmt(new_stmt, insert_pos);
          }
        }
      }

      void *insert_pos_wait_exec = ast->CreateVariableDecl(
          "int", "__insert_pos_wait_exec__", nullptr, sg_scope);
      ast->AppendChild(sg_scope, insert_pos_wait_exec);

      //  3.3 Generate Read buffer
      //  __merlin_read_buffer_kernelname
      //  __merlin_wait_read_kernelname
      {
        void *insert_pos = ast->CreateVariableDecl(
            "int", "__insert_pos_read_buffer__", nullptr, sg_scope);
        ast->AppendChild(sg_scope, insert_pos);
        assert(insert_pos);


        for (size_t i = 0; i < v_kernel.size(); i++) {
          void *kernel = v_kernel[i];
          //  void * call   = v_call[i];
          string kernel_name = ast->GetFuncName(kernel);

          //  __merlin_read_buffer_kernelname
          {
            vector<void *> arg_list = ast->CopyExprList(v_arg_list[i]);
            void *new_call = ast->CreateFuncCall(
                "__merlin_read_buffer_" + kernel_name,
                ast->GetTypeByString("void"), arg_list, sg_scope);
            void *new_stmt = ast->CreateStmt(V_SgExprStatement, new_call);
            ast->InsertStmt(new_stmt, insert_pos);
          }
          //  __merlin_wait_read_kernelname
          {
            vector<void *> arg_list = ast->CopyExprList(v_arg_list[i]);
            vector<void *> empty_list;
            void *new_call = ast->CreateFuncCall(
                "__merlin_wait_read_" + kernel_name,
                ast->GetTypeByString("void"), empty_list, sg_scope);
            void *new_stmt = ast->CreateStmt(V_SgExprStatement, new_call);
            ast->InsertStmt(new_stmt, insert_pos);
          }
        }
        ast->RemoveStmt(insert_pos);
      }

      //  3.4 Generate kernel execution
      //  __merlin_execute_kernelname
      for (size_t i = 0; i < v_kernel.size(); i++) {
        void *kernel = v_kernel[i];
        void *call = v_call[i];
        string kernel_name = ast->GetFuncName(kernel);

        //  __merlin_wait_kernel_kernelname
        {
          vector<void *> arg_list = ast->CopyExprList(v_arg_list[i]);
          vector<void *> empty_list;
          void *new_call = ast->CreateFuncCall(
              "__merlin_wait_kernel_" + kernel_name,
              ast->GetTypeByString("void"), empty_list, sg_scope);
          void *new_stmt = ast->CreateStmt(V_SgExprStatement, new_call);
          ast->InsertStmt(new_stmt, insert_pos_wait_exec);
        }

        //  __merlin_execute_kernelname
        {
          vector<void *> arg_list = ast->CopyExprList(v_arg_list[i]);
          void *new_call = ast->CreateFuncCall(
              "__merlin_execute_" + kernel_name, ast->GetTypeByString("void"),
              arg_list, sg_scope);
          ast->ReplaceExp(call, new_call);
        }
      }

      //  FIXME Han, init and release task kernel
      { //  insert init function call
        vector<void *> empty_list;
        void *call = ast->CreateFuncCall("__merlin_init_" + task_name,
                                         ast->GetTypeByString("int"),
                                         empty_list, sg_scope);
        void *stmt = ast->CreateStmt(V_SgExprStatement, call);
        ast->PrependChild(sg_scope, stmt);
      }

      { //  insert release function call
        vector<void *> empty_list;
        void *call = ast->CreateFuncCall("__merlin_release_" + task_name,
                                         ast->GetTypeByString("int"),
                                         empty_list, sg_scope);
        void *stmt = ast->CreateStmt(V_SgExprStatement, call);
        ast->AppendChild(sg_scope, stmt);
      }

      ast->RemoveStmt(insert_pos_wait_exec);
      return 1;
    }
  }
  //  2. pipeline sub-task
  else if (cmd == "__merlin_pipeline") {
  }
  //  3. sequential sub-task
  else //  cmd == ""
  {
    if (ast->IsBasicBlock(sg_scope)) {
      //  1. Get all the kernels in the scope of parallel
      vector<void *> v_kernel;
      vector<void *> v_call;
      vector<vector<void *>> v_arg_list; //  each list per kernel call

      //  2. create kernels function artument list
      for (auto stmt : ast->GetChildStmts(sg_scope)) {
        //  void * expr = ast->GetExprFromStmt(stmt);
        void *expr = ast->GetFuncCallInStmt(stmt);
        if (!ast->IsFunctionCall(expr))
          continue;
        void *decl = ast->GetFuncDeclByCall(expr);
        if (!mars_ir->is_kernel(decl))
          continue;

        v_kernel.push_back(decl);
        v_call.push_back(expr);
        v_arg_list.push_back(
            ast->CopyExprList(ast->GetFuncCallParamList(expr)));
        int param_num = ast->GetFuncParamNum(decl);
        vector<void *> v_single_list;
        for (int j = 0; j < param_num; j++) {
          void *func_param = ast->GetFuncParam(decl, j);
          void *exp_param = ast->CreateVariableRef(func_param);
          v_single_list.push_back(exp_param);
        }
        //  v_arg_list.push_back(v_single_list);
      }

      //  3. generate __merlin_ function call
      for (size_t i = 0; i < v_kernel.size(); i++) {
        void *kernel = v_kernel[i];
        void *call_exp = v_call[i];
        string kernel_name = ast->GetFuncName(kernel);

        {
          vector<void *> arg_list = ast->CopyExprList(v_arg_list[i]);
          void *new_call = ast->CreateFuncCall("__merlin_" + kernel_name,
                                               ast->GetTypeByString("void"),
                                               arg_list, sg_scope);
          ast->ReplaceExp(call_exp, new_call);
        }
      }
    }
  }

  for (auto stmt : ast->GetChildStmts(sg_scope)) {
    replace_task_with_l2_api(ast, mars_ir, stmt, task_name);
  }
*/
//  4. Generating host program
//  return 1;
//  }

//  Generate task-level APIs
//  int generate_l2_api_top(CSageCodeGen *codegen, void *pTopFunc,
//                        const CInputOptions &options, set<void *> &visited,
//                        set<void *> &visited_global,
//                        vector<InsertNode> *insert_nodes,
//                        map<void *, void *> &s_done) {
//
//  CMarsIrV2 mars_ir;
//  mars_ir.build_mars_ir(codegen, pTopFunc);
//
//  auto kernels = mars_ir.get_top_kernels();
//  CSageCodeGen *ast = &codegen;
//
//  cout << "ksp 2.1.." << endl;
//  printf("\n\nGenerating L2 APIs ... \n");
//
//  //  check task pragma validation
//  int new_user_task_flag =
//      check_task_pragma_legality(codegen, pTopFunc, options);
//  if (new_user_task_flag == 1) {
//    //  check if cpp or c design
//    int is_cpp_design = -1;
//    for (void *kernel : kernels) {
//      if (is_cpp_design != -1 && is_cpp_design !=
//      ast->isWithInCppFile(kernel))
//        cout << "ERROR: detected mix C/C++ design" << endl;
//      is_cpp_design = ast->isWithInCppFile(kernel);
//    }
//    //  create task top file to retore task pragma function
//    string file_name =
//        string("__merlintask_top") + (is_cpp_design ? ".cpp" : ".c");
//    string cmd = "rm -rf " + file_name;
//    system(cmd.c_str());
//    void *task_file = nullptr;
//    task_file = ast->CreateSourceFile(file_name);
//    ast->AddDirectives("\n#include \"merlin_type_define.h\"\n", task_file);
//    ast->AddDirectives("\n#include \"__merlinhead_kernel_top.h\"\n",
//    task_file);
//    //  crete header file
//    string head_file_name =
//        string("__merlintask_top") + "_h" + (is_cpp_design ? ".cpp" : ".c");
//    {
//      string cmd = "rm -rf " + head_file_name;
//      system(cmd.c_str());
//    }
//    void *head_file = ast->CreateSourceFile(head_file_name);
//    string real_head_file_name = string("__merlintask_top") + ".h";
//    ast->AddDirectives("\n#include \"" + real_head_file_name + "\"\n",
//                       task_file);
//
//    //  create APIs in order with parallel or serial
//    map<string, map<string, string>> task_kernel_file;
//    vector<void *> v_pragma;
//    codegen->GetNodesByType(pTopFunc, "preorder", "SgPragmaDeclaration",
//                           &v_pragma);
//    for (auto pragma : v_pragma) {
//      string Cmd = mars_ir.get_pragma_attribute(pragma, "task");
//      boost::algorithm::to_lower(Cmd);
//      if (Cmd != "task")
//        continue;
//      void *next_stmt = codegen->GetNextStmt(pragma);
//      //  task pragma must followed by basic block or function declaration
//      if (codegen->IsFunctionDeclaration(next_stmt)) {
//        void *func_decl = next_stmt;
//        void *func_body = codegen->GetFuncBody(next_stmt);
//        string task_name = ast->GetFuncName(func_decl);
//        //  gnerate json file
//        map<string, string> kernel_file;
//        for (auto stmt : ast->GetChildStmts(func_body)) {
//          void *expr = ast->GetExprFromStmt(stmt);
//          if (!ast->IsFunctionCall(expr))
//            continue;
//          void *decl = ast->GetFuncDeclByCall(expr);
//          kernel_file.insert(pair<string, string>(ast->GetFuncName(decl),
//          ast->get_file(decl)));
//        }
//        task_kernel_file.insert(
//            pair<string, map<string, string>>(task_name, kernel_file));
//
//        int ret = replace_task_with_l2_api(ast, &mars_ir, func_body,
//        task_name); assert(ret);
//        //  insert to __merlintask file and delete the one in source code
//        void *clone_decl = ast->CloneFuncDecl(func_decl, task_file,
//                                              "__merlin_" + task_name, true);
//        ast->SetFuncBody(clone_decl, codegen->CopyStmt(func_body));
//        ast->AppendChild(task_file, clone_decl);
//        ast->RemoveStmt(func_decl);
//        void *clone_decl_head = ast->CloneFuncDecl(
//            func_decl, head_file, "__merlin_" + task_name, false);
//        ast->AppendChild(head_file, clone_decl_head);
//        ast->SetExtern(clone_decl_head);
//      }
//    }
//    writeInterReport("task_kernel.json", task_kernel_file);
//  } else
//  //  Step 1: only support the parallel pragma outside kernal calls(Write by
//  //  Peng)
//  {
//    cout << "get L2 API boundary." << endl;
//    //  1. Get L2 API boundaries in the host
//    //  L2 API boundary:
//    //    //  a. A L2 API scope can be kernel call itself
//    //    b. A L2 API scope can be a BB with "ACCEL pipeline/parallel"
//    //    pragma above, which contains kernel calls c. The L2 API function
//    //    boundray is the outermost L2 API scopes (which is not contained in
//    //    any other L2 API scopes. d. The "contain" relation does NOT
//    //    consider cross-function so far
//    set<void *> v_boundaries;
//    map<void *, void *> bound_org;
//
//    set<void *> boundary_org;
//    map<void *, string> map_task_name;
//    int is_cpp_design = -1;
//    {
//      for (void *kernel : kernels) {
//        vector<void *> v_calls;
//        ast->GetFuncCallsFromDecl(kernel, pTopFunc, &v_calls);
//        for (void *call : v_calls) {
//
//          //  1.1 trace up for valid scopes
//          void *curr_scope = call;
//          void *curr_boundary = nullptr;
//          void *pre_pragma = nullptr;
//          string task_name = "";
//
//          while (curr_scope) {
//            curr_scope = ast->TraceUpToScope(ast->GetParent(curr_scope));
//            if (!curr_scope)
//              break;
//            if (ast->IsFunctionDeclaration(curr_scope))
//              break;
//            if (ast->IsFunctionDefinition(curr_scope))
//              break;
//
//            size_t idx =
//                ast->GetChildStmtIdx(ast->GetParent(curr_scope), curr_scope);
//            if (idx < 1 || idx >=
//            ast->GetChildStmtNum(ast->GetParent(curr_scope))
//              continue;
//
//            //  void * pre_stmt = ast->GetPreviousStmt(curr_scope);
//            void *pre_stmt =
//                ast->GetChildStmt(ast->GetParent(curr_scope), idx - 1);
//            assert(pre_stmt);
//
//            if (!ast->IsPragmaDecl(pre_stmt))
//              continue;
//
//            string Cmd = mars_ir.get_pragma_attribute(pre_stmt, "parallel");
//            string Cmd1 = mars_ir.get_pragma_attribute(pre_stmt, "task");
//            string Cmd2 = mars_ir.get_pragma_attribute(pre_stmt, "pipeline");
//            boost::algorithm::to_lower(Cmd);
//            boost::algorithm::to_lower(Cmd1);
//            boost::algorithm::to_lower(Cmd2);
//            if (Cmd != "__merlin_parallel" && Cmd1 != "task" &&
//                Cmd2 != "__merlin_pipeline")
//              continue;
//
//            curr_boundary = curr_scope;
//            pre_pragma = pre_stmt;
//            task_name = mars_ir.get_pragma_attribute(pre_stmt, "name");
//          }
//
//          cout << "ksp 2.2.." << endl;
//          if (curr_boundary && v_boundaries.count(curr_boundary) == 0) {
//
//            void *bb = ast->CreateBasicBlock();
//            ast->AppendChild(bb, ast->CopyStmt(pre_pragma));
//            ast->AppendChild(bb, ast->CopyStmt(curr_boundary));
//            ast->InsertStmt(bb, curr_boundary);
//
//            v_boundaries.insert(bb);
//            auto one_bound_pragma = pair<void *, void *>(bb, curr_boundary);
//            bound_org.insert(one_bound_pragma);
//            printf("current_boundary = %s\n",
//                   ast->UnparseToString(curr_boundary).c_str());
//            auto one_name = pair<void *, string>(bb, task_name);
//            map_task_name.insert(one_name);
//  #if DEBUG_L2_API_GEN
//            cout << " -- insert_task " << task_name << ": " << ast->_p(bb)
//                 << endl;
//  #endif
//          }
//          cout << "ksp 2.3.." << endl;
//        }
//
//        if (is_cpp_design != -1 &&
//            is_cpp_design != ast->isWithInCppFile(kernel))
//          cout << "ERROR: detected mix C/C++ design" << endl;
//        is_cpp_design = ast->isWithInCppFile(kernel);
//      }
//    }
//    if (v_boundaries.size() == 0)
//      return 1;
//
//    //  2. Outline the scope and create the L2 function signature
//    map<void *, pair<void *, void *>> map_new_func_call;
//    vector<void *> v_scope_to_delete; //  we have added the generated calls
//    into
//                                      //  the AST, so need to delete them
//    cout << "ksp 2.4.." << endl;
//    void *new_file = nullptr;
//    {
//      if (v_boundaries.size() > 0) {
//        string file_name =
//            string("__merlintask_top") + (is_cpp_design ? ".cpp" : ".c");
//        {
//          string cmd = "rm -rf " + file_name;
//          system(cmd.c_str());
//        }
//        new_file = ast->CreateSourceFile(file_name);
//        ast->AddDirectives("\n#include \"merlin_type_define.h\"\n", new_file);
//        ast->AddDirectives("\n#include \"__merlinhead_kernel_top.h\"\n",
//                           new_file);
//        //  FILE *fp = fopen ("altera_const.cl", "r");
//        //  if (fp!=nullptr) {
//        //    fclose (fp);
//        //    return 0;
//        //  }
//      } else {
//        return 1;
//      }
//      cout << "ksp 2.4.1." << endl;
//      int count_boundary = 0;
//      for (auto boundary : v_boundaries) {
//        void *new_call = nullptr;
//        void *new_decl = nullptr;
//        string task_name = map_task_name[boundary];
//        if (task_name == "") {
//          task_name = "merlin_unnamedtask_";
//          //  task_name = "merlin_unnamedtask_" +
//          //  to_string(count_task);
//        }
//        int add_number_suffix = task_name == "merlin_unnamedtask_";
//        //  int flag = 0;
//        //  void * stmt_one;
//        //  int num1 = ast->GetChildStmtNum(boundary);
//        //  if(num1 == 2) {
//        //  void * stmt = ast->GetChildStmt(boundary, 0);
//        //  stmt = ast->GetChildStmt(boundary, 1);
//        //  int num2 = ast->GetChildStmtNum(stmt);
//        //  //  if(num2 == 1) {
//        //  //  stmt_one = ast->GetChildStmt(stmt, 0);
//        //  //  flag = 1;
//        //  //  }
//        //  }
//        cout << "ksp 2.4.2." << endl;
//        auto kernel_insert = copy_decl_in_scope_between_files(
//            boundary, nullptr, ast, visited, visited_global, s_done,
//            /* cross function */ false);
//        insert_nodes->insert(insert_nodes->end(), kernel_insert.begin(),
//                             kernel_insert.end());
//        cout << "ksp 2.4.3." << endl;
//        //  outline new function declaration and call for specify task
//        //  scope
//        ast->function_outline(boundary, new_call, new_decl, task_name,
//                              add_number_suffix, false, true, true, false);
//        cout << "ksp 2.4.4." << endl;
//        count_boundary++;
//        if (new_call && new_decl) {
//          printf("one_decl = %s\n\n\n",
//          ast->UnparseToString(new_decl).c_str()); printf("one_call =
//          %s\n\n\n", ast->UnparseToString(new_call).c_str());
//
//          //          void * clone_decl = ast->CloneFuncDecl(new_decl,
//          //          new_file, false); ast->AppendChild(new_file,
//          //          new_decl);
//          v_scope_to_delete.push_back(ast->TraceUpToStatement(new_call));
//          v_scope_to_delete.push_back(boundary);
//          pair<void *, void *> one_pair = make_pair(new_call, boundary);
//          map_new_func_call.insert(
//              pair<void *, pair<void *, void *>>(new_decl, one_pair));
//        } else
//          v_scope_to_delete.push_back(boundary);
//      }
//    }
//    cout << "ksp 2.5.." << endl;
//    //  2.1 Create header file, and add declarations into it
//    {
//      //  2.1.1 Create header file (using .cpp suffix)
//      string head_file_name =
//          string("__merlintask_top") + "_h" + (is_cpp_design ? ".cpp" : ".c");
//      {
//        string cmd = "rm -rf " + head_file_name;
//        system(cmd.c_str());
//      }
//      cout << "ksp 2.5.1.1." << endl;
//      void *head_file = ast->CreateSourceFile(head_file_name);
//      cout << "ksp 2.5.1.2." << endl;
//      string real_head_file_name = string("__merlintask_top") + ".h";
//      ast->AddDirectives("\n#include \"" + real_head_file_name + "\"\n",
//                         new_file);
//
//      cout << "ksp 2.5.1." << endl;
//      //  2.1.2 Append declarations
//      void *first_decl = nullptr;
//      void *last_decl = nullptr;
//      for (auto one_pair : map_new_func_call) {
//        void *one_decl = one_pair.first;
//
//        //  ..._task_func();
//        cout << "ksp 2.5.2." << endl;
//        void *clone_decl = ast->CloneFuncDecl(one_decl, head_file, false);
//        cout << "ksp 2.5.3." << endl;
//        ast->AppendChild(head_file, clone_decl);
//        cout << "ksp 2.5.4." << endl;
//        ast->SetExtern(clone_decl);
//        if (first_decl == nullptr)
//          first_decl = clone_decl;
//
//        //        vector<string> empty_list;
//        //
//        //        //  ..._init();
//        //        vector<void *> arg_list;
//        //        arg_list.push_back(codegen->CreateVariable(
//        //              codegen->GetTypeByString("const char *"),
//        //              "bitstream"));
//        //        string init_func_name = ast->GetFuncName(one_decl) +
//        //        "_init"; clone_decl = ast->CreateFuncDecl("void",
//        //        init_func_name, empty_list, empty_list, head_file,
//        //        false, nullptr); ast->AppendChild(head_file,
//        //        clone_decl); ast->SetExtern(clone_decl);
//
//      cout << "ksp 2.6.." << endl;
//      //            //  2.1.3 Add header redefinition macros
//      //            string head_file_macro = head_file_name + "_included_";
//      //            boost::algorithm::to_upper(head_file_macro);
//      //            boost::replace_all(head_file_macro, ".", "_");
//      //
//      //            ast->AddDirectives("\n#ifndef " + head_file_macro + "\n",
//      //                               first_decl);
//      //            ast->AddDirectives("\n#define " + head_file_macro + "\n",
//      //                               first_decl);
//      //            //      ast->AddDirectives("\n#include
//      //            //      \"__merlinhead_kernel_top.h\"\n", first_decl);
//      //            ast->AddDirectives("\n#endif // " + head_file_macro +
//      "\n",
//      //                               last_decl, false); //  after
//    }
//
//    //  3. Create Init and Release functions
//    {
//      for (auto one_task : map_new_func_call) {
//        void *one_decl = one_task.first;
//        pair<void *, void *> call_bound = one_task.second;
//        void *one_call = call_bound.first;
//        void *one_boundary = call_bound.second;
//
//        set<void *> v_kernels;
//        {
//          vector<void *> v_all_calls;
//          ast->GetNodesByType(one_decl, "preorder", "SgFunctionCallExp",
//                              &v_all_calls);
//          for (auto call : v_all_calls) {
//            void *decl = ast->GetFuncDeclByCall(call);
//            if (!decl)
//              continue;
//            if (!mars_ir.is_kernel(decl))
//              continue;
//            v_kernels.insert(decl);
//          }
//        }
//
//        //  __merlin_init_kernelname
//        vector<void *> arg_list;
//        vector<void *> arg_list_decl;
//        string opencl_arg;
//        string init_func_name = ast->GetFuncName(one_decl) + "_init";
//        {
//          //  vector<string> empty_list;
//          ///  /  void * bit_decl =
//          ///  /  codegen->CreateVariable(codegen->GetTypeByString("const
//          ///  /  char
//          ///  /  *"), "bitstream"); arg_list_decl.push_back(bit_decl);
//          //  void *init_decl = ast->CreateFuncDecl(
//          //    "void", init_func_name, empty_list, empty_list,
//          //    new_file, true, nullptr);
//          //  ast->AppendChild(new_file, init_decl);
//          //
//          //  void *body = ast->GetFuncBody(init_decl);
//          {
//            string tool_type = "";
//            if ("aocl" == options.get_option_key_value("-a", "impl_tool")) {
//              tool_type = "aocl";
//            } else if ("sdaccel" ==
//                       options.get_option_key_value("-a", "impl_tool")) {
//              tool_type = "sdaccel";
//            } else {
//              tool_type = "aocl";
//            }
//            if (tool_type == "aocl") {
//              opencl_arg = "kernel_top.aocx";
//            } else if (tool_type == "sdaccel") {
//              opencl_arg = "kernel_top.xclbin";
//            } else {
//              opencl_arg = "kernel_top.aocx";
//            }
//            //  arg_list.push_back(ast->CreateVariableRef(bit_decl));
//          }
//
//          ///  /  void * call = ast->CreateFuncCall("__merlin_init_opencl",
//          ///  /  ast->GetTypeByString("void"), arg_list, body); void *
//          ///  /  stmt = ast->CreateStmt(V_SgExprStatement, call);
//          ///  /  ast->AppendChild(body, stmt);
//
//          //  for (auto kernel : v_kernels) {
//          //    string kernel_name = ast->GetFuncName(kernel);
//          //    vector<void *> empty_list;
//          //    void *call = ast->CreateFuncCall(
//          //        "__merlin_init_" + kernel_name,
//          //        ast->GetTypeByString("void"), empty_list, body);
//          //    void *stmt = ast->CreateStmt(V_SgExprStatement, call);
//
//        //  __merlin_release_kernelname
//        //  string release_func_name =
//        //    ast->GetFuncName(one_decl) + "_release";
//        //  {
//        //    vector<string> empty_list;
//        //    void *init_decl = ast->CreateFuncDecl(
//        //        "void", release_func_name, empty_list, empty_list,
//        //        new_file, true, nullptr);
//        //    ast->AppendChild(new_file, init_decl);
//
//        //    void *body = ast->GetFuncBody(init_decl);
//
//        //    for (auto kernel : v_kernels) {
//        //        string kernel_name = ast->GetFuncName(kernel);
//        //        vector<void *> empty_list;
//        //        void *call = ast->CreateFuncCall(
//        //            "__merlin_release_" + kernel_name,
//        //            ast->GetTypeByString("void"), empty_list, body);
//        //        void *stmt = ast->CreateStmt(V_SgExprStatement, call);
//        //        ast->AppendChild(body, stmt);
//        //    }
//
//        //    {
//        //        //  vector<void*> empty_list;
//        //        //  void * call =
//        //        //  ast->CreateFuncCall("__merlin_release_opencl",
//        //        //  ast->GetTypeByString("void"), empty_list, body); void
//        //        //  * stmt = ast->CreateStmt(V_SgExprStatement, call);
//        //        //  ast->AppendChild(body, stmt);
//        //    }
//        //  }
//        //  host program update
//        if (ast->IsBasicBlock(one_boundary)) {
//          void *org_boundary = bound_org.find(one_boundary)->second;
//          if (ast->IsBasicBlock(org_boundary)) {
//            vector<void *> empty_list;
//            vector<void *> arg_list;
//            arg_list.push_back(ast->CreateStringExp(opencl_arg));
//            string file_name = ast->get_file(org_boundary);
//            void *host_decl = ast->TraceUpToFuncDecl(org_boundary);
//            void *new_block = ast->CreateBasicBlock();
//            ast->ReplaceStmt(org_boundary, new_block);
//            //  void * call_init =
//            //  ast->CreateFuncCall(init_func_name,
//            //  ast->GetTypeByString("int"), arg_list, new_block);
//            //  void * call_release =
//            //  ast->CreateFuncCall(release_func_name,
//            //  ast->GetTypeByString("int"), empty_list, new_block);
//            void *call_init = ast->CreateFuncCall("__merlin_init",
//                                                  ast->GetTypeByString("int"),
//                                                  arg_list, new_block);
//            if (count_init_call == 0) {
//              ast->AppendChild(new_block,
//                               ast->CreateStmt(V_SgExprStatement, call_init));
//              //  string head_file_name = "#include
//              //  \"merlin_type_define.h\"\n";
//              //  ast->AddHeader(head_file_name, host_global);
//              string head_file_name = "__merlinhead_kernel_top.h";
//              ast->InsertHeader(head_file_name, host_decl);
//              head_file_name = "__merlintask_top.h";
//              ast->InsertHeader(head_file_name, host_decl);
//              count_init_call++;
//            }
//            ast->AppendChild(
//                new_block,
//                ast->CreateStmt(V_SgExprStatement, ast->CopyExp(one_call)));
//            //  void * call_release =
//            //  ast->CreateFuncCall("__merlin_release_opencl",
//            //  ast->GetTypeByString("int"), empty_list, new_block);
//            //  ast->AppendChild(new_block,
//            //  ast->CreateStmt(V_SgExprStatement, call_release));
//          }
//        }
//      }
//    }
//
//    cout << "ksp 2.7.." << endl;
//    //  4. Repalce the content with L1 API calls
//    //    for each task
//    //    __merlin_write_buffer_kernelname
//    //    __merlin_wait_write_kernelname
//    //    __merlin_execute_kernelname
//    //    __merlin_wait_kernel_kernelname
//    //    __merlin_read_buffer_kernelname
//    //    __merlin_wait_read_kernelname
//    {
//      for (auto one_task : map_new_func_call) {
//        void *one_decl = one_task.first;
//
//        void *func_body = ast->GetFuncBody(one_decl);
//        assert(func_body);
//        string task_name = ast->GetFuncName(one_decl);
//        int ret = replace_task_with_l2_api(ast, &mars_ir, func_body,
//        task_name); assert(ret);
//        //  insert to __merlintask file and delete the one in host
//        //  program
//        void *clone_decl = ast->CloneFuncDecl(one_decl, new_file, true);
//        ast->SetFuncBody(clone_decl, func_body);
//        ast->AppendChild(new_file, clone_decl);
//        ast->RemoveStmt(one_decl);
//      }
//    }
//
//    cout << "ksp 2.8.." << endl;
//    //  5. Cleaning up
//    {
//      for (auto scope : v_scope_to_delete) {
//        if (scope && !ast->is_floating_node(scope))
//          ast->RemoveStmt(scope);
//      }
//    }
//    cout << "ksp 2.9.." << endl;
//
//  } //  End of Step 1: only support the parallel pragma outside kernal calls
//
//  printf("\n\nEnd of Generating L2 APIs ... \n");
//  return 0;
//  }

//  //  extract task API
//  int extract_task_api_top(CSageCodeGen *codegen, void *pTopFunc,
//                        const CInputOptions &options,
//                        map<void *, void *> &s_done) {
//  CMarsIrV2 mars_ir;
//  mars_ir.build_mars_ir(codegen, pTopFunc);
//  auto kernels = mars_ir.get_top_kernels();
//  CSageCodeGen *ast = &codegen;
//  //  crete header file
//  int is_cpp_design = -1;
//  for (void *kernel : kernels) {
//    if (is_cpp_design != -1 && is_cpp_design != ast->isWithInCppFile(kernel))
//      cout << "ERROR: detected mix C/C++ design" << endl;
//    is_cpp_design = ast->isWithInCppFile(kernel);
//  }
//  string head_file_name = string(TASK_HEAD_FILE_PRE) + (is_cpp_design ? ".cpp"
//  : ".c"); {string cmd = "rm -rf " + head_file_name; system(cmd.c_str());}
//  void *head_file = ast->CreateSourceFile(head_file_name);
//
//  printf("\n\nExtract task APIs ... \n");
//  //  check task pragma validation
//  int task_pass = check_task_pragma_legality(codegen, pTopFunc, options);
//  if (task_pass == 1) {
//    set<void *> visited_global;
//    set<void *> visited;
//    map<void *, void *> s_sub_done;
//    map<string, map<string, string>> task_kernel_file;
//    vector<void *> v_pragma;
//    codegen->GetNodesByType(pTopFunc, "preorder", "SgPragmaDeclaration",
//                           &v_pragma);
//    for (auto pragma : v_pragma) {
//      string Cmd = mars_ir.get_pragma_attribute(pragma, "task");
//      boost::algorithm::to_lower(Cmd);
//      if (Cmd != "task")
//        continue;
//      cout << "Find one task pragma : " << ast->UnparseToString(pragma) <<
//      endl; void *next_stmt = codegen->GetNextStmt(pragma); if
//      (codegen->IsFunctionDeclaration(next_stmt)) {
//        void *func_decl = next_stmt;
//        void *func_body = codegen->GetFuncBody(next_stmt);
//        string task_name = ast->GetFuncName(func_decl);
//        //  create task top file to retore task pragma function
//        string file_name = string(TASK_FILE_PREFIX) + task_name +
//        (is_cpp_design ? ".cpp" : ".c"); {string cmd = "rm -rf " + file_name;
//        system(cmd.c_str());} void *task_file = nullptr; task_file =
//        ast->CreateSourceFile(file_name);
//
//        //  insert to __merlintask file and delete the one in source code
//        void *clone_decl = ast->CloneFuncDecl(func_decl, task_file,
//        KERNEL_TASK_API_PREFIX + task_name, true);
//        ast->SetFuncBody(clone_decl, codegen->CopyStmt(func_body));
//        ast->AppendChild(task_file, clone_decl);
//        void * task_pragma = ast->CopyStmt(pragma);
//        ast->InsertStmt(task_pragma, clone_decl);
//
//        //  extract all kernel declaration
//        map<string, string> kernel_file;
//        for (auto stmt : ast->GetChildStmts(func_body)) {
//          void *expr = ast->GetExprFromStmt(stmt);
//          if (!ast->IsFunctionCall(expr))
//            continue;
//          void *decl = ast->GetFuncDeclByCall(expr);
//          cout << "Insert kernel declaration... " <<
//          ast->UnparseToString(decl) << endl; kernel_file.insert(pair<string,
//          string>(ast->GetFuncName(decl), ast->get_file(decl))); void *
//          clone_kernel_decl = ast->CloneFuncDecl(decl, task_file,
//          ast->GetFuncName(decl), false); ast->InsertStmt(clone_kernel_decl,
//          task_pragma);
//        }
//        task_kernel_file.insert(
//            pair<string, map<string, string>>(task_name, kernel_file));
//
//        //  extract all global variables
//        auto insertList = copy_decl_in_scope_between_files(
//            func_decl, task_file, ast, visited, visited_global, s_sub_done);
//        for (const auto &node : insertList) {
//          //  20170521 ast->_up can't work for global variables
//          if (!node.IsType) {
//            //  strip C++-esque modifiers.
//            void *unp_node = node.Node;
//            if(ast->IsVariableDecl(unp_node)) {
//              ast->InsertStmt(unp_node, task_pragma);
//              cout << "Copying global variable... " << ast->_up(unp_node) <<
//              endl;
//            }
//          }
//        }
//        ast->InsertHeader(string(MERLIN_TYPE_DEFINE), task_file, true, false);
//        ast->InsertHeader(string(KERNEL_HEADER_FILE), task_file, true, false);
//        ast->InsertHeader(string(TASK_HEAD_FILE), task_file, true, false);
//        void *clone_decl_head = ast->CloneFuncDecl(func_decl, head_file,
//        KERNEL_TASK_API_PREFIX + task_name, false);
//        ast->AppendChild(head_file, clone_decl_head);
//        ast->SetExtern(clone_decl_head);
//      }
//    }
//    writeInterReport(TASK_KERNEL_JSON, task_kernel_file);
//  }
//  printf("\n\nExit extract task APIs ... \n");
//  return 0;
//  }
