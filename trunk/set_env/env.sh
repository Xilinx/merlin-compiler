# NOTE:
# This file will be automatically called using mars_env_loader. 
# This practice should be deprecated. -- Di 04/24/2019

if [ -z "$WITH_DOCKER" ]; then
  # if the script is running inside docker, disable the following settings
  export MERLIN_COMPILER_HOME=$(readlink -f $(dirname $BASH_SOURCE)/..)
  export PATH=$MERLIN_COMPILER_HOME/build/bin:$PATH

  # -reset_ld_path will be automatically called by mars_env_loader, which messes
  # the environment settings up. 
  if [ "$1" == "-reset_ld_path" ]; then
    export LD_LIBRARY_PATH=
  fi
  if [ -d $MERLIN_COMPILER_HOME/source-opt/lib/boost_1_53_0/lib ]; then
    export LD_LIBRARY_PATH=$MERLIN_COMPILER_HOME/source-opt/lib/boost_1_53_0/lib:$LD_LIBRARY_PATH
  fi
  export LD_LIBRARY_PATH=$MERLIN_COMPILER_HOME/source-opt/lib/rose/lib:$LD_LIBRARY_PATH
  export LD_LIBRARY_PATH=$MERLIN_COMPILER_HOME/source-opt/lib/boost1.67.0/lib:$LD_LIBRARY_PATH
  export LD_LIBRARY_PATH=$MERLIN_COMPILER_HOME/source-opt/lib/gcc4.9.4/lib64:$LD_LIBRARY_PATH
fi

# The following environments will be used in various places, they should be deprecated
# but currently enabled for backward-compatbility

export MARS_COMPILER_HOME=$MERLIN_COMPILER_HOME
export MARS_HOME=$MERLIN_COMPILER_HOME/source-opt
export OPTIMIZER_HOME=$MERLIN_COMPILER_HOME/optimizers
export MARS_BUILD_DIR=$MERLIN_COMPILER_HOME/mars-gen
export ESLOPT_HOME=$MERLIN_COMPILER_HOME/legacy
export MARS_TEST_HOME_GIT=$MERLIN_COMPILER_HOME/regression_test/
