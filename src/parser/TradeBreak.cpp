//
// Created by phamanhtan on 30/5/25.
//

#include "pitch/trade_break.h"
#include <sstream>

namespace CboePitch {
    // TradeBreak TradeBreak::parse(const uint8_t *data, size_t length) {
    //     if (length < 16) throw std::runtime_error("TradeBreak message too short");
    //     uint64_t timestamp = *reinterpret_cast<const uint64_t *>(&data[0]); // Little Endian
    //     uint64_t executionId = *reinterpret_cast<const uint64_t *>(&data[8]); // Little Endian
    //     return TradeBreak(timestamp, executionId);
    // }

    // std::string TradeBreak::toString() const {
    //     std::stringstream ss;
    //     ss << "TradeBreak { Timestamp: " << timestamp
    //             << ", ExecutionId: " << executionId << " }";
    //     return ss.str();
    // }
} // namespace CboePitch
