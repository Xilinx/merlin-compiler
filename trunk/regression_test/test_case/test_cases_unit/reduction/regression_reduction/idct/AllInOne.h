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
#ifndef __AllInOne_h
#define __AllInOne_h

#include <math.h> 
#include <stdarg.h> 

#define MEM_SPACE_SIZE (2048*1024) 


//
// Constant definitions
//
#define MAX_STREAMS			(    8 )
#define MAX_WIDTH				(  720 )
#define MAX_HEIGHT			(  576 )
#define BITS_DIM				(   10 )
#define BITS_PIXEL			(    8 )
#define BITS_QUANT			(    5 )
#define BITS_DCT				(   12 )
#define BITS_DCT_IQ			(   12 )
#define DIMX  100
#define DIMY  22
#define DIMZ  18
//#define MB_WIDTH				(   16 )
//#define MB_HEIGHT				(   16 ) 
#define B_WIDTH					(    8 )
#define B_HEIGHT				(    8 )
#define B_SIZE					(   64 ) // B_WIDTH * B_HEIGHT
#define B_COUNT					(    6 ) 
#define MAXVAL_PIXEL		(  255 ) // 2^BITS_PIXEL - 1
#define MIDVAL_PIXEL		(  128 ) // 2^(BITS_PIXEL - 1)
#define YDEFAULT				(  128 )
#define UDEFAULT				(  128 )
#define VDEFAULT				(  128 )
#define BFR_SIZE				( 2048 ) 
#define INBFR_BYTES			(    4 ) // mininum value: 5 
// marker codes and lengths
#define VOL_START_CODE								(0x012)
#define VOP_START_CODE								(0x1B6)
#define RESYNC_MARKER									(1)
#define VOL_START_CODE_LENGTH					(28)
#define VOP_START_CODE_LENGTH					(32)
#define MCBPC_LENGTH									( 9)
#define USER_DATA_START_CODE_LENGTH		(32)
#define SHORT_VIDEO_START_CODE_LENGTH (22)
#define START_CODE_PREFIX_LENGTH			(24)
#define VOL_HEADER_LENGTH							(18) // # bytes in VOL header
// frame and block types
#define I_VOP						(  0 ) 
#define P_VOP						(  1 ) 
#define B_VOP						(  2 ) 
#define INTRA						(  0 ) 
#define INTER						(  1 ) 
#define BITS_COEF				( 13 )
#define BITS_SHIFT			(  3 )

//added for OPENCL compilation
#define PARSER 1
#define DEQUANT 2 
#define IDCT_ROW 3 
#define IDCT_COL 4 
#define CC_MC 5 
#define TU 6 
#define DENOISE 7 

#endif 
