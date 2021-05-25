/************************************************************************************
 *  (c) Copyright 2014-2020 Falcon Computing Solutions, Inc. All rights
 *reserved.
 *
 *  This file contains confidential and proprietary information
 *  of Falcon Computing Solutions, Inc. and is protected under U.S. and
 *  international copyright and other intellectual property laws.
 *
 ************************************************************************************/

//******************************************************************************************//
// module           :   final code generation
// description      :   generate a final opencl output
// input            :
// output           :
// limitation       :
// author           :   Han
//******************************************************************************************//
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <cstring>
#include <boost/algorithm/string.hpp>
#include "stream_update.h"
#include "PolyModel.h"
#include "cmdline_parser.h"
#include "file_parser.h"
#include "mars_opt.h"
#include "tldm_annotate.h"
#include "xml_parser.h"
#include "analPragmas.h"
#include "codegen.h"
#include "id_update.h"
#include "ir_types.h"
#include "mars_ir.h"
#include "mars_ir_v2.h"
#include "mars_message.h"
#include "math.h"
#include "program_analysis.h"
#include "rose.h"
#include "stream_ir.h"
#include "report.h"
#include "common.h"
#include "input_checker.h"
#include "regex"  // NOLINT

#define _DEBUG_DATA_PARSER 1
#define USED_CODE_IN_COVERAGE_TEST 0
#define EXTERN_C_BEGIN "#ifdef __cplusplus\nextern \"C\" {\n#endif\n";
#define EXTERN_C_END "#ifdef __cplusplus\n}\n#endif\n";
#define WRAPPER_PREFIX "__merlinwrapper_"
// Xilinx name bound length is 235, set it as 100 temp
// Because when generate .vhd file, it may extend the file name again
#define MAX_FUNC_NAME 100

using std::exception;
using std::find;
using std::max;
using std::ofstream;
using std::replace;
using std::reverse;
using std::string;
// using namespace MarsProgramAnalysis;
// using namespace MerlinStreamModel;
using MarsProgramAnalysis::ArrayRangeAnalysis;
using MarsProgramAnalysis::CheckAccessSeparabilityInScope;
using MarsProgramAnalysis::CMarsExpression;
using MarsProgramAnalysis::CMarsRangeExpr;
using MerlinStreamModel::CStreamBase;
using MerlinStreamModel::CStreamIR;
//  using namespace SageInterface;
//  using namespace SageBuilder;
//  Temp use global variable to save effort
bool pure_kernel_flow = false;
static int count_memcpy = 0;

extern int check_kernel_argument_number(CSageCodeGen *codegen,
                                        vector<void *> vec_kernels,
                                        string tool);
extern void GetTLDMInfo_withPointer4(void *sg_node, void *pArg);
extern vector<string> get_unique_interface_attribute(
    void *sg_init, string attribute, CSageCodeGen *ast,
    CMarsIrV2
        *mars_ir);  //  source_opt/tools/array_linearize/src/array_linearize.cpp

// the function is defined in loop_paralla.cpp
extern int convert_attribute_pragma_to_type(CSageCodeGen *codegen,
                                            void *pTopFunc,
                                            const CInputOptions &options);
extern bool check_opencl_conflict_name(CSageCodeGen *codegen, void *TopFunc,
                                       bool is_intel);

// legalize kernel name
// 1. "__" => "_"
// 2. "_x" => "x"
// 3. "x_" => "x"
// 4. opencl keyword
// 5. HDL keyword
// 6. other reserved keyword
string legalize_kernel_name(string name) {
  cout << "    Legalizing_kernel_name from : " << name << " to ";
  int pos = name.find("__");
  while (pos >= 0) {
    name.replace(pos, 2, "_");
    pos = name.find("__");
  }
  while (!isalpha(name[0])) {
    name.erase(0, 1);
  }
  while (!isalpha(name[name.length() - 1]) &&
         !isdigit(name[name.length() - 1])) {
    name.erase(name.length() - 1, 1);
  }
  set<string> keywords;
  for (auto key : opencl_keyword) {
    keywords.insert(key);
  }
  for (auto key : hdl_keyword) {
    keywords.insert(key);
  }
  for (auto key : other_keyword) {
    keywords.insert(key);
  }
  for (auto key : internal_name) {
    keywords.insert(key);
  }
  int index = 0;
  while (std::find(begin(keywords), std::end(keywords), name) !=
         std::end(keywords)) {
    name += "_merlinkernel" + std::to_string(index);
    index++;
  }
  cout << name << endl;
  return name;
}

// legalize kernel
void legalize_kernel(CSageCodeGen *codegen, void *kernel) {
  printf("    legalizing kernel...\n");
  CMarsAST_IF *ast = codegen;
  string kernel_name = ast->GetFuncName(kernel);
  string new_name = legalize_kernel_name(kernel_name);
  if (new_name != kernel_name) {
    ast->SetFuncName(kernel, new_name);
  }
}
string opencl_buffer(string kernel_name, string port_name,
                     bool is_migrate = false) {
  return "__" + kernel_name + "__" + port_name + "_buffer" +
         (is_migrate ? "__migrate" : "");
}
string migrate_vector_cl_mem(string kernel_name, bool is_input) {
  return "v_cl_mem_" + kernel_name + "_" + (is_input ? "i" : "o");
}

int replace_memcpy_with_opencl_in_lib_wrapper(CSageCodeGen *codegen,
                                              void *pTopFunc,
                                              const CInputOptions &options,
                                              string pcie_transfer_opt) {
  cout << "Data transfer generation for OpenCL" << endl;
  CSageCodeGen *ast = codegen;

  CMarsIrV2 mars_ir;
  mars_ir.build_mars_ir(codegen, pTopFunc);

  map<void *, vector<void *>> map_kwrapper2pragma;
  vector<void *> vec_pragmas;
  ast->GetNodesByType(ast->GetProject(), "preorder", "SgPragmaDeclaration",
                      &vec_pragmas);

  for (auto pragma : vec_pragmas) {
    string s_wrapper = mars_ir.get_pragma_attribute(pragma, "wrapper");
    boost::algorithm::to_lower(s_wrapper);
    if ("wrapper" != s_wrapper) {
      continue;
    }

    void *kwrapper = ast->TraceUpToFuncDecl(pragma);

    if (kwrapper == nullptr) {
      continue;
    }

    if (map_kwrapper2pragma.find(kwrapper) == map_kwrapper2pragma.end()) {
      vector<void *> empty;
      map_kwrapper2pragma[kwrapper] = empty;
    }
    map_kwrapper2pragma[kwrapper].push_back(pragma);
  }

  ast->RegisterType("__CMOST_COMMENT__");
  for (auto it : map_kwrapper2pragma) {
    void *kwrapper = it.first;
    vector<void *> pragmas = it.second;
    void *kwrapper_body = ast->GetFuncBody(kwrapper);
    string s_kernel = ast->GetFuncName(kwrapper);
    s_kernel = s_kernel.substr(strlen(MERLIN_WRAPPER_PREFIX));

    // this basic block used to insert cl_migrate variable declaration
    // and buffer creation
    map<string, void *> mapPort2Pragma;
    for (auto pragma : pragmas) {
      string s_port = mars_ir.get_pragma_attribute(pragma, "port");

      string s_local_var = "__m_" + s_port;
      void *sg_var = ast->find_variable_by_name(s_local_var, kwrapper_body);
      if (sg_var != nullptr) {
        void *sg_type = ast->GetTypebyVar(sg_var);
        if (pcie_transfer_opt == "on" && ast->IsArrayType(sg_type)) {
          string migrate_decl_name = opencl_buffer(s_kernel, s_port, true);
          string migrate_decl = "\n\tcl_mem " + migrate_decl_name + ";\n";
          ast->AddDirectives(migrate_decl, codegen->TraceUpToStatement(sg_var));
          void *sg_base_type;
          vector<size_t> sg_sizes;
          ast->get_type_dimension(sg_type, &sg_base_type, &sg_sizes);
          int bitwidth = ast->get_bitwidth_from_type(sg_base_type, false);
          int bytewidth = bitwidth / 8;
          size_t total_size = 1;
          for (auto one_size : sg_sizes) {
            total_size *= one_size;
          }
          void *assert_point = codegen->TraceUpToStatement(sg_var);
          // alignment to 256 to enable DMA transfer
          string alignment = "\n\tposix_memalign((void**)&" + s_local_var +
                             ", 256, " + std::to_string(total_size) + " * " +
                             std::to_string(bytewidth) + ");\n";
          ast->AddDirectives(alignment, assert_point, 0);
          // create buffer
          string create_buffer =
              "\n\topencl_create_buffer(&" + migrate_decl_name + ", " +
              std::to_string(total_size) + " * " + std::to_string(bytewidth) +
              ", 2" + ", " + s_local_var + ");";
          ast->AddDirectives(create_buffer, assert_point, 0);
        }
      }
    }
  }

  int count_wrapper = 0;
  for (auto it : map_kwrapper2pragma) {
    void *kwrapper = it.first;
    vector<void *> pragmas = it.second;
    void *kwrapper_body = ast->GetFuncBody(kwrapper);

    string s_kernel = ast->GetFuncName(kwrapper);
    int j0 = s_kernel.find(MERLIN_WRAPPER_PREFIX);
    if (j0 != 0) {
      dump_critical_message(CLGEN_ERROR_4(
          "Found unexpected kernel wrapper function " + s_kernel + "\n"));
      throw std::exception();
    }
    s_kernel = s_kernel.substr(strlen(MERLIN_WRAPPER_PREFIX));

    map<string, void *> mapPort2Pragma;
    for (auto pragma : pragmas) {
      string s_port = mars_ir.get_pragma_attribute(pragma, "port");
      if (!s_port.empty()) {
        mapPort2Pragma[s_port] = pragma;
      }
    }

    vector<void *> v_memcpy;
    {
      vector<void *> v_calls;
      ast->GetNodesByType(kwrapper, "preorder", "SgFunctionCallExp", &v_calls);
      for (auto call : v_calls) {
        if ("memcpy" == ast->GetFuncNameByCall(call)) {
          v_memcpy.push_back(call);
        }
      }
      void *write_buffer;
      write_buffer =
          ast->GetFuncDeclByName(MERLIN_WRITE_BUFFER_PREFIX + s_kernel);
      ast->GetNodesByType(write_buffer, "preorder", "SgFunctionCallExp",
                          &v_calls);
      for (auto call : v_calls) {
        if ("memcpy" == ast->GetFuncNameByCall(call)) {
          v_memcpy.push_back(call);
        }
      }
      void *read_buffer;
      read_buffer =
          ast->GetFuncDeclByName(MERLIN_READ_BUFFER_PREFIX + s_kernel);
      ast->GetNodesByType(read_buffer, "preorder", "SgFunctionCallExp",
                          &v_calls);
      for (auto call : v_calls) {
        if ("memcpy" == ast->GetFuncNameByCall(call)) {
          v_memcpy.push_back(call);
        }
      }
    }

    string s_cmd = "commands";
    void *dummy_command_decl =
        ast->CreateVariableDecl(ast->GetTypeByString("__CMOST_COMMENT__"),
                                s_cmd, nullptr, kwrapper_body);
    ast->PrependChild(kwrapper_body, dummy_command_decl);
    void *cmd_ref = ast->CreateExp(V_SgPntrArrRefExp,
                                   ast->CreateVariableRef(dummy_command_decl),
                                   ast->CreateConst(count_wrapper));
    for (auto one_call : v_memcpy) {
      void *length_exp;
      void *array1 = nullptr;
      void *pntr1 = nullptr;
      vector<void *> idx1;
      int pointer_dim1;
      void *array2 = nullptr;
      void *pntr2 = nullptr;
      vector<void *> idx2;
      int pointer_dim2;
      ast->parse_memcpy(one_call, &length_exp, &array1, &pntr1, &idx1,
                        &pointer_dim1, &array2, &pntr2, &idx2, &pointer_dim2);

      if (array1 != nullptr) {
        string s_port = ast->_up(array1);
        if (s_port.substr(0, 4) == "__m_") {
          void *sg_var = ast->find_variable_by_name(s_port, kwrapper_body);
          s_port = s_port.substr(4);

          void *scope = ast->TraceUpToScope(one_call);
          void *stmt = ast->TraceUpToStatement(one_call);
          void *in_func_decl = ast->TraceUpToFuncDecl(one_call);
          string in_func_name = ast->GetFuncName(in_func_decl);
          bool is_from_wrapper = false;
          if (in_func_name.find("__merlinwrapper_") != std::string::npos) {
            is_from_wrapper = true;
          }

          if (mapPort2Pragma.find(s_port) != mapPort2Pragma.end()) {
            void *pragma = mapPort2Pragma[s_port];
            string s_type = mars_ir.get_pragma_attribute(pragma, "data_type");
            bool is_migrate =
                mars_ir.get_pragma_attribute(pragma, "migrate_mode") == "on";
            is_migrate |= (pcie_transfer_opt == "on");
            is_migrate &= is_from_wrapper;
            void *sg_type = ast->GetTypeByString(s_type);
            void *sg_pointer_type = ast->CreatePointerType(sg_type);
            assert(idx1.size() == 1 && pointer_dim1 == 0);

            void *sg_offset =
                ast->CreateExp(V_SgMultiplyOp, ast->CopyExp(idx1[0]),
                               ast->CreateExp(V_SgSizeOfOp, sg_type));

            string s_buf = "__" + s_kernel + "__" + s_port + "_buffer";
            string s_buf_id = mars_ir.get_pragma_attribute(pragma, "buffer_id");
            if (!s_buf_id.empty()) {
              s_buf = s_buf_id;
            }

            void *dummy_decl = ast->CreateVariableDecl(
                ast->GetTypeByString("__CMOST_COMMENT__"), s_buf, nullptr,
                scope);
            ast->InsertStmt(dummy_decl, stmt);

            vector<void *> args;
            args.push_back(ast->CreateVariableRef(dummy_decl));
            args.push_back(ast->CopyExp(cmd_ref));
            args.push_back(sg_offset);
            args.push_back(
                ast->CreateExp(V_SgCastExp,
                               ast->CopyExp(ast->RemoveCast(
                                   ast->GetFuncCallParam(one_call, 1))),
                               sg_pointer_type));
            args.push_back(ast->CopyExp(ast->GetFuncCallParam(one_call, 2)));

            void *new_call;
            string sub_string = "__merlin_write_buffer_";
            string merlin_string;
            void *dummy_write_decl;
            string s_event = "__event_" + s_buf;
            int flag = 0;
            if (in_func_name.find(sub_string) != std::string::npos) {
              flag = 1;
              merlin_string = "__MERLIN_EVENT_WRITE__ " + s_event;
              dummy_write_decl = ast->CreateVariableDecl(
                  "int", "__MERLIN_EVENT_WRITE__" + s_event, nullptr, scope);
              void *dummy_decl_event = ast->CreateVariableDecl(
                  ast->GetTypeByString("__CMOST_COMMENT__"), s_event, nullptr,
                  scope);
              args.push_back(ast->CreateExp(
                  V_SgAddressOfOp, ast->CreateVariableRef(dummy_decl_event)));
              ast->InsertStmt(dummy_decl_event, stmt);
              new_call = ast->CreateFuncCall("opencl_write_buffer_nb",
                                             ast->GetTypeByString("void"), args,
                                             scope);
            } else {
              new_call = ast->CreateFuncCall("opencl_write_buffer",
                                             ast->GetTypeByString("void"), args,
                                             scope);
            }

            void *new_stmt = ast->CreateStmt(V_SgExprStatement, new_call);

            // if migrate mode, not replace the memcpy
            // and add cl migrate after memcpy
            if (is_migrate) {
              string var = migrate_vector_cl_mem(s_kernel, true);
              string buffer_name = opencl_buffer(s_kernel, s_port, true);
              string add =
                  "\n\t" + var + "[" + var + "_p++]=" + buffer_name + ";\n";
              // cout << "add = " << add << endl;
              ast->AddDirectives(add, codegen->TraceUpToStatement(sg_var), 0);
            } else {
              ast->ReplaceStmt(stmt, new_stmt);
            }

            if (flag == 1) {
              ast->AddComment(merlin_string, new_stmt);
              ast->InsertStmt(dummy_write_decl, new_stmt);
            }
          }
        }
      }

      if (array2 != nullptr) {
        string s_port = ast->_up(array2);
        if (s_port.substr(0, 4) == "__m_") {
          void *sg_var = ast->find_variable_by_name(s_port, kwrapper_body);
          s_port = s_port.substr(4);

          void *scope = ast->TraceUpToScope(one_call);
          void *stmt = ast->TraceUpToStatement(one_call);
          void *in_func_decl = ast->TraceUpToFuncDecl(one_call);
          string in_func_name = ast->GetFuncName(in_func_decl);
          bool is_from_wrapper = false;
          if (in_func_name.find("__merlinwrapper_") != std::string::npos) {
            is_from_wrapper = true;
          }

          if (mapPort2Pragma.find(s_port) != mapPort2Pragma.end()) {
            void *pragma = mapPort2Pragma[s_port];
            string s_type = mars_ir.get_pragma_attribute(pragma, "data_type");
            bool is_migrate =
                mars_ir.get_pragma_attribute(pragma, "migrate_mode") == "on" &&
                mars_ir.get_pragma_attribute(pragma, "io") == "RO";
            is_migrate |= (pcie_transfer_opt == "on");
            is_migrate &= is_from_wrapper;
            void *sg_type = ast->GetTypeByString(s_type);
            void *sg_pointer_type = ast->CreatePointerType(sg_type);
            assert(idx2.size() == 1 && pointer_dim2 == 0);

            void *sg_offset =
                ast->CreateExp(V_SgMultiplyOp, ast->CopyExp(idx2[0]),
                               ast->CreateExp(V_SgSizeOfOp, sg_type));

            string s_buf = "__" + s_kernel + "__" + s_port + "_buffer";
            string s_buf_id = mars_ir.get_pragma_attribute(pragma, "buffer_id");
            if (!s_buf_id.empty()) {
              s_buf = s_buf_id;
            }
            void *dummy_decl = ast->CreateVariableDecl(
                ast->GetTypeByString("__CMOST_COMMENT__"), s_buf, nullptr,
                scope);
            ast->InsertStmt(dummy_decl, stmt);

            vector<void *> args;
            args.push_back(ast->CreateVariableRef(dummy_decl));
            args.push_back(ast->CopyExp(cmd_ref));
            args.push_back(sg_offset);
            args.push_back(
                ast->CreateExp(V_SgCastExp,
                               ast->CopyExp(ast->RemoveCast(
                                   ast->GetFuncCallParam(one_call, 0))),
                               sg_pointer_type));
            args.push_back(ast->CopyExp(ast->GetFuncCallParam(one_call, 2)));

            void *new_call;
            string sub_string = "__merlin_read_buffer_";
            string merlin_string;
            void *dummy_read_decl;
            string s_event = "__event_" + s_buf;
            int flag = 0;
            if (in_func_name.find(sub_string) != std::string::npos) {
              flag = 1;
              merlin_string = "__MERLIN_EVENT_READ__ " + s_event;
              dummy_read_decl = ast->CreateVariableDecl(
                  "int", "__MERLIN_EVENT_READ__" + s_event, nullptr, scope);
              void *dummy_decl_event = ast->CreateVariableDecl(
                  ast->GetTypeByString("__CMOST_COMMENT__"), s_event, nullptr,
                  scope);
              args.push_back(ast->CreateExp(
                  V_SgAddressOfOp, ast->CreateVariableRef(dummy_decl_event)));
              ast->InsertStmt(dummy_decl_event, stmt);
              new_call = ast->CreateFuncCall("opencl_read_buffer_nb",
                                             ast->GetTypeByString("void"), args,
                                             scope);
            } else {
              new_call = ast->CreateFuncCall("opencl_read_buffer",
                                             ast->GetTypeByString("void"), args,
                                             scope);
            }

            void *new_stmt = ast->CreateStmt(V_SgExprStatement, new_call);

            // if migrate mode, not replace the memcpy
            // and add cl migrate before memcpy
            if (is_migrate) {
              string var = migrate_vector_cl_mem(s_kernel, false);
              string buffer_name = opencl_buffer(s_kernel, s_port, true);
              string add =
                  "\n\t" + var + "[" + var + "_p++]=" + buffer_name + ";\n";
              // cout << "add = " << add << endl;
              ast->AddDirectives(add, codegen->TraceUpToStatement(sg_var), 0);
            } else {
              ast->ReplaceStmt(stmt, new_stmt);
            }

            if (flag == 1) {
              ast->AddComment(merlin_string, new_stmt);
              ast->InsertStmt(dummy_read_decl, new_stmt);
            }
          }
        }
      }
    }
    count_wrapper++;
  }

  //  remove __port internal variables
  for (auto it : map_kwrapper2pragma) {
    void *kwrapper = it.first;
    vector<void *> pragmas = it.second;
    void *kwrapper_body = ast->GetFuncBody(kwrapper);
    string s_kernel = ast->GetFuncName(kwrapper);
    s_kernel = s_kernel.substr(strlen(MERLIN_WRAPPER_PREFIX));

    map<string, void *> mapPort2Pragma;
    for (auto pragma : pragmas) {
      string s_port = mars_ir.get_pragma_attribute(pragma, "port");

      string s_local_var = "__m_" + s_port;
      void *sg_var = ast->find_variable_by_name(s_local_var, kwrapper_body);
      if (sg_var != nullptr) {
        if (pcie_transfer_opt != "on") {
          ast->SetTypetoVar(sg_var, ast->GetTypeByString("__CMOST_COMMENT__"));
        } else {
          // change array to pointer for alignment
          void *sg_type = ast->GetTypebyVar(sg_var);
          void *base_type = codegen->GetBaseType(sg_type, false);
          void *point_type = ast->CreatePointerType(base_type);
          ast->SetTypetoVar(sg_var, point_type);
        }
      }
    }
  }
  return 0;
}

int handle_single_assignment(CSageCodeGen *codegen, void *sg_array1,
                             const vector<void *> &sg_idx1, void *sg_array2,
                             const vector<void *> &sg_idx2, void *sg_length,
                             int type_size, void *func_call) {
  CMarsExpression length(sg_length, codegen, func_call);
  if (length.IsConstant() == 0) {
    return 0;
  }
  int64_t c_length = length.GetConstant() / type_size;
  if (c_length != 1) {
    return 0;
  }
  void *pntr1 = codegen->CreateVariableRef(sg_array1);
  for (auto sg_idx : sg_idx1) {
    pntr1 =
        codegen->CreateExp(V_SgPntrArrRefExp, pntr1, codegen->CopyExp(sg_idx));
  }

  void *pntr2 = codegen->CreateVariableRef(sg_array2);
  for (auto sg_idx : sg_idx2) {
    pntr2 =
        codegen->CreateExp(V_SgPntrArrRefExp, pntr2, codegen->CopyExp(sg_idx));
  }

  void *single_assign_expr = codegen->CreateExp(V_SgAssignOp, pntr1, pntr2);
  codegen->ReplaceExp(func_call, single_assign_expr);
  count_memcpy++;
  return 1;
}

string stmt_return_0() { return "\treturn 0;\n"; }
string head_opencl_kernel_buffer() {
  return "#include \"__merlin_opencl_kernel_buffer.h\"\n";
}
string head_opencl_if() { return "#include \"__merlin_opencl_if.h\"\n\n"; }

string opencl_kernel(string kernel_name) {
  return "__" + kernel_name + "_kernel";
}
string opencl_kernel_event(string kernel_name) {
  return "__event_" + kernel_name;
}
string opencl_buffer_event(string kernel_name, string port_name) {
  return "__event___" + kernel_name + "__" + port_name + "_buffer";
}

string decl_kernel(string kernel_name) {
  return "opencl_kernel " + opencl_kernel(kernel_name) + ";\n";
}
// string decl_kernel_migrate(string kernel_name) {
//  return "opencl_kernel " + opencl_kernel(kernel_name) + "_migrate;\n";
//}
string decl_kernel_event(string kernel_name) {
  return "cl_event " + opencl_kernel_event(kernel_name) + ";\n";
}
string decl_buffer(string kernel_name, string port_name,
                   bool is_migrate = false) {
  return "opencl_mem " + opencl_buffer(kernel_name, port_name, is_migrate) +
         ";\n";
}
string decl_buffer_event(string kernel_name, string port_name) {
  return "cl_event " + opencl_buffer_event(kernel_name, port_name) + ";\n";
}
string decl_buffer_ext(string kernel_name, string port_name,
                       bool is_migrate = false) {
  return "opencl_mem_ext ext_" +
         opencl_buffer(kernel_name, port_name, is_migrate) + ";\n";
}

string func_opencl_init_kernel_buffer() {
  return "int opencl_init_kernel_buffer()";
}
string func_merlin_init(string name) {
  return "int __merlin_init_" + name + "()";
}
string func_merlin_release(string name) {
  return "int __merlin_release_" + name + "()";
}
string func_release_kernel_buffer() {
  return "int __merlin_release_kernel_buffer()";
}
string func_wait_kernel(string kernel_name) {
  return "int __merlin_wait_kernel_" + kernel_name + "()";
}
string func_wait_write(string kernel_name) {
  return "int __merlin_wait_write_" + kernel_name + "()";
}
string func_wait_read(string kernel_name) {
  return "int __merlin_wait_read_" + kernel_name + "()";
}
string func_create_kernel(string kernel_name) {
  string str;
  str += "\t#pragma ACCEL debug printf(\"[Merlin Info] "
         "Start create kernel for %s....\\n\", \"" +
         kernel_name + "\");\n";
  str += "\t#pragma ACCEL debug fflush(stdout);\n";
  str += "\topencl_create_kernel(&" + opencl_kernel(kernel_name) +
         ", (char *)\"" + legalize_kernel_name(kernel_name) + "\");\n";
  str += "\t#pragma ACCEL debug printf(\"[Merlin Info] "
         "Successful create kernel for %s....\\n\", \"" +
         kernel_name + "\");\n";
  str += "\t#pragma ACCEL debug fflush(stdout);\n";
  return str;
}
string func_create_buffer(string kernel_name, string port_name,
                          string data_size_in_byte, string impl_tool,
                          string bank, bool is_task, bool is_migrate = false,
                          string var_name = "") {
  string str;
  if (var_name == "")
    var_name = port_name;

  str += "\t#pragma ACCEL debug printf(\"[Merlin Info] Start create buffer for "
         "%s....\\n\", \"" +
         port_name + "\");\n";
  str += "\t#pragma ACCEL debug fflush(stdout);\n";
  if (impl_tool == "sdaccel" && bank != "nullptr") {
    str += "\text_" + opencl_buffer(kernel_name, port_name) +
           ".flags = XCL_MEM_DDR_BANK" + bank + ";\n";
    str +=
        "\text_" + opencl_buffer(kernel_name, port_name) + ".obj   = NULL;\n";
    str += "\text_" + opencl_buffer(kernel_name, port_name) + ".param = 0;\n";
    str += "\topencl_create_ext_buffer(&" +
           opencl_buffer(kernel_name, port_name) + ",&ext_" +
           opencl_buffer(kernel_name, port_name) + ", (int64_t)" +
           data_size_in_byte + ", 2);\n";
    if (!is_task) {
      transform(port_name.begin(), port_name.end(), port_name.begin(),
                ::toupper);
      str += "\t// --xp misc:map_connect=add.kernel." + kernel_name +
             "_1.M_AXI_" + port_name + ".core.OCL_REGION_0.M0" + bank +
             "_AXI\n";
      str += "\t// --sp " + kernel_name + "_1.m_axi_bank" + bank + ":DDR[" +
             bank + "]\n";
    }
  } else if (/*impl_tool == "sdaccel" && */ is_migrate) {
  } else {
    str += "\topencl_create_buffer(&" + opencl_buffer(kernel_name, port_name) +
           ", (int64_t)" + data_size_in_byte + ", 2, NULL);\n";
  }
  str += "\t#pragma ACCEL debug printf(\"[Merlin Info] Successful create "
         "buffer for %s....\\n\", \"" +
         port_name + "\");\n";
  str += "\t#pragma ACCEL debug fflush(stdout);\n";
  return str;
}
string func_release_kernel(string kernel_name) {
  string str;
  str += "\tif(" + opencl_kernel(kernel_name) + ") {\n";
  str += "\t\topencl_release_kernel(" + opencl_kernel(kernel_name) + ");\n";
  str += "\t}\n";
  return str;
}
string func_release_kernel_event(string kernel_name) {
  string str;
  //  str += "#ifdef __MERLIN_SDACCEL\n";
  str += "\tif(" + opencl_kernel_event(kernel_name) + ") {\n";
  str +=
      "\t\topencl_release_event(" + opencl_kernel_event(kernel_name) + ");\n";
  str += "\t}\n";
  //  str += "#endif\n";
  return str;
}
string func_release_buffer(string kernel_name, string port_name,
                           bool is_migrate = false) {
  string str;
  str += "\tif(" + opencl_buffer(kernel_name, port_name, is_migrate) + ") {\n";
  str += "\t\topencl_release_mem_object(" +
         opencl_buffer(kernel_name, port_name, is_migrate) + ");\n";
  str += "\t}\n";
  return str;
}
string func_wait_kernel_event(string kernel_name) {
  return "\topencl_wait_event(" + opencl_kernel_event(kernel_name) + ");\n";
}
string func_wait_buffer_event(string kernel_name, string port_name) {
  return "\t\topencl_wait_event(" +
         opencl_buffer_event(kernel_name, port_name) + ");\n";
}

string func_migrate_call(string kernel_name, int queue_index, int num_cl_mem,
                         bool is_input) {
  string ret = "";
  string in = "in";
  string out = "out";
  string cl_list_name = migrate_vector_cl_mem(kernel_name, is_input);
  string cl_list_name_p = cl_list_name + "_p";
  ret += "\t#pragma ACCEL debug printf(\"[Merlin Info] Start migrate buffer ";
  ret += is_input ? in : out;
  ret += ".\\n\");\n";
  ret += "\t#pragma ACCEL debug fflush(stdout);\n";
  ret += "\topencl_migrate_buffer(" + cl_list_name_p + ", " + cl_list_name +
         ", commands[" + std::to_string(queue_index) + "], " +
         std::to_string(is_input ? true : false) + ");\n";
  ret += "\t#pragma ACCEL debug "
         "printf(\"[Merlin Info] Finish migrate buffer.\\n\");\n";
  return ret;
}
//  string func_release_buffer_event(string kernel_name, string port_name) {
//  return "\t\topencl_release_event(" + opencl_buffer_event(kernel_name,
//  port_name) + ");\n"; }
string func_set_kernel_arg(string kernel_name, int index, string data_type,
                           string name) {
  return "\topencl_set_kernel_arg(" + opencl_kernel(kernel_name) + ", " +
         std::to_string(index) + ", sizeof(" + data_type + "), &" + name +
         ");\n";
}
string func_start_exec_kernel(string kernel_name, int queue_index) {
  string str;
  str += "\tsize_t __gid[1];\n";
  str += "\t__gid[0] = 1;\n";
  str += "\topencl_enqueue_kernel(" + opencl_kernel(kernel_name) +
         ", commands[" + std::to_string(queue_index) + "], 1, __gid, &" +
         opencl_kernel_event(kernel_name) + ");\n";
  return str;
}
string func_finish_exec_kernel(string kernel_name) {
  return "\tclWaitForEvents(1, &" + opencl_kernel_event(kernel_name) + ");\n";
}

string info_start_set_kernel_arg(string kernel_name) {
  string str;
  str += "\t#pragma ACCEL debug printf(\"[Merlin Info] Start "
         "set kernel %s argument....\\n\", \"" +
         kernel_name + "\");\n";
  str += "\t#pragma ACCEL debug fflush(stdout);\n";
  return str;
}
string info_finish_set_kernel_arg(string kernel_name) {
  string str;
  str += "\t#pragma ACCEL debug printf(\"[Merlin Info] "
         "Finish set kernel %s argument....\\n\", \"" +
         kernel_name + "\");\n";
  str += "\t#pragma ACCEL debug fflush(stdout);\n";
  return str;
}
string info_start_exec_kernel(string kernel_name) {
  string str;
  str += "\t#pragma ACCEL debug printf(\"[Merlin Info] Start "
         "execute kernel %s....\\n\", \"" +
         kernel_name + "\");\n";
  str += "\t#pragma ACCEL debug fflush(stdout);\n";
  return str;
}
string info_finish_exec_kernel(string kernel_name) {
  string str;
  str += "\t#pragma ACCEL debug printf(\"[Merlin Info] "
         "Finish execute kernel %s....\\n\", \"" +
         kernel_name + "\");\n";
  str += "\t#pragma ACCEL debug fflush(stdout);\n";
  return str;
}
string get_attribute_from_wrapper_pragma(void *sg_port, string str_attr,
                                         CSageCodeGen *ast,
                                         CMarsIrV2 *mars_ir) {
  void *kernel = ast->TraceUpToFuncDecl(sg_port);
  string func_name = ast->GetFuncName(kernel);
  if (!mars_ir->is_kernel(kernel)) {
    return "";
  }

  //  func_name = get_wrapper_name_by_kernel(ast, kernel);
  func_name = ast->get_wrapper_name_by_kernel(kernel);
  void *wrapper = ast->GetFuncDeclByName(func_name);
  if (wrapper == nullptr) {
    return "";
  }
  if (ast->GetFuncBody(wrapper) == nullptr) {
    return "";
  }

  vector<void *> vec_pragmas;
  ast->GetNodesByType(wrapper, "preorder", "SgPragmaDeclaration", &vec_pragmas);

  for (auto &pragma : vec_pragmas) {
    string s_value = mars_ir->get_pragma_attribute(pragma, str_attr);
    string s_port = mars_ir->get_pragma_attribute(pragma, "port");
    string s_intf = mars_ir->get_pragma_attribute(pragma, "wrapper");
    boost::algorithm::to_lower(s_intf);
    if (s_port == ast->_up(sg_port) && s_intf == "wrapper") {
      return s_value;
    }
  }

  return ast->_up(sg_port);
}

string get_org_name_from_wrapper_pragma(void *sg_port, CSageCodeGen *ast,
                                        CMarsIrV2 *mars_ir) {
  void *kernel = ast->TraceUpToFuncDecl(sg_port);
  string func_name = ast->GetFuncName(kernel);
  if (!mars_ir->is_kernel(kernel)) {
    return "";
  }

  //  func_name = get_wrapper_name_by_kernel(ast, kernel);
  func_name = ast->get_wrapper_name_by_kernel(kernel);
  void *wrapper = ast->GetFuncDeclByName(func_name);
  if (wrapper == nullptr) {
    return "";
  }
  if (ast->GetFuncBody(wrapper) == nullptr) {
    return "";
  }

  vector<void *> vec_pragmas;
  ast->GetNodesByType(wrapper, "preorder", "SgPragmaDeclaration", &vec_pragmas);

  for (auto &pragma : vec_pragmas) {
    string s_var = mars_ir->get_pragma_attribute(pragma, "variable");
    string s_port = mars_ir->get_pragma_attribute(pragma, "port");
    string s_intf = mars_ir->get_pragma_attribute(pragma, "wrapper");
    boost::algorithm::to_lower(s_intf);
    if (s_port == ast->_up(sg_port) && s_intf == "wrapper") {
      return s_var;
    }
  }

  return ast->_up(sg_port);
}

string get_org_data_type_from_wrapper_pragma(void *sg_port, CSageCodeGen *ast,
                                             CMarsIrV2 *mars_ir) {
  void *kernel = ast->TraceUpToFuncDecl(sg_port);
  string func_name = ast->GetFuncName(kernel);
  if (!mars_ir->is_kernel(kernel)) {
    return "";
  }

  //  func_name = get_wrapper_name_by_kernel(ast, kernel);
  func_name = ast->get_wrapper_name_by_kernel(kernel);
  void *wrapper = ast->GetFuncDeclByName(func_name);
  if (wrapper == nullptr) {
    return "";
  }
  if (ast->GetFuncBody(wrapper) == nullptr) {
    return "";
  }

  vector<void *> vec_pragmas;
  ast->GetNodesByType(wrapper, "preorder", "SgPragmaDeclaration", &vec_pragmas);

  for (auto &pragma : vec_pragmas) {
    string s_port = mars_ir->get_pragma_attribute(pragma, "port");
    string s_data_type = mars_ir->get_pragma_attribute(pragma, "data_type");
    string s_intf = mars_ir->get_pragma_attribute(pragma, "wrapper");
    boost::algorithm::to_lower(s_intf);
    if (s_port == ast->_up(sg_port) && s_intf == "wrapper") {
      return s_data_type;
    }
  }

  return "";
}

void *map_kernel_from_subkernel_to_topkernel(
    CMarsAST_IF *ast, void *sg_sub_kernel,
    const map<void *, vector<void *>> &kernel_subkernel) {
  for (auto one_info : kernel_subkernel) {
    void *top_kernel = one_info.first;
    vector<void *> sub_kernels = one_info.second;

    for (auto one_sub : sub_kernels) {
      if (one_sub == sg_sub_kernel) {
        return top_kernel;
      }
    }
  }
  return nullptr;
}
void *map_port_from_subkernel_to_topkernel(
    CMarsAST_IF *ast, void *sg_port_,
    const map<void *, vector<void *>> &kernel_subkernel) {
  void *ret_port = nullptr;

  void *sub_kernel = ast->TraceUpToFuncDecl(sg_port_);
  if (sub_kernel == nullptr) {
    return nullptr;
  }

  string param_name = ast->_up(sg_port_);

  void *top_kernel =
      map_kernel_from_subkernel_to_topkernel(ast, sub_kernel, kernel_subkernel);

  for (auto param : ast->GetFuncParams(top_kernel)) {
    if (param_name == ast->_up(param)) {
      ret_port = param;
      break;
    }
  }
  return ret_port;
}

//  generate map for top kernel and it's sub kernels
map<void *, vector<void *>> GetKernelMap(CSageCodeGen *codegen, void *pTopFunc,
                                         const CInputOptions &options) {
  CMarsAST_IF *ast = codegen;
  printf("    Enter getKernelMap...\n");

  //  int ret = 0;
  CMarsIrV2 mars_ir;
  mars_ir.build_mars_ir(codegen, pTopFunc);
  vector<void *> sub_kernels;
  map<void *, vector<void *>> kernel_subkernel;

  vector<void *> top_kernels = mars_ir.get_top_kernels();
  for (auto top_kernel : top_kernels) {
    if (kernel_subkernel.find(top_kernel) == kernel_subkernel.end()) {
      vector<void *> empty;
      empty.push_back(top_kernel);
      kernel_subkernel.insert(pair<void *, vector<void *>>(top_kernel, empty));
    }
  }

  //  get all the kernel pragmas and parse the parent field
  {
    vector<void *> all_pragmas;
    ast->GetNodesByType(ast->GetProject(), "preorder", "SgPragmaDeclaration",
                        &all_pragmas);

    for (auto one_pragma : all_pragmas) {
      string sub_task = mars_ir.get_pragma_attribute(one_pragma, "sub_kernel");
      boost::algorithm::to_lower(sub_task);
      if (sub_task != "sub_kernel") {
        continue;
      }

      void *sub_kernel_call = ast->GetNextStmt(one_pragma);
      if (sub_kernel_call == nullptr) {
        continue;
      }
      if (ast->IsExprStatement(sub_kernel_call) == 0) {
        continue;
      }
      if (ast->IsFunctionCall(ast->GetExprFromStmt(sub_kernel_call)) == 0) {
        continue;
      }

      void *sub_kernel_decl =
          ast->GetFuncDeclByCall(ast->GetExprFromStmt(sub_kernel_call));

      if (ast->IsFunctionDeclaration(sub_kernel_decl) == 0) {
        continue;
      }
      string parent_name = mars_ir.get_pragma_attribute(one_pragma, "parent");

      void *parent_kernel = nullptr;
      for (auto one_kernel : top_kernels) {
        if (parent_name == ast->GetFuncName(one_kernel)) {
          parent_kernel = one_kernel;
        }
      }
      if (parent_kernel == nullptr) {
        continue;
      }

      kernel_subkernel[parent_kernel].push_back(sub_kernel_decl);

      cout << " -- Pragma: " << ast->_p(one_pragma) << endl;
      cout << "    Sub-kernel: " << ast->GetFuncName(sub_kernel_decl)
           << "Top-kernel: " << ast->GetFuncName(parent_kernel) << endl;
    }
  }

  return kernel_subkernel;
}

//  generate kernel file list json file
//  this include both sub-kernel and auto kernel
void generate_kernel_file_list(CSageCodeGen *codegen, void *pTopFunc,
                               map<string, map<string, string>> task_kernel,
                               map<void *, vector<void *>> kernel_subkernel,
                               map<void *, vector<void *>> kernel_autokernel) {
  cout << "Enter generate_kernel_file_list." << endl;
  vector<string> vec_tasks;
  if (!task_kernel.empty()) {
    map<string, map<string, string>>::iterator iter0;
    for (iter0 = task_kernel.begin(); iter0 != task_kernel.end(); iter0++) {
      string task_name = iter0->first;
      vec_tasks.push_back(task_name);
    }
  }
  map<string, map<string, string>> kernel_subkernel_type;
  map<string, map<string, string>> kernel_subkernel_file;
  string output;
  for (auto one_kernel_info : kernel_subkernel) {
    string top_kernel_name = codegen->GetFuncName(one_kernel_info.first);
    if (std::find(vec_tasks.begin(), vec_tasks.end(), top_kernel_name) !=
        vec_tasks.end()) {
      continue;
    }
    map<string, string> subkernel_type;
    map<string, string> subkernel_file;
    vector<void *> sub_kernels = one_kernel_info.second;
    for (auto sub : sub_kernels) {
      string kernel_name = codegen->GetFuncName(sub);
      string file_name = codegen->get_file(sub);
      int number = codegen->GetFuncParamNum(sub);
      if (top_kernel_name != kernel_name) {
        legalize_kernel(codegen, sub);
      }
      kernel_name = legalize_kernel_name(kernel_name);
      output += "//" + kernel_name + "=" + file_name + "\n";
      if (number == 0) {
        subkernel_type.insert(pair<string, string>(kernel_name, "autokernel"));
      } else {
        subkernel_type.insert(pair<string, string>(kernel_name, "subkernel"));
      }
      subkernel_file.insert(pair<string, string>(kernel_name, file_name));
    }
    top_kernel_name = legalize_kernel_name(top_kernel_name);
    kernel_subkernel_type.insert(
        pair<string, map<string, string>>(top_kernel_name, subkernel_type));
    kernel_subkernel_file.insert(
        pair<string, map<string, string>>(top_kernel_name, subkernel_file));
  }
  for (auto one_kernel_info : kernel_autokernel) {
    void *top_kernel = one_kernel_info.first;
    string top_kernel_name = codegen->GetFuncName(top_kernel);
    top_kernel_name = legalize_kernel_name(top_kernel_name);
    map<string, string> subkernel_type;
    map<string, string> subkernel_file;
    vector<void *> sub_kernels = one_kernel_info.second;
    for (auto sub : sub_kernels) {
      string kernel_name = codegen->GetFuncName(sub);
      string file_name = codegen->get_file(sub);
      kernel_name = legalize_kernel_name(kernel_name);
      legalize_kernel(codegen, sub);
      subkernel_type.insert(pair<string, string>(kernel_name, "autokernel"));
      subkernel_file.insert(pair<string, string>(kernel_name, file_name));
    }
    if (kernel_autokernel.find(top_kernel) == kernel_autokernel.end()) {
      kernel_subkernel_type.insert(
          pair<string, map<string, string>>(top_kernel_name, subkernel_type));
      kernel_subkernel_file.insert(
          pair<string, map<string, string>>(top_kernel_name, subkernel_file));
    } else {
      kernel_subkernel_type[top_kernel_name].insert(subkernel_type.begin(),
                                                    subkernel_type.end());
      kernel_subkernel_file[top_kernel_name].insert(subkernel_file.begin(),
                                                    subkernel_file.end());
    }
  }
  for (auto one_kernel_info : kernel_subkernel) {
    legalize_kernel(codegen, one_kernel_info.first);
  }
  writeInterReport(KERNEL_LIST_JSON, kernel_subkernel_type);
  writeInterReport(KERNEL_FILE_JSON, kernel_subkernel_file);
  string output_file_name = "__merlin_kernel_list.h";
  std::ofstream out(output_file_name);
  out << output;
  out.close();
}

//  =======================================================//
//  data attributes collection
//
//  kernel_attributes : kernel-key-value
//      -------------------------------------------------------------------------------------
//      kernel          key             value           from
//      -------------------------------------------------------------------------------------
//      kernel_name     device      fpga/gpu/cpu...     kernel pragma
//                      die         depend on platform  kernel_pragma   (TODO:
//                      code is here, to test) return_type int/char...
//                      kernel_decl file_name   a.cpp b.cpp c.cpp   json
//                      file(ignored here, will generated in preprocess)
//      -------------------------------------------------------------------------------------
//
//  kernel_port_attributes : kernel-port-key-value
//      -------------------------------------------------------------------------------------
//      kernel          port        key             value               from
//      -------------------------------------------------------------------------------------
//      kernel_name     port_name   port_index      0/1/2... kernel_decl
//                                  actual_name     name                ...
//                                  format_name     name                ...
//                                  data_type       int/char...         ...
//                                  port_type       scalar/array/pointer...
//                                  volatile        yes/no              ...
//                                  read_valid      0/1                 ...
//                                  read_length     0/100/1000...       ...
//                                  read_offset     0/100/1000...       ...
//                                  wtite_valid     0/1                 ...
//                                  wtite_length    0/100/1000...       ...
//                                  wtite_offset    0/100/1000...       ...
//                                  bitwidth        512/32... interface_pragma
//                                  depth           100/var....         ...
//                                  max_depth       100/1000...         ...
//                                  size_in_byte    100/1000...         ...
//                                  bank            nullptr/0/1/2/3        ...
//                                  (TODO: code is here, to test)
//      -------------------------------------------------------------------------------------
//
//  =======================================================/n
int data_parser(
    CSageCodeGen *codegen, void *pTopFunc, const CInputOptions &options,
    map<string, map<string, string>> *task_kernel,
    map<string, map<string, string>> *kernel_attributes,
    map<string, map<string, map<string, string>>> *kernel_port_attributes,
    map<void *, vector<void *>> *kernel_subkernel,
    map<void *, vector<void *>> map_kernel_spawn_kernel) {
  *kernel_subkernel = GetKernelMap(codegen, pTopFunc, options);
  CMarsAST_IF *ast = codegen;
  printf("    Enter data parser...\n");

  string file = TASK_KERNEL_JSON;
  std::ifstream ifile(file);
  if (ifile) {
    printf("file exist\n");
    readInterReport(file, task_kernel);
  }
  pure_kernel_flow = "on" == options.get_option_key_value("-a", "pure_kernel");
  CMarsIrV2 mars_ir;
  mars_ir.build_mars_ir(codegen, pTopFunc);
  vector<void *> kernel_list = mars_ir.get_top_kernels();

  // HH: add spawn kernels to kernel_subkernel
  {
    for (auto one_kernel_info : map_kernel_spawn_kernel) {
      void *top_kernel = one_kernel_info.first;
      vector<void *> sub_kernels = one_kernel_info.second;
      for (auto sub : sub_kernels) {
        cout << "top_kernel = " << codegen->UnparseToString(top_kernel)
             << ", sub_kernel = " << codegen->UnparseToString(sub) << endl;
      }
    }
    for (auto one_kernel_info : *kernel_subkernel) {
      void *top_kernel = one_kernel_info.first;
      vector<void *> sub_kernels = one_kernel_info.second;
      if (map_kernel_spawn_kernel.count(top_kernel) == 0) {
        continue;
      }
      map<void *, vector<void *>>::iterator it;
      it = map_kernel_spawn_kernel.find(top_kernel);
      void *top_spawn_kernel = it->first;
      vector<void *> spawn_kernels = it->second;
      if (it != map_kernel_spawn_kernel.end()) {
        for (auto one_spawn_kernel : spawn_kernels) {
          cout << " add spawn kernel: "
               << ast->UnparseToString(one_spawn_kernel) << endl;
          sub_kernels.push_back(one_spawn_kernel);
        }
        (*kernel_subkernel)[top_kernel] = sub_kernels;
      } else {
        kernel_subkernel->insert(
            pair<void *, vector<void *>>(top_spawn_kernel, spawn_kernels));
      }
    }
  }

  //  ZP: fix the issue that sub_kernel is not in get_top_kernels();
  {
    for (auto one_kernel_info : *kernel_subkernel) {
      void *top_kernel = one_kernel_info.first;
      vector<void *> sub_kernels = one_kernel_info.second;

      for (auto sub : sub_kernels) {
        if (sub != top_kernel) {
          kernel_list.push_back(sub);
        }
      }
    }
  }

  for (auto kernel_ : kernel_list) {
    void *kernel = map_kernel_from_subkernel_to_topkernel(codegen, kernel_,
                                                          *kernel_subkernel);
    if (kernel == nullptr) {
      continue;
    }

    cout << "-- Kernel: " << ast->_p(kernel) << endl;
    map<string, string> one_map;
    void *ret_type = ast->GetFuncReturnType(kernel);
    string str_type = ast->GetStringByType(ret_type);
    string kernel_name = ast->GetFuncName(kernel);

    one_map.insert(pair<string, string>("return_type", str_type));
    vector<void *> vec_calls;
    void *wrapper_call = nullptr;
    codegen->GetFuncCallsByName(kernel_name, pTopFunc, &vec_calls);
    for (auto call : vec_calls) {
      string file_name = codegen->get_file(call);
      if (file_name.find("__merlinwrapper_") != std::string::npos) {
        wrapper_call = call;
        continue;
      }
    }
    string die_idx;
    string device_type;
    // if (wrapper_call == nullptr && !pure_kernel_flow) {
    //   dump_critical_message(CLGEN_ERROR_3(
    //       "Cannot find a kernel call in file __merlinwrapper_xxx.\n"));
    //   throw std::exception();
    // }
    if (wrapper_call != nullptr) {
      cout << "-- Kernel call: " << ast->_p(wrapper_call) << endl;

      void *kernel_pragma =
          ast->GetPreviousStmt(ast->TraceUpToStatement(wrapper_call));
      //  void * kernel_pragma = ast->GetPreviousStmt(kernel);
      cout << "-- Kernel pragma: " << ast->_p(kernel_pragma) << endl;
      if (ast->IsPragmaDecl(kernel_pragma) != 0) {
        die_idx = mars_ir.get_pragma_attribute(kernel_pragma, "die");
        device_type = mars_ir.get_pragma_attribute(kernel_pragma, "device");
      }
    }
    if (device_type.empty()) {
      device_type = "fpga";
    }
    one_map.insert(pair<string, string>("device", device_type));
    if (!die_idx.empty()) {
      one_map.insert(pair<string, string>("die", die_idx));
    }

    kernel_attributes->insert(
        pair<string, map<string, string>>(kernel_name, one_map));
  }
  //  Postproc for kernel_attribute
  //   - Copy the top kernel attributes to sub_kernels
  {
    for (auto one_info : *kernel_subkernel) {
      void *top_kernel = one_info.first;
      vector<void *> sub_kernels = one_info.second;

      string top_kernel_name = codegen->GetFuncName(top_kernel);
      if (kernel_attributes->find(top_kernel_name) ==
          (*kernel_attributes).end()) {
        continue;
      }
      map<string, string> one_map = (*kernel_attributes)[top_kernel_name];

      for (auto sub : sub_kernels) {
        string kernel_name = codegen->GetFuncName(sub);
        if (sub != top_kernel) {
          kernel_attributes->insert(
              pair<string, map<string, string>>(kernel_name, one_map));
        }
      }
    }
  }

  for (auto kernel : kernel_list) {
    cout << "-- Port: " << ast->_p(kernel) << endl;
    map<string, map<string, string>> one_kernel;
    string kernel_name = ast->GetFuncName(kernel);

    auto port_list = ast->GetFuncParams(kernel);
    int port_idx = 0;
    for (auto sg_port_ : port_list) {
      void *sg_port = map_port_from_subkernel_to_topkernel(codegen, sg_port_,
                                                           *kernel_subkernel);
      if (sg_port == nullptr) {
        continue;
      }

      map<string, string> one_port;

      string str_port_idx = my_itoa(port_idx++);
      one_port.insert(pair<string, string>("port_idx", str_port_idx));

      string str_port_name = ast->_up(sg_port);
      one_port.insert(pair<string, string>("actual_name", str_port_name));
      one_port.insert(pair<string, string>("formal_name", str_port_name));
      string sub_string = str_port_name.substr(0, 7);
      string org_name =
          get_org_name_from_wrapper_pragma(sg_port, ast, &mars_ir);
      string org_data_type =
          get_org_data_type_from_wrapper_pragma(sg_port, ast, &mars_ir);
      one_port.insert(pair<string, string>("org_name", org_name));

      void *sg_type = ast->GetTypebyVar(sg_port);
      void *sg_type_org = sg_type;
      sg_type = ast->GetBaseType(sg_type, false);
      string str_type = ast->GetStringByType(sg_type);
      one_port.insert(pair<string, string>("data_type", str_type));
      if (org_data_type.empty()) {
        org_data_type = str_type;
      }
      one_port.insert(pair<string, string>("orig_data_type", org_data_type));
      string port_type = "unknown";
      if (ast->IsScalarType(sg_type_org) != 0 ||
          ast->IsStructureWithAlignedScalarData(sg_type_org) != 0) {
        port_type = "scalar";
      } else if (ast->IsArrayType(sg_type_org) != 0) {
        port_type = "array";
      } else if (ast->IsPointerType(sg_type_org) != 0) {
        port_type = "pointer";
      }
      one_port.insert(pair<string, string>("port_type", port_type));

      string volatile_type = "no";
      if (ast->IsVolatileType(sg_type_org) != 0) {
        volatile_type = "yes";
      }
      one_port.insert(pair<string, string>("volatile", volatile_type));

      if (port_type != "scalar") {
        void *sg_scope = ast->GetFuncBody(kernel);
        ArrayRangeAnalysis mar(sg_port, ast, sg_scope, sg_scope, false, true);
        int is_write = mar.has_write();
        int is_read = mar.has_read();

        one_port.insert(pair<string, string>("read_valid", my_itoa(is_read)));
        one_port.insert(pair<string, string>("write_valid", my_itoa(is_write)));

        int cont_flag = 0;
        int array_dim_size = 0;
        vector<size_t> dim_sizes =
            mars_ir.get_array_dimension(sg_port, &cont_flag);
        if (!dim_sizes.empty()) {
          array_dim_size = dim_sizes[0];
        }
        size_t total_size = 1;
        for (auto one_size : dim_sizes) {
          total_size *= one_size;
        }

        if (is_read != 0) {
          CMarsRangeExpr mr = mar.GetFlattenRangeExprRead();
          string range = mr.print();
          int read_start;
          int read_end;
          if (mr.is_const_lb() != 0) {
            read_start = mr.get_const_lb();
          } else {
            read_start = 0;
          }
          if (mr.is_const_ub() != 0) {
            read_end = mr.get_const_ub();
          } else {
            read_end = array_dim_size - 1;
          }

          if (read_start < 0 || read_end >= array_dim_size) {
            string msg = "Finding a possible out-of-bound access: \n  range=[" +
                         range + "] task=" + kernel_name +
                         " argument=" + codegen->UnparseToString(sg_port) +
                         "\n";
            dump_critical_message("PROCS", "WARNING", msg, 201, 1);
          }

          one_port.insert(
              pair<string, string>("read_offset", my_itoa(read_start)));
          one_port.insert(pair<string, string>(
              "read_length", my_itoa(read_end - read_start + 1)));
        }
        if (is_write != 0) {
          CMarsRangeExpr mr = mar.GetFlattenRangeExprWrite();
          string range = mr.print();
          int write_start;
          int write_end;
          if (mr.is_const_lb() != 0) {
            write_start = mr.get_const_lb();
          } else {
            write_start = 0;
          }
          if (mr.is_const_ub() != 0) {
            write_end = mr.get_const_ub();
          } else {
            write_end = array_dim_size - 1;
          }

          if (write_start < 0 || write_end >= array_dim_size) {
            string msg = "Finding a possible out-of-bound access: \n  range=[" +
                         range + "] task=" + kernel_name +
                         " argument=" + codegen->UnparseToString(sg_port) +
                         "\n";
            dump_critical_message("PROCS", "WARNING", msg, 201, 1);
          }

          one_port.insert(
              pair<string, string>("write_offset", my_itoa(write_start)));
          one_port.insert(pair<string, string>(
              "write_length", my_itoa(write_end - write_start + 1)));
        }

        int bitwidth = ast->get_bitwidth_from_type(sg_type, false);
        one_port.insert(pair<string, string>("bitwidth", my_itoa(bitwidth)));
        int org_bitwidth =
            ast->get_bitwidth_from_type_str(org_data_type, false);
        one_port.insert(
            pair<string, string>("org_bitwidth", my_itoa(org_bitwidth)));

        vector<string> depths =
            ast->GetUniqueInterfaceAttribute(sg_port, "depth", &mars_ir);
        vector<string> max_depths =
            ast->GetUniqueInterfaceAttribute(sg_port, "max_depth", &mars_ir);
        one_port.insert(pair<string, string>("depth", str_merge(',', depths)));
        one_port.insert(
            pair<string, string>("max_depth", str_merge(',', max_depths)));
        string total_depth = std::to_string(
            codegen->get_sizeof_arbitrary_precision_integer(org_bitwidth));
        for (auto one_depth : max_depths) {
          total_depth += "*" + one_depth;
        }
        one_port.insert(pair<string, string>("data_size_in_byte", total_depth));

        vector<string> banks =
            ast->GetUniqueInterfaceAttribute(sg_port, "bank", &mars_ir);
        if (!banks.empty()) {
          one_port.insert(pair<string, string>("bank", banks[0]));
        } else {
          one_port.insert(pair<string, string>("bank", "nullptr"));
        }

        // vector<string> migrate_mode =
        //    ast->GetUniqueInterfaceAttribute(sg_port, "migrate_mode",
        //    &mars_ir);
        string migrate_mode = get_attribute_from_wrapper_pragma(
            sg_port, "migrate_mode", ast, &mars_ir);
        if (migrate_mode != "")
          one_port.insert(pair<string, string>("migrate_mode", migrate_mode));

        string var_name = get_attribute_from_wrapper_pragma(sg_port, "VARIABLE",
                                                            ast, &mars_ir);
        if (var_name != "") {
          one_port.insert(pair<string, string>("VARIABLE", var_name));
        }

        string io =
            get_attribute_from_wrapper_pragma(sg_port, "io", ast, &mars_ir);
        if (io != "")
          one_port.insert(pair<string, string>("io", io));

        string bundle_name = CMOST_explict_bundle;
        vector<string> bundle =
            ast->GetUniqueInterfaceAttribute(sg_port, bundle_name, &mars_ir);
        for (auto one_bundle : bundle) {
          one_port.insert(pair<string, string>("bundle", bundle_name));
          break;
        }
        string org_bundle_name = "";
        vector<string> vec_org_bundle =
            ast->GetUniqueInterfaceAttribute(sg_port, "bundle", &mars_ir);
        for (auto one_bundle : vec_org_bundle) {
          one_port.insert(pair<string, string>("org_bundle", one_bundle));
          break;
        }
      }

      one_kernel.insert(
          pair<string, map<string, string>>(str_port_name, one_port));
    }

    kernel_port_attributes->insert(
        pair<string, map<string, map<string, string>>>(kernel_name,
                                                       one_kernel));
  }

#if _DEBUG_DATA_PARSER
  {
    cout << "\n ************* " << endl;
    cout << "Data Parser Output (Kernels):" << endl;
    for (auto one_kernel : *kernel_attributes) {
      cout << "--kernel (" << one_kernel.first << "): ";
      for (auto one_pair : one_kernel.second) {
        cout << one_pair.first << "=" << one_pair.second << " ";
      }
      cout << endl;
    }
  }

  {
    cout << "\n ************* " << endl;
    cout << "Data Parser Output (Ports):" << endl;
    for (auto one_kernel : *kernel_port_attributes) {
      cout << "--kernel (" << one_kernel.first << "): " << endl;
      for (auto one_port : one_kernel.second) {
        cout << "  port (" << one_port.first << "): ";
        for (auto one_pair : one_port.second) {
          cout << one_pair.first << "=" << one_pair.second << " ";
        }
        cout << endl;
      }
      cout << endl;
    }
  }

#endif
  return 1;
}

//  -----------------------------------------------------------------------------
//  get and record kernel number
//  -----------------------------------------------------------------------------
void __merlin_parameter_head_gen(
    CSageCodeGen *codegen, void *pTopFunc, const CInputOptions &options,
    const map<string, map<string, string>> &kernel_attributes,
    const map<string, map<string, map<string, string>>> &kernel_port_attributes,
    const vector<string> &vec_auto_kernel, string impl_tool) {
  printf("    Enter merlin parameter head generation...\n");
  string parameter_s = "__merlin_parameter_h.cpp";
  {
    string cmd = "rm -rf " + parameter_s;
    system(cmd.c_str());
  }
  void *parameter = codegen->CreateSourceFile(parameter_s);
  int kernel_number = kernel_attributes.size();

  // remove auto kernel number
  kernel_number -= vec_auto_kernel.size();

  string kernel_number_s =
      "#define KERNEL_NUM " + std::to_string(kernel_number) + "\n";
  codegen->AddDirectives(kernel_number_s, parameter);
}

//  -----------------------------------------------------------------------------
//  generate buffer, kernel and event declaration
//  create wait event, init and release releated API declaration
//  -----------------------------------------------------------------------------
void __merlin_kernel_buffer_head_gen(
    CSageCodeGen *codegen, void *pTopFunc, const CInputOptions &options,
    const map<string, map<string, string>> &kernel_attributes,
    const map<string, map<string, map<string, string>>> &kernel_port_attributes,
    const map<void *, vector<void *>> &kernel_subkernel, string impl_tool,
    const map<string, map<string, string>> task_kernel,
    const vector<string> &vec_auto_kernel) {
  printf("    Enter merlin kernel buffer head generation...\n");
  string kernel_buffer_head_file = "__merlin_opencl_kernel_buffer_h.cpp";
  {
    string cmd = "rm -rf " + kernel_buffer_head_file;
    system(cmd.c_str());
  }
  void *kernel_buffer_head = codegen->CreateSourceFile(kernel_buffer_head_file);

  string kernel_buffer_header_s;
  //  init and release API definition
  kernel_buffer_header_s += head_opencl_if();
  kernel_buffer_header_s += func_opencl_init_kernel_buffer() + ";\n";
  kernel_buffer_header_s += func_release_kernel_buffer() + ";\n";

  map<void *, vector<void *>>::const_iterator iter1;
  map<string, map<string, string>>::const_iterator iter2;
  for (iter1 = kernel_subkernel.begin(); iter1 != kernel_subkernel.end();
       iter1++) {
    void *top_kernel = iter1->first;
    cout << "one kernel = " << codegen->UnparseToString(top_kernel) << endl;
    vector<void *> subkernels = iter1->second;
    for (size_t i = 0; i < subkernels.size(); i++) {
      void *one_kernel = subkernels[i];
      string kernel_name = codegen->GetFuncName(one_kernel);
      //      cout << "DEbug one kernel = " <<
      //      codegen->UnparseToString(one_kernel) << endl; cout << "DEbug auto
      //      kernel = " << codegen->UnparseToString(vec_auto_kernel[0]) <<
      //      endl;
      // if auto kernel. not generate related API
      if (find(vec_auto_kernel.begin(), vec_auto_kernel.end(), kernel_name) !=
          vec_auto_kernel.end()) {
        continue;
      }

      //  generate kernel and event
      kernel_buffer_header_s += "extern " + decl_kernel(kernel_name);
      kernel_buffer_header_s += "extern " + decl_kernel_event(kernel_name);

      if (one_kernel != top_kernel) {
        continue;
      }

      //  generate merlin APIs
      kernel_buffer_header_s += EXTERN_C_BEGIN;
      kernel_buffer_header_s += func_merlin_init(kernel_name) + ";\n";
      kernel_buffer_header_s += func_merlin_release(kernel_name) + ";\n";
      kernel_buffer_header_s += func_wait_kernel(kernel_name) + ";\n";
      kernel_buffer_header_s += func_wait_write(kernel_name) + ";\n";
      kernel_buffer_header_s += func_wait_read(kernel_name) + ";\n";
      kernel_buffer_header_s += EXTERN_C_END;

      const map<string, map<string, string>> kernel_port =
          kernel_port_attributes.find(kernel_name)->second;
      for (iter2 = kernel_port.begin(); iter2 != kernel_port.end(); iter2++) {
        string port_name = iter2->first;
        map<string, string> port_attributes_m = iter2->second;
        string bank = port_attributes_m["bank"];
        string port_type = port_attributes_m["port_type"];
        if (port_type == "array" || port_type == "pointer") {
          //  generate opencl buffer and event
          kernel_buffer_header_s +=
              "extern " + decl_buffer(kernel_name, port_name);
          kernel_buffer_header_s +=
              "extern " + decl_buffer_event(kernel_name, port_name);
          if (impl_tool == "sdaccel" && bank != "nullptr") {
            kernel_buffer_header_s +=
                "extern " + decl_buffer_ext(kernel_name, port_name);
          } else {
          }
        }
      }
    }
  }
  codegen->AddDirectives(kernel_buffer_header_s, kernel_buffer_head);
}

//  -----------------------------------------------------------------------------
//  generate API implementation in header files
//  -----------------------------------------------------------------------------
void __merlin_kernel_buffer_file_gen(
    CSageCodeGen *codegen, void *pTopFunc, const CInputOptions &options,
    const map<string, map<string, string>> &kernel_attributes,
    const map<string, map<string, map<string, string>>> &kernel_port_attributes,
    const map<void *, vector<void *>> &kernel_subkernel, string impl_tool,
    const map<string, map<string, string>> &task_kernel,
    const vector<string> &vec_auto_kernel, string pcie_transfer_opt) {
  printf("    Enter merlin kernel buffer file generation...\n");
  string kernel_buffer_file_name = "__merlin_opencl_kernel_buffer.cpp";
  {
    string cmd = "rm -rf " + kernel_buffer_file_name;
    system(cmd.c_str());
  }
  void *kernel_buffer_file = codegen->CreateSourceFile(kernel_buffer_file_name);

  string kernel_buffer_file_s;
  //  insert .h file
  kernel_buffer_file_s += head_opencl_kernel_buffer();
  kernel_buffer_file_s += "\n";

  map<void *, vector<void *>>::const_iterator iter1;
  map<string, map<string, string>>::const_iterator iter2;
  //  Create opencl mem and event declaration
  for (iter1 = kernel_subkernel.begin(); iter1 != kernel_subkernel.end();
       iter1++) {
    void *top_kernel = iter1->first;
    vector<void *> subkernels = iter1->second;
    for (size_t i = 0; i < subkernels.size(); i++) {
      void *one_kernel = subkernels[i];
      string kernel_name = codegen->GetFuncName(one_kernel);
      // if auto kernel. not generate related API
      if (find(vec_auto_kernel.begin(), vec_auto_kernel.end(), kernel_name) !=
          vec_auto_kernel.end()) {
        continue;
      }

      kernel_buffer_file_s += decl_kernel(kernel_name);
      kernel_buffer_file_s += decl_kernel_event(kernel_name);

      if (one_kernel != top_kernel) {
        continue;
      }

      map<string, map<string, string>> kernel_port =
          kernel_port_attributes.find(kernel_name)->second;
      for (iter2 = kernel_port.begin(); iter2 != kernel_port.end(); iter2++) {
        string port_name = iter2->first;
        map<string, string> port_attributes_m = iter2->second;
        string bank = port_attributes_m["bank"];
        string port_type = port_attributes_m["port_type"];
        if (port_type == "array" || port_type == "pointer") {
          kernel_buffer_file_s += decl_buffer(kernel_name, port_name);
          kernel_buffer_file_s += decl_buffer_event(kernel_name, port_name);
          if (impl_tool == "sdaccel" && bank != "nullptr") {
            kernel_buffer_file_s += decl_buffer_ext(kernel_name, port_name);
          } else {
          }
        }
      }
    }
  }
  kernel_buffer_file_s += "\n";

  //  generate __merlin_init_kernel_name()
  vector<string> vec_tasks;
  if (!task_kernel.empty()) {
    map<string, map<string, string>>::const_iterator iter0;
    for (iter0 = task_kernel.begin(); iter0 != task_kernel.end(); iter0++) {
      string task_name = iter0->first;
      vec_tasks.push_back(task_name);
    }
  }
  for (iter1 = kernel_subkernel.begin(); iter1 != kernel_subkernel.end();
       iter1++) {
    void *top_kernel = iter1->first;
    string top_kernel_name = codegen->GetFuncName(top_kernel);
    kernel_buffer_file_s += func_merlin_init(top_kernel_name) + "{\n";
    vector<void *> subkernels = iter1->second;
    for (size_t i = 0; i < subkernels.size(); i++) {
      void *one_kernel = subkernels[i];
      string kernel_name = codegen->GetFuncName(one_kernel);
      // if auto kernel. not generate related API
      if (find(vec_auto_kernel.begin(), vec_auto_kernel.end(), kernel_name) !=
          vec_auto_kernel.end()) {
        continue;
      }

      if (std::find(vec_tasks.begin(), vec_tasks.end(), top_kernel_name) !=
          vec_tasks.end()) {
        map<string, string> kernel_file =
            task_kernel.find(top_kernel_name)->second;
        map<string, string>::iterator iter0_sub;
        for (iter0_sub = kernel_file.begin(); iter0_sub != kernel_file.end();
             iter0_sub++) {
          string kernel_name = iter0_sub->first;
          kernel_buffer_file_s += func_create_kernel(kernel_name);
        }
      } else {
        kernel_buffer_file_s += func_create_kernel(kernel_name);
      }

      map<string, map<string, string>> kernel_port =
          kernel_port_attributes.find(kernel_name)->second;

      if (one_kernel != top_kernel) {
        continue;
      }

      for (iter2 = kernel_port.begin(); iter2 != kernel_port.end(); iter2++) {
        string port_name = iter2->first;
        map<string, string> port_attributes_m = iter2->second;
        string bank = port_attributes_m["bank"];
        string data_size_in_byte = port_attributes_m["data_size_in_byte"];
        string port_type = port_attributes_m["port_type"];
        bool is_migrate = port_attributes_m["migrate_mode"] == "on";
        is_migrate |= (pcie_transfer_opt == "on");
        if (port_type == "array" || port_type == "pointer") {
          bool is_task = false;
          if (find(vec_tasks.begin(), vec_tasks.end(), kernel_name) !=
              vec_tasks.end()) {
            is_task = true;
          }
          if (!is_migrate)
            kernel_buffer_file_s +=
                func_create_buffer(kernel_name, port_name, data_size_in_byte,
                                   impl_tool, bank, is_task);
        }
      }
    }
    kernel_buffer_file_s += stmt_return_0();
    kernel_buffer_file_s += "}\n";
  }
  kernel_buffer_file_s += "\n";

  //  generate opencl_init_kernel_buffer()
  kernel_buffer_file_s += func_opencl_init_kernel_buffer() + "{\n";
  for (iter1 = kernel_subkernel.begin(); iter1 != kernel_subkernel.end();
       iter1++) {
    void *top_kernel = iter1->first;
    string top_kernel_name = codegen->GetFuncName(top_kernel);
    kernel_buffer_file_s += "\t" + func_merlin_init(top_kernel_name) + ";\n";
  }
  kernel_buffer_file_s += stmt_return_0();
  kernel_buffer_file_s += "}\n";
  kernel_buffer_file_s += "\n";

  //  generate __merlin_release_kernel_name()
  for (iter1 = kernel_subkernel.begin(); iter1 != kernel_subkernel.end();
       iter1++) {
    void *top_kernel = iter1->first;
    string top_kernel_name = codegen->GetFuncName(top_kernel);
    kernel_buffer_file_s += func_merlin_release(top_kernel_name) + "{\n";
    vector<void *> subkernels = iter1->second;
    for (size_t i = 0; i < subkernels.size(); i++) {
      void *one_kernel = subkernels[i];
      string kernel_name = codegen->GetFuncName(one_kernel);
      // if auto kernel. not generate related API
      if (find(vec_auto_kernel.begin(), vec_auto_kernel.end(), kernel_name) !=
          vec_auto_kernel.end()) {
        continue;
      }

      if (one_kernel != top_kernel) {
        continue;
      }

      kernel_buffer_file_s += func_release_kernel(kernel_name);
      kernel_buffer_file_s += func_release_kernel_event(kernel_name);
      map<string, map<string, string>> kernel_port =
          kernel_port_attributes.find(kernel_name)->second;
      for (iter2 = kernel_port.begin(); iter2 != kernel_port.end(); iter2++) {
        string port_name = iter2->first;
        map<string, string> port_attributes_m = iter2->second;
        string port_type = port_attributes_m["port_type"];
        if (port_type == "array" || port_type == "pointer") {
          kernel_buffer_file_s += func_release_buffer(kernel_name, port_name);
        }
      }
    }
    kernel_buffer_file_s += stmt_return_0();
    kernel_buffer_file_s += "}\n";
  }
  kernel_buffer_file_s += "\n";

  //  gemerate __merlin_release_kernel_buffer()
  kernel_buffer_file_s += func_release_kernel_buffer() + "{\n";
  for (iter1 = kernel_subkernel.begin(); iter1 != kernel_subkernel.end();
       iter1++) {
    void *top_kernel = iter1->first;
    string top_kernel_name = codegen->GetFuncName(top_kernel);
    kernel_buffer_file_s += "\t" + func_merlin_release(top_kernel_name) + ";\n";
  }
  kernel_buffer_file_s += stmt_return_0();
  kernel_buffer_file_s += "}\n";
  kernel_buffer_file_s += "\n";

  //  generate __merlin_wait_kernel_kernel_name()
  for (iter1 = kernel_subkernel.begin(); iter1 != kernel_subkernel.end();
       iter1++) {
    void *top_kernel = iter1->first;
    string top_kernel_name = codegen->GetFuncName(top_kernel);
    kernel_buffer_file_s += func_wait_kernel(top_kernel_name) + "{\n";
    vector<void *> subkernels = iter1->second;
    for (size_t i = 0; i < subkernels.size(); i++) {
      void *one_kernel = subkernels[i];
      string kernel_name = codegen->GetFuncName(one_kernel);
      // if auto kernel. not generate related API
      if (find(vec_auto_kernel.begin(), vec_auto_kernel.end(), kernel_name) !=
          vec_auto_kernel.end()) {
        continue;
      }
      kernel_buffer_file_s += func_wait_kernel_event(kernel_name);
    }
    kernel_buffer_file_s += stmt_return_0();
    kernel_buffer_file_s += "}\n";
    kernel_buffer_file_s += "\n";
  }

  //  generate __merlin_wait_write_kernel_name()
  //  FIXME set variable int __MERLIN_EVENT_WRITE__kernel_name to determin if
  //  need release in read or write
  for (iter1 = kernel_subkernel.begin(); iter1 != kernel_subkernel.end();
       iter1++) {
    void *top_kernel = iter1->first;
    string top_kernel_name = codegen->GetFuncName(top_kernel);
    kernel_buffer_file_s += func_wait_write(top_kernel_name) + "{\n";
    vector<void *> subkernels = iter1->second;
    for (size_t i = 0; i < subkernels.size(); i++) {
      void *one_kernel = subkernels[i];
      string kernel_name = codegen->GetFuncName(one_kernel);
      // if auto kernel. not generate related API
      if (find(vec_auto_kernel.begin(), vec_auto_kernel.end(), kernel_name) !=
          vec_auto_kernel.end()) {
        continue;
      }
      map<string, map<string, string>> kernel_port =
          kernel_port_attributes.find(kernel_name)->second;
      for (iter2 = kernel_port.begin(); iter2 != kernel_port.end(); iter2++) {
        string port_name = iter2->first;
        map<string, string> port_attributes_m = iter2->second;
        string port_type = port_attributes_m["port_type"];
        int valid = 0;
        vector<void *> v_decls;
        codegen->GetNodesByType(pTopFunc, "preorder", "SgVariableDeclaration",
                                &v_decls);
        for (auto decl : v_decls) {
          if (("__MERLIN_EVENT_WRITE__" +
               opencl_buffer_event(kernel_name, port_name)) ==
              codegen->GetVariableName(
                  codegen->GetVariableInitializedName(decl))) {
            valid = 1;
            break;
          }
        }
        if ((port_type == "array" || port_type == "pointer") && (valid != 0)) {
          kernel_buffer_file_s +=
              func_wait_buffer_event(kernel_name, port_name);
          kernel_buffer_file_s += func_release_buffer(kernel_name, port_name);
        }
      }
    }
    kernel_buffer_file_s += stmt_return_0();
    kernel_buffer_file_s += "}\n";
  }
  kernel_buffer_file_s += "\n";

  //  generate __merlin_wait_read_kernel_name()
  for (iter1 = kernel_subkernel.begin(); iter1 != kernel_subkernel.end();
       iter1++) {
    void *top_kernel = iter1->first;
    string top_kernel_name = codegen->GetFuncName(top_kernel);
    kernel_buffer_file_s += func_wait_read(top_kernel_name) + "{\n";
    vector<void *> subkernels = iter1->second;
    for (size_t i = 0; i < subkernels.size(); i++) {
      void *one_kernel = subkernels[i];
      string kernel_name = codegen->GetFuncName(one_kernel);
      // if auto kernel. not generate related API
      if (find(vec_auto_kernel.begin(), vec_auto_kernel.end(), kernel_name) !=
          vec_auto_kernel.end()) {
        continue;
      }
      map<string, map<string, string>> kernel_port =
          kernel_port_attributes.find(kernel_name)->second;
      for (iter2 = kernel_port.begin(); iter2 != kernel_port.end(); iter2++) {
        string port_name = iter2->first;
        map<string, string> port_attributes_m = iter2->second;
        string port_type = port_attributes_m["port_type"];
        int valid = 0;
        vector<void *> v_decls;
        codegen->GetNodesByType(pTopFunc, "preorder", "SgVariableDeclaration",
                                &v_decls);
        for (auto decl : v_decls) {
          if (("__MERLIN_EVENT_READ__" +
               opencl_buffer_event(kernel_name, port_name)) ==
              codegen->GetVariableName(
                  codegen->GetVariableInitializedName(decl))) {
            valid = 1;
            break;
          }
        }
        if ((port_type == "array" || port_type == "pointer") && (valid != 0)) {
          kernel_buffer_file_s +=
              func_wait_buffer_event(kernel_name, port_name);
          kernel_buffer_file_s += func_release_buffer(kernel_name, port_name);
        }
      }
    }
    kernel_buffer_file_s += stmt_return_0();
    kernel_buffer_file_s += "}\n";
  }
  kernel_buffer_file_s += "\n";

  codegen->AddDirectives(kernel_buffer_file_s, kernel_buffer_file);
}

//  -----------------------------------------------------------------------------
//  generate opencl related APIs in __merlinwrapper_xxx.cpp
//  -----------------------------------------------------------------------------
void __merlin_wrapper_mod(
    CSageCodeGen *codegen, void *pTopFunc, const CInputOptions &options,
    const map<string, map<string, string>> &kernel_attributes,
    const map<string, map<string, map<string, string>>> &kernel_port_attributes,
    const map<void *, vector<void *>> &kernel_subkernel, string impl_tool,
    const map<string, map<string, string>> &task_kernel,
    map<string, string> *kernel_exec, map<string, string> *kernel_wait,
    const vector<string> &vec_auto_kernel, string pcie_transfer_opt) {
  printf("    Enter merlin wrapper generation...\n");
  vector<string> vec_tasks;
  map<string, map<string, string>>::const_iterator iter_task;
  for (iter_task = task_kernel.begin(); iter_task != task_kernel.end();
       iter_task++) {
    string task_name = iter_task->first;
    vec_tasks.push_back(task_name);
  }

  map<void *, vector<void *>>::const_iterator iter1;
  map<string, map<string, string>>::const_iterator iter2;
  //  Create opencl mem and event declaration
  int count_kernel = 0;
  for (iter1 = kernel_subkernel.begin(); iter1 != kernel_subkernel.end();
       iter1++) {
    void *top_kernel = iter1->first;
    string top_kernel_name = codegen->GetFuncName(top_kernel);
    if (std::find(vec_tasks.begin(), vec_tasks.end(), top_kernel_name) !=
        vec_tasks.end()) {
      continue;
    }

    void *top_kernel_wrapper = nullptr;
    {
      string wrapper_func_name =
          WRAPPER_PREFIX + codegen->GetFuncName(top_kernel);
      top_kernel_wrapper = codegen->GetFuncDeclByName(wrapper_func_name);
    }
    if (top_kernel_wrapper == nullptr) {
      continue;
    }

    vector<void *> subkernels = iter1->second;
    string kernel_exec_s;
    string str_migrate_decl = "";
    string str_migrate_i = "";
    string str_migrate_o = "";
    int num_cl_mem_i = 0;
    int num_cl_mem_o = 0;
    string str_setarg;
    string str_setarg_exec;
    string str_execute;
    string str_waitexec;
    string str_migrate_release = "";
    for (size_t i = 0; i < subkernels.size(); i++) {
      void *one_kernel = subkernels[i];
      string kernel_name = codegen->GetFuncName(one_kernel);
      //      kernel_name = codegen->legalizeName(kernel_name);
      // if auto kernel. not generate related API
      if (find(vec_auto_kernel.begin(), vec_auto_kernel.end(), kernel_name) !=
          vec_auto_kernel.end()) {
        continue;
      }
      vector<void *> vec_calls;
      codegen->GetFuncCallsByName(kernel_name, top_kernel_wrapper, &vec_calls);
      for (auto call : vec_calls) {
        if (codegen->is_located_in_scope(call, top_kernel_wrapper) != 0) {
          map<string, map<string, string>> kernel_port =
              kernel_port_attributes.find(kernel_name)->second;
          void *kernel = codegen->GetFuncDeclByName(kernel_name);
          str_setarg += "\n{\n";
          str_setarg += info_start_set_kernel_arg(kernel_name);
          str_setarg_exec += "\n{\n";
          str_setarg_exec += info_start_set_kernel_arg(kernel_name);
          //  opencl_set_kernel_arg : set kernel argument index
          for (int i = 0; i < codegen->GetFuncParamNum(kernel); i++) {
            void *var_init = codegen->GetFuncParam(kernel, i);
            string port_name = codegen->UnparseToString(var_init);
            map<string, string> port_attributes_m = kernel_port[port_name];
            string port_type = port_attributes_m["port_type"];
            string data_type = port_attributes_m["data_type"];
            string data_size_in_byte = port_attributes_m["data_size_in_byte"];
            string org_name = port_attributes_m["org_name"];
            string var_name = port_attributes_m["VARIABLE"];
            string bank = port_attributes_m["bank"];
            string read_valid = port_attributes_m["read_valid"];
            string write_valid = port_attributes_m["write_valid"];
            bool is_migrate = port_attributes_m["migrate_mode"] == "on";
            is_migrate |= (pcie_transfer_opt == "on");
            string toSearch = "->";
            string replaceStr = ".";
            size_t pos = org_name.find(toSearch);
            while (pos != std::string::npos) {
              org_name.replace(pos, toSearch.size(), replaceStr);
              pos = org_name.find(toSearch, pos + toSearch.size());
            }
            // bool is_task = false;
            // if (find(vec_tasks.begin(), vec_tasks.end(), kernel_name) !=
            //     vec_tasks.end()) {
            //   is_task = true;
            // }

            string name;
            string name_exec;
            if (port_type == "array" || port_type == "pointer") {
              data_type = "opencl_mem";

              string kernel_name_for_opencl_buffer = kernel_name;
              if (one_kernel != top_kernel) {
                kernel_name_for_opencl_buffer =
                    codegen->GetFuncName(top_kernel);
              }

              name = opencl_buffer(kernel_name_for_opencl_buffer, port_name,
                                   is_migrate);
              name_exec = opencl_buffer(kernel_name_for_opencl_buffer,
                                        port_name, false);
              if (is_migrate) {
                str_migrate_release +=
                    func_release_buffer(kernel_name, port_name, is_migrate);
                if (read_valid == "1") {
                  num_cl_mem_i++;
                }
                if (write_valid == "1") {
                  num_cl_mem_o++;
                }
              }
            } else if (port_type == "scalar") {
              name = org_name;
              name_exec = org_name;
            }
            str_setarg += func_set_kernel_arg(kernel_name, i, data_type, name);
            str_setarg_exec +=
                func_set_kernel_arg(kernel_name, i, data_type, name_exec);
          }
          str_setarg += info_finish_set_kernel_arg(kernel_name);
          str_setarg += "}\n";
          str_setarg_exec += info_finish_set_kernel_arg(kernel_name);
          str_setarg_exec += "}\n";
          str_execute += "{\n";
          str_execute += info_start_exec_kernel(kernel_name);
          int num = count_kernel;
          str_execute += func_start_exec_kernel(kernel_name, num);
          str_execute += "}\n";

          // create migrate call
          // create migrate variable declaration
          if (pcie_transfer_opt == "on") {
            if (num_cl_mem_i > 0) {
              str_migrate_i +=
                  func_migrate_call(kernel_name, num, num_cl_mem_i, true);
            }
            if (num_cl_mem_o > 0) {
              str_migrate_o +=
                  func_migrate_call(kernel_name, num, num_cl_mem_o, false);
            }
            string v_cl_mem_name_i = migrate_vector_cl_mem(kernel_name, true);
            string v_cl_mem_name_p_i = v_cl_mem_name_i + "_p";
            str_migrate_decl = "\n\tcl_mem " + v_cl_mem_name_i + "[4096];\n";
            str_migrate_decl += "\n\tint " + v_cl_mem_name_p_i + "=0;\n";
            string v_cl_mem_name_o = migrate_vector_cl_mem(kernel_name, false);
            string v_cl_mem_name_p_o = v_cl_mem_name_o + "_p";
            str_migrate_decl += "\n\tcl_mem " + v_cl_mem_name_o + "[4096];\n";
            str_migrate_decl += "\n\tint " + v_cl_mem_name_p_o + "=0;\n";
            void *point = codegen->GetFirstNonTrivalStmt(
                codegen->GetFuncBody(top_kernel_wrapper));
            codegen->AddDirectives(str_migrate_decl, point);
          }
        }
      }
      count_kernel++;
    }

    //  Hanhu FIXME write to __merlin_exec_xxx for L2 API
    string exec_API_name = "__merlin_execute_" + top_kernel_name;
    void *exec_func = codegen->GetFuncDeclByName(exec_API_name);
    void *exec_body = codegen->GetFuncBody(exec_func);
    if (exec_body != nullptr) {
      string name = "i";
      codegen->get_valid_local_name(exec_func, &name);
      void *flag_stmt =
          codegen->CreateVariableDecl("int", name, nullptr, exec_body);
      codegen->AppendChild(exec_body, flag_stmt);
      codegen->AddDirectives("\n" + str_setarg_exec + str_execute, flag_stmt,
                             0);
    }

    void *top_kernel_call = nullptr;
    for (size_t i = 0; i < subkernels.size(); i++) {
      void *one_kernel = subkernels[i];
      string kernel_name = codegen->GetFuncName(one_kernel);
      // if auto kernel. not generate related API
      if (find(vec_auto_kernel.begin(), vec_auto_kernel.end(), kernel_name) !=
          vec_auto_kernel.end()) {
        continue;
      }
      vector<void *> vec_calls;
      codegen->GetFuncCallsByName(kernel_name, top_kernel_wrapper, &vec_calls);

      for (auto call : vec_calls) {
        if (codegen->is_located_in_scope(call, top_kernel_wrapper) != 0) {
          if (one_kernel == top_kernel) {
            top_kernel_call = call;
          }

          //  Creating wait events
          str_waitexec += "{\n";
          str_waitexec += func_finish_exec_kernel(kernel_name);
          str_waitexec += info_finish_exec_kernel(kernel_name);
          str_waitexec += "}\n";

          if (one_kernel != top_kernel) {
            void *call_stmt = codegen->TraceUpToStatement(call);
            codegen->RemoveStmt(call_stmt, false);
          }
        }
      }
    }
    void *call_stmt = codegen->TraceUpToStatement(top_kernel_call);
    void *prev_stmt = codegen->GetPreviousStmt(call_stmt);
    if (prev_stmt == nullptr) {
      void *bb = codegen->CreateBasicBlock();
      codegen->InsertStmt(bb, call_stmt);
      prev_stmt = bb;
    }
    kernel_exec->insert(pair<string, string>(top_kernel_name, str_execute));
    kernel_wait->insert(pair<string, string>(top_kernel_name, str_waitexec));
    kernel_exec_s += str_migrate_i;
    kernel_exec_s += str_setarg;
    kernel_exec_s += str_execute;
    kernel_exec_s += str_waitexec;
    kernel_exec_s += str_migrate_o;
    kernel_exec_s += str_migrate_release;
    codegen->AddDirectives(kernel_exec_s, prev_stmt, 0);
    codegen->RemoveStmt(call_stmt, false);
  }
}

//  -----------------------------------------------------------------------------
//  generate opencl related APIs in __merlinwrapper_xxx.cpp
//  -----------------------------------------------------------------------------
void __merlin_task_mod(
    CSageCodeGen *codegen, void *pTopFunc, const CInputOptions &options,
    const map<string, map<string, string>> &kernel_attributes,
    const map<string, map<string, map<string, string>>> &kernel_port_attributes,
    const map<void *, vector<void *>> &kernel_subkernel, string impl_tool,
    const map<string, map<string, string>> &task_kernel,
    const map<string, string> &kernel_exec,
    const map<string, string> &kernel_wait, string pcie_transfer_opt) {
  printf("    Enter merlin task generation...\n");
  string file = TASK_ATTRIBUTE_JSON;
  std::ifstream ifile(file);
  map<string, map<string, string>> task_attribute;
  if (ifile) {
    readInterReport(file, &task_attribute);
  }

  string kernel_exec_s;
  string str_migrate_decl = "";
  string str_migrate_i = "";
  string str_migrate_o = "";
  int num_cl_mem_i = 0;
  int num_cl_mem_o = 0;
  string str_migrate_release = "";
  bool is_migrate = (pcie_transfer_opt == "on");
  vector<string> vec_tasks;
  map<string, map<string, string>>::const_iterator iter_task;
  for (iter_task = task_kernel.begin(); iter_task != task_kernel.end();
       iter_task++) {
    string task_name = iter_task->first;
    string attribute;
    map<string, map<string, string>>::const_iterator iter_task_attribute;
    for (iter_task_attribute = task_attribute.begin();
         iter_task_attribute != task_attribute.end(); iter_task_attribute++) {
      string task_name_a = iter_task_attribute->first;
      if (task_name == task_name_a) {
        map<string, string> attribute_m = iter_task_attribute->second;
        map<string, string>::iterator iter_attribute;
        for (iter_attribute = attribute_m.begin();
             iter_attribute != attribute_m.end(); iter_attribute++) {
          attribute = iter_attribute->first;
        }
      }
    }
    cout << "task:" << task_name << " attribute=" << attribute << endl;
    //  add set argument for each kernel with task related opencl buffer
    string str_setarg;
    str_setarg += "\n{\n";
    str_setarg += info_start_set_kernel_arg(task_name);
    map<string, string> format_actual_name;
    map<void *, vector<void *>>::const_iterator iter2;
    void *task_decl = codegen->GetFuncDeclByName(task_name);
    vector<void *> child_stmts =
        codegen->GetChildStmts(codegen->GetFuncBody(task_decl));
    for (auto one_stmt : child_stmts) {
      void *one_call = codegen->GetFuncCallInStmt(one_stmt);
      if (one_call != nullptr) {
        string call_file_name = codegen->get_file(one_call);
        string kernel_name = codegen->GetFuncNameByCall(one_call);
        vector<void *> decls = codegen->GetAllFuncDeclByName(kernel_name);
        void *one_kernel;
        for (auto one_decl : decls) {
          string decl_file_name = codegen->get_file(one_decl);
          if (decl_file_name == call_file_name) {
            one_kernel = one_decl;
            break;
          }
        }
        for (iter2 = kernel_subkernel.begin(); iter2 != kernel_subkernel.end();
             iter2++) {
          void *top_kernel = iter2->first;
          string top_kernel_name = codegen->GetFuncName(top_kernel);
          if (task_name == top_kernel_name) {
            continue;
          }
          if (kernel_name != top_kernel_name) {
            continue;
          }
          map<string, map<string, map<string, string>>>::const_iterator iter1;
          for (int j = 0; j < codegen->GetFuncCallParamNum(one_call); ++j) {
            void *param_kernel_call = codegen->GetFuncCallParam(one_call, j);
            void *var_ref = nullptr;
            codegen->parse_array_ref_from_pntr(param_kernel_call, &var_ref);
            if (var_ref != nullptr) {
              void *task_var_init =
                  codegen->GetVariableInitializedName(var_ref);
              string ref_name = codegen->UnparseToString(task_var_init);
              void *kernel_var_init = codegen->GetFuncParam(one_kernel, j);
              string format_name = codegen->UnparseToString(kernel_var_init);
              for (iter1 = kernel_port_attributes.begin();
                   iter1 != kernel_port_attributes.end(); iter1++) {
                string record_kernel_name = iter1->first;
                if (record_kernel_name == kernel_name) {
                  map<string, map<string, string>> kernel_port = iter1->second;
                  map<string, string> port_attributes_m =
                      kernel_port[format_name];
                  string port_type = port_attributes_m["port_type"];
                  string data_type = port_attributes_m["data_type"];
                  string org_name = port_attributes_m["org_name"];
                  string read_valid = port_attributes_m["read_valid"];
                  string write_valid = port_attributes_m["write_valid"];
                  string name;
                  if (port_type == "array" || port_type == "pointer") {
                    data_type = "opencl_mem";
                    name = opencl_buffer(task_name, ref_name);
                    if (is_migrate) {
                      str_migrate_release +=
                          func_release_buffer(task_name, ref_name, is_migrate);
                      if (read_valid == "1") {
                        num_cl_mem_i++;
                      }
                      if (write_valid == "1") {
                        num_cl_mem_o++;
                      }
                    }
                  } else if (port_type == "scalar") {
                    name = ref_name;
                  }
                  str_setarg +=
                      func_set_kernel_arg(kernel_name, j, data_type, name);
                }
              }
            }
          }
        }
      }
    }
    str_setarg += info_finish_set_kernel_arg(task_name);
    str_setarg += "}\n";

    map<string, string> kernel_file = iter_task->second;
    if (attribute.find("parallel") != std::string::npos) {
      map<string, string>::iterator iter_kernel_file;
      for (iter_kernel_file = kernel_file.begin();
           iter_kernel_file != kernel_file.end(); iter_kernel_file++) {
        string kernel_name = iter_kernel_file->first;
        kernel_exec_s += kernel_exec.find(kernel_name)->second;
      }
      for (iter_kernel_file = kernel_file.begin();
           iter_kernel_file != kernel_file.end(); iter_kernel_file++) {
        string kernel_name = iter_kernel_file->first;
        kernel_exec_s += kernel_wait.find(kernel_name)->second;
      }
    } else {
      map<string, string>::iterator iter_kernel_file;
      for (iter_kernel_file = kernel_file.begin();
           iter_kernel_file != kernel_file.end(); iter_kernel_file++) {
        string kernel_name = iter_kernel_file->first;
        kernel_exec_s += kernel_exec.find(kernel_name)->second;
        kernel_exec_s += kernel_wait.find(kernel_name)->second;
      }
    }
    // create migrate call
    // create migrate variable declaration
    vector<void *> vec_calls;
    codegen->GetFuncCallsByName(task_name, codegen->GetProject(), &vec_calls);
    for (auto call : vec_calls) {
      string file_name = codegen->get_file(call);
      if (file_name.find("__merlinwrapper_") == 0) {
        void *stmt = codegen->TraceUpToStatement(call);
        void *prev_stmt = codegen->GetPreviousStmt(stmt);
        if (prev_stmt == nullptr) {
          void *bb = codegen->CreateBasicBlock();
          codegen->InsertStmt(bb, stmt);
          prev_stmt = bb;
        }
        codegen->RemoveStmt(stmt, false);
        if (is_migrate) {
          if (num_cl_mem_i > 0) {
            str_migrate_i +=
                func_migrate_call(task_name, 0, num_cl_mem_i, true);
          }
          if (num_cl_mem_o > 0) {
            str_migrate_o +=
                func_migrate_call(task_name, 0, num_cl_mem_o, false);
          }
          string v_cl_mem_name_i = migrate_vector_cl_mem(task_name, true);
          string v_cl_mem_name_p_i = v_cl_mem_name_i + "_p";
          str_migrate_decl = "\n\tcl_mem " + v_cl_mem_name_i + "[4096];\n";
          str_migrate_decl += "\n\tint " + v_cl_mem_name_p_i + "=0;\n";
          string v_cl_mem_name_o = migrate_vector_cl_mem(task_name, false);
          string v_cl_mem_name_p_o = v_cl_mem_name_o + "_p";
          str_migrate_decl += "\n\tcl_mem " + v_cl_mem_name_o + "[4096];\n";
          str_migrate_decl += "\n\tint " + v_cl_mem_name_p_o + "=0;\n";
          void *decl = codegen->TraceUpToFuncDecl(stmt);
          void *point =
              codegen->GetFirstNonTrivalStmt(codegen->GetFuncBody(decl));
          codegen->AddDirectives(str_migrate_decl, point);
        }
        codegen->AddDirectives(str_migrate_i, prev_stmt, 0);
        codegen->AddDirectives(str_setarg, prev_stmt, 0);
        codegen->AddDirectives(kernel_exec_s, prev_stmt, 0);
        codegen->AddDirectives(str_migrate_o, prev_stmt, 0);
      }
    }
  }
}

string get_valid_name(string input, vector<string> vec_bundle) {
  int index = 0;
  string valid = input;
  while (1) {
    if (std::find(vec_bundle.begin(), vec_bundle.end(), valid) !=
        vec_bundle.end()) {
      valid = input + std::to_string(index);
      index++;
    } else {
      break;
    }
  }
  return valid;
}

string find_unique_bundle_name(bool read_or_write, int bitwidth, string prefix,
                               vector<string> *vec_bundle) {
  int index = 0;
  while (1) {
    string new_name = prefix + "_" + std::to_string(bitwidth) + "_" +
                      std::to_string(index) + "_" +
                      std::to_string(read_or_write);
    if (std::find(vec_bundle->begin(), vec_bundle->end(), new_name) !=
        vec_bundle->end()) {
      index++;
    } else {
      vec_bundle->push_back(new_name);
      break;
    }
  }
  string name =
      prefix + "_" + std::to_string(bitwidth) + "_" + std::to_string(index);
  name = get_valid_name(name, *vec_bundle);
  cout << "rw = " << read_or_write << ", bitwidth = " << bitwidth
       << ", name = " << name << endl;
  return name;
}

// update existing HLS interface
void update_bundle_name_of_interface_pragma(CSageCodeGen *codegen,
                                            void *pTopFunc,
                                            const CInputOptions &options,
                                            vector<string> *vec_bundle,
                                            vector<string> *vec_defined) {
  vector<pair<void *, string>> vecTldmPragmas;
  codegen->TraverseSimple(pTopFunc, "preorder", GetTLDMInfo_withPointer4,
                          &vecTldmPragmas);
  vector<void *> vecTopKernels;
  // check axilite bundle first, this bundle can share the same bundle name
  for (size_t i = vecTldmPragmas.size() - 1; i < vecTldmPragmas.size(); i--) {
    string sFilter;
    string sCmd;
    map<string, pair<vector<string>, vector<string>>> mapParams;
    tldm_pragma_parse_whole(vecTldmPragmas[i].second, &sFilter, &sCmd,
                            &mapParams, true);
    boost::algorithm::to_upper(sFilter);
    boost::algorithm::to_upper(sCmd);
    if ("HLS" != sFilter || "INTERFACE" != sCmd) {
      continue;
    }
    void *one_pragma = vecTldmPragmas[i].first;
    cout << "org pragma = " << codegen->GetPragmaString(one_pragma) << endl;
    string pragma_s = codegen->GetPragmaString(one_pragma);
    if (std::strstr(pragma_s.c_str(), "s_axilite") != NULL &&
        std::strstr(pragma_s.c_str(), "port=return") != NULL) {
      cout << "Found return pragma\n" << endl;
      // remove return pragma because will add another one behind
      codegen->RemoveChild(one_pragma);
      continue;
    }
    if (std::strstr(pragma_s.c_str(), "s_axilite") != NULL) {
      std::regex e("bundle=(\\S+)");
      std::smatch sm;
      std::regex_search(pragma_s, sm, e);
      string valid_name = get_valid_name(sm[1], *vec_bundle);
      if (i == 0) {
        vec_bundle->push_back(valid_name);
      }
      valid_name = "bundle=" + valid_name;
      string replace = "bundle=" + string(sm[1]);
      int index = pragma_s.find(replace);
      pragma_s.replace(index, replace.length(), valid_name);
      void *func = codegen->TraceUpToFuncDecl(one_pragma);
      if (func == nullptr) {
        continue;
      }
      void *func_body = codegen->GetFuncBody(func);
      if (func_body == nullptr) {
        continue;
      }
      void *pragma_decl = codegen->CreatePragma(pragma_s, func_body);
      codegen->ReplaceStmt(one_pragma, pragma_decl);
      cout << "newpragma = " << codegen->UnparseToString(pragma_decl) << endl;
    }
  }
  // check maxi bundle first
  vector<string> vec_hls_bundle;
  for (size_t i = vecTldmPragmas.size() - 1; i < vecTldmPragmas.size(); i--) {
    string sFilter;
    string sCmd;
    map<string, pair<vector<string>, vector<string>>> mapParams;
    tldm_pragma_parse_whole(vecTldmPragmas[i].second, &sFilter, &sCmd,
                            &mapParams, true);
    boost::algorithm::to_upper(sFilter);
    boost::algorithm::to_upper(sCmd);
    if ("HLS" != sFilter || "INTERFACE" != sCmd) {
      continue;
    }
    void *one_pragma = vecTldmPragmas[i].first;
    cout << "org pragma = " << codegen->GetPragmaString(one_pragma) << endl;
    string pragma_s = codegen->GetPragmaString(one_pragma);
    if (std::strstr(pragma_s.c_str(), "m_axi") != NULL) {
      std::regex e("bundle=(\\S+)");
      std::smatch sm;
      std::regex_search(pragma_s, sm, e);
      string valid_name = get_valid_name(sm[1], *vec_bundle);
      vec_hls_bundle.push_back(valid_name);
      valid_name = "bundle=" + valid_name;
      string replace = "bundle=" + string(sm[1]);
      int index = pragma_s.find(replace);
      pragma_s.replace(index, replace.length(), valid_name);

      void *func = codegen->TraceUpToFuncDecl(one_pragma);
      if (func == nullptr) {
        continue;
      }
      void *func_body = codegen->GetFuncBody(func);
      if (func_body == nullptr) {
        continue;
      }

      std::regex e2("port=(\\S+)");
      std::smatch sm2;
      std::regex_search(pragma_s, sm2, e2);
      string defined_name = codegen->GetFuncName(func) + "_" + string(sm2[1]);
      vec_defined->push_back(defined_name);

      void *pragma_decl = codegen->CreatePragma(pragma_s, func_body);
      codegen->ReplaceStmt(one_pragma, pragma_decl);
      cout << "newpragma = " << codegen->UnparseToString(pragma_decl) << endl;
    }
  }
  vec_bundle->insert(vec_bundle->end(), vec_hls_bundle.begin(),
                     vec_hls_bundle.end());
}

//  -----------------------------------------------------------------------------
//  for Xilinx mode ,insert HLS interface pragma
//  -----------------------------------------------------------------------------
void insert_interface_pragma(
    CSageCodeGen *codegen, void *pTopFunc, const CInputOptions &options,
    const map<string, map<string, string>> &kernel_attributes,
    const map<string, map<string, map<string, string>>> &kernel_port_attributes,
    string impl_tool, string tool_version) {
  printf("    Enter insert hls interface pragma...\n");
  bool is_hls = false;
  if (tool_version == "vivado_hls" || tool_version == "vitis_hls") {
    is_hls = true;
  }
  // in hls flow, we do not generate interface pragma
  if (is_hls) {
    return;
  }
  map<string, map<string, map<string, string>>>::const_iterator iter1;
  map<string, map<string, string>>::const_iterator iter2;
  vector<string> vec_bundle;
  vector<string> vec_defined;
  // add port name to the list
  for (iter1 = kernel_port_attributes.begin();
       iter1 != kernel_port_attributes.end(); iter1++) {
    string kernel_name = iter1->first;
    map<string, map<string, string>> kernel_port = iter1->second;
    for (iter2 = kernel_port.begin(); iter2 != kernel_port.end(); iter2++) {
      string port_name = iter2->first;
      map<string, string> port_attributes_m = iter2->second;
      vec_bundle.push_back(port_name);
    }
  }
  // add existing name to the list
  update_bundle_name_of_interface_pragma(codegen, pTopFunc, options,
                                         &vec_bundle, &vec_defined);

  for (iter1 = kernel_port_attributes.begin();
       iter1 != kernel_port_attributes.end(); iter1++) {
    string kernel_name = iter1->first;
    void *kernel = codegen->GetFuncDeclByName(kernel_name);
    void *body = codegen->GetFuncBody(kernel);
    if (!body) {
      return;
    }
    map<string, map<string, string>> kernel_port = iter1->second;
    string pragma_s;
    //  insert HLS s_axilite pragma for return
    pragma_s = "HLS INTERFACE s_axilite port=return bundle=control\n";
    void *return_pragma_decl = codegen->CreatePragma(pragma_s, body);
    codegen->PrependChild(body, return_pragma_decl);
    //  insert HLS m_axi interface pragma for array and pointer
    for (iter2 = kernel_port.begin(); iter2 != kernel_port.end(); iter2++) {
      string port_name = iter2->first;
      string defined_name = kernel_name + "_" + port_name;
      if (std::find(vec_defined.begin(), vec_defined.end(), defined_name) !=
          vec_defined.end()) {
        // if user already have HLS interface pragma, ignore this one
        continue;
      }

      map<string, string> port_attributes_m = iter2->second;
      string max_depth = port_attributes_m["max_depth"];
      string port_type = port_attributes_m["port_type"];
      string port_bank = port_attributes_m["bank"];
      string bundle = port_attributes_m["bundle"];
      string org_bundle = port_attributes_m["org_bundle"];
      string read_valid = port_attributes_m["read_valid"];
      string write_valid = port_attributes_m["write_valid"];
      vector<string> v_s = str_split(max_depth, ',');
      int depth_size = v_s.size();
      size_t depth_int = 1;
      for (auto depth : v_s) {
        try {
          depth_int *= stoi(depth);
        } catch (std::exception &e) {
          cerr << "invalid depth " << depth << '\n';
        }
      }
      if (port_type == "array" || port_type == "pointer") {
        size_t org_bitwidth = 0;
        try {
          org_bitwidth = stoi(port_attributes_m["org_bitwidth"]);
        } catch (std::exception &e) {
          cerr << "invalid org_bitwidth " << port_attributes_m["org_bitwidth"]
               << '\n';
        }
        size_t bitwidth = 0;
        try {
          bitwidth = stoi(port_attributes_m["bitwidth"]);
        } catch (std::exception &e) {
          cerr << "invalid bitwidth " << port_attributes_m["bitwidth"] << '\n';
        }
        depth_int = ceil(1.0 * depth_int * org_bitwidth / bitwidth);
        string depth_str = "depth=" + std::to_string(depth_int);
        if (depth_size == 0) {
          depth_str = "";
        }
        if (org_bundle == "") {
          if (port_bank != "nullptr") {
            pragma_s = "HLS INTERFACE m_axi port=" + port_name +
                       " offset=slave bundle=merlin_gmem_" + kernel_name +
                       "_bank" + port_bank + " " + depth_str + "\n";
          } else {
            if (options.get_option_key_value("-a", "explicit_bundle") == "on") {
              // || bundle == CMOST_explict_bundle) {
              pragma_s = "HLS INTERFACE m_axi port=" + port_name +
                         " offset=slave bundle=merlin_gmem_" + kernel_name +
                         "_" + port_name + " " + depth_str + "\n";
            } else {
              string new_name = "merlin_gmem_" + kernel_name;
              if (read_valid == "1" && write_valid == "1") {
                new_name += "_" + std::to_string(bitwidth) + "_" + port_name;
              } else if (read_valid == "1") {
                new_name =
                    find_unique_bundle_name(0, bitwidth, new_name, &vec_bundle);
              } else if (write_valid == "1") {
                new_name =
                    find_unique_bundle_name(1, bitwidth, new_name, &vec_bundle);
              }
              cout << "bundle for " << port_name << " is " << new_name << endl;
              pragma_s = "HLS INTERFACE m_axi port=" + port_name +
                         " offset=slave " + depth_str + " bundle=" + new_name +
                         "\n";
            }
          }
        } else {
          string valid_name = get_valid_name(org_bundle, vec_bundle);
          pragma_s = "HLS INTERFACE m_axi port=" + port_name +
                     " offset=slave bundle=" + valid_name + " " + depth_str +
                     "\n";
        }
        void *pragma_decl = codegen->CreatePragma(pragma_s, body);
        codegen->InsertStmt(pragma_decl, return_pragma_decl);
      } else if (port_type == "scalar") {
      }
    }
    //  insert HLS s_axilite pragma for array, pointer and scalar
    for (iter2 = kernel_port.begin(); iter2 != kernel_port.end(); iter2++) {
      string port_name = iter2->first;
      string defined_name = kernel_name + "_" + port_name;
      if (std::find(vec_defined.begin(), vec_defined.end(), defined_name) !=
          vec_defined.end()) {
        // if user already have HLS interface pragma, ignore this one
        continue;
      }
      map<string, string> port_attributes_m = iter2->second;
      pragma_s =
          "HLS INTERFACE s_axilite port=" + port_name + " bundle=control\n";
      void *pragma_decl = codegen->CreatePragma(pragma_s, body);
      codegen->InsertStmt(pragma_decl, return_pragma_decl);
    }
  }
}

//  =======================================================//
//  opencl code generation
//  __merlin_opencl_kernel_buffer.cpp
//  __merlin_opencl_kernel_buffer.h
//  __merlin_parameter.h
//  __merlinwrapper_xxx.cpp
//  =======================================================//
int opencl_code_gen(
    CSageCodeGen *codegen, void *pTopFunc, const CInputOptions &options,
    const map<string, map<string, string>> &task_kernel,
    const map<string, map<string, string>> &kernel_attributes,
    const map<string, map<string, map<string, string>>> &kernel_port_attributes,
    const map<void *, vector<void *>> &kernel_subkernel,
    const vector<string> &vec_auto_kernel, string impl_tool,
    string tool_version, string pcie_transfer_opt, bool opencl_disable) {
  printf("    Enter opencl code generation...\n");

  if (!pure_kernel_flow && !opencl_disable) {
    //  -----------------------------------------------------------------------------
    //  generate __merlin_parameter.h file
    //  -----------------------------------------------------------------------------
    __merlin_parameter_head_gen(codegen, pTopFunc, options, kernel_attributes,
                                kernel_port_attributes, vec_auto_kernel,
                                impl_tool);

    //-----------------------------------------------------------------------------
    // generate __merlin_kernel_buffer_head.h file
    //-----------------------------------------------------------------------------
    __merlin_kernel_buffer_head_gen(
        codegen, pTopFunc, options, kernel_attributes, kernel_port_attributes,
        kernel_subkernel, impl_tool, task_kernel, vec_auto_kernel);

    //-----------------------------------------------------------------------------
    // generate __merlin_kernel_buffer_head.c file
    //-----------------------------------------------------------------------------
    __merlin_kernel_buffer_file_gen(codegen, pTopFunc, options,
                                    kernel_attributes, kernel_port_attributes,
                                    kernel_subkernel, impl_tool, task_kernel,
                                    vec_auto_kernel, pcie_transfer_opt);

    //-----------------------------------------------------------------------------
    // modify __merlinwrapper_xxx.c file
    //-----------------------------------------------------------------------------
    map<string, string> kernel_exec;
    map<string, string> kernel_wait;
    __merlin_wrapper_mod(codegen, pTopFunc, options, kernel_attributes,
                         kernel_port_attributes, kernel_subkernel, impl_tool,
                         task_kernel, &kernel_exec, &kernel_wait,
                         vec_auto_kernel, pcie_transfer_opt);

    //-----------------------------------------------------------------------------
    // modify __merlintask_xxx.c file
    //-----------------------------------------------------------------------------
    __merlin_task_mod(codegen, pTopFunc, options, kernel_attributes,
                      kernel_port_attributes, kernel_subkernel, impl_tool,
                      task_kernel, kernel_exec, kernel_wait, pcie_transfer_opt);

    //-----------------------------------------------------------------------------
    // generate xilinx interface pragma
    //-----------------------------------------------------------------------------
  }
  // only not in hls mode, we insert interface pragma as original mode
  // else, we do not touch user original interface pragma
  if (impl_tool == "sdaccel") {
    insert_interface_pragma(codegen, pTopFunc, options, kernel_attributes,
                            kernel_port_attributes, impl_tool, tool_version);
  }

  return 1;
}

/*************************************************************************************
//  1. find all the kernel calls
//  2. replace memory allocation for the kernel ports (pointer)
//  3. allocate additional single_element buffer for scalar
//  4. replace kernel call
//  5. add the include file "kestrel_app.h"
int __merlin_host_app_wrapper_for_kestrel(CSageCodeGen * codegen, void *
pTopFunc)
{
#if USED_CODE_IN_COVERAGE_TEST
    size_t i;
    //  printf("Hello kestrel! \n");

    vector<pair<void*, string> > vecTldmPragmas;
    codegen->TraverseSimple(pTopFunc, "preorder", GetTLDMInfo_withPointer4,
&vecTldmPragmas); for (i = 0; i < vecTldmPragmas.size(); i++)
    {
        //  /////////////////////////////////////////////////  /
        //  1. find all the kernel calls
        //  /////////////////////////////////////////////////  /
        string sFilter, sCmd;
        map<string, pair<vector<string>, vector<string> > > mapParams;
        tldm_pragma_parse_whole(vecTldmPragmas[i].second, sFilter, sCmd,
mapParams);

        if (CMOST_PRAGMA != sFilter and ACCEL_PRAGMA != sFilter) continue;
        if ("KERNEL" != sCmd) continue;

        void * sg_kernel_call =
codegen->find_kernel_call(vecTldmPragmas[i].first); assert(sg_kernel_call); void
* sg_task = sg_kernel_call; void * sg_task_stmt =
codegen->TraceUpToStatement(sg_task); void * sg_kernel =
codegen->GetFuncDeclByCall(sg_task);

        //  cout << "-- " << codegen->_p(sg_task) << " : " <<
codegen->get_file(sg_task) << endl; cout << codegen->_p(sg_task) << " : " <<
codegen->_p(sg_kernel) << endl;

        //  /////////////////////////////////////////////////  /
        //  2. replace memory allocation for the kernel ports (pointer)
        //  3. allocate additional single_element buffer for scalar
        //  /////////////////////////////////////////////////  /
        int read_index = 0;
        int write_index = 0;
        int index_alloc = 0;
        for (int j = 0; j < codegen->GetFuncParamNum(sg_kernel); j++)
        {
            void * actual_arg = codegen->GetFuncCallParam(sg_task, j);
            void * formal_arg = codegen->GetFuncParam(sg_kernel, j);

            codegen->remove_cast(&actual_arg);

            //  cout << "  -- " << codegen->_p(actual_arg) << " : " <<
codegen->_p(formal_arg) << endl;

            if (!codegen->IsVarRefExp(actual_arg))
            {
                string msg = "Kernel argument can not be expression: " +
string("") +
                    "\n\t ";
                string var_info = "Argument '" +
codegen->UnparseToString(actual_arg) +
                    "' for kernel '" + codegen->GetFuncName(sg_kernel) +
                    "' " + getUserCodeFileLocation(codegen, actual_arg, true) +
"\n"; dump_critical_message(RUNTM_ERROR_1(var_info)); cout << msg << endl;
                exit(-1);
            }

            //  get type for the actual argument

            void *sg_array;
            void *base_type;
            vector<size_t> nSizes;
            codegen->get_type_dimension_by_ref(actual_arg, sg_array, base_type,
nSizes);

            cout << "  -- " << codegen->_p(actual_arg) << " : " <<
codegen->_p(base_type) << " dim=" << nSizes.size() << endl;


            const string var_suffix =
"_"+codegen->GetFuncName(sg_kernel)+"_kestrel";

            if (nSizes.size() == 0)
            {
                //  data_type * __var_t = kestrel_alloc(1, length,
length*sizeof(data_type), 0);
                //  *__var_t = actual_arg;

                vector<void*> params;
                params.push_back(codegen->CreateStringExp(codegen->GetFuncName(sg_kernel)));
                params.push_back(codegen->CreateConst(read_index));
                params.push_back(codegen->CreateConst(1));
                params.push_back(codegen->CreateConst(1));
                params.push_back(codegen->CreateExp(V_SgSizeOfOp, base_type));
                params.push_back(codegen->CreateConst(0));
                void * sg_call = codegen->CreateFuncCall(
                        "kestrel_alloc",
                        base_type, params, codegen->GetScope(sg_task));
                sg_call = codegen->CreateCastExp(sg_call,
codegen->CreatePointerType(base_type) ); string s_var = "__" +
codegen->UnparseToString(actual_arg) + var_suffix; void * sg_var_decl =
codegen->CreateVariableDecl( codegen->CreatePointerType(base_type), s_var,
sg_call, codegen->GetScope(sg_task)); codegen->InsertStmt(sg_var_decl,
sg_task_stmt);

                void * sg_assign = codegen->CreateExp(V_SgAssignOp,
                        codegen->CreateExp(V_SgPointerDerefExp, sg_var_decl),
                        codegen->CopyExp(actual_arg));
                void * sg_stmt = codegen->createstmt(v_sgexprstatement,
sg_assign); codegen->insertstmt(sg_stmt, sg_task_stmt);

                cout << "  -->" << codegen->_p(sg_var_decl) << endl;
                cout << "  -->" << codegen->_p(sg_stmt) << endl;
                read_index++;
            }
            else if (nSizes.size() == 1)
            {
                int dim_size = nSizes[0];
                if (dim_size == 0)
                {
                    string msg = "Unknown dimension size of array/pointer
variables: " + string("") +
                        "\n\t variable ";
                    string var_info = "'" + codegen->UnparseToString(actual_arg)
+
                        "' for kernel '" + codegen->GetFuncName(sg_kernel) +
                        "' " + getUserCodeFileLocation(codegen, actual_arg,
true) + "\n"; dump_critical_message(RUNTM_ERROR_2(var_info)); cout << msg <<
endl; exit(-1);
                }

                //  1. get the original declaration
                void * org_var_decl =
codegen->get_actual_var_decl_by_ref(actual_arg);  //  by default, find the
unique one if (!org_var_decl)
                {
                    string msg = "Can not find the unique actual declaration for
the variable: " + string("") +
                        "\n\t variable ";
                    string var_info = "'" + codegen->UnparseToString(actual_arg)
+
                        "' for kernel '" + codegen->GetFuncName(sg_kernel) +
                        "' " + getUserCodeFileLocation(codegen, actual_arg,
true) + "\n"; dump_critical_message(RUNTM_ERROR_6(var_info)); cout << msg <<
endl; exit(-1);
                }

                if
(codegen->IsGlobalInitName(codegen->GetVariableInitializedName(org_var_decl)))
                {
                    string msg = "The actual declaration for the kernel
interface port can not be global variable: " + string("") +
                        "\n\t variable ";
                    string var_info = "'" + codegen->UnparseToString(actual_arg)
+
                        "' for kernel '" + codegen->GetFuncName(sg_kernel) +
                        "' " + getUserCodeFileLocation(codegen, actual_arg,
true) + "\n"; dump_critical_message(RUNTM_ERROR_5(var_info)); cout << msg <<
endl; exit(-1);
                }


                //  is_write flag: indicating whether the kernel has write to
the port variable int has_read  = 0; int has_write = 0;
                {
                    void *func_body = codegen->GetFuncBody(sg_kernel);
                    ArrayRangeAnalysis ris(formal_arg, codegen, func_body,
func_body, false, false);

                    has_read = ris.has_read();
                    has_write = ris.has_write();

                    if (has_read && has_write) //  currently Kestrel NAM does
not support READ_WRITE port
                    {
                        string msg = "Kernel interface variable with both read
and write accesses in the kernel is not allowed: " + string(""); string var_info
= "'" + codegen->UnparseToString(actual_arg) +
                            "' for kernel '" + codegen->GetFuncName(sg_kernel) +
                            "' " + getUserCodeFileLocation(codegen, sg_kernel,
true) +
                            "\n";
                        dump_critical_message(RUNTM_ERROR_4(var_info));
                        cout << msg << endl;
                        exit(-1);
                    }
                    if(has_write) {
                        index_alloc = write_index;
                        write_index++;
                    } else if(has_read) {
                        index_alloc = read_index;
                        read_index++;
                    }
                }

                //  2. insert new declaration
                vector<void*> params;
                params.push_back(codegen->CreateStringExp(codegen->GetFuncName(sg_kernel)));
                params.push_back(codegen->CreateConst(index_alloc));
                params.push_back(codegen->CreateConst(1));
                params.push_back(codegen->CreateConst(dim_size));
                //  params.push_back(codegen->CreateExp(V_SgMultiplyOp,
                //            codegen->CreateConst(dim_size),
                //            codegen->CreateExp(V_SgSizeOfOp, base_type)));
                params.push_back(codegen->CreateExp(V_SgSizeOfOp, base_type));
                params.push_back(codegen->CreateConst(has_write));
                void * sg_call = codegen->CreateFuncCall(
                        "kestrel_alloc",
                        base_type, params, codegen->GetScope(org_var_decl));
                sg_call = codegen->CreateCastExp(sg_call,
codegen->CreatePointerType(base_type) ); string s_var = "__" +
codegen->UnparseToString(actual_arg) + var_suffix; void * sg_var_decl =
codegen->CreateVariableDecl( codegen->CreatePointerType(base_type), s_var,
sg_call, codegen->GetScope(org_var_decl)); codegen->InsertAfterStmt(sg_var_decl,
org_var_decl);


                //  3. replace all the original references with the new one,
except malloc void * org_array =
codegen->GetVariableInitializedName(org_var_decl); vector<void*> org_ref =
codegen->GetAllRefInScope(org_array, codegen->GetScope(org_var_decl));

                for (size_t t = 0; t < org_ref.size(); t++)
                {
                    void * curr_ref = org_ref[t];

                    void *sg_pntr;
                    {
                        void *sg_array;
                        vector<void *> sg_indexes;
                        int pointer_dim;
                        codegen->parse_pntr_ref_by_array_ref(curr_ref,
&sg_array, &sg_pntr, &sg_indexes, &pointer_dim);
                    }

                    int arg_idx = codegen->GetFuncCallParamIndex(sg_pntr);
                    if (arg_idx != -1)
                    {
                        void * curr_call = codegen->TraceUpToFuncCall(sg_pntr);
                        assert(curr_call);
                        string curr_func_name =
codegen->GetFuncNameByCall(curr_call);

                        if (curr_func_name == "malloc") continue;
                        if (curr_func_name == codegen->GetFuncName(sg_kernel))
continue; if (curr_func_name.find("cmost_dump") == 0) continue; if
(curr_func_name.find("cmost_free") == 0) continue; if (curr_func_name ==
"cmost_malloc") continue; if (curr_func_name.find("cmost_malloc_") == 0)
                        {
                            void * input_file_name =
codegen->GetFuncCallParam(curr_call, 1);

                            //  create cmost_load_file call
                            if (codegen->UnparseToString(input_file_name) !=
"\"\"")
                            {
                                vector<void*> curr_params;
                                curr_params.push_back(codegen->CreateVariableRef(sg_var_decl));
                                curr_params.push_back(codegen->CopyExp(input_file_name));
                                curr_params.push_back(codegen->CreateExp(V_SgMultiplyOp,
                                            codegen->CreateConst(dim_size),
                                            codegen->CreateExp(V_SgSizeOfOp,
base_type))); void * new_file_call = codegen->CreateFuncCall( "cmost_load_file",
codegen->GetTypeByString("void"), curr_params, codegen->GetScope(curr_ref)
                                        );
                                void * new_stmt =
codegen->CreateStmt(V_SgExprStatement, new_file_call);
                                codegen->InsertAfterStmt(new_stmt,
codegen->TraceUpToStatement(curr_call));
                            }

                            continue;
                        }
                    }

                    codegen->ReplaceExp(curr_ref,
codegen->CreateVariableRef(sg_var_decl));
                }
            }
            else
            {
                string msg = "Multiple dimensional array is not supported in
kernel interface: " + string("") +
                    "\n\t ";
                string var_info = "'" + codegen->UnparseToString(actual_arg) +
                    "' for kernel '" + codegen->GetFuncName(sg_kernel) +
                    "' " + getUserCodeFileLocation(codegen, sg_kernel, true) +
"\n"; dump_critical_message(RUNTM_ERROR_3 (var_info)); cout << msg << endl;
                exit(-1);
            }


        }

        //  /////////////////////////////////////////////////  /
        //  5. add the include file "kestrel_app.h"
        //  /////////////////////////////////////////////////  /

        codegen->AddHeader("#include \"kestrel_app.h\"", sg_task);

        //  /////////////////////////////////////////////////  /
        //  4. replace kernel call
        //  /////////////////////////////////////////////////  /
        {
            //  kestrel_start_kernel("kernel_name");
            string str_kernel_call = codegen->UnparseToString(sg_task_stmt);

            vector<void*> params;
            params.push_back(codegen->CreateStringExp(codegen->GetFuncName(sg_kernel)));
            void * sg_call = codegen->CreateFuncCall("kestrel_start_kernel",
                codegen->GetTypeByString("void"), params,
codegen->GetScope(sg_task)); void * sg_stmt =
codegen->CreateStmt(V_SgExprStatement, sg_call);
            codegen->ReplaceStmt(sg_task_stmt, sg_stmt);
            codegen->AddComment(str_kernel_call, sg_stmt);

        }

    }
    printf("\n");
    return 1;
#endif
    //  No return, add by Yuxin Apr 2018
    return 1;
}
*************************************************************************************/

static int remove_opencl_keyword(CSageCodeGen *codegen, void *func,
                                 set<void *> *visited) {
  static const string opencl_keyword[] = {
      "__global",     "global",     "__local",      "local",
      "__constant",   "constant",   "__private",    "private",
      "__kernel",     "kernel",     "__read_only",  "read_only",
      "__write_only", "write_only", "__read_write", "read_write"};
  set<string> opencl_keyword_set;
  for (auto key : opencl_keyword) {
    opencl_keyword_set.insert(key);
  }

  if (visited->count(func) > 0) {
    return 0;
  }
  visited->insert(func);
  int ret = 0;
  vector<void *> vec_calls;
  codegen->GetNodesByType(func, "preorder", "SgFunctionCallExp", &vec_calls);
  for (auto call : vec_calls) {
    void *func_decl = codegen->GetFuncDeclByCall(call);
    if (func_decl == nullptr) {
      continue;
    }
    cout << "remove for function:" << codegen->_p(func_decl) << endl;
    remove_opencl_keyword(codegen, func_decl, visited);
  }

  vector<void *> vec_init;
  set<void *> set_init;
  codegen->GetNodesByType_int(func, "preorder", V_SgInitializedName, &vec_init);
  set_init.insert(vec_init.begin(), vec_init.end());
  vector<void *> vec_refs;
  codegen->GetNodesByType_int(func, "preorder", V_SgVarRefExp, &vec_refs);
  for (auto ref : vec_refs) {
    set_init.insert(codegen->GetVariableInitializedName(ref));
  }
  for (auto var : set_init) {
    string var_name = codegen->GetVariableName(var);
    if (opencl_keyword_set.count(var_name) > 0) {
      var_name += "_r";
      if (codegen->IsLocalInitName(var) != 0) {
        codegen->get_valid_local_name(func, &var_name);
      } else {
        codegen->get_valid_global_name(codegen->GetScope(func), &var_name);
      }
      codegen->SetVariableName(var, var_name);
      ret = 1;
    }
  }

  string func_name = codegen->GetFuncName(func);
  if (opencl_keyword_set.count(func_name) > 0) {
    func_name += "_r";
    codegen->get_valid_local_name(codegen->GetScope(func), &func_name);
    codegen->SetFuncName(func, func_name);
    ret = 1;
  }
  return ret;
}

int remove_opencl_keyword_top(CSageCodeGen *codegen,
                              vector<void *> vec_kernels) {
  cout << "\tEnter remove opencl keyword" << endl;
  set<void *> visited;
  int changed = 0;
  for (auto kernel : vec_kernels) {
    changed |= remove_opencl_keyword(codegen, kernel, &visited);
  }
  return changed;
}

static int remove_access_range_intrinsic(CSageCodeGen *codegen,
                                         void *pTopFunc) {
  int ret = 0;
  vector<void *> vec_decls;
  codegen->GetNodesByType(nullptr, "preorder", "SgFunctionDeclaration",
                          &vec_decls);
  vector<void *> vec_calls;
  codegen->GetNodesByType(nullptr, "preorder", "SgFunctionCallExp", &vec_calls);
  for (auto call : vec_calls) {
    string func_name = codegen->GetFuncNameByCall(call);
    if (!codegen->IsMerlinInternalIntrinsic(func_name)) {
      continue;
    }
    void *call_stmt = codegen->TraceUpToStatement(call);
    codegen->RemoveStmt(call_stmt);
    ret = 1;
  }
  return ret;
}

static int remove_dead_pragma(CSageCodeGen *codegen, void *pTopFunc) {
  int ret = 0;
  vector<void *> vec_decls;
  codegen->GetNodesByType(nullptr, "preorder", "SgPragmaDeclaration",
                          &vec_decls);
  for (auto decl : vec_decls) {
    if (codegen->IsPragmaWithDeadVariable(decl)) {
      codegen->AddComment_v1(codegen->GetScope(decl),
                             "Dead: #pragma " + codegen->GetPragmaString(decl));
      codegen->RemoveStmt(decl);
    }
  }
  return ret;
}

static int function_uniquify(CSageCodeGen *codegen, void *func_decl,
                                        CMarsIr *mars_ir, set<void *> kernels,
                                        set<void *> *visited_funcs) {
  if ((*visited_funcs).count(func_decl) > 0) {
    return 0;
  }
  vector<void *> vec_calls;
  codegen->GetFuncCallsFromDecl(func_decl, nullptr, &vec_calls);
  if (kernels.count(func_decl) <= 0) {
    //  check whether all the caller have been preprocessed
    for (auto call : vec_calls) {
      void *caller = codegen->TraceUpToFuncDecl(call);
      if (visited_funcs->count(caller) <= 0) {
        return 0;
      }
    }
  }
  visited_funcs->insert(func_decl);
  int ret = 0;
  if (kernels.count(func_decl) <= 0 && vec_calls.size() > 1) {
    size_t i;
    int j;
    bool need_changed = false;
    for (j = 0; j < codegen->GetFuncParamNum(func_decl); ++j) {
      void *param = codegen->GetFuncParam(func_decl, j);
      void *type = codegen->GetTypebyVar(param);
      if ((codegen->IsPointerType(type) == 0) &&
          (codegen->IsArrayType(type) == 0)) {
        continue;
      }
      //  check whether there are both local and global pointer/array
      if (mars_ir->every_trace_is_bus(codegen, func_decl, param, nullptr)) {
        continue;
      }
      map<void *, bool> res;
      if (mars_ir->any_trace_is_bus(codegen, func_decl, param, &res)) {
        need_changed = true;
        break;
      }
    }
    if (need_changed) {
      map<string, vector<void *>> group;
      for (i = 0; i < vec_calls.size(); ++i) {
        void *one_call = vec_calls[i];
        void *func_caller = codegen->TraceUpToFuncDecl(one_call);
        string p;
        for (j = 0; j < codegen->GetFuncParamNum(func_decl); ++j) {
          void *param = codegen->GetFuncParam(func_decl, j);
          void *type = codegen->GetTypebyVar(param);
          if ((codegen->IsPointerType(type) == 0) &&
              (codegen->IsArrayType(type) == 0)) {
            continue;
          }
          void *arg = codegen->GetFuncCallParam(one_call, j);
          void *var_ref = nullptr;
          codegen->parse_array_ref_from_pntr(arg, &var_ref);
          if ((var_ref != nullptr) && (func_caller != nullptr)) {
            void *sg_name = codegen->GetVariableInitializedName(var_ref);
            if (sg_name != nullptr) {
              if (mars_ir->every_trace_is_bus(codegen, func_caller, sg_name,
                                              one_call)) {
                p += "g";
                continue;
              }
            }
          }
          p += "l";
        }
        group[p].push_back(one_call);
      }
      if (group.size() > 1) {
        map<void *, void *> call2new_func;
        i = 0;
        for (auto p_vec_call : group) {
          if (i == 0) {
            ++i;
            continue;
          }
          string new_func_name =
              codegen->GetFuncName(func_decl) + "_" + my_itoa(i++);
          codegen->get_valid_global_name(codegen->GetScope(func_decl),
                                         &new_func_name);
          void *new_func_decl = codegen->CloneFuncDecl(
              func_decl, codegen->GetScope(func_decl), new_func_name, true);

          void *insert_pos = func_decl;
          while (codegen->IsPragmaDecl(codegen->GetPreviousStmt(insert_pos)) !=
                 0) {
            insert_pos = codegen->GetPreviousStmt(insert_pos);
          }
          codegen->InsertStmt(new_func_decl, insert_pos);
          visited_funcs->insert(new_func_decl);
          for (auto one_call : p_vec_call.second) {
            call2new_func[one_call] = new_func_decl;
          }
        }
        for (i = 0; i < vec_calls.size(); ++i) {
          void *one_call = vec_calls[i];
          if (call2new_func.count(one_call) <= 0) {
            continue;
          }
          void *new_func = call2new_func[one_call];
          call2new_func.erase(one_call);
          vector<void *> vec_args;
          for (j = 0; j < codegen->GetFuncCallParamNum(one_call); ++j) {
            vec_args.push_back(
                codegen->CopyExp(codegen->GetFuncCallParam(one_call, j)));
          }
          void *new_call =
              codegen->CreateFuncCall(new_func, vec_args, one_call);
          codegen->ReplaceExp(one_call, new_call);
          setFuncDeclUserCodeScopeId(codegen, new_func, new_call);
          ret = 1;
        }
        codegen->reset_func_call_cache();
        mars_ir->clear_port_is_bus_cache();
      }
    }
  }
  vec_calls.clear();
  codegen->GetNodesByType(func_decl, "preorder", "SgFunctionCallExp",
                          &vec_calls);
  vector<void *> vec_sub_funcs;
  for (auto call : vec_calls) {
    void *sub_func = codegen->GetFuncDeclByCall(call, 1);
    if ((sub_func == nullptr) || kernels.count(sub_func) > 0) {
      continue;
    }
    vec_sub_funcs.push_back(sub_func);
  }
  for (auto sub_func : vec_sub_funcs) {
    ret |= function_uniquify(codegen, sub_func, mars_ir, kernels,
                                        visited_funcs);
  }
  //  ALGOPP("FIN", "TIME for function " << printNodeInfo(codegen, func_call));
  return ret;
}

int function_uniquify_top(CSageCodeGen *codegen, void *pTopFunc,
                          const CInputOptions &options,
                          vector<void *> vec_kernels) {
  printf("    Enter function uniquify...\n");
  int ret = 0;
  CMarsIr mars_ir;
  mars_ir.get_mars_ir(codegen, pTopFunc, options);
  auto kernel_subkernel = GetKernelMap(codegen, pTopFunc, options);
  set<void *> kernels;
  for (auto one : kernel_subkernel) {
    kernels.insert(one.first);
    kernels.insert(one.second.begin(), one.second.end());
  }
  set<void *> visited_funcs;
  //  codegen->reset_func_decl_cache();
  //  codegen->reset_func_call_cache();
  for (auto sg_kernel : vec_kernels) {
    ret |= function_uniquify(codegen, sg_kernel, &mars_ir, kernels,
                                        &visited_funcs);
  }
  return ret;
}

static void opencl_kernel_add_extern_C(CSageCodeGen *codegen, void *pTopFunc,
                                       vector<void *> vec_kernels) {
  for (size_t k = 0; k < vec_kernels.size(); k++) {
    void *func = vec_kernels[k];
    assert(func);
    void *body = codegen->GetFuncBody(func);
    assert(func && body);
    if (codegen->isWithInCppFile(func)) {
      void *pos = codegen->CreateVariableDecl(
          "int", codegen->GetFuncName(func) + "_dummy_pos", nullptr,
          codegen->GetParent(func));
      codegen->SetStatic(pos);
      void *insert_pos = func;
      while (codegen->IsPragmaDecl(codegen->GetPreviousStmt(insert_pos)) != 0) {
        insert_pos = codegen->GetPreviousStmt(insert_pos);
      }
      codegen->InsertStmt(pos, insert_pos);
      codegen->AddDirectives("\nextern \"C\" { \n", func, 1);
      codegen->AddDirectives("}\n", func, 0);
    }
  }
}

//  collect all exist variablesin kernel
//  avoid global and local conflict
static vector<string>
get_global_and_local_variables(CSageCodeGen *codegen, void *pTopFunc,
                               vector<void *> vec_kernels) {
  vector<string> v_variable;
  vector<void *> vec_decls;
  codegen->GetNodesByType(codegen->GetProject(), "preorder",
                          "SgVariableDeclaration", &vec_decls);
  for (size_t k = 0; k < vec_decls.size(); k++) {
    //      cout << "decl variable: " << codegen->_p(vec_decls[k]) << endl;
    void *var_init = codegen->GetVariableInitializedName(vec_decls[k]);
    v_variable.push_back(codegen->UnparseToString(var_init));
  }
  return v_variable;
}

//  change function name for both function declaration and call
static void change_function_name(CSageCodeGen *codegen, void *pTopFunc,
                                 void *func, string new_func_name) {
  vector<void *> vec_calls;
  codegen->GetFuncCallsFromDecl(func, pTopFunc, &vec_calls);
  vector<void *> func_decls = codegen->GetAllFuncDecl(func);
  void *new_func = nullptr;
  for (auto decl : func_decls) {
    void *body = codegen->GetFuncBody(decl);
    if (body == nullptr) {
      void *clone_decl = codegen->CloneFuncDecl(func, codegen->GetGlobal(func),
                                                new_func_name, false);
      codegen->ReplaceStmt(decl, clone_decl);
      new_func = clone_decl;
    }
  }
  if (codegen->GetFuncBody(func) != nullptr) {
    void *clone_decl = codegen->CloneFuncDecl(func, codegen->GetGlobal(func),
                                              new_func_name, true);
    codegen->ReplaceStmt(func, clone_decl);
    if (new_func == nullptr)
      new_func = clone_decl;
  }
  if (new_func != nullptr) {
    for (auto call : vec_calls) {
      vector<void *> call_param_list = codegen->GetFuncCallParamList(call);
      void *func_call = codegen->CreateFuncCall(
          new_func, codegen->CopyExprList(call_param_list),
          codegen->GetGlobal(call));
      codegen->ReplaceExp(call, func_call);
    }
  }
}

//  change function name tp xxx_merlin_clonex
static void check_and_change_function_name(CSageCodeGen *codegen,
                                           void *pTopFunc,
                                           vector<void *> vec_kernels,
                                           vector<string> exist_name) {
  for (size_t k = 0; k < vec_kernels.size(); k++) {
    vector<void *> vec_funs;
    codegen->GetNodesByType(vec_kernels[k], "preorder", "SgFunctionDeclaration",
                            &vec_funs, false, true);
    for (auto func : vec_funs) {
      // do not change the name of kernel function itself
      // otherwise it may cause the mismatch of the wrapper and kernel
      string func_name = codegen->GetFuncName(func);
      if (func == vec_kernels[k]) {
        exist_name.push_back(func_name);
        // Han
        // add legalize kernel name here, because it the legalized name may used
        // at the last step: legalize_kernel
        // can avoid conflict legalized kernel name conflict with other sub func
        exist_name.push_back(legalize_kernel_name(func_name));
        break;
      }
    }
  }
  for (size_t k = 0; k < vec_kernels.size(); k++) {
    vector<void *> vec_funs;
    codegen->GetNodesByType(vec_kernels[k], "preorder", "SgFunctionDeclaration",
                            &vec_funs, false, true);
    for (auto func : vec_funs) {
      string func_name = codegen->GetFuncName(func);
      cout << "checking func_name: " << func_name << endl;
      // if kernel name, do not change name and continue
      if (func == vec_kernels[k]) {
        continue;
      }
      if (std::find(exist_name.begin(), exist_name.end(), func_name) !=
              exist_name.end() &&
          !codegen->isWithInHeaderFile(func)) {
        cout << "find duplicate function name: " << func_name << endl;
        int suffix = 0;
        string new_func_name = func_name + "_merlin_clone" + my_itoa(suffix);
        //  check if new name also exist
        while (suffix >= 0) {
          if (std::find(exist_name.begin(), exist_name.end(), new_func_name) !=
              exist_name.end()) {
            suffix++;
            new_func_name = func_name + "_merlin_clone" + my_itoa(suffix);
          } else {
            break;
          }
        }
        change_function_name(codegen, pTopFunc, func, new_func_name);
        exist_name.push_back(new_func_name);
      } else {
        exist_name.push_back(func_name);
      }
    }
  }
}

//  change function name if find duplicate name
static void set_unique_function_name_top(CSageCodeGen *codegen, void *pTopFunc,
                                         vector<void *> vec_kernels) {
  printf("    Enter set_unique_function_name_top...\n");
  //  collect all global variable name
  vector<string> exist_name =
      get_global_and_local_variables(codegen, pTopFunc, vec_kernels);
  check_and_change_function_name(codegen, pTopFunc, vec_kernels, exist_name);
}

//  change function name tp xxx_merlin_clonex
static void check_and_short_function_name(CSageCodeGen *codegen, void *pTopFunc,
                                          vector<void *> vec_kernels,
                                          vector<string> exist_name) {
  printf("\t\tEnter check_and_short_function_name\n");
  for (size_t k = 0; k < vec_kernels.size(); k++) {
    vector<void *> vec_funs;
    codegen->GetNodesByType(vec_kernels[k], "preorder", "SgFunctionDeclaration",
                            &vec_funs, false, true);
    for (auto func : vec_funs) {
      //  skip the top kernel
      if (func == vec_kernels[k]) {
        continue;
      }
      //  skip template/member/system functions
      if ((codegen->IsTemplateInstantiationFunctionDecl(func) != 0) ||
          (codegen->IsTemplateInstantiationMemberFunctionDecl(func) != 0) ||
          codegen->IsMemberFunction(func) ||
          (codegen->IsTemplateFunctionDecl(func) != 0) ||
          codegen->isWithInHeaderFile(func)) {
        continue;
      }
      string func_name = codegen->GetFuncName(func);
      int length = func_name.length();
      if (length > MAX_FUNC_NAME) {
        cout << "find long function name: " << func_name << endl;
        string new_func_name = func_name.substr(0, MAX_FUNC_NAME);
        //      cout << "func_name: " << func_name << endl;
        if (std::find(exist_name.begin(), exist_name.end(), new_func_name) !=
                exist_name.end() &&
            !codegen->isWithInHeaderFile(func)) {
          cout << "find duplicate function name: " << new_func_name << endl;
          int suffix = 0;
          new_func_name += my_itoa(suffix);
          //  check if new name also exist
          while (suffix >= 0) {
            if (std::find(exist_name.begin(), exist_name.end(),
                          new_func_name) != exist_name.end()) {
              suffix++;
              new_func_name += my_itoa(suffix);
            } else {
              break;
            }
          }
        }
        change_function_name(codegen, pTopFunc, func, new_func_name);
        exist_name.push_back(new_func_name);
        cout << "\tshort function name: " << new_func_name << endl;
      } else {
        exist_name.push_back(func_name);
      }
    }
  }
}

static void shorten_function_name_top(CSageCodeGen *codegen, void *pTopFunc,
                                      vector<void *> vec_kernels) {
  printf("    Enter shorten_function_name_top...\n");
  //  collect all global variable name
  vector<string> exist_name;
  check_and_short_function_name(codegen, pTopFunc, vec_kernels, exist_name);
}

void ReplaceMemcpyByBruteForce(CSageCodeGen *codegen, void *expr) {
  if (codegen->IsFunctionCall(expr) == 0) {
    return;
  }

  string func_name = codegen->GetFuncNameByCall(expr);
  if (func_name != "memcpy") {
    return;
  }

  void *memcpy_stmt = codegen->TraceUpToStatement(expr);
  if (codegen->IsExprStatement(memcpy_stmt) == 0) {
    return;
  }
  void *scope = codegen->GetGlobal(memcpy_stmt);

  vector<void *> args = codegen->GetFuncCallParamList(expr);
  assert(args.size() == 3 && "invalid memcpy arg number");

  void *iter_init_exp = codegen->CreateConst(0);
  //  int __memcpy_br_iter_ = 0;
  string iter_name = "__memcpy_br_iter";
  void *for_init =
      codegen->CreateVariableDecl("int", iter_name, iter_init_exp, scope);
  //  merlin_memcpy_iter_i < N
  void *for_cond_var = codegen->CreateVariableRef(for_init, scope);
  void *for_cond_bound = codegen->CopyExp(args[2]);
  void *for_cond_exp =
      codegen->CreateExp(V_SgLessThanOp, for_cond_var, for_cond_bound);
  void *for_cond_stmt = codegen->CreateStmt(V_SgExprStatement, for_cond_exp);

  //  merlin_memcpy_iter_i++
  void *for_inc_var = codegen->CreateVariableRef(for_init, scope);
  void *for_inc = codegen->CreateExp(V_SgPlusPlusOp, for_inc_var);

  //  *((char*)pdst + i) = *((char*)psrc + i);
  void *dst_arg = codegen->CopyExp(args[0]);
  void *dst_cast = codegen->CreateCastExp(dst_arg, "char *");
  void *dst_iter_var = codegen->CreateVariableRef(for_init, scope);
  void *dst_add_iter = codegen->CreateExp(V_SgAddOp, dst_cast, dst_iter_var);
  void *dst_deref = codegen->CreateExp(V_SgPointerDerefExp, dst_add_iter);

  void *src_arg = codegen->CopyExp(args[1]);
  void *src_cast = codegen->CreateCastExp(src_arg, "char *");
  void *src_iter_var = codegen->CreateVariableRef(for_init, scope);
  void *src_add_iter = codegen->CreateExp(V_SgAddOp, src_cast, src_iter_var);
  void *src_deref = codegen->CreateExp(V_SgPointerDerefExp, src_add_iter);

  void *replace_exp = codegen->CreateExp(V_SgAssignOp, dst_deref, src_deref);
  void *replace_stmt = codegen->CreateStmt(V_SgExprStatement, replace_exp);
  void *for_bb = codegen->CreateBasicBlock(replace_stmt);

  //  finish for statement
  void *for_stmt =
      codegen->CreateForLoop(for_init, for_cond_stmt, for_inc, for_bb);

  //  replace
  codegen->ReplaceStmt(memcpy_stmt, for_stmt);
}

static int
replace_memcpy_with_for_loop_in_function(CSageCodeGen *codegen, void *func_call,
                                         int suffix,
                                         vector<void *> vec_kernels) {
  void *func_decl = codegen->TraceUpToFuncDecl(func_call);
  void *sg_array1 = nullptr;
  void *sg_pntr1 = nullptr;
  vector<void *> sg_idx1;
  int pointer_dim1;
  void *sg_array2 = nullptr;
  void *sg_pntr2 = nullptr;
  vector<void *> sg_idx2;
  int pointer_dim2;
  void *sg_length;

  int ret = codegen->parse_memcpy(func_call, &sg_length, &sg_array1, &sg_pntr1,
                                  &sg_idx1, &pointer_dim1, &sg_array2,
                                  &sg_pntr2, &sg_idx2, &pointer_dim2);
  if (ret == 0) {
#if PROJDEBUG
    cout << "cannot inline memcpy " << codegen->_p(func_call) << endl;
#endif
    ReplaceMemcpyByBruteForce(codegen, func_call);
    return 0;
  }

  void *array1_type_var = codegen->GetTypebyVar(sg_array1);
  int type_size1 = codegen->get_type_unit_size(
      codegen->RemoveCast(array1_type_var), sg_array1);
  void *array1_basic_type = nullptr;
  vector<size_t> array1_basic_size;
  codegen->get_type_dimension(array1_type_var, &array1_basic_type,
                              &array1_basic_size, sg_array1);
  void *array2_type_var = codegen->GetTypebyVar(sg_array2);
  int type_size2 = codegen->get_type_unit_size(
      codegen->RemoveCast(array2_type_var), sg_array2);
  void *array2_basic_type = nullptr;
  vector<size_t> array2_basic_size;
  codegen->get_type_dimension(array2_type_var, &array2_basic_type,
                              &array2_basic_size, sg_array2);
  if ((array1_basic_type == nullptr) ||
      codegen->GetBaseType(array1_basic_type, true) !=
          codegen->GetBaseType(array2_basic_type, true) ||
      type_size1 == 0) {
#if PROJDEBUG
    cout << "cannot inline memcpy " << codegen->_p(func_call)
         << " because of different types" << endl;
#endif
    return 0;
    //  ReplaceMemcpyByBruteForce(codegen, func_call);
    //  return 1;
  }

  assert(type_size1 == type_size2);
  if (handle_single_assignment(codegen, sg_array1, sg_idx1, sg_array2, sg_idx2,
                               sg_length, type_size1, func_call) != 0) {
    return 1;
  }
  int dimension1 = array1_basic_size.size();
  int opt_dimension1 = dimension1;
  {
    //  Youxiang 20170409
    //  if offset + length is less than the lowest dimension,
    //  we can pass partial access to be friendly to HLS
    int element_bitwidth =
        codegen->get_bitwidth_from_type(array1_basic_type, false);
    if (element_bitwidth != 0) {
      int curr_dim = 0;
      int element_bytes = element_bitwidth / 8;
      CMarsExpression me_total_offset =
          (CMarsExpression(sg_length, codegen, func_call) / element_bytes);
      int64_t curr_size = 1;
      while (curr_dim < dimension1) {
        void *curr_index = sg_idx1[dimension1 - 1 - curr_dim];
        CMarsExpression me_curr_offset =
            CMarsExpression(curr_index, codegen, func_call) * curr_size;
        me_total_offset = me_total_offset + me_curr_offset;
        CMarsRangeExpr mr_total_offset = me_total_offset.get_range();
        int64_t c_ub = mr_total_offset.get_const_ub();
        curr_size *= array1_basic_size[curr_dim];
        if (c_ub <= curr_size) {
          opt_dimension1 = curr_dim + 1;
          break;
        }
        curr_dim++;
      }
    }
  }

  int dimension2 = array2_basic_size.size();
  int opt_dimension2 = dimension2;
  {
    //  Youxiang 20170409
    //  if offset + length is less than the lowest dimension,
    //  we can pass partial access to be friendely to HLS
    int element_bitwidth =
        codegen->get_bitwidth_from_type(array2_basic_type, false);
    if (element_bitwidth != 0) {
      int curr_dim = 0;
      int element_bytes = element_bitwidth / 8;
      CMarsExpression me_total_offset =
          (CMarsExpression(sg_length, codegen, func_call) / element_bytes);
      int64_t curr_size = 1;
      while (curr_dim < dimension2) {
        void *curr_index = sg_idx2[dimension2 - 1 - curr_dim];
        CMarsExpression me_curr_offset =
            CMarsExpression(curr_index, codegen, func_call) * curr_size;
        me_total_offset = me_total_offset + me_curr_offset;
        CMarsRangeExpr mr_total_offset = me_total_offset.get_range();
        int64_t c_ub = mr_total_offset.get_const_ub();
        curr_size *= array2_basic_size[curr_dim];
        if (c_ub <= curr_size) {
          opt_dimension2 = curr_dim + 1;
          break;
        }
        curr_dim++;
      }
    }
  }

  string merlin_memcpy_name = "merlin_memcpy_" + my_itoa(suffix);
  string instance_var_list;
  vector<void *> fp_list;
  vector<void *> arg_list;
  vector<void *> fp_idx1;
  vector<void *> fp_idx2;
  vector<size_t> new_array1_basic_size;
  size_t max_len;
  size_t burst_size_1 = 1;
  size_t burst_size_2 = 1;
  int type_size = type_size1;
  for (int i = 0; i < opt_dimension1; ++i) {
    size_t curr_size = array1_basic_size[i];
    new_array1_basic_size.push_back(curr_size);
    burst_size_1 *= curr_size;
  }
  void *new_array1_type_var = codegen->CreateArrayType(
      array1_basic_type, vector<size_t>(new_array1_basic_size.rbegin(),
                                        new_array1_basic_size.rend()));
  vector<size_t> new_array2_basic_size;
  for (int i = 0; i < opt_dimension2; ++i) {
    size_t curr_size = array2_basic_size[i];
    new_array2_basic_size.push_back(curr_size);
    burst_size_2 *= curr_size;
  }
  max_len = std::max(burst_size_1, burst_size_2);
  void *new_array2_type_var = codegen->CreateArrayType(
      array2_basic_type, vector<size_t>(new_array2_basic_size.rbegin(),
                                        new_array2_basic_size.rend()));
  void *fp_dst = codegen->CreateVariable(new_array1_type_var, "dst");
  void *fp_src = codegen->CreateVariable(new_array2_type_var, "src");
  void *fp_len =
      codegen->CreateVariable(codegen->GetTypeByExp(sg_length), "len");
  void *fp_max_len =
      codegen->CreateVariable(codegen->GetTypeByExp(sg_length), "max_len");
  CMarsExpression me_length(sg_length, codegen, func_call);
  size_t length_upper = me_length.get_range().get_const_ub() / type_size;
  if (length_upper < max_len) {
    max_len = length_upper;
  }

  void *dst_arg = codegen->CreateVariableRef(sg_array1);
  for (int i = 0; i < dimension1 - opt_dimension1; ++i) {
    dst_arg = codegen->CreateExp(V_SgPntrArrRefExp, dst_arg,
                                 codegen->CopyExp(sg_idx1[i]));
  }
  fp_list.push_back(fp_dst);
  arg_list.push_back(dst_arg);
  for (int i = dimension1 - opt_dimension1; i < dimension1; ++i) {
    void *idx = sg_idx1[i];
    string offset_var_name = "dst_idx_" + my_itoa(i);
    void *fp_idx =
        codegen->CreateVariable(codegen->GetTypeByExp(idx), offset_var_name);
    fp_idx1.push_back(fp_idx);
    fp_list.push_back(fp_idx);
    if (!instance_var_list.empty()) {
      instance_var_list += ",";
    }
    instance_var_list += offset_var_name;
    arg_list.push_back(codegen->CopyExp(idx));
  }
  void *src_arg = codegen->CreateVariableRef(sg_array2);
  for (int i = 0; i < dimension2 - opt_dimension2; ++i) {
    src_arg = codegen->CreateExp(V_SgPntrArrRefExp, src_arg,
                                 codegen->CopyExp(sg_idx2[i]));
  }
  fp_list.push_back(fp_src);
  arg_list.push_back(src_arg);
  for (int i = dimension2 - opt_dimension2; i < dimension2; ++i) {
    void *idx = sg_idx2[i];
    string offset_var_name = "src_idx_" + my_itoa(i);
    void *fp_idx =
        codegen->CreateVariable(codegen->GetTypeByExp(idx), offset_var_name);
    fp_idx2.push_back(fp_idx);
    fp_list.push_back(fp_idx);
    if (!instance_var_list.empty()) {
      instance_var_list += ",";
    }
    instance_var_list += offset_var_name;
    arg_list.push_back(codegen->CopyExp(idx));
  }

  fp_list.push_back(fp_len);
  arg_list.push_back(codegen->CopyExp(sg_length));
  fp_list.push_back(fp_max_len);
  arg_list.push_back(codegen->CreateConst(max_len * type_size));
  //  create function declaration
  void *merlin_memcpy_decl =
      codegen->CreateFuncDecl("void", merlin_memcpy_name, fp_list,
                              codegen->GetGlobal(func_call), true, func_call);
  void *new_func_body = codegen->GetFuncBody(merlin_memcpy_decl);
  //  implement function
  //  add inline off in the function
  string inline_off_pragma_str = "HLS inline off";
  void *inline_off_pragma =
      codegen->CreatePragma(inline_off_pragma_str, new_func_body);
  codegen->AppendChild(new_func_body, inline_off_pragma);
  //  YX: 20170711 Bug1163
  //  add function instantiation
  string function_instantiation_str =
      "HLS function_instantiate variable=" + instance_var_list;
  void *function_instantiation_pragma =
      codegen->CreatePragma(function_instantiation_str, new_func_body);
  codegen->AppendChild(new_func_body, function_instantiation_pragma);

  assert((size_t)pointer_dim1 <= sg_idx1.size() - 1);  //  check pointer_dim
  assert((size_t)pointer_dim2 <= sg_idx2.size() - 1);
  void *sg_index_i =
      codegen->CreateVariableDecl("unsigned long", "i", nullptr, new_func_body);
  codegen->AppendChild(new_func_body, sg_index_i);
  void *index_name = codegen->GetVariableInitializedName(sg_index_i);

  //  generate leftside of "="
  vector<void *> array1_index;
  void *sg_total_offset1 = codegen->CreateConst(0);
  for (int i = 0; i < opt_dimension1; i++) {
    sg_total_offset1 =
        codegen->CreateExp(V_SgMultiplyOp, sg_total_offset1,
                           codegen->CreateConst(static_cast<int>(
                               new_array1_basic_size[opt_dimension1 - 1 - i])));
    sg_total_offset1 = codegen->CreateExp(
        V_SgAddOp, sg_total_offset1, codegen->CreateVariableRef(fp_idx1[i]));
  }
  string s_total_offset1 = "total_offset1";
  void *sg_total_offset_decl1 = codegen->CreateVariableDecl(
      "unsigned long", s_total_offset1, sg_total_offset1, new_func_body);
  codegen->AppendChild(new_func_body, sg_total_offset_decl1);
  bool zero_size_1 = false;
  for (int i = 0; i < opt_dimension1 - 1; ++i) {
    if (new_array1_basic_size[i] == 0) {
      zero_size_1 = true;
      break;
    }
  }
  if (zero_size_1) {
    for (int i = 0; i < opt_dimension1 - 1; i++) {
      array1_index.push_back(codegen->CreateConst(0));
    }
    array1_index.push_back(codegen->CreateExp(
        V_SgAddOp, codegen->CreateVariableRef(sg_total_offset_decl1),
        codegen->CreateVariableRef(sg_index_i)));
  } else {
    for (int i = 0; i < opt_dimension1; i++) {
      array1_index.push_back(codegen->CreateExp(
          V_SgAddOp, codegen->CreateVariableRef(sg_total_offset_decl1),
          codegen->CreateVariableRef(sg_index_i)));
    }

    for (int i = 0; i < opt_dimension1 - 1; i++) {
      for (int j = 0; j < opt_dimension1 - i; j++) {
        if (j == opt_dimension1 - i - 1) {
          array1_index[j] = codegen->CreateExp(
              V_SgModOp, array1_index[j],
              codegen->CreateConst(static_cast<int>(new_array1_basic_size[i])));
        } else {
          array1_index[j] = codegen->CreateExp(
              V_SgDivideOp, array1_index[j],
              codegen->CreateConst(static_cast<int>(new_array1_basic_size[i])));
        }
      }
    }
  }

  void *array1_ref =
      codegen->CreateArrayRef(codegen->CreateVariableRef(fp_dst), array1_index);

  //  generate rightside of "="

  vector<void *> array2_index;
  void *sg_total_offset2 = codegen->CreateConst(0);
  for (int i = 0; i < opt_dimension2; i++) {
    sg_total_offset2 =
        codegen->CreateExp(V_SgMultiplyOp, sg_total_offset2,
                           codegen->CreateConst(static_cast<int>(
                               new_array2_basic_size[opt_dimension2 - i - 1])));
    sg_total_offset2 = codegen->CreateExp(
        V_SgAddOp, sg_total_offset2, codegen->CreateVariableRef(fp_idx2[i]));
  }
  string s_total_offset2 = "total_offset2";
  void *sg_total_offset_decl2 = codegen->CreateVariableDecl(
      "unsigned long", s_total_offset2, sg_total_offset2, new_func_body);
  codegen->AppendChild(new_func_body, sg_total_offset_decl2);
  bool zero_size_2 = false;
  for (int i = 0; i < opt_dimension2 - 1; ++i) {
    if (new_array2_basic_size[i] == 0) {
      zero_size_2 = true;
      break;
    }
  }
  if (zero_size_2) {
    for (int i = 0; i < opt_dimension2 - 1; i++) {
      array2_index.push_back(codegen->CreateConst(0));
    }
    array2_index.push_back(codegen->CreateExp(
        V_SgAddOp, codegen->CreateVariableRef(sg_total_offset_decl2),
        codegen->CreateVariableRef(sg_index_i)));
  } else {
    for (int i = 0; i < opt_dimension2; i++) {
      array2_index.push_back(codegen->CreateExp(
          V_SgAddOp, codegen->CreateVariableRef(sg_total_offset_decl2),
          codegen->CreateVariableRef(sg_index_i)));
    }

    for (int i = 0; i < opt_dimension2 - 1; i++) {
      for (int j = 0; j < opt_dimension2 - i; j++) {
        if (j == opt_dimension2 - i - 1) {
          array2_index[j] = codegen->CreateExp(
              V_SgModOp, array2_index[j],
              codegen->CreateConst(static_cast<int>(new_array2_basic_size[i])));
        } else {
          array2_index[j] = codegen->CreateExp(
              V_SgDivideOp, array2_index[j],
              codegen->CreateConst(static_cast<int>(new_array2_basic_size[i])));
        }
      }
    }
  }

  void *array2_ref =
      codegen->CreateArrayRef(codegen->CreateVariableRef(fp_src), array2_index);

  void *transfer_exp = codegen->CreateExp(V_SgAssignOp, array1_ref, array2_ref);
  void *sg_length_c =
      codegen->CreateExp(V_SgDivideOp, codegen->CreateVariableRef(fp_len),
                         codegen->CreateConst(type_size));
  void *for_body_stmt = codegen->CreateStmt(V_SgExprStatement, transfer_exp);
  void *new_loop_body = codegen->CreateBasicBlock();
  string pipeline_pragma_str = std::string(HLS_PRAGMA) + " " + HLS_PIPELINE +
                               " " + HLS_PIPELINE_II + "=1";
  void *pipeline_pragma =
      codegen->CreatePragma(pipeline_pragma_str, new_loop_body);
  codegen->AppendChild(new_loop_body, pipeline_pragma);
  string tripcount_pragma_str = std::string(HLS_PRAGMA) + " " + HLS_TRIPCOUNT +
                                " " + HLS_MAX + "=" + my_itoa(max_len);
  void *tripcount_pragma =
      codegen->CreatePragma(tripcount_pragma_str, new_loop_body);
  codegen->AppendChild(new_loop_body, tripcount_pragma);
  codegen->AppendChild(new_loop_body, for_body_stmt);
  void *for_loop_stmt =
      codegen->CreateStmt(V_SgForStatement, index_name,  //  index_type,
                          codegen->CreateConst(0), sg_length_c, new_loop_body,
                          nullptr, nullptr, func_call);
  codegen->AppendChild(new_func_body, for_loop_stmt);

  //  insert function declaration
  void *insert_pos = func_decl;
  while (codegen->IsPragmaDecl(codegen->GetPreviousStmt(insert_pos)) != 0) {
    insert_pos = codegen->GetPreviousStmt(insert_pos);
  }
  codegen->InsertStmt(merlin_memcpy_decl, insert_pos);
  codegen->SetStatic(merlin_memcpy_decl);

  //  create new function call
  void *new_func_call =
      codegen->CreateFuncCall(merlin_memcpy_decl, arg_list, func_call);
  codegen->ReplaceExp(func_call, new_func_call);
  setFuncDeclUserCodeScopeId(codegen, merlin_memcpy_decl, new_func_call);
  setQoRMergeMode(codegen, new_func_call, "major");
  return 1;
}

static int
replace_memcpy_with_for_loop_in_function(CSageCodeGen *codegen, void *pTopFunc,
                                         vector<void *> vec_kernels) {
  cout << "Enter replace memcpy with for loop in function" << endl;
  set<void *> all_funcs;
  for (auto func : vec_kernels) {
    SetVector<void *> sub_decls;
    SetVector<void *> sub_calls;
    codegen->GetSubFuncDeclsRecursively(func, &sub_decls, &sub_calls);
    all_funcs.insert(sub_decls.begin(), sub_decls.end());
    all_funcs.insert(func);
  }

  bool Changed = false;
  int suffix = 0;
  for (auto decl : all_funcs) {
    vector<void *> vec_calls;
    vector<void *> vec_memcpy;
    codegen->GetNodesByType(decl, "preorder", "SgFunctionCallExp", &vec_calls);
    for (auto call : vec_calls) {
      string func_name = codegen->GetFuncNameByCall(call);
      if (func_name != "memcpy") {
        continue;
      }
      vec_memcpy.push_back(call);
    }
    for (auto call : vec_memcpy) {
      cout << "find one memcpy call: " << codegen->_p(call) << endl;
      replace_memcpy_with_for_loop_in_function(codegen, call, suffix++,
                                               vec_kernels);
      Changed = true;
    }
  }
  return static_cast<int>(Changed);
}

static int add_loop_label(CSageCodeGen *codegen, void *pTopFunc,
                          vector<void *> vec_kernels) {
  cout << "Enter adding loop label" << endl;
  SetVector<void *> all_funcs;
  for (auto func : vec_kernels) {
    SetVector<void *> sub_decls;
    SetVector<void *> sub_calls;
    codegen->GetSubFuncDeclsRecursively(func, &sub_decls, &sub_calls);
    all_funcs.insert(sub_decls.begin(), sub_decls.end());
    all_funcs.insert(func);
  }

  bool Changed = false;
  set<string> used_labels;
  vector<void *> all_loops_without_label;
  for (auto decl : all_funcs) {
    vector<void *> vec_labels;
    codegen->GetNodesByType_int(decl, "preorder", V_SgLabelStatement,
                                &vec_labels);
    for (auto label : vec_labels) {
      string orig_label_name = codegen->GetLabelName(label);
      string new_label_name = orig_label_name;
      //  replace '_'
      std::replace(new_label_name.begin(), new_label_name.end(), '_', 'u');
      if (new_label_name != orig_label_name ||
          used_labels.count(new_label_name) > 0) {
        int suffix = 0;
        while (used_labels.count(new_label_name + my_itoa(suffix)) > 0) {
          suffix++;
        }
        new_label_name = new_label_name + my_itoa(suffix);
        codegen->SetLabelName(label, new_label_name);
      }
      used_labels.insert(new_label_name);
    }
    vector<void *> vec_loops;
    codegen->GetNodesByType_compatible(decl, "SgLoopStatement", &vec_loops);
    for (size_t i = vec_loops.size(); i > 0; --i) {
      // post order processing
      void *loop = vec_loops[i - 1];
      if ((codegen->IsFromInputFile(loop) == 0 &&
           codegen->IsTransformation(loop) == 0) ||
          codegen->isWithInHeaderFile(loop)) {
        continue;
      }
      if (codegen->IsLabelStatement(codegen->GetParent(loop)) == 0) {
        all_loops_without_label.push_back(loop);
      }
    }
  }
  int suffix = 0;
  string new_label = "merlinL";
  for (auto loop : all_loops_without_label) {
    while (used_labels.count(new_label + my_itoa(suffix)) > 0) {
      suffix++;
    }
    used_labels.insert(new_label + my_itoa(suffix));
    Changed |= codegen->add_label(new_label + my_itoa(suffix), loop);
  }
  return static_cast<int>(Changed);
}

//=======================================================//
// post wrapper process
//=======================================================//
int post_process(
    CSageCodeGen *codegen, void *pTopFunc, const CInputOptions &options,
    const map<string, map<string, string>> &task_kernel,
    const map<string, map<string, string>> &kernel_attributes,
    const map<string, map<string, map<string, string>>> &kernel_port_attributes,
    map<void *, vector<void *>> *kernel_autokernel,
    const vector<string> &vec_auto_kernel, string impl_tool,
    string tool_version, string pcie_transfer_opt, bool opencl_disable) {
  printf("    Enter post process...\n");
  //  get all tasks to vector
  vector<string> vec_tasks;
  map<string, map<string, string>>::const_iterator iter_task;
  for (iter_task = task_kernel.begin(); iter_task != task_kernel.end();
       iter_task++) {
    string task_name = iter_task->first;
    vec_tasks.push_back(task_name);
  }

  //  get all function declarations
  vector<void *> vec_kernels;
  map<string, map<string, string>>::const_iterator iter0;
  for (iter0 = kernel_attributes.begin(); iter0 != kernel_attributes.end();
       iter0++) {
    string kernel_name = iter0->first;
    if (std::find(vec_tasks.begin(), vec_tasks.end(), kernel_name) !=
        vec_tasks.end()) {
      continue;
    }
    vec_kernels.push_back(codegen->GetFuncDeclByName(kernel_name));
  }

  //    if ("kestrel" == options.get_option("-a")) {
  //        __merlin_host_app_wrapper_for_kestrel(codegen, pTopFunc);
  //        return 1;
  //    }

  remove_access_range_intrinsic(codegen, pTopFunc);
  InputChecker instance(codegen, pTopFunc, options);
  instance.CheckKernelArgumentNumber(vec_kernels, impl_tool);
  remove_dead_pragma(codegen, pTopFunc);

  auto extern_c_flag = options.get_option_key_value("-a", "extern_c");
  boost::to_lower(extern_c_flag);
  if ((impl_tool == "sdaccel" && tool_version != "vivado_hls" &&
       tool_version != "vitis_hls") ||
      extern_c_flag == "on") {
    opencl_kernel_add_extern_C(codegen, pTopFunc, vec_kernels);
  }

  set_unique_function_name_top(codegen, pTopFunc, vec_kernels);

  if (impl_tool == "sdaccel") {
    replace_memcpy_with_for_loop_in_function(codegen, pTopFunc, vec_kernels);
  }

  if (impl_tool == "sdaccel") {
    shorten_function_name_top(codegen, pTopFunc, vec_kernels);
  }

  if (impl_tool == "sdaccel") {
    add_loop_label(codegen, pTopFunc, vec_kernels);
  }
  // generate opencl library ont only for kernel but also for task
  if (!pure_kernel_flow && !opencl_disable) {
    replace_memcpy_with_opencl_in_lib_wrapper(codegen, pTopFunc, options,
                                              pcie_transfer_opt);
  }
  return 1;
}

//  This function is declared in array_linearize.cpp

//  =======================================================//
//  top function
//  =======================================================//
int final_code_gen(CSageCodeGen *codegen, void *pTopFunc,
                   const CInputOptions &options) {
  printf("Enter final code gen...\n");
  string mode;
  string impl_tool;
  if ("opencl" == options.get_option_key_value("-a", "mode")) {
    mode = "opencl";
  }
  if ("sdaccel" == options.get_option_key_value("-a", "impl_tool")) {
    impl_tool = "sdaccel";
  }
  string tool_version = options.get_option_key_value("-a", "tool_version");
  cout << "tool version = " << tool_version << endl;
  string pcie_transfer_opt = "off";
  if ("on" == options.get_option_key_value("-a", "pcie_transfer_opt")) {
    pcie_transfer_opt = "on";
  }
  // bool hls_flow = false;
  // if ("vitis_hls" == tool_version || "vivado_hls" == tool_version) {
  //   hls_flow = true;
  // }
  bool opencl_disable =
      test_file_for_read(OPENCL_HOST_GENERATION_DISABLED_TAG) != 0;

  // merlin_stream handling in final_streaming.cpp
  map<void *, vector<void *>> map_kernel_spawn_kernel;
  vector<string> vec_auto_kernel;
  StreamingUpdate(codegen, pTopFunc, options, impl_tool,
                  &map_kernel_spawn_kernel, &vec_auto_kernel);
  //  return 0;

  //  =======================================================//
  //  data collection
  //  =======================================================//
  map<string, map<string, string>> task_kernel;
  map<string, map<string, string>> kernel_attributes;
  map<string, map<string, map<string, string>>> kernel_port_attributes;
  map<void *, vector<void *>> kernel_subkernel;
  map<void *, vector<void *>> kernel_autokernel;
  data_parser(codegen, pTopFunc, options, &task_kernel, &kernel_attributes,
              &kernel_port_attributes, &kernel_subkernel,
              map_kernel_spawn_kernel);

  //  =======================================================//
  //  post process
  //  =======================================================//
  post_process(codegen, pTopFunc, options, task_kernel, kernel_attributes,
               kernel_port_attributes, &kernel_autokernel, vec_auto_kernel,
               impl_tool, tool_version, pcie_transfer_opt, opencl_disable);

  //=======================================================//
  // generate all opencl codeszsa
  //=======================================================//
  if (mode == "opencl") {
    opencl_code_gen(codegen, pTopFunc, options, task_kernel, kernel_attributes,
                    kernel_port_attributes, kernel_subkernel, vec_auto_kernel,
                    impl_tool, tool_version, pcie_transfer_opt, opencl_disable);
  }

  generate_kernel_file_list(codegen, pTopFunc, task_kernel, kernel_subkernel,
                            kernel_autokernel);

  return 1;
}
