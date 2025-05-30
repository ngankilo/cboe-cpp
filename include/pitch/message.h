//
// Created by phamanhtan on 30/5/25.
//

#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>

namespace CboePitch {
    class Message {
    public:
        virtual ~Message() = default;

        virtual std::string toString() const = 0;

        virtual std::string getSymbol() const { return ""; }

        virtual void setSymbol(const std::string &symbol) {
        }

        uint8_t getMessageType() const { return messageType; }

    protected:
        uint8_t messageType;

        explicit Message(uint8_t type) : messageType(type) {
        }
    };
} // namespace CboePitch

#endif // MESSAGE_H
