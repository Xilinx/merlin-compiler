make -f sdaccel.mk host
make -f sdaccel.mk xbin_hw
./mmult_ex bin_mmult_hw.xclbin xilinx:adm-pcie-ku3:2ddr-xpr:4.0.json
