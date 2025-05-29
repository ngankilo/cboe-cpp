#pragma once
#include "order_base.h"
#include <string>
#include <memory>
#include <stdexcept>

namespace CboePitch {

    class Modify_Order : public OrderBase {
    private:
        uint32_t shares;

    public:
        Modify_Order(uint32_t ts, const std::string& oid, uint32_t sh, const std::string& sym = "")
            : OrderBase(ts, oid, sym), shares(sh) {
        }

        static std::unique_ptr<ModifyOrder> parse(const std::string& line) {
            if (line.size() < 26) {
                throw std::invalid_argument("Invalid ModifyOrder length");
            }
            try {
                uint32_t ts = std::stoul(line.substr(0, 8));
                std::string oid = line.substr(9, 12);
                uint32_t sh = std::stoul(line.substr(21, 6));
                return std::make_unique<ModifyOrder>(ts, oid, sh);
            } catch (const std::exception& e) {
                throw std::invalid_argument("ModifyOrder parse error: " + std::string(e.what()));
            }
        }

        char getMessageType() const override { return 'M'; }

        std::string toString() const override {
            char buffer[100];
            snprintf(buffer, sizeof(buffer),
                     "ModifyOrder: Timestamp=%u, OrderID=%s, Shares=%u, Symbol=%s",
                     timestamp, orderId.c_str(), shares, symbol.c_str());
            return std::string(buffer);
        }
    };

} // namespace CboePitch