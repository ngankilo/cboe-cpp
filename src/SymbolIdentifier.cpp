/**
 * @file    SymbolIdentifier.cpp
 * @brief   Implementation of order ID to symbol mapping.
 *
 * Developer: Hoang Nguyen & Tan A. Pham
 * Copyright: Equix Technologies Pty Ltd (contact@equix.com.au)
 * Filename: SymbolIdentifier.cpp
 * Created: 28/May/2025
 *
 * Description:
 *   Implements fast, non-thread-safe mapping from order IDs to symbols.
 */

#include "SymbolIdentifier.hpp"

namespace equix_md {

/**
 * @brief Constructor. Optionally pre-reserves capacity to minimize rehashing.
 */
SymbolIdentifier::SymbolIdentifier(size_t estimated_mapping_count) {
    if (estimated_mapping_count > 0)
        order_to_symbol_map_.reserve(estimated_mapping_count);
}

/**
 * @brief Adds a mapping from order_id to symbol_name, if not already present.
 * @return True if inserted, false if already present.
 */
bool SymbolIdentifier::add_mapping(uint64_t order_id, const std::string& symbol_name) {
    auto [it, inserted] = order_to_symbol_map_.emplace(order_id, symbol_name);
    return inserted;
}

/**
 * @brief Looks up the symbol name associated with order_id.
 * @return Optional symbol string if found, std::nullopt otherwise.
 */
std::optional<std::string> SymbolIdentifier::find_symbol(uint64_t order_id) const {
    auto it = order_to_symbol_map_.find(order_id);
    if (it != order_to_symbol_map_.end())
        return it->second;
    return std::nullopt;
}

/**
 * @brief Removes the mapping for the given order_id, if present.
 * @return True if the mapping existed and was erased.
 */
bool SymbolIdentifier::remove_mapping(uint64_t order_id) {
    return order_to_symbol_map_.erase(order_id) > 0;
}

/**
 * @brief Returns the current number of stored mappings.
 */
size_t SymbolIdentifier::mapping_count() const {
    return order_to_symbol_map_.size();
}

/**
 * @brief Reserves space for at least min_capacity mappings to optimize future insertions.
 */
void SymbolIdentifier::reserve(size_t min_capacity) {
    order_to_symbol_map_.reserve(min_capacity);
}

} // namespace equix_md
