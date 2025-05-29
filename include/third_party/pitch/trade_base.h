//
// Created by phamanhtan on 29/5/25.
//
#pragma once

#ifndef TRADEBASE_H_
#define TRADEBASE_H_

#include "message.h"
#include <string>
#include <stdexcept>

namespace CboePitch {
    class TradeBase : public Message {
    protected:
        uint32_t timestamp;
        uint32_t shares;
        std::string symbol;

        TradeBase(uint32_t ts, uint32_t sh, const std::string &sym)
            : timestamp(ts), shares(sh), symbol(sym) {
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
            if (symbol.size() > 8) throw std::invalid_argument("Symbol must be <= 8 characters");
        }
    };
} // namespace CboePitch
#endif
