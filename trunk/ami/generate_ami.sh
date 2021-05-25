#!/bin/bash

script_dir=$(cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)

print_help() {
  echo "USAGE: $0 [options] cmd";
  echo "  Available options are:";
  echo "  -k|--key: ";
  echo "          AWS key location";
  echo "  -l|--license: ";
  echo "          Falcon license location";
  echo "  -b|--tarball: ";
  echo "          The release docker image tarball(optional)";
  echo "          The script will try to build one if tarball is not given.";
  echo "  -t|--tag: ";
  echo "          AMI name(optional)";
  echo "          Default: Merlin_<git_revison_tag> or Merlin_temp";
  echo "  -h|--help: ";
  echo "          print this message";
  echo "";
  echo "";
}

while [[ $# -gt 0 ]]; do
  key="$1"
  if [[ $key != -* ]]; then break; fi
  case $key in
  -t|--tag)
    echo "INFO: Set AMI name"
    git_revision=$2
    if [ -z "${git_revision}" ]; then
      echo "ERROR: Empty AMI name"
      exit 1
    fi
    shift
    ;;
  -k|--key)
    echo "INFO: Set AWS login key"
    aws_key=$2
    if [ -z "${aws_key}" ]; then
      echo "ERROR: Empty AWS login key"
      exit 1
    fi
    if [ ! -f "${aws_key}" ]; then
      echo "ERROR: AWS login key does not exist"
      exit 1
    fi
    shift
    ;;
  -l|--license)
    echo "INFO: Set Falcon license"
    license=$2
    if [ -z "${license}" ]; then
      echo "ERROR: Empty Falcon license"
      exit 1
    fi  
    if [ ! -f "${license}" ]; then
      echo "ERROR: Falcon license file does not exist"
      exit 1
    fi  
    shift
    ;;
  -b|--tarball)
    echo "INFO: Set Merlin docker image tarball"
    tarball=$2
    if [ -z "${tarball}" ]; then
      echo "ERROR: Empty release tarball"
      exit 1
    fi
    if [ ! -f "${tarball}" ]; then
      echo "ERROR: Tarball file does not exist"
      exit 1
    fi
    shift
    ;;
  -h|--help)
    echo "INFO: Print help message"
    print_help
    exit 1
    ;;
  *)
    echo "ERROR: Failed to recognize argument '$1'"
    print_help
    exit 1
    ;;
  esac
    shift
done

if [ -z "$aws_key" ] ; then
  echo "ERROR: AWS login key must be given."
  print_help
  exit 1
fi

if [ -z "$license" ] ; then
  echo "ERROR: Falcon license must be given."
  print_help
  exit 1
fi

if [ -z "$git_revision" ] ; then
  if git describe --tags &> /dev/null ; then
    git_revision=`git describe --tags | tr '-' '_'`
    ami_description="Merlin ${git_revision} AMI"
  else
    git_revision=temp
    ami_description="Merlin temp image"
  fi
  git_revision=Merlin_${git_revision}
  echo "INFO: AMI name is not given, automatically set to ${git_revision}."
fi

if [ -z "$tarball" ] ; then
  if [ -d "${script_dir}/../build" ] ; then
    echo "INFO: Tar ball file not set, try to build Merlin now..."
    cd ../build
    build_dir=$(cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd) 
    if module purge &> ${script_dir}/build.log \
        && module load merlin_build &>> ${script_dir}/build.log ; then
      echo "INFO: Loaded merlin_build module"
    fi
    if cmake3 -GNinja -DCMAKE_BUILD_TYPE=Release .. &>> ${script_dir}/build.log ; then
      echo "INFO: Passed cmake"
    fi
    if ninja &>> ${script_dir}/build.log ; then
      echo "INFO: Passed ninja"
    fi
    if ninja dist &>> ${script_dir}/build.log ; then
      echo "INFO: Passed ninja dist"
    fi
    cd - &> /dev/null
    tarball="${build_dir}/merlin-package.tgz"
  else
    echo "ERROR: Tar ball file not given."
  fi
fi

if ! aws ec2 run-instances \
  --image-id ami-072158842833d8d00 \
  --count 1 \
  --instance-type t2.micro \
  --security-group-ids sg-ff23a3b4 \
  --key-name user \
  --block-device-mappings \
    '[{"DeviceName":"/dev/sda1","Ebs":{"DeleteOnTermination":true,"VolumeSize":100}}]' \
  --subnet-id subnet-ce147b84  \
  --output json &> aws_instance.info ; then
  echo "ERROR: Instance create failed."
  exit 1
else
  echo "INFO: Passed instance creation."
fi

instance_id=`jq '."Instances"[0]."InstanceId"' aws_instance.info | sed -e 's/^"//' -e 's/"$//'`
instance_ip=`jq '."Instances"[0]."PrivateIpAddress"' aws_instance.info | sed -e 's/^"//' -e 's/"$//'`

while true ; do
  aws ec2 describe-instances --instance-id ${instance_id} --output json > aws_instance_state.info
  instance_state=`jq '."Reservations"[0]."Instances"[0]."State"."Name"' aws_instance_state.info | sed -e 's/^"//' -e 's/"$//'`
  if [ "${instance_state}" = "running" ] ; then
    break
  fi
  sleep 10
done

echo "INFO: Waiting for instance initialization..."
while true ; do
  if ssh -i ${aws_key} \
      -o StrictHostKeyChecking=no \
      centos@${instance_ip} echo "pp" &> /dev/null ; then
     break
  fi
  printf "."
  sleep 10
done
echo ""
echo "INFO: Instance initialization done."

if aws ec2 create-tags \
  --resources ${instance_id} \
  --tags Key=Name,Value="${USER}_create_sprint_image" &> /dev/null ; then
  echo "INFO: Passed instance tag creation."
fi

if scp -i ${aws_key} \
  -o StrictHostKeyChecking=no \
  ${tarball} \
  centos@${instance_ip}:merlin-package.tgz &> /dev/null ; then
  echo "INFO: Passed merlin image package upload."
fi

if scp -i ${aws_key} \
  -o StrictHostKeyChecking=no \
  $license centos@${instance_ip}:license.lic &> /dev/null ; then
  echo "INFO: Passed Falcon license upload."
fi

if scp -i ${aws_key} \
  -o StrictHostKeyChecking=no \
  ${script_dir}/setup_ami_aws.sh \
  centos@${instance_ip}:setup_ami_aws.sh &> /dev/null ; then
  echo "INFO: Passed execution script upload."
fi

for step in 0 1 2
  do
  if ssh -i ${aws_key} -o StrictHostKeyChecking=no \
       centos@${instance_ip} bash setup_ami_aws.sh ; then
    if [ $step -eq 2 ] ; then
      break
    fi
  fi
  
  echo "INFO: Waiting for reboot..."
  while true ; do
    if ssh -i ${aws_key} -o StrictHostKeyChecking=no \
         centos@${instance_ip} echo "P" &> /dev/null ; then
      break
    fi
    printf "."
    sleep 10
  done
  echo ""
  echo "INFO: Reboot done."
done

if ssh -i ${aws_key} -o StrictHostKeyChecking=no \
     centos@${instance_ip} rm setup_ami_aws.sh ; then
  echo "INFO: Passed remove execution script."
fi

if ! aws ec2 create-image \
       --instance-id ${instance_id} \
       --name ${git_revision} \
       --description "${ami_description}" \
       --output json > aws_image.info; then
  echo "ERROR: Image creation failed"
  exit 1
else
  echo "INFO: Waiting for Image creation..."
  image_id=`jq '."ImageId"' aws_image.info | sed -e 's/^"//' -e 's/"$//'`
  while true; do
    aws ec2 describe-images --image-ids $image_id \
      --output json > aws_image.info
    image_state=`jq '."Images"[0]."State"' aws_image.info | sed -e 's/^"//' -e 's/"$//'`
    if [ "$image_state" = "available" ] ; then
      aws ec2 terminate-instances --instance-ids ${instance_id} &> /dev/null
      break
    fi
    printf "."
    sleep 10
  done
  echo ""
  echo "INFO: Image creation done."
fi

echo "INFO: Exiting.."
