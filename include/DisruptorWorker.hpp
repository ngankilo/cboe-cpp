/**
 * @file    DisruptorWorker.hpp
 * @brief   Consumer thread for a Disruptor ring buffer pipeline.
 *
 * Developer: Hoang Nguyen & Tan A. Pham
 * Copyright: Equix Technologies Pty Ltd (contact@equix.com.au)
 * Filename: DisruptorWorker.hpp
 * Created: 28/May/2025
 *
 * Description:
 *   Template class that launches a background thread to consume from a ring buffer,
 *   invoking a user handler on each event. Uses disruptorplus strategies for low-latency.
 */

#pragma once

#include <cstdint>
#include <disruptorplus/ring_buffer.hpp>
#include <disruptorplus/single_threaded_claim_strategy.hpp>
#include <disruptorplus/spin_wait_strategy.hpp>
#include <disruptorplus/sequence_barrier.hpp>
#include <thread>
#include <atomic>
#include <functional>
#include <memory>
#include <iostream>

namespace disruptor_pipeline {

/**
 * @class DisruptorWorker
 * @brief Consumes events from a ring buffer and calls a user handler in a dedicated thread.
 *
 * @tparam Event  Type of event to process.
 */
template<typename Event>
class DisruptorWorker {
public:
    using Handler           = std::function<void(const Event&)>;
    using RingBuffer        = disruptorplus::ring_buffer<Event>;
    using ClaimStrategy     = disruptorplus::single_threaded_claim_strategy<disruptorplus::spin_wait_strategy>;
    using SequenceBarrier   = disruptorplus::sequence_barrier<disruptorplus::spin_wait_strategy>;
    using SequenceIndex     = disruptorplus::sequence_t;

    /**
     * @brief Construct and start a worker.
     * @param ring_buffer         Reference to the ring buffer of events.
     * @param claim_strategy      Reference to the claim strategy.
     * @param sequence_barrier    Reference to the consumer sequence barrier.
     * @param handler             Function to call for each consumed event.
     */
    DisruptorWorker(RingBuffer& ring_buffer, ClaimStrategy& claim_strategy,
                    SequenceBarrier& sequence_barrier, Handler handler)
        : ring_buffer_(ring_buffer), claim_strategy_(claim_strategy),
          sequence_barrier_(sequence_barrier), handler_(std::move(handler)),
          stop_flag_(false),
          worker_thread_([this] { this->run(); })
    {}

    /**
     * @brief Destructor. Signals worker to stop and waits for thread to finish.
     */
    ~DisruptorWorker() {
        stop_flag_ = true;
        if (worker_thread_.joinable())
            worker_thread_.join();
    }

private:
    /**
     * @brief Main loop: consume and process available events in sequence.
     */
    void run() {
        SequenceIndex next_to_read = 0;
        while (!stop_flag_) {
            SequenceIndex available_seq = claim_strategy_.wait_until_published(next_to_read);

            do {
                handler_(ring_buffer_[next_to_read]);
            } while (next_to_read++ != available_seq);

            sequence_barrier_.publish(available_seq);
        }
        std::cerr << "[DEBUG] DisruptorWorker thread exiting run() loop." << std::endl;
    }

    RingBuffer&        ring_buffer_;
    ClaimStrategy&     claim_strategy_;
    SequenceBarrier&   sequence_barrier_;
    Handler            handler_;
    std::atomic<bool>  stop_flag_;
    std::thread        worker_thread_;
};

} // namespace disruptor_pipeline
