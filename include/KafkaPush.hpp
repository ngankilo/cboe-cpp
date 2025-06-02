#pragma once

#ifndef _KAFKA_PUSH_HPP_
#define _KAFKA_PUSH_HPP_

#include "KafkaProducer.hpp"
#include <string>
#include <cstddef>
#include <iostream>

// Called by any worker thread to push (JSON) message to topic/partition
inline void KafkaPush(const std::string& symbol, int partition, const void* data, size_t len) {
    KafkaProducer& kp = KafkaProducer::instance();
    rd_kafka_t* producer = kp.get_producer();
    rd_kafka_topic_t* topic = kp.get_topic(symbol);

    if (!producer || !topic) {
        std::cerr << "[KafkaPush] Error: Producer or topic (" << symbol << ") not available!" << std::endl;
        return;
    }

    rd_kafka_resp_err_t err = rd_kafka_produce(
        topic,
        partition,
        RD_KAFKA_MSG_F_COPY,
        const_cast<void*>(data), len,
        nullptr, 0,
        nullptr);

    if (err != RD_KAFKA_RESP_ERR_NO_ERROR) {
        std::cerr << "[KafkaPush] Produce failed for topic " << symbol
                  << " partition " << partition << ": " << rd_kafka_err2str(err) << std::endl;
        // Optionally: implement backoff/retry or stats
    }
}

#endif
