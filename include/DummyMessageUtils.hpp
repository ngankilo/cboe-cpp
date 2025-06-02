/**
 * @file    DummyMessageUtils.hpp
 * @brief   Example message type and dummy parser/push utilities for testing pipelines.
 *
 * Developer: Hoang Nguyen & Tan A. Pham
 * Copyright: Equix Technologies Pty Ltd (contact@equix.com.au)
 * Filename: DummyMessageUtils.hpp
 * Created: 28/May/2025
 *
 * Description:
 *   Defines a sample Message struct for simulation and testing, along with
 *   dummy parser and Kafka push functions that introduce artificial delays.
 *   Useful for benchmarking and unit test scaffolding.
 */

#pragma once

#include <string>
#include <thread>
#include <chrono>
#include <iostream>

/**
 * @struct Message
 * @brief Example message containing symbol and payload.
 */
struct Message {
    std::string symbol;   ///< Symbol or topic string for the message.
    std::string payload;  ///< Payload data (e.g., content/body of the message).
};

/**
 * @brief Dummy parser function that simulates parsing delay.
 * @param msg  Reference to a Message to be "parsed".
 *
 * Adds a "_parsed" suffix to the payload and sleeps for a few microseconds.
 */
inline void dummy_message_parser(Message& msg) {
    std::this_thread::sleep_for(std::chrono::microseconds(5));
    msg.payload += "_parsed";
}

/**
 * @brief Dummy Kafka push function that simulates network/push delay.
 * @param msg  Constant reference to a Message to "send".
 *
 * Sleeps for a few microseconds. Uncomment the cout for debugging/logging.
 */
inline void dummy_kafka_push(const Message& msg) {
    std::this_thread::sleep_for(std::chrono::microseconds(3));
    // std::cout << "Kafka: " << msg.symbol << ": " << msg.payload << std::endl;
}
