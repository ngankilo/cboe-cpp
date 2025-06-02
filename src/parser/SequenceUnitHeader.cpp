#include "pitch/seq_unit_header.h"
#include <sstream>
#include <stdexcept>

namespace CboePitch {
    // SeqUnitHeader SeqUnitHeader::parse(const uint8_t* data, size_t length) {
    //     if (length < 8) throw std::runtime_error("SeqUnitHeader too short");
    //     uint16_t len = *reinterpret_cast<const uint16_t*>(&data[0]); // Little Endian
    //     uint8_t count = data[2];
    //     uint8_t unit = data[3];
    //     uint32_t sequence = *reinterpret_cast<const uint32_t*>(&data[4]); // Little Endian
    //     return SeqUnitHeader(len, count, unit, sequence);
    // }
    //
    // std::string SeqUnitHeader::toString() const {
    //     std::stringstream ss;
    //     ss << "SeqUnitHeader { Length: " << length
    //        << ", Count: " << static_cast<int>(count)
    //        << ", Unit: " << static_cast<int>(unit)
    //        << ", Sequence: " << sequence << " }";
    //     return ss.str();
    // }
} // namespace CboePitch