//
// Created by phamanhtan on 30/5/25.
//

#ifndef DELETE_ORDER_H
#define DELETE_ORDER_H

#include "message.h"

namespace CboePitch {
    class DeleteOrder final : public Message {
    public:
        static DeleteOrder parse(const uint8_t *data, size_t length);

        std::string toString() const override;

        uint64_t getTimestamp() const { return timestamp; }
        uint64_t getOrderId() const { return orderId; }

    private:
        uint64_t timestamp;
        uint64_t orderId;

        DeleteOrder(uint64_t ts, uint64_t ordId)
            : Message(0x3C), timestamp(ts), orderId(ordId) {
        }
    };
} // namespace CboePitch

#endif // DELETE_ORDER_H
