# Merlin Compiler

## Prerequisites

Operating System
- CentOS 7.x

These system packages are required:
- CMake 3.10+
- boost 1.53.0
- sqlite3-devel
- python 2.7+ with virtualenv
- docker

To install system packages:
```
sudo yum -y install epel-release
sudo yum install -y boost boost-devel java build-essential jsoncpp cmake3 glibc-devel gcc libstdc++-static 
sudo yum install -y libgcrypt-devel perl-XML-Simple ninja-build
sudo yum install -y python python-pip python-virtualenv sqlite sqlite-devel python-devel gnuplot 
sudo yum install docker
```

To install and start docker, please refer to:
https://phoenixnap.com/kb/how-to-install-docker-centos-7

In addition, several python packages are required to run DSE correctly, which can be installed with:
```
pip install -r ./optimizers/autodse/libs/opentuner/requirements.txt --user
```

These packages are required, but is already installed in /curr/software
-   ninja 1.8.1+: /curr/software/ninja-1.8.2
-   libLLVM\* libClang\* linked against libc++: /curr/software/llvm.
-   clang 7+, libc++, asan, ubsan (part of compiler-rt): packaged inside repo
-   rose, rapidjson

In addition, customized compiler include files are required to run `mcheck` and `mrefactor` correctly, which are installed in `/curr/software/merlin_deps/` and 
linked to `./trunk/source-opt/lib` symbolically. These two folders are required to run unit tests successfully. 

Currently we use docker to containerize the merlin build. To use docker, we need to setup docker environment. Try running the following command to validate:
```
docker ps
```
If the above command fails, please contact the system admin. 

## Building Merlin

    $ export MERLIN_PATH=/path/to/merlin-deps-binary-library-local/
    $ export llvm_path=$MERLIN_PATH/llvm
    $ export deps_path=$MERLIN_PATH/merlin-deps
    $ export clang_path=$MERLIN_PATH/merlin-deps/clang
    $ export rose_path=$MERLIN_PATH/merlin-deps/rose
    $ export rjson_path=$MERLIN_PATH/merlin-deps/rapidjson
    $ export gcc_path=$MERLIN_PATH/merlin-deps/gcc4.9.4
    $ export boost_path=$MERLIN_PATH/merlin-deps/boost_1_67

    $ export MERLIN_DEPS=$deps_path
    $ export LLVM_ROOT_DIR=$llvm_path
    $ export CLANG_ROOT_DIR=$clang_path
    $ export ROSE_ROOT_DIR=$rose_path
    $ export RAPIDJSON_ROOT_DIR=$rjson_path
    $ export GCC_ROOT_DIR=$gcc_path
    $ export BOOST_ROOT=$boost_path

    $export PATH=$gcc_path/bin/:$PATH
    $export PATH=$llvm_path/bin/:$PATH
    
    $ cd trunk/build
    $ cmake3 -GNinja -DCMAKE_BUILD_TYPE=Release ..
    $ ninja

To build debug:

    $ cmake3 -GNinja -DCMAKE_BUILD_TYPE=Debug ..

To change the compiler:

    $ CC=`which gcc` CXX=`which g++` cmake3 -GNinja ..

To build a specific target:

    $ ninja mars-gen/tools/mars_flow/all

To build after change in sources (`ninja clean` is never necessary):

    $ ninja

To clean everything except what's tracked by git:

    $ git clean -dfx

## Testing Merlin
To run unit tests:
  
    $ ninja test

In addition, to run other test suite:

    $ ../test/run-test.sh $suite

To run other test suite inside docker container:

    $ ../docker/docker-run.sh ../test/run-test.sh $suite

To run regression tests:
  
    $ ninja regression

To push current docker image to AWS ECR in order to run AutoQoR or other tests:

    $ ninja docker_push

To generate a release package containing a docker image
   
    $ ninja dist

To generate an AMI containing Merlin & SDAccel, follow instructions in `trunk/ami/README.md`

## Formating 

To format without check:

    $make format

To format with check:
    
    $make format_check=1 format

To check format style

    edit file .clang_format

## Running Address Sanitizer and Undefined-Behavior Sanitizer

    $ cmake3 -GNinja -DSANITIZE=ON -DCMAKE_BUILD_TYPE=Debug ..

## Legacy build (for backwards-compatibility)

    $ cd trunk
    $ make all

This flow is largely unsupported; if you run into problems, simply `make clean` beforehand.

## Troubleshooting

If `ninja` goes into an infinite loop, there is likely a clock skew:

    $ find . -name CMakeLists.txt -exec touch {} \;

If a build fails, you can view build logs in `CMakeFiles/{CMakeOutput.log,CMakeError.log}`.
