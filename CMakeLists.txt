project (rpc)
cmake_minimum_required (VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
include_directories(${CMAKE_SOURCE_DIR}/third_party/cityhash/include)
link_directories(${CMAKE_SOURCE_DIR}/lib)
link_directories(${CMAKE_SOURCE_DIR}/third_party/cityhash/lib)

add_subdirectory(${CMAKE_SOURCE_DIR}/src)
