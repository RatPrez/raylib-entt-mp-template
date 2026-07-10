#pragma once
#include <entt/entt.hpp>
#include <raylib.h>

#include "core/NetContext.hpp"

struct WorldContext
{
    entt::registry registry;
    Camera3D camera{};
    NetContext net;
    float gameTime = 0.f;
    float dt = 0.f;
};
