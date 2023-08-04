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
int sub(int *a) {
  int sum = 0;
  for (int i = 0; i < 10; ++i) {
    sum += a[i];
    a[i]++;
  }
  return sum;
}
#pragma ACCEL kernel
int top(int *a) {
  // should not infer burst here because of return statements
  return sub(a);
}

#pragma ACCEL kernel
void top_2(int *a) {
  // should not infer burst because of return statements
  for (int i = 0; i < 100; ++i) {
    if (i == 2) {
      return;
    }
    a[i]++;
  }
}

#pragma ACCEL kernel
void top_3(int *a) {
  for (int i = 0; i < 100; ++i) {
  // should not infer burst here because of break statements
    if (i == 2) {
      break;
    }
    for (int j = 0; j < 100; ++j) {
      a[i * 100000 + j]++;
    }
  }
}

#pragma ACCEL kernel
void top_4(int *a) {
  for (int i = 0; i < 100; ++i) {
  // should not infer burst here because of continue statements
    if (i == 2) {
      continue;
    }
    for (int j = 0; j < 100; ++j) {
      a[i * 100000 + j]++;
    }
  }
}

#pragma ACCEL kernel
void top_5(int *a) {
  for (int i = 0; i < 100; ++i) {
  // can infer burst here because of local jump
    if (i == 2) {
      goto L1;
    }
L1: for (int j = 0; j < 100; ++j) {
      a[i * 100000 + j]++;
    }
  }
}

#pragma ACCEL kernel
void top_6(int *a) {
  for (int i = 0; i < 100; ++i) {
  // cannot infer burst here because of global jump
    if (i == 2) {
      goto L1;
    }
 for (int j = 0; j < 100; ++j) {
      a[i * 100000 + j]++;
    }
  }
L1: return;
}

#pragma ACCEL kernel
void top_7(int *a) {
 for (int j = 0; j < 100; ++j) {
  for (int i = 0; i < 100; ++i) {
  // cannot infer burst here because of global jump
    if (i == 2) {
      continue;
    }
      a[i * 100000 + j]++;
  }
 }
}
#pragma ACCEL kernel
void top_8(int *a) {
 for (int j = 0; j < 100; ++j) {
  for (int i = 0; i < 100; ++i) {
  // cannot infer burst here because of global jump
    if (i == 2) {
      break;
    }
      a[i * 100000 + j]++;
  }
 }
}

#pragma ACCEL kernel
void top_9(int *a) {
 for (int j = 0; j < 100; ++j) {
  for (int i = 0; i < 100; ++i) {
  // cannot infer burst here because of global jump
    if (i == 2) {
      continue;
    }
      a[j * 100000 + i]++;
  }
 }
}
#pragma ACCEL kernel
void top_10(int *a) {
 for (int j = 0; j < 100; ++j) {
  for (int i = 0; i < 100; ++i) {
  // cannot infer burst here because of global jump
    if (i == 2) {
      break;
    }
      a[j * 100000 + i]++;
  }
 }
}
