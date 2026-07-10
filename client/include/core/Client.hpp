#pragma once
#include "core/WorldContext.hpp"

class Client
{
public:
    Client();
    ~Client();
    void run();

private:
    void connect(const char *address);
    static void onNetStatusChanged(SteamNetConnectionStatusChangedCallback_t *info);

    WorldContext m_ctx;

    static Client *s_instance;
};
