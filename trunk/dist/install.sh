#!/bin/bash
script_dir=$(cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)

package=$script_dir/package/image.tgz

if [ ! -f "$package" ]; then
  echo "[merlin-install] cannot find image: $package"
  exit 2
fi

tag=latest
if [ $# -gt 0 ]; then
  tag=$1
fi

# check docker 
if ! which docker &> /dev/null; then
  echo "[merlin-install] cannot find docker executable, please install docker first"
  exit 1
fi

echo "[merlin-install] unpacking image"
image=$(docker load -i $package | grep Loaded | awk '{print $NF}')
docker tag $image merlin:$tag
echo "[merlin-install] created a docker image as 'merlin:latest'"
