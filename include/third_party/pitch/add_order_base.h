//
// Created by phamanhtan on 29/5/25.
//
#pragma once

#ifndef ADD_ORDER_BASE_H
#define ADD_ORDER_BASE_H

#include "message.h"
#include <string>
#include <stdexcept>

namespace CboePitch {
    class AddOrderBase : public Message {
    protected:
        uint32_t timestamp;
        std::string orderId;
        char sideIndicator;
        uint32_t shares;
        std::string symbol;
        char display;

        AddOrderBase(uint32_t ts, const std::string &oid, char side, uint32_t sh,
                     const std::string &sym, char disp)
            : timestamp(ts), orderId(oid), sideIndicator(side), shares(sh),
              symbol(sym), display(disp) {
            validate();
        }

    public:
        std::string getSymbol() const override { return symbol; }

        void setSymbol(const std::string &sym) override {
            if (sym.size() > 8) throw std::invalid_argument("Symbol must be <= 8 characters");
            symbol = sym;
        }

    protected:
        void validate() {
            if (orderId.size() != 12) throw std::invalid_argument("Order ID must be 12 characters");
            if (symbol.size() > 8) throw std::invalid_argument("Symbol must be <= 8 characters");
            if (sideIndicator != 'B' && sideIndicator != 'S') throw std::invalid_argument("Side must be 'B' or 'S'");
            if (display != 'Y' && display != 'N') throw std::invalid_argument("Display must be 'Y' or 'N'");
        }
    };
} // namespace CboePitch
#endif //ADD_ORDER_BASE_H
