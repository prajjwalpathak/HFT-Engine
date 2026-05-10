#pragma once

#include <cstdint>

namespace hft {

enum class MessageType : uint8_t {
    NewOrder = 1,
    CancelOrder = 2
};

struct alignas(64) NewOrderMessage {

    MessageType type;

    uint64_t order_id;

    uint32_t price;

    uint32_t quantity;

    uint64_t timestamp;

    uint8_t side;
};

struct alignas(64) CancelOrderMessage {

    MessageType type;

    uint64_t order_id;
};

static_assert(sizeof(NewOrderMessage) == 64, "Packet must remain cache-line sized");

}