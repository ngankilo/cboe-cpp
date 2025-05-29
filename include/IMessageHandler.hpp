#pragma once
#include "IMessage.hpp"

class IMessageHandler {
public:
    virtual ~IMessageHandler() {}
    virtual void handle(const IMessagePtr& msg) = 0;
};