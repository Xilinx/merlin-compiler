#!/bin/bash
vendor=0

# check and load vendor env
if [ ! -z "$XILINX_SDX" -a ! -z "$XILINX_XRT" ]; then
  source $XILINX_SDX/settings64.sh
  export LD_LIBRARY_PATH=$XILINX_XRT/lib:$LD_LIBRARY_PATH
  export PATH=$XILINX_XRT/bin:$PATH

  vendor=1
  # echo "Loaded Xilinx SDX/XRT tools"
fi

if [ ! -z "$ALTERA_QUARTUS_HOME" ]; then
  export ALTERAOCLSDKROOT=$ALTERA_QUARTUS_HOME/hld
  export INTELFPGAOCLSDKROOT=$ALTERA_QUARTUS_HOME/hld
  export AOCL_BOARD_PACKAGE_ROOT=$ALTERA_QUARTUS_HOME/hld/board/a10_ref
  export PATH=$ALTERA_QUARTUS_HOME/quartus/bin:$ALTERA_QUARTUS_HOME/quartus/sopc_builder/bin:$ALTERA_QUARTUS_HOME/modelsim_ase/linuxaloem:$PATH
  export PATH=$ALTERA_QUARTUS_HOME/hld/bin:$ALTERA_QUARTUS_HOME/hls/bin:$ALTERA_QUARTUS_HOME/modelsim_ase/bin:$PATH
  export PATH=$ALTERA_QUARTUS_HOME/ip/:$PATH
  export PATH=$ALTERA_QUARTUS_HOME/hld/board/a10_ref/hardware/a10gx/ip/:$PATH
  export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$ALTERA_QUARTUS_HOME/hld/linux64/lib:$ALTERA_QUARTUS_HOME/hld/board/a10_ref/linux64/lib:$ALTERA_QUARTUS_HOME/hld/host/linux64/lib
  export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$ALTERA_QUARTUS_HOME/hls/linux64/lib:$ALTERA_QUARTUS_HOME/hls/host/linux64/lib

  vendor=2
  # echo "Loaded Intel OpenCL tools"
fi

if [ ! -z "$XILINX_VITIS" -a ! -z "$XILINX_XRT" ]; then
  export PATH=$XILINX_XRT/bin:$PATH
  export PATH=$XILINX_VIVADO/bin:$PATH
  export PATH=$XILINX_VITIS/bin:$XILINX_VITIS/runtime/bin:$PATH
  export LD_LIBRARY_PATH=$XILINX_VITIS/lib/lnx64.o/Default:$XILINX_VITIS/lib/lnx64.o:$XILINX_VITIS/runtime/lib/x86_64:$XILINX_XRT/lib:$LD_LIBRARY_PATH

  vendor=1
fi

## comment for merlin_core
#if [ "$vendor" -eq 0 ]; then
#  echo "No vendor tool is loaded, exiting..."
#  exit 1
#fi

source $MERLIN_COMPILER_HOME/set_env/env.sh

# let docker think home directory is current
#rand_str=$(head /dev/urandom | tr -dc a-z0-9 | head -c 8; echo '')
#home_dir=$(pwd)/.merlin-home$rand_str
mkdir -p /tmp/home
export HOME=/tmp/home

exec "$@"
