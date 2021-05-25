// (C) Copyright 2016-2021 Xilinx, Inc.
// All Rights Reserved.
//
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.


#ifndef TRUNK_LEGACY_TOOLS_IR_TEST_IDCT_ALLINONE_H_
#define TRUNK_LEGACY_TOOLS_IR_TEST_IDCT_ALLINONE_H_

#include <math.h>
#include <stdarg.h>
#include <sys/timeb.h>
#ifndef WIN32
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#else
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include "picture.h"
#endif

// **************
// Directives
// **************
#ifdef WIN32
// #define VERBOSE
#define STOP_ON_ERROR
// #define USE_DISPLAY_GDI
#ifndef USE_DISPLAY_DIRECTX
#define USE_DISPLAY_DIRECTX
#endif
// #define WRITE_FILE
// #define READ_EXTMEM
// #define COMPARE_FILE
// #define WRITE_TB_FILES            // Uncomment to create HW testbench files
// #define LIMIT_TO_30FPS
#define VIEW_STATISTICS
#endif

#define USE_ORIG_IDCT
// #define USE_FAST_IDCT
#define likeHW_TC
// #define printCoefficients

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
#define MAX_STREAMS (8)
#define MAX_WIDTH (720)
#define MAX_HEIGHT (576)
#define BITS_DIM (10)
#define BITS_PIXEL (8)
#define BITS_QUANT (5)
#define BITS_DCT (12)
#define BITS_DCT_IQ (12)
#define MB_WIDTH (16)
#define MB_HEIGHT (16)
#define B_WIDTH (8)
#define B_HEIGHT (8)
#define B_SIZE (64)  // B_WIDTH * B_HEIGHT
#define B_COUNT (6)
#define MAXVAL_PIXEL (255)  // 2^BITS_PIXEL - 1
#define MIDVAL_PIXEL (128)  // 2^(BITS_PIXEL - 1)
#define YDEFAULT (128)
#define UDEFAULT (128)
#define VDEFAULT (128)
#define BFR_SIZE (2048)
#define INBFR_BYTES (7)  // mininum value: 5
// marker codes and lengths
#define VOL_START_CODE (0x012)
#define VOP_START_CODE (0x1B6)
#define RESYNC_MARKER (1)
#define VOL_START_CODE_LENGTH (28)
#define VOP_START_CODE_LENGTH (32)
#define MCBPC_LENGTH (9)
#define USER_DATA_START_CODE_LENGTH (32)
#define SHORT_VIDEO_START_CODE_LENGTH (22)
#define START_CODE_PREFIX_LENGTH (24)
#define VOL_HEADER_LENGTH (18)  // # bytes in VOL header
// frame and block types
#define I_VOP (0)
#define P_VOP (1)
#define B_VOP (2)
#define INTRA (0)
#define INTER (1)
#define BITS_COEF (13)
#define BITS_SHIFT (3)

// Maximum size BufferY/U/V including pipeline delay
#define LBwidth 64    // ((2*MAX_WIDTH/MB_WIDTH) + 5)
#define LB_MAX (128)  // power of 2 > LBwidth

#define PARSER_CLOCK (48)  // clock frequency for parser/VLD (in MHz)

#define CYCLES_IDCT_SLOW (225)  // cycles of texture/IDCT when intermittent
#define CYCLES_IDCT_FAST                                                       \
  (81)  // cycles of texture/IDCT using pipeline (3 stages)
#define CYCLES_IDCT_FAST_OLD                                                   \
  (161)  // cycles of texture/IDCT using pipeline (2 stages)

// Texture/IDCT input markers used in object FIFO (for generation of testbench
// files)
#define DC_MARKER (0xC0000)
#define AC_MARKER (0xE0000)
#define EOB_MARKER (0x60000)

//
// Typedefs
//
typedef uint8_t UChar;
typedef int16_t SInt;
typedef int Int;
typedef uint8_t Byte;
typedef uint8_t BYTE;
typedef int16_t Short;
typedef char Char;
typedef uint64_t UInt;
typedef float Float;
typedef FILE File;
typedef void Void;
typedef double Double;
typedef const int C_Int;
typedef const UInt C_UInt;

//
// Structures
//
struct Tcoef {
  SInt last;
  SInt run;
  SInt level;
  SInt sign;
};

struct VLCtab {
  Int val;
  Int len;
};

typedef struct {
  File *fptr;
  UChar *rdptr;
  UChar rdbfr[BFR_SIZE + 4];
  UChar inbfr[3 * INBFR_BYTES];
  uint16_t incnt;
  UChar *rdtop;
  uint64_t bitcount;
  uint64_t bstart, bfinish;
  uint16_t streamnum;
} Bitstream;

struct vol {
  int displayWidth;
  int displayHeight;
  float framerate;
  FILE *y_file;
  FILE *u_file;
  FILE *v_file;
};
typedef struct vol Vol;

struct vop;
typedef struct vop Vop;

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
uint64_t framenum, framenum_total;
#ifdef VERBOSE
int iframes, pframes;
int partitioning_used;
int vol_header_bits, vop_header_bits;
int vp_header_bits, mb_header_bits;
int motion_bits, texture_bits;
int motion_blocks, texture_blocks;
int motion_bits_prev, texture_bits_prev;
int motion_blocks_prev, texture_blocks_prev;
int16_t mvx_max, mvy_max;
uint64_t parser_cycles;
uint64_t idct_fast_blocks, idct_slow_blocks;
uint64_t idct_total_blocks;

#endif
// filenames and file pointers
char bitfile[MAX_STREAMS][300];
char outfile[300];
char outfileU[300];
char outfileV[300];
int resync_errors;
#ifdef WRITE_FILE
File *yuv_file, *u_file, *v_file;
#endif
#ifdef COMPARE_FILE
File *ref_file;
char reffile[300];
int frames_in_error;
#endif
#ifdef WRITE_TB_FILES
File *mc_in_exp_file;
File *idct_in_exp_file;
File *mc_out_exp_file;
File *idct_out_exp_file;
File *dec_out_exp_file;
File *tu2dc_datain_file;
File *tu2dc_cntlin_file;
File *idct_obj_in_file;
File *idct_in_regfile;
File *idct_obj_out_exp_file;
#endif
#ifdef WRITE_INST_FILE
File *dct_file1, *dct_file2;
File *mot_file1, *mot_file2;
#endif
File *info_file;
uint64_t loop_count, error_count;
uint64_t bytes_dma, bytes_non_dma;

// clock_t hw_time, hw_time2;

//
// Register files
//
// NOTE: these are accessible by the ParserVLD HW block
// as well as either the CopyControl, MotionComp or TextureIDCT HW block
unsigned int dec_regfile[MAX_STREAMS];
unsigned int mc_regfile[1];
unsigned int idct_regfile;

/////////////////////////////////////////////////////////////////////
// Function Prototypes
/////////////////////////////////////////////////////////////////////
// Top level
void DecoderChip(Vol *VOlist[], int dwidth, int dheight, uint16_t streams,
                 int *ulx_display, int *uly_display);

// Utilities
void FatalError(char *string);
char *my_strrepl(char *Str, uint16_t BufSiz, char *OldStr, char *NewStr);

// Bitstream handling
int CheckBitStuffing(Bitstream *stream);
UInt BitstreamByteAlign(Bitstream *stream);
void BitstreamFlushBits(Bitstream *stream, int nbits);
UInt BitstreamShowBits(Bitstream *stream, int nbits);
uint64_t BitstreamReadBits(Bitstream *stream, int nbits);
int BitstreamInit(Bitstream *stream, int num);
void BitstreamClose(Bitstream *stream);
UInt BitstreamShowBitsByteAlign(Bitstream *stream, int nbits);

// Parser/VLD FSM
UChar ParserVLD(int16_t streams, int *width, int *height,
                unsigned int *frameMemoryOffset, unsigned char *write_frame,
                unsigned char *perform_cc, unsigned char *perform_mc,
                unsigned char *perform_idct, unsigned char *stnum_out,
                int16_t *mvx_out, int16_t *mvy_out, unsigned char *blnum_out,
                unsigned char *comp_block, int16_t *q_block,
                int16_t *texture_block, unsigned char *MCcoded_out,
                unsigned char *IDCTcoded_out, unsigned char *hOut1,
                unsigned char *vOut1, unsigned char *tu_mode,
                unsigned char *hOut2, unsigned char *vOut2,
                unsigned char *btype_out, unsigned char *ACpred_flag_out,
                uint16_t *DCpos_out, int *CBP_out, unsigned char *bp_prev);

// Motion decoder
unsigned char MotionDecoder(Bitstream *stream, unsigned char ftype,
                            unsigned char coded, unsigned char skipped_flag,
                            int x_pos, int y_pos, int mbnum, uint16_t DCpos,
                            int MB_width, int f_code, SInt *mot_x, SInt *mot_y,
                            SInt *mvx, SInt *mvy, unsigned char MBtype,
                            SInt *slice_nb, uint16_t LB);

// Texture decoder
unsigned char TextureVLD(Bitstream *stream, unsigned char ftype,
                         unsigned char btype, unsigned char comp, int CBP,
                         SInt *q_block);
// Texture/IDCT
#ifdef likeHW_TC
void hwInitDCTcoefficients(int cwidth);
#endif
// unsigned char TextureIDCT(SInt *q_block, const unsigned char blnum,
// const int x_pos, const int y_pos, const unsigned char btype, const
// unsigned char ACpred_flag, const unsigned short DCpos, const int
// CBP, const unsigned char BufPatPrev[3], SInt *texture_block);

#if 0
// void textureUpdate(unsigned char *compBlock,
// short *textBlock,  // object fifo in
// unsigned char h, unsigned char v, unsigned char k,
// unsigned char tu_mode,  // scalar fifo in
// unsigned int *tu_regfile, int frameMemoryOffset,  // parameters
// unsigned char *recBlock,  // object fifo out
// unsigned char *hOut, unsigned char *vOut,
// unsigned char *kOut,  // scalar fifo out
// unsigned char *recFrame  // shared memory out
// );
#endif

#ifdef WIN32

#ifdef USE_DISPLAY_GDI
// Display Control
void displayControl(unsigned char *recBlock,  // objectFifo
                    unsigned char hIn, unsigned char vIn,
                    unsigned blockNumber,  // scalar fifo in
                    unsigned int *dc_regfile,
                    unsigned int frameMemoryOffset,  // parameters in
                    CPicture *pic);
#endif

#ifdef USE_DISPLAY_DIRECTX
// Display Control
void displayControl(unsigned char *recBlock,  // objectFifo
                    unsigned char hIn, unsigned char vIn,
                    unsigned blockNumber,  // scalar fifo in
                    unsigned int *dc_regfile,
                    unsigned int frameMemoryOffset,  // parameters in
                    int stream_index);
#endif

void initDisplays(Vol *VOlist[], unsigned char totalNumberOfVOs,
                  CPicture *picSet[]);
void stopDisplays(unsigned char totalNumberOfVOs, CPicture *picSet[]);
#endif  // WIN32

// Write to output file
void WriteVopRaw(FILE *y_file, FILE *u_file, FILE *v_file,
                 unsigned char *recFrame, int width, int height,
                 unsigned int frameMemoryOffset);

#endif  // TRUNK_LEGACY_TOOLS_IR_TEST_IDCT_ALLINONE_H_
