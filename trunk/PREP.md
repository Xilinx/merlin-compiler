# Preparing your machine to build Merlin

## Installing clang 7+, libc++, asan, ubsan (part of compiler-rt) in standard locations

Use GNU Stow to manage installations in /usr/local. CMake, ninja, and git should be easy enough to
build and install on any machine. On old Linux machines, we need gcc 7+ and python 2.7+ to build the
LLVM suite:

    $ scl enable devtoolset-7 python27 bash

Then, build with shared libs to save disk space on the root filesystem:

    $ cmake -GNinja -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON \
        -DCMAKE_INSTALL_PREFIX=/usr/local/stow/llvm ..
    $ ninja install
    $ cd /usr/local/stow

Now create symbolic links in /usr/local pointing to /usr/local/stow:

    $ sudo stow llvm

## Installing libLLVM\*, libClang\* in /curr/software/llvm (already installed on us-1, us-2)

From a `build` subdirectory (assuming clang, libc++ is already installed in a standard location),
build with LIBCXX enabled:

    $ CC=`which clang` CXX=`which clang++` cmake -GNinja -DCMAKE_BUILD_TYPE=Release \
        -DLLVM_ENABLE_LIBCXX=ON -DCMAKE_INSTALL_PREFIX=/curr/software/llvm ..
    $ ninja install

## Troubleshooting on noisy, merlin\*, falcon\*

The following commands should give the following outputs:

    ramk@noisy ~ $ which cmake
    /usr/local/bin/cmake
    ramk@noisy ~ $ which ninja
    /usr/local/bin/ninja
    ramk@noisy ~ $ which clang++
    /usr/local/bin/clang++

On noisy (CentOS 6), with the additional `source scl_source enable python27` in `~/.bash_profile`:

    ramk@noisy ~ $ which python
    /opt/rh/python27/root/usr/bin/python

On merlin\* (CentOS 7):

    ramk@merlin3 ~ $ which python
    /usr/bin/python
