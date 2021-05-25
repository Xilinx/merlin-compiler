#!/bin/bash

export MARS_VERSION="2016.4"

export MERLIN_COMPILER_HOME=`pwd`/$(dirname "$BASH_SOURCE")
export ESLOPT_HOME=$MERLIN_COMPILER_HOME/legacy
export MARS_TEST_HOME_GIT=$MERLIN_COMPILER_HOME/regression_test/

export MARS_HOME=$MERLIN_COMPILER_HOME/source-opt

export BOOST_HOME="${MARS_HOME}/lib/boost_1_53_0/"
export LD_LIBRARY_PATH=${BOOST_HOME}/lib:$LD_LIBRARY_PATH
export ROSE_HOME="${MARS_HOME}/lib/rose-0.9.6a-28564-nov2016_r1_20180131"
export MARS_ROSE_HOME="${ROSE_HOME}"
export JAVA_HOME="${MARS_HOME}/lib/jdk1.8.0_05"
export LD_LIBRARY_PATH=${JAVA_HOME}/jre/lib/amd64/server:$LD_LIBRARY_PATH
export PATH=${JAVA_HOME}/jre/lib/amd64/server:$PATH
export PATH=${JAVA_HOME}/bin:$PATH

echo "BOOST_HOME     =$BOOST_HOME"
echo "ROSE_HOME      =$ROSE_HOME"
echo "JAVA_HOME      =$JAVA_HOME"
echo "ESLOPT_HOME    =$ESLOPT_HOME"
echo "MARS_HOME      =$MARS_HOME"

export PATH=$MERLIN_COMPILER_HOME/bin:$PATH
export LD_LIBRARY_PATH=$ROSE_HOME/lib:${LD_LIBRARY_PATH}

source $MERLIN_COMPILER_HOME/set_env/set_env.sh

export FCSRT_HOME=$MERLIN_COMPILER_HOME/runtime/dist

source $MERLIN_COMPILER_HOME/set_env/set_altera.sh
