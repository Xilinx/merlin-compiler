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


// 1. loop structure
// 2. function structure
// 3. access pattern
// 4. pragma modes

#pragma ACCEL kernel
void func_top_0()
{
    int * a;
    int * b;
    int i;
#pragma ACCEL pipeline_parallel factor = 10
    for (i = 0; i < 100; i++)
    {
        a[i] = b[i];
    }
}


#pragma ACCEL kernel
void func_top_1()
{
    int a[100][100];
    int b[100][100];
    int i, j;
#pragma ACCEL parallel factor = 10
    for (i = 0; i < 100; i++)
    for (j = 0; j < 100; j++)   //bracelet
    {
        a[i][j] = b[i][j];
    }
}


#pragma ACCEL kernel
void func_top_2()
{
    int a[100][100];
    int b[100][100];

    int i, j;
#pragma ACCEL pipeline_parallel factor = 10 flatten
    for (i = 0; i < 100; i++) {
        for (j = 0; j < 100; j++)
        {
            a[i][j] = b[i][j];
        }
    }
}

#pragma ACCEL kernel
void func_top_3()
{
    int a[100][100];
    int b[100][100];

    int i, j;
    for (i = 0; i < 100; i++) {
#pragma ACCEL parallel factor = 10
        for (j = 0; j < 100; j++)
            a[i][j] = b[i][j];
    }
}

//Loop structure 
#pragma ACCEL kernel
void func_top_4()
{
    int a[100][100];
    int b[100][100];

    int i, j;
#pragma ACCEL pipeline
    for (i = 0; i < 100; i++)
    {
#pragma ACCEL parallel factor = 10
        for (j = 0; j < 100; j++)
        {
            a[i][j] = b[i][j];
        }
#pragma ACCEL parallel factor = 10
        for (j = 0; j < 100; j++)
        {
            a[i+1][j] = b[i][j+1];
        }
    }
}

#pragma ACCEL kernel
void func_top_5()
{
    int a[100][100];
    int b[100][100];

    int i=0, j=0;

#pragma ACCEL parallel factor = 10
    for (j = 0; j < 100; j++)
    {
        a[i+1][j] = b[i][j+1];
    }

#pragma ACCEL pipeline flatten
    for (i = 0; i < 100; i++)
    {
        for (j = 0; j < 100; j++)
        {
            a[i][j] = b[i][j];
        }
#pragma ACCEL pipeline flatten
#pragma ACCEL pipeline flatten
        for (j = 0; j < 100; j++)
        {
            a[i+2][j] = b[i+2][j];
        }
#pragma ACCEL pipeline factor = 10
        for (j = 0; j < 100; j++)
        {
            a[i+1][j] = b[i][j+1];
        }
    }

}

//ifelse
#pragma ACCEL kernel
void func_top_6()
{
    int a[100][100];
    int b[100][100];

    int i, j;
    for (i = 0; i < 100; i++) {
#pragma ACCEL parallel factor = 10
        for (j = 0; j < 100; j++) {
            if (a[0][0]) 
                a[i][j] = b[i][j];
        }
    }
}

#pragma ACCEL kernel
void func_top_7()
{
    int a[100][100];
    int b[100][100];

    int i, j;
    for (i = 0; i < 100; i++) {
        if (a[0][0]) 
#pragma ACCEL parallel factor = 10
          for (j = 0; j < 100; j++) {
                a[i][j] = b[i][j];
        }
    }
}

#pragma ACCEL kernel
void func_top_8()
{
    int a[100][100];
    int b[100][100];

    int i, j;
    for (i = 0; i < 100; i++) {
        if (a[0][0]) 
#pragma ACCEL parallel factor = 10
          for (j = 0; j < 100; j++) {
            a[i][j] = b[i][j];
        }
    }
}

// sub function
void sub_0(int a[100], int b[100])
{

    int i, j;
    for (i = 0; i < 100; i++) {
        if (a[0]) 
#pragma ACCEL parallel factor = 10
          for (j = 0; j < 100; j++) {
            a[j] = b[j];
        }
    }
}
#pragma ACCEL kernel
void func_top_9()
{
    int a[100][100];
    int b[100][100];

#pragma ACCEL pipeline FLATTEN
for (int i = 0; i < 100; ++i)
    sub_0(a[i], b[i]);
}



int sub_0(int a[100], int b[100][100], int c)
{
    int i, j;
    for (i = 0; i < 100; i++) {
#pragma ACCEL parallel factor = 10
        for (j = 0; j < 100; j++) {
            a[i] = b[i][j] + c;
        }
    }
    return 1024;
}
#pragma ACCEL kernel
void func_top_10()
{
    int a[100][100];
    int b[100][100];


    int c = 1;
    int sum = sub_0(a[0], b, c);
}



void sub_1(int a[100], int b[100])
{
    int i, j;
    for (i = 0; i < 100; i++) {
        if (a[0]) 
#pragma ACCEL parallel factor = 10
          for (j = 0; j < 100; j++) {
            a[j] = b[j];
        }
    }
}
#pragma ACCEL kernel
void func_top_11()
{
    int a[100][100];
    int b[100][100];
#pragma ACCEL pipeline flatten
 for (int i = 0; i < 100; ++i) {
    sub_1(a[i], b[i]);
    sub_1(a[i], b[i]);

 }
}


int sub_2(int b[100][100], int i, int j)
{
    int sum = 0;
    for (int k = 0; k < 100; ++k)
      sum += b[i][k] + b[k][j+1];
    return sum;
}

int sub_00(int a[100], int b[100][100], int c)
{
    int i, j;
    for (i = 0; i < 100; i++) {
#pragma ACCEL parallel factor = 10
        for (j = 0; j < 100; j++) {
            a[i] = sub_2(b, i, j);
        }
    }
    return 1024;
}
#pragma ACCEL kernel
void func_top_12()
{
    int a[100][100];
    int b[100][100];
    int sum = 0;
#pragma ACCEL pipeline flatten reduction=sum 
    for (int i = 0; i < 100; ++i) {
     sum += sub_00(a[i], b, i);
    }

}

int sub_3(int b[100][100], int i, int j)
{
    int sum = 0;
    for (int k = 0; k < 100; ++k)
      sum += b[i][k] + b[k][j+1];
    return sum;
}

int sub_30(int a[100], int b[100][100], int c)
{
    int i, j;
    for (i = 0; i < 100; i++) {
#pragma ACCEL parallel factor = 10
        for (j = 0; j < 100; j++) {
            a[i] = sub_3(b, i, j);
        }
    }
    return 1024;
}
#pragma ACCEL kernel
void func_top_13()
{
    int a[100][100];
    int b[100][100];
    int sum = 0;
#pragma ACCEL pipeline flatten  
    for (int i = 0; i < 100; ++i) {
     sum += sub_30(a[i], b, i);
    }

}


// pragma modes
#pragma ACCEL kernel
void func_top_26()
{
    int a[100][100];
    int b[100][100];

    int i, j;
    for (i = 0; i < 100; i++)
#pragma ACCEL pipeline
        for (j = 0; j < 100; j++)
        {
            a[i][j] += a[i+1][j] * a[i-1][j];
        }
}


// pragma modes
#pragma ACCEL kernel
void func_top_27()
{
    int a[100][100];
    int b[100][100];

    int i, j;
    for (i = 0; i < 100; i++)
#pragma ACCEL parallel factor = 10
        for (j = 0; j < 100; j++)
        {
            a[i][j] += a[i+1][j] * a[i-1][j];
        }
}



// pragma modes
#pragma ACCEL kernel
void func_top_28()
{
    int a[100][100];
    int b[100][100];

    int i, j;
    for (i = 0; i < 100; i++)
#pragma ACCEL parallel complete
        for (j = 0; j < 100; j++)
        {
            a[i][j] += a[i+1][j] * a[i-1][j];
        }
}


// pragma modes
#pragma ACCEL kernel
void func_top_29()
{
    int a[100][100];
    int b[100][100];

    int i, j;
    for (i = 0; i < 100; i++)
#pragma ACCEL parallel complete
        for (j = 0; j < 100; j++)
        {
            a[i][j] += a[i+1][j] * a[i-1][j];
        }
}


// pragma modes
#pragma ACCEL kernel
void func_top_30()
{
    int a[100][100];
    int b[100][100];

    int i, j;
    for (i = 0; i < 100; i++)
#pragma ACCEL pipeline complete priority = logic
        for (j = 0; j < 100; j++)
        {
            a[i][j] = b[i-1][j] + b[i+1][j] + b[i][j];
        }
}

// test priority
#pragma ACCEL kernel
void func_top_31()
{
    int a[100][100];
    int b[100][100];

    int i, j;
    for (i = 0; i < 100; i++)
#pragma ACCEL parallel factor=4 priority=logic
        for (j = 0; j < 100; j++)
        {
            a[i][j] = b[i-1][j] + b[i+1][j] + b[i][j];
        }
}

//test pragma parameter
#pragma ACCEL kernel
void func_top_32()
{
    int a[100];
    int b[100];

  int i;
#pragma ACCEL parallel factor=0
  for (i = 0; i < 100; i++) 
{
    a[i] = b[i];
  }
}

#pragma ACCEL kernel
void func_top_33()
{
    int a[100];
    int b[100];

  int i;
#pragma ACCEL parallel factor = -1
  for (i = 0; i < 100; i++) {
    
    a[i] = b[i];
  }
}

#pragma ACCEL kernel
void func_top_34()
{
    int a[100];
    int b[100];

    int i;
    int NN = 100;
#pragma ACCEL parallel factor = NN
    for (i = 0; i < NN; i++)
    {
        a[i] = b[i];
    }
}

#pragma ACCEL kernel
void func_top_38()
{
    int a[100][100];
    int b[100][100];

    int i, j;
#pragma ACCEL pipeline
#pragma ACCEL false_dependence variable=a
    for (i = 0; i < 100; i++)
    {
#pragma ACCEL parallel complete
        for (j = 0; j < 100; j++)
        {
            a[i][j] = b[i][j];
        }
    }
}

#pragma ACCEL kernel
void func_top_39()
{
    int a[100][100];
    int b[100][100];

    int i, j;
#pragma ACCEL pipeline
    for (i = 0; i < 100; i++)
    {
#pragma ACCEL parallel factor=10
        for (j = 0; j < 100; j++)
        {
            a[i][j] = b[i][j];
        }
    }
}

#pragma ACCEL kernel
void func_top_41()
{
    int a[100][100];
    int b[100][100];

    int i, j, k;
        #pragma ACCEL pipeline
    for (k = 0; k < 100; k++)
    {
        for (i = 0; i < 100; i++)
        {
            for (j = 0; j < 100; j++)
            {
                a[i][j] = b[i][j];
            }
        }
    }
}

#pragma ACCEL kernel
void func_top_42()
{
    int a[100][100];
    int b[100][100];

    int i, j, k;
#pragma ACCEL parallel factor = 10
    for (k = 0; k < 100; k++)
    {
        for (i = 0; i < 100; i++)
        {
            for (j = 0; j < 100; j++)
            {
                a[i][j] = b[i][j];
            }
        }
    }
}
#pragma ACCEL kernel
void func_top_43()
{
    int a[100];
    int b[100];

    int i;

    for (i = 0; i < 100; i++)
	{
		b[i]=i;
	}
#pragma ACCEL parallel factor = 10
    for (i = 0; i < 100; i++)
    {
        a[i] = b[i];
    }
}

void sub_46(int a[100][100])
{
    int b[100][100];
    int i, j, k;
    for (i = 0; i < 100; i++) {
        for (j = 0; j < 100; j++) {
        int var1=i+1;
        int var0=i;
#pragma ACCEL pipeline
            for (k = 0; k < 100; k++) {
            a[var1][k] += b[var1][j+k];
            a[var0][k] += b[var0][j+k];
        }
    }
   }
}

#pragma ACCEL kernel
void func_top_46()
{
	int aa[100][100];

	sub_46(aa);
}

#pragma ACCEL kernel
void func_top_47()
{
    int a[100][100];
    int b[100][100];

    int i, j, k;
#pragma ACCEL parallel factor = 10
    k = 0;
    while (k < 100)
    {
        i = 0;
        while (i < 100)
        {
            j = 0;
            while (j < 100)
            {
                a[i][j] = b[i][j];
                ++j;
            }
            ++i;
        }
        ++k;
    }
}

#pragma ACCEL kernel
void func_top_48()
{
    int a[100][100];
    int b[100][100];

    int i, j, k;
#pragma ACCEL parallel factor = 10
    k = 0;
    while (k < 100)
    {
        i = 0;
        while (i < 100)
        {
            j = 0;
            while (j < 100)
            {
                a[i][j] = b[i][j];
                ++j;
            }
            ++i;
        }
        ++k;
    }
}

/*counter example*/
#pragma ACCEL kernel
void func_top_49()
{
    int a[100][100];
    int b[100][100];

    int i, j, k;
#pragma ACCEL tiling factor = 10
    k = 0;
    while (k < 100)
    {
        i = 0;
        while (i < 100)
        {
            j = 0;
            while (j < 100)
            {
                a[i][j] = b[i][j];
                ++j;
            }
            ++i;
        }
        ++k;
    }
}

#define F 2
#define skip_check
#pragma ACCEL kernel
void func_top_50() {
  for (int i = 0; i < 10; ++i) {
    // should not replace 'skip_check'
#pragma HLS unroll factor=F skip_check
  }
}
