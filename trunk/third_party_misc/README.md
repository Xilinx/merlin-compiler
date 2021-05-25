1. file 0001-Our-own-patch-to-v0.11.4.14.patch:
  it is the patch applied rose v0.11.4.14 version. When we try to upgrade the
  ROSE library in the future, we need to try to apply this patch before building
  new ROSE library
2. file llvm-3.1.patch:
  it is the patch applied llvm (https://github.com/llvm-mirror/llvm) branch
  (release_31). It is used to build mars-gen/tools/llvm/opt which is used by
  report system to generate a json file describing loop locations. Before
  applying the patch, please separate
  llvm-3.1/lib/Analysis/DbgInfoPrinter_merlin.cpp from the whole patch and
  create a new file llvm-3.1/lib/Analysis/DbgInfoPrinter_merlin.cpp
3. file build-rose
  it is build script for ROSE library 
