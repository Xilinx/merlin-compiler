FROM merlin-base:latest
MAINTAINER Di.Wu allwu@facon-computing.com
LABEL Version "0.1"

# add merlin folders
ADD . /opt/merlin

# setup environment
ENV MERLIN_COMPILER_HOME /opt/merlin
ENV PATH $MERLIN_COMPILER_HOME/build/bin:$PATH
ENV LD_LIBRARY_PATH $MERLIN_COMPILER_HOME/source-opt/lib/rose/lib:$LD_LIBRARY_PATH
ENV LD_LIBRARY_PATH $MERLIN_COMPILER_HOME/source-opt/lib/boost_1_67/lib:$LD_LIBRARY_PATH
ENV LD_LIBRARY_PATH $MERLIN_COMPILER_HOME/source-opt/lib/gcc4.9.4/lib64:$LD_LIBRARY_PATH
ENV LD_LIBRARY_PATH /usr/lib/jvm/jre/lib/amd64:/usr/lib/jvm/jre/lib/amd64/server:$LD_LIBRARY_PATH

RUN chmod +x /opt/merlin/entrypoint.sh
ENTRYPOINT ["/opt/merlin/entrypoint.sh"]
