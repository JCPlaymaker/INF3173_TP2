#include "finddup_read.h"
#include "crc32.h"
#include "hashmap.h"
#include "finddup_main.h"
#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int finddup_read(struct list* files, struct hashmap* hmap, int block_size) {
  // À COMPLÉTER
  // Astuce: prendre le contenu de finddup_simple() et adapter
    int ok = 0;
    struct list_node *node = list_head(files);

    while (!list_end(node)) {
        char* fname = node->data;
        int fd = open(fname, O_RDONLY);
        
	if (fd == -1) {
            perror("Erreur d'ouverture du fichier");
            node = node->next;
            continue;
        }

        // Allocation du buffer
        char *buffer = malloc(block_size);
        if (!buffer) {
            perror("Erreur d'allocation de mémoire");
            close(fd);
            return -1;
        }

        uint32_t hash = 0;
        int bytes;

        // Calcul du checksum en blocs
        while ((bytes = read(fd, buffer, block_size)) > 0) {
            hash = crc32(hash, buffer, bytes);
        }

        if (bytes == -1) {
            perror("Erreur de lecture du fichier");
            free(buffer);
            close(fd);
            return -1;
        }

        // Ajout du checksum dans le hashmap
        filegroup_add(hmap, hash, fname);
        ok++;
        free(buffer);
        close(fd);

        node = node->next;
    }

    return (list_size(files) == ok) ? 0 : -1;
}

