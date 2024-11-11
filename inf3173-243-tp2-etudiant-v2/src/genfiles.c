#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

void print_usage() {
  fprintf(stderr,
          "Usage: %s "
          "-n num "
          "-r srand "
          "-s size "
          "--help]\n",
          "genfiles");
  exit(1);
}

int main(int argc, char** argv) {
  int ret = EXIT_FAILURE;

  struct option options[] = {
      {"num-files", 1, 0, 'n'},  //
      {"srand", 1, 0, 'r'},      //
      {"destdir", 1, 0, 'd'},    //
      {"size", 1, 0, 's'},       //
      {"help", 0, 0, 'h'},       //
      {0, 0, 0, 0}               //
  };

  int n = 10;
  int rseed = 5;
  int fsize = 100;
  const char* destdir = "repo";

  int opt;
  int idx;
  static const char* fmt = "n:r:s:h";
  while ((opt = getopt_long(argc, argv, fmt, options, &idx)) != -1) {
    switch (opt) {
      case 'n':
        n = atoi(optarg);
        break;
      case 'r':
        rseed = atoi(optarg);
        break;
      case 's':
        fsize = atoi(optarg);
        break;
      case 'd':
        destdir = optarg;
        break;
      case 'h':
      default:
        print_usage();
    }
  }

  // Affichage des options
  printf("options\n");
  printf("  destdir   : %s\n", destdir);
  printf("  num-files : %d\n", n);
  printf("  rseed     : %d\n", rseed);
  printf("  fsize     : %d\n", fsize);

  {
    // Création du répertoire de destination
    int tmp;
    if ((tmp = open(destdir, O_DIRECTORY)) < 0) {
      // Si on ne peut pas ouvrir le répertoire, on essaye de le créer
      if (mkdir(destdir, 0777) < 0) {
        perror("mkdir");
        return 1;
      }
    } else {
      close(tmp);
    }
  }

  for (int i = 0; i < n; i++) {
    int s = rand() % rseed;
    unsigned short seed[3] = {s, s, s};
    seed48(seed);

    char* fname;
    asprintf(&fname, "%s/content_%06d.dat", destdir, i);
    FILE* file = fopen(fname, "wb");
    int nblocs = fsize / sizeof(long int);
    int reste = fsize % sizeof(long int);
    for (int j = 0; j < nblocs; j++) {
      long int val = jrand48(seed);
      fwrite(&val, sizeof(val), 1, file);
    }
    if (reste > 0) {
      long int val = jrand48(seed);
      fwrite(&val, 1, reste, file);
    }
    fclose(file);
    free(fname);
  }

  return 0;
}
