#include "network/tcp_server.hpp"

#include <iostream>
#include <cstring>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <cerrno>
#include <sys/epoll.h>
#include <atomic>

#include "network/protocol.hpp"
#include "memory/order_pool.hpp"

namespace hft {

std::atomic<uint64_t> packets_processed = 0;

TCPServer::TCPServer(
    uint16_t port,
    MatchingEngine& engine):
    server_fd(-1),
    port(port),
    engine(engine) {}

bool set_non_blocking(int fd) {

    int flags = fcntl(fd, F_GETFL, 0);

    if (flags < 0) {
        return false;
    }

    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
        return false;
    }

    return true;
}

void TCPServer::start() {

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0) {
        std::cerr << "Socket creation failed\n";
        return;
    }

    if (!set_non_blocking(server_fd)) {
        std::cerr << "Failed to set non-blocking mode\n";
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
        std::cerr << "Listen failed\n";
        return;
    }

    std::cout << "Listening on port " << port << "\n";

    // For epoll
    int epoll_fd = epoll_create1(0);

    if (epoll_fd < 0) {
        std::cerr << "epoll_create failed\n";
        return;
    }

    epoll_event server_event {};
    server_event.events = EPOLLIN;
    server_event.data.fd = server_fd;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &server_event) < 0) {
        std::cerr << "epoll_ctl failed\n";
        return;
    }

    constexpr int MAX_EVENTS = 64;
    epoll_event events[MAX_EVENTS];

    while (true) {
        int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);

        for (int i = 0; i < num_events; i++) {
            int fd = events[i].data.fd;

            if (fd == server_fd) {
                sockaddr_in client_addr {};

                socklen_t client_len = sizeof(client_addr);

                int client_fd = accept(
                                server_fd,
                                reinterpret_cast<sockaddr*>(&client_addr),
                                &client_len);
                
                if (client_fd < 0) {
                    continue;
                }

                set_non_blocking(client_fd);

                epoll_event client_event {};
                client_event.events = EPOLLIN;
                client_event.data.fd = client_fd;

                epoll_ctl(
                    epoll_fd,
                    EPOLL_CTL_ADD,
                    client_fd,
                    &client_event
                );
            }
            else {
                NewOrderMessage msg {};

                ssize_t bytes_received = recv(fd, &msg, sizeof(msg), 0);

                if (bytes_received <= 0) {
                    close(fd);
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
                    continue;
                }

                // std::cout << "Received Order:\n";
                // std::cout << "Order ID: " << msg.order_id << "\n";
                // std::cout << "Price: " << msg.price << "\n";
                // std::cout << "Quantity: " << msg.quantity << "\n";

                uint32_t idx = engine.book.pool.allocate();

                Order& order = engine.book.pool.get(idx);

                order = {
                    .order_id = msg.order_id,

                    .price = msg.price,

                    .quantity = msg.quantity,

                    .next = 0,

                    .prev = 0,

                    .timestamp = msg.timestamp,

                    .side = static_cast<Side>(msg.side),

                    .active = true
                };

                packets_processed++;

                engine.process_order(idx);
                // std::cout << "Order processed by engine\n";

                if (packets_processed % 10000 == 0) {
                    std::cout << "Packets processed: " << packets_processed.load() << "\n";
                }
            }
        }
    }
}

}

int main() {

    using namespace hft;

    OrderPool pool(1000000);

    UDPPublisher publisher("127.0.0.1", 9999);

    MatchingEngine engine(pool, publisher);

    TCPServer server(
        9000,
        engine
    );

    server.start();

    return 0;
}