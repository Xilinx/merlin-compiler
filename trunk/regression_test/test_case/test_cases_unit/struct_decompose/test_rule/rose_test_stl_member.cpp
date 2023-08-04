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
#include <fstream>
#include <string>
#include <vector>
const int N = 100;

class Student 
{
  public: int id;
  std::string name;
  std::vector< char  , class std::allocator< char  >  > address;
  

  inline void update_name()
{
    for (int j = 0; j < (this) -> name . size(); j++) 
      (this) -> name[j] = (((this) -> name[j]) + '0');
  }
  

  inline void update_address()
{
    for (int j = 0; j < (this) -> address . size(); j++) 
      (this) -> address[j] = (((this) -> address[j]) + '0');
  }
}
;

#pragma ACCEL kernel
void top(class Student *a)
{
  int i;
  int j;
  for (i = 0; i < N; ++i) {
    a[i] . Student::id = a[i] . Student::id + 10;
    a[i] .  update_name ();
    a[i] .  update_address ();
  }
  return ;
}

int main(int argc,char *argv[])
{
  
#pragma ACCEL wrapper variable=a port=a
  class Student *a = new Student [N];
  std::ifstream fin;
  fin . open(argv[1]);
  if (fin . is_open()) {
    std::string name;
    std::string address;
    for (int i = 0; i < N; ++i) {
      a[i] . Student::id = i;
      (fin>>name)>>address;
      a[i] . Student::name = name;
      for (int j = 0; j < address . size(); j++) {
        a[i] . Student::address . push_back((address[j]));
      }
    }
  }
   else {
    return 1;
  }
  
  top(a);
  fin . close();
  delete []a;
  return 0;
}
