#include "pitch/seq_unit_header.h"
#include "pitch/message_dispatcher.h"
#include <sstream>
#include <stdexcept>

namespace CboePitch {
    SeqUnitHeader SeqUnitHeader::parse(const uint8_t *data, size_t length) {
        if (length < 8) throw std::runtime_error("SeqUnitHeader too short");
        uint16_t len = *reinterpret_cast<const uint16_t *>(&data[0]);
        uint8_t count = data[2];
        uint8_t unit = data[3];
        uint32_t sequence = *reinterpret_cast<const uint32_t *>(&data[4]);
        return SeqUnitHeader(len, count, unit, sequence);
    }

    std::string SeqUnitHeader::toString() const {
        std::stringstream ss;
        ss << "SeqUnitHeader { Length: " << length
                << ", Count: " << static_cast<int>(count)
                << ", Unit: " << static_cast<int>(unit)
                << ", Sequence: " << sequence << " }";
        return ss.str();
    }

    std::vector<std::shared_ptr<Message> > SeqUnitHeader::decodeMessages(const uint8_t *data, size_t length) {
        std::vector<std::shared_ptr<Message> > messages;
        if (length < 8) throw std::runtime_error("Data too short for SeqUnitHeader");
        SeqUnitHeader header = parse(data, length);
        size_t offset = 8;
        size_t remainingLength = header.getLength() - 8;

        if (remainingLength > length - offset) {
            throw std::runtime_error("Insufficient data for all messages");
        }

        const auto &dispatchTable = MessageDispatch::getDispatchTable();

        for (uint8_t i = 0; i < header.getCount() && offset < length; ++i) {
            if (offset >= header.getLength()) break;
            if (remainingLength == 0) break;
            uint8_t messageType = data[offset];
            auto it = dispatchTable.find(messageType);
            if (it == dispatchTable.end()) {
                throw std::runtime_error("Unknown message type: " + std::to_string(messageType));
            }

            const auto &dispatchInfo = it->second;
            size_t msgLength = dispatchInfo.length;

            if (msgLength > remainingLength) {
                throw std::runtime_error("Message length exceeds remaining data");
            }

            std::shared_ptr<Message> msg = dispatchInfo.parser(data + offset, remainingLength);
            messages.push_back(std::move(msg));

            offset += msgLength;
            remainingLength -= msgLength;
        }

        if (offset != header.getLength()) {
            throw std::runtime_error("Decoded length does not match Hdr Length");
        }

        return messages;
    }
} // namespace CboePitch
