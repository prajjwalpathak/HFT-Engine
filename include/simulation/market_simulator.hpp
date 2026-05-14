#pragma once

#include <atomic>
#include <thread>

#include "core/matching_engine.hpp"

namespace hft {

enum class SimulationMode {
    NORMAL,
    HFT,
    STRESS
};

class MarketSimulator {

public:

    explicit MarketSimulator(
        MatchingEngine& engine
    );

    ~MarketSimulator();

    void start();

    void stop();

    void set_mode(SimulationMode mode);

    bool is_running() const;

private:

    void run();

private:

    MatchingEngine& engine;

    std::atomic<bool> running;

    std::thread simulator_thread;

    uint64_t next_order_id;

    SimulationMode mode;

    std::atomic<bool> shutdown;
};

}