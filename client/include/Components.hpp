#pragma once
#include <raylib.h>

#include "shared/Components.hpp"

struct Cube
{
    Vector3 size{1.f, 1.f, 1.f};
    Color color = RED;
};

struct LocalPlayer
{
};

struct TargetPosition
{
    float x = 0.f, y = 0.f, z = 0.f;
};
