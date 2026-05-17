#pragma once

#include <iostream>

#include <chrono>

#include "core/order_book.hpp"
#include "memory/order_pool.hpp"
#include "core/engine_stats.hpp"
#include "network/udp_publisher.hpp"
#include "network/market_data.hpp"

namespace hft {

class MatchingEngine {
public:

    OrderBook book;

    EngineStats stats;

    UDPPublisher& publisher;

    MatchingEngine(OrderPool& pool, UDPPublisher& publisher): book(pool), publisher(publisher){}

    uint64_t next_trade_id = 1;

    void process_order(uint32_t incoming_index) {
        auto start = std::chrono::high_resolution_clock::now();

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
            publish_top_of_book();
            ++stats.active_orders;
        }
        else {
            book.pool.free(incoming_index);
        }

        auto end = std::chrono::high_resolution_clock::now();

        uint64_t latency = std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count();

        stats.total_latency_ns += latency;

        uint64_t current_max = stats.max_latency_ns.load();

        while(latency > current_max && stats.max_latency_ns.compare_exchange_weak(current_max,
                    latency
                )){}

        if(stats.orders_processed.load()% 50000 == 0){

            double avg_latency = static_cast<double>(stats.total_latency_ns.load())/
                stats.orders_processed.load();

            std::cout
                << "Orders: "
                << stats.orders_processed.load()
                << "\n";

            std::cout
                << "Avg latency(ns): "
                << avg_latency
                << "\n";

            std::cout
                << "Max latency(ns): "
                << stats.max_latency_ns.load()
                << "\n";

            stats.max_latency_ns = 0;
        }
    }

private:

    uint64_t market_sequence = 0;

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

                Order& resting = book.pool.get(resting_index);

                Quantity traded = std::min(incoming.quantity, resting.quantity);
                
                ++stats.trades_executed;

                incoming.reduce(traded);
                resting.reduce(traded);

                TradeMessage trade {
                    .type = MarketDataType::Trade,

                    .sequence_number = ++market_sequence,

                    .trade_id = ++next_trade_id,

                    .buy_order_id =
                        incoming.side == Side::Buy
                            ? incoming.order_id
                            : resting.order_id,

                    .sell_order_id =
                        incoming.side == Side::Sell
                            ? incoming.order_id
                            : resting.order_id,

                    .price = resting.price,

                    .quantity = traded,

                    .aggressor_side = static_cast<uint8_t>(Side::Buy),

                    .timestamp = incoming.timestamp
                };

                publisher.publish(&trade, sizeof(trade));
                publish_top_of_book();

                limit.total_quantity -= traded;

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

                Order& resting = book.pool.get(resting_index);

                Quantity traded = std::min(incoming.quantity, resting.quantity);

                ++stats.trades_executed;

                incoming.reduce(traded);
                resting.reduce(traded);

                TradeMessage trade {

                    .type = MarketDataType::Trade,

                    .sequence_number = ++market_sequence,

                    .trade_id = next_trade_id++,

                    .buy_order_id = resting.order_id,

                    .sell_order_id = incoming.order_id,

                    .price = resting.price,

                    .quantity = traded,

                    .aggressor_side = static_cast<uint8_t>(Side::Sell),

                    .timestamp = incoming.timestamp
                };

                publisher.publish(&trade, sizeof(trade));
                publish_top_of_book();

                limit.total_quantity -= traded;

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

    void publish_top_of_book() {
        uint32_t best_bid_price = 0;
        uint32_t best_bid_quantity = 0;

        if (!book.bids.empty()) {
            auto& best_bid = book.bids.begin()->second;
            best_bid_price = book.bids.begin()->first;
            best_bid_quantity = best_bid.total_quantity;
        }

        uint32_t best_ask_price = 0;
        uint32_t best_ask_quantity = 0;

        if (!book.asks.empty()) {
            auto& best_ask = book.asks.begin()->second;
            best_ask_price = book.asks.begin()->first;
            best_ask_quantity = best_ask.total_quantity;
        }

        TopOfBookMessage tob {

            .type = MarketDataType::TopOfBook,

            .sequence_number = ++market_sequence,

            .best_bid_price = best_bid_price,

            .best_bid_quantity = best_bid_quantity,

            .best_ask_price = best_ask_price,

            .best_ask_quantity = best_ask_quantity,

            .timestamp = next_trade_id
        };

        publisher.publish(&tob, sizeof(tob));
    }
};

} // namespace hft