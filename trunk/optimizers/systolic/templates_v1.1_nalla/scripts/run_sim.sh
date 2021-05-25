#!/bin/sh

version=$1

#source $SYSTOLIC_ROOT/scripts/setenv_altera_16.1.sh
source $SYSTOLIC_ROOT/scripts/setenv_altera_${version}.sh

echo "[Info] host program will run with a default .aocx file"
echo "[Info] consider passing desired .aocx file as a host program cmd line arg"

env CL_CONTEXT_EMULATOR_DEVICE_ALTERA=1 $SYSTOLIC_ROOT/host/bin/cnn_sim.out
