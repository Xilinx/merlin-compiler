#!/bin/sh

#source $SYSTOLIC_ROOT/scripts/setenv_altera_16.0.sh
#source $SYSTOLIC_ROOT/scripts/setenv_altera_16.1.sh
source $SYSTOLIC_ROOT/scripts/setenv_altera_17.1.sh

echo "=== Cleaning up ==="
make -C $SYSTOLIC_ROOT/host clean TARGET=cnn.out
echo "=== Compiling host program ==="
make -C $SYSTOLIC_ROOT/host CXXFLAGS="-g -DLINUX -DALTERA_CL $PARAMS" TARGET=cnn.out VERBOSE=1

