// The authority plane: one process, one zone, one writer.
//
// The tick used to be an h2o timer rearming itself on an HTTP server's event loop. A plane
// has no networking, so it has no HTTP server to borrow a loop from, and the cycle comes
// from the ring instead: `iox2_node_wait` is the harness's own wait, and it is what every
// plane paces itself with.
//
// That is not a rename. An h2o loop is a listening socket's loop, and a plane that owns one
// is an edge plane wearing the wrong word. What terminates client transport here is
// `fabric-fanout-edge`, and it reaches this plane over the ring.
//
// SPDX-License-Identifier: Apache-2.0
#include "iox2_api.h"
#include "weft/bus.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
#include "authority_tick.h"
}

// 20 Hz, which is `PBVH_SIM_TICK_HZ` and not a number chosen here.
#define TICK_NS (50 * 1000 * 1000)

int main(int argc, char** argv) {
    const uint32_t zone = (argc > 1) ? (uint32_t)std::atoi(argv[1]) : 0;

    if (!weft::load_bus()) {
        return 1;
    }
    iox2_set_log_level_from_env_or(iox2_log_level_e_ERROR);

    iox2_node_h node = nullptr;
    if (iox2_node_builder_create(iox2_node_builder_new(nullptr), nullptr,
                                 iox2_service_type_e_IPC, &node)
        != IOX2_OK) {
        std::fprintf(stderr, "authority: no node\n");
        return 1;
    }

    // The state handle the tick reads and writes through. An actor never opens a database,
    // so this is the Actor Runtime Socket and not a driver.
    rivet_state_t state;
    std::memset(&state, 0, sizeof(state));

    std::printf("authority: zone %u, 20 Hz, no networking\n", zone);
    std::fflush(stdout);

    for (;;) {
        if (authority_tick(&state, zone) != 0) {
            std::fprintf(stderr, "authority: zone %u tick failed\n", zone);
        }
        // The wait is the tick. A plane that slept on its own clock would drift against
        // every other plane on the ring; this one does not.
        (void)iox2_node_wait(&node, 0, TICK_NS);
    }

    iox2_node_drop(node);
    return 0;
}
