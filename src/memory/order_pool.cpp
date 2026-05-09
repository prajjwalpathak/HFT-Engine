#include "memory/order_pool.hpp"

namespace hft {

OrderPool::OrderPool(uint32_t capacity)
    : orders_(capacity + 1),
      free_list_(capacity + 1),
      free_head_(1)
{
    // Build free list:
    // 1 -> 2 -> 3 -> ... -> capacity

    for (uint32_t i = 1; i <= capacity; ++i) {

        if (i == capacity) {
            free_list_[i] = INVALID_INDEX;
        } else {
            free_list_[i] = i + 1;
        }
    }
}

uint32_t OrderPool::allocate() {

    if (free_head_ == INVALID_INDEX) {
        throw std::runtime_error("OrderPool exhausted");
    }

    uint32_t index = free_head_;

    free_head_ = free_list_[index];

    return index;
}

void OrderPool::free(uint32_t index) {

    free_list_[index] = free_head_;

    free_head_ = index;
}

Order& OrderPool::get(uint32_t index) {
    return orders_[index];
}

const Order& OrderPool::get(uint32_t index) const {
    return orders_[index];
}

uint32_t OrderPool::capacity() const {
    return static_cast<uint32_t>(orders_.size() - 1);
}

uint32_t OrderPool::available() const {

    uint32_t count = 0;

    uint32_t current = free_head_;

    while (current != INVALID_INDEX) {
        ++count;
        current = free_list_[current];
    }

    return count;
}

}