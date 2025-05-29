#ifndef _LIBPITCH_TYPES_AUCTION_H
#define _LIBPITCH_TYPES_AUCTION_H
#include <cstdint>
#include <stdexcept>
namespace pitch::types
{

    /// @brief auction types
    enum auction_type
    {
        unknown_auction_type,
        opening_auction,
        closing_auction,
        halt_auction,
        ipo_auction,
        cboe_market_close,
        periodic_auction
    };

    /**
     * convert auction type field in stream into auction_type, for usage in
     * auction_summary and auction_update messages.
     * @param begin iterator to character
     * @throw std::invalid_argument if character is not 'A'/'H'/'Q'/'S'/'T'
     */
    template <typename T>
    auction_type get_auction_type(T begin)
    {
        switch (*begin)
        {
        case 'O':
            return auction_type::opening_auction;
        case 'C':
            return auction_type::closing_auction;
        case 'H':
            return auction_type::halt_auction;
        case 'I':
            return auction_type::ipo_auction;
        case 'M':
            return auction_type::cboe_market_close;
        case 'P':
            return auction_type::periodic_auction;
        default:
            throw std::invalid_argument("Valid auction type is 'O'/'C'/'H'/'I'/'M'/'P'");
        }
    };
}
#endif
