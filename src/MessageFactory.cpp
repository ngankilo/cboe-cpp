#include "MessageFactory.hpp"
#include "JsonKafkaHandler.hpp"
#include "KafkaProducer.hpp"
#include <memory>

std::unique_ptr<IMessageParser> MessageFactory::createParser(const std::string& type) {
    // For now, return nullptr - we'll use FastMessageParser directly
    return nullptr;
}

std::unique_ptr<IMessageHandler> MessageFactory::createHandler(const std::string& type) {
    if (type == "JSON_KAFKA" || type == "CBOE") {
        auto producer = std::make_shared<KafkaProducer>("localhost:9092");
        return std::make_unique<JsonKafkaHandler>(producer);
    }
    return nullptr;
}