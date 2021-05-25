# install system libraries
yum install -y epel-release
yum install -y vim perl-XML-Simple which rsync zip unzip tcl
yum install -y boost java 
yum install -y python python-pip python-virtualenv sqlite sqlite-devel python-devel gnuplot
yum install -y redhat-lsb-core
yum install -y gcc gcc-c++

# for xilinx hal hwemu
yum install -y protobuf protobuf-devel 
yum install -y ocl-icd ocl-icd-devel

# install python3 for dse and aocl 19.1
yum install -y https://centos7.iuscommunity.org/ius-release.rpm
yum install -y python36u python36u-pip
rm -rf /usr/bin/python3
ln -s /usr/bin/python3.6 /usr/bin/python3

# install DSE dependencies
pip install -r ./opentuner-requirements.txt

pip3.6 install --upgrade pip pytest coverage pytest-cov pylint
