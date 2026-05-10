#include <iostream>
#include <random>

#include "memory/order_pool.hpp"
#include "core/matching_engine.hpp"
#include "core/book_validator.hpp"
#include "common/timer.hpp"
#include "network/protocol.hpp"

int main() {

    #ifdef ENABLE_VALIDATION

    std::cout << "Running VALIDATION build\n";

    #else

    std::cout << "Running RELEASE build\n";

    #endif

    using namespace hft;

    constexpr uint32_t NUM_ORDERS = 100000;

    OrderPool pool(1000000);

    MatchingEngine engine(pool);

    // Random generators
    std::mt19937 rng(42);

    std::uniform_int_distribution<uint32_t>
        price_dist(10000, 10010);

    std::uniform_int_distribution<uint32_t>
        qty_dist(1, 1000);

    std::uniform_int_distribution<int>
        side_dist(0, 1);

    Timer timer;

    timer.start();

    for (uint32_t i = 1; i <= NUM_ORDERS; ++i) {

        auto idx = pool.allocate();

        auto& order = pool.get(idx);

        order = {
            .order_id = i,

            .price = static_cast<Price>(
                price_dist(rng)
            ),

            .quantity = static_cast<Quantity>(
                qty_dist(rng)
            ),

            .next = 0,

            .prev = 0,

            .timestamp = static_cast<Timestamp>(
                i
            ),

            .side = side_dist(rng)
                ? Side::Buy
                : Side::Sell,

            .active = true
        };

        engine.process_order(idx);

        // Periodic correctness validation
        #ifdef ENABLE_VALIDATION

        if ((i % 1000) == 0) {

            BookValidator::validate(
                engine.book
            );
        }

        #endif
    }

    auto elapsed = timer.elapsed_ns();

    #ifdef ENABLE_VALIDATION

    BookValidator::validate(
        engine.book
    );

    #endif

    std::cout << "\n=== Engine Memory Layout ===\n";

    std::cout << "Order Size: "
              << sizeof(Order)
              << " bytes\n";

    std::cout << "Limit Size: "
              << sizeof(Limit)
              << " bytes\n";

    std::cout << "\n=== Benchmark Results ===\n";

    std::cout << "Elapsed Time: "
              << elapsed
              << " ns\n";

    std::cout << "Orders/sec: "
              << (1000000000ULL * NUM_ORDERS / elapsed)
              << "\n";

    std::cout << "\n=== Engine Statistics ===\n";

    std::cout << "Orders Processed: "
              << engine.stats.orders_processed
              << "\n";

    std::cout << "Trades Executed: "
              << engine.stats.trades_executed
              << "\n";

    std::cout << "Orders Cancelled: "
              << engine.stats.orders_cancelled
              << "\n";

    std::cout << "Active Orders: "
              << engine.stats.active_orders
              << "\n";

    std::cout << "Pool Capacity: "
              << pool.capacity()
              << "\n";

    std::cout << "Pool Available: "
              << pool.available()
              << "\n";
    
    std::cout << "NewOrderMessage Size: " << sizeof(NewOrderMessage) << " bytes\n";

    return 0;
}