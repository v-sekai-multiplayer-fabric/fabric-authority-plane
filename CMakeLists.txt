# The authority plane: one zone's single writer, on the ring.
#
# It links weft::harness and nothing that listens. A plane has no networking, so there is no
# h2o here and no socket to bind: the tick paces itself with iox2_node_wait, which is the
# same wait every other plane uses.
#
# It does not build yet, and the reason is worth stating rather than hiding. `gen/rebac.h`
# and `gen/xr_grid_entity_packet.h` are generated — from lean-rebac-core and
# lean-entity-packet — and they live in the repository that generates them. Copying them
# here would put one decision in two places, which is the drift the harness subtree exists
# to avoid. Issue #1 is wiring that generation, and until it is done this target is
# deliberately not added to the default build.
cmake_minimum_required(VERSION 3.22)
project(fabric_authority_plane C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(thirdparty/harness)

# The generated headers this plane reads. Point at the tree that produces them.
set(WEFT_GEN_DIR "" CACHE PATH "directory holding gen/rebac.h and gen/xr_grid_entity_packet.h")

if(WEFT_GEN_DIR)
  add_executable(authority-plane src/main.cpp src/authority_tick.c)
  target_include_directories(authority-plane PRIVATE src ${WEFT_GEN_DIR})
  target_link_libraries(authority-plane PRIVATE weft::harness)
  target_compile_options(authority-plane PRIVATE -Wall -Wextra)
else()
  message(STATUS
    "authority-plane not built: set -DWEFT_GEN_DIR to the tree holding gen/rebac.h. See #1.")
endif()
