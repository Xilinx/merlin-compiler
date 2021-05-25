
source $SYSTOLIC_ROOT/scripts/setenv_sdaccel_2017_1.sh

KERNEL_LANG=$1

HOST_EXE_DIR=$SYSTOLIC_ROOT/host/bin
HOST_EXE=cnn.out
HOST_ARGS=cnn_cpu_emu.xclbin

cp $HOST_EXE_DIR/$HOST_EXE $SYSTOLIC_ROOT/kernels/$KERNEL_LANG
cd $SYSTOLIC_ROOT/kernels/$KERNEL_LANG
make xconfig
XCL_EMULATION_MODE=true ./$HOST_EXE $HOST_ARGS
