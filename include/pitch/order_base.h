//
// Created by phamanhtan on 29/5/25.
//
#pragma once

#ifndef ORDER_BASE_H
#define ORDER_BASE_H
#include "message.h"
#include <string>
#include <stdexcept>

namespace CboePitch {
    class OrderBase : public Message {
    protected:
        uint64_t timestamp;
        uint64_t orderId;
        std::string symbol;

    public:
        OrderBase(uint64_t ts, uint64_t oid, const std::string &sym)
            : timestamp(ts), orderId(oid), symbol(sym) {
            validate();
        }

        const std::string &getSymbol() const override { return symbol; }

        void setSymbol(const std::string &sym) override {
            if (sym.size() > 6) throw std::invalid_argument("Symbol must be <= 6 characters");
            symbol = sym;
        }

    protected:
        void validate() {
            if (symbol.size() > 6) throw std::invalid_argument("Symbol must be <= 6 characters");
        }
    };
} // namespace CboePitch
#endif
