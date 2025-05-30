//
// Created by phamanhtan on 30/5/25.
//

#ifndef MESSAGE_FACTORY_H
#define MESSAGE_FACTORY_H

#include "gap_login.h"
#include "gap_login_response.h"
#include "gap_request.h"
#include "gap_response.h"
#include "unit_clear.h"
#include "trading_status.h"
#include "add_order.h"
#include "order_executed.h"
#include "order_executed_at_price.h"
#include "reduce_size.h"
#include "modify_order.h"
#include "delete_order.h"
#include "trade.h"
#include "trade_break.h"
#include "calculated_value.h"
#include "end_of_session.h"
#include "auction_update.h"
#include "auction_summary.h"
#include "sequence_unit_header.h"
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <functional>

namespace CboePitch {
    class MessageFactory {
    public:
        static std::unique_ptr<Message> parseMessage(const uint8_t *data, size_t size) {
            if (size < 1) {
                throw std::invalid_argument("Message too short");
            }

            uint8_t messageType = data[0]; // First byte of the message payload (after header)

            static const std::unordered_map<uint8_t, std::function<std::unique_ptr<Message>(const uint8_t *, size_t)> >
                    messageCreators = {
                        {
                            0x01,
                            [](const uint8_t *d, size_t s) { return std::make_unique<GapLogin>(GapLogin::parse(d, s)); }
                        },
                        {
                            0x02,
                            [](const uint8_t *d, size_t s) {
                                return std::make_unique<LoginResponse>(LoginResponse::parse(d, s));
                            }
                        },
                        {
                            0x03,
                            [](const uint8_t *d, size_t s) {
                                return std::make_unique<GapRequest>(GapRequest::parse(d, s));
                            }
                        },
                        {
                            0x04,
                            [](const uint8_t *d, size_t s) {
                                return std::make_unique<GapResponse>(GapResponse::parse(d, s));
                            }
                        },
                        {
                            0x97,
                            [](const uint8_t *d, size_t s) {
                                return std::make_unique<UnitClear>(UnitClear::parse(d, s));
                            }
                        },
                        {
                            0x3B,
                            [](const uint8_t *d, size_t s) {
                                return std::make_unique<TradingStatus>(TradingStatus::parse(d, s));
                            }
                        },
                        {
                            0x37,
                            [](const uint8_t *d, size_t s) { return std::make_unique<AddOrder>(AddOrder::parse(d, s)); }
                        },
                        {
                            0x38,
                            [](const uint8_t *d, size_t s) {
                                return std::make_unique<OrderExecuted>(OrderExecuted::parse(d, s));
                            }
                        },
                        {
                            0x58,
                            [](const uint8_t *d, size_t s) {
                                return std::make_unique<OrderExecutedAtPrice>(OrderExecutedAtPrice::parse(d, s));
                            }
                        },
                        {
                            0x39,
                            [](const uint8_t *d, size_t s) {
                                return std::make_unique<ReduceSize>(ReduceSize::parse(d, s));
                            }
                        },
                        {
                            0x3A,
                            [](const uint8_t *d, size_t s) {
                                return std::make_unique<ModifyOrder>(ModifyOrder::parse(d, s));
                            }
                        },
                        {
                            0x3C,
                            [](const uint8_t *d, size_t s) {
                                return std::make_unique<DeleteOrder>(DeleteOrder::parse(d, s));
                            }
                        },
                        {0x3D, [](const uint8_t *d, size_t s) { return std::make_unique<Trade>(Trade::parse(d, s)); }},
                        {
                            0x3E,
                            [](const uint8_t *d, size_t s) {
                                return std::make_unique<TradeBreak>(TradeBreak::parse(d, s));
                            }
                        },
                        {
                            0xE3,
                            [](const uint8_t *d, size_t s) {
                                return std::make_unique<CalculatedValue>(CalculatedValue::parse(d, s));
                            }
                        },
                        {
                            0x2D,
                            [](const uint8_t *d, size_t s) {
                                return std::make_unique<EndOfSession>(EndOfSession::parse(d, s));
                            }
                        },
                        {
                            0x59,
                            [](const uint8_t *d, size_t s) {
                                return std::make_unique<AuctionUpdate>(AuctionUpdate::parse(d, s));
                            }
                        },
                        {
                            0x5A,
                            [](const uint8_t *d, size_t s) {
                                return std::make_unique<AuctionSummary>(AuctionSummary::parse(d, s));
                            }
                        }
                    };

            auto it = messageCreators.find(messageType);
            if (it == messageCreators.end()) {
                throw std::invalid_argument("Unknown message type: " + std::to_string(messageType));
            }

            return it->second(data, size);
        }

        static SeqUnitHeader parseHeader(const uint8_t *data, size_t size) {
            return SeqUnitHeader::parse(data, size);
        }
    };
} // namespace CboePitch

#endif // MESSAGE_FACTORY_H
