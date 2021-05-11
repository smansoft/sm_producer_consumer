/*
 *  Copyright (c) 2021 SManSoft <http://www.smansoft.com/>
 *  Sergey Manoylo <info@smansoft.com>
 *
 *  sm_consumer_mid.cpp
 *
 *  Created on: Apr 26, 2021
 *      Author: sman
 */

#include "pch.h"

#include "sm_consumer_factory.h"
#include "sm_consumer_mid.h"
#include "sm_exceptions.h"
#include "sm_producer.h"
#include "sm_producer_factory.h"
#include "sm_log_tool.h"

extern sm_log_config gsm_log_config;    //  global instance of main log support structure
#define SM_LOG_CONFIG &gsm_log_config   //  just synonym: SM_LOG_CONFIG == &gsm_log_config - for usage in log api calls

namespace sm_producer_consumer {

/************************** Reader **************************/

CSMConsumerMid::CSMConsumerMid(ProducerType producer_type) :
        m_producer_type (producer_type),
        m_producer(NULL),
        m_run_thread(false),
        m_delay(DEF_CONSUMER_MID_DELAY)
{
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
    CSMProducerFactory::getInstance()->openProducer(m_producer_type, &m_producer);
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");
}

CSMConsumerMid::~CSMConsumerMid()
{
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
    if(m_run_thread)
        stopReader();
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");
}

void CSMConsumerMid::startReader()
{
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
    if(!m_producer) {
        CSMProducerFactory::getInstance()->openProducer(m_producer_type, &m_producer);
        if(!m_producer->isStarted())
            m_producer->startAddData();
    }
    if(m_run_thread)
        throw CSMConsumerMidException("Reader: Reader already started");
    m_run_thread = true;
    m_thread = std::unique_ptr<std::thread>(new std::thread(&CSMConsumerMid::threadFunc, this));
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");
}

void CSMConsumerMid::stopReader()
{
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
    if(!m_run_thread)
        throw CSMConsumerMidException(std::string("Reader: Reader isn't started"));
    m_run_thread = false;
    if(m_thread.get()->joinable())
        m_thread.get()->join();
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");
}

void CSMConsumerMid::threadFunc()
{
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
    try {
        while(m_run_thread) {
            processProducerConsumerItem();
            usleep(m_delay*1000);
        }
    }
    catch (sm_producer_consumer::CSMProducerException &ex) {
        sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_FATAL, __PRETTY_FUNCTION__, "Error: %s", ex.message().c_str());
    }
    catch (sm_producer_consumer::CSMProducerFactoryException &ex) {
        sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_FATAL, __PRETTY_FUNCTION__, "Error: %s", ex.message().c_str());
    }
    catch (sm_producer_consumer::CSMConsumerFactoryException &ex) {
        sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_FATAL, __PRETTY_FUNCTION__, "Error: %s", ex.message().c_str());
    }
    catch (sm_producer_consumer::CSMConsumerMidException &ex) {
        sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_FATAL, __PRETTY_FUNCTION__, "Error: %s", ex.message().c_str());
    }
    catch (sm_producer_consumer::CSMConsumerFinException &ex) {
        sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_FATAL, __PRETTY_FUNCTION__, "Error: %s", ex.message().c_str());
    }
    catch (sm_producer_consumer::CSMBaseException &ex) {
        sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_FATAL, __PRETTY_FUNCTION__, "Error: %s", ex.message().c_str());
    }
    catch (std::exception *ex) {
        sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_FATAL, __PRETTY_FUNCTION__, "Error: %s", ex->what());
    }
    catch (std::exception &ex) {
        sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_FATAL, __PRETTY_FUNCTION__, "Error: %s", ex.what());
    }
    catch (...) {
        sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_FATAL, __PRETTY_FUNCTION__, "Error: Some exception happened...");
    }
    sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_DEBUG, __PRETTY_FUNCTION__, "%s is finished...", __PRETTY_FUNCTION__);
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");
}

void CSMConsumerMid::processProducerConsumerItem()
{
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
    CSMProducerItem producer_item;
    if(m_producer->getProducerItem(producer_item)) {
        size_t size_from_producer = producer_item.getSize();
        unsigned char* data_from_producer = producer_item.getData();
        sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_DEBUG, __PRETTY_FUNCTION__, "size_from_producer = %d", size_from_producer);
        sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_DEBUG, __PRETTY_FUNCTION__, "data_from_producer = %s", data_from_producer);
        CSMConsumerItem consumer_item;
        if(size_from_producer > 0 && data_from_producer) {
            unsigned char* data_for_queue = new unsigned char[size_from_producer];
            std::strncpy((char*)data_for_queue, (char*)data_from_producer, size_from_producer);
            consumer_item.setDataAndSize(data_for_queue, size_from_producer);
            CSMConsumerFactory::getInstance()->getConsumerFin()->addConsumerItem(consumer_item);
            sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_DEBUG, __PRETTY_FUNCTION__, "new Queue item has been added to the SHARED QUEUE");
        }
    }
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");
}

}
