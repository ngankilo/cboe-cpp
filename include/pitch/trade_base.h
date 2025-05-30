//
// Created by phamanhtan on 30/5/25.
//

#ifndef TRADE_BASE_H
#define TRADE_BASE_H

#include "message.h"

namespace CboePitch {
    class TradeBase : public Message {
    public:
        virtual ~TradeBase() = default;

        virtual std::string toString() const = 0;

        uint64_t getTimestamp() const { return timestamp; }
        uint64_t getExecutionId() const { return executionId; }

    protected:
        uint64_t timestamp;
        uint64_t executionId;

        TradeBase(uint8_t type, uint64_t ts, uint64_t execId)
            : Message(type), timestamp(ts), executionId(execId) {
        }
    };
} // namespace CboePitch

#endif // TRADE_BASE_H
