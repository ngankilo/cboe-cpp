/**
 * @file    SymbolIdentifier.hpp
 * @brief   Maps order IDs to trading symbols with fast lookup.
 *
 * Developer: Hoang Nguyen & Tan A. Pham
 * Copyright: Equix Technologies Pty Ltd (contact@equix.com.au)
 * Filename: SymbolIdentifier.hpp
 * Created: 28/May/2025
 * Modified: Enhanced to store symbol+price as byte array
 *
 * Description:
 *   Provides an efficient mapping from unique order IDs to trading symbol+price data.
 *   Uses tsl::robin_map for high-performance insert and lookup.
 *   Thread-safety is NOT provided by this class.
 *
 *   Data Format: 14 bytes total
 *   - Bytes 0-5: Symbol (6 characters, space-padded)
 *   - Bytes 6-13: Price (8-byte double, little-endian)
 */

#pragma once

#ifndef SYMBOL_IDENTIFIER_HPP_
#define SYMBOL_IDENTIFIER_HPP_

#include <string>
#include <optional>
#include <vector>
#include <cstring>
#include "tsl/robin_map.h"

namespace equix_md {

/**
 * @class SymbolIdentifier
 * @brief Fast mapping from order IDs to trading symbols+price using robin_map.
 *
 * Not thread-safe: external synchronization is required for concurrent use.
 * Now stores symbol and price as a compact byte array for efficiency.
 */
class SymbolIdentifier {
public:
    static constexpr size_t SYMBOL_SIZE = 6;    ///< Fixed symbol size in bytes
    static constexpr size_t PRICE_SIZE = 8;     ///< Price size in bytes (double)
    static constexpr size_t DATA_SIZE = SYMBOL_SIZE + PRICE_SIZE; ///< Total data size per entry

    /**
     * @brief Constructor, optionally reserves space for estimated number of mappings.
     * @param estimated_mapping_count Initial capacity for the hash map (optional).
     */
    explicit SymbolIdentifier(size_t estimated_mapping_count = 0);

    /**
     * @brief Add a mapping from an order ID to symbol+price data.
     * @param order_id      Unique order identifier.
     * @param symbol_name   Associated trading symbol string (will be truncated/padded to 6 chars).
     * @param price         Associated price value.
     * @return true if inserted, false if order_id already existed.
     */
    bool add_mapping(uint64_t order_id, const std::string& symbol_name, double price);

    /**
     * @brief Legacy method: Add mapping with symbol only (price set to 0.0).
     * @param order_id      Unique order identifier.
     * @param symbol_name   Associated trading symbol string.
     * @return true if inserted, false if order_id already existed.
     */
    bool add_mapping(uint64_t order_id, const std::string& symbol_name);

    /**
     * @brief Update the price for an existing order ID.
     * @param order_id  Unique order identifier.
     * @param price     New price value.
     * @return true if order existed and was updated, false otherwise.
     */
    bool update_price(uint64_t order_id, double price);

    /**
     * @brief Look up the symbol associated with a given order ID.
     * @param order_id  Unique order identifier.
     * @return Optional symbol string if found, otherwise std::nullopt.
     */
    std::optional<std::string> find_symbol(uint64_t order_id) const;

    /**
     * @brief Extract symbol string from stored data for given order ID.
     * @param order_id  Unique order identifier.
     * @return Symbol string if found, empty string otherwise.
     */
    std::string getSymbol(uint64_t order_id) const;

    /**
     * @brief Get the raw byte data (symbol+price) for a given order ID.
     * @param order_id  Unique order identifier.
     * @return Optional byte vector if found, otherwise std::nullopt.
     */
    std::optional<std::vector<uint8_t>> findData(uint64_t order_id) const;

    /**
     * @brief Extract price from stored data for given order ID.
     * @param order_id  Unique order identifier.
     * @return Price value if found, 0.0 otherwise.
     */
    double getPrice(uint64_t order_id) const;

    /**
     * @brief Remove a mapping for a given order ID.
     * @param order_id  Unique order identifier.
     * @return true if mapping existed and was erased, false otherwise.
     */
    bool remove_mapping(uint64_t order_id);

    /**
     * @brief Get the number of mappings currently stored.
     * @return Number of (order_id, symbol+price) mappings.
     */
    size_t mapping_count() const;

    /**
     * @brief Reserve space for at least n mappings.
     * @param min_capacity Number of mappings to reserve space for.
     */
    void reserve(size_t min_capacity);

private:
    tsl::robin_map<uint64_t, std::vector<uint8_t>> order_to_data_map_; ///< Internal mapping from order ID to symbol+price data

    /**
     * @brief Serialize symbol and price into byte array.
     * @param symbol_name Symbol string (will be truncated/padded to 6 chars).
     * @param price Price value.
     * @return 14-byte vector containing serialized data.
     */
    std::vector<uint8_t> serialize_data(const std::string& symbol_name, double price) const;

    /**
     * @brief Extract symbol from byte array.
     * @param data 14-byte data array.
     * @return Symbol string (trimmed of trailing spaces).
     */
    std::string extract_symbol(const std::vector<uint8_t>& data) const;

    /**
     * @brief Extract price from byte array.
     * @param data 14-byte data array.
     * @return Price value.
     */
    double extract_price(const std::vector<uint8_t>& data) const;
};

} // namespace equix_md

#endif // SYMBOL_IDENTIFIER_HPP_