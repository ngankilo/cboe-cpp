#pragma once
#include "IMessage.hpp"
#include <string>

class Message : public IMessage {
public:
    Message(const std::string& header, int msgType, const std::string& symbol, const std::string& data)
        : header_(header), msgType_(msgType), symbol_(symbol), rawData_(data) {}
    
    std::string getSymbol() const override {
        return symbol_;
    }
    
    int getMsgType() const override {
        return msgType_;
    }
    
    const std::string& getRawData() const override {
        return rawData_;
    }
    
    const std::string& getHeader() const {
        return header_;
    }

private:
    std::string header_;
    int msgType_;
    std::string symbol_;
    std::string rawData_;
}; 