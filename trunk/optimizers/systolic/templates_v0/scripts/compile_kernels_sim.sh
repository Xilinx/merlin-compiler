#!/bin/sh


#source $SYSTOLIC_ROOT/scripts/setenv_altera_16.1.sh
source $SYSTOLIC_ROOT/scripts/setenv_altera_17.1.sh

$SYSTOLIC_ROOT/scripts/kernel_sim.sh $SYSTOLIC_ROOT/kernels/kernel_top.cl a10gx


