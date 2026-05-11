#pragma once

#include <cstdint>
#include <cstddef>

struct sockaddr_in;

namespace hft {

class UDPPublisher {
public:

    UDPPublisher(const char* ip, uint16_t port);

    ~UDPPublisher();

    void publish(const void* data, size_t size);

private:

    int socket_fd;

    sockaddr_in* address;
};

}