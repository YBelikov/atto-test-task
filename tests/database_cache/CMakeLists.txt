find_package(GTest REQUIRED)
add_executable(test_lru_cache test_lru_cache.cpp)

include_directories(${PROJECT_SOURCE_DIR}/src/database_cache/include)
target_compile_definitions(test_lru_cache PRIVATE TEST_LRU_CACHE_MODE)

target_link_libraries(test_lru_cache PRIVATE database_cache_lib GTest::GTest GTest::Main pthread)
add_test(NAME LRUCacheTest COMMAND test_lru_cache)
