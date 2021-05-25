DSE_ROOT=$1
VENV=$2
RPT=$3
VERSION=$4
FLAG="--config-file ${DSE_ROOT}/checks/mypy.ini "

if [ ${VERSION} -eq 2 ]; then
    FLAG="${FLAG} --py2"
fi

exit_code=0
rm ${RPT}
source ${VENV}/bin/activate
export PATHONPATH_BAK=${PYTHONPATH}
export PYTHONPATH=${DSE_ROOT}/scripts/:${PYTHONPATH}
export PYTHONPATH=${DSE_ROOT}/libs/opentuner/:${PYTHONPATH}
# The following two lines should be uncommented to include OpenTuner code
export MYPYPATH=${DSE_ROOT}/scripts/:${MYPYPATH}
#export MYPYPATH=${DSE_ROOT}/libs/opentuner/:${MYPYPATH}
mypy ${FLAG} ${DSE_ROOT}/scripts/*.py > ${RPT}
export PATHONPATH=${PYTHONPATH_BAK}
deactivate

line_conv=`head -n 1 linecount.txt | perl -ne 'print int(100.0*int($1)/int($2)) if /(\d+) +(\d+).+total/'`
if [ ${line_conv} -lt 50 ]; then # FIXME: The final target is 100%
    exit_code=1
fi

func_conv=`head -n 1 linecount.txt | perl -ne 'print int(100.0*int($1)/int($2)) if /.+ +(\d+) +(\d+) +total/'`
if [ ${func_conv} -lt 55 ]; then # FIXME: The final target is 100%
    exit_code=1
fi

err=`grep error type2.rpt | wc | perl -ne 'print $1 if /(\d+) +.*/'`
if [ ${err} -gt 0 ]; then
    exit_code=1
fi

if [ $exit_code -eq 1 ]; then
    echo "Type checking: line conv ${line_conv}%, func conv ${func_conv}% with $err errors...FAILED"
    echo "    We require 100% coverage with NO errors"
else
    echo "Type checking: line conv ${line_conv}%, func conv ${func_conv}% with $err errors...OK"
fi
echo "    See `pwd`/{linecount.txt,${RPT}} for detail reports"
exit $exit_code
