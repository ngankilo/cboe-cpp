//
// Created by phamanhtan on 30/5/25.
//

#include "pitch/reduce_size.h"
#include <sstream>

namespace CboePitch {
    ReduceSize ReduceSize::parse(const uint8_t *data, size_t length) {
        if (length < 20) throw std::runtime_error("ReduceSize message too short");
        uint64_t timestamp = *reinterpret_cast<const uint64_t *>(&data[0]); // Little Endian
        uint64_t orderId = *reinterpret_cast<const uint64_t *>(&data[8]); // Little Endian
        uint32_t cancelledQuantity = *reinterpret_cast<const uint32_t *>(&data[16]); // Little Endian
        return ReduceSize(timestamp, orderId, cancelledQuantity);
    }

    std::string ReduceSize::toString() const {
        std::stringstream ss;
        ss << "ReduceSize { Timestamp: " << timestamp
                << ", OrderId: " << orderId
                << ", CancelledQuantity: " << cancelledQuantity << " }";
        return ss.str();
    }
} // namespace CboePitch
