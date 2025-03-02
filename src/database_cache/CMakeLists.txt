option(ENABLE_DATABASE_CACHE_EXECUTABLE "Build the database_cache executable" ON)

add_library(database_cache_lib 
                src/LRUCache.cpp
                src/i_db.cpp)

target_include_directories(database_cache_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_definitions(database_cache_lib PRIVATE DATABASE_CACHE_MODE)

if (ENABLE_DATABASE_CACHE_EXECUTABLE)
    add_executable(database_cache src/database_cache.cpp
                                    src/LRUCache.cpp
                                    src/i_db.cpp)
    target_include_directories(database_cache PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_link_libraries(database_cache PRIVATE database_cache_lib)
endif()
