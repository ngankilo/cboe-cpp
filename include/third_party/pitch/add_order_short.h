//
// Created by phamanhtan on 29/5/25.
//

#ifndef ADD_ORDER_SHORT_H
#define ADD_ORDER_SHORT_H

#pragma once
#include "add_order_base.h"
#include <string>
#include <memory>
#include <stdexcept>

namespace CboePitch {
    class AddOrderShort : public AddOrderBase {
    private:
        uint32_t price;

    public:
        AddOrderShort(uint32_t ts, const std::string &oid, char side, uint32_t sh,
                      const std::string &sym, uint32_t pr, char disp)
            : AddOrderBase(ts, oid, side, sh, sym, disp), price(pr) {
        }

        static std::unique_ptr<AddOrderShort> parse(const std::string &line) {
            if (line.size() < 27) {
                throw std::invalid_argument("Invalid AddOrderShort length");
            }
            try {
                uint32_t ts = std::stoul(line.substr(0, 8));
                std::string oid = line.substr(9, 12);
                char side = line[21];
                uint32_t sh = std::stoul(line.substr(22, 6));
                std::string sym = line.substr(28, 6);
                uint32_t pr = std::stoul(line.substr(34, 10));
                char disp = line[44];
                return std::make_unique<AddOrderShort>(ts, oid, side, sh, sym, pr, disp);
            } catch (const std::exception &e) {
                throw std::invalid_argument("AddOrderShort parse error: " + std::string(e.what()));
            }
        }

        char getMessageType() const override { return 'A'; }

        std::string toString() const override {
            char buffer[100];
            snprintf(buffer, sizeof(buffer),
                     "AddOrderShort: Timestamp=%u, OrderID=%s, Side=%c, Shares=%u, "
                     "Symbol=%s, Price=%u, Display=%c",
                     timestamp, orderId.c_str(), sideIndicator, shares,
                     symbol.c_str(), price, display);
            return std::string(buffer);
        }
    };
} // namespace CboePitch
#endif
