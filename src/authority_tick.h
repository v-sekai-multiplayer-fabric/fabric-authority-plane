// What the plane loop calls, and the state it calls through.
//
// The tick is unchanged from the zone server it came out of, except that it lost its event
// loop. `rivet_state_t` is a handle onto the Actor Runtime Socket: an actor never opens a
// database, so this is an API and not a driver.
//
// SPDX-License-Identifier: Apache-2.0
#ifndef WEFT_AUTHORITY_TICK_H
#define WEFT_AUTHORITY_TICK_H

#include <stddef.h>
#include <stdint.h>

// The packet is codegen from `lean-entity-packet`, and is the one this plane must not
// diverge from: it is the same 100 bytes the fan-out edge sends on the wire.
#include "gen/xr_grid_entity_packet.h"

typedef struct {
	int (*read_entities)(void *conn, uint32_t z_id,
			     xr_grid_entity_packet_t *out, size_t cap, size_t *n);
	int (*write_entities)(void *conn, uint32_t z_id,
			      const xr_grid_entity_packet_t *ents, size_t n);
	int (*commit)(void *conn);
	void *conn;
} rivet_state_t;

// One tick of one zone. Returns 0 when the tick landed.
int authority_tick(const rivet_state_t *state, uint32_t z_id);

#endif
