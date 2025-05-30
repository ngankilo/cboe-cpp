//
// Created by phamanhtan on 30/5/25.
//

#ifndef ADD_ORDER_H
#define ADD_ORDER_H

#include "message.h"

namespace CboePitch {
    class AddOrder final : public Message {
    public:
        static AddOrder parse(const uint8_t *data, size_t length);

        std::string toString() const override;

        std::string getSymbol() const override { return symbol; }
        void setSymbol(const std::string &sym) override { symbol = sym; }

        uint64_t getTimestamp() const { return timestamp; }
        uint64_t getOrderId() const { return orderId; }
        char getSideIndicator() const { return sideIndicator; }
        uint32_t getQuantity() const { return quantity; }
        double getPrice() const { return price; }
        std::string getPID() const { return pid; }

    private:
        uint64_t timestamp;
        uint64_t orderId;
        char sideIndicator;
        uint32_t quantity;
        double price;
        std::string symbol;
        std::string pid;

        AddOrder(uint64_t ts, uint64_t ordId, char side, uint32_t qty, double prc,
                 const std::string &sym, const std::string &p)
            : Message(0x37), timestamp(ts), orderId(ordId), sideIndicator(side), quantity(qty),
              price(prc), symbol(sym), pid(p) {
        }
    };
} // namespace CboePitch

#endif // ADD_ORDER_H
