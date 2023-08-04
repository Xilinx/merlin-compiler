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
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#define DIM11 2
#define DIM22 4

void top_kernel(int *aa,int *bb,int *c);

void baseline(int aa[DIM22], int bb[DIM22], int *c_base) {
	int i;
	int j;
	int a_buf[4];
	int val = 2;
	for (j = 0; j < DIM11; ++j) 
	{
		for (i = 0; i < DIM11; ++i) {
			a_buf[i] = 0;
		}
		i = 0;
		while (i < 2) {
			a_buf[i] = aa[val*i+j]+ bb[DIM11*j+i];
			c_base[i] += a_buf[i];
			i++;
		}
	}


	return;
}


int main() {
	int i;
	int err = 0;

	int aa[DIM22] = {1,1,1,1};
	int bb[DIM22]={4,3,2,1};
	int cc[DIM22]={0,0,0,0};	
	int cc_base[DIM22]={0,0,0,0};	

	int *c_base = cc_base;

	baseline(aa,bb,c_base);

	top_kernel(aa, bb, cc);

	for(i = 0; i < DIM22; i++){
		if(cc[i]!=cc_base[i])
			err++;
	}
	if (0 == err){
		printf ("Success! %d\n",err);
		char file_name_s[256] = "pass.o";
		FILE *fp_s=fopen(file_name_s,"ab+");
		if(fp_s==NULL) printf("can't create file!");
	}
	else  {
		printf ("Fail! %d\n",err);
		char file_name[256] = "fail.o";
		FILE *fp=fopen(file_name,"ab+");
		if(fp==NULL) printf("can't create file!");
	}

	return 0;
}
