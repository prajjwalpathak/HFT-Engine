#include <iostream>
#include "core/order.hpp"
#include "core/order_book.hpp"

int main() {
    using namespace hft;

    Order pool[10];

    pool[1] = {1, 10000, 50, 0, 0, Side::Buy, 1};
    pool[2] = {2, 10100, 30, 0, 0, Side::Sell, 2};
    pool[3] = {3, 10000, 20, 0, 0, Side::Buy, 3};

    OrderBook book(pool);

    book.add_order(1);
    book.add_order(2);
    book.add_order(3);

    std::cout << "Best Bid: " << book.best_bid() << "\n";
    std::cout << "Best Ask: " << book.best_ask() << "\n";

    return 0;
}
