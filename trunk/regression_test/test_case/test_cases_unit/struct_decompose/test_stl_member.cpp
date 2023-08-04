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

class Student {
public:
  int id;
  std::string name;
  std::vector<char> address;

  void update_name() {
	  for (int j = 0; j < name.size(); j++)
		  name[j] = name[j] + '0';
  }
  void update_address() {
	  for (int j = 0; j < address.size(); j++)
		  address[j] = address[j] + '0';
  }
};

#pragma ACCEL kernel
void top(Student *a) {
#pragma ACCEL interface variable=a depth=100
	int i, j;
  for (i = 0; i < N; ++i) {
    a[i].id = a[i].id + 10;
	a[i].update_name();
	a[i].update_address();
  }

  return;
}

int main(int argc, char *argv[]) {

  Student *a = new Student[N];
  std::ifstream fin;

  fin.open(argv[1]);

  if (fin.is_open()) {
	  std::string name, address;
	  for (int i = 0; i < N; ++i) {
		  a[i].id = i;
		  fin >> name >> address;
		  a[i].name = name;
		  for (int j = 0; j < address.size(); j++) {
			  a[i].address.push_back(address[j]);
		  }
	  }
  }
  else {
	  return 1;
  }

  top(a);

  fin.close();

  delete [] a;

  return 0;

}
