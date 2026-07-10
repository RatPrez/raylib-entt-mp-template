#pragma once
#include "shared/Components.hpp"

constexpr float kMoveSpeed = 5.f;

inline void ApplyMovement(Position &position, const InputState &input, float dt)
{
    position.x += input.moveX * kMoveSpeed * dt;
    position.z += input.moveZ * kMoveSpeed * dt;
}
