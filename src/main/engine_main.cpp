#include <iostream>
#include "core/matching_engine.hpp"

int main() {

    using namespace hft;

    Order pool[10];

    MatchingEngine engine(pool);

    pool[1] = {
        1,
        10000,
        50,
        0,0,
        Side::Buy,
        1
    };

    pool[2] = {
        2,
        10000,
        30,
        0,0,
        Side::Buy,
        2
    };

    engine.process_order(1);
    engine.process_order(2);

    std::cout
        << "Best Bid Before Cancel: "
        << engine.book.best_bid()
        << "\n";

    engine.book.cancel_order(1);

    std::cout
        << "Cancel completed\n";

    return 0;
}