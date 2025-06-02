#ifndef AUCTION_SUMMARY_H
#define AUCTION_SUMMARY_H

#include "message.h"
#include <string>
#include <stdexcept>
#include <cstring>

namespace CboePitch {
    class AuctionSummary : public Message {
    public:
        static AuctionSummary parse(const uint8_t *data, size_t length, size_t offset = 0) {
            if (length < MESSAGE_SIZE) {
                throw std::invalid_argument("AuctionSummary message too short");
            }

            uint64_t timestamp = readUint64LE(data + offset + 2);
            std::string symbol(reinterpret_cast<const char *>(data + offset + 10), 6);
            symbol = symbol.substr(0, symbol.find_last_not_of(' ') + 1); // trim right padding
            char auctionType = static_cast<char>(data[offset + 16]);

            double price = decodePrice(data + offset + 17, 7);
            uint32_t shares = readUint32LE(data + offset + 25);

            return AuctionSummary(timestamp, symbol, auctionType, price, shares);
        }

        std::string toString() const override {
            return "AuctionSummary{timestamp=" + std::to_string(timestamp) +
                   ", symbol=" + symbol +
                   ", auctionType=" + std::string(1, auctionType) +
                   ", clearingPrice=" + std::to_string(clearingPrice) +
                   ", executedQuantity=" + std::to_string(executedQuantity) + "}";
        }

        static constexpr size_t MESSAGE_SIZE = 30;
        static constexpr uint8_t MESSAGE_TYPE = 0x5A;

        size_t getMessageSize() const override { return MESSAGE_SIZE; }
        uint8_t getMessageType() const override { return MESSAGE_TYPE; }

        uint64_t getTimestamp() const { return timestamp; }
        const std::string &getSymbol() const override { return symbol; }
        char getAuctionType() const { return auctionType; }
        double getClearingPrice() const { return clearingPrice; }
        uint32_t getExecutedQuantity() const { return executedQuantity; }

    private:
        uint64_t timestamp;
        std::string symbol;
        char auctionType;
        double clearingPrice;
        uint32_t executedQuantity;

        AuctionSummary(uint64_t ts, const std::string &sym, char auctType,
                       double price, uint32_t qty)
            : timestamp(ts), symbol(sym), auctionType(auctType),
              clearingPrice(price), executedQuantity(qty) {
        }
    };
} // namespace CboePitch

#endif // AUCTION_SUMMARY_H
