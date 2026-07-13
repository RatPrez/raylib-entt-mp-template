#include "Components.hpp"
#include "core/WorldContext.hpp"
#include "shared/Components.hpp"

static entt::entity GetOrCreateActor(WorldContext &ctx, uint32_t netId)
{
    auto it = ctx.net.netToEnt.find(netId);
    if (it != ctx.net.netToEnt.end()) {
        return it->second;
    }

    auto entity = ctx.registry.create();
    ctx.registry.emplace<NetId>(entity, netId);
    ctx.registry.emplace<Position>(entity);
    ctx.registry.emplace<Cube>(entity, Cube{.color = netId == ctx.net.localNetId ? RED : BLUE});
    if (netId == ctx.net.localNetId) {
        ctx.registry.emplace<LocalPlayer>(entity);
        ctx.registry.emplace<InputState>(entity);
    } else {
        ctx.registry.emplace<TargetPosition>(entity);
    }
    ctx.net.netToEnt[netId] = entity;
    return entity;
}

static void HandleWelcome(WorldContext &ctx, const void *data)
{
    const auto *pkt = static_cast<const SPacketWelcome *>(data);
    ctx.net.localNetId = pkt->netId;
    printf("welcome: netId %u, server tick rate %u\n", pkt->netId, pkt->tickRate);
}

static void HandleSpawn(WorldContext &ctx, const void *data)
{
    const auto *pkt = static_cast<const SPacketSpawn *>(data);
    auto entity = GetOrCreateActor(ctx, pkt->netId);
    ctx.registry.get<Position>(entity) = {pkt->x, pkt->y, pkt->z};
    if (auto *target = ctx.registry.try_get<TargetPosition>(entity)) {
        *target = {pkt->x, pkt->y, pkt->z};
    }
}

static void HandleDespawn(WorldContext &ctx, const void *data)
{
    const auto *pkt = static_cast<const SPacketDespawn *>(data);
    auto it = ctx.net.netToEnt.find(pkt->netId);
    if (it == ctx.net.netToEnt.end()) {
        return;
    }

    ctx.registry.destroy(it->second);
    ctx.net.netToEnt.erase(it);
}

static void HandleState(WorldContext &ctx, const void *data)
{
    const auto *pkt = static_cast<const SPacketState *>(data);
    auto entity = GetOrCreateActor(ctx, pkt->netId);
    auto &position = ctx.registry.get<Position>(entity);

    if (pkt->netId != ctx.net.localNetId) {
        ctx.registry.get<TargetPosition>(entity) = {pkt->x, pkt->y, pkt->z};
        return;
    }

    float dx = pkt->x - position.x;
    float dz = pkt->z - position.z;
    if (dx * dx + dz * dz > 1.f) {
        position = {pkt->x, pkt->y, pkt->z};
    } else {
        position.x += dx * 0.1f;
        position.z += dz * 0.1f;
    }
}

void registerPacketHandlers(WorldContext &ctx)
{
    ctx.net.reg<SPacketWelcome>(HandleWelcome);
    ctx.net.reg<SPacketSpawn>(HandleSpawn);
    ctx.net.reg<SPacketDespawn>(HandleDespawn);
    ctx.net.reg<SPacketState>(HandleState);
}
