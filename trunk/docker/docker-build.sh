#!/bin/bash
script_dir=$(cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)
source $script_dir/common.bash

if [ "${git_branch}" = "release" ]; then
  label="release"
else
  label="feature"
fi

if [ -z "$MERLIN_DEPS" ]; then
  MERLIN_DEPS=/curr/software/merlin-deps
fi

sed "s/\${HOSTNAME}/${HOSTNAME}/g" $script_dir/Dockerfile-base > $script_dir/Dockerfile-base-temp
# build base image first
if ! diff $script_dir/Dockerfile-base-temp $MERLIN_DEPS/Dockerfile-${HOSTNAME} &> /dev/null || \
   ! diff $script_dir/merlin-base_dockerscript.sh $MERLIN_DEPS/merlin-base_dockerscript-${HOSTNAME}.sh &> /dev/null; then
  echo "building merlin-base"
  cp $script_dir/Dockerfile-base-temp $MERLIN_DEPS/Dockerfile-${HOSTNAME}
  cp $script_dir/merlin-base_dockerscript.sh $MERLIN_DEPS/merlin-base_dockerscript-${HOSTNAME}.sh
  docker image build -t merlin-base:latest -f $MERLIN_DEPS/Dockerfile-${HOSTNAME} $MERLIN_DEPS
fi

echo "building merlin:$docker_tag"
docker image build \
    --label "$label" \
    -t merlin:$docker_tag \
    -f $script_dir/../docker/Dockerfile $script_dir/../

# make the assumption of latest branch is always built latest
if [ "${git_branch}" = "release" ]; then
  docker image tag merlin:$docker_tag merlin:latest
fi

rm -rf $script_dir/Dockerfile-base-temp
