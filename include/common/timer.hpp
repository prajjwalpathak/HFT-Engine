#pragma once

#include <chrono>

namespace hft {

class Timer {
public:

    using Clock = std::chrono::high_resolution_clock;

    void start() {
        start_ = Clock::now();
    }

    uint64_t elapsed_ns() const {

        auto end = Clock::now();

        return std::chrono::duration_cast<
            std::chrono::nanoseconds
        >(end - start_).count();
    }

private:

    Clock::time_point start_;
};

}