#include "IMessageHandler.hpp"
#include "KafkaProducer.hpp"
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

class CboeHandler : public IMessageHandler {
public:
    explicit CboeHandler(std::shared_ptr<KafkaProducer> producer)
        : kafka_(producer) {}
    void handle(const IMessagePtr& msg) override {
        // Convert msg to JSON and push to Kafka
    }
private:
    std::shared_ptr<KafkaProducer> kafka_;
};