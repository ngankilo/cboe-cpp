#include "UdpMessageHandler.hpp"
#include "pitch/seq_unit_header.h"
#include "pitch/message_factory.h"
#include <iostream>

UdpMessageHandler::UdpMessageHandler(MessageProcessor messageProcessor)
    : messageProcessor(messageProcessor) {
}

void UdpMessageHandler::handleUdpPacket(const std::vector<char>& data) {
    try {
        // Cast to uint8_t* as required by our parsing functions
        const uint8_t* rawData = reinterpret_cast<const uint8_t*>(data.data());
        size_t dataSize = data.size();

        // Parse the header first
        CboePitch::SeqUnitHeader header = CboePitch::SeqUnitHeader::parse(rawData, dataSize);

        // Validate the packet length matches the header
        if (header.getLength() > dataSize) {
            std::cerr << "Warning: Header specifies length " << header.getLength() 
                      << " but received only " << dataSize << " bytes" << std::endl;
            return;
        }

        // Decode all messages in the packet
        //auto messages = CboePitch::SeqUnitHeader::decodeMessages(rawData, dataSize);

        // Process each message
        // for (auto& message : messages) {
        //     messageProcessor(std::move(message));
        // }
    } catch (const std::exception& e) {
        std::cerr << "Error processing UDP packet: " << e.what() << std::endl;
    }
}
