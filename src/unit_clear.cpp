//
// Created by phamanhtan on 30/5/25.
//

#include "pitch/unit_clear.h"
#include <sstream>

namespace CboePitch {
    UnitClear UnitClear::parse(const uint8_t *data, size_t length) {
        if (length < 1) throw std::runtime_error("UnitClear message too short");
        return UnitClear();
    }

    std::string UnitClear::toString() const {
        return "UnitClear {}";
    }
} // namespace CboePitch
