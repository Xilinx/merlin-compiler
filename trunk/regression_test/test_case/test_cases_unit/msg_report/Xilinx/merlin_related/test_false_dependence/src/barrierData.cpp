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

#include "barrierData.h"
#ifdef __CLIANG__
#include <iostream>
using namespace std;
#endif
barrierData::barrierData(data_t upBarrier, data_t downBarrier) {
  if (upBarrier < downBarrier) {
    this->upBarrier = 0;
    this->downBarrier = 0;
  } else {
    this->upBarrier = upBarrier;
    this->downBarrier = downBarrier;
  }
}

barrierData::barrierData(const barrierData &data) {
  this->upBarrier = data.upBarrier;
  this->downBarrier = data.downBarrier;
}

bool barrierData::checkRange(data_t stockPrice) const {
  return (stockPrice >= downBarrier) && (stockPrice <= upBarrier);
}

#ifdef __CLIANG__
void barrierData::print() const {
  cout << "downBarrier=" << downBarrier << ' ' << "upBarrier=" << upBarrier
       << endl;
}
#endif
