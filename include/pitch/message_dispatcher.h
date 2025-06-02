//
// Created by phamanhtan on 30/5/25.
//

#ifndef MESSAGE_DISPATCHER_H
#define MESSAGE_DISPATCHER_H

#include "message.h"
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
#include <cstdint>
#include <functional>
#include <unordered_map>
#include <memory>

namespace CboePitch {
    struct MessageDispatchInfo {
        size_t length;
        std::function<std::shared_ptr<Message>(const uint8_t*, size_t, size_t)> parser;
    };

    class MessageDispatch {
    public:
        static const std::unordered_map<uint8_t, MessageDispatchInfo>& getDispatchTable() {
            static const std::unordered_map<uint8_t, MessageDispatchInfo> dispatchTable = {
                {0x97, {1, [](const uint8_t* d, size_t s, size_t o) { return std::make_shared<UnitClear>(UnitClear::parse(d, s,o )); }}},
                {0x3B, {22, [](const uint8_t* d, size_t s, size_t o) { return std::make_shared<TradingStatus>(TradingStatus::parse(d, s,o )); }}},
                {0x37, {42, [](const uint8_t* d, size_t s, size_t o) { return std::make_shared<AddOrder>(AddOrder::parse(d, s,o )); }}},
                {0x38, {43, [](const uint8_t* d, size_t s, size_t o) { return std::make_shared<OrderExecuted>(OrderExecuted::parse(d, s, o)); }}},
                {0x58, {48, [](const uint8_t* d, size_t s, size_t o) { return std::make_shared<OrderExecutedAtPrice>(OrderExecutedAtPrice::parse(d, s,o )); }}},
                {0x39, {20, [](const uint8_t* d, size_t s, size_t o) { return std::make_shared<ReduceSize>(ReduceSize::parse(d, s,o )); }}},
                {0x3A, {31, [](const uint8_t* d, const size_t s, size_t o) { return std::make_shared<ModifyOrder>(ModifyOrder::parse(d, s, o)); }}},
                {0x3C, {18, [](const uint8_t* d, const size_t s, size_t o) { return std::make_shared<DeleteOrder>(DeleteOrder::parse(d, s, o)); }}},
                {0x3D, {69, [](const uint8_t* d, size_t s, size_t o) { return std::make_shared<Trade>(Trade::parse(d, s,o )); }}},
                {0x3E, {16, [](const uint8_t* d, size_t s, size_t o) { return std::make_shared<TradeBreak>(TradeBreak::parse(d, s,o )); }}},
                {0xE3, {31, [](const uint8_t* d, size_t s, size_t o) { return std::make_shared<CalculatedValue>(CalculatedValue::parse(d, s,o )); }}},
                {0x2D, {1, [](const uint8_t* d, size_t s, size_t o) { return std::make_shared<EndOfSession>(EndOfSession::parse(d, s,o )); }}},
                {0x59, {27, [](const uint8_t* d, size_t s, size_t o) { return std::make_shared<AuctionUpdate>(AuctionUpdate::parse(d, s,o )); }}},
                {0x5A, {26, [](const uint8_t* d, size_t s, size_t o) { return std::make_shared<AuctionSummary>(AuctionSummary::parse(d, s,o )); }}},
            };
            return dispatchTable;
        }
    };
} // namespace CboePitch

#endif // MESSAGE_DISPATCHER_H