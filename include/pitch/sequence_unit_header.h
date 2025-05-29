//
// Created by phamanhtan on 29/5/25.
//

#ifndef SEQUENCE_UNIT_HEADER_H
#define SEQUENCE_UNIT_HEADER_H
#pragma once
#include "message.h"
#include <string>
#include <memory>
#include <stdexcept>
#include <cstring>

namespace CboePitch {

    class SeqUnitHeader : public Message {
    private:
        uint32_t hseq;
        uint8_t unit;
        uint8_t count;
        std::string symbol;

    public:
        SeqUnitHeader(uint32_t hs, uint8_t u, uint8_t c, const std::string& sym = "")
            : hseq(hs), unit(u), count(c), symbol(sym) {
            validate();
        }

        static std::unique_ptr<SeqUnitHeader> parse(const uint8_t* data, size_t size) {
            if (size < 8) throw std::invalid_argument("Invalid SeqUnitHeader length");
            uint16_t len;
            std::memcpy(&len, data, 2);
            uint8_t cnt = data[2];
            uint8_t u = data[3];
            uint32_t hs;
            std::memcpy(&hs, data + 4, 4);
            return std::make_unique<SeqUnitHeader>(hs, u, cnt);
        }

        char getMessageType() const override { return 'H'; }
        std::string getSymbol() const override { return symbol; }
        void setSymbol(const std::string& sym) override { symbol = sym; }

        std::string toString() const override {
            char buffer[100];
            snprintf(buffer, sizeof(buffer),
                     "SeqUnitHeader: Sequence=%u, Unit=%u, Count=%u, Symbol=%s",
                     hseq, unit, count, symbol.c_str());
            return std::string(buffer);
        }

    private:
        void validate() {}
    };

} // namespace CboePitch
#endif
