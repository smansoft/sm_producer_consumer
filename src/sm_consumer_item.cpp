/*
 *	Copyright (c) 2021 SManSoft <http://www.smansoft.com/>
 *	Sergey Manoylo <info@smansoft.com>
 *
 * 	sm_consumer_item.cpp
 *
 *  Created on: Apr 26, 2021
 *      Author: sman
 */

#include "pch.h"

#include "sm_consumer_item.h"
#include "sm_log_tool.h"

extern sm_log_config gsm_log_config;	//	global instance of main log support structure
#define SM_LOG_CONFIG &gsm_log_config	//	just synonym: SM_LOG_CONFIG == &gsm_log_config - for usage in log api calls

namespace sm_producer_consumer {

/************************** QueueItem **************************/

CSMConsumerItem::CSMConsumerItem() : m_data(0), m_size(0)
{
	sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
	sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");
}

CSMConsumerItem::CSMConsumerItem(const unsigned char *const  data, const unsigned int size) : m_size(size)
{
	sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
	m_data = std::shared_ptr<unsigned char> ((unsigned char*)data);
	sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_DEBUG, __PRETTY_FUNCTION__, "data = %s size = %d", m_data.get(), m_size);
	sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");
}

CSMConsumerItem::CSMConsumerItem(const CSMConsumerItem& consumer_item) : m_size(consumer_item.m_size)
{
	sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
	m_data = consumer_item.m_data;
	sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_DEBUG, __PRETTY_FUNCTION__, "data = %s size = %d", m_data.get(), m_size);
	sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");
}

CSMConsumerItem::~CSMConsumerItem()
{
	sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
	sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");
}

CSMConsumerItem& CSMConsumerItem::operator = (const CSMConsumerItem &consumer_item)
{
	sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
	m_data = consumer_item.m_data;
	m_size = consumer_item.m_size;
	sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_DEBUG, __PRETTY_FUNCTION__, "data = %s size = %d", m_data.get(), m_size);
	sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");

	return *this;
}

void CSMConsumerItem::setDataAndSize(const unsigned char* const data, const unsigned int size)
{
	sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
	m_data = std::shared_ptr<unsigned char> ((unsigned char*)data);
	m_size = size;
	sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_DEBUG, __PRETTY_FUNCTION__, "data = %s size = %d", m_data.get(), m_size);
	sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");
}

}
