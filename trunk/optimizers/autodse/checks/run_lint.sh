DSE_ROOT=$1
VENV=$2
RPT=$3
VERSION=$4

exit_code=0
rm ${RPT}
source ${VENV}/bin/activate
export PATHONPATH_BAK=${PYTHONPATH}
export PYTHONPATH=${DSE_ROOT}/scripts/:${PYTHONPATH}
export PYTHONPATH=${DSE_ROOT}/libs/opentuner/:${PYTHONPATH}
pylint -j32 --msg-template='{msg_id}:{line:3d},{column}: {obj}: {msg} ({symbol})' \
       --rcfile=${DSE_ROOT}/checks/.pylintrc \
       ${DSE_ROOT}/scripts/*.py ${DSE_ROOT}/libs/opentuner/opentuner/search/gradientsearch.py \
       ${DSE_ROOT}/scripts/*.py ${DSE_ROOT}/libs/opentuner/opentuner/search/hotspotopt.py > ${RPT}
export PATHONPATH=${PYTHONPATH_BAK}
deactivate

score=`perl -ne 'print $1 if /Your code has been rated at (\d+)\..+\//' ${RPT}`
if [ ${score} -lt 9 ]; then
    exit_code=1
fi

err=`perl -ne 'print $2 if /^\|error( +)\|(\d+).+/' ${RPT}`
if [ ${err} -gt 0 ]; then
    exit_code=1
fi
warn=`perl -ne 'print $2 if /^\|warning( +)\|(\d+).+/' ${RPT}`
if [ ${warn} -gt 0 ]; then
    exit_code=1
fi

if [ $exit_code -eq 1 ]; then
    echo "Python$VERSION: $score/10 with $err errors and $warn warnings...FAILED"
    echo "    We require score >=9 with NO errors and warnings"
else
    echo "Python$VERSION: $score/10 with $err errors and $warn warnings...OK"
fi
echo "    See `pwd`/${RPT} for detail report"
exit $exit_code
