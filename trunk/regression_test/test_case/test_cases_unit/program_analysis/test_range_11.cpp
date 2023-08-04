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
//Testing canonizalize the loops with label
#pragma ACCEL kernel
void func_loop(int aa[100], int bb[100])
{
L1:   for (unsigned char ii = 0; ii < 100; ii++)
			{
				aa[ii] = bb[ii] ;    
			}
			int a[10];
			int b[10];
			int c[10];

			int j;
			int x, y, z;
			int N = 100;
			char i;
L2:    for (i = 0; i < 100; i++)
			 {
				 c[i] = a[i] + b[i] ;    
			 }
			 {
L20:    for (i = 0; i < 100; i++)
			 {
				 c[i] = a[i] + b[i] ;    
			 }
			 }
			 {
			unsigned char i0;
L3:    for (i0 = 0; i0 < 100; i0++)
			 {
				 c[i0] = a[i0] + b[i0] ;    
			 }
			 }
L4:    for (i = 0, j = 0; i < 100; i++)
			 {
				 c[i] = a[i] + b[j] ;    
			 }
			 {
L5:    for (i = 0, j = 0; i < 100; i++)
			 {
				 c[i] = a[i] + b[j] ;    
			 }
			 }
			 {
			unsigned char i0;
L6:    for (i0 = 0, j = 1; i0 < 100; i0++)
			 {
				 c[i0] = a[i0] + b[j] ;    
			 }
			 }
}

int main() {
  int aa[100];
  int bb[100];
  func_loop(aa, bb);
}
