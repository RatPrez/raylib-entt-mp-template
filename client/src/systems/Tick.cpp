#include "systems/Tick.hpp"
#include "Components.hpp"

void System::Tick(WorldContext &ctx)
{
    InputState input{};
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

    for (auto [entity, localInput] : ctx.registry.view<InputState, LocalPlayer>().each()) {
        localInput = input;
    }

    if (ctx.net.state == eConnState::Connected) {
        ctx.net.send(CPacketInput{.moveX = input.moveX, .moveZ = input.moveZ}, eNetMode::Unreliable);
    }

    for (auto [entity, position] : ctx.registry.view<Position, LocalPlayer>().each()) {
        ctx.camera.target = {position.x, position.y, position.z};
    }

    UpdateCamera(&ctx.camera, CAMERA_ORBITAL);
}
