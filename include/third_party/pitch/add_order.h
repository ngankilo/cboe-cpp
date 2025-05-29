//
// Created by phamanhtan on 29/5/25.
//

#ifndef ADD_ORDER_H
#define ADD_ORDER_H
#pragma once
#include "order_base.h"
#include <string>
#include <memory>
#include <stdexcept>
#include <cstring>

namespace CboePitch {

    class AddOrder : public OrderBase {
    private:
        char side;
        uint32_t quantity;
        uint64_t price;
        std::string pid;

    public:
        AddOrder(uint64_t ts, uint64_t oid, char s, uint32_t qty, const std::string& sym, uint64_t prc, const std::string& p)
            : OrderBase(ts, oid, sym), side(s), quantity(qty), price(prc), pid(p) {
            validate();
        }

        static std::unique_ptr<AddOrder> parse(const uint8_t* data, size_t size) {
            if (size < 42) throw std::invalid_argument("Invalid AddOrder length");
            if (data[1] != 0x37) throw std::invalid_argument("Invalid AddOrder type");
            uint64_t ts, oid, prc;
            std::memcpy(&ts, data + 2, 8);
            std::memcpy(&oid, data + 10, 8);
            char side = data[18];
            uint32_t qty;
            std::memcpy(&qty, data + 19, 4);
            std::string sym(reinterpret_cast<const char*>(data + 23), 6);
            std::memcpy(&prc, data + 29, 8);
            std::string pid(reinterpret_cast<const char*>(data + 37), 4);
            return std::make_unique<AddOrder>(ts, oid, side, qty, sym, prc, pid);
        }

        char getMessageType() const override { return 0x37; }

        std::string toString() const override {
            char buffer[150];
            snprintf(buffer, sizeof(buffer),
                     "AddOrder: Timestamp=%llu, OrderId=%llu, Side=%c, Quantity=%u, Symbol=%s, Price=%llu, PID=%s",
                     timestamp, orderId, side, quantity, symbol.c_str(), price, pid.c_str());
            return std::string(buffer);
        }

    private:
        void validate() {
            OrderBase::validate();
            if (side != 'B' && side != 'S') throw std::invalid_argument("Side must be 'B' or 'S'");
            if (pid.size() > 4) throw std::invalid_argument("PID must be <= 4 characters");
        }
    };

} // namespace CboePitch
#endif //ADD_ORDER_H
