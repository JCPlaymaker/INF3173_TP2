#include "finddup_fread.h"
#include "hashmap.h"
#include "crc32.h"
#include "finddup_main.h"
#include "list.h"
#include <stdio.h>
#include <stdlib.h>

int finddup_fread(struct list* files, struct hashmap* hmap, int block_size) {
  // À COMPLÉTER
  // Astuce: prendre le contenu de finddup_simple() et adapter
  int ok =0;
  struct list_node* node = list_head(files);
  while (!list_end(node)) {
     char* fname = node->data;

     FILE* file = NULL;
     if ((file = fopen(fname, "rb"))) {
        char* buffer = malloc(block_size);
	uint32_t hash = 0;
	int bytes;

	// Calcul du checksum en blocs
	while((bytes = fread(buffer, 1, block_size, file)) > 0) {
	   hash = crc32(hash, buffer, bytes);
	}

	//Ajout du checksum dans le hashmap
	filegroup_add(hmap,hash, fname);
	ok++;
	free(buffer);
	fclose(file);
     }

     node = node->next;

  }

  return (list_size(files) == ok) ? 0 : -1;

}
