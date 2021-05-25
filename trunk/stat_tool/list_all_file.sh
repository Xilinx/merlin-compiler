#!/bin/bash
print_file() {
  for f in $(ls $1)
    do
      if [ "$f" = "bin" ] || [ "$f" = "build" ] || [ "$f" = "regression_test" ] || [ "$f" = "lib" ] || [ "$f" = "runtime" ] || [ "$f" = "xilinx_flow" ]; then continue; fi
      if [ -d $1/$f ]; then 
        print_file $1/$f
      else 
        echo $1/$f
      fi
    done
}
print_file . 
