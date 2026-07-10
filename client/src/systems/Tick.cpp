#include "systems/Tick.hpp"
#include "Components.hpp"

void System::Tick(WorldContext &ctx)
{
    CPacketInput input{};
    if (IsKeyDown(KEY_W)) {
        input.moveZ -= 1.f;
    }
    if (IsKeyDown(KEY_S)) {
        input.moveZ += 1.f;
    }
    if (IsKeyDown(KEY_A)) {
        input.moveX -= 1.f;
    }
    if (IsKeyDown(KEY_D)) {
        input.moveX += 1.f;
    }

    if (ctx.net.state == eConnState::Connected) {
        ctx.net.send(input, eNetMode::Unreliable);
    }

    for (auto [entity, transform] : ctx.registry.view<CTransform, CLocalPlayer>().each()) {
        ctx.camera.target = transform.position;
    }

    UpdateCamera(&ctx.camera, CAMERA_ORBITAL);
}
