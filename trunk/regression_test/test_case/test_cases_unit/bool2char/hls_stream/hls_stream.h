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
#ifndef X_HLS_STREAM_SYN_H
#define X_HLS_STREAM_SYN_H

/*
 * This file contains a C++ model of hls::stream.
 * It defines AutoESL synthesis model.
 */
#ifndef __cplusplus
#error C++ is required to include this header file

#else

namespace hls {


//////////////////////////////////////////////
// Synthesis models for stream
//////////////////////////////////////////////
template<typename __STREAM_T__>
class stream
{
  public:
    /// Constructors
     stream(); 

     stream(const char* name); 

  /// Make copy constructor and assignment operator private
  private:  
     stream(const stream< __STREAM_T__ >& chn);

     stream& operator= (const stream< __STREAM_T__ >& chn); 

  public:
    /// Overload >> and << operators to implement read() and write()
     void operator >> (__STREAM_T__& rdata); 

     void operator << (const __STREAM_T__& wdata); 

  /// Public API
  public:
    /// Empty & Full
     bool empty() const; 

     bool full() const ;

    /// Blocking read
     void read(__STREAM_T__& dout); 

     __STREAM_T__ read(); 

    /// Nonblocking read
     bool read_nb(__STREAM_T__& dout); 

    /// Blocking write
     void write(const __STREAM_T__& din); 

    /// Nonblocking write
     bool write_nb(const __STREAM_T__& din); 

    /// Fifo size
     unsigned size(); 
    
  public:  
    __STREAM_T__ V; 
};


} // namespace hls

#endif // __cplusplus
#endif  // X_HLS_STREAM_SYN_H


