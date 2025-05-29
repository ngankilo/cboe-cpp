#pragma once
#include "add_order_base.h"
#include <string>
#include <memory>
#include <stdexcept>

namespace CboePitch {

    class AddOrderLong : public AddOrderBase {
    private:
        uint64_t price;

    public:
        AddOrderLong(uint32_t ts, const std::string& oid, char side, uint32_t sh,
                     const std::string& sym, uint64_t pr, char disp)
            : AddOrderBase(ts, oid, side, sh, sym, disp), price(pr) {
        }

        static std::unique_ptr<AddOrderLong> parse(const std::string& line) {
            if (line.size() < 35) {
                throw std::invalid_argument("Invalid AddOrderLong length");
            }
            try {
                uint32_t ts = std::stoul(line.substr(0, 8));
                std::string oid = line.substr(9, 12);
                char side = line[21];
                uint32_t sh = std::stoul(line.substr(22, 6));
                std::string sym = line.substr(28, 8);
                uint64_t pr = std::stoull(line.substr(36, 10));
                char disp = line[46];
                return std::make_unique<AddOrderLong>(ts, oid, side, sh, sym, pr, disp);
            } catch (const std::exception& e) {
                throw std::invalid_argument("AddOrderLong parse error: " + std::string(e.what()));
            }
        }

        char getMessageType() const override { return 'B'; }

        std::string toString() const override {
            char buffer[100];
            snprintf(buffer, sizeof(buffer),
                     "AddOrderLong: Timestamp=%u, OrderID=%s, Side=%c, Shares=%u, "
                     "Symbol=%s, Price=%llu, Display=%c",
                     timestamp, orderId.c_str(), sideIndicator, shares,
                     symbol.c_str(), price, display);
            return std::string(buffer);
        }
    };

} // namespace CboePitch