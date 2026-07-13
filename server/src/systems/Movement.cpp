#include "systems/Movement.hpp"
#include "shared/Components.hpp"
#include "shared/Movement.hpp"

void System::Movement(WorldContext &ctx)
{
    for (auto [entity, netId, position, input] : ctx.registry.view<NetId, Position, InputState>().each()) {
        ApplyMovement(position, input, ctx.tickDt);

        ctx.net.broadcast(SPacketState{.netId = netId.value, .x = position.x, .y = position.y, .z = position.z},
                          eNetMode::Unreliable);
    }
}
