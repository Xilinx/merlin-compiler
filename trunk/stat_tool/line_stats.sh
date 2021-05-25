#!/bin/bash

source_dirs="source-opt/tools  legacy/tools/parsers/src/
optimizers/autodse/tools optimizers/systolic/tools/systolic_frontend/src
mars-gen set_env docker ami test cpplint clang-tidy install_env"
total_cpp_lines=0
for dir in ${source_dirs}
  do
    for f in $(find $dir -name "*.h") $(find $dir -name "*.cpp")
      do
        line=`wc -l $f | awk '{print $1}'`
        echo $f
        let total_cpp_lines=total_cpp_lines+$line
      done
  done
echo "total lines of C++ is $total_cpp_lines"

total_script_lines=0
for dir in ${source_dirs}
  do
    for f in $(find $dir -name "*.py") $(find $dir -name "*.pl") $(find $dir -name "*.sh")
      do
        line=`wc -l $f | awk '{print $1}'`
        echo $f
        let total_script_lines=total_script_lines+$line
      done
  done
echo "total lines of script is $total_script_lines"
