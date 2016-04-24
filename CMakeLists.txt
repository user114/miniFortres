cmake_minimum_required (VERSION 2.6)
project (3dwarffortress)
set (CMAKE_CXX_STANDARD 11)
include_directories ("${PROJECT_SOURCE_DIR}/irrlicht/lib/Linux")
include_directories ("${PROJECT_SOURCE_DIR}/irrlicht/include")

add_subdirectory(tests)
add_executable(my_df main.cpp map.cpp)
target_link_libraries (my_df Irrlicht)