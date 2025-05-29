#pragma once
#include "trade_base.h"
#include <string>
#include <memory>
#include <stdexcept>

namespace CboePitch {

class TradeLong : public TradeBase {
private:
    std::string orderId;
    char sideIndicator;
    uint64_t price;
    std::string executionId;

public:
    TradeLong(uint32_t ts, const std::string& oid, char side, uint32_t sh,
              const std::string& sym, uint64_t pr, const std::string& eid)
        : TradeBase(ts, sh, sym), orderId(oid), sideIndicator(side), price(pr), executionId(eid) {
        validate();
    }

    static std::unique_ptr<TradeLong> parse(const std::string& line) {
        if (line.size() < 39) {
            throw std::invalid_argument("Invalid TradeLong length");
        }
        try {
            uint32_t ts = std::stoul(line.substr(0, 8));
            std::string oid = line.substr(9, 12);
            char side = line[21];
            uint32_t sh = std::stoul(line.substr(22, 6));
            std::string sym = line.substr(28, 8);
            uint64_t pr = std::stoull(line.substr(36, 10));
            std::string eid = line.substr(46, 12);
            return std::make_unique<TradeLong>(ts, oid, side, sh, sym, pr, eid);
        } catch (const std::exception& e) {
            throw std::invalid_argument("TradeLong parse error: " + std::string(e.what()));
        }
    }

    char getMessageType() const override { return 'r'; }

    std::string toString() const override {
        char buffer[100];
        snprintf(buffer, sizeof(buffer),
                 "TradeLong: Timestamp=%u, OrderID=%s, Side=%c, Shares=%u, "
                 "Symbol=%s, Price=%llu, ExecutionID=%s",
                 timestamp, orderId.c_str(), sideIndicator, shares,
                 symbol.c_str(), price, executionId.c_str());
        return std::string(buffer);
    }

private:
    void validate() {
        TradeBase::validate();
        if (orderId.size() != 12) throw std::invalid_argument("Order ID must be 12 characters");
        if (sideIndicator != 'B' && sideIndicator != 'S') throw std::invalid_argument("Side must be 'B' or 'S'");
        if (executionId.size() != 12) throw std::invalid_argument("Execution ID must be 12 characters");
    }
};

} // namespace CboePitch