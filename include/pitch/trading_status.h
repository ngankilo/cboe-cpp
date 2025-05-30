//
// Created by phamanhtan on 30/5/25.
//

#ifndef TRADING_STATUS_H
#define TRADING_STATUS_H

#include "message.h"

namespace CboePitch {
    class TradingStatus : public Message {
    public:
        static TradingStatus parse(const uint8_t *data, size_t length);

        std::string toString() const override;

        std::string getSymbol() const override { return symbol; }
        void setSymbol(const std::string &sym) override { symbol = sym; }

        uint64_t getTimestamp() const { return timestamp; }
        std::string getTradingStatus() const { return std::string(1, tradingStatus); }
        std::string getMarketIdCode() const { return marketIdCode; }

    private:
        uint64_t timestamp;
        std::string symbol;
        char tradingStatus;
        std::string marketIdCode;

        TradingStatus(uint64_t ts, const std::string &sym, char tsStatus, const std::string &mktId)
            : Message(0x3B), timestamp(ts), symbol(sym), tradingStatus(tsStatus), marketIdCode(mktId) {
        }
    };
} // namespace CboePitch

#endif // TRADING_STATUS_H
