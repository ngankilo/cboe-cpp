#ifndef _LIBPITCH_TYPES_BASE_H
#define _LIBPITCH_TYPES_BASE_H
#include <cstdint>
#include <stdexcept>
namespace pitch::types {



    /***
     * Convert a range of characters into base-N number. Accepts 0-9 and A-Z as digits
     * type is a forward iterator to a char
     * @param base template parameter for base to be used
     * @param begin iterator to a begining of number to convert
     * @param end iterator to an ending of number to convert
     * @throw std::invalid_argument if incorrect digit
     */
    template <int base, typename T>
    std::uint64_t get_base(T begin, const T &end)
    {
        std::uint64_t result = 0;
        while (begin != end)
        {
            static_assert(base <= 36 && base >=2, "base 2-36 handled");
            int d;
            if constexpr (base > 10) {
                if (*begin >= '0' && *begin <= '9')
                    d = *begin - '0';
                else if (*begin >= 'A' && *begin < 'A' + base - 10)
                    d = *begin - 'A' + 10;
                else
                    throw std::invalid_argument("Not a base" + std::to_string(base) +" digit");
            } else {
                if (*begin >= '0' && *begin < '0' + base)
                    d = *begin - '0';
                else
                    throw std::invalid_argument("Not a base" + std::to_string(base) +" digit");
            }
            result *= base;
            result += d;

            begin++;
        }

        return result;
    };

    /***
     * Convert a string-equivalent to base-N number
     * @param s string-equivalent input, at most 12-characters long
     * @throw std::invalid_argument if incorrect digit
     */
    template <int base, typename T>
    std::uint64_t get_base(T s)
    {
        return get_base<base>(s.begin(), s.end());
    }

    /***
     * Convert a range of characters into base36 number. Accepts 0-9 and A-Z as digits
     * type is a forward iterator to a char
     * @param begin iterator to a begining of number to convert
     * @param end iterator to an ending of number to convert
     * @throw std::invalid_argument if incorrect digit
     */
    template <typename T>
    std::uint64_t get_base36(T begin, const T &end) {
        return get_base<36>(begin, end);
    }

    /***
     * Convert a string-equivalent to base36 number
     * @param s string-equivalent input, at most 12-characters long
     * @throw std::invalid_argument if incorrect digit
     */
    template <typename T>
    std::uint64_t get_base36(T s)
    {
        return get_base36(s.begin(), s.end());
    }

}
#endif
