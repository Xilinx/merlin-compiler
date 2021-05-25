


sudo yum install -y gcc gcc-c++ cpan perl-XML-Simple bison flex automake libtool libgcrypt-devel  libgpg-error-devel


# trouble shooting;
#  /lib64/ld-lsb-x86-64.so.3: bad ELF interpreter: No such file or directory
sudo yum install -y redhat-lsb    #needed by license_check

# Needed by opentuner for design space exploration
sudo yum install sqlite-devel.x86_64
pip install --user opentuner
