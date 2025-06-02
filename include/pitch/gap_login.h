#ifndef GAP_LOGIN_H
#define GAP_LOGIN_H

#include "message.h"
#include <string>
#include <stdexcept>

namespace CboePitch {
    class GapLogin : public Message {
    public:
        static GapLogin parse(const uint8_t* data, size_t size) {
            if (size < 14) throw std::invalid_argument("GapLogin too short");
            std::string subId(reinterpret_cast<const char*>(&data[1]), 4);
            std::string user(reinterpret_cast<const char*>(&data[5]), 4);
            char fill = data[9];
            std::string pass(reinterpret_cast<const char*>(&data[10]), 4);
            return GapLogin(subId, user, fill, pass);
        }

        std::string toString() const override {
            return "GapLogin{sessionSubId=" + sessionSubId + ", username=" + username +
                   ", filler=" + filler + ", password=" + password + "}";
        }

        size_t getMessageSize() const override { return 14; }
        uint8_t getMessageType() const override { return 0x01; }

    private:
        std::string sessionSubId;
        std::string username;
        char filler;
        std::string password;

        GapLogin(const std::string& subId, const std::string& user, char fill, const std::string& pass)
            : sessionSubId(subId), username(user), filler(fill), password(pass) {}
    };
} // namespace CboePitch

#endif // GAP_LOGIN_H