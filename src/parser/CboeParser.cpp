#include "IMessageParser.hpp"
#include "IMessage.hpp"
#include <memory>

class CboeMessage : public IMessage {
public:
    // implement members, parsing etc
};

class CboeParser : public IMessageParser {
public:
    IMessagePtr parse(const std::string& datagram) override {
        // Parse datagram and return a CboeMessage instance
    }
};