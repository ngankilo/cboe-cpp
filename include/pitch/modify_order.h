//
// Created by phamanhtan on 30/5/25.
//

#ifndef MODIFY_ORDER_H
#define MODIFY_ORDER_H

#include "message.h"

namespace CboePitch {
    class ModifyOrder : public Message {
    public:
        static ModifyOrder parse(const uint8_t *data, size_t length);

        std::string toString() const override;

        uint64_t getTimestamp() const { return timestamp; }
        uint64_t getOrderId() const { return orderId; }
        uint32_t getQuantity() const { return quantity; }
        double getPrice() const { return price; }

    private:
        uint64_t timestamp;
        uint64_t orderId;
        uint32_t quantity;
        double price;

        ModifyOrder(uint64_t ts, uint64_t ordId, uint32_t qty, double prc)
            : Message(0x3A), timestamp(ts), orderId(ordId), quantity(qty), price(prc) {
        }
    };
} // namespace CboePitch

#endif // MODIFY_ORDER_H
