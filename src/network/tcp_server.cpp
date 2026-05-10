#include "network/tcp_server.hpp"

#include <iostream>
#include <cstring>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "network/protocol.hpp"

namespace hft {

TCPServer::TCPServer(uint16_t port) : server_fd(-1), port(port) {}

void TCPServer::start() {

    server_fd = socket(
        AF_INET,
        SOCK_STREAM,
        0
    );

    if (server_fd < 0) {

        std::cerr << "Socket creation failed\n";

        return;
    }

    std::cout << "Socket created\n";

    // Bind Socket To Port
    sockaddr_in server_addr {};

    server_addr.sin_family = AF_INET;

    server_addr.sin_addr.s_addr = INADDR_ANY;

    server_addr.sin_port = htons(port);

    // Bind
    if (bind(
        server_fd,
        reinterpret_cast<sockaddr*>(&server_addr),
        sizeof(server_addr)
        ) < 0) {

        std::cerr << "Bind failed\n";

        return;
    }

    // Listen For Connections
    if (listen(server_fd, 16) < 0) {

        std::cerr
            << "Listen failed\n";

        return;
    }

    std::cout << "Listening on port " << port << "\n";

    // Accept one client
    sockaddr_in client_addr {};

    socklen_t client_len =
        sizeof(client_addr);

    int client_fd = accept(
        server_fd,
        reinterpret_cast<sockaddr*>(&client_addr),
        &client_len
    );

    // Receive Message
    NewOrderMessage msg {};

    ssize_t bytes_received = recv(
        client_fd,
        &msg,
        sizeof(msg),
        0
    );

    if (bytes_received > 0) {

        std::cout << "Received Order:\n";

        std::cout << "Order ID: " << msg.order_id << "\n";

        std::cout << "Price: " << msg.price << "\n";

        std::cout << "Quantity: " << msg.quantity << "\n";
    }
}

}

int main() {

    hft::TCPServer server(9000);

    server.start();

    return 0;
}