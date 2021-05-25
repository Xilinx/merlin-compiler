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


#include <vector>
#include <iostream>
#include <unordered_set>

#include "interface_transform.h"
#include "mars_message.h"

using std::cout;
using std::unordered_set;
using std::vector;

unordered_set<string> C_library_functions = {
    "isalnum", "isalpha", "isblank", "iscntrl", "isdigit", "isgraph", "islower",
    "isprint", "ispunct", "isspace", "issuper", "isxdigit", "toupper",
    "tolower",  // cctype
    "errno",    // cerrno
    "feclearexcept", "feraiseexcept", "fegetexceptflag", "fesetexceptflag",
    "fegetround", "fesetround", "fegetenv", "fesetenv", "feholdexcept",
    "feupdateenv", "fetestexcept",  // cfenv
    "setlocale", "localeconv",      // clocale
    "longjump", "setjmp",           // csetjump
    "signal", "raise",              // csignal
    "offsetof",                     // cstddef
    "atof", "atoi", "atol", "atoll", "strtod", "strtof", "strtol", "strtold",
    "strtoll", "strtoul", "strtoull", "rand", "srand", "bsearch", "qsort",
    "mblen", "mbtowc", "wctomb", "mbstowcs", "wcstombs", "calloc", "free",
    "malloc", "realloc",
    // cstdlib
    "memmove", "strcpy", "strncpy", "strcat", "strncat", "memcmp", "strcmp",
    "strcoll", "strncmp", "strxfrm", "memchr", "strchr", "strcspn", "strpbrk",
    "strrchr", "strspn", "strstr", "strtok", "memset", "strerror",
    "strlen",  // cstring
    "clock", "difftime", "mktime", "time", "asctime", "ctime", "gmtime",
    "localtime",
    "strftime",  // ctime
    "c16rtomb", "c32rtomb", "mbrtoc16", "mbrtoc32",
    // cuchar
    "fgetwc", "fgetws", "fputwc", "fputws", "fwide", "fwprintf", "fwscanf",
    "getwc", "getwchar", "putwc", "putwchar", "swprintf", "swscanf", "ungetwc",
    "vfwprintf", "vfwscanf", "vswprintf", "vswscanf", "vwprintf", "vwscanf",
    "wprintf", "wscanf", "wcstod", "wcstof", "wcstol", "wcstold", "wcstoll",
    "wcstoul", "wcstoull", "btowc", "mbrlen", "mbrtowc", "mbsinit", "mbsrtowcs",
    "wcrtomb", "wctob", "wcsrtombs", "wcscat", "wcschr", "wcscmp", "wcscoll",
    "wcscpy", "wcscspn", "wcslen", "wcsncat", "wcsncmp", "wcsncpy", "wcspbrk",
    "wcsrchr", "wcsspn", "wcsstr", "wcstok", "wcsxfrm", "wmemchr", "wmemcmp",
    "wmemcpy", "wmemmove", "wmemset", "wcsftime",  // cwchar
    "remove", "rename", "tmpfile", "tmpnam", "setbuf", "setvbuf", "fgetc",
    "fgets", "getc", "getchar", "gets", "ungetc", "fread", "fwrite", "fgetpos",
    "fseek", "fsetpos", "ftell", "rewind", "clearerr", "feof", "ferror",
    "perror"  // cstdio
};

unordered_set<string> C_Synthesizable_lib_functions = {
    "imaxabs",   "imaxdiv",    "strtoimax", "strtoumax",  "wcstoimax",
    "wcstoumax",  // cinttype
    "cos",       "sin",        "tan",       "acos",       "asin",
    "atan",      "atan2",      "cosh",      "sinh",       "tanh",
    "acosh",     "asinh",      "atanh",     "exp",        "frexp",
    "ldexp",     "log",        "log10",     "modf",       "exp2",
    "expm1",     "ilogb",      "log1p",     "log2",       "logb",
    "scalbn",    "scalbln",    "pow",       "sqrt",       "cbrt",
    "hypot",     "erf ",       "erfc",      "tgamma",     "lgamma",
    "ceil",      "floor",      "fmod",      "trunc",      "round",
    "lround",    "llround",    "rint",      "lrint",      "llrint",
    "nearbyint", "remainder",  "remquo",    "copysign",   "nan",
    "nextafter", "nexttoward", "fdim",      "fmax",       "fmin",
    "fabs",      "abs",        "fma",       "fpclassify", "isfinite",
    "isinf",     "isnan",      "isnormal",  "signbit",  // cmath
    "fclose",    "fflush",     "fopen",     "freopen",    "fprintf",
    "fscanf",    "printf",     "scanf",     "snprintf",   "sprintf",
    "sscanf",    "vfprintf",   "vfscanf",   "vprintf",    "vscanf",
    "vsnprintf", "vsprintf",   "vsscanf",   "fputc",      "fputs",
    "putc",      "putchar",    "puts",  // cstdio
    "memcpy",    "abort",      "atexit",    "exit",       "at_quick_exit",
    "getenv",    "quick_exit", "system",    "_Exit"  // cstdlib
};

bool IsInternal(const string &fn_name) {
  return fn_name.find("__merlin") == 0 || fn_name.find("__assert") == 0 ||
         fn_name.find("merlin_") == 0 || fn_name.find("__builtin") == 0;
}

bool DetectLibraryCalls(CMarsIrV2 *p_mars_ir, CSageCodeGen *codegen,
                        void *func_decl) {
  vector<void *> calls;
  bool all_valid = true;
  codegen->GetNodesByType(func_decl, "preorder", "SgFunctionCallExp", &calls,
                          true, true);
  for (auto &call : calls) {
    if (codegen->IsFunctionPointerCall(call) != 0) {
      continue;
    }
    if (codegen->IsSupportedMerlinSystemFunctionCall(call))
      continue;
    void *fn_decl = codegen->GetFuncDeclByCall(call);
    string fn_name = codegen->GetFuncNameByCall(call);
    if (fn_decl == nullptr) {
      fn_decl = codegen->GetFuncDeclByCall(call, 0);
      //  skip assignment operator and constructor
      if (codegen->IsConstructor(fn_decl) || codegen->IsAssignOperator(fn_decl))
        continue;
      if (codegen->IsXilinxIntrinsic(fn_decl)) {
        continue;
      }
      if (codegen->IsSystemFunction(fn_decl) &&
          C_library_functions.count(fn_name)) {
        string error_info = codegen->GetFuncNameByCall(call) + " " +
                            getUserCodeFileLocation(codegen, call, true);
        dump_critical_message(SYNCHK_ERROR_33(error_info));
      } else {
        void *fn_decl_only = codegen->GetFuncDeclByCall(call, 0);
        string root_name = fn_name;
        if (root_name.back() == 'l' || root_name.back() == 'f')
          root_name.pop_back();
        if (fn_decl_only != nullptr &&
            codegen->IsSystemFunction(fn_decl_only) &&
            (C_Synthesizable_lib_functions.count(fn_name) ||
             C_Synthesizable_lib_functions.count(root_name))) {
          continue;
        }
        if (IsInternal(fn_name))
          continue;
        if (codegen->IsSystemFunction(fn_decl) == 0) {
          if (codegen->IsTemplateInstantiationMemberFunctionDecl(fn_decl) !=
              0) {
            void *tf = codegen->GetTemplateMemFuncDecl(fn_decl);
            if ((tf != nullptr) && (isSgTemplateMemberFunctionDeclaration(
                                        static_cast<SgNode *>(tf))
                                        ->get_definition() != nullptr)) {
              continue;
            }
          }
          string func_info = "\'" + codegen->DemangleName(fn_name) + "\' ";
          if (codegen->IsCompilerGenerated(call)) {
            auto *first_non_compiler_exp = call;
            while (codegen->IsCompilerGenerated(first_non_compiler_exp)) {
              first_non_compiler_exp =
                  codegen->GetParent(first_non_compiler_exp);
            }
            func_info +=
                getUserCodeFileLocation(codegen, first_non_compiler_exp, true);
          } else {
            func_info += getUserCodeFileLocation(codegen, call, true);
          }
          string decl_info;
          if (codegen->IsMemberFunction(fn_decl)) {
            if (codegen->IsAssignOperator(fn_decl) ||
                codegen->IsConstructor(fn_decl)) {
              // skip assign operator and constructor
              continue;
            }
            auto *m_fn_decl =
                static_cast<SgMemberFunctionDeclaration *>(fn_decl);
            SgClassDeclaration *cls_decl = static_cast<SgClassDeclaration *>(
                m_fn_decl->get_associatedClassDeclaration());
            string cls_name = cls_decl->get_name().getString();
            decl_info = "\'" + cls_name + "\' " +
                        getUserCodeFileLocation(codegen, cls_decl, true);
            dump_critical_message(SYNCHK_ERROR_17(func_info, decl_info));
            continue;
          }
        }
        auto info = getUserCodeInfo(codegen, call);
        string error_msg = "'";
        error_msg += info.name.empty() ? codegen->_up(call) : info.name;
        error_msg += "'";
        error_msg += " (" + info.file_location + ")";
        dump_critical_message(SYNCHK_ERROR_34(error_msg));
      }
      all_valid = false;
    }
  }
  return all_valid;
}

int DetectLibraryCallsTop(CSageCodeGen *codegen, void *pTopFunc,
                          const CInputOptions &options) {
  CMarsIrV2 mars_ir;
  mars_ir.build_mars_ir(codegen, pTopFunc);

  auto kernels = mars_ir.get_top_kernels();
  bool all_valid = true;
  for (auto kernel : kernels) {
    all_valid &= DetectLibraryCalls(&mars_ir, codegen, kernel);
  }
  if (!all_valid) {
    throw std::exception();
  }
  return 0;
}
