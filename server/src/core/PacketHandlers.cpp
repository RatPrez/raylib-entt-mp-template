#include "core/WorldContext.hpp"
#include "shared/Components.hpp"

static void HandleInput(WorldContext &ctx, HSteamNetConnection conn, const void *data)
{
    auto it = ctx.net.connToEnt.find(conn);
    if (it == ctx.net.connToEnt.end()) {
        return;
    }

    const auto *pkt = static_cast<const CPacketInput *>(data);
    auto &input = ctx.registry.get<InputState>(it->second);
    input.moveX = pkt->moveX;
    input.moveZ = pkt->moveZ;
}

void registerPacketHandlers(WorldContext &ctx) { ctx.net.reg<CPacketInput>(HandleInput); }
