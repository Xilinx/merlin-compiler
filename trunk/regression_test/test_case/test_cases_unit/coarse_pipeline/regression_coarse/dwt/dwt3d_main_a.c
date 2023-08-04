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


#include <math.h>

void dwt3d_kernel(
        //float* img, 
        float* img_0, 
        float* img_1, 
        float* img_2, 
        float* img_3, 
        float* img_4, 
        float* img_5, 
        float* img_6, 
        float* img_7, 
        int m, int n,  
        int MM, int NN);

//int main(int argc, char *argv[]) 
int main() 
{
    double	elapsedTime = 0;

    char fname_out[50] = "dwt3d_out.dat";

    int level = 1;

    //#define IMG_SIZE 16777216  // 256*256*256
#define IMG_SIZE 1048576 // 16*256*256
#define IMG_SIZE_TILE 131072 // 16*32*256
    int m, n, p, M, N;
    int i;
    float *img; cmost_malloc_1d( &img, "raw_img_256.dat" , 4, IMG_SIZE);

    float * img_0; cmost_malloc_1d( &img_0, "" , 4, IMG_SIZE_TILE);
    //memcpy(img_0, img, IMG_SIZE);
    float * img_1; cmost_malloc_1d( &img_1, "" , 4, IMG_SIZE_TILE);
    //memcpy(img_1, img, IMG_SIZE);
    float * img_2; cmost_malloc_1d( &img_2, "" , 4, IMG_SIZE_TILE);
    //memcpy(img_2, img, IMG_SIZE);
    float * img_3; cmost_malloc_1d( &img_3, "" , 4, IMG_SIZE_TILE);
    //memcpy(img_3, img, IMG_SIZE);
    float * img_4; cmost_malloc_1d( &img_4, "" , 4, IMG_SIZE_TILE);
    //memcpy(img_4, img, IMG_SIZE);
    float * img_5; cmost_malloc_1d( &img_5, "" , 4, IMG_SIZE_TILE);
    //memcpy(img_5, img, IMG_SIZE);
    float * img_6; cmost_malloc_1d( &img_6, "" , 4, IMG_SIZE_TILE);
    //memcpy(img_6, img, IMG_SIZE);
    float * img_7; cmost_malloc_1d( &img_7, "" , 4, IMG_SIZE_TILE);
    //memcpy(img_7, img, IMG_SIZE);

    int idx, ip;
    for (ip = 0; ip < 16; ip++)
    {
        for (i = 0; i < 32; i++)
        {
            memcpy(&(img_0[ip*32*256 + i*256]), &(img[ip*256*256 + (i*8+0)*256]), 256*sizeof(float));
            memcpy(&(img_1[ip*32*256 + i*256]), &(img[ip*256*256 + (i*8+1)*256]), 256*sizeof(float));
            memcpy(&(img_2[ip*32*256 + i*256]), &(img[ip*256*256 + (i*8+2)*256]), 256*sizeof(float));
            memcpy(&(img_3[ip*32*256 + i*256]), &(img[ip*256*256 + (i*8+3)*256]), 256*sizeof(float));
            memcpy(&(img_4[ip*32*256 + i*256]), &(img[ip*256*256 + (i*8+4)*256]), 256*sizeof(float));
            memcpy(&(img_5[ip*32*256 + i*256]), &(img[ip*256*256 + (i*8+5)*256]), 256*sizeof(float));
            memcpy(&(img_6[ip*32*256 + i*256]), &(img[ip*256*256 + (i*8+6)*256]), 256*sizeof(float));
            memcpy(&(img_7[ip*32*256 + i*256]), &(img[ip*256*256 + (i*8+7)*256]), 256*sizeof(float));
        }
    }


    //float *tmp; cmost_malloc_1d( &tmp, ""   , 4, IMG_SIZE);

    m = n = 256;
    M = N = 256;

    int * a; 
    int MM = M;
    int NN = N;


    dwt3d_kernel(
            //img,
            img_0, 
            img_1, 
            img_2, 
            img_3, 
            img_4, 
            img_5, 
            img_6, 
            img_7, 
            m, n, MM, NN);

    for (ip = 0; ip < 16; ip++)
    {
        for (i = 0; i < 32; i++)
        {
            memcpy(&(img[ip*256*256 + (i*8+0)*256]), &(img_0[ip*32*256 + i*256]), 256*sizeof(float));
            memcpy(&(img[ip*256*256 + (i*8+1)*256]), &(img_1[ip*32*256 + i*256]), 256*sizeof(float));
            memcpy(&(img[ip*256*256 + (i*8+2)*256]), &(img_2[ip*32*256 + i*256]), 256*sizeof(float));
            memcpy(&(img[ip*256*256 + (i*8+3)*256]), &(img_3[ip*32*256 + i*256]), 256*sizeof(float));
            memcpy(&(img[ip*256*256 + (i*8+4)*256]), &(img_4[ip*32*256 + i*256]), 256*sizeof(float));
            memcpy(&(img[ip*256*256 + (i*8+5)*256]), &(img_5[ip*32*256 + i*256]), 256*sizeof(float));
            memcpy(&(img[ip*256*256 + (i*8+6)*256]), &(img_6[ip*32*256 + i*256]), 256*sizeof(float));
            memcpy(&(img[ip*256*256 + (i*8+7)*256]), &(img_7[ip*32*256 + i*256]), 256*sizeof(float));
        }
    }

    cmost_dump_1d(img, "img_out.dat");

    cmost_free_1d(img);
    cmost_free_1d(img_0);
    cmost_free_1d(img_1);
    cmost_free_1d(img_2);
    cmost_free_1d(img_3);
    cmost_free_1d(img_4);
    cmost_free_1d(img_5);
    cmost_free_1d(img_6);
    cmost_free_1d(img_7);


    return 0;
}
