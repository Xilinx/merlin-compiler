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


#include "mdiag.h"
#include "clang/AST/Type.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"

using clang::ASTConsumer;
using clang::ASTFrontendAction;
using clang::CompilerInstance;
using clang::CXXNewExpr;
using clang::CXXOperatorCallExpr;
using clang::Decl;
using clang::DesignatedInitExpr;
using clang::DiagnosticOptions;
using clang::DiagnosticsEngine;
using clang::FieldDecl;
using clang::FixItHint;
using clang::FrontendOptions;
using clang::FullSourceLoc;
using clang::IndirectFieldDecl;
using clang::InputKind;
using clang::LangOptions;
using clang::MemberPointerType;
using clang::NamedDecl;
using clang::NamespaceDecl;
using clang::PredefinedExpr;
using clang::PrintingPolicy;
using clang::RecordDecl;
using clang::ReturnStmt;
using clang::SourceLocation;
using clang::SourceManager;
using clang::SourceRange;
using clang::TemplateTypeParmDecl;
using clang::TypeOfExprType;
using clang::ast_matchers::classTemplateDecl;
using clang::ast_matchers::cxxNewExpr;
using clang::ast_matchers::cxxOperatorCallExpr;
using clang::ast_matchers::designatedInitExpr;
using clang::ast_matchers::hasName;
using clang::ast_matchers::hasReturnValue;
using clang::ast_matchers::MatchFinder;
using clang::ast_matchers::namespaceDecl;
using clang::ast_matchers::predefinedExpr;
using clang::ast_matchers::returnStmt;
using clang::ast_matchers::templateTypeParmDecl;
using clang::ast_matchers::type;
using clang::ast_matchers::internal::HasNameMatcher;
using clang::ast_matchers::internal::Matcher;
using clang::ast_matchers::internal::SingleNodeMatcherInterface;
using clang::ast_matchers::internal::VariadicDynCastAllOfMatcher;
using clang::SrcMgr::C_User;
using llvm::errs;
using llvm::IntrusiveRefCntPtr;
using llvm::None;
using llvm::Optional;
using llvm::raw_string_ostream;
using llvm::Twine;
using llvm::cl::OptionCategory;

static OptionCategory MCheckCategory("MCheck-specific options");

const VariadicDynCastAllOfMatcher<Decl, IndirectFieldDecl> indFieldDecl;

struct PrettyFn : public SingleNodeMatcherInterface<PredefinedExpr> {
  bool matchesNode(const PredefinedExpr &P) const override {
    // We want to only reject non-function-scope occurrences of
    // __PRETTY_FUNCTION__. However, neither Exprs nor Stmts (which
    // PredefinedExpr subclasses) retain a pointer to their scope, so the
    // the conventional way of reaching the surrounding scope AST node is
    // via ASTContext::getParents, which requires doing an entire AST
    // traversal. So we short-cut by checking directly the evaluated string
    // literal (cf. the bottom of PredefinedExpr::ComputeName).
    //
    // Also, apparently, when inside template functions, getFunctionName
    // will return nullptr.
    return (P.getIdentType() == PredefinedExpr::PrettyFunction ||
            P.getIdentType() == PredefinedExpr::PrettyFunctionNoVirtual) &&
           ((P.getFunctionName() != nullptr) &&
            (P.getFunctionName()->getString() == "" ||
             P.getFunctionName()->getString() == "top level"));
  }

  static Matcher<PredefinedExpr> mk() {
    return Matcher<PredefinedExpr>(new PrettyFn());
  }
};

struct SynthCheck : public MatchFinder::MatchCallback {
  explicit SynthCheck(MatchFinder *MF) { registerMatchers(*MF); }

  void registerMatchers(MatchFinder & /*MF*/);

  void run(const MatchFinder::MatchResult & /*Res*/) override;

  // We only want to emit at most one error per user source location.
  Optional<FullSourceLoc> firstErrorHere(const SourceLocation &Loc,
                                         const SourceManager &SM) {
    // Walks up the include stack for F.
    auto rec = [&](const FullSourceLoc &F) {
      FullSourceLoc rv(F.getPresumedLoc().getIncludeLoc(), SM);
      return rv;
    };

    // Try to figure out where in the user's code that this error
    // originated.
    FullSourceLoc UserLoc(Loc, SM);
    for (FullSourceLoc G = rec(UserLoc); G.getPresumedLoc().isValid();
         G = rec(G)) {
      if (SM.getFileCharacteristic(UserLoc) == C_User) {
        break;
      }
      UserLoc = G;
    }

    if (AlreadyErrored.count(UserLoc) == 0U) {
      AlreadyErrored.insert(UserLoc);
      return UserLoc;
    }
    return None;
  }

  template <unsigned N>
  bool errGeneric(SourceLocation Loc, const char (&Msg)[N],
                  DiagnosticsEngine *DE, const SourceManager &SM) {
    // Loc.dump(SM);
    if (SM.isInSystemHeader(Loc))
      return false;
    if (auto UserLoc = firstErrorHere(Loc, SM)) {
      const unsigned ID =
          DE->getCustomDiagID(clang::DiagnosticsEngine::Error, Msg);
      DE->Report(*UserLoc, ID);
    }
    return true;
  }

  void errNamespaceStd(const NamespaceDecl &NS, DiagnosticsEngine *DE,
                       const SourceManager &SM) {
    if (auto UserLoc = firstErrorHere(NS.getLocStart(), SM)) {
      const unsigned ID = DE->getCustomDiagID(DiagnosticsEngine::Error,
                                              "namespace std is unsupported");
      DE->Report(NS.getLocStart(), ID);

      const unsigned RID = DE->getCustomDiagID(
          DiagnosticsEngine::Remark,
          "perhaps a standard library header was included?");
      DE->Report(*UserLoc, RID)
          .AddFixItHint(FixItHint::CreateRemoval(SourceRange(*UserLoc)));
    }
  }

 private:
  std::set<FullSourceLoc> AlreadyErrored;
};

void SynthCheck::run(const MatchFinder::MatchResult &Res) {
  DiagnosticsEngine &DE = Res.Context->getDiagnostics();
  SourceManager &SM = Res.Context->getSourceManager();
  auto &Nodes = Res.Nodes;
  /*
  if (const auto *N = Nodes.getNodeAs<NamespaceDecl>("std_ns")) {
    errNamespaceStd(*N, &DE, SM);
  } else
  */
  if (const auto *N = Nodes.getNodeAs<CXXNewExpr>("opnew")) {
    if (N->isArray()) {
      const char Msg[] = "direct return of operator new[] expressions is "
                         "currently unsupported.";
      DE.Report(N->getLocStart(),
                DE.getCustomDiagID(clang::DiagnosticsEngine::Error, Msg));

      // Print a fix-it suggestion.
      const auto *Ret = Nodes.getNodeAs<ReturnStmt>("ret");
      std::string Repl;
      raw_string_ostream OS(Repl);
      OS << "auto *Ret = ";
      N->printPretty(OS, nullptr, PrintingPolicy(LangOptions()));
      OS << "; return Ret;";
      const unsigned RID =
          DE.getCustomDiagID(clang::DiagnosticsEngine::Remark,
                             "perhaps break it into two statements");
      DE.Report(Ret->getLocStart(), RID)
          .AddFixItHint(
              FixItHint::CreateReplacement(Ret->getSourceRange(), OS.str()));
    }
  } else if (const auto *T = Nodes.getNodeAs<TypeOfExprType>("tynode")) {
    if (auto T_expr = T->getUnderlyingExpr()) {
      if (!errGeneric(T_expr->getLocStart(),
                      "GCC extension `typeof` is unsupported", &DE, SM))
        return;
    }
  } else if (const auto *T =
                 Nodes.getNodeAs<DesignatedInitExpr>("designinit")) {
    if (!errGeneric(T->getLocStart(),
                    "designated initialized expression is unsupported", &DE,
                    SM))
      return;
  } else if (const auto *T =
                 Nodes.getNodeAs<TemplateTypeParmDecl>("tptyparam")) {
    if (T->isParameterPack()) {
      if (!errGeneric(T->getLocStart(), "variadic templates is unsupported",
                      &DE, SM))
        return;
    }
  } else if (const auto *T = Nodes.getNodeAs<MemberPointerType>("tynode")) {
    if (auto T_decl = T->getClass()->getAsCXXRecordDecl()) {
      if (!errGeneric(T_decl->getLocStart(),
                      "Member pointer types cannot be accepted", &DE, SM))
        return;
    }
  } else if (const auto *IFD = Nodes.getNodeAs<IndirectFieldDecl>("indfield")) {
    if (!errGeneric(IFD->getLocStart(), "indirect field decls are unsupported",
                    &DE, SM))
      return;

    // Emit a hint to manually unpack the anon field, if possible.
    if (const FieldDecl *F = IFD->getAnonField()) {
      if (const RecordDecl *R = F->getParent()) {
        const unsigned RID = DE.getCustomDiagID(
            clang::DiagnosticsEngine::Remark,
            "consider unpacking this record into the parent scope");
        DE.Report(R->getLocStart(), RID)
            .AddFixItHint(FixItHint::CreateRemoval(R->getSourceRange()));
      }
    }
  } else if (const auto *P = Nodes.getNodeAs<PredefinedExpr>("pretty")) {
    if (!errGeneric(P->getLocStart(), "this magic is unsupported", &DE, SM))
      return;
    // Drop a hint.
    const unsigned RID = DE.getCustomDiagID(clang::DiagnosticsEngine::Remark,
                                            "perhaps replace with a literal");
    Twine PVal = "\"" + P->getFunctionName()->getString() + "\"";
    DE.Report(P->getLocStart(), RID)
        .AddFixItHint(
            FixItHint::CreateReplacement(P->getSourceRange(), PVal.str()));
  }
  // else if (const auto *TC = Nodes.getNodeAs<ClassTemplateDecl>("reservap"))
  // { errGeneric(TC->getLocStart(), "Xilinx arbitrary precision types are
  // currently unsupported", DE, SM);
  // }
}

void SynthCheck::registerMatchers(MatchFinder &MF) {
  // We don't permit std lib includes.
  MF.addMatcher(namespaceDecl(hasName("std")).bind("std_ns"), this);

  // bug1778: ROSE's will erroneously unparse returned operator new
  // expressions.
  MF.addMatcher(
      returnStmt(hasReturnValue(cxxNewExpr().bind("opnew"))).bind("ret"), this);

  // bug1778: ROSE's codegen  desugars __typeof__ into typeof, which causes
  // problems with clang/g++. Note that typeof is a GCC extension.
  MF.addMatcher(type().bind("tynode"), this);

  MF.addMatcher(indFieldDecl().bind("indfield"), this);

  MF.addMatcher(predefinedExpr(PrettyFn::mk()).bind("pretty"), this);

  MF.addMatcher(designatedInitExpr().bind("designinit"), this);

  MF.addMatcher(templateTypeParmDecl().bind("tptyparam"), this);

  // MER-1434: ROSE will erroneously unparse operator ~ expression
  MF.addMatcher(cxxOperatorCallExpr().bind("operator_call"), this);
}

struct CheckerAct : public ASTFrontendAction {
  CheckerAct() : Chk(&MF) {}

  std::unique_ptr<ASTConsumer>
  CreateASTConsumer(CompilerInstance & /*CI*/, StringRef /*InFile*/) override {
    return MF.newASTConsumer();
  }

  MatchFinder MF;
  SynthCheck Chk;
};

int main(int argc, const char **argv) {
  std::vector<std::string> newargs;

  auto add_arg = [&](std::string s) {
    if (s.back() != 0) {
      s.push_back('\0');
    }
    newargs.push_back(std::move(s));
  };

  auto add_include_dir = [&](std::string dir) {
    add_arg("-I");
    add_arg(dir);
  };

  bool dashdash = false;
  for (unsigned i = 0; i < argc; i += 1) {
    if (strncmp(argv[i], "--", 3) == 0) {
      dashdash = true;
      break;
    }
  }

  if (!dashdash) {
    add_arg("--");
  }
  std::string gcc_path;
  // Add a base set of include directories. From CSageCodeGen::OpenSourceFile.
  // TODO(youxiang): This block and the one in OpenSourceFile should be factored
  // out.
  if (const char *mcchome = getenv("MERLIN_COMPILER_HOME")) {
    std::string mcc_home(mcchome);
    gcc_path = mcc_home + "/source-opt/lib/gcc4.9.4";
    add_include_dir(std::string(mcchome) +
                    "/source-opt/lib/clang/lib/clang/7.0.0/include");
    add_include_dir(gcc_path + "/include/c++/4.9.4");
    add_include_dir(gcc_path + "/include/c++/4.9.4/x86_64-unknown-linux-gnu");
    add_include_dir(gcc_path + "/include/c++/4.9.4/backward");
    add_include_dir(gcc_path +
                    "/lib/gcc/x86_64-unknown-linux-gnu/4.9.4/include");
    add_include_dir(gcc_path +
                    "/lib/gcc/x86_64-unknown-linux-gnu/4.9.4/include-fixed");
    add_include_dir(gcc_path + "/include");
  } else {
    errs() << "MCheck expected the MERLIN_COMPILER_HOME env var to be set.\n";
    return -1;
  }

  std::vector<const char *> args{argv, argv + argc};
  for (const std::string &arg : newargs) {
    args.push_back(arg.c_str());
  }

  DEBUG({
    for (const char *arg : args)
      dbgs() << "\targ: " << arg << "\n";
  });
  argc = args.size();

  clang::tooling::CommonOptionsParser Opts(argc, args.data(), MCheckCategory);
  clang::tooling::ClangTool Tool(Opts.getCompilations(),
                                 Opts.getSourcePathList());
  Tool.appendArgumentsAdjuster(
      [=](const clang::tooling::CommandLineArguments &Args,
          StringRef Filename) {
        clang::tooling::CommandLineArguments Ret = Args;
        InputKind IK = FrontendOptions::getInputKindForExtension(
            Filename.rsplit('.').second);
        if (IK.getLanguage() == InputKind::CXX) {
          Ret.push_back("-std=c++03");
          Ret.push_back("-fno-exceptions");
          // FIXME: we can support part of c++11 extensions, such as "long long"
          // , default template arguments in template functions which is common
          // in Xilinx vendor library. But we need to filter out the other
          // parts. The following is all the lists
          // warning: alias declarations are a C++11 extension
          // warning: implicit conversion from array size expression of type A
          // to integral enumeration type C is a C++11 extension
          // warning: 'auto' type specifier is a C++11 extension
          // warning: enumeration types with a fixed underlying type are a C++11
          // extension
          // warning: defaulted deleted function definitions are a C++11
          // extension
          // warning: befriending enumeration type A is a C++11 extension
          // warning: commas at the end of enumerator lists are a C++11
          // extension
          // warning: explicit conversion functions are a C++11 extension
          // warning: extern templates are a C++11 extension
          // warning: range-based for loop is a C++11 extension
          // warning: generalized initializer lists are a C++11 extension
          // warning: use of enumeration in a nested name specifier is a C++11
          // extension
          // warning: non-class friend type A is a C++11 extension
          // warning: in-class initialization of non-static data member is a
          // C++11 extension
          // warning: 'A' keyword is a C++11 extension
          // warning: reference qualifiers on functions are a C++11 extension
          // warning: rvalue references are a C++11 extension
          // warning: scoped enumerations are a C++11 extension
          // warning: static data member A in union is a C++11 extension
          // warning: non-type template argument referring to function object
          // B with internal linkage is a C++11 extension
          // warning: 'template' keyword outside of a template
          // warning: default template arguments for a function template are a
          // C++11 extension
          // warning: 'typename' occurs outside of a template
          // warning: unelaborated friend declaration is a C++11 extension;
          // specify 'struct/interface/union/class/enum' to befriend B
          // warning: variadic templates are a C++11 extension

          // Ret.push_back("-Werror-c++11-extensions");
          // Ret.push_back("-Wno-c++11-long-long");
          // Ret.push_back("-Wno-c++11-extra-semi");
        } else {
          // TODO(youxiang) shall we add standard for C input?
        }
        if (!gcc_path.empty()) {
          Ret.push_back("--gcc-toolchain=" + gcc_path);
        }
        return Ret;
      });

  IntrusiveRefCntPtr<DiagnosticOptions> DO(new DiagnosticOptions());
  MDiagPrinter MDP(&errs(), DO);
  Tool.setDiagnosticConsumer(&MDP);

  return Tool.run(clang::tooling::newFrontendActionFactory<CheckerAct>().get());
}
