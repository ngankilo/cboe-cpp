//
// Created by phamanhtan on 30/5/25.
//

#ifndef AUCTION_SUMMARY_H
#define AUCTION_SUMMARY_H

#include "message.h"

namespace CboePitch {
    class AuctionSummary final : public Message {
    public:
        static AuctionSummary parse(const uint8_t *data, size_t length);

        std::string toString() const override;

        std::string getSymbol() const override { return symbol; }
        void setSymbol(const std::string &sym) override { symbol = sym; }

        uint64_t getTimestamp() const { return timestamp; }
        uint32_t getExecutedQuantity() const { return executedQuantity; }
        double getClearingPrice() const { return clearingPrice; }

    private:
        uint64_t timestamp;
        std::string symbol;
        uint32_t executedQuantity;
        double clearingPrice;

        AuctionSummary(uint64_t ts, const std::string &sym, uint32_t qty, double price)
            : Message(0x5A), timestamp(ts), symbol(sym), executedQuantity(qty), clearingPrice(price) {
        }
    };
} // namespace CboePitch

#endif // AUCTION_SUMMARY_H
