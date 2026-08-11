# The authority plane: one zone's single writer, on the ring.
#
# It links weft::harness and nothing that listens. A plane has no networking, so there is no
# h2o here and no socket to bind: the tick paces itself with iox2_node_wait, the same wait
# every other plane uses.
#
# Self contained. `gen/` holds the codegen output this reads, vendored rather than fetched,
# for the reason `thirdparty/harness` is a subtree: a repository that cannot build without
# reaching into another one is a note, not a repository. Regenerate those files, never edit
# them.
cmake_minimum_required(VERSION 3.22)
project(fabric_authority_plane C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(WEFT_HARNESS_PROOF OFF CACHE BOOL "" FORCE)
add_subdirectory(thirdparty/harness)

add_executable(authority-plane
  src/main.cpp
  src/authority_tick.c
  gen/rebac.c
  gen/xr_grid_entity_packet.c)
target_include_directories(authority-plane PRIVATE src ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(authority-plane PRIVATE weft::harness)
target_compile_options(authority-plane PRIVATE -Wall -Wextra)
