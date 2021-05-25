#!/bin/bash

# common functions and variables used in testing utils

script_dir=$(cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)
work_dir=$(pwd)/test

merlin_home=$(readlink -f $script_dir/../)
suite_src=$merlin_home/regression_test/configuration

function check_file {
  local file="$1";
  if [ ! -f $file ]; then
    echo "Error: cannot find $file";
    exit 1;
  fi;
}

function check_run {
  local cmd="$@";
  #eval "$cmd" &> /dev/null;
  eval "$cmd";
  if [ "$?" -ne 0 ]; then
    echo "Error: failed to execute: $cmd";
    exit 1
  fi;
}

function filter_cases {
  # remove test cases that starts with '#'
  local suite="$1";
  local src="$suite_src/${suite}.list";
  check_file $src;

  # ignore lines start with '#'
  # also replaces MERLIN_COMPILER_HOME with the absolute path
  sed '/^#/ d' $src | sed '/^$/d' | sed -e 's|\$MERLIN_COMPILER_HOME|'$merlin_home'|g';
}

function copy_cases {
  # copy test cases to work_dir 
  local suite_file="$1";

  while read line; do
    local case=$(echo "$line" | awk '{print $1}');
    local src=$(echo "$line" | awk '{print $2}');
    local dst="$(dirname $suite_file)/$case";
    if [ ! -z "$case" ]; then
      mkdir -p $(dirname $dst) && cp -r $src $dst;
    fi;
  done < $suite_file;
}
