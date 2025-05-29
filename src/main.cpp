// main.cpp
#include <iostream>
#include <memory>
#include "../include/pitch/message_factory.h"

int main() {
    // Sample AddOrder message (binary data based on CXA spec, Section 7.1)
    unsigned char sample_data[] = {
        0x2A, 0x37, 0xF0, 0x77, 0xBB, 0xCE, 0x2A, 0x6A, 0x62, 0x16,
        0x05, 0x40, 0x5B, 0x77, 0x8F, 0x56, 0x1D, 0x0B, 0x42, 0xBC, 0x02, 0x00, 0x00,
        0x5A, 0x56, 0x5A, 0x54, 0x20, 0x20, 0x15, 0xCD, 0x5B, 0x07, 0x00, 0x00, 0x00, 0x00,
        0x31, 0x32, 0x33, 0x34, 0x00
    };
    size_t size = sizeof(sample_data);

    try {
        // Parse the message
        auto message = CboePitch::parseMessage(sample_data, size);
        std::cout << "Parsed message: " << message->toString() << std::endl;
        std::cout << "Symbol: " << message->getSymbol() << std::endl;

        // Test symbol update
        message->setSymbol("TESTSY");
        std::cout << "Updated Symbol: " << message->getSymbol() << std::endl;
        std::cout << "Updated message: " << message->toString() << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
