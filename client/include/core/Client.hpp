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

    void tick();
    void lateTick();
    void draw3D();
    void draw2D();

    WorldContext m_ctx;

    static Client *s_instance;
};
