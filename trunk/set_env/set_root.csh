#!/bin/csh

setenv MARS_VERSION "2016.4"

set sourced = ($_)
set tmpvar = `dirname $sourced[2]`
set tmpvar = `readlink -f $tmpvar`

echo "the cmd is: $sourced"

#setenv MERLIN_COMPILER_HOME `readlink -f $tmpvar/..`
setenv ESLOPT_HOME $MERLIN_COMPILER_HOME/legacy/
setenv MARS_TEST_HOME_GIT $MERLIN_COMPILER_HOME/regression_test/

if ( ! $?LD_LIBRARY_PATH ) then
    setenv LD_LIBRARY_PATH .
endif

#if ( ! $?BOOST_HOME ) then
    setenv BOOST_HOME "${MERLIN_COMPILER_HOME}/source-opt/lib/boost_1_53_0"
    setenv LD_LIBRARY_PATH ${BOOST_HOME}/lib:$LD_LIBRARY_PATH
#endif
#if ( ! $?ROSE_HOME ) then
    setenv ROSE_HOME      "${MERLIN_COMPILER_HOME}/source-opt/lib/rose-0.9.6a-28564-nov2016_r1_20180131"
    setenv MARS_ROSE_HOME "${ROSE_HOME}"
    setenv LD_LIBRARY_PATH $ROSE_HOME/lib:${LD_LIBRARY_PATH}
#endif
#if ( ! $?JAVA_HOME ) then
    setenv JAVA_HOME "${MERLIN_COMPILER_HOME}/source-opt/lib/jdk1.8.0_05"
    setenv GLIBCXX_HOME "${ESLOPT_HOME}/lib/libstdc++6-4.8.2"
    setenv LD_LIBRARY_PATH ${JAVA_HOME}/jre/lib/amd64/server:$LD_LIBRARY_PATH
    setenv PATH ${JAVA_HOME}/jre/lib/amd64/server:$PATH
    setenv PATH ${JAVA_HOME}/bin:$PATH
#endif

echo "BOOST_HOME     =$BOOST_HOME"
echo "ROSE_HOME      =$ROSE_HOME"
echo "JAVA_HOME      =$JAVA_HOME"
echo "ESLOPT_HOME    =$ESLOPT_HOME"

setenv PATH $MERLIN_COMPILER_HOME/bin:$MERLIN_COMPILER_HOME/bin/util:$PATH


source $MERLIN_COMPILER_HOME/set_env/set_env.csh
setenv FCSRT_HOME $MERLIN_COMPILER_HOME/runtime/dist
source $MERLIN_COMPILER_HOME/runtime/dist/set_root.csh

