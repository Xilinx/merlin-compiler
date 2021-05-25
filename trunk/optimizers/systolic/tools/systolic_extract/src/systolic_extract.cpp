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



#include "xml_parser.h"
#include "cmdline_parser.h"
#include "file_parser.h"

#include "mars_opt.h"
#include "PolyModel.h"
#include "tldm_annotate.h"
#include "codegen.h"

#include "mars_ir_v2.h"
#include "array_delinearize.h"

#include "SystolicData.h"
#include "index_transform_codegen.h"

int systolic_extract_top(CSageCodeGen *codegen, void *pTopFunc,
                         const CInputOptions &options) {
  CMarsIrV2 mars_ir;
  mars_ir.build_mars_ir(codegen, pTopFunc);

  vector<void *> all_pragmas;
  codegen->GetNodesByType(nullptr, "preorder", "SgPragmaDeclaration",
                          &all_pragmas);

  CSageCodeGen *ast = codegen;

  vector<void *> pragmas;
  for (auto pragma : all_pragmas) {
    if ("systolic" == mars_ir.get_pragma_attribute(pragma, "systolic")) {
      pragmas.push_back(pragma);
    }
  }

  for (auto pragma : pragmas) {
    cout << "Processing a pragma: " << ast->_up(pragma) << endl;

    if ("preprocess" == mars_ir.get_pragma_attribute(pragma, "preprocess")) {
      cout << "Preprocess pragma" << endl;
      // Processing proprocess to an OpenCL kernel
    } else if ("postprocess" ==
               mars_ir.get_pragma_attribute(pragma, "postprocess")) {
      cout << "Postprocess pragma" << endl;
      // Processing postprocess to an OpenCL kernel
    } else {
      // Processing systolic array to XML files
      SystolicData sys_data(pragma, ast, &mars_ir);
      sys_data.Analyze();
      // sys_data.toString();
      sys_data.DumpXML("systolic_cfg.xml");

      // audo generate index transform code
      SystolicIndexTransformCodegen systolic_codegen(pragma, ast, &mars_ir);
      systolic_codegen.autoCodegen();
    }
  }
  return 0;
}
