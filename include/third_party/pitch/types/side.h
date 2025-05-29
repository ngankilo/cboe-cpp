#ifndef _LIBPITCH_TYPES_SIDE_H
#define _LIBPITCH_TYPES_SIDE_H
#include <cstdint>
#include <stdexcept>
namespace pitch::types
{

    /// @brief enum to differentiate buy from sell orders
    enum side_type
    {
        unknown_side,
        sell,
        buy
    };

    /**
     * convert side field in stream into side_type, for usage in trade and add_order
     * messages.
     * @param begin iterator to character
     * @throw std::invalid_argument if character is not 'S' or 'B'
     */
    template <typename T>
    side_type get_side(T begin)
    {
        if (*begin == 'S')
            return side_type::sell;
        else if (*begin == 'B')
            return side_type::buy;
        else
            throw std::invalid_argument("Valid side is either 'S' or 'B'");
    };
}
#endif
