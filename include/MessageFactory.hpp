/**
 * @file    MessageFactory.hpp
 * @brief   Pluggable factory for message parsers and handlers.
 *
 * Developer: Hoang Nguyen & Tan A. Pham
 * Copyright: Equix Technologies Pty Ltd (contact@equix.com.au)
 * Filename: MessageFactory.hpp
 * Created: 28/May/2025
 *
 * Description:
 *   Provides factory methods for constructing IMessageParser and IMessageHandler
 *   objects by type string. Extendable for additional protocol/message families.
 */

#pragma once

#include <memory>
#include <string>
#include "IMessageParser.hpp"
#include "IMessageHandler.hpp"

/**
 * @class MessageFactory
 * @brief Factory for creating protocol-specific message parsers and handlers.
 *
 * To support new message types, extend the implementation in MessageFactory.cpp.
 */
class MessageFactory {
public:
    /**
     * @brief Creates a message parser of the requested type.
     * @param type String identifier (e.g., "CBOE", "FIX", etc.).
     * @return Unique pointer to IMessageParser, or nullptr if unsupported.
     */
    static std::unique_ptr<IMessageParser> createParser(const std::string& type);

    /**
     * @brief Creates a message handler of the requested type.
     * @param type String identifier (e.g., "CBOE", "FIX", etc.).
     * @return Unique pointer to IMessageHandler, or nullptr if unsupported.
     */
    static std::unique_ptr<IMessageHandler> createHandler(const std::string& type);
};
