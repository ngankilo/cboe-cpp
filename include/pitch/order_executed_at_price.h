#ifndef ORDER_EXECUTED_AT_PRICE_H
#define ORDER_EXECUTED_AT_PRICE_H

#include "message.h"
#include <string>
#include <stdexcept>
#include <sstream>

namespace CboePitch {
    class OrderExecutedAtPrice : public Message {
    public:
        static constexpr uint8_t MESSAGE_TYPE = 0x58;
        static constexpr size_t MESSAGE_SIZE = 52;

        static OrderExecutedAtPrice parse(const uint8_t *data, size_t size, equix_md::SymbolIdentifier &symbol_map,
                                          size_t offset = 0) {
            if (size < MESSAGE_SIZE) {
                throw std::invalid_argument("OrderExecutedAtPrice message too short");
            }

            // Parse all execution details including the execution price
            uint64_t timestamp = Message::readUint64LE(data + offset + 2);
            uint64_t orderId = Message::readUint64LE(data + offset + 10);
            uint32_t executedQuantity = Message::readUint32LE(data + offset + 18);
            uint64_t executionId = Message::readUint64LE(data + offset + 22);
            uint64_t contraOrderId = Message::readUint64LE(data + offset + 30);

            // Contra PID: 4 ASCII characters, right-padded with spaces
            std::string contraPid(reinterpret_cast<const char *>(data + offset + 38), 4);
            contraPid.erase(contraPid.find_last_not_of(' ') + 1); // trim trailing spaces

            char executionType = static_cast<char>(data[offset + 42]);
            double executionPrice = Message::decodePrice(data + offset + 43); // This is the actual execution price

            // Reserved byte at offset + 51 ignored

            OrderExecutedAtPrice order_executed_at_price(timestamp, orderId, executedQuantity, executionPrice,
                                                         executionId, contraOrderId, contraPid, executionType);
            order_executed_at_price.setSymbolMap(&symbol_map);

            // CRITICAL: Enhanced payload with dual-price context
            // This message is special because it contains both:
            // 1. Original order price (from SymbolIdentifier)
            // 2. Actual execution price (from this message)
            // This provides complete price transparency for analysis
            order_executed_at_price.createDualPricePayload(data + offset, MESSAGE_SIZE, symbol_map);

            return order_executed_at_price;
        }

        // Override getSymbol to fetch from SymbolIdentifier
        const std::string &getSymbol() const override {
            if (symbol_map_ && cached_symbol.empty()) {
                cached_symbol = symbol_map_->getSymbol(orderId);
                if (cached_symbol.empty()) {
                    std::cerr << "CRITICAL: No symbol found for executed order ID " << orderId << std::endl;
                    cached_symbol = "UNKNOWN";
                }
            }
            return cached_symbol;
        }

        std::string toString() const override {
            std::ostringstream oss;
            oss << "OrderExecutedAtPrice{timestamp=" << timestamp
                    << ", orderId=" << orderId
                    << ", symbol=" << getSymbol()
                    << ", executedQuantity=" << executedQuantity
                    << ", executionId=" << executionId
                    << ", contraOrderId=" << contraOrderId
                    << ", contraPid='" << contraPid << "'"
                    << ", executionType='" << executionType << "'"
                    << ", executionPrice=" << price;

            // Show original order price if available for comparison
            if (symbol_map_) {
                double originalPrice = symbol_map_->getPrice(orderId);
                if (originalPrice != price) {
                    oss << ", originalPrice=" << originalPrice;
                }
            }
            oss << "}";
            return oss.str();
        }

        size_t getMessageSize() const override {
            return payload.size(); // Enhanced payload size
        }

        uint8_t getMessageType() const override { return MESSAGE_TYPE; }
        uint64_t getOrderId() const override { return orderId; }

        // Standard accessors
        uint64_t getTimestamp() const { return timestamp; }
        uint32_t getExecutedQuantity() const { return executedQuantity; }
        double getPrice() const { return price; } // This returns the execution price
        uint64_t getExecutionId() const { return executionId; }
        uint64_t getContraOrderId() const { return contraOrderId; }
        std::string getContraPID() const { return contraPid; }
        char getExecutionType() const { return executionType; }

        // Enhanced accessors for dual-price information
        double getExecutionPrice() const { return price; } // Alias for clarity

        double getOriginalOrderPrice() const {
            if (symbol_map_) {
                return symbol_map_->getPrice(orderId);
            }
            return 0.0;
        }

        // Calculate price improvement/deterioration
        double getPriceImprovement() const {
            if (symbol_map_) {
                double originalPrice = symbol_map_->getPrice(orderId);
                return price - originalPrice; // Positive = better execution, Negative = worse
            }
            return 0.0;
        }

        // Method to get all enhanced context data
        std::vector<uint8_t> getDualPriceContext() const {
            if (payload.size() > MESSAGE_SIZE) {
                return std::vector<uint8_t>(payload.begin() + MESSAGE_SIZE, payload.end());
            }
            return {};
        }

    private:
        uint64_t timestamp;
        uint64_t orderId;
        uint32_t executedQuantity;
        double price; // This is the execution price from the message
        uint64_t executionId;
        uint64_t contraOrderId;
        std::string contraPid;
        char executionType;
        mutable std::string cached_symbol;

        OrderExecutedAtPrice(uint64_t ts, uint64_t ordId, uint32_t qty, double prc,
                             uint64_t execId, uint64_t contraId, const std::string &contraP,
                             char execType)
            : timestamp(ts), orderId(ordId), executedQuantity(qty), price(prc),
              executionId(execId), contraOrderId(contraId), contraPid(contraP),
              executionType(execType) {
        }

        /**
         * @brief Creates enhanced payload with dual-price context information
         *
         * This is the most sophisticated payload enhancement in our system because
         * it handles the complex case where we have TWO different price points:
         *
         * 1. Original Order Price: The price at which the order was originally placed
         *    (retrieved from SymbolIdentifier)
         * 2. Execution Price: The actual price at which the trade occurred
         *    (contained in this message)
         *
         * This dual-price information is crucial for:
         * - Trade analysis and reporting
         * - Price improvement calculations
         * - Market impact studies
         * - Regulatory reporting requirements
         *
         * Enhanced Payload Structure:
         * - Bytes 0-51: Original OrderExecutedAtPrice message (52 bytes)
         * - Bytes 52-65: Original order symbol+price context (14 bytes)
         * - Bytes 66-73: Execution price as 8-byte double (8 bytes)
         *
         * Total enhanced size: 74 bytes (52 + 14 + 8)
         *
         * This creates a completely self-contained execution record with full context.
         *
         * @param original_data Original message bytes
         * @param original_size Size of original message
         * @param symbol_map Reference to SymbolIdentifier for original order context
         */
        void createDualPricePayload(const uint8_t* original_data, size_t original_size,
                                   equix_md::SymbolIdentifier& symbol_map) {
            // Start with original execution message
            payload.assign(original_data, original_data + original_size);

            // Get the original order context (symbol + original price)
            auto original_context = symbol_map.findData(orderId);
            if (original_context.has_value()) {
                // Append original order context
                const auto& context_data = original_context.value();
                payload.insert(payload.end(), context_data.begin(), context_data.end());

                // Append execution price as separate field for easy comparison
                // This allows downstream systems to easily access both prices
                uint8_t execution_price_bytes[8];
                std::memcpy(execution_price_bytes, &price, 8);
                payload.insert(payload.end(), execution_price_bytes, execution_price_bytes + 8);

                // Log the complex enhancement
                double original_price = symbol_map.getPrice(orderId);
                std::cout << "Enhanced OrderExecutedAtPrice for order " << orderId
                         << ": original=" << original_size << "B + context=" << context_data.size()
                         << "B + exec_price=8B = " << payload.size() << "B total"
                         << " (prices: original=" << original_price << ", execution=" << price << ")" << std::endl;

                // Alert on significant price differences (this could indicate market volatility)
                double price_diff = std::abs(price - original_price);
                if (price_diff > (original_price * 0.01)) { // More than 1% difference
                    std::cout << "NOTICE: Significant price difference detected - Original: "
                             << original_price << ", Execution: " << price
                             << ", Difference: " << price_diff << " ("
                             << (price_diff/original_price * 100) << "%)" << std::endl;
                }
            } else {
                std::cerr << "ERROR: Cannot find original order context for executed order "
                         << orderId << " in OrderExecutedAtPrice" << std::endl;
                // Continue with original message only
            }
        }
    };
} // namespace CboePitch

#endif // ORDER_EXECUTED_AT_PRICE_H