#pragma once
#include <unordered_map>
#include <queue>
#include <mutex>
#include <memory>
#include <thread>
#include <condition_variable>
#include "IMessage.hpp"
#include "IMessageHandler.hpp"

class Dispatcher {
public:
    Dispatcher(std::shared_ptr<IMessageHandler> handler, size_t thread_num = std::thread::hardware_concurrency());
    void dispatch(const IMessagePtr& msg);
    void start();
    void stop();

private:
    struct SymbolQueue {
        std::queue<IMessagePtr> queue;
        bool busy = false;
        std::mutex mtx;
    };

    std::unordered_map<std::string, std::shared_ptr<SymbolQueue>> symbol_queues_;
    std::mutex queues_mtx_;

    std::vector<std::thread> workers_;
    std::condition_variable cv_;
    std::mutex cv_mtx_;
    bool stopping_ = false;
    std::shared_ptr<IMessageHandler> handler_;

    void workerThread();
};