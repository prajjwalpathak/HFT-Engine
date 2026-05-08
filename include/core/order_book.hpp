#pragma once

#include <map>
#include <unordered_map>
#include "core/limit.hpp"

namespace hft {

class OrderBook {
public:
    // BUY side (highest price first)
    std::map<Price, Limit, std::greater<>> bids;

    // SELL side (lowest price first)
    std::map<Price, Limit, std::less<>> asks;

    // Order Lookup for Cancelling order
    std::unordered_map<OrderId, uint32_t> order_lookup;

    // Order pool (temporary for now)
    Order* order_pool;

    OrderBook(Order* pool) : order_pool(pool) {}

    // Add a limit order
    void add_order(uint32_t order_index) {
        Order& order = order_pool[order_index];
        order_lookup[order.order_id] = order_index;

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

    bool cancel_order(OrderId order_id) {

        auto it = order_lookup.find(order_id);

        if (it == order_lookup.end()) {
            return false;
        }

        uint32_t index = it->second;

        Order& order = order_pool[index];

        if (order.side == Side::Buy) {
            cancel_from_side(order, bids);
        } else {
            cancel_from_side(order, asks);
        }

        order_lookup.erase(it);

        return true;
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

private:

    template<typename BookSide>
    void cancel_from_side(Order& order, BookSide& side) {

        auto limit_it = side.find(order.price);

        if (limit_it == side.end()) {
            return;
        }

        Limit& limit = limit_it->second;

        if (order.prev != 0) {
            order_pool[order.prev].next = order.next;
        } else {
            limit.head = order.next;
        }

        if (order.next != 0) {
            order_pool[order.next].prev = order.prev;
        } else {
            limit.tail = order.prev;
        }

        limit.total_quantity -= order.quantity;

        if (limit.empty()) {
            side.erase(limit_it);
        }
    }
};

}
