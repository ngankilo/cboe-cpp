//
// Created by phamanhtan on 29/5/25.
//

#ifndef AUCTION_UPDATE_H
#define AUCTION_UPDATE_H
#pragma once
#include <string>
#include <memory>
#include <stdexcept>
#include <cstring>
#pragma once

namespace CboePitch {
    class AuctionUpdate : public Message {
    private:
        uint64_t timestamp;
        std::string symbol;
        char auctionType;
        uint32_t buyShares;
        uint32_t sellShares;
        uint64_t indicativePrice;

    public:
        AuctionUpdate(uint64_t ts, const std::string &sym, char at, uint32_t bs, uint32_t ss, uint64_t ip)
            : timestamp(ts), symbol(sym), auctionType(at), buyShares(bs), sellShares(ss), indicativePrice(ip) {
            validate();
        }

        static std::unique_ptr<AuctionUpdate> parse(const uint8_t *data, size_t size) {
            if (size < 34) throw std::invalid_argument("Invalid AuctionUpdate length");
            if (data[1] != 0x59) throw std::invalid_argument("Invalid AuctionUpdate type");
            uint64_t ts, ip;
            std::memcpy(&ts, data + 2, 8);
            std::string sym(reinterpret_cast<const char *>(data + 10), 6);
            char at = data[16];
            uint32_t bs, ss;
            std::memcpy(&bs, data + 17, 4);
            std::memcpy(&ss, data + 21, 4);
            std::memcpy(&ip, data + 25, 8);
            return std::make_unique<AuctionUpdate>(ts, sym, at, bs, ss, ip);
        }

        char getMessageType() const override { return 0x59; }
        std::string getSymbol() const override { return symbol; }

        void setSymbol(const std::string &sym) override {
            if (sym.size() > 6) throw std::invalid_argument("Symbol must be <= 6 characters");
            symbol = sym;
        }

        std::string toString() const override {
            char buffer[150];
            snprintf(buffer, sizeof(buffer),
                     "AuctionUpdate: Timestamp=%llu, Symbol=%s, AuctionType=%c, BuyShares=%u, SellShares=%u, IndicativePrice=%llu",
                     timestamp, symbol.c_str(), auctionType, buyShares, sellShares, indicativePrice);
            return std::string(buffer);
        }

    private:
        void validate() {
            if (symbol.size() > 6) throw std::invalid_argument("Symbol must be <= 6 characters");
            if (auctionType != 'O' && auctionType != 'C' && auctionType != 'H')
                throw std::invalid_argument("Invalid AuctionType");
        }
    };
} // namespace CboePitch
#endif //AUCTION_UPDATE_H
