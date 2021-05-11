/*
 *  Copyright (c) 2021 SManSoft <http://www.smansoft.com/>
 *  Sergey Manoylo <info@smansoft.com>
 *
 *  sm_producer.cpp
 *
 *  Created on: Apr 26, 2021
 *      Author: sman
 */

#include "pch.h"

#include "sm_consumer_item.h"
#include "sm_exceptions.h"
#include "sm_log_tool.h"
#include "sm_producer.h"
#include "sm_producer_item.h"

extern sm_log_config gsm_log_config;    //  global instance of main log support structure
#define SM_LOG_CONFIG &gsm_log_config   //  just synonym: SM_LOG_CONFIG == &gsm_log_config - for usage in log api calls

namespace sm_producer_consumer {

/************************** DataSource **************************/

CSMProducerBase::CSMProducerBase(const ProducerType type) :
            m_max_num(0),
            m_delay(0),
            m_items_queue_max(0),
            m_type(type),
            m_run_thread(false)
{
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
    sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_DEBUG, __PRETTY_FUNCTION__, "type = %d", m_type);
    timeval tv_o;
    gettimeofday(&tv_o, NULL);
    m_rnd_eng.seed((unsigned long)(tv_o.tv_sec + tv_o.tv_usec));
    std::srand(std::time(nullptr));
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");
}

CSMProducerBase::~CSMProducerBase()
{
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
    if(m_run_thread)
        stopAddData();
    clearData();
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");
}

size_t CSMProducerBase::getQueueSize() {
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
    std::unique_lock<std::mutex> lock(m_mtx_queue);
    size_t size = m_items_queue.size();
    sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_DEBUG, __PRETTY_FUNCTION__, "size = %d", size);
    lock.unlock();
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");
    return size;
}

bool CSMProducerBase::getProducerItem(CSMProducerItem& producer_item)
{
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
    bool res = false;
    std::unique_lock<std::mutex> lock(m_mtx_queue);
    size_t src_size = m_items_queue.size();
    if(m_run_thread && !src_size) {
        sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "starting m_cv_queue.wait(lock)...");
        m_cv_queue.wait(lock);
        sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "finishing m_cv_queue.wait(lock)...");
    }
    if(src_size) {
        sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_DEBUG, __PRETTY_FUNCTION__, "m_items_queue.size() = %d", m_items_queue.size());
        producer_item = m_items_queue.front();
        m_items_queue.pop();
        sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_DEBUG, __PRETTY_FUNCTION__, "type = %d m_items_queue.size() (after pop) = %d",
                m_type, m_items_queue.size());
        sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_INFO, "CSMProducerBase", "type = %d item has been removed:  m_data_queue.size() (after pop) = %d",
                m_type, m_items_queue.size());
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
    sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_DEBUG, __PRETTY_FUNCTION__, "res = %d", res);
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");
    return res;
}

void CSMProducerBase::genProducerItem()
{
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
    std::stringstream str_stream;
    int num = std::rand()%(m_max_num+1);
    if (!num)
        num = m_max_num+1;
    sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_DEBUG, __PRETTY_FUNCTION__, "number of integer numbers will be generated: num = %d", num);
    std::generate_n(std::ostream_iterator<std::mt19937::result_type>(str_stream), num, std::ref(m_rnd_eng));
    std::string str_strm = str_stream.str();
    const char *str = str_strm.c_str();
    sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_DEBUG, __PRETTY_FUNCTION__, "line of random: str = %s", str);
    int len = str_strm.length();
    sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_DEBUG, __PRETTY_FUNCTION__, "len of line of random: len = %d", len);
    CSMProducerItem item;
    char* item_rand_data = new char[len + 1];
    item_rand_data[0] = '\0';
    std::strncpy(item_rand_data, str, len + 1);
    item.setDataAndSize((unsigned char*)item_rand_data, (len + 1));
    std::unique_lock<std::mutex> lock(m_mtx_queue);
    unsigned int src_size = m_items_queue.size();
    if(m_run_thread && src_size >= m_items_queue_max) {
        sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "starting m_cv_queue.wait(lock)...");
        m_cv_queue.wait(lock);
        sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "finishing m_cv_queue.wait(lock)...");
    }
    m_items_queue.push(item);
    sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_DEBUG, __PRETTY_FUNCTION__, "m_items_queue.size() = %d", m_items_queue.size());
    sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_INFO, "CSMProducerBase", "type = %d item has been added: m_items_queue.size() = %d",
            m_type, m_items_queue.size());
    if(!src_size) {
        sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "starting m_cv_queue.notify_one()");
        m_cv_queue.notify_one();
        sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "finishing m_cv_queue.notify_one()");
    }
    lock.unlock();
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");
}

void CSMProducerBase::stopAddData()
{
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
    if(!m_run_thread)
        throw CSMProducerException(std::string("DataSource isn't started"));
    m_run_thread = false;
    m_cv_queue.notify_one();
    if(m_thread.get()->joinable())
        m_thread.get()->join();
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");
}

void CSMProducerBase::clearData()
{
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
    std::unique_lock<std::mutex> lock(m_mtx_queue);
    while(!m_items_queue.empty())
        m_items_queue.pop();
    lock.unlock();
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");
}

void CSMProducerBase::startAddData()
{
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
    if(m_run_thread)
        throw CSMProducerException(std::string("CSMProducerBase already started"));
    m_run_thread = true;
    m_thread = std::unique_ptr<std::thread>(new std::thread(&CSMProducerBase::threadFunc, this));
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");
}

void CSMProducerBase::threadFunc()
{
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- >>");
    try {
        while(m_run_thread) {
            genProducerItem();
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
    sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_DEBUG, __PRETTY_FUNCTION__, "type = %d is finished...", m_type);
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "---------- <<");
}

/************************** DataSourceType1 **************************/

CSMProducerType1::CSMProducerType1() : CSMProducerBase(ProducerType_1)
{
    m_max_num = DEF_MAX_SIZE_RND_TYPE1;
    m_delay = DEF_PRODUCER_DELAY_TYPE1;
    m_items_queue_max = DEF_PRODUCER_QUEUE_TYPE1_MAX;
}

CSMProducerType1::~CSMProducerType1()
{
}

/************************** DataSourceType2 **************************/

CSMProducerType2::CSMProducerType2() : CSMProducerBase(ProducerType_2)
{
    m_max_num = DEF_MAX_SIZE_RND_TYPE2;
    m_delay = DEF_PRODUCER_DELAY_TYPE2;
    m_items_queue_max = DEF_PRODUCER_QUEUE_TYPE2_MAX;
}

CSMProducerType2::~CSMProducerType2()
{
}

/************************** DataSourceType3 **************************/

CSMProducerType3::CSMProducerType3() : CSMProducerBase(ProducerType_3)
{
    m_max_num = DEF_MAX_SIZE_RND_TYPE3;
    m_delay = DEF_PRODUCER_DELAY_TYPE3;
    m_items_queue_max = DEF_PRODUCER_QUEUE_TYPE3_MAX;
}

CSMProducerType3::~CSMProducerType3()
{
}

/************************** DataSourceType4 **************************/

CSMProducerType4::CSMProducerType4() : CSMProducerBase(ProducerType_4)
{
    m_max_num = DEF_MAX_SIZE_RND_TYPE4;
    m_delay = DEF_PRODUCER_DELAY_TYPE4;
    m_items_queue_max = DEF_PRODUCER_QUEUE_TYPE4_MAX;
}

CSMProducerType4::~CSMProducerType4()
{
}

/************************** DataSourceType5 **************************/

CSMProducerType5::CSMProducerType5() : CSMProducerBase(ProducerType_5)
{
    m_max_num = DEF_MAX_SIZE_RND_TYPE5;
    m_delay = DEF_PRODUCER_DELAY_TYPE5;
    m_items_queue_max = DEF_PRODUCER_QUEUE_TYPE5_MAX;
}

CSMProducerType5::~CSMProducerType5()
{
}

}
