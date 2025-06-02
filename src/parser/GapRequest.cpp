//
// Created by phamanhtan on 30/5/25.
//

#include "pitch/gap_request.h"
#include <sstream>

namespace CboePitch {
    // GapRequest GapRequest::parse(const uint8_t *data, size_t length) {
    //     if (length < 7) throw std::runtime_error("GapRequest message too short");
    //     uint8_t unit = data[0];
    //     uint32_t sequence = *reinterpret_cast<const uint32_t *>(&data[1]); // Little Endian
    //     uint16_t count = *reinterpret_cast<const uint16_t *>(&data[5]); // Little Endian
    //     return GapRequest(unit, sequence, count);
    // }
    //
    // std::string GapRequest::toString() const {
    //     std::stringstream ss;
    //     ss << "GapRequest { Unit: " << static_cast<int>(unit)
    //             << ", Sequence: " << sequence
    //             << ", Count: " << count << " }";
    //     return ss.str();
    // }
} // namespace CboePitch
