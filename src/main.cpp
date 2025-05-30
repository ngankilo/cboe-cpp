#include <iostream>
#include "../include/pitch/seq_unit_header.h"
#include "../include/pitch/message_factory.h"

int main() {
    uint8_t data[] = {
        0x2A, 0x00, // Hdr Length = 42
        0x02, // Hdr Count = 2
        0x01, // Hdr Unit = 1
        0x01, 0x00, 0x00, 0x00, // Hdr Sequence = 1
        // Message 1: AddOrder (Type 0x37, length 39)
        0x37, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
        0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x42, // SideIndicator
        0x64, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x42, // Price
        0x41, 0x42, 0x43, 0x44, 0x45, 0x46,
        0x50, 0x51, 0x52, 0x53,
        // Message 2: EndOfSession (Type 0x2D, length 1)
        0x2D
    };
    size_t dataLength = sizeof(data);

    try {
        auto messages = CboePitch::SeqUnitHeader::decodeMessages(data, dataLength);
        for (const auto &msg: messages) {
            std::cout << msg->toString() << std::endl;
        }
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
