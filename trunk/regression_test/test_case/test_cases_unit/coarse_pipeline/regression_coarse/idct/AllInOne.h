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
//#include <sys/timeb.h>
//#include "autopilot_tech.h"
#include "intfix.h"

#ifdef WIN32 
#include "picture.h"
#include <cstdlib> 
#include <cstdio> 
#include <cstring> 
#else 
//#include <unistd.h> 
//#include <stdlib.h> 
//#include <stdio.h> 
//#include <string.h> 
#endif


//#include "hw_config.h"

#define MEM_SPACE_SIZE (2048*1024) 


// **************
// Directives
// **************
//#define VERBOSE
#ifdef WIN32
//#define VERBOSE
#define STOP_ON_ERROR
//#define USE_DISPLAY_GDI
#ifndef USE_DISPLAY_DIRECTX
#define USE_DISPLAY_DIRECTX
#endif
//#define WRITE_FILE
//#define READ_EXTMEM
//#define COMPARE_FILE
//#define WRITE_TB_FILES			// Uncomment to create HW testbench files
//#define LIMIT_TO_30FPS
#define VIEW_STATISTICS
#endif
 
//#define USE_ORIG_IDCT
#define USE_FAST_IDCT
#define likeHW_TC
//#define printCoefficients

// Do not write file when doing infinite loop on stream!
#ifdef TEST_INFINITE_LOOP
#undef WRITE_FILE
#endif
// Only one type of viewer allowed
#ifdef USE_DISPLAY_DIRECTX
#undef USE_DISPLAY_GDI
#endif

/////////////////////////////////////////////////////////////////////
// Typedefs, Structures, & Constants
/////////////////////////////////////////////////////////////////////

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
  
// Maximum size BufferY/U/V including pipeline delay
#define LBwidth ((2*MAX_WIDTH/MB_WIDTH) + 5)
#define LB_MAX					(  128 ) // power of 2 > LBwidth

#define PARSER_CLOCK		(   48 ) // clock frequency for parser/VLD (in MHz)

#define CYCLES_IDCT_SLOW			(  225 ) // cycles of texture/IDCT when intermittent
#define CYCLES_IDCT_FAST			(   81 ) // cycles of texture/IDCT using pipeline (3 stages)
#define CYCLES_IDCT_FAST_OLD	(  161 ) // cycles of texture/IDCT using pipeline (2 stages)

// Texture/IDCT input markers used in object FIFO (for generation of testbench files)
#define DC_MARKER	 						( 0xC0000 )
#define AC_MARKER							( 0xE0000 )
#define EOB_MARKER						( 0x60000 )

//
// Typedefs
//
typedef unsigned char UChar;
typedef short SInt;
typedef int Int;
typedef unsigned char Byte;
typedef unsigned char BYTE;
typedef short Short;
typedef char Char;
typedef unsigned long UInt;
typedef float Float;
//typedef FILE File;
typedef void Void;
typedef double Double;
typedef const int C_Int; 
typedef const UInt C_UInt; 

//
// Structures
//
struct Tcoef 
{ 
	uint1 last; 
	uint8 run; 
	uint12 level; 
	uint1 sign; 
};

struct VLCtab
{ 
	int18 val; 
	uint8 len; 
};

typedef struct  
{ 
	
//	UChar rdptr;
//	UChar rdbfr[BFR_SIZE+4];



        uint32 inbfr;
//	uint1 inbfr[8*3*INBFR_BYTES];
	//INBFR_BYTES=7
	uint3 incnt; 
	//UChar rdtop;
	#ifdef C_DEBUG
	unsigned long bitcount;
	#endif
	//unsigned long bstart,bfinish;
	//unsigned short streamnum;
} Bitstream;


//struct vol
//{
//	int displayWidth;
//	int displayHeight;
//	float framerate;
//	FILE *y_file; FILE *u_file; FILE *v_file;
//};
//typedef struct vol Vol;
//
//struct vop;
//typedef struct vop Vop; 

//
// Macros
//
  
// Comment macro (show only in verbose mode) 
#ifdef VERBOSE
#define COMMENT(x) x
#else
#define COMMENT(x)
#endif

// 
// Global variables
//
// for test and analysis only
extern unsigned long framenum;
extern unsigned long framenum_total;

#ifdef VERBOSE
int iframes, pframes; 
int partitioning_used;
int vol_header_bits, vop_header_bits;
int vp_header_bits, mb_header_bits;
int motion_bits, texture_bits;
int motion_blocks, texture_blocks;
int motion_bits_prev, texture_bits_prev;
int motion_blocks_prev, texture_blocks_prev;
short mvx_max, mvy_max;
unsigned long parser_cycles;
unsigned long idct_fast_blocks, idct_slow_blocks;
unsigned long idct_total_blocks;

#endif
// filenames and file pointers
extern char bitfile[MAX_STREAMS][300];
extern char outfile[300];
extern char outfileU[300];
extern char outfileV[300];
//extern int resync_errors;
#ifdef WRITE_FILE
//File *yuv_file, *u_file, *v_file;
#endif
#ifdef COMPARE_FILE
//File *ref_file;
char reffile[300];
int frames_in_error;
#endif
#ifdef WRITE_TB_FILES
//File *mc_in_exp_file;
//File *idct_in_exp_file;
//File *mc_out_exp_file;
//File *idct_out_exp_file;
//File *dec_out_exp_file;
//File *tu2dc_datain_file;
//File *tu2dc_cntlin_file;
//File *idct_obj_in_file;
//File *idct_in_regfile;
//File *idct_obj_out_exp_file;
#endif
#ifdef WRITE_INST_FILE
//File *dct_file1, *dct_file2;
//File *mot_file1, *mot_file2;
#endif

//extern File *info_file;
extern unsigned long loop_count, error_count;
extern unsigned long bytes_dma, bytes_non_dma;
//extern clock_t hw_time, hw_time2;

//
// Register files
//
// NOTE: these are accessible by the ParserVLD HW block
// as well as either the CopyControl, MotionComp or TextureIDCT HW block
//extern unsigned int dec_regfile;
//extern unsigned int mc_regfile;
//extern unsigned int idct_regfile;

/////////////////////////////////////////////////////////////////////
// Function Prototypes
/////////////////////////////////////////////////////////////////////
// Top level
//void DecoderChip(Vol *VOlist[], int dwidth, int dheight, unsigned short streams, int *ulx_display, int *uly_display);
//// Utilities
//void FatalError(char *string);
//char *my_strrepl(char *Str, unsigned short BufSiz, char *OldStr, char *NewStr);
//
//// Bitstream handling
//int MyCheckBitStuffing(Bitstream *stream,volatile unsigned char * a);
//uint16 MyBitstreamByteAlign(Bitstream *stream,volatile unsigned char *a);
//void MyBitstreamFlushBits(Bitstream *stream, uint4 nbits,volatile unsigned char *a);
//uint16 MyBitstreamShowBits(Bitstream *stream, uint4 nbits,volatile unsigned char *a);
//uint16 MyBitstreamShowBits_PE(Bitstream *stream, uint4 nbits,volatile unsigned char *a);
//uint16 MyBitstreamReadBits(Bitstream *stream, uint4 nbits,volatile unsigned char *a);
//uint16 MyBitstreamReadBits_PE(Bitstream *stream, uint4 nbits,volatile unsigned char *a);
//uint16 MyBitstreamReadBits_IL(Bitstream *stream, uint4 nbits,volatile unsigned char *a);
//int BitstreamInit(Bitstream *stream, int num);
////void BitstreamClose(Bitstream *stream);
//uint16 MyBitstreamShowBitsByteAlign(Bitstream *stream, uint4 nbits,volatile unsigned char *a); 

#define BitstreamFillBuffer(a)  MyBitstreamFillBuffer(a,stream_fifo)
#define BitstreamShowBits(a,b) MyBitstreamShowBits_PE(a,b,stream_fifo)
#define BitstreamReadBits(a,b) MyBitstreamReadBits_PE(a,b,stream_fifo)
#define BitstreamShowBits_PE(a,b) MyBitstreamShowBits_PE(a,b,stream_fifo)
#define BitstreamReadBits_PE(a,b) MyBitstreamReadBits_PE(a,b,stream_fifo)
#define BitstreamShowBits_IL(a,b) MyBitstreamShowBits(a,b,stream_fifo)
#define BitstreamReadBits_IL(a,b) MyBitstreamReadBits_IL(a,b,stream_fifo)
#define BitstreamFlushBits(a,b) MyBitstreamFlushBits(a,b,stream_fifo)
#define CheckBitStuffing(a) MyCheckBitStuffing(a,stream_fifo)
#define BitstreamByteAlign(a) MyBitstreamByteAlign(a,stream_fifo)
#define BitstreamShowBitsByteAlign(a,b) MyBitstreamShowBitsByteAlign(a,b,stream_fifo)
#define FatalError(a)

// Parser/VLD FSM
/*UChar ParserVLD(volatile UChar * stream_fifo,short streams, int *width, int *height, unsigned int *frameMemoryOffset, 
							  unsigned char *write_frame, unsigned char *perform_cc, unsigned char *perform_mc, unsigned char *perform_idct, 
							  unsigned char *stnum_out, short *mvx_out, short *mvy_out, unsigned char *blnum_out, 
							  unsigned char *comp_block, int13 *q_block, short *texture_block,
							  unsigned char *MCcoded_out, unsigned char *IDCTcoded_out,
								unsigned char *hOut1, unsigned char *vOut1,unsigned char *tu_mode, unsigned char *hOut2, unsigned char *vOut2,
								unsigned char *btype_out, unsigned char *ACpred_flag_out, unsigned short *DCpos_out, int *CBP_out, unsigned char *bp_prev);
*/
// Copy Controller
void copyControl(unsigned char *recFrame,  //shared memory in
unsigned char hIn, unsigned char vIn,  // scalar fifo in
unsigned int *cc_regfile, int frameMemoryOffset, // parameters in 
unsigned char *bufferY, unsigned char *bufferU, unsigned char *bufferV, /*unsigned char *searchArea,*/ // shared memory out
unsigned char *hOut, unsigned char *vOut // scalar fifo out
);

// Motion decoder
unsigned char MotionDecoder(Bitstream *stream,uint1 btype, uint1 ftype, uint1 coded, 
									 uint1 skipped_flag, uint6 x_pos, uint6 y_pos, uint12 mbnum, UChar DCpos, 
									 uint10 MB_width, uint3 f_code, int6 *mot_x, int6 *mot_y, int6 *mvx, int6 *mvy, 
									 uint3 MBtype,  UChar LB,volatile UChar* a,volatile uint32* mc_fifo);

// Motion compensation
void MotionCompensateBlock(unsigned char *bufferY, unsigned char *bufferU, unsigned char *bufferV, //shared memory in
/*unsigned char *currentMB,*/ // object fifo in
unsigned char kIn, unsigned char hIn, unsigned char vIn, short mv_xIn, short mv_yIn,/* unsigned char modeIn,*/ // scalar fifo in
unsigned int *mc_regfile, // parameters
unsigned char *compBlock/*, short *errorBlock,*/ //object fifo out
/*unsigned char *kOut, unsigned char *modeOut, short *mv_xOut, short *mv_yOut, int *SADblock,unsigned char *hOut1, unsigned char *vOut1, unsigned char *hOut2, unsigned char *vOut2*/ // scalar fifo out
);

// Texture decoder
unsigned char TextureVLD(Bitstream *stream, uint20 idct_regfile,uint20 idct_parameter1,uint20 idct_parameter2,uint1 ftype, uint1 btype,
                                                          uint3 comp, uint6 CBP, /*int13 *q_block,*/volatile UChar* stream_fifo,volatile uint20* idct_fifo);

// Texture/IDCT
#ifdef likeHW_TC
void hwInitDCTcoefficients(int cwidth);
#endif
unsigned char TextureIDCT(SInt *q_block, const unsigned char blnum, const int x_pos, const int y_pos, 
													const unsigned char btype, const unsigned char ACpred_flag, const unsigned short DCpos,  
													const int CBP, const unsigned char BufPatPrev[3], SInt *texture_block);

void textureUpdate(unsigned char *compBlock, short *textBlock, // object fifo in
unsigned char h, unsigned char v, unsigned char k, unsigned char tu_mode, // scalar fifo in
unsigned int *tu_regfile, int frameMemoryOffset, // parameters
unsigned char *recBlock, // object fifo out
unsigned char *hOut, unsigned char *vOut, unsigned char *kOut, // scalar fifo out
unsigned char *recFrame // shared memory out
);

#ifdef WIN32

#ifdef USE_DISPLAY_GDI
// Display Control 
void displayControl(unsigned char *recBlock, //objectFifo
unsigned char hIn, unsigned char vIn, unsigned blockNumber, //scalar fifo in
unsigned int *dc_regfile, unsigned int frameMemoryOffset, // parameters in				
CPicture *pic
);
#endif

#ifdef USE_DISPLAY_DIRECTX
// Display Control 
void displayControl(unsigned char *recBlock, //objectFifo
unsigned char hIn, unsigned char vIn, unsigned blockNumber, //scalar fifo in
unsigned int *dc_regfile, unsigned int frameMemoryOffset, // parameters in				
int stream_index
);
#endif

void initDisplays(Vol *VOlist[],unsigned char totalNumberOfVOs,CPicture *picSet[]);
void stopDisplays(unsigned char totalNumberOfVOs,CPicture *picSet[]);
#endif //WIN32

// Write to output file
//void WriteVopRaw(FILE *y_file, FILE *u_file, FILE *v_file, unsigned char *recFrame, int width, int height, unsigned int frameMemoryOffset) ;


void debug_printf(char * str);

#endif 
