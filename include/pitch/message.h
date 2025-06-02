#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>

namespace CboePitch {
    class Message {
    public:
        virtual ~Message() = default;

        virtual std::string toString() const = 0;

        virtual size_t getMessageSize() const = 0;

        virtual uint8_t getMessageType() const = 0;

        // Utility to read Little Endian unsigned integers
        static uint64_t readUintLE(const uint8_t *ptr, size_t length) {
            if (length > 8) {
                throw std::invalid_argument("readUintLE: length too large");
            }
            uint64_t value = 0;
            for (size_t i = 0; i < length; ++i) {
                value |= static_cast<uint64_t>(ptr[i]) << (8 * i);
            }
            return value;
        }

        static uint64_t readUint64LE(const uint8_t *ptr) {
            return readUintLE(ptr, 8);
        }

        static uint32_t readUint32LE(const uint8_t *ptr) {
            return static_cast<uint32_t>(readUintLE(ptr, 4));
        }

        // Price decode: raw 8-byte LE uint64_t → double with 7 decimal places
        static double decodePrice(const uint8_t *ptr, int decimalPlaces = 7) {
            uint64_t raw = readUint64LE(ptr);
            return static_cast<double>(raw) / std::pow(10.0, decimalPlaces);
        }

        // Price encode: double → raw uint64_t with 7 decimal places
        static uint64_t encodePrice(double price) {
            return static_cast<uint64_t>(price * 10000000.0 + 0.5); // round to nearest
        }
        static std::string readAscii(const uint8_t* data, size_t length) {
            return std::string(reinterpret_cast<const char*>(data), length);
        }

        static std::string trimRight(const std::string& str) {
            size_t end = str.find_last_not_of(' ');
            return (end == std::string::npos) ? "" : str.substr(0, end + 1);
        }
    };
} // namespace CboePitch

#endif // MESSAGE_H
