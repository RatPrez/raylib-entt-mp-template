# template-mp

Personal multiplayer game template. raylib + EnTT client, GameNetworkingSockets server, shared packet definitions.

## Usage

```
./build.sh          # or build.bat on Windows
./run_sv.sh 30      # server, optional tick rate arg (default 20)
./run_cl.sh         # client, connects to 127.0.0.1
```

Needs GameNetworkingSockets installed (default `/opt/gns`, override with `GNS_ROOT=/path ./build.sh`). raylib and EnTT are fetched by CMake. `compile_commands.json` is symlinked to the project root for the IDE.

## Layout

- `shared/` contains `Packets.hpp` (packet structs, each with a `kType`), `Net.hpp` (`eNetMode` and outbox message helpers), `Components.hpp` (`NetId`, `Position`, `InputState`), and `Movement.hpp` (movement code used by both sides).
- `client/` has the same layout as template-sp, plus `NetContext`, a `Network` system, and a `Movement` system that predicts the local player with the shared movement code. Server `State` packets set a `TargetPosition` on remote players, which an `Interp` system smoothly (but quickly) moves them toward. Only the local player gets corrected (lerp, or snap when way off).
- `server/` is a headless entt loop with a fixed tick: `NetReceive -> Tick -> NetSend`.

## Abstraction

`Client` and `Server` (in `core/`) own the `WorldContext` (registry, net state, camera, timing) and expose the game loop as a handful of private entry-point methods, each of which just calls out to systems in order. Systems themselves are free functions (`System::Foo(ctx)`) with no knowledge of each other. The entry points are the only place ordering is decided.

- `Client::tick()` runs `Input -> Movement -> Interp`
- `Client::lateTick()` is currently empty, a hook for post-draw / end-of-frame work
- `Client::draw3D()` runs `Render` (called inside `BeginMode3D` / `EndMode3D`)
- `Client::draw2D()` runs `ConnText` (screen-space UI, connection status text)
- `Server::tick()` runs `Movement`

`Client::run()` and `Server::run()` wrap these with the net pump: `NetReceive` before `tick()`, `NetSend` after everything else. Adding a new system means writing the free function and adding one line to the relevant entry-point method. The loop shape itself doesn't change.

## PacketHandlers

`core/PacketHandlers.cpp` (one per side) is where incoming packets are wired to logic. It's kept separate from `Client`/`Server` so the connection and game-loop code doesn't get cluttered with packet-specific handling. Each file defines `registerPacketHandlers(WorldContext &ctx)`, called once during setup, which calls `ctx.net.reg<PacketType>(handler)` for every packet type that side cares about.

- Client handlers (`Welcome`, `Spawn`, `Despawn`, `State`) take `(WorldContext &, const void *data)` and apply server updates to the local registry. For example, `HandleState` reconciles the local player (small errors lerp, big ones snap) and just sets `TargetPosition` for remote players, letting `Interp` handle smoothing.
- Server handlers (`Input`) take `(WorldContext &, HSteamNetConnection, const void *data)`, since the server needs to know which connection/entity a packet came from. `HandleInput` writes the client's move axes into that entity's `InputState` for the next `Movement` tick.

Handlers stay dumb: they just move data from the wire into components. The actual per-tick behavior lives in the systems.

## Networking

Add a packet in `shared/Packets.hpp` (enum entry + packed struct with `kType`), then register a handler:

```cpp
ctx.net.reg<SPacketSpawn>(HandleSpawn);
```

Send through the net context, reliability per call:

```cpp
ctx.net.send(pkt);                              // client, reliable default
ctx.net.send(pkt, eNetMode::Unreliable);        // client
net.send(conn, pkt, eNetMode::Reliable);        // server, one client
net.broadcast(pkt, eNetMode::Unreliable);       // server, all clients
```

## Credits

- [raylib](https://www.raylib.com/) for client rendering and windowing
- [EnTT](https://github.com/skypjack/entt) for the entity-component system
- [GameNetworkingSockets](https://github.com/ValveSoftware/GameNetworkingSockets) for client/server networking
