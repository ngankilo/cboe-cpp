#ifndef END_OF_SESSION_H
#define END_OF_SESSION_H

#include "message.h"
#include <string>
#include <stdexcept>

namespace CboePitch {
    class EndOfSession : public Message {
    public:
        static constexpr size_t MESSAGE_SIZE = 6;
        static constexpr uint8_t MESSAGE_TYPE = 0x2D;

        static EndOfSession parse(const uint8_t *data, size_t length, size_t offset = 0) {
            if (length < MESSAGE_SIZE) {
                throw std::invalid_argument("EndOfSession message too short");
            }
            // Có thể đọc thêm các trường nếu cần (hiện không có trường nào khác)
            return EndOfSession();
        }

        std::string toString() const override {
            return "EndOfSession{}";
        }

        size_t getMessageSize() const override { return MESSAGE_SIZE; }
        uint8_t getMessageType() const override { return MESSAGE_TYPE; }

    private:
        EndOfSession() = default;
    };
} // namespace CboePitch

#endif // END_OF_SESSION_H
