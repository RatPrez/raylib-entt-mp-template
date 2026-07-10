#include <cstdio>

#include <steam/isteamnetworkingutils.h>

#include "Components.hpp"
#include "core/Client.hpp"
#include "systems/Draw2D.hpp"
#include "systems/Draw3D.hpp"
#include "systems/Network.hpp"
#include "systems/Tick.hpp"

Client *Client::s_instance = nullptr;

namespace
{
    entt::entity GetOrCreateActor(WorldContext &ctx, uint32_t netId)
    {
        auto it = ctx.net.netToEnt.find(netId);
        if (it != ctx.net.netToEnt.end()) {
            return it->second;
        }

        auto entity = ctx.registry.create();
        ctx.registry.emplace<CNetId>(entity, netId);
        ctx.registry.emplace<CTransform>(entity);
        ctx.registry.emplace<CCube>(entity, CCube{.color = netId == ctx.net.localNetId ? RED : BLUE});
        if (netId == ctx.net.localNetId) {
            ctx.registry.emplace<CLocalPlayer>(entity);
        }
        ctx.net.netToEnt[netId] = entity;
        return entity;
    }

    void HandleWelcome(WorldContext &ctx, const void *data)
    {
        const auto *pkt = static_cast<const SPacketWelcome *>(data);
        ctx.net.localNetId = pkt->netId;
        printf("welcome: netId %u, server tick rate %u\n", pkt->netId, pkt->tickRate);
    }

    void HandleSpawn(WorldContext &ctx, const void *data)
    {
        const auto *pkt = static_cast<const SPacketSpawn *>(data);
        auto entity = GetOrCreateActor(ctx, pkt->netId);
        ctx.registry.get<CTransform>(entity).position = {pkt->x, pkt->y, pkt->z};
    }

    void HandleDespawn(WorldContext &ctx, const void *data)
    {
        const auto *pkt = static_cast<const SPacketDespawn *>(data);
        auto it = ctx.net.netToEnt.find(pkt->netId);
        if (it == ctx.net.netToEnt.end()) {
            return;
        }

        ctx.registry.destroy(it->second);
        ctx.net.netToEnt.erase(it);
    }

    void HandleState(WorldContext &ctx, const void *data)
    {
        const auto *pkt = static_cast<const SPacketState *>(data);
        auto entity = GetOrCreateActor(ctx, pkt->netId);
        ctx.registry.get<CTransform>(entity).position = {pkt->x, pkt->y, pkt->z};
    }
} // namespace

Client::Client()
{
    s_instance = this;

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    InitWindow(1280, 720, "template-mp client");
    SetTargetFPS(144);

    m_ctx.camera.position = {10.f, 10.f, 10.f};
    m_ctx.camera.target = {0.f, 0.f, 0.f};
    m_ctx.camera.up = {0.f, 1.f, 0.f};
    m_ctx.camera.fovy = 60.f;
    m_ctx.camera.projection = CAMERA_PERSPECTIVE;

    SteamDatagramErrMsg errMsg;
    if (!GameNetworkingSockets_Init(nullptr, errMsg)) {
        fprintf(stderr, "GameNetworkingSockets_Init failed: %s\n", errMsg);
        exit(1);
    }
    m_ctx.net.sockets = SteamNetworkingSockets();

    m_ctx.net.reg<SPacketWelcome>(HandleWelcome);
    m_ctx.net.reg<SPacketSpawn>(HandleSpawn);
    m_ctx.net.reg<SPacketDespawn>(HandleDespawn);
    m_ctx.net.reg<SPacketState>(HandleState);

    connect("127.0.0.1");
}

Client::~Client()
{
    if (m_ctx.net.conn != k_HSteamNetConnection_Invalid) {
        m_ctx.net.sockets->CloseConnection(m_ctx.net.conn, 0, "client quit", false);
    }
    GameNetworkingSockets_Kill();
    CloseWindow();
    s_instance = nullptr;
}

void Client::connect(const char *address)
{
    SteamNetworkingIPAddr addr;
    addr.Clear();
    addr.ParseString(address);
    addr.m_port = kServerPort;

    SteamNetworkingConfigValue_t opt;
    opt.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void *)onNetStatusChanged);

    m_ctx.net.conn = m_ctx.net.sockets->ConnectByIPAddress(addr, 1, &opt);
    m_ctx.net.state = eConnState::Connecting;
}

void Client::onNetStatusChanged(SteamNetConnectionStatusChangedCallback_t *info)
{
    auto &net = s_instance->m_ctx.net;

    switch (info->m_info.m_eState) {
    case k_ESteamNetworkingConnectionState_Connected:
        net.state = eConnState::Connected;
        printf("connected to server\n");
        break;

    case k_ESteamNetworkingConnectionState_ClosedByPeer:
    case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
        net.sockets->CloseConnection(info->m_hConn, 0, nullptr, false);
        net.conn = k_HSteamNetConnection_Invalid;
        net.state = eConnState::Disconnected;
        printf("disconnected from server\n");
        break;

    default:
        break;
    }
}

void Client::run()
{
    while (!WindowShouldClose()) {
        m_ctx.dt = GetFrameTime();
        m_ctx.gameTime += m_ctx.dt;

        System::NetReceive(m_ctx);
        System::Tick(m_ctx);
        System::NetSend(m_ctx);

        BeginDrawing();
        ClearBackground(Color{24, 24, 28, 255});

        BeginMode3D(m_ctx.camera);
        System::Draw3D(m_ctx);
        EndMode3D();

        System::Draw2D(m_ctx);
        EndDrawing();
    }
}
