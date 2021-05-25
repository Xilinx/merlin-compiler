
source $SYSTOLIC_ROOT/scripts/setenv_sdaccel.sh

echo "=== Cleaning up ==="
make -C $SYSTOLIC_ROOT/src/kernels clean
echo "=== Compiling host program ==="
make -C $SYSTOLIC_ROOT/src/kernels SDA_FLOW=$1
