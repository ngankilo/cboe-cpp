#include <iostream>
#include "pitch/message_factory.h"
#include "pitch/message_dispatcher.h"
#include "pitch/seq_unit_header.h"
#include "SymbolIdentifier.hpp"
#include <stdexcept>
#include <vector>
#include <sstream>
#include <iomanip>

namespace CboePitch {
    std::shared_ptr<Message> MessageFactory::parseMessage(const uint8_t *data, size_t size, equix_md::SymbolIdentifier& symbol_map) {
        if (size < 1) {
            throw std::invalid_argument("Data too short");
        }
        uint8_t messageType = data[0];
        const auto &dispatchTable = MessageDispatch::getDispatchTable();
        auto it = dispatchTable.find(messageType);
        if (it == dispatchTable.end()) {
            throw std::runtime_error("Unknown message type: " + std::to_string(messageType));
        }

        return it->second.parser(data, size, 0, symbol_map);
    }

    SeqUnitHeader MessageFactory::parseHeader(const uint8_t *data, size_t size) {
        return SeqUnitHeader::parse(data, size);
    }

    std::vector<std::shared_ptr<Message>> MessageFactory::parseMessages(const uint8_t *data, size_t length, equix_md::SymbolIdentifier& symbol_map) {
        if (length < 8) {
            throw std::runtime_error("Data too short for SeqUnitHeader");
        }

        SeqUnitHeader header = SeqUnitHeader::parse(data, length);
        std::vector<std::shared_ptr<Message>> messages;
        messages.reserve(header.getCount());
        size_t offset = 8; // Skip 8-byte header
        size_t remainingLength = header.getLength() - 8;
        if (remainingLength > length - offset) {
            throw std::runtime_error("Insufficient data for all messages");
        }

        const auto &dispatchTable = MessageDispatch::getDispatchTable();

        for (uint8_t i = 0; i < header.getCount() && offset < length; ++i) {
            if (offset >= header.getLength()) break;
            if (remainingLength == 0) break;
            uint8_t messageLength = data[offset];
            uint8_t messageType = data[offset + 1];
            auto it = dispatchTable.find(messageType);
            if (it == dispatchTable.end()) {
                std::cerr << "Skipping unknown message type: " << std::to_string(messageType) << std::endl;
                throw std::runtime_error("Unknown message type: " + std::to_string(messageType));
            }

            const auto &dispatchInfo = it->second;
            size_t msgLength = dispatchInfo.length;

            if (msgLength > remainingLength) {
                throw std::runtime_error("Message length exceeds remaining data");
            }

            std::shared_ptr<Message> msg = dispatchInfo.parser(data, remainingLength, offset, symbol_map);
            // msg->printPayloadHex();
            messages.push_back(msg);

            offset += msgLength;
            remainingLength -= msgLength;
        }

        if (offset != header.getLength()) {
            throw std::runtime_error("Decoded length does not match Hdr Length");
        }

        return messages;
    }
    //
    // std::vector<std::shared_ptr<Message>> MessageFactory::parseMessages(const uint8_t *data, size_t length) {
    //     equix_md::SymbolIdentifier symbol_map; // Create a local SymbolIdentifier
    //     return parseMessages(data, length, symbol_map);
    // }
} // namespace CboePitch