/**
 * @file    SymbolMap.hpp
 * @brief   Thread-safe mapping from symbol names to queues of messages.
 *
 * Developer: Hoang Nguyen & Tan A. Pham
 * Copyright: Equix Technologies Pty Ltd (contact@equix.com.au)
 * Filename: SymbolMap.hpp
 * Created: 28/May/2025
 *
 * Description:
 *   Provides a concurrent symbol-to-message-queue map. Allows safe insertion
 *   and removal of messages by symbol, and listing of non-empty symbols.
 */

#pragma once

#ifndef SYMBOL_MAP_HPP_
#define SYMBOL_MAP_HPP_

#include <unordered_map>
#include <deque>
#include <vector>
#include <shared_mutex>
#include <string>
#include "IMessage.hpp"

/**
 * @class SymbolMap
 * @brief Thread-safe map from symbol string to message queue.
 *
 * Each symbol maps to a deque (FIFO queue) of IMessagePtr.
 * Provides safe concurrent access using shared_mutex.
 */
class SymbolMap {
public:
    /**
     * @brief Enqueue a message for its symbol.
     * @param message_ptr  Shared pointer to IMessage instance.
     */
    void push(const IMessagePtr& message_ptr);

    /**
     * @brief Pop the oldest message for the given symbol, or nullptr if empty.
     * @param symbol  Symbol string.
     * @return Shared pointer to IMessage if available, otherwise nullptr.
     */
    IMessagePtr pop(const std::string& symbol);

    /**
     * @brief Get all symbols that currently have at least one queued message.
     * @return Vector of non-empty symbol strings.
     */
    std::vector<std::string> nonempty_symbols();

private:
    std::unordered_map<std::string, std::deque<IMessagePtr>> symbol_to_queue_map_; ///< Map from symbol to message queue.
    std::shared_mutex symbol_map_mutex_; ///< Mutex for thread-safe access.
};

#endif // SYMBOL_MAP_HPP_
