#pragma once

#include <cstdint>

namespace hft {

// Unique identifier for orders
using OrderId = uint64_t;

// Price represented as integer (e.g. Rs.100.50 → 10050)
using Price = uint32_t;

// Quantity of shares
using Quantity = uint32_t;

// Timestamp (nanoseconds or sequence-based)
using Timestamp = uint64_t;

// Side of order
enum class Side : uint8_t {
    Buy = 0,
    Sell = 1
};

}
