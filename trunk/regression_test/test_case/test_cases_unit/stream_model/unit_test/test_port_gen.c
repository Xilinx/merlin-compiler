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

void read_only(int x)
{
  //x; 
  x++; // even in this case, it is read only on interface
}
void write_only(int * x)
{
  *x = 0;
}
void update_only(int * x)
{
  *x++;
}


#if FOCUS_TEST



void func4(int a[100])  {
  a[0] = 0;
}

#pragma ACCEL kernel
void kernel_top4(int a_buf[100])
{
  func4(a_buf);
}






//void func_sub(int a[10])             
//{                                
//  for (int i = 0; i < 10; ++i) {
//    a[i];                      
//  }                            
//}                              
//                               
//#pragma ACCEL kernel
//void top(int *a,int *b,int *c)            
//{                                        
//  int a_buf_0[100][10];                 
//  for (int j = 0; j < 100; ++j)        
//  {                                   
//#pragma ACCEL PARALLEL COMPLETE      
//    func_sub(a_buf_0[j]);           
//  }                                
//}


//#pragma ACCEL kernel
//void test_5_2_1(int a[100], int j)
//{
//  a[j];
//}
//#pragma ACCEL kernel
//void test_5_2_2(int a[100], int j)
//{
//  j++;
//  a[j];
//}
//#pragma ACCEL kernel
//void test_5_2_3(int a[100], int j)
//{
//  a[j++];
//}
//#pragma ACCEL kernel
//void test_5_3(int j)
//{
//  j;
//  j+1;
//}
//#pragma ACCEL kernel
//void test_5_3_1(int j)
//{
//  j++;
//}

//#pragma ACCEL kernel
//void test_5_2(int a[100], int j)
//{
//  (a[j]);
//  for (int i = 0; i < 100; i++)
//    a[i] = 0;
//  a[j];
//  for (int k = 0; k < 100; k++)
//    a[j+2];
//}


#if 0



#endif

#else // FOCUS_TEST


#pragma ACCEL kernel
void test_1_3_0(int *a, int base )
{
  for (int i = 0; i < 100; i++)
  {
    int new_base;
    if (i == 0) new_base = 1;
    else new_base = base;
    //new_base = (i==0) ? 1 : base;
    a[i + new_base];
  }
}

#pragma ACCEL kernel
void test_1_3_1(int *a, int base )
{
  for (int i = 0; i < 100; i++)
  {
    int new_base = base;
    write_only(&new_base);
    a[i + new_base]; // not apply if write
  }
}
#pragma ACCEL kernel
void test_1_3_1_1(int *a, int base )
{
  for (int i = 0; i < 100; i++)
  {
    int new_base = base;
    read_only(new_base);
    a[i + new_base]; // apply if read_only
  }
}

#pragma ACCEL kernel
void test_1_3_2(int *a, int base )
{
  for (int i = 0; i < 100; i++)
  {
    int new_base = base;
    new_base ++;
    a[i + new_base];
  }
}

#pragma ACCEL kernel
void test_1_3_2_1(int *a, int base )
{
  for (int i = 0; i < 100; i++)
  {
    a++;
    a[i + base];
  }
}


#pragma ACCEL kernel
void test_1_3_3(int *a, int base )
{
  int new_base; write_only(&new_base);
  for (int i = 0; i < new_base; i++)
  {
    a[i];
  }
}


#pragma ACCEL kernel
void test_1_3_4(int * a)
{
  int i;
  for (i = 0; i < 100; i++)
  {
  }
  a[i];
}

#pragma ACCEL kernel
void test_1_3_5(int b[100],int a[100][100])
{
  int i,j;
  for (i = 0; i < 100; i++)
  for (j = 0; j< 100; j++)
  {
    a[i][j];
    b[a[i][j]];
  }
}

#pragma ACCEL kernel
void test_2_0_0(int * a)
{
  int i = 0;
  while(i)
    a[i] = 245;
}

#pragma ACCEL kernel
void test_2_0_1(int * a)
{
  int i = 0;
  do
  {
    a[i];
  }while (i = 0);
}

#pragma ACCEL kernel
void test_2_1_0(int * a)
{
L_2_1_0:
  for (int i = 0; i < 5; i++)
  {
    a[i];
    goto L_2_1_0;
  }
}
#pragma ACCEL kernel
void test_2_1_1(int * a)
{
L_2_1_1:
  for (int i = 0; i < 5; i++)
  {
    a[i];
  }
}
//#pragma ACCEL kernel
//void test_2_1_2(int * a)
//{
//L_2_1_2:
//  for (int i = 0; i < 5; i++)
//  {
//    a[i];
//  }
//}
//void test_2_1_2_sub(int * a)
//{
//  goto L_2_1_2; //syntax error, label are local to function
//}
#pragma ACCEL kernel
void test_2_1_3(int * a) 
{
  for (int i = 0; i < 5; i++)
  {
    a[i] = 0;
  }
}
void test_2_1_3_sub(int * a)
{
L_2_1_3:
  test_2_1_3(a);
  goto L_2_1_3;
}

#pragma ACCEL kernel
void test_2_2_0(int * a, int * b)
{
  for (int i = 0; i < 5; i++)
  {
    a[i] = b[i];
    continue;
  }
}
#pragma ACCEL kernel
void test_2_2_1(int * a, int * b)
{
  for (int i = 0; i < 5; i++)
  {
    a[i] = b[i];
    break;
  }
}
#pragma ACCEL kernel
void test_2_2_2(int * a, int * b)
{
  for (int i = 0; i < 5; i++)
  {
    a[i] = b[i];
    for (int j = 0; j < 10; j++)
      continue;
  }
}


#pragma ACCEL kernel
void test_2_3(int * a, int * b, int base)
{
  int j;
  if (base == 0) {
    for (int i = 0; i < 5; i++)
    {
      if (i == 0) {
        for (j = 0; j < 5; j++)
        {
          if (a[i] == 0)
            a[i+j];
        }
      }
      a[i];
    }
    for (int i = 0; i < 5; i++)
    {
      b[i];
      a[i+2];
    }
  }
}

#pragma ACCEL kernel
void test_2_4(int *a )
{
  int i = 0;
  switch (i)
  {
    case 0:
      a[i];
    case 1:
      a[i+1];
  }
}

#pragma ACCEL kernel
void test_2_5_1(int *a )
{
  int i = 0;
  for (; i < 10; i++)
    a[i];
}

#pragma ACCEL kernel
void test_2_5_2(int *a )
{
  int i;
  for (i = 0; i < 10; i+=4)
    a[i];
}

#pragma ACCEL kernel
void test_2_6(int *a )
{
  int i;
  for (i = 0; i < 10; i++)
  {
    a[i];
    i++;
  }
}

#pragma ACCEL kernel
void test_2_7(int *a )
{
  int i;
  for (i = 0; i < 10; i++)
  {
    for (int i = 0; i < 10; i++)
      a[i];
  }
}


#pragma ACCEL kernel
void test_2_8(int * a, int * b, int base)
{
  int j;
  int i;
  for (i = 0; i < 5; i++)
  {
    for (int j = 0; j < 5; j++)
    {
      a[i+j];
    }
    for (int j = 0; j < 5; j++)
    {
      a[i+j+1];
    }
    a[i];
  }
  for (i = 0; i < 5; i++)
  {
    int j;
    for (j = 0; j < 5; j++)
    {
      a[i+j];
    }
    b[i+6]; 
  }
}

#pragma ACCEL kernel
void test_2_9(int *a )
{
  int i;
  for (i = 0; i < 10; i++)
  {
#pragma ACCEL a
    for (int j = 0; j < 10; j++)
#pragma ACCEL1 b
#pragma ACCEL c
#pragma ACCEL d
      a[i+j];
  }
  int j = 0;
#pragma ACCEL e
  for (i = 0; i < 10; i++)
#pragma ACCEL f
#pragma ACCEL g
    a[i+2];
}

#pragma ACCEL kernel
void test_2_10(int *a, int * b)
{
  for (int i = 0; i < 100; i++)
  {
    if (i < 10) a[i];
    else if (i > 0) a[i+1];
    else for (int j = 0; j < 100; j++) a[i+j];
  }

  if (1 < 10) {}
  else b[0];
}


#pragma ACCEL kernel
void test_2_11(int *a )
{
  for (int i = 0; i < 100; i++)
  {
    int b = (i > 0) ? 0 : a[i];
  }
}


#pragma ACCEL kernel
void test_2_12(int * a, int * b, int base)
{
  if (a[0] == 0) ;

  for (int i = 0;i < a[1]; i++);
}


#pragma ACCEL kernel
void test_5_0(int a[100],int *b, int *c, int j)
{
  c[0] = 0;
  (a[j]);
  for (int i = 0; i < 100; i++)
    b[i], a[i];
}

#pragma ACCEL kernel
void test_5_1(int a[100], int j)
{
  (a[j]);
  for (int i = 0; i < 100; i++)
    a[i] ;
  a[j];
  for (int k = 0; k < 100; k++)
    a[j+2];
}

#pragma ACCEL kernel
void test_5_1_2(int a[100], int j)
{
  a[j+1] = 0;
  a[j];
}


void sub_5_4(int (*a)[10])
{
  for (int i = 0; i < 10; i++)
  for (int j = 0; j < 10; j++)
  a[i][j] = 0;
}

#pragma ACCEL kernel
void test_5_4(int a[100][10][10],int **b, int j)
{
  sub_5_4(a[j]);

  b[j][j];
}

#pragma ACCEL kernel
void test_5_10(int *a, int base )
{
  base;
  while (1) a[0];
}









#pragma ACCEL kernel
void org_test_0(int * a)
{
  for (int i = 0; i < 100; i++)
    a[i] = 245;
}

void sub_func(int * aa, int idx)
{
  int j;
  for (j = 0; j < idx; j++)
  {
    int b = aa[idx - j];
  }
}

#pragma ACCEL kernel
void org_test_1(int * a, int base)
{
  int b[100];
  int i, j;
  int o = 2 ;
  int offset = 100+o;

  for (i = 0; i < 100; i++)
  for (j = i; j < 2*i; j++)
  {
    if (i >= 25)
    {
      a[i+j+base+offset];
    }
  }

  b[0] = 0;

  for (int i = 0; i < 100 + base ; i++)
  {
    sub_func(a+i*i, i+3);
  }
}

void sub_2(int * a, int i)
{
  a[i] = 245;
}

#pragma ACCEL kernel
void org_test_2(int * a)
{
  for (int i = 0; i < 100; i++)
    sub_2(a, i);
}

#pragma ACCEL kernel
void org_test_3(int * a, int i)
{
  int * b = a+1;
  b[i];
}

#endif

