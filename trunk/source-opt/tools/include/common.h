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


#pragma once

#include <math.h>
#include <sys/time.h>
#include <time.h>

#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#define KEEP_UNUSED 0
#define KEEP_UNREACHABLE 0

#define getSEC(us) ((static_cast<float>(us)) / 1000000)

class AccTime;
extern int64_t INIT_TIME;
extern std::vector<AccTime *> accTimes;

#if 0
static inline int64_t getUS() {
  struct timeval tv;
  gettimeofday(&tv, nullptr);
  return ((int64_t)tv.tv_sec) * 1000000 + ((int64_t)tv.tv_usec);
}

#define DEFINE_START_END int64_t start, end
#define STEMP(x) x = getUS();
#define TIMEP_BOUND(module, start, end, bound)                                 \
  end = getUS();                                                               \
  if (end - start >= bound) {                                                  \
    std::cout << "TIME: " << setw(15);                                         \
    std::cout << getSEC(end - start) << "," << setw(15)                        \
              << fmod(getSEC(start - INIT_TIME), 10000) << "," << setw(15)     \
              << fmod(getSEC(end - INIT_TIME), 10000) << ",\t" << module       \
              << std::endl;                                                    \
  }
#define TIMEP(module, start, end) TIMEP_BOUND(module, start, end, 10000)
#define ACCTM(module, start, end)                                              \
  TIMEP_BOUND(#module, start, end, 10000)                                      \
  static AccTime module##AccTime(#module);                                     \
  {                                                                            \
    int64_t tmp = module##AccTime.getAccTime();                                \
    module##AccTime.acc(end - start);                                          \
    if (module##AccTime.getAccTime() / 10000 != tmp / 10000) {                 \
      std::cout << "TIME" << module##AccTime.to_string() << std::endl;         \
    }                                                                          \
  }
#else
#define DEFINE_START_END
#define STEMP(x)
#define TIMEP(module, start, end)
#define ACCTM(module, start, end)
#endif

extern std::map<void *, void *> cache_analysis_induct_map_in_scope;
extern void invalidateCache();

class AccTime {
 public:
  explicit AccTime(std::string pInfo) {
    info = "[ACC TIME] " + pInfo;
    timeUS = 0;
    index = accTimes.size();
    accTimes.push_back(this);
  }

  void acc(int64_t pTimeUS) {
    this->timeUS += pTimeUS;
    ++this->freq;
  }
  static int getIndex() { return AccTime::index; }
  std::string to_string() {
    return info + ": " + std::to_string(getSEC(timeUS)) + "s (" +
           std::to_string(freq) + "times)";
  }

  int64_t getAccTime() { return this->timeUS; }
  ~AccTime() { std::cout << "TIME" << to_string() << std::endl; }

 private:
  std::string info;
  int64_t timeUS;
  int64_t freq = 0;
  static int index;
};

#define ID(x) x
#define LOG(level, title, x)                                                   \
  if ((level) == 0) {                                                          \
    std::cout << "[" << (title) << " ERROR][" << __func__ << ", " << __LINE__  \
              << "]" << ID(x) << std::endl;                                    \
  } else if ((level) == 1) {                                                   \
    std::cout << "[" << (title) << " WARNING][" << __func__ << ", "            \
              << __LINE__ << "]" << ID(x) << std::endl;                        \
  } else if ((level) == 2) {                                                   \
    std::cout << "[" << (title) << " ALGO][" << __func__ << ", " << __LINE__   \
              << "]" << ID(x) << std::endl;                                    \
  } else if ((level) == 3) {                                                   \
    std::cout << "[" << (title) << " INFO][" << __func__ << ", " << __LINE__   \
              << "]" << ID(x) << std::endl;                                    \
  }
#if 1  //  logs
#define PRESTP(title, x) LOG(2, title, x)
#else
#define PRESTP(title, x)
#endif

#if 0  //  logs
#define INFOP(title, x) LOG(3, title, x)
#define ERRORP(title, x) LOG(0, title, x)
#define WARNINGP(title, x) LOG(1, title, x)
#define ALGOPP(title, x) LOG(2, title, x)
#define ALGOP(title, x) LOG(2, title, x)
#else
#define ERRORP(title, x)
#define WARNINGP(title, x)
#define ALGOPP(title, x)
#define INFOP(title, x)
#define ALGOP(title, x)
#endif

template <typename T> static std::string to_string(std::vector<T> vec) {
  std::stringstream ss;

  if (vec.size() == 0) {
    return "";
  }

  ss << vec[0];
  for (size_t i = 1; i < vec.size(); ++i) {
    ss << ", " << vec[i];
  }

  return ss.str();
}
