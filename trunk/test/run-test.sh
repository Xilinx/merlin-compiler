#!/bin/bash
# run a test suite from a definition

script_dir=$(cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)
source $script_dir/common.bash

# check work_dir to avoid running test in /curr
if [ "$(echo $work_dir | cut -d '/' -f2)" = "curr" ]; then
  echo "Please run the test in local disk rather than your home folder"
  exit 1
fi

suite=$1
suite_file=$work_dir/${suite}/cases.list

if [[ $suite =~ "unit_test" ]]; then
  target=unit_test
else
  target=test
fi

echo "[Suite $suite]: started at $(date)"
start_ts=$(date +%s)
rm -rf $work_dir/$suite
mkdir -p  $work_dir/$suite

filter_cases $suite > $suite_file
check_run copy_cases $suite_file

# copy the files there
cp $script_dir/make-rpt $work_dir/$suite/
cp $script_dir/test-template.mk $work_dir/$suite/

# generate Makefile that references the template
echo "TARGET := $target" > $work_dir/$suite/Makefile
echo "MAKE   := ./make-rpt" >> $work_dir/$suite/Makefile
echo "include test-template.mk" >> $work_dir/$suite/Makefile
cat ${MERLIN_COMPILER_HOME}/regression_test/msg_report/update_json_makefile >> $work_dir/$suite/Makefile

# use Makefile to run tests in parallel
# -s silent the makefile output
# -j runs cases in parallel
# -k ignores error and continues the test
make -s -C $work_dir/$suite -j $(nproc --all) 2>&1 | tee ${suite}.log

# generate a summary
total_cases=$(cat ${suite}.log | grep '^ -'  | wc -l)
pass_cases=$(cat ${suite}.log | grep PASS | wc -l)
printf "[Suite $suite]: %d/%d cases passed\n" $pass_cases $total_cases

err=0
suite_fail=$suite_src/${suite}-failed.list
rm -f $suite_fail
if [ $total_cases -ne $pass_cases ]; then
  printf "[Suite $suite]: failed cases :\n"

  for c in $(cat ${suite}.log | grep '^ -' | grep -v PASS | awk '{print $2}'); do
    echo " - $c"
    err=$((err + 1))

    # write the failed case to a file
    grep $c $suite_src/${suite}.list >> $suite_fail
  done
fi

end_ts=$(date +%s)
echo "[Suite $suite]: finishes in $((end_ts - start_ts)) seconds"

# exit non-zero if there are failures
exit $err
