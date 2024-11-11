#include "finddup_main.h"

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "finddup_fread.h"
#include "finddup_mmap.h"
#include "finddup_read.h"
#include "finddup_simple.h"
#include "hashmap.h"
#include "list.h"

int finddup_main(struct finddup_app* app) {
  int ret = 1;

  struct hashmap* hmap
      = hashmap_new(sizeof(struct filegroup), 1000, 0, 0, filegroup_key, filegroup_cmp, filegroup_free, NULL);

  // Chercher les fichiers qui correspondent au critère
  struct list* files = list_new(NULL, free);
  {
    int i = 0;
    int run = 1;
    while (run) {
      char* fname;
      struct stat info;
      asprintf(&fname, app->pattern, i);
      int ret = stat(fname, &info);
      if (ret == 0) {
        list_push_back(files, list_node_new(strdup(fname)));
      } else {
        run = 0;
      }
      i++;
      free(fname);
    }
  }

  printf("Nombre de fichiers à traiter: %ld\n", list_size(files));

  if (strcmp(app->method, "simple") == 0) {
    ret = finddup_simple(files, hmap, app->block_size);
  } else if (strcmp(app->method, "fread") == 0) {
    ret = finddup_fread(files, hmap, app->block_size);
  } else if (strcmp(app->method, "read") == 0) {
    ret = finddup_read(files, hmap, app->block_size);
  } else if (strcmp(app->method, "mmap") == 0) {
    ret = finddup_mmap(files, hmap, app->block_size);
  } else {
    printf("Erreur: methode %s invalide\n", app->method);
  }

  // On peut maintenant libérer la liste
  list_free(files);

  if (ret == 0) {
    printf("RAPPORT\n");
    size_t iter = 0;
    void* item;
    while (hashmap_iter(hmap, &iter, &item)) {
      const struct filegroup* group = item;
      printf("filegroup 0x%08x\n", group->hash);
      struct list_node* node = list_head(group->files);
      int i = 0;
      while (!list_end(node)) {
        printf("  %-6d %s\n", i++, (char*)node->data);
        node = node->next;
      }
    }
  }

  hashmap_free(hmap);
  return ret;
}

// NOTE: la chaine fname est copiée dans la liste
void filegroup_add(struct hashmap* hmap, uint32_t hash, const char* fname) {
  /*
   * Si le hash existe déjà, alors ajouter le fichier dans la liste.
   * Sinon, créer un struct filegroup, ajouter le fichier dans
   * la liste et ajouter l'item dans la table de hachage.
   */
  struct filegroup query = {.hash = hash};
  const struct filegroup* entry = hashmap_get(hmap, &query);
  if (entry == NULL) {
    // L'objet est copiée dans la table de hachage (shallow copy)
    // Donc un objet temportaire est ok
    struct filegroup tmp = {
        .hash = hash,                  //
        .files = list_new(NULL, free)  //
    };
    list_push_back(tmp.files, list_node_new(strdup(fname)));
    hashmap_set(hmap, &tmp);
  } else {
    list_push_back(entry->files, list_node_new(strdup(fname)));
  }
}

int filegroup_cmp(const void* a, const void* b, void* udata) {
  const struct filegroup* g1 = a;
  const struct filegroup* g2 = b;
  if (g1->hash > g2->hash) {
    return 1;
  } else if (g1->hash == g2->hash) {
    return 0;
  } else {
    return -1;
  }
}

uint64_t filegroup_key(const void* item, uint64_t seed0, uint64_t seed1) {
  const struct filegroup* grp = item;
  return hashmap_sip(&grp->hash, sizeof(grp->hash), seed0, seed1);
}

void filegroup_free(void* item) {
  struct filegroup* grp = item;
  list_free(grp->files);
}
