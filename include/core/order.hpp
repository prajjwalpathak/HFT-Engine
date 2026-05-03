#pragma once

#include "common/types.hpp"

namespace hft {

struct Order {
    OrderId  order_id;
    Price    price;
    Quantity quantity;

    uint32_t next;   // index in pool (for linked list)
    uint32_t prev;

    Side side;

    uint64_t timestamp;

    // Reduce quantity after match
    inline void reduce(Quantity qty) {
        quantity -= qty;
    }

    inline bool is_filled() const {
        return quantity == 0;
    }
};

}