/*
 *  Copyright (c) 2021 SManSoft <http://www.smansoft.com/>
 *  Sergey Manoylo <info@smansoft.com>
 *
 *  sm_producer_factory.h
 *
 *  Created on: Apr 26, 2021
 *      Author: sman
 */

#pragma once

#include "pch.h"

#include "sm_main.h"
#include "sm_producer.h"

namespace sm_producer_consumer {

/**
 *  CSMProducerFactory
 *
 *  multi-thread synchronized singleton;
 *
 *  storage and control CSMProducerBase based (inherited) objects (several instances),
 *  i.e. instances of
 *  follow classes CSMProducerType1, CSMProducerType2,
 *  CSMProducerType3, CSMProducerType4, CSMProducerType5
 */
class CSMProducerFactory {

public:
    static CSMProducerFactory* getInstance();   // provides access to the singleton, creates instance
    static void freeInstance();                 // deletes (frees) instance

public:
    static void createProducer(const ProducerType producer_type, CSMProducerBase** const producer);
    void addProducer(CSMProducerBase* const producer);
    void openProducer(const ProducerType producer_type, CSMProducerBase** const producer);
    bool getProducer(const ProducerType producer_type, CSMProducerBase** const producer);
    size_t getNumProducers();

    void startAllProducers();
    void stopAllProducers();

private:
    CSMProducerFactory();       // private constructor, instance of the class will be created by
                                // getInstance() function
    ~CSMProducerFactory();      // private destructor, instance of the class will be deleted (free) by
                                // freeInstance function

    std::vector<std::shared_ptr<CSMProducerBase>> m_producers;  // vector of pointers to CSMProducerBase
                                                                // (CSMProducerType1, CSMProducerType2, ... CSMProducerType5) objects
    std::mutex m_mtx_producers;                                 // synchronization primitive, that makes
                                                                // m_mtx_producers an thread-safe queue

    static CSMProducerFactory* m_instance;                      // pointer to instance of the singleton
    static std::mutex m_mtx_instance;                           // thread-safe pointer
};

}
