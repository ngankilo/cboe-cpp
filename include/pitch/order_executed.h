//
// Created by phamanhtan on 30/5/25.
//

#ifndef ORDER_EXECUTED_H
#define ORDER_EXECUTED_H

#include "message.h"

namespace CboePitch {
    class OrderExecuted : public Message {
    public:
        static OrderExecuted parse(const uint8_t *data, size_t length);

        std::string toString() const override;

        uint64_t getTimestamp() const { return timestamp; }
        uint64_t getOrderId() const { return orderId; }
        uint32_t getExecutedQuantity() const { return executedQuantity; }
        uint64_t getExecutionId() const { return executionId; }
        uint64_t getContraOrderId() const { return contraOrderId; }
        std::string getContraPID() const { return contraPid; }

    private:
        uint64_t timestamp;
        uint64_t orderId;
        uint32_t executedQuantity;
        uint64_t executionId;
        uint64_t contraOrderId;
        std::string contraPid;

        OrderExecuted(uint64_t ts, uint64_t ordId, uint32_t qty, uint64_t execId,
                      uint64_t contraId, const std::string &contraP)
            : Message(0x38), timestamp(ts), orderId(ordId), executedQuantity(qty),
              executionId(execId), contraOrderId(contraId), contraPid(contraP) {
        }
    };
} // namespace CboePitch

#endif // ORDER_EXECUTED_H
