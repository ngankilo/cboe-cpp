#ifndef SEQ_UNIT_HEADER_H
#define SEQ_UNIT_HEADER_H

#include <string>
#include <stdexcept>

namespace CboePitch {
    class SeqUnitHeader {
    public:
        static SeqUnitHeader parse(const uint8_t* data, size_t length) {
            if (length < 8) {
                throw std::invalid_argument("Data too short for SeqUnitHeader");
            }
            uint16_t len = (data[0] | (data[1] << 8)); // Little-endian
            uint8_t cnt = data[2];
            uint8_t unit = data[3];
            uint32_t seq = (data[4] | (data[5] << 8) | (data[6] << 16) | (data[7] << 24)); // Little-endian

            return SeqUnitHeader(len, cnt, unit, seq);
        }

        std::string toString() const {
            return "SeqUnitHeader{length=" + std::to_string(length) +
                   ", count=" + std::to_string(count) +
                   ", unit=" + std::to_string(unit) +
                   ", sequence=" + std::to_string(sequence) + "}";
        }

        uint16_t getLength() const { return length; }
        uint8_t getCount() const { return count; }
        uint8_t getUnit() const { return unit; }
        uint32_t getSequence() const { return sequence; }

    private:
        uint16_t length;
        uint8_t count;
        uint8_t unit;
        uint32_t sequence;

        SeqUnitHeader(uint16_t len, uint8_t cnt, uint8_t u, uint32_t seq)
            : length(len), count(cnt), unit(u), sequence(seq) {}
    };
} // namespace CboePitch

#endif // SEQ_UNIT_HEADER_H