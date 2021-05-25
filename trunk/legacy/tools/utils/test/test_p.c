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




//; my $scalar   = param_define("scalar_one", "default");
//; my @array    = param_array_define("array_one", "default");
//; my $tree_var = param_tree_define("item_one", "", "force_arr0 force_arr1");
//; my $all_tree = param_tree_define("", "", "param");
//; my $input_file_tree = param_tree_define("dummy", "another.cfg.xml", "");

//; # is the line prefix for the comments, pls make sure there is a space after
//#

//; # for param_tree_define:
//; #     the second parameter ("" by default) indicates a xml file name for the
// tree_var. ; #     If the tree id 'item_one' is "", the xml root of the
// configure xml file will be return. (like all_tree) ; #     If the tree id
//'item_one' is not "" and not defined in the cfg file, the xml root of this ; #
// new xml file will be used for variable tree_var, and in this case the tree id
//; #     'item_one' will be ignored.

Here is a scalar : `$scalar` Here is a vector
    : `$array[0]` `$array[1]` Here is the value of tree_var.cost : `$tree_var->{
  cost
}
` Here is the value of tree_var.value : `$tree_var->{ value }
` Here is the name of force_arr0[0] : `$tree_var->{force_arr0}[0]->{ name }
` Here is the name of force_arr0[0] : `$tree_var->{force_arr0}[0]->{ name }
` Here is a parameter in the un - named tree : `$all_tree->{param}[0]->{ name }
`=`$all_tree->{param}[0]->{ content }
`

    Here is a parameter from another cfg file : param_name =`$input_file_tree->{
  additional_param
}
`

    //; my $number = @{$tree_var->{force_arr0}};
    //; my @array_local = @{$tree_var->{force_arr0}};
    //; my $number1 = $#array_local;
    Array force_arr0 has element numer : `$number`

    //; foreach $element (@{$tree_var->{force_arr0}}) {
    Here is the name of force_arr0 : `$element->{
  name
}
`
    //; }

    //; #include "in.h"
