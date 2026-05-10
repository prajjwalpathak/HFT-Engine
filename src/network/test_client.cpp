#include "network/protocol.hpp"

#include <iostream>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

using namespace hft;

int main() {

    int sock = socket(
        AF_INET,
        SOCK_STREAM,
        0
    );

    sockaddr_in server_addr {};

    server_addr.sin_family = AF_INET;

    server_addr.sin_port =
        htons(9000);

    inet_pton(
        AF_INET,
        "127.0.0.1",
        &server_addr.sin_addr
    );

    connect(
        sock,
        reinterpret_cast<sockaddr*>(
            &server_addr
        ),
        sizeof(server_addr)
    );

    NewOrderMessage msg {

        .type = MessageType::NewOrder,

        .order_id = 1,

        .price = 10100,

        .quantity = 100,

        .timestamp = 1,

        .side = 0
    };

    send(
        sock,
        &msg,
        sizeof(msg),
        0
    );

    std::cout << "Order sent\n";

    close(sock);

    return 0;
}