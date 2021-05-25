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


/* Enabling traces.  */
#ifndef YYDEBUG
#define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Tokens.  */
#ifndef YYTOKENTYPE
#define YYTOKENTYPE
/* Put the tokens into the symbol table, so that GDB and other debuggers
   know about them.  */
enum yytokentype {
  INTEGER = 258,
  CC = 259,
  PT = 260,
  PPT = 261,
  comment = 262,
  identifier = 263,
  RANGE = 264,
  BR_LEFT = 265,
  BR_RIGHT = 266,
  LEFT_QUOTE = 267,
  RIGHT_QUOTE = 268,
  QUOTE_INFO = 269,
  LADD = 270,
  LSUB = 271,
  LT = 272,
  GT = 273,
  EQ = 274,
  LE = 275,
  GE = 276
};
#endif
/* Tokens.  */
#define INTEGER 258
#define CC 259
#define PT 260
#define PPT 261
#define comment 262
#define identifier 263
#define RANGE 264
#define BR_LEFT 265
#define BR_RIGHT 266
#define LEFT_QUOTE 267
#define RIGHT_QUOTE 268
#define QUOTE_INFO 269
#define LADD 270
#define LSUB 271
#define LT 272
#define GT 273
#define EQ 274
#define LE 275
#define GE 276

#if !defined YYSTYPE && !defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE {
/* Line 2058 of yacc.c  */
#line 44 "tldm.y"

  int iValue; /* integer value */
  class CXMLNode *pNode;

/* Line 2058 of yacc.c  */
#line 105 "y.tab.h"
} YYSTYPE;
#define YYSTYPE_IS_TRIVIAL 1
#define yystype YYSTYPE /* obsolescent; will be withdrawn */
#define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse(void *YYPARSE_PARAM);
#else
int yyparse();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse(void);
#else
int yyparse();
#endif
#endif /* ! YYPARSE_PARAM */

#endif  // TRUNK_LEGACY_TOOLS_PARSERS_SRC_TLDM_Y_TAB_H_
