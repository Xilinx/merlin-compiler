export LM_LICENSE_FILE=2100@fcs:$LM_LICENSE_FILE

VERSION=2017.1

#export XILINX_OPENCL=/curr/hanhu/sdaccel_library/pkg_ku3_2015.4_2ddr/pcie
#export XILINX_OPENCL=/curr/software/pkg_ku3_2015.4_2ddr/pcie
#export XCL_PLATFORM=xilinx_adm-pcie-ku3_2ddr_2_1
#export XCL_PLATFORM=xilinx_adm-pcie-ku3_1ddr_3_0



export XILINX_SDX=/curr/software/Xilinx/SDx/$VERSION
source $XILINX_SDX/settings64.sh
export PATH=$PATH:$XILINX_SDX/Vivado_HLS/bin/
export PATH=$PATH:$XILINX_SDX/Vivado/bin/
#export LD_LIBRARY_PATH=$XILINX_OPENCL/lib/lnx64.o:$XILINX_OPENCL/runtime/lib/x86_64:$LD_LIBRARY_PATH

