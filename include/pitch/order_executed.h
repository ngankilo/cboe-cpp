//
// Created by phamanhtan on 29/5/25.
//
#pragma once

#ifndef ORDER_EXECUTED_H
#define ORDER_EXECUTED_H
#include <string>
#include <memory>
#include <stdexcept>
#include <cstring>
#pragma once
#include "trade_base.h"

namespace CboePitch {
    class OrderExecuted : public TradeBase {
    private:
        uint64_t orderId;
        uint64_t contraOrderId;
        std::string contraPid;

    public:
        OrderExecuted(uint64_t ts, const std::string &sym, uint32_t qty, uint64_t prc, uint64_t eid,
                      uint64_t oid, uint64_t coid, const std::string &cpid)
            : TradeBase(ts, sym, qty, prc, eid), orderId(oid), contraOrderId(coid), contraPid(cpid) {
            validate();
        }

        static std::unique_ptr<OrderExecuted> parse(const uint8_t *data, size_t size) {
            if (size < 43) throw std::invalid_argument("Invalid OrderExecuted length");
            if (data[1] != 0x38) throw std::invalid_argument("Invalid OrderExecuted type");
            uint64_t ts, oid, eid, coid, prc = 0;
            std::memcpy(&ts, data + 2, 8);
            std::memcpy(&oid, data + 10, 8);
            uint32_t qty;
            std::memcpy(&qty, data + 18, 4);
            std::memcpy(&eid, data + 22, 8);
            std::memcpy(&coid, data + 30, 8);
            std::string cpid(reinterpret_cast<const char *>(data + 38), 4);
            std::string sym(6, ' ');
            return std::make_unique<OrderExecuted>(ts, sym, qty, prc, eid, oid, coid, cpid);
        }

        char getMessageType() const override { return 0x38; }

        std::string toString() const override {
            char buffer[150];
            snprintf(buffer, sizeof(buffer),
                     "OrderExecuted: Timestamp=%llu, Symbol=%s, Quantity=%u, Price=%llu, ExecutionId=%llu, "
                     "OrderId=%llu, ContraOrderId=%llu, ContraPID=%s",
                     timestamp, symbol.c_str(), quantity, price, executionId, orderId, contraOrderId,
                     contraPid.c_str());
            return std::string(buffer);
        }

    private:
        void validate() {
            TradeBase::validate();
            if (contraPid.size() > 4) throw std::invalid_argument("ContraPID must be <= 4 characters");
        }
    };
} // namespace CboePitch
#endif
