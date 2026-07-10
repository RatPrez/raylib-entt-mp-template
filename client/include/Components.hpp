#pragma once
#include <cstdint>
#include <raylib.h>

struct CNetId
{
    uint32_t value = 0;
};

struct CTransform
{
    Vector3 position{};
};

struct CCube
{
    Vector3 size{1.f, 1.f, 1.f};
    Color color = RED;
};

struct CLocalPlayer
{
};
