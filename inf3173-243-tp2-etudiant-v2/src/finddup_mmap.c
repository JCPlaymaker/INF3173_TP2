#include "finddup_mmap.h"

#include "crc32.h"
#include "finddup_main.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int finddup_mmap(struct list* files, struct hashmap* hmap, int block_size) {
  int ok = 0;
  struct list_node* node = list_head(files);

  while (!list_end(node)) {
    const char* fname = node->data;

    int fd = open(fname, O_RDONLY);
     if (fd != -1) {
            struct stat st;
            if (fstat(fd, &st) == 0) {
                size_t size = st.st_size;
                // projection en mémoire
                char* data = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
                if (data != MAP_FAILED) {
                  // Calcul du checksum en blocs
                    uint32_t hash = 0;
                    for (size_t offset = 0; offset < size; offset += block_size) {
                        size_t bytes_to_read = (offset + block_size < size) ? block_size : size - offset;
                        hash = crc32(hash, data + offset, bytes_to_read);
                    }
                    p_add(hmap, hash, fname);
                    ok++;
                    munmap(data, size);
                }
            }
            close(fd);
        }

    
    node = node->next;
  }

  return (list_size(files) == ok) ? 0 : -1;
}
