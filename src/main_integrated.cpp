#include <iostream>
#include <memory>
#include <signal.h>
#include <string>
#include <atomic>
#include <chrono>
#include <thread>

#include "UdpReceiver.hpp"
#include "Dispatcher.hpp"
#include "JsonKafkaHandler.hpp"
#include "KafkaProducer.hpp"
#include "Message.hpp"

std::atomic<bool> quit{false};

void signal_handler(int sig) {
    quit = true;
}

// Fast message parser for UDP packets
class FastMessageParser {
public:
    static std::shared_ptr<Message> parse(const std::vector<char>& packet) {
        std::string message(packet.begin(), packet.end());
        
        size_t pos = 0;
        size_t prev = 0;
        
        // Parse header
        pos = message.find('-', prev);
        if (pos == std::string::npos) return nullptr;
        std::string header = message.substr(prev, pos - prev);
        prev = pos + 1;
        
        // Parse mstype
        pos = message.find('-', prev);
        if (pos == std::string::npos) return nullptr;
        std::string mstype = message.substr(prev, pos - prev);
        prev = pos + 1;
        
        // Parse symbol
        pos = message.find('-', prev);
        if (pos == std::string::npos) return nullptr;
        std::string symbol = message.substr(prev, pos - prev);
        prev = pos + 1;
        
        // Parse data
        std::string data = message.substr(prev);
        
        try {
            int msgType = std::stoi(mstype);
            return std::make_shared<Message>(header, msgType, symbol, data);
        } catch (const std::exception&) {
            return nullptr;
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <ip> <port> <kafka_brokers>" << std::endl;
        return 1;
    }
    
    std::string ip = argv[1];
    uint16_t port = static_cast<uint16_t>(std::stoi(argv[2]));
    std::string kafka_brokers = argv[3];
    
    try {
        // Initialize Kafka producer
        auto kafka_producer = std::make_shared<KafkaProducer>(kafka_brokers);
        
        // Initialize message handler
        auto handler = std::make_shared<JsonKafkaHandler>(kafka_producer);
        
        // Initialize dispatcher with optimal thread count
        size_t num_threads = std::thread::hardware_concurrency();
        Dispatcher dispatcher(handler, num_threads);
        
        // Initialize UDP receiver
        UdpReceiver receiver(ip, port);
        
        std::signal(SIGINT, signal_handler);
        
        std::cout << "Starting CBOE Feed Handler..." << std::endl;
        std::cout << "UDP: " << ip << ":" << port << std::endl;
        std::cout << "Kafka: " << kafka_brokers << std::endl;
        std::cout << "Threads: " << num_threads << std::endl;
        
        // Start receiving UDP packets
        receiver.start([&dispatcher](const std::vector<char>& packet) {
            auto message = FastMessageParser::parse(packet);
            if (message) {
                dispatcher.dispatch(message);
            }
        }, 2, 80); // Pin to CPU core 2 with RT priority 80
        
        std::cout << "Server started. Press Ctrl+C to stop." << std::endl;
        
        // Statistics
        auto start_time = std::chrono::steady_clock::now();
        uint64_t message_count = 0;
        
        while (!quit.load()) {
            std::this_thread::sleep_for(std::chrono::seconds(5));
            
            auto current_time = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(current_time - start_time).count();
            
            if (elapsed > 0) {
                std::cout << "Running for " << elapsed << "s, processing messages..." << std::endl;
            }
        }
        
        std::cout << "Stopping receiver..." << std::endl;
        receiver.stop();
        
        std::cout << "Flushing Kafka producer..." << std::endl;
        kafka_producer->flush(5000);
        
        std::cout << "Shutdown complete." << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
} 