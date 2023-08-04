//(C) Copyright 2016-2021 Xilinx, Inc.
//All Rights Reserved.
//
//Licensed to the Apache Software Foundation (ASF) under one
//or more contributor license agreements.  See the NOTICE file
//distributed with this work for additional information
//regarding copyright ownership.  The ASF licenses this file
//to you under the Apache License, Version 2.0 (the
//"License"); you may not use this file except in compliance
//with the License.  You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
//Unless required by applicable law or agreed to in writing,
//software distributed under the License is distributed on an
//"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
//KIND, either express or implied.  See the License for the
//specific language governing permissions and limitations
//under the License. (edited)
unsigned char p_Z9factorialIhEh(unsigned char n);
unsigned int p_Z9factorialIjEj(unsigned int n);

unsigned int p_Z9factorialIjEj(unsigned int n)
{
  return n + ((unsigned int )1);
}
static void __merlin_dummy_SgTemplateFunctionDeclaration_template_();
// should be ignored.
static void __merlin_dummy_SgTemplateInstantiationFunctionDecl_unsigned_c_();
template < typename T > void use_result ( T t ) { t; }
static void __merlin_dummy_kernel_pragma();
// should not insert statements between pragma and kernel function
extern "C" {

void foo()
{
  unsigned int arg = 23;
// implicit instantiation.
  p_Z9factorialIjEj(arg);
}
}
