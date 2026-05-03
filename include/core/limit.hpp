#pragma once

#include "core/order.hpp"

namespace hft {

struct Limit {
    Price price;

    uint32_t head;   // first order index
    uint32_t tail;   // last order index

    Quantity total_quantity;

    // Constructor
    Limit(Price p)
        : price(p), head(0), tail(0), total_quantity(0) {}

    // Add order to end (FIFO)
    void add_order(uint32_t order_index, Order* pool) {
        Order& order = pool[order_index];

        order.next = 0;
        order.prev = tail;

        if (tail != 0) {
            pool[tail].next = order_index;
        }

        tail = order_index;

        if (head == 0) {
            head = order_index;
        }

        total_quantity += order.quantity;
    }

    // Remove order from front (used in matching)
    uint32_t pop_front(Order* pool) {
        if (head == 0) return 0;

        uint32_t order_index = head;
        Order& order = pool[order_index];

        head = order.next;

        if (head != 0) {
            pool[head].prev = 0;
        } else {
            tail = 0;
        }

        total_quantity -= order.quantity;

        return order_index;
    }

    bool empty() const {
        return head == 0;
    }
};

}
