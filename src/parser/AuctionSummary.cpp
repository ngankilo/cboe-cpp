//
// Created by phamanhtan on 30/5/25.
//

#include "pitch/auction_summary.h"
#include <sstream>

namespace CboePitch {
    AuctionSummary AuctionSummary::parse(const uint8_t *data, size_t length) {
        if (length < 26) throw std::runtime_error("AuctionSummary message too short");
        uint64_t timestamp = *reinterpret_cast<const uint64_t *>(&data[0]); // Little Endian
        std::string symbol(reinterpret_cast<const char *>(&data[8]), 6);
        uint32_t executedQuantity = *reinterpret_cast<const uint32_t *>(&data[14]); // Little Endian
        int64_t rawPrice = *reinterpret_cast<const int64_t *>(&data[18]); // Little Endian
        double clearingPrice = rawPrice / 10000000.0; // 7 decimal places
        return AuctionSummary(timestamp, symbol, executedQuantity, clearingPrice);
    }

    // std::string AuctionSummary::toString() const {
    //     std::stringstream ss;
    //     ss << "AuctionSummary { Timestamp: " << timestamp
    //             << ", Symbol: " << symbol
    //             << ", ExecutedQuantity: " << executedQuantity
    //             << ", ClearingPrice: " << clearingPrice << " }";
    //     return ss.str();
    // }
} // namespace CboePitch
