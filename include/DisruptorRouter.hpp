#pragma once
#include "DisruptorWorker.hpp"

namespace disruptor_pipeline {

template<typename Event>
class DisruptorRouter {
public:
    using ring_buffer_t = typename DisruptorWorker<Event>::ring_buffer_t;
    using claim_strategy_t = typename DisruptorWorker<Event>::claim_strategy_t;
    using sequence_barrier_t = typename DisruptorWorker<Event>::sequence_barrier_t;
    using Handler = typename DisruptorWorker<Event>::Handler;
    using sequence_t = disruptorplus::sequence_t;

    DisruptorRouter(size_t bufferSize, Handler handler)
    : _buffer(bufferSize),
      _waitStrategy(),
      _claimStrategy(bufferSize, _waitStrategy),
      _barrier(_waitStrategy),
      _worker(_buffer, _claimStrategy, _barrier, handler)
    {
        _claimStrategy.add_claim_barrier(_barrier);
    }

    // Enqueue a new event (returns reference so you can fill it before publishing!)
    Event& claim_and_get_slot(sequence_t& out_seq) {
        out_seq = _claimStrategy.claim_one();
        return _buffer[out_seq];
    }

    void publish(sequence_t seq) {
        _claimStrategy.publish(seq);
    }

private:
    ring_buffer_t _buffer;
    disruptorplus::spin_wait_strategy _waitStrategy;
    claim_strategy_t _claimStrategy;
    sequence_barrier_t _barrier;
    DisruptorWorker<Event> _worker;
};

} // namespace disruptor_pipeline