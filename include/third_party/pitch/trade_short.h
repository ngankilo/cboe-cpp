#pragma once
#include "trade_base.h"
#include <string>
#include <memory>
#include <stdexcept>

namespace CboePitch {

    class TradeShort : public TradeBase {
    private:
        uint32_t price;

    public:
        TradeShort(uint32_t ts, uint32_t sh, const std::string& sym, uint32_t pr)
            : TradeBase(ts, sh, sym), price(pr) {
        }

        static std::unique_ptr<TradeShort> parse(const std::string& line) {
            if (line.size() < 20) {
                throw std::invalid_argument("Invalid TradeShort length");
            }
            try {
                uint32_t ts = std::stoul(line.substr(0, 8));
                uint32_t sh = std::stoul(line.substr(9, 6));
                std::string sym = line.substr(15, 6);
                uint32_t pr = std::stoul(line.substr(21, 10));
                return std::make_unique<TradeShort>(ts, sh, sym, pr);
            } catch (const std::exception& e) {
                throw std::invalid_argument("TradeShort parse error: " + std::string(e.what()));
            }
        }

        char getMessageType() const override { return 'P'; }

        std::string toString() const override {
            char buffer[100];
            snprintf(buffer, sizeof(buffer),
                     "TradeShort: Timestamp=%u, Shares=%u, Symbol=%s, Price=%u",
                     timestamp, shares, symbol.c_str(), price);
            return std::string(buffer);
        }
    };

} // namespace CboePitch