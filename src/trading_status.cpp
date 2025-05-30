//
// Created by phamanhtan on 30/5/25.
//

#include "pitch/trading_status.h"
#include <sstream>

namespace CboePitch {
    TradingStatus TradingStatus::parse(const uint8_t *data, size_t length) {
        if (length < 14) throw std::runtime_error("TradingStatus message too short");
        uint64_t timestamp = *reinterpret_cast<const uint64_t *>(&data[0]); // Little Endian
        std::string symbol(reinterpret_cast<const char *>(&data[8]), 6);
        char tradingStatus = static_cast<char>(data[14]);
        std::string marketIdCode(reinterpret_cast<const char *>(&data[15]), 4);
        return TradingStatus(timestamp, symbol, tradingStatus, marketIdCode);
    }

    std::string TradingStatus::toString() const {
        std::stringstream ss;
        ss << "TradingStatus { Timestamp: " << timestamp
                << ", Symbol: " << symbol
                << ", TradingStatus: " << tradingStatus
                << ", MarketIdCode: " << marketIdCode << " }";
        return ss.str();
    }
} // namespace CboePitch
