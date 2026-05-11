#include "network/market_data.hpp"

#include <iostream>
#include <cstring>
#include <atomic>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

using namespace hft;

int main() {

    std::atomic<uint64_t> feed_messages = 0;

    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock < 0) {
        std::cerr << "Socket creation failed\n";
        return 1;
    }

    sockaddr_in addr {};

    addr.sin_family = AF_INET;

    addr.sin_addr.s_addr = INADDR_ANY;

    addr.sin_port = htons(9999);

    if (bind(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::cerr << "Bind failed\n";
        return 1;
    }

    std::cout << "Listening for market data...\n";

    while (true) {
        char buffer[64];

        ssize_t bytes_received = recvfrom(
            sock,
            buffer,
            sizeof(buffer),
            0,
            nullptr,
            nullptr
        );

        if (bytes_received <= 0) {
            continue;
        }

        ++feed_messages;

        if (feed_messages.load() % 10000 == 0) {
            std::cout << "Feed messages: " << feed_messages.load() << "\n";
        }

        auto type = *reinterpret_cast<MarketDataType*>(buffer);

        if (type == MarketDataType::Trade) {
            auto* trade = reinterpret_cast<TradeMessage*>(buffer);

            // std::cout << "[TRADE] " << "ID=" << trade->trade_id
            //         << " Price=" << trade->price
            //         << " Qty=" << trade->quantity << "\n";
        }
        else if (type == MarketDataType::TopOfBook) {
            auto* tob = reinterpret_cast<TopOfBookMessage*>(buffer);

            // std::cout << "[TOB] " << "Bid " << tob->best_bid_price
            //         << " x " << tob->best_bid_quantity
            //         << " | Ask " << tob->best_ask_price
            //         << " x " << tob->best_ask_quantity << "\n";
        }
    }
    close(sock);

    return 0;
}