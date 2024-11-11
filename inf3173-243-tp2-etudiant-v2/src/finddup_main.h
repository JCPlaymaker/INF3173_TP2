#pragma once

#include <inttypes.h>

#include "hashmap.h"
#include "list.h"

#ifdef __cplusplus
extern "C" {
#endif

struct finddup_app {
  int block_size;       // taille du tampon de travail
  char* method;         // méthode: fread, read, ou mmap
  const char* pattern;  // patron de nom de fichier (gabarit)
};

struct filegroup {
  uint32_t hash;
  struct list* files;
};

int finddup_main(struct finddup_app* app);
void filegroup_add(struct hashmap* hmap, uint32_t hash, const char* fname);
int filegroup_cmp(const void* a, const void* b, void* udata);
uint64_t filegroup_key(const void* item, uint64_t seed0, uint64_t seed1);
void filegroup_free(void* item);

#ifdef __cplusplus
}
#endif
