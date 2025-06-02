//
// Created by phamanhtan on 30/5/25.
//

#include "pitch/order_executed_at_price.h"
#include <sstream>

namespace CboePitch {

    OrderExecutedAtPrice OrderExecutedAtPrice::parse(const uint8_t* data, size_t length) {
        if (length < 43) throw std::runtime_error("OrderExecutedAtPrice message too short");
        uint64_t timestamp = *reinterpret_cast<const uint64_t*>(&data[0]); // Little Endian
        uint64_t orderId = *reinterpret_cast<const uint64_t*>(&data[8]); // Little Endian
        uint32_t executedQuantity = *reinterpret_cast<const uint32_t*>(&data[16]); // Little Endian
        int64_t rawPrice = *reinterpret_cast<const int64_t*>(&data[20]); // Little Endian
        double price = rawPrice / 10000000.0; // 7 decimal places
        uint64_t executionId = *reinterpret_cast<const uint64_t*>(&data[28]); // Little Endian
        uint64_t contraOrderId = *reinterpret_cast<const uint64_t*>(&data[36]); // Little Endian
        std::string contraPid(reinterpret_cast<const char*>(&data[44]), 4);
        return OrderExecutedAtPrice(timestamp, orderId, executedQuantity, price, executionId, contraOrderId, contraPid);
    }

    // std::string OrderExecutedAtPrice::toString() const {
    //     std::stringstream ss;
    //     ss << "OrderExecutedAtPrice { Timestamp: " << timestamp
    //        << ", OrderId: " << orderId
    //        << ", ExecutedQuantity: " << executedQuantity
    //        << ", Price: " << price
    //        << ", ExecutionId: " << executionId
    //        << ", ContraOrderId: " << contraOrderId
    //        << ", ContraPID: " << contraPid << " }";
    //     return ss.str();
    // }

} // namespace CboePitch