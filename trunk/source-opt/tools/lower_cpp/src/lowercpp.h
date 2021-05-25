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
#include <limits>
#include <algorithm>
#include <forward_list>
#include <type_traits>
#include <list>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <utility>
#include "codegen.h"
#include "mars_ir_v2.h"
#include "mars_message.h"
#include "program_analysis.h"
#include "rose.h"
using std::list;
using std::map;
using std::pair;
using std::set;
using std::string;
using std::unordered_map;
using std::unordered_set;
using std::vector;
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
#if DISABLE_DEBUG
#else
std::ostream &operator<<(std::ostream &out, const SgNode *n);
#endif
#define MERLIN_DUMMY_ENUM "MERLIN_MAX_ENUM_"

namespace helpers {
enum struct Unsup {
  Supported = 0,
  Dtor,
  TryCatch,
  OperatorNew,
  OperatorDelete,
  BitFields,
  VirtualMemFn,
  MultInheritance,
  VirtInheritance,
  DelegatedCtor,
  NestedAgg,
  InlineAggDefs,
  ConstRefOperatorCast,
  MemberPointerOp,
  RefTypedMember,
  NonFnOrClsCtor,
  TemplateArgument,
};

//  Unsupported codes specific to krnel top-levels.
enum struct KernUnsup {
  Supported = 0,
  NoOperators,
  NoTemplates,
  NoMembers,
  CantBeNamespaced,
  CantBeOverloaded,
  UnnestedCType,
  CantBeStatic,
};

void show_unsupported_construct(SgNode *construct, Unsup code,
                                CSageCodeGen *cg) {
  assert(code != Unsup::Supported && "It's supported! Why show an error.");
  std::stringstream msg;
  switch (code) {
  case Unsup::Supported:
    //  Unreachable.
    break;
  case Unsup::Dtor:
    msg << "Destructor";
    break;
  case Unsup::TryCatch:
    msg << "Try-catch clause";
    break;
  case Unsup::OperatorNew:
    msg << "Operator new expression";
    break;
  case Unsup::OperatorDelete:
    msg << "Operator delete expression";
    break;
  case Unsup::BitFields:
    msg << "Bit-fields";
    break;
  case Unsup::VirtualMemFn:
    msg << "Virtual function declaration";
    break;
  case Unsup::MultInheritance:
    msg << "Multiple Inheritance";
    break;
  case Unsup::VirtInheritance:
    msg << "Virtual Inheritance";
    break;
  case Unsup::DelegatedCtor:
    msg << "Delegated ctor";
    break;
  case Unsup::NestedAgg:
    msg << "Nested aggregate";
    break;
  case Unsup::InlineAggDefs:
    msg << "Inline struct definition";
    break;
  case Unsup::ConstRefOperatorCast:
    msg << "Const ref operator cast";
    break;
  case Unsup::MemberPointerOp:
    msg << "Pointer-to-member access operator";
    break;
  case Unsup::RefTypedMember:
    msg << "Reference-typed member variable";
    break;
  case Unsup::NonFnOrClsCtor:
    msg << "Constructor call outside of function or class scope";
    break;
  case Unsup::TemplateArgument:
    msg << "unknown template argument";
    break;
  }
  msg << " is not supported: " << cg->UnparseToString(construct, 80) << " "
      << getUserCodeFileLocation(cg, construct, true);
  dump_critical_message(LWCPP_ERROR_1(msg.str()), 0);

  // TODO(youxiang): Refactor this. This function is meant only to display the
  //  constructs. Hints and fix-its should be done in a general diagnostics
  //  emitting function.
  if (code == Unsup::InlineAggDefs) {
    const char *fixit = "Hint: Consider separating the aggregate "
                        "definition from variable declaration.";
    dump_critical_message(LWCPP_INFO_2(fixit), 0);
  }
}

void show_unsupported_kern(SgFunctionDeclaration *kern, KernUnsup code,
                           CSageCodeGen *cg, bool is_warning) {
  std::stringstream msg;
  std::stringstream hint;
  switch (code) {
  case KernUnsup::Supported:
    return;
  case KernUnsup::NoOperators:
    msg << "kernel cannot be an operator function";
    break;
  case KernUnsup::NoTemplates:
    msg << "kernel cannot be a template function";
    break;
  case KernUnsup::NoMembers:
    msg << "kernel cannot be a member function";
    break;
  case KernUnsup::CantBeNamespaced:
    msg << "kernel cannot reside within a namespace";
    break;
  case KernUnsup::CantBeOverloaded:
    msg << "kernel cannot be overloaded";
    break;
  case KernUnsup::UnnestedCType:
    msg << "kernel's type signature is not reference C-like or "
           "global-namespace types. Down stream simulation may have "
           "compilation problem";
    break;
  case KernUnsup::CantBeStatic:
    msg << "Kernel cannot be static";
    break;
  }
  msg << ": " << cg->UnparseToString(kern, 80) << " "
      << getUserCodeFileLocation(cg, kern, true);
  if (is_warning)
    dump_critical_message(LWCPP_WARNING_1(msg.str()), 0);
  else
    dump_critical_message(LWCPP_ERROR_2(msg.str()), 0);
}

#if DISABLE_DEBUG
#else
template <typename NodeTy> const NodeTy *isa(const SgNode *node) {
  return static_cast<const NodeTy *>(isa<NodeTy>(const_cast<SgNode *>(node)));
}
#endif

//  This acts as a modern replacement to ROSE's isSgXXX set of RTTI functions.
//  20181204 Youxiang: ROSE's isSgXXX has been implemented similarly like the
//  following
template <typename NodeTy> NodeTy *isa(SgNode *node) {
  assert(node && "Use isa_or_null for possibly nullptrs.");
  if (rose_ClassHierarchyCastTable[node->variantT()]
                                  [NodeTy::static_variant >> 3] &
      (1 << (NodeTy::static_variant & 7))) {
    return static_cast<NodeTy *>(node);
  }
  return nullptr;
}

template <typename NodeTy> NodeTy *isa_or_null(SgNode *node) {
  return node ? isa<NodeTy>(node) : nullptr;
}

template <typename It, typename Fn> bool any_of(It &&iter, Fn &&f) {
  return std::any_of(std::begin(iter), std::end(iter), std::forward<Fn>(f));
}

SgClassSymbol *cls_symbol(SgClassDeclaration *d) {
  if (d->get_declaration_associated_with_symbol() == nullptr) {
    return nullptr;
  }
  auto *res = d->search_for_symbol_from_symbol_table();
  if (nullptr == res)
    return nullptr;
  return isa<SgClassSymbol>(res);
}

SgTypedefSymbol *typedef_symbol(SgTypedefDeclaration *d) {
  if (d->get_declaration_associated_with_symbol() == nullptr) {
    return nullptr;
  }
  return isa<SgTypedefSymbol>(d->search_for_symbol_from_symbol_table());
}

SgEnumSymbol *enum_symbol(SgEnumDeclaration *d) {
  if (d->get_declaration_associated_with_symbol() == nullptr) {
    return nullptr;
  }
  return isa<SgEnumSymbol>(d->search_for_symbol_from_symbol_table());
}

SgFunctionSymbol *fn_symbol(SgFunctionDeclaration *f) {
  if (f->get_declaration_associated_with_symbol() == nullptr) {
    return nullptr;
  }
  auto *res = f->search_for_symbol_from_symbol_table();
  if (nullptr == res)
    return nullptr;
  return isa<SgFunctionSymbol>(res);
}

template <typename NodeTy> SgName mangle(NodeTy *node) {
  // TODO(youxiang): Implement proper mangling logic.
  SgName rv("_merlinc_" + node->get_qualified_name().getString());
  return rv;
}

bool leaf_type(SgType *ty) {
  //  Set of types that wrap some inner type:
  //
  //  SgPointerType::get_base_type () const
  //  SgReferenceType::get_base_type () const
  //  SgTypedefType::get_base_type() const
  //  SgModifierType::get_base_type () const
  //  SgArrayType::get_base_type () const
  //  SgQualifiedNameType::get_base_type () const
  //  SgTypeComplex::get_base_type () const
  //  SgTypeImaginary::get_base_type () const
  //  SgRvalueReferenceType::get_base_type () const
  //  SgDeclType::get_base_type() const
  //  SgTypeOfType::get_base_type() const
  //  SgTypeMatrix::get_base_type () const
  //  SgTypedefDeclaration::get_base_type () const

  return (isa<SgTypeBool>(ty) != nullptr) ||
         (isa<SgTypeCAFTeam>(ty) != nullptr) ||
         (isa<SgTypeChar>(ty) != nullptr) ||
         (isa<SgTypeCrayPointer>(ty) != nullptr) ||
         (isa<SgTypeDefault>(ty) != nullptr) ||
         (isa<SgTypeDouble>(ty) != nullptr) ||
         (isa<SgTypeEllipse>(ty) != nullptr) ||
         (isa<SgTypeFloat>(ty) != nullptr) || (isa<SgTypeInt>(ty) != nullptr) ||
         (isa<SgTypeLong>(ty) != nullptr) ||
         (isa<SgTypeLongDouble>(ty) != nullptr) ||
         (isa<SgTypeLongLong>(ty) != nullptr) ||
         (isa<SgTypeShort>(ty) != nullptr) ||
         (isa<SgTypeSigned128bitInteger>(ty) != nullptr) ||
         (isa<SgTypeSignedChar>(ty) != nullptr) ||
         (isa<SgTypeSignedInt>(ty) != nullptr) ||
         (isa<SgTypeSignedLong>(ty) != nullptr) ||
         (isa<SgTypeSignedLongLong>(ty) != nullptr) ||
         (isa<SgTypeSignedShort>(ty) != nullptr) ||
         (isa<SgTypeString>(ty) != nullptr) ||
         (isa<SgTypeTuple>(ty) != nullptr) ||
         (isa<SgTypeUnknown>(ty) != nullptr) ||
         (isa<SgTypeUnsigned128bitInteger>(ty) != nullptr) ||
         (isa<SgTypeUnsignedChar>(ty) != nullptr) ||
         (isa<SgTypeUnsignedInt>(ty) != nullptr) ||
         (isa<SgTypeUnsignedLong>(ty) != nullptr) ||
         (isa<SgTypeUnsignedLongLong>(ty) != nullptr) ||
         (isa<SgTypeUnsignedShort>(ty) != nullptr) ||
         (isa<SgTypeVoid>(ty) != nullptr) ||
         (isa<SgTypeWchar>(ty) != nullptr) ||
         (isa<SgTypeComplex>(ty) != nullptr);
}

//  Given an AST of SgTypes rooted under `haystack`, replace all occurrences
//  of `needle` with `repl`. TODO: Either merge this into the CMars API, or
//  submit for merging into rose-develop.
bool replace_type(const SgType &needle, SgType *repl, SgType *haystack,
                  CSageCodeGen *cg) {
  bool res = false;
  if (auto *ptr = isa<SgPointerType>(haystack)) {
    if (SgType *base = ptr->get_base_type()) {
      if (base == &needle) {
        res = true;
        ptr->set_base_type(repl);
      } else if (auto *tydef = isa<SgTypedefType>(base)) {
        if (&needle == tydef->get_base_type()) {
          res = true;
          ptr->set_base_type(repl);
        }
      } else {
        res |= replace_type(needle, repl, base, cg);
      }
    }
  } else if (auto *fnty = isa<SgFunctionType>(haystack)) {
    if (SgType *base = fnty->get_return_type()) {
      if (base == &needle) {
        res = true;
        fnty->set_return_type(repl);
        // we need to set both return type and original return type, otherwise
        // it will not work, i.e. unparsed output is not correct
        fnty->set_orig_return_type(repl);
      } else if (auto *tydef = isa<SgTypedefType>(base)) {
        if (&needle == tydef->get_base_type()) {
          res = true;
          fnty->set_return_type(repl);
          fnty->set_orig_return_type(repl);
        }
      } else {
        res |= replace_type(needle, repl, base, cg);
      }
    }
    for (size_t i = 0; i < fnty->get_arguments().size(); i += 1) {
      DEBUG(cerr << "replace_type: argty: " << fnty->get_arguments()[i]
                 << "\n");
      if (fnty->get_arguments()[i] == &needle) {
        res = true;
        fnty->get_arguments()[i] = repl;
      } else {
        res |= replace_type(needle, repl, fnty->get_arguments()[i], cg);
      }
    }
  } else if (auto *arrty = isa<SgArrayType>(haystack)) {
    if (SgType *base = arrty->get_base_type()) {
      if (base == &needle) {
        res = true;
        arrty->set_base_type(repl);
      } else if (auto *tydef = isa<SgTypedefType>(base)) {
        if (&needle == tydef->get_base_type()) {
          res = true;
          arrty->set_base_type(repl);
        }
      } else {
        res |= replace_type(needle, repl, base, cg);
      }
    }
  } else if (auto *clsty = isa<SgClassType>(haystack)) {
    assert(clsty != &needle && "Should have replaced clsty from its parent.");
    if (auto *tc = isa<SgTemplateInstantiationDecl>(clsty->get_declaration())) {
      for (SgTemplateArgument *targ : tc->get_templateArguments()) {
        if (auto *tt = targ->get_type()) {
          if (tt == &needle) {
            targ->set_type(repl);
            res = true;
          } else if (auto *tydef = isa<SgTypedefType>(tt)) {
            if (&needle == tydef->get_base_type()) {
              res = true;
              targ->set_type(repl);
            }
          } else {
            res |= replace_type(needle, repl, tt, cg);
          }
        }
      }
      if (auto def = tc->get_definingDeclaration()) {
        if (auto *tc_def = isa<SgTemplateInstantiationDecl>(def)) {
          for (SgTemplateArgument *targ : tc_def->get_templateArguments()) {
            if (auto *tt = targ->get_type()) {
              if (tt == &needle) {
                targ->set_type(repl);
                res = true;
              } else if (auto *tydef = isa<SgTypedefType>(tt)) {
                if (&needle == tydef->get_base_type()) {
                  res = true;
                  targ->set_type(repl);
                }
              } else {
                res |= replace_type(needle, repl, tt, cg);
              }
            }
          }
        }
      }
    }
  } else if (auto *mod = isa<SgModifierType>(haystack)) {
    if (SgType *base = mod->get_base_type()) {
      if (base == &needle) {
        res = true;
        mod->set_base_type(repl);
      } else if (auto *tydef = isa<SgTypedefType>(base)) {
        if (&needle == tydef->get_base_type()) {
          res = true;
          mod->set_base_type(repl);
        }
      } else {
        res |= replace_type(needle, repl, base, cg);
      }
    }
  } else if (auto *qual = isa<SgQualifiedNameType>(haystack)) {
    if (SgType *base = qual->get_base_type()) {
      if (base == &needle) {
        res = true;
        qual->set_base_type(repl);
      } else if (auto *tydef = isa<SgTypedefType>(base)) {
        if (&needle == tydef->get_base_type()) {
          res = true;
          qual->set_base_type(repl);
        }
      } else {
        res |= replace_type(needle, repl, base, cg);
      }
    }
  } else if (auto *ref = isa<SgReferenceType>(haystack)) {
    if (SgType *base = ref->get_base_type()) {
      if (base == &needle) {
        res = true;
        ref->set_base_type(repl);
      } else if (auto *tydef = isa<SgTypedefType>(base)) {
        if (&needle == tydef->get_base_type()) {
          res = true;
          ref->set_base_type(repl);
        }
      } else {
        res |= replace_type(needle, repl, base, cg);
      }
    }
  } else if (auto *ref = isa<SgRvalueReferenceType>(haystack)) {
    if (SgType *base = ref->get_base_type()) {
      if (base == &needle) {
        res = true;
        ref->set_base_type(repl);
      } else if (auto *tydef = isa<SgTypedefType>(base)) {
        if (&needle == tydef->get_base_type()) {
          res = true;
          ref->set_base_type(repl);
        }
      } else {
        res |= replace_type(needle, repl, base, cg);
      }
    }
  } else if (helpers::leaf_type(haystack) ||
             (isa<SgTypedefType>(haystack) != nullptr) ||
             (isa<SgTemplateType>(haystack) != nullptr) ||
             (isa<SgNamedType>(haystack) != nullptr)) {
    DEBUG(cerr << "leaf type: " << haystack << "\n");
  } else {
    DEBUG(cerr << "unknown type node: " << haystack->class_name() << " "
               << haystack << "\n");
    assert(false && "Unknown, unhandled type node.");
  }
  return res;
}

string get_template_arg_str(CSageCodeGen *cg, const SgTemplateArgument &targ) {
  DEBUG(cerr << "analysis template arg " << &targ << "\n");
  if (SgType *tyarg = targ.get_type()) {
    return cg->GetStringByType(tyarg);
  }
  if (SgExpression *expr = targ.get_expression()) {
    if (expr->get_type()->isIntegerType()) {
      MarsProgramAnalysis::CMarsExpression idx(expr, cg);
      assert(idx.IsConstant() && "Mangler expected a constant prim-expr.");

      //  GetConstant returns some signed integer type.
      auto c = idx.GetConstant();
      std::stringstream out;
      out << c;
      return out.str();
    } else {
      return expr->unparseToString();
    }
  }
  DEBUG(cerr << "TODO: analysis this: " << &targ << "\n");
  assert(false && "Unhandled template arg kind.");
  return "";
}

SgStatement *get_insert_position(SgStatement *decl, CSageCodeGen *cg) {
  SgStatement *s = decl;
  while ((s->get_scope() != nullptr) &&
         (isa<SgGlobal>(s->get_scope()) == nullptr)) {
    s = s->get_scope();
  }
  assert(isa<SgGlobal>(s->get_scope()));
  SgStatement *res = s;
  if (auto *ns = isa<SgNamespaceDefinitionStatement>(s)) {
    res = ns->get_namespaceDeclaration();
  } else if (auto *def = isa<SgClassDefinition>(s)) {
    res = def->get_declaration();
  } else if (auto *def = isa<SgFunctionDefinition>(s)) {
    res = def->get_declaration();
  } else if (isa<SgScopeStatement>(s)) {
    DEBUG(cerr << "inserting before an "
                  "unhandled scope type (which could result in "
                  "a crash): "
               << s->unparseToString() << "\n");
  }
  void *prev_stmt = res;
  while (cg->IsPragmaDecl(cg->GetPreviousStmt(prev_stmt))) {
    prev_stmt = cg->GetPreviousStmt(prev_stmt);
  }
  return static_cast<SgStatement *>(prev_stmt);
}

//  This exists solely to work around the following ROSE bug: When presented
//  with multiple input files, ROSE will generate an SgGlobal
void insert_fn_decl(CSageCodeGen *cg, SgStatement *inst,
                    SgFunctionDeclaration *fndecl, bool before) {
  SgScopeStatement *oldscope = inst->get_scope();
  assert(oldscope &&
         "Expected the insert point to not be floating and have a scope.");
  if (fndecl->get_definition() == nullptr) {
    auto *correctscope = isa<SgScopeStatement>(inst->get_parent());
    assert(correctscope && "Expected fn decl to be child of a scope.");
    inst->set_scope(correctscope);
  }

  if (before) {
    cg->InsertStmt(fndecl, inst);
  } else {
    cg->InsertAfterStmt(fndecl, inst);
  }

  if (fndecl->get_definition() == nullptr) {
    inst->set_scope(oldscope);
  }
}

//  Given a declaration statement `fn`, return a singly linked list containing
//  its qualification path. Left-to-right = out-to-in. Contrary to its name,
//  `fn` can be any decl statement.
//
//  Note: It's really inefficient that SgNamespaceDeclarationStatement::get_name
//  returns by value instead of by reference, which forces us to allocate
//  strings.
std::forward_list<SgName> qualify(SgScopeStatement *scope) {
  std::forward_list<SgName> rv;
  DEBUG(cerr << "retrieving scope for " << scope->get_qualified_name() << "\n");
  for (SgScopeStatement *gs = scope; isa<SgGlobal>(gs) == nullptr;
       gs = gs->get_scope()) {
    DEBUG(cerr << "qualify: hit scope: " << gs << "\n");
    if (isa<SgNamespaceDefinitionStatement>(gs) != nullptr) {
      DEBUG(cerr << "\tqualified scope! " << gs->associatedScopeName() << "\n");
      rv.push_front(gs->associatedScopeName());
    } else if (auto *clsdef = isa<SgClassDefinition>(gs)) {
      if (auto *tydef = isa<SgTypedefDeclaration>(
              clsdef->get_declaration()->get_parent())) {
        rv.push_front(tydef->get_name());
      } else {
        rv.push_front(gs->associatedScopeName());
      }
    }
  }
  return rv;
}

bool is_ctor(const SgMemberFunctionDeclaration &decl) {
  SgClassDeclaration *cls =
      isa<SgClassDeclaration>(decl.get_associatedClassDeclaration());
  string class_name = cls->get_name();
  if (auto tinstcls = isa<SgTemplateInstantiationDecl>(cls)) {
    class_name = tinstcls->get_templateName();
  }
  return decl.get_name() == class_name;
}

//  This is needed because ROSE treats operator casts as normal (member)
//  functions, i.e., fn.get_specialFunctionModifier().isOperator() will return
//  false for casts.
SgType *get_operator_cast(const SgFunctionDeclaration &fn) {
  string n = fn.get_name().getString();
  return (n.find("operator") == 0 && n.find(' ') != std::string::npos)
             ? fn.get_type()->get_return_type()
             : nullptr;
}

bool is_const_ref_operator_cast(const SgFunctionDeclaration &fn) {
  if (SgType *castty = get_operator_cast(fn)) {
    if (auto *ref = isa<SgReferenceType>(castty)) {
      return SageInterface::isConstType(ref->get_base_type());
    }
  }
  return false;
}

struct Mangler {
  std::ostream &out;
  CSageCodeGen *cg;
  bool valid;
  Mangler(std::ostream &out, CSageCodeGen *cg)
      : out(out), cg(cg), valid(true) {}

 private:
  void mangle_source_name(const char *n, size_t len) {
    //  <source-name> ::= <positive length number> <identifier>
    out << len << n;
  }

  void mangle_source_name(const string &n) {
    //  'n' may be a template class name
    string tmp = cg->legalizeName(n);
    mangle_source_name(tmp.c_str(), tmp.size());
  }

  void mangle_builtin_type(SgType *ty) {
    if (isa<SgTypeVoid>(ty) != nullptr) {
      out << "v";
    } else if (isa<SgTypeWchar>(ty) != nullptr) {
      out << "w";
    } else if (isa<SgTypeBool>(ty) != nullptr) {
      out << "b";
    } else if (isa<SgTypeChar>(ty) != nullptr) {
      out << "c";
    } else if (isa<SgTypeSignedChar>(ty) != nullptr) {
      out << "a";
    } else if (isa<SgTypeUnsignedChar>(ty) != nullptr) {
      out << "h";
    } else if ((isa<SgTypeShort>(ty) != nullptr) ||
               (isa<SgTypeSignedShort>(ty) != nullptr)) {
      out << "s";
    } else if (isa<SgTypeUnsignedShort>(ty) != nullptr) {
      out << "t";
    } else if ((isa<SgTypeInt>(ty) != nullptr) ||
               (isa<SgTypeSignedInt>(ty) != nullptr)) {
      out << "i";
    } else if (isa<SgTypeUnsignedInt>(ty) != nullptr) {
      out << "j";
    } else if ((isa<SgTypeLong>(ty) != nullptr) ||
               (isa<SgTypeSignedLong>(ty) != nullptr)) {
      out << "l";
    } else if (isa<SgTypeUnsignedLong>(ty) != nullptr) {
      out << "m";
    } else if ((isa<SgTypeLongLong>(ty) != nullptr) ||
               (isa<SgTypeSignedLongLong>(ty) != nullptr)) {
      out << "x";
    } else if (isa<SgTypeUnsignedLongLong>(ty) != nullptr) {
      out << "y";
    } else if (isa<SgTypeSigned128bitInteger>(ty) != nullptr) {
      out << "n";
    } else if (isa<SgTypeUnsigned128bitInteger>(ty) != nullptr) {
      out << "o";
    } else if (isa<SgTypeFloat>(ty) != nullptr) {
      out << "f";
    } else if (isa<SgTypeDouble>(ty) != nullptr) {
      out << "d";
    } else if (isa<SgTypeLongDouble>(ty) != nullptr) {
      out << "e";
      // TODO(youxiang): if(isa<SgType'>(ty)) out << "g";
    } else if (isa<SgTypeEllipse>(ty) != nullptr) {
      out << "z";
    } else {
      DEBUG(cerr << "unknown leaf type: " << cg->_p(ty) << "\n");
      assert(false && "Unknown, unhandled leaf type.");
      valid = false;
    }
  }

  //  Mangle a function type. This exists in its own method because special
  //  handling needs to be given to array type arguments, in that they are
  //  decayed into pointer types.
  void mangle_fn_ty(SgFunctionType *fnty, bool isroot) {
    //  For root level function types (ones that are directly returned by
    //  SgFunctionDeclaration::get_type), we ignore the return type because
    //  return types alone do not disambiguate overloaded fn decls.
    if (!isroot) {
      mangle_type(fnty->get_return_type());
    }

    if (fnty->get_arguments().empty()) {
      mangle_type(SgTypeVoid::createType());
    } else {
      for (SgType *argty : fnty->get_arguments()) {
        DEBUG(cerr << "mangle: mangling arg " << argty << "\n");
        if (auto *arrty = isa<SgArrayType>(argty)) {
          //  Array-typed params are adjusted to pointers.
          mangle_type(SgPointerType::createType(arrty->get_base_type()));
          // TODO(youxiang): We might need to convert function types to
          //  pointers, too.
        } else {
          mangle_type(argty);
        }
      }
    }
  }

  // TODO(youxiang): This is not entirely correct because it only mangles the
  // name of
  //  `ty` and ignores its name qualification. In other words, if `ty` were
  //  nested in a namespace, we must emit those as well.
  void mangle_class_type(const SgClassType &ty) {
    if (auto *tc = isa<SgTemplateInstantiationDecl>(ty.get_declaration())) {
      // TODO(youxiang): This is totally non-standard and a hack. We should
      // follow
      //  the ABI spec when we have time.
      mangle_source_name(tc->get_templateName().getString());
      out << "I";
      for (SgTemplateArgument *targ : tc->get_templateArguments()) {
        mangle_template_arg(*targ);
      }
      out << "E";
    } else {
      mangle_source_name(ty.get_name().getString());
    }
  }

  void mangle_type(SgType *ty) {
    DEBUG(cerr << "mangle_type: hit " << cg->_p(ty, 80) << "\n");
    if (auto *ptr = isa<SgPointerType>(ty)) {
      out << "P";
      mangle_type(ptr->get_base_type());
    } else if (auto *fnty = isa<SgFunctionType>(ty)) {
      out << "F";
      mangle_fn_ty(fnty, /*isroot =*/false);
      out << "E";
    } else if (auto *arrty = isa<SgArrayType>(ty)) {
      out << "A";
      // TODO(youxiang): Using CMarsExpression indirectly constrains us to index
      //  values that can fit into a C++ long long. Should instead use an
      //  arbitrary precision type.

      SgExpression *idxexpr = arrty->get_index();
      if ((idxexpr != nullptr) && (isa<SgNullExpression>(idxexpr) == nullptr)) {
        MarsProgramAnalysis::CMarsExpression idx(idxexpr, cg);
        DEBUG(cerr << "hit an array type. dumping index expr ast:\n");
        DEBUG(cg->TraversePre(idxexpr, [&](SgNode *n) {
          cerr << "node: " << cg->_p(n, 80) << "\n\t" << n->class_name()
               << "\n";
        }));
        if (idx.IsConstant()) {
          out << idx.GetConstant();
        } else {
          cerr << "Dimension of array decls should be constant."
               << cg->_p(idxexpr, 0) << "\n";
          valid = false;
        }
      }
      out << "_";
      mangle_type(arrty->get_base_type());
    } else if (auto *clsty = isa<SgClassType>(ty)) {
      mangle_class_type(*clsty);
    } else if (auto *enty = isa<SgEnumType>(ty)) {
      out << enty->get_name().getString();
    } else if (auto *tydef = isa<SgTypedefType>(ty)) {
      //  ROSE treats __builtin_va_list as a typedef.
      if (tydef->get_name().getString() == "__builtin_va_list") {
        //  Note that va_list treatment is architecture-dependent. We
        //  currently follow x86-64 clang's behavior, which treats
        //  va_list args as `__va_list_tag *` types.
        out << "P";
        mangle_source_name("__va_list_tag");
      } else if (cg->IsAnonymousType(tydef->get_base_type()) != 0) {
        out << tydef->get_name().getString();
      } else {
        mangle_type(tydef->get_base_type());
      }
    } else if (auto *c = isa<SgTypeComplex>(ty)) {
      out << "C";
      mangle_type(c->get_base_type());
    } else if (auto *mod = isa<SgModifierType>(ty)) {
      auto &cv = mod->get_typeModifier().get_constVolatileModifier();
      if (cv.isConst()) {
        out << "K";
      } else if (cv.isVolatile()) {
        out << "V";
      }
      mangle_type(mod->get_base_type());
    } else if (auto *qual = isa<SgQualifiedNameType>(ty)) {
      // TODO(youxiang): Itanium has its own spec for qualified types. Do that.
      mangle_type(qual->get_base_type());
    } else if (auto *ref = isa<SgReferenceType>(ty)) {
      out << "R";
      mangle_type(ref->get_base_type());
    } else if (auto *rref = isa<SgRvalueReferenceType>(ty)) {
      out << "O";
      mangle_type(rref->get_base_type());
    } else if (helpers::leaf_type(ty)) {
      mangle_builtin_type(ty);
    } else {
      cerr << "unknown type node: " << ty << "\n";
      // assert(false && "Unknown, unhandled type node.");
      valid = false;
    }
  }

  void mangle_template_arg(const SgTemplateArgument &targ) {
    DEBUG(cerr << "mangling template arg " << &targ << "\n");
    if (SgType *tyarg = targ.get_type()) {
      mangle_type(tyarg);
    } else if (SgExpression *expr = targ.get_expression()) {
      if (expr->get_type()->isIntegerType()) {
        out << "L";
        mangle_type(expr->get_type());
        MarsProgramAnalysis::CMarsExpression idx(expr, cg);
        assert(idx.IsConstant() && "Mangler expected a constant prim-expr.");

        //  GetConstant returns some signed integer type.
        auto c = idx.GetConstant();

        if (c < 0) {
          out << "n" << (-c) << "E";
        } else {
          out << c << "E";
        }
      } else {
        out << expr->unparseToString();
      }
    } else {
      cerr << "TODO: Mangle this: " << &targ << "\n";
      // assert(false && "Unhandled template arg kind.");
      valid = false;
    }
  }

  const char *op_fn_mangled(string op, bool is_unary) {
    auto u32 = [](const char *s) {
      return *reinterpret_cast<const uint32_t *>(s);
    };

    //  Copied from the Itanium mangling spec.
    //  https://  itanium-cxx-abi.github.io/cxx-abi/abi.html#mangling-operator
    // TODO(youxiang): Handle:
    //  ::= nw    # new
    //  ::= na    # new[]
    //  ::= dl    # delete
    //  ::= da    # delete[]
    //
    //  ::= qu    # ?
    //             ^ what is this?!
    //  ::= cv <type>    # (cast)
    //  ::= li <source-name>          # operator ""
    //  ::= v <digit> <source-name>    # vendor extended operator
    //
    //  The key is a pair of (operator symbol, is unary).
    static const std::map<std::pair<uint32_t, bool>, const char *> op_to_abbrev{
        {{u32("+  "), true}, "ps"},  {{u32("-  "), true}, "ng"},
        {{u32("&  "), true}, "ad"},  {{u32("*  "), true}, "de"},
        {{u32("+  "), false}, "pl"}, {{u32("-  "), false}, "mi"},
        {{u32("*  "), false}, "ml"}, {{u32("&  "), false}, "an"},
    };

    static const std::map<uint32_t, const char *> uniqe_op_to_abbrev{
        {u32("~  "), "co"}, {u32("/  "), "dv"}, {u32("%  "), "rm"},
        {u32("|  "), "or"}, {u32("^  "), "eo"}, {u32("=  "), "aS"},
        {u32("+= "), "pL"}, {u32("-= "), "mI"}, {u32("*= "), "mL"},
        {u32("/= "), "dV"}, {u32("%= "), "rM"}, {u32("&= "), "aN"},
        {u32("|= "), "oR"}, {u32("^= "), "eO"}, {u32("<< "), "ls"},
        {u32(">> "), "rs"}, {u32("<<="), "lS"}, {u32(">>="), "rS"},
        {u32("== "), "eq"}, {u32("!= "), "ne"}, {u32("<  "), "lt"},
        {u32(">  "), "gt"}, {u32("<= "), "le"}, {u32(">= "), "ge"},
        {u32("!  "), "nt"}, {u32("&& "), "aa"}, {u32("|| "), "oo"},
        {u32("++ "), "pp"}, {u32("-- "), "mm"}, {u32(",  "), "cm"},
        {u32("->*"), "pm"}, {u32("-> "), "pt"}, {u32("() "), "cl"},
        {u32("[] "), "ix"},
    };

    if (op.size() > 3) {
      return nullptr;
    }
    op.resize(4, ' ');
    op[3] = '\0';
    auto it = op_to_abbrev.find({u32(op.c_str()), is_unary});
    if (it != op_to_abbrev.end()) {
      return it->second;
    }
    auto u_it = uniqe_op_to_abbrev.find(u32(op.c_str()));
    return u_it == uniqe_op_to_abbrev.end() ? nullptr : u_it->second;
  }

  //  <unqualified-name> E ::= N [<CV-qualifiers>] [<ref-qualifier>]
  //                            <template-prefix> <template-args> E
  void mangle_unqualified_fn(SgFunctionDeclaration *fndecl) {
    auto *mfn = isa<SgMemberFunctionDeclaration>(fndecl);
    if ((mfn != nullptr) && is_ctor(*mfn)) {
      // TODO(youxiang): What about C1 and C3 constructors?
      out << "C2";
    } else if (fndecl->get_specialFunctionModifier().isOperator()) {
      //  Recall sizeof("a") == 2 because of the implicit terminating
      //  null byte.
      DEBUG(cerr << "mangling an operator fn: " << cg->_p(fndecl, 80) << "\n");

      //  Figure out the true arity of this operator fn. This is not
      //  exactly trivial, because the arity for member op fns would be one
      //  less than that of non-member op fn.
      unsigned arity = fndecl->get_args().size();
      arity = mfn != nullptr ? arity : arity - 1;

      if (const char *mg = op_fn_mangled(
              fndecl->get_name().getString().substr(sizeof("operato")),
              arity == 0u)) {
        DEBUG(cerr << "mangled op is " << mg << "\n");
        out << mg;
      } else {
        cerr << "Unhandled operator function " << fndecl->get_name() << endl;
        valid = false;
      }
    } else if (SgType *castty = helpers::get_operator_cast(*fndecl)) {
      DEBUG(cerr << "assuming that this is a udf cast. cast type is "
                 << cg->_p(castty) << "\n");
      out << "cv";
      mangle_type(castty);
    } else if (auto *tfinst =
                   isa<SgTemplateInstantiationFunctionDecl>(fndecl)) {
      // TODO(youxiang): Find some way to ensure that this is a fully
      // specialized
      //  instantiation (SgDeclarationStatement::get_specialization does
      //  not work).
      if (auto tf = tfinst->get_templateDeclaration()) {
        mangle_source_name(tf->get_name().getString());
      } else {
        mangle_source_name(tfinst->get_name().getString());
      }
    } else if (auto *tmfinst =
                   isa<SgTemplateInstantiationMemberFunctionDecl>(fndecl)) {
      // TODO(youxiang): Find some way to ensure that this is a fully
      // specialized
      //  instantiation (SgDeclarationStatement::get_specialization does
      //  not work).
      if (auto tmf = tmfinst->get_templateDeclaration()) {
        mangle_source_name(tmf->get_name().getString());
      } else {
        mangle_source_name(tmfinst->get_name().getString());
      }

    } else {
      mangle_source_name(fndecl->get_name().getString());
    }

    if (auto *tfinst = isa<SgTemplateInstantiationFunctionDecl>(fndecl)) {
      out << "I";
      for (SgTemplateArgument *targ : tfinst->get_templateArguments()) {
        mangle_template_arg(*targ);
      }
      out << "E";
    } else if (auto *tfinst =
                   isa<SgTemplateInstantiationMemberFunctionDecl>(fndecl)) {
      out << "I";
      for (SgTemplateArgument *targ : tfinst->get_templateArguments()) {
        mangle_template_arg(*targ);
      }
      out << "E";
    }
  }

  //  <nested-name> ::= N [<CV-qualifiers>] [<ref-qualifier>] <prefix>
  template <typename Decl, typename UnqualifiedMangler>
  void mangle_nested_name(Decl *decl, UnqualifiedMangler &&um) {
    std::forward_list<SgName> path = qualify(decl->get_scope());
    if (!path.empty()) {
      out << "N";
      if (auto *mfnty = isa<SgMemberFunctionType>(decl->get_type())) {
        if (mfnty->isVolatileFunc()) {
          out << "V";
        }
        if (mfnty->isConstFunc()) {
          out << "K";
        }
      }
      for (const SgName &component : path) {
        const string &n = component.getString();
        if (!n.empty()) {
          mangle_source_name(n);
        } else {
          //  Unnamed namespace, which means internal linkage, which
          //  means any random name could be assigned. We follow
          //  clang's convention of _GLOBAL__N_1.
          mangle_source_name("_GLOBAL__N_1", sizeof("_GLOBAL__N_"));
        }
      }
    }
    um();
    if (!path.empty()) {
      out << "E";
    }
  }

 public:
  //  Top-level entry point.
  bool mangle(SgFunctionDeclaration *fndecl) {
    //  <mangled-name> ::= _Z <encoding>
    //  <encoding> ::= <function name> <bare-function-type>
    //            ::= <data name>
    //            ::= <special-name>

    DEBUG(cerr << "mangle root: " << fndecl->get_name() << "\n");
    // TODO(youxiang): The Itanium spec requires _Z, but we intentionally use
    // p_Z to
    //  prevent Xilinx's report from de-mangling our mangled output. Remove
    //  this once we have a de-mangler API that our own reporting tools can
    //  use to generate names that match the HLS report.
    out << "p_Z";

    mangle_nested_name(fndecl, [&]() { mangle_unqualified_fn(fndecl); });
    mangle_fn_ty(fndecl->get_type(), /*isroot = */ true);
    return valid;
  }

  void mangle_var(SgInitializedName *var) {
    DEBUG(cerr << "mangle_var: " << cg->_p(var, 80) << "\n");
    out << "_Z";
    mangle_nested_name(var, [&]() { mangle_source_name(var->get_name()); });
  }
};

SgName cls_name(SgClassDeclaration *decl) {
  if (auto *tcinst = isa<SgTemplateInstantiationDecl>(decl)) {
    return tcinst->get_templateName();
  }
  return decl->get_name();
}

bool reserved_class(SgClassDeclaration *decl) {
  static const std::set<string> reserveds{
      "ap_private",   "ap_int",      "ap_uint",       "ap_fixed",
      "ap_ufixed",    "ap_int_base", "ap_fixed_base", "af_range_ref",
      "ap_range_ref", "ap_bit_ref",  "af_bit_ref",    "ap_concat_ref",
      "stream",       "complex"};
  if (reserveds.find(cls_name(decl)) != reserveds.end()) {
    return true;
  }
  if (auto *p_decl = isa<SgClassDeclaration>(decl->get_scope())) {
    return reserved_class(p_decl);
  }
  if (auto *defn = isa<SgClassDefinition>(decl->get_scope())) {
    return reserved_class(defn->get_declaration());
  }
  if (auto *tcinst = isa<SgTemplateInstantiationDefn>(decl->get_scope())) {
    return reserved_class(tcinst->get_declaration());
  }
  return false;
}

bool is_within_hls_namespace(SgNode *n) {
  auto p = n;
  while (p) {
    auto ns = isa<SgNamespaceDeclarationStatement>(p);
    if (ns != nullptr && ns->get_name() == "hls") {
      return true;
    }
    p = p->get_parent();
  }

  return false;
}

bool should_lower_sub(SgNode *n, CSageCodeGen *cg) {
  static unordered_map<SgNode *, bool> cachedRes;
  if (cachedRes.count(n) > 0) {
    return cachedRes[n];
  }

  if (is_within_hls_namespace(n))
    return false;  // ZP: Nov-2019: keep hls:: elements untouched

  if (isa<SgTemplateClassDeclaration>(n) || isa<SgTemplateClassDefinition>(n) ||
      isa<SgTemplateFunctionDeclaration>(n) ||
      isa<SgTemplateMemberFunctionDeclaration>(n) ||
      isa<SgTemplateFunctionDefinition>(n) ||
      (isa<SgTemplateInstantiationDecl>(n) &&
       static_cast<SgTemplateInstantiationDecl *>(n)
           ->isPartialSpecialization()) ||
      ((isa<SgTemplateInstantiationFunctionDecl>(n) ||
        isa<SgTemplateInstantiationMemberFunctionDecl>(n)) &&
       static_cast<SgFunctionDeclaration *>(n)->isPartialSpecialization())) {
    DEBUG(cerr << "should_lower: ignoring\n");
    cachedRes[n] = false;
    return false;
  }

  if (auto *tcinst = isa<SgTemplateInstantiationDefn>(n)) {
    DEBUG(cerr << "should_lower: caught a tcinst " << cg->_p(tcinst, 80)
               << "\n");
    if (reserved_class(tcinst->get_declaration())) {
      DEBUG(cerr << "should_lower: ignoring\n");
      cachedRes[n] = false;
      return false;
    }
  } else if (auto *decl = isa<SgClassDeclaration>(n)) {
    if (reserved_class(decl)) {
      DEBUG(cerr << "should_lower: avoiding decl " << cg->_p(decl, 80) << "\n");
      cachedRes[n] = false;
      return false;
    }
  } else if (auto *tmfn = isa<SgMemberFunctionDeclaration>(n)) {
    if (reserved_class(
            isa<SgClassDeclaration>(tmfn->get_associatedClassDeclaration()))) {
      DEBUG(cerr << "should_lower: ignoring memfn " << cg->_p(tmfn, 80)
                 << "\n");
      cachedRes[n] = false;
      return false;
    }
  } else if (auto *def = isa<SgClassDefinition>(n)) {
    if (reserved_class(def->get_declaration())) {
      DEBUG(cerr << "should_lower: ignoring def " << cg->_p(def, 80) << "\n");
      cachedRes[n] = false;
      return false;
    }
  } else if (auto *tfn = isa<SgTemplateInstantiationFunctionDecl>(n)) {
    if (cg->IsSystemFunction(tfn) != 0) {
      DEBUG(cerr << "should_lower: ignoring tfn " << cg->_p(tfn, 80) << "\n");
      cachedRes[n] = false;
      return false;
    }
  }

  if (auto *fn = isa<SgFunctionDeclaration>(n)) {
    std::stringstream mangled;
    if (!helpers::Mangler(mangled, cg).mangle(fn)) {
      DEBUG(cerr << "should_lower: ignoring fn " << cg->_p(fn, 80) << "\n");
      cachedRes[n] = false;
      return false;
    }
  }

  if (auto *decl = isa<SgDeclarationStatement>(n)) {
    bool res = should_lower_sub(decl->get_scope(), cg);
    cachedRes[n] = res;
    return res;
  }

  cachedRes[n] = true;
  return true;
}

bool should_lower(SgNode *n, CSageCodeGen *cg) {
  bool res = should_lower_sub(n, cg);
  return res;
}

bool is_cpp_agg(SgClassDefinition *def) {
  if (isa<SgTemplateInstantiationDefn>(def))
    return true;
  if (!isa<SgGlobal>(def->get_declaration()->get_scope()))
    return true;
  if (!def->get_inheritances().empty())
    return true;
  return any_of(def->get_members(), [&](SgDeclarationStatement *memdecl) {
    return ((isa<SgVariableDeclaration>(memdecl) == nullptr) &&
            (isa<SgPragmaDeclaration>(memdecl) == nullptr)) ||
           SageInterface::isStatic(memdecl);
  });
}
}  //  namespace helpers

using helpers::isa;
using helpers::isa_or_null;

#if DISABLE_DEBUG
#else
std::ostream &operator<<(std::ostream &out, const SgNode *n) {
  if (n == nullptr) {
    out << "null";
    return out;
  }
  if (auto *plist = isa<SgFunctionParameterList>(n)) {
    for (auto it = plist->get_args().begin(); it != plist->get_args().end();
         it++) {
      if (it != plist->get_args().begin())
        out << ",";
      out << (*it)->get_type()->unparseToString() << " "
          << (*it)->unparseToString();
    }
  } else if (auto *func_decl = isa<SgFunctionDeclaration>(n)) {
    out << func_decl->get_type();
    out << " " << func_decl->get_name().getString() << "(";
    out << func_decl->get_parameterList();
    out << ") {...}";
  } else if (auto *project = isa<SgProject>(n)) {
    out << "_p_";
  } else {
    out << n->unparseToString();
  }
  return out;
}
#endif

class LowerCpp {
 public:
  typedef void (LowerCpp::*Handler)(void *, const vector<void *> &,
                                    vector<void *> *, vector<SgStatement *> *,
                                    SgMemberFunctionDeclaration *);

 private:
  struct RemappedFn {
    void *inner;
    bool is_def;
    bool was_static_mfn = false;
    //  ^ True if and only if this remap was created from a static member
    //  function.

    explicit RemappedFn(SgFunctionDefinition *def) : inner(def), is_def(true) {}
    explicit RemappedFn(SgFunctionDeclaration *decl)
        : inner(decl), is_def(false) {}
    SgFunctionDefinition *get_def() {
      return is_def ? static_cast<SgFunctionDefinition *>(inner) : nullptr;
    }
    SgFunctionDeclaration *get_decl() {
      return is_def ? nullptr : static_cast<SgFunctionDeclaration *>(inner);
    }
    //  forces a decl.
    SgFunctionDeclaration *decl() {
      return get_def() != nullptr ? get_def()->get_declaration() : get_decl();
    }
    RemappedFn &operator=(SgFunctionDefinition *def) {
      inner = def;
      is_def = true;
      return *this;
    }
    RemappedFn &operator=(SgFunctionDeclaration *decl) {
      inner = decl;
      is_def = false;
      return *this;
    }
  };

  std::set<const SgFunctionSymbol *> set_kernel;
  std::unordered_map<const SgFunctionSymbol *, RemappedFn> fn_remap;
  //  ^ Maps symbols of unlowered functions to their lowered counterparts. If
  //  the function has no definition (e.g., extern decl), then its declaration
  //  is stored.
  std::unordered_map<const SgTemplateClassSymbol *,
                     SgTemplateClassDeclaration *>
      tcs_syms;
  std::unordered_map<const SgTemplateFunctionSymbol *,
                     SgTemplateFunctionDeclaration *>
      tfn_syms;
  std::unordered_map<const SgTemplateMemberFunctionSymbol *,
                     SgTemplateMemberFunctionDeclaration *>
      tmfn_syms;
  //  ^ For unspecialized template function declaration, we map its symbol to
  //  its defining declaration. This primarily used to answer queries of the
  //  form, "Given an instantiation f<A>, find the AST node for unspecialized
  //  f."

  std::unordered_map<const SgClassSymbol *, SgClassDefinition *> cls_remap;

  std::unordered_map<const SgClassSymbol *, SgClassDefinition *> sym_to_def;
  std::unordered_multimap<const void *, const void *> cls_topo;

  std::unordered_map<const SgTypedefSymbol *, SgTypedefDeclaration *>
      typedef_remap;
  std::unordered_map<const SgEnumSymbol *, SgEnumDeclaration *> enum_remap;
  std::unordered_map<const SgTypedefSymbol *, SgClassDefinition *>
      typedef2cls_remap;
  //  ^ ROSE is horrible. At various points of our pass, we will need to
  //  retrieve the definition from some class symbol. This is different from
  //  cls_remap because this maintains the correspondence between old sym and
  //  old def.

  std::set<const SgFunctionSymbol *> lowered_fns;
  //  ^ Tracks the symbols of all the functions generated from lowering.

  std::unordered_map<string, string> this_map;
  //  ^ For each member function with a body (presumably mangled to a unique
  //  SgName), track the variable name that represents `this`.

  std::unordered_map<const SgFunctionSymbol *, string> mangle_cache;

  std::unordered_map<const SgVariableSymbol *,
                     pair<string, SgClassDefinition *>>
      mv_remap;
  //  ^ Records the correspondence between member variables of old aggs and
  //  lowered ones. Needed to remap SgVarRefExps that reference the old member
  //  variables.

  std::unordered_map<const SgVariableSymbol *, SgInitializedName *>
      static_remap;
  //  ^ Maps from old static duration variable to lowered counterpart.

  std::forward_list<SgStatement *> del_queue;
  std::unordered_set<SgStatement *> del_set;
  CSageCodeGen *cg;
  CMarsIrV2 m_mars_ir;
  bool err;

 private:
  void
  get_token_remap_in_template_class(const SgTemplateInstantiationDecl &tcinst,
                                    unordered_map<string, string> *token_map) {
    auto orig_tc = [&](const SgTemplateInstantiationDecl &tcinst)
        -> SgTemplateClassDeclaration * {
      auto tc = tcinst.get_templateDeclaration();
      if (nullptr == tc ||
          tc->get_declaration_associated_with_symbol() == nullptr)
        return nullptr;
      auto declsym =
          isa<SgTemplateClassSymbol>(tc->search_for_symbol_from_symbol_table());
      assert(declsym && "Expected a symbol to unspecialized template.");
      auto it = tcs_syms.find(declsym);
      return it != tcs_syms.end() ? it->second : nullptr;
    };
    auto realtc = orig_tc(tcinst);
    if (realtc == nullptr)
      return;
    vector<string> vec_tp_args;
    for (SgTemplateArgument *targ : tcinst.get_templateArguments()) {
      string targ_str = helpers::get_template_arg_str(cg, *targ);
      vec_tp_args.push_back(targ_str);
    }
    int tp_pos = 0;
    for (SgTemplateParameter *tp : realtc->get_templateParameters()) {
      string targ_name = cg->_up(tp);  //  tp->get_mangled_name();
      if (tp_pos < vec_tp_args.size()) {
        string targ_str = vec_tp_args[tp_pos];
        DEBUG(cerr << "targ: " << targ_name << " -> " << targ_str << "\n");
        (*token_map)[targ_name] = targ_str;
      }
      tp_pos++;
    }
    return;
  }

  void get_token_remap_in_template_function(
      const SgTemplateInstantiationFunctionDecl &tfinst,
      unordered_map<string, string> *token_map) {
    auto orig_tf = [&](const SgTemplateInstantiationFunctionDecl &tfinst)
        -> SgTemplateFunctionDeclaration * {
      auto tf = tfinst.get_templateDeclaration();
      if (nullptr == tf ||
          tf->get_declaration_associated_with_symbol() == nullptr)
        return nullptr;
      auto declsym = isa<SgTemplateFunctionSymbol>(
          tf->search_for_symbol_from_symbol_table());
      assert(declsym && "Expected a symbol to unspecialized template.");
      auto it = tfn_syms.find(declsym);
      return it != tfn_syms.end() ? it->second : nullptr;
    };
    auto realtf = orig_tf(tfinst);
    if (realtf == nullptr)
      return;
    vector<string> vec_tp_args;
    for (SgTemplateArgument *targ : tfinst.get_templateArguments()) {
      string targ_str = helpers::get_template_arg_str(cg, *targ);
      vec_tp_args.push_back(targ_str);
    }
    int tp_pos = 0;
    for (SgTemplateParameter *tp : realtf->get_templateParameters()) {
      string targ_name = cg->_up(tp);  //  tp->get_mangled_name();
      if (tp_pos < vec_tp_args.size()) {
        string targ_str = vec_tp_args[tp_pos];
        DEBUG(cerr << "targ: " << targ_name << " -> " << targ_str << "\n");
        (*token_map)[targ_name] = targ_str;
      }
      tp_pos++;
    }
    return;
  }

  void get_token_remap_in_template_member_function(
      const SgTemplateInstantiationMemberFunctionDecl &tmfinst,
      unordered_map<string, string> *token_map) {
    auto orig_tmf =
        [&](const SgTemplateInstantiationMemberFunctionDecl &tmfinst)
        -> SgTemplateMemberFunctionDeclaration * {
      auto tmf = tmfinst.get_templateDeclaration();
      if (tmf == nullptr ||
          tmf->get_declaration_associated_with_symbol() == nullptr)
        return nullptr;
      auto declsym = isa<SgTemplateMemberFunctionSymbol>(
          tmf->search_for_symbol_from_symbol_table());
      assert(declsym && "Expected a symbol to unspecialized template.");
      auto it = tmfn_syms.find(declsym);
      return it != tmfn_syms.end() ? it->second : nullptr;
    };
    auto realtmf = orig_tmf(tmfinst);
    if (realtmf == nullptr)
      return;
    vector<string> vec_tp_args;
    for (SgTemplateArgument *targ : tmfinst.get_templateArguments()) {
      string targ_str = helpers::get_template_arg_str(cg, *targ);
      vec_tp_args.push_back(targ_str);
    }
    int tp_pos = 0;
    for (SgTemplateParameter *tp : realtmf->get_templateParameters()) {
      string targ_name = cg->_up(tp);  //  tp->get_mangled_name();
      if (tp_pos < vec_tp_args.size()) {
        string targ_str = vec_tp_args[tp_pos];
        DEBUG(cerr << "targ: " << targ_name << " -> " << targ_str << "\n");
        (*token_map)[targ_name] = targ_str;
      }
      tp_pos++;
    }
    return;
  }

  bool duplicated_function_definition(SgFunctionDeclaration *decl) {
    if (decl->get_definition() == nullptr)
      return false;
    auto old = helpers::fn_symbol(decl);
    auto it = fn_remap.find(old);
    if (it != fn_remap.end()) {
      if (it->second.get_def() != nullptr) {
        return true;
      }
    }
    return false;
  }

 public:
  LowerCpp(CSageCodeGen *cg, void *pTopFunc) : cg(cg), err(false) {
    m_mars_ir.build_mars_ir(cg, pTopFunc);
  }

  LowerCpp &delete_later(SgStatement *node) {
    DEBUG(cerr << "scheduling for deletion " << node << "\n");
    del_queue.push_front(node);
    del_set.insert(node);
    return *this;
  }

  void delete_now(SgStatement *stmt) {
    if (cg->is_floating_node(stmt) != 0) {
      return;
    }
    if ((isa<SgTemplateInstantiationDecl>(stmt) ||
         isa<SgTemplateInstantiationFunctionDecl>(stmt) ||
         isa<SgTemplateMemberFunctionDeclaration>(stmt) ||
         isa<SgTemplateInstantiationMemberFunctionDecl>(stmt)) &&
        cg->IsCompilerGenerated(stmt)) {
      return;
    }
    if (isa<SgMemberFunctionDeclaration>(stmt) &&
        isa<SgClassDefinition>(stmt->get_parent())) {
      return;
    }
    DEBUG(cerr << "deleting statement " << cg->_p(stmt, 80) << "\n");

    //  We want to track deleted nodes for testing purposes. So, instead
    //  of wholesale deletion, we replace each to-be-deleted node with a
    //  non-defining fn decl (which are almost always harmless)
    //  containing the original node's AST information. In production,
    //  these marker fn decls would eventually be deleted.
    void *dummy_stmt = nullptr;
    if (cg->TraceUpToFuncDecl(cg->GetParent(stmt)) == nullptr) {
      string dummy_func = cg->legalizeName(cg->_p(stmt, 10));
      vector<string> empty;
      dummy_stmt =
          cg->CreateFuncDecl("void", "__merlin_dummy_" + dummy_func, empty,
                             empty, cg->GetGlobal(stmt), false, stmt);

      //  `stmt` could be a function-scope statement, in which case its
      //  deletion tag `new_decl` would be a nested function decl. Such
      //  decls cannot be static.
      cg->SetStatic(dummy_stmt);
    } else {
      dummy_stmt = cg->CreateStmt(V_SgExprStatement,
                                  cg->CreateExp(V_SgNullExpression), stmt);
    }

    cg->ReplaceStmt(stmt, dummy_stmt, true);
  }

  void add_fn_remap(const SgFunctionSymbol &old, SgFunctionDefinition *new_,
                    bool static_ = false) {
    //  Ensure that `old` hasn't already been remapped. Remapping the same
    //  function twice is generally a bad sign. For instance, it could happen
    //  upon encountering two separate definitions for the same member
    //  function, which breaks the rules for well-formed C++.
    DEBUG(cerr << "add_fn_remap from " << old.get_name() << " to " << new_
               << "\n");
    auto it = fn_remap.find(&old);
    if (it != fn_remap.end()) {
      DEBUG(cerr << "encountered previous decl " << &old
                 << "\n\tasserting that it was a non-defining decl.\n");
      if (it->second.get_def() == nullptr) {
        it->second = new_;
      }
      assert(it->second.get_def() && "wtf.");
    } else {
      it = fn_remap.insert({&old, RemappedFn(new_)}).first;
    }
    lowered_fns.insert(helpers::fn_symbol(new_->get_declaration()));
    it->second.was_static_mfn |= static_;
  }

  void add_decl_remap(const SgFunctionSymbol &old, SgFunctionDeclaration *d,
                      bool static_ = false) {
    DEBUG(cerr << "add_decl_remap from " << old.get_name() << " to " << d
               << "\n");
    auto it = fn_remap.find(&old);
    if (it != fn_remap.end()) {
      //  `fn_remap[&old]` could either be a def or a decl. Either way, it
      //  would be safe to ignore `d`.
      return;
    }
    it = fn_remap.insert({&old, RemappedFn(d)}).first;
    lowered_fns.insert(helpers::fn_symbol(d));
    it->second.was_static_mfn |= static_;
  }

  bool is_static_memfn(SgFunctionDeclaration *d) {
    if (cg->IsStatic(d)) {
      return true;
    }
    auto it = fn_remap.find(helpers::fn_symbol(d));
    if (it != fn_remap.end()) {
      return it->second.was_static_mfn;
    }
    return false;
  }

  SgClassDeclaration *get_remapped_type(SgClassDeclaration *origcls) const {
    return get_remapped_type(helpers::cls_symbol(origcls));
  }

  SgClassDeclaration *get_remapped_type(SgClassSymbol *origsym) const {
    auto it = cls_remap.find(origsym);
    return it != cls_remap.end() ? it->second->get_declaration() : nullptr;
  }

  //  Convenience function. Given a class, return its lowered type.
  SgClassType *clone_cls_ty(SgClassDeclaration *cls) {
    SgClassDeclaration *rv = get_remapped_type(cls);
    assert(rv && "Should have encountered the class definition prior "
                 "to lowering mem fn decls.");
    return SgClassType::createType(rv);
  }

  SgClassType *clone_cls_ty(SgMemberFunctionDeclaration *decl) {
    return clone_cls_ty(
        isa<SgClassDeclaration>(decl->get_associatedClassDeclaration()));
  }

  //  Clone the def and record the mapping from old to new defs.
  void lower_class_def(SgClassDefinition *def) {
    DEBUG(cerr << "lower_class_def " << cg->_p(def, 80) << "\n");
    if (def->get_scope() != cg->GetGlobal(def)) {
      DEBUG(cerr << "it doesn't live in global scope: " << def->get_scope()
                 << "\n");
    }
    SgClassDeclaration *decl = def->get_declaration();
    if (!helpers::should_lower(decl, cg)) {
      return;
    }

    // E.g. class A : public B {}
    //  =>  class A : public B { B B; }
    // note: the new data member B is not added in the remapped class definition
    // but the original one which is then further lowered. That's a much easier
    // way especially for implicit construction of base class object B.
    // Also, record in a map that A is derived from B for later query about
    // class hierarchy.
    for (SgBaseClass *ba : def->get_inheritances()) {
      SgClassDeclaration *cls = ba->get_base_class();
      SgClassType *type = cls->get_type();
      cls_topo.insert({decl->get_type(), type});
      string id = cg->GetClassName(cls);
      auto var = static_cast<SgVariableDeclaration *>(
          cg->CreateVariableDecl(type, id, nullptr, def));
      // 'prepend' ensures the order in which base class object gets
      // initialized first, then other data members
      def->prepend_member(var);
    }

    //  This could be a `typedef struct T { ... } U;`
    auto *tydef = isa<SgTypedefDeclaration>(decl->get_parent());
    SgClassSymbol *clssym = helpers::cls_symbol(decl);

    //  Copy declaration node. For anonymous typedefs, lower into the
    //  typedef-ed typename.
    SgName n = cg->legalizeName(tydef != nullptr ? tydef->get_qualified_name()
                                                 : helpers::mangle(decl));
    SgClassDeclaration::class_types aggty =
        decl->get_class_type() == SgClassDeclaration::e_union
            ? SgClassDeclaration::e_union
            : SgClassDeclaration::e_struct;
    SgClassDeclaration *newdecl =
        static_cast<SgClassDeclaration *>(cg->CreateClassDecl(
            n, aggty, static_cast<SgScopeStatement *>(cg->GetGlobal(decl)),
            nullptr, false, nullptr));
    //  Copy definition node, and excise non-variable member decls.
    SgClassDefinition *newdef = newdecl->get_definition();
    if (cg->is_floating_node(newdef) == 0) {
      //  When a class definition is in header, which is in multiple files,
      //  we will try to lower the class multiple times. Although we tried
      //  to create a new struct decl, the return struct decl has existed
      //  in AST. To avoid duplicate member fields, we cannot lower the class
      //  multiple times.
      cls_remap[clssym] = newdef;
      if (tydef) {
        typedef2cls_remap[helpers::typedef_symbol(tydef)] = newdef;
      }
      return;
    }

    vector<void *> vec_friend_funcs;
    for (SgDeclarationStatement *memdecl : def->get_members()) {
      if (auto *vardecl = isa<SgVariableDeclaration>(memdecl)) {
        if (cg->IsStatic(vardecl)) {
          continue;
        }
        //  Lower each member variable and record correspondences between
        //  old and new memvar syms.
        for (SgInitializedName *iname : vardecl->get_variables()) {
          bool changed = false;
          auto *mvty = static_cast<SgType *>(
              cg->RemoveConstType(iname->get_type(), nullptr, &changed));
          auto *newvar =
              static_cast<SgVariableDeclaration *>(cg->CreateVariableDecl(
                  mvty, iname->get_name().getString(), nullptr, newdef));
          newdef->append_member(newvar);

          SgVariableSymbol *oldsym = isa<SgVariableSymbol>(
              iname->search_for_symbol_from_symbol_table());
          mv_remap.insert({oldsym, {iname->get_name(), newdef}});
        }
      } else if (auto *pragma = isa<SgPragmaDeclaration>(memdecl)) {
        auto *newprag =
            static_cast<SgPragmaDeclaration *>(cg->CopyStmt(pragma));
        newdef->append_member(newprag);
      } else if (auto *func = isa<SgFunctionDeclaration>(memdecl)) {
        if (!isa<SgMemberFunctionDeclaration>(func)) {
          vec_friend_funcs.push_back(func);
        }
      }
    }
    for (auto friend_func : vec_friend_funcs) {
      // MER-1821 remove friend function declaration
      cg->RemoveStmt(friend_func);
    }
    cls_remap[clssym] = newdef;
    if (tydef) {
      typedef2cls_remap[helpers::typedef_symbol(tydef)] = newdef;
    }

    //  Now insert the newly created declaration/definition before the old
    //  one.
    assert(newdecl && "Expected decl to have been cloned!");

    //  If we're inside a typedef decl, lower adjacent to it.
    auto pos =
        tydef != nullptr ? isa<SgStatement>(tydef) : isa<SgStatement>(decl);
    cg->InsertStmt(newdecl, helpers::get_insert_position(pos, cg));

    if (auto tcinst = isa<SgTemplateInstantiationDecl>(decl)) {
      unordered_map<string, string> token_map;
      get_token_remap_in_template_class(*tcinst, &token_map);
      cg->ReplacePragmaTokens(newdecl, token_map);
    }
    if (tydef != nullptr) {
      delete_later(tydef);
    } else {
      delete_later(decl);
    }
  }

  //  Clone a non-defining aggregate declaration.
  void lower_class_decl(SgClassDeclaration *decl) {
    DEBUG(cerr << "lower_class_decl " << decl << "\n");
    if (decl->get_scope() != cg->GetGlobal(decl)) {
      DEBUG(cerr << "it doesn't live in global scope: " << decl->get_scope()
                 << "\n");
    }

    auto *tydef = isa<SgTypedefDeclaration>(decl->get_parent());

    assert(!decl->get_definition() && "We only lower non-def decls here.");
    SgClassDeclaration::class_types aggty =
        decl->get_class_type() == SgClassDeclaration::e_union
            ? SgClassDeclaration::e_union
            : SgClassDeclaration::e_struct;
    SgClassDeclaration *newdecl =
        SageBuilder::buildNondefiningClassDeclaration_nfi(
            cg->legalizeName(helpers::mangle(decl)), aggty,
            static_cast<SgGlobal *>(cg->GetGlobal(decl)), false, nullptr);

    //  Now insert the newly created declaration/definition before the old
    //  one.
    assert(newdecl && "Expected decl to have been cloned!");
    SgStatement *orig_decl = decl;
    if (nullptr != tydef) {
      orig_decl = tydef;
    }
    //  If we're inside a typedef decl, lower adjacent to it.
    cg->InsertStmt(newdecl, helpers::get_insert_position(orig_decl, cg));
    delete_later(orig_decl);
  }

  bool IsRightCopyCtor(void *ty_decl, void *chk_name,
                       const SgMemberFunctionDeclaration &mfn_decl) {
    //  get all copy ctors;
    //  FIXME: shoule it be stored statically
    vector<void *> members;
    SgClassDeclaration *cls_decl =
        isa<SgClassDeclaration>(static_cast<SgNode *>(ty_decl));
    if ((cls_decl == nullptr) || !helpers::should_lower(cls_decl, cg)) {
      return false;
    }
    SgType *cls_ty = cls_decl->get_type();
    cg->GetClassMembers(cls_ty, &members);
    vector<void *> copy_ctors;
    for (auto &member : members) {
      SgMemberFunctionDeclaration *mem_decl =
          isa<SgMemberFunctionDeclaration>(static_cast<SgNode *>(member));
      if (mem_decl == nullptr) {
        continue;
      }
      if (!helpers::is_ctor(*mem_decl)) {
        continue;
      }
      if (mem_decl->get_args().size() != 1) {
        continue;
      }
      copy_ctors.push_back(member);
    }

    //  arguments stores in ops (only 1 argument)
    //  find the right one in copy_ctors
    //  return true if the found is exactly mfn_decl
    //  FIXME: cls(const cls &&) and cls(cls &&) are not handled for now
    void *chk_ty = cg->GetTypebyVar(chk_name);
    chk_ty = cg->IsReferenceType(chk_ty) != 0 ? cg->GetBaseTypeOneLayer(chk_ty)
                                              : chk_ty;
    bool has_const = cg->IsConstType(chk_ty) != 0;

    if (has_const) {
      //  must use the one with const
      for (auto copy_ctor : copy_ctors) {
        void *param_name = cg->GetFuncParam(copy_ctor, 0);
        //  Wrap under reference type
        void *param_ty = cg->GetTypebyVar(param_name);
        param_ty = cg->IsReferenceType(param_ty) != 0
                       ? cg->GetBaseTypeOneLayer(param_ty)
                       : param_ty;
        if (cg->IsConstType(param_ty) != 0) {
          if (&mfn_decl == copy_ctor) {
            return true;
          }
        }
      }
    } else {
      //  if there's non-const version, use non-const
      //  otherwise, use the const one
      bool const_only = true;
      for (auto copy_ctor : copy_ctors) {
        void *param_name = cg->GetFuncParam(copy_ctor, 0);
        void *param_ty = cg->GetTypebyVar(param_name);
        param_ty = cg->IsReferenceType(param_ty) != 0
                       ? cg->GetBaseTypeOneLayer(param_ty)
                       : param_ty;
        if (cg->IsConstType(param_ty) == 0) {
          const_only = false;
          if (&mfn_decl == copy_ctor) {
            return true;
          }
        }
      }
      // TODO(youxiang): leave const_only here in case of any other cases
      if (const_only) {
        return true;
      }
    }
    return false;
  }

  void handle_overload_copy_ctors(void *ty_decl, const vector<void *> &ops,
                                  vector<void *> *init_names,
                                  vector<SgStatement *> *stmts,
                                  SgMemberFunctionDeclaration *mfn_decl) {
    if (!helpers::is_ctor(*mfn_decl)) {
      return;
    }
    const SgFunctionSymbol *sym = helpers::fn_symbol(mfn_decl);
    std::unordered_map<const SgFunctionSymbol *, RemappedFn>::iterator it =
        fn_remap.find(sym);
    if (it != fn_remap.end()) {
      if (mfn_decl->get_args().size() == 1) {
        //  Match all the copy ctors inside of this cls
        //  Find out which one is copy
        if (IsRightCopyCtor(ty_decl, init_names->back(), *mfn_decl)) {
          void *dot_exp = cg->CopyExp(ops[0]);
          for (size_t i = 0; i < init_names->size(); i++) {
            dot_exp = cg->CreateExp(V_SgDotExp, dot_exp, (*init_names)[i]);
          }
          vector<void *> args;
          void *addr_exp = cg->CreateExp(V_SgAddressOfOp, dot_exp);
          args.push_back(addr_exp);

          void *dot_exp_2 = cg->CopyExp(ops[1]);
          for (size_t i = 0; i < init_names->size(); i++) {
            dot_exp_2 = cg->CreateExp(V_SgDotExp, dot_exp_2, (*init_names)[i]);
          }
          args.push_back(dot_exp_2);
          void *args_list = cg->CreateExpList(args);
          stmts->push_back(static_cast<SgExprStatement *>(
              cg->CreateFuncCallStmt(it->second.decl(), args_list)));
        }
      }
    }
  }

  void handle_overload_assigns(void *ty_decl, const vector<void *> &ops,
                               vector<void *> *init_names,
                               vector<SgStatement *> *stmts,
                               SgMemberFunctionDeclaration *mfn_decl) {
    const SgFunctionSymbol *sym = helpers::fn_symbol(mfn_decl);
    string f_name = sym->get_name().getString();
    if (f_name != "operator=") {
      return;
    }
    std::unordered_map<const SgFunctionSymbol *, RemappedFn>::iterator it =
        fn_remap.find(sym);
    if (it != fn_remap.end()) {
      //  first operand
      void *dot_exp_1 = cg->CopyExp(ops[0]);
      for (size_t i = 0; i < init_names->size(); i++) {
        dot_exp_1 = cg->CreateExp(V_SgDotExp, dot_exp_1, (*init_names)[i]);
      }
      vector<void *> args;
      void *addr_exp = cg->CreateExp(V_SgAddressOfOp, dot_exp_1);
      args.push_back(addr_exp);

      //  second operand
      void *dot_exp_2 = cg->CopyExp(ops[1]);
      for (size_t i = 0; i < init_names->size(); i++) {
        dot_exp_2 = cg->CreateExp(V_SgDotExp, dot_exp_2, (*init_names)[i]);
      }
      void *second_exp = dot_exp_2;
      args.push_back(second_exp);

      void *args_list = cg->CreateExpList(args);
      stmts->push_back(static_cast<SgExprStatement *>(
          cg->CreateFuncCallStmt(it->second.decl(), args_list)));
    }
  }

  void handle_overload_ctors(void *ty_decl, const vector<void *> &ops,
                             vector<void *> *init_names,
                             vector<SgStatement *> *stmts,
                             SgMemberFunctionDeclaration *mfn_decl) {
    if (!helpers::is_ctor(*mfn_decl)) {
      return;
    }
    const SgFunctionSymbol *sym = helpers::fn_symbol(mfn_decl);
    std::unordered_map<const SgFunctionSymbol *, RemappedFn>::iterator it =
        fn_remap.find(sym);
    if (it != fn_remap.end()) {
      //  so this one is the default constructor
      //  create a big dot expr
      //  Open up for other ctors as well
      if (mfn_decl->get_args().empty()) {
        void *dot_exp = cg->CopyExp(ops[0]);
        for (size_t i = 0; i < init_names->size(); i++) {
          dot_exp = cg->CreateExp(V_SgDotExp, dot_exp, (*init_names)[i]);
        }
        vector<void *> args;
        void *addr_exp = cg->CreateExp(V_SgAddressOfOp, dot_exp);
        args.push_back(addr_exp);
        for (auto it = mfn_decl->get_args().begin();
             it != mfn_decl->get_args().end(); ++it) {
          void *arg_init_name = static_cast<void *>(*it);
          args.push_back(cg->CreateVariableRef(arg_init_name));
        }
        void *args_list = cg->CreateExpList(args);
        stmts->push_back(static_cast<SgExprStatement *>(
            cg->CreateFuncCallStmt(it->second.decl(), args_list)));
      }
    }
  }

  void traverse_all_overload_mfns(void *ty_decl, const vector<void *> &ops,
                                  vector<void *> *init_names,
                                  vector<SgStatement *> *stmts,
                                  Handler handler) {
    if (cg->IsClassDecl(ty_decl) == 0) {
      return;
    }
    //  check its children
    trav<SgInitializedName>(static_cast<SgNode *>(ty_decl),
                            [&](SgInitializedName *init_name) {
                              if (!cg->IsMemberVariable(init_name)) {
                                return;
                              }
                              SgType *var_type = init_name->get_type();
                              void *ty_decl = cg->GetTypeDeclByType(var_type);
                              //  FIXME: ctor, op=, copy ctor on pointer member
                              //  and simple array memeber create for loops,
                              //  inside for loop, create a temp variable, and
                              //  insert calls recursively, assign each member
                              //  to array elements.
                              init_names->push_back(init_name);
                              //  FIXME: support for array struct member
                              //  create a for loop, inside loop create a temp
                              //  variable, perform ctor, op=, copy ctor to that
                              //  temp var recusively assign each array element
                              //  to the temp var
                              traverse_all_overload_mfns(
                                  ty_decl, ops, init_names, stmts, handler);
                              init_names->pop_back();
                            },
                            postorder);

    //  check this cls/struct has been lowered or not, and if so whether the
    //  default ctor has been overwritten, and add the constructor if needed.
    trav<SgMemberFunctionDeclaration>(
        static_cast<SgNode *>(ty_decl),
        [&](SgMemberFunctionDeclaration *mfn_decl) {
          (this->*handler)(ty_decl, ops, init_names, stmts, mfn_decl);
        },
        postorder);
  }

  bool HasOverrideCopyCtor(void *ty_decl) {
    if (cg->IsClassDecl(ty_decl) == 0) {
      return false;
    }
    SgClassDeclaration *cls_decl =
        isa<SgClassDeclaration>(static_cast<SgNode *>(ty_decl));
    if ((cls_decl == nullptr) || !helpers::should_lower(cls_decl, cg)) {
      return false;
    }
    SgType *cls_ty = cls_decl->get_type();
    vector<void *> members;
    cg->GetClassMembers(cls_ty, &members);
    for (auto &member : members) {
      if (!cg->IsMemberFunction(member)) {
        continue;
      }
      //  Get name
      SgMemberFunctionDeclaration *mem_fn =
          static_cast<SgMemberFunctionDeclaration *>(member);
      if (!helpers::is_ctor(*mem_fn)) {
        continue;
      }
      //  Get num args
      if (cg->GetFuncParamNum(mem_fn) == 1) {
        return true;
      }
    }
    return false;
  }

  void traverse_all_cls_cp_members(void *ty_decl, const vector<void *> &ops,
                                   vector<void *> *init_names,
                                   vector<SgStatement *> *stmts) {
    if (cg->IsClassDecl(ty_decl) == 0) {
      return;
    }
    vector<SgInitializedName *> copy_vars;
    vector<void *> members;
    SgClassDeclaration *cls_decl =
        isa<SgClassDeclaration>(static_cast<SgNode *>(ty_decl));
    if ((cls_decl == nullptr) || !helpers::should_lower(cls_decl, cg)) {
      return;
    }
    SgType *cls_ty = cls_decl->get_type();
    cg->GetClassMembers(cls_ty, &members);
    for (auto &member : members) {
      if (cg->IsVariableDecl(member) == 0) {
        continue;
      }
      SgInitializedName *init_name = static_cast<SgInitializedName *>(
          cg->GetVariableInitializedName(member));
      //  potentially reduant of IsVariableDecl
      if (!cg->IsMemberVariable(init_name)) {
        continue;
      }
      SgType *var_type = init_name->get_type();
      void *ty_decl = cg->GetTypeDeclByType(var_type);
      //  if (HasOverrideCopyCtor(ty_decl)) {
      if (cg->IsCompoundType(var_type) != 0) {
        init_names->push_back(init_name);
        traverse_all_cls_cp_members(ty_decl, ops, init_names, stmts);
        init_names->pop_back();
      } else if (cg->IsArrayType(var_type)) {
        void *basic_type;
        vector<size_t> dims;
        cg->get_type_dimension(var_type, &basic_type, &dims);
        auto *array = cg->CreateExp(V_SgDotExp, cg->CopyExp(ops[0]),
                                    cg->CreateVariableRef(init_name));
        auto *from_array = cg->CreateExp(V_SgDotExp, cg->CopyExp(ops[1]),
                                         cg->CreateVariableRef(init_name));
        void *block = cg->reset_array_from_array(array, basic_type, dims,
                                                 from_array, nullptr);
        stmts->push_back(static_cast<SgStatement *>(block));
      } else {
        copy_vars.push_back(init_name);
      }
    }
    //  copy vars
    for (auto &var : copy_vars) {
      void *dot_exp_l = cg->CopyExp(ops[0]);
      for (size_t i = 0; i < init_names->size(); i++) {
        dot_exp_l = cg->CreateExp(V_SgDotExp, dot_exp_l, (*init_names)[i]);
      }
      dot_exp_l = cg->CreateExp(V_SgDotExp, dot_exp_l, var);

      void *dot_exp_r = cg->CopyExp(ops[1]);
      for (size_t i = 0; i < init_names->size(); i++) {
        dot_exp_r = cg->CreateExp(V_SgDotExp, dot_exp_r, (*init_names)[i]);
      }
      dot_exp_r = cg->CreateExp(V_SgDotExp, dot_exp_r, var);
      void *assign_exp = cg->CreateExp(V_SgAssignOp, dot_exp_l, dot_exp_r);
      stmts->push_back(static_cast<SgStatement *>(
          cg->CreateStmt(V_SgExprStatement, assign_exp)));
    }
  }

  bool idle_ctor(SgMemberFunctionDeclaration *ctor) {
    void *cls_decl = cg->GetTypeDeclByMemberFunction(ctor);
    if (auto def = cg->GetFuncDefinitionByDecl(ctor)) {
      if (cg->GetChildStmtNum(def) > 0) {
        return false;
      }
      auto decl = static_cast<SgMemberFunctionDeclaration *>(
          cg->GetFuncDeclByDefinition(def));
      if (decl->get_CtorInitializerList()->get_ctors().size() > 0) {
        return false;
      }
    }
    for (auto base_decl : cg->GetInheritances(cls_decl)) {
      if (!idle_ctor(static_cast<SgMemberFunctionDeclaration *>(
              cg->GetDefaultConstructor(base_decl))))
        return false;
    }
    vector<void *> members;
    cg->GetClassNonStaticDataMembers(cg->GetTypeDeclByMemberFunction(ctor),
                                     &members);
    for (auto mbr : members) {
      auto type = static_cast<SgType *>(
          cg->GetTypebyVar(cg->GetVariableInitializedName(mbr)));
      while (cg->IsArrayType(type)) {
        type = static_cast<SgType *>(cg->GetElementType(type));
      }
      auto curr_ctor = static_cast<SgMemberFunctionDeclaration *>(
          cg->GetDefaultConstructor(type));
      if (!idle_ctor(curr_ctor)) {
        return false;
      }
    }
    return true;
  }

  //  This function does the actual lowering of ctor calls. It transforms:
  //
  //     C c(args...);
  //        ^^^^^^^^^-- SgConstructorInitializer &ctorcall.
  //     C(args...);
  //
  //  into:
  //
  //     C_mangled c;
  //     ctor_mangled(&c, args...);
  //                  ^^-- SgExpression &lhs.
  //     C_mangled tmp;
  //     ctor_mangled(&tmp, args...);
  //
  //  An SgExprStatement is returned because in all cases, a temporary object
  //  must be initialized.
  vector<SgStatement *> lower_ctor_call(SgExpression *lhs,
                                        SgConstructorInitializer *ctorcall) {
    SgClassDeclaration *sg_cls = ctorcall->get_class_decl();
    vector<SgStatement *> init_stmts;
    if ((sg_cls == nullptr) || !helpers::should_lower(sg_cls, cg)) {
      return init_stmts;
    }
    //  Copy the ctor call args.
    auto *ctorargs =
        static_cast<SgExprListExp *>(cg->CopyExp(ctorcall->get_args()));
    std::unordered_map<const SgFunctionSymbol *, RemappedFn>::iterator it;
    if ((ctorcall->get_declaration() == nullptr) ||
        (it = fn_remap.find(helpers::fn_symbol(ctorcall->get_declaration()))) ==
            fn_remap.end()) {
      //  Could not find a user-declared/-defined ctor. Check if it's the
      //  implicit default ctor. If so, don't try to lower.
      //
      //  Min Gao, MER717 it's true no need to lower default ctor of current
      //  level. However, if its member is a lowered class type with overrided
      //  ctor, its member ctor needs to be called explicitly.
      if (ctorargs->get_expressions().empty()) {
        //  return nullptr;
        //
        //  Get all member type, and check whether they are on the cls_remap.
        //  If so we check whether the default ctor has been overwritten on
        //  the fn_remap. A search hit indicates we need to insert that call
        //  as well.
        //  auto *init_name_top = (SgInitializedName
        //  *)cg->TraceUpToInitializedName(ctorcall); SgType *var_type_top =
        //  init_name_top->get_type();
        void *var_type_top = cg->GetTypeByExp(lhs);
        void *ty_decl =
            cg->GetTypeDeclByType(cg->GetBaseType(var_type_top, true));

        vector<void *> ops;
        vector<void *> init_names;
        ops.push_back(lhs);

        Handler handler = &LowerCpp::handle_overload_ctors;
        traverse_all_overload_mfns(ty_decl, ops, &init_names, &init_stmts,
                                   handler);

        return init_stmts;
      }

      DEBUG(cerr << "trying copy ctor\n");
      //  Alternatively, could be a call to implicitly defined copy ctor.
      if (ctorargs->get_expressions().size() == 1) {
        void *var_type_top = cg->GetTypeByExp(lhs);
        void *ty_decl =
            cg->GetTypeDeclByType(cg->GetBaseType(var_type_top, true));

        vector<void *> ops;
        vector<void *> init_names;
        ops.push_back(lhs);
        auto arg_exprs = ctorargs->get_expressions();
        for (auto it = arg_exprs.begin(); it != arg_exprs.end(); ++it) {
          ops.push_back(cg->CopyExp(*it));
        }

        //  copy over variables
        traverse_all_cls_cp_members(ty_decl, ops, &init_names, &init_stmts);

        //  copy constructors
        Handler handler = &LowerCpp::handle_overload_copy_ctors;
        traverse_all_overload_mfns(ty_decl, ops, &init_names, &init_stmts,
                                   handler);
        if (init_stmts.empty()) {
          // TODO(youxiang): In order for control to arrive here, ctorcall had
          // to
          //  have been an implicit call of some sort. So it sort of seems
          //  very likely that ctorcall is an implicit copy ctor or copy
          //  assign (they both lower to the same thing), but this may not
          //  always be true, in which case we will need to add further
          //  checks besides the arity of the calli.
          SgType *argty = ctorargs->get_expressions().front()->get_type();
          if (SgType *clsty = ctorcall->get_type()) {
            if (argty->findBaseType() == clsty->findBaseType()) {
              //  Lower `lhs(copied)` to `lhs = copied`.
              auto *assign = static_cast<SgAssignOp *>(cg->CreateExp(
                  V_SgAssignOp, lhs,
                  cg->CopyExp(ctorargs->get_expressions().front()),
                  /* unary_mode_ = */ 0));
              assert(assign && "Failed to create the assign op.");
              //  return static_cast<SgExprStatement *>(
              //     cg->CreateStmt(V_SgExprStatement, assign));
              init_stmts.push_back(static_cast<SgStatement *>(
                  cg->CreateStmt(V_SgExprStatement, assign)));
            }
          }
        }
        return init_stmts;
      }

      DEBUG(cerr << "fail to lower " << cg->_p(ctorcall->get_declaration())
                 << '\n');
      assert(false &&
             "Expected the ctor's decl to have been lowered before call.");
    }
    ctorargs->prepend_expression(
        static_cast<SgAddressOfOp *>(cg->CreateExp(V_SgAddressOfOp, lhs)));
    if (!idle_ctor(ctorcall->get_declaration())) {
      init_stmts.push_back(static_cast<SgStatement *>(
          cg->CreateFuncCallStmt(it->second.decl(), ctorargs)));
    }
    return init_stmts;
  }

  void lower_ctor_call(SgConstructorInitializer *ctorcall) {
    //  Possible occurrences of SgConstructorInitializers AST nodes:
    //
    //  - SgVariableDeclaration(SgInitializedName("some name",
    //   SgConstructorInitializer))
    //
    //  - Anonymously constructed object, such as:
    //
    //   struct S { unsigned u; };
    //   S(arglist).u;
    //   SgExprStatement(
    //      SgDotExp(SgConstructorInitializer(arglist), SgVarRefExp("u")))
    DEBUG(cerr << "lower_ctor_call on " << cg->_p(ctorcall->get_parent(), 80)
               << " type " << cg->_p(ctorcall->get_type(), 80) << "\n");

    if (SgClassDeclaration *decl = ctorcall->get_class_decl()) {
      if (helpers::reserved_class(decl)) {
        DEBUG(cerr << "\tignoring because class is reserved: "
                   << decl->get_name() << "\n");
        return;
      }
      if (!helpers::should_lower(decl, cg)) {
        DEBUG(cerr << "\tignoring because class is reserved: "
                   << decl->get_name() << "\n");
        return;
      }
    }

    //  HACK: If ctorcall calls a non-defining ctor decl, (e.g., `struct S {
    //  S(); }`), then ctorcall->get_type() will return `void`, which is
    //  obviously not what we want. On the other hand, ctorcall's could be
    //  some primitive type (e.g., `return int()`), in which case
    //  get_class_decl would return nullptr.
    auto *cls_decl = ctorcall->get_class_decl();
    SgType *ty =
        cls_decl != nullptr ? cls_decl->get_type() : ctorcall->get_type();
    bool ty_remapped = false;
    //  If it's a user-defined ctor, create a variable of the lowered type.
    if (auto *clsty = isa<SgClassType>(ty)) {
      if (auto *decl =
              isa_or_null<SgClassDeclaration>(clsty->get_declaration())) {
        if (SgClassDeclaration *remapped = get_remapped_type(decl)) {
          ty = SgClassType::createType(remapped);
          ty_remapped = true;
        }
      }
    }
    if (!ty_remapped) {
      if (cls_decl != nullptr) {
        auto *tydef = isa<SgTypedefDeclaration>(cls_decl->get_parent());
        if (tydef != nullptr)
          ty = tydef->get_type();
      }
    }

    if (auto *iname = isa<SgInitializedName>(ctorcall->get_parent())) {
      //  ctorcall constructs some named object.
      if (auto *var = isa<SgVariableDeclaration>(iname->get_declaration())) {
        SgScopeStatement *sc = var->get_scope();

        //  Rename references to the old variable (`var`) and the old
        //  decl to some temporary name. This ensures that the
        //  SgVarRefExp created below references the new var decl.
        SgName oldname = iname->get_name();
        SageInterface::set_name(iname, "__tmp_" + oldname);
        auto *newdecl = static_cast<SgVariableDeclaration *>(
            cg->CreateVariableDecl(ty, oldname, nullptr, sc));
        cg->InsertStmt(newdecl, var);
        void *static_flag = nullptr;
        if (cg->IsStatic(var)) {
          cg->SetStatic(newdecl);
          void *func_decl = cg->TraceUpToFuncDecl(sc);
          string static_flag_var = "merlin_static_flag_" + oldname;
          cg->get_valid_local_name(func_decl, &static_flag_var);
          static_flag =
              static_cast<SgVariableDeclaration *>(cg->CreateVariableDecl(
                  "int", static_flag_var, cg->CreateConst(0), sc));
          cg->InsertStmt(static_flag, var);
          cg->SetStatic(static_flag);
        }
        auto *varref =
            static_cast<SgVarRefExp *>(cg->CreateVariableRef(newdecl, sc));

        DEBUG(cerr << "\tconverted to a named: " << newdecl << "\n");
        vector<SgStatement *> initcalls = lower_ctor_call(varref, ctorcall);
        if (!initcalls.empty()) {
          //  potential bug on this branch no initcall?
          if (cg->IsStatic(var)) {
            void *init_bb = cg->CreateBasicBlock();
            void *set_flag = cg->CreateStmt(V_SgAssignStatement,
                                            cg->CreateVariableRef(static_flag),
                                            cg->CreateConst(1));
            cg->AppendChild(init_bb, set_flag);
            for (auto &initcall : initcalls) {
              cg->AppendChild(init_bb, initcall);
            }
            void *if_stmt = cg->CreateIfStmt(
                cg->CreateExp(V_SgNotOp, cg->CreateVariableRef(static_flag)),
                init_bb, nullptr, var);
            cg->InsertStmt(if_stmt, var);

          } else {
            for (auto &initcall : initcalls) {
              cg->InsertStmt(initcall, var);
            }
          }
          DEBUG(for (auto initcall
                     : initcalls) cerr
                << "\tinit:" << initcall << "\n");
        }

        //  Re-create existing references to the old var.
        auto *oldsym =
            isa<SgVariableSymbol>(iname->search_for_symbol_from_symbol_table());
        assert(oldsym);
        std::vector<SgVarRefExp *> oldrefs;
        cg->TraversePre(sc, [&](SgNode *n) {
          if (auto *oldref = isa<SgVarRefExp>(n)) {
            if (oldref->get_symbol() == oldsym) {
              oldrefs.push_back(oldref);
            }
          }
        });
        for (SgVarRefExp *oldref : oldrefs) {
          cg->ReplaceExp(oldref, cg->CreateVariableRef(newdecl, sc));
        }

        delete_later(var);
      } else {
        DEBUG(cerr << "\tFound a ctor call on unknown decl: "
                   << cg->_p(iname->get_declaration(), 80) << "\n");
      }
    } else {
      //  If there are multiple anon ctors in the same tree, evaluation
      //  order is up to us.
      SgStatement *owningstmt = nullptr;
      SgFunctionDefinition *owningfn = nullptr;
      for (SgNode *s = ctorcall->get_parent(); isa<SgGlobal>(s) == nullptr;
           s = s->get_parent()) {
        if (auto *fndef = isa<SgFunctionDefinition>(s)) {
          owningfn = fndef;
          break;
        }
      }
      for (SgNode *s = ctorcall->get_parent(); isa<SgGlobal>(s) == nullptr;
           s = s->get_parent()) {
        if (auto *stmt = isa<SgStatement>(s)) {
          owningstmt = stmt;
          break;
        }
      }
      assert(owningfn &&
             "No support for anonymous objects in non-function scopes.");
      assert(owningstmt && "Naked sgExpression in the AST.");
      string objname = "temp";
      assert(isa<SgFunctionDeclaration>(owningfn->get_declaration()));
      cg->get_valid_local_name(owningfn->get_declaration(), &objname);
      auto *tmpobj = static_cast<SgVariableDeclaration *>(
          cg->CreateVariableDecl(ty, objname, nullptr, nullptr));
      cg->InsertStmt(tmpobj, owningstmt);
      auto *tmpref =
          static_cast<SgVarRefExp *>(cg->CreateVariableRef(tmpobj, owningfn));
      DEBUG(cerr << "\tconverted to an anon: " << tmpobj << "\n");
      vector<SgStatement *> initcalls = lower_ctor_call(tmpref, ctorcall);
      DEBUG(for (auto &initcall
                 : initcalls) { cerr << "\tinit:" << initcall << "\n"; });
      int is_empty_aggregate = 0;
      void *sg_init = cg->TraceUpToInitializedName(ctorcall);
      void *initer = nullptr;
      if (sg_init != nullptr) {
        initer = cg->GetInitializer(sg_init);
        if (initer != nullptr) {
          if (cg->UnparseToString(initer).empty()) {
            is_empty_aggregate = 1;
            cg->SetInitializer(sg_init, nullptr);
            if (initcalls.empty()) {
              delete_later(tmpobj);
              return;
            }
          }
        }
      }
      if (is_empty_aggregate == 1 && ctorcall->get_declaration() &&
          !idle_ctor(ctorcall->get_declaration())) {
        void *sg_init_type = cg->GetTypeByExp(initer);
        void *basic_type;
        vector<size_t> nSizes;
        cg->get_type_dimension(sg_init_type, &basic_type, &nSizes, sg_init);
        void *init_stmt = cg->create_for_loop_for_reset_array(
            sg_init, basic_type, nSizes, tmpobj, sg_init);
        void *var_decl = cg->GetVariableDecl(sg_init);
        if (cg->IsStatic(var_decl)) {
          void *func_decl = cg->TraceUpToFuncDecl(var_decl);
          string var_name = cg->GetVariableName(sg_init);
          string static_flag_var = "merlin_static_flag_" + var_name;
          cg->get_valid_local_name(func_decl, &static_flag_var);
          void *static_flag = cg->CreateVariableDecl(
              "int", static_flag_var, cg->CreateConst(0), var_decl);
          cg->InsertStmt(static_flag, var_decl);
          cg->SetStatic(static_flag);
          void *init_bb = cg->CreateBasicBlock();
          void *set_flag = cg->CreateStmt(V_SgAssignStatement,
                                          cg->CreateVariableRef(static_flag),
                                          cg->CreateConst(1));
          cg->AppendChild(init_bb, set_flag);
          for (auto &initcall : initcalls) {
            cg->AppendChild(init_bb, initcall);
          }
          cg->AppendChild(init_bb, init_stmt);
          void *if_stmt = cg->CreateIfStmt(
              cg->CreateExp(V_SgNotOp, cg->CreateVariableRef(static_flag)),
              init_bb, nullptr, var_decl);
          cg->InsertAfterStmt(if_stmt, var_decl);

        } else {
          for (auto &initcall : initcalls) {
            cg->InsertStmt(initcall, owningstmt);
          }
          cg->InsertAfterStmt(init_stmt, var_decl);
        }
      } else {
        for (auto &initcall : initcalls) {
          cg->InsertStmt(initcall, owningstmt);
        }
        void *sg_var_ref = cg->CreateVariableRef(tmpobj);
        void *sg_new_init = SageBuilder::buildAssignInitializer(
            isSgVarRefExp(static_cast<SgNode *>(sg_var_ref)));
        cg->ReplaceExp(ctorcall, sg_new_init);
      }
    }
  }

  //  Lower the initializer list of a constructor definition. For instance,
  //
  //  struct A { T t; A (T t, ...args) : t(t) { body; } };
  //  //                                 ^^^^-- initializer list.
  //
  //  would be lowered into
  //
  //  void _ZN1AC2E(struct A *this_, T t, ...args) {
  //     //  member initialization
  //     this_->t = t;
  //
  //     body;
  //  }
  //
  //  We assume that `body` already exists in def (since def was most likely
  //  created by cloning), so our job here is initialize members by translating
  //  the SgCtorInitializerList into a series of assignments. Note: The C++
  //  standard requires us to initialize member variables in the exact order
  //  that they were declared in the aggregate definition (agg decl order):
  //  http://  eel.is/c++draft/class.base.init#13.3
  void lower_init_list(SgCtorInitializerList *ctorlist,
                       SgClassDefinition *oldcls, SgFunctionDefinition *def) {
    SgClassDeclaration *cls = oldcls->get_declaration();
    if ((cls == nullptr) || !helpers::should_lower(cls, cg)) {
      return;
    }
    DEBUG(cerr << "inside lower_init_list:\n\tcls = " << oldcls
               << "\n\tdef = " << def << "\n");

    //  Ordered seq of `this_->t = val`. string is t, SgInitializer is val.
    list<pair<SgInitializedName *, SgInitializer *>> inits;
    unordered_map<string, SgInitializer **> initmap;

    //  Figure out memvar init order and also account for field initializers.
    for (SgDeclarationStatement *memdecl : oldcls->get_members()) {
      if (auto *vardecl = isa<SgVariableDeclaration>(memdecl)) {
        if (cg->IsStatic(vardecl)) {
          continue;
        }
        for (SgInitializedName *iname : vardecl->get_variables()) {
          SgName n = iname->get_name();
          DEBUG(cerr << "\tpushing memvar " << n.getString()
                     << " with init val " << iname->get_initializer() << "\n");
          // As of C++03, data members can only be initialized from initializer-
          // list, so here iname->get_initializer() returns nullptr.
          inits.emplace_back(iname, iname->get_initializer());
          initmap.insert({n.getString(), &inits.back().second});
        }
      }
    }

    DEBUG(cerr << "\tctorlist " << ctorlist << " has "
               << ctorlist->get_ctors().size() << " inits.\n");
    //  Now account for ctor init list. Remember, it could be out of order.
    for (SgInitializedName *iname : ctorlist->get_ctors()) {
      DEBUG(cerr << "\tctor init-list: " << iname->get_name().getString()
                 << " <= " << iname->get_initializer() << "\n");
      string id{iname->get_name().getString()};
      auto it = initmap.find(id);
      if (it == initmap.end()) {
        assert(it != initmap.end() && "Implication: Ctor init list "
                                      "initialized a non-member variable.");
      }
      assert(iname->get_initializer() && "nullptr ctor init??");
      // For any explicit initialization from initializer-list, here
      // iname->get_initializer() returns not null.
      *it->second = iname->get_initializer();
    }

    //  Retrieve a handle to the `this` var.
    SgInitializedName *this_ = def->get_declaration()->get_args().front();

    // Now create assign stmts to initialize data members including
    // the ones representing base class object for derived class.
    for (auto initit = inits.rbegin(); initit != inits.rend(); ++initit) {
      //  It's possible that some members weren't initialized by field- or
      //  ctor-initializers.
      DEBUG(cerr << "\tmemvar " << initit->first->get_name() << "\n");
      auto type = static_cast<SgType *>(cg->GetTypebyVar(initit->first));
      if (initit->second == nullptr) {
        // which means no explicit initialization at all, but for structure or
        // class type, default constructor is the last resort.
        if (cg->IsStructureType(type) || cg->IsClassType(type)) {
          auto ctor = static_cast<SgMemberFunctionDeclaration *>(
              cg->GetDefaultConstructor(type));
          if (!idle_ctor(ctor)) {
            auto args = SageBuilder::buildExprListExp(vector<SgExpression *>());
            initit->second = SageBuilder::buildConstructorInitializer(
                ctor, args, type, false, false, false, false);
          }
        } else if (cg->IsArrayType(type)) {
          auto etype = static_cast<SgType *>(cg->GetElementType(type));
          while (cg->IsArrayType(etype)) {
            etype = static_cast<SgType *>(cg->GetElementType(etype));
          }
          if (cg->IsStructureType(etype) || cg->IsClassType(etype)) {
            SgConstructorInitializer *ci = nullptr;
            {
              auto ctor = static_cast<SgMemberFunctionDeclaration *>(
                  cg->GetDefaultConstructor(etype));
              if (!idle_ctor(ctor)) {
                vector<SgExpression *> v;
                auto args = SageBuilder::buildExprListExp(v);
                ci = SageBuilder::buildConstructorInitializer(
                    ctor, args, etype, false, false, false, false);
              }
            }
            if (ci != nullptr) {
              vector<SgExpression *> v;
              v.push_back(ci);
              auto args = SageBuilder::buildExprListExp(v);
              auto ai = SageBuilder::buildAggregateInitializer(args, type);
              initit->second = ai;
            }
          }
        }
      }
      if (initit->second == nullptr) {
        continue;
      }
      // TODO(youxiang): Teach CSageCodeGen::CreateExp to build this.
      SgArrowExp *lhs = SageBuilder::buildArrowExp(
          static_cast<SgVarRefExp *>(cg->CreateVariableRef(this_, def)),
          static_cast<SgVarRefExp *>(cg->CreateVariableRef(
              initit->first,
              get_remapped_type(oldcls->get_declaration())->get_definition())));
      if (auto *ccall = isa<SgConstructorInitializer>(initit->second)) {
        vector<SgStatement *> cs = lower_ctor_call(lhs, ccall);
        SgStatement *c = cs.empty() ? NULL : cs.back();
        if (c != NULL) {
          cg->PrependChild(def->get_body(), c);
        }
      } else if (auto ai = isa<SgAggregateInitializer>(initit->second)) {
        void *basic_type;
        vector<size_t> dims;
        cg->get_type_dimension(type, &basic_type, &dims);
        auto init = ai->get_initializers()->get_expressions()[0];
        SgVariableDeclaration *tmpobj;
        {
          tmpobj = static_cast<SgVariableDeclaration *>(cg->CreateVariableDecl(
              basic_type, "temp", init, def->get_body()));
        }
        auto loop = cg->create_for_loop_for_reset_array(lhs, basic_type, dims,
                                                        tmpobj, initit->first);
        cg->PrependChild(def->get_body(), loop);
        cg->PrependChild(def->get_body(), tmpobj);
      } else {
        DEBUG(cerr << "\tinitval " << cg->_p(initit->second, 80) << "\n");
        assert(isa<SgExpression>(initit->second));
        auto *rhs = static_cast<SgInitializer *>(
            cg->CopyExp(cg->GetInitializerOperand(initit->second)));
        cg->PrependChild(def->get_body(),
                         cg->CreateStmt(V_SgAssignStatement, lhs, rhs));
      }
    }
  }

  //  Lowers the SgMemberFunctionDeclaration (and its definition, if it exists)
  //  into an SgFunctionDeclaration. A record of the mapping from the old
  //  member function's symbol to the new function is added to `fn_remap`. Note
  //  that constructors are lowered here as well, since they manifest as
  //  SgMemberFunctionDeclarations.
  void lower_memfn_decl(SgMemberFunctionDeclaration *decl) {
    if (isa<SgTemplateMemberFunctionDeclaration>(decl) != nullptr) {
      return;
    }
    DEBUG(cerr << "lower_memfn_decl " << cg->_p(decl, 80) << "\n");
    //  Figure out the right place to insert the lowered decl->
    SgStatement *inspt = decl;
    SgClassDeclaration *clsdecl =
        isa<SgClassDeclaration>(decl->get_associatedClassDeclaration());

    if ((clsdecl == nullptr) || !helpers::should_lower(clsdecl, cg)) {
      return;
    }
    SgClassDeclaration *remapped_decl =
        get_remapped_type(helpers::cls_symbol(clsdecl));
    if (remapped_decl == nullptr) {
      DEBUG(cerr << "\tignoring because class wasn't lowered: "
                 << clsdecl->get_name() << "\n");
      return;
    }

    if (duplicated_function_definition(decl)) {
      return;
    }
    if (decl->isDefinedInClass() || (decl->get_definition() == nullptr)) {
      if (auto *tydef = isa<SgTypedefDeclaration>(clsdecl->get_parent())) {
        inspt = tydef;
      } else {
        inspt = clsdecl;
      }
    }

    //  Compute the name of the lowered fn decl->
    std::stringstream newname;
    helpers::Mangler(newname, cg).mangle(decl);
    auto *global = static_cast<SgScopeStatement *>(cg->GetGlobal(decl));

    unordered_map<string, string> token_map;
    //  Generate the cloned function declaration.
    SgFunctionDeclaration *newdecl = nullptr;
    if (is_static_memfn(decl)) {
      DEBUG(cerr << "it's a static\n");
      newdecl = static_cast<SgFunctionDeclaration *>(cg->CloneFuncDecl(
          decl, global, newname.str(), decl->get_definition() != nullptr));
      //  The lowered function should not be static, because `static` on
      //  global scope fn decls has a totally different meaning.
      cg->UnSetStatic(newdecl);
    } else {
      DEBUG(cerr << "it's a " << (helpers::is_ctor(*decl) ? "ctor" : "memfn")
                 << "\n");
      //  We're lowering a non-static member fn decl, which means that an
      //  additional argument needs to be inserted into the clone's
      //  parameter list.

      //  Build SgInitializedName for the `this` parameter.
      SgType *basety = clone_cls_ty(decl);
      if (isa<SgMemberFunctionType>(decl->get_type())->isConstFunc()) {
        basety = SageBuilder::buildConstType(basety);
      }
      if (isa<SgMemberFunctionType>(decl->get_type())->isVolatileFunc()) {
        basety = SageBuilder::buildVolatileType(basety);
      }
      SgPointerType *selfty = SageBuilder::buildPointerType(basety);

      //  Figure out variable name.
      auto thisit = this_map.find(newname.str());
      SgInitializedName *thisparam = nullptr;
      if (thisit != this_map.end()) {
        thisparam = static_cast<SgInitializedName *>(
            cg->CreateVariable(selfty, thisit->second));
      } else {
        //  Generate a new `this` replacement.
        string this_ = "this_";
        cg->get_valid_local_name(decl, &this_);
        this_map.insert({newname.str(), this_});
        thisparam =
            static_cast<SgInitializedName *>(cg->CreateVariable(selfty, this_));
      }

      //  Clone the parameter list and insert `this` decl into the start.
      auto pair = cg->cloneFuncParams(*decl);
      vector<SgInitializedName *> &params = pair.first;
      params.insert(params.begin(), thisparam);

      if (decl->get_definition() != nullptr) {
        newdecl =
            cg->cloneFuncWith(decl, global, decl->get_type()->get_return_type(),
                              newname.str(), params, pair.second);
      } else {
        newdecl = static_cast<SgFunctionDeclaration *>(cg->CreateFuncDecl(
            decl->get_type()->get_return_type(), newname.str(),
            {params.begin(), params.end()}, global, false, nullptr));
      }
      vector<void *> orig_vec_mems;
      cg->GetClassNonStaticDataMembers(clsdecl, &orig_vec_mems);
      vector<void *> new_vec_mems;
      cg->GetClassNonStaticDataMembers(remapped_decl->get_type(),
                                       &new_vec_mems);
      if (clsdecl->get_definition()->get_inheritances().size() == 0) {
        assert(orig_vec_mems.size() == new_vec_mems.size());
      }
      string this_access = thisparam->get_name() + "->";
      for (size_t i = 0; i < orig_vec_mems.size(); ++i) {
        token_map[cg->GetVariableName(orig_vec_mems[i])] =
            this_access + cg->GetVariableName(new_vec_mems[i]);
      }
    }

    if (helpers::is_ctor(*decl)) {
      if (SgCtorInitializerList *ctorlist = decl->get_CtorInitializerList()) {
        if (SgFunctionDefinition *newdef = newdecl->get_definition()) {
          //  We'll need to translate the initializer list into
          //  initialization statements in the lowered ctor.
          lower_init_list(ctorlist,
                          isa<SgClassDefinition>(decl->get_class_scope()),
                          newdef);
        }
      }
    }

    //  Insert the new declaration/definition before old one.
    assert(newdecl && "Expected decl to have been cloned!");
    cg->InsertStmt(newdecl, helpers::get_insert_position(inspt, cg));

    //  If this is a defining declaration, then record the correspondence
    //  between the old mem fn's symbol to our new definition body.
    if (SgFunctionDefinition *newdef = newdecl->get_definition()) {
      DEBUG(cerr << "got body: " << cg->_p(newdef, 80) << "\n");
      add_fn_remap(*helpers::fn_symbol(decl), newdef, cg->IsStatic(decl));
    } else {
      add_decl_remap(*helpers::fn_symbol(decl), newdecl, cg->IsStatic(decl));
    }
    if (auto tcinst = isa<SgTemplateInstantiationDecl>(clsdecl)) {
      get_token_remap_in_template_class(*tcinst, &token_map);
    }
    if (auto tmfinst = isa<SgTemplateInstantiationMemberFunctionDecl>(decl)) {
      get_token_remap_in_template_member_function(*tmfinst, &token_map);
    }
    cg->ReplacePragmaTokens(newdecl, token_map);
    delete_later(decl);
  }

  bool is_up_cast(void *exp, void *cty) {
    void *ety = cg->GetTypeByExp(exp);
    if (cg->IsPointerType(cty)) {
      cty = cg->GetBaseTypeOneLayer(cty);
    }
    if (cg->IsPointerType(ety)) {
      ety = cg->GetBaseTypeOneLayer(ety);
    }
    if ((cg->IsClassType(ety) || cg->IsStructureType(ety)) &&
        (cg->IsClassType(cty) || cg->IsStructureType(cty))) {
      auto ra = cls_topo.equal_range(ety);
      for (auto it = ra.first; it != ra.second; ++it) {
        if (it->second == cty) {
          return true;
        }
      }
    }
    return false;
  }

  void *make_ref_to_base_for(void *exp, void *cty) {
    if (cg->IsPointerType(cty)) {
      cty = cg->GetBaseTypeOneLayer(cty);
    }

    void *ety = cg->GetTypeByExp(exp);
    void *base_ety = ety;
    if (cg->IsPointerType(ety)) {
      base_ety = cg->GetBaseTypeOneLayer(ety);
    }
    auto cty_cls =
        static_cast<SgClassDeclaration *>(cg->GetTypeDeclByType(cty));
    auto ety_cls =
        static_cast<SgClassDeclaration *>(cg->GetTypeDeclByType(base_ety));
    void *iname = cg->GetClassMemberByName(
        cg->GetClassName(cty_cls), get_remapped_type(ety_cls)->get_type());
    void *to_base =
        cg->CreateExp(cg->IsPointerType(ety) ? V_SgArrowExp : V_SgDotExp,
                      cg->CopyExp(exp), cg->CreateVariableRef(iname));
    return to_base;
  }

  // ((base*)a)->f => (&a->base)->f
  // ((base)a).f => a.base.f
  void trans_cast_to_base(void *_mref) {
    bool make_pntr = false;
    _trans_cast_to_base(_mref, &make_pntr);
  }
  void _trans_cast_to_base(void *_mref, bool *make_pntr) {
    auto mref = static_cast<SgNode *>(_mref);
    if (auto cast = isSgCastExp(mref)) {
      void *op1 = cg->GetExpUniOperand(cast);
      void *cty = cg->GetTypeByExp(cast);
      _trans_cast_to_base(op1, make_pntr);
      op1 = cg->GetExpUniOperand(cast);
      if (is_up_cast(op1, cty)) {
        void *new_mref = make_ref_to_base_for(op1, cty);
        if (*make_pntr == true) {
          new_mref = cg->CreateExp(V_SgAddressOfOp, new_mref);
        }
        cg->ReplaceExp(cast, new_mref);
      }
    } else {
      if (cg->IsPointerType(cg->GetTypeByExp(_mref))) {
        *make_pntr = true;
      }
    }
  }

  //  Lowers member function calls. All such calls will comprise of an
  //  SgMemberFunctionRefExp contained within an AST sub-tree rooted by an
  //  SgFunctionCallExp.
  void lower_memfn_call(SgMemberFunctionRefExp *mfnref) {
    if (cg->is_floating_node(mfnref) != 0) {
      return;
    }
    DEBUG(cerr << "lower_memfn_call: " << mfnref << " "
               << cg->get_location(mfnref, true, true) << "\n");

    //  If mfnref references a mem fn of an unlowered class -- for instance,
    //  calls to ap_int::range -- we simply don't lower the call.
    SgClassDeclaration *cls =
        isa<SgClassDeclaration>(mfnref->getAssociatedMemberFunctionDeclaration()
                                    ->get_associatedClassDeclaration());
    if (!helpers::should_lower(cls, cg)) {
      return;
    }
    if (get_remapped_type(helpers::cls_symbol(cls)) == nullptr) {
      //  Personally I dont like this style to get =
      //  GetFuncNameByRef returns "" on assignment
      string f_name = mfnref->get_symbol()->get_name().getString();
      if (f_name == "operator=") {
        vector<void *> ops;
        //  get init_names of both oprands
        auto *fn_call =
            static_cast<SgFunctionCallExp *>(cg->TraceUpToFuncCall(mfnref));
        auto *fn = fn_call->get_function();
        void *op_l = cg->GetExpLeftOperand(fn);
        ops.push_back(op_l);
        auto fn_args = fn_call->get_args()->get_expressions();
        for (auto it = fn_args.begin(); it != fn_args.end(); ++it) {
          ops.push_back(*it);
        }

        if (ops.size() == 2) {
          vector<void *> member_names;
          vector<SgStatement *> assigns;
          //  strange to me why Handler handler =
          //  &(LowerCpp::handle_overload_assigns); doesnt work
          Handler handler = &LowerCpp::handle_overload_assigns;

          traverse_all_overload_mfns(cls, ops, &member_names, &assigns,
                                     handler);
          reverse(assigns.begin(), assigns.end());
          for (auto &call : assigns) {
            cg->InsertAfterStmt(call, cg->TraceUpToStatement(mfnref));
          }
        }
      }
      DEBUG(cerr << "\tignoring because class wasn't lowered: "
                 << cls->get_name() << "\n");
      return;
    }

    //  Ensure that we've previously encountered the definition of whatever
    //  we're calling.
    auto it = fn_remap.find(mfnref->get_symbol());
    if (it == fn_remap.end()) {
      void *decl = mfnref->getAssociatedMemberFunctionDeclaration();
      if (V_SgAssignOp == cg->GetOperatorOp(decl)) {
        //  skip implicit assignment operator function
        return;
      }
    }
    if (it == fn_remap.end()) {
      DEBUG(cerr << "Call to an undefined function.");
      return;
    }

    //  Starting from `mfnref`, walk towards the root and take note of the
    //  first arrow or dot expr node. Also, for sanity, check for the
    //  aforementioned SgFunctionCallExp node.
    SgFunctionCallExp *callexp = nullptr;
    SgBinaryOp *op = nullptr;
    for (SgNode *node = mfnref->get_parent();
         (callexp = isa<SgFunctionCallExp>(node)) == nullptr;
         node = node->get_parent()) {
      //  Take note of the closest parent arrow or dot expr node.
      if (op == nullptr) {
        if (auto *arr = isa<SgArrowExp>(node)) {
          op = arr;
        } else if (auto *dot = isa<SgDotExp>(node)) {
          op = dot;
        }
      }

      if (node == cg->GetProject()) {
        DEBUG(cerr << "Ignoring mem fn call not contained in "
                      "SgFunctionCallExp sub-tree.\n");
        return;
      }
    }

    if (it->second.was_static_mfn) {
      DEBUG(cerr << "it's a static\n");
      SgFunctionRefExp *newref =
          SageBuilder::buildFunctionRefExp(it->second.decl());
      SgExpression *oldfn = callexp->get_function();
      cg->ReplaceExp(oldfn, newref);
    } else {
      //  Handle a limited number of cases for now. TODO: Handle other
      //  cases as they are encountered.
      assert(op && "Non-static mem fn call should be child to an arrow "
                   "or dot expr.");
      assert(op->get_rhs_operand() == mfnref && "Unhandled case.");
      assert(op->get_parent() == callexp && "Unhandled case.");
      //  Lowering rules, in a gist:
      //  - If it's an arrow: x->f() => f_mangled(x, ...).
      //  - If it's a dot: x.f() => f_mangled(&x, ...).
      //    ((Base)x).f => f_mangled(&x.Base, ...).
      SgFunctionRefExp *newref =
          SageBuilder::buildFunctionRefExp(it->second.decl());
      SgExpression *lhs = op->get_lhs_operand();

      if (auto *arr = isa<SgArrowExp>(op)) {
        //  replace the expression, but keep the original LHS for the
        //  call's first argument.
        callexp->get_args()->prepend_expression(
            static_cast<SgExpression *>(cg->CopyExp(lhs)));
        cg->ReplaceExp(arr, newref, false);
      } else if (auto *dot = isa<SgDotExp>(op)) {
        if (!lhs->isLValue()) {
          string var_name = "tmp";
          cg->get_valid_global_name(cg->GetGlobal(callexp), &var_name);
          cg->get_valid_local_name(cg->TraceUpToFuncDecl(callexp), &var_name);
          auto block = cg->TraceUpToBasicBlock(callexp);
          if (block == nullptr) {
            block = cg->GetGlobal(callexp);
          }
          auto type = cg->GetBaseType(cg->GetTypeByExp(lhs), false);
          auto var_decl =
              cg->CreateVariableDecl(type, var_name, nullptr, block);
          auto stmt = cg->TraceUpToStatement(callexp);
          cg->InsertStmt(var_decl, stmt);
          auto assign = cg->CreateExp(
              V_SgAssignOp, cg->CreateVariableRef(var_decl), cg->CopyExp(lhs));
          cg->ReplaceExp(dot, newref, false);
          auto call = cg->CopyExp(callexp);
          static_cast<SgFunctionCallExp *>(call)
              ->get_args()
              ->prepend_expression(static_cast<SgExpression *>(cg->CreateExp(
                  V_SgAddressOfOp, cg->CreateVariableRef(var_decl))));
          auto comma = cg->CreateExp(V_SgCommaOpExp, assign, call);
          cg->ReplaceExp(callexp, comma, false);
        } else {
          callexp->get_args()->prepend_expression(static_cast<SgExpression *>(
              cg->CreateExp(V_SgAddressOfOp, cg->CopyExp(lhs))));
          cg->ReplaceExp(dot, newref, false);
        }
      }
    }
  }

  //  Lower (calls to) function declarations with default parameters. For
  //  instance, `void f(unsigned = 42); f();` becomes `void f(unsigned);
  //  f(42);`.
  void lower_default_params(SgNode *n) {
    std::unordered_map<SgFunctionSymbol *, SgFunctionDeclaration *> defparmfns;
    std::set<SgInitializedName *> defparams;
    //  First figure out which decls actually have default params, and lower
    //  those.
    trav<SgFunctionDeclaration>(
        n,
        [&](SgFunctionDeclaration *decl) {
          SgFunctionSymbol *fnsym = helpers::fn_symbol(decl);
          bool has_default_val = false;
          for (SgInitializedName *arg : decl->get_args()) {
            if (arg->get_initializer() != nullptr) {
              has_default_val = true;
              defparams.insert(arg);
            }
          }
          if (has_default_val) {
            defparmfns[fnsym] = decl;
          }
        },
        [&](SgFunctionDeclaration *decl) {
          if (cg->IsTemplateFunctionDecl(decl) ||
              cg->IsTemplateMemberFunctionDecl(decl)) {
            return false;
          }
          if (void *cls = cg->TraceUpToClassDecl(decl)) {
            if (cg->IsTemplateClassDecl(cls)) {
              return false;
            }
          }
          if (isa<SgMemberFunctionDeclaration>(decl) &&
              helpers::is_ctor(*isa<SgMemberFunctionDeclaration>(decl))) {
            return false;
          }
          return true;
        },
        preorder);

    //  Now lower calls to functions that we saw had default params. ROSE
    //  ensures that the arity of an SgFunctionCallExp's SgExprListExp
    //  matches that of the called function by adding SgExpression nodes of
    //  the function's default values where necessary. So we only have to
    //  flag each SgExpression call argument to be printed during unparsing.
    trav<SgFunctionCallExp>(
        n,
        [&](SgFunctionCallExp *call) {
          SgExpression *fnref = call->get_function();
          while (isa<SgDotExp>(fnref) || isa<SgArrowExp>(fnref)) {
            if (auto *dot = isa<SgDotExp>(fnref)) {
              fnref = dot->get_rhs_operand();
            } else if (auto *arrow = isa<SgArrowExp>(fnref)) {
              fnref = arrow->get_rhs_operand();
            }
          }

          if (isa<SgVarRefExp>(fnref)) {
            // which means function pointer call
            return;
          }

          SgFunctionDeclaration *fndecl = nullptr;
          if (auto *ref = isa<SgFunctionRefExp>(fnref)) {
            fndecl = ref->getAssociatedFunctionDeclaration();
          } else if (auto *ref = isa<SgMemberFunctionRefExp>(fnref)) {
            fndecl = ref->getAssociatedMemberFunctionDeclaration();
          }

          if (fndecl && defparmfns.count(helpers::fn_symbol(fndecl))) {
            for (SgExpression *arg : call->get_args()->get_expressions()) {
              cg->ReplaceExp(arg, cg->CopyExp(arg));
            }
            auto decl = defparmfns[helpers::fn_symbol(fndecl)];
            int i = 0;
            int call_arg_size = call->get_args()->get_expressions().size();
            for (auto arg : decl->get_args()) {
              i++;
              if (i <= call_arg_size)
                continue;
              if (auto init = arg->get_initializer()) {
                call->get_args()->append_expression(static_cast<SgExpression *>(
                    cg->CopyExp(cg->GetInitializerOperand(init))));
              }
            }
          }
        },
        [&](SgFunctionCallExp *call) {
          if (void *fn = cg->TraceUpToFuncDecl(call)) {
            if (cg->IsTemplateFunctionDecl(fn) ||
                cg->IsTemplateMemberFunctionDecl(fn)) {
              return false;
            }
          }
          if (void *cls = cg->TraceUpToClassDecl(call)) {
            if (cg->IsTemplateClassDecl(cls)) {
              return false;
            }
          }
          return true;
        },
        postorder);

    for (auto arg : defparams) {
      arg->set_initializer(nullptr);
    }
  }

  //  Lower uses of `this`.
  void lower_this(SgThisExp *t) {
    //  Since `this` must only occur inside a function def, traverse up the
    //  AST until we find the one that owns `t`.
    SgFunctionDefinition *def = nullptr;

    SgNode *scope = t->get_parent();
    while ((def = isa<SgFunctionDefinition>(scope)) == nullptr &&
           scope != cg->GetProject()) {
      scope = scope->get_parent();
    }
    if (scope == cg->GetProject()) {
      cout << "Reached the AST root without finding a def.\n";
      return;
    }
    //  If there is no `this` to remap to, then there's nothing to do.
    auto it = this_map.find(def->get_declaration()->get_name());
    if (it == this_map.end()) {
      return;
    }

    cg->ReplaceExp(t, cg->CreateVariableRef(it->second, def));
  }

  void debug_print_all(SgNode *ast, std::ostream &out) {
    out << "Result:\n";
    cg->TraversePre(ast, [&](SgNode *n) { out << cg->_p(n, 80) << "\n"; });
  }

  //  Lower template function instances.
  void lower_tfn_inst(SgTemplateInstantiationFunctionDecl *tfinst) {
    //  Given an instantiation, return its unspecialized decl.
    auto orig_tfn = [&](const SgTemplateInstantiationFunctionDecl &tfinst) {
      auto tf = tfinst.get_templateDeclaration();
      if (tf->get_declaration_associated_with_symbol() != nullptr) {
        auto *declsym = isa<SgTemplateFunctionSymbol>(
            tf->search_for_symbol_from_symbol_table());
        assert(declsym && "Expected a symbol to unspecialized template.");
        auto it = tfn_syms.find(declsym);
        return it != tfn_syms.end() ? it->second : nullptr;
      }
      return static_cast<SgTemplateFunctionDeclaration *>(nullptr);
    };

    //  Filter out non-user-defined template instances. For instance, it's
    //  not our job to lower std::min<size_t>.
    SgTemplateFunctionDeclaration *realtfn = orig_tfn(*tfinst);
    DEBUG(cerr << "lower_tfn_inst " << tfinst
               << "\n\tunspecialized: " << realtfn << "\n");
    if ((realtfn == nullptr) || (!realtfn->isTransformation() &&
                                 !m_mars_ir.is_used_by_kernel(realtfn))) {
      DEBUG(cerr << "\tignoring.\n");
      return;
    }
    if (duplicated_function_definition(tfinst)) {
      return;
    }

    DEBUG(cerr << "lower_tfn_inst called on " << tfinst << "\n");

    std::stringstream mangled;
    helpers::Mangler(mangled, cg).mangle(tfinst);

    void *c = cg->CloneFuncDecl(tfinst, cg->GetGlobal(realtfn), mangled.str(),
                                tfinst->get_definition() != nullptr);
    auto &cloned = *static_cast<SgFunctionDeclaration *>(c);
    //  The correct place to insert such that `cloned` dominates all its
    //  uses is just before the unspecialized def.
    cg->InsertStmt(&cloned, helpers::get_insert_position(realtfn, cg));

    SgFunctionSymbol *sym = helpers::fn_symbol(tfinst);
    if (tfinst->get_definition() != nullptr) {
      //  Note the mapping between old symbol and lowered fn def. Needed
      //  for replacing calls to old template fn with calls to the new def.
      add_fn_remap(*sym, cloned.get_definition());
      DEBUG(cerr << "lowered " << tfinst << " to " << &cloned << "\n");
    } else {
      add_decl_remap(*sym, &cloned);
    }
    unordered_map<string, string> token_map;
    get_token_remap_in_template_function(*tfinst, &token_map);
    cg->ReplacePragmaTokens(&cloned, token_map);
    //  If this was an extern template fn decl, then its
    //  SgTemplateInstantiationFunctionDecl would live under an
    //  SgTemplateInstantiationDirectiveStatement.
    if (auto *tids = isa<SgTemplateInstantiationDirectiveStatement>(
            tfinst->get_parent())) {
      DEBUG(cerr << "\thas dir stmt parent. extern? "
                 << SageInterface::isExtern(tids) << "\n");
      DEBUG(cerr << "\tlinkage: " << tids->get_linkage() << "\n");
      //  FIXME: This is an utter hack to detect extern template fns. For
      //  such a decl, ROSE emits a DirectiveStatement containing a
      //  FunctionDecl, but neither node is extern.
      cg->SetExtern(&cloned);
      delete_later(tids);
    } else {
      delete_later(tfinst);
    }
  }

  //  Lower all regular (non-member, non-template) fns to a mangled clone.
  void lower_overloaded_fns(SgNode *ast) {
    //  First figure out which functions actually have overloads. If a
    //  function is regular and has no overloads, then we don't bother
    //  lowering it.
    //
    //  Also note that since C++ rules forbid main and extern "C" functions
    //  from being overloaded, this effectively prevents them from being
    //  mangled, as desired.
    map<string, set<string>> name_to_overloads;
    vector<SgFunctionDeclaration *> decls;
    trav<SgFunctionDeclaration>(ast, [&](SgFunctionDeclaration *decl) {
      //  skip all functions in template declaration
      SgNode *p = decl;
      while ((p != nullptr) && (isa<SgGlobal>(p) == nullptr)) {
        if ((isa<SgTemplateClassDeclaration>(p) != nullptr) ||
            (isa<SgTemplateFunctionDeclaration>(p) != nullptr) ||
            (isa<SgTemplateMemberFunctionDeclaration>(p) != nullptr)) {
          return;
        }
        p = p->get_parent();
      }

      SgFunctionSymbol &fnsym = *helpers::fn_symbol(decl);
      if ((isa<SgMemberFunctionDeclaration>(decl) == nullptr) &&
          (isa<SgTemplateFunctionDeclaration>(decl) == nullptr) &&
          (isa<SgTemplateInstantiationFunctionDecl>(decl) == nullptr) &&
          (lowered_fns.count(&fnsym) == 0u) && (fn_remap.count(&fnsym) == 0u)) {
        decls.push_back(decl);
        std::stringstream mangled;
        helpers::Mangler(mangled, cg).mangle(decl);
        string manged = mangled.str();
        mangle_cache.insert({&fnsym, manged});
        auto it = name_to_overloads.find(decl->get_name());
        if (it == name_to_overloads.end()) {
          name_to_overloads.insert({decl->get_name(), set<string>{manged}});
        } else {
          it->second.insert(manged);
        }
      }
    });

    DEBUG(for (auto &pair
               : name_to_overloads) {
      cerr << "name: " << pair.first << ":\n";
      for (const string &overload : pair.second)
        cerr << "\toverload: " << overload << "\n";
    });

    //  Now mangle the ones with overloads.
    for (SgFunctionDeclaration *decl : decls) {
      SgFunctionSymbol &sym = *helpers::fn_symbol(decl);
      unsigned num_overloads = name_to_overloads[decl->get_name()].size();
      if (num_overloads > 1) {
        if (set_kernel.count(&sym) > 0) {
          helpers::show_unsupported_kern(
              decl, helpers::KernUnsup::CantBeOverloaded, cg, false);
          err = true;
        }
        if (duplicated_function_definition(decl)) {
          continue;
        }
        const string &manged = mangle_cache[&sym];
        //  `decl` has an overload. Lower it into mangled clone.
        auto *cloned = static_cast<SgFunctionDeclaration *>(
            cg->CloneFuncDecl(decl, cg->GetGlobal(decl), manged,
                              decl->get_definition() != nullptr));
        cg->InsertStmt(cloned, helpers::get_insert_position(decl, cg));
        DEBUG(cerr << "function " << decl->get_name() << " has "
                   << num_overloads << " overloads; lowering to " << manged
                   << "\n\t"
                   << " with body " << cloned->get_definition() << "\n");

        if (decl->get_definition() != nullptr) {
          add_fn_remap(sym, cloned->get_definition());
        } else {
          add_decl_remap(sym, cloned);
        }

        delete_later(decl);
      } else {
        DEBUG(cerr << "function " << decl->get_name() << " has no overloads\n");
      }
    }
  }

  void lower_operator_fns(SgNode *ast) {
    trav<SgFunctionDeclaration>(ast, [&](SgFunctionDeclaration *fn) {
      SgFunctionSymbol &sym = *helpers::fn_symbol(fn);
      //  We've already lowered these, or they will be lowered elsewhere.
      //  Don't double-lower.
      if (del_set.count(isa<SgStatement>(fn)) > 0) {
        return;
      }
      if ((isa<SgMemberFunctionDeclaration>(fn) != nullptr) ||
          (isa<SgTemplateFunctionDeclaration>(fn) != nullptr) ||
          (isa<SgTemplateInstantiationFunctionDecl>(fn) != nullptr) ||
          (!fn->get_specialFunctionModifier().isOperator() &&
           (helpers::get_operator_cast(*fn) == nullptr))) {
        return;
      }
      if (duplicated_function_definition(fn)) {
        return;
      }

      std::stringstream mangled;
      helpers::Mangler(mangled, cg).mangle(fn);
      auto *cloned = static_cast<SgFunctionDeclaration *>(cg->CloneFuncDecl(
          fn, cg->GetGlobal(fn), cg->legalizeName(mangled.str()),
          fn->get_definition() != nullptr));
      cg->InsertStmt(cloned, helpers::get_insert_position(fn, cg));
      if (fn->get_definition() != nullptr) {
        add_fn_remap(sym, cloned->get_definition());
      } else {
        add_decl_remap(sym, cloned);
      }

      delete_later(fn);
    });
  }

  // lower simple function in namespace
  void lower_simple_fns_in_namespace(SgNode *ast) {
    trav<SgFunctionDeclaration>(ast, [&](SgFunctionDeclaration *fn) {
      SgFunctionSymbol &sym = *helpers::fn_symbol(fn);
      cout << cg->_p(fn, 0) << endl;
      //  We've already lowered these, or they will be lowered elsewhere.
      //  Don't double-lower.
      if (del_set.count(isa<SgStatement>(fn)) > 0) {
        return;
      }
      if ((isa<SgMemberFunctionDeclaration>(fn) != nullptr) ||
          (isa<SgTemplateFunctionDeclaration>(fn) != nullptr) ||
          (isa<SgTemplateInstantiationFunctionDecl>(fn) != nullptr) ||
          fn->get_specialFunctionModifier().isOperator() ||
          (helpers::get_operator_cast(*fn) != nullptr)) {
        return;
      }
      if (!isa<SgNamespaceDefinitionStatement>(fn->get_scope())) {
        return;
      }
      if (duplicated_function_definition(fn)) {
        return;
      }
      cout << cg->_p(fn, 0) << endl;
      std::stringstream mangled;
      helpers::Mangler(mangled, cg).mangle(fn);
      auto *cloned = static_cast<SgFunctionDeclaration *>(cg->CloneFuncDecl(
          fn, cg->GetGlobal(fn), cg->legalizeName(mangled.str()),
          fn->get_definition() != nullptr));
      cg->InsertStmt(cloned, helpers::get_insert_position(fn, cg));

      cout << cg->_p(cloned, 0) << endl;
      if (fn->get_definition() != nullptr) {
        add_fn_remap(sym, cloned->get_definition());
      } else {
        add_decl_remap(sym, cloned);
      }

      delete_later(fn);
    });
  }
  //  We group together and run all transformations that create function
  //  decls/defs that correspond one-to-one with old ones.
  void build_fn_remap(SgNode *ast) {
    cout << " -- LowerCPP step 4.1" << endl;
    //  Build tmfn_syms, which is needed by function de-templatization.
    trav<SgTemplateMemberFunctionDeclaration>(
        ast,
        [&](SgTemplateMemberFunctionDeclaration *tmfn) {
          if (tmfn->get_declaration_associated_with_symbol() != nullptr) {
            auto *tmfnsym = isa<SgTemplateMemberFunctionSymbol>(
                tmfn->search_for_symbol_from_symbol_table());
            DEBUG(cerr << "Found template fn decl sym " << cg->_p(tmfn, 40)
                       << "\n");

            tmfn_syms[tmfnsym] = tmfn;
            //  Since we're here already, schedule the template for deletion
            //  (remember, there are no templates in C).
            delete_later(tmfn);
          }
        },
        [&](SgTemplateMemberFunctionDeclaration *n) {
          bool used_by_kernel = true;
          if (n->isTransformation() ||
              (used_by_kernel = m_mars_ir.is_used_by_kernel(n))) {
            return true;
          }
          DEBUG(cerr << "skipping non-user-defined node " << cg->_p(n, 80)
                     << "\n\tptr: " << &n
                     << " used by kernel: " << used_by_kernel << "\n");
          return false;
        },
        preorder);

    //  Lower member functions. This depends on the cls_remap built during
    //  class lowering.
    trav<SgMemberFunctionDeclaration>(
        ast, [&](SgMemberFunctionDeclaration *mfn) { lower_memfn_decl(mfn); });

    cout << " -- LowerCPP step 4.2" << endl;
    //  Build tfn_syms, which is needed by function de-templatization.
    trav<SgTemplateFunctionDeclaration>(
        ast,
        [&](SgTemplateFunctionDeclaration *tfn) {
          if (tfn->get_declaration_associated_with_symbol() != nullptr) {
            auto tfnsym = isa<SgTemplateFunctionSymbol>(
                tfn->search_for_symbol_from_symbol_table());
            DEBUG(cerr << "Found template fn decl sym " << cg->_p(tfn, 40)
                       << "\n");

            tfn_syms[tfnsym] = tfn;
            //  Since we're here already, schedule the template for deletion
            //  (remember, there are no templates in C).
            delete_later(tfn);
          }
        },
        [&](SgTemplateFunctionDeclaration *n) {
          bool used_by_kernel = true;
          if (n->isTransformation() ||
              (used_by_kernel = m_mars_ir.is_used_by_kernel(n))) {
            return true;
          }
          DEBUG(cerr << "skipping non-user-defined node " << cg->_p(n, 80)
                     << "\n\tptr: " << n
                     << " used by kernel: " << used_by_kernel << "\n");
          return false;
        },
        preorder);

    cout << " -- LowerCPP step 4.3" << endl;
    //  Lower template function instances.
    trav<SgTemplateInstantiationFunctionDecl>(
        ast,
        //  Handler
        [&](SgTemplateInstantiationFunctionDecl *node) {
          lower_tfn_inst(node);
        });
    cout << " -- LowerCPP step 4.4" << endl;
    lower_operator_fns(ast);

    cout << " -- LowerCPP step 4.7" << endl;
    if (cg->isWithInCppFile(ast)) {
      lower_overloaded_fns(ast);
    }

    cout << " -- LowerCPP step 4.8" << endl;

    lower_simple_fns_in_namespace(ast);
  }

  void remap_fn_ref_exps(SgNode *ast) {
    //  Lower ctor decls. Needs to happen in post-order to handle nested
    //  ctors.
    trav<SgConstructorInitializer>(
        ast,
        [&](SgConstructorInitializer *ci) {
          DEBUG(cerr << "processing a ctor call "
                     << cg->_p(ci->get_parent(), 80)
                     << "\n\tptr: " << ci->get_parent() << "\n");
          lower_ctor_call(ci);
        },
        [&](SgConstructorInitializer *n) {
          bool used_by_kernel = true;
          if (n->isTransformation() ||
              ((used_by_kernel = m_mars_ir.is_used_by_kernel(n)) &&
               helpers::should_lower(n, cg))) {
            return true;
          }
          DEBUG(cerr << "skipping non-user-defined node " << cg->_p(n, 80)
                     << "\n\tptr: " << n
                     << " used by kernel: " << used_by_kernel << "\n");
          return false;
        },
        postorder);

    //  Update mem fn refs first because they sub-class SgFunctionRefExp and
    //  require special matching and handling.
    trav<SgMemberFunctionRefExp>(
        ast, [&](SgMemberFunctionRefExp *t) { lower_memfn_call(t); },
        postorder);

    //  Update all other function references according to fn_remap.
    trav<SgFunctionRefExp>(ast,
                           [&](SgFunctionRefExp *fnref) {
                             auto it = fn_remap.find(fnref->get_symbol());
                             if (it == fn_remap.end()) {
                               //  Don't need to remap this function ref.
                               return;
                             }
                             SgFunctionDeclaration *newdecl = it->second.decl();
                             auto newfnsym = helpers::fn_symbol(newdecl);
                             fnref->set_symbol(newfnsym);
                           },
                           postorder);

    //  //  Lower ctor decls. Needs to happen in post-order to handle nested
    //  //  ctors.
    //  trav<SgAssignInitializer>(
    //    ast,
    //    [&](SgAssignInitializer *ai) {
    //      DEBUG(cerr << "processing a ctor call " << cg->_p(ai->get_parent(),
    //      80)
    //                 << "\n\tptr: " << ai->get_parent() << "\n");
    //      lower_var_decl_assign(*ai);
    //    },
    //    postorder);
  }

  void lower_static_stor_vars(SgNode *ast) {
    unordered_set<SgSymbol *> used_vars;
    trav<SgVarRefExp>(ast, [&](SgVarRefExp *ref) {
      used_vars.insert(ref->get_symbol());
      return;
    });

    trav<SgPragmaDeclaration>(ast, [&](SgPragmaDeclaration *decl) {
      string token_str = "";
      bool local_err = false;
      auto res =
          cg->GetVariablesUsedByPragma(decl, &local_err, &token_str, false);
      for (auto &var_pair : res) {
        used_vars.insert(
            isSgInitializedName(static_cast<SgNode *>(var_pair.first))
                ->search_for_symbol_from_symbol_table());
      }
      return;
    });
    //  For each var decl, check if it's a namespace-global or static class
    //  member.
    trav<SgVariableDeclaration>(ast, [&](SgVariableDeclaration *decl) {
      if (!((isa<SgNamespaceDefinitionStatement>(decl->get_scope()) !=
             nullptr) ||
            ((isa<SgClassDefinition>(decl->get_scope()) != nullptr) &&
             cg->IsStatic(decl)))) {
        return;
      }
      for (SgInitializedName *iname : decl->get_variables()) {
        DEBUG(cerr << "lower_static_stor_vars " << cg->_p(iname, 80) << "\n");
        auto *vsym =
            isa<SgVariableSymbol>(iname->search_for_symbol_from_symbol_table());
        if (static_remap.count(vsym) > 0) {
          DEBUG(cerr << "Lowering encountered the same static-dur var twice "
                     << cg->_p(iname, 80) << "\n");
          continue;
        }
        if (used_vars.count(vsym) <= 0) {
          DEBUG(cerr << "Lowering encountered satic-dur var without reference "
                     << cg->_p(iname, 80) << "\n");
          continue;
        }
        auto _decl = static_cast<SgVariableDeclaration *>(
            cg->TraceUpToStatement(cg->GetVariableDefinitionByName(iname)));
        auto _iname = static_cast<SgInitializedName *>(
            cg->GetVariableInitializedName(_decl));

        //  Duplicate the variable to a mangled clone.
        std::stringstream mangled;
        helpers::Mangler(mangled, cg).mangle_var(_iname);

        void *ty = cg->GetTypebyVar(_iname);
        void *ii = cg->GetInitializer(_iname);
        ii = ii != nullptr ? cg->CopyExp(ii) : ii;
        if (cg->IsConstType(ty) && ii == nullptr) {
          auto info = getUserCodeInfo(cg, _decl);
          DEBUG(cerr << "Uninitialised const variable " << cg->_p(_decl)
                     << "\n");
          string msg{"Uninitialised const variable: '" + info.name + "' (" +
                     info.file_location + ")"};
          dump_critical_message(LWCPP_ERROR_3(msg), 0);
          err = true;
        }
        auto *var = static_cast<SgVariableDeclaration *>(cg->CreateVariableDecl(
            ty, cg->legalizeName(mangled.str()), ii, cg->GetGlobal(_decl)));
        assert(var);

        //  Update `static_remap` so that we can remap SgVarRefExp later.
        SgInitializedName *lowered = var->get_variables().front();
        static_remap[vsym] = lowered;

        cg->InsertStmt(var, helpers::get_insert_position(_decl, cg));
        DEBUG(cerr << "lower_static_stor_vars inserted " << cg->_p(var, 80)
                   << " before " << cg->_p(_decl->get_scope(), 80) << "\n");
        delete_later(_decl);
      }
    });
  }

  void lower_inheritance_cast(SgNode *ast) {
    trav<SgCastExp>(ast, [&](SgCastExp *cast) {
      DEBUG(cout << cg->_p(cast) << endl);
      DEBUG(cout << cg->_p(cg->TraceUpToScope(cast)) << endl);
      if (cg->is_floating_node(cast) != 0) {
        return;
      }
      trans_cast_to_base(cast);
      return;
    });
  }

  void remap_var_refs(SgNode *ast) {
    trav<SgVarRefExp>(ast, [&](SgVarRefExp *ref) {
      DEBUG(cout << cg->_p(ref) << endl);
      DEBUG(cout << cg->_p(cg->TraceUpToScope(ref)) << endl);
      if (cg->is_floating_node(ref) != 0) {
        return;
      }

      auto it = mv_remap.find(ref->get_symbol());
      if (it != mv_remap.end()) {
        const string &name = it->second.first;
        SgClassDefinition *clsdef = it->second.second;
        auto *newref =
            static_cast<SgVarRefExp *>(cg->CreateVariableRef(name, clsdef));
        cg->ReplaceExp(ref, newref, true);
        return;
      }

      auto itt = static_remap.find(ref->get_symbol());
      if (itt != static_remap.end()) {
        auto newref = cg->CreateVariableRef(itt->second, cg->GetGlobal(ref));
        auto ref_parent = cg->GetParent(ref);
        void *target = ref;
        if ((cg->IsArrowExp(ref_parent) != 0 ||
             cg->IsDotExp(ref_parent) != 0) &&
            cg->GetExpRightOperand(ref_parent) == ref) {
          target = ref_parent;
        }
        cg->ReplaceExp(target, newref, true);
      }
    });
    vector<void *> vec_refs;
    cg->get_floating_var_refs(cg->GetProject(), &vec_refs);
    for (size_t i = vec_refs.size(); i > 0; i--) {
      void *ref = vec_refs[i - 1];
      auto *var_ref = static_cast<SgVarRefExp *>(ref);
      auto it = static_remap.find(var_ref->get_symbol());
      if (it != static_remap.end()) {
        auto newref = cg->CreateVariableRef(it->second, cg->GetGlobal(ref));
        auto ref_parent = cg->GetParent(ref);
        void *target = ref;
        if ((cg->IsArrowExp(ref_parent) != 0 ||
             cg->IsDotExp(ref_parent) != 0) &&
            cg->GetExpRightOperand(ref_parent) == ref) {
          target = ref_parent;
        }
        void *exp_parent = cg->GetParent(target);
        if (cg->IsExpression(exp_parent)) {
          cg->ReplaceExp(target, newref, true);
        } else if (auto *sg_array_type =
                       isSgArrayType(static_cast<SgNode *>(exp_parent))) {
          sg_array_type->set_index(static_cast<SgExpression *>(newref));
        } else {
          DEBUG(cerr << cg->_p(exp_parent, 0) << endl);
          assert(0 && "unexpected parent of floating expression");
        }
      }
    }
  }

  void remap_var_used_in_pragma(SgNode *ast) {
    trav<SgPragmaDeclaration>(ast, [&](SgPragmaDeclaration *decl) {
      if (!helpers::should_lower(decl, cg))
        return;
      DEBUG(cerr << "pragma in " << cg->_p(cg->GetScope(decl), 0) << endl);
      string token_str = "";
      bool local_err = false;
      auto used_vars =
          cg->GetVariablesUsedByPragma(decl, &local_err, &token_str, false);
      unordered_map<string, string> token_map;
      for (auto var_pair : used_vars) {
        void *var = var_pair.first;
        string var_str = var_pair.second;
        auto init_var = static_cast<SgInitializedName *>(var);
        auto it = static_remap.find(static_cast<SgVariableSymbol *>(
            init_var->search_for_symbol_from_symbol_table()));
        if (it != static_remap.end()) {
          token_map[var_str] = cg->GetVariableName(it->second);
        }
        if (auto em_decl =
                isa<SgEnumDeclaration>(init_var->get_declaration())) {
          if (enum_remap.count(helpers::enum_symbol(em_decl)) > 0) {
            auto initializer = init_var->get_initializer();
            if (initializer != nullptr) {
              auto assign_initializer = isa<SgAssignInitializer>(initializer);
              if (assign_initializer != nullptr) {
                auto initializer_exp = assign_initializer->get_operand();
                auto em_val = isa<SgEnumVal>(initializer_exp);
                if (em_val != nullptr) {
                  token_map[var_str] = std::to_string(em_val->get_value());
                }
              }
            }
          }
        }
      }
      cg->ReplacePragmaTokens(decl, token_map);
    });
  }

  helpers::Unsup is_supported(SgNode *node) {
    using helpers::Unsup;
    if (auto *memfn = isa<SgMemberFunctionDeclaration>(node)) {
      SgClassDeclaration *cls =
          isa<SgClassDeclaration>(memfn->get_associatedClassDeclaration());
      string name(memfn->get_name().getString());
      if (name[0] == '~') {
        if (auto *memdef = memfn->get_definition()) {
          if (!memdef->get_body()->get_statements().empty()) {
            return Unsup::Dtor;
          }
        }
      }
      if (memfn->get_functionModifier().isVirtual()) {
        return Unsup::VirtualMemFn;
      }

      if (helpers::is_const_ref_operator_cast(*memfn)) {
        return Unsup::ConstRefOperatorCast;
      }

      //  Check for delegated ctors. Those belong to C++11.
      if (helpers::is_ctor(*memfn)) {
        if (SgCtorInitializerList *ctor = memfn->get_CtorInitializerList()) {
          string clsname(cls->get_name().getString());
          for (SgInitializedName *iname : ctor->get_ctors()) {
            DEBUG(cerr << "ctorinit " << cg->_p(iname, 80) << "\n");
            if (iname->get_name().getString() == clsname) {
              return Unsup::DelegatedCtor;
            }
          }
        }
      }

      if (memfn->get_specialFunctionModifier().isOperator()) {
        if (name.find("new") != std::string::npos) {
          return Unsup::OperatorNew;
        }
        if (name.find("delete") != std::string::npos) {
          return Unsup::OperatorDelete;
        }
      }
    } else if (isa<SgTryStmt>(node) != nullptr) {
      //  try-except will never be supported.
      return Unsup::TryCatch;
    } else if (isa<SgNewExp>(node) != nullptr) {
      return Unsup::OperatorNew;
    } else if (isa<SgDeleteExp>(node) != nullptr) {
      return Unsup::OperatorDelete;
    } else if (auto *def = isa<SgClassDefinition>(node)) {
      //  Reject reference-typed members because we're currently too lazy
      //  to lower references ourselves. TODO: Factor this with the
      //  is_cpp_agg tests below to avoid iterating too much over the
      //  members.
      auto is_ref = [&](SgInitializedName *i) {
        return !(isa<SgReferenceType>(i->get_type()) == nullptr);
      };
      if (helpers::any_of(def->get_members(), [&](SgDeclarationStatement *mem) {
            if (auto *var = isa<SgVariableDeclaration>(mem)) {
              return helpers::any_of(var->get_variables(), is_ref);
            }
            return false;
          })) {
        return Unsup::RefTypedMember;
      }

      //  Check for inline agg defs that would need lowering. We don't do
      //  those.
      if ((isa<SgVariableDeclaration>(def->get_declaration()->get_parent()) !=
           nullptr) &&
          helpers::is_cpp_agg(def)) {
        return Unsup::InlineAggDefs;
      }
#if 0
      if ((isa<SgGlobal>(def->get_scope()) == nullptr) &&
          (isa<SgNamespaceDefinitionStatement>(def->get_scope()) == nullptr) &&
          helpers::is_cpp_agg(def)) {
        return Unsup::NestedAgg;
      }
#endif
      if (def->get_inheritances().size() > 1) {
        return Unsup::MultInheritance;
      }

      for (auto base : def->get_inheritances()) {
        if (base->get_baseClassModifier()->isVirtual()) {
          return Unsup::VirtInheritance;
        }
      }

      //  Check for bit-fields.
      for (SgDeclarationStatement *memdecl : def->get_members()) {
        if (auto *vardecl = isa<SgVariableDeclaration>(memdecl)) {
          if (vardecl->get_bitfield() != nullptr) {
            return Unsup::BitFields;
          }
        }
      }
    } else if (auto *def = isa<SgDotStarOp>(node)) {
      return Unsup::MemberPointerOp;
    } else if (auto *fn = isa<SgFunctionDeclaration>(node)) {
      if (fn->get_specialFunctionModifier().isOperator()) {
        string name(fn->get_name().getString());
        if (name.find("new") != std::string::npos) {
          return Unsup::OperatorNew;
        }
        if (name.find("delete") != std::string::npos) {
          return Unsup::OperatorDelete;
        }
      }
    } else if (auto *ctor = isa<SgConstructorInitializer>(node)) {
      if (auto *sg_class = ctor->get_class_decl()) {
        if (!helpers::should_lower(sg_class, cg)) {
          return Unsup::Supported;
        }
      }
      if (SgScopeStatement *sc = SageInterface::getScope(ctor)) {
        if ((isa<SgGlobal>(sc) != nullptr) ||
            (isa<SgNamespaceDefinitionStatement>(sc) != nullptr)) {
          return Unsup::NonFnOrClsCtor;
        }
      }
    } else if (auto *tcinst = isa<SgTemplateInstantiationDecl>(node)) {
      for (SgTemplateArgument *targ : tcinst->get_templateArguments()) {
        if (targ->get_type() == nullptr && targ->get_expression() == nullptr) {
          return Unsup::TemplateArgument;
        }
      }
    } else if (auto *tfinst = isa<SgTemplateInstantiationFunctionDecl>(node)) {
      for (SgTemplateArgument *targ : tfinst->get_templateArguments()) {
        if (targ != nullptr && targ->get_type() == nullptr &&
            targ->get_expression() == nullptr) {
          return Unsup::TemplateArgument;
        }
      }
    } else if (auto *tmfinst =
                   isa<SgTemplateInstantiationMemberFunctionDecl>(node)) {
      for (SgTemplateArgument *targ : tmfinst->get_templateArguments()) {
        if (targ != nullptr && targ->get_type() == nullptr &&
            (targ->get_expression() == nullptr)) {
          return Unsup::TemplateArgument;
        }
      }
    }

    return Unsup::Supported;
  }

  bool is_cpp_ty(SgType *kernty) {
    if (SgType *ty = kernty->findBaseType()) {
      if (auto *nty = isa<SgNamedType>(ty)) {
        if (SgDeclarationStatement *decl = nty->get_declaration()) {
          if (auto *cls = isa<SgClassDeclaration>(decl)) {
            auto it = sym_to_def.find(helpers::cls_symbol(cls));
            if (it != sym_to_def.end() && helpers::is_cpp_agg(it->second)) {
              return true;
            }

            if (auto *tc_def = isa<SgTemplateInstantiationDecl>(decl)) {
              for (SgTemplateArgument *targ : tc_def->get_templateArguments()) {
                if (auto *tt = targ->get_type()) {
                  if (is_cpp_ty(tt)) {
                    return true;
                  }
                }
              }
            }
          }

          if (auto *enum_decl = isa<SgEnumDeclaration>(decl)) {
            if (isa<SgGlobal>(enum_decl->get_scope()) == nullptr ||
                enum_decl->get_isScopedEnum()) {
              return true;
            }
          }
        }
      }
    }
    return false;
  }

  helpers::KernUnsup check_supported_kernel(SgFunctionDeclaration *kern) {
    using helpers::KernUnsup;
    //  Ensure that kern lives in the global scope.
    if (isa<SgGlobal>(kern->get_scope()) == nullptr) {
      return KernUnsup::CantBeNamespaced;
    }
    if (kern->get_specialFunctionModifier().isOperator()) {
      return KernUnsup::NoOperators;
    }
    if ((isa<SgTemplateFunctionDeclaration>(kern) != nullptr) ||
        (isa<SgTemplateInstantiationDecl>(kern) != nullptr)) {
      return KernUnsup::NoTemplates;
    }
    if (isa<SgMemberFunctionDeclaration>(kern) != nullptr) {
      return KernUnsup::NoMembers;
    }
    for (SgType *argty : kern->get_type()->get_arguments()) {
      if (is_cpp_ty(argty)) {
        return KernUnsup::UnnestedCType;
      }
    }

    if (is_cpp_ty(kern->get_type()->get_return_type())) {
      return KernUnsup::UnnestedCType;
    }

    if (cg->IsStatic(kern)) {
      return KernUnsup::CantBeStatic;
    }

    return KernUnsup::Supported;
  }

  void check_unsupported(SgNode *ast) {
    //  vector<SgNode *> unsupporteds;
    bool err = false;
    trav<SgLocatedNode>(
        ast,
        [&](SgLocatedNode *locnode) {
          helpers::Unsup code = is_supported(locnode);
          if (code != helpers::Unsup::Supported) {
            err = true;
            helpers::show_unsupported_construct(locnode, code, cg);
          }
        },
        [&](SgLocatedNode *n) {
          bool used_by_kernel = true;
          if (n->isTransformation() ||
              (used_by_kernel = m_mars_ir.is_used_by_kernel(n))) {
            return true;
          }
          DEBUG(cerr << "skipping non-user-defined node " << cg->_p(n, 80)
                     << "\n\tptr: " << &n
                     << " used by kernel: " << used_by_kernel << "\n");
          return false;
        },
        preorder);

    for (void *v : m_mars_ir.get_top_kernels()) {
      //  At the time of writing, CMarsIrV2::mKernelTops is only inserted
      //  from CMarsIrV2::build_mars_ir with what appear to be
      //  SgFunctionDeclarations.
      auto *kernel = static_cast<SgFunctionDeclaration *>(v);
      assert(kernel && "MarsIR fed us a non-kernel.");
      helpers::KernUnsup code = check_supported_kernel(kernel);
      if (code != helpers::KernUnsup::Supported) {
        if (code == helpers::KernUnsup::UnnestedCType) {
          helpers::show_unsupported_kern(kernel, code, cg, true /*warning*/);
        } else {
          err = true;
          helpers::show_unsupported_kern(kernel, code, cg, false /*error*/);
        }
      }
      set_kernel.insert(helpers::fn_symbol(kernel));
    }

    if (err) {
      throw std::exception();
    }
  }

  void *find_proper_operator_typecast(void *sg_class_type) {
    static std::set<string> basic_integral_types{
        "int",           "signed int",           "unsigned int",
        "long",          "signed long",          "unsigned long",
        "long int",      "signed long int",      "unsigned long int",
        "short",         "signed short",         "unsigned short",
        "short int",     "signed short int",     "unsigned short int",
        "char",          "signed char",          "unsigned char",
        "long long",     "signed long long",     "unsigned long long",
        "long long int", "signed long long int", "unsigned long long int"};

    static std::set<string> basic_float_types{"float", "double", "long double"};

    vector<void *> members;
    cg->GetClassMembers(sg_class_type, &members);
    auto for_op_name = [this](void *memb) {
      return cg->IsFunctionDeclaration(memb) &&
                     cg->GetFuncName(memb).find("operator ") == 0
                 ? cg->GetFuncName(memb).substr(9)
                 : "";
    };
    std::function<bool(void *)> tests[]{
        [&](void *memb) { return for_op_name(memb) == "bool"; },
        [&, this](void *memb) {
          return basic_integral_types.count(for_op_name(memb)) > 0 ||
                 cg->IsEnumType(cg->GetTypeByName(for_op_name(memb)));
        },
        [&](void *memb) {
          return basic_float_types.count(for_op_name(memb)) > 0;
        }};
    for (auto te : tests) {
      auto res = std::find_if(members.begin(), members.end(), te);
      if (res != members.end()) {
        return *res;
      }
    }
    return nullptr;
  }

  void *make_obj_ref_as_condition(void *var_ref, void *sg_class_type) {
    auto *mfn = static_cast<SgMemberFunctionDeclaration *>(
        find_proper_operator_typecast(sg_class_type));
    assert(mfn != nullptr);
    void *mfn_ref = SageBuilder::buildMemberFunctionRefExp(
        static_cast<SgMemberFunctionSymbol *>(
            mfn->search_for_symbol_from_symbol_table()),
        false, false);
    auto *dot = static_cast<SgExpression *>(
        cg->CreateExp(V_SgDotExp, var_ref, mfn_ref));
    vector<SgExpression *> vec;
    SgExprListExp *args = SageBuilder::buildExprListExp(vec);
    return SageBuilder::buildFunctionCallExp(dot, args);
  }

  void lower_decl_as_if_condition(SgIfStmt *node) {
    auto var = node->get_conditional();
    void *condition_exp = cg->CreateVariableRef(var);
    void *ty = cg->GetTypebyVarRef(condition_exp);
    if (cg->IsReferenceType(ty)) {
      ty = cg->GetBaseTypeOneLayer(ty);
    }
    if (cg->IsClassType(ty) || cg->IsStructureType(ty)) {
      condition_exp = make_obj_ref_as_condition(condition_exp, ty);
    }
    cg->detachFromParent(var);
    SgStatement *condition = static_cast<SgStatement *>(
        cg->CreateStmt(V_SgExprStatement, condition_exp));
    node->set_conditional(condition);
    condition->set_parent(node);

    SgStatement *parent = static_cast<SgStatement *>(node->get_parent());
    SgStatement *block;
    if (cg->GetChildStmtNum(parent) == 1) {
      block = parent;
    } else {
      block = static_cast<SgStatement *>(cg->CreateBasicBlock());
      block->set_parent(parent);
      cg->myStatementInsert(node, block, false);
      cg->myRemoveStatement(node);
      cg->AppendChild(block, node);
    }
    cg->PrependChild(block, var);
  }

  void lower_decl_as_switch_condition(SgSwitchStatement *node) {
    auto var = node->get_item_selector();
    void *condition_exp = cg->CreateVariableRef(var);
    void *ty = cg->GetTypebyVarRef(condition_exp);
    if (cg->IsReferenceType(ty)) {
      ty = cg->GetBaseTypeOneLayer(ty);
    }
    if (cg->IsClassType(ty) || cg->IsStructureType(ty)) {
      condition_exp = make_obj_ref_as_condition(condition_exp, ty);
    }
    SgStatement *condition = static_cast<SgStatement *>(
        cg->CreateStmt(V_SgExprStatement, condition_exp));
    cg->detachFromParent(var);
    node->set_item_selector(condition);
    condition->set_parent(node);

    SgStatement *parent = static_cast<SgStatement *>(node->get_parent());
    SgStatement *block;
    if (cg->GetChildStmtNum(parent) == 1) {
      block = parent;
    } else {
      block = static_cast<SgStatement *>(cg->CreateBasicBlock());
      block->set_parent(parent);
      cg->myStatementInsert(node, block, false);
      cg->myRemoveStatement(node);
      cg->AppendChild(block, node);
    }
    cg->PrependChild(block, var);
  }

  void lower_decl_as_for_condition(SgForStatement *node) {
    auto var = node->get_test();
    void *condition = cg->CreateVariableRef(var);
    void *ty = cg->GetTypebyVarRef(condition);
    if (cg->IsReferenceType(ty)) {
      ty = cg->GetBaseTypeOneLayer(ty);
    }
    if (cg->IsClassType(ty) || cg->IsStructureType(ty)) {
      condition = make_obj_ref_as_condition(condition, ty);
    }

    auto inc_exp = node->get_increment();
    cg->detachFromParent(inc_exp);
    auto null_exp =
        static_cast<SgExpression *>(cg->CreateExp(V_SgNullExpression));
    node->set_increment(null_exp);
    null_exp->set_parent(node);
    auto true_block = node->get_loop_body();
    cg->detachFromParent(true_block);
    auto false_block = cg->CreateBasicBlock();
    cg->AppendChild(false_block, cg->CreateStmt(V_SgBreakStmt));
    void *if2 = cg->CreateIfStmt(condition, true_block, false_block);

    auto loop_block = static_cast<SgStatement *>(cg->CreateBasicBlock());
    cg->detachFromParent(var);
    cg->AppendChild(loop_block, var);
    cg->AppendChild(loop_block, if2);
    cg->AppendChild(loop_block, cg->CreateStmt(V_SgExprStatement, inc_exp));
    node->set_loop_body(loop_block);
    loop_block->set_parent(node);

    auto loop_test = static_cast<SgStatement *>(
        cg->CreateStmt(V_SgExprStatement, cg->CreateConst(1)));
    node->set_test(loop_test);
    loop_test->set_parent(node);
  }

  void lower_decl_as_while_condition(SgWhileStmt *node) {
    auto var = node->get_condition();
    void *condition = cg->CreateVariableRef(var);
    void *ty = cg->GetTypebyVarRef(condition);
    if (cg->IsReferenceType(ty)) {
      ty = cg->GetBaseTypeOneLayer(ty);
    }
    if (cg->IsClassType(ty) || cg->IsStructureType(ty)) {
      condition = make_obj_ref_as_condition(condition, ty);
    }

    auto true_block = node->get_body();
    cg->detachFromParent(true_block);
    auto false_block = cg->CreateBasicBlock();
    cg->AppendChild(false_block, cg->CreateStmt(V_SgBreakStmt));
    void *if2 = cg->CreateIfStmt(condition, true_block, false_block);

    auto loop_block = static_cast<SgStatement *>(cg->CreateBasicBlock());
    cg->detachFromParent(var);
    cg->AppendChild(loop_block, var);
    cg->AppendChild(loop_block, if2);
    node->set_body(loop_block);
    loop_block->set_parent(node);

    auto loop_test = static_cast<SgStatement *>(
        cg->CreateStmt(V_SgExprStatement, cg->CreateConst(1)));
    node->set_condition(loop_test);
    loop_test->set_parent(node);
  }

  void separate_inlined_agg_decl(SgNode *ast) {
    auto filter = [&](auto *decl) {
      if (void *fn = cg->TraceUpToFuncDecl(decl)) {
        if (cg->IsTemplateFunctionDecl(fn) ||
            cg->IsTemplateMemberFunctionDecl(fn)) {
          return false;
        }
      }
      if (void *cls = cg->TraceUpToClassDecl(decl)) {
        if (cg->IsTemplateClassDecl(cls)) {
          return false;
        }
      }
      return cg->IsVariableDecl(cg->GetParent(decl));
    };

    trav<SgEnumDeclaration>(ast,
                            [&](SgEnumDeclaration *decl) {
                              void *var = cg->GetParent(decl);
                              cg->InsertStmt(decl, var);
                              void *new_var = cg->CreateVariableDecl(
                                  decl->get_type(), cg->GetVariableName(var),
                                  nullptr, cg->GetScope(var));
                              cg->ReplaceStmt(var, new_var);
                            },
                            filter, postorder);
  }

  void lower_decl_as_condition(SgNode *ast) {
    trav<SgFunctionDefinition>(ast, [&](SgFunctionDefinition *def) {
      if (ast == nullptr) {
        return;
      }
      vector<void *> nodes;
      cg->GetNodesByType_int(def, "postorder", V_SgIfStmt, &nodes);
      for (auto &nd : nodes) {
        SgIfStmt *s = static_cast<SgIfStmt *>(nd);
        if (cg->IsVariableDecl(s->get_conditional())) {
          lower_decl_as_if_condition(s);
        }
      }

      nodes.clear();
      cg->GetNodesByType_int(def, "postorder", V_SgSwitchStatement, &nodes);
      for (auto &nd : nodes) {
        SgSwitchStatement *s = static_cast<SgSwitchStatement *>(nd);
        if (cg->IsVariableDecl(s->get_item_selector())) {
          lower_decl_as_switch_condition(s);
        }
      }

      nodes.clear();
      cg->GetNodesByType_int(def, "postorder", V_SgWhileStmt, &nodes);
      for (auto &nd : nodes) {
        SgWhileStmt *s = static_cast<SgWhileStmt *>(nd);
        if (cg->IsVariableDecl(s->get_condition())) {
          lower_decl_as_while_condition(s);
        }
      }

      nodes.clear();
      cg->GetNodesByType_int(def, "postorder", V_SgForStatement, &nodes);
      for (auto &nd : nodes) {
        SgForStatement *s = static_cast<SgForStatement *>(nd);
        if (cg->IsVariableDecl(s->get_test())) {
          lower_decl_as_for_condition(s);
        }
      }
    });
  }

  void build_cls_remap(SgNode *ast) {
    //  Currently, we only lower C++-like aggregates, i.e ones that contain
    //  member functions (TODO: or contains a static member variable). For
    //  C-like aggregates, we merely set them to e_struct type and keep the
    //  original decls/defs.

    //  First adjust the aggregate declarations.
    trav<SgClassDeclaration>(ast, [&](SgClassDeclaration *decl) {
      if (cls_remap.find(helpers::cls_symbol(decl)) != cls_remap.end()) {
        //  Was previously remapped, therefore a C++-like agg.
        if (decl->get_definition() == nullptr) {
          lower_class_decl(decl);
        }
      } else if (decl->get_class_type() == SgClassDeclaration::e_class) {
        //  It's a C-like agg. Don't touch it except to adjust it into a
        //  struct.
        decl->set_class_type(SgClassDeclaration::e_struct);
      }
    });

    //  Now adjust function definition which aggregates are C++-like.
    trav<SgClassDefinition>(ast, [&](SgClassDefinition *def) {
      using helpers::any_of;
      if (helpers::is_cpp_agg(def)) {
        //  At least one non-var member, ergo C++-like.
        lower_class_def(def);
      }
    });
  }

  void build_typedef_remap(SgNode *ast) {
    //  Now adjust the typedef declarations.
    trav<SgTypedefDeclaration>(ast, [&](SgTypedefDeclaration *decl) {
      if (typedef_remap.count(helpers::typedef_symbol(decl)) > 0)
        return;
      if (!isa<SgGlobal>(decl->get_scope())) {
        auto *newtypedef = static_cast<SgTypedefDeclaration *>(
            SageBuilder::buildTypedefDeclaration(
                cg->legalizeName(helpers::mangle(decl)),
                decl->get_type()->get_base_type(),
                isa<SgScopeStatement>(
                    static_cast<SgNode *>(cg->GetGlobal(decl)))));
        cg->InsertStmt(newtypedef, helpers::get_insert_position(decl, cg));
        typedef_remap[helpers::typedef_symbol(decl)] = newtypedef;
        if (cg->TraceUpToFuncDecl(decl)) {
          delete_now(decl);
        } else {
          delete_later(decl);
        }
      }
    });
  }

  void build_enum_remap(SgNode *ast) {
    //  Now adjust the enum declarations.
    static map<string, int> enum_suffix;
    static int suffix = 0;
    trav<SgEnumDeclaration>(
        ast,
        [&](SgEnumDeclaration *decl) {
          if (enum_remap.count(helpers::enum_symbol(decl)) > 0)
            return;
          if (!isa<SgGlobal>(decl->get_scope()) || decl->get_isScopedEnum()) {
            if (!decl->get_isUnNamed()) {
              int64_t min_enum_val = std::numeric_limits<int64_t>::max();
              int64_t max_enum_val = std::numeric_limits<int64_t>::min();
              for (auto one_enum : decl->get_enumerators()) {
                int64_t curr_val = 0;
                if (cg->IsConstantInt(cg->GetInitializer(one_enum), &curr_val,
                                      true, nullptr)) {
                  min_enum_val = min(curr_val, min_enum_val);
                  max_enum_val = max(curr_val, max_enum_val);
                }
              }
              int64_t num_enumerator = max_enum_val - min_enum_val + 1;
              string new_enum_name = cg->legalizeName(
                  SgName(decl->get_qualified_name().getString()));

              if (enum_suffix.count(new_enum_name) > 0) {
                new_enum_name += "_" + my_itoa(enum_suffix[new_enum_name]++);
              } else {
                enum_suffix[new_enum_name] = 1;
              }
              auto *new_enum_decl = static_cast<SgEnumDeclaration *>(
                  SageBuilder::buildEnumDeclaration(
                      new_enum_name,
                      isa<SgScopeStatement>(
                          static_cast<SgNode *>(cg->GetGlobal(decl)))));
              cg->InsertStmt(new_enum_decl,
                             helpers::get_insert_position(decl, cg));
              auto *dummy_max_enumerator =
                  static_cast<SgInitializedName *>(cg->CreateVariable(
                      "long", MERLIN_DUMMY_ENUM + my_itoa(suffix++)));
              dummy_max_enumerator->set_initializer(
                  SageBuilder::buildAssignInitializer(
                      static_cast<SgExpression *>(
                          cg->CreateConst(num_enumerator)),
                      static_cast<SgType *>(cg->GetTypeByString("long"))));
              dummy_max_enumerator->set_scope(
                  static_cast<SgScopeStatement *>(cg->GetScope(new_enum_decl)));
              new_enum_decl->append_enumerator(dummy_max_enumerator);

              enum_remap[helpers::enum_symbol(decl)] = new_enum_decl;
            } else {
              enum_remap[helpers::enum_symbol(decl)] = nullptr;
            }

            if (cg->TraceUpToFuncDecl(decl)) {
              delete_now(decl);
            } else {
              delete_later(decl);
            }
          }
        },
        [&](SgEnumDeclaration *decl) {
          if (void *fn = cg->TraceUpToFuncDecl(decl)) {
            if (cg->IsTemplateInstantiationFunctionDecl(fn) ||
                cg->IsTemplateInstantiationMemberFunctionDecl(fn)) {
              return true;
            }
            if (cg->IsTemplateFunctionDecl(fn) ||
                cg->IsTemplateMemberFunctionDecl(fn)) {
              return false;
            }
          }
          if (void *cls = cg->TraceUpToClassDecl(decl)) {
            if (cg->IsTemplateClassDecl(cls)) {
              return false;
            }
          }
          return true;
        },
        postorder);
  }

  void build_cls_mapping(SgNode *ast) {
    trav<SgClassDefinition>(ast, [&](SgClassDefinition *def) {
      if (SgClassDeclaration *decl = def->get_declaration()) {
        if (helpers::should_lower(decl, cg)) {
          sym_to_def.insert({helpers::cls_symbol(decl), def});
        }
      }
    });

    //  Build tcs_syms, which is needed by class de-templatization.
    trav<SgTemplateClassDeclaration>(
        ast,
        [&](SgTemplateClassDeclaration *tcs) {
          if (tcs->get_declaration_associated_with_symbol() != nullptr) {
            auto *tcssym = isa<SgTemplateClassSymbol>(
                tcs->search_for_symbol_from_symbol_table());
            DEBUG(cerr << "Found template class decl sym " << cg->_p(tcs, 40)
                       << "\n");

            tcs_syms[tcssym] = tcs;
            //  Since we're here already, schedule the template for deletion
            //  (remember, there are no templates in C).
            delete_later(tcs);
          }
        },
        [&](SgTemplateClassDeclaration *n) {
          bool used_by_kernel = true;
          if (n->isTransformation() ||
              (used_by_kernel = m_mars_ir.is_used_by_kernel(n))) {
            return true;
          }
          DEBUG(cerr << "skipping non-user-defined node " << cg->_p(n, 80)
                     << "\n\tptr: " << &n
                     << " used by kernel: " << used_by_kernel << "\n");
          return false;
        },
        preorder);
  }

  void lower_ty_refs(SgNode *node, const SgType &oldty, SgType *newty) {
    //  Filter out nodes that aren't user-defined, but accept ones that were
    //  generated by previous passes.
    DEBUG(cerr << "lower_ty_refs for " << node << "\n\toldty: " << &oldty
               << " newty: " << newty << "\n");
    if (auto *vardecl = isa<SgVariableDeclaration>(node)) {
      //  Need to do wholesale replacement.
      assert(vardecl->get_variables().size() == 1 &&
             "multiple vars in one decl?!");
      SgInitializedName &init = *vardecl->get_variables().front();
      SgName name = init.get_name();
      // if ((isa_or_null<SgConstructorInitializer>(init.get_initializer()) !=
      //     nullptr) ||
      //    name.is_null()) {
      //  // TODO(youxiang): Handle var decls without a name.
      //  return;
      //}

      assert(init.get_type());
      SgType *var_type = init.get_type();
      if (var_type == &oldty) {
        init.set_type(newty);
      } else {
        helpers::replace_type(oldty, newty, var_type, cg);
      }

    } else if (auto *fndecl = isa<SgFunctionDeclaration>(node)) {
      //  In-place type node replacement.
      assert(fndecl->get_type() && "Function without a type?");
      helpers::replace_type(oldty, newty, fndecl->get_type(), cg);
      //  Replace the types of the SgInitializedNames that are the params.
      SgInitializedNamePtrList &argz = fndecl->get_args();
      for (size_t i = 0; i < argz.size(); i += 1) {
        if (&oldty == argz[i]->get_type()) {
          argz[i]->set_type(newty);
        } else {
          helpers::replace_type(oldty, newty, argz[i]->get_type(), cg);
        }
      }
      // update firstNonDefined declaration which is bound to FunctionSymbol and
      // function call
      auto *firstNonDefinedDecl = fndecl->get_firstNondefiningDeclaration();
      if (firstNonDefinedDecl && firstNonDefinedDecl != fndecl)
        lower_ty_refs(firstNonDefinedDecl, oldty, newty);
    } else if (auto *typedecl = isa<SgTypedefDeclaration>(node)) {
      auto *base_type = typedecl->get_base_type();
      if (&oldty == base_type) {
        typedecl->set_base_type(newty);
      } else {
        helpers::replace_type(oldty, newty, base_type, cg);
      }
    } else if (auto *cast = isa<SgCastExp>(node)) {
      auto *cast_type = cast->get_type();
      if (&oldty == cast_type) {
        cast->set_type(newty);
      } else {
        helpers::replace_type(oldty, newty, cast_type, cg);
      }
    } else if (auto *szof = isa<SgSizeOfOp>(node)) {
      auto *szof_type = szof->get_operand_type();
      if (&oldty == szof_type) {
        szof->set_operand_type(newty);
      } else if (szof_type != nullptr) {
        helpers::replace_type(oldty, newty, szof_type, cg);
      }
    } else {
      //  Ignore. Don't lower.
    }
  }

  //  Traverse the AST under `ast`, calling `handler` on all nodes of type
  //  NodeTy. This will ignore nodes that aren't (transitively) used by the
  //  kernel interface function(s). Traverses in post-order.
  template <typename NodeTy, typename Fn>
  vector<NodeTy *> trav(SgNode *ast, Fn &&handler,
                        AstSimpleProcessing::Order ord = postorder) {
    return trav<NodeTy>(
        ast, std::forward<Fn>(handler),
        [&](NodeTy *n) {
          bool used_by_kernel = true;
          if (n->isTransformation() ||
              ((used_by_kernel = m_mars_ir.is_used_by_kernel(n)) &&
               helpers::should_lower(n, cg))) {
            return true;
          }
          DEBUG(cerr << "skipping non-user-defined node " << cg->_p(n, 80)
                     << "\n\tptr: " << &n
                     << " used by kernel: " << used_by_kernel << "\n");
          return false;
        },
        ord);
  }

  //  Traverse the AST under `ast` and call `handler` on all nodes of type
  //  NodeTy for which `filter(node) == true`.
  template <typename NodeTy, typename Fn, typename Gn>
  vector<NodeTy *> trav(SgNode *ast, Fn &&handler, Gn &&filter,
                        AstSimpleProcessing::Order ord) {
    //  we need to first store the set of nodes on which to call `handler`
    //  because handler could insert into the AST (which would affect
    //  traversal).
    vector<NodeTy *> nodes;
    auto filt = [&](SgNode *node) {
      if (NodeTy *n = isa<NodeTy>(node)) {
        //  This assumes that NodeTy is a subclass of SgLocatedNode.
        if (filter(n)) {
          nodes.push_back(n);
        }
      }
    };
    CSageWalker<decltype(filt)> *walker =
        new CSageWalker<decltype(filt)>(std::move(filt));
    walker->traverse(ast, ord);
    delete walker;
    for (NodeTy *node : nodes) {
      handler(node);
    }
    return nodes;
  }

  void remove_floating_refs(SgNode *ast) {
    trav<SgFunctionDeclaration>(ast, [&](SgFunctionDeclaration *decl) {
      cg->replace_floating_var_refs(decl);
      // update firstNonDefined declaration which is bound to FunctionSymbol and
      // function call
      auto *firstNonDefinedDecl = decl->get_firstNondefiningDeclaration();
      if (firstNonDefinedDecl)
        cg->replace_floating_var_refs(firstNonDefinedDecl);
    });
  }

  void add_missing_brace(SgNode *ast) {
    trav<SgFunctionDefinition>(
        ast, [&](SgFunctionDefinition *def) { cg->add_missing_brace(def); });
  }
  void lower_type_ref(SgNode *ast) {
    for (auto &pair : cls_remap) {
      const SgType &oldty = *pair.first->get_declaration()->get_type();
      SgType *newty = pair.second->get_declaration()->get_type();
      trav<SgLocatedNode>(
          ast, [&](SgLocatedNode *node) { lower_ty_refs(node, oldty, newty); });
    }
    for (auto &pair : typedef_remap) {
      const SgType &oldty = *pair.first->get_declaration()->get_type();
      SgType *newty = pair.second->get_type();
      trav<SgLocatedNode>(
          ast, [&](SgLocatedNode *node) { lower_ty_refs(node, oldty, newty); });
    }

    for (auto &pair : typedef2cls_remap) {
      const SgType &oldty = *pair.first->get_declaration()->get_type();
      SgType *newty = pair.second->get_declaration()->get_type();
      trav<SgLocatedNode>(
          ast, [&](SgLocatedNode *node) { lower_ty_refs(node, oldty, newty); });
    }

    for (auto &pair : enum_remap) {
      const SgType &oldty = *pair.first->get_declaration()->get_type();
      auto new_decl = pair.second;
      if (new_decl == nullptr)
        continue;
      SgType *newty = new_decl->get_type();
      trav<SgLocatedNode>(
          ast, [&](SgLocatedNode *node) { lower_ty_refs(node, oldty, newty); });
    }

    trav<SgEnumVal>(ast, [&](SgEnumVal *v) {
      auto decl = v->get_declaration();
      auto sym = helpers::enum_symbol(decl);
      if (enum_remap.count(sym) > 0) {
        auto new_decl = enum_remap.find(sym)->second;
        if (new_decl != nullptr) {
          cg->ReplaceExp(v, cg->CreateCastExp(cg->CreateConst(v->get_value()),
                                              new_decl->get_type()));
        } else {
          cg->ReplaceExp(v, cg->CreateConst(v->get_value()));
        }
      }
    });
  }

  LowerCpp &run_pass(SgNode *ast) {
    //  LowerCpp is structured as a collection of transformations, where each
    //  transformation performs a single traversal over the AST and lowers
    //  some particular kind of C++ construct into the C equivalent. Some
    //  transformations also behave like analysis passes and accumulate
    //  information needed by downstream transformations, so take heed of
    //  ordering.

    cout << " -- LowerCPP step 0" << endl;

    //  Build the sym_to_def mapping.
    build_cls_mapping(ast);

    cout << " -- LowerCPP step 1" << endl;

    //  Error out as early as possible if unsupported constructs are present.
    check_unsupported(ast);

    add_missing_brace(ast);

    lower_default_params(ast);
    separate_inlined_agg_decl(ast);
    lower_decl_as_condition(ast);

    cout << " -- LowerCPP step 2" << endl;

    //  Replace variable reference in array index
    remove_floating_refs(ast);

    cout << " -- LowerCPP step 3" << endl;

    //  Run passes that build `cls_remap`.
    build_cls_remap(ast);

    cout << " -- LowerCPP step 4" << endl;
    //  Run passes that build `typedef_remap`.
    build_typedef_remap(ast);

    build_enum_remap(ast);

    cout << " -- LowerCPP step 5" << endl;
    lower_static_stor_vars(ast);

    cout << " -- LowerCPP step 6" << endl;
    // Remap Variable that used on pragmas
    remap_var_used_in_pragma(ast);

    cout << " -- LowerCPP step 7" << endl;

    //  Now run passes that build `fn_remap`.
    build_fn_remap(ast);

    lower_inheritance_cast(ast);
    cout << " -- LowerCPP step 8" << endl;
    //  Run passes that make use of `fn_remap`.
    remap_fn_ref_exps(ast);

    lower_default_params(ast);
    cout << " -- LowerCPP step 4" << endl;
    //  Run passes that build `typedef_remap`.
    build_typedef_remap(ast);

    //  Run passes to lower type reference
    //  Replace all type references to lowered classes. TODO: Placing this
    //  here is an utter hack. The reason it's here is because tfn and member
    //  lowering could generate new decls whose parameters would need to be
    //  lowered; but it must happen before regular function mangling because
    //  the mangling process depends on the type of the parameters.

    cout << " -- LowerCPP step 9" << endl;
    lower_type_ref(ast);
    cout << " -- LowerCPP step 10" << endl;

    // Remap SgVarRefExps that reference old member variables.
    remap_var_refs(ast);

    cout << " -- LowerCPP step 11" << endl;

    trav<SgThisExp>(ast, [&](SgThisExp *t) { lower_this(t); }, postorder);

    cout << " -- LowerCPP step 12" << endl;

    // TODO(youxiang): update type references to old classes to their
    // replacements.

    //  Transformations finished. Delete scheduled nodes.
    for (SgStatement *stmt : del_queue) {
      delete_now(stmt);
    }

    cout << " -- LowerCPP step 13" << endl;

    // TODO(youxiang): This is another god awful hack to circumvent ROSE. The
    // goal
    //  here is to allow lowered types to have names identical to their
    //  unlowered originators. For instance, we might want to lower:
    //
    //  class Cls { unsigned u; void f() {} };
    //
    //  into:
    //
    //  struct Cls { unsigned u; };  //  also Cls
    trav<SgClassDeclaration>(ast, [&](SgClassDeclaration *decl) {
      string name = decl->get_name().getString();
      const string prefix = "_merlinc_";
      if (name.compare(0, prefix.size(), prefix) == 0) {
        string oldname = name.substr(prefix.size(), name.size());
        DEBUG(cerr << "renaming a _merlinc_: " << name << " to " << oldname
                   << "\n");
        decl->set_name(SgName(oldname));
      }
    });
    trav<SgTypedefDeclaration>(ast, [&](SgTypedefDeclaration *decl) {
      string name = decl->get_name().getString();
      const string prefix = "_merlinc_";
      if (name.compare(0, prefix.size(), prefix) == 0) {
        string oldname = name.substr(prefix.size(), name.size());
        DEBUG(cerr << "renaming a _merlinc_: " << name << " to " << oldname
                   << "\n");
        decl->set_name(SgName(oldname));
      }
    });
    trav<SgEnumDeclaration>(ast, [&](SgEnumDeclaration *decl) {
      string name = decl->get_name().getString();
      const string prefix = "_merlinc_";
      if (name.compare(0, prefix.size(), prefix) == 0) {
        string oldname = name.substr(prefix.size(), name.size());
        DEBUG(cerr << "renaming a _merlinc_: " << name << " to " << oldname
                   << "\n");
        decl->set_name(SgName(oldname));
      }
    });
    cout << " -- LowerCPP step 14" << endl;

    //  static_cast to c_style cast
    trav<SgCastExp>(ast,
                    [&](SgCastExp *old_cast_ref) {
                      if (old_cast_ref->cast_type() ==
                          SgCastExp::e_static_cast) {
                        void *cast_op = cg->GetExpUniOperand(old_cast_ref);
                        void *cast_ty = cg->GetTypeByExp(old_cast_ref);
                        void *new_cast_exp =
                            cg->CreateCastExp(cg->CopyExp(cast_op), cast_ty);
                        cg->ReplaceExp(old_cast_ref, new_cast_exp);
                      }
                    },
                    postorder);
    DEBUG(debug_print_all(ast, cerr));

    cout << " -- LowerCPP step 15" << endl;

    // finally check whether there is any error.
    // currently, we will only catch overloaded kernel functions
    if (err) {
      throw std::exception();
    }
    return *this;
  }
};

#undef MERLIN_DUMMY_ENUM
