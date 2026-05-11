#pragma once

#include <cstdint>

#include "core/matching_engine.hpp"

namespace hft {

class TCPServer {
public:

    MatchingEngine& engine;

    explicit TCPServer(uint16_t port, MatchingEngine& engine);

    void start();

private:

    int server_fd;

    uint16_t port;
};

}