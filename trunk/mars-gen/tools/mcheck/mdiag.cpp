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


#include <utility>
#include "mdiag.h"
#include "merlin/unmerlinprj.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Lex/Lexer.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/Path.h"

using clang::ArrayRef;
using clang::CharSourceRange;
using clang::Diagnostic;
using clang::DiagnosticIDs;
using clang::DiagnosticOptions;
using clang::DiagnosticsEngine;
using clang::DirectoryEntry;
using clang::FileEntry;
using clang::FileID;
using clang::FullSourceLoc;
using clang::LangOptions;
using clang::Lexer;
using clang::Preprocessor;
using clang::PresumedLoc;
using clang::raw_ostream;
using clang::SmallString;
using clang::SmallVector;
using clang::SourceManager;
using clang::StringRef;
using clang::TextDiagnostic;
using clang::diag::fatal_too_many_errors;

// Note: All of the MDiagPrinter impls were copy-pasted straight from
// clang::TextDiagnosticPrinter.
void MDiagPrinter::BeginSourceFile(const LangOptions &LO,
                                   const Preprocessor *PP) {
  TextDiag.reset(new MDiag(OS, LO, &*DiagOpts));
}

void MDiagPrinter::EndSourceFile() { TextDiag.reset(); }

static void printDiagnosticOptions(raw_ostream *OS,
                                   DiagnosticsEngine::Level Level,
                                   const Diagnostic &Info,
                                   const DiagnosticOptions &DiagOpts) {
  bool Started = false;
  if (DiagOpts.ShowOptionNames) {
    // Handle special cases for non-warnings early.
    if (Info.getID() == fatal_too_many_errors) {
      *OS << " [-ferror-limit=]";
      return;
    }

    // The code below is somewhat fragile because we are essentially trying
    // to report to the user what happened by inferring what the diagnostic
    // engine did. Eventually it might make more sense to have the
    // diagnostic engine include some "why" information in the diagnostic.

    // If this is a warning which has been mapped to an error by the user
    // (as inferred by checking whether the default mapping is to an error)
    // then flag it as such. Note that diagnostics could also have been
    // mapped by a pragma, but we don't currently have a way to distinguish
    // this.
    if (Level == DiagnosticsEngine::Error &&
        DiagnosticIDs::isBuiltinWarningOrExtension(Info.getID()) &&
        !DiagnosticIDs::isDefaultMappingAsError(Info.getID())) {
      *OS << " [-Werror";
      Started = true;
    }

    StringRef Opt = DiagnosticIDs::getWarningOptionForDiag(Info.getID());
    if (!Opt.empty()) {
      *OS << (Started ? "," : " [")
          << (Level == DiagnosticsEngine::Remark ? "-R" : "-W") << Opt;
      StringRef OptValue = Info.getDiags()->getFlagValue();
      if (!OptValue.empty()) {
        *OS << "=" << OptValue;
      }
      Started = true;
    }
  }

  // If the user wants to see category information, include it too.
  if (DiagOpts.ShowCategories != 0U) {
    unsigned DiagCategory =
        DiagnosticIDs::getCategoryNumberForDiag(Info.getID());
    if (DiagCategory != 0U) {
      *OS << (Started ? "," : " [");
      Started = true;
      if (DiagOpts.ShowCategories == 1U) {
        *OS << DiagCategory;
      } else {
        assert(DiagOpts.ShowCategories == 2U && "Invalid ShowCategories value");
        *OS << DiagnosticIDs::getCategoryNameFromID(DiagCategory);
      }
    }
  }
  if (Started) {
    *OS << ']';
  }
}

void MDiagPrinter::HandleDiagnostic(DiagnosticsEngine::Level Level,
                                    const Diagnostic &Info) {
  // Default implementation (Warnings/errors count).
  DiagnosticConsumer::HandleDiagnostic(Level, Info);

  // Render the diagnostic message into a temporary buffer eagerly. We'll use
  // this later as we print out the diagnostic to the terminal.
  SmallString<100> OutStr;
  Info.FormatDiagnostic(OutStr);

  llvm::raw_svector_ostream DiagMessageStream(OutStr);
  printDiagnosticOptions(&DiagMessageStream, Level, Info, *DiagOpts);

  // Keeps track of the starting position of the location
  // information (e.g., "foo.c:10:4:") that precedes the error
  // message. We use this information to determine how long the
  // file+line+column number prefix is.
  uint64_t StartOfLocationInfo = OS->tell();

  // Use a dedicated, simpler path for diagnostics without a valid location.
  // This is important as if the location is missing, we may be emitting
  // diagnostics in a context that lacks language options, a source manager,
  // or other infrastructure necessary when emitting more rich diagnostics.
  if (!Info.getLocation().isValid()) {
    TextDiagnostic::printDiagnosticLevel(*OS, Level, DiagOpts->ShowColors,
                                         DiagOpts->CLFallbackMode);
    TextDiagnostic::printDiagnosticMessage(
        *OS, Level != 0U, DiagMessageStream.str(),
        OS->tell() - StartOfLocationInfo, DiagOpts->MessageLength,
        DiagOpts->ShowColors);
    OS->flush();
    return;
  }

  // Assert that the rest of our infrastructure is setup properly.
  assert(DiagOpts && "Unexpected diagnostic without options set");
  assert(Info.hasSourceManager() &&
         "Unexpected diagnostic with no source manager");
  assert(TextDiag && "Unexpected diagnostic outside source file processing");

  TextDiag->emitDiagnostic(
      FullSourceLoc(Info.getLocation(), Info.getSourceManager()), Level,
      DiagMessageStream.str(), Info.getRanges(), Info.getFixItHints());

  OS->flush();
}

// This hacks the diagnostics printer to strip off pathnames from `Filename`,
// showing only the base filename. Needed to hack around revealing .merlin_prj.
void MDiag::emitFilename(StringRef Filename, const SourceManager &SM) {
  SmallVector<char, 128> AbsoluteFilename;
  // Check if any of the path components is .merlin_prj. If so, print only the
  // filename.
  if (std::any_of(
          llvm::sys::path::begin(Filename, llvm::sys::path::Style::native),
          llvm::sys::path::end(Filename),
          [](const StringRef p) { return p == ".merlin_prj"; })) {
    Filename = llvm::sys::path::filename(Filename);
  } else if (DiagOpts->AbsolutePath) {
    if (const DirectoryEntry *Dir = SM.getFileManager().getDirectory(
            llvm::sys::path::filename(Filename))) {
      StringRef DirName = SM.getFileManager().getCanonicalName(Dir);
      llvm::sys::path::append(AbsoluteFilename, DirName,
                              llvm::sys::path::filename(Filename));
      Filename = StringRef(AbsoluteFilename.data(), AbsoluteFilename.size());
    }
  }
  *OS << Filename;
}

void MDiag::emitIncludeLocation(FullSourceLoc Loc, PresumedLoc PL) {
  if (DiagOpts->ShowLocation && PL.isValid()) {
    *OS << "In file included from " << merlin::unmerlinprj(PL.getFilename())
        << ':' << PL.getLine() << ":\n";
  } else {
    *OS << "In included file:\n";
  }
}

void MDiag::emitDiagnosticLoc(FullSourceLoc Loc, PresumedLoc PLoc,
                              DiagnosticsEngine::Level Level,
                              ArrayRef<CharSourceRange> Ranges) {
  if (PLoc.isInvalid()) {
    // At least print the file name if available:
    FileID FID = Loc.getFileID();
    if (FID.isValid()) {
      const FileEntry *FE = Loc.getFileEntry();
      if ((FE != nullptr) && FE->isValid()) {
        emitFilename(FE->getName(), Loc.getManager());
        if (FE->isInPCH()) {
          *OS << " (in PCH)";
        }
        *OS << ": ";
      }
    }
    return;
  }
  unsigned LineNo = PLoc.getLine();

  if (!DiagOpts->ShowLocation) {
    return;
  }

  emitFilename(PLoc.getFilename(), Loc.getManager());
  switch (DiagOpts->getFormat()) {
  case DiagnosticOptions::Clang:
    *OS << ':' << LineNo;
    break;
  case DiagnosticOptions::MSVC:
    *OS << '(' << LineNo;
    break;
  case DiagnosticOptions::Vi:
    *OS << " +" << LineNo;
    break;
  }

  if (DiagOpts->ShowColumn) {
    // Compute the column number.
    if (unsigned ColNo = PLoc.getColumn()) {
      if (DiagOpts->getFormat() == DiagnosticOptions::MSVC) {
        *OS << ',';
        // Visual Studio 2010 or earlier expects column number to be off
        // by one
        if ((LangOpts.MSCompatibilityVersion != 0U) &&
            !LangOpts.isCompatibleWithMSVC(LangOptions::MSVC2012)) {
          ColNo--;
        }
      } else {
        *OS << ':';
      }
      *OS << ColNo;
    }
  }
  switch (DiagOpts->getFormat()) {
  case DiagnosticOptions::Clang:
  case DiagnosticOptions::Vi:
    *OS << ':';
    break;
  case DiagnosticOptions::MSVC:
    // MSVC2013 and before print 'file(4) : error'. MSVC2015 gets rid of the
    // space and prints 'file(4): error'.
    *OS << ')';
    if ((LangOpts.MSCompatibilityVersion != 0U) &&
        !LangOpts.isCompatibleWithMSVC(LangOptions::MSVC2015)) {
      *OS << ' ';
    }
    *OS << ": ";
    break;
  }

  if (DiagOpts->ShowSourceRanges && !Ranges.empty()) {
    FileID CaretFileID = Loc.getExpansionLoc().getFileID();
    bool PrintedRange = false;

    for (ArrayRef<CharSourceRange>::const_iterator RI = Ranges.begin(),
                                                   RE = Ranges.end();
         RI != RE; ++RI) {
      // Ignore invalid ranges.
      if (!RI->isValid()) {
        continue;
      }

      FullSourceLoc B =
          FullSourceLoc(RI->getBegin(), Loc.getManager()).getExpansionLoc();
      FullSourceLoc E =
          FullSourceLoc(RI->getEnd(), Loc.getManager()).getExpansionLoc();

      // If the End location and the start location are the same and are a
      // macro location, then the range was something that came from a
      // macro expansion or _Pragma.  If this is an object-like macro, the
      // best we can do is to highlight the range.  If this is a
      // function-like macro, we'd also like to highlight the arguments.
      if (B == E && RI->getEnd().isMacroID()) {
        E = FullSourceLoc(RI->getEnd(), Loc.getManager())
                .getExpansionRange()
                .second;
      }

      std::pair<FileID, unsigned> BInfo = B.getDecomposedLoc();
      std::pair<FileID, unsigned> EInfo = E.getDecomposedLoc();

      // If the start or end of the range is in another file, just discard
      // it.
      if (BInfo.first != CaretFileID || EInfo.first != CaretFileID) {
        continue;
      }

      // Add in the length of the token, so that we cover multi-char
      // tokens.
      unsigned TokSize = 0;
      if (RI->isTokenRange()) {
        TokSize = Lexer::MeasureTokenLength(E, E.getManager(), LangOpts);
      }

      *OS << '{' << B.getLineNumber() << ':' << B.getColumnNumber() << '-'
          << E.getLineNumber() << ':' << (E.getColumnNumber() + TokSize) << '}';
      PrintedRange = true;
    }

    if (PrintedRange) {
      *OS << ':';
    }
  }
  *OS << ' ';
}
