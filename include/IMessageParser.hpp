/**
 * @file    IMessageParser.hpp
 * @brief   Interface for message parser implementations.
 *
 * Developer: Hoang Nguyen & Tan A. Pham
 * Copyright: Equix Technologies Pty Ltd (contact@equix.com.au)
 * Filename: IMessageParser.hpp
 * Created: 28/May/2025
 *
 * Description:
 *   Defines an abstract interface for all message parser implementations.
 *   Parsers convert datagram (raw string) into IMessagePtr. 
 */

#pragma once

#ifndef I_MESSAGE_PARSER_HPP_
#define I_MESSAGE_PARSER_HPP_

#include "IMessage.hpp"

/**
 * @class IMessageParser
 * @brief Abstract base for all protocol message parser implementations.
 *
 * Implementations should parse raw input and return a new IMessagePtr on success, or nullptr on failure.
 */
class IMessageParser {
public:
    virtual ~IMessageParser() = default;

    /**
     * @brief Parse a datagram string into a message object.
     * @param datagram  Raw datagram or message as a string.
     * @return Shared pointer to IMessage if parse is successful, nullptr otherwise.
     */
    virtual IMessagePtr parse(const std::string& datagram) = 0;
};

#endif // I_MESSAGE_PARSER_HPP_
