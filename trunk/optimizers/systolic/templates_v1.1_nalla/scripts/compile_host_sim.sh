#!/bin/sh

source $SYSTOLIC_ROOT/scripts/setenv_altera_16.1.sh

echo "=== Cleaning up ==="
make -C $SYSTOLIC_ROOT/host clean TARGET=cnn_sim.out
echo "=== Compiling host program ==="
make -C $SYSTOLIC_ROOT/host CXXFLAGS="-g -DLINUX -DALTERA_CL -DEMULATOR $PARAMS" TARGET=cnn_sim.out VERBOSE=1

