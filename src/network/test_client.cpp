#include "network/protocol.hpp"

#include <iostream>
#include <thread>
#include <thread>
#include <vector>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

using namespace hft;

void run_client(uint64_t client_id) {
    int sock = socket(
        AF_INET,
        SOCK_STREAM,
        0
    );

    sockaddr_in server_addr {};

    server_addr.sin_family = AF_INET;

    server_addr.sin_port = htons(9000);

    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    connect(
        sock,
        reinterpret_cast<sockaddr*>(&server_addr),
        sizeof(server_addr)
    );

    for (uint64_t i = 1; i <= 10000; i++) {
        NewOrderMessage msg {

            .type = MessageType::NewOrder,

            .order_id = client_id * 1000000 + i,

            .price = static_cast<uint32_t>(10000 + i),

            .quantity = 100,

            .timestamp = i,

            .side = static_cast<uint8_t>(i % 2)
        };

        send(sock, &msg, sizeof(msg), 0);
    }

    std::cout << "Order sent\n";

    close(sock);
}

int main() {

    constexpr int NUM_CLIENTS = 8;

    std::vector<std::thread> clients;

    for (int i = 0; i < NUM_CLIENTS; i++) {
        clients.emplace_back(run_client, i);
    }

    for (auto& thread : clients) {
        thread.join();
    }

    return 0;
}