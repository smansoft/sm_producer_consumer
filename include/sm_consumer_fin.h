/*
 *  Copyright (c) 2021 SManSoft <http://www.smansoft.com/>
 *  Sergey Manoylo <info@smansoft.com>
 *
 *  sm_consumer_fin.h
 *
 *  Created on: Apr 28, 2021
 *      Author: sman
 */

#pragma once

#include "pch.h"

#include "sm_main.h"

namespace sm_producer_consumer {

/**
 *  CSMConsumerFin
 *
 *  final consumer class,contains queue of CSMConsumerItem elements:
 *  std::queue<CSMConsumerItem> m_items_queue;
 *
 *  m_items_queue has limited size (defined by m_items_queue_max);
 *
 *  opens text file (m_consumer_output) (file path: m_consumer_output_fpath);
 *
 *  creates thread, that reads queue of CSMConsumerItem elements:
 *  std::queue<CSMConsumerItem> m_items_queue;
 *  and writes content (unsigned char array and length) into
 *  the output text file (m_consumer_output) in the follow format;
 *
 *  len:    30  line:   165419817718862070872477220052
 *  len:    20  line:   39200062562017294509
 *  len:    50  line:   17602029623591885893229577471612569214254259623284
 *  len:    106 line:   2312287005485858224349874085119518972282993005321207792955811589305733100220415361105722182633373291193122
 *  len:    57  line:   276877865641402651882762481943439227966147003631211487406
 *  len:    30  line:   336727954911150551753810078128
 *  len:    97  line:   4493950942307061909176887685717840987692401872029587605656171416438224493589713863559573367050933
 *
 *  uses synchronizing primitives for access to the queue (m_items_queue):
 *  std::mutex m_mtx_queue;
 *  std::condition_variable m_cv_queue;
 *
 *  periodicity of generation of items are defined, by m_delay (sleep value)
 */
class CSMConsumerFin {

public:
    CSMConsumerFin(const std::string& consumer_output_fpath);
    CSMConsumerFin(CSMConsumerFin&& consumer_fin) = delete;
    virtual ~CSMConsumerFin();

    void addConsumerItem(const CSMConsumerItem& consumer_item);
    bool getConsumerItem(CSMConsumerItem& consumer_item);

    void startReaderProcessor();
    void stopReaderProcessor();

    bool isStarted() const {    return m_run_thread;    }

    size_t getQueueSize();

    void printQueue();

private:
    void processConsumerItem(); // is called by the threadFunc() function
                                // - reading data (CSMConsumerItem) from std::queue<CSMConsumerItem> m_items_queue
                                // - writing data to the file std::ofstream m_consumer_output
    void threadFunc();          // thread function (is used by std::unique_ptr<std::thread>m_thread)

    std::unique_ptr<std::thread>m_thread;       // thread of this consumer class, provides processing of data: threadFunc() -> readData()
    bool m_run_thread;                          // if m_run_thread == false, thread (m_thread) will be finished

    unsigned int m_items_queue_max;             // max size of m_items_queue (queue of CSMConsumerItem elements)
    std::queue<CSMConsumerItem> m_items_queue;  // queue of CSMConsumerItem elements, which are generated in threads, created by CSMConsumerMid objects
    std::mutex  m_mtx_queue;                    // synchronization primitive (mutex) for access to m_items_queue
    std::condition_variable m_cv_queue;         // synchronization primitive (mutex) for access to m_items_queue
                                                // condition is used, when m_mtx_queue is locked, but
                                                // m_items_queue is empty or its size equals m_items_queue_max
                                                // i.e. maximum value and new items can't be added
                                                // and CSMConsumerMid, calling addConsumerItem, should wait till the readData function extracts
                                                // item from m_items_queue

    std::string         m_consumer_output_fpath;    // full file path of m_consumer_output
    std::ofstream       m_consumer_output;          // file, where result data (lines) will be saved:
                                                    // len: 30  line:   165419817718862070872477220052
                                                    // len: 20  line:   39200062562017294509

    unsigned int m_delay;   // delay in msecs (sleep time in the thread)
};

}
