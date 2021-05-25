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



#ifndef TRUNK_SOURCE_OPT_TOOLS_INCLUDE_POOL_VISIT_H_
#define TRUNK_SOURCE_OPT_TOOLS_INCLUDE_POOL_VISIT_H_

#include <rose.h>
#include <vector>
#include <string>

#include <boost/algorithm/string/replace.hpp>

#include "cmdline_parser.h"
#include "file_parser.h"
#include "xml_parser.h"

using std::string;
using std::vector;

void pool_visit(SgProject *project);
void __attribute__((noinline)) ptree(void *n, const CInputOptions &option);
void __attribute__((noinline)) ptrace(void *n);
void psymbol(void *n);
typedef void (*t_func_rose_simple_traverse)(void *sg_node, void *pArg);
void traverse_all_nodes(void *n, const CInputOptions &option,
                        t_func_rose_simple_traverse node_func,
                        void *node_func_arg);
void traverse_all_nodes(void *n, const CInputOptions &option,
                        vector<SgNode *> *vec);

bool is_builtin_located_node(SgLocatedNode *n);
bool is_builtin_symbol(SgSymbol *n);
vector<SgSymbol *> GetSymbolsFromDecl(SgNode *s);
SgLocatedNode *GetDeclFromSymbol(SgSymbol *s);
SgType *GetTypeFromSymbol(SgSymbol *s);
SgNode *get_scope(SgNode *n);

class ASTDebug {
 public:
  static string print(void *node, int len = 0, bool simple = false) {
    // node: any
    // simple: format "classname pointer [name]"
    //         normal mode format "classname pointer
    //         [FLOATING:][@file_info@][unparse] [details]"
    // len: default no limit and 80 for declaration
    string output = "";
    SgNode *n = reinterpret_cast<SgNode *>(node);

    if (!node)
      return "";

    // Print General Information
    output += print_brief(n) + " " + get_subtree_attribute_string(n) + " ";
    if (has_file_info(n) && !simple)
      output += "@" + get_file_info(n) + "@";
    if (is_unparsable(n) && !simple)
      output += unparse(n, len) + " ";
    if (has_name(n))
      output += "MANGLE=" + get_name(n) +
                " ";  // in simple mode, replacing unparse by name

    int show_ancestors = false;
    if (show_ancestors) {
      int level = -1;
      is_floating(n->get_parent(), false, &level);
      string str_level = level == -1 ? "" : my_itoa(level);
      output += "P" + str_level + "=" + print(n->get_parent(), 0, true) +
                " ";  // recursively print all the ancestors
    }

    // Print Special Function Outputs
    if (!simple) {
      output += "[";
      if (n->get_parent()) {
        if (!show_ancestors) {
          output += "P=" + print_brief(n->get_parent()) + " ";
        }
        output += "SCOPE=" + print_brief(get_scope(n)) + " ";
      }

      if (isSgSupport(n))
        output += print_detail_support(isSgSupport(n), len);
      if (isSgSymbol(n))
        output += print_detail_symbol(isSgSymbol(n), len);
      if (isSgType(n))
        output += print_detail_type(isSgType(n), len);
      if (isSgStatement(n))
        output += print_detail_statement(isSgStatement(n), len);
      if (isSgExpression(n))
        output += print_detail_expression(isSgExpression(n), len);
      if (isSgLocatedNodeSupport(n))
        output += print_detail_located_support(isSgLocatedNodeSupport(n), len);
      output += "]";
    }

    boost::replace_all(output, "\n", " ");
    boost::replace_all(output, "\r", " ");
    return output;
  }
  static string print_brief(SgNode *n) {
    if (!n)
      return "";
    return n->class_name() + "_" + p2s(n);
  }
  static bool is_floating(SgNode *n, bool check_is_child = false,
                          int *level_ptr = nullptr) {
    int max_depth = 1024 * 1024;  // give a max_depth to avoid deadlock

    SgNode *p = n;
    for (int i = 0; p && i < max_depth; i++) {
      if (check_is_child && !isSgProject(p) && !is_child_of_parent(p))
        return true;
      if (isSgProject(p)) {
        if (level_ptr)
          *level_ptr = i;
        return false;
      }
      p = p->get_parent();
    }
    return true;
  }
  static void set_pos(SgNode *pos) { s_pos = pos; }
  static void set_builtin(bool has_builtin) { s_has_builtin = has_builtin; }

 protected:
  static string p2s(void *p) {
    std::stringstream s;
    string t;
    s << p;
    s >> t;
    return t;
  }
  static bool has_parent(SgNode *n) { return n && nullptr != n->get_parent(); }
  static bool is_child(SgNode *n, SgNode *p) {
    if (!p)
      return false;
    for (auto ch : p->get_traversalSuccessorContainer()) {
      if (ch && ch == n)
        return true;  // excluding null children
    }
    return false;
  }
  // static bool is_child_of_parent(SgNode * n) { return n && n->get_parent() &&
  // n->get_parent()->isChild(n); }
  static bool is_child_of_parent(SgNode *n) {
    return n && is_child(n, n->get_parent());
  }
  static string get_subtree_attribute_string(SgNode *n) {
    string flag = "";
    if (!n)
      return flag;
    int level = 0;
    if (is_floating(n, false, &level)) {
      flag += "F";  // indicate it has no trace-up path to Project
    } else {
      if (is_floating(n, true))
        flag += "T" + my_itoa(level);  // it can be traced-up to Project, but
                                       // has broken down edge
      else
        flag += "L" + my_itoa(level);  // it is fully on the AST bidirectional
    }
    flag +=
        !has_parent(n)
            ? "N"
            : is_child_of_parent(n)
                  ? "C"
                  : "B";  // indicating if it has no parent, or it is a child of
                          // its parent, or a broken link from his parent
    int depth = n->depthOfSubtree();
    flag += "D" + my_itoa(depth);
    int num = n->numberOfNodesInSubtree();
    flag += "S" + my_itoa(num);
    return flag;
  }
  static bool has_name(SgNode *n) {
    // incomplete
    if (isSgDeclarationStatement(n))
      return true;
    return false;
  }
  static string get_name(SgNode *n) {
    // incomplete
    // if (isSgDeclarationStatement(n)) return
    // isSgDeclarationStatement(n)->unparseNameToString(); // not work
    if (isSgDeclarationStatement(n))
      return isSgDeclarationStatement(n)->get_mangled_name();

    return "";
  }
  static bool has_file_info(SgNode *n) {
    if (!isSgLocatedNode(n))
      return false;
    if (!isSgLocatedNode(n)->get_file_info())
      return false;
    string sname = isSgLocatedNode(n)->get_file_info()->get_filenameString();
    if (sname == "")
      return false;
    return true;
  }
  static string get_file_info(SgNode *n) {
    if (!isSgLocatedNode(n))
      return "";
    if (!isSgLocatedNode(n)->get_file_info())
      return "";

    string sname = isSgLocatedNode(n)->get_file_info()->get_filenameString();
    if (sname.rfind("/") != string::npos) {
      sname = sname.substr(sname.rfind("/") + 1);
    }
    int sLine = isSgLocatedNode(n)->get_file_info()->get_line();
    int sCol = isSgLocatedNode(n)->get_file_info()->get_col();
    return sname + ":" + my_itoa(sLine) + ":" + my_itoa(sCol);
  }
  static string unparse(SgNode *n, int len = 0) {
    if (!n)
      return "";
    if (isSgScopeStatement(n) && len == 0)
      len = 80;  // adapt default len for declarations
    string s = n->unparseToString();
    if (len > 0 && s.length() > len)
      s = s.substr(0, len);
    return s;
  }
  static bool is_unparsable(SgNode *n) {
    // SgSupport
    if (isSgSupport(n)) {
      // incomplete set (others ignored)
      if (isSg_File_Info(n))
        return false;
      if (isSgDirectory(n))
        return false;
      if (isSgDirectoryList(n))
        return false;
      if (isSgFile(n))
        return false;
      if (isSgFileList(n))
        return false;
      if (isSgModifier(n))
        return false;
      if (isSgName(n))
        return true;
      if (isSgProject(n))
        return false;
      if (isSgSymbolTable(n))
        return false;
      if (isSgTemplateArgument(n))
        return true;
      if (isSgTemplateArgumentList(n))
        return true;
      if (isSgTemplateParameter(n))
        return true;
      if (isSgTemplateParameterList(n))
        return true;
      if (isSgTypedefSeq(n))
        return false;
      if (isSgTypeTable(n))
        return false;
      return false;
    }

    // SgSymbol
    if (isSgSymbol(n))
      return false;

    // SgType
    if (isSgType(n))
      return true;

    // SgLocatedNode
    if (isSgLocatedNode(n)) {
      // SgLocatedNode
      // this are the special SgStatement that can not unparse
      if (isSgGlobal(n))
        return false;
      if (isSgDeclarationScope(n))
        return false;
      if (isSgFileList(n))
        return false;

      // incomplete
      if (isSgExpression(n))
        return true;
      if (isSgStatement(n))
        return true;
      if (isSgInitializedName(n))
        return true;
    }

    return false;
  }

  static SgNode *get_definition(SgNode *n) {
    SgNode *decl = n;
    // incomplete:
    if (isSgDeclarationStatement(decl) &&
        isSgDeclarationStatement(decl)->get_definingDeclaration()) {
      auto defining_decl =
          isSgDeclarationStatement(decl)->get_definingDeclaration();

      if (isSgClassDeclaration(defining_decl))
        return isSgClassDeclaration(defining_decl)->get_definition();
      if (isSgFunctionDeclaration(defining_decl))
        return isSgFunctionDeclaration(defining_decl)->get_definition();
      if (isSgNamespaceDeclarationStatement(defining_decl))
        return isSgNamespaceDeclarationStatement(defining_decl)
            ->get_definition();
      if (isSgVariableDeclaration(defining_decl))
        return isSgVariableDeclaration(defining_decl)->get_definition();
    }

    return nullptr;
  }

  static string print_set_brief(vector<SgNode *> node_set) {
    string out = "";
    for (auto n : node_set)
      out += print_brief(n) + ",";
    return out;
  }
  static string print_detail_support(SgSupport *n, int len = 0) {
    // incomplete
    string out = "";

    if (isSg_File_Info(n)) {
      out += isSg_File_Info(n)->displayString() + " ";
    }

    if (isSgModifier(n)) {
      // Complete set: SgModifier has totally 13 sub-classes
      if (isSgAccessModifier(n)) {
        out += isSgAccessModifier(n)->displayString() + " ";
      }
      if (isSgBaseClassModifier(n)) {
        out += isSgBaseClassModifier(n)->displayString() + " ";
      }
      if (isSgConstVolatileModifier(n)) {
        out += isSgConstVolatileModifier(n)->displayString() + " ";
      }
      if (isSgDeclarationModifier(n)) {
        out += isSgDeclarationModifier(n)->displayString() + " ";
      }
      if (isSgElaboratedTypeModifier(n)) {
        out += isSgElaboratedTypeModifier(n)->displayString() + " ";
      }
      if (isSgFunctionModifier(n)) {
        out += isSgFunctionModifier(n)->displayString() + " ";
      }
      if (isSgLinkageModifier(n)) {
        out += isSgLinkageModifier(n)->displayString() + " ";
      }
      // if (isSgModifierNodes(n)) {
      // }
      if (isSgOpenclAccessModeModifier(n)) {
        out += isSgOpenclAccessModeModifier(n)->displayString() + " ";
      }
      if (isSgSpecialFunctionModifier(n)) {
        out += isSgSpecialFunctionModifier(n)->displayString() + " ";
      }
      if (isSgStorageModifier(n)) {
        out += isSgStorageModifier(n)->displayString() + " ";
      }
      if (isSgTypeModifier(n)) {
        out += isSgTypeModifier(n)->displayString() + " ";
      }
      if (isSgUPC_AccessModifier(n)) {
        out += isSgUPC_AccessModifier(n)->displayString() + " ";
      }
    }

    if (isSgSymbolTable(n)) {
      vector<SgNode *> node_set;
      int built_in_num = 0;
      int total_num = 0;
      for (auto x : isSgSymbolTable(n)->get_symbolSet()) {
        if (is_builtin_symbol(isSgSymbol(x))) {
          built_in_num++;
          if (s_has_builtin)
            node_set.push_back(x);
        } else {
          node_set.push_back(x);
        }

        total_num++;
      }

      string str_num = my_itoa(total_num - built_in_num);
      if (built_in_num != 0) {
        str_num = my_itoa(built_in_num) + "+" + str_num;
      }

      out += "SET(" + str_num + ")=" + print_set_brief(node_set) + " ";
    }
    if (isSgTypeTable(n)) {
      vector<SgNode *> node_set;
      node_set.clear();
      for (auto x : isSgTypeTable(n)->get_type_table()->get_symbolSet())
        node_set.push_back(x);
      out += "SET(" + my_itoa(node_set.size()) +
             ")=" + print_set_brief(node_set) + " ";
      // {
      //  std::stringstream s; string t; //s<<p; s>>t; return t;
      //  isSgTypeTable(n)->print_typetable(s); s>>t;
      //  out += t + " ";
      // }
    }

    if (isSgTypedefSeq(n)) {
      vector<SgNode *> node_set;
      for (auto x : isSgTypedefSeq(n)->get_typedefs()) {
        if (isSgType(x))
          node_set.push_back(x);
      }
      out += "SET(" + my_itoa(node_set.size()) +
             ")=" + print_set_brief(node_set) + " ";
    }
    return out;
  }
  static int get_num_builtin(SgSymbolTable *s_table) {
    int num = 0;
    for (auto x : s_table->get_symbolSet()) {
      if (is_builtin_symbol(isSgSymbol(x)))
        num++;
    }
    return num;
  }

  static SgSymbolTable *get_type_table(SgNode *n) {
    if (isSgScopeStatement(n)) {
      if (!isSgScopeStatement(n)->get_type_table())
        return nullptr;
      return isSgScopeStatement(n)->get_type_table()->get_type_table();
    }
    if (isSgProject(n)) {
      // search the pool to get the table
      class RosePoolVisitor : public ROSE_VisitTraversal {
       public:
        explicit RosePoolVisitor(SgProject *p) {
          m_result = nullptr;
          m_project = p;
        }
        SgSymbolTable *get_result() { return m_result; }
        void visit(SgNode *node) {
          if (m_project && isSgTypeTable(node) &&
              node->get_parent() == m_project) {
            m_result = isSgTypeTable(node)->get_type_table();
          }
        }

       protected:
        SgSymbolTable *m_result;
        SgNode *m_project;
      };
      RosePoolVisitor visit(isSgProject(n));
      visit.traverseMemoryPool();
      return visit.get_result();
    }
    return nullptr;
  }

  static SgFunctionTypeSymbol *search_up_for_type_symbol(SgType *sg_type,
                                                         SgNode *pos) {
    SgNode *p = reinterpret_cast<SgNode *>(pos);

    while (p) {
      auto table = get_type_table(p);
      if (table) {
        for (auto x : table->get_symbolSet()) {
          if (isSgFunctionTypeSymbol(x) &&
              isSgFunctionTypeSymbol(x)->get_type() == sg_type)
            return isSgFunctionTypeSymbol(x);
        }
      }
      p = p->get_parent();
    }

    return nullptr;
  }

  static string print_detail_statement(SgStatement *n, int len = 0) {
    string out = "";

    if (isSgScopeStatement(n)) {
      SgSymbolTable *decl_table = isSgScopeStatement(n)->get_symbol_table();
      SgSymbolTable *type_table =
          isSgScopeStatement(n)->get_type_table()->get_type_table();

      int num_builtin_decl = get_num_builtin(decl_table);
      int num_builtin_type = get_num_builtin(type_table);

      string str_builtin_decl = my_itoa(decl_table->get_symbolSet().size());
      if (num_builtin_decl != 0) {
        str_builtin_decl =
            my_itoa(num_builtin_decl) + "+" +
            my_itoa(decl_table->get_symbolSet().size() - num_builtin_decl);
      }
      string str_builtin_type = my_itoa(type_table->get_symbolSet().size());
      if (num_builtin_type != 0) {
        str_builtin_type =
            my_itoa(num_builtin_type) + "+" +
            my_itoa(type_table->get_symbolSet().size() - num_builtin_type);
      }

      out +=
          "DTABLE(" + str_builtin_decl + ")=" + print_brief(decl_table) + " ";
      out +=
          "TTABLE(" + str_builtin_type + ")=" + print_brief(type_table) + " ";
    }

    if (isSgDeclarationStatement(n)) {
      auto decl_symbol = GetSymbolsFromDecl(n);
      {
        vector<SgNode *> tmp;
        for (auto x : decl_symbol)
          tmp.push_back(x);
        out += "DSYMBOL=" + print_set_brief(tmp) + " ";
      }

      {
        vector<SgNode *> tmp;
        for (auto x : decl_symbol)
          tmp.push_back(search_up_for_type_symbol(GetTypeFromSymbol(x), n));
        out += "TSYMBOL=" + print_set_brief(tmp) + " ";
      }
    }

    return out;
  }
  static string print_detail_expression(SgExpression *n, int len = 0) {
    string out = "";
    return out;
  }
  static string print_detail_located_support(SgLocatedNodeSupport *n,
                                             int len = 0) {
    string out = "";

    if (isSgInitializedName(n)) {
      auto decl_symbol =
          GetSymbolsFromDecl(isSgInitializedName(n)->get_declaration());
      {
        vector<SgNode *> tmp;
        for (auto x : decl_symbol)
          tmp.push_back(x);
        out += "DSYMBOL=" + print_set_brief(tmp) + " ";
      }
    }

    return out;
  }

  static string print_detail_type(SgType *n, int len = 0) {
    string out = "";

    auto symbol = search_up_for_type_symbol(n, s_pos);
    out += "TSYMBOL=" + print_brief(symbol) + " ";

    return out;
  }
  static string print_detail_symbol(SgSymbol *n, int len = 0) {
    string out = "";
    // There are totally 20 sub-classes of SgSymbol, and we handles 10 of them

    // print result of is_builtin_symbol()
    {
      if (is_builtin_symbol(n))
        out += "BUILTIN=T";
      else
        out += "BUILTIN=F";
      out += " ";
    }

    // print result of get_declaration()
    {
      auto decl = GetDeclFromSymbol(isSgSymbol(n));
      if (decl) {
        if (get_definition(decl)) {
          auto def = get_definition(decl);
          out += "DEF=" + print(def, len, true);
        } else {
          out += "DECL=" + print(decl, len, true);
        }
        out += " ";
      } else {
        out += "DECL= ";  // explisit show that DECL is nullptr
      }
    }

    // print result of get_type()
    {
      auto type = GetTypeFromSymbol(isSgSymbol(n));
      out += "TYPE=" + print(type, len, true) + " ";  // will handle if !type
    }

    return out;
  }

 protected:
  static SgNode *s_pos;
  static bool s_has_builtin;
};

#endif  // TRUNK_SOURCE_OPT_TOOLS_INCLUDE_POOL_VISIT_H_
