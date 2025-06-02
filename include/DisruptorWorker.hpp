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

namespace disruptor_pipeline {

template<typename Event>
class DisruptorWorker {
public:
    using Handler = std::function<void(const Event&)>;
    using ring_buffer_t = disruptorplus::ring_buffer<Event>;
    using claim_strategy_t = disruptorplus::single_threaded_claim_strategy<disruptorplus::spin_wait_strategy>;
    using sequence_barrier_t = disruptorplus::sequence_barrier<disruptorplus::spin_wait_strategy>;
    using sequence_t = disruptorplus::sequence_t;

    DisruptorWorker(ring_buffer_t& buffer, claim_strategy_t& claim_strat, 
                    sequence_barrier_t& consumer_barrier, Handler handler)
    : _buffer(buffer), _claim_strat(claim_strat),
      _consumer_barrier(consumer_barrier),
      _handler(handler),
      _thread([this] { run(); })
    {}

    ~DisruptorWorker() {
        _stop = true;
        if (_thread.joinable())
            _thread.join();
    }

private:
    void run() {
        sequence_t nextToRead = 0;
        while (!_stop) {
            sequence_t available = _claim_strat.wait_until_published(nextToRead);

            do {
                _handler(_buffer[nextToRead]);
            } while (nextToRead++ != available);

            _consumer_barrier.publish(available);
        }
    }

    ring_buffer_t& _buffer;
    claim_strategy_t& _claim_strat;
    sequence_barrier_t& _consumer_barrier;
    Handler _handler;
    std::atomic<bool> _stop{false};
    std::thread _thread;
};

} // namespace disruptor_pipeline