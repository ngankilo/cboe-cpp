//
// Created by phamanhtan on 30/5/25.
//

#ifndef ORDER_EXECUTED_AT_PRICE_H
#define ORDER_EXECUTED_AT_PRICE_H

#include "message.h"

namespace CboePitch {
    class OrderExecutedAtPrice : public Message {
    public:
        static OrderExecutedAtPrice parse(const uint8_t *data, size_t length);

        std::string toString() const override;

        uint64_t getTimestamp() const { return timestamp; }
        uint64_t getOrderId() const { return orderId; }
        uint32_t getExecutedQuantity() const { return executedQuantity; }
        double getPrice() const { return price; }
        uint64_t getExecutionId() const { return executionId; }
        uint64_t getContraOrderId() const { return contraOrderId; }
        std::string getContraPID() const { return contraPid; }

    private:
        uint64_t timestamp;
        uint64_t orderId;
        uint32_t executedQuantity;
        double price;
        uint64_t executionId;
        uint64_t contraOrderId;
        std::string contraPid;

        OrderExecutedAtPrice(uint64_t ts, uint64_t ordId, uint32_t qty, double prc, uint64_t execId,
                             uint64_t contraId, const std::string &contraP)
            : Message(0x58), timestamp(ts), orderId(ordId), executedQuantity(qty), price(prc),
              executionId(execId), contraOrderId(contraId), contraPid(contraP) {
        }
    };
} // namespace CboePitch

#endif // ORDER_EXECUTED_AT_PRICE_H
