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
#define BUFSIZE 2048

class Bar
{
public:
    void Process()
    {
        for (int i = 0; i < mLen; ++i) {
            mBuf[i] += 99;
        }
    }

    void operator()() {
        Process();
    }

    Bar(int *buf, int len)
        :mBuf(buf), mLen(len)
    {
    }
private:
    int *mBuf;
    int mLen;
};

class Foo
{
public:
    void DoProcess()
    {
        for (int i = 0; i < mLen; ++i) {
            mBuf[i] = i * 3 + mBuf[i];
        }
    }

    void DoProcess(int count)
    {
        for (int i = 0; i < count; ++i) {
            mBar();
        }
    }

    static void DoProcess(int *buf, int len)
    {
        for (int i = 0; i < len; ++i) {
            buf[i] /= 3.5;
        }
    }

    Foo(int *buf, int len)
        :mBuf(buf), mLen(len), mBar(buf, len)
    {
    }
    static int BufLen;
private:
    int *mBuf;
    int mLen;
    Bar mBar;
};

int Foo::BufLen = BUFSIZE;

#pragma ACCEL kernel
void mykernel(int orig[BUFSIZE], int cpy[BUFSIZE])
{
    Foo foo(orig, BUFSIZE);

    foo.DoProcess();

    Foo::DoProcess(orig, BUFSIZE);

    Foo::BufLen = BUFSIZE;

    foo.DoProcess(Foo::BufLen);

    for (int i = 0; i < BUFSIZE; ++i) {
        cpy[i] = orig[i];
    }
}
