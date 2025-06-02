//
// Created by phamanhtan on 30/5/25.
//

#ifndef GAP_RESPONSE_H
#define GAP_RESPONSE_H

#include "message.h"

namespace CboePitch {
    class GapResponse final : public Message {
    public:
        static GapResponse parse(const uint8_t *data, size_t length);

        std::string toString() const override;

        uint8_t getUnit() const { return unit; }
        uint32_t getSequence() const { return sequence; }
        uint16_t getCount() const { return count; }
        char getStatus() const { return status; }

    private:
        uint8_t unit;
        uint32_t sequence;
        uint16_t count;
        char status;

        GapResponse(uint8_t u, uint32_t seq, uint16_t cnt, char stat)
            : unit(u), sequence(seq), count(cnt), status(stat) {
        }
    };
} // namespace CboePitch

#endif // GAP_RESPONSE_H
