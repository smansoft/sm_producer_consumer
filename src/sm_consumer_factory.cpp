/*
 *	Copyright (c) 2021 SManSoft <http://www.smansoft.com/>
 * 	Sergey Manoylo <info@smansoft.com>
 *
 * 	sm_consumer_factory.cpp
 *
 *  Created on: Apr 26, 2021
 *      Author: sman
 */

#include "pch.h"

#include "sm_consumer_factory.h"
#include "sm_exceptions.h"
#include "sm_log_tool.h"
#include "sm_tools.h"

extern sm_log_config gsm_log_config;	//	global instance of main log support structure
#define SM_LOG_CONFIG &gsm_log_config	//	just synonym: SM_LOG_CONFIG == &gsm_log_config - for usage in log api calls

namespace sm_producer_consumer {

CSMConsumerFactory* CSMConsumerFactory::m_instance = NULL;
std::mutex CSMConsumerFactory::m_mtx_instance;

CSMConsumerFactory* CSMConsumerFactory::getInstance()
{
	if(!CSMConsumerFactory::m_instance) {
		m_mtx_instance.lock();
		if(!CSMConsumerFactory::m_instance)
			CSMConsumerFactory::m_instance = new CSMConsumerFactory();
		m_mtx_instance.unlock();
	}
	return CSMConsumerFactory::m_instance;
}

void CSMConsumerFactory::freeInstance()
{
	if(CSMConsumerFactory::m_instance) {
		m_mtx_instance.lock();
		if(CSMConsumerFactory::m_instance)
			delete CSMConsumerFactory::m_instance;
		m_mtx_instance.unlock();
	}
}

CSMConsumerFactory::CSMConsumerFactory() : m_consumer_fin(NULL)
{
	sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
	sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");
}

CSMConsumerFactory::~CSMConsumerFactory()
{
	sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
	m_consumer_mids.clear();
	m_mtx_consumer_mids.unlock();
	sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");
}

void CSMConsumerFactory::addConsumerMid(CSMConsumerMid* const consumer_mid)
{
	sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
	std::shared_ptr<CSMConsumerMid> reader_ptr(consumer_mid);
	std::unique_lock<std::mutex> lock(m_mtx_consumer_mids);
	m_consumer_mids.push_back(reader_ptr);
	lock.unlock();
	sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");
}

void CSMConsumerFactory::createConsumerMid(const ProducerType producer_type, CSMConsumerMid** const consumer_mid)
{
	sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
	sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_DEBUG, __PRETTY_FUNCTION__, "producer_type = %d", producer_type);
	*consumer_mid = new CSMConsumerMid(producer_type);
	sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");
}

void CSMConsumerFactory::createAddConsumerMid(const ProducerType producer_type, const bool auto_start)
{
	sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
	sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_DEBUG, __PRETTY_FUNCTION__, "producer_type = %d", producer_type);
	CSMConsumerMid *reader;
	createConsumerMid(producer_type, &reader);
	addConsumerMid(reader);
	if(auto_start && !reader->isStarted())
		reader->startReader();
	sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");
}

void CSMConsumerFactory::startAllConsumerMids()
{
	sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
	std::unique_lock<std::mutex> lock(m_mtx_consumer_mids);
	for(std::vector<std::shared_ptr<CSMConsumerMid>>::iterator it = m_consumer_mids.begin();
			it != m_consumer_mids.end(); it++) {
		if(!(*it).get()->isStarted())
			(*it).get()->startReader();
	}
	lock.unlock();
	sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");
}

void CSMConsumerFactory::stopAllConsumerMids()
{
	sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
	std::unique_lock<std::mutex> lock(m_mtx_consumer_mids);
	for(std::vector<std::shared_ptr<CSMConsumerMid>>::iterator it = m_consumer_mids.begin();
			it != m_consumer_mids.end(); it++) {
		if((*it).get()->isStarted())
			(*it).get()->stopReader();
	}
	lock.unlock();
	sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");
}

size_t CSMConsumerFactory::getConsumerMids()
{
	sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
	size_t size = 0;
	std::unique_lock<std::mutex> lock(m_mtx_consumer_mids);
	size = m_consumer_mids.size();
	sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_DEBUG, __PRETTY_FUNCTION__, "size  = %d", size);
	lock.unlock();
	sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");
	return size;
}

size_t CSMConsumerFactory::getNumStartedConsumerMids()
{
	sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
	size_t size = 0;
	std::unique_lock<std::mutex> lock(m_mtx_consumer_mids);
	for(std::vector<std::shared_ptr<CSMConsumerMid>>::const_iterator it = m_consumer_mids.begin(); it != m_consumer_mids.end(); it++) {
		if((*it).get()->isStarted())
			size++;
	}
	sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_DEBUG, __PRETTY_FUNCTION__, "size  = %d", size);
	lock.unlock();
	sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");
	return size;
}

void CSMConsumerFactory::setConsumerFin(CSMConsumerFin* const reader_processor)
{
	sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
	CSMConsumerFin* reader_processor_prev = m_consumer_fin.get();
	if(reader_processor_prev) {
		if(reader_processor_prev->isStarted())
			reader_processor_prev->stopReaderProcessor();
		delete reader_processor_prev;
	}
	m_consumer_fin = std::shared_ptr<CSMConsumerFin>(reader_processor);
	sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");
}

void CSMConsumerFactory::createConsumerFin(const std::string& out_dpath, const std::string& out_fname, CSMConsumerFin** const consumer_fin)
{
	sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
	errno_t err = SM_RES_ERROR;
	char fpath[MAX_PATH];
	err = sm_init_fpath(fpath, SM_ARRAY_SIZE(fpath), out_dpath.c_str(), out_fname.c_str());
	if (err != SM_RES_OK)
		throw CSMConsumerFactoryException(std::string("CSMConsumerFactory: error initializing of the file path, using: out_dpath = ") + out_dpath + " out_fname = " + out_fname);
	std::string res_fpath(fpath);
	if(res_fpath.empty())
		throw CSMConsumerFactoryException(std::string("CSMConsumerFactory: res_fpath (fpath) is empty"));
	*consumer_fin = new CSMConsumerFin(res_fpath);
	sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");
}

void CSMConsumerFactory::createSetConsumerFin(const std::string& out_dpath, const std::string& out_fname, const bool auto_start)
{
	sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
	sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_DEBUG, __PRETTY_FUNCTION__,
			"out_dpath = %s out_fname = %s auto_start = %d",
			out_dpath.c_str(), out_fname.c_str(), auto_start);
	CSMConsumerFin *consumer_fin;
	createConsumerFin(out_dpath, out_fname, &consumer_fin);
	m_consumer_fin = std::shared_ptr<CSMConsumerFin>(consumer_fin);
	if(auto_start && !consumer_fin->isStarted())
		consumer_fin->startReaderProcessor();
	sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");
}

CSMConsumerFin* CSMConsumerFactory::getConsumerFin()
{
	return m_consumer_fin.get();
}

void CSMConsumerFactory::startConsumerFin()
{
	sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
	CSMConsumerFin* consumer_fin = m_consumer_fin.get();
	if(consumer_fin->isStarted())
		throw CSMConsumerFactoryException("CSMConsumerFactory: ConsumerMid already started");
	consumer_fin->startReaderProcessor();
	sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
}

void CSMConsumerFactory::stopConsumerFin()
{
	sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
	CSMConsumerFin* consumer_fin = m_consumer_fin.get();
	if(!consumer_fin->isStarted())
		throw CSMConsumerFactoryException("CSMConsumerFactory: ConsumerMid isn't started");
	consumer_fin->stopReaderProcessor();
	sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");
}

}
