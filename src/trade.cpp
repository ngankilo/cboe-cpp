//
// Created by phamanhtan on 30/5/25.
//

#include "pitch/trade.h"
#include <sstream>

namespace CboePitch {
    Trade Trade::parse(const uint8_t *data, size_t length) {
        if (length < 64) throw std::runtime_error("Trade message too short");
        uint64_t timestamp = *reinterpret_cast<const uint64_t *>(&data[0]); // Little Endian
        std::string symbol(reinterpret_cast<const char *>(&data[8]), 6);
        uint32_t quantity = *reinterpret_cast<const uint32_t *>(&data[14]); // Little Endian
        int64_t rawPrice = *reinterpret_cast<const int64_t *>(&data[18]); // Little Endian
        double price = rawPrice / 10000000.0; // 7 decimal places
        uint64_t executionId = *reinterpret_cast<const uint64_t *>(&data[26]); // Little Endian
        uint64_t orderId = *reinterpret_cast<const uint64_t *>(&data[34]); // Little Endian
        uint64_t contraOrderId = *reinterpret_cast<const uint64_t *>(&data[42]); // Little Endian
        std::string pid(reinterpret_cast<const char *>(&data[50]), 4);
        std::string contraPid(reinterpret_cast<const char *>(&data[54]), 4);
        char tradeType = static_cast<char>(data[58]);
        char tradeDesignation = static_cast<char>(data[59]);
        char tradeReportType = static_cast<char>(data[60]);
        uint64_t tradeTransactionTime = *reinterpret_cast<const uint64_t *>(&data[61]); // Little Endian
        return Trade(timestamp, symbol, quantity, price, executionId, orderId, contraOrderId, pid, contraPid,
                     tradeType, tradeDesignation, tradeReportType, tradeTransactionTime);
    }

    std::string Trade::toString() const {
        std::stringstream ss;
        ss << "Trade { Timestamp: " << timestamp
                << ", Symbol: " << symbol
                << ", Quantity: " << quantity
                << ", Price: " << price
                << ", ExecutionId: " << executionId
                << ", OrderId: " << orderId
                << ", ContraOrderId: " << contraOrderId
                << ", PID: " << pid
                << ", ContraPID: " << contraPid
                << ", TradeType: " << tradeType
                << ", TradeDesignation: " << tradeDesignation
                << ", TradeReportType: " << tradeReportType
                << ", TradeTransactionTime: " << tradeTransactionTime << " }";
        return ss.str();
    }
} // namespace CboePitch
