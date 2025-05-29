//
// Created by phamanhtan on 29/5/25.
//

#ifndef CALCULATED_VALUE_H
#define CALCULATED_VALUE_H
#pragma once
#include "message.h"
#include <string>
#include <memory>
#include <stdexcept>
#include <cstring>

namespace CboePitch {
    class CalculatedValue : public Message {
    private:
        uint64_t timestamp;
        std::string symbol;
        char valueCategory;
        uint64_t value;
        uint64_t valueTimestamp;

    public:
        CalculatedValue(uint64_t ts, const std::string &sym, char vc, uint64_t val, uint64_t vts)
            : timestamp(ts), symbol(sym), valueCategory(vc), value(val), valueTimestamp(vts) {
            validate();
        }

        static std::unique_ptr<CalculatedValue> parse(const uint8_t *data, size_t size) {
            if (size < 33) throw std::invalid_argument("Invalid CalculatedValue length");
            if (data[1] != 0xE3) throw std::invalid_argument("Invalid CalculatedValue type");
            uint64_t ts, val, vts;
            std::memcpy(&ts, data + 2, 8);
            std::string sym(reinterpret_cast<const char *>(data + 10), 6);
            char vc = data[16];
            std::memcpy(&val, data + 17, 8);
            std::memcpy(&vts, data + 25, 8);
            return std::make_unique<CalculatedValue>(ts, sym, vc, val, vts);
        }

        char getMessageType() const override { return 0xE3; }
        std::string getSymbol() const override { return symbol; }

        void setSymbol(const std::string &sym) override {
            if (sym.size() > 6) throw std::invalid_argument("Symbol must be <= 6 characters");
            symbol = sym;
        }

        std::string toString() const override {
            char buffer[150];
            snprintf(buffer, sizeof(buffer),
                     "CalculatedValue: Timestamp=%llu, Symbol=%s, ValueCategory=%c, Value=%llu, ValueTimestamp=%llu",
                     timestamp, symbol.c_str(), valueCategory, value, valueTimestamp);
            return std::string(buffer);
        }

    private:
        void validate() {
            if (symbol.size() > 6) throw std::invalid_argument("Symbol must be <= 6 characters");
            if (valueCategory < '1' || valueCategory > '4') throw std::invalid_argument("Invalid ValueCategory");
        }
    };
} // namespace CboePitch
#endif //CALCULATED_VALUE_H
