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
#include <stdio.h>
#include<stdlib.h>

#define N 100

typedef struct Student {
  int id;
  char *name;
} Student;

#pragma ACCEL kernel
int top(Student *a) {
#pragma ACCEL interface variable=a depth=100
#pragma ACCEL interface variable=a->name depth=10
  int i;
  int count = 0;
  if (!a) return 0;
  if (a && 0) return 0;
  if (a || 0) return 0;
  count = a ? 1 : 0;
  if (a) {
    for (i = 0; i < N; ++i) {
      a[i].id = a[i].id + 10;
      a[i].name[1] = a[i].name[0];

      count += ((Student*) &a[i])->id;
      count += ((const Student*) &a[i])->id;
    }
  }

  return count;
}

int main(int argc, char *argv[]) {
	int i;
    int output;
  Student *a = malloc(N * sizeof (Student));
  FILE *list;

  list = fopen(argv[1], "r");

  if (list == NULL) {
	  exit(1);
  }

  for (i = 0; i < N; ++i) {
    a[i].id = i;
	a[i].name = (char *)malloc(10 * sizeof(char));
	fscanf(list, "%s", a[i].name);
  }
  output = top(a);

  fclose(list);

  free(a);

  return 0;

}
