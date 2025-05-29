//
// Created by phamanhtan on 29/5/25.
//

#ifndef END_OF_SESSION_H
#define END_OF_SESSION_H
#pragma once
#include <string>
#include <memory>
#include <stdexcept>

namespace CboePitch {

    class EndOfSession : public Message {
    private:
        std::string symbol;

    public:
        EndOfSession(const std::string& sym = "") : symbol(sym) {}

        static std::unique_ptr<EndOfSession> parse(const uint8_t* data, size_t size) {
            if (size < 6) throw std::invalid_argument("Invalid EndOfSession length");
            if (data[1] != 0x2D) throw std::invalid_argument("Invalid EndOfSession type");
            return std::make_unique<EndOfSession>();
        }

        char getMessageType() const override { return 0x2D; }
        std::string getSymbol() const override { return symbol; }
        void setSymbol(const std::string& sym) override {
            if (sym.size() > 6) throw std::invalid_argument("Symbol must be <= 6 characters");
            symbol = sym;
        }

        std::string toString() const override {
            return "EndOfSession: Symbol=" + symbol;
        }
    };

} // namespace CboePitch
#endif //END_OF_SESSION_H
