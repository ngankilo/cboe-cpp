#ifndef _LIBPITCH_TYPES_RPI_H
#define _LIBPITCH_TYPES_RPI_H
#include <cstdint>
#include <stdexcept>
namespace pitch::types
{

    /// @brief rpi types
    enum rpi_type
    {
        unknown_rpi,
        buy_side,
        sell_side,
        buy_and_sell_side,
        no_rpi
    };

    /**
     * convert rpi type field in stream into rpi_type, for usage in
     * retail_price_improvement messages.
     * @param begin iterator to character
     * @throw std::invalid_argument if character is not 'A'/'H'/'Q'/'S'/'T'
     */
    template <typename T>
    rpi_type get_rpi_type(T begin)
    {
        switch (*begin)
        {
        case 'B':
            return rpi_type::buy_side;
        case 'S':
            return rpi_type::sell_side;
        case 'A':
            return rpi_type::buy_and_sell_side;
        case 'N':
            return rpi_type::no_rpi;
        default:
            throw std::invalid_argument("Valid rpi type is 'B'/'S'/'A'/'N'");
        }
    };
}
#endif
