#include "UdpReceiver.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <pthread.h>
#include <sched.h>

UdpReceiver::UdpReceiver(const std::string &ip, uint16_t port)
    : sockfd(-1), running(false) // Match declaration order
{
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        throw std::runtime_error("Failed to create socket");
    }
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip.c_str());

    if (bind(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("bind");
        close(sockfd);
        throw std::runtime_error("Failed to bind");
    }
}

UdpReceiver::~UdpReceiver() {
    stop();
    if (sockfd >= 0)
        close(sockfd);
}

void UdpReceiver::start(UdpCallback cb, int core_affinity, int rt_priority) {
    affinity_core = core_affinity;
    priority = rt_priority;
    running = true;

    recv_thread = std::thread([this, cb]() {
        // Set affinity (only on platforms that support it, e.g., Linux)
#if defined(__linux__)
        if (affinity_core >= 0) {
            cpu_set_t cpuset;
            CPU_ZERO(&cpuset);
            CPU_SET(affinity_core, &cpuset);
            int rc = pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
            if (rc != 0) {
                std::cerr << "Failed to set thread affinity: "
                          << strerror(errno) << std::endl;
            }
        }
#else
        if (affinity_core >= 0) {
            std::cerr << "CPU affinity not supported on this platform (e.g., macOS)\n";
        }
#endif

        // Set RT priority
        if (priority > 0) {
            sched_param sch_params{};
            sch_params.sched_priority = priority;
            int rc = pthread_setschedparam(pthread_self(), SCHED_FIFO, &sch_params);
            if (rc != 0) {
                std::cerr << "Failed to set RT thread priority: "
                        << strerror(errno) << " (are you root?)" << std::endl;
            }
        }

        // Receive loop
        while (running) {
            std::vector<char> buffer(2048); // Adjust as needed
            sockaddr_in src_addr{};
            socklen_t addrlen = sizeof(src_addr);
            ssize_t len = recvfrom(sockfd, buffer.data(), buffer.size(), 0,
                                   (struct sockaddr *) &src_addr, &addrlen);
            if (len > 0) {
                buffer.resize(len);
                cb(buffer);
            } else if (len < 0 && running) {
                perror("recvfrom");
            }
        }
    });
}

void UdpReceiver::stop() {
    running = false;
    if (recv_thread.joinable()) {
        // Send a dummy packet to unblock recvfrom
        struct sockaddr_in dummyAddr{};
        dummyAddr.sin_family = AF_INET;
        dummyAddr.sin_port = htons(0);
        dummyAddr.sin_addr.s_addr = INADDR_LOOPBACK;
        sendto(sockfd, "", 1, 0, (struct sockaddr *) &dummyAddr, sizeof(dummyAddr));
        recv_thread.join();
    }
}
