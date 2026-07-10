#include "systems/Network.hpp"

void System::NetReceive(WorldContext &ctx)
{
    ctx.net.sockets->RunCallbacks();

    if (ctx.net.state != eConnState::Connected) {
        return;
    }

    ISteamNetworkingMessage *msgs[64];
    int count = ctx.net.sockets->ReceiveMessagesOnConnection(ctx.net.conn, msgs, 64);
    for (int i = 0; i < count; i++) {
        ISteamNetworkingMessage *msg = msgs[i];

        if (msg->m_cbSize < (int)sizeof(eServerPacket)) {
            msg->Release();
            continue;
        }

        auto type = *static_cast<const eServerPacket *>(msg->m_pData);
        size_t idx = (size_t)type;

        if (idx < ctx.net.handlers.size() && ctx.net.handlers[idx].fn != nullptr &&
            (size_t)msg->m_cbSize >= ctx.net.handlers[idx].size) {
            ctx.net.handlers[idx].fn(ctx, msg->m_pData);
        }

        msg->Release();
    }
}

void System::NetSend(WorldContext &ctx)
{
    if (ctx.net.state != eConnState::Connected) {
        ctx.net.outbox.clear();
        return;
    }

    for (const auto &msg : ctx.net.outbox) {
        ctx.net.sockets->SendMessageToConnection(ctx.net.conn, msg.data.data(), msg.size, ToSendFlags(msg.mode),
                                                 nullptr);
    }
    ctx.net.outbox.clear();
}
