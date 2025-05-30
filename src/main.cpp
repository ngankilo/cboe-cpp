#include <csignal>
#include <iomanip>
#include <iostream>
#include <thread>

#include "../include/pitch/seq_unit_header.h"
#include "../include/pitch/message_factory.h"
#include "../include/UdpReceiver.hpp"
#include <atomic>
#include <chrono>

std::atomic<bool> quit{false};

void signal_handler(int sig) {
    quit = true;
}

int main() {
    // uint8_t data[] = {
    //     0x2A, 0x00, // Hdr Length = 42
    //     0x02, // Hdr Count = 2
    //     0x01, // Hdr Unit = 1
    //     0x01, 0x00, 0x00, 0x00, // Hdr Sequence = 1
    //     // Message 1: AddOrder (Type 0x37, length 39)
    //     0x37, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    //     0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    //     0x42, // SideIndicator
    //     0x64, 0x00, 0x00, 0x00,
    //     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x42, // Price
    //     0x41, 0x42, 0x43, 0x44, 0x45, 0x46,
    //     0x50, 0x51, 0x52, 0x53,
    //     // Message 2: EndOfSession (Type 0x2D, length 1)
    //     0x2D
    // };
    // size_t dataLength = sizeof(data);
    //
    // try {
    //     auto messages = CboePitch::SeqUnitHeader::decodeMessages(data, dataLength);
    //     for (const auto &msg: messages) {
    //         std::cout << msg->toString() << std::endl;
    //     }
    // } catch (const std::exception &e) {
    //     std::cerr << "Error: " << e.what() << std::endl;
    // }
    //
    // return 0;
    UdpReceiver receiver("0.0.0.0", 9000);

    std::signal(SIGINT, signal_handler);
    receiver.start(
        [](const std::vector<char> &packet) {
            std::cout << "[UDP] Received " << packet.size() << " bytes: ";
            // Print hex dump of up to first 32 bytes
            std::cout << std::hex << std::setfill('0');
            for (size_t i = 0; i < std::min(packet.size(), size_t(32)); ++i) {
                std::cout << std::setw(2) << static_cast<unsigned int>(static_cast<unsigned char>(packet[i])) << " ";
            }
            if (packet.size() > 32) std::cout << "...";
            std::cout << std::dec << '\n';

            // Convert std::vector<char> to const uint8_t* for decodeMessages
            auto messages = CboePitch::SeqUnitHeader::decodeMessages(reinterpret_cast<const uint8_t *>(packet.data()),
                                                                     packet.size());
            for (const auto &msg: messages) {
                std::cout << msg->toString() << std::endl;
            }
        }
        // , 2, 80 // optionally set core affinity/RT priority
    );

    std::cout << "Waiting for UDP packets on 0.0.0.0:9000. Ctrl+C to stop.\n";
    while (!quit.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    receiver.stop();
    std::cout << "Receiver stopped. Exiting.\n";
    return 0;
}
