#include <iostream>

#include "memory/order_pool.hpp"
#include "core/matching_engine.hpp"
#include "common/timer.hpp"

int main() {

    using namespace hft;

    // Large enough pool for benchmark
    OrderPool pool(1'000'000);

    MatchingEngine engine(pool);

    std::cout << "Order Size: "
              << sizeof(Order)
              << " bytes\n";

    std::cout << "Limit Size: "
              << sizeof(Limit)
              << " bytes\n";

    Timer timer;

    constexpr uint32_t NUM_ORDERS = 100000;

    timer.start();

    for (uint32_t i = 1; i <= NUM_ORDERS; ++i) {

        auto idx = pool.allocate();

        auto& order = pool.get(idx);

        order = {
            .order_id = i,

            .price = static_cast<Price>(
                10000 + (i % 10)
            ),

            .quantity = 100,

            .next = 0,
            .prev = 0,

            .timestamp = i,

            .side = (i % 2 == 0)
                ? Side::Buy
                : Side::Sell,

            .active = true
        };

        engine.process_order(idx);
    }

    auto elapsed = timer.elapsed_ns();

    std::cout << "\nBenchmark Results\n";

    std::cout << "Elapsed Time: "
              << elapsed
              << " ns\n";

    std::cout << "Orders/sec: "
              << (1'000'000'000ULL * NUM_ORDERS / elapsed)
              << "\n";

    std::cout << "\nEngine Statistics\n";

    std::cout << "Orders Processed: "
              << engine.stats.orders_processed.load()
              << "\n";

    std::cout << "Trades Executed: "
              << engine.stats.trades_executed.load()
              << "\n";

    std::cout << "Orders Cancelled: "
              << engine.stats.orders_cancelled.load()
              << "\n";

    std::cout << "Active Orders: "
              << engine.stats.active_orders.load()
              << "\n";

    std::cout << "Pool Capacity: "
              << pool.capacity()
              << "\n";

    std::cout << "Pool Available: "
              << pool.available()
              << "\n";

    return 0;
}