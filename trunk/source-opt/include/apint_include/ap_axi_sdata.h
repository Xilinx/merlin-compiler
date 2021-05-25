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



/*
 * This file contains the definition of the data types for AXI streaming. 
 * ap_axi_s is a signed interpretation of the AXI stream
 * ap_axi_u is an unsigned interpretation of the AXI stream
 */

#ifndef __AP__AXI_SDATA__
#define __AP__AXI_SDATA__

#include "ap_int.h"

template<int D,int U,int TI,int TD>
  struct ap_axis{
    ap_int<D>        data;
    ap_uint<(D+7)/8> keep;
    ap_uint<(D+7)/8> strb;
    ap_uint<U>       user;
    ap_uint<1>       last;
    ap_uint<TI>      id;
    ap_uint<TD>      dest;
  };

template<int D>
  struct ap_axis <D, 0, 0, 0>{
    ap_int<D>        data;
    ap_uint<(D+7)/8> keep;
    ap_uint<(D+7)/8> strb;
    ap_uint<1>       last;
  };

template<int D,int U,int TI,int TD>
  struct ap_axiu{
    ap_uint<D>       data;
    ap_uint<(D+7)/8> keep;
    ap_uint<(D+7)/8> strb;
    ap_uint<U>       user;
    ap_uint<1>       last;
    ap_uint<TI>      id;
    ap_uint<TD>      dest;
  };

template<int D>
  struct ap_axiu <D, 0, 0, 0>{
    ap_uint<D>       data;
    ap_uint<(D+7)/8> keep;
    ap_uint<(D+7)/8> strb;
    ap_uint<1>       last;
  };


template<int D,int U,int TI,int TD> struct qdma_axis;

template<int D>
  struct qdma_axis <D, 0, 0, 0>{
//  private:
    ap_uint<D>       data;
    ap_uint<(D+7)/8> keep;
    ap_uint<1>       last;
//  public:
    ap_uint<D> get_data() const; 
    ap_uint<(D+7)/8> get_keep() const; 
    ap_uint<1> get_last() const; 

    void set_data(const ap_uint<D> &d); 
    void set_keep(const ap_uint<(D+7)/8> &k); 
    void set_last(const ap_uint<1> &l); 
    void keep_all(); 

    qdma_axis(ap_uint<D> d = ap_uint<D>(), ap_uint<(D+7)/8> k = ap_uint<(D+7)/8>(), ap_uint<1> l = ap_uint<1>()) ;
    qdma_axis(const qdma_axis<D, 0, 0, 0> &d);
  };
#include "hls_stream.h"
namespace hls {

template <int D,int U,int TI,int TD>
class stream<ap_axis<D, U, TI, TD> > {
  typedef ap_axis<D, U, TI, TD> __STREAM_T__;
public:
  /// Constructors
  inline stream(); 

  inline stream(const char* name); 

  /// Make copy constructor and assignment operator private
private:
  inline stream(const stream< __STREAM_T__ >& chn);

  inline stream& operator= (const stream< __STREAM_T__ >& chn);

public:
  /// Overload >> and << operators to implement read() and write()
  inline void operator >> (__STREAM_T__& rdata);

  inline void operator << (const __STREAM_T__& wdata);

  /// empty & full
  bool empty() const;

  bool full() const;


  /// Blocking read
  void read(__STREAM_T__ &dout);

  __STREAM_T__ read();

  /// Blocking write
  void write(const __STREAM_T__ &din);

  /// Non-Blocking read
  bool read_nb(__STREAM_T__& dout);

  /// Non-Blocking write
  bool write_nb(const __STREAM_T__& in);


  __STREAM_T__ V;

};

// Specialization
template <int D>
class stream<ap_axis<D, 0, 0, 0> > {
  typedef ap_axis<D, 0, 0, 0>   __STREAM_T__ ;
public:
  /// Constructors
  inline stream(); 

  inline stream(const char* name); 

  /// Make copy constructor and assignment operator private
private:
  inline stream(const stream< __STREAM_T__ >& chn);

  inline stream& operator= (const stream< __STREAM_T__ >& chn); 

public:
  /// Overload >> and << operators to implement read() and write()
  inline void operator >> (__STREAM_T__& rdata);

  inline void operator << (const __STREAM_T__& wdata);

  /// Empty & Full
  bool empty() const;

  bool full() const;

  /// Blocking read
  void read(__STREAM_T__ &dout);

  __STREAM_T__ read();

  /// Nonblocking read
  bool read_nb(__STREAM_T__ &dout);

  // Blocking write
  void write(const __STREAM_T__ &din);

  /// Nonblocking write
  bool write_nb(const __STREAM_T__ &din);

  __STREAM_T__ V;
};

// ap_axiu
template <int D,int U,int TI,int TD>
class stream<ap_axiu<D, U, TI, TD> > {
  typedef ap_axiu<D, U, TI, TD>  __STREAM_T__;
public:
  /// Constructors
  inline stream(); 

  inline stream(const char* name); 

  /// Make copy constructor and assignment operator private
private:
  inline stream(const stream< __STREAM_T__ >& chn);

  inline stream& operator= (const stream< __STREAM_T__ >& chn); 
public:
  /// Overload >> and << operators to implement read() and write()
  inline void operator >> (__STREAM_T__& rdata);

  inline void operator << (const __STREAM_T__& wdata);

  /// empty & full
  bool empty() const;

  bool full() const;


  /// Blocking read
  void read(__STREAM_T__ &dout);

  __STREAM_T__ read();

  /// Blocking write
  void write(const __STREAM_T__ &din);

  /// Non-Blocking read
  bool read_nb(__STREAM_T__& dout);

  /// Non-Blocking write
  bool write_nb(const __STREAM_T__& in);


  __STREAM_T__ V;

};

// Specialization
template <int D>
class stream<ap_axiu<D, 0, 0, 0> > {
  typedef ap_axiu<D, 0, 0, 0>   __STREAM_T__;
public:
  /// Constructors
  inline stream();

  inline stream(const char* name);

  /// Make copy constructor and assignment operator private
private:
  inline stream(const stream< __STREAM_T__ >& chn);

  inline stream& operator= (const stream< __STREAM_T__ >& chn);
public:
  /// Overload >> and << operators to implement read() and write()
  inline void operator >> (__STREAM_T__& rdata);

  inline void operator << (const __STREAM_T__& wdata);

  /// Empty & Full
  bool empty() const;

  bool full() const;

  /// Blocking read
  void read(__STREAM_T__ &dout);

  __STREAM_T__ read();

  /// Nonblocking read
  bool read_nb(__STREAM_T__ &dout);

  // Blocking write
  void write(const __STREAM_T__ &din);

  /// Nonblocking write
  bool write_nb(const __STREAM_T__ &din);

  __STREAM_T__ V;
};

// Specialization for qdma
template <int D>
class stream<qdma_axis<D, 0, 0, 0> > {
  typedef qdma_axis<D, 0, 0, 0>  __STREAM_T__;
public:
  /// Constructors
  inline stream(); 

  inline stream(const char* name); 

  /// Make copy constructor and assignment operator private
private:
  inline stream(const stream< __STREAM_T__ >& chn);

  inline stream& operator= (const stream< __STREAM_T__ >& chn); 
public:
  /// Overload >> and << operators to implement read() and write()
  inline void operator >> (__STREAM_T__& rdata);

  inline void operator << (const __STREAM_T__& wdata);

  /// Empty & Full
  bool empty() const;

  bool full() const;

  /// Blocking read
  void read(__STREAM_T__ &dout);

  __STREAM_T__ read();

  /// Nonblocking read
  bool read_nb(__STREAM_T__ &dout);

  // Blocking write
  void write(const __STREAM_T__ &din);

  /// Nonblocking write
  bool write_nb(const __STREAM_T__ &din);

  /// Fifo size
  //inline unsigned size(); 

  __STREAM_T__ V;
};

} // end of hls namespace
#endif
