cmake_minimum_required(VERSION 3.18)
project(flatland CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(FLATLAND_BUILD_EXAMPLES "Build the example server" ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/cl-bootstrap.cmake)
cl_repo()

file(GLOB_RECURSE FLATLAND_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_library(flatland STATIC ${FLATLAND_SOURCES})
target_include_directories(flatland PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/src/registries)

find_package(Threads REQUIRED)
target_link_libraries(flatland PUBLIC Threads::Threads)

if(FLATLAND_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
