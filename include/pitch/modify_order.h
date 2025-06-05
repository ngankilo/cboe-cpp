#ifndef MODIFY_ORDER_H
#define MODIFY_ORDER_H

#include "message.h"
#include "SymbolIdentifier.hpp"
#include <string>
#include <stdexcept>
#include <sstream>

namespace CboePitch {
    class ModifyOrder : public Message {
    public:
        static constexpr uint8_t MESSAGE_TYPE = 0x3A;
        static constexpr size_t MESSAGE_SIZE = 31;

        static ModifyOrder parse(const uint8_t *data, size_t size, equix_md::SymbolIdentifier& symbol_map, size_t offset = 0) {
            if (size < MESSAGE_SIZE) {
                throw std::invalid_argument("ModifyOrder message too short");
            }

            // Parse the core ModifyOrder fields
            uint64_t timestamp = Message::readUint64LE(data + offset + 2);
            uint64_t orderId = Message::readUint64LE(data + offset + 10);
            uint32_t quantity = Message::readUint32LE(data + offset + 18);
            double price = Message::decodePrice(data + offset + 22);

            // CRITICAL STEP 1: Update the price in SymbolIdentifier
            // This ensures that future OrderExecuted messages will have the correct price
            if (!symbol_map.update_price(orderId, price)) {
                std::cerr << "Warning: Order ID " << orderId << " not found in SymbolIdentifier for price update" << std::endl;
            }

            // Create the ModifyOrder instance first
            ModifyOrder modify_order(timestamp, orderId, quantity, price);
            modify_order.setSymbolMap(&symbol_map);

            // CRITICAL STEP 2: Enhanced payload creation
            // We append the symbol+price data to the original message payload
            // This creates a "rich" message that contains both the modify information
            // and the complete symbol context
            modify_order.createEnhancedPayload(data + offset, MESSAGE_SIZE, symbol_map);

            return modify_order;
        }

        // Override getSymbol to fetch from SymbolIdentifier rather than local storage
        // This demonstrates the new data flow where symbol info comes from the map
        const std::string &getSymbol() const override {
            // We need to return a reference, but SymbolIdentifier::getSymbol() returns by value
            // So we cache the result in a mutable member variable
            if (symbol_map_ && cached_symbol.empty()) {
                cached_symbol = symbol_map_->getSymbol(orderId);
            }
            return cached_symbol;
        }

        std::string toString() const override {
            std::ostringstream oss;
            oss << "ModifyOrder{timestamp=" << timestamp
                << ", orderId=" << orderId
                << ", quantity=" << quantity
                << ", price=" << price
                << ", symbol=" << getSymbol() << "}";
            return oss.str();
        }

        size_t getMessageSize() const override {
            // The enhanced payload is larger than the original message
            return payload.size();
        }

        uint8_t getMessageType() const override { return MESSAGE_TYPE; }
        uint64_t getOrderId() const override { return orderId; }

        // Accessors remain the same for API compatibility
        uint64_t getTimestamp() const { return timestamp; }
        uint32_t getQuantity() const { return quantity; }
        double getPrice() const { return price; }

        // New accessor to get the appended symbol+price data
        // This allows downstream systems to access the raw byte data if needed
        std::vector<uint8_t> getAppendedData() const {
            if (payload.size() > MESSAGE_SIZE) {
                // Return only the appended portion (symbol+price data)
                return std::vector<uint8_t>(payload.begin() + MESSAGE_SIZE, payload.end());
            }
            return {}; // Return empty if no data was appended
        }

    private:
        uint64_t timestamp;
        uint64_t orderId;
        uint32_t quantity;
        double price;
        mutable std::string cached_symbol; // Cache for getSymbol() return reference

        ModifyOrder(uint64_t ts, uint64_t ordId, uint32_t qty, double prc)
            : timestamp(ts), orderId(ordId), quantity(qty), price(prc) {}

        /**
         * @brief Creates an enhanced payload by appending symbol+price data
         *
         * This method demonstrates the core concept: we take the original message payload
         * and append additional context data from the SymbolIdentifier. This creates
         * a "rich" message that downstream systems can process without additional lookups.
         *
         * Enhanced Payload Structure:
         * - Bytes 0-30: Original ModifyOrder message (31 bytes)
         * - Bytes 31-44: Symbol+Price data from SymbolIdentifier (14 bytes)
         *
         * @param original_data Original message bytes
         * @param original_size Size of original message
         * @param symbol_map Reference to SymbolIdentifier for data lookup
         */
        void createEnhancedPayload(const uint8_t* original_data, size_t original_size,
                                 equix_md::SymbolIdentifier& symbol_map) {
            // Start with the original message payload
            payload.assign(original_data, original_data + original_size);

            // Try to get the symbol+price data from SymbolIdentifier
            auto symbol_price_data = symbol_map.findData(orderId);
            if (symbol_price_data.has_value()) {
                // Append the symbol+price data to the payload
                const auto& data = symbol_price_data.value();
                payload.insert(payload.end(), data.begin(), data.end());

                std::cout << "Enhanced ModifyOrder payload: original " << original_size
                         << " bytes + " << data.size() << " bytes appended = "
                         << payload.size() << " bytes total" << std::endl;
            } else {
                // Warning: This shouldn't happen if the system is working correctly
                std::cerr << "Warning: Could not find symbol+price data for order ID "
                         << orderId << " during ModifyOrder payload enhancement" << std::endl;
                // Set payload to just the original message
                // This ensures the system remains functional even if there's an issue
            }
        }
    };
} // namespace CboePitch

#endif // MODIFY_ORDER_H