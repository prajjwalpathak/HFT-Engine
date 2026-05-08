#pragma once

#include <iostream>
#include "core/order_book.hpp"

namespace hft {

class MatchingEngine {
public:
    OrderBook book;

    MatchingEngine(Order* pool)
        : book(pool) {}

    void process_order(uint32_t incoming_index) {
        Order& incoming = book.order_pool[incoming_index];

        if (incoming.side == Side::Buy) {
            match_buy(incoming_index);
        } else {
            match_sell(incoming_index);
        }

        // If still remaining → add to book
        if (!incoming.is_filled()) {
            book.add_order(incoming_index);
        }
    }

private:

    void match_buy(uint32_t incoming_index) {
        Order& incoming = book.order_pool[incoming_index];

        while (!incoming.is_filled() && !book.asks.empty()) {

            auto best_ask_it = book.asks.begin();

            Price best_ask_price = best_ask_it->first;

            // Cannot match anymore
            if (incoming.price < best_ask_price) {
                break;
            }

            Limit& limit = best_ask_it->second;

            while (!incoming.is_filled() && !limit.empty()) {

                uint32_t resting_index = limit.head;

                Order& resting =
                    book.order_pool[resting_index];

                Quantity traded =
                    std::min(incoming.quantity,
                             resting.quantity);

                incoming.reduce(traded);
                resting.reduce(traded);

                limit.total_quantity -= traded;

                std::cout << "TRADE: "
                          << traded
                          << " @ "
                          << resting.price
                          << "\n";

                // Remove filled resting order
                if (resting.is_filled()) {
                    limit.pop_front(book.order_pool);
                }
            }

            // Remove empty price level
            if (limit.empty()) {
                book.asks.erase(best_ask_it);
            }
        }
    }

    void match_sell(uint32_t incoming_index) {
        Order& incoming = book.order_pool[incoming_index];

        while (!incoming.is_filled() && !book.bids.empty()) {

            auto best_bid_it = book.bids.begin();

            Price best_bid_price = best_bid_it->first;

            // Cannot match anymore
            if (incoming.price > best_bid_price) {
                break;
            }

            Limit& limit = best_bid_it->second;

            while (!incoming.is_filled() && !limit.empty()) {

                uint32_t resting_index = limit.head;

                Order& resting =
                    book.order_pool[resting_index];

                Quantity traded =
                    std::min(incoming.quantity,
                             resting.quantity);

                incoming.reduce(traded);
                resting.reduce(traded);

                limit.total_quantity -= traded;

                std::cout << "TRADE: "
                          << traded
                          << " @ "
                          << resting.price
                          << "\n";

                if (resting.is_filled()) {
                    limit.pop_front(book.order_pool);
                }
            }

            if (limit.empty()) {
                book.bids.erase(best_bid_it);
            }
        }
    }
};

} // namespace hft