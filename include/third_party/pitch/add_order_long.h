//
// Created by phamanhtan on 29/5/25.
//

#ifndef ADD_ORDER_LONG_H
#define ADD_ORDER_LONG_H

// AddOrderLong message
class AddOrderLong : public Message {
private:
    uint32_t timestamp;
    std::string orderId;
    char sideIndicator;
    uint32_t shares;
    std::string symbol;
    uint64_t price;
    char display;

public:
    AddOrderLong(uint32_t ts, const std::string& oid, char side, uint32_t sh,
                 const std::string& sym, uint64_t pr, char disp)
        : timestamp(ts), orderId(oid), sideIndicator(side), shares(sh),
          symbol(sym), price(pr), display(disp) {
        validate();
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

    std::string getSymbol() const { return symbol; }
    void setSymbol(const std::string& sym) {
        if (sym.size() > 8) throw std::invalid_argument("Symbol must be <= 8 characters");
        symbol = sym;
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

private:
    void validate() {
        if (orderId.size() != 12) throw std::invalid_argument("Order ID must be 12 characters");
        if (symbol.size() > 8) throw std::invalid_argument("Symbol must be <= 8 characters");
        if (sideIndicator != 'B' && sideIndicator != 'S') throw std::invalid_argument("Side must be 'B' or 'S'");
        if (display != 'Y' && display != 'N') throw std::invalid_argument("Display must be 'Y' or 'N'");
    }
};

#endif //ADD_ORDER_LONG_H
