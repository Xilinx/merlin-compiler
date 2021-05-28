// (C) Copyright 2016-2021 Xilinx, Inc.
// All Rights Reserved.
//
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.



#include "__merlin_parameter.h"
#include "__merlin_opencl_if.h"
#include "__merlin_opencl_kernel_buffer.h"
#define MEM_SIZE_LIMITATION (MERLIN_MEM_SIZE_LIMITATION_NUMBER)

static char hint_info[256] =
    "Please call __merlin_init_opencl(bitstream) before kernel call.\n";
static int count_init = 0;
cl_platform_id platform_id;            // platform id
cl_device_id device_id;                // compute device id
cl_context context;                    // compute context
cl_command_queue commands[KERNEL_NUM]; // compute command queue
cl_program program;                    // compute program
// cl_kernel kernel;                   // compute kernel

int load_file_to_memory(const char *filename, char **result) {
  size_t size = 0;
  FILE *f = fopen(filename, "rb");
  if (f == NULL) {
    printf("ERROR : Kernel binary %s not exist!\n", filename);
    *result = NULL;
    return -1; // -1 means file opening fail
  }
  fseek(f, 0, SEEK_END);
  size = ftell(f);
  fseek(f, 0, SEEK_SET);
  *result = (char *)malloc(size + 1);
  if (size != (int)fread(*result, sizeof(char), size, f)) {
    free(*result);
    return -2; // -2 means file reading fail
  }
  fclose(f);
  (*result)[size] = 0;
  return size;
}

int __merlin_get_platform_info() {
  char cl_platform_vendor[1001];
  char cl_platform_name[1001];
  cl_platform_vendor[0] = 0;
  cl_platform_name[0] = 0;
  int err;

  err = clGetPlatformIDs(1, &platform_id, NULL);
  if (err != CL_SUCCESS) {
    printf("Error: Failed to find an OpenCL platform!\n");
    printf("Error: code %i\n", err);
    exit(EXIT_FAILURE);
  }
  err = clGetPlatformInfo(platform_id, CL_PLATFORM_VENDOR, 1000,
                          (void *)cl_platform_vendor, NULL);
  if (err != CL_SUCCESS) {
    printf("Error: clGetPlatformInfo(CL_PLATFORM_VENDOR) failed!\n");
    printf("Error: code %i\n", err);
    exit(EXIT_FAILURE);
  }
  printf("CL_PLATFORM_VENDOR %s\n", cl_platform_vendor);
  err = clGetPlatformInfo(platform_id, CL_PLATFORM_NAME, 1000,
                          (void *)cl_platform_name, NULL);
  if (err != CL_SUCCESS) {
    printf("Error: clGetPlatformInfo(CL_PLATFORM_NAME) failed!\n");
    printf("Error: code %i\n", err);
    exit(EXIT_FAILURE);
  }
  printf("CL_PLATFORM_NAME %s\n", cl_platform_name);
  return CL_SUCCESS;
}

int __merlin_get_device_id() {
  int err;
  int fpga = 1;
#ifdef __MERLIN_AOCL
  err = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ALL, 1, &device_id, NULL);
#endif
#ifdef __MERLIN_SDACCEL
  err = clGetDeviceIDs(platform_id,
                       fpga ? CL_DEVICE_TYPE_ACCELERATOR : CL_DEVICE_TYPE_CPU,
                       1, &device_id, NULL);
#endif
  if (err != CL_SUCCESS) {
    printf("Error: Failed to create a device group!\n");
    printf("Error: code %i\n", err);
    exit(EXIT_FAILURE);
  }
  return CL_SUCCESS;
}

#ifdef __MERLIN_SDACCEL
int __merlin_release_device() {
  int err;
  err = clReleaseDevice(device_id);
  if (err != CL_SUCCESS) {
    printf("Error: Failed to release device!\n");
    printf("Error: code %i\n", err);
    exit(EXIT_FAILURE);
  }
  return CL_SUCCESS;
}
#endif

int __merlin_create_context() {
  int err;
  context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
  if (!context) {
    printf("Error: Failed to create a compute context!\n");
    printf("Error: code %i\n", err);
    exit(EXIT_FAILURE);
  }
  return CL_SUCCESS;
}

int __merlin_release_context() {
  int err;
  err = clReleaseContext(context);
  if (err != CL_SUCCESS) {
    printf("Error: Failed to release context!\n");
    printf("Error: code %i\n", err);
    exit(EXIT_FAILURE);
  }
  return CL_SUCCESS;
}

int check_context_return() {
  int err = 0;
  size_t deviceBufferSize = -1;
  err =
      clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &deviceBufferSize);
  if (CL_SUCCESS != err) {
    return 0;
  }
  if (0 == deviceBufferSize) {
    return 0;
  }
  return CL_SUCCESS;
}
int __merlin_check_context() {
  int err = 0;
  size_t deviceBufferSize = -1;
  err =
      clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &deviceBufferSize);
  if (CL_SUCCESS != err) {
    fprintf(stderr,
            "Failed call clGetContextInfo(..., CL_CONTEXT_DEVICES, ...)\n");
    fprintf(stderr, "%s\n", hint_info);
    exit(EXIT_FAILURE);
  }
  if (0 == deviceBufferSize) {
    fprintf(stderr, "No OpenCL devices available\n");
    exit(EXIT_FAILURE);
  }
  return CL_SUCCESS;
}

int __merlin_create_command_queue() {
  int err;
  int i;
  for (i = 0; i < KERNEL_NUM; i++) {
#ifdef __MERLIN_AOCL
    commands[i] = clCreateCommandQueue(context, device_id, 0, &err);
#endif
#ifdef __MERLIN_SDACCEL
    commands[i] = clCreateCommandQueue(
        context, device_id, CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, &err);
#endif
    if (!commands[i]) {
      printf("Error: Failed to create a command queue commands[%d]!\n", i);
      printf("Error: code %i\n", err);
      exit(EXIT_FAILURE);
    }
  }
  return CL_SUCCESS;
}

int __merlin_release_command_queue() {
  int err;
  int i;
  for (i = 0; i < KERNEL_NUM; i++) {
    err = clReleaseCommandQueue(commands[i]);
    if (err != CL_SUCCESS) {
      printf("Error: Failed to release a command queue commands[%d]!\n", i);
      printf("Error: code %i\n", err);
      exit(EXIT_FAILURE);
    }
  }
  return CL_SUCCESS;
}

int check_command_queue_return() {
  int err = 0;
  int i;
  for (i = 0; i < KERNEL_NUM; i++) {
    size_t Size = -1;
    err = clGetCommandQueueInfo(commands[i], CL_QUEUE_CONTEXT, 0, NULL, &Size);
    if (CL_SUCCESS != err) {
      return 0;
    }
  }
  return CL_SUCCESS;
}
int __merlin_check_command_queue() {
  int err = 0;
  int i;
  for (i = 0; i < KERNEL_NUM; i++) {
    size_t Size = -1;
    err = clGetCommandQueueInfo(commands[i], CL_QUEUE_CONTEXT, 0, NULL, &Size);
    if (CL_SUCCESS != err) {
      fprintf(
          stderr,
          "Failed call clGetCommandQueueInfo(..., CL_QUEUE_CONTEXT, ...)\n");
      fprintf(stderr, "%s\n", hint_info);
      exit(EXIT_FAILURE);
    }
  }
  return CL_SUCCESS;
}

int __merlin_create_program(const char *bitstream) {
  int err;
  int n_i = 0;
  unsigned char *kernelbinary;
  char *bit_file;
#ifdef __MERLIN_AOCL
  if (bitstream == NULL) {
    bit_file = (char *)"kernel_top.aocx";
  } else {
    bit_file = (char *)bitstream;
  }
#endif
#ifdef __MERLIN_SDACCEL
  if (bitstream == NULL) {
    bit_file = (char *)"kernel_top.xclbin";
  } else {
    bit_file = (char *)bitstream;
  }
#endif
  printf("loading %s\n", bit_file);
  n_i = load_file_to_memory(bit_file, (char **)&kernelbinary);
  if (n_i < 0) {
    printf("ERROR : failed to load kernel from binary.\n");
    exit(EXIT_FAILURE);
  }

  int status;
  size_t n = n_i;
  program = clCreateProgramWithBinary(context, 1, &device_id, &n,
                                      (const unsigned char **)&kernelbinary,
                                      &status, &err);
  if ((!program) || (err != CL_SUCCESS)) {
    printf("Error: Failed to create compute program from binary %d!\n", err);
    printf("Error: code %i\n", err);
    exit(EXIT_FAILURE);
  }

  // Build the program executable
  err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
  if (err != CL_SUCCESS) {
    size_t len;
    char buffer[2048];

    printf("Error: Failed to build program executable!\n");
    clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG,
                          sizeof(buffer), buffer, &len);
    printf("%s\n", buffer);
    exit(EXIT_FAILURE);
  }
  return CL_SUCCESS;
}

int __merlin_release_program() {
  int err;
  err = clReleaseProgram(program);
  if (err != CL_SUCCESS) {
    printf("Error: Failed to release program!\n");
    printf("Error: code %i\n", err);
    exit(EXIT_FAILURE);
  }
  return CL_SUCCESS;
}

int check_program_return() {
  int err = 0;
  {
    size_t Size = -1;
    err = clGetProgramInfo(program, CL_PROGRAM_CONTEXT, 0, NULL, &Size);
    if (CL_SUCCESS != err) {
      return 0;
    }
  }
  return CL_SUCCESS;
}
int __merlin_check_program() {
  int err = 0;
  {
    size_t Size = -1;
    err = clGetProgramInfo(program, CL_PROGRAM_CONTEXT, 0, NULL, &Size);
    if (CL_SUCCESS != err) {
      fprintf(stderr,
              "Failed call clGetProgramInfo(..., CL_PROGRAM_CONTEXT, ...)\n");
      fprintf(stderr, "%s\n", hint_info);
      exit(EXIT_FAILURE);
    }
  }
  return CL_SUCCESS;
}

int opencl_init(const char *bitstream) {
  if (count_init == 0) {
    __merlin_get_platform_info();
    __merlin_get_device_id();
    __merlin_create_context();
    __merlin_create_command_queue();
    __merlin_create_program(bitstream);
    opencl_init_kernel_buffer();
    count_init++;
    return 1;
  } else {
    return 0;
  }
}

int check_opencl_return() {
  if(check_context_return() && check_command_queue_return()
     && check_program_return()) {
    return 1;
  } else {
    return 0;
  }
}


int __merlin_init(const char *bitstream) {
  __merlin_init_opencl(bitstream);
  return CL_SUCCESS;
}

int __merlin_init_opencl(const char *bitstream) {
  if (count_init == 0 || check_opencl_return() == 0) {
    __merlin_get_platform_info();
    __merlin_get_device_id();
    __merlin_create_context();
    __merlin_create_command_queue();
    __merlin_create_program(bitstream);
    count_init++;
    return 1;
  } else {
    return 0;
  }
}

int __merlin_release_opencl() {
#ifdef __MERLIN_SDACCEL
  __merlin_release_device();
#endif
  __merlin_release_context();
  __merlin_release_command_queue();
#ifdef __MERLIN_SDACCEL
  __merlin_release_program();
#endif
  return CL_SUCCESS;
}

int __merlin_check_opencl() {
  __merlin_check_context();
  __merlin_check_command_queue();
  __merlin_check_program();
  return CL_SUCCESS;
}

int __merlin_release() {
  __merlin_release_opencl();
  //    __merlin_release_kernel_buffer();
  return CL_SUCCESS;
}

int opencl_create_kernel(cl_kernel *kernel, char *kernel_name) {
  int err;
  *kernel = clCreateKernel(program, kernel_name, &err);
  if (!(*kernel) || err != CL_SUCCESS) {
    printf("Error: Failed to create compute kernel!\n");
    printf("Error: code %i\n", err);
    exit(EXIT_FAILURE);
  }
  return CL_SUCCESS;
}

int opencl_create_buffer(cl_mem *cl_buffer, long long size, int type, void * migrate_data) {
  // use --attribute mem_size_limitation=4GB to config MEM_SIZE_LIMITATION
  if (size >= MEM_SIZE_LIMITATION) {
    printf("ERROR: Failed to allocate device memory:%lldB needed "
           "(MERLIN_MEM_SIZE_LIMITATION_STRING available).\n",
           size);
    printf("\tuse --attribute mem_size_limitation=?GB to set.\n");
    exit(EXIT_FAILURE);
  }

#ifdef __MERLIN_SDACCEL
  cl_mem_ext_ptr_t * p = NULL;
  cl_mem_ext_ptr_t mem_ext_t = {XCL_MEM_DDR_BANK0, migrate_data, 0}; 
  if (migrate_data != NULL) {
    //cl_mem_ext_ptr_t mem_ext_t = {XBANK(0), migrate_data, 0}; 
    p = &mem_ext_t;
  }
#endif

  size_t flag = 0;
  if (type == 0)
    flag = CL_MEM_READ_ONLY;
  if (type == 1)
    flag = CL_MEM_WRITE_ONLY;
  if (type == 2)
    flag = CL_MEM_READ_WRITE;

#ifdef __MERLIN_SDACCEL
  if (migrate_data != NULL) {
    flag |= CL_MEM_EXT_PTR_XILINX|CL_MEM_USE_HOST_PTR;
  }


  *cl_buffer = clCreateBuffer(context, flag, size, p, NULL);
#else
  *cl_buffer = clCreateBuffer(context, flag, size, NULL, NULL);
#endif
  if (!(*cl_buffer)) {
    printf("Error: Failed to allocate device memory!\n");
    exit(EXIT_FAILURE);
  }
  return CL_SUCCESS;
}

#ifdef __MERLIN_SDACCEL
int opencl_create_ext_buffer(cl_mem *cl_buffer, cl_mem_ext_ptr_t *cl_buffer_ext,
                             long long size, int type) {
  if (size >= (1UL << 32)) {
    printf("ERROR: Failed to allocate device memory:%lldB needed (4 GB "
           "available).\n",
           size);
    exit(EXIT_FAILURE);
  }
  size_t flag = 0;
  if (type == 0)
    flag = CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX;
  if (type == 1)
    flag = CL_MEM_WRITE_ONLY | CL_MEM_EXT_PTR_XILINX;
  if (type == 2)
    flag = CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX;
  *cl_buffer = clCreateBuffer(context, flag, size, cl_buffer_ext, NULL);
  if (!(*cl_buffer)) {
    printf("Error: Failed to allocate device memory!\n");
    exit(EXIT_FAILURE);
  }
  return CL_SUCCESS;
}
#endif

int opencl_write_buffer(cl_mem cl_buffer, cl_command_queue commands,
                        long long offset, void *host_buffer, long long size) {
  if (offset < 0 || offset + size - 1 >= (1UL << 32)) {
    printf("ERROR: offset or size overflow: offset=%lld size=%lld\n", offset,
           size);
    exit(EXIT_FAILURE);
  }
  cl_event event;
  int err = clEnqueueWriteBuffer(commands, cl_buffer, CL_FALSE, offset, size,
                                 host_buffer, 0, NULL, &event);
  clWaitForEvents(1, &event);
  clReleaseEvent(event);
  if (err != CL_SUCCESS) {
    printf("Error: Failed to write to device buffer!\n");
    printf("Error: code %i\n", err);
    exit(EXIT_FAILURE);
  }
  return err;
}

int opencl_read_buffer(cl_mem cl_buffer, cl_command_queue commands,
                       long long offset, void *host_buffer, long long size) {
  if (offset < 0 || offset + size - 1 >= (1UL << 32)) {
    printf("ERROR: offset or size overflow: offset=%lld size=%lld\n", offset,
           size);
    exit(EXIT_FAILURE);
  }
  cl_event readevent;
  int err = clEnqueueReadBuffer(commands, cl_buffer, CL_FALSE, offset, size,
                                host_buffer, 0, NULL, &readevent);
  clWaitForEvents(1, &readevent);
  clReleaseEvent(readevent);
  if (err != CL_SUCCESS) {
    printf("Error: Failed to read from device buffer!\n");
    printf("Error: code %i\n", err);
    exit(EXIT_FAILURE);
  }
  return err;
}

int opencl_migrate_buffer(int num_bufs, cl_mem* cl_bufs, cl_command_queue commands, int direction) {

  cl_event cl_ev;
  int err = clEnqueueMigrateMemObjects(commands, num_bufs, cl_bufs,
          direction==1?0:CL_MIGRATE_MEM_OBJECT_HOST, 0, NULL, &cl_ev);
  clWaitForEvents(1, &cl_ev);
  clReleaseEvent(cl_ev);
  if (err != CL_SUCCESS) {
    printf("Error: Failed to migrate device buffer!\n");
    printf("Error: code %i\n", err);
    exit(EXIT_FAILURE);
  }

  return err;
}

int opencl_set_kernel_arg(cl_kernel kernel, int index, size_t size,
                          const void *content) {
  return clSetKernelArg(kernel, index, size, content);
}

int opencl_enqueue_kernel(cl_kernel kernel, cl_command_queue commands, int dim,
                          size_t global_in[], cl_event *event_out) {
  int i;
  size_t global[100], local[100];
  for (i = 0; i < dim; i++) {
    global[i] = global_in[i];
    local[i] = 1;
  }
  //    cl_event event;
  //    int err = clEnqueueNDRangeKernel(commands, kernel, dim, NULL,
  //    (size_t*)&global, (size_t*)&local, 0, NULL, &event); clWaitForEvents(1,
  //    &event);
  int err =
      clEnqueueNDRangeKernel(commands, kernel, dim, NULL, (size_t *)&global,
                             (size_t *)&local, 0, NULL, event_out);
  return err;
}

int opencl_enqueue_task(cl_kernel kernel, cl_command_queue commands) {
  cl_event event;
  int err = clEnqueueTask(commands, kernel, 0, NULL, &event);
  clWaitForEvents(1, &event);
  return err;
}

int opencl_flush(cl_command_queue commands) { return clFlush(commands); }

int opencl_write_buffer_nb(cl_mem cl_buffer, cl_command_queue commands,
                           long long offset, void *host_buffer, long long size,
                           cl_event *event) {
  if (offset < 0 || offset + size - 1 >= (1UL << 32)) {
    printf("ERROR: offset or size overflow: offset=%lld size=%lld\n", offset,
           size);
    exit(EXIT_FAILURE);
  }
  int err = clEnqueueWriteBuffer(commands, cl_buffer, CL_FALSE, offset, size,
                                 host_buffer, 0, NULL, event);
  if (err != CL_SUCCESS) {
    printf("Error: Failed to write to device buffer!\n");
    printf("Error: code %i\n", err);
    exit(EXIT_FAILURE);
  }
  return err;
}

int opencl_read_buffer_nb(cl_mem cl_buffer, cl_command_queue commands,
                          long long offset, void *host_buffer, long long size,
                          cl_event *event) {
  if (offset < 0 || offset + size - 1 >= (1UL << 32)) {
    printf("ERROR: offset or size overflow: offset=%lld size=%lld\n", offset,
           size);
    exit(EXIT_FAILURE);
  }
  int err = clEnqueueReadBuffer(commands, cl_buffer, CL_FALSE, offset, size,
                                host_buffer, 0, NULL, event);
  if (err != CL_SUCCESS) {
    printf("Error: Failed to read from device buffer!\n");
    printf("Error: code %i\n", err);
    exit(EXIT_FAILURE);
  }
  return err;
}

int opencl_enqueue_kernel_nb(cl_kernel kernel, cl_command_queue commands,
                             int dim, size_t global_in[], cl_event *event_out) {
  int i;
  size_t global[100], local[100];
  for (i = 0; i < dim; i++) {
    global[i] = global_in[i];
    local[i] = 1;
  }
  //    cl_event event;
  //    int err = clEnqueueNDRangeKernel(commands, kernel, dim, NULL,
  //    (size_t*)&global, (size_t*)&local, 0, NULL, &event); clWaitForEvents(1,
  //    &event);
  int err =
      clEnqueueNDRangeKernel(commands, kernel, dim, NULL, (size_t *)&global,
                             (size_t *)&local, 0, NULL, event_out);
  return err;
}

int opencl_wait_event(cl_event event) {
  int err;
  err = clWaitForEvents(1, &event);
  if (err != CL_SUCCESS) {
    printf("Error: Failed to wait event!\n");
    printf("Error: code %i\n", err);
    exit(EXIT_FAILURE);
  }
  return err;
}

int opencl_release_event(cl_event event) {
  int err;
  err = clReleaseEvent(event);
  return err;
}

int opencl_release_mem_object(cl_mem mem) {
  int err;
  err = clReleaseMemObject(mem);
  if (err != CL_SUCCESS) {
    printf("Error: Failed to release event!\n");
    printf("Error: code %i\n", err);
    exit(EXIT_FAILURE);
  }
  return err;
}

int opencl_release_kernel(cl_kernel kernel) {
  int err;
  err = clReleaseKernel(kernel);
  if (err != CL_SUCCESS) {
    printf("Error: Failed to release kernel!\n");
    printf("Error: code %i\n", err);
    exit(EXIT_FAILURE);
  }
  return err;
}
