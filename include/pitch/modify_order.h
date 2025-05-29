#ifndef _MODIFY_ORDER_H_
#define _MODIFY_ORDER_H_
#pragma once
#include "order_base.h"
#include <string>
#include <memory>
#include <stdexcept>
#include <cstring>

namespace CboePitch {
    class ModifyOrder : public OrderBase {
    private:
        uint32_t quantity;
        uint64_t price;

    public:
        ModifyOrder(uint64_t ts, uint64_t oid, uint32_t qty, uint64_t prc, const std::string &sym)
            : OrderBase(ts, oid, sym), quantity(qty), price(prc) {
            validate();
        }

        static std::unique_ptr<ModifyOrder> parse(const uint8_t *data, size_t size) {
            if (size < 31) throw std::invalid_argument("Invalid ModifyOrder length");
            if (data[1] != 0x3A) throw std::invalid_argument("Invalid ModifyOrder type");
            uint64_t ts, oid, prc;
            std::memcpy(&ts, data + 2, 8);
            std::memcpy(&oid, data + 10, 8);
            uint32_t qty;
            std::memcpy(&qty, data + 18, 4);
            std::memcpy(&prc, data + 22, 8);
            std::string sym(6, ' ');
            return std::make_unique<ModifyOrder>(ts, oid, qty, prc, sym);
        }

        char getMessageType() const override { return 0x3A; }

        std::string toString() const override {
            char buffer[100];
            snprintf(buffer, sizeof(buffer),
                     "ModifyOrder: Timestamp=%llu, OrderId=%llu, Quantity=%u, Price=%llu, Symbol=%s",
                     timestamp, orderId, quantity, price, symbol.c_str());
            return std::string(buffer);
        }

    private:
        void validate() {
            OrderBase::validate();
        }
    };
} // namespace CboePitch
#endif
