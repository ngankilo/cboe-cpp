//
// Created by phamanhtan on 29/5/25.
//
#pragma once

#ifndef MESSAGE_FACTORY_H
#define MESSAGE_FACTORY_H

#include "add_order_short.h"
#include "add_order_long.h"
#include "order_executed.h"
#include "trade_short.h"
#include "trade_long.h"
#include "delete_order.h"
#include "modify_order.h"
#include "sequence_unit_header.h"
#include "reduce_size.h"
#include <memory>
#include <stdexcept>

#include "sequence_unit_header.h"

namespace CboePitch {
    std::unique_ptr<Message> parseMessage(const std::string &line) {
        if (line.size() < 9) {
            throw std::invalid_argument("Message too short");
        }
        char messageType = line[8];
        switch (messageType) {
            case 'A': return AddOrderShort::parse(line.substr(8));
            case 'B': return AddOrderLong::parse(line.substr(8));
            case 'E': return OrderExecuted::parse(line.substr(8));
            case 'P': return TradeShort::parse(line.substr(8));
            case 'r': return TradeLong::parse(line.substr(8));
            case 'C': return DeleteOrder::parse(line.substr(8));
            case 'M': return ModifyOrder::parse(line.substr(8));
            case 'H': return SeqUnitHeader::parse(line.substr(8));
            case 'D': return ReduceSize::parse(line.substr(8));
            default: throw std::invalid_argument("Unknown message type: " + std::string(1, messageType));
        }
    }
} // namespace CboePitch
#endif //MESSAGE_FACTORY_H
