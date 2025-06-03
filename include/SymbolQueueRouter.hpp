/**
 * @file    SymbolQueueRouter.hpp
 * @brief   Thread-safe router: maps symbols to concurrent message queues.
 *
 * Developer: Hoang Nguyen & Tan A. Pham
 * Copyright: Equix Technologies Pty Ltd (contact@equix.com.au)
 * Filename: SymbolQueueRouter.hpp
 * Created: 28/May/2025
 *
 * Description:
 *   Provides a thread-safe mechanism for routing messages by symbol.
 *   Each unique symbol is associated with its own concurrent queue.
 *   Queues are created on demand and can be safely accessed from multiple threads.
 */

#pragma once

#ifndef SYMBOL_QUEUE_ROUTER_HPP_
#define SYMBOL_QUEUE_ROUTER_HPP_

#include <memory>
#include <mutex>
#include <vector>
#include <string>
#include <iostream> // DEBUG: for std::cout
#include "tsl/robin_map.h"
#include "concurrent_queue/concurrentqueue.h"
#include "pitch/message_factory.h"

/**
 * @class SymbolQueueRouter
 * @brief Thread-safe router mapping symbols to concurrent message queues.
 *
 * Designed for high-throughput environments where symbols may be added at runtime.
 * Each symbol is associated with a unique, lock-free queue (moodycamel::ConcurrentQueue).
 * Safe for concurrent use.
 */
class SymbolQueueRouter {
public:
    using Message      = CboePitch::Message;
    using SymbolId     = std::string;
    using MessagePtr   = std::shared_ptr<Message>;
    using Queue        = moodycamel::ConcurrentQueue<MessagePtr>;

    /**
     * @brief Construct a router with a given queue capacity and expected symbol count.
     * @param queue_capacity    Per-symbol queue size hint (optional).
     * @param expected_symbols  Expected number of symbols to optimize map allocations.
     */
    explicit SymbolQueueRouter(size_t queue_capacity = 4096, size_t expected_symbols = 300000)
        : queue_capacity_(queue_capacity)
    {
        queues_.reserve(expected_symbols);
        symbol_insertion_order_.reserve(expected_symbols);
        queue_vector_.reserve(expected_symbols);
    }

    /**
     * @brief Thread-safe: Push a message to the queue for a symbol, creating the queue if needed.
     * @param symbol    Symbol string (key).
     * @param msg       Unique pointer to message.
     * @return true if the message was enqueued, false otherwise.
     */
    bool push(const SymbolId& symbol, MessagePtr msg) {

        // Capture queue count before
        size_t before_count = queue_count();
        auto queue_ptr = get_or_create_queue(symbol);
        bool enqueued = queue_ptr->try_enqueue(std::move(msg));
        // DEBUG
        size_t after_count = queue_count();
        if (after_count > before_count) {
            // std::cout << "[SymbolQueueRouter::push] New symbol added: '" << symbol << "'. Now "
                      // << after_count << " queues.\n";
        }
        return enqueued;
    }

    /**
     * @brief Thread-safe: Find the queue for a symbol, or nullptr if not found.
     * @param symbol    Symbol string (key).
     * @return Shared pointer to the queue, or nullptr if not found.
     */
    std::shared_ptr<Queue> find_queue(const SymbolId& symbol) const {
        std::lock_guard<std::mutex> lock(router_mutex_);
        auto it = queues_.find(symbol);
        if (it != queues_.end()) {
            return it->second;
        }
        std::cerr << "[SymbolQueueRouter::find_queue] NO queue for symbol=" << symbol << std::endl;
        return nullptr;
    }

    /**
     * @brief Get a copy of all known symbols in insertion order.
     * @return Vector of all registered symbol strings.
     */
    std::vector<SymbolId> get_symbol_list() const {
        std::lock_guard<std::mutex> lock(router_mutex_);
        return symbol_insertion_order_;
    }

    // Indexed (by insertion) vector of all queues.
    std::vector<std::shared_ptr<Queue>>& get_queue_vector() {
        return queue_vector_;
    }
    const std::vector<std::shared_ptr<Queue>>& get_queue_vector() const {
        return queue_vector_;
    }
    
    /**
     * @brief Get the number of unique symbol queues managed.
     * @return Number of unique symbols/queues.
     */
    size_t queue_count() const {
        std::lock_guard<std::mutex> lock(router_mutex_);
        return symbol_insertion_order_.size();
    }

private:
    /**
     * @brief Internal: Always returns a valid queue, creating it if it doesn't exist.
     * @param symbol    Symbol string (key).
     * @return Shared pointer to the queue.
     */
    std::shared_ptr<Queue> get_or_create_queue(const SymbolId& symbol) {
        {
            std::lock_guard<std::mutex> lock(router_mutex_);
            auto it = queues_.find(symbol);
            if (it != queues_.end()) {
                return it->second;
            }
        }
        // Acquire lock again to safely insert new queue if it was not found
        std::lock_guard<std::mutex> lock(router_mutex_);
        auto& queue_ptr = queues_[symbol];
        if (!queue_ptr) {
            queue_ptr = std::make_shared<Queue>(queue_capacity_);
            symbol_insertion_order_.push_back(symbol); // preserve insertion order for get_symbol_list
            queue_vector_.push_back(queue_ptr);
        } else {
            // std::cout << "[SymbolQueueRouter::get_or_create_queue] Queue for symbol=" << symbol << " already created by another thread (race condition)" << std::endl;
        }
        return queue_ptr;
    }

    mutable std::mutex router_mutex_; ///< Mutex protects queues_ and symbol_insertion_order_
    tsl::robin_map<SymbolId, std::shared_ptr<Queue>> queues_; ///< Map from symbol to concurrent queue
    std::vector<SymbolId> symbol_insertion_order_; ///< Insertion-order vector of symbol strings
    std::vector<std::shared_ptr<Queue>> queue_vector_;
    size_t queue_capacity_; ///< Per-queue capacity hint
};

#endif // SYMBOL_QUEUE_ROUTER_HPP_