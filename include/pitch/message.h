#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <cmath>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <iomanip>
#include "SymbolIdentifier.hpp"

namespace CboePitch {
    class Message {
    public:
        virtual ~Message() = default;

        virtual std::string toString() const = 0;

        virtual const std::string &getSymbol() const {
            if (symbol_map_) {
                auto symbol_opt = symbol_map_->find_symbol(getOrderId());
                if (symbol_opt) {
                    cached_symbol_ = *symbol_opt;
                    return cached_symbol_;
                }
            }
            static const std::string empty = "Unknown";
            return empty;
        }

        virtual size_t getMessageSize() const = 0;
        virtual uint8_t getMessageType() const = 0;

        // Get orderId (default implementation returns 0; override in derived classes if applicable)
        virtual uint64_t getOrderId() const {
            return 0; // Default for messages without orderId
        }

        // Set the SymbolIdentifier for symbol lookup
        void setSymbolMap(equix_md::SymbolIdentifier* symbol_map) {
            symbol_map_ = symbol_map;
        }

        // Get the raw message payload
        const std::vector<uint8_t> &getPayload() const {
            return payload;
        }

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

        // Utility to write Little Endian unsigned integers
        static void writeUintLE(uint8_t *ptr, uint64_t value, size_t length) {
            if (length > 8) {
                throw std::invalid_argument("writeUintLE: length too large");
            }
            for (size_t i = 0; i < length; ++i) {
                ptr[i] = static_cast<uint8_t>(value >> (8 * i));
            }
        }

        static void writeUint64LE(uint8_t *ptr, uint64_t value) {
            writeUintLE(ptr, value, 8);
        }

        static void writeUint32LE(uint8_t *ptr, uint32_t value) {
            writeUintLE(ptr, value, 4);
        }

        void printPayloadHex() const {
            if (payload.empty()) {
                std::cout << "Payload (empty)" << std::endl;
                return;
            }
            std::cout << "Payload (hex): ";
            for (size_t i = 0; i < payload.size(); ++i) {
                std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(payload[i]) << " ";
            }
            std::cout << std::dec << std::endl; // Reset to decimal
        }

        // Print payload as decimal array
        void printPayloadDecimal() const {
            std::cout << "Payload (decimal): [";
            for (size_t i = 0; i < payload.size(); ++i) {
                std::cout << static_cast<int>(payload[i]);
                if (i != payload.size() - 1) std::cout << ", ";
            }
            std::cout << "]" << std::endl;
        }

        // Debug print with metadata
        void debugPrintPayload(const std::string &label = "") const {
            if (!label.empty()) {
                std::cout << label << " ";
            }
            std::cout << "[type=0x" << std::hex << static_cast<int>(getMessageType())
                    << ", size=" << std::dec << getMessageSize() << "] ";
            printPayloadHex();
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

        static std::string readAscii(const uint8_t *data, size_t length) {
            return std::string(reinterpret_cast<const char *>(data), length);
        }

        static std::string trimRight(const std::string &str) {
            size_t end = str.find_last_not_of(' ');
            return (end == std::string::npos) ? "" : str.substr(0, end + 1);
        }

    protected:
        std::vector<uint8_t> payload; // Stores the raw message body
        equix_md::SymbolIdentifier* symbol_map_ = nullptr; // Pointer to shared SymbolIdentifier
        mutable std::string cached_symbol_; // Cache for symbol lookup

        // Set the raw message payload during parsing
        void setPayload(const uint8_t *data, size_t length) {
            payload.assign(data, data + length);
        }
    };
} // namespace CboePitch

#endif // MESSAGE_H