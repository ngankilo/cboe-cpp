#ifndef TRADE_MESSAGE_H
#define TRADE_MESSAGE_H

#include "message.h"
#include <string>
#include <sstream>
#include <stdexcept>
#include <iomanip>

namespace CboePitch {
    class Trade : public Message {
    public:
        static constexpr uint8_t MESSAGE_TYPE = 0x3D;
        static constexpr size_t MESSAGE_SIZE = 72;

        static Trade parse(const uint8_t *data, size_t size, size_t offset = 0) {
            if (size < offset + MESSAGE_SIZE) {
                throw std::invalid_argument("Trade message too short");
            }

            uint64_t timestamp = Message::readUint64LE(data + offset + 2);
            std::string symbol(reinterpret_cast<const char *>(data + offset + 10), 6);
            symbol = Message::trimRight(symbol);

            uint32_t quantity = Message::readUint32LE(data + offset + 16);
            double price = Message::decodePrice(data + offset + 20);

            uint64_t executionId = Message::readUint64LE(data + offset + 28);
            uint64_t orderId = Message::readUint64LE(data + offset + 36);
            uint64_t contraOrderId = Message::readUint64LE(data + offset + 44);

            std::string pid(reinterpret_cast<const char *>(data + offset + 52), 4);
            pid = Message::trimRight(pid);

            std::string contraPid(reinterpret_cast<const char *>(data + offset + 56), 4);
            contraPid = Message::trimRight(contraPid);

            char tradeType = static_cast<char>(data[offset + 60]);
            char tradeDesignation = static_cast<char>(data[offset + 61]);
            char tradeReportType = static_cast<char>(data[offset + 62]);

            uint64_t tradeTxnTime = Message::readUint64LE(data + offset + 63);
            uint8_t flags = data[offset + 71];

            return Trade(timestamp, symbol, quantity, price, executionId, orderId,
                                contraOrderId, pid, contraPid, tradeType,
                                tradeDesignation, tradeReportType, tradeTxnTime, flags);
        }

        std::string toString() const override {
            std::ostringstream oss;
            oss << "Trade{timestamp=" << timestamp
                    << ", symbol=" << symbol
                    << ", quantity=" << quantity
                    << ", price=" << price
                    << ", executionId=" << executionId
                    << ", orderId=" << orderId
                    << ", contraOrderId=" << contraOrderId
                    << ", pid=" << pid
                    << ", contraPid=" << contraPid
                    << ", tradeType=" << tradeType
                    << ", tradeDesignation=" << tradeDesignation
                    << ", tradeReportType=" << tradeReportType
                    << ", tradeTxnTime=" << tradeTxnTime
                    << ", flags=" << static_cast<int>(flags)
                    << "}";
            return oss.str();
        }

        size_t getMessageSize() const override { return MESSAGE_SIZE; }
        uint8_t getMessageType() const override { return MESSAGE_TYPE; }

        // Accessors
        uint64_t getTimestamp() const { return timestamp; }
        const std::string &getSymbol() const override { return symbol; }
        uint32_t getQuantity() const { return quantity; }
        double getPrice() const { return price; }
        uint64_t getExecutionId() const { return executionId; }
        uint64_t getOrderId() const { return orderId; }
        uint64_t getContraOrderId() const { return contraOrderId; }
        const std::string &getPid() const { return pid; }
        const std::string &getContraPid() const { return contraPid; }
        char getTradeType() const { return tradeType; }
        char getTradeDesignation() const { return tradeDesignation; }
        char getTradeReportType() const { return tradeReportType; }
        uint64_t getTradeTransactionTime() const { return tradeTxnTime; }
        uint8_t getFlags() const { return flags; }

    private:
        uint64_t timestamp;
        std::string symbol;
        uint32_t quantity;
        double price;
        uint64_t executionId;
        uint64_t orderId;
        uint64_t contraOrderId;
        std::string pid;
        std::string contraPid;
        char tradeType;
        char tradeDesignation;
        char tradeReportType;
        uint64_t tradeTxnTime;
        uint8_t flags;

        Trade(uint64_t ts, const std::string &sym, uint32_t qty, double prc, uint64_t execId,
                     uint64_t ordId, uint64_t contraId, const std::string &p, const std::string &contraP,
                     char tt, char td, char trt, uint64_t txnTime, uint8_t flgs)
            : timestamp(ts), symbol(sym), quantity(qty), price(prc), executionId(execId),
              orderId(ordId), contraOrderId(contraId), pid(p), contraPid(contraP),
              tradeType(tt), tradeDesignation(td), tradeReportType(trt),
              tradeTxnTime(txnTime), flags(flgs) {
        }
    };
} // namespace CboePitch

#endif // TRADE_MESSAGE_H
