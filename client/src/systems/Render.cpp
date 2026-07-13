#include "systems/Render.hpp"
#include "Components.hpp"

void System::Render(WorldContext &ctx)
{
    DrawGrid(20, 1.f);

    auto view = ctx.registry.view<Position, Cube>();
    for (auto [entity, position, cube] : view.each()) {
        Vector3 pos = {position.x, position.y + cube.size.y * 0.5f, position.z};
        DrawCubeV(pos, cube.size, cube.color);
        DrawCubeWiresV(pos, cube.size, BLACK);
    }
}
