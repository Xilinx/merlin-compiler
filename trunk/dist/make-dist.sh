#!/bin/bash
dir=$(cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)
source $dir/../docker/common.bash
$dir/../docker/docker-build.sh

# save to ./merlin
if [ -d merlin ]; then
  echo "warning: removing existing merlin folder"
  rm -rf merlin/
fi

# build release package
export MERLIN_COMPILER_HOME=$dir/..
export PATH=$PATH:$(pwd)/bin
perl $dir/../make_dist.pl

cp -r Merlin_Compiler merlin
rm -rf merlin/install_env.pl
rm -rf merlin/bin/falconlm merlin/bin/lmgrd merlin/bin/lmutil merlin/bin/readme

mkdir -p merlin/package
mkdir -p merlin/license
mkdir -p merlin/build/common

# save docker image
docker save merlin:$docker_tag | gzip -c > merlin/package/image.tgz

# copy scripts
cp $dir/README.md merlin/
cp $dir/install.sh merlin/
cp $dir/setup.sh merlin/
cp $dir/link.sh merlin/build/common
cp $dir/merlin_core merlin/build/bin
ln -s ../../build/bin/merlin_core merlin/bin/unwrapped/merlin_core

# copy falconlm binaries
cp -r $dir/../flexlm/falcon/distribution/lin64/ merlin/license

# remove redundant files
rm -rf merlin/build/bin/mars_opt merlin/bin/unwrapped/mars_opt
rm -rf merlin/build/bin/mars_opt_org merlin/bin/unwrapped/mars_opt_org
rm -rf merlin/build/bin/mcheck merlin/bin/unwrapped/mcheck
rm -rf merlin/source-opt merlin/mars-gen/scripts/msg_report \
       merlin/optimizers merlin/mars-gen/tools/add_header
rm -rf merlin/build/bin/aprpt2xml merlin/bin/unwrapped/aprpt2xml                                         
rm -rf merlin/build/bin/mhs_gen merlin/bin/unwrapped/mhs_gen                                             
rm -rf merlin/build/bin/mod_gen merlin/bin/unwrapped/mod_gen                                             
rm -rf merlin/build/bin/tldm_parser merlin/bin/unwrapped/tldm_parser                                     
rm -rf merlin/build/bin/merlin_frontend merlin/bin/unwrapped/merlin_frontend                             
rm -rf merlin/build/bin/merlin_midend merlin/bin/unwrapped/merlin_midend                                 
rm -rf merlin/build/bin/cfg2def merlin/bin/unwrapped/cfg2def                                             
rm -rf merlin/build/bin/cfg_gen merlin/bin/unwrapped/cfg_gen                                             
rm -rf merlin/build/bin/display merlin/bin/unwrapped/display                                             
rm -rf merlin/build/bin/file_compare merlin/bin/unwrapped/file_compare                                   
rm -rf merlin/build/bin/mars_aws merlin/bin/unwrapped/mars_aws
rm -rf merlin/build/bin/marscc merlin/bin/unwrapped/marscc                                               
rm -rf merlin/build/bin/mars_cloud_test merlin/bin/unwrapped/mars_cloud_test                             
rm -rf merlin/build/bin/mars_copy merlin/bin/unwrapped/mars_copy                                         
rm -rf merlin/build/bin/mars_copy_dev merlin/bin/unwrapped/mars_copy_dev                                 
rm -rf merlin/build/bin/mars_exec_cloud merlin/bin/unwrapped/mars_exec_cloud                             
rm -rf merlin/build/bin/mars_perl_dec merlin/bin/unwrapped/mars_perl_dec                                 
rm -rf merlin/build/bin/mars_python_dec merlin/bin/unwrapped/mars_python_dec                             
rm -rf merlin/build/bin/mars_test merlin/bin/unwrapped/mars_test 
rm -rf merlin/set_env/.gitignore
rm -rf merlin/bin/merlinprof
rm -rf merlin/.version

# create a tarball
tar zcf merlin-package.tgz merlin/
rm -rf merlin/
