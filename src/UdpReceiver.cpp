#include "UdpReceiver.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

#include <pthread.h>
#include <sched.h>

#ifdef __APPLE__
#include <mach/thread_policy.h>
#include <mach/thread_act.h>
#include <mach/mach.h>
#endif

UdpReceiver::UdpReceiver(const std::string& ip, uint16_t port)
    : running(false), sockfd(-1)
{
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0) {
        perror("socket");
        throw std::runtime_error("Failed to create socket");
    }
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip.c_str());

    if(bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(sockfd);
        throw std::runtime_error("Failed to bind");
    }
}

UdpReceiver::~UdpReceiver() {
    stop();
    if(sockfd >= 0)
        close(sockfd);
}

void UdpReceiver::start(UdpCallback cb, int core_affinity, int rt_priority) {
    affinity_core = core_affinity;
    priority = rt_priority;
    running = true;

    recv_thread = std::thread([this, cb]() {
        /*  Set affinity  */
        if (affinity_core >= 0) {
#ifdef __linux__
            cpu_set_t cpuset;
            CPU_ZERO(&cpuset);
            CPU_SET(affinity_core, &cpuset);
            int rc = pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
            if (rc != 0) {
                std::cerr << "Failed to set thread affinity: " 
                          << strerror(errno) << std::endl;
            }
#elif defined(__APPLE__)
            // macOS thread affinity - limited support
            thread_affinity_policy_data_t policy = { affinity_core };
            thread_port_t mach_thread = pthread_mach_thread_np(pthread_self());
            kern_return_t result = thread_policy_set(mach_thread, THREAD_AFFINITY_POLICY,
                                                   (thread_policy_t)&policy, 1);
            if (result != KERN_SUCCESS) {
                std::cerr << "Failed to set thread affinity on macOS" << std::endl;
            }
#else
            std::cerr << "Thread affinity not supported on this platform" << std::endl;
#endif
        }

        /* Set RT priority */
        if (priority > 0) {
#ifdef __linux__
            sched_param sch_params{};
            sch_params.sched_priority = priority;
            int rc = pthread_setschedparam(
                pthread_self(), SCHED_FIFO, &sch_params
            );
            if (rc != 0) {
                std::cerr << "Failed to set RT thread priority: "
                          << strerror(errno) << " (are you root?)" << std::endl;
            }
#elif defined(__APPLE__)
            // macOS real-time scheduling
            thread_time_constraint_policy_data_t ttcpolicy;
            ttcpolicy.period = 1000000; // 1ms in nanoseconds  
            ttcpolicy.computation = 500000; // 0.5ms
            ttcpolicy.constraint = 1000000; // 1ms
            ttcpolicy.preemptible = 1;
            
            thread_port_t mach_thread = pthread_mach_thread_np(pthread_self());
            kern_return_t result = thread_policy_set(mach_thread, THREAD_TIME_CONSTRAINT_POLICY,
                                                   (thread_policy_t)&ttcpolicy,
                                                   THREAD_TIME_CONSTRAINT_POLICY_COUNT);
            if (result != KERN_SUCCESS) {
                std::cerr << "Failed to set RT thread priority on macOS" << std::endl;
            }
#else
            std::cerr << "RT scheduling not supported on this platform" << std::endl;
#endif
        }

        /* Receive loop */
        while(running) {
            std::vector<char> buffer(2048); // adjust as needed
            sockaddr_in src_addr{};
            socklen_t addrlen = sizeof(src_addr);
            ssize_t len = recvfrom(sockfd, buffer.data(), buffer.size(), 0, 
                                   (struct sockaddr *)&src_addr, &addrlen);
            if(len > 0) {
                buffer.resize(len);
                cb(buffer);
            } else if(len < 0 && running) {
                perror("recvfrom");
            }
        }
    });
}

void UdpReceiver::stop() {
    running = false;
    if(recv_thread.joinable())
        recv_thread.join();
}