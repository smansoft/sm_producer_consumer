/*
 *  Copyright (c) 2021 SManSoft <http://www.smansoft.com/>
 *  Sergey Manoylo <info@smansoft.com>
 *
 *  sm_producer_factory.cpp
 *
 *  Created on: Apr 26, 2021
 *      Author: sman
 */

#include "pch.h"

#include "sm_producer_factory.h"
#include "sm_exceptions.h"
#include "sm_log_tool.h"

extern sm_log_config gsm_log_config;    //  global instance of main log support structure
#define SM_LOG_CONFIG &gsm_log_config   //  just synonym: SM_LOG_CONFIG == &gsm_log_config - for usage in log api calls

namespace sm_producer_consumer {

CSMProducerFactory* CSMProducerFactory::m_instance = NULL;
std::mutex CSMProducerFactory::m_mtx_instance;

CSMProducerFactory* CSMProducerFactory::getInstance()
{
    if(!CSMProducerFactory::m_instance) {
        m_mtx_instance.lock();
        if(!CSMProducerFactory::m_instance)
            CSMProducerFactory::m_instance = new CSMProducerFactory();
        m_mtx_instance.unlock();
    }
    return CSMProducerFactory::m_instance;
}

void CSMProducerFactory::freeInstance()
{
    if(CSMProducerFactory::m_instance) {
        m_mtx_instance.lock();
        if(CSMProducerFactory::m_instance)
            delete CSMProducerFactory::m_instance;
        m_mtx_instance.unlock();
    }
}

CSMProducerFactory::CSMProducerFactory()
{
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");
}

CSMProducerFactory::~CSMProducerFactory()
{
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
    std::unique_lock<std::mutex> lock(m_mtx_producers);
    m_producers.clear();
    lock.unlock();
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");
}

void CSMProducerFactory::createProducer(const ProducerType producer_type, CSMProducerBase** const producer)
{
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
    sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_DEBUG, __PRETTY_FUNCTION__, "producer_type = %d ", producer_type);
    switch(producer_type) {
    case ProducerType_None:
        throw CSMProducerException(std::string("CSMProducerFactory: wrong value of producer_type: ") + std::to_string(producer_type));
        break;
    case ProducerType_1:
        *producer = new CSMProducerType1();
        break;
    case ProducerType_2:
        *producer = new CSMProducerType2();
        break;
    case ProducerType_3:
        *producer = new CSMProducerType3();
        break;
    case ProducerType_4:
        *producer = new CSMProducerType4();
        break;
    case ProducerType_5:
        *producer = new CSMProducerType5();
        break;
    default:
        throw CSMProducerException(std::string("CSMProducerFactory: wrong value of producer_type: ") + std::to_string(producer_type));
        break;
    }
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");
}

void CSMProducerFactory::addProducer(CSMProducerBase* const producer)
{
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
    std::shared_ptr<CSMProducerBase> producer_ptr(producer);
    ProducerType producer_type = producer->getProducerType();
    std::unique_lock<std::mutex> lock(m_mtx_producers);
    for(std::vector<std::shared_ptr<CSMProducerBase>>::iterator it = m_producers.begin();
            it != m_producers.end(); it++) {
        if((*it).get()->getProducerType() == producer_type)
            throw CSMProducerFactoryException(std::string("Producer with type = ") + std::to_string((int)producer_type) + std::string(" already exists..."));
    }
    m_producers.push_back(producer_ptr);
    lock.unlock();
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");
}

void CSMProducerFactory::openProducer(const ProducerType producer_type, CSMProducerBase** const producer)
{
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
    bool res = getProducer(producer_type, producer);
    if(!res || !(*producer))
        throw CSMProducerFactoryException(std::string("Error opening of Producer type = ") + std::to_string((int)producer_type));
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");
}

bool CSMProducerFactory::getProducer(const ProducerType producer_type, CSMProducerBase** const producer)
{
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
    *producer = NULL;
    std::unique_lock<std::mutex> lock(m_mtx_producers);
    for(std::vector<std::shared_ptr<CSMProducerBase>>::iterator it = m_producers.begin();
            it != m_producers.end(); it++) {
        if((*it).get()->getProducerType() == producer_type)
            *producer = (*it).get();
    }
    lock.unlock();
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");
    return (*producer != NULL);
}

void CSMProducerFactory::startAllProducers()
{
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
    std::unique_lock<std::mutex> lock(m_mtx_producers);
    for(std::vector<std::shared_ptr<CSMProducerBase>>::iterator it = m_producers.begin();
            it != m_producers.end(); it++) {
        if(!(*it).get()->isStarted())
            (*it).get()->startAddData();
    }
    lock.unlock();
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");
}

void CSMProducerFactory::stopAllProducers()
{
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
    std::unique_lock<std::mutex> lock(m_mtx_producers);
    for(std::vector<std::shared_ptr<CSMProducerBase>>::iterator it = m_producers.begin();
            it != m_producers.end(); it++) {
        if((*it).get()->isStarted())
            (*it).get()->stopAddData();
    }
    lock.unlock();
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");
}

size_t CSMProducerFactory::getNumProducers()
{
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
    size_t res = 0;
    std::unique_lock<std::mutex> lock(m_mtx_producers);
    res = m_producers.size();
    lock.unlock();
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");
    return res;
}

}
