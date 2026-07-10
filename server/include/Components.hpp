#pragma once
#include <cstdint>

struct CNetId
{
    uint32_t value = 0;
};

struct CTransform
{
    float x = 0.f, y = 0.f, z = 0.f;
};

struct CInput
{
    float moveX = 0.f;
    float moveZ = 0.f;
};
