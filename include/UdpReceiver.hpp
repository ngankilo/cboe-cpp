#ifndef _UDP_RECEIVER_H_
#define _UDP_RECEIVER_H_

#pragma once
#include <functional>
#include <atomic>
#include <thread>
#include <vector>
#include <string>
#include <cstdint>

/**
 * UdpReceiver: Receives UDP datagrams and calls a callback for each.
 * Optionally sets CPU affinity and real-time priority on the receive thread.
 */
class UdpReceiver {
public:
    using UdpCallback = std::function<void(const std::vector<char>&)>;

    UdpReceiver(const std::string& ip, uint16_t port);
    ~UdpReceiver();

    // core_affinity: CPU core to pin (~thread affinity), -1 for no pin
    // rt_priority: SCHED_FIFO priority (1-99), 0 for default
    void start(UdpCallback cb, int core_affinity = -1, int rt_priority = 0);
    void stop();

private:
    int sockfd;
    std::atomic<bool> running;
    std::thread recv_thread;
    // Store for affinity/priority (not strictly necessary)
    int affinity_core = -1;
    int priority = 0;
};

#endif
