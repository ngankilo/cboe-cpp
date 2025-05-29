#ifndef _LIBPITCH_PITCH_SYMBOL_CLEAR_H
#define _LIBPITCH_PITCH_SYMBOL_CLEAR_H

#include "message.h"
#include "common_macros.h"
#include "../types/padded_string.h"

namespace pitch::messages
{

    /**
     * Class representing Symbol Clear message
     *
     *   ---------------------------- SYMBOL CLEAR ----------------------------
     *   FIELD           OFFSET    LENGTH    DATA TYPE (cboe)   DATA TYPE (C++)
     *   timestamp       0         8         decimal            uint64_t
     *   type            8         1         alpha ("s")        message_type
     *   stock_symbol    9         8         printable ascii    std::string
     *   LF              17
     *
     * Note: there is no long and short version here, and length of symbol corresponds to
     * lopng version of add_order and trade 
     * 
     * Source specification:
     * https://cdn.cboe.com/resources/membership/Cboe_US_Equities_TCP_PITCH_Specification.pdf
     * As of: March 25, 2022 (page 6)
     */
    class symbol_clear : public message
    {
    public:
        symbol_clear(uint64_t timestamp, std::string _symbol) : message(timestamp),
                                                                symbol(_symbol) {}
        message_type get_type() { return message_type::symbol_clear; }

        /**
         * stock symbol
         */
        const std::string &get_symbol() { return symbol; }

    private:
        std::string symbol;
    };

    /**
     * Class implementing builder pattern for symbol_clear. All methods and data fields are private
     * as it is only meant to be called by friend pitch::decoder<T1>
     */
    template <typename T1>
    class _symbol_clear_decoder
    {
    private:
        template <typename T2>
        static T1 decode_message(T2 begin, T2 end, uint64_t timestamp)
        {
            if (end - begin != length)
                throw std::invalid_argument("expected length of " + std::to_string(length) + " for symbol clear message");
            std::string symbol = pitch::types::get_padded_string(_OFFSET_PAIR(symbol_offset, symbol_length));

            return T1(new symbol_clear(timestamp, symbol));
        }

        static const off_t symbol_offset = 9;
        static const off_t symbol_length = 8;
        static const off_t length = 17;

        friend class pitch::decoder<T1>;
    };
}

#endif