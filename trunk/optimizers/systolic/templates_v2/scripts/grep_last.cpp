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



#include <limits>

#include <stdio.h>
#include <stdlib.h>
#include <string>
using std::string;

string my_itoa(int64_t i)
{
	char str[1024];
    if (i > std::numeric_limits<int>::max() ||
        i < std::numeric_limits<int>::min())
	    sprintf(str, "0x%llxll", i);
    else
        sprintf(str, "%d", i);

	return string(str);
}

int main()
{

    //printf("HELLO WORLD\n");

    printf("\n\n========== grep_last report =============\n");
    // grep feeder_A
    for (int i = 0; i < PE_ROWS; i++)
    {
        //for (int j = 0; j < PE_COLS; j++)
        {

            string idx = string(my_itoa(i)) ; //+ "," + string(my_itoa(j));
            string cmd = "cat log | grep 'Feeder_A("+idx+") wr=' | tail -n 1";
            system(cmd.c_str());
        }
    }
    // grep feeder_B
    //for (int i = 0; i < PE_ROWS; i++)
    {
        for (int j = 0; j < PE_COLS; j++)
        {

            //string idx = string(my_itoa(i)) ; //+ "," + string(my_itoa(j));
            string idx = string(my_itoa(j));
            string cmd = "cat log | grep 'Feeder_B("+idx+") wr=' | tail -n 1";
            system(cmd.c_str());
        }
    }

    // Uncomment to get the total cycles in PE (including the flushing zeros)
//    // grep PE
//    for (int i = 0; i < PE_ROWS; i++)
//    {
//        for (int j = 0; j < PE_COLS; j++)
//        {
//
//            string idx = string(my_itoa(i)) + "," + string(my_itoa(j));
//            string cmd = "cat log | grep 'PE("+idx+") idx=' | tail -n 1";
//            system(cmd.c_str());
//        }
//    }
    // grep PE write
    for (int i = 0; i < PE_ROWS; i++)
    {
        for (int j = 0; j < PE_COLS; j++)
        {

            string idx = string(my_itoa(i)) + "," + string(my_itoa(j));
            string cmd = "cat log | grep 'PE("+idx+") wr=' | tail -n 1";
            system(cmd.c_str());
        }
    }
   
    // grep Chain_C
    //for (int i = 0; i < PE_ROWS; i++)
    {
        for (int j = 0; j < PE_COLS; j++)
        {

            //string idx = string(my_itoa(i)) ; //+ "," + string(my_itoa(j));
            string idx = string(my_itoa(j));
            string cmd = "cat log | grep 'Chain_C("+idx+") wr=' | tail -n 1";
            system(cmd.c_str());
        }
    }
    {
        string cmd = "cat log | grep 'Drain_C wr=' | tail -n 1";
        system(cmd.c_str());
    }
    {
        string cmd = "cat log | grep 'Loader_A wr=' | tail -n 1";
        system(cmd.c_str());
    }
    {
        string cmd = "cat log | grep 'Loader_B wr=' | tail -n 1";
        system(cmd.c_str());
    }


    // grep PE read_c_in
    //if (0)
    //for (int i = 0; i < PE_ROWS; i++)
    {
        int i = 0;
        for (int j = 0; j < PE_COLS; j++)
        {
            string idx = string(my_itoa(i)) + "," + string(my_itoa(j));
            string cmd = "cat log | grep 'PE("+idx+") read_c_in=' ";
            system(cmd.c_str());
        }
    }
    return 0;
}
