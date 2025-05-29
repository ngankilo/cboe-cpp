#ifndef _LIBPITCH_PITCH_RETAIL_PRICE_IMPROVEMENT_H
#define _LIBPITCH_PITCH_RETAIL_PRICE_IMPROVEMENT_H

#include "message.h"
#include "common_macros.h"
#include "../types/padded_string.h"
#include "../types/rpi.h"

namespace pitch::messages
{

    /**
     * Class representing Retail Price Improvement message
     *
     *   ----------------------- RETAIL PRICE IMPROVEMENT ----------------------
     *   FIELD           OFFSET    LENGTH    DATA TYPE (cboe)   DATA TYPE (C++)
     *   timestamp       0         8         decimal            uint64_t
     *   type            8         1         alpha ("R")        message_type
     *   stock_symbol    9         8         printable ascii    std::string
     *   rpi             17        1         alpha (enum)       rpi_type
     *   LF              18
     *
     * Note: there is no long and short version here, and length of symbol corresponds to
     * lopng version of add_order and trade
     *
     * Source specification:
     * https://cdn.cboe.com/resources/membership/Cboe_US_Equities_TCP_PITCH_Specification.pdf
     * As of: March 25, 2022 (page 17)
     */
    class retail_price_improvement : public message
    {
    public:
        retail_price_improvement(uint64_t timestamp,
                                 std::string _symbol,
                                 types::rpi_type _rpi_type) : message(timestamp),
                                                        symbol(_symbol),
                                                        rpi_type(_rpi_type) {}
        message_type get_type() { return message_type::retail_price_improvement; }

        /**
         * stock symbol
         */
        const std::string &get_symbol() { return symbol; }
        /**
         * Type of retail price improvement for a given symbol
         */
        types::rpi_type get_rpi_type() { return rpi_type; }

    private:
        std::string symbol;
        types::rpi_type rpi_type;
    };

    /**
     * Class implementing builder pattern for retail_price_improvement. All methods and data fields are private
     * as it is only meant to be called by friend pitch::decoder<T1>
     */
    template <typename T1>
    class _retail_price_improvement_decoder
    {
    private:
        template <typename T2>
        static T1 decode_message(T2 begin, T2 end, uint64_t timestamp)
        {
            if (end - begin != length)
                throw std::invalid_argument("expected length of " + std::to_string(length) 
                + " for retail price improvement message");
            std::string symbol = pitch::types::get_padded_string(_OFFSET_PAIR(symbol_offset, symbol_length));
            types::rpi_type rpi_type = pitch::types::get_rpi_type(begin + rpi_offset);
            return T1(new retail_price_improvement(timestamp, symbol, rpi_type));
        }

        static const off_t symbol_offset = 9;
        static const off_t symbol_length = 8;
        static const off_t rpi_offset = 17;
        static const off_t length = 18;

        friend class pitch::decoder<T1>;
    };
}

#endif