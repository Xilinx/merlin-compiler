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
#include<hls_stream.h>
#include <ap_int.h>
void write_sub(hls::stream<ap_int<16> > &in, hls::stream<ap_int<16> > &out) {
  out.write(in.read());
  out.write(in.read());
}
void read_sub(hls::stream<ap_int<16> > &in, hls::stream<ap_int<16> > &out) {
  int c = in.read(); // dump the first data
  out.write(in.read());
}
#pragma ACCEL kernel
void top(hls::stream<ap_int<16> > &in, hls::stream<ap_int<16> > &out) {
#pragma HLS dataflow
  hls::stream<ap_int<16> > channel;
  write_sub(in, channel);
  read_sub(channel, out);
}
