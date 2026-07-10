#include "systems/Draw2D.hpp"

void System::Draw2D(WorldContext &ctx)
{
    DrawFPS(10, 10);

    const char *status = "disconnected";
    if (ctx.net.state == eConnState::Connecting) {
        status = "connecting...";
    }
    if (ctx.net.state == eConnState::Connected) {
        status = TextFormat("connected (netId %u)", ctx.net.localNetId);
    }
    DrawText(status, 10, 34, 20, RAYWHITE);
}
