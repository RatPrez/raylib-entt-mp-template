#include <chrono>
#include <cstdio>
#include <thread>

#include <steam/isteamnetworkingutils.h>

#include "Components.hpp"
#include "core/Server.hpp"
#include "systems/Network.hpp"
#include "systems/Tick.hpp"

Server *Server::s_instance = nullptr;

namespace
{
    void HandleInput(WorldContext &ctx, HSteamNetConnection conn, const void *data)
    {
        auto it = ctx.net.connToEnt.find(conn);
        if (it == ctx.net.connToEnt.end()) {
            return;
        }

        const auto *pkt = static_cast<const CPacketInput *>(data);
        auto &input = ctx.registry.get<CInput>(it->second);
        input.moveX = pkt->moveX;
        input.moveZ = pkt->moveZ;
    }
} // namespace

Server::Server(uint32_t tickRate) : m_tickRate(tickRate)
{
    s_instance = this;
    setup();
}

Server::~Server()
{
    cleanup();
    s_instance = nullptr;
}

void Server::setup()
{
    SteamDatagramErrMsg errMsg;
    if (!GameNetworkingSockets_Init(nullptr, errMsg)) {
        fprintf(stderr, "GameNetworkingSockets_Init failed: %s\n", errMsg);
        exit(1);
    }

    m_ctx.net.sockets = SteamNetworkingSockets();

    SteamNetworkingIPAddr addr;
    addr.Clear();
    addr.m_port = kServerPort;

    SteamNetworkingConfigValue_t opt;
    opt.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void *)onNetStatusChanged);

    m_ctx.net.listenSocket = m_ctx.net.sockets->CreateListenSocketIP(addr, 1, &opt);
    if (m_ctx.net.listenSocket == k_HSteamListenSocket_Invalid) {
        fprintf(stderr, "failed to listen on port %u\n", kServerPort);
        exit(1);
    }
    m_ctx.net.pollGroup = m_ctx.net.sockets->CreatePollGroup();

    m_ctx.net.reg<CPacketInput>(HandleInput);

    printf("server listening on port %u, tick rate %u\n", kServerPort, m_tickRate);
}

void Server::cleanup()
{
    for (auto conn : m_ctx.net.clients) {
        m_ctx.net.sockets->CloseConnection(conn, 0, "server shutdown", false);
    }
    m_ctx.net.sockets->DestroyPollGroup(m_ctx.net.pollGroup);
    m_ctx.net.sockets->CloseListenSocket(m_ctx.net.listenSocket);
    GameNetworkingSockets_Kill();
}

void Server::onConnect(HSteamNetConnection conn)
{
    auto &net = m_ctx.net;
    net.sockets->SetConnectionPollGroup(conn, net.pollGroup);
    net.clients.push_back(conn);

    uint32_t netId = m_ctx.nextNetId++;
    auto entity = m_ctx.registry.create();
    m_ctx.registry.emplace<CNetId>(entity, netId);
    m_ctx.registry.emplace<CTransform>(entity);
    m_ctx.registry.emplace<CInput>(entity);
    net.connToEnt[conn] = entity;

    net.send(conn, SPacketWelcome{.netId = netId, .tickRate = m_tickRate});

    for (auto [other, otherNetId, transform] : m_ctx.registry.view<CNetId, CTransform>().each()) {
        net.send(conn, SPacketSpawn{.netId = otherNetId.value, .x = transform.x, .y = transform.y, .z = transform.z});
    }

    net.broadcast(SPacketSpawn{.netId = netId});
    printf("client connected: netId %u\n", netId);
}

void Server::onDisconnect(HSteamNetConnection conn)
{
    auto &net = m_ctx.net;
    auto it = net.connToEnt.find(conn);
    if (it != net.connToEnt.end()) {
        uint32_t netId = m_ctx.registry.get<CNetId>(it->second).value;
        m_ctx.registry.destroy(it->second);
        net.connToEnt.erase(it);
        net.broadcast(SPacketDespawn{.netId = netId});
        printf("client disconnected: netId %u\n", netId);
    }

    std::erase(net.clients, conn);
    net.sockets->CloseConnection(conn, 0, nullptr, false);
}

void Server::onNetStatusChanged(SteamNetConnectionStatusChangedCallback_t *info)
{
    auto &self = *s_instance;

    switch (info->m_info.m_eState) {
    case k_ESteamNetworkingConnectionState_Connecting:
        if (self.m_ctx.net.sockets->AcceptConnection(info->m_hConn) != k_EResultOK) {
            self.m_ctx.net.sockets->CloseConnection(info->m_hConn, 0, nullptr, false);
        }
        break;

    case k_ESteamNetworkingConnectionState_Connected:
        self.onConnect(info->m_hConn);
        break;

    case k_ESteamNetworkingConnectionState_ClosedByPeer:
    case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
        self.onDisconnect(info->m_hConn);
        break;

    default:
        break;
    }
}

void Server::run()
{
    m_running = true;

    const auto tickDuration = std::chrono::duration<double>(1.0 / m_tickRate);
    const auto start = std::chrono::steady_clock::now();
    m_ctx.tickDt = (float)tickDuration.count();

    while (m_running) {
        auto tickStart = std::chrono::steady_clock::now();
        m_ctx.gameTime = std::chrono::duration<float>(tickStart - start).count();

        System::NetReceive(m_ctx);
        System::Tick(m_ctx);
        System::NetSend(m_ctx);

        m_ctx.tick++;

        auto elapsed = std::chrono::steady_clock::now() - tickStart;
        auto sleepTime = std::chrono::duration_cast<std::chrono::steady_clock::duration>(tickDuration) - elapsed;
        if (sleepTime > std::chrono::steady_clock::duration::zero()) {
            std::this_thread::sleep_for(sleepTime);
        }
    }
}
