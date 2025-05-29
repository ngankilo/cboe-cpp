#ifndef _LIBPITCH_PITCH_TRADING_STATUS_H
#define _LIBPITCH_PITCH_TRADING_STATUS_H

#include "message.h"
#include "common_macros.h"
#include "../types/padded_string.h"
#include "../types/status_types.h"

namespace pitch::messages
{

    /**
     * Class representing Symbol Clear message
     *
     *   --------------------------- TRADING STATUS ---------------------------
     *   FIELD           OFFSET    LENGTH    DATA TYPE (cboe)   DATA TYPE (C++)
     *   timestamp       0         8         decimal            uint64_t
     *   type            8         1         alpha ("H")        message_type
     *   stock_symbol    9         8         printable ascii    std::string
     *   halt_status     17        1         alpha (enum)       halt_status_type
     *   reg_sho_action  18        1         numeric (parsed)   reg_sho_action_type
     *   reserved 1      19        1         alpha              N/A (ignore)
     *   reserved 1      20        1         alpha              N/A (ignore)
     *   LF              21
     *
     * Note: there is no long and short version here, and length of symbol corresponds to
     * lopng version of add_order and trade
     *
     * Source specification:
     * https://cdn.cboe.com/resources/membership/Cboe_US_Equities_TCP_PITCH_Specification.pdf
     * As of: March 25, 2022 (page 13)
     */
    class trading_status : public message
    {
    public:
        trading_status(uint64_t timestamp,
                       std::string _symbol,
                       types::halt_status_type _halt_status,
                       types::reg_sho_action_type _reg_sho_action) : message(timestamp),
                                                                     symbol(_symbol),
                                                                     halt_status(_halt_status),
                                                                     reg_sho_action(_reg_sho_action) {}
        message_type get_type() { return message_type::trading_status; }

        /**
         * stock symbol
         */
        const std::string &get_symbol() { return symbol; }

        /**
         * return halt status for the symbol
         */
        types::halt_status_type get_halt_status() { return halt_status; }

        /**
         * return reg SHO action on price test
         */
        types::reg_sho_action_type get_reg_sho_action() { return reg_sho_action; }

    private:
        std::string symbol;
        types::halt_status_type halt_status;
        types::reg_sho_action_type reg_sho_action;
    };

    /**
     * Class implementing builder pattern for trading_status. All methods and data fields are private
     * as it is only meant to be called by friend pitch::decoder<T1>
     */
    template <typename T1>
    class _trading_status_decoder
    {
    private:
        template <typename T2>
        static T1 decode_message(T2 begin, T2 end, uint64_t timestamp)
        {
            if (end - begin != length)
                throw std::invalid_argument("expected length of " + std::to_string(length) + " for trading status message");
            std::string symbol = pitch::types::get_padded_string(_OFFSET_PAIR(symbol_offset, symbol_length));
            types::halt_status_type halt_status = pitch::types::get_halt_status(begin + halt_status_offset);
            types::reg_sho_action_type reg_sho_action = pitch::types::get_reg_sho_action(begin + reg_sho_action_offset);

            return T1(new trading_status(timestamp, symbol, halt_status, reg_sho_action));
        }

        static const off_t symbol_offset = 9;
        static const off_t symbol_length = 8;
        static const off_t halt_status_offset = 17;
        static const off_t reg_sho_action_offset = 18;
        static const off_t length = 21;

        friend class pitch::decoder<T1>;
    };
}

#endif