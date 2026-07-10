#include <cmath>

#include "Components.hpp"
#include "systems/Interp.hpp"

void System::Interp(WorldContext &ctx)
{
    constexpr float kSharpness = 15.f;
    constexpr float kSnapDistSq = 25.f;

    float t = 1.f - expf(-kSharpness * ctx.dt);

    for (auto [entity, position, target] : ctx.registry.view<Position, TargetPosition>().each()) {
        float dx = target.x - position.x;
        float dy = target.y - position.y;
        float dz = target.z - position.z;

        if (dx * dx + dy * dy + dz * dz > kSnapDistSq) {
            position = {target.x, target.y, target.z};
            continue;
        }

        position.x += dx * t;
        position.y += dy * t;
        position.z += dz * t;
    }
}
