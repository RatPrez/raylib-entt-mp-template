#pragma once
#include <array>
#include <unordered_map>
#include <vector>

#include <entt/entity/entity.hpp>

#include "shared/Net.hpp"
#include "shared/Packets.hpp"

struct WorldContext;

using PacketHandler = void (*)(WorldContext &, HSteamNetConnection, const void *);

struct PacketHandlerEntry
{
    PacketHandler fn = nullptr;
    size_t size = 0;
};

struct OutMessage
{
    NetMessage msg;
    HSteamNetConnection target = k_HSteamNetConnection_Invalid;
    bool toAll = false;
};

struct NetContext
{
    ISteamNetworkingSockets *sockets = nullptr;
    HSteamListenSocket listenSocket = k_HSteamListenSocket_Invalid;
    HSteamNetPollGroup pollGroup = k_HSteamNetPollGroup_Invalid;

    std::vector<HSteamNetConnection> clients;
    std::unordered_map<HSteamNetConnection, entt::entity> connToEnt;

    std::vector<OutMessage> outbox;
    std::array<PacketHandlerEntry, (size_t)eClientPacket::Count> handlers{};

    template <typename T> void send(HSteamNetConnection conn, const T &packet, eNetMode mode = eNetMode::Reliable)
    {
        outbox.push_back({MakeMessage(packet, mode), conn, false});
    }

    template <typename T> void broadcast(const T &packet, eNetMode mode = eNetMode::Reliable)
    {
        outbox.push_back({MakeMessage(packet, mode), k_HSteamNetConnection_Invalid, true});
    }

    template <typename T> void reg(PacketHandler fn) { handlers[(size_t)T::kType] = {fn, sizeof(T)}; }
};
