#source /curr/pengzh/fcs_setting64.csh

export LM_LICENSE_FILE=1900@10.0.0.4

version=16.1_pro

export ALTERA_AOCL_HOME=/curr/software/Altera/$version
export ALTERA_QUARTUS_HOME=/curr/software/Altera/$version
export ALTERAOCLSDKROOT=/curr/software/Altera/$version/hld/
#export AOCL_BOARD_PACKAGE_ROOT=/curr/software/Altera/$version/hld/board/a10_ref
#export AOCL_BOARD_PACKAGE_ROOT=$ALTERA_QUARTUS_HOME/hld/board/terasic/de5net
export AOCL_BOARD_PACKAGE_ROOT=/curr/software/Altera/$version/hld/board/nalla_pcie


export ALTERA_VERSION_PATH=$ALTERA_QUARTUS_HOME
export QUARTUS_ROOTDIR=$ALTERA_VERSION_PATH/quartus
#export LD_LIBRARY_PATH=$QUARTUS_ROOTDIR/linux64:${LD_LIBRARY_PATH}
#export PATH=$ALTERA_VERSION_PATH/quartus/bin/:$PATH
export PATH=$ALTERAOCLSDKROOT/bin/:$PATH
export PATH=$ALTERAOCLSDKROOT/linux64/bin/:$PATH
export LD_LIBRARY_PATH=$ALTERAOCLSDKROOT/linux64/lib:${LD_LIBRARY_PATH}
export LD_LIBRARY_PATH=$ALTERAOCLSDKROOT/host/lin64/lib:${LD_LIBRARY_PATH}
export LD_LIBRARY_PATH=$ALTERAOCLSDKROOT/host/linux64/lib:${LD_LIBRARY_PATH}
export LD_LIBRARY_PATH=$AOCL_BOARD_PACKAGE_ROOT/linux64/lib:${LD_LIBRARY_PATH}
export ACL_PROFILE_TIMER=1

