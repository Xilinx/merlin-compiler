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


#include <time.h>
#include <sys/time.h>
#include <iterator>
#include <vector>
#include <set>
#include <string>
#include <algorithm>
#include <fstream>
#include <utility>
#include <list>
#include <map>

#include "xml_parser.h"
#include "cmdline_parser.h"
#include "file_parser.h"

#include "mars_opt.h"
#include "PolyModel.h"
#include "tldm_annotate.h"
#include "codegen.h"
#include "program_analysis.h"

#include "mars_ir_v2.h"

#include "DSELoop.h"
#include "SystolicData.h"
#include "systolic_frontend.h"
using std::list;

#define DEMO_MODE 0
#define DEV_MODE 1
#define EXPERIMENT_SEARCH_WHOLE_DESIGN_SPACE
#define ENABLE_STUCK_FILTER 1

#if DEV_MODE == 1
#define PASS_NAME "systolic_frontend"
#define SYS_FRONT_LOG
// #define SYS_FRONT_FINEST "[" << PASS_NAME << "][" << __FUNCTION__ << "]
// FINEST: " #define SYS_FRONT_FINE "[" << PASS_NAME << "][" << __FUNCTION__ <<
//"] FINE: "
#define SYS_FRONT_INFO "[" << PASS_NAME << "][" << __FUNCTION__ << "] INFO: "
#define SYS_FRONT_WARN "[" << PASS_NAME << "][" << __FUNCTION__ << "] WARNING: "
#define SYS_FRONT_ERROR "[" << PASS_NAME << "][" << __FUNCTION__ << "] ERROR: "
#else
// #define SYS_FRONT_LOG
#define SYS_FRONT_INFO "INFO: "
#define SYS_FRONT_WARN "WARNING: "
#define SYS_FRONT_ERROR "ERROR: "
#endif

// #define REMOVE_CANDIDATES_EARLY

char workingStrMap[] = {'|', '/', '-', '\\'};
int currWorkingStr = 0;

std::ofstream dseLog;

int GetRoundUpPow2(int v) {
  return static_cast<int>(pow(2.0, ceil(log10(v) / log10(2))));
}

int CalcFeederBlocks(int size, int num) {
  // One feeder: ((float(4) * double_buf(2) * size) / KB(1024)) / BRAM_BLK(2)
  // +4 due to Altera flow with an unknown reason
  int block = ((4 * 2 * size) / 1024) / 2 + 4;
  return block * num;
}

float CalcSystolicArrayScore(float thrpt, int simd, int row, int col) {
  float per = 10.0f * thrpt;
  int res = (SYS_RES_BOUND / simd) - abs(row - col);
  float bal = static_cast<float>(res) / 100.0f;
  float vec = (simd == 4 || simd == 8) ? 1 : 0.1f;

  return per + bal + vec;
}

// Select three loops from the set to form a systolic array.
void SystolicArrayLoopExploration(CSageCodeGen *codegen,
                                  list<vector<pair<void *, int>>> *candidates,
                                  SystolicData *sysDataImpl) {
  SystolicData &data = *sysDataImpl;
  vector<vector<int>> sysSizes;

  map<void *, vector<int>> mapLoop2TCs = data.GetLoopTCs();
  set<void *> setLoopIters = data.GetLoopIters();
  map<void *, int> tagmap = data.GetTagMap();

  // Generate design space for systolic array configuration
  for (int i = 8; i >= 1; i /= 2) {  // SIMD size
    // for (int i = 16; i >= 1; i /= 2) { // SIMD size
    int numPE = SYS_RES_BOUND / i;

#ifdef EXPERIMENT_SEARCH_WHOLE_DESIGN_SPACE
    // Search a whole design space: R * C * V <= resource bound
#if (ENABLE_STUCK_FILTER == 0)
    for (int j = 2; j < floor(sqrt(numPE)); j++) {
      for (int k = j; k < floor(numPE / j); k++)
        sysSizes.push_back({i, j, k});
    }
#else
    /*
     * xuechao: 04/23/2019
     * fix j as pe_row, i as pe_simd
     * PE_ROW <= (BN_K*BS_K+1)/2
     * */
    // for (int j = 2; j < floor(sqrt(numPE)); j++)
    for (int j = 1; j < floor(sqrt(numPE)); j++) {
      int bound = 1;
      for (auto iter : setLoopIters) {
        // collect all reduction variables
        if (tagmap[iter] > 0) {
          int tc = mapLoop2TCs[iter][0];

          bound *= tc;
        }
      }

      bound = (bound + i - 1) / i;
      //    bound = (bound + 2 - 1) / 2;

      if (2 * j > bound) {
        continue;
      }

      for (int k = j; k < numPE / j; k++) {
        if (((k + j - 1) / j) > 16) {
          continue;
        }
        sysSizes.push_back({i, j, k});
      }
    }
#endif  // ENABLE_STUCK_FILTER
#else
#if (ENABLE_STUCK_FILTER == 0)
    for (int j = 2; j < floor(sqrt(numPE)); j++) {
      int initK = floor(static_cast<float>(0.8 * numPE) / j);
      for (int k = initK; k <= floor(numPE / j); k++) {
        int currNumPE = j * k;

        if (currNumPE * 9 < BRAM_AVAIL)
          sysSizes.push_back({i, j, k});
      }
    }
#else
    for (int j = 2; j < floor(sqrt(numPE)); j++) {
      int bound = 1;
      for (auto iter : setLoopIters) {
        // collect all reduction variables
        if (tagmap[iter] > 0) {
          int tc = mapLoop2TCs[iter][0];

          bound *= tc;
        }
      }

      bound = (bound + i - 1) / i;
      //    bound = (bound + 2 - 1) / 2;

      if (2 * j > bound) {
        continue;
      }

      int initK = floor(static_cast<float>(0.8 * numPE) / j);
      for (int k = initK; k <= floor(numPE / j); k++) {
        //    for (int k = 2; k <= floor(numPE / j); k++)
        if (((k + j - 1) / j) > 16) {
          continue;
        }
        int currNumPE = j * k;

        if (currNumPE * 9 < BRAM_AVAIL)
          sysSizes.push_back({i, j, k});
      }
    }
#endif  // ENABLE_STUCK_FILTER
#endif  // EXPERIMENT_SEARCH_WHOLE_DESIGN_SPACE
  }

  std::cout << __func__ << ": " << __LINE__ << std::endl;
  std::cout << "Systolic Array Candidates:" << endl;
  for (auto v : sysSizes) {
    for (auto i : v) {
      std::cout << i << " ";
    }
    std::cout << endl;
  }

  list<float> candidateScores;
  list<vector<pair<void *, int>>> allCandidates;

  set<void *> outSet;
  set<void *> inSet;
  set<void *> wtSet;
  outSet = data.GetNotRelatedLoop(SystolicData::OUT);
  /*
  std::cout << "outSet\n";
  for (auto var : outSet)
  {
      std::cout << codegen->_up(var) << std::endl;
  }
  */
  inSet = data.GetNotRelatedLoop(SystolicData::IN);
  wtSet = data.GetNotRelatedLoop(SystolicData::WT);

  // map<void *, vector<int>> mapLoop2TCs = data.GetLoopTCs();

#ifdef SYS_FRONT_INFO
  cerr << SYS_FRONT_INFO;
  cerr << "Searching design space for systolic PE array..." << endl;
#endif

  int iterDSETotal = 0;
  int iterDSE = 0;
  float maxThrpt = 0;

#ifdef SYS_FRONT_LOG
  data.OpenLogFile("dse_systolic.log");
#endif

  // Create a loop trip count map cache
  map<void *, int> mapLoop2MaxTC;
  for (auto pair : mapLoop2TCs) {
    mapLoop2MaxTC[pair.first] =
        *(max_element(pair.second.begin(), pair.second.end()));
  }

  // Select one loop from each set
  for (auto parOut : outSet) {
    /*
     * xuechao: 04/27/2019
     * force filter variables not to be pe_simd
     * */
    if (tagmap[parOut] == 1) {
      continue;
    }
    for (auto parIn : inSet) {
      for (auto parWt : wtSet) {
        // Explore systolic array size
        for (auto sizes : sysSizes) {
          data.SetSystolicPEArr(SystolicData::SIMD, parOut, sizes[0]);

          // FIXME: Due to hardware limitation, now only "feature map row"
          // can be systolic PE row. And we get all possible row into set_index
          set<string> set_index;  // index may have r + p, so use set to store
          // the separated indexes
          void *firstExp = nullptr;
          // get the first exp, as it stored in a reverse order, so the last is
          // actually the first.
          for (auto exp : data.GetIdxExps(SystolicData::WT)) {
            firstExp = exp;
          }
          if (codegen->IsVarRefExp(firstExp) != 0) {
            set_index.insert(codegen->_up(firstExp));
          } else if (codegen->IsBinaryOp(firstExp) != 0) {
            void *lhs;
            void *rhs;
            codegen->GetExpOperand(firstExp, &lhs, &rhs);
            if ((isSgBinaryOp(static_cast<SgNode *>(firstExp)) != nullptr) &&
                isSgBinaryOp(static_cast<SgNode *>(firstExp))->variantT() ==
                    V_SgAddOp &&
                (codegen->IsVarRefExp(lhs) != 0) &&
                (codegen->IsVarRefExp(rhs) != 0)) {
              set_index.insert(codegen->_up(lhs));
              set_index.insert(codegen->_up(rhs));
            }
          }

          // Explore loop -> systolic array mapping
          /* xuechao: 04/22/2019
           * try to generate some PE shapes that would not stuck
           * fix j as pe_row, i as pe_simd
           */
#if (ENABLE_STUCK_FILTER == 0)
          for (int j = 1; j <= 2; j++) {
#else
          for (int j = 1; j <= 1; j++) {
#endif
            data.SetSystolicPEArr(SystolicData::ROW, parIn, sizes[j]);
            data.SetSystolicPEArr(SystolicData::COL, parWt, sizes[3 - j]);

            // FIXME: Due to hardware limitation, now only "feature map row"
            // can be systolic PE row. However, using iterator name to
            // detect it is inappropriate. Need to figure out another
            // reliable solution.
            // if (j == 1 || codegen->_up(parWt) != "r")
            //     continue;

            if (set_index.end() == set_index.find(codegen->_up(parIn))) {
              continue;
            }

            float thrpt = data.GetCompThrpt();

            iterDSETotal++;
            if (parOut == parIn || parOut == parWt || parIn == parWt) {
              continue;
            }

            // Record the solutions
            float score =
                CalcSystolicArrayScore(thrpt, sizes[0], sizes[1], sizes[2]);
#ifdef SYS_FRONT_LOG
            data.WritelnToLogFile(
                std::to_string(sizes[0] * sizes[1] * sizes[2]) + "\t" +
                std::to_string(thrpt));
#endif
            if (thrpt > maxThrpt) {
              maxThrpt = thrpt;
            }

            // Create a new candidate
            vector<pair<void *, int>> newCandidate;
            newCandidate.resize(3);

            // FIXME: guarantee that loop order of PE loops:
            // {parIn->ROW, parWt->COL, parOut->VECTOR}
            // but it will omit some high performance candidates
            int row_idx = min(j, 3 - j);
            int col_idx = max(j, 3 - j);
            newCandidate[0] = std::make_pair(parOut, sizes[0]);
            // newCandidate[j] = std::make_pair(parIn, sizes[j]);
            // newCandidate[3 - j] = std::make_pair(parWt, sizes[3 - j]);
            newCandidate[row_idx] = std::make_pair(parIn, sizes[j]);
            newCandidate[col_idx] = std::make_pair(parWt, sizes[3 - j]);

            // Insert the new candidate to a suitable position
            if (allCandidates.empty()) {
              candidateScores.push_back(score);
              allCandidates.push_back(newCandidate);
            } else {
              // Find the position for the score and insert
              int pos = 0;
              for (auto ite = candidateScores.begin();
                   ite != candidateScores.end(); ite++) {
                if (score > *ite) {
                  candidateScores.insert(ite, score);
                  break;
                }
                pos++;
              }

              // Insert the candidate to the position
              for (auto ite = allCandidates.begin(); ite != allCandidates.end();
                   ite++) {
                if (pos == 0) {
                  allCandidates.insert(ite, newCandidate);
                  break;
                }
                pos--;
              }
            }
            iterDSE++;

#ifdef SYS_FRONT_INFO
            if ((iterDSE % 10000) == 0) {
              cerr << SYS_FRONT_INFO;
              cerr << "Found " << allCandidates.size()
                   << " designs, max throughput ";
              cerr << maxThrpt << " GFlop/s (";
              cerr << FREQ << " MHz) " << endl;
#ifdef SYS_FRONT_FINE
              // for (auto ite : *candidates) {
              for (auto ite : allCandidates) {
                cerr << SYS_FRONT_FINE;
                cerr << "Design: ";
                cerr << "Parallel: VECTOR -> ";
                cerr << codegen->_up(ite[0].first);
                cerr << "(" << ite[0].second << "); ";
                cerr << "ROW -> " << codegen->_up(ite[1].first);
                cerr << "(" << ite[1].second << "); ";
                cerr << "COL -> " << codegen->_up(ite[2].first);
                cerr << "(" << ite[2].second << ")" << endl;
              }
#endif
            }
#endif
          }
        }
      }
    }
  }

#ifdef SYS_FRONT_LOG
  data.CloseLogFile();
#endif

#ifdef EXPERIMENT_SEARCH_WHOLE_DESIGN_SPACE
  cerr << SYS_FRONT_INFO;
  cerr << "Total " << candidateScores.size() << " designs." << endl;
  float sampling = 1;
  int sampleCnt = 0;
#endif

  // Pruning: Only keep systolic arrays with highest 10% throughput
#ifdef REMOVE_CANDIDATES_EARLY
  float maxScore = candidateScores.front();
#endif

  while (!candidateScores.empty()) {
#ifndef EXPERIMENT_SEARCH_WHOLE_DESIGN_SPACE

// we shouldn't remove candidates early,
// because candidates left may out of resource, see MER-894
#ifdef REMOVE_CANDIDATES_EARLY
    float currScore = candidateScores.front();
    if (currScore >= maxScore * 0.9)
      candidates->push_back(allCandidates.front());
    else
      break;
#else
    candidates->push_back(allCandidates.front());
#endif

#else
    if (sampleCnt == 0) {  // Sampling
      candidates->push_back(allCandidates.front());
    }
    sampleCnt = (static_cast<float>(sampleCnt) >= sampling) ? 0 : sampleCnt + 1;
    if (sampleCnt == 0) {  // Adjust interval after sampling
      sampling *= 1.02;
    }
#endif
    candidateScores.pop_front();
    allCandidates.pop_front();
  }

#ifdef SYS_FRONT_INFO
  cerr << SYS_FRONT_INFO;
  cerr << "Total " << candidates->size() << " designs, ideal throughput ";
  cerr << maxThrpt << " GFlop/s (";
  cerr << FREQ << " MHz)" << endl;

  cerr << SYS_FRONT_INFO;
  cerr << "Explored " << iterDSE << " designs from ";
  cerr << iterDSETotal << " design options" << endl;
#endif

#ifdef SYS_FRONT_FINE
  for (auto ite : *candidates) {
    cerr << SYS_FRONT_FINE;
    cerr << "Design: ";
    cerr << "Parallel: VECTOR -> ";
    cerr << codegen->_up(ite[0].first);
    cerr << "(" << ite[0].second << "); ";
    cerr << "ROW -> " << codegen->_up(ite[1].first);
    cerr << "(" << ite[1].second << "); ";
    cerr << "COL -> " << codegen->_up(ite[2].first);
    cerr << "(" << ite[2].second << ")" << endl;
  }
#endif
  return;
}

void ExploreFeederSizes(CSageCodeGen *codegen,
                        const vector<DSELoop *> &vecLoops, SystolicData *data,
                        int implNum, list<SystolicData *> *topDesigns, int idx,
                        int *iterCnt) {
  vecLoops[idx]->ResetTileSize();

  int prevSize = 0;

  do {
    if (static_cast<size_t>(idx) < (vecLoops.size() - 1)) {
      // Keep setting configuration
      ExploreFeederSizes(codegen, vecLoops, data, implNum, topDesigns, idx + 1,
                         iterCnt);
    } else {
      // Finish setting at the last loop.

#ifdef SYS_FRONT_FINE
      cerr << SYS_FRONT_FINE;
      cerr << "Feeder: ";
      for (auto loop : vecLoops) {
        cerr << codegen->_up(loop->GetLoopIter());
        cerr << "(" << loop->GetTileSize() << ") ";
      }
      cerr << endl;
#endif

      // Setup feeder loop configuration
      // NOTE: We don't care outer loop at this point
      data->CleanFeeder();
      for (auto loop : vecLoops) {
        if (loop->GetTileSize() != 0) {
          data->AddFeeder(loop->GetLoopIter(), loop->GetTileSize());
        }
      }

      // Estimate BRAM utilization
      int bramBlk = data->GetBRAMBlk();
      bool testBRAM = static_cast<float>(bramBlk) <= BRAM_AVAIL;

      // Estimate overall throughput
      float thrpt = data->GetThrpt();

      if (!testBRAM) {
#ifdef SYS_FRONT_FINE
        cerr << SYS_FRONT_FINE;
        cerr << "...out of BRAM blocks" << endl;
#endif
      } else {
#ifdef SYS_FRONT_LOG
        data->WritelnToLogFile(std::to_string(data->GetDSPNum()) + "\t" +
                               std::to_string(bramBlk) + "\t" +
                               std::to_string(thrpt));
#endif

        if (topDesigns->empty()) {
          topDesigns->push_back(new SystolicData(*data));
        } else {
          for (auto ite = topDesigns->begin(); ite != topDesigns->end();
               ite++) {
            float gThrpt = (*ite)->GetThrpt();
            //    if (thrpt > gThrpt ||
            //            (thrpt == gThrpt &&
            //             bramBlk < (*ite)->GetBRAMBlk()))
            if (thrpt > gThrpt || (fabs(thrpt - gThrpt) < 1e-9 &&
                                   bramBlk < (*ite)->GetBRAMBlk())) {
              topDesigns->insert(ite, new SystolicData(*data));
              break;
            }
          }
        }

        if (topDesigns->size() > static_cast<size_t>(implNum)) {
          delete topDesigns->back();
          topDesigns->pop_back();
        }

#ifdef SYS_FRONT_FINEST
        cerr << SYS_FRONT_FINEST;
        cerr << " Systolic Array Design Data Report:" << endl;
        cerr << data->toString() << endl;
#endif
      }

      if ((*iterCnt % 10000) == 0) {
#ifdef SYS_FRONT_FINEST
        cerr << SYS_FRONT_INFO;
        cerr << "Top 3 designs (BRAM, Thrpt): ";

        bool isFirst = true;
        int cnt = 0;
        for (auto d : *topDesigns) {
          if (!isFirst)
            cerr << ", ";
          int optBlk = d->GetBRAMBlk();
          cerr << "(" << (optBlk / BRAM_BOUND) * 100.0 << "%, ";
          float thrpt = d->GetThrpt();
          cerr << thrpt << " GFlop/s)";
          isFirst = false;
          cnt++;
          if (cnt == 3)
            break;
        }
        cerr << endl;
#else
        cerr << "\b \b";  // Erase the last char
        cerr << workingStrMap[currWorkingStr];
        currWorkingStr = (currWorkingStr + 1) % 4;
#endif
      }
      (*iterCnt)++;
    }

    prevSize = vecLoops[idx]->GetTileSize();

    // xuechao: 11/13/2018
    // temporarily remove the constraints for multi-layer
    if (idx == 1) {  // Constraint 1: BS_O * O == BS_I * I
      // FIXME: Now idx 0, 1 must be o and i
      DSELoop *loopO = vecLoops[0];
      DSELoop *loopI = vecLoops[1];

      int sysO = loopO->GetSystolicSize();
      sysO = (sysO) != 0 ? sysO : 1;
      int feederO = loopO->GetTileSize();
      feederO = (feederO) != 0 ? feederO : 1;
      int blockO = sysO * feederO;

      int sysI = loopI->GetSystolicSize();
      sysI = (sysI) != 0 ? sysI : 1;

      if (blockO % sysI != 0) {
        break;
      }

      int feederI = blockO / sysI;
      loopI->SetTileSize(feederI);
      if (feederI != loopI->GetTileSize()) {
        break;
      }
    } else {
      if (codegen->_up(vecLoops[idx]->GetLoopIter()) == "r" ||
          codegen->_up(vecLoops[idx]->GetLoopIter()) == "c") {
        // Constraint 2: BS_C * C and BS_R * R are even numbers
        int sys = vecLoops[idx]->GetSystolicSize();
        sys = (sys) != 0 ? sys : 1;
        if ((sys % 2) != 0) {
          // We need to limit BS_R or BS_C if R or C is not even
          if ((vecLoops[idx]->GetTileSize() % 2) != 0) {
            vecLoops[idx]->IncTileSize(1);
          } else {
            vecLoops[idx]->IncTileSize(2);
          }
        }
      } else {
        vecLoops[idx]->IncTileSize();
      }
    }
    //    vecLoops[idx]->IncTileSize();
  } while (prevSize != vecLoops[idx]->GetTileSize());
}

// Determine loops to form the feeders.
// TODO(youxiang): Deal with not dividable cases
void FeederLoopExploration(CSageCodeGen *codegen,
                           list<vector<pair<void *, int>>> *sysCandidates,
                           vector<SystolicData *> *dataImpls, int step) {
#ifdef SYS_FRONT_INFO
  static int maxMsgLength = 0;
  cerr << SYS_FRONT_INFO;
  cerr << "Searching design space for systolic feeders..." << endl;
#endif

  SystolicData &data = *(*dataImpls)[0];

#ifdef SYS_FRONT_LOG
  data.OpenLogFile("dse_all.log");
#endif

  int iterCnt = 0;
  list<SystolicData *> topDesigns;
  vector<pair<void *, int>> currSys;

  // Setup feeder candidate loops
  // NOTE: Force o- and i- loops to go first for pruning
  set<void *> setLoops = data.GetLoopIters();
  vector<DSELoop *> vecLoops;
  vector<DSELoop *> tmpLoops;

  map<void *, int> mapIter2Tag = data.GetTagMap();

  vector<void *> vecPE = data.GetvecPE();
  for (auto loop : setLoops) {
    DSELoop *dseLoop = new DSELoop(step);
    dseLoop->SetLoopIter(loop);
    dseLoop->SetTripCount(data.GetLoopTCs()[loop]);

    dseLoop->SetLoopTag(mapIter2Tag[loop]);

    // xuechao: 11/12/2018
    //  if (codegen->_up(dseLoop->GetLoopIter()) == "i" ||
    //          codegen->_up(dseLoop->GetLoopIter()) == "o")
    if (codegen->_up(dseLoop->GetLoopIter()) ==
            codegen->_up(vecPE[SystolicData::COL]) ||
        codegen->_up(dseLoop->GetLoopIter()) ==
            codegen->_up(vecPE[SystolicData::SIMD])) {
      vecLoops.push_back(dseLoop);
    } else {
      tmpLoops.push_back(dseLoop);
    }
  }
  // xuechao: 11/12/2018
  //    if (codegen->_up(vecLoops[0]->GetLoopIter()) == "i")
  if (codegen->_up(vecLoops[0]->GetLoopIter()) ==
      codegen->_up(vecPE[SystolicData::SIMD])) {
    std::swap(vecLoops[0], vecLoops[1]);
  }
  for (auto loop : tmpLoops) {
    vecLoops.push_back(loop);
  }

  int allDesignNum = sysCandidates->size();

#ifdef SYS_FRONT_INFO
  std::stringstream ss;
  ss << SYS_FRONT_INFO;
  ss << "0% (0/" << allDesignNum << ") completed.  ";
  string msg = ss.str();
  cerr << msg;
  if (msg.length() > static_cast<size_t>(maxMsgLength)) {
    maxMsgLength = msg.length();
  } else {
    cerr << string(maxMsgLength - msg.length(), ' ');
  }
#endif

  int64_t timer = clock();

  // Design space exploration for feeders
  while (static_cast<unsigned int>(!sysCandidates->empty()) != 0U) {
    currSys = sysCandidates->front();
    sysCandidates->pop_front();

    set<void *> setSysLoops;
    for (auto p : currSys) {
      setSysLoops.insert(p.first);
    }

    // Setup systolic array loops
    data.SetSystolicPEArr(SystolicData::SIMD, currSys[0].first,
                          currSys[0].second);
    data.SetSystolicPEArr(SystolicData::ROW, currSys[1].first,
                          currSys[1].second);
    data.SetSystolicPEArr(SystolicData::COL, currSys[2].first,
                          currSys[2].second);

    for (auto dseLoop : vecLoops) {
      dseLoop->Reset();
    }

    for (int i = 0; i < 3; i++) {
      for (auto dseLoop : vecLoops) {
        if (dseLoop->GetLoopIter() == currSys[i].first) {
          dseLoop->SetAsSystolicLoop(i);
          dseLoop->SetSystolicSize(currSys[i].second);
          break;
        }
      }
    }

    // Design space exploration
    ExploreFeederSizes(codegen, vecLoops, &data, dataImpls->size(), &topDesigns,
                       0, &iterCnt);
    /*
       dseLog << "Systolic array: ";
       for (auto p : vecLoops) {
       if (p->IsSystolicLoop()) {
       dseLog << codegen->_up(p->GetLoopIter());
       dseLog << "(" << p->GetSystolicSize() << ") ";
       }
       }
       dseLog << endl;
       dseLog << "\tTotal " << iterCnt << " iterations performed." << endl;
       */
#ifdef SYS_FRONT_INFO

    //            cerr << ": Total " << iterCnt << " iterations performed (";
    cerr << "\r";  // Reset cursor
    cerr << SYS_FRONT_INFO;
    cerr << static_cast<int>((1.0 - (static_cast<float>(sysCandidates->size()) /
                                     static_cast<float>(allDesignNum))) *
                             100);
    cerr << "%";
    //        cerr << "(" << allDesignNum - sysCandidates->size() << "/" <<
    // allDesignNum << ")";
    cerr << " completed. (";
    cerr << (static_cast<float>(clock()) - timer) / CLOCKS_PER_SEC << "s) ";
    cerr << "Top design (BRAM, Thrpt): ";
    if (!topDesigns.empty()) {
      int optBlk = topDesigns.front()->GetBRAMBlk();
      cerr << "(" << static_cast<float>(optBlk) / BRAM_BOUND * 100.0 << "%, ";
      float thrpt = topDesigns.front()->GetThrpt();
      cerr << thrpt << " GFlop/s)   ";
    }
#endif

    iterCnt = 0;
  }

#ifdef SYS_FRONT_INFO
  cerr << endl;
#endif

#ifdef SYS_FRONT_LOG
  data.CloseLogFile();
#endif

  for (auto loop : vecLoops) {
    free(loop);
  }

#ifdef SYS_FRONT_WARN
  if (topDesigns.empty()) {
    cerr << SYS_FRONT_WARN;
    cerr << "Cannot find a feasible design." << endl;
    exit(0);
  }
#endif

  for (size_t i = 1; i < dataImpls->size(); i++) {
    (*dataImpls)[i] = topDesigns.front();
    topDesigns.pop_front();
  }
}

void InsertProcessStmts(CSageCodeGen *codegen, string layerLoopName,
                        void *layerLoopIter, map<string, void *> mapInitName,
                        vector<void *> vecStmts, string pragmaName,
                        bool isBefore, void *posStmt) {
  for (auto stmt : vecStmts) {
    void *copiedStmt = codegen->CopyStmt(stmt);

    // Update variable references
    vector<void *> vecRefs;
    codegen->GetNodesByType(copiedStmt, "preorder", "SgVarRefExp", &vecRefs);
    for (auto ref : vecRefs) {
      string name = codegen->GetVariableName(ref);
      void *newRef = nullptr;
      if (layerLoopName == name) {
        newRef = codegen->CreateVariableRef(layerLoopIter);
      } else if (mapInitName.find(name) != mapInitName.end()) {
        newRef = codegen->CreateVariableRef(mapInitName[name]);
      } else {  // Other variables that doesn't use in CONV
        void *decl = codegen->GetVariableDecl(ref);
        if (decl == nullptr) {
          cerr << SYS_FRONT_ERROR;
          cerr << " Cannot find the declaration of variable ";
          cerr << codegen->GetVariableName(ref) << " used in postprocess"
               << endl;
        } else {  // Constant variable
          // NOTE: Now we put all those variables to global,
          // but maybe it exists exceptions.
          void *newDecl = codegen->CopyStmt(decl);
          codegen->InsertStmt(newDecl,
                              codegen->GetFirstInsertPosInGlobal(posStmt));
          newRef = codegen->CreateVariableRef(newDecl);
        }
      }
      codegen->ReplaceExp(ref, newRef);
    }

    void *pragma =
        codegen->CreatePragma(pragmaName, codegen->GetGlobal(posStmt));
    void *bb = codegen->CreateBasicBlock();
    codegen->AppendChild(bb, pragma);
    codegen->AppendChild(bb, copiedStmt);
    if (isBefore) {
      codegen->InsertStmt(bb, posStmt);
    } else {
      codegen->InsertAfterStmt(bb, posStmt);
    }
  }
}

void *CreateSystolicArrayFunc(CSageCodeGen *codegen,
                              vector<string> vecInterfacePragmas,
                              vector<void *> vecPreprocess,
                              vector<void *> vecPostprocess,
                              SystolicData *sysData, int index) {
#define DEBUG_CODEGEN 1

  // SystolicData &data = *sysData;

  map<string, void *> mapInitName;

  // Prepare arguments
  void *outInitName = sysData->GetInitializedNameFromPntrArrRefExp(
      sysData->GetExp(SystolicData::OUT));
  mapInitName[codegen->GetVariableName(outInitName)] =
      codegen->CreateVariable(sysData->GetMultiDimTypeByString(
                                  codegen->GetVariableTypeName(outInitName)),
                              codegen->GetVariableName(outInitName));

  void *inInitName = sysData->GetInitializedNameFromPntrArrRefExp(
      sysData->GetExp(SystolicData::IN));
  mapInitName[codegen->GetVariableName(inInitName)] =
      codegen->CreateVariable(sysData->GetMultiDimTypeByString(
                                  codegen->GetVariableTypeName(inInitName)),
                              codegen->GetVariableName(inInitName));

  void *wtInitName = sysData->GetInitializedNameFromPntrArrRefExp(
      sysData->GetExp(SystolicData::WT));
  mapInitName[codegen->GetVariableName(wtInitName)] =
      codegen->CreateVariable(sysData->GetMultiDimTypeByString(
                                  codegen->GetVariableTypeName(wtInitName)),
                              codegen->GetVariableName(wtInitName));

  vector<void *> vecParams;
  for (auto pair : mapInitName) {
    vecParams.push_back(pair.second);
  }

  // Create a new file with the systolic array kernel function
  void *newFile = codegen->CreateSourceFile("systolic_array_kernel" +
                                            std::to_string(index) + ".cpp");
  void *func = codegen->CreateFuncDecl("void", "systolic_array_kernel",
                                       vecParams, newFile, true, nullptr);
  codegen->AppendChild(newFile, func);

  // add depth pragma
  for (auto pragma : vecInterfacePragmas) {
    void *interfacePragma =
        codegen->CreatePragma(pragma, codegen->GetGlobal(func));
    codegen->AppendChild(codegen->GetFuncBody(func), interfacePragma);
  }

#if (ENABLE_MULTI_LAYER_PROC == 1)
  // Create a outermost loop for multi-layer
  string layerLoopIterName = "layer";
  if (sysData->GetLayerLoop()) {
    layerLoopIterName = codegen->GetVariableName(
        codegen->GetLoopIterator(sysData->GetLayerLoop()));
  }
  void *layerLoop = sysData->CreateSimpleForLoop(
      layerLoopIterName, codegen->CreateConst(sysData->GetLayerNum()),
      codegen->CreateBasicBlock(), codegen->GetFuncBody(func));
  void *layerLoopIter = codegen->GetLoopIterator(layerLoop);
  codegen->AppendChild(codegen->GetFuncBody(func), layerLoop);
#endif
#if (DEBUG_CODEGEN == 0)
  codegen->AppendChild(codegen->GetFuncBody(func), layerLoop);
#endif

  // Create systolic array loops
  void *SIMDloop = sysData->CreateSimpleForLoop(
      codegen->_up(sysData->GetSystolicPE(SystolicData::SIMD)) + "_simd_pe",
      codegen->CreateConst(sysData->GetSystolicPESize(SystolicData::SIMD)),
      codegen->CreateBasicBlock(), codegen->GetFuncBody(func));

  void *COLloop = sysData->CreateSimpleForLoop(
      codegen->_up(sysData->GetSystolicPE(SystolicData::COL)) + "_col_pe",
      codegen->CreateConst(sysData->GetSystolicPESize(SystolicData::COL)),
      SIMDloop, codegen->GetFuncBody(func));

  void *ROWloop = sysData->CreateSimpleForLoop(
      codegen->_up(sysData->GetSystolicPE(SystolicData::ROW)) + "_row_pe",
      codegen->CreateConst(sysData->GetSystolicPESize(SystolicData::ROW)),
      COLloop, codegen->GetFuncBody(func));

  // Add parallel pragma
  void *systolicBB = codegen->CreateBasicBlock();
  void *parPragma =
      codegen->CreatePragma("ACCEL parallel scope", codegen->GetGlobal(func));
  codegen->AppendChild(systolicBB, parPragma);
  codegen->AppendChild(systolicBB, ROWloop);

  // Create feeder loops
  map<void *, int> mapFeeder2Value = sysData->GetFeeders();
  map<void *, void *> mapFeederLoops;
  void *feederLoop = codegen->CopyStmt(systolicBB);

  /*
  for (auto feeder : mapFeeder2Value) {
    if (feeder.first == sysData->GetSystolicPE(SystolicData::COL)) {
      feederLoop =
          sysData->CreateSimpleForLoop(codegen->_up(feeder.first) + "_feeder",
                                      codegen->CreateConst(feeder.second),
                                      feederLoop, codegen->GetFuncBody(func));
      mapFeederLoops[feeder.first] = feederLoop;

      break;
    }
  }

  for (auto feeder : mapFeeder2Value) {
    if (feeder.first == sysData->GetSystolicPE(SystolicData::ROW)) {
      feederLoop =
          sysData->CreateSimpleForLoop(codegen->_up(feeder.first) + "_feeder",
                                      codegen->CreateConst(feeder.second),
                                      feederLoop, codegen->GetFuncBody(func));
      mapFeederLoops[feeder.first] = feederLoop;

      break;
    }
  }

  //    feederBB = codegen->CreateBasicBlock(feederLoop);

  // pair<void *, int> accFeeder;
  map<void *, int> accFeeders;
  for (auto feeder : mapFeeder2Value)
  {
    if (feeder.first == sysData->GetSystolicPE(SystolicData::SIMD))
    {
      // accFeeder = feeder;
      accFeeders.insert(feeder);
      continue;
    }

    // bypass feeders with PE row/col loop iterators
    if (feeder.first == sysData->GetSystolicPE(SystolicData::COL) ||
        feeder.first == sysData->GetSystolicPE(SystolicData::ROW)) {
      continue;
    }

    feederLoop =
        sysData->CreateSimpleForLoop(codegen->_up(feeder.first) + "_feeder",
                                    codegen->CreateConst(feeder.second),
                                    feederLoop, codegen->GetFuncBody(func));

    mapFeederLoops[feeder.first] = feederLoop;
  }
  */

  /*
   * xuechao: 04/28/2019
   * non-reduction goes first, and
   * must adhere to the order in OUT expression */
  vector<void *> outIndices = sysData->GetIdxExps(SystolicData::OUT);
  for (size_t i = 0; i < outIndices.size(); i++) {
    void *iter = outIndices[i];

    for (auto feeder : mapFeeder2Value) {
      if (codegen->_up(iter) == codegen->_up(feeder.first)) {
        std::cout << codegen->_up(feeder.first) << " "
                  << mapFeeder2Value[feeder.first] << std::endl;

        feederLoop = sysData->CreateSimpleForLoop(
            codegen->_up(feeder.first) + "_feeder",
            codegen->CreateConst(feeder.second), feederLoop,
            codegen->GetFuncBody(func));
        mapFeederLoops[feeder.first] = feederLoop;
      }
    }
  }

  /*
   * xuechao: 04/16/2019
   * reduction variable first
   * FIXME: Eliminate this restriction
   */
  map<void *, int> tagmap = sysData->GetTagMap();
  for (auto feeder : mapFeeder2Value) {
    if (tagmap[feeder.first] > 0) {
      feederLoop =
          sysData->CreateSimpleForLoop(codegen->_up(feeder.first) + "_feeder",
                                       codegen->CreateConst(feeder.second),
                                       feederLoop, codegen->GetFuncBody(func));

      mapFeederLoops[feeder.first] = feederLoop;
    }
  }

  // Add data_reuse pragma
  void *feederBB = codegen->CreateBasicBlock();
  void *feederPragma =
      codegen->CreatePragma("ACCEL data_reuse scope", codegen->GetGlobal(func));
  codegen->AppendChild(feederBB, feederPragma);
  codegen->AppendChild(feederBB, feederLoop);

  map<void *, vector<int>> mapOuter2Value = sysData->GetOuters();

#if (ENABLE_MULTI_LAYER_PROC)
  // Create outer loop bound variables
  map<void *, void *> mapOuter2Bound;
  for (auto outer : mapOuter2Value) {
    vector<void *> vecVals;
    for (auto v : outer.second)
      vecVals.push_back(codegen->CreateConst(v));
    SgExprListExp *valueExpList =
        isSgExprListExp(static_cast<SgNode *>(codegen->CreateExpList(vecVals)));
    SgAggregateInitializer *initizer =
        SageBuilder::buildAggregateInitializer(valueExpList);
    void *bounds = codegen->CreateVariableDecl(
        "int [" + std::to_string(sysData->GetLayerNum()) + "]",
        codegen->_up(outer.first) + "BOUND", initizer, newFile);
    codegen->InsertStmt(bounds, func);
    mapOuter2Bound[outer.first] = bounds;
  }
#endif

  // Create outer loops
  void *outerLoop = codegen->CopyStmt(feederBB);
  map<void *, void *> mapOuterLoops;
  // int BBFlag = 1;
  for (auto outer : mapOuter2Value) {
    //    if (!BBFlag)
    //    {
    //        outerLoop = codegen->CreateBasicBlock(outerLoop);
    //    }
    //    else
    //    {
    //        BBFlag = 0;
    //    }
#if (ENABLE_MULTI_LAYER_PROC)
    vector<void *> idx;
    idx.push_back(codegen->CreateVariableRef(layerLoopIter));
    outerLoop = sysData->CreateSimpleForLoop(
        codegen->_up(outer.first) + "_outer",
        codegen->CreateArrayRef(
            codegen->CreateVariableRef(mapOuter2Bound[outer.first]), idx),
        outerLoop, codegen->GetFuncBody(func));
#else
    outerLoop =
        sysData->CreateSimpleForLoop(codegen->_up(outer.first) + "_outer",
                                     codegen->CreateConst((outer.second)[0]),
                                     outerLoop, codegen->GetFuncBody(func));
#endif
    mapOuterLoops[outer.first] = outerLoop;
  }

  // Setup function declaration
  void *systolicPragma =
      codegen->CreatePragma("ACCEL systolic", codegen->GetFuncBody(func));

  // void *outerBB = codegen->CreateBasicBlock();
  // codegen->AppendChild(outerBB, feederPragma);
  // codegen->AppendChild(outerBB, feederBB);

#if (ENABLE_MULTI_LAYER_PROC)
  codegen->AppendChild(codegen->GetLoopBody(layerLoop), outerLoop);
#else
  codegen->AppendChild(codegen->GetFuncBody(func), systolicPragma);
  codegen->AppendChild(codegen->GetFuncBody(func), outerLoop);
#endif

  void *innerMostBody = outerLoop;
  while (true) {
    if (codegen->IsForStatement(innerMostBody) != 0) {
      innerMostBody = codegen->GetLoopBody(innerMostBody);
    } else if (codegen->IsBasicBlock(innerMostBody) != 0) {
      if (2 == codegen->GetChildStmtNum(innerMostBody)) {
        std::cout << "codegen->GetChildStmtNum(innerMostBody)="
                  << codegen->GetChildStmtNum(innerMostBody) << std::endl;
        void *innerMostBody0 = codegen->GetChildStmt(innerMostBody, 0);
        std::cout << codegen->_up(innerMostBody0) << std::endl;
        innerMostBody = codegen->GetChildStmt(innerMostBody, 1);
        std::cout << codegen->_up(innerMostBody) << std::endl;
      } else if (1 == codegen->GetChildStmtNum(innerMostBody)) {
        innerMostBody = codegen->GetChildStmt(innerMostBody, 0);
      } else {
        break;
      }
    } else {
    }
  }

  // Create memory access reordering statements
  map<void *, void *> mapIters;
  for (auto iter : sysData->GetLoopIters()) {
    void *initExp = nullptr;
    int currFactor = 1;
    if (sysData->IsFeeder(iter)) {
      void *exp = codegen->CreateVariableRef(
          codegen->GetLoopIterator(mapFeederLoops[iter]));
      currFactor = mapFeeder2Value[iter];
      initExp = (initExp == nullptr)
                    ? exp
                    : codegen->CreateExp(V_SgAddOp, exp, initExp);
    }
    if (sysData->IsSystolicPE(iter)) {
      void *exp = nullptr;
      int peFactor = 0;

      if (iter == sysData->GetSystolicPE(SystolicData::ROW)) {
        peFactor = sysData->GetSystolicPESize(SystolicData::ROW);
        exp = codegen->CreateVariableRef(codegen->GetLoopIterator(ROWloop));
      } else if (iter == sysData->GetSystolicPE(SystolicData::COL)) {
        peFactor = sysData->GetSystolicPESize(SystolicData::COL);
        exp = codegen->CreateVariableRef(codegen->GetLoopIterator(COLloop));
      } else {
        peFactor = sysData->GetSystolicPESize(SystolicData::SIMD);
        exp = codegen->CreateVariableRef(codegen->GetLoopIterator(SIMDloop));
      }
      exp = codegen->CreateExp(V_SgMultiplyOp, exp,
                               codegen->CreateConst(currFactor));

      currFactor *= peFactor;
      initExp = (initExp == nullptr)
                    ? exp
                    : codegen->CreateExp(V_SgAddOp, exp, initExp);
    }
    if (sysData->IsOuter(iter)) {
      void *exp =
          codegen->CreateExp(V_SgMultiplyOp,
                             codegen->CreateVariableRef(
                                 codegen->GetLoopIterator(mapOuterLoops[iter])),
                             codegen->CreateConst(currFactor));
      initExp = (initExp == nullptr)
                    ? exp
                    : codegen->CreateExp(V_SgAddOp, exp, initExp);
    }
    void *newIdxDecl = codegen->CreateVariableDecl(
        "int", codegen->_up(iter) + "_sys", initExp, innerMostBody);
    codegen->AppendChild(innerMostBody, newIdxDecl);
    mapIters[iter] = newIdxDecl;
  }

  //    for (auto iter : mapIters)
  //    {
  //        std::cout << "iter={" << codegen->_p(iter.first) << "," <<
  // codegen->_p(iter.second) << "}" << std::endl;
  //    }

  // Create computation statement
  vector<void *> vecTmp;

  vecTmp.clear();
  void *out = codegen->CopyExp(sysData->GetExp(SystolicData::OUT));
  codegen->GetNodesByType(out, "preorder", "SgVarRefExp", &vecTmp);
  for (auto ref : vecTmp) {
    void *iter = codegen->GetVariableInitializedName(ref);
    if (mapIters.find(iter) == mapIters.end()) {  // Array reference
      codegen->ReplaceExp(
          ref, codegen->CreateVariableRef(
                   mapInitName[codegen->GetVariableName(outInitName)]));
      continue;
    }
    void *newRef = codegen->CreateVariableRef(mapIters[iter]);
    codegen->ReplaceExp(ref, newRef);
  }

  vecTmp.clear();
  void *in = codegen->CopyExp(sysData->GetExp(SystolicData::IN));
  codegen->GetNodesByType(in, "preorder", "SgVarRefExp", &vecTmp);
  for (auto ref : vecTmp) {
    void *iter = codegen->GetVariableInitializedName(ref);
    if (mapIters.find(iter) == mapIters.end()) {  // Array reference
      codegen->ReplaceExp(
          ref, codegen->CreateVariableRef(
                   mapInitName[codegen->GetVariableName(inInitName)]));
      continue;
    }
    void *newRef = codegen->CreateVariableRef(mapIters[iter]);
    codegen->ReplaceExp(ref, newRef);
  }

  vecTmp.clear();
  void *wt = codegen->CopyExp(sysData->GetExp(SystolicData::WT));
  codegen->GetNodesByType(wt, "preorder", "SgVarRefExp", &vecTmp);
  for (auto ref : vecTmp) {
    void *iter = codegen->GetVariableInitializedName(ref);
    if (mapIters.find(iter) == mapIters.end()) {  // Array reference
      codegen->ReplaceExp(
          ref, codegen->CreateVariableRef(
                   mapInitName[codegen->GetVariableName(wtInitName)]));
      continue;
    }
    void *newRef = codegen->CreateVariableRef(mapIters[iter]);
    codegen->ReplaceExp(ref, newRef);
  }

  void *comExp = codegen->CreateExp(sysData->GetMapOp(), wt, in);
  if (V_SgAddOp == sysData->GetReductionOp()) {
    comExp = codegen->CreateExp(V_SgPlusAssignOp, out, comExp);
  } else if (V_SgSubtractOp == sysData->GetReductionOp()) {
    comExp = codegen->CreateExp(V_SgMinusAssignOp, out, comExp);
  } else {
    cerr << SYS_FRONT_ERROR;
    cerr << "Not supported reduction type" << endl;
  }

  void *comStmt = codegen->CreateStmt(V_SgExprStatement, comExp);
  codegen->AppendChild(innerMostBody, comStmt);

#if (ENABLE_PRE_POST_PROC)
  // Add preprocess
  InsertProcessStmts(codegen, layerLoopIterName, layerLoopIter, mapInitName,
                     vecPreprocess, "ACCEL preprocess", true, systolicPragma);
#endif

#if (ENABLE_PRE_POST_PROC)
  // Add postprocess
  InsertProcessStmts(codegen, layerLoopIterName, layerLoopIter, mapInitName,
                     vecPostprocess, "ACCEL postprocess", false, outerLoop);
#endif

  // Setup function call
  vector<void *> vecCallParams;
  // xuechao: 2018-10-30
  // rose_xxx.cpp: keep argument order of function call same with function
  // declaration
  // vecCallParams.push_back(codegen->CreateVariableRef(outInitName));
  // vecCallParams.push_back(codegen->CreateVariableRef(inInitName));
  // vecCallParams.push_back(codegen->CreateVariableRef(wtInitName));
  vecCallParams.push_back(codegen->CreateVariableRef(wtInitName));
  vecCallParams.push_back(codegen->CreateVariableRef(inInitName));
  vecCallParams.push_back(codegen->CreateVariableRef(outInitName));

  //    void *funcCall = codegen->CreateFuncCall(func,
  //            vecCallParams, codegen->GetScope(outInitName));
  //    void *funcCallStmt = codegen->CreateStmt(V_SgExprStatement, funcCall);

  // Replace loop with a function call
  //        codegen->ReplaceStmt(sysData->GetSysLoop(), funcCallStmt);
  //
  //// Remove systolic pragma and add extern
  //        codegen->RemoveStmt(pragma);

  void *externStmt =
      codegen->CloneFuncDecl(func, codegen->GetGlobal(outInitName), false);

  void *pos = codegen->TraceUpToFuncDecl(outInitName);
  pos = codegen->GetPreviousStmt(pos);
  while (codegen->IsPragmaDecl(pos) != 0) {
    pos = codegen->GetPreviousStmt(pos);
  }
  codegen->InsertStmt(externStmt, pos);

  void *funcCall = codegen->CreateFuncCall(externStmt, vecCallParams,
                                           codegen->GetScope(outInitName));
  void *funcCallStmt = codegen->CreateStmt(V_SgExprStatement, funcCall);

  return funcCallStmt;
}

int systolic_frontend_top(CSageCodeGen *codegen, void *pTopFunc,
                          const CInputOptions &options) {
  cerr << "Systolic Array Frontend" << endl;
  int implNum = 1;
  int searchStep = 2;
  dseLog.open("systolic_trans_dse.log", std::ios::out);

  if (options.get_option_num("-a") != 0) {
    if (!options.get_option_key_value("-a", "impl_num").empty()) {
      implNum = stoi(options.get_option_key_value("-a", "impl_num"));
    }
    if (!options.get_option_key_value("-a", "search_step").empty()) {
      searchStep = stoi(options.get_option_key_value("-a", "search_step"));
    }
  }

#ifdef SYS_FRONT_INFO
  cerr << SYS_FRONT_INFO;
  cerr << "Set output implementation number to " << implNum << endl;

  cerr << SYS_FRONT_INFO;
  cerr << "Set search step to " << searchStep << endl;
#endif

  CMarsIrV2 marsIR;
  marsIR.build_mars_ir(codegen, pTopFunc);

  vector<void *> vecPragmas;
  codegen->GetNodesByType(nullptr, "preorder", "SgPragmaDeclaration",
                          &vecPragmas);

  vector<void *> vecSysPragmas;
  vector<string> vecInterfacePragmas;
  for (auto pragma : vecPragmas) {
    if ("systolic" == marsIR.get_pragma_attribute(pragma, "systolic")) {
      vecSysPragmas.push_back(pragma);
    }

    if ("interface" == marsIR.get_pragma_attribute(pragma, "interface")) {
      string s = string("ACCEL interface variable=") +
                 marsIR.get_pragma_attribute(pragma, "variable");
      s += " depth=";
      s += marsIR.get_pragma_attribute(pragma, "depth");
      vecInterfacePragmas.push_back(s);
    }
  }

  for (auto pragma : vecSysPragmas) {
#ifdef SYS_FRONT_INFO
    cerr << SYS_FRONT_INFO;
    cerr << "Analyzing a loop at ";
    cerr << codegen->get_file(pragma) << ":";
    cerr << codegen->get_line(pragma) << endl;
#endif

    // Log title
    dseLog << "==============================" << endl;
    dseLog << codegen->get_file(pragma) << ":";
    dseLog << codegen->get_line(pragma) << endl;
    dseLog << "==============================" << endl;

    // Create objects to store systolic array configuration
    // The first one is used for DSE and we won't output it
    vector<SystolicData *> sysDataImpls;
    sysDataImpls.resize(implNum + 1);

    // Analyze loop
    sysDataImpls[0] = new SystolicData(pragma, codegen, &marsIR);
    SystolicData &sysData = *sysDataImpls[0];

    if (!sysData.IsHighLevel()) {
      cerr << SYS_FRONT_INFO;
      cerr << "Bypass low-level model" << endl;
      return 1;
    }
    sysData.Analyze();

#ifdef SYS_FRONT_INFO
    cerr << SYS_FRONT_INFO;
    cerr << "Constructing systolic array design" << endl;
#endif

    // Select systolic array loops
    list<vector<pair<void *, int>>> sysCandidates;
    SystolicArrayLoopExploration(codegen, &sysCandidates, &sysData);

    dseLog << "Total systolic array options: " << sysCandidates.size() << endl;

    /*
    std::cout << "HHHHHH" << std::endl;
    for (auto sys : sysCandidates)
    {
        for (auto loop : sys)
        {
            std::cout << codegen->_up(loop.first) << " " << loop.second <<
    std::endl;
        }
        std::cout << endl;
    }
    */

    // Construct feeder loops
    FeederLoopExploration(codegen, &sysCandidates, &sysDataImpls, searchStep);

#ifdef SYS_FRONT_FINE
    for (int i = 1; i < implNum + 1; i++) {
      cerr << SYS_FRONT_FINE;
      cerr << "Systolic Array Design Data Report:" << endl;
      cerr << sysDataImpls[i]->toString() << endl;
    }
#endif

    vector<void *> vecPreprocess = sysData.GetPreProcessStmts();
    vector<void *> vecPostprocess = sysData.GetPostProcessStmts();

#ifdef SYS_FRONT_FINE
    for (auto stmt : vecPreprocess) {
      cerr << SYS_FRONT_FINE;
      cerr << "Preprocess: " << endl;
      cerr << codegen->_up(stmt) << endl;
    }
    for (auto stmt : vecPostprocess) {
      cerr << SYS_FRONT_FINE;
      cerr << "Postprocess: " << endl;
      cerr << codegen->_up(stmt) << endl;
    }
#endif

    // Code generation
    map<void *, int> tagmap = sysData.GetTagMap();
    void *funcCallStmt = nullptr;

    for (int i = 1; i < implNum + 1; i++) {
      // for (int i = 0; i < implNum; i++) {
      if (sysDataImpls[i]->IsValid()) {
#ifdef SYS_FRONT_INFO
        cerr << SYS_FRONT_INFO;
        cerr << "Generating systolic array C-model for design #" << i << endl;
#endif
        // xuechao: 04/16/2019 - group feeder variables
        // into reduction and non-reduction
        SystolicData &data = *sysDataImpls[i];
        for (auto feeder : data.GetFeeders()) {
          std::cout << codegen->_p(feeder.first) << " " << feeder.second
                    << std::endl;
          std::cout << feeder.first << " " << feeder.second << std::endl;
        }

        for (auto feeder : data.GetFeeders()) {
          data.AddTag(feeder.first, tagmap[feeder.first]);
        }

        funcCallStmt =
            CreateSystolicArrayFunc(codegen, vecInterfacePragmas, vecPreprocess,
                                    vecPostprocess, sysDataImpls[i], i);
      }
    }

#ifdef SYS_FRONT_INFO
    cerr << SYS_FRONT_INFO;
    cerr << "Updating source code" << endl;
#endif

    // Replace loop with a function call
    codegen->ReplaceStmt(sysData.GetSysLoop(), funcCallStmt);

    // Remove systolic pragma and add extern
    codegen->RemoveStmt(pragma);
    //        void *externStmt = codegen->CloneFuncDecl(
    //                codegen->GetFuncDeclByCall(codegen->GetExprFromStmt(funcCallStmt)),
    //                codegen->GetGlobal(funcCallStmt), false);
    //
    //        void *pos = codegen->TraceUpToFuncDecl(funcCallStmt);
    //        pos = codegen->GetPreviousStmt(pos);
    //        while (codegen->IsPragmaDecl(pos))
    //            pos = codegen->GetPreviousStmt(pos);
    //        codegen->InsertStmt(externStmt, pos);

#ifdef SYS_FRONT_INFO
    cerr << SYS_FRONT_INFO;
    cerr << "Finished." << endl;
#endif
  }

  dseLog.close();
  return 1;
}
