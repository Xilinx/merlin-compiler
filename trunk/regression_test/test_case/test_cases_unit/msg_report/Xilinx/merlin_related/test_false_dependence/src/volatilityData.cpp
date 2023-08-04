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
/*----------------------------------------------------------------------------
 *
 * Author:   Liang Ma (liang-ma@polito.it)
 *
 *----------------------------------------------------------------------------
 */

#include "volatilityData.h"
volData::volData(data_t expect, data_t kappa, data_t variance, data_t initValue,
                 data_t correlation) {
  this->expect = expect;
  this->kappa = kappa;
  this->variance = variance;
  this->initValue = initValue;
  this->correlation = correlation;
}

volData::volData(const volData &data) {
  this->expect = data.expect;
  this->kappa = data.kappa;
  this->variance = data.variance;
  this->initValue = data.initValue;
  this->correlation = data.correlation;
}

data_t volData::truncFun1(data_t x) { return x; }
data_t volData::truncFun2(data_t x) { return fmax(x, 0); }
data_t volData::truncFun3(data_t x) { return fmax(x, 0); }

#ifdef __CLIANG__
using namespace std;
void volData::print() const {
  cout << "expect:" << expect << ' ' << "variance:" << variance << ' '
       << "initValue:" << initValue << ' ' << "correlation:" << correlation
       << endl;
}
#endif
