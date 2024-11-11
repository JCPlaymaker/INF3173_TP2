add_library(testutils testutils.cpp wrapper.c)
target_include_directories(testutils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(testutils PUBLIC core)

add_executable(test_base test_base.cpp)
target_link_libraries(test_base PRIVATE core testutils Catch2::Catch2WithMain)

add_executable(test_finddup test_finddup.cpp)
target_link_libraries(test_finddup PRIVATE core testutils Catch2::Catch2WithMain)

add_executable(bench_finddup bench_finddup.cpp)
target_link_libraries(bench_finddup PRIVATE core testutils Catch2::Catch2WithMain)

include(Catch)
catch_discover_tests(test_base)
catch_discover_tests(test_finddup)
catch_discover_tests(bench_finddup)

