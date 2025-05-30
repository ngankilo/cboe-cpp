//
// Created by phamanhtan on 30/5/25.
//

#ifndef MESSAGE_DISPATCHER_H
#define MESSAGE_DISPATCHER_H

#include "pitch/message.h"
#include "pitch/gap_login.h"
#include "pitch/gap_login_response.h"
#include "pitch/gap_request.h"
#include "pitch/gap_response.h"
#include "pitch/unit_clear.h"
#include "pitch/trading_status.h"
#include "pitch/add_order.h"
#include "pitch/order_executed.h"
#include "pitch/order_executed_at_price.h"
#include "pitch/reduce_size.h"
#include "pitch/modify_order.h"
#include "pitch/delete_order.h"
#include "pitch/trade.h"
#include "pitch/trade_break.h"
#include "pitch/calculated_value.h"
#include "pitch/end_of_session.h"
#include "pitch/auction_update.h"
#include "pitch/auction_summary.h"
#include <cstdint>
#include <functional>
#include <unordered_map>
#include <memory>

namespace CboePitch {
    struct MessageDispatchInfo {
        size_t length;
        std::function<std::unique_ptr<Message>(const uint8_t *, size_t)> parser;
    };

    class MessageDispatch {
    public:
        static const std::unordered_map<uint8_t, MessageDispatchInfo> &getDispatchTable() {
            static const std::unordered_map<uint8_t, MessageDispatchInfo> dispatchTable = {
                {
                    0x01,
                    {14, [](const uint8_t *d, size_t s) { return std::make_unique<GapLogin>(GapLogin::parse(d, s)); }}
                }, // GapLogin
                {
                    0x02,
                    {
                        1,
                        [](const uint8_t *d, size_t s) {
                            return std::make_unique<LoginResponse>(LoginResponse::parse(d, s));
                        }
                    }
                }, // LoginResponse
                {
                    0x03,
                    {
                        7,
                        [](const uint8_t *d, size_t s) { return std::make_unique<GapRequest>(GapRequest::parse(d, s)); }
                    }
                }, // GapRequest
                {
                    0x04,
                    {
                        8,
                        [](const uint8_t *d, size_t s) {
                            return std::make_unique<GapResponse>(GapResponse::parse(d, s));
                        }
                    }
                }, // GapResponse
                {
                    0x97,
                    {1, [](const uint8_t *d, size_t s) { return std::make_unique<UnitClear>(UnitClear::parse(d, s)); }}
                }, // UnitClear
                {
                    0x3B,
                    {
                        19,
                        [](const uint8_t *d, size_t s) {
                            return std::make_unique<TradingStatus>(TradingStatus::parse(d, s));
                        }
                    }
                }, // TradingStatus
                {
                    0x37,
                    {39, [](const uint8_t *d, size_t s) { return std::make_unique<AddOrder>(AddOrder::parse(d, s)); }}
                }, // AddOrder
                {
                    0x38,
                    {
                        40,
                        [](const uint8_t *d, size_t s) {
                            return std::make_unique<OrderExecuted>(OrderExecuted::parse(d, s));
                        }
                    }
                }, // OrderExecuted
                {
                    0x58,
                    {
                        48,
                        [](const uint8_t *d, size_t s) {
                            return std::make_unique<OrderExecutedAtPrice>(OrderExecutedAtPrice::parse(d, s));
                        }
                    }
                }, // OrderExecutedAtPrice
                {
                    0x39,
                    {
                        20,
                        [](const uint8_t *d, size_t s) { return std::make_unique<ReduceSize>(ReduceSize::parse(d, s)); }
                    }
                }, // ReduceSize
                {
                    0x3A,
                    {
                        28,
                        [](const uint8_t *d, size_t s) {
                            return std::make_unique<ModifyOrder>(ModifyOrder::parse(d, s));
                        }
                    }
                }, // ModifyOrder
                {
                    0x3C,
                    {
                        16,
                        [](const uint8_t *d, size_t s) {
                            return std::make_unique<DeleteOrder>(DeleteOrder::parse(d, s));
                        }
                    }
                }, // DeleteOrder
                {0x3D, {69, [](const uint8_t *d, size_t s) { return std::make_unique<Trade>(Trade::parse(d, s)); }}},
                // Trade
                {
                    0x3E,
                    {
                        16,
                        [](const uint8_t *d, size_t s) { return std::make_unique<TradeBreak>(TradeBreak::parse(d, s)); }
                    }
                }, // TradeBreak
                {
                    0xE3,
                    {
                        31,
                        [](const uint8_t *d, size_t s) {
                            return std::make_unique<CalculatedValue>(CalculatedValue::parse(d, s));
                        }
                    }
                }, // CalculatedValue
                {
                    0x2D,
                    {
                        1,
                        [](const uint8_t *d, size_t s) {
                            return std::make_unique<EndOfSession>(EndOfSession::parse(d, s));
                        }
                    }
                }, // EndOfSession
                {
                    0x59,
                    {
                        27,
                        [](const uint8_t *d, size_t s) {
                            return std::make_unique<AuctionUpdate>(AuctionUpdate::parse(d, s));
                        }
                    }
                }, // AuctionUpdate
                {
                    0x5A,
                    {
                        26,
                        [](const uint8_t *d, size_t s) {
                            return std::make_unique<AuctionSummary>(AuctionSummary::parse(d, s));
                        }
                    }
                }
            };
            return dispatchTable;
        }
    };
} // namespace CboePitch

#endif // MESSAGE_DISPATCHER_H
