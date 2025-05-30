//
// Created by phamanhtan on 30/5/25.
//

#ifndef GAP_LOGIN_H
#define GAP_LOGIN_H

#include "message.h"

namespace CboePitch {
    class GapLogin final : public Message {
    public:
        static GapLogin parse(const uint8_t *data, size_t length);

        std::string toString() const override;

        std::string getSessionSubId() const { return sessionSubId; }
        std::string getUsername() const { return username; }
        std::string getPassword() const { return password; }

    private:
        std::string sessionSubId;
        std::string username;
        std::string filler;
        std::string password;

        GapLogin(const std::string &subId, const std::string &user, const std::string &fill,
                 const std::string &pass)
            : Message(0x01), sessionSubId(subId), username(user), filler(fill), password(pass) {
        }
    };
} // namespace CboePitch

#endif // GAP_LOGIN_H
