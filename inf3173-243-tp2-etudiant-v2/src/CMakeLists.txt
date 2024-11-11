add_library(core STATIC

  finddup_main.c
  finddup_main.h

  finddup_simple.c
  finddup_simple.h

  finddup_fread.c
  finddup_fread.h

  finddup_read.c
  finddup_read.h

  finddup_mmap.c
  finddup_mmap.h

  list.c
  list.h

  cwalk.c
  cwalk.h

  hashmap.c
  hashmap.h

  utils.c
  utils.h

  crc32.cpp
  crc32.h
  crc32.hpp
)
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(finddup finddup.c)
target_link_libraries(finddup core)

add_executable(genfiles genfiles.c)
target_link_libraries(genfiles core)
