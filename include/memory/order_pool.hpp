#pragma once

#include <vector>
#include <cstdint>
#include <stdexcept>
#include "core/order.hpp"

namespace hft {

class OrderPool {
public:

    static constexpr uint32_t INVALID_INDEX = 0;

    explicit OrderPool(uint32_t capacity);

    // Allocate one slot
    uint32_t allocate();

    // Return slot back to pool
    void free(uint32_t index);

    // Access order
    Order& get(uint32_t index);

    const Order& get(uint32_t index) const;

    uint32_t capacity() const;

    uint32_t available() const;

private:

    std::vector<Order> orders_;

    // Free-list linked through indices
    std::vector<uint32_t> free_list_;

    uint32_t free_head_;
};

} // namespace hft