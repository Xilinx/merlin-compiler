
#export MARS_VERSION="2015.3"

#export PATH=$ESLOPT_HOME/$MARS_VERSION/mars-gen/bin:$PATH
export MARS_BUILD_DIR=$MERLIN_COMPILER_HOME/mars-gen
export OPTIMIZER_HOME=$MERLIN_COMPILER_HOME/optimizers

#alias  mars_flow="python ${MARS_BUILD_DIR}/scripts/mars_flow_start_new.py "
alias  run_opencl="perl   ${MARS_BUILD_DIR}/scripts/run_opencl/run_opencl.pl "

#alias  mars_create_project="python ${MARS_BUILD_DIR}/scripts/project/create_project.py"
alias  frontend_pass="perl   ${MARS_BUILD_DIR}/scripts/tldm/frontend_pass.pl"
alias  mars_stop="jobs > jobs.log; python ${MARS_BUILD_DIR}/scripts/mars_stop.py; rm -rf jobs.log"

echo "MARS_VERSION   =$MARS_VERSION"
echo "MARS_BUILD_DIR =$MARS_BUILD_DIR"
echo "OPTIMIZER_HOME =$OPTIMIZER_HOME"


# avoid the problem that can not be back to the original directory
#cd $MARS_BUILD_DIR/../runtime/release; source set_root.csh; cd -;
