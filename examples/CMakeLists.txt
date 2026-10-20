bundle_add(flatland_example_assets
  BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets
  DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/assets
  COMPRESSION DEFLATE
)

add_executable(flatland-example main.cpp)
target_link_libraries(flatland-example PRIVATE flatland flatland_example_assets)
