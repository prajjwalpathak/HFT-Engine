#pragma once

#include <cstdint>

namespace hft {

enum class MarketDataType : uint8_t {

    Trade = 1,

    TopOfBook = 2
};

struct alignas(64) TradeMessage {

    MarketDataType type;

    uint64_t trade_id;

    uint64_t buy_order_id;

    uint64_t sell_order_id;

    uint32_t price;

    uint32_t quantity;

    uint64_t timestamp;
};

struct alignas(64) TopOfBookMessage {

    MarketDataType type;

    uint32_t best_bid_price;

    uint32_t best_bid_quantity;

    uint32_t best_ask_price;

    uint32_t best_ask_quantity;

    uint64_t timestamp;
};

static_assert(
    sizeof(TradeMessage) <= 64
);

static_assert(
    sizeof(TopOfBookMessage) <= 64
);

}