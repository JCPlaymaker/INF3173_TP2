#include "finddup_mmap.h"
#include "hashmap.h"
#include "crc32.h"
#include "finddup_main.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>


int finddup_mmap(struct list* files, struct hashmap* hmap, int block_size) {
  int ok = 0;
  struct list_node* node = list_head(files);

  while (!list_end(node)) {
    const char* fname = node->data;

    int fd = open(fname, O_RDONLY);
    if(fd < 0){
            perror("open");
            node = node->next;
            continue;
    }else{
	  struct stat st;
	  if (fstat(fd, &st) == -1){
	     perror("fstat");
	     close(fd);
	     node = node -> next;
	     continue;
	  } 
              size_t file_size = st.st_size;
                // Mapper le ficher en mémoire

              char* buff = mmap(NULL,
			      file_size, PROT_READ | PROT_WRITE,
			      MAP_SHARED | MAP_PRIVATE,
			      fd,
			      0);
              if (buff == MAP_FAILED) {
		perror("mmap");
		close(fd);
		node = node -> next;
		continue;		
	      } else {
                  // Calcul du checksum en blocs
                  uint32_t hash = 0;
		  size_t offset = 0;

                  while (offset < file_size) {
            	    // Taille du bloc actuel (le dernier bloc peut être plus petit)
                   size_t current_block_size = (file_size - offset < block_size) ? (file_size - offset) : block_size;

                    // Calculer le CRC pour ce bloc
                    hash = crc32(hash, buff + offset, current_block_size);

                    // Passer au bloc suivant
                    offset += current_block_size;
        }
                  struct {
                      uint32_t hash;
                      const char* fname;
                  } item = { hash, fname };

                  hashmap_set(hmap, &item);

		  ok++;
                  munmap(buff, file_size);
                }
        }
    close(fd);    
    node = node->next;
  }

  return (list_size(files) == ok) ? 0 : -1;
}
