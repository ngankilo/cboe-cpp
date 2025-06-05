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
#include <fcntl.h>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <errno.h>
#include <pthread.h>

#ifdef __linux__
#include <sched.h>
#endif

/**
 * @brief Constructs and binds the UDP socket to the specified IP and port.
 */
UdpReceiver::UdpReceiver(const Config &config)
    : config_(config) {
    socket_fd_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd_ < 0) {
        throw std::runtime_error("Failed to create UDP socket: " + std::string(strerror(errno)));
    }

    // Optional: allow address reuse
    int opt = 1;
    setsockopt(socket_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(config_.bind_port);
    addr.sin_addr.s_addr = inet_addr(config_.bind_ip.c_str());

    if (bind(socket_fd_, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        close(socket_fd_);
        throw std::runtime_error("Failed to bind UDP socket: " + std::string(strerror(errno)));
    }

    // Set socket to non-blocking mode (for safe shutdown)
    fcntl(socket_fd_, F_SETFL, fcntl(socket_fd_, F_GETFL, 0) | O_NONBLOCK);
}

UdpReceiver::~UdpReceiver() {
    stop();
}

void UdpReceiver::start(PacketCallback callback) {
    if (running_) return;

    running_ = true;

    receiver_thread_ = std::thread([this, callback]() {
        setup_thread_affinity();
        setup_realtime_priority();

        char buffer[2048];

        while (running_) {
            sockaddr_in src_addr{};
            socklen_t addrlen = sizeof(src_addr);

            ssize_t len = recvfrom(socket_fd_, buffer, sizeof(buffer), 0,
                                   (struct sockaddr *) &src_addr, &addrlen);

            if (len > 0) {
                callback(std::vector<char>(buffer, buffer + len));
            } else if (len < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    continue;
                }
                if (running_) {
                    std::cerr << "[UdpReceiver] recvfrom error: " << strerror(errno) << std::endl;
                }
            }
        }
    });
}

void UdpReceiver::stop() {
    running_ = false;
    if (receiver_thread_.joinable()) {
        receiver_thread_.join();
    }

    if (socket_fd_ >= 0) {
        close(socket_fd_);
        socket_fd_ = -1;
    }
}

void UdpReceiver::setup_thread_affinity() {
#ifdef __linux__
    if (config_.cpu_affinity_core < 0) return;

    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(config_.cpu_affinity_core, &cpuset);
    int rc = pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
    if (rc != 0) {
        std::cerr << "[UdpReceiver] Failed to set CPU affinity: " << strerror(errno) << std::endl;
    }
#else
    if (config_.cpu_affinity_core >= 0) {
        std::cerr << "[UdpReceiver] CPU affinity not supported on this platform.\n";
    }
#endif
}

void UdpReceiver::setup_realtime_priority() {
#ifdef __linux__
    if (config_.thread_realtime_priority <= 0) return;

    sched_param param{};
    param.sched_priority = config_.thread_realtime_priority;
    int rc = pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);
    if (rc != 0) {
        std::cerr << "[UdpReceiver] Failed to set real-time priority: " << strerror(errno) << std::endl;
    }
#else
    if (config_.thread_realtime_priority > 0) {
        std::cerr << "[UdpReceiver] Real-time priority not supported on this platform.\n";
    }
#endif
}
