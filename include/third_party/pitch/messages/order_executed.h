#ifndef _LIBPITCH_PITCH_ORDER_EXECUTE_H
#define _LIBPITCH_PITCH_ORDER_EXECUTE_H

#include "message.h"
#include "common_macros.h"

namespace pitch::messages
{

    /**
     * Class representing Order Executed message
     * 
     *   --------------------------- ORDER EXECUTED ---------------------------
     *   FIELD           OFFSET    LENGTH    DATA TYPE (cboe)   DATA TYPE (C++)
     *   timestamp       0         8         decimal            uint64_t
     *   type            8         1         alpha ("E")        message_type
     *   order_id        9         12        base36             uint64_t
     *   executed_shares 21        6         decimal            uint64_t
     *   execution_id    27        12        base36             uint64_t
     *   LF              39
     *
     * Source specification:
     * https://cdn.cboe.com/resources/membership/Cboe_US_Equities_TCP_PITCH_Specification.pdf
     * As of: March 25, 2022 (page 9)
     */
    class order_executed : public message
    {
    public:
        order_executed(uint64_t timestamp,
                  uint64_t _order_id,
                  uint64_t _shares,
                  uint64_t _execution_id) : message(timestamp),
                                     order_id(_order_id),
                                     executed_shares(_shares),
                                     execution_id(_execution_id) {}


        message_type get_type() { return message_type::order_executed; }

        /**
         * Get id of an order (this is id earlier set in add_order)
         */
        uint64_t get_order_id() { return order_id; }

        /**
         * number of shares executed
         */
        uint64_t get_executed_shares() { return executed_shares; }

        /**
         * Get id of execution (don't confuse w/ order id)
         */
        uint64_t get_execution_id() { return execution_id; }

        private:
        uint64_t order_id;
        uint64_t executed_shares;
        uint64_t execution_id;
    };


    /**
     * Class implementing builder pattern for order_executed. All methods and data fields are private
     * as it is only meant to be called by friend pitch::decoder<T1>
    */
    template <typename T1>
    class _order_executed_decoder
    {
    private:
        template <typename T2>
        static T1 decode_message(T2 begin, T2 end, uint64_t timestamp)
        {
            if (end - begin != length)
                throw std::invalid_argument("expected length of " + std::to_string(length)
                + " for order executed message");

            uint64_t order_id = pitch::types::get_base36(_OFFSET_PAIR(order_id_offset, order_id_length));
            uint64_t shares = pitch::types::get_base<10>(_OFFSET_PAIR(shares_offset, shares_length));
            uint64_t execution_id = pitch::types::get_base36(_OFFSET_PAIR(execution_id_offset, execution_id_length));

            return T1(new order_executed(timestamp, order_id, shares, execution_id));
        }

        static const off_t order_id_offset = 9;
        static const off_t order_id_length = 12;
        static const off_t shares_offset = 21;
        static const off_t shares_length = 6;
        static const off_t execution_id_offset = 27;
        static const off_t execution_id_length = 12;
        static const off_t length = 39;

        friend class pitch::decoder<T1>;
    };
}

#endif