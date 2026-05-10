#pragma once

#include <cassert>
#include <unordered_set>
#include "core/order_book.hpp"

#ifdef ENABLE_VALIDATION

namespace hft {

class BookValidator {
public:

    static void validate(OrderBook& book) {

        validate_side(book.bids, book);

        validate_side(book.asks, book);
    }

private:

    template<typename MapType>
    static void validate_side(
        MapType& side,
        OrderBook& book
    ) {

        for (auto& [price, limit] : side) {

            validate_limit(limit, book);
        }
    }

    static void validate_limit(
        Limit& limit,
        OrderBook& book
    ) {

        uint32_t current = limit.head;

        Quantity computed_total = 0;

        std::unordered_set<uint32_t> visited;

        while (current != 0) {

            // Detect loops
            assert(
                visited.find(current)
                == visited.end()
            );

            visited.insert(current);

            Order& order =
                book.pool.get(current);

            // Active orders only
            assert(order.active);

            // Nonzero quantity
            assert(order.quantity > 0);

            computed_total += order.quantity;

            current = order.next;
        }

        // Verify quantity accounting
        assert(
            computed_total
            == limit.total_quantity
        );
    }
};

}

#endif