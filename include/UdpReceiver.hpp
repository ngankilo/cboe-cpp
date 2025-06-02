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

#include <string>
#include <thread>
#include <atomic>
#include <functional>
#include <cstdint>
#include <mutex>
#include <netinet/in.h>
/**
 * @class UdpReceiver
 * @brief Receives UDP datagrams asynchronously and calls a user callback for each packet.
 */
class UdpReceiver {
public:
    /**
     * @struct Config
     * @brief Receiver configuration for binding address and thread options.
     */
    struct Config {
        std::string bind_ip = "0.0.0.0";     ///< IP address to bind for receiving UDP datagrams.
        uint16_t bind_port = 9000;           ///< Port to bind for receiving UDP datagrams.
        int cpu_affinity_core = -1;          ///< CPU core index for affinity (-1 means no affinity).
        int thread_realtime_priority = 0;    ///< Realtime priority for receiver thread (0 = normal).
    };

    using PacketCallback = std::function<void(const char* data, size_t size)>;

    /**
     * @brief Construct a new UdpReceiver instance with the given configuration.
     * @param config Configuration options for binding and thread control.
     */
    explicit UdpReceiver(const Config& config);

    /**
     * @brief Destructor. Stops the receiver and closes resources.
     */
    ~UdpReceiver();

    /**
     * @brief Start the UDP receiver. Each datagram triggers the user callback.
     * @param callback User-supplied function invoked for each UDP packet.
     */
    void start(PacketCallback callback);

    /**
     * @brief Stop the receiver thread and release all resources.
     */
    void stop();

private:
    int socket_fd_;                         ///< File descriptor for the UDP socket.
    std::atomic<bool> running_{false};      ///< Whether the receiver is currently running.
    std::thread receiver_thread_;           ///< Background thread for receiving UDP datagrams.
    Config config_;                         ///< Configuration instance for this receiver.
};

#endif // UDP_RECEIVER_HPP_
