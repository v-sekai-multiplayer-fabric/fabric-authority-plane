# fabric-authority-plane

One zone's single writer. It owns the truth for that zone and it ticks at 20 Hz.

A **plane** is a process, and a plane has no networking. This one has none: no listening
socket, no HTTP server, no h2o. It paces itself with `iox2_node_wait`, the harness's own
wait, which is what puts every plane on the same cycle rather than on its own clock.

It came out of `gyreplane`, where the tick was an h2o timer rearming on an HTTP server's
event loop. That made it an edge plane wearing the wrong word: an h2o loop is a listening
socket's loop. What terminates client transport is `fabric-fanout-edge`, and it reaches this
plane over the ring.

## What it is not

**Not a database client.** An actor never opens FoundationDB, postgres, or a file. This
reaches its state through the Rivet API — the Actor Runtime Socket, SQLite over a Unix
socket, `leaseKey` transactions. The engine behind that socket is what decides whether the
bytes land in rocksdb or fdb, and this plane never knows.

**Not the interest filter.** Deciding who should hear about which entity is the fan-out
edge's job, because it is per subscriber and this plane has no subscribers.

## The domain it belongs to

A ring forces co-location. This plane publishes entity state per tick and
`fabric-fanout-edge` reads it per tick, so the two share a ring and therefore a machine.
`fabric-asset-edge` does not: content-addressed chunks are immutable and shared with nobody
per tick, so it needs no ring and lands wherever it likes.

That is the whole test, and it is the same one `fabric-zone-domain` applies.

## State

`src/authority_tick.c` is the tick, carried over unchanged except for losing its event loop,
and `src/main.cpp` is the loop that replaced it. It builds, and CI builds it.

`gen/` is codegen output — `rebac` from `lean-rebac-core`, the entity packet from
`lean-entity-packet` — vendored here rather than fetched. A copy can drift from the Lean
that produced it, and that is the smaller problem: a repository that cannot build without
cloning another one is a note, not a repository. `gyreplane` vendors the same files the same
way. Regenerate them, never edit them.

`thirdparty/harness` is a subtree, and nothing links iceoryx2: the dispatch table is
generated from `iceoryx2.sigs` and dlopens the library at start. CI checks the binary links
no transport, because that is what the word plane is claiming.
