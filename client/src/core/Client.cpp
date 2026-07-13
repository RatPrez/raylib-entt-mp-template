#include <cstdio>

#include <steam/isteamnetworkingutils.h>

#include "core/Client.hpp"

#include "systems/ConnText.hpp"
#include "systems/Input.hpp"
#include "systems/Interp.hpp"
#include "systems/Movement.hpp"
#include "systems/Network.hpp"
#include "systems/Render.hpp"

Client *Client::s_instance = nullptr;

void registerPacketHandlers(WorldContext &ctx);

// =================== GAME LOOPS ===================

void Client::tick()
{
    System::Input(m_ctx);
    System::Movement(m_ctx);
    System::Interp(m_ctx);
}

void Client::lateTick() {}

void Client::draw2D() { System::ConnText(m_ctx); }

void Client::draw3D() { System::Render(m_ctx); }

// =================== MAIN ===================

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

    registerPacketHandlers(m_ctx);

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

        System::NetReceive(m_ctx); // receive packets before anything
        tick();

        BeginDrawing();
        ClearBackground(Color{24, 24, 28, 255});

        BeginMode3D(m_ctx.camera);
        draw3D();
        EndMode3D();

        draw2D();
        EndDrawing();

        lateTick();
        System::NetSend(m_ctx); // send packets last
    }
}
