#!/bin/bash

# defines regression suite and generate a regression list,
# which can then be executed with run-test.sh regression
# to run the full regression

script_dir=$(cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)
source $script_dir/common.bash

test_suites=(
    "message"
    "cmd_test"
    "rodinia"
    "commandline"
    "bugzilla"
    "onehour_a"
    "onehour_x")

# write the regression set to configuration folder
regression_list=$suite_src/regression.list
rm -f $regression_list

for suite in "${test_suites[@]}"; do
  filter_cases $suite | sed -e "s|^|$suite/|" | sed -e "s|$merlin_home|\$MERLIN_COMPILER_HOME|" >> $regression_list
done
