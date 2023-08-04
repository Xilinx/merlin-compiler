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
#ifndef _HEADER_H
#define _HEADER_H

//#include "stdlib.h"
//#include "string.h"
//#include "stdio.h"

#include "math.h"
#define SQR(x) (x)*(x)
#define CUB(x) (x)*(x)*(x)

/*
#define IMG_P 10 
#define IMG_N 10 
#define IMG_M 10 
#define TS_P 5 
#define TS_N 5 
#define TS_M 5 
#define PITCH_P (IMG_P+2)
#define PITCH_N (IMG_N+2) 
#define PITCH_M (IMG_M+2) 
*/

#define IMG_SIZE ((IMG_M+2)*(IMG_N+2)*(IMG_P+2))

union cmost_int_float_type
{
	int i;
	float f;
};

#endif
