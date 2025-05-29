#ifndef _LIBPITCH_PITCH_TRADE_BREAK_H
#define _LIBPITCH_PITCH_TRADE_BREAK_H

#include "message.h"
#include "common_macros.h"

namespace pitch::messages
{

    /**
     * Class representing trade break message
     * 
     *   ----------------------------- TRADE BREAK -----------------------------
     *   FIELD           OFFSET    LENGTH    DATA TYPE (cboe)   DATA TYPE (C++)
     *   timestamp       0         8         decimal            uint64_t
     *   type            8         1         alpha ("B")        message_type
     *   execution_id    9         12        base36             uint64_t
     *   LF              21
     *
     * Source specification:
     * https://cdn.cboe.com/resources/membership/Cboe_US_Equities_TCP_PITCH_Specification.pdf
     * As of: March 25, 2022 (page 12)
     */
    class trade_break : public message
    {
    public:
        trade_break(uint64_t timestamp,
                  uint64_t _execution_id) : message(timestamp),
                                     execution_id(_execution_id) {}


        message_type get_type() { return message_type::trade_break; }

        /**
         * Get id of execution (matching to execution id on trade or executed message)
         */
        uint64_t get_execution_id() { return execution_id; }

        private:
        uint64_t execution_id;
    };


    /**
     * Class implementing builder pattern for trade_break. All methods and data fields are private
     * as it is only meant to be called by friend pitch::decoder<T1>
    */
    template <typename T1>
    class _trade_break_decoder
    {
    private:
        template <typename T2>
        static T1 decode_message(T2 begin, T2 end, uint64_t timestamp)
        {
            if (end - begin != length)
                throw std::invalid_argument("expected length of " + std::to_string(length)
                + " for trade break message");

            uint64_t execution_id = pitch::types::get_base36(_OFFSET_PAIR(execution_id_offset, execution_id_length));

            return T1(new trade_break(timestamp, execution_id));
        }
        static const off_t execution_id_offset = 9;
        static const off_t execution_id_length = 12;
        static const off_t length = 21;

        friend class pitch::decoder<T1>;
    };
}

#endif