//
// Created by phamanhtan on 29/5/25.
//

#ifndef DELETE_ORDER_H
#define DELETE_ORDER_H
#pragma once
#include "order_base.h"
#include <string>
#include <memory>
#include <stdexcept>
#include <cstring>

namespace CboePitch {

    class DeleteOrder : public OrderBase {
    public:
        DeleteOrder(uint64_t ts, uint64_t oid, const std::string& sym)
            : OrderBase(ts, oid, sym) {
            validate();
        }

        static std::unique_ptr<DeleteOrder> parse(const uint8_t* data, size_t size) {
            if (size < 18) throw std::invalid_argument("Invalid DeleteOrder length");
            if (data[1] != 0x3C) throw std::invalid_argument("Invalid DeleteOrder type");
            uint64_t ts, oid;
            std::memcpy(&ts, data + 2, 8);
            std::memcpy(&oid, data + 10, 8);
            std::string sym(6, ' ');
            return std::make_unique<DeleteOrder>(ts, oid, sym);
        }

        char getMessageType() const override { return 0x3C; }

        std::string toString() const override {
            char buffer[100];
            snprintf(buffer, sizeof(buffer),
                     "DeleteOrder: Timestamp=%llu, OrderId=%llu, Symbol=%s",
                     timestamp, orderId, symbol.c_str());
            return std::string(buffer);
        }

    private:
        void validate() {
            OrderBase::validate();
        }
    };

} // namespace CboePitch
#endif
