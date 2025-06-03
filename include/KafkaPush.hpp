/**
 * @file    KafkaPush.hpp
 * @brief   Inline function for pushing messages to a Kafka topic (thread-safe).
 *
 * Developer: Hoang Nguyen & Tan A. Pham
 * Copyright: Equix Technologies Pty Ltd (contact@equix.com.au)
 * Filename: KafkaPush.hpp
 * Created: 30/May/2025
 *
 * Description:
 *   Provides an inline helper function for any worker thread to publish
 *   (typically JSON) messages into a given Kafka topic and partition,
 *   using the KafkaProducer singleton backend.
 */
#pragma once

#ifndef KAFKA_PUSH_HPP_
#define KAFKA_PUSH_HPP_

#include "KafkaProducer.hpp"
#include <string>
#include <cstddef>
#include <iostream>

/**
 * @brief   Publishes a message to a Kafka topic and partition (thread-safe).
 *
 *          Uses the KafkaProducer singleton instance. If the producer or topic handle
 *          is unavailable, logs an error. Errors during publishing (asynchronous) are logged.
 *
 * @param   symbol      The Kafka topic name.
 * @param   partition   The Kafka partition to publish to.
 * @param   data        Pointer to message payload (typically JSON).
 * @param   len         Size in bytes of the payload.
 *
 * @note    Safe for calls from multiple threads. If publishing fails, logs error to std::cerr.
 */
inline void KafkaPush(const std::string& symbol, int partition, const void* data, size_t len) {
    KafkaProducer& kp = KafkaProducer::instance();
    rd_kafka_t* producer = kp.get_producer();
    rd_kafka_topic_t* topic = kp.get_or_create_topic(symbol);

    if (!producer || !topic) {
        std::cerr << std::hex
              << "[KafkaPush] Error: Producer or topic (" << symbol << ") not available!  "
              << "producer=0x" << (uintptr_t)producer
              << ", topic=0x"  << (uintptr_t)topic << std::dec << std::endl;
        return;
    }

    int ret = rd_kafka_produce(
        topic,
        partition,
        RD_KAFKA_MSG_F_COPY,
        const_cast<void*>(data), len,
        nullptr, 0,
        nullptr);
    std::cout << "Push to Kafka with ret = %d" << ret << std::endl;
    if (ret == -1) {
        rd_kafka_resp_err_t err = rd_kafka_last_error();
        std::cerr << "[KafkaPush] Produce failed for topic " << symbol
                  << " partition " << partition << ": " << rd_kafka_err2str(err) << std::endl;
        // Optionally: you could do backoff/retry or stats here, if needed
    }
    // else: success (asynchronous), nothing to do
}

#endif