#pragma once
#include <cstdint>

enum class eClientPacket : uint8_t
{
    None,
    Input,
    Count,
};

enum class eServerPacket : uint8_t
{
    None,
    Welcome,
    Spawn,
    Despawn,
    State,
    Count,
};

#pragma pack(push, 1)

struct CPacketInput
{
    static constexpr eClientPacket kType = eClientPacket::Input;
    eClientPacket type = kType;
    float moveX = 0.f;
    float moveZ = 0.f;
};

struct SPacketWelcome
{
    static constexpr eServerPacket kType = eServerPacket::Welcome;
    eServerPacket type = kType;
    uint32_t netId = 0;
    uint32_t tickRate = 0;
};

struct SPacketSpawn
{
    static constexpr eServerPacket kType = eServerPacket::Spawn;
    eServerPacket type = kType;
    uint32_t netId = 0;
    float x = 0.f, y = 0.f, z = 0.f;
};

struct SPacketDespawn
{
    static constexpr eServerPacket kType = eServerPacket::Despawn;
    eServerPacket type = kType;
    uint32_t netId = 0;
};

struct SPacketState
{
    static constexpr eServerPacket kType = eServerPacket::State;
    eServerPacket type = kType;
    uint32_t netId = 0;
    float x = 0.f, y = 0.f, z = 0.f;
};

#pragma pack(pop)
