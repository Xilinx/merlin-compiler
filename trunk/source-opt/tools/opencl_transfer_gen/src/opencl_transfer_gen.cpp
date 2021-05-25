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


//  ******************************************************************************************//
//  module           :   kernel wrapper
//  description      :   generate a wrapper for specified kernel
//  input            :
//  output           :
//  limitation       :
//  author           :   Han
//  ******************************************************************************************//
#include <iostream>
#include <string>
#include "math.h"
#include <boost/algorithm/string.hpp>

#include "codegen.h"
#include "ir_types.h"
#include "mars_opt.h"
#include "opencl_transfer_gen.h"
#include "rose.h"
//  #include "inliner.h"
// using namespace std;
//  using namespace SageInterface;
//  using namespace SageBuilder;
extern void GetTLDMInfo_withPointer4(void *sg_node, void *pArg);

int opencl_transfer_gen(CSageCodeGen *codegen, void *pTopFunc,
                        const CInputOptions &options) {
  printf("Hello opencl_transfer_gen...\n");
  size_t i;
  vector<pair<void *, string>> vecTldmPragmas;
  codegen->TraverseSimple(pTopFunc, "preorder", GetTLDMInfo_withPointer4,
                          &vecTldmPragmas);
  for (i = 0; i < vecTldmPragmas.size(); i++) {
    string sFilter;
    string sCmd;
    map<string, pair<vector<string>, vector<string>>> mapParams;
    tldm_pragma_parse_whole(vecTldmPragmas[i].second, &sFilter, &sCmd,
                            &mapParams);
    boost::algorithm::to_upper(sFilter);
    boost::algorithm::to_upper(sCmd);

    if (CMOST_PRAGMA != sFilter && ACCEL_PRAGMA != sFilter &&
        TLDM_PRAGMA != sFilter) {
      continue;
    }
    if ("KERNEL" != sCmd) {
      continue;
    }
    printf("Find a kernel pragma : %s\n", vecTldmPragmas[i].second.c_str());

    void *sg_task_stmt =
        codegen->GetChildStmt(codegen->GetNextStmt(vecTldmPragmas[i].first), 0);
    assert(codegen->IsExprStatement(sg_task_stmt));
    void *sg_kernel_call = codegen->GetExprFromStmt(sg_task_stmt);
    assert(sg_kernel_call);

    //  for (j = 0; j < vecTldmPragmas.size(); j++)
    //  {
    //    string sFilter_w, sCmd_w;
    //    map<string, pair<vector<string>, vector<string> > > mapParams_w;
    //    tldm_pragma_parse_whole(vecTldmPragmas[j].second, sFilter_w, sCmd_w,
    //    mapParams_w); boost::algorithm::to_upper(sFilter_w);
    //    boost::algorithm::to_upper(sCmd_w);
    //
    //    if (CMOST_PRAGMA != sFilter_w and ACCEL_PRAGMA != sFilter_w
    //        and TLDM_PRAGMA != sFilter_w) {
    //        continue;
    //    }
    //    if ("WRAPPER" != sCmd_w){
    //        continue;
    //    }
    //    printf("Find a wrapper pragma : %s\n",
    //    vecTldmPragmas[j].second.c_str());
    //
    //    if (mapParams_w["variable"].first.size() > 0) {
    //        printf("variable = %s\n",
    //        mapParams_w["variable"].first[0].c_str());
    //    } else {
    //        printf("Error : Did not generate attrribute variable.\n");
    //    }
    //  }
  }
  return 0;
}
