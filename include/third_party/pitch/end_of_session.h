//
// Created by phamanhtan on 29/5/25.
//

#ifndef END_OF_SESSION_H
#define END_OF_SESSION_H
#pragma once
#include "message.h"
#include <string>
#include <memory>
#include <stdexcept>
#include <cstring>

namespace CboePitch {

    class TradeBreak : public Message {
    private:
        uint64_t timestamp;
        uint64_t executionId;
        std::string symbol;

    public:
        TradeBreak(uint64_t ts, uint64_t eid, const std::string& sym = "")
            : timestamp(ts), executionId(eid), symbol(sym) {
            validate();
        }

        static std::unique_ptr<TradeBreak> parse(const uint8_t* data, size_t size) {
            if (size < 18) throw std::invalid_argument("Invalid TradeBreak length");
            if (data[1] != 0x3E) throw std::invalid_argument("Invalid TradeBreak type");
            uint64_t ts, eid;
            std::memcpy(&ts, data + 2, 8);
            std::memcpy(&eid, data + 10, 8);
            std::string sym(6, ' ');
            return std::make_unique<TradeBreak>(ts, eid, sym);
        }

        char getMessageType() const override { return 0x3E; }
        std::string getSymbol() const override { return symbol; }
        void setSymbol(const std::string& sym) override {
            if (sym.size() > 6) throw std::invalid_argument("Symbol must be <= 6 characters");
            symbol = sym;
        }

        std::string toString() const override {
            char buffer[100];
            snprintf(buffer, sizeof(buffer),
                     "TradeBreak: Timestamp=%llu, ExecutionId=%llu, Symbol=%s",
                     timestamp, executionId, symbol.c_str());
            return std::string(buffer);
        }

    private:
        void validate() {
            if (symbol.size() > 6) throw std::invalid_argument("Symbol must be <= 6 characters");
        }
    };

} // namespace CboePitch
#endif //END_OF_SESSION_H
