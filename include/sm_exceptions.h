/*
 *  Copyright (c) 2021 SManSoft <http://www.smansoft.com/>
 *  Sergey Manoylo <info@smansoft.com>
 *
 *  sm_exceptions.h
 *
 *  Created on: Apr 26, 2021
 *      Author: sman
 */

#pragma once

#include "pch.h"

#include "sm_main.h"

namespace sm_producer_consumer {

/**
 * Suite of exception classes;
 */

/**
 * CSMBaseException
 *
 * exception class
 */
class CSMBaseException : public std::exception
{
public:
    CSMBaseException() noexcept;
    CSMBaseException(const CSMBaseException &ex) noexcept;
    CSMBaseException(const std::string &message) noexcept;
    virtual ~CSMBaseException() noexcept;

    CSMBaseException& operator = (const CSMBaseException &ex) noexcept;

    virtual const char* what() const noexcept;
    virtual const std::string message() const noexcept;

protected:
    std::string m_message;

private:

};

/**
 * CSMProducerException
 *
 * exception class
 */
class CSMProducerException : public CSMBaseException
{
public:
    CSMProducerException() noexcept;
    CSMProducerException(const CSMProducerException &ex) noexcept;
    CSMProducerException(const std::string &message) noexcept;
    virtual ~CSMProducerException() noexcept;

protected:

private:

};

/**
 * CSMConsumerMidException
 *
 * exception class
 */
class CSMConsumerMidException : public CSMBaseException
{
public:
    CSMConsumerMidException() noexcept;
    CSMConsumerMidException(const CSMConsumerMidException &ex) noexcept;
    CSMConsumerMidException(const std::string &message) noexcept;
    virtual ~CSMConsumerMidException() noexcept;

protected:

private:

};

/**
 * CSMConsumerFinException
 *
 * exception class
 */
class CSMConsumerFinException : public CSMBaseException
{
public:
    CSMConsumerFinException() noexcept;
    CSMConsumerFinException(const CSMConsumerFinException &ex) noexcept;
    CSMConsumerFinException(const std::string &message) noexcept;
    virtual ~CSMConsumerFinException() noexcept;

protected:

private:

};

/**
 * CSMProducerFactoryException
 *
 * exception class
 */
class CSMProducerFactoryException : public CSMBaseException
{
public:
    CSMProducerFactoryException() noexcept;
    CSMProducerFactoryException(const CSMProducerFactoryException &ex) noexcept;
    CSMProducerFactoryException(const std::string &message) noexcept;
    virtual ~CSMProducerFactoryException() noexcept;

protected:

private:

};

/**
 * CSMConsumerFactoryException
 *
 * exception class
 */
class CSMConsumerFactoryException : public CSMBaseException
{
public:
    CSMConsumerFactoryException() noexcept;
    CSMConsumerFactoryException(const CSMConsumerFactoryException &ex) noexcept;
    CSMConsumerFactoryException(const std::string &message) noexcept;
    virtual ~CSMConsumerFactoryException() noexcept;

protected:

private:

};

}

