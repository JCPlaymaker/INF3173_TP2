#define _GNU_SOURCE
#include "wrapper.h"

#include <dlfcn.h>

static int fread_cnt;
static int fwrite_cnt;
static int read_cnt;
static int write_cnt;
static int mmap_cnt;
static int munmap_cnt;
static int fread_size_max;
static int read_size_max;
static int write_size_max;
static int pipe_cnt;

static size_t (*fread_func)(void* ptr, size_t size, size_t nmemb, FILE* stream);
static size_t (*fwrite_func)(const void* ptr, size_t size, size_t nmemb, FILE* stream);
static ssize_t (*read_func)(int fd, void* ptr, size_t size);
static ssize_t (*write_func)(int fd, const void* ptr, size_t size);
static ssize_t (*mmap_func)(void *addr, size_t len, int prot, int flags, int fd, off_t off);
static ssize_t (*munmap_func)(void* ptr, size_t size);
static int (*pipe_func)(int fd[2]);

size_t fread(void* ptr, size_t size, size_t nmemb, FILE* stream) {
  fread_cnt++;
  if (size*nmemb > fread_size_max) {
    fread_size_max = size*nmemb;
  }
  if (!fread_func) {
    fread_func = dlsym(RTLD_NEXT, "fread");
  }
  return fread_func(ptr, size, nmemb, stream);
}

size_t fwrite(const void* ptr, size_t size, size_t nmemb, FILE* stream) {
  fwrite_cnt++;

  if (!fwrite_func) {
    fwrite_func = dlsym(RTLD_NEXT, "fwrite");
  }

  return fwrite_func(ptr, size, nmemb, stream);
}

ssize_t read(int fd, void* ptr, size_t size) {
  read_cnt++;
  if (size > read_size_max) {
    read_size_max = size;
  }

  if (!read_func) {
    read_func = dlsym(RTLD_NEXT, "read");
  }

  return read_func(fd, ptr, size);
}

ssize_t write(int fd, const void* ptr, size_t size) {
  write_cnt++;
  if (size > write_size_max) {
    write_size_max = size;
  }

  if (!write_func) {
    write_func = dlsym(RTLD_NEXT, "write");
  }

  return write_func(fd, ptr, size);
}

ssize_t mmap(void *addr, size_t len, int prot, int flags, int fd, off_t off) {
  mmap_cnt++;
  if (!mmap_func) {
    mmap_func = dlsym(RTLD_NEXT, "mmap");
  }

  return mmap_func(addr, len, prot, flags, fd, off);
}

ssize_t munmap(void* ptr, size_t size) {
  munmap_cnt++;
  if (!munmap_func) {
    munmap_func = dlsym(RTLD_NEXT, "munmap");
  }

  return munmap_func(ptr, size);
}

int pipe(int fd[2]) {
  pipe_cnt++;
  if (!pipe_func) {
    pipe_func = dlsym(RTLD_NEXT, "pipe");
  }
  return pipe_func(fd);
}

void wrapper_clear() {
  fread_cnt = 0;
  fwrite_cnt = 0;
  read_cnt = 0;
  write_cnt = 0;
  mmap_cnt = 0;
  munmap_cnt = 0;
  fread_size_max = 0;
  read_size_max = 0;
  write_size_max = 0;
  pipe_cnt = 0;
}

int wrapper_fread_count() {
  return fread_cnt;
}
int wrapper_fwrite_count() {
  return fwrite_cnt;
}
int wrapper_read_count() {
  return read_cnt;
}
int wrapper_write_count() {
  return write_cnt;
}
int wrapper_mmap_count() {
  return mmap_cnt;
}
int wrapper_munmap_count() {
  return munmap_cnt;
}
int wrapper_fread_size_max() {
  return fread_size_max;
}
int wrapper_read_size_max() {
  return read_size_max;
}
int wrapper_write_size_max() {
  return write_size_max;
}
int wrapper_pipe_count() {
  return pipe_cnt;
}
