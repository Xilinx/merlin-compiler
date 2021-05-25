#!/bin/bash
script_dir=$(cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)
source $script_dir/common.bash

# force to build the image even if it already exist
$script_dir/docker-build.sh

$(aws ecr get-login --region us-east-1)
docker tag merlin:$docker_tag 520870693817.dkr.ecr.us-east-1.amazonaws.com/merlin:$docker_tag
docker push 520870693817.dkr.ecr.us-east-1.amazonaws.com/merlin:$docker_tag

echo "Pushed merlin:$docker_tag to ECR"
