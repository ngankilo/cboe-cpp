/**
 * @file    SymbolIdentifier.cpp
 * @brief   Implementation of order ID to symbol+price mapping.
 *
 * Developer: Hoang Nguyen & Tan A. Pham
 * Copyright: Equix Technologies Pty Ltd (contact@equix.com.au)
 * Filename: SymbolIdentifier.cpp
 * Created: 28/May/2025
 * Modified: Enhanced to store symbol+price as byte array
 *
 * Description:
 *   Implements fast, non-thread-safe mapping from order IDs to symbol+price data.
 *   Uses compact byte array storage for memory efficiency.
 */

#include "SymbolIdentifier.hpp"
#include <algorithm>

namespace equix_md {

/**
 * @brief Constructor. Optionally pre-reserves capacity to minimize rehashing.
 */
SymbolIdentifier::SymbolIdentifier(size_t estimated_mapping_count) {
    if (estimated_mapping_count > 0)
        order_to_data_map_.reserve(estimated_mapping_count);
}

/**
 * @brief Adds a mapping from order_id to symbol+price data, if not already present.
 * @return True if inserted, false if already present.
 */
bool SymbolIdentifier::add_mapping(uint64_t order_id, const std::string& symbol_name, double price) {
    auto serialized_data = serialize_data(symbol_name, price);
    auto [it, inserted] = order_to_data_map_.emplace(order_id, std::move(serialized_data));
    return inserted;
}

/**
 * @brief Legacy overload: adds mapping with symbol only (price defaults to 0.0).
 * This maintains backward compatibility with existing code.
 */
bool SymbolIdentifier::add_mapping(uint64_t order_id, const std::string& symbol_name) {
    return add_mapping(order_id, symbol_name, 0.0);
}

/**
 * @brief Updates the price for an existing order ID without changing the symbol.
 *
 * This implementation uses the erase-and-insert pattern to work around potential
 * const-iterator issues with tsl::robin_map. While this might seem less efficient
 * than in-place modification, modern hash maps are highly optimized for this
 * pattern, and the performance difference is negligible while ensuring compatibility.
 */
bool SymbolIdentifier::update_price(uint64_t order_id, double price) {
    auto it = order_to_data_map_.find(order_id);
    if (it == order_to_data_map_.end()) {
        return false; // Order ID not found
    }

    // Extract existing symbol before we erase the entry
    // This is important: we must extract the symbol BEFORE erasing because
    // the iterator becomes invalid after erase()
    std::string existing_symbol = extract_symbol(it->second);

    // Erase the old entry first
    // This invalidates our iterator, but that's okay because we've already
    // extracted the information we need
    order_to_data_map_.erase(it);

    // Insert the new entry with updated price
    // This approach is guaranteed to work because we're using the standard
    // insertion interface that all hash maps must support
    auto new_data = serialize_data(existing_symbol, price);
    order_to_data_map_.emplace(order_id, std::move(new_data));

    return true;
}

/**
 * @brief Legacy method: looks up symbol name only (for backward compatibility).
 */
std::optional<std::string> SymbolIdentifier::find_symbol(uint64_t order_id) const {
    auto it = order_to_data_map_.find(order_id);
    if (it != order_to_data_map_.end()) {
        return extract_symbol(it->second);
    }
    return std::nullopt;
}

/**
 * @brief Extracts symbol string from stored data.
 * This is the preferred method for getting symbol data.
 */
std::string SymbolIdentifier::getSymbol(uint64_t order_id) const {
    auto it = order_to_data_map_.find(order_id);
    if (it != order_to_data_map_.end()) {
        return extract_symbol(it->second);
    }
    return ""; // Return empty string if not found
}

/**
 * @brief Gets the raw byte data for external processing.
 * This allows other components to append the data to their payloads.
 */
std::optional<std::vector<uint8_t>> SymbolIdentifier::findData(uint64_t order_id) const {
    auto it = order_to_data_map_.find(order_id);
    if (it != order_to_data_map_.end()) {
        return it->second; // Return copy of the byte vector
    }
    return std::nullopt;
}

/**
 * @brief Extracts price value from stored data.
 */
double SymbolIdentifier::getPrice(uint64_t order_id) const {
    auto it = order_to_data_map_.find(order_id);
    if (it != order_to_data_map_.end()) {
        return extract_price(it->second);
    }
    return 0.0; // Return 0.0 if not found
}

/**
 * @brief Removes the mapping for the given order_id, if present.
 * @return True if the mapping existed and was erased.
 */
bool SymbolIdentifier::remove_mapping(uint64_t order_id) {
    return order_to_data_map_.erase(order_id) > 0;
}

/**
 * @brief Returns the current number of stored mappings.
 */
size_t SymbolIdentifier::mapping_count() const {
    return order_to_data_map_.size();
}

/**
 * @brief Reserves space for at least min_capacity mappings to optimize future insertions.
 */
void SymbolIdentifier::reserve(size_t min_capacity) {
    order_to_data_map_.reserve(min_capacity);
}

/**
 * @brief Serializes symbol and price into a compact byte array.
 *
 * Data layout:
 * - Bytes 0-5: Symbol (space-padded to exactly 6 characters)
 * - Bytes 6-13: Price (8-byte double in little-endian format)
 *
 * This format matches the binary layout used in the message parsing,
 * allowing for efficient data handling without additional conversions.
 */
std::vector<uint8_t> SymbolIdentifier::serialize_data(const std::string& symbol_name, double price) const {
    std::vector<uint8_t> data(DATA_SIZE);

    // Serialize symbol: pad with spaces to exactly 6 characters
    std::string padded_symbol = symbol_name;
    if (padded_symbol.length() > SYMBOL_SIZE) {
        padded_symbol = padded_symbol.substr(0, SYMBOL_SIZE); // Truncate if too long
    } else {
        padded_symbol.resize(SYMBOL_SIZE, ' '); // Pad with spaces if too short
    }

    // Copy symbol to first 6 bytes
    std::memcpy(data.data(), padded_symbol.data(), SYMBOL_SIZE);

    // Serialize price: store as little-endian double (8 bytes)
    std::memcpy(data.data() + SYMBOL_SIZE, &price, PRICE_SIZE);

    return data;
}

/**
 * @brief Extracts symbol from byte array, removing trailing spaces.
 *
 * This handles the common case where symbols are shorter than 6 characters
 * and were space-padded during serialization.
 */
std::string SymbolIdentifier::extract_symbol(const std::vector<uint8_t>& data) const {
    if (data.size() < SYMBOL_SIZE) {
        return ""; // Invalid data size
    }

    // Create string from first 6 bytes
    std::string symbol(reinterpret_cast<const char*>(data.data()), SYMBOL_SIZE);

    // Trim trailing spaces using standard algorithm
    auto end_pos = symbol.find_last_not_of(' ');
    if (end_pos != std::string::npos) {
        symbol = symbol.substr(0, end_pos + 1);
    } else {
        symbol.clear(); // All spaces, return empty string
    }

    return symbol;
}

/**
 * @brief Extracts price from byte array.
 *
 * Reads the 8-byte double value stored in little-endian format
 * at bytes 6-13 of the data array.
 */
double SymbolIdentifier::extract_price(const std::vector<uint8_t>& data) const {
    if (data.size() < DATA_SIZE) {
        return 0.0; // Invalid data size
    }

    double price;
    std::memcpy(&price, data.data() + SYMBOL_SIZE, PRICE_SIZE);
    return price;
}

} // namespace equix_md