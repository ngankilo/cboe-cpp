#ifndef UDP_MESSAGE_HANDLER_HPP
#define UDP_MESSAGE_HANDLER_HPP

#include <vector>
#include <functional>
#include <memory>
#include "pitch/message.h"

/**
 * Handler for UDP messages that follows the CBOE PITCH protocol
 * with Sequenced Unit Header structure.
 */
class UdpMessageHandler {
public:
    // Callback type for processing parsed messages
    using MessageProcessor = std::function<void(std::unique_ptr<CboePitch::Message>)>;

    explicit UdpMessageHandler(MessageProcessor messageProcessor);

    // Process a UDP packet, extracting and handling all contained messages
    void handleUdpPacket(const std::vector<char>& data);

private:
    MessageProcessor messageProcessor;
};

#endif // UDP_MESSAGE_HANDLER_HPP
