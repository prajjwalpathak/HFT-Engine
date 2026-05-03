#pragma once

#include <map>
#include "core/limit.hpp"

namespace hft {

class OrderBook {
public:
    // BUY side (highest price first)
    std::map<Price, Limit, std::greater<>> bids;

    // SELL side (lowest price first)
    std::map<Price, Limit, std::less<>> asks;

    // Order pool (temporary for now)
    Order* order_pool;

    OrderBook(Order* pool) : order_pool(pool) {}

    // Add a limit order
    void add_order(uint32_t order_index) {
        Order& order = order_pool[order_index];

        if (order.side == Side::Buy) {
            auto it = bids.find(order.price);

            if (it == bids.end()) {
                auto [new_it, _] = bids.emplace(order.price, Limit(order.price));
                it = new_it;
            }

            it->second.add_order(order_index, order_pool);
        } else {
            auto it = asks.find(order.price);

            if (it == asks.end()) {
                auto [new_it, _] = asks.emplace(order.price, Limit(order.price));
                it = new_it;
            }

            it->second.add_order(order_index, order_pool);
        }
    }

    // Get best bid
    Price best_bid() const {
        if (bids.empty()) return 0;
        return bids.begin()->first;
    }

    // Get best ask
    Price best_ask() const {
        if (asks.empty()) return 0;
        return asks.begin()->first;
    }
};

}
