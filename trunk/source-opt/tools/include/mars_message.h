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
#include "mars_opt.h"
#define MARS_INFO(pass, msg, num) XTR(pass), XTR(INFO), msg, (num) + 100
#define MARS_WARNING(pass, msg, num) XTR(pass), WARNING, msg, (num) + 200
#define MARS_ERROR(pass, msg, num) XTR(pass), XTR(ERROR), msg, (num) + 300
#define MULTIPLE_FUNCITON_CALLS                                                \
  ".\n  Hint: the following statement should contain only one function call"
#define LONG_BURST_HINT                                                        \
  "  Hint: 1. tile large loops into smaller ones to adjust the burst buffer "  \
  "size\n"                                                                     \
  "        2. specify the burst length threshold by adding interface "         \
  "attribute \n"                                                               \
  "        \'max_burst_size=?\'\n"                                             \
  "        3. decrease the single resource threshold by using \n"              \
  "        \'--attribute burst_single_size_threshold=?\'\n"                    \
  "        4. make range analysis accurate by changing \n"                     \
  "         pointer parameter into array parameter in sub function"

#define SHORT_BURST_HINT                                                       \
  "  Hint: 1. tile large loops into smaller ones to adjust the burst buffer "  \
  "size\n"                                                                     \
  "        2. specify the burst length threshold by adding interface "         \
  "attribute \n"                                                               \
  "        \'max_burst_size=?\'\n"                                             \
  "        3. increase the single resource threshold by using \n"              \
  "        \'--attribute burst_single_size_threshold=?\'\n"                    \
  "        4. make range analysis accurate by changing \n"                     \
  "         pointer parameter into array parameter in sub function"
#define INCREASE_TILING_FATOR "  Hint: increase tile factor"
#define DECREASE_TILING_FATOR "  Hint: decrease tile factor"
#define PROCS_INFO(num, msg) MARS_INFO(PROCS, msg, num)
#define PROCS_WARNING(num, msg) MARS_WARNING(PROCS, msg, num)
#define PROCS_ERROR(num, msg) MARS_ERROR(PROCS, msg, num)
#define MDARR_INFO(num, msg) MARS_INFO(MDARR, msg, num)
#define MDARR_WARNING(num, msg) MARS_WARNING(MDARR, msg, num)
#define MDARR_ERROR(num, msg) MARS_ERROR(MDARR, msg, num)
#define SYNCHK_WARNING(num, msg) MARS_WARNING(SYNCHK, msg, num)
#define SYNCHK_ERROR(num, msg) MARS_ERROR(SYNCHK, msg, num)
#define BURST_INFO(num, msg) MARS_INFO(BURST, msg, num)
#define BURST_WARNING(num, msg) MARS_WARNING(BURST, msg, num)
#define BURST_ERROR(num, msg) MARS_ERROR(BURST, msg, num)
#define DISAG_INFO(num, msg) MARS_INFO(DISAG, msg, num)
#define DISAG_WARNING(num, msg) MARS_WARNING(DISAG, msg, num)
#define DISAG_ERROR(num, msg) MARS_ERROR(DISAG, msg, num)
#define GLOBL_INFO(num, msg) MARS_INFO(GLOBL, msg, num)
#define GLOBL_WARNING(num, msg) MARS_WARNING(GLOBL, msg, num)
#define GLOBL_ERROR(num, msg) MARS_ERROR(GLOBL, msg, num)
#define FUCIN_INFO(num, msg) MARS_INFO(FUCIN, msg, num)
#define FUCIN_WARNING(num, msg) MARS_WARNING(FUCIN, msg, num)
#define FUCIN_ERROR(num, msg) MARS_ERROR(FUCIN, msg, num)
#define INFTF_ERROR(num, msg) MARS_ERROR(INFTF, msg, num)
#define INFTF_WARNING(num, msg) MARS_WARNING(INFTF, msg, num)
#define PRAGM_ERROR(num, msg) MARS_ERROR(PRAGM, msg, num)
#define KWRAP_INFO(num, msg) MARS_INFO(KWRAP, msg, num)
#define KWRAP_WARNING(num, msg) MARS_WARNING(KWRAP, msg, num)
#define KWRAP_ERROR(num, msg) MARS_ERROR(KWRAP, msg, num)
#define WDBUS_INFO(num, msg) MARS_INFO(WDBUS, msg, num)
#define WDBUS_WARNING(num, msg) MARS_WARNING(WDBUS, msg, num)
#define WDBUS_ERROR(num, msg) MARS_ERROR(WDBUS, msg, num)
#define REDUC_INFO(num, msg) MARS_INFO(REDUC, msg, num)
#define REDUC_WARNING(num, msg) MARS_WARNING(REDUC, msg, num)
#define REDUC_ERROR(num, msg) MARS_ERROR(REDUC, msg, num)
#define LINEBUF_INFO(num, msg) MARS_INFO(LINEBUF, msg, num)
#define LINEBUF_WARNING(num, msg) MARS_WARNING(LINEBUF, msg, num)
#define LINEBUF_ERROR(num, msg) MARS_ERROR(LINEBUF, msg, num)
#define CGPIP_INFO(num, msg) MARS_INFO(CGPIP, msg, num)
#define CGPIP_WARNING(num, msg) MARS_WARNING(CGPIP, msg, num)
#define CGPIP_ERROR(num, msg) MARS_ERROR(CGPIP, msg, num)
#define CGPAR_INFO(num, msg) MARS_INFO(CGPAR, msg, num)
#define CGPAR_WARNING(num, msg) MARS_WARNING(CGPAR, msg, num)
#define CGPAR_ERROR(num, msg) MARS_ERROR(CGPAR, msg, num)
#define TILE_INFO(num, msg) MARS_INFO(TILE, msg, num)
#define TILE_WARNING(num, msg) MARS_WARNING(TILE, msg, num)
#define TILE_ERROR(num, msg) MARS_ERROR(TILE, msg, num)
#define SEPAR_ERROR(num, msg) MARS_ERROR(SEPAR, msg, num)
#define SEPAR_WARNING(num, msg) MARS_WARNING(SEPAR, msg, num)
#define AST_IF_INFO(num, msg) MARS_INFO(AST_IF, msg, num)
#define AST_IF_WARNING(num, msg) MARS_WARNING(AST_IF, msg, num)
#define AST_IF_ERROR(num, msg) MARS_ERROR(AST_IF, msg, num)
#define FGPIP_INFO(num, msg) MARS_INFO(FGPIP, msg, num)
#define FGPIP_WARNING(num, msg) MARS_WARNING(FGPIP, msg, num)
#define AUREG_WARNING(num, msg) MARS_WARNING(AUREG, msg, num)
#define FGPIP_ERROR(num, msg) MARS_ERROR(FGPIP, msg, num)
#define MSG_RPT_INFO(num, msg) MARS_INFO(MSG_RPT, msg, num)
#define MSG_RPT_WARNING(num, msg) MARS_WARNING(MSG_RPT, msg, num)
#define MSG_RPT_ERROR(num, msg) MARS_ERROR(MSG_RPT, msg, num)
#define LWCPP_INFO(num, msg) MARS_INFO(LWCPP, msg, num)
#define LWCPP_WARNING(num, msg) MARS_WARNING(LWCPP, msg, num)
#define LWCPP_ERROR(num, msg) MARS_ERROR(LWCPP, msg, num)

//  ////////////////////////////////////////////  /
//  All Error out messages (limitations)
//  ////////////////////////////////////////////  /
#define LWCPP_ERROR_1(x) LWCPP_ERROR(1, (x))
#define LWCPP_ERROR_2(x) LWCPP_ERROR(2, (x))
#define LWCPP_ERROR_3(x) LWCPP_ERROR(3, (x))
#define LWCPP_WARNING_1(x) LWCPP_WARNING(1, (x))
#define LWCPP_INFO_2(x) LWCPP_INFO(2, (x))
#define PROCS_ERROR_3(x)                                                       \
  PROCS_ERROR(3, "Found a variable " + (x) +                                   \
                     " with C++ reference type in the kernel")
#define PROCS_ERROR_4(x)                                                       \
  PROCS_ERROR(4, "Found an class memeber variable " + (x) + "in the kernel")
#define PROCS_ERROR_6 PROCS_ERROR(6, "Multiple tasks are not supported")
#define PROCS_ERROR_7(x)                                                       \
  PROCS_ERROR(7, "Detecting an unsupported design: "                           \
                 "the kernel function and all the functions used in the "      \
                 "kernel must be in the same source file."                     \
                 " The following functions are not in the same file: \n" +     \
                     (x))
#define PROCS_ERROR_8(x)                                                       \
  PROCS_ERROR(8,                                                               \
              "Kernel function declaration cannot be in header files\n" + (x))
#define PROCS_ERROR_11(x)                                                      \
  PROCS_ERROR(11, "Found a parameter " + (x) +                                 \
                      " whose type is reference in the kernel")
#define PROCS_ERROR_12(x, y)                                                   \
  PROCS_ERROR(                                                                 \
      12, "Cannot find a kernel function call following the task pragma " +    \
              (x) +                                                            \
              ((y) ? MULTIPLE_FUNCITON_CALLS                                   \
                   : ".\n  Hint: the task pragma should be placed right "      \
                     "before its "                                             \
                     "specified kernel function call"))
#define PROCS_ERROR_13(x, y)                                                   \
  PROCS_ERROR(13, "Cannot find the kernel function " + (x) +                   \
                      " specified by the pragma " + (y))
#define PROCS_ERROR_14(x, y)                                                   \
  PROCS_ERROR(14, "Cannot find a kernel function"                              \
                  " following the kernel pragma " +                            \
                      (x) +                                                    \
                      ((y) ? MULTIPLE_FUNCITON_CALLS                           \
                           : ".\n  Hint: the kernel pragma should be placed "  \
                             "right before its "                               \
                             "specified kernel function\n"))
#define PROCS_ERROR_15(x)                                                      \
  PROCS_ERROR(15, "TODO")  //  merged with RPOCS_ERROR_12
#define PROCS_ERROR_16(x)                                                      \
  PROCS_ERROR(16, "Cannot find the function definition of " + (x))
#define PROCS_ERROR_17(x)                                                      \
  PROCS_ERROR(17, "Recursive function " + (x) + " is not supported")
#define PROCS_ERROR_18(x)                                                      \
  PROCS_ERROR(18, "TODO")  //  merged with PROCS_ERROR_14
#define PROCS_ERROR_19                                                         \
  PROCS_ERROR(                                                                 \
      19,                                                                      \
      "Cannot find kernel pragma "                                             \
      "in the program.\n"                                                      \
      " Hint: please add a kernel pragma right before the kernel function")
#define PROCS_ERROR_20(x)                                                      \
  PROCS_ERROR(20, "Found multiple definitions for the function " + (x) +       \
                      "\n Hint: it may be due to including a duplicated "      \
                      "input file in other input file")
#define PROCS_ERROR_21(x, y)                                                   \
  PROCS_ERROR(21, "Found the kernel " + (x) +                                  \
                      " function with unmatched task instance " + (y))
#define PROCS_ERROR_22(x)                                                      \
  PROCS_ERROR(22, "Cannot find the kernel function " + (x))
#define PROCS_ERROR_23(x)                                                      \
  PROCS_ERROR(                                                                 \
      23,                                                                      \
      "Found an invalid task pragma: " + (x) +                                 \
          "\n  Hint: the correct format is \n"                                 \
          "    #pragma ACCEL task name=\"taskname\"\n"                         \
          "      (\'taskname\' can only include digits, characters or \'_\' "  \
          "but cannot start with digits)\n"                                    \
          "    Or \n"                                                          \
          "    #pragma ACCEL task")
#define PROCS_ERROR_24(x)                                                      \
  PROCS_ERROR(24, "Found an invalid kernel pragma: " + (x) +                   \
                      "\n  Hint: the correct format is #pragma ACCEL kernel "  \
                      "name=\"taskname\" \n"                                   \
                      "        Or \n"                                          \
                      "        #pragma ACCEL kernel")
#define PROCS_ERROR_25(x)                                                      \
  PROCS_ERROR(25, "Found an invalid interface pragma: " + (x))
#define PROCS_ERROR_26(x, y)                                                   \
  PROCS_ERROR(26, "Cannot find the reduction function " + (x) +                \
                      " specified by the pragma " + (y))
#define PROCS_ERROR_27(x)                                                      \
  PROCS_ERROR(27, "Returning a pointer or reference in function " + (x) +      \
                      " is not supported")
#define PROCS_ERROR_28(x)                                                      \
  PROCS_ERROR(28, "Structure type which contains "                             \
                  "unsupported types, \ne.g. recursive type, function "        \
                  "pointer, static member at the kernel interface:\n " +       \
                      (x) + " is not supported")
#define PROCS_ERROR_29(x)                                                      \
  PROCS_ERROR(29, "Class type which contains "                                 \
                  "unsupported types, \ne.g. recursive type, function "        \
                  "pointer, static member at the kernel interface:\n " +       \
                      (x) + " is not supported")
#define PROCS_ERROR_30(x)                                                      \
  PROCS_ERROR(30, "Union type at the kernel interface: " + (x) +               \
                      " is not supported")
#define PROCS_ERROR_31(x)                                                      \
  PROCS_ERROR(31, "Enum type at the kernel interface: " + (x) +                \
                      " is not supported")
#define PROCS_ERROR_32(x, y, z)                                                \
  PROCS_ERROR(32, "Implicit type cast for argument " + (x) +                   \
                      " : implicit cast from actual argument type " + (y) +    \
                      " to formal argument type " + (z) + " is not supported")
#define PROCS_ERROR_34(x)                                                      \
  PROCS_ERROR(34, "The reduction function is invalid: " + (x) +                \
                      ". Please check Merlin User Guide for the correct "      \
                      "coding style of the reduction function.")
#define PROCS_ERROR_40                                                         \
  PROCS_ERROR(40, "Found an empty design")  //  ZP: rephrase it
#define PROCS_ERROR_41(x)                                                      \
  PROCS_ERROR(41, "Anonymous type on interface is not supported: " + (x) +     \
                      "\n  Hint: please change it into named type")
#define PROCS_ERROR_42(x)                                                      \
  PROCS_ERROR(42, "Specifying system function " + (x) +                        \
                      " as kernel is not supported")
#define PROCS_ERROR_43(x)                                                      \
  PROCS_ERROR(43, "Multiple kernel with same task name \"" + (x) +             \
                      "\" is not supported")
#define PROCS_ERROR_44(x)                                                      \
  PROCS_ERROR(                                                                 \
      44, "Multiple calls on parameter " + (x) +                               \
              " by both local and interface variables are not supported\n" +   \
              "  Hint: please uniquify the function manually")
#define PROCS_ERROR_45(x, y)                                                   \
  PROCS_ERROR(45, "Kernel " + (x) + " which is called by another kernel " +    \
                      (y) + " is not supported")
#define PROCS_ERROR_47(x)                                                      \
  PROCS_ERROR(47, "Kernel " + (x) + " with name 'main' is not supported")
#define PROCS_ERROR_54(x)                                                      \
  PROCS_ERROR(54, "Unamed kernel interface port is not supported: " + (x))
#define PROCS_ERROR_55(x)                                                      \
  PROCS_ERROR(55, "Loop with continue in its init/condition/increment is not " \
                  "supported: " +                                              \
                      (x))
#define PROCS_ERROR_62(x, y)                                                   \
  PROCS_ERROR(62, "Found an invalid " + (x) + " pragma: " + (y))
#define PROCS_ERROR_63(x, y, z)                                                \
  PROCS_ERROR(63, "Found an invalid " + (x) + " pragma: " + (y) +              \
                      ".\n  Diagnosis information: \n" + (z))
#define PROCS_ERROR_66(x)                                                      \
  PROCS_ERROR(66, "Pointer cast " + (x) + " is not supported")
#define PROCS_ERROR_68(x)                                                      \
  PROCS_ERROR(68, "More than one task pragma is not supported")
#define PROCS_ERROR_74(x)                                                      \
  PROCS_ERROR(74,                                                              \
              "Found a variable with multi-dimensional pointer " + (x) + ".")
#define PROCS_ERROR_79(x)                                                      \
  PROCS_ERROR(79, "Do not support array of pointers for char type variable " + \
                      (x))
#define PROCS_ERROR_80(x)                                                      \
  PROCS_ERROR(80, "Found an array variable " + (x) + " with a zero size.")
#define PROCS_ERROR_81(x, y)                                                   \
  PROCS_ERROR(81, "Found an undefined variable '" + (x) + "' in pragma: " + (y))
#define MDARR_ERROR_2(x)                                                       \
  MDARR_ERROR(2, "The dimensions of interface array "                          \
                 "variables do not match in multiple "                         \
                 "function calls or alias: \n" +                               \
                     (x))
#define MDARR_ERROR_3(x)                                                       \
  MDARR_ERROR(                                                                 \
      3,                                                                       \
      "Found overflowed dimension size of the interface variable : \n" + (x))
#define MDARR_ERROR_4(x)                                                       \
  MDARR_ERROR(4, "Complex pointer reinterpretation on "                        \
                 "interface array/pointer is not supported : \n" +             \
                     (x))
#define MDARR_ERROR_5(var_info, type_info, pragma_info)                        \
  MDARR_ERROR(                                                                 \
      5, "Cannot determine some of the dimension sizes of the interface "      \
         "variable " +                                                         \
             (var_info) + " of type \'" + (type_info) +                        \
             "\'.\n  Hint: please " +                                          \
             "specify the missing dimension sizes in the following pragma "    \
             "and add it " +                                                   \
             "to your kernel:\n" + (pragma_info))
#define MDARR_ERROR_6(x)                                                       \
  MDARR_ERROR(6, "Complex access on multiple dimension "                       \
                 "array/pointer is not supported : \n" +                       \
                     (x))
#define MDARR_ERROR_7(depth_info, var_info, dim_info, pragma_info)             \
  MDARR_ERROR(7, "Incorrect max_depth or depth info \'" + (depth_info) +       \
                     "\' for variable \'" + (var_info) +                       \
                     "\',\n  whose dimensin size is " + (dim_info) +           \
                     " in pragma " + (pragma_info))
// Previous MDARR_ERROR_7
#define SYNCHK_ERROR_1(x)                                                      \
  SYNCHK_ERROR(                                                                \
      1, "Found an out-of-bound write access on "                              \
         "argument " +                                                         \
             (x) + "\n" +                                                      \
             "  Hint: please check the array size or the 'depth/max_depth'"    \
             " in the interface pragma.")
#define SYNCHK_ERROR_2(x)                                                      \
  SYNCHK_ERROR(                                                                \
      2, "Found an out-of-bound read access on "                               \
         "argument " +                                                         \
             (x) + "\n" +                                                      \
             "  Hint: please check the array size or the 'depth/max_depth'"    \
             " in the interface pragma.")
#define SYNCHK_ERROR_3 SYNCHK_ERROR(3, "C/C++ mixed design is not supported")
#define SYNCHK_ERROR_4(x)                                                      \
  SYNCHK_ERROR(4, "Find name " + (x) + " which conflict with OpenCL key word.")
#define SYNCHK_ERROR_5(x)                                                      \
  SYNCHK_ERROR(5, "Expression " + (x) +                                        \
                      " with \'long long\' type is not supported")
#define SYNCHK_ERROR_6(x)                                                      \
  SYNCHK_ERROR(6, "Found an overloaded kernel function " + (x))
#define SYNCHK_ERROR_7(x)                                                      \
  SYNCHK_ERROR(7, "Found a local array variable " + (x) +                      \
                      " with statically unknown size in the kernel")
#define SYNCHK_ERROR_8(x)                                                      \
  SYNCHK_ERROR(8, "Do not support array of pointers for char type variable " + \
                      (x))
#define SYNCHK_ERROR_9(x)                                                      \
  SYNCHK_ERROR(9, "Do not support empty array size for variable " + (x))
#define SYNCHK_ERROR_10(x)                                                     \
  SYNCHK_ERROR(10, "Missing the header file <assert.h> in the file " + (x))
#define SYNCHK_ERROR_11(x)                                                     \
  SYNCHK_ERROR(11, "Found a class member function " + (x))
#define SYNCHK_ERROR_12(x)                                                     \
  SYNCHK_ERROR(12,                                                             \
               "Reference type at the kernel interface is not supported:\n " + \
                   (x) +                                                       \
                   "\n  Please try pure kernel flow with \'--attribute "       \
                   "pure_kernel=on\'")
#define SYNCHK_ERROR_13(x) SYNCHK_ERROR(13, (x))
//  PROCS_ERROR(53, "The kernel interface port is not supported: " + (x))
#define SYNCHK_ERROR_14(x, y, z)                                               \
  SYNCHK_ERROR(14, "Argument number for kernel " + (x) + " is " + (y) +        \
                       ".\n"                                                   \
                       "The limitation is " +                                  \
                       (z) + ".")
#define SYNCHK_ERROR_15(x, y, z)                                               \
  SYNCHK_ERROR(15, "Scalar argument number for kernel " + (x) + " is " + (y) + \
                       ".\n"                                                   \
                       "The limitation is " +                                  \
                       (z) + ".")
#define SYNCHK_ERROR_16(x)                                                     \
  SYNCHK_ERROR(16, "System function " + (x) +                                  \
                       " is not supported as top kernel" +                     \
                       ". Please change to another name")
#define SYNCHK_ERROR_17(x, y)                                                  \
  SYNCHK_ERROR(                                                                \
      17, "Implicit function call " + (x) + " declared in " + (y) +            \
              " is not supported\n" +                                          \
              "Hint: Please explicitly specify the function definition " +     \
              "under the delaration")
#define SYNCHK_ERROR_18(x)                                                     \
  SYNCHK_ERROR(18,                                                             \
               "Function call " + (x) + " is not supported\n" +                \
                   " because its callee definition is not included in the "    \
                   "input file(s)")
#define SYNCHK_ERROR_19(x)                                                     \
  SYNCHK_ERROR(19, "Function call " + (x) +                                    \
                       " is not supported"                                     \
                       " because of mismatch between formal parameters and "   \
                       "actual arguments")
#define SYNCHK_ERROR_20(x)                                                     \
  SYNCHK_ERROR(20, "System function call : " + (x) + " is not supported" +     \
                       ". Please rename the called function")
#define SYNCHK_ERROR_21(x)                                                     \
  SYNCHK_ERROR(21, "Unsupported function usage " + (x) +                       \
                       ", possible reasons: through a function pointer or "    \
                       "function reference")
#define SYNCHK_ERROR_22(x)                                                     \
  SYNCHK_ERROR(22, "Asm statement " + (x) + " is not supported")
#define SYNCHK_ERROR_23(x)                                                     \
  SYNCHK_ERROR(23, "Variable argument operation " + (x) + " is not supported")
#define SYNCHK_ERROR_24(x)                                                     \
  SYNCHK_ERROR(24,                                                             \
               "Class declaration within a function body is not supported: " + \
                   (x) + "\n" + "  Hint: please move it out of function")
#define SYNCHK_ERROR_25(x)                                                     \
  SYNCHK_ERROR(                                                                \
      25, "Pointer to interface arrays/pointers is not supported: \n" + (x))
#define SYNCHK_ERROR_26(x)                                                     \
  SYNCHK_ERROR(                                                                \
      26, "The assignment on interface arrays/pointers is not supported: \n" + \
              (x))
#define SYNCHK_ERROR_27(x, y)                                                  \
  SYNCHK_ERROR(27, "Found interface " + (x) + " and " + (y) +                  \
                       " are pointer alias")
#define SYNCHK_ERROR_28(x)                                                     \
  SYNCHK_ERROR(28, "Kernel name " + (x) +                                      \
                       " is not supported."                                    \
                       " Please change to another name")
#define SYNCHK_ERROR_29(x)                                                     \
  SYNCHK_ERROR(29, "Kernel name start with \"__merlin\" is not supported: " +  \
                       (x) + ". Please change to another name")
#define SYNCHK_ERROR_30(x)                                                     \
  SYNCHK_ERROR(30, "Function pointer call " + (x) + " is not supported")
#define SYNCHK_ERROR_31(x)                                                     \
  SYNCHK_ERROR(31, "Duplicate type definitions outside header files:\n" +      \
                       (x) + " is not supported")
#define SYNCHK_ERROR_32(x)                                                     \
  SYNCHK_ERROR(32, "Old style function declaration "                           \
                   "whose parameter type(s) is(are) specified outside the "    \
                   "function parameter list: " +                               \
                       (x) + " is not supported")
#define SYNCHK_ERROR_33(x)                                                     \
  SYNCHK_ERROR(33, "Standard library function " + (x) + " is not supported. ")
#define SYNCHK_ERROR_34(x)                                                     \
  SYNCHK_ERROR(34, "Function " + (x) + " has no definition. ")
#define SYNCHK_ERROR_35(x)                                                     \
  SYNCHK_ERROR(35, "Static kernel function " + (x) + " is not supported.\n" +  \
                       "Please remove static modifier.")
#define SYNCHK_ERROR_36(x)                                                     \
  SYNCHK_ERROR(36, "Pointer comparison detected " + (x) + ".")
#define SYNCHK_ERROR_37(x)                                                     \
  SYNCHK_ERROR(37, "Found pointer casting " + (x) + ".")
#define SYNCHK_ERROR_38(x)                                                     \
  SYNCHK_ERROR(38, "hls::stream<> used as non-reference or in non-pure "       \
                   "kernel flow is not supported:\n" +                         \
                       (x))
#define SYNCHK_ERROR_39(x, y)                                                  \
  SYNCHK_ERROR(39, "Member function " + (x) + "of class " + (y) +              \
                       " has no definition")

#define BURST_ERROR_1                                                          \
  BURST_ERROR(1, "Cannot pass function uniquifying checker\n"                  \
                 "  Hint: Please turn off memory burst or uniquify function "  \
                 "calls\n")  // ZP: rephrase it
#define INFTF_ERROR_6(x, y)                                                    \
  INFTF_ERROR(                                                                 \
      6, "Found multiple interface pragmas for port " + (x) + ":\n" + (y) +    \
             "  Hint: please try to merge them into one single pragma")
#define SEPAR_ERROR_1(x)                                                       \
  SEPAR_ERROR(1, "Recursive function is not supported : " + (x))
#define SEPAR_ERROR_2(x)                                                       \
  SEPAR_ERROR(2, "Recursive type is not supported: " + (x))
#define SEPAR_ERROR_3(x)                                                       \
  SEPAR_ERROR(3, "Function pointer is not supported : " + (x))
#define SEPAR_ERROR_4(x)                                                       \
  SEPAR_ERROR(4, "Template function is not supported: " + (x))
#define SEPAR_ERROR_5(x)                                                       \
  SEPAR_ERROR(5, "Template class is not supported : " + (x))
#define SEPAR_ERROR_6(x)                                                       \
  SEPAR_ERROR(6, "Function call is not supported :\n" + (x))
#define SEPAR_ERROR_7(x) SEPAR_ERROR(7, "System type is not supported: " + (x))
#define SEPAR_ERROR_8(x)                                                       \
  SEPAR_ERROR(8, "Inlined or undefined type is not supported : " + (x))
#define SEPAR_ERROR_9(x)                                                       \
  SEPAR_ERROR(9, "Global variable " + (x) +                                    \
                     " with recursive assignment is not supported")
#define SEPAR_ERROR_10(x)                                                      \
  SEPAR_ERROR(10, "Anonymous type is not supported : " + (x) +                 \
                      "\n  Hint: please add a name for the above type")
#define SEPAR_ERROR_11(var, var_pos, x)                                        \
  SEPAR_ERROR(11, "Global or static variable '" + (var) + "' " + (var_pos) +   \
                      "\n  shared in multiple kernels " + (x) +                \
                      " is not supported")
#define SEPAR_ERROR_12(x) SEPAR_ERROR(12, x)
#define SEPAR_ERROR_13(x) SEPAR_ERROR(13, x)
#define SEPAR_ERROR_14(x) SEPAR_ERROR(14, x)
#define SEPAR_ERROR_15(x) SEPAR_ERROR(15, x)
#define SEPAR_ERROR_16(x) SEPAR_ERROR(16, x)
#define SEPAR_ERROR_17(x) SEPAR_ERROR(17, x)
#define SEPAR_ERROR_18(x) SEPAR_ERROR(18, x)
#define SEPAR_ERROR_19(x) SEPAR_ERROR(19, x)
#define SEPAR_ERROR_20(x)                                                      \
  SEPAR_ERROR(20,                                                              \
              "Type defined inside a function body is not supported : " + (x))
#define SEPAR_WARNING_1(x)                                                     \
  SEPAR_WARNING(                                                               \
      1, "Nested struct definitions are not supported : " + (x) +              \
             "\n  Hint: please replace nested struct with ordinary struct")
#define SEPAR_WARNING_2(x, y)                                                  \
  SEPAR_WARNING(2, "Stream channel '" + (x) +                                  \
                       "' lacks the 'channel_depth' attribute. Set as " +      \
                       (y) + " by default.\n")
#define SEPAR_WARNING_3(x)                                                     \
  SEPAR_WARNING(                                                               \
      3, "Stream channel '" + (x) +                                            \
             "' is read-only in the task, which may lead to deadlock.\n")
#define SEPAR_WARNING_4(x)                                                     \
  SEPAR_WARNING(                                                               \
      4, "Stream channel '" + (x) +                                            \
             "' is write-only in the task, which may lead to deadlock.\n")

#define CLGEN_INFO(num, msg) MARS_INFO(CLGEN, msg, num)
#define CLGEN_WARNING(num, msg) MARS_WARNING(CLGEN, msg, num)
#define CLGEN_ERROR(num, msg) MARS_ERROR(CLGEN, msg, num)
#define CLGEN_ERROR_1(x)                                                       \
  CLGEN_ERROR(                                                                 \
      1, "Function declaration in the header file is not supported\n" + (x))
#define CLGEN_ERROR_2(x)                                                       \
  CLGEN_ERROR(                                                                 \
      2, "Finding a pointer variable " + (x) +                                 \
             " which is assigned to both global variable and local variable")
#define CLGEN_ERROR_3(x) CLGEN_ERROR(3, x)
#define CLGEN_ERROR_4(x) CLGEN_ERROR(4, x)
#define RUNTM_INFO(num, msg) MARS_INFO(RUNTM, msg, num)
#define RUNTM_WARNING(num, msg) MARS_WARNING(RUNTM, msg, num)
#define RUNTM_ERROR(num, msg) MARS_ERROR(RUNTM, msg, num)
#define RUNTM_ERROR_1(x)                                                       \
  RUNTM_ERROR(1, "Expression as kernel argument is not supported : " + (x))
#define RUNTM_ERROR_2(x)                                                       \
  RUNTM_ERROR(                                                                 \
      2,                                                                       \
      "Finding array/pointer variable with unknown dimension size : " + (x) +)
#define RUNTM_ERROR_3(x)                                                       \
  RUNTM_ERROR(                                                                 \
      3,                                                                       \
      "Multiple dimensional array in kernel interface is not supported : " +   \
          (x))
#define RUNTM_ERROR_4(x)                                                       \
  RUNTM_ERROR(4, "Kernel interface variable with both read "                   \
                 "and write accesses in the kernel is not supported: " +       \
                     (x))
#define RUNTM_ERROR_5(x)                                                       \
  RUNTM_ERROR(5, "Global variable as the actual declaration "                  \
                 "for the kernel interface port is not supported: " +          \
                     (x))
#define RUNTM_ERROR_6(x)                                                       \
  RUNTM_ERROR(                                                                 \
      6,                                                                       \
      "Cannot find the unique actual declaration for the variable: " + (x) +)

#define POSTW_ERROR(num, msg) MARS_ERROR(POSTW, msg, num)

#define POSTW_ERROR_1(x, y)                                                    \
  POSTW_ERROR(1, "Kernel function " + (x) +                                    \
                     " with reference return type is not supported: " + (y))
#define POSTW_ERROR_2(x)                                                       \
  POSTW_ERROR(2, "Kernel return expresion can not be aggregate initilizer: " + \
                     (x))

//  ////////////////////////////////////////////  /
//  Pure information messages (Optimization applied, or configurarion is set
//  according to the use inputs)
//  ////////////////////////////////////////////  /
#define PROCS_INFO_1                                                           \
  PROCS_INFO(1, "Automatic fine-grained loop pipelining is enabled")
#define PROCS_INFO_3                                                           \
  PROCS_INFO(3, "Automatic find-grained loop parallelization is enabled")
#define BURST_INFO_1(x, z, y)                                                  \
  BURST_INFO(1, "Memory burst inferred: variable " + (x) + " in scope " +      \
                    (z) + "\n  burst buffer size = " + (y) + " bytes")
#define BURST_INFO_2(x, y)                                                     \
  BURST_INFO(2, "Detected manual memory burst: variable " + (x) +              \
                    "\n  burst buffer size = " + (y) + " bytes")
#define BURST_INFO_3(x)                                                        \
  BURST_INFO(3, "Memory burst inferred manually: variable " + (x))
#define WDBUS_INFO_1(x, y)                                                     \
  WDBUS_INFO(1, "Memory coalescing inferred: variable " + (x) +                \
                    "\n"                                                       \
                    "  interface bitwidth = " +                                \
                    (y) + " bits")
#define WDBUS_INFO_2(x, y)                                                     \
  WDBUS_INFO(2, "Memory coalescing inferred: variable " + (x) +                \
                    "\n"                                                       \
                    "  max interface bitwidth = " +                            \
                    (y) + " bits")
#define REDUC_INFO_1(x, y)                                                     \
  REDUC_INFO(1, "Applying reduction on operation " + (y))
// Yuxin: Dec/06/2019, dont report cyclic/block scheme
#define CGPIP_INFO_1(x, y, z)                                                  \
  CGPIP_INFO(1, "Loop " + (x) + " is pipelined " + +"with " + (z) + " stages")
#define CGPAR_INFO_1(x) CGPAR_INFO(1, "Loop " + (x) + " is fully parallelized")
#define FGPIP_INFO_2(x, y)                                                     \
  FGPIP_INFO(2, "Loop " + (x) + (y) + " is fully parallelized")
#define FGPIP_INFO_3(x, y) FGPIP_INFO(3, "Loop " + (x) + (y) + " is pipelined")
#define FGPIP_INFO_8(x, y, z)                                                  \
  FGPIP_INFO(8,                                                                \
             "Loop " + (x) + (y) + " is pipelined with a target II of " + (z))
#define FGPIP_INFO_5(x, y)                                                     \
  FGPIP_INFO(5, "Loop " + (x) + " is parallelized with a factor of " + (y))
#define FGPIP_INFO_6(x, y)                                                     \
  FGPIP_INFO(6, "False loop-carried data dependence applied to variable " +    \
                    (x) + "\n in loop " + (y))
#define FGPIP_INFO_7(x)                                                        \
  FGPIP_INFO(7, "False loop-carried data dependence applied to all the "       \
                "arrays in loop " +                                            \
                    (x))

#define AUREG_WARNING_1(x)

//  ////////////////////////////////////////////  /
//  General Warning Messages
//  ////////////////////////////////////////////  /
//  General coding style warning
#define SYNCHK_WARNING_1(x)                                                    \
  SYNCHK_WARNING(                                                              \
      1, "Found a potential out-of-bound write access on "                     \
         "the argument " +                                                     \
             (x) + "\n" +                                                      \
             "  Hint: please check the array size or the 'depth/max_depth'"    \
             " in the interface pragma.")
#define SYNCHK_WARNING_2(x)                                                    \
  SYNCHK_WARNING(                                                              \
      2, "Found a potential out-of-bound read access on "                      \
         "the argument " +                                                     \
             (x) + "\n" +                                                      \
             "  Hint: please check the array size or the 'depth/max_depth'"    \
             " in the interface pragma.")
#define SYNCHK_WARNING_3(x, y)                                                 \
  SYNCHK_WARNING(                                                              \
      3, ((x) ? "Cannot determine the lower bound for the expression "         \
              : "Cannot determine the upper bound for the expression ") +      \
             (y) + ", this may disable certain optimizations")
#define SYNCHK_WARNING_4(x)                                                    \
  SYNCHK_WARNING(4, "Found a class member function " + (x))
#define SYNCHK_WARNING_5(x, y, z)                                              \
  SYNCHK_WARNING(5, "Argument number for kernel " + (x) + " is " + (y) +       \
                        ".\n"                                                  \
                        "This may caused the unoptimal design. "               \
                        "Suggest kernel argument number not larger than " +    \
                        (z) + ".")
#define SYNCHK_WARNING_6(x)                                                    \
  SYNCHK_WARNING(6, "Goto statement may cause sub-optimal results " +          \
                        (x))  //  ZP: do we support goto
#define MDARR_WARNING_1(var_info, type_info, pragma_info)                      \
  MDARR_WARNING(                                                               \
      1, "OpenCL host library generation is disabled because some of the "     \
         "dimension sizes of the interface variable " +                        \
             (var_info) + " of type \'" + (type_info) +                        \
             "\' is not determined.\n" +                                       \
             "  Hint: please specify the missing dimension sizes in the " +    \
             "following pragma and add it to your kernel:\n" + (pragma_info))
#define MDARR_WARNING_3(x)                                                     \
  MDARR_WARNING(                                                               \
      3, "Found a task interface variable unused in the kernel function " +    \
             (x))  //  ZP: rephrase it
#define PROCS_INFO_2                                                           \
  PROCS_INFO(                                                                  \
      2,                                                                       \
      "Hint: you may specify the range of the variables in the loop bounds "   \
      "using 'assert'. Please refer to Merlin User Guide")  //  ZP: Hint ->
                                                            //  Warning?

//  Vendor tool warning
#define DISAG_WARNING_1(x, y)
#define GLOBL_WARNING_1(x, y)
#define WDBUS_WARNING_1                                                        \
  WDBUS_WARNING(                                                               \
      1,                                                                       \
      "OpenCL simulation is disabled "                                         \
      "as SDAccel does not support OpenCL simulation for memory coalescing "   \
      "in C designs.\n"                                                        \
      "  Hint: move the design to C++ or disable memory coalescing.")

//  ////////////////////////////////////////////  /
//  All Disabling Messages (pragma is ignored)
//  ////////////////////////////////////////////  /
#define PROCS_WARNING_5(x)                                                     \
  PROCS_WARNING(5, "Ignoring pragma " + (x) +                                  \
                       " because it can only be applied to a for-loop")
#define PROCS_WARNING_6(x)                                                     \
  PROCS_WARNING(6, "Ignoring pragma " + (x) +                                  \
                       " which isnot followed by a for-loop")
#define PROCS_WARNING_16(x) PROCS_WARNING(16, "Found an unknown pragma: " + (x))
#define PROCS_WARNING_17(x)                                                    \
  PROCS_WARNING(17, "Found an invalid pipeline pragma: " + (x))
#define PROCS_WARNING_18(x)                                                    \
  PROCS_WARNING(18, "Found an invalid parallel pragma: " + (x))
#define PROCS_WARNING_21(x)                                                    \
  PROCS_WARNING(                                                               \
      21,                                                                      \
      "Found an interface pragma without specified interface depth: " + (x))
#define PROCS_WARNING_20(x)                                                    \
  PROCS_WARNING(20, "Found an invalid pipeline_parallel pragma: " + (x))
#define PROCS_WARNING_29(x)                                                    \
  PROCS_WARNING(29, "Found an invalid loop tiling pragma: " + (x))
#define PROCS_WARNING_30(x, y)
#define PROCS_WARNING_31(x)                                                    \
  PROCS_WARNING(31, "Cannot flatten loop " + (x) +                             \
                        " because of unsupported subloops (such as while, "    \
                        "do-while and memcpy)")
#define PROCS_WARNING_32(x, y, z)
#define PROCS_WARNING_33(x)
#define PROCS_WARNING_34(x)                                                    \
  PROCS_WARNING(34, "Ignoring pragma " + (x) +                                 \
                        " because Xilinx tool cannot support a false "         \
                        "dependence pragma without a specified variable")
#define PROCS_WARNING_35(x)                                                    \
  PROCS_WARNING(34, "Ignoring pragma " + (x) +                                 \
                        " because cannot find the specified variable")
#define PROCS_WARNING_36(x)                                                    \
  PROCS_WARNING(                                                               \
      36, "Cannot parallel loop " + (x) +                                      \
              " because of unsupported loop types (such as while, do-while)")
#define PROCS_WARNING_37(x)                                                    \
  PROCS_WARNING(                                                               \
      37, "Found pointer type casting " + (x) +                                \
              ". Vendor tool may have unstable bahaviour on such operations")
#define PROCS_WARNING_38(x)                                                    \
  PROCS_WARNING(                                                               \
      38, "Found non-const initialization on const global vars" + (x) +        \
              ". Vendor tool may not treat it as a compile-time constant.")
#define PROCS_WARNING_39(x)                                                    \
  PROCS_WARNING(39, "Pragma " + (x) +                                          \
                        " with \'auto\' value is not supported.\n" +           \
                        "  Hint: please enable custom DSE flow")
#define PROCS_WARNING_40(x)                                                    \
  PROCS_WARNING(40, "Found unaligned memcpy on " + (x) +                       \
                        "\n"                                                   \
                        " Hint: Please consider a copying size that aligns "   \
                        "source buffer size of memcpy")
#define PROCS_WARNING_41(x, y)                                                 \
  PROCS_WARNING(41, "Found a large const array " + (x) + " of size " + (y) +   \
                        "Byte"                                                 \
                        " which may result in excessive on-chip memory usage." \
                        "  Suggest the const array size smaller than 1MB.")
#define PROCS_WARNING_45(x, y)                                                 \
  PROCS_WARNING(45, "Found a loop " + (x) + " without a bound on " +           \
                        "its tripcount. " + "Please specify one using " +      \
                        "'#pragma HLS loop_tripcount max=?' or 'assert'.\n" +  \
                        "Use default max tripcount (" + (y) +                  \
                        ") for analysis")
#define BURST_WARNING_1(x, y, z)                                               \
  BURST_WARNING(                                                               \
      1, "Memory burst NOT inferred: variable " + (x) + " in scope " + (z) +   \
             "\n"                                                              \
             "  Reason: the bound of the access range is not determined (" +   \
             (y) +                                                             \
             ")\n"                                                             \
             "  Hint: please specify the bounds of the variables using "       \
             "'assert'")
#define BURST_WARNING_2(x, z, y, c)                                            \
  BURST_WARNING(2, "Memory burst inferred: variable " + (x) + " in scope " +   \
                       (z) +                                                   \
                       "\n"                                                    \
                       "  with insufficient on-chip memory (" +                \
                       (y) + " bytes required)\n" +                            \
                       ((c) ? DECREASE_TILING_FATOR                            \
                            : " Hint: Tile it to reduce the cache size"))
#define BURST_WARNING_3(x)                                                     \
  BURST_WARNING(3, "Cannot burst all the accesses of variable " + (x) +        \
                       " because of data dependence")
#define BURST_WARNING_4(x, y)                                                  \
  BURST_WARNING(                                                               \
      4, "Memory burst NOT inferred: variable " + (x) +                        \
             ". Reason: burst length cannot be determined (" + (y) +           \
             ") \n"                                                            \
             "  Hint:   set accurate bound for variables using 'assert'")
#define BURST_WARNING_5(x, z, y, c)                                            \
  BURST_WARNING(5, "Memory burst NOT inferred : variable " + (x) +             \
                       " in scope " + (z) + " is too big to be cached.\n" +    \
                       ((c) ? DECREASE_TILING_FATOR                            \
                            : " Hint: Tile it to reduce the cache size"))
#define BURST_WARNING_6(x)                                                     \
  BURST_WARNING(6, "Memory burst NOT inferred: variable " + (x) +              \
                       ". Reason: improper streaming access order")
#define BURST_WARNING_7(x)                                                     \
  BURST_WARNING(7, "Memory burst NOT inferred: variable " + (x) +              \
                       ". Reason: unsupported access, such as pointer alias, " \
                       "pointer arithmetic operation")
#define BURST_WARNING_8(x)                                                     \
  BURST_WARNING(8, "Memory burst NOT inferred: variable " + (x) +              \
                       ". Reason: improper streaming access order")
#define BURST_WARNING_9(x)                                                     \
  BURST_WARNING(9, "Memory burst NOT inferred: variable " + (x) +              \
                       ". Reason: improper streaming access order")
#define BURST_WARNING_10(x)                                                    \
  BURST_WARNING(10, "Memory burst NOT inferred: variable " + (x) +             \
                        ". Reason: both read and write access")
#define BURST_WARNING_11(x)                                                    \
  BURST_WARNING(11, "Memory burst NOT inferred: variable " + (x) +             \
                        ". Reason: improper streaming access order")
#define BURST_WARNING_12(x, y)                                                 \
  BURST_WARNING(                                                               \
      12, "Memory burst NOT inferred: variable " + (x) +                       \
              ". Reason: access bound cannot be determined (" + (y) +          \
              ") \n"                                                           \
              "  Hint:   set accurate bound for variables using 'assert'")
#define BURST_WARNING_13(x)                                                    \
  BURST_WARNING(13, "Memory burst NOT inferred: variable " + (x) +             \
                        ". Reason: possible negative burst length\n" +         \
                        "  Hint: please specify the bounds of the variables "  \
                        "using 'assert' ")
#define BURST_WARNING_14(x, y)                                                 \
  BURST_WARNING(                                                               \
      14, "Memory burst NOT inferred: variable " + (x) +                       \
              ". Reason: negative burst start address (" + (y) + ")\n" +       \
              "  Hint:  set accurate bound for variables using 'assert'")
#define WDBUS_WARNING_2(x, y)                                                  \
  WDBUS_WARNING(                                                               \
      2, "Memory coalescing NOT inferred: variable " + (x) +                   \
             "\n  Reason: unsupported reference " + (y) +                      \
             "\n  Hint: Please use simple assignment or memcpy if possible")
#define WDBUS_WARNING_4(x)                                                     \
  WDBUS_WARNING(                                                               \
      4, "Memory coalescing NOT inferred: variable " + (x) +                   \
             "\n  Reason: the number of array dimensions is larger than 4 ")
#define WDBUS_WARNING_7(x)                                                     \
  WDBUS_WARNING(7, "Memory coalescing not inferred because of unsupported "    \
                   "member function\n" +                                       \
                       (x))
#define WDBUS_WARNING_8                                                        \
  WDBUS_WARNING(                                                               \
      8, "Memory coalescing not inferred because of mixed C/C++ design")
#define WDBUS_WARNING_9(x, y)                                                  \
  WDBUS_WARNING(9, "Memory coalescing variable " + (x) +                       \
                       " may cause long compilation time of downstream tool."  \
                       "\n  Reason: the access offset is not divisible by " +  \
                       (y))
#define WDBUS_WARNING_10(x, y, z)                                              \
  WDBUS_WARNING(10, "Memory coalescing NOT inferred: variable " + (x) +        \
                        "\n  Reason: type " + (y) +                            \
                        " is not supported now.\n"                             \
                        "  Note: only " +                                      \
                        (z) + " are supported now.")
#define WDBUS_WARNING_11(x, y)                                                 \
  WDBUS_WARNING(11, "Memory coalescing NOT inferred: variable " + (x) +        \
                        "\n  Reason: burst length is less than threshold (" +  \
                        (y) + " elements).\n" +                                \
                        "  Hint: reset threshold by using --attribute "        \
                        "bus_length_threshold=?")
#define WDBUS_WARNING_12(x)                                                    \
  WDBUS_WARNING(12,                                                            \
                "Memory coalescing NOT inferred explicitly: variable " + (x))
#define WDBUS_WARNING_13(x, y)                                                 \
  WDBUS_WARNING(13, "Memory coalescing NOT inferred: variable " + (x) +        \
                        "\n  because of invalid bus bitwidth (" + (y) +        \
                        " bits).")

#define WDBUS_WARNING_14(x)                                                    \
  WDBUS_WARNING(14, "Memory coalescing NOT inferred: variable " + (x) +        \
                        "  Reason: the original bitwidth is unknown.\n" +      \
                        "  Hint: cannot support struct/class type now")

#define WDBUS_WARNING_15(x, y)                                                 \
  WDBUS_WARNING(15, "Memory coalescing NOT inferred: variable " + (x) +        \
                        "\n  because of unsupported original bitwidth (" +     \
                        (y) + " bits)." + "\n Hint: only support power " +     \
                        "of two and no less than 8 bitwidth")
#define WDBUS_WARNING_16(x, y)                                                 \
  WDBUS_WARNING(                                                               \
      16, "Memory coalescing NOT inferred: variable " + (x) +                  \
              "\n  Reason: unaligned write only access in dataflow process " + \
              (y) + "\n  Hint: Please align access if possible")

#define REDUC_WARNING_1(x, y)                                                  \
  REDUC_INFO(2, "Disabling automatic reduction in loop " + (x) +               \
                    " because of multiple reduction operations:\n" + (y) +     \
                    "   Hint: try to group these operations into a reduction " \
                    "function")
#define REDUC_WARNING_2(x)                                                     \
  REDUC_WARNING(                                                               \
      2, "Reduction on operation " + (x) + " not applied: no benefit.\n" +     \
             "Hint: using parallel factor to apply loop tiling may help "      \
             "trigger reduction optimization and enhance the performance. ")
#define REDUC_WARNING_3(x)                                                     \
  REDUC_WARNING(3, "Reduction in loop " + (x) +                                \
                       " not applied: "                                        \
                       "noncanonical loop.")
#define REDUC_WARNING_4(x)                                                     \
  REDUC_WARNING(4, "Reduction in loop " + (x) +                                \
                       " not applied: "                                        \
                       ": it uses the outer loop's iterator in " +             \
                       "the loop bound expression.")
#define REDUC_WARNING_5(x, y)                                                  \
  REDUC_WARNING(5, "Reduction on variable '" + (x) + "' in loop " + (y) +      \
                       " not applied: "                                        \
                       "no valid reduction operation.")
#define REDUC_WARNING_6(x, y)                                                  \
  REDUC_WARNING(                                                               \
      6, "Reduction on variable '" + (x) + "' in loop " + (y) +                \
             " not applied"                                                    \
             ": multiple accesses on the variable exist in the loop.\n"        \
             "Hint: try to reduce the accesses to the variable in the loop " + \
             "or change the scope of reduction optimization. ")
#define REDUC_WARNING_7(x, y)                                                  \
  REDUC_WARNING(                                                               \
      7, "Reduction on variable '" + (x) + "' in loop " + (y) +                \
             " may has no performance benefit.\n" +                            \
             "Hint: using parallel factor to apply loop tiling may help " +    \
             "enhance the performance. ")
#define REDUC_WARNING_8(x, y)                                                  \
  REDUC_WARNING(8, "Reduction on variable '" + (x) + "' in loop " + (y) +      \
                       " not applied: unknown tripcount.\n"                    \
                       "  Hint: please specify the bounds of the tripcount "   \
                       "using 'assert' ")
#define REDUC_WARNING_9(x, y, z)                                               \
  REDUC_WARNING(9, "Reduction on variable '" + (x) + "' in loop " + (y) +      \
                       " not applied: " +                                      \
                       "unsupported reduction operation '" + (z) + "'")
#define REDUC_WARNING_10(x)                                                    \
  REDUC_WARNING(10, "Ignoring reduction in loop " + (x) +                      \
                        " as it is fully parallelized")
#define REDUC_WARNING_11(x, y)                                                 \
  REDUC_WARNING(11, "Ignoring reduction request on variable '" + (x) +         \
                        "' in loop " + (y) +                                   \
                        ": the variable is declared in the loop scope\n")

#define LINEBUF_INFO_1(x, y, z)                                                \
  LINEBUF_INFO(1, "Applying line buffer on variable '" + (x) + "' in loop " +  \
                      (y) + " with a " + (z) + " stencil")
#define LINEBUF_WARNING_1(x, y)                                                \
  LINEBUF_WARNING(1, "Stopping line buffer optimization on variable '" + (x) + \
                         "' in loop " + (y) + ": line buffer is only for " +   \
                         "off-chip memory accesses.\n")
#define LINEBUF_WARNING_2(x, y)                                                \
  LINEBUF_WARNING(2, "Stopping line buffer optimization on variable '" + (x) + \
                         "' in loop " + (y) +                                  \
                         ": the accesses are not stencil.\n")
#define LINEBUF_WARNING_3(x, y)                                                \
  LINEBUF_WARNING(3, "Stopping line buffer optimization on variable '" + (x) + \
                         "' in loop " + (y) +                                  \
                         ": the variable cannot be delinearized.\n")
#define LINEBUF_WARNING_4(x, y)                                                \
  LINEBUF_WARNING(                                                             \
      4, "Stopping line buffer optimization on variable '" + (x) +             \
             "' in loop " + (y) +                                              \
             ": the array size is inconsistent with the loop's tripcount.\n")
#define CGPIP_WARNING_1(x, y)                                                  \
  CGPIP_WARNING(                                                               \
      1,                                                                       \
      "Pipelining NOT applied on loop " + (x) +                                \
          "\n  Reason: data dependency across loop iterations on" + (y) +      \
          "\n  Hint:   try to move dependency into the same loop iteration")
#define CGPIP_WARNING_2(x, y, z, w)                                            \
  CGPIP_WARNING(                                                               \
      2, "Coarse-grained pipelining NOT applied on loop " + (x) +              \
             "\n  Reason: " + (y) +                                            \
             " statements are not allowed in coarse-grained pipeline " + (z) + \
             (w))
#define CGPIP_WARNING_3(x, y, z)                                               \
  CGPIP_WARNING(3, "Coarse-grained pipelining NOT applied on loop " + (x) +    \
                       "\n  Reason: multiple accesses to " + (y) +             \
                       " in multiple pipeline stages " + (z) +                 \
                       ".\n  Hint:   try to group the pipeline stages using "  \
                       "brackets {}. Refer to Merlin User Guide")
#define CGPIP_WARNING_4(x)                                                     \
  CGPIP_WARNING(4, "Coarse-grained pipelining NOT applied on loop " + (x) +    \
                       "\n  Reason: multiple bus accesses exist in multiple "  \
                       "pipeline stages.")
#define CGPIP_WARNING_5(x)                                                     \
  CGPIP_WARNING(5, "Coarse-grained pipelining NOT applied on loop " + (x) +    \
                       "\n  Reason: the loop is non-canonical. ")
#define CGPIP_WARNING_6(x, y, z)                                               \
  CGPIP_WARNING(6, "Coarse-grained pipelining NOT applied on loop " + (x) +    \
                       "\n  Reason: potential cross-iteration data "           \
                       "dependence on variable " +                             \
                       (y) + ".\n" + (z))
#define CGPIP_WARNING_7(x, y)                                                  \
  CGPIP_WARNING(                                                               \
      7, "Coarse-grained pipelining NOT applied on loop " + (x) +              \
             "\n  Reason: multiple variable declarations in a statement " +    \
             (y))
#define CGPIP_WARNING_8(x)                                                     \
  CGPIP_WARNING(8,                                                             \
                "Coarse-grained pipelining NOT applied on loop " + (x) +       \
                    "\n  Reason: there is only one statement in the loop. ")
#define CGPIP_WARNING_9(x, y)                                                  \
  CGPIP_WARNING(                                                               \
      9,                                                                       \
      "Coarse-grained pipelining NOT applied on loop " + (x) +                 \
          "\n  Reason: potential uninitialized data elements in variable " +   \
          (y) + ".")
#define CGPAR_WARNING_1(x, y)                                                  \
  CGPAR_WARNING(1, "Coarse-grained parallelization NOT applied: loop " + (x) + \
                       " because of access on the external memory port " +     \
                       (y))
#define CGPAR_WARNING_2(x, y)                                                  \
  CGPAR_WARNING(2,                                                             \
                "Cannot parallel the loop " + (x) + " because the access " +   \
                    "to the following array(s) cannot be separated among " +   \
                    "parallel threads. Please consider to separate the " +     \
                    "accesses by introducing a dimension along " + (x) +       \
                    " to " + "reshape the array(s):\n" + (y))
#define CGPAR_WARNING_3(x)                                                     \
  CGPAR_WARNING(3, "Coarse-grained parallelization NOT applied: loop " + (x) + \
                       " because of non-canonical loop")
#define CGPAR_WARNING_4(x)                                                     \
  CGPAR_WARNING(4, "Coarse-grained parallelization NOT applied: loop " + (x) + \
                       " because of non-constant loop bound")
#define CGPAR_WARNING_5(x)                                                     \
  CGPAR_WARNING(5, "Coarse-grained parallelization NOT applied: loop " + (x) + \
                       " because of break statements")
#define CGPAR_WARNING_6(x)                                                     \
  CGPAR_WARNING(6, "Coarse-grained parallelization NOT applied: loop " + (x) + \
                       " because of continue statements")
#define CGPAR_WARNING_7(x)                                                     \
  CGPAR_WARNING(7, "Coarse-grained parallelization NOT applied: loop " + (x) + \
                       " because of return statements")
#define CGPAR_WARNING_8(x)                                                     \
  CGPAR_WARNING(8, "Coarse-grained parallelization NOT applied: loop " + (x) + \
                       " because of goto statements")
#define CGPAR_WARNING_9(x)                                                     \
  CGPAR_WARNING(9, "Coarse-grained parallelization NOT applied: loop " + (x) + \
                       " because of label statements")
#define CGPAR_WARNING_10(x, y)                                                 \
  CGPAR_WARNING(10, "Coarse-grained parallelization NOT applied: loop " +      \
                        (x) + " because of unsupported task spawn:\n" + (y))
#define CGPAR_WARNING_11(x, y)                                                 \
  CGPAR_WARNING(                                                               \
      11, "Coarse-grained parallelization NOT applied: loop " + (x) +          \
              " because of access on non-constant global variable " + (y))
#define CGPAR_WARNING_12(x, y)                                                 \
  CGPAR_WARNING(                                                               \
      12, "Coarse-grained parallelization NOT applied: loop " + (x) +          \
              " because of access on non-constant local static variable " +    \
              (y))
#define CGPAR_WARNING_13(x, y, z)                                              \
  CGPAR_WARNING(13, "Coarse-grained parallelization NOT applied: loop " +      \
                        (y) + " because access on array " + (x) +              \
                        " cannot be parallelized due to " + (z))

#define CGPAR_WARNING_14(y, z)                                                 \
  CGPAR_WARNING(14, "Coarse-grained parallelization NOT applied: loop " +      \
                        (y) +                                                  \
                        " because access on ports cannot be streamed due to "  \
                        "following reasons\n" +                                \
                        (z))

#define TILE_WARNING_1(x)                                                      \
  TILE_WARNING(1,                                                              \
               "Ignoring a loop tiling pragma for non-canonical loop: " + (x))
#define TILE_WARNING_2(x)                                                      \
  TILE_WARNING(2, "Ignoring a loop tiling pragma for loop with decremental "   \
                  "iteration space: " +                                        \
                      (x))
#define TILE_WARNING_3(x)                                                      \
  TILE_WARNING(                                                                \
      3,                                                                       \
      "Ignoring a loop tiling pragma for loop with non-constant step: " + (x))
#define TILE_WARNING_4(x)                                                      \
  TILE_WARNING(4, "Cannot tile a loop with loop increment not equal to 1: " +  \
                      (x))
#define TILE_WARNING_5(x)                                                      \
  TILE_WARNING(5, "Ignoring a loop tiling pragma for loop with non-constant "  \
                  "lower bound: " +                                            \
                      (x))
#define TILE_WARNING_6(x)                                                      \
  TILE_WARNING(6, "Ignoring a loop tiling pragma for a tiling factor <=1: " +  \
                      (x))
#define TILE_WARNING_7(x)                                                      \
  TILE_WARNING(7, "Ignoring a loop tiling pragma for an indivisible loop "     \
                  "with label in its loop body: " +                            \
                      (x))
#define TILE_WARNING_8(x)                                                      \
  TILE_WARNING(8, "Loop tiling is disabled because tiling factor "             \
                  ">= loop tripcount: " +                                      \
                      (x))
#define TILE_WARNING_9(x)                                                      \
  TILE_WARNING(9, "Ignoring a loop tiling pragma for a loop with write "       \
                  "access on its iterator in its loop body: " +                \
                      (x))
#define TILE_WARNING_10(x, y)                                                  \
  TILE_WARNING(10,                                                             \
               "The tripcount of loop " + (x) +                                \
                   " is not divisible by the factor '" + (y) +                 \
                   "'. This will negatively impact both resource utilization " \
                   "and performance.")
#define TILE_WARNING_11(x, y)                                                  \
  TILE_WARNING(11,                                                             \
               "The tripcount of loop " + (x) +                                \
                   " may not be divisible by the factor '" + (y) +             \
                   "'. This will negatively impact both resource utilization " \
                   "and performance.\n Hint: Please rewrite the "              \
                   "tripcount as a multiple of the factor.")

#define FGPIP_WARNING_2(x, y)                                                  \
  FGPIP_WARNING(2,                                                             \
                "Loop fine-grained parallelization CANNOT be applied: loop " + \
                    (x) + (y) +                                                \
                    " because the loop contains subloop(s) not being "         \
                    "parallelized.")
#define FGPIP_WARNING_4(x, y)                                                  \
  FGPIP_WARNING(4, "Parallelization NOT applied: loop " + (x) +                \
                       "  Reason: Invalid parallel factor: " + (y) +           \
                       " \n"                                                   \
                       "  Hint:   parallel factor can only be constant "       \
                       "positive integer ")
#define FGPIP_WARNING_5(x, y, z)                                               \
  FGPIP_WARNING(                                                               \
      5,                                                                       \
      "Parallelization NOT applied: loop " + (x) +                             \
          "  Reason: Reason: do not support the partitioning on varaible " +   \
          (y) + "' dimension " + (z))
#define FGPIP_WARNING_6(x, y)                                                  \
  FGPIP_WARNING(6, "Fine-grained pipelining NOT applied: loop " + (x) +        \
                       "  Reason: conflict partitioning factor on variable " + \
                       (y) + " ")

#define FGPIP_WARNING_7(x)                                                     \
  FGPIP_WARNING(7, "Parallelization NOT applied: loop " + (x) +                \
                       "  Reason: loop " + (x) + "' is non-canonical ")
#define FGPIP_WARNING_10(x, y)                                                 \
  FGPIP_WARNING(10, "Fine-grained pipelining NOT applied: loop " + (x) +       \
                        "  Reason: no partitioning solution for variable " +   \
                        (y) + " in the loop")
#define FGPIP_WARNING_12(x, y)                                                 \
  FGPIP_WARNING(12, "Complete parallelization of loop " + (x) + (y) +          \
                        "\n may be ignored or lead to vendor tool errors "     \
                        "because its tripcount is variable.")
#define FGPIP_WARNING_14(x, y, z, t)                                           \
  FGPIP_WARNING(                                                               \
      14, "Partitioning array " + (y) + " on dimension " + (z) +               \
              " with factor " + (t) +                                          \
              "\n"                                                             \
              "Note that loop " +                                              \
              (x) +                                                            \
              " probably cannot be fully-pipelined due to partially access " + \
              "conflicts of variable " + (y))
// Yuxin: Feb/24/2020
// Not reporting array partitioning to the user
#define FGPIP_WARNING_15(x, y, z, t)                                           \
  FGPIP_WARNING(15, "Parallelizing loop " + (x) +                              \
                        " may lead to excessive resource usage " +             \
                        "and long runtime in downstream tools.")
#define FGPIP_WARNING_16(x, y)                                                 \
  FGPIP_WARNING(16,                                                            \
                "Fine-grained pipelining of loop " + (x) + (y) +               \
                    "\n may be ignored because the loop contains subloop(s) "  \
                    "may not being parallelized.\n")
#define FGPIP_WARNING_17(x, y)                                                 \
  FGPIP_WARNING(17,                                                            \
                "To apply dependency false pragma "                            \
                "on variable " +                                               \
                    (x) + " in loop " + (y) +                                  \
                    " may result in an incorrect hardware implementation.")
#define FGPIP_WARNING_18(x, y)                                                 \
  FGPIP_WARNING(                                                               \
      18, "Ignoring pragma '" + (x) +                                          \
              "'  because of invalid dimension information 'dim=" + (y) + "'")
#define FGPIP_WARNING_19(x, y)                                                 \
  FGPIP_WARNING(19, "To apply fine-grained parallelization on loop " + (x) +   \
                        (y) + " may lead to vendor tool error out " +          \
                        "because loop tripcount is variable. \n")
#define FGPIP_WARNING_21(x, y)                                                 \
  FGPIP_WARNING(21, "Ignoring pragma '" + (x) +                                \
                        "'  because of unknown partitioning factor '" + (y) +  \
                        "'")
#define FGPIP_WARNING_22(x, y, z, t)                                           \
  FGPIP_WARNING(22, "Pipelining loop " + (x) +                                 \
                        " may lead to excessive resource usage " +             \
                        "and long runtime in downstream tools.")

//  ////////////////////////////////////////////  /
//  All QoR Warning Messages (potential sub-optimal)
//  ////////////////////////////////////////////  /
#define PROCS_WARNING_3(x)                                                     \
  PROCS_WARNING(3, "Found a loop " + (x) + " with unknown loop tripcount. ")
#define BURST_WARNING_15(x, y, z, tiled_loop, full_access)                     \
  BURST_WARNING(15, "Low bandwidth efficiency of memory burst: variable " +    \
                        (x) + " in scope " + (z) + ".\n" +                     \
                        "  Reason: short burst length = " + (y) + " bytes\n" + \
                        ((full_access)                                         \
                             ? " due to small external memory"                 \
                             : ((tiled_loop) ? INCREASE_TILING_FATOR : "")))

#define BURST_WARNING_16(x, y, r, s)                                           \
  BURST_WARNING(                                                               \
      16, "Stopping inferring longer memory burst: variable " + (x) +          \
              " in scope " + (y) + ".\n" + "  Reason: required resource (" +   \
              (r) + " bytes)  more than lifting threshold (" + (s) +           \
              " bytes).\n" +                                                   \
              "  Hint: increase the lifting threshold by setting \n" +         \
              "        \'burst_lifting_size_threshold\'")

#define BURST_WARNING_17(x)                                                    \
  BURST_WARNING(                                                               \
      17, "Stopping inferring longer memory burst: variable " + (x) + ".\n" +  \
              "  Reason: required resource is more than the remaining "        \
              "resource.\n" +                                                  \
              "  Hint: increase the total resource threshold by setting \n" +  \
              "        \'burst_total_size_threshold\'")

#define BURST_WARNING_18(x, y, c, r, s)                                        \
  BURST_WARNING(18, "Stopping inferring longer memory burst: variable " +      \
                        (x) + " in scope " + (y) + ".\n" +                     \
                        "  Reason: required resource (" + (r) +                \
                        " bytes) more than single resource "                   \
                        "threshold (" +                                        \
                        (s) + " bytes).\n" +                                   \
                        ((c) ? DECREASE_TILING_FATOR : SHORT_BURST_HINT))

#define BURST_WARNING_19(x, y, z)                                              \
  BURST_WARNING(                                                               \
      19, "Found a possible "                                                  \
          "negative lower bound " +                                            \
              (x) + " in memory burst for array " + (y) + " reference " +      \
              (z) + "\n" + "  Hint: possible solutions are\n" +                \
              "  1. Set accurate bound for variables using 'assert'\n" +       \
              "  2. Please consider shifting the data layout to ensure "       \
              "the lower " +                                                   \
              " bounds are non-negative\n" +                                   \
              "  3. Or consider separating the boundary loop "                 \
              "iterations from the main " +                                    \
              "loop, so that the main loop will always have a "                \
              "non-negative lower bound " +                                    \
              "for memory burst")

#define BURST_WARNING_20                                                       \
  BURST_WARNING(                                                               \
      20, "Found on-chip buffer is almost used out before inferring burst")

#define BURST_WARNING_21(x, y)                                                 \
  BURST_WARNING(21, "Variable " + (x) + " in scope " + (y) +                   \
                        " is set to "                                          \
                        "write_only, which may result in an incorrect design")

#define BURST_WARNING_22(x, y, z)                                              \
  BURST_WARNING(22, "Memory burst NOT inferred: variable " + (x) +             \
                        " in scope " + (y) +                                   \
                        " which is fine grained scope.\n" +                    \
                        " Hint: please apply cache pragma in the scope\n" +    \
                        "  #pragma ACCEL cache variable=" + (z))
#define BURST_WARNING_23(x, y, z)                                              \
  BURST_WARNING(23, "Memory burst NOT inferred: variable " + (x) +             \
                        " in scope " + (z) +                                   \
                        "\n"                                                   \
                        "  Reason: burst start address is variable (" +        \
                        (y) + ")")

#define WDBUS_WARNING_3(x, y)                                                  \
  WDBUS_WARNING(3, "Suboptimal memory coalescing: variable " + (x) +           \
                       "\n  Reason: the lowest dimension size of on-chip "     \
                       "buffer is not divisible by " +                         \
                       (y))
#define WDBUS_WARNING_5(x, y)                                                  \
  WDBUS_WARNING(5, "Suboptimal memory coalescing: variable " + (x) +           \
                       "\n  Reason: the access offset is not divisible by " +  \
                       (y))
#define WDBUS_WARNING_6(x)                                                     \
  WDBUS_WARNING(6, "Suboptimal memory coalescing: variable " + (x) +           \
                       "  Reason:  the lowest dimension of on-chip buffer "    \
                       " is less than the coalescing length")

#define FGPIP_WARNING_1(x)                                                     \
  FGPIP_WARNING(                                                               \
      1,                                                                       \
      "Suboptimal loop parallelization: variable " + (x) +                     \
          "  Reason: too many parallelized accesses on the same array\n"       \
          "  Hint: try to match the access pattens in the different loops\n"   \
          "        with the access pattern in memory coalescing")

#define FGPIP_WARNING_11(x, y)                                                 \
  FGPIP_WARNING(11, "Memory partitioning NOT applied: scalar " + (x) +         \
                        "  \nReason: possible out-of-bound access or invalid " \
                        "parallel factor in loop " +                           \
                        (y) + " ")
#define FGPIP_WARNING_13(x, y)                                                 \
  FGPIP_WARNING(13, "Loop " + (x) +                                            \
                        " probably cannot be fully-pipelined as array " +      \
                        (y) +                                                  \
                        " has an irregular access pattern and cannot be "      \
                        "split into different sub-arrays.")
#define FGPIP_WARNING_3(x)                                                     \
  FGPIP_WARNING(                                                               \
      3, "suboptimal loop parallelization: loop " + (x) +                      \
             "  Reason: Unknown loop bound may disable parallelization.")
#define FGPIP_WARNING_8(x, y)                                                  \
  FGPIP_WARNING(                                                               \
      8, "Ignoring pragma " + (x) +                                            \
             "  because loop-carried dependence exists on variable " + (y))
#define FGPIP_WARNING_20(x)                                                    \
  FGPIP_WARNING(20, "Potentially inefficient variable configuration: "         \
                    "array " +                                                 \
                        (x) +                                                  \
                        "\n Reason: multiple accesses or complex "             \
                        "access pattern. ")

//  ////////////////////////////////////////////  /
//  To be discussed
//  ////////////////////////////////////////////  /
#define PROCS_WARNING_1(x)                                                     \
  PROCS_WARNING(1, "Ignoring all pragmas in the header file " +                \
                       (x))  //  ZP: is it still necessary
#define PROCS_WARNING_10(x)                                                    \
  PROCS_WARNING(                                                               \
      10,                                                                      \
      "Function " + (x) + " with the return statements " +                     \
          "in the middle of the function body is not supported")  // ZP: to
                                                                  // confirm,
                                                                  // seems
//  removable
#define PROCS_WARNING_15(x)                                                    \
  PROCS_WARNING(15, "Found an incomplete pragma: " +                           \
                        (x))  //  ZP: to give more useful info, like the full
                              //  format for the specific pragma
#define PROCS_WARNING_23(x, y)                                                 \
  PROCS_WARNING(23, "Found the access indices of the array " + (x) +           \
                        " are not identical within " + (y) +                   \
                        ".\n   Hints: Coarse-grained pipelining requires "     \
                        "access indices to be identical in each loop.")
#define PROCS_WARNING_26(x)                                                    \
  PROCS_WARNING(26, "Found the access indices of array " + (x) +               \
                        " are not identical"                                   \
                        ".\n  Hints: Array multidimensional delinearization "  \
                        "requires access indices to be identical.")
#define PROCS_WARNING_27(x)                                                    \
  PROCS_WARNING(27, "Loop with the continue/break statement " + (x) +          \
                        " is not supported")
#define DISAG_ERROR_1(x)                                                       \
  DISAG_ERROR(                                                                 \
      1, "Cannot decompose " + (x) +                                           \
             "\n Hint: complex pointer re-interpretation is not supported. " + \
             "Please rewrite")
#define DISAG_ERROR_2(x) DISAG_ERROR(2, (x) + " is not decomposed")
#define DISAG_ERROR_3(x, y)
#define DISAG_ERROR_4(x) DISAG_ERROR(4, "Depth " + (x) + " is not supported")

#define DISAG_ERROR_5(x, y)                                                    \
  DISAG_ERROR(5, (x) + " contains unsupported references:\n" + (y))
#define GLOBL_ERROR_3(x)                                                       \
  GLOBL_ERROR(3, "Cannot modify global pointer in the kernel\n" + (x))
#define GLOBL_ERROR_4(x)                                                       \
  GLOBL_ERROR(                                                                 \
      4,                                                                       \
      "Global variable name conflicts with a local variable or argument\n" +   \
          (x))
#define GLOBL_ERROR_5(x)                                                       \
  GLOBL_ERROR(5, "Uninitialized constant global variable " + (x) +             \
                     " is not supported")
#define GLOBL_ERROR_6(x)                                                       \
  GLOBL_ERROR(6, "Global array variable " + (x) + " with undetermined size" +  \
                     " is not supported")

#define INFTF_ERROR_4(x) INFTF_ERROR(4, x)
#define INFTF_ERROR_5(x) INFTF_ERROR(5, x)

#define INFTF_ERROR_7(x, y, z)                                                 \
  INFTF_ERROR(7, "Found incorrect depth info '" + (y) + "' for port " + (x) +  \
                     "with dimension size " +                                  \
                     (z))  //  why incorrect? expectation?
#define INFTF_ERROR_8(x, y, z)                                                 \
  INFTF_ERROR(8, "Found incorrect max_depth info '" + (y) + "'for port " +     \
                     (x) + "with dimension size " + (z))
#define INFTF_ERROR_9(x)                                                       \
  INFTF_ERROR(9,                                                               \
              "Found non-static pointer " + (x) +                              \
                  " (this pointer may refer to different memory locations)")
#define PRAGM_ERROR_1(x) PRAGM_ERROR(1, x)  //  ?? move the text here
#define PRAGM_ERROR_2(x) PRAGM_ERROR(2, x)
#define PRAGM_ERROR_3(x) PRAGM_ERROR(3, x)
#define PRAGM_ERROR_4(x) PRAGM_ERROR(4, x)
#define PRAGM_ERROR_5(x) PRAGM_ERROR(5, x)
#define PRAGM_ERROR_6(x) PRAGM_ERROR(6, x)
#define PRAGM_ERROR_7(x) PRAGM_ERROR(7, x)
#define PRAGM_WARNING(num, msg) MARS_WARNING(INFTF, msg, num)
#define PRAGM_WARNING_1(x) PRAGM_WARNING(1, x)

#define CGPAR_ERROR_1(x, y)                                                    \
  CGPAR_ERROR(                                                                 \
      1,                                                                       \
      "Undetermined channel size for access '" + (x) + "' in scope '" + (y) +  \
          "'\nHint: Possible solutions: \n  a) Use copy_in/copy_out clause "   \
          "in the parallel pragma\n  b) Set range assertion for the "          \
          "surrouding loops with unknown bounds\n  c) Remove parallel pragma")

#define TILE_ERROR_1(x)                                                        \
  TILE_ERROR(1, "Cannot tile a loop with non-zero start: " + (x))
#define TILE_ERROR_2(x)                                                        \
  TILE_ERROR(2,                                                                \
             "Cannot tile a loop with a bound not divisible by the factor: " + \
                 (x))
#define TILE_ERROR_3(x, y)                                                     \
  TILE_ERROR(3, "Cannot tile an indivisible loop with labels: " + (x) +        \
                    "Reason: the label '" + (y) +                              \
                    "' is targeted by a goto statement.")
#define TILE_ERROR_4(x, y)                                                     \
  TILE_ERROR(3, "Cannot parallel an indivisible loop with labels: " + (x) +    \
                    "Reason: the label '" + (y) +                              \
                    "' is targeted by a goto statement.")

#define AST_IF_WARNING_1(x)                                                    \
  AST_IF_WARNING(1, "Unknown bitwidth for interface type: " + (x) +            \
                        "\n")  // ZP: is it a limitation to error out?

#define FGPIP_ERROR_1(x, y)                                                    \
  FGPIP_ERROR(1, "Unable to completely parallel loop " + (x) + (y) +           \
                     "because its tripcount is variable.")
#define FGPIP_ERROR_2(x, y)                                                    \
  FGPIP_ERROR(2, "Loop fine-grained pipelining CANNOT be applied: loop " +     \
                     (x) + (y) +                                               \
                     "Because unable to achieve non-conflict memory access.")
#define FGPIP_ERROR_3(x, y, z)                                                 \
  FGPIP_ERROR(3, "Found out-of-bound access on variable " + (x) + " in " +     \
                     (y) + " :\n" + (z))
#define FGPIP_ERROR_4(x, y, z)                                                 \
  FGPIP_ERROR(4, "Found out-of-bound access " + (x) + " on variable " + (y) +  \
                     (z))
#define FGPIP_WARNING_9(x, y, z)                                               \
  FGPIP_WARNING(9, "Loop fine-grained parallelization applied: loop " + (x) +  \
                       "\n"                                                    \
                       "  Hint: parallel factor " +                            \
                       (y) + " is adjusted to the loop tripcount " + (z) +     \
                       ".")
#define FGPIP_INFO_1(x, y, z)                                                  \
  FGPIP_INFO(1, "Partitioning array '" + (x) + "' on dimension " + (y) +       \
                    (z))  //  / ZP: to be only displayed internally?
#define FGPIP_INFO_4                                                           \
  FGPIP_INFO(                                                                  \
      4,                                                                       \
      "Automated loop flattening is turned off\n")  // ZP: I understand auto
                                                    //  loop flatten is
                                                    //  globally turned off?

//  ////////////////////////////////////////////  /
//  To be discarded (suppose not happen)
//  ////////////////////////////////////////////  /
#define PROCS_WARNING_4(x)                                                     \
  PROCS_WARNING(4, "Found an class member variable " + (x) + "in the kernel")
#define PROCS_WARNING_7(x)                                                     \
  PROCS_WARNING(7, "Ignoring pragma " + (x) +                                  \
                       " because only complete parallelism is supported.")
#define PROCS_WARNING_8(x, y)                                                  \
  PROCS_WARNING(8, "Ignoring parallel pragma " + (x) + " on the outer loop " + \
                       (y) +                                                   \
                       " when its sub-loops are not fully parallelized.\n" +   \
                       "  Hint: use \"parallel flatten\" clause to "           \
                       "parallelize all the sub-loops")
#define GLOBL_ERROR_1(x)                                                       \
  GLOBL_ERROR(1, "Kernel and all the functions used in a kernel cannot be "    \
                 "declared in the header "                                     \
                 "file, if it contains global variable\n" +                    \
                     (x))
#define GLOBL_ERROR_2(x)                                                       \
  GLOBL_ERROR(2, "Kernel and all the functions used in a kernel cannot be "    \
                 "shared by host, "                                            \
                 "if it contains global variable\n" +                          \
                     (x))
#define INFTF_ERROR_1(x, y)                                                    \
  INFTF_ERROR(                                                                 \
      1, "Kernel and all the functions used in a kernel cannot be declared "   \
         "in the header file, if it contains struct parameters\n"              \
         "    Function name  : " +                                             \
             (x) + "\n    File location  : " + (y))
#define INFTF_ERROR_2(x, y)                                                    \
  INFTF_ERROR(                                                                 \
      2, "Kernel and all the functions used in a kernel  cannot be declared "  \
         "in the header file, if it contains multiple dimension parameters\n"  \
         "    Function name  : " +                                             \
             (x) + "\n    File location  : " + (y))
#define INFTF_ERROR_3(x)                                                       \
  INFTF_ERROR(3, "Invalid interface depth value : " + (x) +                    \
                     "\n    Hint: depth can only be constant integer number "  \
                     "or scalar from function parameter but not expression")
#define INFTF_ERROR_10(x, y, z)                                                \
  INFTF_ERROR(                                                                 \
      10, "We do not support functions with arguments that have both local "   \
          "variables and global variables as parameters in different "         \
          "function calls\n    Function name  : " +                            \
              (x) + "\n    Parameter name  : " + (y) +                         \
              "\n    File location  : " + (z))

#define KWRAP_ERROR_1(x)                                                       \
  KWRAP_ERROR(                                                                 \
      1, "User defined types outside header files is not supported: \n" + (x))
#define FUCIN_ERROR_1(x)                                                       \
  FUCIN_ERROR(                                                                 \
      1, "Non-constant global variable in kernel is not supported: \n" + (x))
#define FUCIN_ERROR_2(x)                                                       \
  FUCIN_ERROR(2, "Host function and kernel function cannot be "                \
                 "defined in the same file, please separate them into "        \
                 "different files. \n" +                                       \
                     (x))
#define FUCIN_ERROR_3(x, y)                                                    \
  FUCIN_ERROR(3, "A kernel function and a function used in the kernel are "    \
                 "defined in the different files.\n"                           \
                 "Kernel function: " +                                         \
                     (y) +                                                     \
                     "\n"                                                      \
                     "Sub function: " +                                        \
                     (x) + "\n  Hint: please merge them into the same file")
#define FUCIN_ERROR_4(x)                                                       \
  FUCIN_ERROR(4,                                                               \
              "Cannot inline memcpy because of non primitive array copy: \n" + \
                  (x) + "Please rewrite")
#define FUCIN_ERROR_5(x)                                                       \
  FUCIN_ERROR(5, "Cannot inline memcpy because of different base types: \n" +  \
                     (x) + "Please rewrite")

#define SWITCH_MESSAGE                                                         \
  ""  //  "\n  The design flow will switch to low effort mode automatically"

#define PROCS_WARNING_2(x, y)                                                  \
  PROCS_WARNING(2, "Found a non-canonical loop " + (x) +                       \
                       " which may affect the quality of the result" +         \
                       ((y) ? SWITCH_MESSAGE : ""))

#define PROCS_WARNING_24(x)                                                    \
  PROCS_WARNING(                                                               \
      24, "Found multiple loops containing the accesses of an local array " +  \
              (x) + ".\n" + SWITCH_MESSAGE)
#define PROCS_WARNING_25(x)                                                    \
  PROCS_WARNING(                                                               \
      25, "Found more than one loop with the accesses of a port array " +      \
              (x) +                                                            \
              ".\n   Hints: Coarse-grained pipelining requires one read-only " \
              "or write-only loop for each port array.\n" +                    \
              SWITCH_MESSAGE)

#define FUCIN_WARNING_1(x, y)                                                  \
  FUCIN_WARNING(1, "Function with void return type called "                    \
                   "as an expression is not supported\n" +                     \
                       (x) + ((y) ? SWITCH_MESSAGE : ""))
#define FUCIN_WARNING_2(x, y)                                                  \
  FUCIN_WARNING(2, "Cannot inline a function with \"return\" in the middle "   \
                   "of function body \n" +                                     \
                       (x) + ((y) ? SWITCH_MESSAGE : ""))
#define FUCIN_WARNING_3(x, y)                                                  \
  FUCIN_WARNING(3, "Cannot inline a function with \"goto\" statement \n" +     \
                       (x) + ((y) ? SWITCH_MESSAGE : ""))

#define COMCK_INFO(num, msg) MARS_INFO(COMCK, msg, num)
#define COMCK_WARNING(num, msg) MARS_WARNING(COMCK, msg, num)
#define COMCK_ERROR(num, msg) MARS_ERROR(COMCK, msg, num)

#define COMCK_WARNING_1(x)                                                     \
  COMCK_WARNING(1, "Non-canonical loop " + (x) + " is not supported")
#define COMCK_WARNING_2(x)                                                     \
  COMCK_WARNING(2, "Loop " + (x) + " without the simple bound is not supported")
#define COMCK_WARNING_3(x)                                                     \
  COMCK_WARNING(3, "Loop with the continue/break statement " + (x) +           \
                       " is not supported")
#define COMCK_WARNING_4(x, y)                                                  \
  COMCK_WARNING(4, "The Communication on the shared port " + (x) +             \
                       " whose references are on either the loop " + (y) +     \
                       " or if condition cannot be supported.")
#define COMCK_WARNING_5(x)                                                     \
  COMCK_WARNING(5, "Found a conditionally executed loop: \n " + (x) +          \
                       " \n  Coarse-grained pipeline does not allow loop "     \
                       "within if-statement")
#define COMCK_WARNING_6(x)                                                     \
  COMCK_WARNING(6, "Pointer alias as shared port " + (x) + " is not supported")
#define COMCK_WARNING_7(x)                                                     \
  COMCK_WARNING(7, "While/do-while loop " + (x) + " is not supported")

#define STREAM_ERROR(num, msg) MARS_ERROR(STREAM, msg, num)
#define STREAM_ERROR_1(x)                                                      \
  STREAM_ERROR(1, "cannot detect access patten for variable: " + (x))
#define STREAM_ERROR_2(x, y)                                                   \
  STREAM_ERROR(2, "stream port " + (x) + " cannot be separated:\n    " + (y))
#define STREAM_ERROR_3(x)                                                      \
  STREAM_ERROR(3, "spawn pragma " + (x) +                                      \
                      " must be defined before singal "                        \
                      "function call statement")
#define STREAM_ERROR_4(x)                                                      \
  STREAM_ERROR(4, "spawn function " + (x) + " can not have return inside")
#define STREAM_ERROR_5(x)                                                      \
  STREAM_ERROR(                                                                \
      5, "Found merlin_stream variable usage in " + (x) + "\n" +               \
             "merlin_stream variable must be used in function call named:\n"   \
             "merlin_stream_init\n"                                            \
             "merlin_stream_reset\n"                                           \
             "merlin_stream_write\n"                                           \
             "merlin_stream_read\n")
#define STREAM_ERROR_6(x)                                                      \
  STREAM_ERROR(6, "Found spawn kernel interface not from "                     \
                  "original kenrel interface : " +                             \
                      (x) + "\n")
#define STREAM_ERROR_7(x)                                                      \
  STREAM_ERROR(7, "Spawn pragma with parallel factor : " + (x) +               \
                      "\n"                                                     \
                      "must defined inside for loop.")
#define STREAM_ERROR_8(x)                                                      \
  STREAM_ERROR(8, "Spawn pragma with parallel factor : " + (x) +               \
                      "\n"                                                     \
                      "the factor must equal to the loop bound.")

#define STREAM_WARNING(num, msg) MARS_WARNING(STREAM, msg, num)
#define STREAM_WARNING_1(x)                                                    \
  STREAM_WARNING(1, "stream ports cannot be separated:\n    " + (x))
#define STREAM_WARNING_2(x)                                                    \
  STREAM_WARNING(                                                              \
      2, "Streaming port " + (x) +                                             \
             " with multiple accesses may result in QoR "                      \
             "degradation.\n" +                                                \
             "  Hint: please merge multiple accesses into single access")
#define STREAM_WARNING_3                                                       \
  STREAM_WARNING(3, "Streaming for Xilinx platform is not supported "          \
                    "for C design.\n Hint: please change into C++ design")
#define STREAM_WARNING_4(port_info)                                            \
  STREAM_WARNING(4,                                                            \
                 "Streaming checking failed for variable " + (port_info) +     \
                     ".\n  Hint: only support read-only or write-only, single" \
                     ", unconditional and sequential access")

#define STREAM_WARNING_5(port_info, loop_info)                                 \
  STREAM_WARNING(5, "Disable streaming for variable " + (port_info) +          \
                        " because of data reuse in loop " + (loop_info))
#define STREAM_WARNING_6(port_info, loop_info)                                 \
  STREAM_WARNING(6, "Disable streaming for variable " + (port_info) +          \
                        " because of coarse grained paralleled loop " +        \
                        (loop_info))
#define STREAM_WARNING_7(port_info, loop_info)                                 \
  STREAM_WARNING(7, "Disable streaming for variable " + (port_info) +          \
                        " because of partial paralleled loop " + (loop_info))
#define STREAM_WARNING_8(port_info, parallel_factor, wide_bus_factor)          \
  STREAM_WARNING(                                                              \
      8, "Disable streaming for variable " + (port_info) +                     \
             " because of incompatible coalescing and computation rates " +    \
             "(parallel fator = " + (parallel_factor) +                        \
             ", coalescing factor = " + (wide_bus_factor) + ")")
#define STREAM_WARNING_9(port_info, start_address, burst_length,               \
                         wide_bus_factor)                                      \
  STREAM_WARNING(                                                              \
      9,                                                                       \
      "Disable streaming for variable " + (port_info) +                        \
          " because of unaligned start address or unaligned access length " +  \
          "(start address = " + (start_address) +                              \
          ",  access length = " + (burst_length) + ", " +                      \
          "coalescing factor = " + (wide_bus_factor) + ")")
#define STREAM_WARNING_10(port_info, loop_info)                                \
  STREAM_WARNING(10, "Disable streaming for variable " + (port_info) +         \
                         " because of unsupported paralleled loop " +          \
                         (loop_info))
#define STREAM_WARNING_11(port_info)                                           \
  STREAM_WARNING(                                                              \
      11, "Streaming checking failed for variable " + (port_info) +            \
              " because of multiple accesses.\n  Hint: please merge " +        \
              "multiple continuous single accesses into a paralleled loop")
#define STREAM_WARNING_12(port_info)                                           \
  STREAM_WARNING(                                                              \
      11, "Streaming checking failed for variable " + (port_info) +            \
              " because of complex write access.\n  Hint: only simple " +      \
              "assignment is supported")
#define STREAM_WARNING_13(port_info, streaming_length)                         \
  STREAM_WARNING(13, "Disable streaming for variable " + (port_info) +         \
                         " because of too short length (" +                    \
                         (streaming_length) + " elements")
#define STREAM_INFO(num, msg) MARS_INFO(STREAM, msg, num)
#define STREAM_INFO_1(port_info, scope_info, length, wide_bus_factor)          \
  STREAM_INFO(1, "Streaming " + (length) + " elements of variable " +          \
                     (port_info) + " in scope " + (scope_info) +               \
                     " with a coalescing factor of " + (wide_bus_factor))
#define FETCH_WARNING_1(x)                                                     \
  FETCH_WARNING(                                                               \
      1, "we do not support multiple access in stream for variable: " + (x))
#define FETCH_WARNING(num, msg) MARS_WARNING(FETCH, msg, num)

#define CMOPT_INFO(num, msg) MARS_INFO(CMOPT, msg, num)
#define CMOPT_WARNING(num, msg) MARS_WARNING(CMOPT, msg, num)
#define CMOPT_ERROR(num, msg) MARS_ERROR(CMOPT, msg, num)

#define CMOPT_ERROR_1(x)                                                       \
  CMOPT_ERROR(                                                                 \
      1, "Cannot apply the synchronization on the non-canonical loop " + (x))

#define CMOPT_WARNING_1(x)                                                     \
  CMOPT_WARNING(                                                               \
      1, "Coarse-grained dataflow pipelining cannot be generated because " +   \
             (x) + " is accessed in more than two loops")
#define CMOPT_WARNING_2(x)                                                     \
  CMOPT_WARNING(                                                               \
      2, "Coarse-grained dataflow pipelining cannot be generated because " +   \
             (x) + " is read in more than one loop")
#define CMOPT_WARNING_3(x)                                                     \
  CMOPT_WARNING(                                                               \
      3, "Coarse-grained dataflow pipelining cannot be generated because " +   \
             (x) + " is writen in more than one loop")
#define CMOPT_WARNING_4(x)                                                     \
  CMOPT_WARNING(4, "Coarse-grained dataflow pipelining cannot be generated "   \
                   "becuase of the possible backward dependency " +            \
                       (x))
#define CMOPT_WARNING_5(x)                                                     \
  CMOPT_WARNING(                                                               \
      5, "Coarse-grained dataflow pipelining cannot be generated because " +   \
             (x) + " is accessed multiple times in a loop")
#define CMOPT_WARNING_6(x)                                                     \
  CMOPT_WARNING(6, "Coarse-grained dataflow pipelining cannot be generated "   \
                   "because the indices of array " +                           \
                       (x) + " is too complex")
#define CMOPT_WARNING_7(x)                                                     \
  CMOPT_WARNING(7, "Coarse-grained dataflow pipelining cannot be generated "   \
                   "because the range of the read and write access to " +      \
                       (x) + " is not matched")
#define CMOPT_WARNING_8(x)                                                     \
  CMOPT_WARNING(8, "Coarse-grained dataflow pipelining cannot be generated "   \
                   "because the parallelism of two references to " +           \
                       (x) + " is not matched")
#define CMOPT_WARNING_9(x)                                                     \
  CMOPT_WARNING(9, "Coarse-grained dataflow pipelining cannot be generated "   \
                   "because of synchronization on host interface " +           \
                       (x))
#define CMOPT_WARNING_10(x)                                                    \
  CMOPT_WARNING(10, "Coarse-grained dataflow pipelining cannot be generated "  \
                    "because the index patterns of two references to " +       \
                        (x) + " are not matched")

#define CMOPT_INFO_1(x)                                                        \
  CMOPT_INFO(1, "Setting the variable " + (x) + " into FIFO channel type")

#define MSG_RPT_WARNING_1(x)                                                   \
  MSG_RPT_WARNING(1, "Define varaible " + (x) +                                \
                         " in header file is not a good "                      \
                         "practice")
