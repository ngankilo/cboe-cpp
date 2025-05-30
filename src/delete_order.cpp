//
// Created by phamanhtan on 30/5/25.
//

#include "pitch/delete_order.h"
#include <sstream>

namespace CboePitch {
    DeleteOrder DeleteOrder::parse(const uint8_t *data, size_t length) {
        if (length < 16) throw std::runtime_error("DeleteOrder message too short");
        uint64_t timestamp = *reinterpret_cast<const uint64_t *>(&data[0]); // Little Endian
        uint64_t orderId = *reinterpret_cast<const uint64_t *>(&data[8]); // Little Endian
        return DeleteOrder(timestamp, orderId);
    }

    std::string DeleteOrder::toString() const {
        std::stringstream ss;
        ss << "DeleteOrder { Timestamp: " << timestamp
                << ", OrderId: " << orderId << " }";
        return ss.str();
    }
} // namespace CboePitch
