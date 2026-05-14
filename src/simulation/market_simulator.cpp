#include "simulation/market_simulator.hpp"

#include <chrono>
#include <thread>
#include <random>

#include "core/order.hpp"

namespace hft {

MarketSimulator::MarketSimulator(
    MatchingEngine& engine):
    engine(engine),
    running(false),
    shutdown(false),
    next_order_id(1),
    mode(SimulationMode::NORMAL)
{
    simulator_thread =
        std::thread(
            &MarketSimulator::run,
            this
        );
}

MarketSimulator::~MarketSimulator() {

    shutdown = true;

    if (
        simulator_thread.joinable()
    ) {
        simulator_thread.join();
    }
}

void MarketSimulator::start() {
    running = true;
}

void MarketSimulator::stop() {
    running = false;
}

void MarketSimulator::set_mode(SimulationMode new_mode) {
    mode = new_mode;
}

bool MarketSimulator::is_running() const {
    return running;
}

void MarketSimulator::run() {

    std::mt19937 rng(
        std::random_device{}()
    );

    std::uniform_int_distribution<
        uint32_t
    > qty_dist(
        1,
        500
    );

    std::uniform_int_distribution<
        int
    > side_dist(
        0,
        1
    );

    while (!shutdown) {

        if (!running) {

            std::this_thread::sleep_for(
                std::chrono::milliseconds(10)
            );

            continue;
        }

        uint32_t price;
        uint32_t quantity;

        if (
            mode ==
            SimulationMode::NORMAL
        ) {

            std::uniform_int_distribution<
                uint32_t
            > normal_price_dist(
                9950,
                10050
            );

            std::uniform_int_distribution<
                uint32_t
            > normal_qty_dist(
                10,
                500
            );

            price =
                normal_price_dist(rng);

            quantity =
                normal_qty_dist(rng);
        }

        else if (
            mode ==
            SimulationMode::HFT
        ) {

            std::uniform_int_distribution<
                uint32_t
            > hft_price_dist(
                9995,
                10005
            );

            std::uniform_int_distribution<
                uint32_t
            > hft_qty_dist(
                1,
                50
            );

            price =
                hft_price_dist(rng);

            quantity =
                hft_qty_dist(rng);
        }

        else if (
            mode ==
            SimulationMode::STRESS
        ) {

            std::uniform_int_distribution<
                uint32_t
            > stress_price_dist(
                8500,
                11500
            );

            std::uniform_int_distribution<
                uint32_t
            > stress_qty_dist(
                100,
                5000
            );

            price =
                stress_price_dist(rng);

            quantity =
                stress_qty_dist(rng);
        }

        Side side =
            side_dist(rng)
            ? Side::Buy
            : Side::Sell;

        uint32_t idx =
            engine.book.pool.allocate();

        Order& order =
            engine.book.pool.get(idx);

        order = {

            .order_id = next_order_id++,

            .price = price,

            .quantity = quantity,

            .next = 0,

            .prev = 0,

            .timestamp = 0,

            .side = side,

            .active = true
        };

        engine.process_order(idx);

        if (mode == SimulationMode::NORMAL) {
            std::this_thread::sleep_for(
                std::chrono::milliseconds(5)
            );
        }

        else if (mode == SimulationMode::HFT) {
            std::this_thread::sleep_for(
                std::chrono::microseconds(200)
            );
        }

        else if (mode == SimulationMode::STRESS) {
            std::this_thread::sleep_for(
                std::chrono::milliseconds(1)
            );
        }
    }
}
}