//
// Created by phamanhtan on 29/5/25.
//

#ifndef ORDER_EXECUTED_H
#define ORDER_EXECUTED_H

#pragma once
#include "order_base.h"
#include <string>
#include <memory>
#include <stdexcept>

namespace CboePitch {
    class OrderExecuted : public OrderBase {
    private:
        uint32_t executedShares;
        std::string executionId;

    public:
        OrderExecuted(uint32_t ts, const std::string &oid, uint32_t es, const std::string &eid,
                      const std::string &sym = "")
            : OrderBase(ts, oid, sym), executedShares(es), executionId(eid) {
            validate();
        }

        static std::unique_ptr<OrderExecuted> parse(const std::string &line) {
            if (line.size() < 29) {
                throw std::invalid_argument("Invalid OrderExecuted length");
            }
            try {
                uint32_t ts = std::stoul(line.substr(0, 8));
                std::string oid = line.substr(9, 12);
                uint32_t es = std::stoul(line.substr(21, 6));
                std::string eid = line.substr(27, 12);
                return std::make_unique<OrderExecuted>(ts, oid, es, eid);
            } catch (const std::exception &e) {
                throw std::invalid_argument("OrderExecuted parse error: " + std::string(e.what()));
            }
        }

        char getMessageType() const override { return 'E'; }

        std::string toString() const override {
            char buffer[100];
            snprintf(buffer, sizeof(buffer),
                     "OrderExecuted: Timestamp=%u, OrderID=%s, ExecutedShares=%u, ExecutionID=%s, Symbol=%s",
                     timestamp, orderId.c_str(), executedShares, executionId.c_str(), symbol.c_str());
            return std::string(buffer);
        }

    private:
        void validate() {
            OrderBase::validate();
            if (executionId.size() != 12) throw std::invalid_argument("Execution ID must be 12 characters");
        }
    };
} // namespace CboePitch
#endif
