#ifndef _SYMBOL_MAP_HPP_
#define _SYMBOL_MAP_HPP_

#pragma once
#include <unordered_map>
#include <deque>
#include <shared_mutex>
#include "IMessage.hpp"

class SymbolMap {
    std::unordered_map<std::string, std::deque<IMessagePtr>> m_;
    std::shared_mutex mtx_;
public:
    void push(const IMessagePtr& msg);
    IMessagePtr pop(const std::string& symbol);
    std::vector<std::string> nonempty_symbols();
};

#endif