#ifndef GAP_LOGIN_RESPONSE_H
#define GAP_LOGIN_RESPONSE_H

#include "message.h"
#include <stdexcept>

namespace CboePitch {
    class LoginResponse : public Message {
    public:
        static LoginResponse parse(const uint8_t* data, size_t size) {
            if (size < 1) throw std::invalid_argument("LoginResponse too short");
            char stat = data[0];
            return LoginResponse(stat);
        }

        std::string toString() const override {
            return "LoginResponse{status=" + std::string(1, status) + "}";
        }

        size_t getMessageSize() const override { return 1; }
        uint8_t getMessageType() const override { return 0x02; }

    private:
        char status;

        LoginResponse(char stat) : status(stat) {}
    };
} // namespace CboePitch

#endif // GAP_LOGIN_RESPONSE_H