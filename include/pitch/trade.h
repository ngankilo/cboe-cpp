//
// Created by phamanhtan on 30/5/25.
//

#ifndef TRADE_H
#define TRADE_H

#include "trade_base.h"

namespace CboePitch {
    class Trade : public TradeBase {
    public:
        static Trade parse(const uint8_t *data, size_t length);

        std::string toString() const override;

        std::string getSymbol() const override { return symbol; }
        void setSymbol(const std::string &sym) override { symbol = sym; }

        uint32_t getQuantity() const { return quantity; }
        double getPrice() const { return price; }
        uint64_t getOrderId() const { return orderId; }
        uint64_t getContraOrderId() const { return contraOrderId; }
        std::string getPID() const { return pid; }
        std::string getContraPID() const { return contraPid; }
        char getTradeType() const { return tradeType; }
        char getTradeDesignation() const { return tradeDesignation; }
        char getTradeReportType() const { return tradeReportType; }
        uint64_t getTradeTransactionTime() const { return tradeTransactionTime; }

    private:
        std::string symbol;
        uint32_t quantity;
        double price;
        uint64_t orderId;
        uint64_t contraOrderId;
        std::string pid;
        std::string contraPid;
        char tradeType;
        char tradeDesignation;
        char tradeReportType;
        uint64_t tradeTransactionTime;

        Trade(uint64_t ts, const std::string &sym, uint32_t qty, double prc, uint64_t execId,
              uint64_t ordId, uint64_t contraId, const std::string &p, const std::string &contraP,
              char tt, char td, char trt, uint64_t ttt)
            : TradeBase(0x3D, ts, execId), symbol(sym), quantity(qty), price(prc),
              orderId(ordId), contraOrderId(contraId), pid(p), contraPid(contraP), tradeType(tt),
              tradeDesignation(td), tradeReportType(trt), tradeTransactionTime(ttt) {
        }
    };
} // namespace CboePitch

#endif // TRADE_H
