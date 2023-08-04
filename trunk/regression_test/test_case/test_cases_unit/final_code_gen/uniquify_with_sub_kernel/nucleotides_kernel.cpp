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
#include <stdint.h>
#include <stdio.h>
#include "nucleotides.h"
#include <string.h>
#define MIN_U8 (0)
#define MIN_I64 (0x8000000000000000)
#define E_CONST (0)
#define F_CONST (1)
#define H_CONST (2)
#define TMP_CONST (3)
#define ROWSTRIDE (4)
#define TMPVEC(R,C,RSTRIDE,CSTRIDE) 0 + ((R)*(RSTRIDE)+(C)*(CSTRIDE)+(TMP_CONST))
#define EVEC(R,C,RSTRIDE,CSTRIDE) 0 + ((R)*(RSTRIDE)+(C)*(CSTRIDE)+(E_CONST))
#define HVEC(R,C,RSTRIDE,CSTRIDE) 0 + ((R)*(RSTRIDE)+(C)*(CSTRIDE)+(H_CONST))
#define FVEC(R,C,RSTRIDE,CSTRIDE) 0 + ((R)*(RSTRIDE)+(C)*(CSTRIDE)+(F_CONST))
extern void *alignedMalloc(size_t size);
static const unsigned long NBYTES_PER_REG = 16;
static const unsigned long NWORDS_PER_REG = 16;
//static const size_t NBITS_PER_WORD  = 8;
static const unsigned long NBYTES_PER_WORD = 1;
extern char ran_char();
#define FOLD 32
// In end-to-end mode, we start high (255) and go low (0).  Factoring in
// a query profile involves unsigned saturating subtraction, so all the
// query profile elements should be expressed as a positive penalty rather
// than a negative score.
typedef uint8_t TCScore;
struct m128i profbuf_vec_buf[128 * 10000];
static void __merlin_dummy_kernel_pragma();

void align_kernel_fpga(char rf_vec[2048 * 10000],int rf_len_vec[10000],int profbuf_size_vec[10000],struct m128i profbuf_vec[128 * 10000],long results[10000],int N)
{
  N >= 1 && N <= 10000?(static_cast < void  >  (0)) : __assert_fail("N >= 1 && N <= (10000)","nucleotides_kernel.cpp",44,__PRETTY_FUNCTION__);
  
#pragma ACCEL interface variable=profbuf_vec stream_prefetch=on depth=1280000
  
#pragma ACCEL interface variable=rf_vec stream_prefetch=on depth=20480000
  
#pragma ACCEL interface variable=rf_len_vec stream_prefetch=on depth=10000
  
#pragma ACCEL interface variable=profbuf_size_vec stream_prefetch=on depth=10000
  
#pragma ACCEL interface variable=results stream_prefetch=on depth=10000
//   memcpy(profbuf_vec_buf, profbuf_vec, sizeof(m128i) * MAX_PROFBUF_LEN*MAX_NUM_SAMPLES);
  
#pragma ACCEL parallel factor=4
//for (int i = 0; i < N; i++) {  // 10000
// 10000
  for (int i = 0; i < 10000; i++) {
// 128
    struct m128i profbuf[128];
    for (int j = 0; j < 128; j++) {
      for (int k = 0; k < 16; ++k) {
        profbuf[j] . data[k] = profbuf_vec[i * 128 + j] . data[k];
      }
    }
// rf_vec: 2048
//results[i] = align_kernel(&rf_vec[i*MAX_RF_LEN], rf_len_vec[i],
//                          profbuf_size_vec[i], &profbuf_vec[i*MAX_PROFBUF_LEN], i);
    results[i] = align_kernel(rf_vec,rf_len_vec[i],profbuf_size_vec[i],profbuf,i);
  }
}
/**
 * Solve the current alignment problem using SSE instructions that operate on 16
 * unsigned 8-bit values packed into a single 128-bit register.
 */

long align_kernel(char *rf_,int rf_len,int profbuf_size,struct m128i profbuf[128],int idx)
{
// Skip this step; the profile buffer is loaded as a kernel argument
//buildQueryProfileEnd2EndSseU8(fw_);
// Constants
  const int dpRows = 36;
  const int refGapOpen = 8;
  const int refGapExtend = 3;
  const int readGapOpen = 8;
  const int readGapExtend = 3;
  const int rfi_ = 0;
  const int rff_ = 1035;
  const int lastWord_ = 11;
  const int lastIter_ = 2;
  const int minsc_ = 0;
  int firsts5[32] = {(- 1), (0), (1), (0), (2), (0), (1), (0), (3), (0), (1), (0), (2), (0), (1), (0), (4), (0), (1), (0), (2), (0), (1), (0), (3), (0), (1), (0), (2), (0), (1), (0)};
  const unsigned long iter = (dpRows + (NWORDS_PER_REG - 1)) / NWORDS_PER_REG;
// iter = segLen
  int dup;
// Many thanks to Michael Farrar for releasing his striped Smith-Waterman
// implementation:
//
//  http://sites.google.com/site/farrarmichael/smith-waterman
//
// Much of the implmentation below is adapted from Michael's code.
// Set all elts to reference gap open penalty
  struct m128i rfgapo = mm_setzero_si128();
  struct m128i rfgape = mm_setzero_si128();
  struct m128i rdgapo = mm_setzero_si128();
  struct m128i rdgape = mm_setzero_si128();
  struct m128i vlo = mm_setzero_si128();
  struct m128i vhi = mm_setzero_si128();
//	m128i ve       = mm_setzero_si128();
//	m128i vf       = mm_setzero_si128();
//	m128i vh       = mm_setzero_si128();
  struct m128i vtmp = mm_setzero_si128();
  struct m128i vzero = mm_setzero_si128();
  struct m128i vhilsw = mm_setzero_si128();
  dup = refGapOpen << 8 | refGapOpen & 0x00ff;
  rfgapo = mm_insert_epi16(rfgapo,dup,0);
  rfgapo = mm_shufflelo_epi16(rfgapo,0);
  rfgapo = mm_shuffle_epi32(rfgapo,0);
// Set all elts to reference gap extension penalty
  dup = refGapExtend << 8 | refGapExtend & 0x00ff;
  rfgape = mm_insert_epi16(rfgape,dup,0);
  rfgape = mm_shufflelo_epi16(rfgape,0);
  rfgape = mm_shuffle_epi32(rfgape,0);
// Set all elts to read gap open penalty
  dup = readGapOpen << 8 | readGapOpen & 0x00ff;
  rdgapo = mm_insert_epi16(rdgapo,dup,0);
  rdgapo = mm_shufflelo_epi16(rdgapo,0);
  rdgapo = mm_shuffle_epi32(rdgapo,0);
// Set all elts to read gap extension penalty
  dup = readGapExtend << 8 | readGapExtend & 0x00ff;
  rdgape = mm_insert_epi16(rdgape,dup,0);
  rdgape = mm_shufflelo_epi16(rdgape,0);
  rdgape = mm_shuffle_epi32(rdgape,0);
// all elts = 0xffff
  vhi = mm_cmpeq_epi16(vhi,vhi);
// all elts = 0
  vlo = mm_xor_si128(vlo,vlo);
// vhilsw: topmost (least sig) word set to 0x7fff, all other words=0
  vhilsw = mm_shuffle_epi32(vhi,0);
  vhilsw = mm_srli_si128(vhilsw,(NBYTES_PER_REG - NBYTES_PER_WORD));
// Points to a long vector of m128i where each element is a block of
// contiguous cells in the E, F or H matrix.  If the index % 3 == 0, then
// the block of cells is from the E matrix.  If index % 3 == 1, they're
// from the F matrix.  If index % 3 == 2, then they're from the H matrix.
// Blocks of cells are organized in the same interleaved manner as they are
// calculated by the Farrar algorithm.
// points into the query profile
  int pvProfbuf;
//m128i vScore[12432];
  
#pragma ACCEL register variable=vScore_data
  struct m128i vScore[32];
//assert((long) &vScore[0] & 15 == 0);
//m128i * vScore = NULL;
//vScore = (m128i *) alignedMalloc(12432 * sizeof(m128i));
//d.mat_.init(dpRows, rff_ - rfi_, NWORDS_PER_REG);
//const size_t colstride = d.mat_.colstride();
  const unsigned long colstride = 12;
//const size_t rowstride = d.mat_.rowstride();
// Initialize the H and E vectors in the first matrix column
//m128i *pvHTmp = d.mat_.tmpvec(0, 0);
//m128i *pvETmp = d.mat_.evec(0, 0);
  int pvHTmp = (0 + ((0 * 4) + 0 * colstride + 3));
  int pvETmp = (0 + ((0 * 4) + 0 * colstride + 0));
// Maximum score in final row
  unsigned char found = false;
//TCScore lrmax = MIN_U8;
  unsigned char lrmax = 0;
// 3 constant
  for (unsigned long i = 0; i < iter; i++) {
    mm_store_si128(&vScore[pvETmp],vlo);
// start high in end-to-end mode
    mm_store_si128(&vScore[pvHTmp],vlo);
    pvETmp += 4;
    pvHTmp += 4;
  }
// These are swapped just before the innermost loop
//m128i *pvHStore = d.mat_.hvec(0, 0);
//m128i *pvHLoad  = d.mat_.tmpvec(0, 0);
//m128i *pvELoad  = d.mat_.evec(0, 0);
//m128i *pvEStore = d.mat_.evecUnsafe(0, 1);
//m128i *pvFStore = d.mat_.fvec(0, 0);
//m128i *pvFTmp   = NULL;
//int pvHStore = HVEC(0,0,ROWSTRIDE,colstride);
  const int pvHStore_init = (0 + ((0 * 4) + 0 * colstride + 2));
  int pvHLoad = (0 + ((0 * 4) + 0 * colstride + 3));
  const int pvELoad = (0 + ((0 * 4) + 0 * colstride + 0));
  int pvEStore = (0 + ((0 * 4) + 1 * colstride + 0));
//int pvFStore = FVEC(0,0,ROWSTRIDE,colstride);
  const int pvFStore_init = (0 + ((0 * 4) + 0 * colstride + 1));
  int pvFTmp = 0;
  unsigned long nfixup = 0;
// Fill in the table as usual but instead of using the same gap-penalty
// vector for each iteration of the inner loop, load words out of a
// pre-calculated gap vector parallel to the query profile.  The pre-
// calculated gap vectors enforce the gap barrier constraint by making it
// infinitely costly to introduce a gap in barrier rows.
//
// AND use a separate loop to fill in the first row of the table, enforcing
// the st_ constraints in the process.  This is awkward because it
// separates the processing of the first row from the others and might make
// it difficult to use the first-row results in the next row, but it might
// be the simplest and least disruptive way to deal with the st_ constraint.
  int colstop_ = rff_ - 1;
  int lastsolcol_ = 0;
//  printf("%d\n", pvHLoad);
  main_inner_loop:
{
    
#pragma ACCEL pipeline flatten
// 1035
    for (unsigned long ii = (unsigned long )rfi_; ii < ((unsigned long )rff_); ii++) {
//pvHStore_init + colstride * ii;
      int pvHStore = (2 + 12 * ii);
//pvFStore_init + colstride * ii;
      int pvFStore = (1 + 12 * ii);
//12 + 3 * ROWSTRIDE;
      const int pvEStore = 24;
// new pvHLoad = pvHStore
      if (ii > 0) 
        pvHLoad = pvHStore - 12;
       else 
        pvHLoad = 3;
//   printf("%d %d\n", pvHStore, pvFStore);
      pvHStore == pvFStore + 1?(static_cast < void  >  (0)) : __assert_fail("pvHStore == pvFStore + 1","nucleotides_kernel.cpp",248,__PRETTY_FUNCTION__);
      int curr_idx = (2 + 12 * ii);
      pvHStore == curr_idx?(static_cast < void  >  (0)) : __assert_fail("pvHStore == curr_idx","nucleotides_kernel.cpp",251,__PRETTY_FUNCTION__);
// Fetch the appropriate query profile.  Note that elements of rf_ must
// be numbers, not masks.
      const int refc = (int )rf_[(idx * 2048) + ii];
      refc != 0?(static_cast < void  >  (0)) : __assert_fail("refc != 0","nucleotides_kernel.cpp",257,__PRETTY_FUNCTION__);
      unsigned long off = ((unsigned long )firsts5[refc]) * iter * 2;
//assert(off >= 0 && off < profbuf_size);
//if (off < 0) off = 0;
      off >= 0?(static_cast < void  >  (0)) : __assert_fail("off >= 0","nucleotides_kernel.cpp",261,__PRETTY_FUNCTION__);
//if (off >= profbuf_size) off = profbuf_size - 2;
      off < profbuf_size?(static_cast < void  >  (0)) : __assert_fail("off < profbuf_size","nucleotides_kernel.cpp",263,__PRETTY_FUNCTION__);
//pvProfbuf = profbuf + off; // even elts = query profile, odd = gap barrier
// even elts = query profile, odd = gap barrier
      pvProfbuf = off;
// Set all cells to low value
//mm_xor_si128(vf, vf);
      struct m128i vf = mm_setzero_si128();
// Load H vector from the final row of the previous column
//m128i vh = mm_load_si128(&vScore[pvHLoad + colstride - ROWSTRIDE]);
      struct m128i vh = mm_load_si128(&vScore[(pvHLoad + colstride - 4) % 32]);
// Shift 2 bytes down so that topmost (least sig) cell gets 0
      vh = mm_slli_si128(vh,NBYTES_PER_WORD);
// Fill topmost (least sig) cell with high value
      vh = mm_or_si128(vh,vhilsw);
// For each character in the reference text:
      unsigned long j;
      struct m128i ve_t[3];
      for (j = 0; j < 3; j++) 
        ve_t[j] = vScore[4 * j];
      struct m128i vhl_t[3];
      for (j = 0; j < 3; j++) 
        vhl_t[j] = mm_load_si128(&vScore[(pvHLoad + j * 4) % 32]);
      struct m128i vh_n[3];
      struct m128i vf_n[3];
      struct m128i ve_n[3];
      struct m128i pr_t[6];
      for (j = 0; j < 3; j++) {
        pr_t[j * 2 + 0] = mm_load_si128(&profbuf[j * 2 + off + 0]);
        pr_t[j * 2 + 1] = mm_load_si128(&profbuf[j * 2 + off + 1]);
      }
// printf("pvHLoad %d, pvHStore=%d \n", pvHLoad, pvHStore);
// 3 // scan rows  // constant
      for (j = 0; j < iter; j++) {
// Load cells from E, calculated previously
//m128i ve = mm_load_si128(&vScore[pvELoad + ROWSTRIDE * j]);
        struct m128i ve = mm_load_si128(&ve_t[j]);
//      printf("%d\n", pvELoad);
//      assert(pvELoad == 0 );
//pvELoad += ROWSTRIDE;
// Store cells in F, calculated previously
//vf = mm_subs_epu8(vf, profbuf[pvProfbuf+1]); // veto some ref gap extensions
// veto some ref gap extensions
        vf = mm_subs_epu8(vf,pr_t[1 + 2 * j]);
//mm_store_si128(&vScore[pvFStore + j * ROWSTRIDE ], vf);
        mm_store_si128(&vf_n[j],vf);
// Factor in query profile (matches and mismatches)
//vh = mm_subs_epu8(vh, profbuf[pvProfbuf+0 + 2*j]);
        vh = mm_subs_epu8(vh,pr_t[0 + 2 * j]);
// Update H, factoring in E and F
        vh = mm_max_epu8(vh,ve);
        vh = mm_max_epu8(vh,vf);
// Save the new vH values
//mm_store_si128(&vScore[pvHStore + j * ROWSTRIDE], vh);
        mm_store_si128(&vh_n[j],vh);
//pvHStore += ROWSTRIDE;
// Update vE value
        vtmp = vh;
        vh = mm_subs_epu8(vh,rdgapo);
//vh = mm_subs_epu8(vh, profbuf[pvProfbuf+1 + 2*j]); // veto some read gap opens
// veto some read gap opens
        vh = mm_subs_epu8(vh,pr_t[1 + 2 * j]);
        ve = mm_subs_epu8(ve,rdgape);
        ve = mm_max_epu8(ve,vh);
// Load the next h value
        vh = mm_load_si128(&vhl_t[j]);
//pvHLoad += ROWSTRIDE;
// Save E values
//mm_store_si128(&vScore[pvEStore], ve);
        mm_store_si128(&ve_n[j],ve);
// Update vf value
        vtmp = mm_subs_epu8(vtmp,rfgapo);
        vf = mm_subs_epu8(vf,rfgape);
        vf = mm_max_epu8(vf,vtmp);
//pvProfbuf += 2; // move on to next query profile / gap veto
      }
//	pvEStore += ROWSTRIDE * 3;
//printf("%d %d\n", pvHStore, pvHLoad);
//assert(pvHStore == pvHLoad + 12);
//pvHStore += 3*ROWSTRIDE;
//pvHStore -= colstride; // reset to start of column
// pvHStore, pvELoad, pvEStore have all rolled over to the next column
      pvFTmp = pvFStore + 4 * 3;
//		pvFStore += ROWSTRIDE * 3;
//	pvFStore -= colstride; // reset to start of column
//vtmp = mm_load_si128(&vScore[pvFStore]);
//vh = mm_load_si128(&vScore[pvHStore]);
//vtmp = mm_load_si128(&vScore[1 + 12 * ii]);
//vh = mm_load_si128(&vScore[2 + 12*ii]);
      vtmp = mm_load_si128(&vf_n[0]);
      vh = mm_load_si128(&vh_n[0]);
//pvEStore -= colstride; // reset to start of column
//m128i ve = mm_load_si128(&vScore[pvEStore]);
      struct m128i ve = mm_load_si128(&ve_n[0]);
      int pvHStore1 = pvHStore;
//       if (off+1 >= profbuf_size) off = profbuf_size - 2;
//printf("%d, %d\n", off, profbuf_size);
      off + 1 < profbuf_size?(static_cast < void  >  (0)) : __assert_fail("off+1 < profbuf_size","nucleotides_kernel.cpp",395,__PRETTY_FUNCTION__);
// reset veto vector
      pvProfbuf = (off + 1);
// vf from last row gets shifted down by one to overlay the first row
// rfgape has already been subtracted from it.
      vf = mm_slli_si128(vf,NBYTES_PER_WORD);
//vf = mm_subs_epu8(vf, profbuf[pvProfbuf]); // veto some ref gap extensions
// veto some ref gap extensions
      vf = mm_subs_epu8(vf,pr_t[1]);
      vf = mm_max_epu8(vtmp,vf);
      vtmp = mm_subs_epu8(vf,vtmp);
      vtmp = mm_cmpeq_epi8(vtmp,vzero);
      int cmp = (mm_movemask_epi8(vtmp));
// If any element of vtmp is greater than H - gap-open...
      j = 0;
      int pvFStore1 = pvFStore;
      int pvEStore1 = pvEStore;
      int tt = 0;
//      static int max_tt = 0;
//while(cmp != 0xffff) 
      if (cmp != 0xffff) 
//for (int tt = 0; tt < 20; tt++) 
{
//  if (cmp == 0xffff) break;
        
#pragma ACCEL loop_tripcount max=20
// Store this vf
//mm_store_si128(&vScore[pvFStore1], vf); // 1 + 12 * ii
// 1 + 12 * ii
        mm_store_si128(&vf_n[j],vf);
//			pvFStore += ROWSTRIDE;
//pvFStore1 += ROWSTRIDE;
// Update vh w/r/t new vf
        vh = mm_max_epu8(vh,vf);
// Save vH values
//mm_store_si128(&vScore[pvHStore1], vh);
        mm_store_si128(&vh_n[j],vh);
// Update E in case it can be improved using our new vh
        vh = mm_subs_epu8(vh,rdgapo);
//vh = mm_subs_epu8(vh, profbuf[pvProfbuf]); // veto some read gap opens
// veto some read gap opens
        vh = mm_subs_epu8(vh,pr_t[1]);
        ve = mm_max_epu8(ve,vh);
//mm_store_si128(&vScore[pvEStore1], ve);
        mm_store_si128(&ve_n[j],ve);
//pvProfbuf += 2;
//if(++j == iter) {
//		//		pvFStore -= colstride;
//  //pvFStore1 += ROWSTRIDE;
//	//pvFStore1 -= 8;
//	vtmp = mm_load_si128(&vScore[pvFStore1-8]);   // load next vf ASAP
//	//pvHStore1 -= 8;
//	vh = mm_load_si128(&vScore[pvHStore1-8]);     // load next vh ASAP
//	//pvEStore1 -= 8;
//	ve = mm_load_si128(&vScore[pvEStore1-8]);     // load next ve ASAP
//          assert(off+1 <= profbuf_size);
//	//pvProfbuf = off + 1;
//	j = 0;
//	vf = mm_slli_si128(vf, NBYTES_PER_WORD);
//  pvFStore1 -= 12;;
//  pvHStore1 -= 12;
//  pvEStore1 -= 12;
//} else 
{
//vtmp = mm_load_si128(&vScore[pvFStore1]);     // load next vf ASAP
//vh   = mm_load_si128(&vScore[pvHStore1]);     // load next vh ASAP
//ve   = mm_load_si128(&vScore[pvEStore1]);     // load next vh ASAP
// load next vf ASAP
          vtmp = mm_load_si128(&vf_n[j]);
// load next vh ASAP
          vh = mm_load_si128(&vh_n[j]);
// load next vh ASAP
          ve = mm_load_si128(&ve_n[j]);
        }
        ++j;
        j = j % 3;
//pvFStore1 += 4;;
//pvHStore1 += 4;
//pvEStore1 += 4;
// Update F with another gap extension
        vf = mm_subs_epu8(vf,rfgape);
//vf = mm_subs_epu8(vf, profbuf[pvProfbuf]); // veto some ref gap extensions
// veto some ref gap extensions
        vf = mm_subs_epu8(vf,pr_t[1]);
        vf = mm_max_epu8(vtmp,vf);
        vtmp = mm_subs_epu8(vf,vtmp);
        vtmp = mm_cmpeq_epi8(vtmp,vzero);
// exit if the [7] bit are all one
        cmp = (mm_movemask_epi8(vtmp));
        nfixup++;
        tt++;
//      if (tt > max_tt) max_tt = tt;
      }
      for (int j = 0; j < 3; j++) {
        mm_store_si128(&vScore[(1 + 12 * ii + (4 * j)) % 32],vf_n[j]);
        mm_store_si128(&vScore[(2 + 12 * ii + (4 * j)) % 32],vh_n[j]);
        mm_store_si128(&vScore[(24 + 4 * j) % 32],ve_n[j]);
      }
//   printf("idx=%d ii=%d, max_tt=%d\n", idx, ii, max_tt);
      int vtmp = (0 + ((lastIter_ * 4) + (ii - rfi_) * colstride + 2));
// Note: we may not want to extract from the final row
//TCScore lr = ((TCScore*)(vtmp))[lastWord_];
//TCScore lr = vtmp->data[lastWord_];
//TODO hack!!!
      unsigned char lr = vScore[vtmp % 32] . data[lastWord_];
      found = true;
      if (lr > lrmax) {
        lrmax = lr;
      }
// pvELoad and pvHLoad are already where they need to be
// Adjust the load and store vectors here.  
//pvHStore = pvHStore + colstride;
//pvEStore = pvELoad + colstride + 3 * ROWSTRIDE;
//pvFStore = pvFStore + ROWSTRIDE * 3;
    }
  }
//// Update metrics
//if(!debug) {
//size_t ninner = (rff_ - rfi_) * iter;
//met.col   += (rff_ - rfi_);             // DP columns
//met.cell  += (ninner * NWORDS_PER_REG); // DP cells
//met.inner += ninner;                    // DP inner loop iters
//met.fixup += nfixup;                    // DP fixup loop iters
//}
//flag = 0;
// Did we find a solution?
  long score = 0x8000000000000000;
  if (!found) {
//flag = -1; // no
//if(!debug) met.dpfail++;
    score = 0x8000000000000000;
  }
   else {
    score = ((long )(lrmax - 0xff));
    if (score < minsc_) {
//flag = -1; // no
//if(!debug) met.dpfail++;
//return score;
    }
  }
// Could we have saturated?
  if (lrmax == 0) {
//flag = -2; // yes
//if(!debug) met.dpsat++;
    score = 0x8000000000000000;
  }
// Return largest score
//if(!debug) met.dpsucc++;
//free(vScore);
  return score;
}
