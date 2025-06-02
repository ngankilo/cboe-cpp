/**
 * @file    SymbolMap.cpp
 * @brief   Implementation of SymbolMap (symbol to message queue map).
 *
 * Developer: Hoang Nguyen & Tan A. Pham
 * Copyright: Equix Technologies Pty Ltd (contact@equix.com.au)
 * Filename: SymbolMap.cpp
 * Created: 28/May/2025
 *
 * Description:
 *   Implements thread-safe queueing and retrieval of messages by symbol name.
 */

#include "SymbolMap.hpp"

/**
 * @brief Enqueue a message pointer for its symbol.
 */
void SymbolMap::push(const IMessagePtr& message_ptr) {
    std::unique_lock lock(symbol_map_mutex_);
    symbol_to_queue_map_[message_ptr->getSymbol()].push_back(message_ptr);
}

/**
 * @brief Remove and return the oldest message for a given symbol, or nullptr if queue empty.
 */
IMessagePtr SymbolMap::pop(const std::string& symbol) {
    std::unique_lock lock(symbol_map_mutex_);
    auto it = symbol_to_queue_map_.find(symbol);
    if (it != symbol_to_queue_map_.end() && !it->second.empty()) {
        auto message = it->second.front();
        it->second.pop_front();
        return message;
    }
    return nullptr;
}

/**
 * @brief Return a list of symbols that have at least one queued message.
 */
std::vector<std::string> SymbolMap::nonempty_symbols() {
    std::shared_lock lock(symbol_map_mutex_);
    std::vector<std::string> nonempty;
    for (const auto& [symbol, queue] : symbol_to_queue_map_) {
        if (!queue.empty()) {
            nonempty.push_back(symbol);
        }
    }
    return nonempty;
}
