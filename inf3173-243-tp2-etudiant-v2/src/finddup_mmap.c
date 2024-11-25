#include "finddup_mmap.h"

#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "crc32.h"
#include "finddup_main.h"
#include "hashmap.h"


uint32_t calculate_crc(char* mapped_file, size_t file_size, int block_size){
  uint32_t crc = 0;
  size_t offset = 0;

  while (offset < file_size){
    size_t current_block_size = (file_size - offset < block_size) ? (file_size - offset) : block_size;
    crc = crc32(crc, mapped_file + offset, current_block_size);
    offset += current_block_size;
  }
  return crc;
}


int finddup_mmap(struct list* files, struct hashmap* hmap, int block_size) {
  int ok = 0;
  struct list_node* node = list_head(files);

  printf("finddup_mmap\n");
  FILE* f = NULL;

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
		uint32_t hash = calculate_crc(buff, file_size, block_size);
		filegroup_add(hmap, hash, fname);
		ok++;
              }
       	munmap(buff, file_size);
	close(fd);

	node = node->next;
      }
  }
  return (list_size(files) == ok) ? 0 : -1;
}

