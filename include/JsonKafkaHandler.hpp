#pragma once
#include "IMessageHandler.hpp"
#include "KafkaProducer.hpp"
#include <memory>
#include <iostream>
#include <chrono>
#include <nlohmann/json.hpp>

class JsonKafkaHandler : public IMessageHandler {
public:
    JsonKafkaHandler(std::shared_ptr<KafkaProducer> producer) 
        : producer_(producer) {}
    
    void handle(const IMessagePtr& msg) override {
        try {
            // Convert raw data to JSON
            nlohmann::json j;
            
            // Try to parse as JSON first
            try {
                j = nlohmann::json::parse(msg->getRawData());
            } catch (const nlohmann::json::parse_error&) {
                // If not JSON, wrap in a JSON object
                j["data"] = msg->getRawData();
            }
            
            // Add metadata
            j["symbol"] = msg->getSymbol();
            j["msgType"] = msg->getMsgType();
            j["timestamp"] = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
            
            // Send to Kafka: topic = symbol, partition = msgType
            producer_->produce(msg->getSymbol(), j.dump(), std::to_string(msg->getMsgType()));
            
        } catch (const std::exception& e) {
            std::cerr << "Error in JsonKafkaHandler: " << e.what() << std::endl;
        }
    }

private:
    std::shared_ptr<KafkaProducer> producer_;
}; 