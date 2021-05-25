#######################################
# Merlin 
#######################################
export MERLIN_COMPILER_HOME=/home/hhu/WORK/Merlin_Compiler/trunk

export MERLIN_PATH=$MERLIN_COMPILER_HOME/source-opt/lib/
export llvm_path=$MERLIN_PATH/llvm7.0.0
export deps_path=$MERLIN_PATH/
export clang_path=$MERLIN_PATH/clang
export rose_path=$MERLIN_PATH/rose
export rjson_path=$MERLIN_PATH/rapidjson
export gcc_path=$MERLIN_PATH/gcc4.9.4
export boost_path=$MERLIN_PATH/boost_1_67

export MERLIN_DEPS=$deps_path
export LLVM_ROOT_DIR=$llvm_path
export CLANG_ROOT_DIR=$clang_path
export ROSE_ROOT_DIR=$rose_path
export RAPIDJSON_ROOT_DIR=$rjson_path
export GCC_ROOT_DIR=$gcc_path
export BOOST_ROOT=$boost_path

export PATH=$gcc_path/bin/:$PATH
export PATH=$llvm_path/bin/:$PATH

export LD_LIBRARY_PATH=$gcc_path/lib64:$LD_LIBRARY_PATH

export PATH=$MERLIN_COMPILER_HOME/build/bin:$PATH
