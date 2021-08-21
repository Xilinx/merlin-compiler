#ifndef UNMERLINPRJ_H
#define UNMERLINPRJ_H

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Path.h"

namespace merlin {

using namespace llvm;

static bool containsMerlinprj(StringRef P) {
  return std::any_of(sys::path::begin(P, sys::path::Style::native),
                     sys::path::end(P),
                     [](const StringRef p) { return p == ".merlin_prj"; });
}

// Checks if any of the path components is `.merlin_prj`. If so, return a
// StringRef containing just the filename. Otherwise, return the first param.
static StringRef unmerlinprj(StringRef Filepath) {
  return containsMerlinprj(Filepath) ? sys::path::filename(Filepath) : Filepath;
}

} // namespace merlin

#endif
