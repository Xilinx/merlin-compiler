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
void sub(int *a, int n) {
   if (n == 0) {
    a[0] = 0;
  }
  if (n != 0) {
    a[0] = 0;
  } else {
    a[1] = 0;
  }
  for (int i = 0; i < n; ++i) {
    a[i] = i;
  }
  sub(a, n);
  if (0 == 0) {
    a[0] = 0;
  }

  if (0 != 0) {
    a[1] = 1;
  }

  if (0 == 0) {
    a[2] = 2;
  } else {
    a[3] = 3;
  }

  if (0 == 0) {
    if (0 < 1) {
      a[4] = 4;
    } else if (1 == 1) {
      a[5] = 5;
    }
  } else {
    if (1 > 2) {
      a[6] = 6;
    } else {
      a[7] = 7;
    }
  }

  for (int i = 0; i < 0; ++i) {
    a[i] = i;
  }

  for (int i = 0; i < 0; ++i) {
    for (int j = 0; j < 0; ++j) {
      a[i + j] = 0;
    }
  }

  for (int i = 0; i < 0; ++i) {
    if (0 == 1) {
      a[i] = 0;
    }
  }
  
  if (0 > 2) {
    for (int i = 0; i < 0; ++i) {
      a[i] = 0;
    }
  }
  if (2 > 0) {
    for (int i = 0; i < 0; ++i) {
      a[i] = 0;
    }
  } 
  if (2 + 2 > 0) {
    for (int i = 0; i < 0; ++i) {
      a[i] = 0;
    }
  }
  if (2 - 2 > 0) {
    for (int i = 0; i < 0; ++i) {
      a[i] = 0;
    }
  }
  if (2 * 2 > 0) {
    for (int i = 0; i < 0; ++i) {
      a[i] = 0;
    }
  }
  if (2 / 2 > 0) {
    for (int i = 0; i < 0; ++i) {
      a[i] = 0;
    }
  }
  if (2 >> 2 > 0) {
    for (int i = 0; i < 0; ++i) {
      a[i] = 0;
    }
  }
  if (2 << 2 > 0) {
    for (int i = 0; i < 0; ++i) {
      a[i] = 0;
    }
  }
  if (2 + (2 % 1) > 0) {
    for (int i = 0; i < 0; ++i) {
      a[i] = 0;
    }
  }
  if (-2 + 2 > 0) {
    for (int i = 0; i < 0; ++i) {
      a[i] = 0;
    }
  }
  if ((2 ^ 2) > 0) {
    for (int i = 0; i < 0; ++i) {
      a[i] = 0;
    }
  }
  if ((2 & 2) > 0) {
    for (int i = 0; i < 0; ++i) {
      a[i] = 0;
    }
  }
  if ((2 && 2) > 0) {
    for (int i = 0; i < 0; ++i) {
      a[i] = 0;
    }
  }
  if ((2 || 2) > 0) {
    for (int i = 0; i < 0; ++i) {
      a[i] = 0;
    }
  }
  if ((2 | 2) > 0) {
    for (int i = 0; i < 0; ++i) {
      a[i] = 0;
    }
  }
  while (0) {
    a[0] = 0;
  }
  do {
    a[1] = 1;
  } while (0);
  while (1) {
    a[2] = 2;
  }
  do {
    a[3] = 3;
  } while (1);
  if (0) {
    a[4] = 4;
  }
  if (1) {
    a[5] = 5;
  }
  if (0) {
    a[6] = 6;
  } else {
    a[7] = 7;
  }
  if (1) {
    a[8] = 8;
  } else {
    a[9] = 9;
  }
}

#pragma ACCEL kernel
void top(int *a, int n) {
  if (n == 0) {
    a[0] = 0;
  }
  if (n != 0) {
    a[0] = 0;
  } else {
    a[1] = 0;
  }
  for (int i = 0; i < n; ++i) {
    a[i] = i;
  }
  sub(a, n);
  if (0 == 0) {
    a[0] = 0;
  }

  if (0 != 0) {
    a[1] = 1;
  }

  if (0 == 0) {
    a[2] = 2;
  } else {
    a[3] = 3;
  }

  if (0 == 0) {
    if (0 < 1) {
      a[4] = 4;
    } else if (1 == 1) {
      a[5] = 5;
    }
  } else {
    if (1 > 2) {
      a[6] = 6;
    } else {
      a[7] = 7;
    }
  }

  for (int i = 0; i < 0; ++i) {
    a[i] = i;
  }

  for (int i = 0; i < 0; ++i) {
    for (int j = 0; j < 0; ++j) {
      a[i + j] = 0;
    }
  }

  for (int i = 0; i < 0; ++i) {
    if (0 == 1) {
      a[i] = 0;
    }
  }
  
  if (0 > 2) {
    for (int i = 0; i < 0; ++i) {
      a[i] = 0;
    }
  }
  if (2 > 0) {
    for (int i = 0; i < 0; ++i) {
      a[i] = 0;
    }
  }

  for (int i = 0; i < 10; ++i) {
    //double brace
    if (1 == 1) {
      a[i] = 0;
    }
  }
}
