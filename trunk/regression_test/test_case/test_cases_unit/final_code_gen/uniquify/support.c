//(C) Copyright 2016-2021 Xilinx, Inc.
//All Rights Reserved.
//
//Licensed to the Apache Software Foundation (ASF) under one
//or more contributor license agreements.  See the NOTICE file
//distributed with this work for additional information
//regarding copyright ownership.  The ASF licenses this file
//to you under the Apache License, Version 2.0 (the
//"License"); you may not use this file except in compliance
//with the License.  You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
//Unless required by applicable law or agreed to in writing,
//software distributed under the License is distributed on an
//"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
//KIND, either express or implied.  See the License for the
//specific language governing permissions and limitations
//under the License. (edited)
#include "support.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
// In general, fd_printf is used for individual values.
#define SUFFICIENT_SPRINTF_SPACE 256
// It'd be nice if dprintf was c99. But it ain't.

inline static int fd_printf(int fd,const char *format,... )
{
  __builtin_va_list args;
  int buffered;
  int written;
  int status;
  char buffer[256];
  __builtin_va_start(args,format);
  buffered = vsnprintf(buffer,256,format,args);
  __builtin_va_end(args);
  buffered < 256 && "Overran fd_printf buffer---output possibly corrupt"?((void )0) : __assert_fail("buffered<256 && \"Overran fd_printf buffer---output possibly corrupt\"","support.c",22,__PRETTY_FUNCTION__);
  written = 0;
  while(written < buffered){
    status = (write(fd,(&buffer[written]),(buffered - written)));
    status >= 0 && "Write failed"?((void )0) : __assert_fail("status>=0 && \"Write failed\"","support.c",26,__PRETTY_FUNCTION__);
    written += status;
  }
  written == buffered && "Wrote more data than given"?((void )0) : __assert_fail("written==buffered && \"Wrote more data than given\"","support.c",29,__PRETTY_FUNCTION__);
  return written;
}
///// File and section functions

char *readfile(int fd)
{
  char *p;
  struct stat s;
  long len;
  long bytes_read;
  long status;
  fd > 1 && "Invalid file descriptor"?((void )0) : __assert_fail("fd>1 && \"Invalid file descriptor\"","support.c",40,__PRETTY_FUNCTION__);
  0 == fstat(fd,&s) && "Couldn't determine file size"?((void )0) : __assert_fail("0==fstat(fd, &s) && \"Couldn't determine file size\"","support.c",41,__PRETTY_FUNCTION__);
  len = s . st_size;
  len > 0 && "File is empty"?((void )0) : __assert_fail("len>0 && \"File is empty\"","support.c",43,__PRETTY_FUNCTION__);
  p = ((char *)(malloc((len + 1))));
  bytes_read = 0;
  while(bytes_read < len){
    status = read(fd,(&p[bytes_read]),(len - bytes_read));
    status >= 0 && "read() failed"?((void )0) : __assert_fail("status>=0 && \"read() failed\"","support.c",48,__PRETTY_FUNCTION__);
    bytes_read += status;
  }
// Add NULL terminator
  p[len] = ((char )0);
  close(fd);
  return p;
}

char *find_section_start(char *s,int n)
{
  int i = 0;
  n >= 0 && "Invalid section number"?((void )0) : __assert_fail("n>=0 && \"Invalid section number\"","support.c",59,__PRETTY_FUNCTION__);
  if (n == 0) 
    return s;
// Find the nth "%%\n" substring (if *s==0, there wasn't one)
  while(i < n && ( *s) != ((char )0)){
// This comparison will short-circuit before overrunning the string, so no length check.
    if (s[0] == '%' && s[1] == '%' && s[2] == '\n') {
      i++;
    }
    s++;
  }
  if (( *s) != ((char )0)) 
// Skip the section header itself, return pointer to the content
    return s + 2;
// Hit the end, return an empty string
  return s;
}
///// Array read functions

int parse_string(char *s,char *arr,int n)
{
  int k;
  s != ((void *)0) && "Invalid input string"?((void )0) : __assert_fail("s!=((void *)0) && \"Invalid input string\"","support.c",79,__PRETTY_FUNCTION__);
// terminated string
  if (n < 0) {
    k = 0;
    while(s[k] != ((char )0) && s[k + 1] != ((char )0) && s[k + 2] != ((char )0) && !(s[k] == '\n' && s[k + 1] == '%' && s[k + 2] == '%')){
      k++;
    }
// fixed-length string
  }
   else {
    k = n;
  }
  memcpy(arr,s,k);
  if (n < 0) 
    arr[k] = 0;
  return 0;
}
#define generate_parse_TYPE_array(TYPE,STRTOTYPE) int parse_ ##TYPE ##_array(char *s, TYPE *arr, int n) { char *line, *endptr; int i=0; TYPE v; assert(s!=NULL && "Invalid input string"); line = strtok(s,"\n"); while( line!=NULL && i<n ) { endptr = line; v = (TYPE)(STRTOTYPE(line, &endptr)); if( (*endptr)!=(char)0 ) { fprintf(stderr, "Invalid input: line %d of section\n", i); } arr[i] = v; i++; line[strlen(line)] = '\n'; line = strtok(NULL,"\n"); } if(line!=NULL) { line[strlen(line)] = '\n'; } return 0; }
#define strtol_10(a,b) strtol(a,b,10)

int parse_uint8_t_array(char *s,unsigned char *arr,int n)
{
  char *line;
  char *endptr;
  int i = 0;
  unsigned char v;
  s != ((void *)0) && "Invalid input string"?((void )0) : __assert_fail("s!=((void *)0) && \"Invalid input string\"","support.c",132,__PRETTY_FUNCTION__);
  line = strtok(s,"\n");
  while(line != ((void *)0) && i < n){
    endptr = line;
    v = ((unsigned char )(strtol(line,&endptr,10)));
    if (( *endptr) != ((char )0)) {
      fprintf(stderr,"Invalid input: line %d of section\n",i);
    }
    arr[i] = v;
    i++;
    line[strlen(line)] = '\n';
    line = strtok(((void *)0),"\n");
  }
  if (line != ((void *)0)) {
    line[strlen(line)] = '\n';
  }
  return 0;
}

int parse_uint16_t_array(char *s,unsigned short *arr,int n)
{
  char *line;
  char *endptr;
  int i = 0;
  unsigned short v;
  s != ((void *)0) && "Invalid input string"?((void )0) : __assert_fail("s!=((void *)0) && \"Invalid input string\"","support.c",133,__PRETTY_FUNCTION__);
  line = strtok(s,"\n");
  while(line != ((void *)0) && i < n){
    endptr = line;
    v = ((unsigned short )(strtol(line,&endptr,10)));
    if (( *endptr) != ((char )0)) {
      fprintf(stderr,"Invalid input: line %d of section\n",i);
    }
    arr[i] = v;
    i++;
    line[strlen(line)] = '\n';
    line = strtok(((void *)0),"\n");
  }
  if (line != ((void *)0)) {
    line[strlen(line)] = '\n';
  }
  return 0;
}

int parse_uint32_t_array(char *s,unsigned int *arr,int n)
{
  char *line;
  char *endptr;
  int i = 0;
  unsigned int v;
  s != ((void *)0) && "Invalid input string"?((void )0) : __assert_fail("s!=((void *)0) && \"Invalid input string\"","support.c",134,__PRETTY_FUNCTION__);
  line = strtok(s,"\n");
  while(line != ((void *)0) && i < n){
    endptr = line;
    v = ((unsigned int )(strtol(line,&endptr,10)));
    if (( *endptr) != ((char )0)) {
      fprintf(stderr,"Invalid input: line %d of section\n",i);
    }
    arr[i] = v;
    i++;
    line[strlen(line)] = '\n';
    line = strtok(((void *)0),"\n");
  }
  if (line != ((void *)0)) {
    line[strlen(line)] = '\n';
  }
  return 0;
}

int parse_uint64_t_array(char *s,unsigned long *arr,int n)
{
  char *line;
  char *endptr;
  int i = 0;
  unsigned long v;
  s != ((void *)0) && "Invalid input string"?((void )0) : __assert_fail("s!=((void *)0) && \"Invalid input string\"","support.c",135,__PRETTY_FUNCTION__);
  line = strtok(s,"\n");
  while(line != ((void *)0) && i < n){
    endptr = line;
    v = ((unsigned long )(strtol(line,&endptr,10)));
    if (( *endptr) != ((char )0)) {
      fprintf(stderr,"Invalid input: line %d of section\n",i);
    }
    arr[i] = v;
    i++;
    line[strlen(line)] = '\n';
    line = strtok(((void *)0),"\n");
  }
  if (line != ((void *)0)) {
    line[strlen(line)] = '\n';
  }
  return 0;
}

int parse_int8_t_array(char *s,signed char *arr,int n)
{
  char *line;
  char *endptr;
  int i = 0;
  signed char v;
  s != ((void *)0) && "Invalid input string"?((void )0) : __assert_fail("s!=((void *)0) && \"Invalid input string\"","support.c",136,__PRETTY_FUNCTION__);
  line = strtok(s,"\n");
  while(line != ((void *)0) && i < n){
    endptr = line;
    v = ((signed char )(strtol(line,&endptr,10)));
    if (( *endptr) != ((char )0)) {
      fprintf(stderr,"Invalid input: line %d of section\n",i);
    }
    arr[i] = v;
    i++;
    line[strlen(line)] = '\n';
    line = strtok(((void *)0),"\n");
  }
  if (line != ((void *)0)) {
    line[strlen(line)] = '\n';
  }
  return 0;
}

int parse_int16_t_array(char *s,short *arr,int n)
{
  char *line;
  char *endptr;
  int i = 0;
  short v;
  s != ((void *)0) && "Invalid input string"?((void )0) : __assert_fail("s!=((void *)0) && \"Invalid input string\"","support.c",137,__PRETTY_FUNCTION__);
  line = strtok(s,"\n");
  while(line != ((void *)0) && i < n){
    endptr = line;
    v = ((short )(strtol(line,&endptr,10)));
    if (( *endptr) != ((char )0)) {
      fprintf(stderr,"Invalid input: line %d of section\n",i);
    }
    arr[i] = v;
    i++;
    line[strlen(line)] = '\n';
    line = strtok(((void *)0),"\n");
  }
  if (line != ((void *)0)) {
    line[strlen(line)] = '\n';
  }
  return 0;
}

int parse_int32_t_array(char *s,int *arr,int n)
{
  char *line;
  char *endptr;
  int i = 0;
  int v;
  s != ((void *)0) && "Invalid input string"?((void )0) : __assert_fail("s!=((void *)0) && \"Invalid input string\"","support.c",138,__PRETTY_FUNCTION__);
  line = strtok(s,"\n");
  while(line != ((void *)0) && i < n){
    endptr = line;
    v = ((int )(strtol(line,&endptr,10)));
    if (( *endptr) != ((char )0)) {
      fprintf(stderr,"Invalid input: line %d of section\n",i);
    }
    arr[i] = v;
    i++;
    line[strlen(line)] = '\n';
    line = strtok(((void *)0),"\n");
  }
  if (line != ((void *)0)) {
    line[strlen(line)] = '\n';
  }
  return 0;
}

int parse_int64_t_array(char *s,long *arr,int n)
{
  char *line;
  char *endptr;
  int i = 0;
  long v;
  s != ((void *)0) && "Invalid input string"?((void )0) : __assert_fail("s!=((void *)0) && \"Invalid input string\"","support.c",139,__PRETTY_FUNCTION__);
  line = strtok(s,"\n");
  while(line != ((void *)0) && i < n){
    endptr = line;
    v = ((long )(strtol(line,&endptr,10)));
    if (( *endptr) != ((char )0)) {
      fprintf(stderr,"Invalid input: line %d of section\n",i);
    }
    arr[i] = v;
    i++;
    line[strlen(line)] = '\n';
    line = strtok(((void *)0),"\n");
  }
  if (line != ((void *)0)) {
    line[strlen(line)] = '\n';
  }
  return 0;
}

int parse_float_array(char *s,float *arr,int n)
{
  char *line;
  char *endptr;
  int i = 0;
  float v;
  s != ((void *)0) && "Invalid input string"?((void )0) : __assert_fail("s!=((void *)0) && \"Invalid input string\"","support.c",141,__PRETTY_FUNCTION__);
  line = strtok(s,"\n");
  while(line != ((void *)0) && i < n){
    endptr = line;
    v = ((float )(strtof(line,&endptr)));
    if (( *endptr) != ((char )0)) {
      fprintf(stderr,"Invalid input: line %d of section\n",i);
    }
    arr[i] = v;
    i++;
    line[strlen(line)] = '\n';
    line = strtok(((void *)0),"\n");
  }
  if (line != ((void *)0)) {
    line[strlen(line)] = '\n';
  }
  return 0;
}

int parse_double_array(char *s,double *arr,int n)
{
  char *line;
  char *endptr;
  int i = 0;
  double v;
  s != ((void *)0) && "Invalid input string"?((void )0) : __assert_fail("s!=((void *)0) && \"Invalid input string\"","support.c",142,__PRETTY_FUNCTION__);
  line = strtok(s,"\n");
  while(line != ((void *)0) && i < n){
    endptr = line;
    v = ((double )(strtod(line,&endptr)));
    if (( *endptr) != ((char )0)) {
      fprintf(stderr,"Invalid input: line %d of section\n",i);
    }
    arr[i] = v;
    i++;
    line[strlen(line)] = '\n';
    line = strtok(((void *)0),"\n");
  }
  if (line != ((void *)0)) {
    line[strlen(line)] = '\n';
  }
  return 0;
}
///// Array write functions

int write_string(int fd,char *arr,int n)
{
  int status;
  int written;
  fd > 1 && "Invalid file descriptor"?((void )0) : __assert_fail("fd>1 && \"Invalid file descriptor\"","support.c",147,__PRETTY_FUNCTION__);
// NULL-terminated string
  if (n < 0) {
    n = (strlen(arr));
  }
  written = 0;
  while(written < n){
    status = (write(fd,(&arr[written]),(n - written)));
    status >= 0 && "Write failed"?((void )0) : __assert_fail("status>=0 && \"Write failed\"","support.c",154,__PRETTY_FUNCTION__);
    written += status;
  }
// Write terminating '\n'
  do {
    status = (write(fd,"\n",1));
    status >= 0 && "Write failed"?((void )0) : __assert_fail("status>=0 && \"Write failed\"","support.c",160,__PRETTY_FUNCTION__);
  }while (status == 0);
  return 0;
}
// Not strictly necessary, but nice for future-proofing.
#define generate_write_TYPE_array(TYPE,FORMAT) int write_ ##TYPE ##_array(int fd, TYPE *arr, int n) { int i; assert(fd>1 && "Invalid file descriptor"); for( i=0; i<n; i++ ) { fd_printf(fd, "%" FORMAT "\n", arr[i]); } return 0; }

int write_uint8_t_array(int fd,unsigned char *arr,int n)
{
  int i;
  fd > 1 && "Invalid file descriptor"?((void )0) : __assert_fail("fd>1 && \"Invalid file descriptor\"","support.c",177,__PRETTY_FUNCTION__);
  for (i = 0; i < n; i++) {
    fd_printf(fd,"%u\n",arr[i]);
  }
  return 0;
}

int write_uint16_t_array(int fd,unsigned short *arr,int n)
{
  int i;
  fd > 1 && "Invalid file descriptor"?((void )0) : __assert_fail("fd>1 && \"Invalid file descriptor\"","support.c",178,__PRETTY_FUNCTION__);
  for (i = 0; i < n; i++) {
    fd_printf(fd,"%u\n",arr[i]);
  }
  return 0;
}

int write_uint32_t_array(int fd,unsigned int *arr,int n)
{
  int i;
  fd > 1 && "Invalid file descriptor"?((void )0) : __assert_fail("fd>1 && \"Invalid file descriptor\"","support.c",179,__PRETTY_FUNCTION__);
  for (i = 0; i < n; i++) {
    fd_printf(fd,"%u\n",arr[i]);
  }
  return 0;
}

int write_uint64_t_array(int fd,unsigned long *arr,int n)
{
  int i;
  fd > 1 && "Invalid file descriptor"?((void )0) : __assert_fail("fd>1 && \"Invalid file descriptor\"","support.c",180,__PRETTY_FUNCTION__);
  for (i = 0; i < n; i++) {
    fd_printf(fd,"%lu\n",arr[i]);
  }
  return 0;
}

int write_int8_t_array(int fd,signed char *arr,int n)
{
  int i;
  fd > 1 && "Invalid file descriptor"?((void )0) : __assert_fail("fd>1 && \"Invalid file descriptor\"","support.c",181,__PRETTY_FUNCTION__);
  for (i = 0; i < n; i++) {
    fd_printf(fd,"%d\n",arr[i]);
  }
  return 0;
}

int write_int16_t_array(int fd,short *arr,int n)
{
  int i;
  fd > 1 && "Invalid file descriptor"?((void )0) : __assert_fail("fd>1 && \"Invalid file descriptor\"","support.c",182,__PRETTY_FUNCTION__);
  for (i = 0; i < n; i++) {
    fd_printf(fd,"%d\n",arr[i]);
  }
  return 0;
}

int write_int32_t_array(int fd,int *arr,int n)
{
  int i;
  fd > 1 && "Invalid file descriptor"?((void )0) : __assert_fail("fd>1 && \"Invalid file descriptor\"","support.c",183,__PRETTY_FUNCTION__);
  for (i = 0; i < n; i++) {
    fd_printf(fd,"%d\n",arr[i]);
  }
  return 0;
}

int write_int64_t_array(int fd,long *arr,int n)
{
  int i;
  fd > 1 && "Invalid file descriptor"?((void )0) : __assert_fail("fd>1 && \"Invalid file descriptor\"","support.c",184,__PRETTY_FUNCTION__);
  for (i = 0; i < n; i++) {
    fd_printf(fd,"%ld\n",arr[i]);
  }
  return 0;
}

int write_float_array(int fd,float *arr,int n)
{
  int i;
  fd > 1 && "Invalid file descriptor"?((void )0) : __assert_fail("fd>1 && \"Invalid file descriptor\"","support.c",186,__PRETTY_FUNCTION__);
  for (i = 0; i < n; i++) {
    fd_printf(fd,"%.16f\n",arr[i]);
  }
  return 0;
}

int write_double_array(int fd,double *arr,int n)
{
  int i;
  fd > 1 && "Invalid file descriptor"?((void )0) : __assert_fail("fd>1 && \"Invalid file descriptor\"","support.c",187,__PRETTY_FUNCTION__);
  for (i = 0; i < n; i++) {
    fd_printf(fd,"%.16f\n",arr[i]);
  }
  return 0;
}

int write_section_header(int fd)
{
  fd > 1 && "Invalid file descriptor"?((void )0) : __assert_fail("fd>1 && \"Invalid file descriptor\"","support.c",190,__PRETTY_FUNCTION__);
// Just prints %%
  fd_printf(fd,"%%%%\n");
  return 0;
}
