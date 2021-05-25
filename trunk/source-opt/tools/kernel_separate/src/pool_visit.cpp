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


#include "pool_visit.h"

#include <rose.h>
#include <string>
#include <set>
#include <map>
#include <utility>
#include <stack>
#include <algorithm>

#include <boost/algorithm/string/replace.hpp>

#include "cmdline_parser.h"
#include "file_parser.h"
#include "xml_parser.h"
// #define debug_print
// using namespace std;
using std::cout;
using std::endl;
using std::map;
using std::set;
using std::vector;

SgNode *__attribute__((noinline)) Sg(void *p) {
  return reinterpret_cast<SgNode *>(p);
}

vector<SgNode *> GetCh(SgNode *p) {
  vector<SgNode *> child_set;
  child_set.clear();
  // fix fake child issue in SgEnumType->SgEnumDeclaration
  for (auto x : p->get_traversalSuccessorContainer()) {
    if (x && x->get_parent() == p)
      child_set.push_back(x);
  }
  return child_set;
  // return p->get_traversalSuccessorContainer();
}

SgLocatedNode *GetDeclFromSymbol(SgSymbol *s) {
  // Complete list: within totaly 20 SgSymbol sub-classes, we support the
  // following 10:
  if (isSgAliasSymbol(s))
    return nullptr;
  // if (SgLocatedNode*)  return (isSgAliasSymbol (s))->get_declaration() ; //
  // crash if template function
  if (isSgClassSymbol(s))
    return reinterpret_cast<SgLocatedNode *>(
        (isSgClassSymbol(s))->get_declaration());
  if (isSgEnumFieldSymbol(s))
    return reinterpret_cast<SgLocatedNode *>(
        (isSgEnumFieldSymbol(s))->get_declaration());
  if (isSgEnumSymbol(s))
    return reinterpret_cast<SgLocatedNode *>(
        (isSgEnumSymbol(s))->get_declaration());
  if (isSgFunctionSymbol(s))
    return reinterpret_cast<SgLocatedNode *>(
        (isSgFunctionSymbol(s))->get_declaration());
  if (isSgFunctionTypeSymbol(s))
    return nullptr;  // will assert if calling get_declaration()
  if (isSgLabelSymbol(s))
    return reinterpret_cast<SgLocatedNode *>(
        (isSgLabelSymbol(s))->get_declaration());  // SgLabelStatement
  if (isSgNamespaceSymbol(s))
    return reinterpret_cast<SgLocatedNode *>(
        (isSgNamespaceSymbol(s))->get_declaration());
  if (isSgTemplateSymbol(s))
    return reinterpret_cast<SgLocatedNode *>(
        (isSgTemplateSymbol(s))->get_declaration());
  if (isSgTypedefSymbol(s))
    return reinterpret_cast<SgLocatedNode *>(
        (isSgTypedefSymbol(s))->get_declaration());
  if (isSgVariableSymbol(s))
    return reinterpret_cast<SgLocatedNode *>(
        (isSgVariableSymbol(s))->get_declaration());
  return nullptr;  // incomplete list
}

SgType *GetTypeFromSymbol(SgSymbol *s) {
  // Complete list: within totaly 20 SgSymbol sub-classes, we support the
  // following 10:
  if (isSgAliasSymbol(s))
    return (isSgAliasSymbol(s))->get_type();
  if (isSgClassSymbol(s))
    return (isSgClassSymbol(s))->get_type();
  if (isSgEnumFieldSymbol(s))
    return (isSgEnumFieldSymbol(s))->get_type();
  if (isSgEnumSymbol(s))
    return (isSgEnumSymbol(s))->get_type();
  if (isSgFunctionSymbol(s))
    return (isSgFunctionSymbol(s))->get_type();
  if (isSgFunctionTypeSymbol(s))
    return (isSgFunctionTypeSymbol(s))->get_type();
  if (isSgLabelSymbol(s))
    return nullptr;  // it will assert failure if call get_type() function
  if (isSgNamespaceSymbol(s))
    return (isSgNamespaceSymbol(s))->get_type();
  if (isSgTemplateSymbol(s))
    return (isSgTemplateSymbol(s))->get_type();
  if (isSgTypedefSymbol(s))
    return (isSgTypedefSymbol(s))->get_type();
  if (isSgVariableSymbol(s))
    return (isSgVariableSymbol(s))->get_type();
  return nullptr;  // incomplete list
}

SgType *GetTypeFromDecl(SgDeclarationStatement *s) {
  return (isSgClassDeclaration(s))
             ? (isSgClassDeclaration(s))->get_type()
             : (isSgCtorInitializerList(s))
                   ? (isSgCtorInitializerList(s))->get_type()
                   : (isSgEnumDeclaration(s))
                         ? (isSgEnumDeclaration(s))->get_type()
                         : (isSgFunctionDeclaration(s))
                               ? (isSgFunctionDeclaration(s))->get_type()
                               :
                               // (isSgFunctionParameterList   (s)) ?
                               // (isSgFunctionParameterList  (s))->get_type() :
                               // (isSgNamespaceDeclarationStatement    (s)) ?
                               // (isSgNamespaceDeclarationStatement
                               // (s))->get_type() : (isSgPragmaDeclaration (s))
                               // ? (isSgPragmaDeclaration      (s))->get_type()
                               // :
                               (isSgTemplateDeclaration(s))
                                   ? (isSgTemplateDeclaration(s))->get_type()
                                   : (isSgTypedefDeclaration(s))
                                         ? (isSgTypedefDeclaration(s))
                                               ->get_type()
                                         :
                                         // (isSgUseStatement            (s)) ?
                                         // (isSgUseStatement (s))->get_type() :
                                         // (isSgUsingDeclarationStatement(s))?
                                         // (isSgUsingDeclarationStatement
                                         // (s))->get_type() :
                                         // (isSgUsingDirectiveStatement  (s))?
                                         // (isSgUsingDirectiveStatement(s))->get_type()
                                         // : (isSgVariableDeclaration     (s))
                                         // ? (isSgVariableDeclaration
                                         // (s))->get_type() :  // it has no
                                         //  get_type function
                                         (isSgVariableDefinition(s))
                                             ? (isSgVariableDefinition(s))
                                                   ->get_type()
                                             : nullptr;  // incomplete
  // list
}

///////////////////////////////////////////////////////////////////
// Note: the return type is a vector instead of a single object, it is because
// SgVariableDeclaration may have multiple initialized name (e.g. int a, b;)
//       However, in the real case, ROSE has automatically split 'int a, b;'
//       into two declarations "int a; int b;" during AST creation We keep the
//       code structure like this because ROSE data structure remains in this
//       original form.
vector<SgSymbol *> GetSymbolsFromDecl(SgNode *s) {
  vector<SgSymbol *> out;
  out.clear();

  // incomplete list
  // if (isSgClassDeclaration(s)) {
  // }
  if (isSgCtorInitializerList(s))
    return out;
  if (isSgAsmStmt(s))
    return out;
  // if (isSgEnumDeclaration(s)) {
  // }
  // if (isSgFunctionDeclaration(s)) {
  // }
  if (isSgFunctionParameterList(s))
    return out;
  // if (isSgNamespaceDeclarationStatement(s)) {
  // }
  if (isSgPragmaDeclaration(s))
    return out;
  // if (isSgTemplateDeclaration(s)) {
  // }
  // if (isSgTypedefDeclaration(s)) {
  // }
  if (isSgUseStatement(s))
    return out;
  if (isSgUsingDeclarationStatement(s))
    return out;
  if (isSgUsingDirectiveStatement(s))
    return out;
  if (isSgVariableDeclaration(s)) {
    auto vars = isSgVariableDeclaration(s)->get_variables();
    for (auto x : vars) {
      auto symbol = x->search_for_symbol_from_symbol_table();
      if (symbol)
        out.push_back(symbol);
    }
    return out;
  }
  if (isSgVariableDefinition(s))
    return out;

  if (isSgDeclarationStatement(s)) {
    // StatidAssertDeclaration does not have symbol
    if (isSgStaticAssertionDeclaration(s))
      return out;

    auto symbol =
        isSgDeclarationStatement(s)->search_for_symbol_from_symbol_table();
    if (symbol)
      out.push_back(symbol);
  }

  return out;
}

SgNode *get_scope(SgNode *n) {
  SgNode *scope = n ? n->get_parent() : nullptr;
  while (scope && !isSgScopeStatement(scope) && !isSgProject(scope))
    scope = scope->get_parent();
  return scope;
}

bool is_builtin_located_node(SgLocatedNode *n) {
  string file_info = "";
  if (!n)
    return false;

  // check file info
  file_info = (isSgLocatedNode(n))->get_file_info()->get_filenameString();
  if (file_info.rfind("/") != string::npos) {
    file_info = file_info.substr(file_info.rfind("/") + 1);
  }
  if (file_info == "rose_edg_required_macros_and_functions.h")
    return true;

  // check declaration name: special cases
  string name = "";
  if (isSgDeclarationStatement(n))
    name = isSgDeclarationStatement(n)->get_mangled_name();
  if (name.find("__builtin_") != string::npos)
    return true;
  if (name == "L0R")
    return true;
  if (name == "typedef__Booli__typedef_declaration")
    return true;

  return false;
}

set<SgType *> g_builtin_type_set;

bool is_builtin_symbol(SgSymbol *n) {
  if (!n)
    return false;

  // for decl symbol, check its declaration
  auto decl = GetDeclFromSymbol(n);
  if (decl)
    return is_builtin_located_node(decl);

  // for type symbol, check the scope of the type
  if (isSgFunctionTypeSymbol(
          n)) {  // type symbol is using class SgFunctionTypeSymbol
    if (isSgProject(get_scope(n)))
      return true;  // primitive type symbols are registered at SgProject

    auto type = n->get_type();
    if (g_builtin_type_set.end() != g_builtin_type_set.find(type))
      return true;
  }

  return false;
}
class BuiltinChecker {
 public:
  bool is_builtin(SgNode *node);

  static void init(SgProject *prj) {
    class RosePoolVisitor : public ROSE_VisitTraversal {
     public:
      void visit(SgNode *node) {
        // printf("PoolVisitor[%d]: %s\n", m_count++,
        // ASTDebug::print(node).c_str());

        // store the type of built-in funtion decl as built-in type
        auto decl = GetDeclFromSymbol(isSgSymbol(node));  // find a decl symbol
        if (decl && is_builtin_located_node(decl)) {  // this decl is built-in
          auto type =
              isSgSymbol(node)->get_type();  // so add the type of this decl
          g_builtin_type_set.insert(type);
        }
      }
    };
    RosePoolVisitor visit;
    g_builtin_type_set.clear();
    visit.traverseMemoryPool();
  }

 protected:
};

SgType *type_get_base_type(SgType *t) {
  SgType *out = nullptr;

  // if (t->isPrimativeType()) return out;  // NOTE: this API exists in ROSE
  // documentation but not in code

  if (isSgArrayType(t))
    return isSgArrayType(t)->get_base_type();
  // if (isSgAutoType         (t))    return out; // NOTE: this API exists in
  // ROSE documentation but not in code
  // if (isSgDeclType(t))
  //  return out;
  // if (isSgFunctionType(t))
  //  return out;
  // if (isSgJavaUnionType(t))
  //  return out;
  if (isSgModifierType(t))
    return isSgModifierType(t)->get_base_type();
  // if (isSgNamedType(t))
  //  return out;
  if (isSgPointerType(t))
    return isSgPointerType(t)->get_base_type();
  // if (isSgQualifiedNameType(t))
  //  return out;
  if (isSgReferenceType(t))
    return isSgReferenceType(t)->get_base_type();
  if (isSgRvalueReferenceType(t))
    return isSgRvalueReferenceType(t)->get_base_type();
  // if (isSgTemplateType(t))
  //  return out;
  // if (isSgTypeOfType(t))
  //  return isSgTypeOfType(t)->get_base_type();
  // if (isSgTypeString(t))
  //  return out;
  // if (isSgTypeTuple(t))
  //  return out;
  // if (isSgTypeUnknown(t))
  //  return out;

  return out;  // SgTypexxx primitive types
}
SgSymbol *type_get_symbol(SgType *t) {
  SgSymbol *out = nullptr;

  // if (t->isPrimativeType()) return out;  // NOTE: this API exists in ROSE
  // documentation but not in code if (isSgAutoType         (t))    return out;
  // // NOTE: this API exists in ROSE documentation but not in code

  // if (isSgArrayType(t))
  //  return out;
  // if (isSgDeclType(t))
  //  return out;
  if (isSgFunctionType(t))
    return isSgFunctionType(t)->get_symbol_from_symbol_table();
  // if (isSgJavaUnionType(t))
  //  return out;
  // if (isSgModifierType(t))
  //  return out;
  if (isSgNamedType(t))
    return isSgNamedType(t)
        ->get_declaration()
        ->search_for_symbol_from_symbol_table();
  // {
  //    if (isSgClassType    (t))    return ...
  //    if (isSgEnumType     (t))    return ...
  //    if (isSgTypedefType  (t))    return ...
  // }
  // if (isSgPointerType(t))
  //  return out;
  // if (isSgQualifiedNameType(t))
  //  return out;
  // if (isSgReferenceType(t))
  //  return out;
  // if (isSgRvalueReferenceType(t))
  //  return out;
  // if (isSgTemplateType(t))
  //  return out;
  // if (isSgTypeOfType(t))
  //  return out;
  // if (isSgTypeString(t))
  //  return out;
  // if (isSgTypeTuple(t))
  //  return out;
  // if (isSgTypeUnknown(t))
  //  return out;

  return out;  // SgTypexxx primitive types
}
SgExpression *type_get_exp(SgType *t) {
  SgExpression *out = nullptr;

  // if (t->isPrimativeType()) return out;  // NOTE: this API exists in ROSE
  // documentation but not in code if (isSgAutoType         (t))    return out;
  // // NOTE: this API exists in ROSE documentation but not in code

  if (isSgArrayType(t))
    return isSgArrayType(t)->get_index();
  // if (isSgDeclType(t))
  //  return out;
  // if (isSgFunctionType(t))
  //  return out;
  // if (isSgJavaUnionType(t))
  //  return out;
  // if (isSgModifierType(t))
  //  return out;
  // if (isSgNamedType(t))
  //  return out;
  // {
  //    if (isSgClassType    (t))    return ...
  //    if (isSgEnumType     (t))    return ...
  //    if (isSgTypedefType  (t))    return ...
  // }
  // if (isSgPointerType(t))
  //  return out;
  // if (isSgQualifiedNameType(t))
  //  return out;
  // if (isSgReferenceType(t))
  //  return out;
  // if (isSgRvalueReferenceType(t))
  //  return out;
  // if (isSgTemplateType(t))
  //  return out;
  if (isSgTypeOfType(t))
    return isSgTypeOfType(t)->get_base_expression();
  if (isSgTypeString(t))
    return isSgTypeString(t)->get_lengthExpression();
  // if (isSgTypeTuple(t))
  //  return out;
  // if (isSgTypeUnknown(t))
  //  return out;

  return out;
}

SgNode *ASTDebug::s_pos = nullptr;
bool ASTDebug::s_has_builtin = false;

//
class CTraverseAllNode {
 public:
  CTraverseAllNode() {
    m_has_builtin = true;
    m_is_preorder = true;
    m_func = nullptr;
    m_pArg = nullptr;
    m_map_missing_ast_edge.clear();
  }
  void set_builtin(bool has_builtin) { m_has_builtin = has_builtin; }
  void set_order(bool is_preorder = true) { m_is_preorder = is_preorder; }
  void set_func_arg(t_func_rose_simple_traverse func, void *pArg) {
    m_func = func;
    m_pArg = pArg;
  }

  void traverse_with_func(SgNode *n, t_func_rose_simple_traverse func,
                          void *pArg, bool is_print = 0, int depth = 0) {
    if (depth == 0)
      set_func_arg(func, pArg);
    traverse(n, nullptr, is_print, depth);
  }

  // To add mechanism to avoid recursive deadlock
  void traverse(SgNode *n, vector<SgNode *> *vec_out, bool is_print = 0,
                int depth = 0) {
    // setup and cleanup
    if (depth == 0) {
      if (vec_out)
        vec_out->clear();
      m_map_missing_ast_edge.clear();
    }

    if (n == nullptr)
      return;
#ifdef debug_print
    if (!m_has_builtin && is_builtin_located_node(isSgLocatedNode(n))) {
      return;
    }
#endif
    // if (isSgFunctionDeclaration(n) &&
    // isSgFunctionDeclaration(n)->get_name().getString().substr(0,10) ==
    // "__builtin_")
    //  return;
    if (m_is_preorder) {
      if (vec_out)
        vec_out->push_back(n);
      if (m_func)
        (*m_func)(n, m_pArg);
    }

    string edge_prefix = "";
#ifdef debug_print  // ZP: added the check to reduce runtime
    if (is_print) {
      SgNode *pos = nullptr;  // m_map_missing_ast_edge[n].second;
      if (!ASTDebug::is_floating(n,
                                 false)) {  // check_is_child is false because
                                            // we only trace up from the pos
        pos = n;
        m_ontree_stack.push(n);
      } else {
        pos = (m_ontree_stack.size() == 0) ? nullptr : m_ontree_stack.top();
      }

      int hit_missing_edge =
          m_map_missing_ast_edge.find(n) != m_map_missing_ast_edge.end();
      if (hit_missing_edge) {
        edge_prefix = m_map_missing_ast_edge[n].first + "=";
        m_map_missing_ast_edge.erase(
            n);  // The lifetime of this map is very short, between node and
                 // their direct missing child. So just cleaning it up is enough
                 // for correctness in case of multi-reference of the same type
                 // node
      }

      for (int i = 0; i < depth; i++)
        cout << "  ";

      ASTDebug::set_pos(pos);
      ASTDebug::set_builtin(m_has_builtin);
    }
    if (is_print) {
      cout << edge_prefix << ASTDebug::print(n);

      cout << endl;
    }
#endif
    auto child = GetCh(n);

    ////////////////////////////
    // ZP: this is the adhoc parts to recover the broken link for a complete
    // traverse purpose
    // 1. SgInitializedName -> SgType
    // 2. SgSizeofOp -> SgType
    // ...

    const int fix_missing_nodes = true;
    if (fix_missing_nodes) {
      //////////////////////////////////////////////////
      // Express Pathway (for runtime reduction)
      //////////////////////////////////////////////////
      //     if (isSgBinaryOp(n)) {}  // do nothing
      // else if (isSgUnaryOp(n)) {}  // do nothing
      // else if (isSgValueExp(n)) {}  // do nothing
      // else if (isSgFunctionCallExp(n)) {}  // do nothing
      // else if (isSgVariableDeclaration(n)) {}  // do nothing
      // else if (isSgExprStatement(n)) {}  // do nothing
      // else if (isSgIfStmt(n)) {}  // do nothing
      // else if (isSgForStatement(n)) {}  // do nothing
      // else if (!is_print && isSgScopeStatement(n)) {}  // do nothing

      //////////////////////////////////////////////////
      // SgExpression
      //////////////////////////////////////////////////
      if (isSgExpression(n)) {
        if (isSgBinaryOp(n)) {
        } else {  // to reduce execution time
          if (isSgSizeOfOp(n)) {
            // child.push_back(isSgSizeOfOp(n)->get_operand_expr()); // expr
            // will be on the tree, in the case of sizeof(expr)
            child.push_back(isSgSizeOfOp(n)->get_operand_type());
#ifdef debug_print
            if (is_print)
              m_map_missing_ast_edge[isSgSizeOfOp(n)->get_operand_type()] =
                  pair<string, SgNode *>("get_operand_type()", n);
#endif
          } else if (isSgAlignOfOp(n)) {
            child.push_back(isSgAlignOfOp(n)->get_operand_type());
#ifdef debug_print
            if (is_print)
              m_map_missing_ast_edge[isSgAlignOfOp(n)->get_operand_type()] =
                  pair<string, SgNode *>("get_operand_type()", n);
#endif
            child.push_back(isSgAlignOfOp(n)->get_operand_expr());
#ifdef debug_print
            if (is_print)
              m_map_missing_ast_edge[isSgAlignOfOp(n)->get_operand_expr()] =
                  pair<string, SgNode *>("get_operand_expr()", n);
#endif
          } else if (isSgCompoundLiteralExp(n)) {
            // traverse the initialized name it contains in the symbol
            auto init =
                isSgCompoundLiteralExp(n)->get_symbol()->get_declaration();
            child.push_back(init);
            // NOTE: in principle we only link symbol or type in the sub-tree,
            // in order to avoid recursive traverse Here the decl is directly
            // linked because the initialized name has a fully local scope, no
            // symbol is necessary visible to the outside of this expression
#ifdef debug_print
            if (is_print)
              m_map_missing_ast_edge[init] =
                  pair<string, SgNode *>("get_symbol()->get_declaration()", n);
#endif
          } else if (isSgTypeTraitBuiltinOperator(n)) {
            // traverse the args it contains in the symbol
            SgNodePtrList &nodes = isSgTypeTraitBuiltinOperator(n)->get_args();
            for (auto t : nodes) {
              child.push_back(t);
#ifdef debug_print
              if (is_print)
                m_map_missing_ast_edge[t] =
                    pair<string, SgNode *>("get_args()[]", n);
#endif
            }
          } else if (isSgVarRefExp(n)) {
#ifdef debug_print
            if (is_print) {
              auto s = isSgVarRefExp(n)->get_symbol();
              child.push_back(s);
              m_map_missing_ast_edge[s] =
                  pair<string, SgNode *>("get_symbol()", n);
            }
#endif
          } else if (isSgFunctionRefExp(n)) {
#ifdef debug_print
            if (is_print) {
              auto s = isSgFunctionRefExp(n)->get_symbol();
              child.push_back(s);
              m_map_missing_ast_edge[s] =
                  pair<string, SgNode *>("get_symbol()", n);
            }
#endif
          }
        }
      } else if (isSgDeclarationStatement(n)) {
        if (isSgStaticAssertionDeclaration(n)) {
          auto cond = isSgStaticAssertionDeclaration(n)->get_condition();
          child.insert(child.begin(), cond);
#ifdef debug_print
          if (is_print)
            m_map_missing_ast_edge[cond] =
                pair<string, SgNode *>("get_condition()", n);
#endif
        } else if (isSgTypedefDeclaration(n)) {
          auto base_type = isSgTypedefDeclaration(n)->get_base_type();
          child.insert(child.begin(), base_type);
#ifdef debug_print
          if (is_print)
            m_map_missing_ast_edge[base_type] =
                pair<string, SgNode *>("get_base_type()", n);
#endif
        } else if (isSgUsingDeclarationStatement(n)) {
#ifdef debug_print
          if (is_print) {
            auto declaration =
                isSgUsingDeclarationStatement(n)->get_declaration();
            auto symbol = GetSymbolsFromDecl(declaration);
            for (auto x : symbol) {
              child.insert(child.begin(), x);
              m_map_missing_ast_edge[x] =
                  pair<string, SgNode *>("get_symbol()", n);
            }
          }
#endif
        } else if (isSgUsingDirectiveStatement(n)) {
#ifdef debug_print
          if (is_print) {
            auto ns_decl =
                isSgUsingDirectiveStatement(n)->get_namespaceDeclaration();
            auto symbol = GetSymbolsFromDecl(ns_decl);
            for (auto x : symbol) {
              child.insert(child.begin(), x);
              m_map_missing_ast_edge[x] =
                  pair<string, SgNode *>("get_symbol()", n);
            }
          }
#endif
        }
        //////////////////////////////////////////////////
        // SgLocatedSupport
        //////////////////////////////////////////////////
      } else if (isSgInitializedName(n)) {
        child.push_back(isSgInitializedName(n)->get_type());
#ifdef debug_print
        if (is_print)
          m_map_missing_ast_edge[isSgInitializedName(n)->get_type()] =
              pair<string, SgNode *>("get_type()", n);
#endif
        //////////////////////////////////////////////////
        // SgType
        //////////////////////////////////////////////////
      } else if (isSgType(n)) {
        // Handling ArrayType/PointerType/etc.
        SgType *base_type;
        if ((base_type = type_get_base_type(isSgType(n))) != nullptr) {
          child.push_back(base_type);
#ifdef debug_print
          if (is_print)
            m_map_missing_ast_edge[base_type] =
                pair<string, SgNode *>("get_base_type()", n);
#endif
        }

#ifdef debug_print
        if (is_print) {
          SgSymbol *symbol;
          if ((symbol = type_get_symbol(isSgType(n))) != nullptr) {
            child.push_back(symbol);
            m_map_missing_ast_edge[symbol] =
                pair<string, SgNode *>("get_symbol()", n);
          }
        }
#endif

        SgExpression *expr;
        if ((expr = type_get_exp(isSgType(n))) != nullptr) {
          child.push_back(expr);
#ifdef debug_print
          if (is_print)
            m_map_missing_ast_edge[expr] =
                pair<string, SgNode *>("get_expression()", n);
#endif
        }
        //////////////////////////////////////////////////
        // SgStatement
        //////////////////////////////////////////////////
      } else if (isSgGotoStatement(n)) {
        // traverse the args it contains in the symbol

        // these two function has no output:
        // child.push_back(isSgGotoStatement(n)->get_label_expression());
        // m_map_missing_ast_edge[isSgGotoStatement(n)->get_label_expression()]
        // = "get_label_expression()";
        // child.push_back(isSgGotoStatement(n)->get_selector_expression());
        // m_map_missing_ast_edge[isSgGotoStatement(n)->get_selector_expression()]
        // = "get_selector_expression()";
        if (isSgLabelStatement(isSgGotoStatement(n)->get_label())) {
#ifdef debug_print
          if (is_print) {
            auto symbol = isSgGotoStatement(n)
                              ->get_label()
                              ->get_symbol_from_symbol_table();
            child.push_back(symbol);
            m_map_missing_ast_edge[symbol] =
                pair<string, SgNode *>("get_label()->get_symbol()", n);
          }
#endif
        } else if (isSgLabelRefExp(
                       isSgGotoStatement(n)->get_label_expression())) {
          auto label_exp = isSgGotoStatement(n)->get_label_expression();
          child.push_back(label_exp);
#ifdef debug_print
          if (is_print)
            m_map_missing_ast_edge[label_exp] =
                pair<string, SgNode *>("get_label_expression()", n);
#endif
        } else if (isSgExpression(
                       isSgGotoStatement(n)->get_selector_expression())) {
          auto selector = isSgGotoStatement(n)->get_selector_expression();
          child.push_back(selector);
#ifdef debug_print
          if (is_print)
            m_map_missing_ast_edge[selector] =
                pair<string, SgNode *>("get_selector_expression()", n);
#endif
        }

      } else if (isSgScopeStatement(n)) {
#ifdef debug_print
        if (is_print) {  // ZP: added the check to reduce runtime
          SgSymbolTable *type_table =
              isSgScopeStatement(n)->get_type_table()->get_type_table();
          child.insert(child.begin(), type_table);
          m_map_missing_ast_edge[type_table] =
              pair<string, SgNode *>("TTABLE", n);

          SgSymbolTable *decl_table = isSgScopeStatement(n)->get_symbol_table();
          child.insert(child.begin(), decl_table);
          m_map_missing_ast_edge[decl_table] =
              pair<string, SgNode *>("DTABLE", n);
        }
#endif
      }
    }

    if (!fix_missing_nodes || (!isSgSymbol(n) && !isSgSymbolTable(n))) {
      for (auto c : child) {
        traverse(c, vec_out, is_print, depth + 1);
      }
    }
#ifdef debug_print
    if (is_print) {  // ZP: added the check to reduce runtime
      if (!ASTDebug::is_floating(n,
                                 false)) {  // popstack due to a previous push
        m_ontree_stack.pop();
      }
    }
#endif
    if (!m_is_preorder) {
      if (vec_out)
        vec_out->push_back(n);
      if (m_func)
        (*m_func)(n, m_pArg);
    }
  }

  void traverse(void *n, bool is_print = 1, int depth = 0) {
    vector<SgNode *> vec;
    traverse(reinterpret_cast<SgNode *>(n), &vec, is_print, depth);
  }

 private:
  map<void *, pair<string, SgNode *>>
      m_map_missing_ast_edge;  // node missing from the AST -> ROSE API name to
                               // setup the link (called from its parent), and
                               // its parent node
  bool m_has_builtin;
  bool m_is_preorder;
  std::stack<SgNode *> m_ontree_stack;

 protected:
  t_func_rose_simple_traverse m_func;
  void *m_pArg;
};

void traverse_all_nodes(void *n, const CInputOptions &option,
                        t_func_rose_simple_traverse node_func,
                        void *node_func_arg) {
  CTraverseAllNode trav;
  string str_skip_builtin = option.get_option_key_value("-a", "skip_builtin");
  string str_order = option.get_option_key_value("-a", "order");

  std::transform(str_skip_builtin.begin(), str_skip_builtin.end(),
                 str_skip_builtin.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  if ("true" == str_skip_builtin || "1" == str_skip_builtin ||
      "on" == str_skip_builtin)
    trav.set_builtin(false);
  if ("postorder" == str_order)
    trav.set_order(false);
  trav.traverse_with_func(reinterpret_cast<SgNode *>(n), node_func,
                          node_func_arg);
}

void traverse_all_nodes(void *n, const CInputOptions &option,
                        vector<SgNode *> *vec) {
  CTraverseAllNode trav;
  string str_skip_builtin = option.get_option_key_value("-a", "skip_builtin");
  string str_order = option.get_option_key_value("-a", "order");

  std::transform(str_skip_builtin.begin(), str_skip_builtin.end(),
                 str_skip_builtin.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  if ("true" == str_skip_builtin || "1" == str_skip_builtin ||
      "on" == str_skip_builtin)
    trav.set_builtin(false);
  if ("postorder" == str_order)
    trav.set_order(false);
  trav.traverse(reinterpret_cast<SgNode *>(n), vec);
}

void __attribute__((noinline)) ptree(void *n, const CInputOptions &option) {
  CTraverseAllNode trav;
  CTraverseAllNode *g_trav = nullptr;
  g_trav = &trav;
  string str_skip_builtin = option.get_option_key_value("-a", "skip_builtin");
  std::transform(str_skip_builtin.begin(), str_skip_builtin.end(),
                 str_skip_builtin.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  if ("true" == str_skip_builtin || "1" == str_skip_builtin ||
      "on" == str_skip_builtin)
    g_trav->set_builtin(false);
  g_trav->traverse(reinterpret_cast<SgNode *>(n));
}
void __attribute__((noinline)) ptree(void *n) {
  CInputOptions option;
  ptree(n, option);
}
void __attribute__((noinline)) ptrace(void *n) {
  SgNode *p = reinterpret_cast<SgNode *>(n);

  while (p) {
    cout << ASTDebug::print_brief(p) << endl;
    p = p->get_parent();
  }
}
void psymbol(void *n) {
  SgNode *p = reinterpret_cast<SgNode *>(n);

  if (isSgScopeStatement(p)) {
    p = isSgScopeStatement(p)->get_symbol_table();
  }

  if (isSgSymbolTable(p)) {
    string msg;
    isSgSymbolTable(p)->print(msg);

    cout << msg << endl;
  }
}

void tree_visit(SgProject *project) {
  class RoseVisitor : public AstSimpleProcessing {
   public:
    int count;
    void visit(SgNode *node) {
      int is_valid_SgLocatedNode = !!isSgLocatedNode(node) &&
                                   !isSgGlobal(node) &&
                                   !isSgDeclarationScope(node);
      int is_valid_SgType = !!isSgType(node);
      if (is_valid_SgType || is_valid_SgLocatedNode) {
        printf("TreeVisitor[%d]: %s %s\n", count,
               ASTDebug::print_brief(node).c_str(),
               node->unparseToString().c_str());
      }
      count++;
    }
    RoseVisitor() : count(0) {}
  };
  RoseVisitor visit;
  visit.traverse(project, preorder);
}

void pool_visit(SgProject *project) {
  printf("\n\n Start pool visit: \n");
  class RosePoolVisitor : public ROSE_VisitTraversal {
   public:
    void visit(SgNode *node) {
      printf("PoolVisitor[%d]: %s\n", m_count++, ASTDebug::print(node).c_str());
    }
    RosePoolVisitor() : m_count(0) {}

   protected:
    int m_count;
  };
  BuiltinChecker::init(project);

  ASTDebug::set_pos(project);
  ASTDebug::set_builtin(true);
  RosePoolVisitor visit;
  visit.traverseMemoryPool();
}
