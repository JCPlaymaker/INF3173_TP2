#pragma once

#include "hashmap.h"
#include "list.h"

#ifdef __cplusplus
extern "C" {
#endif

int finddup_simple(struct list* files, struct hashmap* hmap, int block_size);

#ifdef __cplusplus
}
#endif
