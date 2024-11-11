#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "finddup_main.h"

void print_usage() {
  fprintf(stderr,
          "Usage: %s "
          "-m [ simple | fread | read | mmap ] "
          "-b block-size "
          "-p pattern "
          "--help]\n",
          "finddup");
  exit(1);
}

int main(int argc, char** argv) {
  int ret = EXIT_FAILURE;

  struct option options[] = {
      {"block-size", 1, 0, 'b'},  //
      {"method", 1, 0, 'm'},      //
      {"pattern", 1, 0, 'p'},     //
      {"help", 0, 0, 'h'},        //
      {0, 0, 0, 0}                //
  };

  struct finddup_app app = {
      .block_size = -1,                    //
      .method = "simple",                  //
      .pattern = "repo/content_%06d.dat",  //
  };

  int opt;
  int idx;
  static const char* fmt = "b:m:p:h";
  while ((opt = getopt_long(argc, argv, fmt, options, &idx)) != -1) {
    switch (opt) {
      case 'b':
        app.block_size = atoi(optarg);
        break;
      case 'm':
        app.method = optarg;
        break;
      case 'p':
        app.pattern = optarg;
        break;
      case 'h':
      default:
        print_usage();
    }
  }

  // Affichage des options
  printf("options\n");
  printf("  block-size: %d\n", app.block_size);
  printf("  method    : %s\n", app.method);
  printf("  pattern   : %s\n", app.pattern);

  return finddup_main(&app);
}
