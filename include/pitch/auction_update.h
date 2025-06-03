#ifndef AUCTION_UPDATE_H
#define AUCTION_UPDATE_H

#include "message.h"
#include <string>
#include <stdexcept>
#include <cstring>

namespace CboePitch {
    class AuctionUpdate : public Message {
    public:
        static AuctionUpdate parse(const uint8_t *data, size_t length, size_t offset = 0) {
            if (length < MESSAGE_SIZE) {
                throw std::invalid_argument("AuctionUpdate message too short");
            }

            uint64_t timestamp = readUint64LE(data + offset + 2);
            std::string symbol(reinterpret_cast<const char *>(data + offset + 10), 6);
            symbol = symbol.substr(0, symbol.find_last_not_of(' ') + 1); // Trim right-padding spaces
            char auctionType = static_cast<char>(data[offset + 16]);

            uint32_t buyShares = readUint32LE(data + offset + 17);
            uint32_t sellShares = readUint32LE(data + offset + 21);
            double indicativePrice = decodePrice(data + offset + 25, 7);

            AuctionUpdate auction_update(timestamp, symbol, auctionType, buyShares, sellShares, indicativePrice);
            auction_update.payload.assign(data + offset, data + offset + MESSAGE_SIZE);
            return auction_update;
        }

        std::string toString() const override {
            return "AuctionUpdate{timestamp=" + std::to_string(timestamp) +
                   ", symbol=" + symbol +
                   ", auctionType=" + std::string(1, auctionType) +
                   ", buyShares=" + std::to_string(buyShares) +
                   ", sellShares=" + std::to_string(sellShares) +
                   ", indicativePrice=" + std::to_string(indicativePrice) + "}";
        }

        static constexpr size_t MESSAGE_SIZE = 34;
        static constexpr uint8_t MESSAGE_TYPE = 0x59;

        size_t getMessageSize() const override { return MESSAGE_SIZE; }
        uint8_t getMessageType() const override { return MESSAGE_TYPE; }

        uint64_t getTimestamp() const { return timestamp; }
        const std::string &getSymbol() const override { return symbol; }
        char getAuctionType() const { return auctionType; }
        uint32_t getBuyShares() const { return buyShares; }
        uint32_t getSellShares() const { return sellShares; }
        double getIndicativePrice() const { return indicativePrice; }

    private:
        uint64_t timestamp;
        std::string symbol;
        char auctionType;
        uint32_t buyShares;
        uint32_t sellShares;
        double indicativePrice;

        AuctionUpdate(uint64_t ts, const std::string &sym, char type,
                      uint32_t buy, uint32_t sell, double price)
            : timestamp(ts), symbol(sym), auctionType(type),
              buyShares(buy), sellShares(sell), indicativePrice(price) {
        }
    };
} // namespace CboePitch

#endif // AUCTION_UPDATE_H
