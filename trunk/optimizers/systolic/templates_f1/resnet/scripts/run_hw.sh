
source $SYSTOLIC_ROOT/scripts/setenv_sdaccel.sh

HOST_EXE_DIR=$SYSTOLIC_ROOT/src/host/bin
HOST_EXE=cnn.out
HOST_ARGS=cnn_hw.xclbin

cp $HOST_EXE_DIR/$HOST_EXE $SYSTOLIC_ROOT/src/kernels
cd $SYSTOLIC_ROOT/src/kernels
XCL_EMULATION_MODE=false ./$HOST_EXE $HOST_ARGS
