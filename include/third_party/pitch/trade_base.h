//
// Created by phamanhtan on 29/5/25.
//
#pragma once
#include "message.h"
#include <string>
#include <cstdint>
#include <stdexcept>

namespace CboePitch {

    class TradeBase : public Message {
    protected:
        uint64_t timestamp;
        std::string symbol;
        uint32_t quantity;
        uint64_t price;
        uint64_t executionId;

    public:
        TradeBase(uint64_t ts, const std::string& sym, uint32_t qty, uint64_t prc, uint64_t eid)
            : timestamp(ts), symbol(sym), quantity(qty), price(prc), executionId(eid) {
            validate();
        }

        std::string getSymbol() const override { return symbol; }
        void setSymbol(const std::string& sym) override {
            if (sym.size() > 6) throw std::invalid_argument("Symbol must be <= 6 characters");
            symbol = sym;
        }

    protected:
        void validate() {
            if (symbol.size() > 6) throw std::invalid_argument("Symbol must be <= 6 characters");
        }
    };

} // namespace CboePitch
#endif //TRADE_BASE