#ifndef _LIBPITCH_TYPES_PADDED_STRING_H
#define _LIBPITCH_TYPES_PADDED_STRING_H
#include <string>
#include <stdexcept>
#include <functional>
#include <algorithm>
namespace pitch::types
{

    /***
     * Extract space-padded string into a target string
     * @param begin input iterator for begining of string
     * @param begin input iterator right after last character
     * @param out_begin output iterator where string is to be written
     * @return output iterator just after string that was added
     */
    template <typename T1, typename T2>
    T2 copy_padded_string(T1 begin, const T1 &end, T2 out_begin)
    {
        if (*begin == ' ')
        {
            throw std::invalid_argument("Empty space padded string");
        }

        while (begin != end)
        {
            *out_begin = *begin;
            out_begin++;
            begin++;

            if (*begin == ' ')
            {
                break;
            }
        }

        if (!std::all_of(begin, end, std::bind(std::equal_to<char>(), ' ', std::placeholders::_1)))
        {
            throw std::invalid_argument("Invalid space padded string");
        }
        return out_begin;
    };

    /***
     * Extract space-padded string, return it
     * @param begin input iterator for begining of string
     * @param begin input iterator right after last character
     * @return value of string
     */
    template <typename T>
    std::string get_padded_string(T begin, const T &end)
    {
        std::string result;
        copy_padded_string(begin, end, back_inserter(result));
        return result;
    };
}
#endif