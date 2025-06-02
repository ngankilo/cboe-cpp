//
// Created by phamanhtan on 30/5/25.
//

#include "pitch/add_order.h"
#include <sstream>

namespace CboePitch {

    // AddOrder AddOrder::parse(const uint8_t* data, size_t length) {
    //     if (length < 34) throw std::runtime_error("AddOrder message too short");
    //     uint64_t timestamp = *reinterpret_cast<const uint64_t*>(&data[0]); // Little Endian
    //     uint64_t orderId = *reinterpret_cast<const uint64_t*>(&data[8]); // Little Endian
    //     char sideIndicator = static_cast<char>(data[16]);
    //     uint32_t quantity = *reinterpret_cast<const uint32_t*>(&data[17]); // Little Endian
    //     int64_t rawPrice = *reinterpret_cast<const int64_t*>(&data[21]); // Little Endian
    //     double price = rawPrice / 10000000.0; // 7 decimal places
    //     std::string symbol(reinterpret_cast<const char*>(&data[29]), 6);
    //     std::string pid(reinterpret_cast<const char*>(&data[35]), 4);
    //     return AddOrder(timestamp, orderId, sideIndicator, quantity, price, symbol, pid);
    // }
    //
    // std::string AddOrder::toString() const {
    //     std::stringstream ss;
    //     ss << "AddOrder { Timestamp: " << timestamp
    //        << ", OrderId: " << orderId
    //        << ", SideIndicator: " << sideIndicator
    //        << ", Quantity: " << quantity
    //        << ", Price: " << price
    //        << ", Symbol: " << symbol << " }";
    //     return ss.str();
    // }

} // namespace CboePitch