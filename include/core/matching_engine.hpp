#pragma once

#include <iostream>
#include "core/order_book.hpp"
#include "memory/order_pool.hpp"
#include "core/engine_stats.hpp"

namespace hft {

class MatchingEngine {
public:
    OrderBook book;

    EngineStats stats;

    MatchingEngine(OrderPool& pool)
    : book(pool) {}

    void process_order(uint32_t incoming_index) {
        ++stats.orders_processed;

        Order& incoming = book.pool.get(incoming_index);

        if (incoming.side == Side::Buy) {
            match_buy(incoming_index);
        } else {
            match_sell(incoming_index);
        }

        // If still remaining → add to book
        if (!incoming.is_filled()) {
            book.add_order(incoming_index);
            ++stats.active_orders;
        }
        else {
            book.pool.free(incoming_index);
        }
    }

private:

    void match_buy(uint32_t incoming_index) {

        Order& incoming = book.pool.get(incoming_index);

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
                    book.pool.get(resting_index);

                Quantity traded =
                    std::min(incoming.quantity,
                             resting.quantity);
                
                ++stats.trades_executed;

                incoming.reduce(traded);
                resting.reduce(traded);

                limit.total_quantity -= traded;

                // std::cout << "TRADE: "
                //           << traded
                //           << " @ "
                //           << resting.price
                //           << "\n";

                // Remove filled resting order
                if (__builtin_expect(resting.is_filled(), 0)) {
                    limit.pop_front(book.pool);
                    book.pool.free(resting_index);
                    --stats.active_orders;
                }
            }

            // Remove empty price level
            if (limit.empty()) {
                book.asks.erase(best_ask_it);
            }
        }
    }

    void match_sell(uint32_t incoming_index) {

        Order& incoming = book.pool.get(incoming_index);

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
                    book.pool.get(resting_index);

                Quantity traded =
                    std::min(incoming.quantity,
                             resting.quantity);

                ++stats.trades_executed;

                incoming.reduce(traded);
                resting.reduce(traded);

                limit.total_quantity -= traded;

                // std::cout << "TRADE: "
                //           << traded
                //           << " @ "
                //           << resting.price
                //           << "\n";

                if (__builtin_expect(resting.is_filled(), 0)) {
                    limit.pop_front(book.pool);
                    book.pool.free(resting_index);
                    --stats.active_orders;
                }
            }

            if (limit.empty()) {
                book.bids.erase(best_bid_it);
            }
        }
    }
};

} // namespace hft