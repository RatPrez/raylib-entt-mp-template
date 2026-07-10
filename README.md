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

- `shared/` — `Packets.hpp` (packet structs, each with a `kType`) and `Net.hpp` (`eNetMode`, outbox message helpers)
- `client/` — same layout as template-sp plus `NetContext` and a `Network` system
- `server/` — headless entt loop, fixed tick: `NetReceive -> Tick -> NetSend`

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
