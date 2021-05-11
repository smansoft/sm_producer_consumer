/*
 *	Copyright (c) 2021 SManSoft <http://www.smansoft.com/>
 *  Sergey Manoylo <info@smansoft.com>
 *
 * 	sm_exceptions.cpp
 *
 *  Created on: Apr 26, 2021
 *      Author: sman
 */

#include "pch.h"

#include "sm_exceptions.h"
#include "sm_log_tool.h"

extern sm_log_config gsm_log_config;	//	global instance of main log support structure // @suppress("Unused variable declaration in file scope")
#define SM_LOG_CONFIG &gsm_log_config	//	just synonym: SM_LOG_CONFIG == &gsm_log_config - for usage in log api calls

namespace sm_producer_consumer {

/************************** CSMBaseException **************************/

CSMBaseException::CSMBaseException() noexcept
{
}

CSMBaseException::CSMBaseException(const CSMBaseException &ex) noexcept
{
	m_message = ex.m_message;
}

CSMBaseException::CSMBaseException(const std::string &message) noexcept : m_message(message)
{
}

CSMBaseException::~CSMBaseException() noexcept
{
}

CSMBaseException& CSMBaseException::operator = (const CSMBaseException &ex) noexcept
{
	m_message = ex.m_message;
	return *this;
}

const char* CSMBaseException::what() const noexcept
{
	return m_message.c_str();
}

const std::string CSMBaseException::message() const noexcept
{
	return m_message;
}

/************************** CSMProducerException **************************/

CSMProducerException::CSMProducerException() noexcept
{
}

CSMProducerException::CSMProducerException(const CSMProducerException &ex) noexcept :
		CSMBaseException(ex)
{
}

CSMProducerException::CSMProducerException(const std::string &message) noexcept :
		CSMBaseException(message)
{
}

CSMProducerException::~CSMProducerException() noexcept
{
}

/************************** CSMConsumerMidException **************************/

CSMConsumerMidException::CSMConsumerMidException() noexcept
{
}

CSMConsumerMidException::CSMConsumerMidException(const CSMConsumerMidException &ex) noexcept :
		CSMBaseException(ex)
{
}

CSMConsumerMidException::CSMConsumerMidException(const std::string &message) noexcept :
		CSMBaseException(message)
{
}

CSMConsumerMidException::~CSMConsumerMidException() noexcept
{
}

/************************** CSMConsumerFinException **************************/

CSMConsumerFinException::CSMConsumerFinException() noexcept
{
}

CSMConsumerFinException::CSMConsumerFinException(const CSMConsumerFinException &ex) noexcept :
		CSMBaseException(ex)
{
}

CSMConsumerFinException::CSMConsumerFinException(const std::string &message) noexcept :
		CSMBaseException(message)
{
}

CSMConsumerFinException::~CSMConsumerFinException() noexcept
{
}

/************************** CSMProducerFactoryException **************************/

CSMProducerFactoryException::CSMProducerFactoryException() noexcept
{
}

CSMProducerFactoryException::CSMProducerFactoryException(const CSMProducerFactoryException &ex) noexcept :
		CSMBaseException(ex)
{
}

CSMProducerFactoryException::CSMProducerFactoryException(const std::string &message) noexcept
		: CSMBaseException(message)
{
}

CSMProducerFactoryException::~CSMProducerFactoryException() noexcept
{
}

/************************** CSMConsumerFactoryException **************************/

CSMConsumerFactoryException::CSMConsumerFactoryException() noexcept
{
}

CSMConsumerFactoryException::CSMConsumerFactoryException(const CSMConsumerFactoryException &ex) noexcept :
		CSMBaseException(ex)
{
}

CSMConsumerFactoryException::CSMConsumerFactoryException(const std::string &message) noexcept
		: CSMBaseException(message)
{
}

CSMConsumerFactoryException::~CSMConsumerFactoryException() noexcept
{
}

}
