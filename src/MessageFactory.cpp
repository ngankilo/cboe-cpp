#include "MessageFactory.hpp"
#include "parsers/CboeParser.cpp"
#include "handlers/CboeHandler.cpp"

std::unique_ptr<IMessageParser> MessageFactory::createParser(const std::string& type) {
    if (type == "CBOE") return std::make_unique<CboeParser>();
    // add more
    return nullptr;
}
std::unique_ptr<IMessageHandler> MessageFactory::createHandler(const std::string& type) {
    if (type == "CBOE") {
        auto producer = std::make_shared<KafkaProducer>("localhost:9092");
        return std::make_unique<CboeHandler>(producer);
    }
    // add more
    return nullptr;
}