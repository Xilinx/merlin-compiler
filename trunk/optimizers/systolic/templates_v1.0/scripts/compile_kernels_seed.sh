#!/bin/sh


source ./scripts/setenv_altera_16.0.sh

seed=$1

./scripts/kernel_build.sh ./kernels/kernel_top.cl a10gx_es3 flat $seed


