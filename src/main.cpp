/**
 * @file    main.cpp
 * @brief   Market data UDP receiver and disruptor router application entry point.
 *
 * Developer: Hoang Nguyen & Tan A. Pham
 * Copyright: Equix Technologies Pty Ltd (contact@equix.com.au)
 * Created: 2/June/2025
 *
 * Description:
 *   Application entry point for high-performance UDP market data reception, per-symbol queueing,
 *   and disruptor-based routing for downstream processing. 
 *   Handles runtime configuration via YAML, supports signal-based graceful shutdown,
 *   launches UDP receivers and worker threads, and orchestrates end-to-end message flow.
 */

#include <iostream>
#include <csignal>
#include <atomic>
#include <chrono>
#include <thread>
#include <vector>
#include <memory>
#include <string>
#include <yaml-cpp/yaml.h>

#include "UdpReceiver.hpp"
#include "DisruptorRouter.hpp"
#include "SymbolIdentifier.hpp"
#include "SymbolQueueRouter.hpp"
#include "KafkaProducer.hpp"
#include "KafkaPush.hpp"
#include "DisruptorDispatcher.hpp"
//Pitch library
#include "pitch/message_factory.h"
#include "pitch/seq_unit_header.h"
#include "pitch/message.h"

// Atomic flag set by signal handler to trigger application shutdown.
// All threads check this to exit cleanly.
std::atomic<bool> shutdown_requested{false};

// Global counter for processed messages; used for monitoring/statistics.
std::atomic<uint64_t> total_messages_processed{0};

// Configuration constants
constexpr int NUM_KAFKA_PARTITIONS = 8; // Adjust based on your Kafka topic setup

/**
 * @brief Convert payload to hex string for debugging/logging
 * @param payload The message payload
 * @return Hex string representation
 */
inline std::string payload_to_hex_string(const std::vector<uint8_t> &payload) {
    if (payload.empty()) return "";

    std::string hex_str;
    hex_str.reserve(payload.size() * 2);

    for (uint8_t byte: payload) {
        char hex_chars[3];
        snprintf(hex_chars, sizeof(hex_chars), "%02x", byte);
        hex_str += hex_chars;
    }

    return hex_str;
}

/**
 * @brief Create JSON message with payload and metadata
 * @param message_type The message type
 * @param symbol The symbol
 * @param partition The partition number
 * @param payload The raw payload
 * @param include_payload_data Whether to include actual payload data in JSON
 * @return JSON string
 */
inline std::string create_json_message(uint8_t message_type,
                                       const std::string &symbol,
                                       int partition,
                                       const std::vector<uint8_t> &payload,
                                       bool include_payload_data = false) {
    std::string json = R"({"messageType": )" + std::to_string(static_cast<int>(message_type)) +
                       R"(, "symbol": ")" + symbol +
                       R"(", "partition": )" + std::to_string(partition) +
                       R"(, "payloadSize": )" + std::to_string(payload.size()) +
                       R"(, "timestamp": )" + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
                           std::chrono::system_clock::now().time_since_epoch()).count());

    if (include_payload_data && !payload.empty()) {
        json += R"(, "payloadHex": ")" + payload_to_hex_string(payload) + R"(")";
    }

    json += "}";
    return json;
}

/**
 * @brief Hash message type to determine Kafka partition
 * @param message_type The message type from getMessageType()
 * @param num_partitions Total number of partitions available (default: NUM_KAFKA_PARTITIONS)
 * @return Partition number (0 to num_partitions-1)
 */
inline int hash_message_type_to_partition(uint8_t message_type, int num_partitions = NUM_KAFKA_PARTITIONS) {
    if (num_partitions <= 0) {
        return 0; // Default partition
    }

    // Simple hash: use message type modulo number of partitions
    // This ensures same message types always go to same partition
    return static_cast<int>(message_type % static_cast<uint8_t>(num_partitions));
}

/**
 * @brief Alternative hash function using std::hash for better distribution
 * @param message_type The message type from getMessageType()
 * @param num_partitions Total number of partitions available
 * @return Partition number (0 to num_partitions-1)
 */
inline int hash_message_type_to_partition_advanced(uint8_t message_type, int num_partitions = NUM_KAFKA_PARTITIONS) {
    if (num_partitions <= 0) {
        return 0;
    }

    // Use std::hash for better distribution
    std::hash<uint8_t> hasher;
    size_t hash_value = hasher(message_type);

    return static_cast<int>(hash_value % static_cast<size_t>(num_partitions));
}

/**
 * @brief Signal handler for SIGINT/SIGTERM.
 * Sets shutdown flag and prints notification.
 */
void handle_signal(int signal_number) {
    shutdown_requested.store(true);
    std::cerr << "\nSignal (" << signal_number << ") received, shutting down.\n";
}

/**
 * @brief Event structure for passing messages through the disruptor.
 * Includes message pointer and shutdown flag.
 */
struct MessageEvent {
    std::shared_ptr<CboePitch::Message> message;
    bool shutdown_event = false;
};

using MsgPtr = std::shared_ptr<CboePitch::Message>;

// Constants for symbol queue configuration.
constexpr size_t kSymbolQueueCapacity = 4096;
constexpr size_t kInitialSymbolTableSize = 300000;

// Router manages queues for each symbol (for per-symbol concurrency).
SymbolQueueRouter symbol_queue_router(kSymbolQueueCapacity, kInitialSymbolTableSize);

int main() {
    // ---- Signal Handling ----
    // Install signal handlers for graceful termination on SIGINT/SIGTERM.
    std::signal(SIGINT, handle_signal);
    std::signal(SIGTERM, handle_signal);

    // Initialize SymbolIdentifier
    equix_md::SymbolIdentifier symbol_map(kInitialSymbolTableSize);

    constexpr size_t kDisruptorRingSize = 4096;

    // ------------------------------------------------------------------------
    // 1. Initialize Kafka
    // ------------------------------------------------------------------------
    try {
        KafkaProducer::instance().initialize("config/config.yaml");
        // If you want to preallocate all topics: uncomment next lines
        // std::vector<std::string> all_symbol_names = ... // fill this with all symbols
        // KafkaProducer::instance().preallocate_topics(all_symbol_names);
        std::cout << "[MAIN] Finished setting up Kafka" << std::endl;
    } catch (const std::exception &ex) {
        std::cerr << "[Main] KafkaProducer init failed: " << ex.what() << std::endl;
        return 1;
    }
    // ---- Disruptor Setup ----
    // Define handler for disruptor pipeline events (processes MessageEvent).
    // Place application-specific downstream logic here.
    // ------------------------------------------------------------------------
    // 2. Disruptor Handler: message to Kafka
    // ------------------------------------------------------------------------
    auto disruptor_event_handler = [](const MsgPtr &msgPtr) {
        if (!msgPtr) {
            std::cerr << "[DisruptorHandler] Received shutdown event\n";
            return;
        }
        ++total_messages_processed;

        // 1. Symbol extraction (real implementation should not use placeholder!)
        std::string symbol = msgPtr->getSymbol();
        if (symbol.empty())symbol = "UNKNOWN";

        // 2. Partition (let Kafka decide or hash your way)
        int partition = hash_message_type_to_partition_advanced(msgPtr->getMessageType(), NUM_KAFKA_PARTITIONS); //
        // std::cout << "Partition: " << partition << std::endl;
        // 3. Push to Kafka
        // std::string json_body = R"({"dummy": "data", "id": )" + std::to_string(msgPtr->getOrderId()) + "}";
        KafkaPush(symbol, partition, msgPtr->getPayload().data(), msgPtr->getPayload().size());
    };
    disruptor_pipeline::DisruptorRouter<MsgPtr> disruptor_router(
        kDisruptorRingSize, disruptor_event_handler);
    // Function to publish a shutdown event to the disruptor, unblocking its worker.
    auto publish_shutdown_to_disruptor = [&disruptor_router] {
        disruptorplus::sequence_t seq;
        MsgPtr &slot = disruptor_router.claim_and_get_slot(seq);
        slot = nullptr; // <-- Signal shutdown!
        disruptor_router.publish(seq);
        std::cerr << "[Main] Published shutdown event to Disruptor\n";
    };

    // ---- UDP Receiver Initialization ----
    // Loads configuration, instantiates UDP receivers, or falls back to default if config is missing/invalid.
    std::vector<std::unique_ptr<UdpReceiver> > udp_receivers;
    try {
        YAML::Node config_node = YAML::LoadFile("config/config.yaml");
        if (config_node["udp_receivers"] && config_node["udp_receivers"].IsSequence() && config_node["udp_receivers"].
            size() > 0) {
            // Multiple UDP receivers may be configured for different IPs/ports.
            for (const auto &receiver_node: config_node["udp_receivers"]) {
                UdpReceiver::Config receiver_config;
                if (receiver_node["ip"]) receiver_config.bind_ip = receiver_node["ip"].as<std::string>();
                if (receiver_node["port"]) receiver_config.bind_port = receiver_node["port"].as<uint16_t>();
                if (receiver_node["core_affinity"])
                    receiver_config.cpu_affinity_core = receiver_node["core_affinity"].as<int>();
                udp_receivers.emplace_back(std::make_unique<UdpReceiver>(receiver_config));
            }
        } else {
            // Fallback to default config if no receivers defined.
            std::cerr << "[WARN] 'udp_receivers' config missing or empty – using default config.\n";
            udp_receivers.emplace_back(std::make_unique<UdpReceiver>(UdpReceiver::Config{}));
        }
    } catch (const YAML::Exception &exception) {
        std::cerr << "[ERROR] Failed to load config file (" << exception.what() << ") – using default config.\n";
        udp_receivers.emplace_back(std::make_unique<UdpReceiver>(UdpReceiver::Config{}));
    }

    // ---- UDP Packet Processing ----
    // Lambda called for every UDP packet received. Parses message and enqueues by symbol.
    auto on_udp_packet = [&symbol_map](const std::vector<char> &packet) {
        try {
            // 1. Parse SeqUnitHeader (optional, for logging/debug)
            auto header = CboePitch::SeqUnitHeader::parse(reinterpret_cast<const uint8_t *>(packet.data()),
                                                          packet.size());
            //std::cout << "[SeqUnitHeader] " << header.toString() << std::endl;

            // 2. Parse messages with SymbolIdentifier
            auto messages = CboePitch::MessageFactory::parseMessages(
                reinterpret_cast<const uint8_t *>(packet.data()), packet.size(), symbol_map);

            // 3. For each message, push to symbol queue
            for (const auto &msgPtr: messages) {
                if (!msgPtr) continue;

                // If desired, print or log the message:
                // std::cout << msgPtr->toString() << std::endl;

                // Extract symbol from message
                std::string symbol = msgPtr->getSymbol();
                // If no symbol detected, add default
                if (symbol.empty()) {
                    symbol = "UNKNOWN";
                }
                // Push to per-symbol queue
                symbol_queue_router.push(symbol, msgPtr);
            }
        } catch (const std::exception &ex) {
            std::cerr << "[UDP] Parse error: " << ex.what() << std::endl;
        }
    };

    // ---- Start UDP Receivers ----
    // All receivers run in background threads; incoming packets trigger above callback.
    for (auto &udp_receiver: udp_receivers)
        udp_receiver->start(on_udp_packet);
    std::cout << "[MAIN] All UDP receivers started\n";

    // ---- Worker Thread Setup ----
    // Determine how many worker threads to launch (typically 1 per CPU core).
    unsigned thread_count = std::thread::hardware_concurrency();
    if (thread_count == 0) thread_count = 4; // Sensible default if concurrency not reported.

    // Wait until symbol queues are created (first message received).
    // If no queues ever appear, we exit early.
    auto &symbol_queues = symbol_queue_router.get_queue_vector();
    while (symbol_queues.empty() && !shutdown_requested.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    size_t total_queues = symbol_queues.size();
    if (total_queues == 0) {
        std::cerr << "No symbol queues detected. Exiting.\n";
        for (auto &udp_receiver: udp_receivers) udp_receiver->stop();

        publish_shutdown_to_disruptor();
        KafkaProducer::instance().shutdown();
        return 1;
    }

    // ---- Launch Worker Threads ----
    // Each worker thread is responsible for a subset of symbol queues.
    std::cout << "[MAIN] Dispatcher and Disruptor pipeline launched.\n";
    std::vector<std::unique_ptr<DisruptorDispatcher<MsgPtr> > > dispatchers;
    for (size_t dispatcher_id = 0; dispatcher_id < thread_count; ++dispatcher_id) {
        dispatchers.emplace_back(
            std::make_unique<DisruptorDispatcher<MsgPtr> >(
                dispatcher_id,
                thread_count,
                symbol_queues,
                disruptor_router
            )
        );
    }

    std::cout << "UDP receivers running as configured. Ctrl+C or SIGTERM to exit.\n";

    // ---- Main Wait Loop ----
    // Sleep until shutdown requested; worker/receiver threads will be signaled to stop.
    while (!shutdown_requested.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // ---- Shutdown Sequence ----
    // Stop all UDP receivers to cease packet intake.
    for (auto &udp_receiver: udp_receivers)
        udp_receiver->stop();

    // Clear the dispatchers vector 
    dispatchers.clear();
    // Signal shutdown to disruptor so its event handler exits.
    publish_shutdown_to_disruptor();

    KafkaProducer::instance().shutdown();

    std::cout << "All receivers and worker threads stopped. Exiting.\n";
    return 0;
}
