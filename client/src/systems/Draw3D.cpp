#include "systems/Draw3D.hpp"
#include "Components.hpp"

void System::Draw3D(WorldContext &ctx)
{
    DrawGrid(20, 1.f);

    auto view = ctx.registry.view<CTransform, CCube>();
    for (auto [entity, transform, cube] : view.each()) {
        Vector3 pos = {transform.position.x, transform.position.y + cube.size.y * 0.5f, transform.position.z};
        DrawCubeV(pos, cube.size, cube.color);
        DrawCubeWiresV(pos, cube.size, BLACK);
    }
}
