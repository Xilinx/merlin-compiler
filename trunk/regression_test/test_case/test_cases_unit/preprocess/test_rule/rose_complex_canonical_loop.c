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
#pragma ACCEL kernel

void top()
{
  unsigned short k;
  unsigned short l;
  k = ((unsigned short )0);
/* Canonicalized from: for((k =((unsigned short )0) , l =((unsigned short )0));(((int )k) <((int )9) ,((int )l) <((int )16));(k +=((int )9) , l +=((int )16))) {...} */
/* Standardize from: for(l =((unsigned short )0);l <= 15;l += 16) {...} */
  for (l = 0; l <= 0; l++) {
    unsigned short _in_l = 0 + 16L * l;
    if ((((int )k) < ((int )9) , ((int )_in_l) < ((int )16))) {
      k += ((int )9);
    }
  }
  l = 0 + 16L;
}
