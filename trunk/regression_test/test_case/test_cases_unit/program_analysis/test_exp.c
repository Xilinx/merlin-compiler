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


int g_const = 10;
int g = 10;
const int c_g = 10;
int func(int i);

int main(int * a, int * b, int* c)
{
    int x;
    int y;
    int z;
    int i;
    x+x;
    (2+1+3*x)/3;
    1;
    2;
    0;
    -10;
    2.5;
    (35);
    (3*5);
    24*(-1);
    3/2;
    -3/2;
    3/-2;
    -3/-2;

    +1;
    g;
    g++;
	++g;
	--g;
	g--;
    g;
    x;
    +x;
    -x;
    -(x);
    -(-x);
    x+y;
    x-y;
    -x+y;
    1-x;
    x+20;
    x-y+20;
    x-(y+20);
    (x-y)+y;

    2*x;
    -(2*x);
    1-2*x;
    -1*x;
    (-1) * x;
    3*x+y-4*x;

    2*x/2;
    (2+2*x)/2;
    (2+3*x)/2;

    (3+2*x+3*y+4*z)/2;
    (3+2*x+3*y+4*z)/3;

    x*y;
    func_a();
    x < y;
    33*(125*(128*x)+y)+z;
    x++;
    x+=1;

    c=0;
    c;
    g_const;
   
    int i_a = y;
    int i_b = i_a+1;
    int i_c = 3*i_b;

    i_c;
    int arr[100];
    for (i = 0; i < 100; i++)
    {
        arr[i]; 
        arr[i+1]; 
        arr[0];
    }
    x = arr[i];
    y = arr[i];
    x - y;
    int j;
    x = arr[i * j];
    y = arr[i * j];
    x - y;
    j = i;
    x = arr[i];
    y = arr[j];
    x - y;
    int index1 = i;
    int index2 = i;
    x = arr[index1];
    y = arr[index2];
    x - y;
    x = arr[func(i)];
    y = arr[func(i)];
    x - y;
    x = arr[i++];
    y = arr[i++];
    x - y;
    for (i = 0; i < 100; i++) {
      ++j;
      int k = x;
      k * 2;
      y = arr[i];
      z = arr[k];
      k + i;
      k + z;
      i * j;
    }

    for (i = 0; i < z; i++) {
      arr[i + 1];
      int m = 1;
      for (j = i; j < 100;) {
         j++;
         i * 2;
         y = arr[i];
         z = arr[j];
         i * j + i;
         y + z;
         int k;
         if ( j % 2 ) {
           k = 0;
           m = 2;
         } else 
           k = 1;
         z = arr[k];
         y = arr[m];

      }
      arr[j];
    }
//    int i;
    for (i = 0; i < 100; i++) {
      arr[i] = i;
      arr [5];
      int k = i / 3 + 1;
      int k2 = i / 3;
      int k3 = i / 3;
      k - k2;
      k3 - k2;
    }
    int arr_2[c_g];
    c_g + 10;
    return 1;
}
