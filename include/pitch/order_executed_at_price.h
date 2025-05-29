//
// Created by phamanhtan on 29/5/25.
//

#ifndef ORDER_EXECUTED_AT_PRICE_H
#define ORDER_EXECUTED_AT_PRICE_H
#pragma once
#include "trade_base.h"
#include <string>
#include <memory>
#include <stdexcept>
#include <cstring>

namespace CboePitch {
    class OrderExecutedAtPrice : public TradeBase {
    private:
        uint64_t orderId;
        uint64_t contraOrderId;
        std::string contraPid;
        char executionType;

    public:
        OrderExecutedAtPrice(uint64_t ts, const std::string &sym, uint32_t qty, uint64_t prc, uint64_t eid,
                             uint64_t oid, uint64_t coid, const std::string &cpid, char etype)
            : TradeBase(ts, sym, qty, prc, eid), orderId(oid), contraOrderId(coid), contraPid(cpid),
              executionType(etype) {
            validate();
        }

        static std::unique_ptr<OrderExecutedAtPrice> parse(const uint8_t *data, size_t size) {
            if (size < 52) throw std::invalid_argument("Invalid OrderExecutedAtPrice length");
            if (data[1] != 0x58) throw std::invalid_argument("Invalid OrderExecutedAtPrice type");
            uint64_t ts, oid, eid, coid, prc;
            std::memcpy(&ts, data + 2, 8);
            std::memcpy(&oid, data + 10, 8);
            uint32_t qty;
            std::memcpy(&qty, data + 18, 4);
            std::memcpy(&eid, data + 22, 8);
            std::memcpy(&coid, data + 30, 8);
            std::string cpid(reinterpret_cast<const char *>(data + 38), 4);
            char etype = data[42];
            std::memcpy(&prc, data + 43, 8);
            std::string sym(6, ' ');
            return std::make_unique<OrderExecutedAtPrice>(ts, sym, qty, prc, eid, oid, coid, cpid, etype);
        }

        char getMessageType() const override { return 0x58; }

        std::string toString() const override {
            char buffer[150];
            snprintf(buffer, sizeof(buffer),
                     "OrderExecutedAtPrice: Timestamp=%llu, Symbol=%s, Quantity=%u, Price=%llu, ExecutionId=%llu, "
                     "OrderId=%llu, ContraOrderId=%llu, ContraPID=%s, ExecutionType=%c",
                     timestamp, symbol.c_str(), quantity, price, executionId, orderId, contraOrderId, contraPid.c_str(),
                     executionType);
            return std::string(buffer);
        }

    private:
        void validate() {
            TradeBase::validate();
            if (contraPid.size() > 4) throw std::invalid_argument("ContraPID must be <= 4 characters");
            if (executionType != 'O' && executionType != 'C' && executionType != 'H')
                throw std::invalid_argument("Invalid ExecutionType");
        }
    };
} // namespace CboePitch
#endif //ORDER_EXECUTED_AT_PRICE_H
