cmake_minimum_required(VERSION 3.18)
project(flatland CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(FLATLAND_BUILD_EXAMPLE "Build the example server" ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/cl-bootstrap.cmake)
cl_repo()

file(GLOB_RECURSE FLATLAND_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_library(flatland STATIC ${FLATLAND_SOURCES})
target_include_directories(flatland PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/src/registries)

find_package(Threads REQUIRED)
target_link_libraries(flatland PUBLIC Threads::Threads)

cl_add_dep(flatland miniz PRIVATE)

cl_import_source(
  NAME bundle_cmake
  DOWNLOAD_ONLY
  REPO https://github.com/gradylink/bundle.cmake.git
  REF 01f64897efe84f97033293edc3aa5a6b5e6296c0
)
include("${CL_SOURCE_DIR}/cmake/bundle.cmake")

bundle_add(flatland_shaders
  BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets
  DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/assets
  COMPRESSION DEFLATE
)
target_link_libraries(flatland PUBLIC flatland_shaders)

if(FLATLAND_BUILD_EXAMPLE)
  add_subdirectory(example)
endif()
