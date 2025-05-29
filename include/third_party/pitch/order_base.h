//
// Created by phamanhtan on 29/5/25.
//

#ifndef ORDER_BASE_H
#define ORDER_BASE_H
#pragma once
#include "message.h"
#include <string>
#include <stdexcept>

namespace CboePitch {

    class OrderBase : public Message {
    protected:
        uint32_t timestamp;
        std::string orderId;
        std::string symbol;

        OrderBase(uint32_t ts, const std::string& oid, const std::string& sym = "")
            : timestamp(ts), orderId(oid), symbol(sym) {
            validate();
        }

    public:
        std::string getSymbol() const override { return symbol; }
        void setSymbol(const std::string& sym) override {
            if (sym.size() > 8) throw std::invalid_argument("Symbol must be <= 8 characters");
            symbol = sym;
        }

    protected:
        void validate() {
            if (orderId.size() != 12) throw std::invalid_argument("Order ID must be 12 characters");
            if (symbol.size() > 8) throw std::invalid_argument("Symbol must be <= 8 characters");
        }
    };

} // namespace CboePitch
#endif //ORDER_BASE_H
