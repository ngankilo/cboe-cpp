//
// Created by phamanhtan on 30/5/25.
//

#ifndef UNIT_CLEAR_H
#define UNIT_CLEAR_H

#include "message.h"

namespace CboePitch {
    class UnitClear : public Message {
    public:
        static UnitClear parse(const uint8_t *data, size_t length);

        std::string toString() const override;

    private:
        UnitClear() : Message(0x97) {
        }
    };
} // namespace CboePitch

#endif // UNIT_CLEAR_H
