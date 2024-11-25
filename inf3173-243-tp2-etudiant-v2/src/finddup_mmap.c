#include "finddup_mmap.h"
#include "crc32.h"
#include "finddup_main.h"
#include "hashmap.h"
#include "list.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int finddup_mmap(struct list* files, struct hashmap* hmap, int block_size) {
  struct list_node* node = list_head(files);
  int compteur = 0;

  while (!list_end(node)) {
    char* fname = node->data;

    // Ouvrir le fichier en mode lecture seule
    int fd = open(fname, O_RDONLY);
    if (fd == -1) {
      perror("Erreur d'ouverture du fichier");
      node = node->next;
      continue;
    }

    // Obtenir la taille totale du fichier avec fstat()
    struct stat st;
    if (fstat(fd, &st) == -1) {
      perror("Erreur d'obtention de la taille du fichier");
      close(fd);
      return -1;
    }
    size_t file_size = st.st_size;

    void* file_map = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (file_map == MAP_FAILED) {
      perror("Erreur de mappage du fichier en mémoire");
      close(fd);
      return -1;
    }

    uint32_t hash = 0;

    // Calcul du checksum par blocs
    for (size_t offset = 0; offset < file_size; offset += block_size) {
      size_t chunk_size = (offset + block_size > file_size) ? file_size - offset : block_size;
      hash = crc32(hash, (unsigned char*)file_map + offset, chunk_size);
    }

    // Ajouter du checksum dans le hashmap
    filegroup_add(hmap, hash, fname);
    compteur++;

    // Démappage
    if (munmap(file_map, file_size) == -1) {
      perror("Erreur de démappage du fichier");
      close(fd);
      return -1;
    }

    close(fd);
    node = node->next;
  }
  return (list_size(files) == compteur) ? 0 : -1;
}
