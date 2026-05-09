#pragma once

#include <atomic>
#include <cstdint>

namespace hft {

struct alignas(64) EngineStats {

    std::atomic<uint64_t> orders_processed = 0;

    std::atomic<uint64_t> trades_executed = 0;

    std::atomic<uint64_t> orders_cancelled = 0;

    std::atomic<uint64_t> active_orders = 0;
};

}