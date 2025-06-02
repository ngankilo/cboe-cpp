//
// Created by phamanhtan on 30/5/25.
//

#include "pitch/auction_update.h"
#include <sstream>

namespace CboePitch {
    AuctionUpdate AuctionUpdate::parse(const uint8_t *data, size_t length) {
        if (length < 27) throw std::runtime_error("AuctionUpdate message too short");
        uint64_t timestamp = *reinterpret_cast<const uint64_t *>(&data[0]); // Little Endian
        std::string symbol(reinterpret_cast<const char *>(&data[8]), 6);
        char auctionType = static_cast<char>(data[14]);
        uint32_t indicativeQuantity = *reinterpret_cast<const uint32_t *>(&data[15]); // Little Endian
        int64_t rawPrice = *reinterpret_cast<const int64_t *>(&data[19]); // Little Endian
        double indicativePrice = rawPrice / 10000000.0; // 7 decimal places
        return AuctionUpdate(timestamp, symbol, auctionType, indicativeQuantity, indicativePrice);
    }

    // std::string AuctionUpdate::toString() const {
    //     std::stringstream ss;
    //     ss << "AuctionUpdate { Timestamp: " << timestamp
    //             << ", Symbol: " << symbol
    //             << ", AuctionType: " << auctionType
    //             << ", IndicativeQuantity: " << indicativeQuantity
    //             << ", IndicativePrice: " << indicativePrice << " }";
    //     return ss.str();
    // }
} // namespace CboePitch
