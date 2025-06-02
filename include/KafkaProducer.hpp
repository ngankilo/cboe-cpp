#pragma once
#include <string>
#include "IMessage.hpp"

class KafkaProducer {
public:
    KafkaProducer(const std::string& brokers);
    void send(const std::string& topic, int partition, const std::string& json_data);
    void flush();
    //...
};