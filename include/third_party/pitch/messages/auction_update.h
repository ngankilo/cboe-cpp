#ifndef _LIBPITCH_PITCH_AUCTION_UPDATE_H
#define _LIBPITCH_PITCH_AUCTION_UPDATE_H

#include "message.h"
#include "common_macros.h"
#include "../types/padded_string.h"
#include "../types/auction.h"

namespace pitch::messages
{

    /**
     * Class representing Auction Update message
     *
     *   --------------------------- AUCTION UPDATE ---------------------------
     *   FIELD           OFFSET    LENGTH    DATA TYPE (cboe)   DATA TYPE (C++)
     *   timestamp       0         8         decimal            uint64_t
     *   type            8         1         alpha ("I")        message_type
     *   stock_symbol    9         8         printable ascii    std::string
     *   auction_type    17        1         alpha (enum)       auction_type
     *   reference_price 18        10        decimal (price)    uint64_t
     *   buy_shares      28        10        decimal            uint64_t
     *   sell_shares     38        10        decimal            uint64_t
     *   indicative_price 48       10        decimal (price)    uint64_t
     *   LF              58
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
    class auction_update : public message
    {
    public:
        auction_update(uint64_t timestamp,
                       std::string _symbol,
                       types::auction_type _auction_type,
                       uint64_t _reference_price,
                       uint64_t _buy_shares,
                       uint64_t _sell_shares,
                       uint64_t _indicative_price,
                       uint64_t _auction_price) : message(timestamp),
                                                     symbol(_symbol),
                                                     auction_type(_auction_type),
                                                     reference_price(_reference_price),
                                                     buy_shares(_buy_shares),
                                                     sell_shares(_sell_shares),
                                                     indicative_price(_indicative_price),
                                                     auction_price(_auction_price)
        {
        }
        message_type get_type() { return message_type::auction_update; }

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
         * referece price
         */
        uint64_t get_reference_price() { return reference_price; }
        uint64_t get_buy_shares() { return buy_shares; }
        uint64_t get_sell_shares() { return sell_shares; }
        /**
         * for BYX it's filled with 0s
         */
        uint64_t get_indicative_price() { return indicative_price; }
        /**
         * Auction only price: Price at which the auction book
         * would match using only Eligible Auction Orders
         * 
         * for BYX it's filled with 0s
         */
        uint64_t get_auction_price() { return auction_price; }

    private:
        std::string symbol;
        types::auction_type auction_type;
        uint64_t reference_price;
        uint64_t buy_shares;
        uint64_t sell_shares;
        uint64_t indicative_price;
        uint64_t auction_price;
    };

    /**
     * Class implementing builder pattern for auction_update. All methods and data fields are private
     * as it is only meant to be called by friend pitch::decoder<T1>
     */
    template <typename T1>
    class _auction_update_decoder
    {
    private:
        template <typename T2>
        static T1 decode_message(T2 begin, T2 end, uint64_t timestamp)
        {
            if (end - begin != length)
                throw std::invalid_argument("expected length of " + std::to_string(length) + " for auction update message");
            std::string symbol = types::get_padded_string(_OFFSET_PAIR(symbol_offset, symbol_length));
            types::auction_type auction_type = types::get_auction_type(begin + auction_type_offset);
            uint64_t reference_price = types::get_base<10>(_OFFSET_PAIR(reference_price_offset, reference_price_length));
            uint64_t buy_shares = types::get_base<10>(_OFFSET_PAIR(buy_shares_offset, buy_shares_length));
            uint64_t sell_shares = types::get_base<10>(_OFFSET_PAIR(sell_shares_offset, sell_shares_length));
            uint64_t indicative_price = types::get_base<10>(_OFFSET_PAIR(indicative_price_offset, indicative_price_length));
            uint64_t auction_price = types::get_base<10>(_OFFSET_PAIR(auction_price_offset, auction_price_length));
            return T1(new auction_update(timestamp, symbol, auction_type, reference_price,
                                         buy_shares, sell_shares, indicative_price, auction_price));
        }

        static const off_t symbol_offset = 9;
        static const off_t symbol_length = 8;
        static const off_t auction_type_offset = 17;
        static const off_t reference_price_offset = 18;
        static const off_t reference_price_length = 10;
        static const off_t buy_shares_offset = 28;
        static const off_t buy_shares_length = 10;
        static const off_t sell_shares_offset = 38;
        static const off_t sell_shares_length = 10;
        static const off_t indicative_price_offset = 48;
        static const off_t indicative_price_length = 10;
        static const off_t auction_price_offset = 58;
        static const off_t auction_price_length = 10;
        static const off_t length = 68;

        friend class pitch::decoder<T1>;
    };
}

#endif