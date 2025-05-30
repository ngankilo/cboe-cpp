//
// Created by phamanhtan on 30/5/25.
//

#include "pitch/gap_response.h"
#include <sstream>

namespace CboePitch {
    GapResponse GapResponse::parse(const uint8_t *data, size_t length) {
        if (length < 8) throw std::runtime_error("GapResponse message too short");
        uint8_t unit = data[0];
        uint32_t sequence = *reinterpret_cast<const uint32_t *>(&data[1]); // Little Endian
        uint16_t count = *reinterpret_cast<const uint16_t *>(&data[5]); // Little Endian
        char status = static_cast<char>(data[7]);
        return GapResponse(unit, sequence, count, status);
    }

    std::string GapResponse::toString() const {
        std::stringstream ss;
        ss << "GapResponse { Unit: " << static_cast<int>(unit)
                << ", Sequence: " << sequence
                << ", Count: " << count
                << ", Status: " << status << " }";
        return ss.str();
    }
} // namespace CboePitch
