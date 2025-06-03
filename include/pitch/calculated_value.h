#ifndef CALCULATED_VALUE_H
#define CALCULATED_VALUE_H

#include "message.h"
#include <string>
#include <stdexcept>

namespace CboePitch {
    class CalculatedValue : public Message {
    public:
        static CalculatedValue parse(const uint8_t *data, size_t length, size_t offset = 0) {
            if (length < MESSAGE_SIZE) {
                throw std::invalid_argument("CalculatedValue message too short");
            }

            uint64_t timestamp = readUint64LE(data + offset + 2);
            std::string symbol(reinterpret_cast<const char *>(data + offset + 10), 6);
            symbol = symbol.substr(0, symbol.find_last_not_of(' ') + 1); // trim right spaces

            char valueCategory = static_cast<char>(data[offset + 16]);

            double value = decodePrice(data + offset + 17, 7);

            uint64_t valueTimestamp = readUint64LE(data + offset + 25);

             CalculatedValue calculated_value(timestamp, symbol, valueCategory, value, valueTimestamp);
            calculated_value.payload.assign(data + offset, data + offset + MESSAGE_SIZE);
            return calculated_value;
        }

        std::string toString() const override {
            return "CalculatedValue{timestamp=" + std::to_string(timestamp) +
                   ", symbol=" + symbol +
                   ", valueCategory=" + std::string(1, valueCategory) +
                   ", value=" + std::to_string(value) +
                   ", valueTimestamp=" + std::to_string(valueTimestamp) + "}";
        }

        static constexpr size_t MESSAGE_SIZE = 33;
        static constexpr uint8_t MESSAGE_TYPE = 0xE3;

        size_t getMessageSize() const override { return MESSAGE_SIZE; }
        uint8_t getMessageType() const override { return MESSAGE_TYPE; }

        uint64_t getTimestamp() const { return timestamp; }
        const std::string &getSymbol() const override { return symbol; }
        char getValueCategory() const { return valueCategory; }
        double getValue() const { return value; }
        uint64_t getValueTimestamp() const { return valueTimestamp; }

    private:
        uint64_t timestamp;
        std::string symbol;
        char valueCategory;
        double value;
        uint64_t valueTimestamp;

        CalculatedValue(uint64_t ts, const std::string &sym, char category,
                        double val, uint64_t valTs)
            : timestamp(ts), symbol(sym), valueCategory(category), value(val), valueTimestamp(valTs) {
        }
    };
} // namespace CboePitch

#endif // CALCULATED_VALUE_H
