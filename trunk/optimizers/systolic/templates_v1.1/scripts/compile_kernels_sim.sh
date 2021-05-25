#!/bin/sh

version=$1

#source $SYSTOLIC_ROOT/scripts/setenv_altera_16.1.sh
source $SYSTOLIC_ROOT/scripts/setenv_altera_${version}.sh

$SYSTOLIC_ROOT/scripts/kernel_sim.sh $SYSTOLIC_ROOT/kernels/kernel_top.cl a10gx
#$SYSTOLIC_ROOT/scripts/kernel_sim.sh $SYSTOLIC_ROOT/kernels/kernel_top.cl p385a_sch_ax115


