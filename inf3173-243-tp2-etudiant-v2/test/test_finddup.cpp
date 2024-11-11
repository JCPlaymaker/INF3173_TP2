#include <crc32.h>
#include <fcntl.h>
#include <finddup_fread.h>
#include <finddup_main.h>
#include <finddup_mmap.h>
#include <finddup_read.h>
#include <finddup_simple.h>
#include <sys/stat.h>
#include <unistd.h>
#include <utils.h>

#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <map>
#include <set>
#include <sstream>
#include <string>

#include "inf3173.h"
#include "wrapper.h"

#define FILE_PATTERN SOURCE_DIR "/data/content_%06d.dat"

typedef std::map<uint32_t, std::set<std::string>> my_map;

namespace Catch {
template <>
struct StringMaker<my_map> {
  static std::string convert(const my_map& map) {
    std::ostringstream oss;
    oss << std::hex;
    oss << "{ ";
    for (const auto& pair : map) {
      oss << pair.first << ": {\n";
      for (const auto& value : pair.second) {
        oss << "  " << value << "\n";
      }
      oss << "  }\n";
    }
    oss << "}";
    return oss.str();
  }
};
}  // namespace Catch

void hashmap_convert(struct hashmap* hmap, my_map& stdmap) {
  size_t iter = 0;
  void* item;
  while (hashmap_iter(hmap, &iter, &item)) {
    struct filegroup* group = static_cast<struct filegroup*>(item);
    struct list_node* node = list_head(group->files);
    int i = 0;
    while (!list_end(node)) {
      std::filesystem::path tmp(static_cast<char*>(node->data));
      stdmap[group->hash].insert(tmp.filename());
      node = node->next;
    }
  }
}

TEST_CASE("Finddup") {
  struct hashmap* hmap = hashmap_new(  //
      sizeof(struct filegroup),        //
      1000, 0, 0, filegroup_key,       //
      filegroup_cmp, filegroup_free, NULL);

  static const my_map exp = {
      {0x0674cd09,               //
       {"content_000002.dat",    //
        "content_000007.dat"}},  //
      {0x16e8c9ce,
       {"content_000000.dat",    //
        "content_000004.dat"}},  //
      {0x4999ee35,               //
       {"content_000001.dat",    //
        "content_000006.dat",    //
        "content_000009.dat"}},  //
      {0x52642703,               //
       {"content_000003.dat",    //
        "content_000005.dat"}},  //
      {0xc7ae59ce,               //
       {"content_000008.dat"}},  //
  };

  struct list* files = list_new(NULL, free);
  {
    char tmp[FILENAME_MAX];
    for (int i = 0; i < 10; i++) {
      sprintf(tmp, FILE_PATTERN, i);
      list_push_back(files, list_node_new(strdup(tmp)));
    }
  }

  int nb_files = list_size(files);
  int file_size = 100;

  for (const auto block_size : {1, 32, 33, 1000}) {
    int nb_blocks = file_size / block_size + !!(file_size % block_size);

    SECTION("simple_block_size=" + std::to_string(block_size)) {
      my_map act;
      wrapper_clear();
      hashmap_clear(hmap, 0);
      finddup_simple(files, hmap, block_size);
      hashmap_convert(hmap, act);
      CHECK(exp == act);

      // Vérifier la méthode
      CHECK(wrapper_fread_count() == 1 * nb_files);
      CHECK(wrapper_read_count() == 0);
      CHECK(wrapper_mmap_count() == 0);
    }

    SECTION("fread_block_size=" + std::to_string(block_size)) {
      my_map act;
      wrapper_clear();
      hashmap_clear(hmap, 0);
      finddup_fread(files, hmap, block_size);
      hashmap_convert(hmap, act);
      CHECK(exp == act);

      // Vérifier la méthode
      CHECK(wrapper_fread_count() == nb_blocks * nb_files);
      CHECK(wrapper_read_count() == 0);
      CHECK(wrapper_mmap_count() == 0);
      CHECK(wrapper_fread_size_max() == block_size);
    }

    SECTION("read_block_size=" + std::to_string(block_size)) {
      my_map act;
      wrapper_clear();
      hashmap_clear(hmap, 0);
      finddup_read(files, hmap, block_size);
      hashmap_convert(hmap, act);
      CHECK(exp == act);

      // Vérifier la méthode
      CHECK(wrapper_fread_count() == 0);
      CHECK(wrapper_read_count() == nb_blocks * nb_files);
      CHECK(wrapper_mmap_count() == 0);
      CHECK(wrapper_read_size_max() == block_size);
    }

    SECTION("mmap_block_size=" + std::to_string(block_size)) {
      my_map act;
      wrapper_clear();
      hashmap_clear(hmap, 0);
      finddup_mmap(files, hmap, block_size);
      hashmap_convert(hmap, act);
      CHECK(exp == act);

      // Vérifier la méthode
      CHECK(wrapper_fread_count() == 0);
      CHECK(wrapper_read_count() == 0);
      CHECK(wrapper_mmap_count() >= 1);
      CHECK(wrapper_mmap_count() == wrapper_munmap_count());
    }
  }
  list_free(files);
  hashmap_free(hmap);
}
