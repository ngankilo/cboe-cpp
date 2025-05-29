#include <iostream>
#include <string>
#include <memory>
#include <signal.h>
#include <atomic>
#include <chrono>
#include <thread>
#include <vector>

#include "../include/third_party/pitch/message.h"
#include "../include/third_party/pitch/message_factory.h"


std::atomic<bool> quit{false};

void signal_handler(int sig) {
    quit = true;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <ip> <port> <kafka_brokers>" << std::endl;
        return 1;
    }

    std::string ip = argv[1];
    uint16_t port = static_cast<uint16_t>(std::stoi(argv[2]));
    std::string kafka_brokers = argv[3];

    try {
        signal(SIGINT, signal_handler);

        std::cout << "Starting CBOE Feed Handler..." << std::endl;
        std::cout << "UDP: " << ip << ":" << port << std::endl;

        std::cout << "Server started. Press Ctrl+C to stop." << std::endl;

        // Test messages for verification
        std::vector<std::string> test_messages = {
            "29000020A000ABDDCF0XS000300AMD   0000213700Y", // AddOrderShort
            "29000020B000ABDDCF0XS000300AMD INC00000213700Y", // AddOrderLong
            "29000020E000ABDDCF0X000100ABDDCF0X123", // OrderExecuted
            "29000020P000300AMD   0000213700", // TradeShort
            "29000020r000ABDDCF0XS000300AMD INC00000213700ABDDCF0X123", // TradeLong
            "29000020C000ABDDCF0X", // DeleteOrder
            "29000020M000ABDDCF0X000200", // ModifyOrder
            "29000020H00000001U1", // SeqUnitHeader
            "29000020D000ABDDCF0X000150" // ReduceSize
        };

        for (const auto &msg: test_messages) {
            try {
                auto pitch_message = CboePitch::parseMessage(msg);
                std::cout << pitch_message->toString() << std::endl;
                std::cout << "Original Symbol: " << pitch_message->getSymbol() << std::endl;
                try {
                    pitch_message->setSymbol("INTC INC");
                    std::cout << "Updated Symbol: " << pitch_message->getSymbol() << std::endl;
                    std::cout << pitch_message->toString() << std::endl;
                } catch (const std::exception &e) {
                    std::cout << "SetSymbol error: " << e.what() << std::endl;
                }
                std::cout << "------------------------" << std::endl;
            } catch (const std::exception &e) {
                std::cerr << "Test message parse error: " << e.what() << std::endl;
            }
        }

        while (!quit.load()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        std::cout << "Shutdown complete." << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
