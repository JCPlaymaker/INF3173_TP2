#include <crc32.h>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("CRC32") {
  int v1 = 0xCAFEBABE;
  int v2 = 0xDEADBEEF;
  int arr[] = {v1, v2};

  SECTION("SameValues") {
    uint32_t m1 = crc32(0, &v1, sizeof(v1));
    uint32_t m2 = crc32(0, &v1, sizeof(v1));
    REQUIRE(m1 == m2);
  }

  SECTION("DifferentValues") {
    uint32_t m1 = crc32(0, &v1, sizeof(v1));
    uint32_t m2 = crc32(0, &v2, sizeof(v2));
    REQUIRE(m1 != m2);
  }

  SECTION("Transitivity") {
    uint32_t m0 = crc32(0, arr, sizeof(arr));
    uint32_t m1 = crc32(0, &v1, sizeof(v1));
    uint32_t m2 = crc32(m1, &v2, sizeof(v2));
    REQUIRE(m0 == m2);
  }
}
