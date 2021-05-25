  #   Prerequisite 
### System Requirements
64-bit Linux with at least 32GB memory and Docker installed. 

### Docker Environment
1. Install Docker  
    Merlin Compiler has been tested with Docker CE version *1.13.1* that comes with RHEL. 
    For complete instruction on installing Docker please refer to [https://docs.docker.com/install/].

2. Setup `docker` group  
    Our scripts assume running docker without `sudo`, therefore we need to make sure the current user belongs to the `docker` group. Please refer to [https://docs.docker.com/install/linux/linux-postinstall/] for more details.
    
3. Verify  
    To verify if the setting is correct, the following command can be used to check if there are any error messages:
    ```
    > docker ps
    ```

### Vendor Development & Runtime Setup
Vendor development tools and runtime are required to use Merlin to target platforms based on their devices. For Xilinx, SDAccel or Vitis and matching XRT need to be installed. For Intel, FPGA SDK for OpenCL needs to be installed. Please follow respective vendor instructions to obtain tool license and install their software and supported FPGA platforms.
 
For Xilinx support, the following environment variables need to point at the specific installation folders. 
- `XILINX_SDX` or `XILINX_VITIS`
- `XILINX_VIVADO`
- `XILINX_XRT`

In addition, we infer `XILINX_SDK` based on the default folder structure for Xilinx tools installation:
- `$XILINX_ROOT/`:
   - SDx
   - SDK
   - ..

If Xilinx SDK is installed to a different folder, the env variable `XILINX_SDK` needs to be set accordingly.
License management for Xilinx tools needs to be configured as well. 

### Merlin Installation
1. Install License  
Only floating licenses are supported for Merlin Compiler distribution. 
   * Find the MAC address and request for an license
     ```
     > ./license/lin64/lmutil lmhostid
     ```
   * Open the license file and replace *hostname*, *port* and *merlin_root* to reflect your installation  
     SERVER hostname hostid port  
     VENDOR falconlm merlin_root/license/lin64/falconlm  
   * Start the Falcon license manager and add port@hostname into `$LM_LICENSE_FILE`
     ```
     > ./license/lin64/lmgrd -c <license-file>  -l lmgrd.log
     ```
   * Common license setup issues & solutions
     * Error 'No such file or directory'
       ```
       > ./lmutil
       > -bash: ./lmutil: No such file or directory
       ```
       Please run the following command:
       ```
       > sudo apt-get install lsb-core
       ```
     * Error 'checkout failed: Cannot connect to license server system.' 

       Use command `lmutil lmstat -c port@hostname` to check if you can access the license server. If the server is fine, you can try to add `port@host-ip` to `$LM_LICENSE_FILE` where `host-ip` is the IP address of the host.

2. Install package
```
> ./install.sh
```

3. Environment Setup

Add `./bin` to `PATH`. You can also run:
```
> source ./setup.sh
```

### Usage
Merlin Compiler can be called via `merlincc` command:
```
> merlincc -v
```

Merlin Compiler core is a docker container. It will mount the root of the current directory (`pwd`) to the container and use it as the working directory. Therefore, there are several additional requirements for the command:
- All the files needed by the command should present in the same folder tree. For example, the following works:
   ```
   [/home/user/project] ls src/
   code.cpp
   [/home/user/project] ls include/
   code.h
   [/home/user/project] merlincc -c ./src/code.cpp -I./include -o code_acc
   ```
   But the following may not work:
   ```
   [/home/user/project] merlincc -c ./src/code.cpp -I./include -I/opt/somelibrary/include -o code_acc
   ```
   Because inside the container the folder `/opt/somelibrary/include` may not exist.
- Docker needs root permission to mount a folder. Therefore, if the current folder is a NFS mount that squashes root permission, Merlin may not work properly. 

   
