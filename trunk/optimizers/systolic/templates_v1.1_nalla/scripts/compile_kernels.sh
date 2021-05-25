#!/bin/sh

version=$1

#source ./scripts/setenv_altera_16.0.sh
source ./scripts/setenv_altera_${version}.sh

#./scripts/kernel_build.sh ./kernels/kernel_top.cl a10gx_es3
#./scripts/kernel_build.sh ./kernels/kernel_top.cl a10gx
./scripts/kernel_build.sh ./kernels/kernel_top.cl p385a_sch_ax115


