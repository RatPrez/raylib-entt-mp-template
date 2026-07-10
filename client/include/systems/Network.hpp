#pragma once
#include "core/WorldContext.hpp"

namespace System
{
    void NetReceive(WorldContext &ctx);
    void NetSend(WorldContext &ctx);
} // namespace System
