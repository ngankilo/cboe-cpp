//
// Created by phamanhtan on 29/5/25.
//

#ifndef UNIT_CLEAR_H
#define UNIT_CLEAR_H
#pragma once
#include "message.h"
#include <string>
#include <stdexcept>
#pragma once
#include <memory>

namespace CboePitch {
    class UnitClear : public Message {
    private:
        std::string symbol;

    public:
        UnitClear(const std::string &sym = "") : symbol(sym) {
        }

        static std::unique_ptr<UnitClear> parse(const uint8_t *data, size_t size) {
            if (size < 6) throw std::invalid_argument("Invalid UnitClear length");
            if (data[1] != 0x97) throw std::invalid_argument("Invalid UnitClear type");
            return std::make_unique<UnitClear>();
        }

        char getMessageType() const override { return 0x97; }
        std::string getSymbol() const override { return symbol; }
        void setSymbol(const std::string &sym) override { symbol = sym; }

        std::string toString() const override {
            return "UnitClear: Symbol=" + symbol;
        }
    };
} // namespace CboePitch
#endif //UNIT_CLEAR_H
