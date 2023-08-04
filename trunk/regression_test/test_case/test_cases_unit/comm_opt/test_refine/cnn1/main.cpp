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
#include <fstream>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "config.h"
#include "cmost.h"
#define GEN 0
using namespace std;
//void cnn_kernel(float *Cout, float *Cin, float *weight, float *bias); 
void cnn_kernel(float *Cout,float *Cin,float *weight,float *bias,float *Cout1,float *Cin1,float *weight1,float *bias1);

float rcmp(float a,float b)
{
  return (fabs(((a - b) / (a + b))));
}
/*InHEI*/
/*InWID*/
template < int OUTF, int INF, int HEI, int WID, int InHEI, int InWID, int KER,
          int STD >
void LoadData ( float Cin [ INF ] [ 240 ] [ 240 ], float weight [ OUTF ] [ INF ] [ KER ] [ KER ],
              float bias [ OUTF ] ) {
  cout << "start load input& weight" << endl;
  ifstream fw, fb, fi;
  fw . open ( "weight.dat", ios :: in | ios :: binary );
  fb . open ( "bias.dat", ios :: in | ios :: binary );
  double * t_bias = new double [ OUTF ];
  double * t_wght = new double [ OUTF * INF * KER * KER ];
  fw . read ( ( char * ) t_wght, sizeof ( double ) * OUTF * INF * KER * KER );
  fb . read ( ( char * ) t_bias, sizeof ( double ) * OUTF );
  for ( int i = 0; i < OUTF; i ++ ) {
    bias [ i ] = t_bias [ i ];
    for ( int j = 0; j < INF; j ++ ) {
      for ( int k = 0; k < KER; k ++ ) {
        for ( int s = 0; s < KER; s ++ ) {
          weight [ i ] [ j ] [ k ] [ s ] =
              ( float ) t_wght [ i * INF * KER * KER + j * KER * KER + k * KER + s ];
        }
      }
    }
  }
  cout << "finish load weight" << endl;
  delete [ ] t_bias;
  delete [ ] t_wght;
  fw . close ( );
  fb . close ( );
  double * t_in = new double [ INF * InHEI * InWID ];
  fi . open ( "input.dat", ios :: in | ios :: binary );
  fi . read ( ( char * ) t_in, sizeof ( double ) * INF * InHEI * InWID );
  for ( int i = 0; i < INF; i ++ ) {
    for ( int j = 0; j < 240; j ++ ) {
      for ( int k = 0; k < 240; k ++ ) {
        Cin [ i ] [ j ] [ k ] = ( j < InHEI && k < InWID ) ? ( float ) t_in [ i * InHEI * InWID + j * InWID + k ] : 1;
      }
    }
  }
  cout << "finish load Cin" << endl;
  delete [ ] t_in;
  fi . close ( );
}
// if ( rcmp ( Cout [ i ] [ j ] [ k ], ( float ) t_out [ i * HEI * WID
// + j * WID + k ] ) > ( ( ( ( ( ( ( ( ( ( (
// (1.000000000000000082e-05) ) ) ) ) ) ) ) ) ) ) ) )
//if (rcmp(*(&(Cout[0][0][0]) + i0 * IMROW * IMCOL * TILE + j * IMCOL * TILE + k * TILE + ii),
//((((((((((((1.000000000000000082e-05))))))))))))) {
template < int OUTF, int HEI, int WID > int Verify ( float Cout [ OUTF ] [ HEI ] [ WID ] ) {
  int error = 0;
  ifstream fo;
  fo . open ( "output.dat", ios :: in | ios :: binary );
  double * t_out = new double [ OUTF * HEI * WID ];
  fo . read ( ( char * ) t_out, sizeof ( double ) * OUTF * HEI * WID );
  for ( int i0 = 0; i0 < OUTF / 16; i0 ++ ) {
    for ( int j = 0; j < HEI; j ++ ) {
      for ( int k = 0; k < WID; k ++ ) {
        for ( int ii = 0; ii < 16; ii ++ ) {
          int i = i0 * 16 + ii;
          if ( rcmp ( ( Cout [ i ] [ j ] [ k ] ), ( float ) t_out [ i * HEI * WID + j * WID + k ] ) >
              ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( ( (0.001000000000000000021) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) {
                if ( error < 10 ) printf (
                    "[i=%d j=%d k=%d]: %f,\t%f,\t%f\n", i, j, k, Cout [ i ] [ j ] [ k ],
                    t_out [ i * HEI * WID + j * WID + k ],
                    rcmp ( Cout [ i ] [ j ] [ k ], ( float ) t_out [ i * HEI * WID + j * WID + k ] ) );
          error ++;
          }
        }
      }
    }
  }
  delete [ ] t_out;
  fo . close ( );
  return error;
}
template < int OUTF, int INF, int HEI, int WID, int InHEI, int InWID, int KER,
          int STD >
void GenData ( int flag, float Cout [ OUTF ] [ HEI ] [ WID ], float Cin [ INF ] [ 240 ] [ 240 ],
             float weight [ OUTF ] [ INF ] [ KER ] [ KER ], float bias [ OUTF ] ) {
  if ( flag == 1 ) {
    cout << "start put gold input" << endl;
    ofstream fw, fb, fi;
    fw . open ( "weight.dat", ios :: out | ios :: app | ios :: binary );
    fb . open ( "bias.dat", ios :: out | ios :: app | ios :: binary );
    double * t_bias = new double [ OUTF ];
    double * t_wght = new double [ OUTF * INF * KER * KER ];
    for ( int i = 0; i < OUTF; i ++ ) {
      t_bias [ i ] =
          ( ( rand ( ) % 2 == 0 ) ? 2 : - 2 ) * ( double ) rand ( ) / ( double ) 2147483647;
      bias [ i ] = t_bias [ i ];
      for ( int j = 0; j < INF; j ++ ) {
        for ( int k = 0; k < KER; k ++ ) {
          for ( int s = 0; s < KER; s ++ ) {
            t_wght [ i * INF * KER * KER + j * KER * KER + k * KER + s ] =
                ( ( rand ( ) % 2 == 0 ) ? 0 : - 1 ) * ( double ) rand ( ) /
                ( double ) 2147483647;
            weight [ i ] [ j ] [ k ] [ s ] =
                t_wght [ i * INF * KER * KER + j * KER * KER + k * KER + s ];
          }
        }
      }
    }
    cout << "finish weight" << endl;
    fw . write ( ( char * ) t_wght, sizeof ( double ) * OUTF * INF * KER * KER );
    fb . write ( ( char * ) t_bias, sizeof ( double ) * OUTF );
    delete [ ] t_bias;
    delete [ ] t_wght;
    fw . close ( );
    fb . close ( );
    double * t_in = new double [ INF * InHEI * InWID ];
    fi . open ( "input.dat", ios :: out | ios :: app | ios :: binary );
    for ( int i = 0; i < INF; i ++ ) {
      for ( int j = 0; j < InHEI; j ++ ) {
        for ( int k = 0; k < InWID; k ++ ) {
          t_in [ i * InHEI * InWID + j * InWID + k ] =
              ( ( rand ( ) % 2 == 0 ) ? 0 : - 1 ) * ( double ) rand ( ) / ( double ) 2147483647;
          Cin [ i ] [ j ] [ k ] = t_in [ i * InHEI * InWID + j * InWID + k ];
        }
      }
    }
    cout << "finish Cin" << endl;
    fi . write ( ( char * ) t_in, sizeof ( double ) * INF * InHEI * InWID );
    delete [ ] t_in;
    fi . close ( );
  } else if ( flag == 2 ) {
    ofstream fo;
    fo . open ( "output.dat", ios :: out | ios :: binary );
    double * t_out = new double [ OUTF * HEI * WID ];
    for ( int i = 0; i < OUTF; i ++ ) {
      for ( int j = 0; j < HEI; j ++ ) {
        for ( int k = 0; k < WID; k ++ ) {
          t_out [ i * HEI * WID + j * WID + k ] = ( double ) Cout [ i ] [ j ] [ k ];
        }
      }
    }
    fo . write ( ( char * ) t_out, sizeof ( double ) * OUTF * HEI * WID );
    delete [ ] t_out;
    fo . close ( );
  } else {
    printf ( "What do you want me to do\?\n" );
  }
}

int main()
{
  static float Cout[32][224][224];
//static float Cout_hw[NUM * IMROW * IMROW];
  float *Cout_hw;
  cmost_malloc(((void **)(&Cout_hw)),((size_t )4) * 32 * 224 * 224);
//static float Cin[NUM][INIMROW][INIMCOL];
  static float Cin[32][240][240];
//static float Cin_hw[NUM * INIMROW * INIMROW];
  float *Cin_hw;
//cmost_malloc((void**)&Cin_hw, sizeof(float) * NUM * INIMROW * INIMCOL);
  cmost_malloc(((void **)(&Cin_hw)),((size_t )4) * 32 * 240 * 240);
  static float weight[32][32][3][3];
//static float weight_hw[NUM * NUM * KERNEL * KERNEL];
  float *weight_hw;
  cmost_malloc(((void **)(&weight_hw)),((size_t )4) * 32 * 32 * 3 * 3);
//static float bias[NUM];
  float *bias;
  cmost_malloc(((void **)(&bias)),((size_t )4) * 32);
//#if GEN
//  GenData<NUM, NUM, IMROW, IMCOL, INIMROW, INIMCOL, KERNEL, 1>(
//      GoldInput, Cout, Cin, weight, bias);
//#else
  ::LoadData< 32 , 32 , 224 , 224 , 226 , 226 , 3 , 1 > (Cin,weight,bias);
//#endif
//
//for (int j0 = 0; j0 < NUM / TILE; j0++) {
//  for (int _memcpy_i0_1 = 0; _memcpy_i0_1 < INIMROW; ++_memcpy_i0_1) {
//    for (int _memcpy_i0_0 = 0; _memcpy_i0_0 < INIMCOL; ++_memcpy_i0_0) {
//      for (int _memcpy_i0_2 = 0; _memcpy_i0_2 < TILE; ++_memcpy_i0_2) {
//        Cin_hw[j0 * TILE * INIMROW * INIMCOL + _memcpy_i0_1 * INIMCOL * TILE +
//               _memcpy_i0_0 * TILE + _memcpy_i0_2] =
//            Cin[j0 * TILE + _memcpy_i0_2][_memcpy_i0_1][_memcpy_i0_0];
//      }
//    }
//  }
//}
  for (int i = 0; i < 32 * 240 * 240; i++) {
    Cin_hw[i] =  *(Cin[0][0] + i);
  }
  for (int i0 = 0; i0 < 32 / 16; i0++) {
    for (int j0 = 0; j0 < 32 / 16; j0++) {
      for (int __memcpy_i_1_0 = 0; __memcpy_i_1_0 < 3; ++__memcpy_i_1_0) {
        for (int __memcpy_i_1_1 = 0; __memcpy_i_1_1 < 3; ++__memcpy_i_1_1) {
          for (int jj = 0; jj < 16; jj++) {
            for (int ii = 0; ii < 16; ii++) {
              weight_hw[i0 * 32 * 16 * 3 * 3 + j0 * 3 * 3 * 16 * 16 + __memcpy_i_1_0 * 3 * 16 * 16 + __memcpy_i_1_1 * 16 * 16 + jj * 16 + ii] = weight[i0 * 16 + ii][j0 * 16 + jj][__memcpy_i_1_0][__memcpy_i_1_1];
            }
          }
        }
      }
    }
  }
//memcpy(Cin_hw, &Cin[0][0][0], sizeof(float) * NUM * INIMROW * INIMCOL);
//memcpy(weight_hw, &weight[0][0][0],
//       sizeof(float) * NUM * NUM * KERNEL * KERNEL);
  (cout<<"start cnn computation") << std::endl;
  long beginTime = clock();
  
  cnn_kernel(Cout_hw,Cin_hw,weight_hw,bias,Cout_hw,Cin_hw,weight_hw,bias);
  for (int i0 = 0; i0 < 32 / 16; i0++) {
    for (int j = 0; j < 224; j++) {
      for (int k = 0; k < 224; k++) {
        for (int ii = 0; ii < 16; ii++) {
          int i = i0 * 16 + ii;
          Cout_hw[i0 * 224 * 224 * 16 + j * 224 * 16 + k * 16 + ii] += bias[i];
        }
      }
    }
  }
  memcpy((&Cout[0][0][0]),Cout_hw,((size_t )4) * 32 * 224 * 224);
  long endTime = clock();
  (((cout<<"time: ") << (((float )(endTime - beginTime)) / ((float )1000000l)))<<"s") << std::endl;
#if GEN
#else
  int error = ::Verify< 32 , 224 , 224 > (Cout);
  if (error != 0) {
    printf("error ocurrs %d\n",error);
  }
   else {
    printf("all right\n");
  }
#endif
  cmost_free(Cout_hw);
  cmost_free(Cin_hw);
  cmost_free(weight_hw);
  cmost_free(bias);
  return 0;
}
