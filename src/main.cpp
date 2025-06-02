#include <iostream>
#include <csignal>
#include <atomic>
#include <chrono>
#include <iomanip>
#include <thread>
#include <memory>
#include <unordered_map>

#include "pitch/message.h"
#include "pitch/seq_unit_header.h"
#include "pitch/message_factory.h"
#include "DisruptorRouter.hpp"
#include "UdpReceiver.hpp"

std::atomic<uint64_t> messages_handled{0};
std::atomic<bool> quit{false};
std::atomic<uint64_t> stats_counter{0};

void signal_handler(int) {
    quit = true;
}

struct MsgEvent {
    std::shared_ptr<CboePitch::Message> msg;
};

int main() {
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    constexpr size_t RING_SIZE = 4096;
    using namespace disruptor_pipeline;

    auto handler = [](const MsgEvent &event) {
        if (event.msg) {
            uint64_t n = ++messages_handled;
            ++stats_counter;
            if (n % 1000 == 0) {
                auto now = std::chrono::steady_clock::now();
                static auto last = now;
                double ms = std::chrono::duration<double, std::milli>(now - last).count();
                std::cout << "[Perf] Last 1000 messages avg: "
                        << (ms / 1000.0) << " ms/msg, throughput: "
                        << (1000.0 / (ms / 1000.0)) << " msg/sec\n";
                last = now;
            }
        }
    };

    DisruptorRouter<MsgEvent> router(RING_SIZE, handler);

    UdpReceiver receiver("0.0.0.0", 9000);
    receiver.start([&](const std::vector<char> &packet) {
        try {
            // Log raw packet data
            std::cout << "[UDP] Received " << packet.size() << " bytes: ";
            std::cout << std::hex << std::setfill('0');
            for (size_t i = 0; i < std::min(packet.size(), size_t(32)); ++i) {
                std::cout << std::setw(2) << static_cast<unsigned int>(static_cast<unsigned char>(packet[i])) << " ";
            }
            if (packet.size() > 32) std::cout << "...";
            std::cout << std::dec << '\n';

            // Parse SeqUnitHeader
            auto header = CboePitch::SeqUnitHeader::parse(reinterpret_cast<const uint8_t *>(packet.data()),
                                                          packet.size());
            std::cout << "[SeqUnitHeader] " << header.toString() << std::endl;
            //
            // // Parse messages
            auto messages = CboePitch::MessageFactory::parseMessages(reinterpret_cast<const uint8_t *>(packet.data()),
                                                                     packet.size());
            //
            for (const auto &msg: messages) {
                    std::cout << msg->toString() << std::endl;
                    // Uncomment to enable Disruptor
                    /*
                    disruptorplus::sequence_t seq;
                    MsgEvent& event = router.claim_and_get_slot(seq);
                    event.msg = msg;  // Use the individual message
                    router.publish(seq);
                    */
                }
        } catch (const std::exception &ex) {
            std::cerr << "[UDP] Failed to parse: " << ex.what() << std::endl;
        }
    });

    std::thread stats_thread([&]() {
        using namespace std::chrono_literals;
        while (!quit.load()) {
            std::this_thread::sleep_for(1s);
            uint64_t count = stats_counter.exchange(0, std::memory_order_relaxed);
            if (count > 0) {
                std::cout << "[Stats] Packets processed in last 1s: " << count << std::endl;
            }
        }
    });

    std::cout << "Waiting for UDP packets on 0.0.0.0:9000. Ctrl+C or SIGTERM to stop.\n";
    while (!quit.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    receiver.stop();
    stats_thread.join();
    std::cout << "Receiver stopped. Exiting.\n";
    return 0;
}
