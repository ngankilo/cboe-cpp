#ifndef _LIBPITCH_PITCH_AUCTION_SUMMARY_H
#define _LIBPITCH_PITCH_AUCTION_SUMMARY_H

#include "message.h"
#include "common_macros.h"
#include "../types/padded_string.h"
#include "../types/auction.h"

namespace pitch::messages
{

    /**
     * Class representing Auction Summary message
     *
     *   --------------------------- AUCTION SUMMARY ---------------------------
     *   FIELD           OFFSET    LENGTH    DATA TYPE (cboe)   DATA TYPE (C++)
     *   timestamp       0         8         decimal            uint64_t
     *   type            8         1         alpha ("J")        message_type
     *   stock_symbol    9         8         printable ascii    std::string
     *   auction_type    17        1         alpha (enum)       auction_type
     *   price           18        10        decimal (price)    uint64_t
     *   shares          28        10        decimal            uint64_t
     *   LF              48
     *
     * Note: there is no long and short version here, and length of symbol corresponds to
     * lopng version of add_order and trade. There are different sets of auction types for
     * BZX and BYX exchanges, but overall structure (lengths and offsets of fields) of message
     * is the same for every US CBOE equities exchange, so no need to differentiate at code
     * level
     *
     * Source specification:
     * https://cdn.cboe.com/resources/membership/Cboe_US_Equities_TCP_PITCH_Specification.pdf
     * As of: March 25, 2022 (pages 14-15)
     */
    class auction_summary : public message
    {
    public:
        auction_summary(uint64_t timestamp,
                        std::string _symbol,
                        types::auction_type _auction_type,
                        uint64_t _price,
                        uint64_t _shares) : message(timestamp),
                                            symbol(_symbol),
                                            auction_type(_auction_type),
                                            price(_price),
                                            shares(_shares)
        {
        }
        message_type get_type() { return message_type::auction_summary; }

        /**
         * stock symbol
         */
        const std::string &get_symbol() { return symbol; }
        /**
         * Auction type
         * For BZX: opening auction, closing auction, halt auction, IPO auction, Cboe market close
         * For BYX: periodic auction
         */
        types::auction_type get_auction_type() { return auction_type; }

        /**
         * auction price
         */
        uint64_t get_price() { return price; }
        /**
         * cumulative number of shares executed during an auction
         */
        uint64_t get_shares() { return shares; }

    private:
        std::string symbol;
        types::auction_type auction_type;
        uint64_t price;
        uint64_t shares;
    };

    /**
     * Class implementing builder pattern for auction_summary. All methods and data fields are private
     * as it is only meant to be called by friend pitch::decoder<T1>
     */
    template <typename T1>
    class _auction_summary_decoder
    {
    private:
        template <typename T2>
        static T1 decode_message(T2 begin, T2 end, uint64_t timestamp)
        {
            if (end - begin != length)
                throw std::invalid_argument("expected length of " + std::to_string(length) + " for auction summary message");
            std::string symbol = types::get_padded_string(_OFFSET_PAIR(symbol_offset, symbol_length));
            types::auction_type auction_type = types::get_auction_type(begin + auction_type_offset);
            uint64_t price = types::get_base<10>(_OFFSET_PAIR(price_offset, price_length));
            uint64_t shares = types::get_base<10>(_OFFSET_PAIR(shares_offset, shares_length));
            return T1(new auction_summary(timestamp, symbol, auction_type, 
            price, shares));
        }

        static const off_t symbol_offset = 9;
        static const off_t symbol_length = 8;
        static const off_t auction_type_offset = 17;
        static const off_t price_offset = 18;
        static const off_t price_length = 10;
        static const off_t shares_offset = 28;
        static const off_t shares_length = 10;
        static const off_t length = 38;

        friend class pitch::decoder<T1>;
    };
}

#endif