#ifndef UNIT_CLEAR_MESSAGE_H
#define UNIT_CLEAR_MESSAGE_H

#include "message.h"
#include <sstream>
#include <stdexcept>

namespace CboePitch {
    class UnitClear : public Message {
    public:
        static constexpr uint8_t MESSAGE_TYPE = 0x97;
        static constexpr size_t MESSAGE_SIZE = 6;

        static UnitClear parse(const uint8_t *data, size_t size, size_t offset = 0) {
            if (size < offset + MESSAGE_SIZE) {
                throw std::invalid_argument("Unit Clear message too short");
            }

            uint32_t reserved = Message::readUint32LE(data + offset + 2);
            return UnitClear(reserved);
        }

        std::string toString() const override {
            std::ostringstream oss;
            oss << "UnitClear{reserved=0x" << std::hex << reserved << "}";
            return oss.str();
        }

        size_t getMessageSize() const override { return MESSAGE_SIZE; }
        uint8_t getMessageType() const override { return MESSAGE_TYPE; }

        uint32_t getReserved() const { return reserved; }

    private:
        uint32_t reserved;

        explicit UnitClear(uint32_t res)
            : reserved(res) {
        }
    };
} // namespace CboePitch

#endif // UNIT_CLEAR_MESSAGE_H
