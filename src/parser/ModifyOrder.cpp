//
// Created by phamanhtan on 30/5/25.
//

#include "pitch/modify_order.h"
#include <sstream>

namespace CboePitch {
    ModifyOrder ModifyOrder::parse(const uint8_t *data, size_t length) {
        if (length < 28) throw std::runtime_error("ModifyOrder message too short");
        uint64_t timestamp = *reinterpret_cast<const uint64_t *>(&data[0]); // Little Endian
        uint64_t orderId = *reinterpret_cast<const uint64_t *>(&data[8]); // Little Endian
        uint32_t quantity = *reinterpret_cast<const uint32_t *>(&data[16]); // Little Endian
        int64_t rawPrice = *reinterpret_cast<const int64_t *>(&data[20]); // Little Endian
        double price = rawPrice / 10000000.0; // 7 decimal places
        return ModifyOrder(timestamp, orderId, quantity, price);
    }

    // std::string ModifyOrder::toString() const {
    //     std::stringstream ss;
    //     ss << "ModifyOrder { Timestamp: " << timestamp
    //             << ", OrderId: " << orderId
    //             << ", Quantity: " << quantity
    //             << ", Price: " << price << " }";
    //     return ss.str();
    // }
} // namespace CboePitch
