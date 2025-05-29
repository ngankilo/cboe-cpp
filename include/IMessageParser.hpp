#pragma once
#include "IMessage.hpp"

class IMessageParser {
public:
    virtual ~IMessageParser() {}
    // Returns nullptr if parse failed.
    virtual IMessagePtr parse(const std::string& datagram) = 0;
};