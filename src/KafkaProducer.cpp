#include "KafkaProducer.hpp"
#include <iostream>
#include <stdexcept>

KafkaProducer::KafkaProducer(const std::string& brokers) {
    std::string errstr;
    conf_ = std::unique_ptr<RdKafka::Conf>(RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL));
    
    if (!conf_) {
        throw std::runtime_error("Failed to create Kafka configuration");
    }

    // High performance configurations
    conf_->set("bootstrap.servers", brokers, errstr);
    conf_->set("batch.size", "65536", errstr);           // Larger batch size
    conf_->set("linger.ms", "1", errstr);                // Low latency
    conf_->set("compression.type", "lz4", errstr);       // Fast compression
    conf_->set("acks", "1", errstr);                     // Balance between speed and reliability
    conf_->set("max.in.flight.requests.per.connection", "5", errstr);
    conf_->set("enable.idempotence", "true", errstr);    // Prevent duplicates
    conf_->set("retries", "3", errstr);
    conf_->set("retry.backoff.ms", "100", errstr);

    producer_ = std::unique_ptr<RdKafka::Producer>(
        RdKafka::Producer::create(conf_.get(), errstr)
    );

    if (!producer_) {
        throw std::runtime_error("Failed to create producer: " + errstr);
    }
}

KafkaProducer::~KafkaProducer() {
    if (producer_) {
        producer_->flush(1000);
    }
}

void KafkaProducer::produce(const std::string& topic, const std::string& message, 
                           const std::string& mstype) {
    int32_t partition = calculatePartition(mstype);
    
    RdKafka::ErrorCode err = producer_->produce(
        topic,
        partition,
        RdKafka::Producer::RK_MSG_COPY,
        const_cast<char*>(message.c_str()),
        message.size(),
        nullptr,
        nullptr
    );

    if (err != RdKafka::ERR_NO_ERROR) {
        throw std::runtime_error("Failed to produce message: " + RdKafka::err2str(err));
    }

    // Poll for events to ensure low latency
    producer_->poll(0);
}

void KafkaProducer::flush(int timeout_ms) {
    producer_->flush(timeout_ms);
}

int32_t KafkaProducer::calculatePartition(const std::string& mstype) {
    // Simple hash-based partitioning
    std::hash<std::string> hasher;
    return static_cast<int32_t>(hasher(mstype) % 32); // Assume max 32 partitions
}
