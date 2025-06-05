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
#include <functional>
#include <iostream>
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
     * @param id                Dispatcher thread index (0 <= id < num_dispatchers)
     * @param num_dispatchers   Total dispatcher threads
     * @param symbol_queues     Reference to all symbol queues (could grow!)
     * @param router            Reference to disruptor router
     */
    DisruptorDispatcher(
        size_t id,
        size_t num_dispatchers,
        std::vector<std::shared_ptr<SymbolQueue>>& symbol_queues,
        disruptor_pipeline::DisruptorRouter<Event>& router)
        : id_(id), num_dispatchers_(num_dispatchers),
          symbol_queues_(symbol_queues), router_(router),
          is_running_(true),
          dispatcher_thread_([this] { this->run(); })
    {
        // std::cout << "[DispatcherSetup] Thread " << id
        //           << " of " << num_dispatchers_
        //           << " started.\n";
    }

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
        // std::cout << "[Dispatcher] Thread " << id_ << " starting (handles any queue idx where idx % "
                  // << num_dispatchers_ << " == " << id_ << ")\n";
        while (is_running_) {
            auto N = symbol_queues_.size(); // dynamically changes as new symbols arrive!
            for (size_t idx = 0; idx < N; ++idx) {
                if ((idx % num_dispatchers_) != id_) continue;
                auto& qptr = symbol_queues_[idx];
                if (!qptr) continue; // safety
                Event event;
                size_t drained = 0;
                while (qptr->try_dequeue(event)) {
                    disruptorplus::sequence_t seq;
                    Event& slot = router_.claim_and_get_slot(seq);
                    slot = std::move(event);
                    router_.publish(seq);
                    ++drained;
                }
                if (drained) {
                    // std::cout << "[Dispatcher] Thread " << id_ << " dequeued " << drained
                              // << " message(s) for queue #" << idx << std::endl;
                }
            }
            std::this_thread::yield();
        }
        // std::cout << "[Dispatcher] Thread " << id_ << " exiting\n";
    }

    size_t id_;
    size_t num_dispatchers_;
    std::vector<std::shared_ptr<SymbolQueue>>& symbol_queues_;
    disruptor_pipeline::DisruptorRouter<Event>& router_;
    std::atomic<bool> is_running_;
    std::thread dispatcher_thread_;
};

#endif // DISRUPTOR_DISPATCHER_HPP_