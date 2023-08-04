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

class Student 
{
  public: int id;
  char *name;
}
;
#pragma ACCEL kernel

void top(int *a_id,char **a_name)
{
  
#pragma ACCEL INTERFACE DEPTH=100,10 VARIABLE=a_name max_depth=100,10
  
#pragma ACCEL INTERFACE DEPTH=100 VARIABLE=a_id max_depth=100
  int i;
  for (i = 0; i < 100; ++i) {
    a_id[i] = a_id[i] + 10;
    a_name[i][1] = a_name[i][0];
  }
  return ;
}

int main(int argc,char *argv[])
{
  char **a_name;
  int *a_id;
  
#pragma ACCEL wrapper VARIABLE=a->id port=a_id data_type=int
  
#pragma ACCEL wrapper VARIABLE=a->name port=a_name data_type=char
  
#pragma ACCEL wrapper VARIABLE=a port=a data_type="class Student"
  int i;
  class Student *a = new Student [100];
  FILE *list;
  list = fopen(argv[1],"r");
  if (list == 0L) {
    exit(1);
  }
  for (i = 0; i < 100; ++i) {
    a[i] . id = i;
    a[i] . name = ((char *)(malloc(10 * sizeof(char ))));
    fscanf(list,"%s",a[i] . name);
  }
  top(a_id,a_name);
  fclose(list);
  delete []a;
  return 0;
}
