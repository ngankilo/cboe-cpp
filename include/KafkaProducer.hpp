#pragma once

#include <string>
#include <memory>
#include <functional>
#include <librdkafka/rdkafkacpp.h>

class KafkaProducer {
public:
    KafkaProducer(const std::string& brokers);
    ~KafkaProducer();

    void produce(const std::string& topic, const std::string& message, 
                const std::string& mstype);
    void flush(int timeout_ms = 1000);

private:
    int32_t calculatePartition(const std::string& mstype);
    
    std::unique_ptr<RdKafka::Producer> producer_;
    std::unique_ptr<RdKafka::Conf> conf_;
}; 