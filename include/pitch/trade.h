//
// Created by phamanhtan on 29/5/25.
//

#ifndef TRADE_H
#define TRADE_H
#pragma once
#include "trade_base.h"
#include <string>
#include <memory>
#include <stdexcept>
#include <cstring>

namespace CboePitch {
    class Trade : public TradeBase {
    private:
        uint64_t orderId;
        uint64_t contraOrderId;
        std::string pid;
        std::string contraPid;
        char tradeType;
        char tradeDesignation;
        char tradeReportType;
        uint64_t tradeTransactionTime;
        uint8_t flags;

    public:
        Trade(uint64_t ts, const std::string &sym, uint32_t qty, uint64_t prc, uint64_t eid,
              uint64_t oid, uint64_t coid, const std::string &p, const std::string &cp,
              char tt, char td, char trt, uint64_t ttt, uint8_t f)
            : TradeBase(ts, sym, qty, prc, eid), orderId(oid), contraOrderId(coid),
              pid(p), contraPid(cp), tradeType(tt), tradeDesignation(td),
              tradeReportType(trt), tradeTransactionTime(ttt), flags(f) {
            validate();
        }

        static std::unique_ptr<Trade> parse(const uint8_t *data, size_t size) {
            if (size < 72) throw std::invalid_argument("Invalid Trade length");
            if (data[1] != 0x3D) throw std::invalid_argument("Invalid Trade type");
            uint64_t ts, prc, eid, oid, coid, ttt;
            std::memcpy(&ts, data + 2, 8);
            std::string sym(reinterpret_cast<const char *>(data + 10), 6);
            uint32_t qty;
            std::memcpy(&qty, data + 16, 4);
            std::memcpy(&prc, data + 20, 8);
            std::memcpy(&eid, data + 28, 8);
            std::memcpy(&oid, data + 36, 8);
            std::memcpy(&coid, data + 44, 8);
            std::string pid(reinterpret_cast<const char *>(data + 52), 4);
            std::string cpid(reinterpret_cast<const char *>(data + 56), 4);
            char tt = data[60];
            char td = data[61];
            char trt = data[62];
            std::memcpy(&ttt, data + 63, 8);
            uint8_t f = data[71];
            return std::make_unique<Trade>(ts, sym, qty, prc, eid, oid, coid, pid, cpid, tt, td, trt, ttt, f);
        }

        char getMessageType() const override { return 0x3D; }

        std::string toString() const override {
            char buffer[200];
            snprintf(buffer, sizeof(buffer),
                     "Trade: Timestamp=%llu, Symbol=%s, Quantity=%u, Price=%llu, ExecutionId=%llu, "
                     "OrderId=%llu, ContraOrderId=%llu, PID=%s, ContraPID=%s, TradeType=%c, "
                     "TradeDesignation=%c, TradeReportType=%c, TradeTransactionTime=%llu, Flags=%u",
                     timestamp, symbol.c_str(), quantity, price, executionId, orderId, contraOrderId,
                     pid.c_str(), contraPid.c_str(), tradeType, tradeDesignation, tradeReportType,
                     tradeTransactionTime, flags);
            return std::string(buffer);
        }

    private:
        void validate() {
            TradeBase::validate();
            if (pid.size() > 4 || contraPid.size() > 4) throw std::invalid_argument(
                "PID/ContraPID must be <= 4 characters");
        }
    };
} // namespace CboePitch
#endif //TRADE_H
