/*
 *  Copyright (c) 2021 SManSoft <http://www.smansoft.com/>
 *  Sergey Manoylo <info@smansoft.com>
 *
 *  sm_consumer_mid.h
 *
 *  Created on: Apr 26, 2021
 *      Author: sman
 */

#pragma once

#include "pch.h"

#include "sm_main.h"
#include "sm_producer.h"

namespace sm_producer_consumer {

class CSMConsumerFactory;

/**
 *  CSMConsumerMid
 *
 *  intermediate consumer class, that has some type
 *  (like CSMProducerType1, CSMProducerType2,... CSMProducerType5)
 *
 *  instance of this class (CSMConsumerMid) is joined to
 *  the instance of producer class
 *  (CSMProducerType1, CSMProducerType2, ... CSMProducerType5, ProducerType producer_type);
 *
 *  this class creates thread, that reads data queue,
 *  m_items_queue (CSMProducerItem)
 *  that filled by the correspondent producer;
 *
 *  this class extracts CSMProducerItem from m_items_queue,
 *  converts CSMProducerItem -> CSMConsumerItem
 *  and inserts CSMConsumerItem to
 *  std::queue<CSMConsumerItem> m_items_queue
 *  (one instance in the CSMConsumerFin class);
 *
 *  i.e. processing of items can be presented by follow schema:
 *
 *  CSMProducerType1 --(adding in the thread 1)--> queue 1 <--(extracting in the thread 6 )-- CSMConsumerMid (instance 1) --(adding in the thread 6) --> |                  |
 *  CSMProducerType2 --(adding in the thread 2)--> queue 2 <--(extracting in the thread 7 )-- CSMConsumerMid (instance 2) --(adding in the thread 7) --> | common queue     |
 *  CSMProducerType3 --(adding in the thread 3)--> queue 3 <--(extracting in the thread 8 )-- CSMConsumerMid (instance 3) --(adding in the thread 8) --> | in the class     |
 *  CSMProducerType4 --(adding in the thread 4)--> queue 4 <--(extracting in the thread 9 )-- CSMConsumerMid (instance 4) --(adding in the thread 9) --> | CSMConsumerFin   |
 *  CSMProducerType5 --(adding in the thread 5)--> queue 5 <--(extracting in the thread 10)-- CSMConsumerMid (instance 5) --(adding in the thread 10)--> |                  |
 */
class CSMConsumerMid {

public:
    CSMConsumerMid(const ProducerType producer_type);
    CSMConsumerMid(CSMConsumerMid&& consumer_mid) = delete;
    virtual ~CSMConsumerMid();

    void startReader();
    void stopReader();

    bool isStarted() const {    return m_run_thread;    }

private:
    void processProducerConsumerItem();         // is called by the threadFunc function
                                                // provides processing of data:
                                                // reading from (via CSMProducerBase *m_producer (one of CSMProducerType1, CSMProducerType2, ... CSMProducerType5)),
                                                // generating CSMConsumerItem and inserting it to common/shared (for all Consumer threads) queue
                                                // std::queue<CSMConsumerItem> m_items_queue
                                                // in the CSMConsumerFin class
    void threadFunc();                          // thread function (is used by std::unique_ptr<std::thread>m_thread)

    ProducerType        m_producer_type;        // type of producer
    CSMProducerBase     *m_producer;            // pointer to joined producer (one of CSMProducerType1, CSMProducerType2, ... CSMProducerType5))
                                                // initialized in the constructor and depends on (ProducerType m_producer_type)

    std::unique_ptr<std::thread>m_thread;       // thread, that is used for reading the queue from
                                                // std::queue<CSMProducerItem> m_items_queue
                                                // (via CSMProducerBase *m_producer (one of CSMProducerType1, CSMProducerType2, ... CSMProducerType5)),
                                                // generation CSMConsumerItem and inserting it to
                                                // std::queue<CSMConsumerItem> m_items_queue
                                                // in the CSMConsumerFin class
                                                // instance of the CSMConsumerFin class is obtained via singleton: CSMConsumerFactory
    bool m_run_thread;                          // if m_run_thread == false, thread (m_thread) will be finished

    unsigned int m_delay;                       // delay in msecs (sleep time in the thread)
};

}
