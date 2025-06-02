//
// Created by phamanhtan on 30/5/25.
//

#include "pitch/gap_login.h"
#include <sstream>

namespace CboePitch {
    // GapLogin GapLogin::parse(const uint8_t *data, size_t length) {
    //     if (length < 14) throw std::runtime_error("GapLogin message too short");
    //     std::string sessionSubId(reinterpret_cast<const char *>(&data[0]), 4);
    //     std::string username(reinterpret_cast<const char *>(&data[4]), 4);
    //     std::string filler(reinterpret_cast<const char *>(&data[8]), 2);
    //     std::string password(reinterpret_cast<const char *>(&data[10]), 4);
    //     return GapLogin(sessionSubId, username, filler, password);
    // }

    // std::string GapLogin::toString() const {
    //     std::stringstream ss;
    //     ss << "GapLogin { SessionSubId: " << sessionSubId
    //             << ", Username: " << username
    //             << ", Filler: " << filler
    //             << ", Password: " << password << " }";
    //     return ss.str();
    // }
} // namespace CboePitch
