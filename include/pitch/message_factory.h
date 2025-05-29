//
// Created by phamanhtan on 29/5/25.
//
#pragma once

#ifndef MESSAGE_FACTORY_H
#define MESSAGE_FACTORY_H
#pragma once
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

namespace CboePitch {
    std::unique_ptr<Message> parseMessage(const uint8_t *data, size_t size) {
        if (size < 2) throw std::invalid_argument("Message too short");
        uint8_t messageType = data[1];
        switch (messageType) {
            case 0x97: return UnitClear::parse(data, size);
            case 0x3B: return TradingStatus::parse(data, size);
            case 0x37: return AddOrder::parse(data, size);
            case 0x38: return OrderExecuted::parse(data, size);
            case 0x58: return OrderExecutedAtPrice::parse(data, size);
            case 0x39: return ReduceSize::parse(data, size);
            case 0x3A: return ModifyOrder::parse(data, size);
            case 0x3C: return DeleteOrder::parse(data, size);
            case 0x3D: return Trade::parse(data, size);
            case 0x3E: return TradeBreak::parse(data, size);
            case 0xE3: return CalculatedValue::parse(data, size);
            case 0x2D: return EndOfSession::parse(data, size);
            case 0x59: return AuctionUpdate::parse(data, size);
            case 0x5A: return AuctionSummary::parse(data, size);
            default: throw std::invalid_argument("Unknown message type: " + std::to_string(messageType));
        }
    }

    std::unique_ptr<Message> parseHeader(const uint8_t *data, size_t size) {
        return SeqUnitHeader::parse(data, size);
    }
} // namespace CboePitch
#endif //MESSAGE_FACTORY_H
