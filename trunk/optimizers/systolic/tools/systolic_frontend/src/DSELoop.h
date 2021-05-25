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


#ifndef TRUNK_OPTIMIZERS_SYSTOLIC_TOOLS_SYSTOLIC_FRONTEND_SRC_DSELOOP_H_
#define TRUNK_OPTIMIZERS_SYSTOLIC_TOOLS_SYSTOLIC_FRONTEND_SRC_DSELOOP_H_

#include <vector>
#include "codegen.h"
#include "SystolicData.h"

using std::vector;

class DSELoop {
 private:
  void *loop;
  const int INCREASE_FACTOR;
  enum SystolicData::SYSTOLIC_PE_ARR systolicType;
  int systolicSize;
  int tileSize;
  /*
   * xuechao: 04/27/2019
   * tag: borrowd from SystolicData, to group loop
   * variables into reduction and non-reduction
   * */
  int tag;
  //    vector<int> outerSize;
  vector<int> tripCount;
  int maxTripCount;
  const int THRESHOLD;

 public:
  explicit DSELoop(int step, int threshold = 5)
      : INCREASE_FACTOR(step), THRESHOLD(threshold) {
    maxTripCount = 0;
    Reset();
  }

  void Reset() {
    systolicSize = 0;
    ResetTileSize();
  }

  void SetLoopIter(void *l) { loop = l; }
  void *GetLoopIter() { return loop; }

  void SetLoopTag(int t) { tag = t; }
  int GetLoopTag() { return tag; }

  void SetTripCount(vector<int> tcs) {
    tripCount = tcs;
    maxTripCount = *(max_element(tcs.begin(), tcs.end()));
  }

  vector<int> GetTripCount() { return tripCount; }

  int GetMaxTripCount() { return maxTripCount; }

  int GetThreshold() { return THRESHOLD; }

  void SetAsSystolicLoop(int idx) {
    switch (idx) {
    case 0:
      systolicType = SystolicData::SIMD;
      break;
    case 1:
      systolicType = SystolicData::ROW;
      break;
    case 2:
      systolicType = SystolicData::COL;
      break;
    }
  }

  enum SystolicData::SYSTOLIC_PE_ARR GetSystolicType() {
    assert(systolicSize != 0);
    return systolicType;
  }

  void SetSystolicSize(int size) { systolicSize = size; }

  int GetSystolicSize() { return systolicSize; }

  bool IsSystolicLoop() { return (systolicSize != 0); }

  void ResetTileSize() {
    /*
     * xuechao: 04/27/2019
     * 1. skip the loops with upper bounds less than
     * some threshold, and put it into feeder
     * 2. skip the loops with tag 1 (reduction but not
     * pe_simd variable)
     */
    if ((maxTripCount > THRESHOLD) && (tag != 1)) {
      //    tileSize = 0;
      tileSize = 1;
    } else {
      tileSize = maxTripCount;
    }
    /*
        tileSize = 0;
            for (int i = 0; i < outerSize.size(); i++)
            {
            if (systolicSize)
            outerSize[i] = ceil((float) tripCount[i] / systolicSize);
            else
            outerSize[i] = tripCount[i];
            }
            */
  }

  void SetTileSize(int size) {
    if (systolicSize != 0) {
      if (size <= maxTripCount / systolicSize) {
        tileSize = size;
      } else {
        tileSize = maxTripCount / systolicSize;
      }
    } else {
      if (size <= maxTripCount) {
        tileSize = size;
      } else {
        tileSize = maxTripCount;
      }
    }

    /*
       for (int i = 0; i < outerSize.size(); i++) {
       if (systolicSize) {
       if (tripCount[i] > tileSize * systolicSize)
       outerSize[i] = ceil((float) tripCount[i] / (tileSize * systolicSize));
       }
       else {
       if (tripCount[i] > tileSize)
       outerSize[i] = ceil((float) tripCount[i] / tileSize);
       }
       }
       */
  }

  void IncTileSize() { IncTileSize(INCREASE_FACTOR); }

  void IncTileSize(int f) {
    if (tileSize == 0) {
      SetTileSize(1);
    } else if (f == 1) {
      SetTileSize(tileSize + 1);
    } else {
      SetTileSize(tileSize + f);
    }
  }

  int GetTileSize() { return tileSize; }
  /*
     vector<int> GetOuterSize() {
     return outerSize;
     }
     */
};

#endif  // TRUNK_OPTIMIZERS_SYSTOLIC_TOOLS_SYSTOLIC_FRONTEND_SRC_DSELOOP_H_
