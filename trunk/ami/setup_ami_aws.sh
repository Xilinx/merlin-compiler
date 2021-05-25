log=/dev/null

step=0

if sudo service docker start &> /dev/null; then
  step=1
fi

if docker image ls &> /dev/null; then
  step=2
fi

if [ -f /opt/falcon/merlin/license/license.lic ] ; then
  step=3
fi

if [ $step -eq 0 ]; then
  echo "  Step 0: Install docker and reboot."
  cp ~/.bashrc ~/.bashrc_back
  sudo yum -y install perl perl-Data-Dumper zip &> $log
  sudo yum -y install docker &> $log
  echo "  docker installed. rebooting... you will see connection closed by remote host"
  sudo reboot
  exit 0
fi

if [ $step -eq 1 ]; then
  echo "  Step 1: Resigiter $USER to docker and reboot."
  sudo service docker start &>> $log
  sudo groupadd docker &>> $log
  sudo usermod -a -G docker centos &>> $log
  echo "  docker group added. rebooting... you will see connection closed by remote host"
  sudo reboot
  exit 0
fi

if [ $step -eq 2 ]; then
  echo "  Step 2: Install Merlin docker image."
  if tar zxvf ~/merlin-*.tgz &>> $log ; then
    rm -rf ~/merlin-*.tgz
  fi
  sudo mkdir -p /opt/falcon
  sudo mv merlin /opt/falcon/
  source /opt/falcon/merlin/install.sh &>> $log
  git clone https://github.com/aws/aws-fpga ~/src/project_data/aws-fpga &>> $log
  cp ~/.bashrc_back ~/.bashrc
  echo "source /opt/falcon/merlin/setup.sh" >> ~/.bashrc
  echo "export LM_LICENSE_FILE=/opt/falcon/merlin/license/license.lic" >> ~/.bashrc
  echo "source ~/src/project_data/aws-fpga/vitis_setup.sh &> /dev/null" >> ~/.bashrc
  echo "sudo service docker start &> /dev/null" >> ~/.bashrc
  echo "export MERLIN_AUTO_DEVICE_XILINX=`ls ~/src/project_data/aws-fpga/Vitis/aws_platform/ | grep xilinx | grep aws | grep f1 | head -n 1`" >> ~/.bashrc
  source ~/src/project_data/aws-fpga/vitis_setup.sh &>> $log
  sudo cp -r \
    /home/centos/src/project_data/aws-fpga/Vitis/aws_platform/* \
    ${XILINX_VITIS}/platforms
  sudo mv ~/license.lic /opt/falcon/merlin/license/
  exit 0
fi

if [ $step -eq 3 ]; then
  echo "  Nothing to be done."  
  exit 0
fi
