//
// Created by phamanhtan on 30/5/25.
//

#ifndef TRADE_BREAK_H
#define TRADE_BREAK_H

#include "trade_base.h"

namespace CboePitch {
    class TradeBreak : public TradeBase {
    public:
        static TradeBreak parse(const uint8_t *data, size_t length);

        std::string toString() const override;

    private:
        TradeBreak(uint64_t ts, uint64_t execId)
            : TradeBase(0x3E, ts, execId) {
        }
    };
} // namespace CboePitch

#endif // TRADE_BREAK_H
