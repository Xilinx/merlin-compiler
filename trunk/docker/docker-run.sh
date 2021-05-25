#!/bin/bash
script_dir=$(cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)
source $script_dir/common.bash

print_help() {
  echo "USAGE: $0 [options] cmd";
  echo "  Available options are:";
  echo "";
  echo "   -i|--interactive: ";
  echo "           start an interactive docker session";
  echo "   -s|--sudo: ";
  echo "           use root account and run in priviledged mode";
  echo "   -t|--image: ";
  echo "           specify a docker image to run, will skip image build";
  echo "   -h|--help: ";
  echo "           print this message";
  echo "";
  echo "";
}

options=
# additional options
while [[ $# -gt 0 ]]; do
  key="$1"
  if [[ $key != -* ]]; then break; fi
  case $key in
  -i|--interactive)
    echo "INFO: starting interactive docker session"
    options="$options -it"
    ;;
  -s|--sudo)
    echo "INFO: using root account in docker"
    use_root=1
    ;;
  -t|--image)
    image=$2
    if [ -z "image" ]; then
      echo "ERROR: empty image"
      exit 2
    else 
      echo "INFO: using image $image"
    fi
    shift
    ;;
  *)
    # unknown option
    echo "ERROR: Failed to recongize argument '$1'"
    print_help
    exit 1
    ;;
  esac
  shift # past argument or value
done

if [ "$#" -lt 1 ]; then
  print_help
  exit 1
fi

# force to build the image even if it already exist
if [ -z "$image" ]; then
  $script_dir/docker-build.sh
  image="merlin:$docker_tag"
fi

merlin_home=$(readlink -f $script_dir/../)
script=$(echo $(readlink -f $1) | sed -e "s|$merlin_home|/opt/merlin|")
shift

if [ -z "$use_root" ]; then
  options="$options -u $(id -u):$(id -g)"
else 
  options="$options --privileged"
fi

# module load
module load $(cat $merlin_home/set_env/aocl_env)
#module load $(cat $merlin_home/set_env/sdx_env)
module load $(cat $merlin_home/set_env/vitis_env)

# handle vendor tools
function add_env {
  local var=$1;
  local val="${!var}";
  if [ -z "$val" -o ! -d "$val" ]; then
    echo "Error: $val env variable is invalid"
    exit 1
  fi
  options="$options -v $val:$val" 
  options="$options -e "$var=$val""
}

add_env XILINX_VITIS
#add_env XILINX_SDX
add_env XILINX_XRT
add_env XILINX_VIVADO
#if [ -z "$XILINX_SDK" ]; then
#  XILINX_SDK=$(dirname $(dirname $XILINX_SDX))/SDK/$(basename $XILINX_SDX)
#fi
#add_env XILINX_SDK

add_env ALTERA_QUARTUS_HOME

options="$options -e LM_LICENSE_FILE=""$LM_LICENSE_FILE"""

echo "INFO: CMD"
echo docker run \
    $options \
    -v "$script_dir/../test":"/opt/merlin/test" \
    -v "$script_dir/../regression_test":"/opt/merlin/regression_test" \
    -v "$PWD":"/local" \
    -w="/local" \
    -e "WITH_DOCKER=1" \
    --rm \
    $image \
    $script $@

if ! docker run \
     $options \
     -v "$script_dir/../test":"/opt/merlin/test" \
     -v "$script_dir/../regression_test":"/opt/merlin/regression_test" \
     -v "$PWD":"/local" \
     -w="/local" \
     -e "WITH_DOCKER=1" \
     --rm \
     $image \
     $script $@ ; then
  $script_dir/docker-clean.sh
  exit 1
fi

$script_dir/docker-clean.sh
