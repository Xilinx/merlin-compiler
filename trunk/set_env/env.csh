set sourced = ($_)
set tmpvar = `dirname $sourced[2]`
set tmpvar = `readlink -f $tmpvar`

setenv MERLIN_COMPILER_HOME `readlink -f $tmpvar/..`
setenv MARS_COMPILER_HOME $MERLIN_COMPILER_HOME
setenv MARS_BUILD_DIR $MERLIN_COMPILER_HOME/mars-gen
setenv ESLOPT_HOME $MERLIN_COMPILER_HOME/legacy
setenv PATH $MERLIN_COMPILER_HOME/bin/unwrapped:$PATH

#if($?XILINX_SDX) then
#    setenv XILINX_SDACCEL $XILINX_SDX
#endif
#if($?XILINX_SDACCEL) then
#    if( ! -f $XILINX_SDACCEL/settings64.csh || "$XILINX_SDACCEL" == "" ) then
#        #printf "XILINX_SDACCEL set not correct!\n"
#        printf ""
#    else
#        source $XILINX_SDACCEL/settings64.csh 2>/dev/null
#        setenv PATH $XILINX_SDACCEL/bin:$PATH
#    endif
#endif

if ( "$1" == "-reset_ld_path" ) then
   setenv LD_LIBRARY_PATH 
endif
setenv LD_LIBRARY_PATH $MERLIN_COMPILER_HOME/source-opt/lib/jdk1.8.0_05/jre/lib/amd64/server:$LD_LIBRARY_PATH
setenv LD_LIBRARY_PATH $MERLIN_COMPILER_HOME/source-opt/lib/boost_1_53_0/lib:$LD_LIBRARY_PATH
setenv LD_LIBRARY_PATH $MERLIN_COMPILER_HOME/source-opt/lib:$LD_LIBRARY_PATH
setenv LD_LIBRARY_PATH $MERLIN_COMPILER_HOME/source-opt/lib/rose/lib:$LD_LIBRARY_PATH
setenv LD_LIBRARY_PATH $MERLIN_COMPILER_HOME/source-opt/lib/libstdc++6-4.8.2/lib64:$LD_LIBRARY_PATH
