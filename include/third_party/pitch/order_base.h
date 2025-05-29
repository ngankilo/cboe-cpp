//
// Created by phamanhtan on 29/5/25.
//
#pragma once
#include <string>

namespace CboePitch {

    class Message {
    public:
        virtual ~Message() = default;
        virtual std::string getSymbol() const = 0;
        virtual void setSymbol(const std::string& sym) = 0;
        virtual char getMessageType() const = 0;
        virtual std::string toString() const = 0;
    };

} // namespace CboePitch
