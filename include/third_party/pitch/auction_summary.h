//
// Created by phamanhtan on 29/5/25.
//

#ifndef AUCTION_SUMMARY_H
#define AUCTION_SUMMARY_H
#pragma once
#include "message.h"
#include <string>
#include <memory>
#include <stdexcept>
#include <cstring>

namespace CboePitch {

class AuctionSummary : public Message {
private:
    uint64_t timestamp;
    std::string symbol;
    char auctionType;
    uint64_t price;
    uint32_t shares;

public:
    AuctionSummary(uint64_t ts, const std::string& sym, char at, uint64_t prc, uint32_t sh)
        : timestamp(ts), symbol(sym), auctionType(at), price(prc), shares(sh) {
        validate();
    }

    static std::unique_ptr<AuctionSummary> parse(const uint8_t* data, size_t size) {
        if (size < 30) throw std::invalid_argument("Invalid AuctionSummary length");
        if (data[1] != 0x5A) throw std::invalid_argument("Invalid AuctionSummary type");
        uint64_t ts, prc;
        std::memcpy(&ts, data + 2, 8);
        std::string sym(reinterpret_cast<const char*>(data + 10), 6);
        char at = data[16];
        std::memcpy(&prc, data + 17, 8);
        uint32_t sh;
        std::memcpy(&sh, data + 25, 4);
        return std::make_unique<AuctionSummary>(ts, sym, at, prc, sh);
    }

    char getMessageType() const override { return 0x5A; }
    std::string getSymbol() const override { return symbol; }
    void setSymbol(const std::string& sym) override {
        if (sym.size() > 6) throw std::invalid_argument("Symbol must be <= 6 characters");
        symbol = sym;
    }

    std::string toString() const override {
        char buffer[150];
        snprintf(buffer, sizeof(buffer),
                 "AuctionSummary: Timestamp=%llu, Symbol=%s, AuctionType=%c, Price=%llu, Shares=%u",
                 timestamp, symbol.c_str(), auctionType, price, shares);
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
#endif //AUCTION_SUMMARY_H
