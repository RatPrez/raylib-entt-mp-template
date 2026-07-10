#pragma once
#include <array>
#include <cstdint>
#include <cstring>
#include <type_traits>

#include <steam/steamnetworkingsockets.h>

constexpr uint16_t kServerPort = 27020;
constexpr size_t kMaxPacketSize = 256;

enum class eNetMode : uint8_t
{
    Reliable,
    Unreliable,
};

inline int ToSendFlags(eNetMode mode)
{
    return mode == eNetMode::Reliable ? k_nSteamNetworkingSend_Reliable : k_nSteamNetworkingSend_Unreliable;
}

struct NetMessage
{
    std::array<uint8_t, kMaxPacketSize> data{};
    uint16_t size = 0;
    eNetMode mode = eNetMode::Reliable;
};

template <typename T> NetMessage MakeMessage(const T &packet, eNetMode mode)
{
    static_assert(std::is_trivially_copyable_v<T>, "packet must be trivially copyable");
    static_assert(sizeof(T) <= kMaxPacketSize, "packet too large");

    NetMessage msg{};
    msg.size = sizeof(T);
    msg.mode = mode;
    memcpy(msg.data.data(), &packet, sizeof(T));
    return msg;
}
