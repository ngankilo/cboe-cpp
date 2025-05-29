//
// Created by phamanhtan on 29/5/25.
//

#ifndef TRADING_STATUS_H
#define TRADING_STATUS_H
#pragma once
#include "message.h"
#include <string>
#include <memory>
#include <stdexcept>
#include <cstring>

namespace CboePitch {

    class TradingStatus : public Message {
    private:
        uint64_t timestamp;
        std::string symbol;
        char tradingStatus;
        std::string marketIdCode;

    public:
        TradingStatus(uint64_t ts, const std::string& sym, char status, const std::string& mic)
            : timestamp(ts), symbol(sym), tradingStatus(status), marketIdCode(mic) {
            validate();
        }

        static std::unique_ptr<TradingStatus> parse(const uint8_t* data, size_t size) {
            if (size < 22) throw std::invalid_argument("Invalid TradingStatus length");
            if (data[1] != 0x3B) throw std::invalid_argument("Invalid TradingStatus type");
            uint64_t ts;
            std::memcpy(&ts, data + 2, 8);
            std::string sym(reinterpret_cast<const char*>(data + 10), 6);
            char status = data[16];
            std::string mic(reinterpret_cast<const char*>(data + 17), 4);
            return std::make_unique<TradingStatus>(ts, sym, status, mic);
        }

        char getMessageType() const override { return 0x3B; }
        std::string getSymbol() const override { return symbol; }
        void setSymbol(const std::string& sym) override {
            if (sym.size() > 6) throw std::invalid_argument("Symbol must be <= 6 characters");
            symbol = sym;
        }

        std::string toString() const override {
            char buffer[100];
            snprintf(buffer, sizeof(buffer),
                     "TradingStatus: Timestamp=%llu, Symbol=%s, Status=%c, MarketIdCode=%s",
                     timestamp, symbol.c_str(), tradingStatus, marketIdCode.c_str());
            return std::string(buffer);
        }

    private:
        void validate() {
            if (symbol.size() > 6) throw std::invalid_argument("Symbol must be <= 6 characters");
            if (marketIdCode.size() > 4) throw std::invalid_argument("MarketIdCode must be <= 4 characters");
        }
    };

} // namespace CboePitch
#endif //TRADING_STATUS_H
