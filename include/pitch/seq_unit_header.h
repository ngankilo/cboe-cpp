//
// Created by phamanhtan on 30/5/25.
//

#ifndef SEQ_UNIT_HEADER_H
#define SEQ_UNIT_HEADER_H

#include <cstdint>
#include <string>
#include <memory>
#include <vector>
#include "message.h"

namespace CboePitch {
    class SeqUnitHeader {
    public:
        static SeqUnitHeader parse(const uint8_t *data, size_t length);

        std::string toString() const;

        uint16_t getLength() const { return length; }
        uint8_t getCount() const { return count; }
        uint8_t getUnit() const { return unit; }
        uint32_t getSequence() const { return sequence; }

        static std::vector<std::unique_ptr<Message> > decodeMessages(const uint8_t *data, size_t length);

    private:
        uint16_t length;
        uint8_t count;
        uint8_t unit;
        uint32_t sequence;

        SeqUnitHeader(uint16_t len, uint8_t cnt, uint8_t u, uint32_t seq)
            : length(len), count(cnt), unit(u), sequence(seq) {
        }
    };
} // namespace CboePitch

#endif // SEQ_UNIT_HEADER_H
