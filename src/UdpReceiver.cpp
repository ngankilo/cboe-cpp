/**
 * @file    UdpReceiver.cpp
 * @brief   UDP datagram receiver implementation.
 *
 * Developer: Hoang Nguyen & Tan A. Pham
 * Copyright: Equix Technologies Pty Ltd (contact@equix.com.au)
 * Filename: UdpReceiver.cpp
 * Created: 28/May/2025
 *
 * Description:
 *   Implements the UdpReceiver class for receiving UDP packets asynchronously
 *   on a background thread, with support for CPU core affinity and real-time
 *   thread priority.
 */

#include "UdpReceiver.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <pthread.h>
#include <sched.h>
#include <stdexcept>

/**
 * @brief Constructs and binds the UDP socket to the specified IP and port.
 */
UdpReceiver::UdpReceiver(const Config& config)
    : socket_fd_(-1), running_(false), config_(config)
{
    // Create UDP socket (IPv4)
    socket_fd_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd_ < 0) {
        perror("socket");
        throw std::runtime_error("Failed to create socket");
    }

    sockaddr_in local_addr{};
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(config_.bind_port);
    local_addr.sin_addr.s_addr = inet_addr(config_.bind_ip.c_str());

    // Bind to the specified address and port
    if (bind(socket_fd_, (struct sockaddr*)&local_addr, sizeof(local_addr)) < 0) {
        perror("bind");
        close(socket_fd_);
        throw std::runtime_error("Failed to bind");
    }
}

/**
 * @brief Destructor: ensures resources are cleaned up.
 */
UdpReceiver::~UdpReceiver() {
    stop();
    if (socket_fd_ >= 0) {
        close(socket_fd_);
    }
}

/**
 * @brief Starts the UDP receiver thread, invoking callback on each datagram.
 * @param callback  Function to process each received UDP packet.
 */
void UdpReceiver::start(PacketCallback callback) {
    if (running_) return;

    running_ = true;
    receiver_thread_ = std::thread([this, callback]() {
        // Optionally set CPU affinity for this thread
        if (config_.cpu_affinity_core >= 0) {
            cpu_set_t cpuset;
            CPU_ZERO(&cpuset);
            CPU_SET(config_.cpu_affinity_core, &cpuset);
            int rc = pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
            if (rc != 0) {
                std::cerr << "[UdpReceiver] Failed to set thread affinity: "
                          << strerror(errno) << std::endl;
            }
        }

        // Optionally set real-time priority (if requested)
        if (config_.thread_realtime_priority > 0) {
            sched_param param{};
            param.sched_priority = config_.thread_realtime_priority;
            int rc = pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);
            if (rc != 0) {
                std::cerr << "[UdpReceiver] Failed to set real-time priority: "
                          << strerror(errno) << std::endl;
            }
        }

        // UDP receive loop
        while (running_) {
            char buffer[2048];
            sockaddr_in src_addr{};
            socklen_t addrlen = sizeof(src_addr);
            ssize_t len = recvfrom(
                socket_fd_, buffer, sizeof(buffer), 0,
                (struct sockaddr*)&src_addr, &addrlen);

            if (len > 0) {
                callback(buffer, static_cast<size_t>(len));
            } else if (len < 0 && running_) {
                perror("[UdpReceiver] recvfrom");
            }
        }
        // std::cerr << "Receiver thread exit\n";
    });
}

/**
 * @brief Stops the UDP receiver thread and cleans up resources.
 */
void UdpReceiver::stop() {
    running_ = false;
    if (receiver_thread_.joinable()) {
        receiver_thread_.join();
    }
}
