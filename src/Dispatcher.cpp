#include "Dispatcher.hpp"
#include <algorithm>
#include <iostream>

Dispatcher::Dispatcher(std::shared_ptr<IMessageHandler> handler, size_t thread_num)
    : handler_(handler) {
    workers_.reserve(thread_num);
    for (size_t i = 0; i < thread_num; ++i) {
        workers_.emplace_back(&Dispatcher::workerThread, this);
    }
}

void Dispatcher::dispatch(const IMessagePtr& msg) {
    std::string symbol = msg->getSymbol();
    
    std::lock_guard<std::mutex> lock(queues_mtx_);
    
    // Create or get symbol queue
    auto& symbol_queue = symbol_queues_[symbol];
    if (!symbol_queue) {
        symbol_queue = std::make_shared<SymbolQueue>();
    }
    
    // Add message to symbol-specific queue
    {
        std::lock_guard<std::mutex> queue_lock(symbol_queue->mtx);
        symbol_queue->queue.push(msg);
    }
    
    // Notify workers
    cv_.notify_one();
}

void Dispatcher::start() {
    // Workers are already started in constructor
}

void Dispatcher::stop() {
    {
        std::lock_guard<std::mutex> lock(cv_mtx_);
        stopping_ = true;
    }
    cv_.notify_all();
    
    for (auto& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void Dispatcher::workerThread() {
    while (true) {
        std::shared_ptr<SymbolQueue> available_queue;
        IMessagePtr msg;
        
        // Find an available queue with messages
        {
            std::unique_lock<std::mutex> lock(cv_mtx_);
            cv_.wait(lock, [this] {
                if (stopping_) return true;
                
                std::lock_guard<std::mutex> queues_lock(queues_mtx_);
                for (auto& [symbol, queue] : symbol_queues_) {
                    std::lock_guard<std::mutex> queue_lock(queue->mtx);
                    if (!queue->busy && !queue->queue.empty()) {
                        return true;
                    }
                }
                return false;
            });
            
            if (stopping_) break;
            
            // Find and claim an available queue
            std::lock_guard<std::mutex> queues_lock(queues_mtx_);
            for (auto& [symbol, queue] : symbol_queues_) {
                std::lock_guard<std::mutex> queue_lock(queue->mtx);
                if (!queue->busy && !queue->queue.empty()) {
                    queue->busy = true;
                    msg = queue->queue.front();
                    queue->queue.pop();
                    available_queue = queue;
                    break;
                }
            }
        }
        
        if (available_queue && msg) {
            try {
                // Process message
                handler_->handle(msg);
            } catch (const std::exception& e) {
                std::cerr << "Error processing message: " << e.what() << std::endl;
            }
            
            // Mark queue as available
            {
                std::lock_guard<std::mutex> queue_lock(available_queue->mtx);
                available_queue->busy = false;
            }
            
            // Notify other workers if there are more messages in this queue
            {
                std::lock_guard<std::mutex> queue_lock(available_queue->mtx);
                if (!available_queue->queue.empty()) {
                    cv_.notify_one();
                }
            }
        }
    }
}
