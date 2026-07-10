#pragma once
#include <array>
#include <unordered_map>
#include <vector>

#include <entt/entity/entity.hpp>

#include "shared/Net.hpp"
#include "shared/Packets.hpp"

struct WorldContext;

using PacketHandler = void (*)(WorldContext &, const void *);

struct PacketHandlerEntry
{
    PacketHandler fn = nullptr;
    size_t size = 0;
};

enum class eConnState
{
    Disconnected,
    Connecting,
    Connected,
};

struct NetContext
{
    ISteamNetworkingSockets *sockets = nullptr;
    HSteamNetConnection conn = k_HSteamNetConnection_Invalid;
    eConnState state = eConnState::Disconnected;

    uint32_t localNetId = 0;
    std::unordered_map<uint32_t, entt::entity> netToEnt;

    std::vector<NetMessage> outbox;
    std::array<PacketHandlerEntry, (size_t)eServerPacket::Count> handlers{};

    template <typename T> void send(const T &packet, eNetMode mode = eNetMode::Reliable)
    {
        outbox.push_back(MakeMessage(packet, mode));
    }

    template <typename T> void reg(PacketHandler fn) { handlers[(size_t)T::kType] = {fn, sizeof(T)}; }
};
