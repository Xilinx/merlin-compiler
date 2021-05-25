#!/bin/bash
script_dir=$(cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)
export PATH=$script_dir/bin:$script_dir/bin/unwrapped:$PATH
export MERLIN_COMPILER_HOME=$script_dir
