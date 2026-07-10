#include "systems/Tick.hpp"
#include "Components.hpp"

void System::Tick(WorldContext &ctx)
{
    constexpr float kMoveSpeed = 5.f;

    for (auto [entity, netId, transform, input] : ctx.registry.view<CNetId, CTransform, CInput>().each()) {
        transform.x += input.moveX * kMoveSpeed * ctx.tickDt;
        transform.z += input.moveZ * kMoveSpeed * ctx.tickDt;

        ctx.net.broadcast(SPacketState{.netId = netId.value, .x = transform.x, .y = transform.y, .z = transform.z},
                          eNetMode::Unreliable);
    }
}
