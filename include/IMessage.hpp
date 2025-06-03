/**
 * @file    IMessage.hpp
 * @brief   Interface and basic implementation for generic messages.
 *
 * Developer: Hoang Nguyen & Tan A. Pham
 * Copyright: Equix Technologies Pty Ltd (contact@equix.com.au)
 * Filename: IMessage.hpp
 * Created: 28/May/2025
 *
 * Description:
 *   Defines the IMessage struct for generic messaging. Provides a simple dummy
 *   parser as an example for unit tests or simple pipelines.
 */

#pragma once

#ifndef I_MESSAGE_HPP_
#define I_MESSAGE_HPP_

#include <vector>
#include <string>
#include <memory>

/**
 * @struct IMessage
 * @brief Basic message structure with header, type, symbol, and payload data.
 */
struct IMessage {
    std::string header;            ///< Message header string.
    std::string message_type;      ///< Message type string (mstype).
    std::string symbol;            ///< Trading symbol string.
    std::vector<uint8_t> payload;  ///< Binary payload.
};

using IMessagePtr = std::shared_ptr<IMessage>;

/**
 * @brief Dummy parser: splits raw vector as "header-mstype-symbol-payload".
 * @param raw Raw message as a byte vector.
 * @return Shared pointer to parsed IMessage struct.
 *
 * For testing and demo only! Expects '-' as delimiter.
 */
inline IMessagePtr parse_message(const std::vector<uint8_t>& raw) {
    auto msg = std::make_shared<IMessage>();
    std::string str(raw.begin(), raw.end());
    size_t p1 = str.find('-'), p2 = str.find('-', p1 + 1), p3 = str.find('-', p2 + 1);
    msg->header       = str.substr(0, p1);
    msg->message_type = str.substr(p1 + 1, p2 - p1 - 1);
    msg->symbol       = str.substr(p2 + 1, p3 - p2 - 1);
    msg->payload.assign(str.begin() + p3 + 1, str.end());
    return msg;
}

#endif // I_MESSAGE_HPP_
