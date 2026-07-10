#pragma once
#include <cstdint>

struct NetId
{
    uint32_t value = 0;
};

struct Position
{
    float x = 0.f, y = 0.f, z = 0.f;
};

struct InputState
{
    float moveX = 0.f;
    float moveZ = 0.f;
};
