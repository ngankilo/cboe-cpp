//
// Created by phamanhtan on 30/5/25.
//

#ifndef END_OF_SESSION_H
#define END_OF_SESSION_H

#include "message.h"

namespace CboePitch {
    class EndOfSession final : public Message {
    public:
        static EndOfSession parse(const uint8_t *data, size_t length);

        std::string toString() const override;

    private:
        EndOfSession() : Message(0x2D) {
        }
    };
} // namespace CboePitch

#endif // END_OF_SESSION_H
