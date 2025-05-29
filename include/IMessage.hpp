#pragma once
#include <string>
#include <memory>

class IMessage {
public:
    virtual ~IMessage() {}
    virtual std::string getSymbol() const = 0;
    virtual int getMsgType() const = 0;
    virtual const std::string& getRawData() const = 0;
};
using IMessagePtr = std::shared_ptr<IMessage>;