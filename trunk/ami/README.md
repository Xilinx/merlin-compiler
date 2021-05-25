# Instruction on creating Merlin AMI

1. Check if you have correct AWS configuration by `aws configure`.
2. You need to have Falcon license file looks like:
```
FEATURE FALCON_CUSTOM falconlm 1.000 18-nov-2019 uncounted HOSTID=ANY \
    SIGN="0651 7163 353B 2EA6 FA3F 6BB8 037F 83DB BE61 89E1 4909
    ...
```

3. And a AWS login key file `user.pem`.
4. Build the Merlin release docker tarball `merlin-package.tgz`. (do it in the `build` folder)
5. The whole process takes about 30 mins.

Example:
```
cd Merlin_Compiler/trunk/ami
./generate_ami.sh -k ~/user.pem -l ~/license.lic -t Merlin_sprint_xx -b ../build/merlin-package.tgz
```

You will see
```
INFO: Set AWS login key
INFO: Set Falcon license
INFO: Set AMI name
...
...
...
INFO: Image creation done.
INFO: Exiting..
```
Then you can check the AMI in AWS console.

Note:
If you don't want to build Merlin manually, you can use this script to build by not passing `-b` option.
