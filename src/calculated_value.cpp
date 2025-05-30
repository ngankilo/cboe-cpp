//
// Created by phamanhtan on 30/5/25.
//

#include "pitch/calculated_value.h"
#include <sstream>

namespace CboePitch {
    CalculatedValue CalculatedValue::parse(const uint8_t *data, size_t length) {
        if (length < 31) throw std::runtime_error("CalculatedValue message too short");
        uint64_t timestamp = *reinterpret_cast<const uint64_t *>(&data[0]); // Little Endian
        std::string symbol(reinterpret_cast<const char *>(&data[8]), 6);
        char valueCategory = static_cast<char>(data[14]);
        int64_t rawValue = *reinterpret_cast<const int64_t *>(&data[15]); // Little Endian
        double value = rawValue / 10000000.0; // 7 decimal places
        uint64_t valueTimestamp = *reinterpret_cast<const uint64_t *>(&data[23]); // Little Endian
        return CalculatedValue(timestamp, symbol, valueCategory, value, valueTimestamp);
    }

    std::string CalculatedValue::toString() const {
        std::stringstream ss;
        ss << "CalculatedValue { Timestamp: " << timestamp
                << ", Symbol: " << symbol
                << ", ValueCategory: " << valueCategory
                << ", Value: " << value
                << ", ValueTimestamp: " << valueTimestamp << " }";
        return ss.str();
    }
} // namespace CboePitch
