#ifndef TRADE_BASE_H
#define TRADE_BASE_H

#include "message.h"
#include <string>

namespace CboePitch {
    class TradeBase : public Message {
    public:
        TradeBase(uint64_t ts, const std::string& execId)
            : timestamp(ts), executionId(execId) {}

    protected:
        uint64_t timestamp;
        std::string executionId;
    };
} // namespace CboePitch

#endif // TRADE_BASE_H