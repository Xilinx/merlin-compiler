#!/bin/sh

#source $SYSTOLIC_ROOT/scripts/setenv_altera_16.0.sh
source $SYSTOLIC_ROOT/scripts/setenv_altera_16.1.sh

echo "[Info] host program will run with a default .aocx file"
echo "[Info] consider passing desired .aocx file as a host program cmd line arg"

$SYSTOLIC_ROOT/host/bin/cnn.out
