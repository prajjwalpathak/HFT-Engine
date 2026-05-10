#pragma once

#include "common/types.hpp"

namespace hft {

struct alignas(64) Order {

    OrderId order_id;

    Price price;

    Quantity quantity;

    uint32_t next;

    uint32_t prev;

    Timestamp timestamp;

    Side side;

    bool active = true;

    __attribute__((always_inline)) inline
    void reduce(Quantity qty) {
        quantity -= qty;
    }

    __attribute__((always_inline)) inline
    bool is_filled() const {
        return quantity == 0;
    }
};

static_assert(sizeof(Order) <= 64, "Order exceeds cache line size");

}