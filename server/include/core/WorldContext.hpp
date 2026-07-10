#pragma once
#include <entt/entt.hpp>

#include "core/NetContext.hpp"

struct WorldContext
{
    entt::registry registry;
    NetContext net;
    uint64_t tick = 0;
    float tickDt = 0.f;
    float gameTime = 0.f;
    uint32_t nextNetId = 1;
};
