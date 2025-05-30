//
// Created by phamanhtan on 30/5/25.
//

#ifndef AUCTION_UPDATE_H
#define AUCTION_UPDATE_H

#include "message.h"

namespace CboePitch {
    class AuctionUpdate final : public Message {
    public:
        static AuctionUpdate parse(const uint8_t *data, size_t length);

        std::string toString() const override;

        std::string getSymbol() const override { return symbol; }
        void setSymbol(const std::string &sym) override { symbol = sym; }

        uint64_t getTimestamp() const { return timestamp; }
        char getAuctionType() const { return auctionType; }
        uint32_t getIndicativeQuantity() const { return indicativeQuantity; }
        double getIndicativePrice() const { return indicativePrice; }

    private:
        uint64_t timestamp;
        std::string symbol;
        char auctionType;
        uint32_t indicativeQuantity;
        double indicativePrice;

        AuctionUpdate(uint64_t ts, const std::string &sym, char atype, uint32_t qty, double price)
            : Message(0x59), timestamp(ts), symbol(sym), auctionType(atype),
              indicativeQuantity(qty), indicativePrice(price) {
        }
    };
} // namespace CboePitch

#endif // AUCTION_UPDATE_H
