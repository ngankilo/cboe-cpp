/**
 * @file    SymbolIdentifier.hpp
 * @brief   Maps order IDs to trading symbols with fast lookup.
 *
 * Developer: Hoang Nguyen & Tan A. Pham
 * Copyright: Equix Technologies Pty Ltd (contact@equix.com.au)
 * Filename: SymbolIdentifier.hpp
 * Created: 28/May/2025
 *
 * Description:
 *   Provides an efficient mapping from unique order IDs to trading symbol strings.
 *   Uses tsl::robin_map for high-performance insert and lookup.
 *   Thread-safety is NOT provided by this class.
 */

#pragma once

#ifndef SYMBOL_IDENTIFIER_HPP_
#define SYMBOL_IDENTIFIER_HPP_

#include <string>
#include <optional>
#include "tsl/robin_map.h"

namespace equix_md {

/**
 * @class SymbolIdentifier
 * @brief Fast mapping from order IDs to trading symbols using robin_map.
 *
 * Not thread-safe: external synchronization is required for concurrent use.
 */
class SymbolIdentifier {
public:
    /**
     * @brief Constructor, optionally reserves space for estimated number of mappings.
     * @param estimated_mapping_count Initial capacity for the hash map (optional).
     */
    explicit SymbolIdentifier(size_t estimated_mapping_count = 0);

    /**
     * @brief Add a mapping from an order ID to a symbol.
     * @param order_id      Unique order identifier.
     * @param symbol_name   Associated trading symbol string.
     * @return true if inserted, false if order_id already existed.
     */
    bool add_mapping(uint64_t order_id, const std::string& symbol_name);

    /**
     * @brief Look up the symbol associated with a given order ID.
     * @param order_id  Unique order identifier.
     * @return Optional symbol string if found, otherwise std::nullopt.
     */
    std::optional<std::string> find_symbol(uint64_t order_id) const;

    /**
     * @brief Remove a mapping for a given order ID.
     * @param order_id  Unique order identifier.
     * @return true if mapping existed and was erased, false otherwise.
     */
    bool remove_mapping(uint64_t order_id);

    /**
     * @brief Get the number of mappings currently stored.
     * @return Number of (order_id, symbol) mappings.
     */
    size_t mapping_count() const;

    /**
     * @brief Reserve space for at least n mappings.
     * @param min_capacity Number of mappings to reserve space for.
     */
    void reserve(size_t min_capacity);

private:
    tsl::robin_map<uint64_t, std::string> order_to_symbol_map_; ///< Internal mapping from order ID to symbol name.
};

} // namespace equix_md

#endif // SYMBOL_IDENTIFIER_HPP_
