#ifndef _LIBPITCH_PITCH_TRADE_H
#define _LIBPITCH_PITCH_TRADE_H
#include <cstdint>
#include <string>
#include "message.h"
#include "../types/base.h"
#include "../types/side.h"
#include "../types/padded_string.h"
#include "common_macros.h"

namespace pitch::messages
{

    /**
     * Class representing trade message (message representing an executed trade)
     * that is not related to order previously recorded through add_order)
     *  
     *   -------------------- TRADE: Standard (short) format ------------------
     *   FIELD          OFFSET    LENGTH    DATA TYPE (cboe)    DATA TYPE (C++)
     *   timestamp      0         8         decimal             uint64_t
     *   type           8         1         alpha ("A")         message_type
     *   order_id       9         12        base36              uint64_t
     *   side           21        1         alpha ("B"/"S")     side_type
     *   shares         22        6         decimal             uint64_t
     *   stock_symbol   28        6         printable ascii     std::string
     *   price          34        10        price(decimal)      uint64_t
     *   execution_id   44        12        base36              uint64_t
     *   LF             56
     *
     *   ------------------ ADD ORDER: Extended (long) format -----------------
     *   FIELD          OFFSET    LENGTH    DATA TYPE (cboe)    DATA TYPE (C++)
     *   timestamp      0         8         decimal             uint64_t
     *   type           8         1         alpha ("d")         message_type
     *   order_id       9         12        base36              uint64_t
     *   side           21        1         alpha ("B"/"S")     side_type
     *   shares         22        6         decimal             uint64_t
     *   stock_symbol   28        8         printable ascii     std::string
     *   price          36        10        price(decimal)      uint64_t
     *   execution_id   46        12        base36              uint64_t
     *   LF             58
     *
     * Up to stock symbol offsets are equal; symbol length differs, and offset differ for price
     *
     * Source specification:
     * https://cdn.cboe.com/resources/membership/Cboe_US_Equities_TCP_PITCH_Specification.pdf
     * As of: March 25, 2022 (pages 10-11)
     */
    class trade : public message
    {
    public:
        trade(uint64_t timestamp,
                  uint64_t _order_id,
                  types::side_type _side,
                  uint64_t _shares,
                  std::string _symbol,
                  uint64_t _price,
                  uint64_t _execution_id) : message(timestamp),
                                     order_id(_order_id),
                                     side(_side),
                                     shares(_shares),
                                     symbol(_symbol),
                                     price(_price),
                                     execution_id(_execution_id) 

        {
        }

        // no setters, readonly properties once set

        message_type get_type() { return message_type::trade; }

        /**
         * Get id of an order (this will be cross-referenced when canceling or executing )*/
        uint64_t get_order_id() { return order_id; }

        /**
         * buy or sell */
        types::side_type get_side() { return side; }

        /**
         * number of shares executed
         */
        uint64_t get_shares() { return shares; }

        /**
         * stock symbol
         */
        const std::string &get_symbol() { return symbol; }

        /**
         * price in 1/10000th of dollars (so 213700 equals to $21.37)
         */
        uint64_t get_price() { return price; }

        /**
         * Get id of execution (don't confuse w/ order id)
         */
        uint64_t get_execution_id() { return execution_id; }

    private:
        uint64_t order_id;
        types::side_type side;
        uint64_t shares;
        std::string symbol;
        uint64_t price;
        uint64_t execution_id;
    };

    /**
     * Class implementing builder pattern for trade. All methods and data fields are private
     * as it is only meant to be called by friend pitch::decoder<T1>
    */
    template <typename T1>
    class _trade_decoder
    {
        private:
        
        template <typename T2>
        static T1 decode_message_short(T2 begin, T2 end, uint64_t timestamp)
        {
            if (end - begin != length_s)
                throw std::invalid_argument("expected length of " + std::to_string(length_s)
                + " for short format trade message");

            uint64_t order_id = pitch::types::get_base36(_OFFSET_PAIR(order_id_offset, order_id_length));
            types::side_type side = pitch::types::get_side(begin+side_offset);
            uint64_t shares = pitch::types::get_base<10>(_OFFSET_PAIR(shares_offset, shares_length));
            std::string symbol = pitch::types::get_padded_string(_OFFSET_PAIR(symbol_offset, symbol_length_s));
            uint64_t price = pitch::types::get_base<10>(_OFFSET_PAIR(price_offset_s, price_length));
            uint64_t execution_id = pitch::types::get_base36(_OFFSET_PAIR(execution_id_offset_s, execution_id_length));

            return T1(new trade(timestamp, order_id, side, shares, symbol, price, execution_id));
        }

        template <typename T2>
        static T1 decode_message_long(T2 begin, T2 end, uint64_t timestamp)
        {
            if (end - begin != length_l)
                throw std::invalid_argument("expected length of " + std::to_string(length_l)
                + " for long format trade message");

            uint64_t order_id = pitch::types::get_base36(_OFFSET_PAIR(order_id_offset, order_id_length));
            types::side_type side = pitch::types::get_side(begin+side_offset);
            uint64_t shares = pitch::types::get_base<10>(_OFFSET_PAIR(shares_offset, shares_length));
            std::string symbol = pitch::types::get_padded_string(_OFFSET_PAIR(symbol_offset, symbol_length_l));
            uint64_t price = pitch::types::get_base<10>(_OFFSET_PAIR(price_offset_l, price_length));
            uint64_t execution_id = pitch::types::get_base36(_OFFSET_PAIR(execution_id_offset_l, execution_id_length));

            return T1(new trade(timestamp, order_id, side, shares, symbol, price, execution_id));
        }

        static const off_t order_id_offset = 9;
        static const off_t order_id_length = 12;
        static const off_t side_offset = 21;
        static const off_t shares_offset = 22;
        static const off_t shares_length = 6;
        static const off_t symbol_offset = 28;
        static const off_t symbol_length_s = 6;
        static const off_t symbol_length_l = 8;
        static const off_t price_offset_s = 34;
        static const off_t price_offset_l = 36;
        static const off_t price_length = 10;
        static const off_t execution_id_offset_s = 44;
        static const off_t execution_id_offset_l = 46;
        static const off_t execution_id_length = 12;
        static const off_t length_s = 56;
        static const off_t length_l = 58;
        
        friend class pitch::decoder<T1>;
    };
}

#endif