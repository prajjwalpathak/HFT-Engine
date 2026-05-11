#include "network/udp_publisher.hpp"

#include <iostream>
#include <cstring>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

namespace hft {

    UDPPublisher::UDPPublisher(
        const char* ip,
        uint16_t port
    ) : socket_fd(-1), address(nullptr) {
        socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

        if (socket_fd < 0) {
            std::cerr << "UDP socket creation failed\n";
            return;
        }

        address = new sockaddr_in {};

        address->sin_family = AF_INET;

        address->sin_port = htons(port);

        inet_pton(AF_INET, ip, &address->sin_addr);

        std::cout << "UDP publisher initialized\n";

    }

    UDPPublisher::~UDPPublisher() {
        if (socket_fd >= 0) {
            close(socket_fd);
        }
        delete address;
    }

    void UDPPublisher::publish(const void* data, size_t size) {
        sendto(socket_fd, data, size, 0,
        reinterpret_cast<sockaddr*>(address),
        sizeof(*address));
    }
}