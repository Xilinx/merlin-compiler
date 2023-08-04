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
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <time.h>
#include <math.h>
#include <string.h>
#include "config.h"
using namespace std;

float rcmp(float a,float b)
{
  return (fabs(((a - b) / (a + b))));
}
template < int OUTF, int INF, int HEI, int WID, int InHEI, int InWID, int KER, int STD >
void LoadData ( float Cin [ INF ] [ InHEI ] [ InWID ], float weight [ OUTF ] [ INF ] [ KER ] [ KER ], float bias [ OUTF ] ) {
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
                    weight [ i ] [ j ] [ k ] [ s ] = ( float ) t_wght [ i * INF * KER * KER + j * KER * KER + k * KER + s ];
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
        for ( int j = 0; j < InHEI; j ++ ) {
            for ( int k = 0; k < InWID; k ++ ) {
                Cin [ i ] [ j ] [ k ] = ( float ) t_in [ i * InHEI * InWID + j * InWID + k ];
            }
        }
    }
    cout << "finish load Cin" << endl;
    delete [ ] t_in;
    fi . close ( );
}
template < int OUTF, int HEI, int WID >
int Verify ( float Cout [ OUTF ] [ HEI ] [ WID ] ) {
    int error = 0;
    ifstream fo;
    fo . open ( "output.dat", ios :: in | ios :: binary );
    double * t_out = new double [ OUTF * HEI * WID ];
    fo . read ( ( char * ) t_out, sizeof ( double ) * OUTF * HEI * WID );
    for ( int i = 0; i < OUTF; i ++ ) {
        for ( int j = 0; j < HEI; j ++ ) {
            for ( int k = 0; k < WID; k ++ ) {
                if ( rcmp ( Cout [ i ] [ j ] [ k ], ( float ) t_out [ i * HEI * WID + j * WID + k ] ) > ( ( ( ( ( ( ( ( (1.000000000000000082e-05) ) ) ) ) ) ) ) ) )
                    printf ( "%f,\t%f,\t%f\n", Cout [ i ] [ j ] [ k ], t_out [ i * HEI * WID + j * WID + k ], rcmp ( Cout [ i ] [ j ] [ k ], ( float ) t_out [ i * HEI * WID + j * WID + k ] ) );
            }
        }
    }
    delete [ ] t_out;
    fo . close ( );
    return error;
}
template < int OUTF, int INF, int HEI, int WID, int InHEI, int InWID, int KER, int STD >
void GenData ( int flag, float Cout [ OUTF ] [ HEI ] [ WID ], float Cin [ INF ] [ InHEI ] [ InWID ], float weight [ OUTF ] [ INF ] [ KER ] [ KER ], float bias [ OUTF ] ) {
    if ( flag == 1 ) {
        cout << "start put gold input" << endl;
        ofstream fw, fb, fi;
        fw . open ( "weight.dat", ios :: out | ios :: app | ios :: binary );
        fb . open ( "bias.dat", ios :: out | ios :: app | ios :: binary );
        double * t_bias = new double [ OUTF ];
        double * t_wght = new double [ OUTF * INF * KER * KER ];
        for ( int i = 0; i < OUTF; i ++ ) {
            t_bias [ i ] = ( ( rand ( ) % 2 == 0 ) ? 2 : - 2 ) * ( double ) rand ( ) / ( double ) 2147483647;
            bias [ i ] = t_bias [ i ];
            for ( int j = 0; j < INF; j ++ ) {
                for ( int k = 0; k < KER; k ++ ) {
                    for ( int s = 0; s < KER; s ++ ) {
                        t_wght [ i * INF * KER * KER + j * KER * KER + k * KER + s ] = ( ( rand ( ) % 2 == 0 ) ? 0 : - 1 ) * ( double ) rand ( ) / ( double ) 2147483647;
                        weight [ i ] [ j ] [ k ] [ s ] = t_wght [ i * INF * KER * KER + j * KER * KER + k * KER + s ];
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
                    t_in [ i * InHEI * InWID + j * InWID + k ] = ( ( rand ( ) % 2 == 0 ) ? 0 : - 1 ) * ( double ) rand ( ) / ( double ) 2147483647;
                    Cin [ i ] [ j ] [ k ] = t_in [ i * InHEI * InWID + j * InWID + k ];
                }
            }
        }
        cout << "finish Cin" << endl;
        fi . write ( ( char * ) t_in, sizeof ( double ) * INF * InHEI * InWID );
        delete [ ] t_in;
        fi . close ( );
    }
    else if ( flag == 2 ) {
        ofstream fo;
        fo . open ( "output.dat", ios :: out | ios :: app | ios :: binary );
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
    }
    else {
        printf ( "What do you want me to do\?\n" );
    }
}

int main()
{
  static float Cout[512][224][224];
  static float Cout_hw[114688 * 224];
  static float Cin[512][226][226];
  static float Cin_hw[115712 * 226];
  static float weight[512][512][3][3];
  static float weight_hw[786432 * 3];
  static float bias[512];
#if GEN
#else
  ::LoadData< 512 , 512 , 224 , 224 , 226 , 226 , 3 , 1 > (Cin,weight,bias);
#endif
  memcpy(Cin_hw,(&Cin[0][0][0]),sizeof(float ) * 512 * 226 * 226);
  memcpy(weight_hw,(&weight[0][0][0]),sizeof(float ) * 512 * 512 * 3 * 3);
  (cout<<"start cnn computation") << std::endl;
  long beginTime = clock();
  
  CONV(Cout_hw,Cin_hw,weight_hw,bias);
  memcpy((&Cout[0][0][0]),Cout_hw,sizeof(float ) * 512 * 224 * 224);
  long endTime = clock();
  ((cout<<"time: ") << (((float )(endTime - beginTime)) / ((float )1000000l))) << std::endl;
#if GEN
#else
  int error = ::Verify< 512 , 224 , 224 > (Cout);
  if (error != 0) {
    printf("error ocurrs %d\n",error);
  }
   else {
    printf("all right\n");
  }
#endif
  return 0;
}
