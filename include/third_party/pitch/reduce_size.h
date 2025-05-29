//
// Created by phamanhtan on 29/5/25.
//

#ifndef REDUCE_SIZE_H
#define REDUCE_SIZE_H
#pragma once
#include "order_base.h"
#include <string>
#include <memory>
#include <stdexcept>
#include <cstring>

namespace CboePitch {

    class ReduceSize : public OrderBase {
    private:
        uint32_t cancelledQuantity;

    public:
        ReduceSize(uint64_t ts, uint64_t oid, uint32_t cqty, const std::string& sym)
            : OrderBase(ts, oid, sym), cancelledQuantity(cqty) {
            validate();
        }

        static std::unique_ptr<ReduceSize> parse(const uint8_t* data, size_t size) {
            if (size < 22) throw std::invalid_argument("Invalid ReduceSize length");
            if (data[1] != 0x39) throw std::invalid_argument("Invalid ReduceSize type");
            uint64_t ts, oid;
            std::memcpy(&ts, data + 2, 8);
            std::memcpy(&oid, data + 10, 8);
            uint32_t cqty;
            std::memcpy(&cqty, data + 18, 4);
            std::string sym(6, ' ');
            return std::make_unique<ReduceSize>(ts, oid, cqty, sym);
        }

        char getMessageType() const override { return 0x39; }

        std::string toString() const override {
            char buffer[100];
            snprintf(buffer, sizeof(buffer),
                     "ReduceSize: Timestamp=%llu, OrderId=%llu, CancelledQuantity=%u, Symbol=%s",
                     timestamp, orderId, cancelledQuantity, symbol.c_str());
            return std::string(buffer);
        }

    private:
        void validate() {
            OrderBase::validate();
        }
    };

} // namespace CboePitch
#endif //REDUCE_SIZE_H
