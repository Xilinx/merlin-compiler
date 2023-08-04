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
typedef unsigned char uint8_t;

struct hvm_intack
{
  uint8_t source;
  uint8_t vector;
}
;
enum hvm_intsrc {hvm_intsrc_none=0,hvm_intsrc_pic=1,hvm_intsrc_lapic=2,hvm_intsrc_nmi=3};


struct vcpu
{
  int vcpu_id;
  int processor;
}
;
#pragma ACCEL kernel name="hvm_vcpu_has_pending_irq"

struct hvm_intack hvm_vcpu_has_pending_irq(struct vcpu *v)
{
  return (struct hvm_intack ){(hvm_intsrc_nmi), (2)};
}

