//
// Created by phamanhtan on 30/5/25.
//

#ifndef GAP_REQUEST_H
#define GAP_REQUEST_H

#include "message.h"

namespace CboePitch {
    class GapRequest final : public Message {
    public:
        static GapRequest parse(const uint8_t *data, size_t length);

        std::string toString() const override;

        uint8_t getUnit() const { return unit; }
        uint32_t getSequence() const { return sequence; }
        uint16_t getCount() const { return count; }

    private:
        uint8_t unit;
        uint32_t sequence;
        uint16_t count;

        GapRequest(uint8_t u, uint32_t seq, uint16_t cnt)
            : Message(0x03), unit(u), sequence(seq), count(cnt) {
        }
    };
} // namespace CboePitch

#endif // GAP_REQUEST_H
