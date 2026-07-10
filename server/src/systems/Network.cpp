#include "systems/Network.hpp"

void System::NetReceive(WorldContext &ctx)
{
    ctx.net.sockets->RunCallbacks();

    ISteamNetworkingMessage *msgs[128];
    int count = ctx.net.sockets->ReceiveMessagesOnPollGroup(ctx.net.pollGroup, msgs, 128);
    for (int i = 0; i < count; i++) {
        ISteamNetworkingMessage *msg = msgs[i];

        if (msg->m_cbSize < (int)sizeof(eClientPacket)) {
            msg->Release();
            continue;
        }

        auto type = *static_cast<const eClientPacket *>(msg->m_pData);
        size_t idx = (size_t)type;

        if (idx < ctx.net.handlers.size() && ctx.net.handlers[idx].fn != nullptr &&
            (size_t)msg->m_cbSize >= ctx.net.handlers[idx].size) {
            ctx.net.handlers[idx].fn(ctx, msg->m_conn, msg->m_pData);
        }

        msg->Release();
    }
}

void System::NetSend(WorldContext &ctx)
{
    for (const auto &out : ctx.net.outbox) {
        if (out.toAll) {
            for (auto conn : ctx.net.clients) {
                ctx.net.sockets->SendMessageToConnection(conn, out.msg.data.data(), out.msg.size,
                                                         ToSendFlags(out.msg.mode), nullptr);
            }
        } else {
            ctx.net.sockets->SendMessageToConnection(out.target, out.msg.data.data(), out.msg.size,
                                                     ToSendFlags(out.msg.mode), nullptr);
        }
    }
    ctx.net.outbox.clear();
}
