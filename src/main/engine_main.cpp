#include <iostream>

#include "memory/order_pool.hpp"
#include "core/matching_engine.hpp"

int main() {

    using namespace hft;

    OrderPool pool(100);

    MatchingEngine engine(pool);

    // Allocate SELL order
    uint32_t sell_index = pool.allocate();

    auto& sell = pool.get(sell_index);

    sell = {
        1,
        10100,
        50,
        0,0,
        Side::Sell,
        1
    };

    engine.process_order(sell_index);

    // Allocate BUY order
    uint32_t buy_index = pool.allocate();

    auto& buy = pool.get(buy_index);

    buy = {
        2,
        10200,
        70,
        0,0,
        Side::Buy,
        2
    };

    engine.process_order(buy_index);

    return 0;
}