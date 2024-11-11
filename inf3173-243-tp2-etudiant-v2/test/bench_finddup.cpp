#include <finddup_fread.h>
#include <finddup_main.h>
#include <finddup_mmap.h>
#include <finddup_read.h>
#include <finddup_simple.h>
#include <hashmap.h>
#include <inf3173.h>

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

#include "list.h"
#include "testutils.h"

#define FILE_PATTERN SOURCE_DIR "/data/content_%06d.dat"

TEST_CASE("Benchmark") {
#ifndef NDEBUG
  printf("*** ERREUR \n");
  printf("*** Recompiler en mode Release pour les mesures de performance\n");
#else
  // Créer un seul gros fichier
  generate_file("grosfichier.dat", (1 << 20), 0xCAFEBABE);

  struct hashmap* hmap = hashmap_new(  //
      sizeof(struct filegroup),        //
      1000, 0, 0, filegroup_key,       //
      filegroup_cmp, filegroup_free, NULL);

  struct list* files = list_new(NULL, free);
  list_push_back(files, list_node_new(strdup("grosfichier.dat")));

  std::vector<int> sizes = {1, 8, 64, 512, 4096, 32768};

  for (const auto block_size : sizes) {
    BENCHMARK("fread_block_size=" + std::to_string(block_size)) {
      hashmap_clear(hmap, 0);
      return finddup_fread(files, hmap, block_size);
    };
  }

  for (const auto block_size : sizes) {
    BENCHMARK("read_block_size=" + std::to_string(block_size)) {
      hashmap_clear(hmap, 0);
      return finddup_read(files, hmap, block_size);
    };
  }

  for (const auto block_size : sizes) {
    BENCHMARK("mmap_block_size=" + std::to_string(block_size)) {
      hashmap_clear(hmap, 0);
      return finddup_fread(files, hmap, block_size);
    };
  }

  list_free(files);
  hashmap_free(hmap);
#endif
}
