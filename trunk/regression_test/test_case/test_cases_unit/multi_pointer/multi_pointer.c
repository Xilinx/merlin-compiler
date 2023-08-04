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
#include<string.h>
#include<stdio.h>
#include<assert.h>

#define DIM11 2
#define DIM22 4
#define DIM33 8

const int DIM1 = 2;
const int DIM2 = 4;
const int DIM3 = 8;

int bb_out[DIM22][DIM33];
int cc_out[DIM33];
int cc_out2[DIM33];


int pointer_test_sub(int *a, int *b, int *c){
	
/*#pragma ACCEL interface variable=a depth=DIM3     		continue=false
#pragma ACCEL interface variable=b depth=DIM3     		continue=false*/
	printf("0000....\n");

	int tmp[DIM3];
	int i;
    for(i = 0; i < DIM3; i++)
		tmp[i] = a[i] + b[i] + c[i];
	printf("1111....\n");
	return tmp[0];
}

void pointer_test(int ***a,int **b,int *c,int **b_out,int *c_out,int *c_out2){

/*#pragma ACCEL interface variable=a depth=DIM1,DIM2,DIM3 continue=false
#pragma ACCEL interface variable=b depth=DIM2,DIM3   	continue=false
#pragma ACCEL interface variable=c depth=DIM3     		continue=false
#pragma ACCEL interface variable=bb_out depth=DIM2,DIM3  continue=true
#pragma ACCEL interface variable=cc_out depth=DIM3     	 continue=true
#pragma ACCEL interface variable=cc_out2 depth=DIM3      continue=true*/


	b_out = (int **)a[1];
	//printf("000....\n");

	c_out = (int *)(&a[1][1]);
	//printf("111....\n");
	int **tmp1;
	int tmp2[DIM3];

	tmp1 = *a;
	//printf("222....\n");
	tmp2[0] = pointer_test_sub((int *)tmp1[0],b[1],c);
	//printf("333....\n");
	memcpy(c_out2,tmp2,DIM3*sizeof(int));
	//printf("444....\n");

}

int array_test_sub(int aa[DIM33], int bb[DIM33], int cc[DIM33]){

	int tmp[DIM3];
	int i;
    for(i = 0; i < DIM3; i++)
		tmp[i] = aa[i] + bb[i] + cc[i];
	return tmp[0];
}

void array_test(int aa[DIM11][DIM22][DIM33],int bb[DIM22][DIM33],int cc[DIM33],int bb_out[DIM22][DIM33],int cc_out[DIM33],int cc_out2[DIM33]){
	
/*#pragma ACCEL interface variable=aa depth=DIM1,DIM2,DIM3 continue=true
#pragma ACCEL interface variable=bb depth=DIM2,DIM3   	 continue=true
#pragma ACCEL interface variable=bb_out depth=DIM2,DIM3  continue=true
#pragma ACCEL interface variable=cc_out depth=DIM3     	 continue=true
#pragma ACCEL interface variable=cc_out2 depth=DIM3      continue=true*/

	

	memcpy(bb_out,aa[1],DIM2*DIM3*sizeof(int));
	memcpy(cc_out,aa[1][1],DIM3*sizeof(int));

	int tmp1[DIM2][DIM3];
	int tmp2[DIM3];
	
	memcpy(tmp1,aa[0],DIM2*DIM3*sizeof(int));
    tmp2[0] = array_test_sub(tmp1[1],bb[1],cc);
	memcpy(cc_out2,tmp2,DIM3*sizeof(int));
	
}

int pointer_array_test_sub(int *a[DIM22], int **b, int (*bb)[DIM33]){
	
/*#pragma ACCEL interface variable=a  depth=DIM2,DIM3     continue=false
#pragma ACCEL interface variable=b  depth=DIM2,DIM3     continue=false
#pragma ACCEL interface variable=bb depth=DIM2     		continue=false*/

	//printf("0000....\n");

	int tmp[DIM2][DIM3];
	int i,j;
    for(i = 0; i < DIM2; i++)
		for(j = 0; j < DIM3; j++)
		tmp[i][j] = *(a[i]+j) + bb[i][j];
	//printf("1111....\n");
	return tmp[0][0];
}

void pointer_array_test1(int (*a1)[DIM22][DIM33],int **b,int *c,int *(*a2)[DIM22],int (*b1)[DIM33],int cc[DIM33],
								int **bb_out,int *cc_out,int *cc_out2){

/*#pragma ACCEL interface variable=a1 depth=DIM1,DIM2,DIM3 continue=false
#pragma ACCEL interface variable=b depth=DIM2,DIM3   	continue=false
#pragma ACCEL interface variable=c depth=DIM3     		continue=false

#pragma ACCEL interface variable=a2 depth=DIM1,DIM2 	 continue=false
#pragma ACCEL interface variable=b1 depth=DIM2           continue=false
#pragma ACCEL interface variable=cc depth=DIM3     		 continue=true

#pragma ACCEL interface variable=bb_out depth=DIM2,DIM3  continue=true
#pragma ACCEL interface variable=cc_out depth=DIM3     	 continue=true
#pragma ACCEL interface variable=cc_out2 depth=DIM3      continue=true*/	


	bb_out = a1+2;
	//printf("000....\n");

	cc_out = a1[1][1];
	//printf("111....\n");

	int **tmp1;
	int tmp2[DIM3];

	tmp1 = a1[0]; 
	//printf("222....\n");
	tmp2[0] = pointer_array_test_sub(tmp1,b,b1);
	//printf("333....\n");
//	tmp2[1] = pointer_test_sub(tmp1[1],b[1],b1[1]);
	//printf("444....\n");
	memcpy(cc_out2,tmp2,DIM3*sizeof(int));
	//printf("555....\n");

}


void pointer_array_test2(int *a3[DIM11][DIM22],int **b,int *c,int **a4[DIM11],int a_depth2,int a_depth3,int *b2[DIM22],
								int *bb1_out[DIM22],int *cc_out,int *cc_out2){

/*#pragma ACCEL interface variable=a3 depth=DIM1,DIM2,DIM3 continue=false
#pragma ACCEL interface variable=b depth=DIM2,DIM3   	continue=false
#pragma ACCEL interface variable=c depth=DIM3     		continue=false

#pragma ACCEL interface variable=a4 depth=aa_depth1,aa_depth2 	continue=false
#pragma ACCEL interface variable=b2 depth=DIM2   	 		    continue=false

#pragma ACCEL interface variable=bb1_out depth=DIM2  	 continue=false
#pragma ACCEL interface variable=cc_out depth=DIM3     	 continue=true
#pragma ACCEL interface variable=cc_out2 depth=DIM3      continue=true*/



	bb1_out = a3[1];
	cc_out = a3[1][1]; 

	int **tmp1;
	int tmp2[DIM3];

	tmp1 = a3[1]; 
	tmp2[0] = pointer_array_test_sub(tmp1,b,(int (*)[DIM33])b2);
	tmp2[1] = pointer_test_sub(tmp1[1],b[1],b2[1]);
	memcpy(cc_out2,tmp2,DIM3*sizeof(int));

}

#pragma ACCEL kernel
void kernel_top(int ***a,int **b,int *c,int aa[DIM11][DIM22][DIM33],int bb[DIM22][DIM33],int cc[DIM33],
	int (*a1)[DIM22][DIM33],int *(*a2)[DIM22],int *a3[DIM11][DIM22],int **a4[DIM11],
	int (*b1)[DIM33],int *b2[DIM22],int a_depth2,int a_depth3,
	int bb_out[DIM22][DIM33],int *bb1_out[DIM22],int cc_out[DIM33],int cc_out2[DIM33]){

    assert(a_depth2 < 100);
	assert(a_depth3 < 100);
		
	#pragma ACCEL interface variable=a depth=DIM1,DIM2,DIM3 continue=false
	#pragma ACCEL interface variable=b depth=DIM2,DIM3   	continue=false
	#pragma ACCEL interface variable=c depth=DIM3     		continue=false
	#pragma ACCEL interface variable=aa depth=DIM1,DIM2,DIM3 continue=true
	#pragma ACCEL interface variable=bb depth=DIM2,DIM3   	 continue=true
	#pragma ACCEL interface variable=cc depth=DIM3   	 	continue=true
	#pragma ACCEL interface variable=a1 depth=DIM1,DIM2,DIM3 continue=false
	#pragma ACCEL interface variable=a2 depth=DIM1,,a_depth3 	 continue=false
	#pragma ACCEL interface variable=a3 depth=DIM1,DIM2,DIM3 continue=false
	#pragma ACCEL interface variable=a4 depth=,a_depth2,a_depth3 	continue=false
	#pragma ACCEL interface variable=b1 depth=DIM2,           continue=false
	#pragma ACCEL interface variable=b2 depth=,DIM3   	 		    continue=false
	#pragma ACCEL interface variable=bb_out depth=DIM2,DIM3  continue=true
	#pragma ACCEL interface variable=bb1_out depth=,DIM3  	 continue=false
	#pragma ACCEL interface variable=cc_out depth=DIM3     	 continue=true
	#pragma ACCEL interface variable=cc_out2 depth=DIM3      continue=true

	int **b_out;
	int *c_out;
	int *c_out2;

	array_test(aa,bb,cc,bb_out,cc_out,cc_out2);
    pointer_test(a,b,c,b_out,c_out,c_out2);
	pointer_array_test1(a1,b,c,a2,b1,cc,b_out,c_out,c_out2);
	pointer_array_test2(a3,b,c,a4,a_depth2,a_depth3,b2,bb1_out,c_out,c_out2);

}

void main(void)
{
    int aa[DIM11][DIM22][DIM33]=
		{
			{{1,2,3,4,5,6,7,8},{11,12,13,14,15,16,17,18},{21,22,23,24,25,26,27,28},{31,32,33,34,35,36,37,38}},
			{{101,102,103,104,105,106,107,108},{111,112,113,114,115,116,117,118},{121,122,123,124,125,126,127,128},{131,132,133,134,135,136,137,138}}
		};
	int bb[DIM22][DIM33]={{8,7,6,5,4,3,2,1},{18,17,16,15,14,13,12,11},{28,27,26,25,24,23,22,21},{38,37,36,35,34,33,32,31}};
	int cc[DIM33]={11,22,33,44,55,66,77,88};	
	
    int ***a = aa;
	int **b = bb;
    int *c = cc;

	int (*a1)[DIM22][DIM33] = aa;
	int *(*a2)[DIM22] = aa;
	int *a3[DIM11][DIM22] = 
		{
			{&aa[0][0],&aa[0][1],&aa[0][2],&aa[0][3]},
			{&aa[1][0],&aa[1][1],&aa[1][2],&aa[1][3]}
		};
	int **a4[DIM11] = {&aa[0],&aa[1]};
	int (*b1)[DIM33] = bb;
	int *b2[DIM22] = {&bb[0],&bb[1],&bb[2],&bb[3]};
	int *bb1_out[DIM22] = {&bb[0],&bb[1],&bb[2],&bb[3]};
	int a_depth2 = DIM22;
	int a_depth3 = DIM33;

	
	kernel_top(a,b,c,aa,bb,cc,a1,a2,a3,a4,b1,b2,a_depth2,a_depth3,bb_out,bb1_out,cc_out,cc_out2);

    int i,j;
	for(i = 0; i < DIM22; i++)
		for(j = 0; j < DIM33; j++)
			printf("i = %d, j = %d, bb_out = %d\n",i,j,bb_out[i][j]);

	for(i = 0; i < DIM33; i++){
		printf("i = %d, cc_out = %d\n",i,cc_out[i]);
	}

	for(i = 0; i < DIM33; i++){
		printf("i = %d, cc_out2 = %d\n",i,cc_out2[i]);
	}
	
	
}

