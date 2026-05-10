#pragma once

#include <cstdint>

namespace hft {

class TCPServer {
public:

    explicit TCPServer(uint16_t port);

    void start();

private:

    int server_fd;

    uint16_t port;
};

}