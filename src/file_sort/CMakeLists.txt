add_executable(file_sort 
                src/file_sort.cpp
                src/FileSortAlgorithm.cpp)
                
add_executable(file_generator src/file_generator.cpp)
target_include_directories(file_sort PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_definitions(file_sort PRIVATE FILESORT_MODE)
target_compile_definitions(file_generator PRIVATE FILEGENERATOR_MODE)