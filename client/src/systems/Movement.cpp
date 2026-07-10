#include "systems/Movement.hpp"
#include "Components.hpp"
#include "shared/Movement.hpp"

void System::Movement(WorldContext &ctx)
{
    for (auto [entity, position, input] : ctx.registry.view<Position, InputState, LocalPlayer>().each()) {
        ApplyMovement(position, input, ctx.dt);
    }
}
