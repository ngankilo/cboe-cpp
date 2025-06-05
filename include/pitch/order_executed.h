#ifndef ORDER_EXECUTED_H
#define ORDER_EXECUTED_H

#include "message.h"
#include <string>
#include <stdexcept>
#include <sstream>

namespace CboePitch {
    class OrderExecuted : public Message {
    public:
        static constexpr uint8_t MESSAGE_TYPE = 0x38;
        static constexpr size_t MESSAGE_SIZE = 43;

        static OrderExecuted parse(const uint8_t *data, size_t size, equix_md::SymbolIdentifier &symbol_map,
                                   size_t offset = 0) {
            if (size < MESSAGE_SIZE) {
                throw std::invalid_argument("OrderExecuted message too short");
            }

            // Parse the core execution information from the message
            uint64_t timestamp = Message::readUint64LE(data + offset + 2);
            uint64_t orderId = Message::readUint64LE(data + offset + 10);
            uint32_t executedQuantity = Message::readUint32LE(data + offset + 18);
            uint64_t executionId = Message::readUint64LE(data + offset + 22);
            uint64_t contraOrderId = Message::readUint64LE(data + offset + 30);

            // Contra PID is 4 bytes ASCII, right padded with spaces
            std::string contraPID(reinterpret_cast<const char *>(data + offset + 38), 4);
            // Trim trailing spaces
            contraPID.erase(contraPID.find_last_not_of(' ') + 1);

            // Create the OrderExecuted instance
            OrderExecuted order_executed(timestamp, orderId, executedQuantity, executionId, contraOrderId, contraPID);
            order_executed.setSymbolMap(&symbol_map);

            // CRITICAL: Enhanced payload creation with symbol+price context
            // This is where the magic happens - we transform a simple execution message
            // into a rich, self-contained message with complete trading context
            order_executed.createContextualPayload(data + offset, MESSAGE_SIZE, symbol_map);

            return order_executed;
        }

        // Override getSymbol to fetch from SymbolIdentifier
        // This demonstrates pure consumption - we don't store symbol locally,
        // but always fetch it from the authoritative source
        const std::string &getSymbol() const override {
            if (symbol_map_ && cached_symbol.empty()) {
                cached_symbol = symbol_map_->getSymbol(orderId);
                if (cached_symbol.empty()) {
                    // This is a serious issue - log it for debugging
                    std::cerr << "CRITICAL: No symbol found for executed order ID " << orderId << std::endl;
                    cached_symbol = "UNKNOWN";
                }
            }
            return cached_symbol;
        }

        std::string toString() const override {
            std::ostringstream oss;
            oss << "OrderExecuted{timestamp=" << timestamp
                    << ", orderId=" << orderId
                    << ", symbol=" << getSymbol()
                    << ", executedQuantity=" << executedQuantity
                    << ", executionId=" << executionId
                    << ", contraOrderId=" << contraOrderId
                    << ", contraPID='" << contraPID << "'}";
            return oss.str();
        }

        size_t getMessageSize() const override {
            // Enhanced payload may be larger than original message
            return payload.size();
        }

        uint8_t getMessageType() const override { return MESSAGE_TYPE; }
        uint64_t getOrderId() const override { return orderId; }

        // Accessors remain unchanged for API compatibility
        uint64_t getTimestamp() const { return timestamp; }
        uint32_t getExecutedQuantity() const { return executedQuantity; }
        uint64_t getExecutionId() const { return executionId; }
        uint64_t getContraOrderId() const { return contraOrderId; }
        std::string getContraPID() const { return contraPID; }

        // New method to access the execution price from SymbolIdentifier
        // This provides richer information than the original message contained
        double getExecutionPrice() const {
            if (symbol_map_) {
                return symbol_map_->getPrice(orderId);
            }
            return 0.0; // Return 0.0 if no price information available
        }

        // Method to get the appended context data
        std::vector<uint8_t> getContextData() const {
            if (payload.size() > MESSAGE_SIZE) {
                return std::vector<uint8_t>(payload.begin() + MESSAGE_SIZE, payload.end());
            }
            return {};
        }

    private:
        uint64_t timestamp;
        uint64_t orderId;
        uint32_t executedQuantity;
        uint64_t executionId;
        uint64_t contraOrderId;
        std::string contraPID;
        mutable std::string cached_symbol; // Cache for performance

        OrderExecuted(uint64_t ts, uint64_t ordId, uint32_t qty, uint64_t execId,
                      uint64_t contraOrdId, std::string pid)
            : timestamp(ts), orderId(ordId), executedQuantity(qty), executionId(execId),
              contraOrderId(contraOrdId), contraPID(std::move(pid)) {
        }

        /**
         * @brief Creates a contextual payload by appending symbol+price information
         *
         * This method implements the "pure consumption" pattern. The original OrderExecuted
         * message contains only execution details, but downstream systems often need the
         * complete trading context (symbol, price, execution details).
         *
         * By appending the symbol+price data here, we create a self-contained message
         * that eliminates the need for downstream systems to perform additional lookups.
         *
         * Enhanced Payload Structure:
         * - Bytes 0-42: Original OrderExecuted message (43 bytes)
         * - Bytes 43-56: Symbol+Price context data (14 bytes)
         *
         * This approach provides several benefits:
         * 1. Performance: Eliminates downstream lookups
         * 2. Atomicity: All related data travels together
         * 3. Consistency: Context is captured at execution time
         * 4. Debugging: Complete information available in message dumps
         *
         * @param original_data Original message bytes
         * @param original_size Size of original message
         * @param symbol_map Reference to SymbolIdentifier for context lookup
         */
        void createContextualPayload(const uint8_t* original_data, size_t original_size,
                                    equix_md::SymbolIdentifier& symbol_map) {
            // Start with the original execution message
            payload.assign(original_data, original_data + original_size);

            // Attempt to retrieve symbol+price context for this order
            auto context_data = symbol_map.findData(orderId);
            if (context_data.has_value()) {
                // Append the context data to create a rich, self-contained message
                const auto& data = context_data.value();
                payload.insert(payload.end(), data.begin(), data.end());

                // Log the enhancement for debugging/monitoring
                std::cout << "Enhanced OrderExecuted payload for order " << orderId
                         << ": " << original_size << " bytes + " << data.size()
                         << " bytes context = " << payload.size() << " bytes total" << std::endl;
            } else {
                // This is a serious data consistency issue
                std::cerr << "ERROR: Cannot find symbol+price context for executed order "
                         << orderId << ". This suggests a data flow problem." << std::endl;

                // In production, you might want to:
                // 1. Log this incident for investigation
                // 2. Send an alert to monitoring systems
                // 3. Consider whether to reject the message or continue with incomplete data

                // For now, we continue with just the original message
                // This ensures system stability even with data inconsistencies
            }
        }
    };
} // namespace CboePitch

#endif // ORDER_EXECUTED_H