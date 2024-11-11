#pragma once

#include "list.h"
#include "hashmap.h"

#ifdef __cplusplus
extern "C" {
#endif

int finddup_mmap(struct list* files, struct hashmap* hmap, int block_size);

#ifdef __cplusplus
}
#endif
