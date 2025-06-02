//
// Created by phamanhtan on 30/5/25.
//

#include "pitch/order_executed.h"
#include <sstream>

namespace CboePitch {
    // OrderExecuted OrderExecuted::parse(const uint8_t *data, size_t length) {
    //     if (length < 35) throw std::runtime_error("OrderExecuted message too short");
    //     uint64_t timestamp = *reinterpret_cast<const uint64_t *>(&data[0]); // Little Endian
    //     uint64_t orderId = *reinterpret_cast<const uint64_t *>(&data[8]); // Little Endian
    //     uint32_t executedQuantity = *reinterpret_cast<const uint32_t *>(&data[16]); // Little Endian
    //     uint64_t executionId = *reinterpret_cast<const uint64_t *>(&data[20]); // Little Endian
    //     uint64_t contraOrderId = *reinterpret_cast<const uint64_t *>(&data[28]); // Little Endian
    //     std::string contraPid(reinterpret_cast<const char *>(&data[36]), 4);
    //     return OrderExecuted(timestamp, orderId, executedQuantity, executionId, contraOrderId, contraPid);
    // }

    // std::string OrderExecuted::toString() const {
    //     std::stringstream ss;
    //     ss << "OrderExecuted { Timestamp: " << timestamp
    //             << ", OrderId: " << orderId
    //             << ", ExecutedQuantity: " << executedQuantity
    //             << ", ExecutionId: " << executionId
    //             << ", ContraOrderId: " << contraOrderId
    //             << ", ContraPID: " << contraPid << " }";
    //     return ss.str();
    // }
} // namespace CboePitch
