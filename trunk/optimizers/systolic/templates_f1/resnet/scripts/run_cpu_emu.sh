
source $SYSTOLIC_ROOT/scripts/setenv_sdaccel.sh

HOST_EXE_DIR=$SYSTOLIC_ROOT/src/host/bin
HOST_EXE=cnn.out
HOST_ARGS=cnn_cpu_emu.xclbin

cp $HOST_EXE_DIR/$HOST_EXE $SYSTOLIC_ROOT/src/kernels
cd $SYSTOLIC_ROOT/src/kernels
make xconfig
#XCL_EMULATION_MODE=true ./$HOST_EXE $HOST_ARGS
XCL_EMULATION_MODE=sw_emu ./$HOST_EXE $HOST_ARGS
