/**
 * @file    UdpReceiver.hpp
 * @brief   UDP datagram receiver class definition.
 *
 * Developer: Hoang Nguyen & Tan A. Pham
 * Copyright: Equix Technologies Pty Ltd (contact@equix.com.au)
 * Filename: UdpReceiver.hpp
 * Created: 28/May/2025
 *
 * Description:
 *   UdpReceiver provides asynchronous UDP packet reception. For every
 *   datagram received, it invokes a user-provided callback. The receive
 *   thread can optionally have its CPU affinity or real-time priority set.
 */

#ifndef UDP_RECEIVER_HPP_
#define UDP_RECEIVER_HPP_
#pragma once

#include <string>
#include <thread>
#include <atomic>
#include <functional>
#include <vector>
#include <cstdint>

/**
 * @file    UdpReceiver.hpp
 * @brief   Asynchronous UDP receiver class with thread affinity and real-time support.
 */

class UdpReceiver {
public:
    struct Config {
        std::string bind_ip = "0.0.0.0";
        uint16_t bind_port = 9000;
        int cpu_affinity_core = -1; ///< -1 means no affinity
        int thread_realtime_priority = 0; ///< 0 = normal priority
    };

    using PacketCallback = std::function<void(const std::vector<char> &)>;

    explicit UdpReceiver(const Config &config);

    ~UdpReceiver();

    void start(PacketCallback callback);

    void stop();

private:
    void setup_thread_affinity();

    void setup_realtime_priority();

    int socket_fd_{-1};
    std::atomic<bool> running_{false};
    std::thread receiver_thread_;
    Config config_;
};

#endif // UDP_RECEIVER_HPP_
