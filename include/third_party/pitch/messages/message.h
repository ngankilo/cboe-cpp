#ifndef _LIBPITCH_PITCH_MESSAGES_H
#define _LIBPITCH_PITCH_MESSAGES_H

// forward declaration needed to make decoder a friend;
// making it a friend is necessary to hide setters while exposing getters
namespace pitch
{
    template <typename T>
    class decoder;
}

namespace pitch::messages
{
    /**
     * base class of a message; Cannot be constructed on its own
     */
    class message
    {
    public:
        /**
         * Enumerate types of messages
         */
        enum message_type
        {
            /// parse error or failed construction
            unknown_message = 0,
            /// defined on page 6 of spec.
            symbol_clear = 1,
            /// add pending order, page 7.
            /// this also includes the long version of add order,
            /// to accommodate longer symbold, see page 8
            add_order = 2,
            /// order fully or partially executed, page 9
            order_executed = 3,
            /// page 10
            order_cancel = 4,
            /// trade on symbol that was not part of add_order page 10.
            /// this also includes long version of trade, see page 11
            trade = 5,
            /// page 12
            trade_break = 6,
            /// page 13
            trading_status = 7,
            /// auction update, pages 14-15, there are separate definitions
            /// of this message for BYX and BZX but single class for parser
            /// as these are compatible
            auction_update = 8,
            /// auction summary, pages 15-16
            auction_summary = 9,
            /// retail price improvement (page 17)
            retail_price_improvement = 10
        };

        /**
         * Get type of given message (i.e. Add Order, Order Executed); Types of orders
         * correspond to names of classes
         */
        virtual message_type get_type() { return message_type::unknown_message; }

        /**
         * Get timestamp (number milliseconds past midnight Eastern Time)
        */
        uint64_t get_timestamp() { return timestamp; }

    protected:
        message(uint64_t _timestamp) : timestamp(_timestamp) {}
    private:
        uint64_t timestamp;
    };
}

#include "symbol_clear.h"
#include "add_order.h"
#include "order_executed.h"
#include "order_cancel.h"
#include "trade.h"
#include "trade_break.h"
#include "trading_status.h"
#include "auction_update.h"
#include "auction_summary.h"
#include "retail_price_improvement.h"

#endif