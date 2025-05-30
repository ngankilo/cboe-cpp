//
// Created by phamanhtan on 30/5/25.
//

#ifndef LOGIN_RESPONSE_H
#define LOGIN_RESPONSE_H

#include "message.h"

namespace CboePitch {
    class LoginResponse final : public Message {
    public:
        static LoginResponse parse(const uint8_t *data, size_t length);

        std::string toString() const override;

        char getStatus() const { return status; }

    private:
        char status;

        explicit LoginResponse(char stat)
            : Message(0x02), status(stat) {
        }
    };
} // namespace CboePitch

#endif // LOGIN_RESPONSE_H
