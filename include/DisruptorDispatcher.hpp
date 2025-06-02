/**
 * @file    DisruptorDispatcher.hpp
 * @brief   Threaded dispatcher to transfer from symbol queues into a disruptor ring buffer.
 *
 * Developer: Hoang Nguyen & Tan A. Pham
 * Copyright: Equix Technologies Pty Ltd (contact@equix.com.au)
 * Filename: DisruptorDispatcher.hpp
 * Created: 28/May/2025
 *
 * Description:
 *   Consumes events from multiple concurrent queues (one per symbol),
 *   and routes them into a disruptor ring buffer pipeline using a background thread.
 *   Designed for fast, partitioned, symbol-based ingestion.
 */

#pragma once

#ifndef DISRUPTOR_DISPATCHER_HPP_
#define DISRUPTOR_DISPATCHER_HPP_

#include <vector>
#include <thread>
#include <atomic>
#include "DisruptorRouter.hpp"
#include "concurrent_queue/concurrentqueue.h"

/**
 * @class DisruptorDispatcher
 * @brief Moves events from assigned symbol queues into the disruptor ring buffer pipeline.
 *
 * @tparam Event  Type of event stored in the ring buffer and queues.
 */
template<typename Event>
class DisruptorDispatcher {
public:
    using SymbolQueue = moodycamel::ConcurrentQueue<Event>;

    /**
     * @brief Constructs and starts a dispatcher thread.
     * @param assigned_indices   List of indices in symbol_queues for this worker.
     * @param symbol_queues      Reference to vector of per-symbol queues.
     * @param router             Reference to the disruptor router.
     */
    DisruptorDispatcher(
        std::vector<size_t> assigned_indices,
        std::vector<std::unique_ptr<SymbolQueue>>& symbol_queues,
        disruptor_pipeline::DisruptorRouter<Event>& router)
        : assigned_indices_(std::move(assigned_indices)),
          symbol_queues_(symbol_queues),
          router_(router),
          is_running_(true),
          dispatcher_thread_([this] { this->run(); })
    {}

    /**
     * @brief Destructor. Signals dispatcher to stop and waits for thread exit.
     */
    ~DisruptorDispatcher() {
        is_running_ = false;
        if (dispatcher_thread_.joinable()) dispatcher_thread_.join();
    }

private:
    /**
     * @brief Main dispatcher loop: consumes from symbol queues and routes to ring buffer.
     */
    void run() {
        while (is_running_) {
            for (size_t symbol_idx : assigned_indices_) {
                Event event;
                // Drain the queue for this symbol and publish to ring buffer
                while (symbol_queues_[symbol_idx]->try_dequeue(event)) {
                    disruptorplus::sequence_t seq;
                    Event& slot = router_.claim_and_get_slot(seq);
                    slot = std::move(event);
                    router_.publish(seq);
                }
            }
            // Yield or sleep for fairness/tuning
            std::this_thread::yield();
        }
    }

    std::vector<size_t> assigned_indices_;                  ///< Assigned indices for this worker
    std::vector<std::unique_ptr<SymbolQueue>>& symbol_queues_; ///< Reference to vector of symbol queues
    disruptor_pipeline::DisruptorRouter<Event>& router_;    ///< Reference to disruptor router
    std::atomic<bool> is_running_;                          ///< Running flag for thread loop
    std::thread dispatcher_thread_;                         ///< Background thread for dispatching
};

#endif // DISRUPTOR_DISPATCHER_HPP_
