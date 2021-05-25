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
#include "clang/Rewrite/Core/Rewriter.h"

using clang::ASTConsumer;
using clang::ASTFrontendAction;
using clang::CharSourceRange;
using clang::CompilerInstance;
using clang::CXXMemberCallExpr;
using clang::CXXOperatorCallExpr;
using clang::DeclRefExpr;
using clang::dyn_cast;
using clang::Expr;
using clang::LangOptions;
using clang::Lexer;
using clang::MemberExpr;
using clang::QualType;
using clang::Rewriter;
using clang::SourceLocation;
using clang::SourceManager;
using clang::ValueDecl;
using clang::ast_matchers::cxxMemberCallExpr;
using clang::ast_matchers::cxxOperatorCallExpr;
using clang::ast_matchers::MatchFinder;
using clang::tooling::ClangTool;
using clang::tooling::CommonOptionsParser;
using clang::tooling::newFrontendActionFactory;
using llvm::errs;
using llvm::cl::OptionCategory;

#undef DEBUG
#ifndef NDEBUG
#define DEBUG(x) x
#else
#define DEBUG(x)
#endif
static OptionCategory MRefactorCategory("MRefactor-specific options");

static bool revert = false;

struct Refactor : public MatchFinder::MatchCallback {
  Refactor(MatchFinder &MF, Rewriter &RW) : Rewrite(RW) {
    registerMatchers(MF);
  }

  void registerMatchers(MatchFinder & /*MF*/);

  void run(const MatchFinder::MatchResult & /*Res*/) override;

  std::string get_expr_string(const CXXOperatorCallExpr *exp) {
    SourceManager &sm = Rewrite.getSourceMgr();
    const LangOptions &lopt = Rewrite.getLangOpts();
#if 1
    std::string text = Lexer::getSourceText(
        CharSourceRange::getTokenRange(exp->getSourceRange()), sm, lopt, 0);
    return text;
#else
    clang::SourceLocation b = exp->getSourceRange().getBegin(),
                          _e = exp->getSourceRange().getEnd();
    clang::SourceLocation e(clang::Lexer::getLocForEndOfToken(_e, 0, sm, lopt));
    size_t len = sm.getCharacterData(e) - sm.getCharacterData(b);

    return std::string(sm.getCharacterData(b), len);
#endif
  }

  std::string changeSubscriptIntoAt(std::string org_str) {
    int64_t i = org_str.size() - 1;
    int64_t left_bracket_c = 0;
    int64_t right_bracket_c = 0;
    int64_t left_bracket_pos = -1;
    int64_t right_bracket_pos = i;
    while (i >= 0) {
      if (org_str[i] == ']') {
        right_bracket_c++;
      }
      if (org_str[i] == '[') {
        left_bracket_c++;
      }
      if (right_bracket_c == left_bracket_c) {
        left_bracket_pos = i;
        break;
      }
      --i;
    }
    if (left_bracket_pos == -1) {
      return org_str;
    }
    return org_str.substr(0, left_bracket_pos) + ".at(" +
           org_str.substr(left_bracket_pos + 1,
                          right_bracket_pos - left_bracket_pos - 1) +
           ")";
  }

  bool supported_type(std::string /*t_str*/);

 private:
  Rewriter &Rewrite;
};

bool Refactor::supported_type(std::string t_str) {
  return t_str.find("std::string") == 0 || t_str.find("string") == 0 ||
         t_str.find("const std::string") == 0 ||
         t_str.find("const string") == 0 ||
         t_str.find("std::basic_string") == 0 ||
         t_str.find("basic_string") == 0 ||
         t_str.find("const std::basic_string") == 0 ||
         t_str.find("const basic_string") == 0 ||
         t_str.find("const std::vector<") == 0 ||
         t_str.find("std::vector<") == 0 || t_str.find("const vector<") == 0 ||
         t_str.find("vector<") == 0 || t_str.find("const std::array<") == 0 ||
         t_str.find("std::array<") == 0 || t_str.find("const array<") == 0 ||
         t_str.find("array<") == 0;
}

void Refactor::run(const MatchFinder::MatchResult &Res) {
  SourceManager &SM = Res.Context->getSourceManager();
  auto &Nodes = Res.Nodes;
  if (!revert) {
    if (const auto *CI = Nodes.getNodeAs<CXXOperatorCallExpr>("cxxop")) {
      // dbgs() << "operator " << CI->getOperator() << '\n';
      // CI->dump();
      // dbgs() << "operator[] " << clang::OO_Subscript<< '\n';
      if (CI->getOperator() == clang::OO_Subscript) {
        const Expr *arg = CI->getArg(0);
        // const Expr *index = CI->getArg(1);
        const Expr *callee = CI->getCallee();

        const ValueDecl *decl = nullptr;
        if (const DeclRefExpr *decl_ref_expr = dyn_cast<DeclRefExpr>(arg)) {
          decl = decl_ref_expr->getDecl();
        } else if (const MemberExpr *mem_expr = dyn_cast<MemberExpr>(arg)) {
          decl = mem_expr->getMemberDecl();
        }
        if (decl != nullptr) {
          QualType decl_type = decl->getType();
          std::string t_str = decl_type.getAsString();
          if (supported_type(t_str)) {
            // arg->dump(SM);
            // dbgs() << " @";
            // arg->getSourceRange().getBegin().dump(SM);
            // dbgs() << ",",
            // arg->getSourceRange().getEnd().dump(SM);
            // dbgs() << '\n';
            // index->dump(SM);
            // dbgs() << " @";
            // index->getSourceRange().getBegin().dump(SM);
            // dbgs() << ",",
            // index->getSourceRange().getEnd().dump(SM);
            // dbgs() << '\n';
            // callee->dump(SM);
            // dbgs() << " @";
            // callee->getSourceRange().getBegin().dump(SM);
            // dbgs() << ",",
            // callee->getSourceRange().getEnd().dump(SM);
            // dbgs() << '\n';
            SourceLocation left_bracket_pos =
                callee->getSourceRange().getBegin();
            SourceLocation right_bracket_pos = CI->getOperatorLoc();
            Rewrite.ReplaceText(left_bracket_pos, 1, ".at(");
            Rewrite.ReplaceText(right_bracket_pos, 1, ")");
          }
        }
      }
    }
  }
  if (revert) {
    if (const auto *CI = Nodes.getNodeAs<CXXMemberCallExpr>("cxxmemcall")) {
      auto *MemDecl = CI->getMethodDecl();
      if (MemDecl == nullptr) {
        return;
      }
      std::string mem_fn = MemDecl->getNameAsString();

      // dbgs() << "member function name " << mem_fn<< '\n';
      if ("at" == mem_fn) {
        const MemberExpr *callee = dyn_cast<MemberExpr>(CI->getCallee());
        assert(callee);
        const ValueDecl *decl = callee->getMemberDecl();
        if (decl != nullptr) {
          QualType decl_type = decl->getType();
          std::string t_str = decl_type.getAsString();
          if (supported_type(t_str)) {
            // CI->dump(SM);
            // dbgs() << " @";
            // CI->getSourceRange().getBegin().dump(SM);
            // dbgs() << ",",
            // CI->getSourceRange().getEnd().dump(SM);
            // dbgs() << '\n';
            // callee->dump(SM);
            // dbgs() << " @";
            // callee->getSourceRange().getBegin().dump(SM);
            // dbgs() << ",",
            // callee->getSourceRange().getEnd().dump(SM);
            // dbgs() << '\n';
            // dbgs() << "member info loc: \n";
            // callee->getMemberNameInfo().getLoc().dump(SM);
            // dbgs() << "\n";
            // dbgs() << "member loc: \n";
            // callee->getMemberLoc().dump(SM);
            // dbgs() << "\n";
            int num = 1;
            if (callee->isArrow()) {
              num = 2;
            }
            SourceLocation left_start_pos =
                callee->getMemberLoc().getLocWithOffset(0 - num);
            SourceLocation right_paren_pos = CI->getRParenLoc();
            Rewrite.ReplaceText(left_start_pos, 3 + num, "[");
            Rewrite.ReplaceText(right_paren_pos, 1, "]");
          }
        }
      }
    }
  }
}

void Refactor::registerMatchers(MatchFinder &MF) {
  MF.addMatcher(cxxOperatorCallExpr().bind("cxxop"), this);
  MF.addMatcher(cxxMemberCallExpr().bind("cxxmemcall"), this);
}

struct RefactorAct : public ASTFrontendAction {
  RefactorAct() : Rfc(MF, TheRewriter) {}
  void EndSourceFileAction() override {
    // TheRewriter.getEditBuffer(TheRewriter.getSourceMgr().getMainFileID())
    //  .write(llvm::outs());
    TheRewriter.overwriteChangedFiles();
  }
  std::unique_ptr<ASTConsumer>
  CreateASTConsumer(CompilerInstance &CI, StringRef /*InFile*/) override {
    TheRewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
    return MF.newASTConsumer();
  }

 private:
  MatchFinder MF;
  Refactor Rfc;
  Rewriter TheRewriter;
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
  std::vector<const char *> args;
  for (unsigned i = 0; i < argc; i += 1) {
    if (strncmp(argv[i], "--", 3) == 0) {
      dashdash = true;
    }
    if (strncmp(argv[i], "-revert", 8) == 0) {
      revert = true;
      continue;
    }
    args.push_back(argv[i]);
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
    add_include_dir(mcc_home + "/source-opt/lib/clang/lib/clang/7.0.0/include");
    add_include_dir(gcc_path + "/include/c++/4.9.4");
    add_include_dir(gcc_path + "/include/c++/4.9.4/x86_64-unknown-linux-gnu");
    add_include_dir(gcc_path + "/include/c++/4.9.4/backward");
    add_include_dir(gcc_path +
                    "/lib/gcc/x86_64-unknown-linux-gnu/4.9.4/include");
    add_include_dir(gcc_path +
                    "/lib/gcc/x86_64-unknown-linux-gnu/4.9.4/include-fixed");
    add_include_dir(gcc_path + "/include");

    std::string merlin_path(mcchome);
    add_include_dir(merlin_path + "/source-opt/tools/include");
    add_include_dir(merlin_path + "/source-opt/lib/rose/include/rose");
    add_include_dir(merlin_path + "/source-opt/lib/boost_1_53_0/include");
    add_include_dir(merlin_path + "/legacy/tools/parsers/src");
    add_include_dir(merlin_path + "/legacy/tools/ir");
    add_include_dir(merlin_path + "/mars-gen/lib/flexlm");
    add_include_dir(merlin_path + "/source-opt/lib/rapidjson");
  } else {
    errs()
        << "MRefactor expected the MERLIN_COMPILER_HOME env var to be set.\n";
    return -1;
  }

  add_arg("-DPROJDEBUG=1");
  add_arg("-std=c++14");
  if (!gcc_path.empty()) {
    add_arg("--gcc-toolchain=" + gcc_path);
  }
  for (const std::string &arg : newargs) {
    args.push_back(arg.c_str());
  }

  DEBUG({
    for (const char *arg : args)
      dbgs() << "\targ: " << arg << "\n";
  });
  argc = args.size();

  CommonOptionsParser Opts(argc, args.data(), MRefactorCategory);
  ClangTool Tool(Opts.getCompilations(), Opts.getSourcePathList());

  return Tool.run(newFrontendActionFactory<RefactorAct>().get());
}
