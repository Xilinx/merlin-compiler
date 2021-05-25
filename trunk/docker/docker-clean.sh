#!/bin/bash
script_dir=$(cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)
source $script_dir/common.bash

echo "removing merlin:$docker_tag"
docker image rm merlin:$docker_tag
docker image prune -f
