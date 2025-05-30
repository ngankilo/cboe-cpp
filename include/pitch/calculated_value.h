//
// Created by phamanhtan on 30/5/25.
//

#ifndef CALCULATED_VALUE_H
#define CALCULATED_VALUE_H

#include "message.h"

namespace CboePitch {
    class CalculatedValue final : public Message {
    public:
        static CalculatedValue parse(const uint8_t *data, size_t length);

        std::string toString() const override;

        std::string getSymbol() const override { return symbol; }
        void setSymbol(const std::string &sym) override { symbol = sym; }

        uint64_t getTimestamp() const { return timestamp; }
        char getValueCategory() const { return valueCategory; }
        double getValue() const { return value; }
        uint64_t getValueTimestamp() const { return valueTimestamp; }

    private:
        uint64_t timestamp;
        std::string symbol;
        char valueCategory;
        double value;
        uint64_t valueTimestamp;

        CalculatedValue(uint64_t ts, const std::string &sym, char vc, double val, uint64_t vts)
            : Message(0xE3), timestamp(ts), symbol(sym), valueCategory(vc), value(val), valueTimestamp(vts) {
        }
    };
} // namespace CboePitch

#endif // CALCULATED_VALUE_H
