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


#ifndef TRUNK_MARS_GEN_TOOLS_MCHECK_MDIAG_H_
#define TRUNK_MARS_GEN_TOOLS_MCHECK_MDIAG_H_

#include <memory>
#include "clang/Basic/Diagnostic.h"
#include "clang/Frontend/TextDiagnostic.h"
#include "llvm/Support/raw_ostream.h"

// This is a near-identical duplicate of TextDiagnosticPrinter and exists
// because TDP declares its TextDiag member private, which prevents us from
// using our own TextDiagnostic impl.

class MDiagPrinter : public clang::DiagnosticConsumer {
 protected:
  llvm::raw_ostream *OS;
  llvm::IntrusiveRefCntPtr<clang::DiagnosticOptions> DiagOpts;
  std::unique_ptr<clang::TextDiagnostic> TextDiag;

 public:
  MDiagPrinter(llvm::raw_ostream *OS,
               llvm::IntrusiveRefCntPtr<clang::DiagnosticOptions> DO)
      : OS(OS), DiagOpts(DO) {}
  void BeginSourceFile(const clang::LangOptions &,
                       const clang::Preprocessor *) override;
  void EndSourceFile() override;
  void HandleDiagnostic(clang::DiagnosticsEngine::Level,
                        const clang::Diagnostic &) override;
};

class MDiag : public clang::TextDiagnostic {
 protected:
  llvm::raw_ostream *OS;

 public:
  MDiag(llvm::raw_ostream *OS, const clang::LangOptions &LangOpts,
        clang::DiagnosticOptions *DiagOpts)
      : OS(OS), clang::TextDiagnostic(*OS, LangOpts, DiagOpts) {}
  virtual void emitFilename(llvm::StringRef, const clang::SourceManager &);
  void emitDiagnosticLoc(clang::FullSourceLoc, clang::PresumedLoc,
                         clang::DiagnosticsEngine::Level,
                         llvm::ArrayRef<clang::CharSourceRange>) override;
  void emitIncludeLocation(clang::FullSourceLoc, clang::PresumedLoc) override;
};

#endif  // TRUNK_MARS_GEN_TOOLS_MCHECK_MDIAG_H_
