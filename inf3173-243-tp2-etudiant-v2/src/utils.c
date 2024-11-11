#include "utils.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int split_string_internal(struct list* lst, char* str, const char* delim, int skip) {
  char* state;
  char* token;
  int n = 0;
  token = strtok_r(str, delim, &state);
  while (token) {
    if (n >= skip) {
      list_push_back(lst, list_node_new(strdup(token)));
    }
    token = strtok_r(NULL, delim, &state);
    n++;
  }
  return 0;
}

int split_string_from_file(struct list* lst, const char* path, const char* delim, int skip) {
  int ret = -1;
  // Allocation memoire
  struct stat s;
  if (stat(path, &s) < 0) {
    perror("stat");
    return -1;
  }
  char* buf = malloc(s.st_size);
  if (buf == NULL) {
    return -1;
  }
  int fd = open(path, O_RDONLY);
  if (fd < 0) {
    perror("open");
    goto out1;
  }
  if (read(fd, buf, s.st_size) < 0) {
    perror("read");
    goto out2;
  }
  split_string_internal(lst, buf, delim, skip);
  ret = 0;
out2:
  close(fd);
out1:
  free(buf);
  return ret;
}

int split_string(struct list* lst, const char* str, const char* delim, int skip) {
  char* work = strdup(str);
  int ret = split_string_internal(lst, work, delim, skip);
  free(work);
  return ret;
}

int timespec_cmp(const struct timespec* t1, const struct timespec* t2) {
  // Comparaison des secondes
  __time_t delta_sec = t1->tv_sec - t2->tv_sec;
  __time_t delta_nsec = t1->tv_nsec - t2->tv_nsec;

  if (delta_sec == 0) {
    if (delta_nsec == 0) {
      return 0;
    }
    return delta_nsec;
  }
  return delta_sec;
}
