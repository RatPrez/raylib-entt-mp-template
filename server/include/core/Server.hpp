#pragma once
#include "core/WorldContext.hpp"

class Server
{
public:
    explicit Server(uint32_t tickRate);
    ~Server();
    void run();

private:
    void setup();
    void cleanup();
    void onConnect(HSteamNetConnection conn);
    void onDisconnect(HSteamNetConnection conn);
    static void onNetStatusChanged(SteamNetConnectionStatusChangedCallback_t *info);

    void tick();

    WorldContext m_ctx;
    uint32_t m_tickRate;
    bool m_running = false;

    static Server *s_instance;
};
