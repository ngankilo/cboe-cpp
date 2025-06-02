/**
 * @file    IMessageHandler.hpp
 * @brief   Interface for handling IMessagePtr.
 *
 * Developer: Hoang Nguyen & Tan A. Pham
 * Copyright: Equix Technologies Pty Ltd (contact@equix.com.au)
 * Filename: IMessageHandler.hpp
 * Created: 28/May/2025
 *
 * Description:
 *   Defines an abstract interface for all message handler implementations.
 *   Any protocol or business logic handler should derive from this.
 */

#pragma once

#ifndef I_MESSAGE_HANDLER_HPP_
#define I_MESSAGE_HANDLER_HPP_

#include "IMessage.hpp"

/**
 * @class IMessageHandler
 * @brief Abstract base for all message handler implementations.
 */
class IMessageHandler {
public:
    virtual ~IMessageHandler() = default;

    /**
     * @brief Process a message (polymorphic interface).
     * @param msg  Shared pointer to IMessage instance.
     */
    virtual void handle(const IMessagePtr& msg) = 0;
};

#endif // I_MESSAGE_HANDLER_HPP_
