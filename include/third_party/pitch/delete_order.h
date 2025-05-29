//
// Created by phamanhtan on 29/5/25.
//
#pragma once

#ifdef ADD_ORDER_SHORT_H
#define ADD_ORDER_SHORT_H

#include "order_base.h"
#include <string>
#include <memory>
#include <stdexcept>

namespace CboePitch {
    class DeleteOrder : public OrderBase {
    public:
        DeleteOrder(uint32_t ts, const std::string &oid, const std::string &sym = "")
            : OrderBase(ts, oid, sym) {
        }

        static std::unique_ptr<DeleteOrder> parse(const std::string &line) {
            if (line.size() < 20) {
                throw std::invalid_argument("Invalid DeleteOrder length");
            }
            try {
                uint32_t ts = std::stoul(line.substr(0, 8));
                std::string oid = line.substr(9, 12);
                return std::make_unique<DeleteOrder>(ts, oid);
            } catch (const std::exception &e) {
                throw std::invalid_argument("DeleteOrder parse error: " + std::string(e.what()));
            }
        }

        char getMessageType() const override { return 'C'; }

        std::string toString() const override {
            char buffer[100];
            snprintf(buffer, sizeof(buffer),
                     "DeleteOrder: Timestamp=%u, OrderID=%s, Symbol=%s",
                     timestamp, orderId.c_str(), symbol.c_str());
            return std::string(buffer);
        }
    };
} // namespace CboePitch
#endif
