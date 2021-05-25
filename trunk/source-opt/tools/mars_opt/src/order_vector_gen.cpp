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


#include <stdlib.h>

#include <iostream>

#include "cmdline_parser.h"
#include "file_parser.h"
#include "xml_parser.h"

#include "PolyModel.h"
#include "codegen.h"
#include "mars_opt.h"

#define PRINT_INFO 1

//  also defined in codegen->cpp
#define USE_LOWERCASE_NAME 1

//  Moved to codegen->cpp

//  Edit by Mo
/*string order_vector_gen(CSageCodeGen & codegen, void * sg_node_scope, void *
sg_node) //task: sg_node = basicblock task access: sg_node = one reference // Mo
{
    vector<string> order_vec;
    vector<string> iterators, conditions, lower, upper;
    vector<int> loop_indices;
    if (codegen->IsBasicBlock(sg_node))
    {
        int ret = codegen->analyze_iteration_domain_order(sg_node_scope,
sg_node, iterators, loop_indices, conditions, lower, upper); if (ret == -1)
        {
            //  continue;
            printf("ret = 1!\n");
            exit(1);
        }
    }
    assert(iterators.size() + 1 == loop_indices.size());
    int i;
    for (i = 0; i < iterators.size(); i++)
    {
        order_vec.push_back(my_itoa(loop_indices[i]));
        order_vec.push_back(iterators[i]);
    }
    order_vec.push_back(my_itoa(loop_indices[i]));
    //  convert to a string
    string order_vec_string;
    for (i = 0; i < order_vec.size() - 1; i++)
    {
        order_vec_string += order_vec[i];
        order_vec_string += ",";
    }
    order_vec_string += order_vec[i];

    return order_vec_string;
}
*/
//  For tldm access function
/*string order_vector_gen(CSageCodeGen & codegen, void * sg_node)
{
    int i = 0;
    void * sg_node_inner_loop;
    void * sg_node_scope;
    void * sg_pre;
    bool found = false;
    while (1)
    {
        sg_pre = sg_node;
        sg_node = codegen->GetParent(sg_node);
        if (!found && codegen->IsForStatement(sg_node))
        {
            sg_node_inner_loop = sg_pre;
            found = true;
        }
        if (codegen->IsFunctionDefinition(sg_node))
        {
            sg_node_scope = sg_pre;
            break;
        }
    }
    string order_vec_string = order_vector_gen(codegen, sg_node_scope,
sg_node_inner_loop); return order_vec_string;
}*/
