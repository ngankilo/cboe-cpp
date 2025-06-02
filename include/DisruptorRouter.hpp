/**
 * @file    DisruptorRouter.hpp
 * @brief   Simple claim/publish wrapper for a Disruptor ring buffer pipeline.
 *
 * Developer: Hoang Nguyen & Tan A. Pham
 * Copyright: Equix Technologies Pty Ltd (contact@equix.com.au)
 * Filename: DisruptorRouter.hpp
 * Created: 28/May/2025
 *
 * Description:
 *   Provides an interface for claim/fill/publish pattern over a disruptor ring buffer.
 *   Used as a producer-side helper for efficient event publishing.
 */

#pragma once

#include "DisruptorWorker.hpp"

namespace disruptor_pipeline {

/**
 * @class DisruptorRouter
 * @brief Claim/fill/publish interface for a Disruptor pipeline ring buffer.
 * @tparam Event  Type of event stored in the ring buffer.
 */
template<typename Event>
class DisruptorRouter {
public:
    using RingBuffer        = typename DisruptorWorker<Event>::RingBuffer;
    using ClaimStrategy     = typename DisruptorWorker<Event>::ClaimStrategy;
    using SequenceBarrier   = typename DisruptorWorker<Event>::SequenceBarrier;
    using Handler           = typename DisruptorWorker<Event>::Handler;
    using SequenceIndex     = disruptorplus::sequence_t;

    /**
     * @brief Construct a router with buffer size and event handler.
     * @param buffer_size   Number of slots in the ring buffer.
     * @param handler       Function to call for each processed event.
     */
    DisruptorRouter(size_t buffer_size, Handler handler)
        : ring_buffer_(buffer_size),
          wait_strategy_(),
          claim_strategy_(buffer_size, wait_strategy_),
          sequence_barrier_(wait_strategy_),
          worker_(ring_buffer_, claim_strategy_, sequence_barrier_, std::move(handler))
    {
        claim_strategy_.add_claim_barrier(sequence_barrier_);
    }

    /**
     * @brief Claim a slot in the ring buffer and get reference for filling.
     * @param[out] out_seq  Will be set to the claimed sequence index.
     * @return Reference to the event in the claimed slot.
     */
    Event& claim_and_get_slot(SequenceIndex& out_seq) {
        out_seq = claim_strategy_.claim_one();
        return ring_buffer_[out_seq];
    }

    /**
     * @brief Publish an event at the specified sequence index.
     * @param seq  Sequence index to publish.
     */
    void publish(SequenceIndex seq) {
        claim_strategy_.publish(seq);
    }

private:
    RingBuffer        ring_buffer_;
    disruptorplus::spin_wait_strategy wait_strategy_;
    ClaimStrategy     claim_strategy_;
    SequenceBarrier   sequence_barrier_;
    DisruptorWorker<Event> worker_;
};

} // namespace disruptor_pipeline
