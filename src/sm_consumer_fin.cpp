/*
 *  Copyright (c) 2021 SManSoft <http://www.smansoft.com/>
 *  Sergey Manoylo <info@smansoft.com>
 *
 *  sm_consumer_fin.cpp
 *
 *  Created on: Apr 28, 2021
 *      Author: sman
 */

#include "pch.h"

#include "sm_consumer_factory.h"
#include "sm_consumer_fin.h"
#include "sm_exceptions.h"
#include "sm_log_tool.h"

extern sm_log_config gsm_log_config;    //  global instance of main log support structure
#define SM_LOG_CONFIG &gsm_log_config   //  just synonym: SM_LOG_CONFIG == &gsm_log_config - for usage in log api calls

namespace sm_producer_consumer {

CSMConsumerFin::CSMConsumerFin(const std::string& consumer_output_fpath) : m_delay(DEF_CONSUMER_FIN_DELAY),
        m_run_thread(false),
        m_items_queue_max(DEF_CONSUMER_FIN_QUEUE_MAX),
        m_consumer_output_fpath(consumer_output_fpath)
{
    m_consumer_output.open(m_consumer_output_fpath, std::ios_base::out | std::ios_base::trunc);
}

CSMConsumerFin::~CSMConsumerFin()
{
    m_consumer_output.close();
}

void CSMConsumerFin::threadFunc()
{
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
    try {
        while(m_run_thread) {
            processConsumerItem();
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
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_DEBUG, __PRETTY_FUNCTION__, "reader processor is finished...");
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");
}

void CSMConsumerFin::processConsumerItem()
{
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
    CSMConsumerItem consumer_item;
    if(getConsumerItem(consumer_item)) {
        m_consumer_output << "len:\t" << (consumer_item.getSize() - 1)  << "\t" << "line:\t" << consumer_item.getData() << std::endl;
    }
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");
}

void CSMConsumerFin::startReaderProcessor()
{
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
    if(m_run_thread)
        throw CSMConsumerFinException("CSMConsumerFin: CSMConsumerFin already started");
    m_run_thread = true;
    m_thread = std::unique_ptr<std::thread>(new std::thread(&CSMConsumerFin::threadFunc, this));
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");
}

void CSMConsumerFin::stopReaderProcessor()
{
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
    if(!m_run_thread)
        throw CSMConsumerFinException(std::string("CSMConsumerFin: CSMConsumerFin isn't started"));
    m_run_thread = false;
    if(m_thread.get()->joinable())
        m_thread.get()->join();
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");
}

void CSMConsumerFin::addConsumerItem(const CSMConsumerItem& consumer_item)
{
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
    std::unique_lock<std::mutex> lock(m_mtx_queue);
    size_t src_size = m_items_queue.size();
    if(m_run_thread && src_size >= m_items_queue_max) {
        sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "starting m_cv_queue.wait(lock)...");
        m_cv_queue.wait(lock);
        sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "finishing m_cv_queue.wait(lock)...");
    }
    m_items_queue.push(consumer_item);
    if(!src_size) {
        sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "starting m_cv_queue.notify_one()");
        m_cv_queue.notify_one();
        sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "finishing m_cv_queue.notify_one()");
    }
    lock.unlock();
    sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_INFO, "CSMConsumerFin", "item has been added: m_items_queue.size() = %d", m_items_queue.size());
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");
}

bool CSMConsumerFin::getConsumerItem(CSMConsumerItem& consumer_item)
{
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
    bool res = false;
    std::unique_lock<std::mutex> lock(m_mtx_queue);
    size_t src_size = m_items_queue.size();
    if(m_run_thread && m_items_queue.empty()) {
        sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "starting m_cv_queue.wait(lock)...");
        m_cv_queue.wait(lock);
        sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "finishing m_cv_queue.wait(lock)...");
    }
    if(!m_items_queue.empty()) {
        sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_DEBUG, __PRETTY_FUNCTION__, "m_items_queue.size() = %d", m_items_queue.size());
        consumer_item = m_items_queue.front();
        m_items_queue.pop();
        sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_DEBUG, __PRETTY_FUNCTION__, "m_items_queue.size() (after pop) = %d", m_items_queue.size());
        sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_INFO, "CSMConsumerFin", "item has been removed: m_data_queue.size() (after pop) = %d", m_items_queue.size());
        res = true;
    }
    else
        res = false;
    if(src_size >= m_items_queue_max) {
        sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "starting m_cv_queue.notify_one()");
        m_cv_queue.notify_one();
        sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "finishing m_cv_queue.notify_one()");
    }
    lock.unlock();
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");
    return res;
}

void CSMConsumerFin::printQueue()
{
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_DEBUG, __PRETTY_FUNCTION__, "****************************************************** : start...");
    CSMConsumerItem item;
    std::unique_lock<std::mutex> lock(m_mtx_queue);
    sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_DEBUG, __PRETTY_FUNCTION__, "getQueueSize() = %d", m_items_queue.size());
    sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_INFO, "CSMConsumerFin", "getQueueSize() = %d", m_items_queue.size());
    while(!m_items_queue.empty()) {
        item = m_items_queue.front();
        sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_DEBUG, __PRETTY_FUNCTION__, "item size  = %d data = %s", item.getSize(), item.getData());
        m_items_queue.pop();
    }
    sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_DEBUG, __PRETTY_FUNCTION__, "getQueueSize() = %d", m_items_queue.size());
    sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_INFO, "CSMConsumerFin", "getQueueSize() = %d", m_items_queue.size());
    lock.unlock();
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_DEBUG, __PRETTY_FUNCTION__, "****************************************************** : end...");
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");
}

size_t CSMConsumerFin::getQueueSize() {
    std::unique_lock<std::mutex> lock(m_mtx_queue);
    size_t res_size = m_items_queue.size();
    return res_size;
}

}
