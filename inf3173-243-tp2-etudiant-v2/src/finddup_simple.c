#include "finddup_simple.h"

#include <stdio.h>
#include <stdlib.h>

#include "crc32.h"
#include "finddup_main.h"

int finddup_simple(struct list* files, struct hashmap* hmap, int block_size) {
  /*
   * 1. Boucle de traitement principale:
   *    Générer le nom de fichier à partir du patron.
   *    Terminer la boucle quand le fichier n'est pas trouvé.
   */

  int ok = 0;
  struct list_node* node = list_head(files);
  while (!list_end(node)) {
    const char* fname = node->data;

    FILE* f = NULL;
    if ((f = fopen(fname, "rb"))) {
      /*
       * 2. Calculer la somme de contrôle du fichier au complet.
       *
       * ATTENTION: lire le fichier au complet comme on le fait ici est une
       * mauvaise pratique en général, car la quantité de mémoire à allouer
       * dépend de la taille du fichier. Le traitement par bloc permet de traiter
       * des fichiers de taille arbitraire, peu importe la mémoire vive
       * disponible.
       */
      fseek(f, 0, SEEK_END);
      size_t size = ftell(f);
      fseek(f, 0, SEEK_SET);

      char* buf = malloc(size);
      int bytes = fread(buf, 1, size, f);
      if (bytes == size) {
        uint32_t hash = crc32(0, buf, bytes);

        /*
         * 3. Insérer le fichier dans la table de hachage.
         */
        filegroup_add(hmap, hash, fname);
        ok++;
      }

      // Libérer les ressources
      fclose(f);
      free(buf);
    }

    node = node->next;
  }

  return (list_size(files) == ok) ? 0 : -1;
}
