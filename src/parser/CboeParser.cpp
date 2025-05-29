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
        // Implement parsing logic here
        // Phân tích datagram theo định dạng <header>-<mstype>-<symbol>-<data>
        size_t pos1 = datagram.find('-');
        if (pos1 == std::string::npos) {
            return nullptr;
        }
        
        size_t pos2 = datagram.find('-', pos1 + 1);
        if (pos2 == std::string::npos) {
            return nullptr;
        }
        
        size_t pos3 = datagram.find('-', pos2 + 1);
        if (pos3 == std::string::npos) {
            return nullptr;
        }

        std::string header = datagram.substr(0, pos1);
        std::string mstype = datagram.substr(pos1 + 1, pos2 - pos1 - 1);
        std::string symbol = datagram.substr(pos2 + 1, pos3 - pos2 - 1);
        std::string data = datagram.substr(pos3 + 1);

        auto msg = std::make_shared<CboeMessage>();
        msg->setHeader(header);
        msg->setMsgType(mstype); 
        msg->setSymbol(symbol);
        msg->setData(data);
        return std::make_shared<CboeMessage>();
    }
};