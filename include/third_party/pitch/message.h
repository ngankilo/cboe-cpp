#pragma once
#include <string>

namespace CboePitch {

    // Base class for PITCH messages
    class Message {
    public:
        virtual ~Message() = default;
        virtual std::string toString() const = 0;
        virtual char getMessageType() const = 0;
        virtual std::string getSymbol() const { return ""; }
        virtual void setSymbol(const std::string&) { throw std::invalid_argument("Symbol not supported"); }
    };

} // namespace CboePitch