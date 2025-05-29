#pragma once
#include <memory>
#include "IMessageParser.hpp"
#include "IMessageHandler.hpp"
// You can extend this for pluggable message types.

class MessageFactory {
public:
    static std::unique_ptr<IMessageParser> createParser(const std::string& type);
    static std::unique_ptr<IMessageHandler> createHandler(const std::string& type);
};