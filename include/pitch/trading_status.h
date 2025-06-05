#ifndef TRADING_STATUS_MESSAGE_H
#define TRADING_STATUS_MESSAGE_H

#include "message.h"
#include <string>
#include <sstream>
#include <stdexcept>

namespace CboePitch {
    class TradingStatus : public Message {
    public:
        static constexpr uint8_t MESSAGE_TYPE = 0x3B;
        static constexpr size_t MESSAGE_SIZE = 22;

        static TradingStatus parse(const uint8_t *data, size_t size, size_t offset = 0) {
            if (size < offset + MESSAGE_SIZE) {
                throw std::invalid_argument("Trading Status message too short");
            }

            uint64_t timestamp = Message::readUint64LE(data + offset + 2);
            std::string symbol = Message::readAscii(data + offset + 10, 6);
            char tradingStatus = static_cast<char>(data[offset + 16]);
            std::string marketId = Message::readAscii(data + offset + 17, 4);

            TradingStatus trading_status(timestamp, symbol, tradingStatus, marketId);
            trading_status.payload.assign(data + offset, data + offset + MESSAGE_SIZE);
            return trading_status;
        }

        std::string toString() const override {
            std::ostringstream oss;
            oss << "TradingStatus{timestamp=" << timestamp
                    << ", symbol=" << symbol
                    << ", tradingStatus=" << tradingStatus
                    << ", marketId=" << marketId
                    << "}";
            return oss.str();
        }

        size_t getMessageSize() const override { return MESSAGE_SIZE; }
        uint8_t getMessageType() const override { return MESSAGE_TYPE; }

        uint64_t getTimestamp() const { return timestamp; }
        const std::string &getSymbol() const override { return symbol; }
        char getTradingStatus() const { return tradingStatus; }
        const std::string &getMarketId() const { return marketId; }

    private:
        uint64_t timestamp;
        std::string symbol;
        char tradingStatus;
        std::string marketId;

        TradingStatus(uint64_t ts, const std::string &sym, char status, const std::string &market)
            : timestamp(ts), symbol(sym), tradingStatus(status), marketId(market) {
        }
    };
} // namespace CboePitch

#endif // TRADING_STATUS_MESSAGE_H
